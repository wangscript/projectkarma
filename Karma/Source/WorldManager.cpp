/*---------------------------------------------------------------------------------*/
/* File: WorldManager.cpp														   */
/* Author: Per Karlsson, perkarlsson89@gmail.com								   */
/*																				   */
/* Description:	WorldManager is a class that creates and initiates the terrain,    */
/* both in OGRE and PhysX. It uses the new OGRE 1.7 terrain.			  		   */
/*---------------------------------------------------------------------------------*/

#include "WorldManager.h"

/*---------------------------------------------------------------------------------*/
/*									PUBLIC										   */
/*---------------------------------------------------------------------------------*/
WorldManager::WorldManager(Ogre::SceneManager* s, Ogre::Camera* c, NxOgre::Scene* p, Ogre::Light* dirLight):
mvpSceneMgr(s),
mvpCamera(c),
mvpPhysicsScene(p)
{
	//Global terrain options here
	mvpTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();
	mvpTerrainGlobals->setMaxPixelError(*Settings::getSingletonPtr()->mTerrainMaxPixelError);

	//Important information when calculating lightmaps
	mvpTerrainGlobals->setLightMapDirection(dirLight->getDerivedDirection());
	mvpTerrainGlobals->setCompositeMapAmbient(mvpSceneMgr->getAmbientLight());
	mvpTerrainGlobals->setCompositeMapDiffuse(dirLight->getDiffuseColour());
	// Set the distance at which to start using a composite map if present
	mvpTerrainGlobals->setCompositeMapDistance(*Settings::getSingletonPtr()->mTerrainCompositeMapDistance);

	//From wikipedia:
	//Anisotropic filtering (AF) is a method of enhancing the image quality of textures on surfaces 
	//that are far away and steeply angled with respect to the point of view. Older techniques,
	//such as bilinear and trilinear filtering don't take account of the angle a surface is viewed from,
	//which can result in aliasing or blurring of textures. By reducing detail in one direction more than another,
	//these effects can be reduced.	
	Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
	Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(*Settings::getSingletonPtr()->mTerrainDefaultAnisotropy);

	//Since we want the ability to load not only one terrain, we define a terrain group.
	mvpTerrainGroup = OGRE_NEW Ogre::TerrainGroup(mvpSceneMgr, Ogre::Terrain::ALIGN_X_Z,
		*Settings::getSingletonPtr()->mTerrainSize, 
		Settings::getSingletonPtr()->mTerrainWorldSize);
	mvpTerrainGroup->setFilenameConvention(TERRAIN_FILE_PREFIX, TERRAIN_FILE_SUFFIX);
	mvpTerrainGroup->setResourceGroup("Game");
}

/*---------------------------------------------------------------------------------*/
WorldManager::~WorldManager()
{
	OGRE_DELETE mvpTerrainGroup;
	OGRE_DELETE mvpTerrainGlobals;
}

/*---------------------------------------------------------------------------------*/
void WorldManager::addTextureLayer(Ogre::String& diffuseAndSpecMap, Ogre::String& normalAndHeightMap, Ogre::Real worldSize)
{
	/* Adds a texture Layer to terrain.
	diffuseAndSpecMap = name of the file that has díffuse and specular maps.
	normalAndHeightMap = name of the file that has normal and displacement maps.
	worldSize = How many times they are tilted. If worldSize = 10 and and the terrains total size i 100, then it is tiled 10 times.*/
	Ogre::Terrain::ImportData& defaultimp = mvpTerrainGroup->getDefaultImportSettings();
	int size = defaultimp.layerList.size();
	defaultimp.layerList.resize(size + 1);
	defaultimp.layerList[size].worldSize = worldSize;
	defaultimp.layerList[size].textureNames.push_back(diffuseAndSpecMap);
	defaultimp.layerList[size].textureNames.push_back(normalAndHeightMap);
}

/*---------------------------------------------------------------------------------*/
void WorldManager::buildNxOgreTerrain()
{
	GameFramework::getSingletonPtr()->mpLog->logMessage("Building NxOgre Terrain files...");

	//Loop through all terrains.
	Ogre::TerrainGroup::TerrainIterator ti = mvpTerrainGroup->getTerrainIterator();
	while(ti.hasMoreElements())
	{
		//Get the terrain
		Ogre::Terrain* t = ti.getNext()->instance;
		//Convert to PhysX heightfields.
		loadTerrainGeometry(t->getMaterialName(), t->getHeightData(), t->getSize(),
			t->getWorldSize(), t->getMinHeight(), t->getMaxHeight(), t->getPosition());
	}
	GameFramework::getSingletonPtr()->mpLog->logMessage("NxOgre Terrain files was created!");
}
/*---------------------------------------------------------------------------------*/
Ogre::Terrain* WorldManager::getTerrainAt(int x,int y)
{
	return mvpTerrainGroup->getTerrain(x,y);
}

void WorldManager::initTerrain()
{
	//Since paging is not used in this version, x and y will be 0.
	int x = 0; int y = 0;

	//Check if there is a terrain file available in the resource, otherwhise it will be created from an image.
	Ogre::String filename = mvpTerrainGroup->generateFilename(x, y);
	if (Ogre::ResourceGroupManager::getSingleton().resourceExists(mvpTerrainGroup->getResourceGroup(), filename))
	{
		GameFramework::getSingletonPtr()->mpLog->logMessage("Terrain " + filename + " was found! No need to load terrain from image");
		mvpTerrainGroup->defineTerrain(0, 0);
	}
	else
	{
		GameFramework::getSingletonPtr()->mpLog->logMessage("Terrain file was not found. Generating terrain from image...");

		// Configure default import settings
		Ogre::Terrain::ImportData& defaultimp = mvpTerrainGroup->getDefaultImportSettings();	
		defaultimp.terrainSize = *Settings::getSingletonPtr()->mTerrainSize;
		defaultimp.worldSize = Settings::getSingletonPtr()->mTerrainWorldSize;
		defaultimp.inputScale = *Settings::getSingletonPtr()->mTerrainInputScale;
		defaultimp.minBatchSize = *Settings::getSingletonPtr()->mTerrainMinBatchSize;
		defaultimp.maxBatchSize = *Settings::getSingletonPtr()->mTerrainMaxBatchSize;

		//Loads heightmap from image.
		Ogre::Image img;
		img.load("TerrainTest256.png", "Game");
		mvpTerrainGroup->defineTerrain(x, y, &img);
		mvTerrainsImported = true;
	}	

	// sync load since we want everything in place when we start
	mvpTerrainGroup->loadAllTerrains(true);

	//If new terrains were imported, we have to define how the texture layers are blending
	if (mvTerrainsImported)
	{
		Ogre::TerrainGroup::TerrainIterator ti = mvpTerrainGroup->getTerrainIterator();
		while(ti.hasMoreElements())
		{
			Ogre::Terrain* t = ti.getNext()->instance;
			initBlendMaps(t);
		}
		//Saves the terrains for further use.
		mvpTerrainGroup->saveAllTerrains(true);
	}
	mvpTerrainGroup->freeTemporaryResources();

	//In the future: Fix soft shadows
	//configureShadows(true,true);
}

/*---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*/
/*									PRIVATE										   */
/*---------------------------------------------------------------------------------*/
void WorldManager::initBlendMaps(Ogre::Terrain* terrain)
{
	//Start with checking how many texture layers that were added.
	//Since we can't blend the bottom texture, the needed amount of blend maps is nr of layers -1
	int numberOfBlendMaps = mvpTerrainGroup->getDefaultImportSettings().layerList.size();

	//Creating a vector of pointers to the blendLayerMaps
	std::vector<Ogre::TerrainLayerBlendMap*> blendMaps;

	//Blend pointers point to coordinates at the blendLayerMaps.
	std::vector<float*> blendPointers;

	//The images we're going to get the blend data from
	std::vector<Ogre::Image> blendMapImages;

	//Since paging is not used in this version, x and y at getTerrain will be 0.
	int blendMapSize = mvpTerrainGroup->getTerrain(0, 0)->getLayerBlendMapSize();

	//i starts at 1 since blendlayer map for the second texture has the index 1.
	for (int i= 1; i<numberOfBlendMaps; i++)
	{
		//Since paging is not used in this version, x and y at getTerrain will be 0.
		Ogre::TerrainLayerBlendMap* blendMap = mvpTerrainGroup->getTerrain(0, 0)->getLayerBlendMap(i);
		blendMaps.push_back(blendMap);
		blendPointers.push_back(blendMap->getBlendPointer());

		//Loading image named blendMap"i".png from the Resource Group "Game".
		Ogre::Image img;
		img.load("blendMap" +  Ogre::StringConverter::toString(i) + ".png","Game");
		//If the size isn't right, resize.
		if(img.getWidth() != blendMapSize)
		{
			img.resize(blendMapSize, blendMapSize);
		}
		blendMapImages.push_back(img);
	}

	//Work one pixel at a time. The Blend Pointers are pointing to a one dimensionall array
	//even though the picture is 2D. Looping this way (y first, x second) works.
	for (Ogre::uint16 y = 0; y < blendMapSize; ++y)
	{
		for (Ogre::uint16 x = 0; x < blendMapSize; ++x)
		{       
			for (unsigned int i= 0; i < blendMaps.size(); i++)
			{
				Ogre::Real value;
				//The color value in the blendMapImage.
				Ogre::ColourValue colourValue = blendMapImages[i].getColourAt(x,y,0);
				//The image is black and white, yet getting information from the red channel works.
				value = colourValue.r;
				//The pixel value that the blendPointer is pointing to is getting the colour value.
				// ++ to get to next pixel for next loop.
				*blendPointers[i]++ = value;
			}
		}
	}

	//The final blending.
	for (unsigned int i= 0;i < blendMaps.size(); i++)
	{
		blendMaps[i]->dirty();
		blendMaps[i]->update();
	}

}
/*---------------------------------------------------------------------------------*/
void WorldManager::loadTerrainGeometry(const Ogre::String& name, float* data, Ogre::uint16 size, Ogre::Real worldSize, Ogre::Real minHeight, Ogre::Real maxHeight, const Ogre::Vector3& position)
{
	//Code snippet imported from http://www.ogre3d.org/wiki/index.php/BloodyMess_Ogre::Terrain_Heightfield
	//Takes a Ogre::Terrain and creates a NxOgre HeightField from the Terrain data.

	// Create the manual heightfield
	NxOgre::ManualHeightField *mhf = OGRE_NEW_T(NxOgre::ManualHeightField, Ogre::MEMCATEGORY_GENERAL)();
	mhf->begin(size, size);
	Ogre::Real normMin = -32768.0f;
	Ogre::Real normMax = 32767.0f;
	// Sample the data to the manual heightfield
	for(int x = 0; x < size; ++x)
	{
		NxOgre::Enums::HeightFieldTesselation tess = NxOgre::Enums::HeightFieldTesselation_NW_SE;
		for(int z = size-1; z >= 0; --z)
		{
			Ogre::Real height = data[(size * z) + x];
			short sample = (short)(((height - minHeight) / (maxHeight - minHeight)) * (normMax - normMin) + normMin);
			mhf->sample(sample, 0, 0, tess);
			if(tess == NxOgre::Enums::HeightFieldTesselation_NE_SW)
				tess = NxOgre::Enums::HeightFieldTesselation_NW_SE;
			else
				tess = NxOgre::Enums::HeightFieldTesselation_NE_SW;
		}
	}
	// Create the actual heightfield
	NxOgre::HeightField *hf = mhf->end(name.c_str());
	Ogre::Real hf_size = worldSize + (worldSize / size);
	Ogre::Real hf_height = (maxHeight - minHeight) / 2.0f;
	Ogre::Real hf_pose_x = position.x - (worldSize / 2.0f);
	Ogre::Real hf_pose_y = position.y + ((maxHeight + minHeight) / 2.0f);
	Ogre::Real hf_pose_z = position.z - (worldSize / 2.0f);
#if NxOgreVersionMajor <= 1 && NxOgreVersionMinor <= 5
	NxOgre::HeightFieldGeometry* hfg = new NxOgre::HeightFieldGeometry(hf, NxOgre::Vec3(hf_size, hf_height, hf_size));
	hfg->setLocalPose(NxOgre::Matrix44(NxOgre::Vec3(hf_pose_x, hf_pose_y, hf_pose_z)));
	mvpPhysicsScene->createSceneGeometry(hfg);
#else
	NxOgre::HeightFieldGeometryDescription desc(hf, NxOgre::Vec3(hf_size, hf_height, hf_size));
	mScene->createSceneGeometry(desc, NxOgre::Matrix44(NxOgre::Vec3(hf_pose_x, hf_pose_y, hf_pose_z)));
#endif
	// Free memory
	OGRE_DELETE_T(mhf, ManualHeightField, Ogre::MEMCATEGORY_GENERAL);
}
/*---------------------------------------------------------------------------------*/


/*
For shadow mapping and not shadow volumes:

Ogre::MaterialPtr WorldManager::buildDepthShadowMaterial(const Ogre::String& textureName)
{
	Ogre::String matName = "DepthShadows/" + textureName;

	Ogre::MaterialPtr ret = Ogre::MaterialManager::getSingleton().getByName(matName);
	if (ret.isNull())
	{
		Ogre::MaterialPtr baseMat = Ogre::MaterialManager::getSingleton().getByName("Ogre/shadow/depth/integrated/pssm");
		ret = baseMat->clone(matName);
		Ogre::Pass* p = ret->getTechnique(0)->getPass(0);
		p->getTextureUnitState("diffuse")->setTextureName(textureName);

		Ogre::Vector4 splitPoints;
		const Ogre::PSSMShadowCameraSetup::SplitPointList& splitPointList = 
			static_cast<Ogre::PSSMShadowCameraSetup*>(mtPSSMSetup.get())->getSplitPoints();
		for (int i = 0; i < 3; ++i)
		{
			splitPoints[i] = splitPointList[i];
		}
		p->getFragmentProgramParameters()->setNamedConstant("pssmSplitPoints", splitPoints);
	}
	return ret;
}
void WorldManager::configureShadows(bool enabled, bool depthShadows)
{
	Ogre::TerrainMaterialGeneratorA::SM2Profile* matProfile = 
		static_cast<Ogre::TerrainMaterialGeneratorA::SM2Profile*>(mvpTerrainGlobals->getDefaultMaterialGenerator()->getActiveProfile());
	matProfile->setReceiveDynamicShadowsEnabled(enabled);

	matProfile->setReceiveDynamicShadowsLowLod(false);

	// General scene setup
	//mvpSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);


	// 3 textures per directional light (PSSM)
	mvpSceneMgr->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL, 3);

	if (mtPSSMSetup.isNull())
	{
		// shadow camera setup
		Ogre::PSSMShadowCameraSetup* pssmSetup = new Ogre::PSSMShadowCameraSetup();
		pssmSetup->setSplitPadding(mvpCamera->getNearClipDistance());
		pssmSetup->calculateSplitPoints(3, mvpCamera->getNearClipDistance(), mvpSceneMgr->getShadowFarDistance());
		pssmSetup->setOptimalAdjustFactor(0, 2);
		pssmSetup->setOptimalAdjustFactor(1, 1);
		pssmSetup->setOptimalAdjustFactor(2, 0.5);

		mtPSSMSetup.bind(pssmSetup);

	}
	mvpSceneMgr->setShadowCameraSetup(mtPSSMSetup);


	mvpSceneMgr->setShadowTextureCount(3);
	mvpSceneMgr->setShadowTextureConfig(0, 2048, 2048, Ogre::PF_FLOAT32_R);
	mvpSceneMgr->setShadowTextureConfig(1, 1024, 1024, Ogre::PF_FLOAT32_R);
	mvpSceneMgr->setShadowTextureConfig(2, 1024, 1024, Ogre::PF_FLOAT32_R);
	mvpSceneMgr->setShadowTextureSelfShadow(true);
	mvpSceneMgr->setShadowCasterRenderBackFaces(true);
	mvpSceneMgr->setShadowTextureCasterMaterial("PSSM/shadow_caster");

	std::cout << "SUBENT!!\n\n\n";
	Ogre::MaterialPtr houseMat1 = buildDepthShadowMaterial("ogre.tex.jpg");
	Ogre::MaterialPtr houseMat2 = buildDepthShadowMaterial("ogreahead.tex.jpg");
	Ogre::SubEntity* sub1 = mvpSceneMgr->getEntity("Char")->getSubEntity(0);
	Ogre::SubEntity* sub2 = mvpSceneMgr->getEntity("Char")->getSubEntity(2);
	sub1->setMaterial(houseMat1);
	sub2->setMaterial(houseMat2);


	matProfile->setReceiveDynamicShadowsDepth(depthShadows);
	matProfile->setReceiveDynamicShadowsPSSM(static_cast<Ogre::PSSMShadowCameraSetup*>(mtPSSMSetup.get()));

	//addTextureShadowDebugOverlay(TL_RIGHT, 3);

}
*/