#include "WorldManager.h"

WorldManager::WorldManager(Ogre::SceneManager* s, Ogre::Camera* c, NxOgre::Scene* p, Ogre::Light* dirLight)
{
	//Init Stuff. @todo singleton bla bla maybe
	mtpSceneMgr	= s;	
	mtpCamera = c;
	mtpPhysicsScene = p;

	//Global terrain options here
	mtpTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();
	mtpTerrainGlobals->setMaxPixelError(GameFramework::getSingletonPtr()->mpSettings->mTerrainMaxPixelError);

	//Important information when calculating lightmaps
	mtpTerrainGlobals->setLightMapDirection(dirLight->getDerivedDirection());
	mtpTerrainGlobals->setCompositeMapAmbient(mtpSceneMgr->getAmbientLight());
	mtpTerrainGlobals->setCompositeMapDiffuse(dirLight->getDiffuseColour());
	// Set the distance at which to start using a composite map if present
	mtpTerrainGlobals->setCompositeMapDistance(GameFramework::getSingletonPtr()->mpSettings->mTerrainCompositeMapDistance);

	//From wikipedia:
	//Anisotropic filtering (AF) is a method of enhancing the image quality of textures on surfaces 
	//that are far away and steeply angled with respect to the point of view. Older techniques,
	//such as bilinear and trilinear filtering don't take account of the angle a surface is viewed from,
	//which can result in aliasing or blurring of textures. By reducing detail in one direction more than another,
	//these effects can be reduced.	
	Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
	Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(GameFramework::getSingletonPtr()->mpSettings->mTerrainDefaultAnisotropy);

	// FIX VARIABLES. ska kunna ändra ifrån createScene i GameState
	//Since we want the ability to load not only one terrain, we define a terrain group.
	mtpTerrainGroup = OGRE_NEW Ogre::TerrainGroup(mtpSceneMgr, Ogre::Terrain::ALIGN_X_Z,
		GameFramework::getSingletonPtr()->mpSettings->mTerrainSize, 
		GameFramework::getSingletonPtr()->mpSettings->mTerrainWorldSize);
	mtpTerrainGroup->setFilenameConvention(TERRAIN_FILE_PREFIX, TERRAIN_FILE_SUFFIX);//whats this/Per
	mtpTerrainGroup->setResourceGroup("Karma");
}

WorldManager::~WorldManager()
{
		#ifdef PAGING
		
			OGRE_DELETE mtpTerrainPaging;
			OGRE_DELETE mtpPageManager;
		
		#else
			OGRE_DELETE mtpTerrainGroup;
		#endif

		OGRE_DELETE mtpTerrainGlobals;

}

void WorldManager::addTextureLayer(Ogre::String& diffuseAndSpecMap, Ogre::String& normalAndHeightMap, Ogre::Real worldSize)
{
	Ogre::Terrain::ImportData& defaultimp = mtpTerrainGroup->getDefaultImportSettings();
	int size = defaultimp.layerList.size();
	defaultimp.layerList.resize(size + 1);
	defaultimp.layerList[size].worldSize = worldSize;
	defaultimp.layerList[size].textureNames.push_back(diffuseAndSpecMap);
	defaultimp.layerList[size].textureNames.push_back(normalAndHeightMap);
}

void WorldManager::initTerrain()
{

#ifdef PAGING
	// Paging setup
	mtpPageManager = OGRE_NEW Ogre::PageManager();
	// Since we're not loading any pages from .page files, we need a way just 
	// to say we've loaded them without them actually being loaded
	mtpPageManager->setPageProvider(&mtDummyPageProvider);
	mtpPageManager->addCamera(mtpCamera);
	mtpTerrainPaging = OGRE_NEW Ogre::TerrainPaging(mtpPageManager);
	Ogre::PagedWorld* world = mtpPageManager->createWorld();
	mtpTerrainPaging->createWorldSection(world, mtpTerrainGroup, 120, 240,  
		TERRAIN_PAGE_MIN_X, TERRAIN_PAGE_MIN_Y, 
		TERRAIN_PAGE_MAX_X, TERRAIN_PAGE_MAX_Y);

#else
	mtpTerrainGroup->setOrigin(Ogre::Vector3(0,GameFramework::getSingletonPtr()->mpSettings->mTerrainAdjustY,0));

	//@todo borde kanske kunna sätta dessa värden som parametrar i initTerrain. olika värden för olika banor
	// Configure default import settings
	Ogre::Terrain::ImportData& defaultimp = mtpTerrainGroup->getDefaultImportSettings();	
	defaultimp.terrainSize = GameFramework::getSingletonPtr()->mpSettings->mTerrainSize;
	defaultimp.worldSize = GameFramework::getSingletonPtr()->mpSettings->mTerrainWorldSize;
	defaultimp.inputScale = GameFramework::getSingletonPtr()->mpSettings->mTerrainInputScale;
	defaultimp.minBatchSize = GameFramework::getSingletonPtr()->mpSettings->mTerrainMinBatchSize;
	defaultimp.maxBatchSize = GameFramework::getSingletonPtr()->mpSettings->mTerrainMaxBatchSize;

	for (long x = TERRAIN_PAGE_MIN_X; x <= TERRAIN_PAGE_MAX_X; ++x)
		for (long y = TERRAIN_PAGE_MIN_Y; y <= TERRAIN_PAGE_MAX_Y; ++y)
			defineTerrain(x, y);
	// sync load since we want everything in place when we start
	mtpTerrainGroup->loadAllTerrains(true);
	mtpCamera->setPolygonMode(Ogre::PM_WIREFRAME);
#endif


	//If new terrains were imported, we have to define how the texture layers are blending
	if (mtTerrainsImported)
	{
		Ogre::TerrainGroup::TerrainIterator ti = mtpTerrainGroup->getTerrainIterator();
		while(ti.hasMoreElements())
		{
			Ogre::Terrain* t = ti.getNext()->instance;
			initBlendMaps(t);
			std::cout << "HEJ NY INITBLEND\n";
		}
		//Saves the terrains for further use.
		mtpTerrainGroup->saveAllTerrains(true);
	}

	mtpTerrainGroup->freeTemporaryResources();

	//@todo fixa texture skuggor. mjuk kant
	//configureShadows(true,true);
}


void WorldManager::defineTerrain(long x, long y)
{
	Ogre::String filename = mtpTerrainGroup->generateFilename(x, y);
	if (Ogre::ResourceGroupManager::getSingleton().resourceExists(mtpTerrainGroup->getResourceGroup(), filename))
	{
		mtpTerrainGroup->defineTerrain(x, y);
	}
	else
	{
		Ogre::Image img;
		img.load("terrain.png", "Karma");
		mtpTerrainGroup->defineTerrain(x, y, &img);
		//getTerrainImage(x % 2 != 0, y % 2 != 0, img);
		//mtpTerrainGroup->defineTerrain(x, y, &img);
		mtTerrainsImported = true;
	}
}
//---------------------------------------------------------------------------------
void WorldManager::buildNxOgreTerrain()
{
	GameFramework::getSingletonPtr()->mpLog->logMessage("Building NxOgre Terrain files...");

	Ogre::TerrainGroup*	mTempTerrainGroup;

	mTempTerrainGroup = OGRE_NEW Ogre::TerrainGroup(mtpSceneMgr, Ogre::Terrain::ALIGN_X_Z,
		GameFramework::getSingletonPtr()->mpSettings->mTerrainSize, 
		GameFramework::getSingletonPtr()->mpSettings->mTerrainWorldSize);
	mTempTerrainGroup->setFilenameConvention(TERRAIN_FILE_PREFIX, TERRAIN_FILE_SUFFIX);//whats this/Per
	mTempTerrainGroup->setResourceGroup("Karma");

	Ogre::Terrain::ImportData& defaultimp = mTempTerrainGroup->getDefaultImportSettings();	
	defaultimp.terrainSize = GameFramework::getSingletonPtr()->mpSettings->mTerrainSize;
	defaultimp.worldSize = GameFramework::getSingletonPtr()->mpSettings->mTerrainWorldSize;
	defaultimp.inputScale = GameFramework::getSingletonPtr()->mpSettings->mTerrainInputScale;
	defaultimp.minBatchSize = GameFramework::getSingletonPtr()->mpSettings->mTerrainMinBatchSize;
	defaultimp.maxBatchSize = GameFramework::getSingletonPtr()->mpSettings->mTerrainMaxBatchSize;
		
	for (long x = TERRAIN_PAGE_MIN_X; x <= TERRAIN_PAGE_MAX_X; ++x)
	{
		for (long y = TERRAIN_PAGE_MIN_Y; y <= TERRAIN_PAGE_MAX_Y; ++y)
		{
			Ogre::String filename = mTempTerrainGroup->generateFilename(x, y);
			if (Ogre::ResourceGroupManager::getSingleton().resourceExists(mTempTerrainGroup->getResourceGroup(), filename))
			{
				mTempTerrainGroup->defineTerrain(x, y);
			}
		}
	}
mTempTerrainGroup->loadAllTerrains(true);

	Ogre::TerrainGroup::TerrainIterator ti = mTempTerrainGroup->getTerrainIterator();
	while(ti.hasMoreElements())
	{
		Ogre::Terrain* t = ti.getNext()->instance;
		loadTerrainGeometry(t->getMaterialName(), t->getHeightData(), t->getSize(),
			t->getWorldSize(), t->getMinHeight(), t->getMaxHeight(), t->getPosition());
	}
	mTempTerrainGroup->freeTemporaryResources();
	OGRE_DELETE mTempTerrainGroup;
	GameFramework::getSingletonPtr()->mpLog->logMessage("NxOgre Terrain files was created!");
}
/*---------------------------------------------------------------------------------*/
void WorldManager::configureShadows(bool enabled, bool depthShadows)
{
	Ogre::TerrainMaterialGeneratorA::SM2Profile* matProfile = 
		static_cast<Ogre::TerrainMaterialGeneratorA::SM2Profile*>(mtpTerrainGlobals->getDefaultMaterialGenerator()->getActiveProfile());
	matProfile->setReceiveDynamicShadowsEnabled(enabled);

	matProfile->setReceiveDynamicShadowsLowLod(false);

	// General scene setup
	//mtpSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);


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


	/*mtpSceneMgr->setShadowTextureCount(3);
	mtpSceneMgr->setShadowTextureConfig(0, 2048, 2048, Ogre::PF_FLOAT32_R);
	mtpSceneMgr->setShadowTextureConfig(1, 1024, 1024, Ogre::PF_FLOAT32_R);
	mtpSceneMgr->setShadowTextureConfig(2, 1024, 1024, Ogre::PF_FLOAT32_R);
	mtpSceneMgr->setShadowTextureSelfShadow(true);
	mtpSceneMgr->setShadowCasterRenderBackFaces(true);
	mtpSceneMgr->setShadowTextureCasterMaterial("PSSM/shadow_caster");

	std::cout << "SUBENT!!\n\n\n";
	Ogre::MaterialPtr houseMat1 = buildDepthShadowMaterial("ogre.tex.jpg");
	Ogre::MaterialPtr houseMat2 = buildDepthShadowMaterial("ogreahead.tex.jpg");
	Ogre::SubEntity* sub1 = mtpSceneMgr->getEntity("Char")->getSubEntity(0);
	Ogre::SubEntity* sub2 = mtpSceneMgr->getEntity("Char")->getSubEntity(2);
	sub1->setMaterial(houseMat1);
	sub2->setMaterial(houseMat2);*/


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
	//Start with checking how many texture layers that were added.
	//Since we can't blend the bottom texture, the needed amount of blend maps is nr of layers -1
	int numberOfBlendMaps = mtpTerrainGroup->getDefaultImportSettings().layerList.size();

	//Creating a vector of pointers to the blendLayerMaps
	std::vector<Ogre::TerrainLayerBlendMap*> blendMaps;

	//Blend pointers point to coordinates at the blendLayerMaps.
	std::vector<float*> blendPointers;

	//The images we're going to get the blend data from
	std::vector<Ogre::Image> blendMapImages;

	//@todo (0,0)
	int blendMapSize = terrain->getLayerBlendMapSize();

	//i starts at 1 since blendlayer map for the second texture has the index 1.
	for (int i= 1; i<numberOfBlendMaps; i++)
	{
		//@todo. fixa 0,0 kanske om terrain paging
		Ogre::TerrainLayerBlendMap* blendMap = terrain->getLayerBlendMap(i);
		blendMaps.push_back(blendMap);
		blendPointers.push_back(blendMap->getBlendPointer());

		//Loading image named blendMap"i".png from the Resource Group "Karma".
		Ogre::Image img;
		img.load("blendMap" +  Ogre::StringConverter::toString(i) + ".png","Karma");
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