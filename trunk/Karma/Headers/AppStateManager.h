//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef APP_STATE_MANAGER_H
#define APP_STATE_MANAGER_H

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "AppState.h"

//|||||||||||||||||||||||||||||||||||||||||||||||

class AppStateManager : public AppStateListener
{
public:
	typedef struct
	{
		Ogre::String name;
		AppState* state;
	} state_info;

	AppStateManager();
	~AppStateManager();

	void manageAppState(Ogre::String stateName, AppState* state);

	AppState* findByName(Ogre::String stateName);

	void start(AppState* state);
	void changeAppState(AppState* state);
	bool pushAppState(AppState* state);
	void popAppState(void);
	void popGameState();
	void popAllAppStates();
	void shutdown(void);

protected:
	void init(AppState *state);

	std::vector<AppState*>		mtActiveStateStack;
	std::vector<state_info>		mtStates;
	bool						mtbShutdown;
};

//|||||||||||||||||||||||||||||||||||||||||||||||

#endif

//|||||||||||||||||||||||||||||||||||||||||||||||