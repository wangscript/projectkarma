/*---------------------------------------------------------------------------------*/
/* File: WindowApp.cpp															   */
/* Author: spacegaier															   */
/*																				   */
/* Description:	http://www.ogre3d.org/wiki/index.php/Advanced_Ogre_Framework	   */
/* The WindowApp class is the central application organizing location. It only	   */
/* has two member variables:										  			   */
/* the application state manager												   */
/* an indicator of whether the application is to be shut down or not			   */
/* Note: All used game states must be included here!			  				   */
/*---------------------------------------------------------------------------------*/

#include <WindowApp.h>
/*---------------------------------------------------------------------------------*/
WindowApp::WindowApp()
{
	mtbShutdown = false;
	mvpAppStateManager=0;
}
/*---------------------------------------------------------------------------------*/
WindowApp::~WindowApp()
{
	delete GameFramework::getSingletonPtr();
	if(mvpAppStateManager)
	delete mvpAppStateManager;
}
/*---------------------------------------------------------------------------------*/
void WindowApp::startWindowApp()
{
	new GameFramework();
	if (GameFramework::getSingletonPtr()->initOgre("Karma 3D", 0, 0))
	{
		GameFramework::getSingletonPtr()->mpLog->logMessage("Demo initialized!");

		mvpAppStateManager = new AppStateManager();


		GameState::create(mvpAppStateManager, "GameState");
		MenuState::create(mvpAppStateManager, "MenuState");

		mvpAppStateManager->start(mvpAppStateManager->findByName("MenuState"));
	}
}
/*---------------------------------------------------------------------------------*/