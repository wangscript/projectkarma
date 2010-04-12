#ifndef WORLDMANAGER_H
#define WORLDMANAGER_H

#include "Ogre.h"
#include "NxOgre.h"
#include "OgreTerrain.h"
#include "OgreTerrainGroup.h"
#include "OgreTerrainQuadTreeNode.h"
#include "OgreTerrainMaterialGeneratorA.h"
#include "OgreTerrainPaging.h"
#include "AppState.h"

#define TERRAIN_PAGE_MIN_X 0
#define TERRAIN_PAGE_MIN_Y 0
#define TERRAIN_PAGE_MAX_X 0
#define TERRAIN_PAGE_MAX_Y 0

#define TERRAIN_FILE_PREFIX Ogre::String("testTerrain")
#define TERRAIN_FILE_SUFFIX Ogre::String("dat")
#define TERRAIN_WORLD_SIZE 12000.0f
#define TERRAIN_SIZE 513

class WorldManager
{
public:
	WorldManager(Ogre::SceneManager* s, Ogre::Camera* c, NxOgre::Scene* p);
	virtual ~WorldManager();
	virtual void initTerrain(Ogre::Light* light);
	virtual void buildNxOgreTerrain();
	virtual void configureShadows(bool enabled, bool depthShadows);

protected:

	/* If pagemanager is going to be used
	bool									mtPaging;
	Ogre::TerrainPaging*					mtpTerrainPaging;
	Ogre::PageManager*						mtpPageManager; */

	bool									mtTerrainsImported;
	Ogre::TerrainGlobalOptions*				mtpTerrainGlobals;
	Ogre::TerrainGroup*						mtpTerrainGroup;
	Ogre::ShadowCameraSetupPtr				mtPSSMSetup;

	Ogre::SceneManager*						mtpSceneMgr;		
	Ogre::Camera*							mtpCamera;
	NxOgre::Scene*							mtpPhysicsScene;

	virtual void initBlendMaps(Ogre::Terrain* terrain);
	virtual void loadTerrainGeometry(const Ogre::String& name, float* data, Ogre::uint16 size, Ogre::Real worldSize, Ogre::Real minHeight, Ogre::Real maxHeight, const Ogre::Vector3& position);

};
#endif