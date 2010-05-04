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
	mtpCamera->setNearClipDistance(0.05);

	mtpCamera->setAspectRatio(Real(GameFramework::getSingletonPtr()->mpViewport->getActualWidth()) / 
		Real(GameFramework::getSingletonPtr()->mpViewport->getActualHeight()));

	GameFramework::getSingletonPtr()->mpViewport->setCamera(mtpCamera);

	mtpCamera->setAspectRatio(Real(GameFramework::getSingletonPtr()->mpViewport->getActualWidth()) / 
		Real(GameFramework::getSingletonPtr()->mpViewport->getActualHeight()));

	GameFramework::getSingletonPtr()->mpViewport->setCamera(mtpCamera);

	GameFramework::getSingletonPtr()->mpKeyboard->setEventCallback(this);
	GameFramework::getSingletonPtr()->mpMouse->setEventCallback(this);

	const OIS::MouseState state = GameFramework::getSingletonPtr()->mpMouse->getMouseState();

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
	mtpSceneMgr->setFog(FOG_LINEAR, ColourValue(0.7, 0.7, 0.8), 0, 10000, 25000);

	//Directional light
	Ogre::Vector3 lightdir(0.55, -0.3, 0.75);
	lightdir.normalise();
	mtpSceneMgr->setShadowFarDistance(15);
	Ogre::Light* l = mtpSceneMgr->createLight("dirLight");
	l->setType(Light::LT_DIRECTIONAL);
	l->setDirection(lightdir);
	l->setDiffuseColour(ColourValue::White);
	l->setSpecularColour(ColourValue(0.4, 0.4, 0.4));
	
	//Ambient light and skybox
	mtpSceneMgr->setAmbientLight(ColourValue(0.2, 0.2, 0.2));
	mtpSceneMgr->setSkyBox(true, "Examples/CloudyNoonSkyBox");	

	//Create Nodes for character and camera
	SceneNode *node = mtpSceneMgr->getRootSceneNode()->createChildSceneNode("RootNode", Vector3(0,0,0));
	node->createChildSceneNode("CharNode", Vector3(0, 0, 0));
	node = mtpSceneMgr->getSceneNode("RootNode")->createChildSceneNode("CamOrginNode", Vector3(0, CAMERA_HEIGHT, 0));
	node = mtpSceneMgr->getSceneNode("CamOrginNode")->createChildSceneNode("CamHelperNode", Vector3(0, 0, 0));
	node = mtpSceneMgr->getSceneNode("CamHelperNode")->createChildSceneNode("CamNoCollisionNode", Vector3(0, 0, CAMERA_DISTANCE));
	node = mtpSceneMgr->getRootSceneNode()->createChildSceneNode("CamCollisionNode",Vector3(0, 0, CAMERA_DISTANCE));


	//Create camera
	m_CameraHandler = new CameraHandler(mtpCamera,node,mvpPhysicsMgr->getPhysicsRenderSystem(), mtpSceneMgr);
	//Create Character
	m_Character = new Character ( mtpSceneMgr, mvpPhysicsMgr->getPhysicsRenderSystem());

	//Create PowerUp class. 
	m_PowerUps = new PowerUp(mvpPhysicsMgr->getPhysicsScene(), mtpSceneMgr,m_Character->getCapsule(),m_Character);


	//------------- WILL BE REMOVED ONCE DOTSCENE IS WORKING ---------------
	
	Ogre::Vector3 pwrUpPosition(-5,0.2,-5);
	Ogre::SceneNode* pwrUpEntNode = mtpSceneMgr->getRootSceneNode()->createChildSceneNode();
	Ogre::Entity* pwrUpEnt = mtpSceneMgr->createEntity("pwrup1", "pwrup1.mesh");
	pwrUpEntNode->attachObject(pwrUpEnt);
	pwrUpEntNode->setPosition(pwrUpPosition);
	m_PowerUps->addPowerUp(pwrUpPosition,pwrUpEnt->getName());

	Ogre::Vector3 pwrUpPosition2 (-5,0.2,1);
	pwrUpEntNode = mtpSceneMgr->getRootSceneNode()->createChildSceneNode();
	pwrUpEnt = mtpSceneMgr->createEntity("pwrup2", "pwrup2.mesh");
	pwrUpEntNode->attachObject(pwrUpEnt);
	pwrUpEntNode->setPosition(pwrUpPosition2);
	m_PowerUps->addPowerUp(pwrUpPosition2,pwrUpEnt->getName());

	Ogre::Vector3 pwrUpPosition3(-5,0.2,5);
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
	mvpPhysicsMgr->addKinematicCircle(0.1,Ogre::Vector3(5, 1, -15),4, "stairs");

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
		m_Character->setPowerUp(Character::PowerUp_None);
		GameFramework::getSingletonPtr()->mpGui->changeIngameUIIcon(Character::PowerUp_None);
		break;
	case OIS::KC_2:
		m_Character->setPowerUp(Character::PowerUp_SuperSpeed,3.0f);
		GameFramework::getSingletonPtr()->mpGui->changeIngameUIIcon(Character::PowerUp_SuperSpeed);
		break;
	case OIS::KC_3:
		m_Character->setPowerUp(Character::PowerUp_SuperJump);
		GameFramework::getSingletonPtr()->mpGui->changeIngameUIIcon(Character::PowerUp_SuperJump);
		break;
	case OIS::KC_4:
		m_Character->setPowerUp(Character::PowerUp_MoveBox);
		GameFramework::getSingletonPtr()->mpGui->changeIngameUIIcon(Character::PowerUp_MoveBox);
		break;
	case OIS::KC_Q:
		m_Character->debugMode();
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
		m_Character->setMoveDirection(Character::Move_None);
		break;
	case OIS::KC_A:
		m_Character->setMoveDirection(Character::Move_None);
		break;
	case OIS::KC_D:
		m_Character->setMoveDirection(Character::Move_None);
		break;
	case OIS::KC_S:
		m_Character->setMoveDirection(Character::Move_None);
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
	if (m_Character->getPowerUp()==Character::PowerUp_MoveBox)
	{
		m_Character->moveBoxMoved(evt);
	}

	if(mvRMouseDown)
	{
		m_CameraHandler->Rotate(evt);

	}

	GameFramework::getSingletonPtr()->mpGui->updateCursorPos(evt.state.X.abs,evt.state.Y.abs );
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	if(id == OIS::MB_Left)
	{
		onLeftPressed(evt);
		mvLMouseDown = true;
		if (m_Character->getPowerUp()==Character::PowerUp_MoveBox)
		{
			m_Character->moveBoxPressed(evt);
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
		if (m_Character->getPowerUp()==Character::PowerUp_MoveBox)
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
		m_Character->setMoveDirection(Character::Move_Forward);
	}
	if(GameFramework::getSingletonPtr()->mpKeyboard->isKeyDown(OIS::KC_S))
	{
		m_Character->setMoveDirection(Character::Move_Backward);
	}
	if(GameFramework::getSingletonPtr()->mpKeyboard->isKeyDown(OIS::KC_A))
	{
		m_Character->setMoveDirection(Character::Move_StrafeLeft);
	}
	if(GameFramework::getSingletonPtr()->mpKeyboard->isKeyDown(OIS::KC_D))
	{
		m_Character->setMoveDirection(Character::Move_StrafeLeft);
	}
	if(GameFramework::getSingletonPtr()->mpKeyboard->isKeyDown(OIS::KC_SPACE))
	{
		if ((m_Character->isJumping())==false)
			m_Character->jump();
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
	if (m_Character->getCastingBarTimer())
	{
		m_Character->updateCastingBar(timeSinceLastFrame);
	}
	mvpPhysicsMgr->update(timeSinceLastFrame);
	m_Character->updatePosition();
	m_CameraHandler->MoveCamera();

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