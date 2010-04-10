//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef GAME_FRAMEWORK_H
#define GAME_FRAMEWORK_H

//|||||||||||||||||||||||||||||||||||||||||||||||

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

//|||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||

class GameFramework : public Ogre::Singleton<GameFramework>, OIS::KeyListener, OIS::MouseListener
{
public:
	GameFramework();
	~GameFramework();

	void initOgre(Ogre::String wndTitle, OIS::KeyListener *pKeyListener = 0, OIS::MouseListener *pMouseListener = 0);
	void updateOgre(double timeSinceLastFrame);
	void updateStats();

	bool keyPressed(const OIS::KeyEvent &keyEventRef);
	bool keyReleased(const OIS::KeyEvent &keyEventRef);

	bool mouseMoved(const OIS::MouseEvent &evt);
	bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id); 
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

	Ogre::Overlay*				mtpDebugOverlay;
	Ogre::Overlay*				mtpInfoOverlay;
	int							mtNumScreenShots;
};

//|||||||||||||||||||||||||||||||||||||||||||||||

#endif 

//|||||||||||||||||||||||||||||||||||||||||||||||