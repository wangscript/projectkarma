/*---------------------------------------------------------------------------------*/
/* File: AppStateManager.h														   */
/* Author: spacegaier http://www.ogre3d.org/wiki/index.php/Advanced_Ogre_Framework */
/* Modified by: Per Karlsson, perkarlsson89@gmail.com							   */
/*																				   */
/* Description:	The class AppStateManager inherits from the class AppStateListener */
/* shown above and mainly implements its abstract methods. Furthermore it contains */
/* - a std::vector for all existing states (mtStates).							   */
/* - a std::vector for the active states, so the stack of those states currently.  */
/* is use (mtActiveStateStack) 													   */
/*---------------------------------------------------------------------------------*/

#ifndef APP_STATE_MANAGER_H
#define APP_STATE_MANAGER_H

#include "AppState.h"

class AppStateManager : public AppStateListener
{
public:
	typedef struct
	{
		Ogre::String name;
		AppState* state;
	} state_info;

	/* Constructor, just setting the shutdown indicator. */
	AppStateManager();

	/* Destructor, exiting all active application states and emptying the std::vectors */
	~AppStateManager();

	/* Called from within the state creation macro and sets some information of the new state, 
	as well as pushing it on the active states stack */
	void manageAppState(Ogre::String stateName, AppState* state);

	/* Returns a pointer to the state with the respective name. */
	AppState* findByName(Ogre::String stateName);

	/* Main loop of the application that does the following steps:
	1. Change to the state specified
	2. Start loop
	3. Capture keyboard and mouse input
	4. Update the current state (the top most of the stack)
	5. Call the OgreFramework class to update and render */
	void start(AppState* state);

	/* Exits the current state (if there is any) and starts the new specified one. */
	void changeAppState(AppState* state);

	/* Puts an new state on the top of the stack and starts it. */
	bool pushAppState(AppState* state);

	/* Removes the top most state and resumes the one below if there is one, otherwise shutdown the application. */
	void popAppState(void);

	/* Not in the Advanced Framework originally. Removes the Game state in the states stack. */
	void popGameState();

	/*Not in the Advanced Framework originally. Removes all states and activates shutdown. */
	void popAllAppStates();

	/* Exits the application */
	void shutdown(void);

protected:
	/* Initializes a new state and links the input callback on it, as well as resetting the Ogre statistics 
	(FPS, triangle count, batch count, ...)*/ 
	void init(AppState *state);

	std::vector<AppState*>		mtActiveStateStack;
	std::vector<state_info>		mtStates;
	bool						mtbShutdown;
};

#endif
