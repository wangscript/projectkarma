//|||||||||||||||||||||||||||||||||||||||||||||||

#include <WindowApp.h>

//|||||||||||||||||||||||||||||||||||||||||||||||

WindowApp::WindowApp()
{
	mtbShutdown = false;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

WindowApp::~WindowApp()
{
	delete GameFramework::getSingletonPtr();
	delete mvpAppStateManager;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void WindowApp::startWindowApp()
{
	new GameFramework();
	GameFramework::getSingletonPtr()->initOgre("AdvancedGameFramework", 0, 0);
	
	GameFramework::getSingletonPtr()->mpLog->logMessage("Demo initialized!");

	mvpAppStateManager = new AppStateManager();

	MenuState::create(mvpAppStateManager, "MenuState");
	GameState::create(mvpAppStateManager, "GameState");

	mvpAppStateManager->start(mvpAppStateManager->findByName("GameState"));
}

//|||||||||||||||||||||||||||||||||||||||||||||||