#ifndef WORLDMANAGER_H
#define WORLDMANAGER_H

#include "Ogre.h"
#include "NxOgre.h"
#include "OgreTerrain.h"
#include "OgreTerrainGroup.h"
#include "OgreTerrainQuadTreeNode.h"
#include "OgreTerrainMaterialGeneratorA.h"
#include "AppState.h"

#define TERRAIN_FILE_PREFIX Ogre::String("terrain")
#define TERRAIN_FILE_SUFFIX Ogre::String("dat")

class WorldManager
{
public:
	WorldManager(Ogre::SceneManager* s, Ogre::Camera* c, NxOgre::Scene* p, Ogre::Light* dirLight);
	~WorldManager();
	void initTerrain();
	void addTextureLayer(Ogre::String& diffuseAndSpecMap, Ogre::String& normalAndHeightMap, Ogre::Real worldSize);
	void buildNxOgreTerrain();
	Ogre::Terrain* getTerrainAt(int x,int y);

protected:
	bool									mtTerrainsImported;
	Ogre::TerrainGlobalOptions*				mtpTerrainGlobals;
	Ogre::TerrainGroup*						mtpTerrainGroup;

	Ogre::SceneManager*						mtpSceneMgr;		
	Ogre::Camera*							mtpCamera;
	NxOgre::Scene*							mtpPhysicsScene;

	virtual void initBlendMaps(Ogre::Terrain* terrain);
	virtual void loadTerrainGeometry(const Ogre::String& name, float* data, Ogre::uint16 size, Ogre::Real worldSize, Ogre::Real minHeight, Ogre::Real maxHeight, const Ogre::Vector3& position);
	/* If pagemanager is going to be used
	bool									mtPaging;
	Ogre::TerrainPaging*					mtpTerrainPaging;
	Ogre::PageManager*						mtpPageManager; */

	/* If using shadow maps instead of shadow volumes
	void configureShadows(bool enabled, bool depthShadows);
	Ogre::ShadowCameraSetupPtr				mtPSSMSetup;
	virtual Ogre::MaterialPtr buildDepthShadowMaterial(const Ogre::String& textureName); */
};
#endif