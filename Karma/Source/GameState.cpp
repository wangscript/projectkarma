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
	GameFramework::getSingletonPtr()->mpLog->logMessage("Entering GameState...");
	mtpSceneMgr = GameFramework::getSingletonPtr()->mpRoot->createSceneManager(ST_GENERIC, "GameSceneMgr");
	mtpSceneMgr->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));         

	mtpCamera = mtpSceneMgr->createCamera("GameCamera");
	mtpCamera->setNearClipDistance(0.05f);

	mtpCamera->setAspectRatio(Real(GameFramework::getSingletonPtr()->mpViewport->getActualWidth()) / 
		Real(GameFramework::getSingletonPtr()->mpViewport->getActualHeight()));

	GameFramework::getSingletonPtr()->mpViewport->setCamera(mtpCamera);

	mtpCamera->setAspectRatio(Real(GameFramework::getSingletonPtr()->mpViewport->getActualWidth()) / 
		Real(GameFramework::getSingletonPtr()->mpViewport->getActualHeight()));

	GameFramework::getSingletonPtr()->mpViewport->setCamera(mtpCamera);

	GameFramework::getSingletonPtr()->mpKeyboard->setEventCallback(this);
	GameFramework::getSingletonPtr()->mpMouse->setEventCallback(this);

	const OIS::MouseState state = GameFramework::getSingletonPtr()->mpMouse->getMouseState();


	GameFramework::getSingletonPtr()->mpGui->showMiniMap();
	
	mvLMouseDown = mvRMouseDown = false;
	mvQuit = false;
	mvChatMode = false;
	
	setUnbufferedMode();
	createScene();

	
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::pause()
{
	GameFramework::getSingletonPtr()->mpLog->logMessage("Pausing GameState...");
	mvpPhysicsMgr->pause();
	GameFramework::getSingletonPtr()->mpGui->hideInGameUI();
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::resume()
{
	GameFramework::getSingletonPtr()->mpLog->logMessage("Resuming GameState...");

	GameFramework::getSingletonPtr()->mpViewport->setCamera(mtpCamera);
	mvpPhysicsMgr->resume();
	GameFramework::getSingletonPtr()->mpGui->showInGameUI();
	mvQuit = false;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::exit()
{
	GameFramework::getSingletonPtr()->mpLog->logMessage("Leaving GameState...");
	
	mvpPhysicsMgr->destroyPhysicsWorld();
	delete mvpWorld;
	Chunks::getSingletonPtr()->destroyAll();
	mtpSceneMgr->destroyCamera(mtpCamera);
	if(mtpSceneMgr)
		GameFramework::getSingletonPtr()->mpRoot->destroySceneManager(mtpSceneMgr);
}
void GameState::createMotionBlurEffects()
{
	Ogre::CompositorPtr comp3 = Ogre::CompositorManager::getSingleton().create(
		"Motion Blur", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME
		);
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

	//Shadow Technique. Stencil shadows.  @todo texture shadows
	mtpSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_MODULATIVE);
	//Fog @todo
	mtpSceneMgr->setFog(FOG_LINEAR, ColourValue(0.7f, 0.7f, 0.8f), 0, 10000, 25000);
	
	//Directional light
	Ogre::Vector3 lightdir(0.5f, -0.9f, 0.5f);
	lightdir.normalise();
	mtpSceneMgr->setShadowFarDistance(15);
	Ogre::Light* l = mtpSceneMgr->createLight("dirLight");
	l->setType(Light::LT_DIRECTIONAL);
	l->setDirection(lightdir);
	l->setDiffuseColour(ColourValue::White);
	l->setSpecularColour(ColourValue(0.2f, 0.2f, 0.2f));
	
	//Ambient light and skybox
	mtpSceneMgr->setAmbientLight(ColourValue(0.2f, 0.2f, 0.2f));
	mtpSceneMgr->setSkyBox(true, "Examples/CloudyNoonSkyBox");	

		
	//For screenshotting minimap
	mtpSceneMgr->getRootSceneNode()->createChildSceneNode("CamFree");
	
	//Create Character
	m_Character = new Player( mtpSceneMgr, mvpPhysicsMgr->getPhysicsScene(),mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlender.mesh" , "", Ogre::Vector3(0,0,0),100,1);
	NPC::setPlayerNode(m_Character->getPlayerNode());
	Aimer::setPhysicsScene(mvpPhysicsMgr->getPhysicsScene());
	Chunks::getSingletonPtr()->createGrid();

	//Create camera
	m_CameraHandler = new CameraHandler(mtpCamera,mvpPhysicsMgr->getPhysicsRenderSystem(), mtpSceneMgr, m_Character);

	m_CameraHandler->setPlayerPtr(m_Character);

	//Create PowerUp class. 
	m_PowerUps = new PowerUp(mvpPhysicsMgr->getPhysicsScene(), mtpSceneMgr,m_Character->getHitBox(),m_Character);


	//------------- WILL BE REMOVED ONCE DOTSCENE IS WORKING ---------------
	
	Ogre::Vector3 pwrUpPosition(-5.0f,0.2f,-5.0f);
	Ogre::SceneNode* pwrUpEntNode = mtpSceneMgr->getRootSceneNode()->createChildSceneNode();
	Ogre::Entity* pwrUpEnt = mtpSceneMgr->createEntity("pwrup1", "pwrup1.mesh");
	pwrUpEntNode->attachObject(pwrUpEnt);
	pwrUpEntNode->setPosition(pwrUpPosition);
	m_PowerUps->addPowerUp(pwrUpPosition,pwrUpEnt->getName());

	Ogre::Vector3 pwrUpPosition2 (-5.0f,0.2f,1.0f);
	pwrUpEntNode = mtpSceneMgr->getRootSceneNode()->createChildSceneNode();
	pwrUpEnt = mtpSceneMgr->createEntity("pwrup2", "pwrup2.mesh");
	pwrUpEnt->setCastShadows(true);
	pwrUpEntNode->attachObject(pwrUpEnt);
	pwrUpEntNode->setPosition(pwrUpPosition2);
	m_PowerUps->addPowerUp(pwrUpPosition2,pwrUpEnt->getName());
	pwrUpEnt->setCastShadows(true);

	Ogre::Vector3 pwrUpPosition3(-5.0f,0.2,5.0f);
	pwrUpEntNode = mtpSceneMgr->getRootSceneNode()->createChildSceneNode();
	pwrUpEnt = mtpSceneMgr->createEntity("pwrup3", "pwrup3.mesh");
	pwrUpEntNode->attachObject(pwrUpEnt);
	pwrUpEntNode->setPosition(pwrUpPosition3);
	m_PowerUps->addPowerUp(pwrUpPosition3,pwrUpEnt->getName());

	for (int i=0; i<3; i++)
	{
		for (int j=0; j<3; j++)
		{
			for (int k=0; k<3; k++)
			{
				OGRE3DBody*	cube = mvpPhysicsMgr->getPhysicsRenderSystem()->createBody(new NxOgre::Box(1, 1, 1), NxOgre::Vec3(-15 + (k+0.1), 0.5+i, -15 +(j+0.1)), "cube.1m.mesh");
			}
		}
	}

	mvpPhysicsMgr->addStaticTriangleMesh(Ogre::Vector3(0, 0, 5), "stairs");
	mvpPhysicsMgr->addStaticTriangleMesh(Ogre::Vector3(0, 0, 15), "spiralstairs");
	mvpPhysicsMgr->addStaticTriangleMesh(Ogre::Vector3(0, 0, 25), "ifk");
	std::vector<Ogre::Vector3> v;
	v.push_back(Ogre::Vector3(0,3,0));
	v.push_back(Ogre::Vector3(10,0,0));
	v.push_back(Ogre::Vector3(10,0,10));
	mvpPhysicsMgr->addKinematicTriangleMesh( v, 0.5 , "stairs");
	mvpPhysicsMgr->addKinematicCircle(0.1,Ogre::Vector3(5.0f, 1.0f, -15.0f),4, "stairs");

	//------------- END ---------------

	mvpWorld = new WorldManager(mtpSceneMgr, mtpCamera,mvpPhysicsMgr->getPhysicsScene(),l);

	//Behöver bara laddas en gång. Om man har en .dat fil är detta onödigt @ todo fixa kanske
	Ogre::String difSpecMap = "dirt_grayrocky_diffusespecular.dds";
	Ogre::String normHeightMap = "dirt_grayrocky_normalheight.dds";
	mvpWorld->addTextureLayer(difSpecMap, normHeightMap , 5);
	difSpecMap = "grass_green-01_diffusespecular.dds";
	normHeightMap = "grass_green-01_normalheight.dds";
	mvpWorld->addTextureLayer(difSpecMap,normHeightMap , 5);
	difSpecMap = "growth_weirdfungus-03_diffusespecular.dds";
	normHeightMap = "growth_weirdfungus-03_normalheight.dds";
	mvpWorld->addTextureLayer(difSpecMap, normHeightMap, 5);

	mvpWorld->initTerrain();
	mvpWorld->buildNxOgreTerrain();

	createMotionBlurEffects();
	Ogre::CompositorManager::getSingleton().addCompositor(GameFramework::getSingletonPtr()->mpViewport, "Motion Blur");

	new NPCAimer ( mtpSceneMgr,  mvpPhysicsMgr->getPhysicsScene(),mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlender.mesh" , "Enemy1", Ogre::Vector3(15,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(15,100,0),0),0.2,100,3);
	new NPCAimer ( mtpSceneMgr, mvpPhysicsMgr->getPhysicsScene(),mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlender.mesh" , "Enemy2", Ogre::Vector3(150,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(150,100,0),0),0.2,100,3);
	new NPCAimer ( mtpSceneMgr, mvpPhysicsMgr->getPhysicsScene(),mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlender.mesh" , "Enemy3", Ogre::Vector3(150,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(150,100,150),150),0.2,100,3);
	new NPCAimer ( mtpSceneMgr, mvpPhysicsMgr->getPhysicsScene(),mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlender.mesh" , "Enemy4", Ogre::Vector3(270,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(270,100,0),0),0.2,100,3);
	new NPCAimer ( mtpSceneMgr, mvpPhysicsMgr->getPhysicsScene(),mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlender.mesh" , "Enemy5", Ogre::Vector3(-150,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(-150,100,0),0),0.2,100,3);
	new NPCAimer ( mtpSceneMgr,mvpPhysicsMgr->getPhysicsScene(), mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlender.mesh" , "Enemy6", Ogre::Vector3(15,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(15,100,-150),-150),0.2,100,3);

	SceneLoader* sceneLoad = new SceneLoader(); 
	sceneLoad->parseLevel("scen.scene","Karma",mtpSceneMgr,mtpSceneMgr->getRootSceneNode(),mvpPhysicsMgr,mvpWorld->getTerrainAt(0,0));
}
//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
	if(mvChatMode == true)
	{

	}


	if(GameFramework::getSingletonPtr()->mpKeyboard->isKeyDown(OIS::KC_ESCAPE))
	{
		mvQuit = true;
		return true;
	}

	switch (keyEventRef.key)		
	{
	case OIS::KC_1:
		m_Character->setPowerUp(Game::PowerUp_None);
		GameFramework::getSingletonPtr()->mpGui->changeIngameUIIcon(Game::PowerUp_None);
		m_CameraHandler->setCamMode(Game::Cam_ThirdPerson);
		break;
	case OIS::KC_2:
		m_Character->setPowerUp(Game::PowerUp_SuperSpeed,3.0f);
		GameFramework::getSingletonPtr()->mpGui->changeIngameUIIcon(Game::PowerUp_SuperSpeed);
		break;
	case OIS::KC_3:
		m_Character->setPowerUp(Game::PowerUp_SuperJump);
		GameFramework::getSingletonPtr()->mpGui->changeIngameUIIcon(Game::PowerUp_SuperJump);
		break;
	case OIS::KC_4:
		m_Character->setPowerUp(Game::PowerUp_MoveBox);
		GameFramework::getSingletonPtr()->mpGui->changeIngameUIIcon(Game::PowerUp_MoveBox);
		m_CameraHandler->setCamMode(Game::Cam_MixCursorMovable);
		break;
	case OIS::KC_5:
		m_Character->setPowerUp(Game::PowerUp_GunMode);
		GameFramework::getSingletonPtr()->mpGui->hideInGameUI();
		m_CameraHandler->setCamMode(Game::Cam_MixCursorMovable);
		break;
	case OIS::KC_6:
		m_Character->setPowerUp(Game::PowerUp_GunMode);
		GameFramework::getSingletonPtr()->mpGui->hideInGameUI();
		m_CameraHandler->setCamMode(Game::Cam_MixCursorCenter);
		break;
	case OIS::KC_7:
		m_Character->setPowerUp(Game::PowerUp_GunMode);
		GameFramework::getSingletonPtr()->mpGui->hideInGameUI();
		m_CameraHandler->setCamMode(Game::Cam_FirstPerson);
		break;
	case OIS::KC_8:
		m_Character->setPowerUp(Game::PowerUp_RocketBoots);
		break;
	case OIS::KC_SPACE:
		if ((m_Character->getPowerUp() & Game::PowerUp_RocketBoots) != 0)
		{
			m_Character->startRocketBoots();
			GameFramework::getSingletonPtr()->mpSound->playSound(GameFramework::getSingletonPtr()->mpSound->mpRocketBoots,m_Character->getPlayerNode()->_getDerivedPosition(),true);
		}
		break;
	case OIS::KC_Q:
		m_Character->debugMode();
		break;
	case OIS::KC_C:
		mtpCamera->setPolygonMode(Ogre::PM_WIREFRAME);
		break;
	case OIS::KC_V:
		mtpCamera->setPolygonMode(Ogre::PM_SOLID);
		break;
	case OIS::KC_P:
		m_CameraHandler->setCamMode(Game::Cam_FreeMode);
		break;
	case OIS::KC_F:
		OGRE3DBody* mCube = mvpPhysicsMgr->getPhysicsRenderSystem()->createBody(new NxOgre::Box(1, 1, 1), NxOgre::Vec3(mtpSceneMgr->getSceneNode("CharNode")->_getDerivedPosition().x, 
			mtpSceneMgr->getSceneNode("CharNode")->_getDerivedPosition().y+2, mtpSceneMgr->getSceneNode("CharNode")->_getDerivedPosition().z), "cube.1m.mesh"); 
		Ogre::Vector3 dirCamToChar =  mtpSceneMgr->getSceneNode("CharNode")->_getDerivedPosition() - mtpSceneMgr->getSceneNode("CamCollisionNode")->_getDerivedPosition();
		mCube->addForce(NxOgre::Vec3(dirCamToChar.x*1000,-400,dirCamToChar.z*1000), NxOgre::Enums::ForceMode_Force, true);
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
		if ((m_Character->getPowerUp() & Game::PowerUp_RocketBoots) != 0)
		{
			m_Character->resetRocketBoots();
			GameFramework::getSingletonPtr()->mpSound->stopPlaySound(GameFramework::getSingletonPtr()->mpSound->mpRocketBoots);
		}
		break;
	}


	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::mouseMoved(const OIS::MouseEvent &evt)
{
	if (evt.state.Z.rel != 0)
	{
		m_CameraHandler->Zoom(evt);
	}
	if ((m_Character->getPowerUp() & Game::PowerUp_MoveBox) != 0)
	{
		m_Character->moveBoxMoved(evt);
	}
	
	m_CameraHandler->Rotate(evt);
	if(mvRMouseDown)
	{
		

	}

	//GameFramework::getSingletonPtr()->mpGui->updateCursorPos(evt.state.X.abs,evt.state.Y.abs );
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	if(id == OIS::MB_Left)
	{
		onLeftPressed(evt);
		mvLMouseDown = true;
		if ((m_Character->getPowerUp() & Game::PowerUp_MoveBox) != 0)
		{
			m_Character->moveBoxPressed(evt);
		}
		if ((m_Character->getPowerUp() & Game::PowerUp_GunMode) != 0)
		{
			if (m_CameraHandler->getCamMode() == Game::Cam_MixCursorCenter || m_CameraHandler->getCamMode() == Game::Cam_FirstPerson)
				m_Character->queueFire(0.5,0.5);
			else
			{
				
				double x = double(evt.state.X.abs)/GameFramework::getSingletonPtr()->mpRenderWnd->getWidth();
				double y = double(evt.state.Y.abs)/GameFramework::getSingletonPtr()->mpRenderWnd->getHeight();
				std::cout << "Skickar: " <<  x<< y;
				m_Character->queueFire(x,y);
			}
			//if (m_CameraHandler->getCamMode() == Game::Cam_MixCursorMovable)
			//	int x;
				//m_Character->toggleMuzzleFlash(evt,false);
			//else
				//m_Character->toggleMuzzleFlash(evt,true);
		}

	} 
	else if(id == OIS::MB_Right)
	{
		mvRMouseDown = true;
	} 

	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	if(id == OIS::MB_Left)
	{
		mvLMouseDown = false;
		if ((m_Character->getPowerUp() & Game::PowerUp_MoveBox) != 0)
		{
			m_Character->releaseBox();
		}
	} 
	else if(id == OIS::MB_Right)
	{
		mvRMouseDown = false;
	}

	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::onLeftPressed(const OIS::MouseEvent &evt)
{

}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::getInput()
{
	if(GameFramework::getSingletonPtr()->mpKeyboard->isKeyDown(OIS::KC_W))
	{
		m_Character->setMoveDirection(Game::Move_Forward);
	}
	if(GameFramework::getSingletonPtr()->mpKeyboard->isKeyDown(OIS::KC_S))
	{
		m_Character->setMoveDirection(Game::Move_Backward);
	}
	if(GameFramework::getSingletonPtr()->mpKeyboard->isKeyDown(OIS::KC_A))
	{
		m_Character->setMoveDirection(Game::Move_StrafeLeft);
	}
	if(GameFramework::getSingletonPtr()->mpKeyboard->isKeyDown(OIS::KC_D))
	{
		m_Character->setMoveDirection(Game::Move_StrafeLeft);
	}
	if(GameFramework::getSingletonPtr()->mpKeyboard->isKeyDown(OIS::KC_SPACE))
	{
		m_Character->rocketBoots();
	}

	if(GameFramework::getSingletonPtr()->mpKeyboard->isKeyDown(OIS::KC_SPACE))
	{
		if ((m_Character->getPowerUp() & Game::PowerUp_RocketBoots) != 0)
		{
			m_Character->rocketBoots();
		}
		else
		{
			if ((m_Character->isJumping())==false)
				m_Character->jump();
		}
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
	m_CameraHandler->AdjustCamera();
	m_Character->move(timeSinceLastFrame);
	Chunks::getSingletonPtr()->loopCurrentChunksUpdate(timeSinceLastFrame);
	//Chunks::getSingletonPtr()->updateTempNpcs(timeSinceLastFrame);
	NPCHandler::getSingletonPtr()->updateResetNPCs(timeSinceLastFrame);
	NPCHandler::getSingletonPtr()->updateDeadNPCs(timeSinceLastFrame);
	if (m_Character->getCastingBarTimer())
	{
		m_Character->updateCastingBar(timeSinceLastFrame);
	}
	mvpPhysicsMgr->update(timeSinceLastFrame);
	m_Character->updatePosition();
	//Chunks::getSingletonPtr()->loopCurrentChunksUpdatePosition();
	if (m_CameraHandler->getCamMode() == Game::Cam_ThirdPerson)
		m_CameraHandler->MoveCamera();
	Effects::updateAll(timeSinceLastFrame);
	//else
		//m_Character->updateMuzzleFlash(timeSinceLastFrame);
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::setBufferedMode()
{
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::setUnbufferedMode()
{

}

//|||||||||||||||||||||||||||||||||||||||||||||||