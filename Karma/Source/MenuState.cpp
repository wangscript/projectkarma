/*---------------------------------------------------------------------------------*/
/* File: MenuState.cpp															   */
/* Author: Per Karlsson, perkarlsson89@gmail.com								   */
/*																				   */
/* Description:	http://www.ogre3d.org/wiki/index.php/Advanced_Ogre_Framework	   */
/* Based on the example in the Advanced Ogre Framework by spacegaier.			   */
/*---------------------------------------------------------------------------------*/

#include "MenuState.h"

/*---------------------------------------------------------------------------------*/
void MenuState::enter()
{
	GameFramework::getSingletonPtr()->mpLog->logMessage("Entering MenuState...");

	//Initiate the Scene Manager.
	mtpSceneMgr = GameFramework::getSingletonPtr()->mpRoot->createSceneManager(Ogre::ST_GENERIC, "MenuSceneMgr");

	//Create camera
	mtpCamera = mtpSceneMgr->createCamera("MenuCam");
	mtpCamera->setAspectRatio(Ogre::Real(GameFramework::getSingletonPtr()->mpViewport->getActualWidth()) / 
							  Ogre::Real(GameFramework::getSingletonPtr()->mpViewport->getActualHeight()));
	GameFramework::getSingletonPtr()->mpViewport->setCamera(mtpCamera);

	//Set input
	GameFramework::getSingletonPtr()->mpKeyboard->setEventCallback(this);
	GameFramework::getSingletonPtr()->mpMouse->setEventCallback(this);

	mvQuit = false;

	//If the resource group "Menu" doesn't exist, load and create it.
	if(!Ogre::ResourceGroupManager::getSingletonPtr()->resourceGroupExists("Menu"))
		GameFramework::getSingletonPtr()->loadMenuResources();
	GameFramework::getSingletonPtr()->mpRoot->renderOneFrame();

	//If the resource group "Game" exist, we come from the game. If not, we have just started the application.
	bool fromGame;
	if(Ogre::ResourceGroupManager::getSingletonPtr()->resourceGroupExists("Game"))
		fromGame = true;
	else
		fromGame = false;
	
	//Create GUI with mouse over support
	Ogre::String s = "GuiMenu/Menu";
	mvGuiMouseOver = new GuiMouseOver<MenuState>(this, s);

	//Add Resume button.
	Ogre::String resume = "GuiMenu/Menu/Resume";
	//Bind the function resumeToGameState() to the button.
	void (MenuState::*resumeFunction)() = &MenuState::resumeToGameState;
	//the bool fromGame determines if the button is locked or not.
	int resumeButtoN = mvGuiMouseOver->addMouseOver(resume,resumeFunction,true,!fromGame);

	//Add Quit button.
	Ogre::String quit = "GuiMenu/Menu/Quit";
	//Bind the function quit() to the button.
	void (MenuState::*quitFunction)() = &MenuState::quit;
	int quitButtoN = mvGuiMouseOver->addMouseOver(quit,quitFunction);

	//Add Disconnect button.
	Ogre::String disc = "GuiMenu/Menu/Disconnect";
	//Bind the function quit() to the disconnect.
	void (MenuState::*discFunction)() = &MenuState::disconnect;
	//the bool fromGame determines if the button is locked or not.
	int disconnectButtoN = mvGuiMouseOver->addMouseOver(disc,discFunction,true,!fromGame);

	//Add New Game button.
	Ogre::String newgame = "GuiMenu/Menu/NewGame";
	//Bind the function createNewGameState() to the disconnect.
	void (MenuState::*newGameFunction)() = &MenuState::createNewGameState;
	//the bool fromGame determines if the button is locked or not.
	int newGameButtoN = mvGuiMouseOver->addMouseOver(newgame,newGameFunction,true,fromGame);

	//When the disconnect button is pressed, disconnect and resume button will be locked.
	std::vector<int> disconnectLockedButtons;
	disconnectLockedButtons.push_back(disconnectButtoN);
	disconnectLockedButtons.push_back(resumeButtoN);
	mvGuiMouseOver->addLockedRelation(disconnectButtoN,disconnectLockedButtons);

	//When the disconnect button is pressed, New Game button will be unlocked.
	std::vector<int> disconnectUnlockedButtons;
	disconnectUnlockedButtons.push_back(newGameButtoN);
	mvGuiMouseOver->addUnlockedRelation(disconnectButtoN,disconnectUnlockedButtons);


	createScene();
}
/*---------------------------------------------------------------------------------*/
bool MenuState::pause()
{
	//Didn't bother to set anything here since MenuState will never be paused.
	return true;
}
/*---------------------------------------------------------------------------------*/
void MenuState::resume()
{
	//Didn't bother to set anything here since MenuState will never be resumed.
}
/*---------------------------------------------------------------------------------*/
void MenuState::createScene()
{
}
/*---------------------------------------------------------------------------------*/
void MenuState::exit()
{
	GameFramework::getSingletonPtr()->mpLog->logMessage("Leaving MenuState...");
	
	//Show the loading texture
	Ogre::OverlayManager::getSingletonPtr()->getByName("Loading")->show();

	//Hide the Menu Overlay
	Ogre::OverlayManager::getSingleton().getByName("GuiMenu/Menu")->hide();

	//Render one frame to update and show the loading texture.
	GameFramework::getSingletonPtr()->mpRoot->renderOneFrame();

	//Destroy GuiMouseOver
	delete mvGuiMouseOver;

	//Destroy camera and scene manager.
	mtpSceneMgr->destroyCamera(mtpCamera);
	if(mtpSceneMgr)
		GameFramework::getSingletonPtr()->mpRoot->destroySceneManager(mtpSceneMgr);
}
/*---------------------------------------------------------------------------------*/
void MenuState::createNewGameState()
{
	//Creates a new GameState.
	this->changeAppState(findByName("GameState"));
}
/*---------------------------------------------------------------------------------*/
void MenuState::resumeToGameState()
{
	//The game state will always be in the background waiting.
	this->popAppState();
}
/*---------------------------------------------------------------------------------*/
void MenuState::disconnect()
{
	//Pops the Game state in the states list.
	this->popGameState();
}
/*---------------------------------------------------------------------------------*/
void MenuState::quit()
{
	//Pops all states and quits the application.
	this->popAllAppStates();
}
/*---------------------------------------------------------------------------------*/
bool MenuState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
	//Escape = Quit.
	if(GameFramework::getSingletonPtr()->mpKeyboard->isKeyDown(OIS::KC_ESCAPE))
		mvQuit = true;

	GameFramework::getSingletonPtr()->keyPressed(keyEventRef);
	return true;
}
/*---------------------------------------------------------------------------------*/
bool MenuState::keyReleased(const OIS::KeyEvent &keyEventRef)
{		
	GameFramework::getSingletonPtr()->keyReleased(keyEventRef);
	return true;
}
/*---------------------------------------------------------------------------------*/
bool MenuState::mouseMoved(const OIS::MouseEvent &evt)
{
	//Update GUI cursor.
	GameFramework::getSingletonPtr()->mpGui->updateCursorPos(evt.state.X.abs,evt.state.Y.abs );

	//Check for mouse over elements.
	mvGuiMouseOver->checkMouseOver(evt.state.X.abs, evt.state.Y.abs);
		
	return true;
}
/*---------------------------------------------------------------------------------*/
bool MenuState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	//Fires an event if there is any active Mouse Over Element.
	if(mvGuiMouseOver->mousePressed())
		mvGuiMouseOver->resetMouseOver();
	return true;
}
/*---------------------------------------------------------------------------------*/
bool MenuState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	return true;
}
/*---------------------------------------------------------------------------------*/
void MenuState::update(double timeSinceLastFrame)
{
	//Active as long as mvQuit is false
	if(mvQuit == true)
		quit();
}
/*---------------------------------------------------------------------------------*/