//|||||||||||||||||||||||||||||||||||||||||||||||

#include "GameState.h"



//|||||||||||||||||||||||||||||||||||||||||||||||

using namespace Ogre; //BORT SKA DU

//|||||||||||||||||||||||||||||||||||||||||||||||

GameState::GameState()
{
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::enter()
{
	
	if(!Ogre::ResourceGroupManager::getSingletonPtr()->resourceGroupExists("Game"))
		GameFramework::getSingletonPtr()->loadGameResources();
	
	Ogre::OverlayManager::getSingletonPtr()->getByName("Loading")->hide();
	GameFramework::getSingletonPtr()->mpGui->initGui();

	Settings::getSingletonPtr()->loadSettingsFromFile();
	GameFramework::getSingletonPtr()->mpLog->logMessage("Entering GameState...");
	mtpSceneMgr = GameFramework::getSingletonPtr()->mpRoot->createSceneManager(ST_GENERIC, "GameSceneMgr");     

	mtpCamera = mtpSceneMgr->createCamera("GameCamera");
	mtpCamera->setNearClipDistance(*Settings::getSingletonPtr()->mNearClipDistance);
	mtpCamera->setFarClipDistance(*Settings::getSingletonPtr()->mFarClipDistance);

	mtpCamera->setAspectRatio(Real(GameFramework::getSingletonPtr()->mpViewport->getActualWidth()) / 
		Real(GameFramework::getSingletonPtr()->mpViewport->getActualHeight()));

	GameFramework::getSingletonPtr()->mpViewport->setCamera(mtpCamera);

	mtpCamera->setAspectRatio(Real(GameFramework::getSingletonPtr()->mpViewport->getActualWidth()) / 
		Real(GameFramework::getSingletonPtr()->mpViewport->getActualHeight()));

	GameFramework::getSingletonPtr()->mpViewport->setCamera(mtpCamera);

	GameFramework::getSingletonPtr()->mpKeyboard->setEventCallback(this);
	GameFramework::getSingletonPtr()->mpMouse->setEventCallback(this);
	mvRightButton = false;

	const OIS::MouseState state = GameFramework::getSingletonPtr()->mpMouse->getMouseState();

	
	mvQuit = false;
	
	createScene();
	GameFramework::getSingletonPtr()->mpSound->playBackgroundSound();
	Settings::getSingletonPtr()->deletePointers();
	std::srand ( time(NULL) );	
	GameFramework::getSingletonPtr()->getSingletonPtr()->mpSound->initGameStateSound();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::pause()
{
	GameFramework::getSingletonPtr()->mpLog->logMessage("Pausing GameState...");
	mvpPhysicsMgr->pause();
	GameFramework::getSingletonPtr()->mpGui->changeCursor(Game::Cursor_Normal);
	GameFramework::getSingletonPtr()->mpGui->toggleGameGUI(false);
	GameFramework::getSingletonPtr()->mpSound->stopBackgroundSound();
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::resume()
{
	GameFramework::getSingletonPtr()->mpSound->playBackgroundSound();
	GameFramework::getSingletonPtr()->mpLog->logMessage("Resuming GameState...");
	Ogre::OverlayManager::getSingletonPtr()->getByName("Loading")->hide();
	GameFramework::getSingletonPtr()->mpViewport->setCamera(mtpCamera);
	mvpPhysicsMgr->resume();
	GameFramework::getSingletonPtr()->mpGui->toggleGameGUI(true);
	if ((m_Character->getPowerUp() & Game::PowerUp_GunMode) != 0)
		GameFramework::getSingletonPtr()->mpGui->changeCursor(Game::Cursor_Aim);
	mvQuit = false;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::exit()
{
	GameFramework::getSingletonPtr()->mpLog->logMessage("Leaving GameState...");
	GameFramework::getSingletonPtr()->mpSound->stopBackgroundSound();
	mvpPhysicsMgr->destroyPhysicsWorld();
	delete mvpWorld;
	BulletHoles::getSingletonPtr()->resetBulletHoles();
	Chunks::getSingletonPtr()->destroyAll();

	GameFramework::getSingletonPtr()->getSingletonPtr()->mpSound->removeGameStateSounds();

	mtpSceneMgr->destroyCamera(mtpCamera);
	if(mtpSceneMgr)
		GameFramework::getSingletonPtr()->mpRoot->destroySceneManager(mtpSceneMgr);
}
void GameState::createMotionBlurEffects()
{
	Ogre::CompositorPtr comp3 = Ogre::CompositorManager::getSingleton().create(
		"Motion Blur", "Game");
	{
		Ogre::CompositionTechnique *t = comp3->createTechnique();
		{
			Ogre::CompositionTechnique::TextureDefinition *def = t->createTextureDefinition("scene");
			def->width = 0;
			def->height = 0;
			def->formatList.push_back(Ogre::PF_R8G8B8);
		}
		{
			Ogre::CompositionTechnique::TextureDefinition *def = t->createTextureDefinition("sum");
			def->width = 0;
			def->height = 0;
			def->formatList.push_back(Ogre::PF_R8G8B8);
		}
		{
			Ogre::CompositionTechnique::TextureDefinition *def = t->createTextureDefinition("temp");
			def->width = 0;
			def->height = 0;
			def->formatList.push_back(Ogre::PF_R8G8B8);
		}
		/// Render scene
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_PREVIOUS);
			tp->setOutputName("scene");
		}
		/// Initialisation pass for sum texture
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_PREVIOUS);
			tp->setOutputName("sum");
			tp->setOnlyInitial(true);
		}
		/// Do the motion blur
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
			tp->setOutputName("temp");
			{ Ogre::CompositionPass *pass = tp->createPass();
			pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
			pass->setMaterialName("Ogre/Compositor/Combine");
			pass->setInput(0, "scene");
			pass->setInput(1, "sum");
			}
		}
		/// Copy back sum texture
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
			tp->setOutputName("sum");
			{ Ogre::CompositionPass *pass = tp->createPass();
			pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
			pass->setMaterialName("Ogre/Compositor/Copyback");
			pass->setInput(0, "temp");
			}
		}
		/// Display result
		{
			Ogre::CompositionTargetPass *tp = t->getOutputTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
			{ Ogre::CompositionPass *pass = tp->createPass();
			pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
			pass->setMaterialName("Ogre/Compositor/MotionBlur");
			pass->setInput(0, "sum");
			}
		}
	}
}

void GameState::createScene()
{
	//Create physics
	mvpPhysicsMgr = new PhysicsManager(mtpSceneMgr);

	createMotionBlurEffects();
	Ogre::CompositorManager::getSingleton().addCompositor(GameFramework::getSingletonPtr()->mpViewport, "Motion Blur");

	//Shadow Technique. Stencil shadows.  @todo texture shadows
	mtpSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_MODULATIVE);
	//Fog @todo
	mtpSceneMgr->setSkyBox(true, *Settings::getSingletonPtr()->mSkyBox,150);	
	//mtpSceneMgr->setFog(FOG_LINEAR, ColourValue(*Settings::getSingletonPtr()->mFogColor,*Settings::getSingletonPtr()->mFogColor, *Settings::getSingletonPtr()->mFogColor), 0, *Settings::getSingletonPtr()->mFogStart, *Settings::getSingletonPtr()->mFogEnd);

	//Directional light
	Ogre::Vector3 lightdir(*Settings::getSingletonPtr()->mDirectionalLightDirX, *Settings::getSingletonPtr()->mDirectionalLightDirY, *Settings::getSingletonPtr()->mDirectionalLightDirZ);
	lightdir.normalise();
	mtpSceneMgr->setShadowFarDistance(*Settings::getSingletonPtr()->mShadowFarDistance);
	Ogre::Light* l = mtpSceneMgr->createLight("dirLight");
	l->setType(Light::LT_DIRECTIONAL);
	l->setDirection(lightdir);
	l->setDiffuseColour(ColourValue(0.97f, 0.90f, 0.70f));
	l->setSpecularColour(ColourValue(0.2f, 0.2f, 0.2f));
	 
	//Ambient light and skybox
	mtpSceneMgr->setAmbientLight(ColourValue(*Settings::getSingletonPtr()->mAmbientLight, *Settings::getSingletonPtr()->mAmbientLight,*Settings::getSingletonPtr()->mAmbientLight));

	//For screenshotting minimap
	mtpSceneMgr->getRootSceneNode()->createChildSceneNode("CamFree");
	
	Chunks::getSingletonPtr()->createGrid();
	//Create Character
	m_Character = new Player( mtpSceneMgr, mvpPhysicsMgr->getPhysicsScene(),mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlender.mesh" ,"",
		Ogre::Vector3(*Settings::getSingletonPtr()->mPlayerSpawnPointX,*Settings::getSingletonPtr()->mPlayerSpawnPointY,*Settings::getSingletonPtr()->mPlayerSpawnPointZ),100,1);
	NPC::setPlayerNode(m_Character->getPlayerNode());
	Aimer::setPhysicsScene(mvpPhysicsMgr->getPhysicsScene());
	BulletHoles::getSingletonPtr()->setSceneManager(mtpSceneMgr);

	//Create camera
	CameraHandler::getSingletonPtr()->initCameraHandler(mtpCamera,mvpPhysicsMgr->getPhysicsRenderSystem(), mtpSceneMgr, m_Character);
	
	//Create PowerUp class. 
	m_PowerUps = new PowerUp(mvpPhysicsMgr->getPhysicsScene(), mtpSceneMgr,m_Character->getHitBox(),m_Character);

	m_PowerUps->addPowerUp(Ogre::Vector3(-202,10,202),"BarrelSuperJump");
	m_PowerUps->addPowerUp(Ogre::Vector3(-201,11.6,160),"BarrelSuperSpeed");
	m_PowerUps->addPowerUp(Ogre::Vector3(-168,23,-94),"BarrelMoveBox");
	m_PowerUps->addPowerUp(Ogre::Vector3(-161,34,-170),"BarrelPistol");
	m_PowerUps->addPowerUp(Ogre::Vector3(70,53,82),"BarrelMachineGun");
	m_PowerUps->addPowerUp(Ogre::Vector3(205,25,-38),"BarrelRocketBoots");

	for (int i=0; i<2; i++)
	{
		for (int j=0; j<2; j++)
		{
			for (int k=0; k<3; k++)
			{
				OGRE3DBody*	cube = mvpPhysicsMgr->getPhysicsRenderSystem()->createBody(new NxOgre::Box(1, 1, 1), NxOgre::Vec3(-162 + (k+0.1), 34+i, -166 +(j+0.1)), "cube.1m.mesh");
			}
		}
	}
	mvpPhysicsMgr->getPhysicsRenderSystem()->createBody(new NxOgre::Box(1, 1, 1), NxOgre::Vec3(116,26,50), "cube.1m.mesh");
	mvpPhysicsMgr->getPhysicsRenderSystem()->createBody(new NxOgre::Box(1, 1, 1), NxOgre::Vec3(116.5,26,52), "cube.1m.mesh");
	mvpPhysicsMgr->getPhysicsRenderSystem()->createBody(new NxOgre::Box(1, 1, 1), NxOgre::Vec3(115,26.5,47.2), "cube.1m.mesh");
	mvpPhysicsMgr->getPhysicsRenderSystem()->createBody(new NxOgre::Box(1, 1, 1), NxOgre::Vec3(113.9,26,45.2), "cube.1m.mesh");

	//------------- END ---------------*/

	mvpWorld = new WorldManager(mtpSceneMgr, mtpCamera,mvpPhysicsMgr->getPhysicsScene(),l);

	//Behöver bara laddas en gång. Om man har en .dat fil är detta onödigt @ todo fixa kanske
	Ogre::String difSpecMap = "dirt_grayrocky_diffusespecular.dds";
	Ogre::String normHeightMap = "dirt_grayrocky_normalheight.dds";
	mvpWorld->addTextureLayer(difSpecMap, normHeightMap , *Settings::getSingletonPtr()->mTerrainTextureWorldSize);
	difSpecMap = "grass_green-01_diffusespecular.dds";
	normHeightMap = "grass_green-01_normalheight.dds";
	mvpWorld->addTextureLayer(difSpecMap,normHeightMap , *Settings::getSingletonPtr()->mTerrainTextureWorldSize);
	difSpecMap = "growth_weirdfungus-03_diffusespecular.dds";
	normHeightMap = "growth_weirdfungus-03_normalheight.dds";
	mvpWorld->addTextureLayer(difSpecMap, normHeightMap, *Settings::getSingletonPtr()->mTerrainTextureWorldSize);

	mvpWorld->initTerrain();
	mvpWorld->buildNxOgreTerrain();

	createMotionBlurEffects();
	Ogre::CompositorManager::getSingleton().addCompositor(GameFramework::getSingletonPtr()->mpViewport, "Motion Blur");

	new NPCAimer ( mtpSceneMgr,  mvpPhysicsMgr->getPhysicsScene(),mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy1", Ogre::Vector3(-127,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(-127,100,160),160),0.2,100,3);
	new NPCAimer ( mtpSceneMgr, mvpPhysicsMgr->getPhysicsScene(),mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy2", Ogre::Vector3(-122,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(-122,100,14),14),0.2,100,3);
	new NPCAimer ( mtpSceneMgr, mvpPhysicsMgr->getPhysicsScene(),mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy3", Ogre::Vector3(-46,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(-46,100,-146),-146),0.2,100,3);
	new NPCAimer ( mtpSceneMgr, mvpPhysicsMgr->getPhysicsScene(),mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy4", Ogre::Vector3(33,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(33,100,-112),-112),0.2,100,3);
	new NPCAimer ( mtpSceneMgr, mvpPhysicsMgr->getPhysicsScene(),mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy5", Ogre::Vector3(88,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(88,100,-183),-183),0.2,100,3);
	new NPCAimer ( mtpSceneMgr,mvpPhysicsMgr->getPhysicsScene(), mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy6", Ogre::Vector3(148,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(148,100,-206),-206),0.2,100,3);
	new NPCAimer ( mtpSceneMgr,mvpPhysicsMgr->getPhysicsScene(), mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy7", Ogre::Vector3(142,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(142,100,-88),-88),0.2,100,3);
	new NPCAimer ( mtpSceneMgr,mvpPhysicsMgr->getPhysicsScene(), mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy8", Ogre::Vector3(91,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(91,100,-54),-54),0.2,100,3);
	new NPCAimer ( mtpSceneMgr,mvpPhysicsMgr->getPhysicsScene(), mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy9", Ogre::Vector3(20,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(20,100,3),3),0.2,100,3);
	new NPCAimer ( mtpSceneMgr,mvpPhysicsMgr->getPhysicsScene(), mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy10", Ogre::Vector3(205,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(205,100,-35.6),-35.6),0.2,100,3);
	new NPCAimer ( mtpSceneMgr,mvpPhysicsMgr->getPhysicsScene(), mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy11", Ogre::Vector3(209,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(209,100,-34),-34),0.2,100,3);
	new NPCAimer ( mtpSceneMgr,mvpPhysicsMgr->getPhysicsScene(), mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy12", Ogre::Vector3(206,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(206,100,-49),-49),0.2,100,3);
	new NPCAimer ( mtpSceneMgr,mvpPhysicsMgr->getPhysicsScene(), mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy13", Ogre::Vector3(203,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(203,100,-118),118),0.2,100,3);
	new NPCAimer ( mtpSceneMgr,mvpPhysicsMgr->getPhysicsScene(), mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy14", Ogre::Vector3(178,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(178,100,-176),176),0.2,100,3);
	new NPCAimer ( mtpSceneMgr,mvpPhysicsMgr->getPhysicsScene(), mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy15", Ogre::Vector3(108,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(108,100,-193),193),0.2,100,3);
	new NPCAimer ( mtpSceneMgr,mvpPhysicsMgr->getPhysicsScene(), mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy16", Ogre::Vector3(129,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(129,3,-58),-58),0.2,100,3);
	new NPCAimer ( mtpSceneMgr,mvpPhysicsMgr->getPhysicsScene(), mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy17", Ogre::Vector3(120,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(120,3,-12),-12),0.2,100,3);

	SceneLoader* sceneLoad = new SceneLoader();
	sceneLoad->parseLevel("scen.scene","Game",mtpSceneMgr,mtpSceneMgr->getRootSceneNode(),mvpPhysicsMgr,mvpWorld->getTerrainAt(0,0));

}
//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
	if(GameFramework::getSingletonPtr()->mpKeyboard->isKeyDown(OIS::KC_ESCAPE))
	{
		mvQuit = true;
		return true;
	}

	switch (keyEventRef.key)		
	{
	case OIS::KC_R:
		m_Character->setPowerUp(Game::PowerUp_None);
		CameraHandler::getSingletonPtr()->setCamMode(Game::Cam_ThirdPerson);
		GameFramework::getSingletonPtr()->mpGui->changeCursor(Game::Cursor_Normal);
		break;
	case OIS::KC_2:
		if (GameFramework::getSingletonPtr()->mpGui->isLoaded(Game::PowerUp_SuperSpeed))
			m_Character->setPowerUp(Game::PowerUp_SuperSpeed,3.0f);
		break;
	case OIS::KC_1:
		if (GameFramework::getSingletonPtr()->mpGui->isLoaded(Game::PowerUp_SuperJump))
			m_Character->setPowerUp(Game::PowerUp_SuperJump);
		break;
	case OIS::KC_3:
		if (GameFramework::getSingletonPtr()->mpGui->isLoaded(Game::PowerUp_MoveBoxMode))
		{
			m_Character->setPowerUp(Game::PowerUp_MoveBoxMode);
			GameFramework::getSingletonPtr()->mpGui->changeCursor(Game::Cursor_MoveBox);
			CameraHandler::getSingletonPtr()->setCamMode(Game::Cam_MixCursorMovable);
		}
		break;
	case OIS::KC_4:
		if (GameFramework::getSingletonPtr()->mpGui->isLoaded(Game::PowerUp_GunModePistol))
		{
			m_Character->setPowerUp(Game::PowerUp_GunMode);
			GameFramework::getSingletonPtr()->mpGui->changeCursor(Game::Cursor_Aim);
			m_Character->setPowerUp(Game::PowerUp_GunModePistol);
			CameraHandler::getSingletonPtr()->setCamMode(Game::Cam_MixCursorMovable);
		}
		break;
	case OIS::KC_Z:
		if (GameFramework::getSingletonPtr()->mpGui->isLoaded(Game::PowerUp_GunModePistol))
		{
			m_Character->setPowerUp(Game::PowerUp_GunMode);
			GameFramework::getSingletonPtr()->mpGui->changeCursor(Game::Cursor_Aim);
			m_Character->setPowerUp(Game::PowerUp_GunModePistol);
			CameraHandler::getSingletonPtr()->setCamMode(Game::Cam_MixCursorCenter);
		}
		break;
	case OIS::KC_5:
		if (GameFramework::getSingletonPtr()->mpGui->isLoaded(Game::PowerUp_GunModeMachineGun))
		{
			m_Character->setPowerUp(Game::PowerUp_GunMode);
			GameFramework::getSingletonPtr()->mpGui->changeCursor(Game::Cursor_Aim);
			m_Character->setPowerUp(Game::PowerUp_GunModeMachineGun);
			CameraHandler::getSingletonPtr()->setCamMode(Game::Cam_FirstPerson);
		}
		break;
	case OIS::KC_6:
		if (GameFramework::getSingletonPtr()->mpGui->isLoaded(Game::PowerUp_RocketBootsMode))
		{
			m_Character->setPowerUp(Game::PowerUp_RocketBootsMode);
			GameFramework::getSingletonPtr()->mpGui->loadCastingBar(Game::PowerUp_RocketBootsMode,1.5f);
			GameFramework::getSingletonPtr()->mpGui->updateCastingBar(1.5f);
		}
		break;
	case OIS::KC_SPACE:
		if ((m_Character->getPowerUp() & Game::PowerUp_RocketBootsMode) != 0)
			m_Character->startRocketBoots();
		else if ((m_Character->isJumping())==false)
			m_Character->jump();
		break;
	}		

	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::keyReleased(const OIS::KeyEvent &keyEventRef)
{


	GameFramework::getSingletonPtr()->keyReleased(keyEventRef);
	switch (keyEventRef.key)		
	{
	case OIS::KC_UP:
	case OIS::KC_W:
		m_Character->setMoveDirection(Game::Move_None);
		break;
	case OIS::KC_A:
		m_Character->setMoveDirection(Game::Move_None);
		break;
	case OIS::KC_D:
		m_Character->setMoveDirection(Game::Move_None);
		break;
	case OIS::KC_S:
		m_Character->setMoveDirection(Game::Move_None);
		break;
	case OIS::KC_SPACE:
		if ((m_Character->getPowerUp() & Game::PowerUp_RocketBootsMode) != 0)
			m_Character->resetRocketBoots();
		break;
	}


	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::mouseMoved(const OIS::MouseEvent &evt)
{
	if (evt.state.Z.rel != 0)
		CameraHandler::getSingletonPtr()->Zoom(evt);
	if ((m_Character->getPowerUp() & Game::PowerUp_MoveBoxMode) != 0)
		m_Character->moveBoxMoved(evt);
	
	CameraHandler::getSingletonPtr()->Rotate(evt);

	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	if(id == OIS::MB_Left)
	{
		if ((m_Character->getPowerUp() & Game::PowerUp_MoveBoxMode) != 0)
		{
			m_Character->moveBoxPressed(evt);
		}
		if ((m_Character->getPowerUp() & Game::PowerUp_GunMode) != 0)
		{
			m_Character->toggleContiniousFire(true);
		}

	} 

	if (id==OIS::MB_Right)
	{
		mvRightButton = true;
	}
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	if(id == OIS::MB_Left)
	{
		m_Character->toggleContiniousFire(false);
		if ((m_Character->getPowerUp() & Game::PowerUp_MoveBoxMode) != 0)
		{
			m_Character->releaseBox();
		}
	}

	if (id==OIS::MB_Right)
	{
		mvRightButton = false;
	}
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::getInput()
{
	if(GameFramework::getSingletonPtr()->mpKeyboard->isKeyDown(OIS::KC_W))
		m_Character->setMoveDirection(Game::Move_Forward);
	if(GameFramework::getSingletonPtr()->mpKeyboard->isKeyDown(OIS::KC_S))
		m_Character->setMoveDirection(Game::Move_Backward);
	if(GameFramework::getSingletonPtr()->mpKeyboard->isKeyDown(OIS::KC_A))
		m_Character->setMoveDirection(Game::Move_StrafeLeft);
	if(GameFramework::getSingletonPtr()->mpKeyboard->isKeyDown(OIS::KC_D))
		m_Character->setMoveDirection(Game::Move_StrafeLeft);
	if(GameFramework::getSingletonPtr()->mpKeyboard->isKeyDown(OIS::KC_SPACE))
	{
		if ((m_Character->getPowerUp() & Game::PowerUp_RocketBootsMode) != 0)
			m_Character->queueRocketBoots();
	}

}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::update(double timeSinceLastFrame)
{
	if(mvQuit == true)
	{
		this->pushAppState(findByName("MenuState"));
		return;
	}
	getInput();
	CameraHandler::getSingletonPtr()->AdjustCamera();
	
	Effects::updateAll(timeSinceLastFrame);

	if (CameraHandler::getSingletonPtr()->getCamMode()==Game::Cam_ThirdPerson)
		m_Character->setNeedUpdateRotation(mvRightButton);
	else
		m_Character->setNeedUpdateRotation(true);

	if (!m_Character->isDead())		
		m_Character->move(timeSinceLastFrame);
	else
		m_Character->updateDead(timeSinceLastFrame);

	Chunks::getSingletonPtr()->loopCurrentChunksUpdate(timeSinceLastFrame);
	NPCHandler::getSingletonPtr()->updateResetNPCs(timeSinceLastFrame);
	NPCHandler::getSingletonPtr()->updateDeadNPCs(timeSinceLastFrame);
	if (m_Character->getCastingBarTimer() && (m_Character->getPowerUp() & Game::PowerUp_RocketBootsMode) == 0)
	{
		m_Character->updateCastingBar(timeSinceLastFrame);
	}
	mvpPhysicsMgr->update(timeSinceLastFrame);
	m_Character->updatePosition();
	if (CameraHandler::getSingletonPtr()->getCamMode() == Game::Cam_ThirdPerson)
		CameraHandler::getSingletonPtr()->MoveCamera();
}