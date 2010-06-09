/*---------------------------------------------------------------------------------*/
/* File: AppStateManager.cpp														   */
/* Author: spacegaier http://www.ogre3d.org/wiki/index.php/Advanced_Ogre_Framework */
/* Modified by: Per Karlsson, perkarlsson89@gmail.com							   */
/*																				   */
/* Description:	The class AppStateManager inherits from the class AppStateListener */
/* shown above and mainly implements its abstract methods. Furthermore it contains */
/* - a std::vector for all existing states (mtStates).							   */
/* - a std::vector for the active states, so the stack of those states currently.  */
/* is use (mtActiveStateStack) 													   */
/*---------------------------------------------------------------------------------*/

#include "AppStateManager.h"
#include <OgreWindowEventUtilities.h>

/*---------------------------------------------------------------------------------*/
AppStateManager::AppStateManager()
{
	/* Setting the shutdown indicator. */
	mtbShutdown = false;
}
/*---------------------------------------------------------------------------------*/
AppStateManager::~AppStateManager()
{
	/* Exiting all active application states and emptying the std::vectors */
	while(!mtActiveStateStack.empty()) 
	{
		mtActiveStateStack.back()->exit();
		mtActiveStateStack.pop_back();
	}

	while(!mtStates.empty())
	{
		mtStates.pop_back();
	}
}
/*---------------------------------------------------------------------------------*/
void AppStateManager::manageAppState(Ogre::String stateName, AppState* state)
{
	/* Called from within the state creation macro and sets some information of the new state, 
	as well as pushing it on the active states stack */
	try
	{
		state_info new_state_info;
		new_state_info.name = stateName;
		new_state_info.state = state;
		mtStates.push_back(new_state_info);		
	}
	catch(std::exception& e)
	{
		delete state;
		throw Ogre::Exception(Ogre::Exception::ERR_INTERNAL_ERROR, "Error while trying to manage a new AppState\n" + Ogre::String(e.what()), "AppStateManager.cpp (39)");
	}
}
/*---------------------------------------------------------------------------------*/
AppState* AppStateManager::findByName(Ogre::String stateName)
{
	//Returns a pointer to the state with the respective name.
	std::vector<state_info>::iterator itr;
	for(itr=mtStates.begin();itr!=mtStates.end();itr++)
	{
		if(itr->name==stateName)
			return itr->state;
	}
	return 0;
}
/*---------------------------------------------------------------------------------*/
void AppStateManager::start(AppState* state)
{
	/* Main loop of the application that does the following steps:
	1. Change to the state specified */

	changeAppState(state);
	
	double timeSinceLastFrame = 1;
	int startTime = 0;

	//2. Start loop
	while(!mtbShutdown) 
	{
		if(GameFramework::getSingletonPtr()->mpRenderWnd->isClosed())mtbShutdown = true;

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		Ogre::WindowEventUtilities::messagePump();
#endif	
		if(GameFramework::getSingletonPtr()->mpRenderWnd->isActive())
		{
			startTime = GameFramework::getSingletonPtr()->mpTimer->getMillisecondsCPU();			

			//3. Capture keyboard and mouse input
			GameFramework::getSingletonPtr()->mpKeyboard->capture();
			GameFramework::getSingletonPtr()->mpMouse->capture();

			//Might get info from the keyboard and mouse input about shutting down.
			if (mtbShutdown)
				break;

			//4. Update the current state (the top most of the stack)
			mtActiveStateStack.back()->update(timeSinceLastFrame);
			
			#ifdef DEBUG
			GameFramework::getSingletonPtr()->updateOgre(timeSinceLastFrame);
			#endif
			
			//5. Call the OgreFramework class to update and render */
			GameFramework::getSingletonPtr()->mpRoot->renderOneFrame();
		
			timeSinceLastFrame = GameFramework::getSingletonPtr()->mpTimer->getMillisecondsCPU() - startTime;
			timeSinceLastFrame = timeSinceLastFrame / 1000;
		}
		else
		{
			Sleep(1000);
		}
	}

	GameFramework::getSingletonPtr()->mpLog->logMessage("Main loop quit");
	GameFramework::getSingletonPtr()->mpLog->logMessage("Shutdown OGRE...");
}
/*---------------------------------------------------------------------------------*/
void AppStateManager::changeAppState(AppState* state)
{
	// Exits the current state (if there is any) and starts the new specified one.
	if(!mtActiveStateStack.empty()) 
	{
		mtActiveStateStack.back()->exit();
		mtActiveStateStack.pop_back();
	}

	mtActiveStateStack.push_back(state);
	init(state);
	mtActiveStateStack.back()->enter();
}
/*---------------------------------------------------------------------------------*/
bool AppStateManager::pushAppState(AppState* state)
{
	/* Puts an new state on the top of the stack and starts it. */
	if(!mtActiveStateStack.empty()) 
	{
		if(!mtActiveStateStack.back()->pause())
			return false;
	}

	mtActiveStateStack.push_back(state);
	init(state);
	mtActiveStateStack.back()->enter();

	return true;
}
/*---------------------------------------------------------------------------------*/
void AppStateManager::popAppState(void)
{
	/* Removes the top most state and resumes the one below if there is one, otherwise shutdown the application. */
	if(!mtActiveStateStack.empty()) 
	{
		mtActiveStateStack.back()->exit();
		mtActiveStateStack.pop_back();
	}

	if(!mtActiveStateStack.empty()) 
	{
		init(mtActiveStateStack.back());
		mtActiveStateStack.back()->resume();
	}
    else
		shutdown();
}
/*---------------------------------------------------------------------------------*/
void AppStateManager::popGameState()
{
	//Removes the Game state in the states stack. 
	mtActiveStateStack.front()->exit();
	std::vector<AppState*>::iterator i = mtActiveStateStack.begin();
	mtActiveStateStack.erase(i);
}
/*---------------------------------------------------------------------------------*/
void AppStateManager::popAllAppStates()
{
	//Removes all states and activates shutdown.
	while (!mtActiveStateStack.empty())
	{
	mtActiveStateStack.back()->exit();
	mtActiveStateStack.pop_back();
	}
	shutdown();
}
/*---------------------------------------------------------------------------------*/
void AppStateManager::shutdown()
{
	/* Exits the application */
	mtbShutdown=true;
}
/*---------------------------------------------------------------------------------*/
void AppStateManager::init(AppState* state)
{
	/* Initializes a new state and links the input callback on it, as well as resetting the Ogre statistics 
	(FPS, triangle count, batch count, ...)*/ 
	GameFramework::getSingletonPtr()->mpKeyboard->setEventCallback(state);
	GameFramework::getSingletonPtr()->mpMouse->setEventCallback(state);
	GameFramework::getSingletonPtr()->mpRenderWnd->resetStatistics();
}
/*---------------------------------------------------------------------------------*/