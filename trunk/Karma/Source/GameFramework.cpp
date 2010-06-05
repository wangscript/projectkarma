//|||||||||||||||||||||||||||||||||||||||||||||||

#include "GameFramework.h"

//|||||||||||||||||||||||||||||||||||||||||||||||

template<> GameFramework* Ogre::Singleton<GameFramework>::ms_Singleton = 0;

//|||||||||||||||||||||||||||||||||||||||||||||||

GameFramework::GameFramework()
{
	mpRoot				= 0;
	mpRenderWnd			= 0;
	mpViewport			= 0;
	mpLog				= 0;
	mpTimer				= 0;

	mpInputMgr			= 0;
	mpKeyboard			= 0;
	mpMouse				= 0;
	mpSound				= 0;
	mpGui				= 0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

GameFramework::~GameFramework()
{
	//@todo BUG FIXA SENARE
	/*if(m_pInputMgr)
		
		OIS::InputManager::destroyInputSystem(m_pInputMgr);
		//OIS::InputManager::destroyInputSystem(m_pInputMgr);
		delete m_pKeyboard;
		delete m_pMouse;
*/
	if(mpSound)
		mpSound->deleteSound();

	delete mpGui;
	delete mpSound;	
	delete mpRoot;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameFramework::initOgre(Ogre::String wndTitle, OIS::KeyListener *pKeyListener, OIS::MouseListener *pMouseListener)
{
	Ogre::LogManager* logMgr = new Ogre::LogManager();
	
	mpLog = Ogre::LogManager::getSingleton().createLog("GameLogfile.log", true, true, false);
	mpLog->setDebugOutputEnabled(true);
	
	mpRoot = new Ogre::Root();
	if (!mpRoot->showConfigDialog())
		return false;

	mpRenderWnd = mpRoot->initialise(true, wndTitle);

	mpViewport = mpRenderWnd->addViewport(0);
	mpViewport->setBackgroundColour(Ogre::ColourValue(0.8f, 0.8f, 0.8f, 1.0));

	mpViewport->setCamera(0);

	
	unsigned long hWnd = 0;
    OIS::ParamList paramList;
    mpRenderWnd->getCustomAttribute("WINDOW", &hWnd);

	paramList.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString(hWnd)));

	mpInputMgr = OIS::InputManager::createInputSystem(paramList);

    mpKeyboard = static_cast<OIS::Keyboard*>(mpInputMgr->createInputObject(OIS::OISKeyboard, true));
	mpMouse = static_cast<OIS::Mouse*>(mpInputMgr->createInputObject(OIS::OISMouse, true));
    
	mpMouse->getMouseState().height = mpRenderWnd->getHeight();
	mpMouse->getMouseState().width	 = mpRenderWnd->getWidth();

	if(pKeyListener == 0)
		mpKeyboard->setEventCallback(this);
	else
		mpKeyboard->setEventCallback(pKeyListener);

	if(pMouseListener == 0)
		mpMouse->setEventCallback(this);
	else
		mpMouse->setEventCallback(pMouseListener);
	
	mpRenderWnd->setActive(true);

	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

	//Load cursor resources
	Ogre::ResourceGroupManager::getSingleton().createResourceGroup("Cursor");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../Media/Cursor.zip","Zip","Cursor");
	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Cursor");

	mpGui = new GuiHandler(mpRenderWnd);

#ifdef DEBUG
	loadDebugResources();
	mpGui->initDebugGui();
#endif

	Settings::getSingletonPtr()->setLog(mpLog);

	mpTimer = new Ogre::Timer();
	mpTimer->reset();	

	mpSound = new SoundManager();

	return true;
}

void GameFramework::loadMenuResources()
{
	Ogre::ResourceGroupManager::getSingleton().createResourceGroup("Menu");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../Media/Menu.zip","Zip","Menu");
	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Menu");
}

void GameFramework::loadGameResources()
{
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../Media/Gui.zip","Zip","Game");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../Media/Material.zip","Zip","Game");

	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../Media/Meshes.zip","Zip","Game");

	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../Media/ParticleSystems.zip","Zip","Game");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../Media/Terrain.zip","Zip","Game");	
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../Media/Textures.zip","Zip","Game");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../Media/ActionBar.zip","Zip","Game");

	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Game");
}


#ifdef DEBUG
void GameFramework::loadDebugResources()
{
	Ogre::ResourceGroupManager::getSingleton().createResourceGroup("Debug");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../Media/Debug.zip","Zip","Debug");
	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Debug");
}
#endif

bool GameFramework::keyPressed(const OIS::KeyEvent &keyEventRef)
{
return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameFramework::keyReleased(const OIS::KeyEvent &keyEventRef)
{
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameFramework::mouseMoved(const OIS::MouseEvent &evt)
{
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameFramework::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameFramework::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameFramework::updateOgre(double timeSinceLastFrame)
{
#ifdef DEBUG
	const Ogre::RenderTarget::FrameStats& stats = mpRenderWnd->getStatistics(); 
	mpGui->updateDebugFPS(Ogre::StringConverter::toString(stats.lastFPS));
	mpGui->updateDebugTriangles(Ogre::StringConverter::toString(stats.triangleCount));
#endif
}
