//|||||||||||||||||||||||||||||||||||||||||||||||

#include "AppStateManager.h"
#include <OgreWindowEventUtilities.h>

//|||||||||||||||||||||||||||||||||||||||||||||||

AppStateManager::AppStateManager()
{
	mtbShutdown = false;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

AppStateManager::~AppStateManager()
{
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

//|||||||||||||||||||||||||||||||||||||||||||||||

void AppStateManager::manageAppState(Ogre::String stateName, AppState* state)
{
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

//|||||||||||||||||||||||||||||||||||||||||||||||

AppState* AppStateManager::findByName(Ogre::String stateName)
{
	std::vector<state_info>::iterator itr;

	for(itr=mtStates.begin();itr!=mtStates.end();itr++)
	{
		if(itr->name==stateName)
			return itr->state;
	}

	return 0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void AppStateManager::start(AppState* state)
{
	changeAppState(state);
	
	double timeSinceLastFrame = 1;
	int startTime = 0;

	while(!mtbShutdown) 
	{
		if(GameFramework::getSingletonPtr()->mpRenderWnd->isClosed())mtbShutdown = true;

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		Ogre::WindowEventUtilities::messagePump();
#endif	
		if(GameFramework::getSingletonPtr()->mpRenderWnd->isActive())
		{
			startTime = GameFramework::getSingletonPtr()->mpTimer->getMillisecondsCPU();			
			GameFramework::getSingletonPtr()->mpKeyboard->capture();
			GameFramework::getSingletonPtr()->mpMouse->capture();

			mtActiveStateStack.back()->update(timeSinceLastFrame);
			
			GameFramework::getSingletonPtr()->updateOgre(timeSinceLastFrame);
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

//|||||||||||||||||||||||||||||||||||||||||||||||

void AppStateManager::changeAppState(AppState* state)
{
	if(!mtActiveStateStack.empty()) 
	{
		mtActiveStateStack.back()->exit();
		mtActiveStateStack.pop_back();
	}

	mtActiveStateStack.push_back(state);
	init(state);
	mtActiveStateStack.back()->enter();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool AppStateManager::pushAppState(AppState* state)
{
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

//|||||||||||||||||||||||||||||||||||||||||||||||

void AppStateManager::popAppState(void)
{
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

//|||||||||||||||||||||||||||||||||||||||||||||||

void AppStateManager::shutdown()
{
	mtbShutdown=true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void AppStateManager::init(AppState* state)
{
	GameFramework::getSingletonPtr()->mpKeyboard->setEventCallback(state);
	GameFramework::getSingletonPtr()->mpMouse->setEventCallback(state);

	GameFramework::getSingletonPtr()->mpRenderWnd->resetStatistics();
}

//|||||||||||||||||||||||||||||||||||||||||||||||
