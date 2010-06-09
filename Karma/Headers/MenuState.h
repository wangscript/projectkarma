/*---------------------------------------------------------------------------------*/
/* File: MenuState.h															   */
/* Author: Per Karlsson, perkarlsson89@gmail.com								   */
/*																				   */
/* Description:	http://www.ogre3d.org/wiki/index.php/Advanced_Ogre_Framework	   */
/* Based on the example in the Advanced Ogre Framework by spacegaier.			   */
/*---------------------------------------------------------------------------------*/

#ifndef MENU_STATE_H
#define MENU_STATE_H

#include "AppState.h"
#include "GuiMouseOver.h"

class MenuState : public AppState
{
public:
	/*This line calls the macro defined in AppState.h, making this class a valid application state*/
	DECLARE_APPSTATE_CLASS(MenuState)

	/* Always called when starting this state and does the following steps:
	1. Create SceneManager
	2. Create a Camera
	3. Set input callbacks
	4. Tell the GUI which SceneManager to use (removed)
	5. Set the mouse cursor image (removed)
	6. Tell the GUI System which Window to render (removed)
	7. Get the buttons from the GUI and set the respective click handlers
	8. Call createScene() to fill the scene with content (although no scene to fill)*/
	void enter();

	/* Does nothing */
	void createScene();

	/* Exits the Menu State. Called when starting a new Game State and therefore a loading screen will be shown. */
	void exit();
	
	/* Didn't bother to set anything here since MenuState will never be paused. */
	bool pause();
	/* Didn't bother to set anything here since MenuState will never be resumed. */
	void resume();

	/* Quits on [Escape] and forwards the unhandled input to the GameFramework class */
	bool keyPressed(const OIS::KeyEvent &keyEventRef);

	/* Forwards the unhandled input to the GameFramework class */
	bool keyReleased(const OIS::KeyEvent &keyEventRef);

	/* Injects the mouse movements to the GUI. */ 
	bool mouseMoved(const OIS::MouseEvent &evt);

	/* Injects the mouse clicks to the GUI */
	bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id); 

	/* Does nothing */
	bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

	/* The game state will always be in the background waiting. Pops the Menu state. */
	void resumeToGameState();

	/* Creates a new Game State. */
	void createNewGameState();

	/* Pops the Game state in the states list. */
	void disconnect();

	/* Pops all states and quits the application. */
	void quit();

	void update(double timeSinceLastFrame);

private:
	bool								mvQuit;
	GuiMouseOver<MenuState>*			mvGuiMouseOver;
};
#endif
