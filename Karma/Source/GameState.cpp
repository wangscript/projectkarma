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

	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::resume()
{
	GameFramework::getSingletonPtr()->mpLog->logMessage("Resuming GameState...");

	GameFramework::getSingletonPtr()->mpViewport->setCamera(mtpCamera);

	mvQuit = false;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::exit()
{
	GameFramework::getSingletonPtr()->mpLog->logMessage("Leaving GameState...");

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
//|||||||||||||||||||||||||||||||||||||||||||||||
void GameState::createScene()
{
	// Create the NxOgre physics world
	m_PhysicsWorld = NxOgre::World::createWorld();

	// Create NxOgre physics scene
	NxOgre::SceneDescription sceneDesc;
	sceneDesc.mGravity = NxOgre::Vec3(0, -9.8f, 0);
	sceneDesc.mName = "GameScene";
	m_PhysicsScene = m_PhysicsWorld->createScene(sceneDesc);

	// Set some physical scene values
	m_PhysicsScene->getMaterial(0)->setStaticFriction(0.5);
	m_PhysicsScene->getMaterial(0)->setDynamicFriction(0.5);
	m_PhysicsScene->getMaterial(0)->setRestitution(0);

	// Create NxOgre render system
	m_PhysicsRenderSystem = new OGRE3DRenderSystem(m_PhysicsScene);

	//Create NxOgre time controller
	m_PhysicsTimeController = NxOgre::TimeController::getSingleton();

	//Load own resources for NxOgre
	NxOgre::ResourceSystem::getSingleton()->openArchive("nxogre", "file:nxogre");

	//Creaete SkyDome, set Ambient Light, add Light source and set shadows
	mtpSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);
	mtpSceneMgr->setAmbientLight(ColourValue(0.5f, 0.5f, 0.5f));
	mtpSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);
	Light* l = mtpSceneMgr->createLight("MainLight");
	l->setPosition(20, 80, 50);

	//Create Nodes for character and camera
	SceneNode *node = mtpSceneMgr->getRootSceneNode()->createChildSceneNode("RootNode", Vector3(0,0,0));
	node->createChildSceneNode("CharNode", Vector3(0, 0, 0));
	node = mtpSceneMgr->getSceneNode("RootNode")->createChildSceneNode("CamOrginNode", Vector3(0, CAMERA_HEIGHT, 0));
	node = mtpSceneMgr->getSceneNode("CamOrginNode")->createChildSceneNode("CamHelperNode", Vector3(0, 0, 0));
	node = mtpSceneMgr->getSceneNode("CamHelperNode")->createChildSceneNode("CamNoCollisionNode", Vector3(0, 0, CAMERA_DISTANCE));
	node = mtpSceneMgr->getRootSceneNode()->createChildSceneNode("CamCollisionNode",Vector3(0, 0, CAMERA_DISTANCE));

	//Create camera
	m_CameraHandler = new CameraHandler(mtpCamera,node,m_PhysicsRenderSystem, mtpSceneMgr);
	m_Character = new Character ( mtpSceneMgr, m_PhysicsRenderSystem);

	//Create PowerUp objects. Supposed to handled by dotscene later on
	m_PowerUps = new PowerUp(m_PhysicsScene, mtpSceneMgr,m_Character->getCapsule(),m_Character);

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


	//Create rest of the scene. Lots of craps inc, look out!

	NxOgre::Mesh* triangleMesh = NxOgre::MeshManager::getSingleton()->load("nxogre:stairs.xns");
	NxOgre::Mesh* triangleMesh2 = NxOgre::MeshManager::getSingleton()->load("nxogre:spiralstairs.xns");
	NxOgre::Mesh* triangleMesh3 = NxOgre::MeshManager::getSingleton()->load("nxogre:ifk.xns");
	NxOgre::TriangleGeometry* triangleGeometry = new NxOgre::TriangleGeometry(triangleMesh);
	NxOgre::TriangleGeometry* triangleGeometry2 = new NxOgre::TriangleGeometry(triangleMesh2);
	NxOgre::TriangleGeometry* triangleGeometry3 = new NxOgre::TriangleGeometry(triangleMesh3);

	// Add objects
	OGRE3DBody* mCube = m_PhysicsRenderSystem->createBody(new NxOgre::Box(1, 1, 1), NxOgre::Vec3(0, 40, 0), "cube.1m.mesh");
	OGRE3DBody* mCubeTwo = m_PhysicsRenderSystem->createBody(new NxOgre::Box(1, 1, 1), NxOgre::Vec3(20, 45, 0), "cube.1m.mesh");
	mCubeTwo->addForce(NxOgre::Vec3(-800, -200, 0), NxOgre::Enums::ForceMode_Force);

	for (int i=0; i<3; i++)
	{
		for (int j=0; j<3; j++)
		{
			for (int k=0; k<3; k++)
			{
				OGRE3DBody*	cube = m_PhysicsRenderSystem->createBody(new NxOgre::Box(1, 1, 1), NxOgre::Vec3(-15 + (k+0.1), 0.5+i, -15 +(j+0.1)), "cube.1m.mesh");
			}
		}
	}

	// Create floor plane (BloodyMess)
	m_PhysicsScene->createSceneGeometry(new NxOgre::PlaneGeometry(0, NxOgre::Vec3(0, 1, 0)), Matrix44_Identity);
	m_PhysicsScene->createSceneGeometry(triangleGeometry, NxOgre::Matrix44(NxOgre::Vec3(0, 0, 5)));
	m_PhysicsScene->createSceneGeometry(triangleGeometry2, NxOgre::Matrix44(NxOgre::Vec3(0, 0, 15)));
	m_PhysicsScene->createSceneGeometry(triangleGeometry3, NxOgre::Matrix44(NxOgre::Vec3(0, 0, 25)));

	//Static meshes
	Ogre::Entity* triangleEntity = mtpSceneMgr->createEntity("triangleEntity", "stairs.mesh");
	Ogre::SceneNode* triangleNode = mtpSceneMgr->getRootSceneNode()->createChildSceneNode();
	triangleNode->attachObject(triangleEntity);
	triangleNode->setPosition(Vector3(0, 0, 5));
	triangleEntity = mtpSceneMgr->createEntity("triangleEntity2", "spiralstairs.mesh");
	triangleNode = mtpSceneMgr->getRootSceneNode()->createChildSceneNode();
	triangleNode->attachObject(triangleEntity);
	triangleNode->setPosition(Vector3(0, 0, 15));
	triangleEntity = mtpSceneMgr->createEntity("triangleEntity3", "ifk.mesh");
	triangleEntity->setMaterialName("01-Default");
	triangleNode = mtpSceneMgr->getRootSceneNode()->createChildSceneNode();
	triangleNode->attachObject(triangleEntity);
	triangleNode->setPosition(Vector3(0, 0, 25));

	// Create floor plane (Ogre)
	MovablePlane *plane = new MovablePlane("Plane");
	plane->d = 0;
	plane->normal = Vector3::UNIT_Y;
	Ogre::MeshManager::getSingleton().createPlane("PlaneMesh", 
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		*plane, 120, 120, 1, 1, true, 1, 3, 3, Vector3::UNIT_Z);
	Entity *planeEnt = mtpSceneMgr->createEntity("PlaneEntity", "PlaneMesh");
	planeEnt->setMaterialName("Examples/GrassFloor");

	Ogre::SceneNode* mPlaneNode = mtpSceneMgr->getRootSceneNode()->createChildSceneNode();
	mPlaneNode->attachObject(planeEnt);

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
		OGRE3DBody* mCube = m_PhysicsRenderSystem->createBody(new NxOgre::Box(1, 1, 1), NxOgre::Vec3(mtpSceneMgr->getSceneNode("CharNode")->_getDerivedPosition().x, 
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
		this->popAppState();
		return;
	}
	getInput();
	m_CameraHandler->AdjustCamera();
	m_Character->move(timeSinceLastFrame);		 
	if (m_Character->getCastingBarTimer())
	{
		m_Character->updateCastingBar(timeSinceLastFrame);
	}
	m_PhysicsTimeController->advance(timeSinceLastFrame);
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