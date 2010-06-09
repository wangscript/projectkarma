/*---------------------------------------------------------------------------------*/
/* File: GameFramework.h														   */
/* Author: spacegaier http://www.ogre3d.org/wiki/index.php/Advanced_Ogre_Framework */
/* Modified by: Per Karlsson, perkarlsson89@gmail.com							   */
/*																				   */
/* Description:	It contains most of the Ogre related variables:					   */
/* - Root																		   */	
/* - RenderWindow																   */		
/* - Viewport																       */				
/* - Log																		   */
/* - Timer																		   */
/* - InputManager / Keyboard / Mouse											   */
/*																				   */		
/* It also offers functions to handle input, but that is almost not used here, as  */
/* each application state needs a different input behavior and therefore this is   */
/* directly modeled in the application states. However, everything common for the  */
/* whole application can be put here.											   */
/*---------------------------------------------------------------------------------*/

#ifndef GAME_FRAMEWORK_H
#define GAME_FRAMEWORK_H

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreOverlay.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include "SoundManager.h"
#include "GuiHandler.h"
#include "Settings.h"

class GameFramework : public Ogre::Singleton<GameFramework>, OIS::KeyListener, OIS::MouseListener
{
public:
	/* Constructor */
	GameFramework();
	/* Destructor, clearing up */
	~GameFramework();

	/* Powers up Ogre with the following steps:
	1. Create the log manager
	2. Create the Root
	3. Create the RenderWindow and the Viewport
	4. Power up OIS	
	5. If there was no MouseListener or KeyboardListener passed as a parameter, use the ones 
	from this class, otherwise the passed ones (however you still can use both, by calling 
	the OgreFramework class input functions when you handle input elsewhere)
	6. Load resources
	7. Start Timer
	8. Set up the GUI Renderer and the GUI System and load all needed layouts (removed)
	9. create and show the debug overlay (removed)
	
	Note: After running this function you still won't see anything on the screen as there is 
	no Camera and no SceneManager. Those are members of the individual application states! */
	bool initOgre(Ogre::String wndTitle, OIS::KeyListener *pKeyListener = 0, OIS::MouseListener *pMouseListener = 0);

	/* Loads the resources used for creating the Menu. */
	void loadMenuResources();

	/* Loads the resource used in the Game. */
	void loadGameResources();


#ifdef DEBUG
	/* Loads resources used for debugging. */
	void loadDebugResources();
#endif 

	/* Is called once per frame by the AppStateManager to update everything directly related to Ogre, in our case only the statistics*/
	void updateOgre(double timeSinceLastFrame);

	/* Handles to buffered input, common for the whole application */
	bool keyPressed(const OIS::KeyEvent &keyEventRef);

	/* Handles to buffered input, common for the whole application */
	bool keyReleased(const OIS::KeyEvent &keyEventRef);

	/* Handles to buffered input, common for the whole application */
	bool mouseMoved(const OIS::MouseEvent &evt);
	
	/* Handles to buffered input, common for the whole application */
	bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id); 

	/* Handles to buffered input, common for the whole application */
	bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

	Ogre::Root*					mpRoot;
	Ogre::RenderWindow*			mpRenderWnd;
	Ogre::Viewport*				mpViewport;
	Ogre::Log*					mpLog;
	Ogre::Timer*				mpTimer;

	OIS::InputManager*			mpInputMgr;
	OIS::Keyboard*				mpKeyboard;
	OIS::Mouse*					mpMouse;
	SoundManager*				mpSound;
	GuiHandler*					mpGui;

private:
	GameFramework(const GameFramework&);
	GameFramework& operator= (const GameFramework&);
};

#endif 
