//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef PLAY_STATE_HPP
#define PLAY_STATE_HPP

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "AppState.hpp"
#include "CameraHandler.h"
#include "Character.h"
#include <OgreSubEntity.h>
#include <OgreMaterialManager.h>
#include <NxOgre.h>
#include <NxOgreOGRE3D.h>

#define CAMERA_HEIGHT			1.2
#define CAMERA_DISTANCE			3


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

        void moveCamera();
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

    //NxOgre instances
	NxOgre::World*							m_PhysicsWorld;
	NxOgre::Scene*							m_PhysicsScene;
	NxOgre::TimeController*					m_PhysicsTimeController;
	OGRE3DRenderSystem*						m_PhysicsRenderSystem;

	//NxOgre objects

	CameraHandler*									m_CameraHandler;
	Character*									m_Character;
	bool                                    m_bQuit;

	Ogre::Vector3                           m_TranslateVector;
	Ogre::Real                              m_MoveSpeed; 
	float                                   m_MoveScale; 

	bool                                    m_bLMouseDown, m_bRMouseDown;
	bool                                    m_bChatMode;

        CEGUI::Window*                          m_pMainWnd;
        CEGUI::Window*                          m_pChatWnd;
};

//|||||||||||||||||||||||||||||||||||||||||||||||

#endif

//|||||||||||||||||||||||||||||||||||||||||||||||