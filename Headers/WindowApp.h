//|||||||||||||||||||||||||||||||||||||||||||||||
//
#ifndef WINDOW_APP_H
#define WINDOW_APP_H

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "GameFramework.h"
#include "AppStateManager.h"

#include "MenuState.h"
#include "GameState.h"

//|||||||||||||||||||||||||||||||||||||||||||||||

class WindowApp
{
public:
	WindowApp();
	~WindowApp();

	void startWindowApp();

private:
	AppStateManager*	mvpAppStateManager;
	bool				mtbShutdown;
};

//|||||||||||||||||||||||||||||||||||||||||||||||

#endif 

//|||||||||||||||||||||||||||||||||||||||||||||||