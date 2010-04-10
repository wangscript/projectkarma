//|||||||||||||||||||||||||||||||||||||||||||||||

#include "GameFramework.h"

//|||||||||||||||||||||||||||||||||||||||||||||||

template<> GameFramework* Ogre::Singleton<GameFramework>::ms_Singleton = 0;

//|||||||||||||||||||||||||||||||||||||||||||||||

GameFramework::GameFramework()
{
	mtNumScreenShots	= 0;

	mpRoot				= 0;
	mpRenderWnd			= 0;
	mpViewport			= 0;
	mpLog				= 0;
	mpTimer				= 0;

	mpInputMgr			= 0;
	mpKeyboard			= 0;
	mpMouse				= 0;

	mtpDebugOverlay		= 0;
	mtpInfoOverlay		= 0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

GameFramework::~GameFramework()
{
	//BUG FIXA SENARE
	/*if(m_pInputMgr)
		
		OIS::InputManager::destroyInputSystem(m_pInputMgr);
		//OIS::InputManager::destroyInputSystem(m_pInputMgr);
		delete m_pKeyboard;
		delete m_pMouse;
*/

	mpSound->deleteSound();
	delete mpSound;	
	delete mpRoot;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameFramework::initOgre(Ogre::String wndTitle, OIS::KeyListener *pKeyListener, OIS::MouseListener *pMouseListener)
{
	Ogre::LogManager* logMgr = new Ogre::LogManager();
	
	mpLog = Ogre::LogManager::getSingleton().createLog("GameLogfile.log", true, true, false);
	mpLog->setDebugOutputEnabled(true);
	
	mpRoot = new Ogre::Root();

	mpRoot->showConfigDialog();
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
	
	Ogre::String secName, typeName, archName;
	Ogre::ConfigFile cf;
	cf.load("resources.cfg");

	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
        }
    }
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	mpTimer = new Ogre::Timer();
	mpTimer->reset();
	
	
	/*mtpDebugOverlay = Ogre::OverlayManager::getSingleton().getByName("Core/DebugOverlay");
	mtpDebugOverlay->show();*/

	mpRenderWnd->setActive(true);
	mpSound = new SoundManager();
	mpSound->initSound();

	mpGui = new GuiHandler();
	mpGui->initGui();
	mpGui->initDebugGui();
	mpGui->initIngameUI();
	mpGui->initCastingBar();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameFramework::keyPressed(const OIS::KeyEvent &keyEventRef)
{
	if(mpKeyboard->isKeyDown(OIS::KC_SYSRQ))
	{
		std::ostringstream ss;
		ss << "screenshot_" << ++mtNumScreenShots << ".png";
		mpRenderWnd->writeContentsToFile(ss.str());
		return true;
	}

	if(mpKeyboard->isKeyDown(OIS::KC_O))
	{
		if(mtpDebugOverlay)
		{
			if(!mtpDebugOverlay->isVisible())
				mtpDebugOverlay->show();
			else
				mtpDebugOverlay->hide();
		}
	}

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
	updateStats();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameFramework::updateStats() 
{ 
	const Ogre::RenderTarget::FrameStats& stats = mpRenderWnd->getStatistics(); 
	mpGui->updateDebugFPS(Ogre::StringConverter::toString(stats.lastFPS));
	mpGui->updateDebugTriangles(Ogre::StringConverter::toString(stats.triangleCount));

} 

//|||||||||||||||||||||||||||||||||||||||||||||||