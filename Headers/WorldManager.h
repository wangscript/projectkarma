#ifndef WORLDMANAGER_H
#define WORLDMANAGER_H

#include "Ogre.h"
#include "NxOgre.h"
#include "OgreTerrain.h"
#include "OgreTerrainGroup.h"
#include "OgreTerrainQuadTreeNode.h"
#include "OgreTerrainMaterialGeneratorA.h"
#include "OgreTerrainPaging.h"
#include "OgrePageManager.h"
#include "AppState.h"

#define TERRAIN_PAGE_MIN_X 0
#define TERRAIN_PAGE_MIN_Y 0
#define TERRAIN_PAGE_MAX_X 0
#define TERRAIN_PAGE_MAX_Y 0

//#define PAGING

#define TERRAIN_FILE_PREFIX Ogre::String("testTerrain")
#define TERRAIN_FILE_SUFFIX Ogre::String("dat")

class WorldManager
{
public:
	WorldManager(Ogre::SceneManager* s, Ogre::Camera* c, NxOgre::Scene* p, Ogre::Light* dirLight);
	~WorldManager();
	void initTerrain();
	void defineTerrain(long x, long y);
	void addTextureLayer(Ogre::String& diffuseAndSpecMap, Ogre::String& normalAndHeightMap, Ogre::Real worldSize);
	void buildNxOgreTerrain();
	void configureShadows(bool enabled, bool depthShadows);

protected:

	/* If pagemanager is going to be used
	bool									mtPaging;
	Ogre::TerrainPaging*					mtpTerrainPaging;
	Ogre::PageManager*						mtpPageManager; */

	/// This class just pretends to provide prcedural page content to avoid page loading
	class DummyPageProvider : public Ogre::PageProvider
	{
	public:
		bool prepareProceduralPage(Ogre::Page* page, Ogre::PagedWorldSection* section) { return true; }
		bool loadProceduralPage(Ogre::Page* page, Ogre::PagedWorldSection* section) { return true; }
		bool unloadProceduralPage(Ogre::Page* page, Ogre::PagedWorldSection* section) { return true; }
		bool unprepareProceduralPage(Ogre::Page* page, Ogre::PagedWorldSection* section) { return true; }
	};
	DummyPageProvider						mtDummyPageProvider;

	Ogre::TerrainPaging*							mtpTerrainPaging;
	Ogre::PageManager*							 mtpPageManager;

	bool									mtTerrainsImported;
	Ogre::TerrainGlobalOptions*				mtpTerrainGlobals;
	Ogre::TerrainGroup*						mtpTerrainGroup;
	Ogre::ShadowCameraSetupPtr				mtPSSMSetup;
	
	Ogre::SceneManager*						mtpSceneMgr;		
	Ogre::Camera*							mtpCamera;
	NxOgre::Scene*							mtpPhysicsScene;

	virtual Ogre::MaterialPtr buildDepthShadowMaterial(const Ogre::String& textureName);
	virtual void initBlendMaps(Ogre::Terrain* terrain);
	virtual void loadTerrainGeometry(const Ogre::String& name, float* data, Ogre::uint16 size, Ogre::Real worldSize, Ogre::Real minHeight, Ogre::Real maxHeight, const Ogre::Vector3& position);

};
#endif