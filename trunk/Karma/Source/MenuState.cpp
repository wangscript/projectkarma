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
	if(!Ogre::ResourceGroupManager::getSingletonPtr()->resourceGroupExists("Menu"))
		GameFramework::getSingletonPtr()->loadMenuResources();
	GameFramework::getSingletonPtr()->mpRoot->renderOneFrame();

	bool fromGame;
	if(Ogre::ResourceGroupManager::getSingletonPtr()->resourceGroupExists("Game"))
		fromGame = true;
	else
		fromGame = false;

	Ogre::String s = "GuiMenu/Menu";
	mvGUI = new GUI<MenuState>(this, s);

	Ogre::String resume = "GuiMenu/Menu/Resume";
	void (MenuState::*resumeFunction)() = &MenuState::resumeToGameState;
	int resumeButtoN = mvGUI->addMouseOver(resume,resumeFunction,true,!fromGame);

	Ogre::String quit = "GuiMenu/Menu/Quit";
	void (MenuState::*quitFunction)() = &MenuState::quit;
	int quitButtoN = mvGUI->addMouseOver(quit,quitFunction);

	Ogre::String disc = "GuiMenu/Menu/Disconnect";
	void (MenuState::*discFunction)() = &MenuState::disconnect;
	int disconnectButtoN = mvGUI->addMouseOver(disc,discFunction,true,!fromGame);

	Ogre::String newgame = "GuiMenu/Menu/NewGame";
	void (MenuState::*newGameFunction)() = &MenuState::createNewGameState;
	int newGameButtoN = mvGUI->addMouseOver(newgame,newGameFunction,true,fromGame);

	std::vector<int> test;
	test.push_back(disconnectButtoN);
	test.push_back(resumeButtoN);
	mvGUI->addLockedRelation(disconnectButtoN,test);

	std::vector<int> test2;
	test2.push_back(newGameButtoN);
	mvGUI->addUnlockedRelation(disconnectButtoN,test2);

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
	Ogre::OverlayManager::getSingletonPtr()->getByName("Loading")->show();
	Ogre::OverlayManager::getSingleton().getByName("GuiMenu/Menu")->hide();
	GameFramework::getSingletonPtr()->mpRoot->renderOneFrame();
	mtpSceneMgr->destroyCamera(mtpCamera);
	delete mvGUI;
	if(mtpSceneMgr)
		GameFramework::getSingletonPtr()->mpRoot->destroySceneManager(mtpSceneMgr);
}

//|||||||||||||||||||||||||||||||||||||||||||||||

//|||||||||||||||||||||||||||||||||||||||||||||||

void MenuState::createNewGameState()
{

	this->changeAppState(findByName("GameState"));
}

void MenuState::resumeToGameState()
{
	this->popAppState();
}

void MenuState::disconnect()
{
	this->popGameState();
}

void MenuState::quit()
{
	
	this->popAllAppStates();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool MenuState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
	if(GameFramework::getSingletonPtr()->mpKeyboard->isKeyDown(OIS::KC_ESCAPE))
	{
		mvQuit = true;
		return true;
	}

	if(GameFramework::getSingletonPtr()->mpKeyboard->isKeyDown(OIS::KC_Q))
	{
		resumeToGameState();
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
	GameFramework::getSingletonPtr()->mpGui->updateCursorPos(evt.state.X.abs,evt.state.Y.abs );
	mvGUI->checkMouseOver(evt.state.X.abs, evt.state.Y.abs);
		
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool MenuState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	if(mvGUI->mousePressed())
		mvGUI->resetMouseOver();
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
		quit();
		return;
	}
}

//|||||||||||||||||||||||||||||||||||||||||||||||