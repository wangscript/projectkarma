//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef GAME_STATE_H
#define GAME_STATE_H
//|||||||||||||||||||||||||||||||||||||||||||||||

#include "AppState.h"
#include "CameraHandler.h"
#include "Character.h"
#include "PowerUp.h"
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
		void createMotionBlurEffects();
        void exit();
        bool pause();
        void resume();
        void getInput();

        bool keyPressed(const OIS::KeyEvent &keyEventRef);
        bool keyReleased(const OIS::KeyEvent &keyEventRef);

        bool mouseMoved(const OIS::MouseEvent &arg);
        bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id); 
        bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

        void onLeftPressed(const OIS::MouseEvent &evt);

        void update(double timeSinceLastFrame);

        void setBufferedMode();
        void setUnbufferedMode();

private:

    //NxOgre instances
	
	NxOgre::World*							m_PhysicsWorld;
	NxOgre::Scene*							m_PhysicsScene;
	NxOgre::TimeController*					m_PhysicsTimeController;
	OGRE3DRenderSystem*						m_PhysicsRenderSystem;
	bool                                    mvQuit;

	CameraHandler*							m_CameraHandler;
	Character*								m_Character;
	PowerUp*								m_PowerUps;

	bool                                    mvLMouseDown, mvRMouseDown;
	bool                                    mvChatMode;

};

//|||||||||||||||||||||||||||||||||||||||||||||||

#endif

//|||||||||||||||||||||||||||||||||||||||||||||||