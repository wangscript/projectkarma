/*---------------------------------------------------------------------------------*/
/* File: GameFramework.cpp														   */
/* Author: spacegaier http://www.ogre3d.org/wiki/index.php/Advanced_Ogre_Framework */
/* Modified by: Per Karlsson, perkarlsson89@gmail.com							   */
/*																				   */
/* Description:	It contains most of the Ogre related variables:					   */
/* - Root																		   */	
/* - RenderWindow																   */		
/* - Viewport																       */				
/* - Log																		   */
/* - Timer																		   */
/* - InputManager / Keyboard / Mouse											   */
/*																				   */		
/* It also offers functions to handle input, but that is almost not used here, as  */
/* each application state needs a different input behavior and therefore this is   */
/* directly modeled in the application states. However, everything common for the  */
/* whole application can be put here.											   */
/*---------------------------------------------------------------------------------*/

#include "GameFramework.h"
/*---------------------------------------------------------------------------------*/
//Singleton is initialized
template<> GameFramework* Ogre::Singleton<GameFramework>::ms_Singleton = 0;
/*---------------------------------------------------------------------------------*/
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
/*---------------------------------------------------------------------------------*/
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
/*---------------------------------------------------------------------------------*/
bool GameFramework::initOgre(Ogre::String wndTitle, OIS::KeyListener *pKeyListener, OIS::MouseListener *pMouseListener)
{
	//Create and initiate the Log
	Ogre::LogManager* logMgr = new Ogre::LogManager();
	mpLog = Ogre::LogManager::getSingleton().createLog("GameLogfile.log", true, true, false);
	mpLog->setDebugOutputEnabled(true);
	
	//Create an Ogre Root.
	mpRoot = new Ogre::Root();
	if (!mpRoot->showConfigDialog())
		return false;

	//Init Ogre Render Window
	mpRenderWnd = mpRoot->initialise(true, wndTitle);
	unsigned long hWnd = 0;
    OIS::ParamList paramList;
    mpRenderWnd->getCustomAttribute("WINDOW", &hWnd);
	paramList.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString(hWnd)));
	mpRenderWnd->setActive(true);

	//Add a viewport the to Render window
	mpViewport = mpRenderWnd->addViewport(0);
	mpViewport->setBackgroundColour(Ogre::ColourValue(0.8f, 0.8f, 0.8f, 1.0));
	mpViewport->setCamera(0);

	//Create Input System
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

	//Configure the amount of MipMaps per texture(to avoid aliasing).
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

	//Load cursor resources
	Ogre::ResourceGroupManager::getSingleton().createResourceGroup("Cursor");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../Media/Cursor.zip","Zip","Cursor");
	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Cursor");

	//Create the GUI handler
	mpGui = new GuiHandler(mpRenderWnd);

#ifdef DEBUG
	loadDebugResources();
	mpGui->initDebugGui();
#endif
	
	//Link the log to the Settings singleton
	Settings::getSingletonPtr()->setLog(mpLog);

	//Initiate the Timer.
	mpTimer = new Ogre::Timer();
	mpTimer->reset();	

	//Initiate the sound.
	mpSound = new SoundManager();

	return true;
}
/*---------------------------------------------------------------------------------*/
void GameFramework::loadMenuResources()
{
	Ogre::ResourceGroupManager::getSingleton().createResourceGroup("Menu");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../Media/Menu.zip","Zip","Menu");
	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Menu");
}
/*---------------------------------------------------------------------------------*/
void GameFramework::loadGameResources()
{
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../Media/Gui.zip","Zip","Game");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../Media/Material.zip","Zip","Game");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../Media/Meshes.zip","Zip","Game");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../Media/ParticleSystems.zip","Zip","Game");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../Media/Terrain.zip","Zip","Game");	
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../Media/Textures.zip","Zip","Game");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../Media/ActionBar.zip","Zip","Game");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../Media/Generated Terrain","FileSystem","Game");

	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Game");
}
/*---------------------------------------------------------------------------------*/
#ifdef DEBUG
void GameFramework::loadDebugResources()
{
	Ogre::ResourceGroupManager::getSingleton().createResourceGroup("Debug");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../Media/Debug.zip","Zip","Debug");
	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Debug");
}
#endif
/*---------------------------------------------------------------------------------*/
bool GameFramework::keyPressed(const OIS::KeyEvent &keyEventRef)
{
return true;
}
/*---------------------------------------------------------------------------------*/
bool GameFramework::keyReleased(const OIS::KeyEvent &keyEventRef)
{
	return true;
}
/*---------------------------------------------------------------------------------*/
bool GameFramework::mouseMoved(const OIS::MouseEvent &evt)
{
	return true;
}
/*---------------------------------------------------------------------------------*/
bool GameFramework::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	return true;
}
/*---------------------------------------------------------------------------------*/
bool GameFramework::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	return true;
}
/*---------------------------------------------------------------------------------*/
void GameFramework::updateOgre(double timeSinceLastFrame)
{
#ifdef DEBUG
	const Ogre::RenderTarget::FrameStats& stats = mpRenderWnd->getStatistics(); 
	mpGui->updateDebugFPS(Ogre::StringConverter::toString(stats.lastFPS));
	mpGui->updateDebugTriangles(Ogre::StringConverter::toString(stats.triangleCount));
#endif
}
/*---------------------------------------------------------------------------------*/