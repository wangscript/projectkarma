/*---------------------------------------------------------------------------------*/
/* File: GameState.cpp															   */
/* Author: Per Karlsson, perkarlsson89@gmail.com								   */
/*																				   */
/* Description:	http://www.ogre3d.org/wiki/index.php/Advanced_Ogre_Framework	   */
/* Based on the example in the Advanced Ogre Framework by spacegaier.			   */
/*---------------------------------------------------------------------------------*/

#include "GameState.h"

/*---------------------------------------------------------------------------------*/
void GameState::enter()
{
	//If the resource group "Game" isn't loaded yet, load it.
	if(!Ogre::ResourceGroupManager::getSingletonPtr()->resourceGroupExists("Game"))
		GameFramework::getSingletonPtr()->loadGameResources();
	
	//Hide the loading screen
	Ogre::OverlayManager::getSingletonPtr()->getByName("Loading")->hide();

	//Initiate Game Gui.
	GameFramework::getSingletonPtr()->mpGui->initGui();
	
	//Load all the game sounds.
	GameFramework::getSingletonPtr()->getSingletonPtr()->mpSound->initGameStateSound();

	//Load settings from the settings class.
	Settings::getSingletonPtr()->loadSettingsFromFile();

	GameFramework::getSingletonPtr()->mpLog->logMessage("Entering GameState...");

	//Create Scene Manager.
	mtpSceneMgr = GameFramework::getSingletonPtr()->mpRoot->createSceneManager(Ogre::ST_GENERIC, "GameSceneMgr");     

	//Create camera
	mtpCamera = mtpSceneMgr->createCamera("GameCamera");
	mtpCamera->setNearClipDistance(*Settings::getSingletonPtr()->mNearClipDistance);
	mtpCamera->setFarClipDistance(*Settings::getSingletonPtr()->mFarClipDistance);
	mtpCamera->setAspectRatio(Ogre::Real(GameFramework::getSingletonPtr()->mpViewport->getActualWidth()) / 
		Ogre::Real(GameFramework::getSingletonPtr()->mpViewport->getActualHeight()));
	GameFramework::getSingletonPtr()->mpViewport->setCamera(mtpCamera);
	mtpCamera->setAspectRatio(Ogre::Real(GameFramework::getSingletonPtr()->mpViewport->getActualWidth()) / 
		Ogre::Real(GameFramework::getSingletonPtr()->mpViewport->getActualHeight()));

	//Set input.
	GameFramework::getSingletonPtr()->mpViewport->setCamera(mtpCamera);
	GameFramework::getSingletonPtr()->mpKeyboard->setEventCallback(this);
	GameFramework::getSingletonPtr()->mpMouse->setEventCallback(this);
	mvRightButton = false;
	const OIS::MouseState state = GameFramework::getSingletonPtr()->mpMouse->getMouseState();
	
	mvQuit = false;

	//Create the scene and all contest inside it.
	createScene();

	//Start the ambient background sound.
	GameFramework::getSingletonPtr()->mpSound->playBackgroundSound();
	
	//Delete those settings we don't need anymore (only needed in createScene)
	Settings::getSingletonPtr()->deletePointers();
	
	//Seed for random in NPCAimer (not used 2010-06-04 )
	//std::srand ( time(NULL) );
}
/*---------------------------------------------------------------------------------*/
bool GameState::pause()
{
	GameFramework::getSingletonPtr()->mpLog->logMessage("Pausing GameState...");
	
	//Pause the physics simulation.
	mvpPhysicsMgr->pause();

	//Change to the normal cursor image.
	GameFramework::getSingletonPtr()->mpGui->changeCursor(Game::Cursor_Normal);

	//Hide the Game GUI.
	GameFramework::getSingletonPtr()->mpGui->toggleGameGUI(false);

	//Stop the ambient background sound.
	GameFramework::getSingletonPtr()->mpSound->stopBackgroundSound();
	return true;
}
/*---------------------------------------------------------------------------------*/
void GameState::resume()
{
	//Start the ambient background sound.
	GameFramework::getSingletonPtr()->mpSound->playBackgroundSound();
	GameFramework::getSingletonPtr()->mpLog->logMessage("Resuming GameState...");

	//Hide the loading screen.
	Ogre::OverlayManager::getSingletonPtr()->getByName("Loading")->hide();

	//Re-active the GameState camera.
	GameFramework::getSingletonPtr()->mpViewport->setCamera(mtpCamera);

	//Resume the Physics.
	mvpPhysicsMgr->resume();

	//Show the Game GUI
	GameFramework::getSingletonPtr()->mpGui->toggleGameGUI(true);

	//If the gun mode is active, change cursor to the Aim one.
	if ((mvpPlayer->getPowerUp() & Game::PowerUp_GunMode) != 0)
		GameFramework::getSingletonPtr()->mpGui->changeCursor(Game::Cursor_Aim);

	mvQuit = false;
}
/*---------------------------------------------------------------------------------*/
void GameState::exit()
{
	GameFramework::getSingletonPtr()->mpLog->logMessage("Leaving GameState...");
	
	//Stop the ambient background sound.
	GameFramework::getSingletonPtr()->mpSound->stopBackgroundSound();

	//Delete the physics
	mvpPhysicsMgr->destroyPhysicsWorld();

	//Delete the terrain
	delete mvpWorld;

	//Reset bullet holes
	BulletHoles::getSingletonPtr()->resetBulletHoles();

	//Delete all pointers in the chunk matrix.
	Chunks::getSingletonPtr()->destroyAll();

	//Remove GameState sounds from memory.
	GameFramework::getSingletonPtr()->getSingletonPtr()->mpSound->removeGameStateSounds();

	//Delete GameState camera and scene manager.
	mtpSceneMgr->destroyCamera(mtpCamera);
	if(mtpSceneMgr)
		GameFramework::getSingletonPtr()->mpRoot->destroySceneManager(mtpSceneMgr);
}
/*---------------------------------------------------------------------------------*/
void GameState::createScene()
{
	//Create physics
	mvpPhysicsMgr = new PhysicsManager(mtpSceneMgr);

	//Create motion blur (From samples example)
	createMotionBlurEffects();
	Ogre::CompositorManager::getSingleton().addCompositor(GameFramework::getSingletonPtr()->mpViewport, "Motion Blur");

	//Shadow Technique. Stencil shadows.
	//In the future : Fix soft shadows.
	mtpSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_MODULATIVE);
	
	//Add a skybox.
	mtpSceneMgr->setSkyBox(true, *Settings::getSingletonPtr()->mSkyBox,150);	

	//In the future: Add fog. (Already implemented but didn't look nice. Needs tweaking.)
	//mtpSceneMgr->setFog(FOG_LINEAR, ColourValue(*Settings::getSingletonPtr()->mFogColor,*Settings::getSingletonPtr()->mFogColor, *Settings::getSingletonPtr()->mFogColor), 0, *Settings::getSingletonPtr()->mFogStart, *Settings::getSingletonPtr()->mFogEnd);

	//Create a directional light
	Ogre::Vector3 lightdir(*Settings::getSingletonPtr()->mDirectionalLightDirX, *Settings::getSingletonPtr()->mDirectionalLightDirY, *Settings::getSingletonPtr()->mDirectionalLightDirZ);
	lightdir.normalise();
	mtpSceneMgr->setShadowFarDistance(*Settings::getSingletonPtr()->mShadowFarDistance);
	Ogre::Light* l = mtpSceneMgr->createLight("dirLight");
	l->setType(Ogre::Light::LT_DIRECTIONAL);
	l->setDirection(lightdir);
	l->setDiffuseColour(Ogre::ColourValue(0.97f, 0.90f, 0.70f));
	l->setSpecularColour(Ogre::ColourValue(0.2f, 0.2f, 0.2f));
	 
	//Ambient light
	mtpSceneMgr->setAmbientLight(Ogre::ColourValue(*Settings::getSingletonPtr()->mAmbientLight, *Settings::getSingletonPtr()->mAmbientLight,*Settings::getSingletonPtr()->mAmbientLight));

	//For screenshotting minimap
	mtpSceneMgr->getRootSceneNode()->createChildSceneNode("CamFree");
	
	//Create the grid in the Chunk class.
	Chunks::getSingletonPtr()->createGrid();
	
	//Init the World Manager class.
	mvpWorld = new WorldManager(mtpSceneMgr, mtpCamera,mvpPhysicsMgr->getPhysicsScene(),l);

	//Terrain texture layers.
	Ogre::String difSpecMap = "dirt_grayrocky_diffusespecular.dds";
	Ogre::String normHeightMap = "dirt_grayrocky_normalheight.dds";
	mvpWorld->addTextureLayer(difSpecMap, normHeightMap , *Settings::getSingletonPtr()->mTerrainTextureWorldSize);
	difSpecMap = "grass_green-01_diffusespecular.dds";
	normHeightMap = "grass_green-01_normalheight.dds";
	mvpWorld->addTextureLayer(difSpecMap,normHeightMap , *Settings::getSingletonPtr()->mTerrainTextureWorldSize);
	difSpecMap = "growth_weirdfungus-03_diffusespecular.dds";
	normHeightMap = "growth_weirdfungus-03_normalheight.dds";
	mvpWorld->addTextureLayer(difSpecMap, normHeightMap, *Settings::getSingletonPtr()->mTerrainTextureWorldSize);

	//Create the terrain in Ogre.
	mvpWorld->initTerrain();

	//Create the terrain in NxOgre.
	mvpWorld->buildNxOgreTerrain();

	//Create Player
	mvpPlayer = new Player( mtpSceneMgr, mvpPhysicsMgr->getPhysicsScene(),mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlender.mesh" ,"",
		Ogre::Vector3(*Settings::getSingletonPtr()->mPlayerSpawnPointX,*Settings::getSingletonPtr()->mPlayerSpawnPointY,*Settings::getSingletonPtr()->mPlayerSpawnPointZ),100,1);

	//Set the static pointer playerNode in the NPC class.
	NPC::setPlayerNode(mvpPlayer->getPlayerNode());

	//Link to the physics scene in the Aimer class.
	Aimer::setPhysicsScene(mvpPhysicsMgr->getPhysicsScene());

	//Set scene manager pointer in the bullet holes class.
	BulletHoles::getSingletonPtr()->setSceneManager(mtpSceneMgr);

	//Create camera
	CameraHandler::getSingletonPtr()->initCameraHandler(mtpCamera,mvpPhysicsMgr->getPhysicsRenderSystem(), mtpSceneMgr, mvpPlayer);
	
	//Create PowerUp class. 
	mvpPowerUps = new PowerUp(mvpPhysicsMgr->getPhysicsScene(), mtpSceneMgr,mvpPlayer->getHitBox(),mvpPlayer);

	//Add power ups.
	//In the future: Read info from text file or picture.
	mvpPowerUps->addPowerUp(Ogre::Vector3(-202,10,202),"BarrelSuperJump");
	mvpPowerUps->addPowerUp(Ogre::Vector3(-201,11.6,160),"BarrelSuperSpeed");
	mvpPowerUps->addPowerUp(Ogre::Vector3(-168,23,-94),"BarrelMoveBox");
	mvpPowerUps->addPowerUp(Ogre::Vector3(-161,34,-170),"BarrelPistol");
	mvpPowerUps->addPowerUp(Ogre::Vector3(70,53,82),"BarrelMachineGun");
	mvpPowerUps->addPowerUp(Ogre::Vector3(205,25,-38),"BarrelRocketBoots");

	//Add dynamic boxes.
	//In the future: Read info from text file or picture.
	for (int i=0; i<2; i++)
	{
		for (int j=0; j<2; j++)
		{
			for (int k=0; k<3; k++)
				mvpPhysicsMgr->getPhysicsRenderSystem()->createBody(new NxOgre::Box(1, 1, 1), NxOgre::Vec3(-162 + (k+0.1), 34+i, -166 +(j+0.1)), "cube.1m.mesh");
		}
	}
	mvpPhysicsMgr->getPhysicsRenderSystem()->createBody(new NxOgre::Box(1, 1, 1), NxOgre::Vec3(116,26,50), "cube.1m.mesh");
	mvpPhysicsMgr->getPhysicsRenderSystem()->createBody(new NxOgre::Box(1, 1, 1), NxOgre::Vec3(116.5,26,52), "cube.1m.mesh");
	mvpPhysicsMgr->getPhysicsRenderSystem()->createBody(new NxOgre::Box(1, 1, 1), NxOgre::Vec3(115,26.5,47.2), "cube.1m.mesh");
	mvpPhysicsMgr->getPhysicsRenderSystem()->createBody(new NxOgre::Box(1, 1, 1), NxOgre::Vec3(113.9,26,45.2), "cube.1m.mesh");
	
	//Add NPC Aimers to the world.
	//In the future: Read info from text file or picture.
	new NPCAimer ( mtpSceneMgr,  mvpPhysicsMgr->getPhysicsScene(),mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy1", Ogre::Vector3(-127,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(-127,100,160),160),0.2,100,3);
	new NPCAimer ( mtpSceneMgr, mvpPhysicsMgr->getPhysicsScene(),mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy2", Ogre::Vector3(-122,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(-122,100,14),14),0.2,100,3);
	new NPCAimer ( mtpSceneMgr, mvpPhysicsMgr->getPhysicsScene(),mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy3", Ogre::Vector3(-46,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(-46,100,-146),-146),0.2,100,3);
	new NPCAimer ( mtpSceneMgr, mvpPhysicsMgr->getPhysicsScene(),mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy4", Ogre::Vector3(33,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(33,100,-112),-112),0.2,100,3);
	new NPCAimer ( mtpSceneMgr, mvpPhysicsMgr->getPhysicsScene(),mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy5", Ogre::Vector3(88,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(88,100,-183),-183),0.2,100,3);
	new NPCAimer ( mtpSceneMgr,mvpPhysicsMgr->getPhysicsScene(), mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy6", Ogre::Vector3(148,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(148,100,-206),-206),0.2,100,3);
	new NPCAimer ( mtpSceneMgr,mvpPhysicsMgr->getPhysicsScene(), mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy7", Ogre::Vector3(142,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(142,100,-88),-88),0.2,100,3);
	new NPCAimer ( mtpSceneMgr,mvpPhysicsMgr->getPhysicsScene(), mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy8", Ogre::Vector3(91,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(91,100,-54),-54),0.2,100,3);
	new NPCAimer ( mtpSceneMgr,mvpPhysicsMgr->getPhysicsScene(), mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy9", Ogre::Vector3(20,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(20,100,3),3),0.2,100,3);
	new NPCAimer ( mtpSceneMgr,mvpPhysicsMgr->getPhysicsScene(), mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy10", Ogre::Vector3(205,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(205,100,-35.6),-35.6),0.2,100,3);
	new NPCAimer ( mtpSceneMgr,mvpPhysicsMgr->getPhysicsScene(), mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy11", Ogre::Vector3(209,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(209,100,-34),-34),0.2,100,3);
	new NPCAimer ( mtpSceneMgr,mvpPhysicsMgr->getPhysicsScene(), mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy12", Ogre::Vector3(206,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(206,100,-49),-49),0.2,100,3);
	new NPCAimer ( mtpSceneMgr,mvpPhysicsMgr->getPhysicsScene(), mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy13", Ogre::Vector3(203,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(203,100,-118),118),0.2,100,3);
	new NPCAimer ( mtpSceneMgr,mvpPhysicsMgr->getPhysicsScene(), mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy14", Ogre::Vector3(178,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(178,100,-176),176),0.2,100,3);
	new NPCAimer ( mtpSceneMgr,mvpPhysicsMgr->getPhysicsScene(), mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy15", Ogre::Vector3(108,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(108,100,-193),193),0.2,100,3);
	new NPCAimer ( mtpSceneMgr,mvpPhysicsMgr->getPhysicsScene(), mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy16", Ogre::Vector3(129,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(129,3,-58),-58),0.2,100,3);
	new NPCAimer ( mtpSceneMgr,mvpPhysicsMgr->getPhysicsScene(), mvpPhysicsMgr->getPhysicsRenderSystem(),"HaloBlenderNPC.mesh" , "Enemy17", Ogre::Vector3(120,mvpWorld->getTerrainAt(0,0)->getHeightAtWorldPosition(120,3,-12),-12),0.2,100,3);

	//Load the rest of the world objects via the sceneloader.
	SceneLoader* sceneLoad = new SceneLoader();
	sceneLoad->parseLevel("scen.scene","Game",mtpSceneMgr,mtpSceneMgr->getRootSceneNode(),mvpPhysicsMgr,mvpWorld->getTerrainAt(0,0));
}
/*---------------------------------------------------------------------------------*/
bool GameState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
	//If escape is pushed, quit the game state.
	if(GameFramework::getSingletonPtr()->mpKeyboard->isKeyDown(OIS::KC_ESCAPE))
	{
		mvQuit = true;
		return true;
	}

	switch (keyEventRef.key)		
	{
	//If R is pressed, reset Powerups, change camera mode to Third Person and set cursor to normal.
	case OIS::KC_R:
		mvpPlayer->setPowerUp(Game::PowerUp_None);
		CameraHandler::getSingletonPtr()->setCamMode(Game::Cam_ThirdPerson);
		GameFramework::getSingletonPtr()->mpGui->changeCursor(Game::Cursor_Normal);
		break;
	//If "1" is pressed and the SuperJump button on the action bar is loaded, set PowerUp to Superjump.
	case OIS::KC_1:
		if (GameFramework::getSingletonPtr()->mpGui->isLoaded(Game::PowerUp_SuperJump))
			mvpPlayer->setPowerUp(Game::PowerUp_SuperJump);
		break;
	//If "2" is pressed and the SuperSpeed button on the action bar is loaded, set PowerUp to SuperSpeed.
	case OIS::KC_2:
		if (GameFramework::getSingletonPtr()->mpGui->isLoaded(Game::PowerUp_SuperSpeed))
			mvpPlayer->setPowerUp(Game::PowerUp_SuperSpeed,3.0f);
		break;
	//If "3" is pressed and the MoveBox button on the action bar is loaded, set PowerUp to Move Box and chang to Move Box Cursor.
	//Set camera mode to Semi-Third Person with mouse movable.
	case OIS::KC_3:
		if (GameFramework::getSingletonPtr()->mpGui->isLoaded(Game::PowerUp_MoveBoxMode))
		{
			mvpPlayer->setPowerUp(Game::PowerUp_MoveBoxMode);
			GameFramework::getSingletonPtr()->mpGui->changeCursor(Game::Cursor_MoveBox);
			CameraHandler::getSingletonPtr()->setCamMode(Game::Cam_MixCursorMovable);
		}
		break;
	//If "4" is pressed and the GunMode Pistol button on the action bar is loaded, set PowerUp to GunMode Pistol.
	//Change cursor to the aim cursor.
	//Set camera mode to Semi-Third Person with mouse movable.
	case OIS::KC_4:
		if (GameFramework::getSingletonPtr()->mpGui->isLoaded(Game::PowerUp_GunModePistol))
		{
			mvpPlayer->setPowerUp(Game::PowerUp_GunMode);
			GameFramework::getSingletonPtr()->mpGui->changeCursor(Game::Cursor_Aim);
			mvpPlayer->setPowerUp(Game::PowerUp_GunModePistol);
			CameraHandler::getSingletonPtr()->setCamMode(Game::Cam_MixCursorMovable);
		}
		break;
	//If "Z" is pressed and the GunMode Pistol button on the action bar is loaded, set PowerUp to GunMode Pistol.
	//Change cursor to the aim cursor.
	//Set camera mode to Semi-Third Person with mouse locked.
	case OIS::KC_Z:
		if (GameFramework::getSingletonPtr()->mpGui->isLoaded(Game::PowerUp_GunModePistol))
		{
			mvpPlayer->setPowerUp(Game::PowerUp_GunMode);
			GameFramework::getSingletonPtr()->mpGui->changeCursor(Game::Cursor_Aim);
			mvpPlayer->setPowerUp(Game::PowerUp_GunModePistol);
			CameraHandler::getSingletonPtr()->setCamMode(Game::Cam_MixCursorCenter);
		}
		break;
	//If "5" is pressed and the GunMode MachineGun button on the action bar is loaded, set PowerUp to GunMode MachineGun.
	//Change cursor to the aim cursor.
	//Set camera mode to First Person.
	case OIS::KC_5:
		if (GameFramework::getSingletonPtr()->mpGui->isLoaded(Game::PowerUp_GunModeMachineGun))
		{
			mvpPlayer->setPowerUp(Game::PowerUp_GunMode);
			GameFramework::getSingletonPtr()->mpGui->changeCursor(Game::Cursor_Aim);
			mvpPlayer->setPowerUp(Game::PowerUp_GunModeMachineGun);
			CameraHandler::getSingletonPtr()->setCamMode(Game::Cam_FirstPerson);
		}
		break;
	//If "6" is pressed and the RocketBoots button on the action bar is loaded, set PowerUp to RocketBoots.
	case OIS::KC_6:
		if (GameFramework::getSingletonPtr()->mpGui->isLoaded(Game::PowerUp_RocketBootsMode))
		{
			mvpPlayer->setPowerUp(Game::PowerUp_RocketBootsMode);
			GameFramework::getSingletonPtr()->mpGui->loadCastingBar(Game::PowerUp_RocketBootsMode,1.5f);
			GameFramework::getSingletonPtr()->mpGui->updateCastingBar(1.5f);
		}
		break;
	//If "Space" is pressed, either start the rocket boots (sound and particle effects) or do a regular jump. 
	//Depends on which PowerUp that is active.
	case OIS::KC_SPACE:
		if ((mvpPlayer->getPowerUp() & Game::PowerUp_RocketBootsMode) != 0)
			mvpPlayer->startRocketBoots();
		else if ((mvpPlayer->isJumping())==false)
			mvpPlayer->jump();
		break;
	}		

	return true;
}
/*---------------------------------------------------------------------------------*/
bool GameState::keyReleased(const OIS::KeyEvent &keyEventRef)
{
	//Sends info to the GameFramework. For general events.
	GameFramework::getSingletonPtr()->keyReleased(keyEventRef);


	switch (keyEventRef.key)		
	{
	//If any of the W A D S or UP LEFT RIGHT DOWN is released, the character stops moving.
	case OIS::KC_UP:
	case OIS::KC_W:
		mvpPlayer->setMoveDirection(Game::Move_None);
		break;
	case OIS::KC_LEFT:
	case OIS::KC_A:
		mvpPlayer->setMoveDirection(Game::Move_None);
		break;
	case OIS::KC_RIGHT:
	case OIS::KC_D:
		mvpPlayer->setMoveDirection(Game::Move_None);
		break;
	case OIS::KC_DOWN:
	case OIS::KC_S:
		mvpPlayer->setMoveDirection(Game::Move_None);
		break;
	//If Rocket Boots are active and the space bar is released, the Rocket Boots are resetted.
	case OIS::KC_SPACE:
		if ((mvpPlayer->getPowerUp() & Game::PowerUp_RocketBootsMode) != 0)
			mvpPlayer->resetRocketBoots();
		break;
	}
	return true;
}
/*---------------------------------------------------------------------------------*/
bool GameState::mouseMoved(const OIS::MouseEvent &evt)
{
	//If there has been any movement on the mouse wheel, zoom!
	if (evt.state.Z.rel != 0)
		CameraHandler::getSingletonPtr()->Zoom(evt);

	//If the MoveBox mode is active, call the MoveBox function inside the player class.
	if ((mvpPlayer->getPowerUp() & Game::PowerUp_MoveBoxMode) != 0)
		mvpPlayer->moveBoxMoved(evt);
	
	//Rotate the camera. Different for every camera mode, will be taken care of inside the CameraHandler class.
	CameraHandler::getSingletonPtr()->Rotate(evt);

	return true;
}
/*---------------------------------------------------------------------------------*/
bool GameState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	//If the left mouse button is pressed.
	if(id == OIS::MB_Left)
	{
		//If MoveBox PowerUp is active, call the moveBoxPressed function in the Player class.
		if ((mvpPlayer->getPowerUp() & Game::PowerUp_MoveBoxMode) != 0)
			mvpPlayer->moveBoxPressed(evt);
		//If the GunMode PowerUp is active, active continious fire inside the Player class.
		if ((mvpPlayer->getPowerUp() & Game::PowerUp_GunMode) != 0)
			mvpPlayer->toggleContiniousFire(true);
	} 

	//Tells the third person view that the right mouse button is pressed.
	if (id==OIS::MB_Right)
		mvRightButton = true;
	return true;
}
/*---------------------------------------------------------------------------------*/
bool GameState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	if(id == OIS::MB_Left)
	{
		//Deactivates continious fire the Player class.
		mvpPlayer->toggleContiniousFire(false);

		//If MoveBox mode is active, release the current Box.
		if ((mvpPlayer->getPowerUp() & Game::PowerUp_MoveBoxMode) != 0)
			mvpPlayer->releaseBox();
	}
	
	//Tells the third person view that the right mouse button is not pressed anymore.	
	if (id==OIS::MB_Right)
	{
		mvRightButton = false;
	}
	return true;
}
/*---------------------------------------------------------------------------------*/
void GameState::getInput()
{
	//Tells the Player class what direction you want to go.
	if(GameFramework::getSingletonPtr()->mpKeyboard->isKeyDown(OIS::KC_W))
		mvpPlayer->setMoveDirection(Game::Move_Forward);
	if(GameFramework::getSingletonPtr()->mpKeyboard->isKeyDown(OIS::KC_S))
		mvpPlayer->setMoveDirection(Game::Move_Backward);
	if(GameFramework::getSingletonPtr()->mpKeyboard->isKeyDown(OIS::KC_A))
		mvpPlayer->setMoveDirection(Game::Move_StrafeLeft);
	if(GameFramework::getSingletonPtr()->mpKeyboard->isKeyDown(OIS::KC_D))
		mvpPlayer->setMoveDirection(Game::Move_StrafeLeft);

	//If Rocket Boots PowerUp is active and the space bar is down,  the Rocket Boots functions should be called 
	//somewhere in the game loop.
	if(GameFramework::getSingletonPtr()->mpKeyboard->isKeyDown(OIS::KC_SPACE))
	{
		if ((mvpPlayer->getPowerUp() & Game::PowerUp_RocketBootsMode) != 0)
			mvpPlayer->queueRocketBoots();
	}
}
/*---------------------------------------------------------------------------------*/
void GameState::update(double timeSinceLastFrame)
{
	//If mvQuit is true, then enter Menu State.
	if(mvQuit == true)
	{
		this->pushAppState(findByName("MenuState"));
		return;
	}

	//Get the input for every frame.
	getInput();

	//The physics based camera system tries to mimic the non physics based camera system.
	CameraHandler::getSingletonPtr()->AdjustCamera();
	
	//Update all effects.
	Effects::updateAll(timeSinceLastFrame);

	//If the camera mode is Third Person, we might not want to rotate the Player to face the viewing direction.
	if (CameraHandler::getSingletonPtr()->getCamMode()==Game::Cam_ThirdPerson)
		mvpPlayer->setNeedUpdateRotation(mvRightButton);
	else
		mvpPlayer->setNeedUpdateRotation(true);

	//If the Player isn't dead, move.
	if (!mvpPlayer->isDead())		
		mvpPlayer->move(timeSinceLastFrame);
	//If the player is dead, update the dead timer.
	else
		mvpPlayer->updateDead(timeSinceLastFrame);

	//Updates all NPCs (and effects) in the active Chunk area.
	Chunks::getSingletonPtr()->loopCurrentChunksUpdate(timeSinceLastFrame);

	//Update the currently resetting and the dead NPCs.
	NPCHandler::getSingletonPtr()->updateResetNPCs(timeSinceLastFrame);
	NPCHandler::getSingletonPtr()->updateDeadNPCs(timeSinceLastFrame);

	//If the casting bar is active (and not in Rocket Boots mode), update the casting bar.
	if (mvpPlayer->getCastingBarTimer() && (mvpPlayer->getPowerUp() & Game::PowerUp_RocketBootsMode) == 0)
		mvpPlayer->updateCastingBar(timeSinceLastFrame);

	//Update the physics simulation.
	mvpPhysicsMgr->update(timeSinceLastFrame);

	//Sync the player position with the bound capsules position.
	mvpPlayer->updatePosition();

	//If in Third Person view, move Camera to the same position as the bounding sphere.
	if (CameraHandler::getSingletonPtr()->getCamMode() == Game::Cam_ThirdPerson)
		CameraHandler::getSingletonPtr()->MoveCamera();
}
/*---------------------------------------------------------------------------------*/
void GameState::createMotionBlurEffects()
{
	//Creates motion blur! From the Ogre samples.

	Ogre::CompositorPtr comp3 = Ogre::CompositorManager::getSingleton().create(
		"Motion Blur", "Game");
	{
		Ogre::CompositionTechnique *t = comp3->createTechnique();
		{
			Ogre::CompositionTechnique::TextureDefinition *def = t->createTextureDefinition("scene");
			def->width = 0;
			def->height = 0;
			def->formatList.push_back(Ogre::PF_R8G8B8);
		}
		{
			Ogre::CompositionTechnique::TextureDefinition *def = t->createTextureDefinition("sum");
			def->width = 0;
			def->height = 0;
			def->formatList.push_back(Ogre::PF_R8G8B8);
		}
		{
			Ogre::CompositionTechnique::TextureDefinition *def = t->createTextureDefinition("temp");
			def->width = 0;
			def->height = 0;
			def->formatList.push_back(Ogre::PF_R8G8B8);
		}
		/// Render scene
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_PREVIOUS);
			tp->setOutputName("scene");
		}
		/// Initialisation pass for sum texture
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_PREVIOUS);
			tp->setOutputName("sum");
			tp->setOnlyInitial(true);
		}
		/// Do the motion blur
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
			tp->setOutputName("temp");
			{ Ogre::CompositionPass *pass = tp->createPass();
			pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
			pass->setMaterialName("Ogre/Compositor/Combine");
			pass->setInput(0, "scene");
			pass->setInput(1, "sum");
			}
		}
		/// Copy back sum texture
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
			tp->setOutputName("sum");
			{ Ogre::CompositionPass *pass = tp->createPass();
			pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
			pass->setMaterialName("Ogre/Compositor/Copyback");
			pass->setInput(0, "temp");
			}
		}
		/// Display result
		{
			Ogre::CompositionTargetPass *tp = t->getOutputTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
			{ Ogre::CompositionPass *pass = tp->createPass();
			pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
			pass->setMaterialName("Ogre/Compositor/MotionBlur");
			pass->setInput(0, "sum");
			}
		}
	}
}
/*---------------------------------------------------------------------------------*/