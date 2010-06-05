//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef GAME_STATE_H
#define GAME_STATE_H
//|||||||||||||||||||||||||||||||||||||||||||||||

#include "CameraHandler.h"
#include "Player.h"
#include "PowerUp.h"
#include "WorldManager.h"
#include "PhysicsManager.h"
#include "SceneLoader.h"
#include "NPCAimer.h"
#include "NPCHandler.h"
#include <stdlib.h>
#include <time.h>



#include "GameCommon.h"

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

        void update(double timeSinceLastFrame);

private:

	PhysicsManager*							mvpPhysicsMgr;
	bool                                    mvQuit,mvRightButton;

	Player*								m_Character;
	NPC*								mNPC;//SKA BORT @todo
	PowerUp*								m_PowerUps;
	WorldManager*							mvpWorld;

};

//|||||||||||||||||||||||||||||||||||||||||||||||

#endif

//|||||||||||||||||||||||||||||||||||||||||||||||