/*---------------------------------------------------------------------------------*/
/* File: AppState.h																   */
/* Author: spacegaier http://www.ogre3d.org/wiki/index.php/Advanced_Ogre_Framework */
/* Modified by: Per Karlsson, perkarlsson89@gmail.com							   */
/*																				   */
/* Description:	In this file, two classes are defined:							   */
/* AppStateListener and AppState. More info later down.							   */
/*---------------------------------------------------------------------------------*/

#ifndef APP_STATE_H
#define APP_STATE_H

#include "GameFrameWork.h"

class AppState;

/* The first class will later be inherited by the application state manager, but has to be defined here due to design reasons. */
class AppStateListener
{
public:
	AppStateListener(void){};
	virtual ~AppStateListener(void){};

	/* Function to later add a new state to the manager. */
	virtual void manageAppState(Ogre::String stateName, AppState* state) = 0;

	/* Returns a pointer to the state with the respective name. */
	virtual AppState*	findByName(Ogre::String stateName) = 0;

	/* Exits the current app state and starts the one specified as the parameter. */
	virtual void		changeAppState(AppState *state) = 0;

	/* Puts a new app state on the active state stack that will then be excecuted. */
	virtual bool		pushAppState(AppState* state) = 0;

	/* Removes the top active state from the stack, which results in returning to the one below. */
	virtual void		popAppState() = 0;

	/* Not in the Advanced Framework originally. Removes the Game State. */
	virtual void		popGameState() = 0;

	/* Not in the Advanced Framework originally. Removes all states and shut down the application. */
	virtual void		popAllAppStates() = 0;

	/* Well, guess what happens here... */
	virtual void		shutdown() = 0;
};

/* The second class is the app state blueprint from which each actual application state will inherit: */
class AppState : public OIS::KeyListener, public OIS::MouseListener
{
public:
	static void create(AppStateListener* parent, const Ogre::String name){};

	void destroy(){delete this;}

	virtual void enter(void) = 0;
	virtual void exit(void) = 0;
	virtual bool pause(void){return false;}
	virtual void resume(void){};
	virtual void update(double timeSinceLastFrame) = 0;

protected:
	AppState(void){};
	
	/* see AppStateListener */
	AppState*	findByName(Ogre::String stateName){return mtpParent->findByName(stateName);}

	/* see AppStateListener */
	void		changeAppState(AppState* state){mtpParent->changeAppState(state);}

	/* see AppStateListener */
	bool		pushAppState(AppState* state){return mtpParent->pushAppState(state);}

	/* see AppStateListener */
	void		popAppState(void){mtpParent->popAppState();}

	/* see AppStateListener */
	void		popGameState(){mtpParent->popGameState();}

	/* see AppStateListener */
	void 		popAllAppStates(){mtpParent->popAllAppStates();}

	/* see AppStateListener */
	void		shutdown(void){mtpParent->shutdown();}
	
	AppStateListener*		mtpParent;
	Ogre::Camera*			mtpCamera;
	Ogre::SceneManager*		mtpSceneMgr;
};

/* The last part of this file is a big #define statement. It defines the makro DECLARE_APPSTATE_CLASS via 
that you can later create the game states. Note: The backslashes there are very important as they tell the 
compiler to take those lines as one big one. Otherwise you would get compiler errors...*/

#define DECLARE_APPSTATE_CLASS(T)										\
static void create(AppStateListener* parent, const Ogre::String name)	\
{																		\
	T* myAppState = new T();											\
	myAppState->mtpParent = parent;										\
	parent->manageAppState(name, myAppState);							\
}


#endif