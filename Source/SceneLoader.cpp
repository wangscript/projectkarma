//////////////////////////////////////////////////////////////////////////
// SceneLoader.cpp Rev:1
// Christopher Birger
// @todo Write to LogManager
//////////////////////////////////////////////////////////////////////////

	


#include "SceneLoader.h"
#include <algorithm>

SceneLoader::~SceneLoader()
{
	//@todo
}




//Loads and checks the scenefile. then calls parseScene to parse the data.
bool SceneLoader::parseLevel(const Ogre::String &filename,const Ogre::String &group, Ogre::SceneManager *sceneMgr, Ogre::SceneNode *attachNode, PhysicsManager *physicsMgr)
{
	//Initialize
	mvpXMLDoc = 0;
	mvpSceneMgr = sceneMgr;
	mvpAttachNode = attachNode;
	mvpPhysicsMgr = physicsMgr;
	TiXmlElement *XMLRoot;
	
	try
	{
		//Strip the filename
		Ogre::String basename, path;
		//stripStr(filename, basename, path);
		
		//Fetch the resource to open

		Ogre::DataStreamPtr pStream = Ogre::ResourceGroupManager::getSingleton().openResource(filename);
		//Get the string
		Ogre::String data = pStream->getAsString();

		//Open xmldoc
		mvpXMLDoc = new TiXmlDocument();
		mvpXMLDoc->Parse(data.c_str());

		if( mvpXMLDoc->Error() )
			return false;
		else
			std::cout << filename << " open..." << std::endl;
	}
	catch (...)
	{
		delete mvpXMLDoc;
		return false;		
	}

	// Validate the File as a "Scene" file
	XMLRoot = mvpXMLDoc->RootElement();
	if( Ogre::String( XMLRoot->Value()) != "scene"  ) 
	{
		delete mvpXMLDoc;      
		return false;
	}

	// Check for passed node to attach scene
	if(attachNode == NULL)
	{
		mvpAttachNode = mvpSceneMgr->getRootSceneNode();
	}
	
	//If all is good to here we can begin processing the scene file
	parseScene(XMLRoot);

	return true;
}



bool SceneLoader::parseScene(TiXmlElement *XMLRoot)
{
	TiXmlElement *XMLNodes;
	TiXmlElement *XMLNode;
	
	//@todo process unit type, upaxis..


	//Get the nodes element
	XMLNodes = XMLRoot->FirstChildElement("nodes");
	
	//If there are no nodes return false
	if (!XMLNodes)
		return false;

	//Get the first node
	XMLNode = XMLNodes->FirstChildElement("node");
	//Check if we found a node
	if(!XMLNode)
		return false;

	//Loop through the node elements
	while (XMLNode)
	{
		parseNode(XMLNode);
		XMLNode = XMLNode->NextSiblingElement("node");
	}
	


	return true;
}

bool SceneLoader::parseNode(TiXmlElement *XMLNode)
{
	TiXmlElement *XMLRotation;
	TiXmlElement *XMLPosition;
	TiXmlElement *XMLEntity;
	TiXmlElement *XMLScale;

	Ogre::String  nodeName;
	Ogre::String  meshName;
	Ogre::String  meshFile;
	Ogre::Vector3 vec3Position;
	Ogre::Vector3 vec3Scale;
	Ogre::Quaternion quatRotation;

	Ogre::Entity *ent;


	//Get the name of the node
	nodeName = XMLNode->Attribute("name");
	std::cout << "NodeName: " << nodeName << std::endl;

	//Get the position element;
	XMLPosition = XMLNode->FirstChildElement("position");
	vec3Position.x = Ogre::StringConverter::parseReal(XMLPosition->Attribute("x"));
	vec3Position.y = Ogre::StringConverter::parseReal(XMLPosition->Attribute("y"));
	vec3Position.z = Ogre::StringConverter::parseReal(XMLPosition->Attribute("z"));
	//Debug text
	std::cout << "Position: " << "x: " << XMLPosition->Attribute("x");
	std::cout << " y: " << XMLPosition->Attribute("y");
	std::cout << " z: " << XMLPosition->Attribute("z") << std::endl;

	//Get the rotation
	XMLRotation = XMLNode->FirstChildElement("quaternion");
	std::cout << "DEN HITTADE QUATERNION";
	quatRotation.x = Ogre::StringConverter::parseReal(XMLRotation->Attribute("x"));
	quatRotation.y = Ogre::StringConverter::parseReal(XMLRotation->Attribute("y"));
	quatRotation.z = Ogre::StringConverter::parseReal(XMLRotation->Attribute("z"));
	quatRotation.w = Ogre::StringConverter::parseReal(XMLRotation->Attribute("w"));
	//Debug text
	std::cout << "Quat: " << "qx: " << XMLRotation->Attribute("x");
	std::cout << "qy: " << XMLRotation->Attribute("y");
	std::cout << "qz: " << XMLRotation->Attribute("z");
	std::cout << "qw: " << XMLRotation->Attribute("w") << std::endl;

	//Get the scale
	XMLScale = XMLNode->FirstChildElement("scale");
	vec3Scale.x = Ogre::StringConverter::parseReal(XMLScale->Attribute("x"));
	vec3Scale.y = Ogre::StringConverter::parseReal(XMLScale->Attribute("y"));
	vec3Scale.z = Ogre::StringConverter::parseReal(XMLScale->Attribute("z")); 
	//Degub text
	std::cout << "Scale: " << "x: " << XMLScale->Attribute("x");
	std::cout << " y: " << XMLScale->Attribute("y");
	std::cout << " z: " << XMLScale->Attribute("z") << std::endl;



	//Get the entity
	XMLEntity = XMLNode->FirstChildElement("entity");
	meshName = XMLEntity->Attribute("name");
	meshFile = XMLEntity->Attribute("meshFile");
	std::cout << "meshname: " << meshName << " meshFile: " << meshFile << std::endl;

	//Create the entity
	ent = mvpSceneMgr->createEntity(meshName,meshFile);
	
	//if(Ogre::StringConverter::parseBool(XMLEntity->Attribute("castShadows")))
	ent->setCastShadows(false);
	//if(ent->getCastShadows())
	//	std::cout << "ent casts shadows"<< std::endl;
	std::cout << "nodeName + Node";
	Ogre::String str = nodeName;
	str += "Node";
	Ogre::SceneNode *node = mvpSceneMgr->getRootSceneNode()->createChildSceneNode(str,vec3Position,quatRotation);
	std::cout << "createChildNode";
	node->scale(vec3Scale);
	std::cout << "scale";
	node->attachObject(ent);

	//@todo register ent with PhysicsManager
	
	//char chP[80];
	//std::strcpy(chP,meshName.c_str());
	//mvpPhysicsMgr->addStaticTriangleMesh(vec3Position,chP);

	return true;
}


//Helper method. Just divides "xxxxx.ext" to "xxxxx" and "ext"
void SceneLoader::stripStr(const Ogre::String &string2trip, Ogre::String &base, Ogre::String &path)
{
	unsigned int len = string2trip.length();
	unsigned int i = 0;
	for (i; i<len; i++)
	{
		if(string2trip[i] == '.')
			break;
		i++;
	}

	base = string2trip.substr(0,i);
	path = string2trip.substr(i+1,len-1);	
}