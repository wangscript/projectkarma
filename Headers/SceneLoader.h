#ifndef SCENE_LOADER_H
#define SCENE_LOADER_H


#include <tinyxml.h>
#include "PhysicsManager.h"
#include "Ogre.h"
#include "OgrePrerequisites.h"

class SceneLoader
{
private:
	TiXmlDocument			*mvpXMLDoc;
	Ogre::SceneManager		*mvpSceneMgr;
	Ogre::SceneNode			*mvpAttachNode;
	PhysicsManager			*mvpPhysicsMgr;
	Ogre::String			sFilename;
	
	bool parseNode(TiXmlElement *XMLNode);
	bool parseScene(TiXmlElement *XMLRoot);
	void stripStr(const Ogre::String& string2trip, Ogre::String &base,Ogre::String &path);

public:
	SceneLoader() : mvpSceneMgr(0) {};
	~SceneLoader();

	bool parseLevel(const Ogre::String& filename,const Ogre::String &group, Ogre::SceneManager*,Ogre::SceneNode *attachNode, PhysicsManager *physicsMgr);

};


#endif