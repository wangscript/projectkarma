//|||||||||||||||||||||||||||||||||||||||||||||||

#include "MenuState.h"

//|||||||||||||||||||||||||||||||||||||||||||||||

using namespace Ogre;

//|||||||||||||||||||||||||||||||||||||||||||||||

void MenuState::enter()
{
	GameFramework::getSingletonPtr()->mpLog->logMessage("Entering MenuState...");
	
	mtpSceneMgr = GameFramework::getSingletonPtr()->mpRoot->createSceneManager(ST_GENERIC, "MenuSceneMgr");
	mtpSceneMgr->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f)); 

	mtpCamera = mtpSceneMgr->createCamera("MenuCam");
	mtpCamera->setPosition(Vector3(0, 25, -50));
	mtpCamera->lookAt(Vector3(0, 0, 0));
	mtpCamera->setNearClipDistance(1);

	mtpCamera->setAspectRatio(Real(GameFramework::getSingletonPtr()->mpViewport->getActualWidth()) / 
							  Real(GameFramework::getSingletonPtr()->mpViewport->getActualHeight()));
	
	GameFramework::getSingletonPtr()->mpViewport->setCamera(mtpCamera);

	GameFramework::getSingletonPtr()->mpKeyboard->setEventCallback(this);
	GameFramework::getSingletonPtr()->mpMouse->setEventCallback(this);

	mvQuit = false;
	
	createScene();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool MenuState::pause()
{
	GameFramework::getSingletonPtr()->mpLog->logMessage("Pausing MenuState...");

	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void MenuState::resume()
{
	GameFramework::getSingletonPtr()->mpLog->logMessage("Resuming MenuState...");

	GameFramework::getSingletonPtr()->mpViewport->setCamera(mtpCamera);

	mvQuit = false;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void MenuState::createScene()
{
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void MenuState::exit()
{
	GameFramework::getSingletonPtr()->mpLog->logMessage("Leaving MenuState...");
		
	mtpSceneMgr->destroyCamera(mtpCamera);
	if(mtpSceneMgr)
		GameFramework::getSingletonPtr()->mpRoot->destroySceneManager(mtpSceneMgr);
}

//|||||||||||||||||||||||||||||||||||||||||||||||

//|||||||||||||||||||||||||||||||||||||||||||||||



//|||||||||||||||||||||||||||||||||||||||||||||||

bool MenuState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
	if(GameFramework::getSingletonPtr()->mpKeyboard->isKeyDown(OIS::KC_ESCAPE))
	{
		mvQuit = true;
		return true;
	}

	GameFramework::getSingletonPtr()->keyPressed(keyEventRef);

	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool MenuState::keyReleased(const OIS::KeyEvent &keyEventRef)
{		
	GameFramework::getSingletonPtr()->keyReleased(keyEventRef);

	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool MenuState::mouseMoved(const OIS::MouseEvent &evt)
{

	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool MenuState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{


	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool MenuState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
		

	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void MenuState::update(double timeSinceLastFrame)
{
	if(mvQuit == true)
	{
		this->popAppState();
		return;
	}
}

//|||||||||||||||||||||||||||||||||||||||||||||||