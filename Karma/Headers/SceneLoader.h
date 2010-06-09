//////////////////////////////////////////////////////////////////////////
// SceneLoader.h Rev:1
// Christopher Birger
// @todo Write to LogManager
//////////////////////////////////////////////////////////////////////////

#ifndef SCENE_LOADER_H
#define SCENE_LOADER_H


#include <tinyxml.h>
#include "PhysicsManager.h"
#include "Ogre.h"
#include "OgrePrerequisites.h"
#include "OgreTerrain.h"

class SceneLoader
{
private:
	TiXmlDocument			*mvpXMLDoc;
	Ogre::SceneManager		*mvpSceneMgr;
	Ogre::SceneNode			*mvpAttachNode;
	PhysicsManager			*mvpPhysicsMgr;
	Ogre::String			sFilename;
	Ogre::Terrain*			mvpTerrain;
	
	bool parseNode(TiXmlElement *XMLNode);
	bool parseScene(TiXmlElement *XMLRoot);
	void stripStr(const Ogre::String& string2trip, Ogre::String &base,Ogre::String &path);

public:
	SceneLoader() : mvpSceneMgr(0) {};
	~SceneLoader();

	bool parseLevel(const Ogre::String& filename,const Ogre::String &group, Ogre::SceneManager*,Ogre::SceneNode *attachNode, PhysicsManager *physicsMgr,Ogre::Terrain* terrain);

};


#endif