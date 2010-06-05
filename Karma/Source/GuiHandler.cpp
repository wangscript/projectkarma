#include <GuiHandler.h>

GuiHandler::GuiHandler(Ogre::RenderWindow* renderWnd) :mvpRenderWnd(renderWnd), mvFirstPerson(false),mvCastBar(false)
{
	mtpCursorLayer = Ogre::OverlayManager::getSingleton().getByName("CursorMain");
	mtpCursorNormal = Ogre::OverlayManager::getSingleton().getOverlayElement("CursorNormal");
	mtpCursorMoveBox = Ogre::OverlayManager::getSingleton().getOverlayElement("CursorMoveBox");
	mtpCursorMoveBox->hide();
	mtpCursorMoveBoxGrab = Ogre::OverlayManager::getSingleton().getOverlayElement("CursorMoveBoxGrab");
	mtpCursorMoveBoxGrab->hide();
	mtpCursorAim = Ogre::OverlayManager::getSingleton().getOverlayElement("CursorAim");
	mtpCursorAim->hide();

	mtpCurCursor = mtpCursorNormal;

	showCursor();
}


void GuiHandler::firstPerson(bool state)
{
	mvFirstPerson = state;
	if (mvFirstPerson)
	{
		mtpFirstPersonLayer->show();
		updateCursorPos(mvpRenderWnd->getWidth()/2,mvpRenderWnd->getHeight()/2);
	}
	else
		mtpFirstPersonLayer->hide();
}

void GuiHandler::changeCursor(int cursor)
{
	mtpCurCursor->hide();
	switch (cursor)
	{
	case Game::Cursor_Normal:
		mtpCurCursor = mtpCursorNormal;
		break;
	case Game::Cursor_MoveBox:
		mtpCurCursor = mtpCursorMoveBox;
		break;
	case Game::Cursor_MoveBoxGrab:
		mtpCurCursor = mtpCursorMoveBoxGrab;
		break;
	case Game::Cursor_Aim:
		mtpCurCursor = mtpCursorAim;
	}
	mtpCurCursor->show();
	mtpCurCursor->setPosition(mvMouseX,mvMouseY);
}


void GuiHandler::initGui()
{
	initActionBar();
	initCastingBar();
	initMiniMap();
	initFirstPersonShooter();
	initHP();
}

void GuiHandler::toggleGameGUI(bool state)
{
	if (state)
	{
		mtpMiniMapLayer->show();
		if (mvCastBar)
			mtpCastingBarLayer->show();
		mvActionBar->show();
		mtpHPLayer->show();
		if (mvFirstPerson)
			firstPerson(mvFirstPerson);
	}
	else
	{
		mtpMiniMapLayer->hide();
		mtpCastingBarLayer->hide();
		mvActionBar->hide();
		mtpHPLayer->hide();
		if (mvFirstPerson)
			mtpFirstPersonLayer->hide();
	}
}

void GuiHandler::initHP()
{
	mtpHPLayer = Ogre::OverlayManager::getSingleton().getByName("GuiKarma/HP");
	mtpHPLayer->show();	
	mtpHPText = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("GuiKarma/HPText");
}

void GuiHandler::updateHP(float hp)
{
	mtpHPText->setCaption(Ogre::StringConverter::toString(hp));
}


void GuiHandler::initActionBar()
{
	mvActionBar = Ogre::OverlayManager::getSingletonPtr()->getByName("ActionBarOverlay");
	mvActionBar->show();
	//Load all elements and only show the locked ones (initial state)
	mvActionBarSuperJump.PowerUp = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupSuperJump");
	mvActionBarSuperJump.PowerUpLocked = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupSuperJumpLocked");
	mvActionBarSuperJump.Active = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupSuperJumpActive");
	changeActionBarElement(mvActionBarSuperJump,Game::ActionBar_Locked);

	mvActionBarSuperSpeed.PowerUp = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupSuperSpeed");
	mvActionBarSuperSpeed.PowerUpLocked = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupSuperSpeedLocked");
	mvActionBarSuperSpeed.Active = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupSuperSpeedActive");
	changeActionBarElement(mvActionBarSuperSpeed,Game::ActionBar_Locked);

	mvActionBarMoveBox.PowerUp = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupMoveBox");
	mvActionBarMoveBox.PowerUpLocked = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupMoveBoxLocked");
	mvActionBarMoveBox.Active = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupMoveBoxActive");
	changeActionBarElement(mvActionBarMoveBox,Game::ActionBar_Locked);

	mvActionBarPistol.PowerUp = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupPistol");
	mvActionBarPistol.PowerUpLocked = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupPistolLocked");
	mvActionBarPistol.Active = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupPistolActive");
	changeActionBarElement(mvActionBarPistol,Game::ActionBar_Locked);

	mvActionBarMachineGun.PowerUp = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupMachineGun");
	mvActionBarMachineGun.PowerUpLocked = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupMachineGunLocked");
	mvActionBarMachineGun.Active = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupMachineGunActive");
	changeActionBarElement(mvActionBarMachineGun,Game::ActionBar_Locked);

	mvActionBarRocketBoots.PowerUp = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupRocketBoots");
	mvActionBarRocketBoots.PowerUpLocked = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupRocketBootsLocked");
	mvActionBarRocketBoots.Active = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupRocketBootsActive");
	changeActionBarElement(mvActionBarRocketBoots,Game::ActionBar_Locked);
}

bool GuiHandler::isLoaded(int powerUp)
{
switch (powerUp)		
	{
	case Game::PowerUp_SuperJump:
		return mvActionBarSuperJump.PowerUp->isVisible();
		break;
	case Game::PowerUp_SuperSpeed:
		return mvActionBarSuperSpeed.PowerUp->isVisible();
		break;
	case Game::PowerUp_MoveBoxMode:
		return mvActionBarMoveBox.PowerUp->isVisible();
		break;
	case Game::PowerUp_GunModePistol:
		return mvActionBarPistol.PowerUp->isVisible();
		break;
	case Game::PowerUp_GunModeMachineGun:
		return mvActionBarMachineGun.PowerUp->isVisible();
		break;
	case Game::PowerUp_RocketBootsMode:
		return mvActionBarRocketBoots.PowerUp->isVisible();
	};	
return false;
}

void GuiHandler::updateActionBarElement(int powerUp,int ActionBarMode)
{
	switch (powerUp)		
	{
	case Game::PowerUp_SuperJump:
		changeActionBarElement(mvActionBarSuperJump,ActionBarMode);
		if (ActionBarMode==Game::ActionBar_Active && isLoaded(Game::PowerUp_RocketBootsMode))
			changeActionBarElement(mvActionBarRocketBoots,Game::ActionBar_Normal);
		break;

	case Game::PowerUp_SuperSpeed:
		changeActionBarElement(mvActionBarSuperSpeed,ActionBarMode);
		if (ActionBarMode==Game::ActionBar_Active && isLoaded(Game::PowerUp_RocketBootsMode))
			changeActionBarElement(mvActionBarRocketBoots,Game::ActionBar_Normal);
		if (ActionBarMode==Game::ActionBar_Active && isLoaded(Game::PowerUp_MoveBoxMode))
			changeActionBarElement(mvActionBarMoveBox,Game::ActionBar_Normal);
		break;

	case Game::PowerUp_MoveBoxMode:
		changeActionBarElement(mvActionBarMoveBox,ActionBarMode);
		if (ActionBarMode==Game::ActionBar_Active && isLoaded(Game::PowerUp_GunModePistol))
			changeActionBarElement(mvActionBarPistol,Game::ActionBar_Normal);
		if (ActionBarMode==Game::ActionBar_Active && isLoaded(Game::PowerUp_GunModeMachineGun))
			changeActionBarElement(mvActionBarMachineGun,Game::ActionBar_Normal);
		if (ActionBarMode==Game::ActionBar_Active && isLoaded(Game::PowerUp_SuperSpeed))
			changeActionBarElement(mvActionBarSuperSpeed,Game::ActionBar_Normal);
		if (ActionBarMode==Game::ActionBar_Active && isLoaded(Game::PowerUp_RocketBootsMode))
			changeActionBarElement(mvActionBarRocketBoots,Game::ActionBar_Normal);
		break;

	case Game::PowerUp_GunModePistol:
		changeActionBarElement(mvActionBarPistol,ActionBarMode);
		if (ActionBarMode==Game::ActionBar_Active && isLoaded(Game::PowerUp_MoveBoxMode))
			changeActionBarElement(mvActionBarMoveBox,Game::ActionBar_Normal);
		if (ActionBarMode==Game::ActionBar_Active && isLoaded(Game::PowerUp_GunModeMachineGun))
			changeActionBarElement(mvActionBarMachineGun,Game::ActionBar_Normal);
		break;

	case Game::PowerUp_GunModeMachineGun:
		changeActionBarElement(mvActionBarMachineGun,ActionBarMode);			
		if (ActionBarMode==Game::ActionBar_Active && isLoaded(Game::PowerUp_MoveBoxMode))
			changeActionBarElement(mvActionBarMoveBox,Game::ActionBar_Normal);
		if (ActionBarMode==Game::ActionBar_Active && isLoaded(Game::PowerUp_GunModePistol))
			changeActionBarElement(mvActionBarPistol,Game::ActionBar_Normal);
		break;

	case Game::PowerUp_RocketBootsMode:
		changeActionBarElement(mvActionBarRocketBoots,ActionBarMode);
		if (ActionBarMode==Game::ActionBar_Active && isLoaded(Game::PowerUp_SuperJump))
			changeActionBarElement(mvActionBarSuperJump,Game::ActionBar_Normal);
		if (ActionBarMode==Game::ActionBar_Active && isLoaded(Game::PowerUp_SuperSpeed))
			changeActionBarElement(mvActionBarSuperSpeed,Game::ActionBar_Normal);
		if (ActionBarMode==Game::ActionBar_Active && isLoaded(Game::PowerUp_MoveBoxMode))
			changeActionBarElement(mvActionBarMoveBox,Game::ActionBar_Normal);
		break;
	};	
}

void GuiHandler::changeActionBarElement(ActionBarElements& ae, int ActionBarMode)
{
	if (ActionBarMode == Game::ActionBar_Normal)
	{
		ae.Active->hide();
		ae.PowerUpLocked->hide();
		ae.PowerUp->show();
	}
	else if (ActionBarMode == Game::ActionBar_Active)
	{
		ae.Active->show();
		ae.PowerUpLocked->hide();
		ae.PowerUp->show();
	}
	else
	{
		ae.Active->hide();
		ae.PowerUpLocked->show();
		ae.PowerUp->hide();
	}
}

void GuiHandler::initCastingBar()
{
	mtpCastingBarLayer = Ogre::OverlayManager::getSingleton().getByName("GuiKarma/CastingBar");
	mtpCastingBar =	Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/CastingbarPanel/Texture"); 
	mtpCastingBarCur =Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/CastingBarPanel/TextureCurrent"); 
	mtpCastingBarText =	Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/CastingbarPanel/Text");
	mtpCastingBarTextTime = Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/CastingbarPanel/TextTime");

	mtpCastingBarSuperSpeed = Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/CastingbarPanel/SuperSpeed");
	mtpCastingBarSuperSpeed->hide();
	mtpCastingBarMoveBox = Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/CastingbarPanel/MoveBox");
	mtpCastingBarMoveBox->hide();
	mtpCastingBarRocketBoots = Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/CastingbarPanel/RocketBoots");
	mtpCastingBarRocketBoots->hide();


	mtpCurrCastingBarIcon = 0;


}

void GuiHandler::loadCastingBar(int powerUp , float totalTime)
{
	Ogre::String name;
	switch(powerUp)
	{
	case Game::PowerUp_SuperSpeed:
		mtpCurrCastingBarIcon = mtpCastingBarSuperSpeed;
		name = "Super Speed";
		break;
	case Game::PowerUp_MoveBox:
		mtpCurrCastingBarIcon = mtpCastingBarMoveBox;
		name = "Move Box";
		break;
	case Game::PowerUp_RocketBootsMode:
		mtpCurrCastingBarIcon = mtpCastingBarRocketBoots;
		name = "Rocket Boots";
	}
	mvCastBar = true;
	mtpCastingBarCur->setLeft(24+243);
	mtpCastingBarLayer->show();
	mtpCurrCastingBarIcon->show();
	mtCastingBarTotalTime = totalTime;
	mtpCastingBarText->setCaption(name);
}

void GuiHandler::updateCastingBar(float curTime)
{
	float relWidth = curTime/mtCastingBarTotalTime;
	int precision1 = 2;
	if (curTime<1)
		precision1 = 1;
	if (curTime < 0.1)
	{
		curTime = 0;
		mvCastBar = false;
	}
	static Ogre::String text("/");
	if (relWidth <= 1)
	{
		mtpCastingBarTextTime->setCaption(Ogre::StringConverter::toString(curTime,precision1)+text+Ogre::StringConverter::toString(mtCastingBarTotalTime,3));
		//243 = castingbar width
		mtpCastingBar->setWidth(243 * relWidth);
		mtpCastingBarCur->setLeft(24 + 243 * relWidth);
	}
}

void GuiHandler::hideCastingBar()
{
	if (mtpCurrCastingBarIcon)
	{
		mtpCurrCastingBarIcon->hide();
		mtpCurrCastingBarIcon = 0;
	}
	mvCastBar = false;
	mtpCastingBarLayer->hide();
}
void GuiHandler::showCursor()
{
	mtpCursorLayer->show();
}

void GuiHandler::hideCursor()
{
	mtpCursorLayer->hide();
}

void GuiHandler::updateCursorPos(const int x,const int y)
{
	mvMouseX = x-16;
	mvMouseY = y-16;
	mtpCurCursor->setPosition(mvMouseX,mvMouseY);
#ifdef DEBUG
	//updateDebugCursorPos(x,y);
#endif
}

void GuiHandler::initMiniMap()
{
	mtpMiniMapLayer = Ogre::OverlayManager::getSingleton().getByName("GuiKarma/MiniMap");
	mtpMiniMapLayer->show();
	//mtpCursorLayer->setZOrder(400);
	mtpMiniMap = Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/MiniMapImage"); 
}
void GuiHandler::updateMiniMap(double globalX, double globalZ)
{
	//#define TERRAIN_OFFSET_X -115
	//#define TERRAIN_OFFSET_Z 12
	//#define WORLDSIZE 400

#define TERRAIN_OFFSET_X 0
#define TERRAIN_OFFSET_Z 0
#define WORLDSIZE 600

	//Nytt koordinatsystem där x och z kan gå från (-1) till (1);
	double x = (globalX - TERRAIN_OFFSET_X) / (WORLDSIZE / 2);
	double z = (globalZ - TERRAIN_OFFSET_Z) / (WORLDSIZE / 2);

#define MINIMAPWIDTH 0.24 // 24%
	//Skalar ner koordinatsystem så x och z kan gå från (0) till (1-MINIMAPWIDTH);
	x = (x + 1) / (2/(1-MINIMAPWIDTH));
	z = (z + 1) / (2/(1-MINIMAPWIDTH));

	Ogre::Real uvXstart = 0.76 - z;
	Ogre::Real uvXend = uvXstart + 0.24;

	Ogre::Real uvYstart = x;
	Ogre::Real uvYend = uvYstart + 0.24;
	Ogre::String value = Ogre::StringConverter::toString(uvXstart);
	value += " ";
	value += Ogre::StringConverter::toString(uvYstart);
	value += " ";
	value += Ogre::StringConverter::toString(uvXend);
	value += " ";
	value += Ogre::StringConverter::toString(uvYend);
	mtpMiniMap->setParameter("uv_coords" ,value );
}

void GuiHandler::initFirstPersonShooter()
{
	mtpMuzzleFireFirstPerson = Ogre::OverlayManager::getSingleton().getByName("GuiKarma/FirstPersonMuzzle");
	mtpMuzzleFireFirstPerson->hide();
	mtpFirstPersonLayer = Ogre::OverlayManager::getSingleton().getByName("GuiKarma/FirstPerson");
	mtpFirstPersonLayer->hide();
	mtpMiniMapLayer->setZOrder(1);
}

void GuiHandler::showMuzzleFire(bool state)
{
	if (state)
		mtpMuzzleFireFirstPerson->show();
	else
		mtpMuzzleFireFirstPerson->hide();
}

#ifdef DEBUG
void GuiHandler::initDebugGui()
{
	mtpDebugLayer = Ogre::OverlayManager::getSingleton().getByName("GuiKarma/DebugOverlay");
	mtpDebugLayer->show();
	debugFPS =		Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/DebugOverlay/TextFPS"); 
	debugCharX =	Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/DebugOverlay/TextCharX"); 
	debugCharY =	Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/DebugOverlay/TextCharY"); 
	debugCharZ =	Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/DebugOverlay/TextCharZ"); 
	debugCamX =		Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/DebugOverlay/TextCamX"); 
	debugCamY =		Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/DebugOverlay/TextCamY"); 
	debugCamZ =		Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/DebugOverlay/TextCamZ"); 
	debugDirX =		Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/DebugOverlay/TextDirX"); 
	debugDirY =		Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/DebugOverlay/TextDirY"); 
	debugDirZ =		Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/DebugOverlay/TextDirZ"); 
	debugMouseX =	Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/DebugOverlay/TextMousePosX"); 
	debugMouseY =	Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/DebugOverlay/TextMousePosY"); 
	debugTriangles= Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/DebugOverlay/TextTriangles"); 
	debugChunkX = Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/DebugOverlay/TextPlaceholder1"); 
	debugChunkY = Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/DebugOverlay/TextPlaceholder2"); 
	debugHP =	Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/DebugOverlay/TextPlaceholder3"); 
}

void GuiHandler::updateDebugFPS(Ogre::String& s)
{
	static Ogre::String currFps = "Current FPS: "; 
	debugFPS->setCaption(currFps + s);
}
void GuiHandler::updateDebugCharXYZ(Ogre::Vector3& p)
{
	static Ogre::String CharX = "Char  X: ";
	debugCharX->setCaption(CharX + Ogre::StringConverter::toString(p.x));
	static Ogre::String CharY = "Char  Y: ";
	debugCharY->setCaption(CharY + Ogre::StringConverter::toString(p.y));		
	static Ogre::String CharZ = "Char  Z: ";
	debugCharZ->setCaption(CharZ + Ogre::StringConverter::toString(p.z));
}
void GuiHandler::updateDebugCamXYZ(Ogre::Vector3& p)
{
	static Ogre::String CamX = "Cam  X: ";
	debugCamX->setCaption(CamX + Ogre::StringConverter::toString(p.x));
	static Ogre::String CamY = "Cam  Y: ";
	debugCamY->setCaption(CamY + Ogre::StringConverter::toString(p.y));		
	static Ogre::String CamZ = "Cam  Z: ";
	debugCamZ->setCaption(CamZ + Ogre::StringConverter::toString(p.z));
}
void GuiHandler::updateDebugDirXYZ(Ogre::Vector3& p)
{
	static Ogre::String DirX = "Dir  X: ";
	debugDirX->setCaption(DirX + Ogre::StringConverter::toString(p.x));
	static Ogre::String DirY = "Dir  Y: ";
	debugDirY->setCaption(DirY + Ogre::StringConverter::toString(p.y));		
	static Ogre::String DirZ = "Dir  Z: ";
	debugDirZ->setCaption(DirZ + Ogre::StringConverter::toString(p.z));
}
void GuiHandler::updateDebugTriangles(Ogre::String& s)
{
	static Ogre::String currFps = "Triangle count: "; 
	debugTriangles->setCaption(currFps + s);
}


void GuiHandler::updateDebugCursorPos(const int x,const int y)
{
	static Ogre::String mousePosX = "Mouse X: "; 		
	debugMouseX->setCaption(mousePosX + Ogre::StringConverter::toString(x));
	static Ogre::String mousePosY = "Mouse Y: ";
	debugMouseY->setCaption(mousePosY + Ogre::StringConverter::toString(y));
}

void GuiHandler::updateCurChunk(const int x, const int y)
{
	static Ogre::String ChunkPosX = "Chunk X: "; 		
	debugChunkX->setCaption(ChunkPosX + Ogre::StringConverter::toString(x));
	static Ogre::String ChunkPosY = "Chunk Y: ";
	debugChunkY->setCaption(ChunkPosY + Ogre::StringConverter::toString(y));
}

void GuiHandler::updateDebugHP(float x)
{
	static Ogre::String hp = "HP: "; 		
	debugHP->setCaption(hp + Ogre::StringConverter::toString(x));
}
#endif