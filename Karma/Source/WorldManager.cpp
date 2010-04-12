#include "WorldManager.h"

WorldManager::WorldManager(Ogre::SceneManager* s, Ogre::Camera* c, NxOgre::Scene* p)
{
	//Init Stuff. @todo singleton bla bla maybe
	mtpSceneMgr	= s;	
	mtpCamera = c;
	mtpPhysicsScene = p;
}

WorldManager::~WorldManager()
{
}

void WorldManager::initTerrain(Ogre::Light* light )
{
	bool blankTerrain = false;

	mtpTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();
	// Configure global
	// FIX VARIABLES. ska kunna ändra ifrån createScene i GameState
	mtpTerrainGlobals->setMaxPixelError(1);

	//From mimi.hu:
	//Composite map
	//A single map created by joining together two or more maps that have been digitised seperately. 2
	//connectivity ...

	// FIX VARIABLES. ska kunna ändra ifrån createScene i GameState
	// Important to set these so that the terrain knows what to use for derived (non-realtime) data
	mtpTerrainGlobals->setLightMapDirection(light->getDerivedDirection());
	mtpTerrainGlobals->setCompositeMapAmbient(mtpSceneMgr->getAmbientLight());
	mtpTerrainGlobals->setCompositeMapDiffuse(light->getDiffuseColour());
	mtpTerrainGlobals->setCompositeMapDistance(30);

	// Testing stuff included in terrain.h in Sample

	//mtpTerrainGlobals->setUseRayBoxDistanceCalculation(true);
	//mtpTerrainGlobals->getDefaultMaterialGenerator()->setDebugLevel(1);
	//mtpTerrainGlobals->setLightMapSize(256);
	//matProfile->setLightmapEnabled(false);
	//mtpTerrainGlobals->setCompositeMapAmbient(ColourValue::Red);


	//From wikipedia:
	//Anisotropic filtering (AF) is a method of enhancing the image quality of textures on surfaces 
	//that are far away and steeply angled with respect to the point of view. Older techniques,
	//such as bilinear and trilinear filtering don't take account of the angle a surface is viewed from,
	//which can result in aliasing or blurring of textures. By reducing detail in one direction more than another,
	//these effects can be reduced.

	Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
	// FIX VARIABLES. ska kunna ändra ifrån createScene i GameState
	Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(7);

	// FIX VARIABLES. ska kunna ändra ifrån createScene i GameState
	//Since we want the ability to load not only one terrain, we define a terrain group.
	mtpTerrainGroup = OGRE_NEW Ogre::TerrainGroup(mtpSceneMgr, Ogre::Terrain::ALIGN_X_Z, TERRAIN_SIZE, 120);
	mtpTerrainGroup->setFilenameConvention(TERRAIN_FILE_PREFIX, TERRAIN_FILE_SUFFIX);//whats this/Per
	mtpTerrainGroup->setOrigin(Ogre::Vector3(0,-3,0));

	// FIX VARIABLES. ska kunna ändra ifrån createScene i GameState
	// Configure default import settings for if we use imported image
	Ogre::Terrain::ImportData& defaultimp = mtpTerrainGroup->getDefaultImportSettings();
	defaultimp.terrainSize = TERRAIN_SIZE;
	defaultimp.worldSize = 120;
	defaultimp.inputScale = 6;
	defaultimp.minBatchSize = 33;
	defaultimp.maxBatchSize = 65;
	// textures
	defaultimp.layerList.resize(3);
	defaultimp.layerList[0].worldSize = 100/100;
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
	defaultimp.layerList[1].worldSize = 30/100;
	defaultimp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
	defaultimp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
	defaultimp.layerList[2].worldSize = 200/100;
	defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
	defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");


	Ogre::String filename = mtpTerrainGroup->generateFilename(0, 0);
	if (Ogre::ResourceGroupManager::getSingleton().resourceExists(mtpTerrainGroup->getResourceGroup(), filename))
	{
		mtpTerrainGroup->defineTerrain(0, 0);
	}
	else
	{
		Ogre::Image img;
		img.load("terrain.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		mtpTerrainGroup->defineTerrain(0, 0, &img);
		mtTerrainsImported = true;
	}	

	// sync load since we want everything in place when we start
	mtpTerrainGroup->loadAllTerrains(true);

	if (mtTerrainsImported)
	{
		Ogre::TerrainGroup::TerrainIterator ti = mtpTerrainGroup->getTerrainIterator();
		while(ti.hasMoreElements())
		{
			Ogre::Terrain* t = ti.getNext()->instance;
			initBlendMaps(t);
		}
	}

	mtpTerrainGroup->freeTemporaryResources();
	//configureShadows(true,true);
}

//---------------------------------------------------------------------------------
void WorldManager::buildNxOgreTerrain()
{
	GameFramework::getSingletonPtr()->mpLog->logMessage("Building NxOgre Terrain files...");
	Ogre::TerrainGroup::TerrainIterator ti = mtpTerrainGroup->getTerrainIterator();
	while(ti.hasMoreElements())
	{
		Ogre::Terrain* t = ti.getNext()->instance;
		loadTerrainGeometry(t->getMaterialName(), t->getHeightData(), t->getSize(),
			t->getWorldSize(), t->getMinHeight(), t->getMaxHeight(), t->getPosition());
	}
	GameFramework::getSingletonPtr()->mpLog->logMessage("NxOgre Terrain files was created!");
}
//---------------------------------------------------------------------------------
void WorldManager::configureShadows(bool enabled, bool depthShadows)
{
	Ogre::TerrainMaterialGeneratorA::SM2Profile* matProfile = 
		static_cast<Ogre::TerrainMaterialGeneratorA::SM2Profile*>(mtpTerrainGlobals->getDefaultMaterialGenerator()->getActiveProfile());
	matProfile->setReceiveDynamicShadowsEnabled(enabled);

	matProfile->setReceiveDynamicShadowsLowLod(false);

	// General scene setup
	mtpSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
	mtpSceneMgr->setShadowFarDistance(3000);

	// 3 textures per directional light (PSSM)
	mtpSceneMgr->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL, 3);

	if (mtPSSMSetup.isNull())
	{
		// shadow camera setup
		Ogre::PSSMShadowCameraSetup* pssmSetup = new Ogre::PSSMShadowCameraSetup();
		pssmSetup->setSplitPadding(mtpCamera->getNearClipDistance());
		pssmSetup->calculateSplitPoints(3, mtpCamera->getNearClipDistance(), mtpSceneMgr->getShadowFarDistance());
		pssmSetup->setOptimalAdjustFactor(0, 2);
		pssmSetup->setOptimalAdjustFactor(1, 1);
		pssmSetup->setOptimalAdjustFactor(2, 0.5);

		mtPSSMSetup.bind(pssmSetup);

	}
	mtpSceneMgr->setShadowCameraSetup(mtPSSMSetup);


	mtpSceneMgr->setShadowTextureCount(3);
	mtpSceneMgr->setShadowTextureConfig(0, 2048, 2048, Ogre::PF_FLOAT32_R);
	mtpSceneMgr->setShadowTextureConfig(1, 1024, 1024, Ogre::PF_FLOAT32_R);
	mtpSceneMgr->setShadowTextureConfig(2, 1024, 1024, Ogre::PF_FLOAT32_R);
	mtpSceneMgr->setShadowTextureSelfShadow(true);
	mtpSceneMgr->setShadowCasterRenderBackFaces(true);
	mtpSceneMgr->setShadowTextureCasterMaterial("PSSM/shadow_caster");


	matProfile->setReceiveDynamicShadowsDepth(depthShadows);
	matProfile->setReceiveDynamicShadowsPSSM(static_cast<Ogre::PSSMShadowCameraSetup*>(mtPSSMSetup.get()));

	//addTextureShadowDebugOverlay(TL_RIGHT, 3);





}

//---------------------------------------------------------------------------------
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
	mtpPhysicsScene->createSceneGeometry(hfg);
#else
	NxOgre::HeightFieldGeometryDescription desc(hf, NxOgre::Vec3(hf_size, hf_height, hf_size));
	mScene->createSceneGeometry(desc, NxOgre::Matrix44(NxOgre::Vec3(hf_pose_x, hf_pose_y, hf_pose_z)));
#endif
	// Free memory
	OGRE_DELETE_T(mhf, ManualHeightField, Ogre::MEMCATEGORY_GENERAL);
}
//---------------------------------------------------------------------------------
void WorldManager::initBlendMaps(Ogre::Terrain* terrain)
{
	Ogre::TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
	Ogre::TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
	Ogre::Real minHeight0 = 70;
	Ogre::Real fadeDist0 = 40;
	Ogre::Real minHeight1 = 70;
	Ogre::Real fadeDist1 = 15;
	float* pBlend1 = blendMap1->getBlendPointer();
	for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
	{
		for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
		{
			Ogre::Real tx, ty;

			blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
			Ogre::Real height = terrain->getHeightAtTerrainPosition(tx, ty);
			Ogre::Real val = (height - minHeight0) / fadeDist0;
			val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
			//*pBlend0++ = val;

			val = (height - minHeight1) / fadeDist1;
			val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
			*pBlend1++ = val;
		}
	}
	blendMap0->dirty();
	blendMap1->dirty();
	//blendMap0->loadImage("blendmap1.png", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	blendMap0->update();
	blendMap1->update();

	// set up a colour map
	/*
	if (!terrain->getGlobalColourMapEnabled())
	{
	terrain->setGlobalColourMapEnabled(true);
	Image colourMap;
	colourMap.load("testcolourmap.jpg", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	terrain->getGlobalColourMap()->loadImage(colourMap);
	}
	*/

}