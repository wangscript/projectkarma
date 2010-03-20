//*****************************

#include "TestState.h"
#include "Ogre.h"

//*****************************

using namespace Ogre;

//*****************************

TestState::TestState()
{
	m_MoveSpeed			= 0.1;
	m_RotateSpeed		= 0.3;

	m_pCurrentObject	= 0;
}

//******************************

void TestState::enter()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Entering TestState...");

	m_pSceneMgr = OgreFramework::getSingletonPtr()->m_pRoot->createSceneManager(ST_GENERIC,"TestSceneMgr");
	m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.7,0.7,0.7));
	
	m_pCamera = m_pSceneMgr->createCamera("TestCamera");
	m_pCamera->setPosition(Vector3(5,60,60));
	m_pCamera->lookAt(Vector3(5,20,0));
	m_pCamera->setNearClipDistance(5);
	
	m_pCamera->setAspectRatio(Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualWidth() /
		Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualHeight())));
	
	OgreFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);

	OgreFramework::getSingletonPtr()->m_pKeyboard->setEventCallback(this);
	OgreFramework::getSingletonPtr()->m_pMouse->setEventCallback(this);

	//OgreFramework::getSingletonPtr()->m_pGUIRenderer->setTargetSceneManager(m_pSceneMgr);

	//OgreFramework::getSingletonPtr()->m_pGUISystem->setDefaultMouseCursor((CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"MouseArrow"); 
	//CEGUI::MouseCursor::getSingleton().setImage("TaharezLook", "MouseArrow"); 
	//const OIS::MouseState state = OgreFramework::getSingletonPtr()->m_pMouse->getMouseState();
	//CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition(); 
	//CEGUI::System::getSingleton().injectMouseMove(state.X.abs-mousePos.d_x,state.Y.abs-mousePos.d_y);

	//m_pMainWnd = CEGUI::WindowManager::getSingleton().getWindow("AOF_GUI_GAME");

	//OgreFramework::getSingletonPtr()->m_pGUISystem->setGUISheet(m_pMainWnd);

	//CEGUI::PushButton* pExitButton = (CEGUI::PushButton*)m_pMainWnd->getChild("ExitButton_Game");
	//pExitButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TestState::onExitButtonGame, this));

	m_bLMouseDown = m_bRMouseDown = false;
	m_bQuit = false;

	createScene();

}

//*********************************************

bool TestState::pause()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Pausing TestState...");
	
	//We are not using any gui in this teststate
	//OgreFramework::getSingletonPtr()->m_pGUISystem.setGUISheet(0);
	//OgreFramework::getSingletonPtr()->m_pGUIRenderer->setTargetSceneManager(0);

	return true;

}

//********************************************

void TestState::resume()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Resuming TestState...");

	OgreFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);
	//OgreFramework::getSingletonPtr()->m_pGUIRenderer->setTargetSceneManager(m_pSceneMgr);

	//OgreFramework::getSingletonPtr()->m_pGUISystem->setGUISheet(CEGUI::WindowManager::getSingleton().getWindow("AOF_GUI_GAME"));

	m_bQuit = false;

}

//********************************************

void TestState::exit()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Leaving TestState...");

	//OgreFramework::getSingletonPtr()->m_pGUISystem->setGUISheet(0);
	//OgreFramework::getSingletonPtr()->m_pGUIRenderer->setTargetSceneManager(0);

	m_pSceneMgr->destroyCamera(m_pCamera);
	if (m_pSceneMgr)
		OgreFramework::getSingletonPtr()->m_pRoot->destroySceneManager(m_pSceneMgr);
	
}

//*******************************************

void TestState::createScene()
{

	m_pSceneMgr->setSkyBox(true,"Examples/SpaceSkyBox");

	m_pSceneMgr->createLight("Light")->setPosition(75,75,75);

	Plane plane(Vector3::UNIT_Y, 0);

	Ogre::MeshManager::getSingleton().createPlane("ground",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
		1500,1500,20,20,true,1,5,5,Vector3::UNIT_Z);

	m_pGroundEntity = m_pSceneMgr->createEntity("GroundEntity", "ground");
	m_pGroundNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("GroundNode");
	m_pGroundNode->attachObject(m_pGroundEntity);

	m_pGroundEntity->setMaterialName("Examples/Rockwall");
	m_pGroundEntity->setCastShadows(false);

	m_pSphereEntity = m_pSceneMgr->createEntity("OgreEntity","ogrehead.mesh");
	m_pSphereNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("OgreNode");
	m_pSphereNode->attachObject(m_pSphereEntity);
	m_pSphereNode->setPosition(0, 100,0);
	m_pSphereEntity->setCastShadows(true);

}

//************************************************

bool TestState::keyPressed(const OIS::KeyEvent &keyEventRef)
{

	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_ESCAPE))
	{
		m_bQuit = true;
		return true;
	}

	return true;
}

//*************************************************

bool TestState::keyReleased(const OIS::KeyEvent &keyEventRef)
{
	OgreFramework::getSingletonPtr()->keyReleased(keyEventRef);

	return true;
}

//***************************************************

bool TestState::mouseMoved(const OIS::MouseEvent &evt)
{
	if(m_bRMouseDown)
	{
		m_pCamera->yaw(Degree(evt.state.X.rel * -0.1));
		m_pCamera->pitch(Degree(evt.state.Y.rel * -0.1));
	}

	return true;
}

//***********************************************

bool TestState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	if(id == OIS::MB_Left)
	{
		onLeftPressed(evt);
		m_bLMouseDown = true;

	} 
	else if(id == OIS::MB_Right)
	{
		m_bRMouseDown = true;
	} 

	return true;
}

//************************************************

bool TestState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
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

//***********************************************

void TestState::onLeftPressed(const OIS::MouseEvent &evt)
{
	//Implement stuff to do
}

//***********************************************

bool TestState::onExitButtonGame(const CEGUI::EventArgs &args)
{
	m_bQuit = true;
	return true;
}

//***********************************************

void TestState::moveCamera()
{
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_LSHIFT)) 
	m_pCamera->moveRelative(m_TranslateVector);
	m_pCamera->moveRelative(m_TranslateVector / 10);
}

//***********************************************

void TestState::getInput()
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
	
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_LEFT))
	{
		m_pCamera->yaw(m_RotScale);
	}
	
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_RIGHT))
	{
		m_pCamera->yaw(-m_RotScale);
	}
	
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_UP))
	{
		m_pCamera->pitch(m_RotScale);
	}

	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_DOWN))
	{
		m_pCamera->pitch(-m_RotScale);
	}
}

//************************************************

void TestState::update(double timeSinceLastFrame)
{
	if(m_bQuit == true)
	{
		this->popAppState();
		return;
	}

	m_MoveScale = m_MoveSpeed   * timeSinceLastFrame;
	m_RotScale  = m_RotateSpeed * timeSinceLastFrame;

	m_TranslateVector = Vector3::ZERO;

	getInput();
	moveCamera();
}