//|||||||||||||||||||||||||||||||||||||||||||||||

#include "GameState.hpp"



//|||||||||||||||||||||||||||||||||||||||||||||||

using namespace Ogre;

//|||||||||||||||||||||||||||||||||||||||||||||||

GameState::GameState()
{
        m_MoveSpeed                     = 0.1;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::enter()
{
        OgreFramework::getSingletonPtr()->m_pLog->logMessage("Entering GameState...");
        
        m_pSceneMgr = OgreFramework::getSingletonPtr()->m_pRoot->createSceneManager(ST_GENERIC, "GameSceneMgr");
        m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.7, 0.7, 0.7));         


        m_pCamera = m_pSceneMgr->createCamera("GameCamera");
        m_pCamera->setNearClipDistance(0.05);

        m_pCamera->setAspectRatio(Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualWidth()) / 
                                                          Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualHeight()));
        
        OgreFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);

        OgreFramework::getSingletonPtr()->m_pKeyboard->setEventCallback(this);
        OgreFramework::getSingletonPtr()->m_pMouse->setEventCallback(this);
        
        OgreFramework::getSingletonPtr()->m_pGUIRenderer->setTargetSceneManager(m_pSceneMgr);

        OgreFramework::getSingletonPtr()->m_pGUISystem->setDefaultMouseCursor((CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"MouseArrow"); 
    CEGUI::MouseCursor::getSingleton().setImage("TaharezLook", "MouseArrow"); 
        const OIS::MouseState state = OgreFramework::getSingletonPtr()->m_pMouse->getMouseState();
        CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition(); 
        CEGUI::System::getSingleton().injectMouseMove(state.X.abs-mousePos.d_x,state.Y.abs-mousePos.d_y);

        m_pMainWnd = CEGUI::WindowManager::getSingleton().getWindow("AOF_GUI_GAME");
        m_pChatWnd = CEGUI::WindowManager::getSingleton().getWindow("ChatWnd");

        OgreFramework::getSingletonPtr()->m_pGUISystem->setGUISheet(m_pMainWnd);

        CEGUI::PushButton* pExitButton = (CEGUI::PushButton*)m_pMainWnd->getChild("ExitButton_Game");
        pExitButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameState::onExitButtonGame, this));
        
        m_bLMouseDown = m_bRMouseDown = false;
        m_bQuit = false;
        m_bChatMode = false;

        setUnbufferedMode();

        createScene();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::pause()
{
        OgreFramework::getSingletonPtr()->m_pLog->logMessage("Pausing GameState...");

        OgreFramework::getSingletonPtr()->m_pGUISystem->setGUISheet(0);
        OgreFramework::getSingletonPtr()->m_pGUIRenderer->setTargetSceneManager(0);

        return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::resume()
{
        OgreFramework::getSingletonPtr()->m_pLog->logMessage("Resuming GameState...");

        OgreFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);
        OgreFramework::getSingletonPtr()->m_pGUIRenderer->setTargetSceneManager(m_pSceneMgr);

        OgreFramework::getSingletonPtr()->m_pGUISystem->setGUISheet(CEGUI::WindowManager::getSingleton().getWindow("AOF_GUI_GAME"));

        m_bQuit = false;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::exit()
{
        OgreFramework::getSingletonPtr()->m_pLog->logMessage("Leaving GameState...");

        OgreFramework::getSingletonPtr()->m_pGUISystem->setGUISheet(0);
        OgreFramework::getSingletonPtr()->m_pGUIRenderer->setTargetSceneManager(0);
        
        m_pSceneMgr->destroyCamera(m_pCamera);
        if(m_pSceneMgr)
                OgreFramework::getSingletonPtr()->m_pRoot->destroySceneManager(m_pSceneMgr);
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
		NxOgre::ResourceSystem::getSingleton()->openArchive("media", "file:media");

		//Creaete SkyDome, set Ambient Light, add Light source and set shadows
        m_pSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);
		m_pSceneMgr->setAmbientLight(ColourValue(0.5f, 0.5f, 0.5f));
		m_pSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);
		Light* l = m_pSceneMgr->createLight("MainLight");
		l->setPosition(20, 80, 50);

		//Create Nodes for character and camera
		SceneNode *node = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("RootNode", Vector3(0,0,0));
		node->createChildSceneNode("CharNode", Vector3(0, 0, 0));
		node = m_pSceneMgr->getSceneNode("RootNode")->createChildSceneNode("CamOrginNode", Vector3(0, CAMERA_HEIGHT, 0));
		node = m_pSceneMgr->getSceneNode("CamOrginNode")->createChildSceneNode("CamHelperNode", Vector3(0, 0, 0));
		node = m_pSceneMgr->getSceneNode("CamHelperNode")->createChildSceneNode("CamNoCollisionNode", Vector3(0, 0, CAMERA_DISTANCE));
		node = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("CamCollisionNode",Vector3(0, 0, CAMERA_DISTANCE));
		
		//Create camera
		m_CameraHandler = new CameraHandler(m_pCamera,node,m_PhysicsRenderSystem, m_pSceneMgr);
		m_Player = new Player ( m_pSceneMgr, m_PhysicsRenderSystem);
    
		//Create rest of the scene. Lots of craps inc, look out!

		NxOgre::Mesh* triangleMesh = NxOgre::MeshManager::getSingleton()->load("media:stairs.xns");
		NxOgre::Mesh* triangleMesh2 = NxOgre::MeshManager::getSingleton()->load("media:spiralstairs.xns");
		NxOgre::Mesh* triangleMesh3 = NxOgre::MeshManager::getSingleton()->load("media:ifk.xns");
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
		Ogre::Entity* triangleEntity = m_pSceneMgr->createEntity("triangleEntity", "stairs.mesh");
		Ogre::SceneNode* triangleNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
		triangleNode->attachObject(triangleEntity);
		triangleNode->setPosition(Vector3(0, 0, 5));
		triangleEntity = m_pSceneMgr->createEntity("triangleEntity2", "spiralstairs.mesh");
		triangleNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
		triangleNode->attachObject(triangleEntity);
		triangleNode->setPosition(Vector3(0, 0, 15));
		triangleEntity = m_pSceneMgr->createEntity("triangleEntity3", "ifk.mesh");
		triangleEntity->setMaterialName("01-Default");
		triangleNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
		triangleNode->attachObject(triangleEntity);
		triangleNode->setPosition(Vector3(0, 0, 25));

		// Create floor plane (Ogre)
		MovablePlane *plane = new MovablePlane("Plane");
		plane->d = 0;
		plane->normal = Vector3::UNIT_Y;
		Ogre::MeshManager::getSingleton().createPlane("PlaneMesh", 
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
			*plane, 120, 120, 1, 1, true, 1, 3, 3, Vector3::UNIT_Z);
		Entity *planeEnt = m_pSceneMgr->createEntity("PlaneEntity", "PlaneMesh");
		planeEnt->setMaterialName("Examples/GrassFloor");

		Ogre::SceneNode* mPlaneNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
		mPlaneNode->attachObject(planeEnt);
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
        if(m_bChatMode == true)
        {
                OgreFramework::getSingletonPtr()->m_pGUISystem->injectKeyDown(keyEventRef.key);
                OgreFramework::getSingletonPtr()->m_pGUISystem->injectChar(keyEventRef.text);
        }
        
        if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_ESCAPE))
        {
                m_bQuit = true;
                return true;
        }

        if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_TAB))
        {
                m_bChatMode = !m_bChatMode;
                
                if(m_bChatMode)
                        setBufferedMode();
                else
                        setUnbufferedMode();
                
                return true;
        }

        if(m_bChatMode && OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_RETURN) ||
                OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_NUMPADENTER))
        {
                CEGUI::Editbox* pChatInputBox = (CEGUI::Editbox*)m_pChatWnd->getChild("ChatInputBox");
                CEGUI::MultiLineEditbox *pChatContentBox = (CEGUI::MultiLineEditbox*)m_pChatWnd->getChild("ChatContentBox");
                pChatContentBox->setText(pChatContentBox->getText() + pChatInputBox->getText() + "\n");
                pChatInputBox->setText("");
                pChatContentBox->setCaratIndex(pChatContentBox->getText().size());
                pChatContentBox->ensureCaratIsVisible();
        }
        
        if(!m_bChatMode || (m_bChatMode && !OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_O)))
                OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);

        return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::keyReleased(const OIS::KeyEvent &keyEventRef)
{
        OgreFramework::getSingletonPtr()->keyReleased(keyEventRef);

        return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::mouseMoved(const OIS::MouseEvent &evt)
{
        OgreFramework::getSingletonPtr()->m_pGUISystem->injectMouseWheelChange(evt.state.Z.rel);
        OgreFramework::getSingletonPtr()->m_pGUISystem->injectMouseMove(evt.state.X.rel, evt.state.Y.rel);
        
        if(m_bRMouseDown)
        {
			m_CameraHandler->Rotate(evt);

        }
        
        return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
        if(id == OIS::MB_Left)
    {
        onLeftPressed(evt);
        m_bLMouseDown = true;
                
                OgreFramework::getSingletonPtr()->m_pGUISystem->injectMouseButtonDown(CEGUI::LeftButton);
    } 
    else if(id == OIS::MB_Right)
    {
        m_bRMouseDown = true;
    } 
        
        return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
        if(id == OIS::MB_Left)
    {
        m_bLMouseDown = false;

                OgreFramework::getSingletonPtr()->m_pGUISystem->injectMouseButtonUp(CEGUI::LeftButton);
    } 
    else if(id == OIS::MB_Right)
    {
        m_bRMouseDown = false;
    }
        
        return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::onLeftPressed(const OIS::MouseEvent &evt)
{

}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::onExitButtonGame(const CEGUI::EventArgs &args)
{
        m_bQuit = true;
        return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::moveCamera()
{
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::getInput()
{
        if(m_bChatMode == false)
        {
                if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_A))
                {
                        m_TranslateVector.x = -m_MoveScale;
                }

                if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_D))
                {
                        m_TranslateVector.x = m_MoveScale;
                }

                if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_W))
                {
                        m_TranslateVector.z = -m_MoveScale;
                }

                if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_S))
                {
                        m_TranslateVector.z = m_MoveScale;
                }
        }
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::update(double timeSinceLastFrame)
{
        if(m_bQuit == true)
        {
                this->popAppState();
                return;
        }

        m_MoveScale = m_MoveSpeed   * timeSinceLastFrame;
                
        m_TranslateVector = Vector3::ZERO;

        getInput();
			m_CameraHandler->AdjustCamera();
		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_W))
		 		m_Player->move(timeSinceLastFrame);		 
		m_PhysicsTimeController->advance(timeSinceLastFrame);
			m_CameraHandler->MoveCamera();

}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::setBufferedMode()
{
        CEGUI::Editbox* pModeCaption = (CEGUI::Editbox*)m_pMainWnd->getChild("ModeCaption");
        pModeCaption->setText("Buffered Input Mode");

        CEGUI::Editbox* pChatInputBox = (CEGUI::Editbox*)m_pChatWnd->getChild("ChatInputBox");
        pChatInputBox->setText("");
        pChatInputBox->activate();
        pChatInputBox->captureInput();

        CEGUI::MultiLineEditbox* pControlsPanel = (CEGUI::MultiLineEditbox*)m_pMainWnd->getChild("ControlsPanel");
        pControlsPanel->setText("[Tab] - To switch between input modes\n\nAll keys to write in the chat box.\n\nPress [Enter] or [Return] to send message.\n\n[Print] - Take screenshot\n\n[Esc] - Quit to main menu");
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::setUnbufferedMode()
{
        CEGUI::Editbox* pModeCaption = (CEGUI::Editbox*)m_pMainWnd->getChild("ModeCaption");
        pModeCaption->setText("Unuffered Input Mode");

        CEGUI::MultiLineEditbox* pControlsPanel = (CEGUI::MultiLineEditbox*)m_pMainWnd->getChild("ControlsPanel");
        pControlsPanel->setText("[Tab] - To switch between input modes\n\n[W] - Forward\n[S] - Backwards\n[A] - Left\n[D] - Right\n\nPress [Shift] to move faster\n\n[O] - Toggle Overlays\n[Print] - Take screenshot\n\n[Esc] - Quit to main menu");
}

//|||||||||||||||||||||||||||||||||||||||||||||||