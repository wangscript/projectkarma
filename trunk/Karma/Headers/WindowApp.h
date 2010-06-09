/*---------------------------------------------------------------------------------*/
/* File: WindowApp.h															   */
/* Author: spacegaier															   */
/*																				   */
/* Description:	http://www.ogre3d.org/wiki/index.php/Advanced_Ogre_Framework	   */
/* The WindowApp class is the central application organizing location. It only	   */
/* has two member variables:										  			   */
/* the application state manager												   */
/* an indicator of whether the application is to be shut down or not			   */
/* Note: All used game states must be included here!			  				   */
/*---------------------------------------------------------------------------------*/

#ifndef WINDOW_APP_H
#define WINDOW_APP_H

#include "GameFramework.h"
#include "AppStateManager.h"

#include "MenuState.h"
#include "GameState.h"

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

#endif 