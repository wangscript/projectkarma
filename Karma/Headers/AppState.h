//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef APP_STATE_H
#define APP_STATE_H

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "GameFrameWork.h"

class AppState;

//|||||||||||||||||||||||||||||||||||||||||||||||

class AppStateListener
{
public:
	AppStateListener(void){};
	virtual ~AppStateListener(void){};

	virtual void manageAppState(Ogre::String stateName, AppState* state) = 0;

	virtual AppState*	findByName(Ogre::String stateName) = 0;
	virtual void		changeAppState(AppState *state) = 0;
	virtual bool		pushAppState(AppState* state) = 0;
	virtual void		popAppState() = 0;
	virtual void		shutdown() = 0;
};

//|||||||||||||||||||||||||||||||||||||||||||||||

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
	
	AppState*	findByName(Ogre::String stateName){return mtpParent->findByName(stateName);}
	void		changeAppState(AppState* state){mtpParent->changeAppState(state);}
	bool		pushAppState(AppState* state){return mtpParent->pushAppState(state);}
	void		popAppState(void){mtpParent->popAppState();}
	void		shutdown(void){mtpParent->shutdown();}
	
	AppStateListener*		mtpParent;
	
	Ogre::Camera*			mtpCamera;
	Ogre::SceneManager*		mtpSceneMgr;
};

//|||||||||||||||||||||||||||||||||||||||||||||||

#define DECLARE_APPSTATE_CLASS(T)										\
static void create(AppStateListener* parent, const Ogre::String name)	\
{																		\
	T* myAppState = new T();											\
	myAppState->mtpParent = parent;										\
	parent->manageAppState(name, myAppState);							\
}

//|||||||||||||||||||||||||||||||||||||||||||||||

#endif

//|||||||||||||||||||||||||||||||||||||||||||||||
