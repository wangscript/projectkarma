//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef PLAY_STATE_HPP
#define PLAY_STATE_HPP

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "AppState.hpp"

#include "DotScene.h"
#include <btBulletCollisionCommon.h>
#include "OgreToBtMeshConverter.h"
#include "Player.h"


//|||||||||||||||||||||||||||||||||||||||||||||||

//|||||||||||||||||||||||||||||||||||||||||||||||

class GameState : public AppState
{
public:
	GameState();

	DECLARE_APPSTATE_CLASS(GameState)

	void enter();
	void createScene();
	void exit();
	bool pause();
	void resume();

	void move();
	void getInput();

	bool keyPressed(const OIS::KeyEvent &keyEventRef);
	bool keyReleased(const OIS::KeyEvent &keyEventRef);

	bool mouseMoved(const OIS::MouseEvent &arg);
	bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id); 
	bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	void onLeftPressed(const OIS::MouseEvent &evt);
	bool onExitButtonGame(const CEGUI::EventArgs &args);

	void update(double timeSinceLastFrame);

	void setBufferedMode();
	void setUnbufferedMode();

private:
	bool						m_bQuit;
		
	Ogre::Vector3				m_TranslateVector;
	Ogre::Real					m_MoveSpeed; 
	Ogre::Degree				m_RotateSpeed; 
	float						m_MoveScale; 
	Ogre::Degree				m_RotScale;

	bool						m_bLMouseDown, m_bRMouseDown;
	bool						m_bChatMode;

	CEGUI::Window*				m_pMainWnd;
	CEGUI::Window*				m_pChatWnd;

	Player*						m_Player;
	
	btDefaultCollisionConfiguration*	m_ColConfig; 
    btCollisionWorld*					m_ColWorld;
    btCollisionDispatcher*				m_Dispatcher;
    btAxisSweep3*						m_Broadphase; 
};

//|||||||||||||||||||||||||||||||||||||||||||||||

#endif

//|||||||||||||||||||||||||||||||||||||||||||||||