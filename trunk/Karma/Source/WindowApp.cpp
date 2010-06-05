//|||||||||||||||||||||||||||||||||||||||||||||||

#include <WindowApp.h>

//|||||||||||||||||||||||||||||||||||||||||||||||

WindowApp::WindowApp()
{
	mtbShutdown = false;
	mvpAppStateManager=0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

WindowApp::~WindowApp()
{
	delete GameFramework::getSingletonPtr();
	if(mvpAppStateManager)
	delete mvpAppStateManager;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

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

//|||||||||||||||||||||||||||||||||||||||||||||||