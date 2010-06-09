/*---------------------------------------------------------------------------------*/
/* File: WorldManager.h															   */
/* Author: Per Karlsson, perkarlsson89@gmail.com								   */
/*																				   */
/* Description:	WorldManager is a class that creates and initiates the terrain,    */
/* both in OGRE and PhysX. It uses the new OGRE 1.7 terrain.			  		   */
/*---------------------------------------------------------------------------------*/

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
	/* dirLight = the directional light in the scene. Used for light maps. */
	WorldManager(Ogre::SceneManager* s, Ogre::Camera* c, NxOgre::Scene* p, Ogre::Light* dirLight);
	~WorldManager();
	
	/* Adds a texture Layer to terrain.
	diffuseAndSpecMap = name of the file that has díffuse and specular maps.
	normalAndHeightMap = name of the file that has normal and displacement maps.
	worldSize = How many times they are tilted. If worldSize = 10 and and the terrains total size i 100, then it is tiled 10 times.*/
	void addTextureLayer(Ogre::String& diffuseAndSpecMap, Ogre::String& normalAndHeightMap, Ogre::Real worldSize);
	
	/* Generate the terrain in the Physics Scene. */
	void buildNxOgreTerrain();
	
	/* Get terrain at coordinates x,y */
	Ogre::Terrain* getTerrainAt(int x,int y);		

	/* Initiate the terrain. Loads from a binary file or creates it from a heightmap. */
	void initTerrain();
	
private:
	bool									mvTerrainsImported;
	Ogre::TerrainGlobalOptions*				mvpTerrainGlobals;
	Ogre::TerrainGroup*						mvpTerrainGroup;
	Ogre::SceneManager*						mvpSceneMgr;		
	Ogre::Camera*							mvpCamera;
	NxOgre::Scene*							mvpPhysicsScene;

	/* Blend all the texture layers and creates the final texture for the terrain. */
	void initBlendMaps(Ogre::Terrain* terrain);
	
	/* Creates a PhysX heightfield from the info in the terrain. */
	void loadTerrainGeometry(const Ogre::String& name, float* data, Ogre::uint16 size, Ogre::Real worldSize, Ogre::Real minHeight, Ogre::Real maxHeight, const Ogre::Vector3& position);
	
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