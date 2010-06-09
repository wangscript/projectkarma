/*---------------------------------------------------------------------------------*/
/* File: GuiHandler.cpp															   */
/* Author: Per Karlsson, perkarlsson89@gmail.com								   */
/*																				   */
/* Description:	GuiHandler is a class takes care of all the in-game GUI			   */
/* (and the cursor).															   */
/*---------------------------------------------------------------------------------*/

#include <GuiHandler.h>

/*---------------------------------------------------------------------------------*/
/*									PUBLIC										   */
/*---------------------------------------------------------------------------------*/
GuiHandler::GuiHandler(Ogre::RenderWindow* renderWnd) :mvpRenderWnd(renderWnd), mvFirstPerson(false),mvCastBar(false)
{
	//Initiate all the different cursors.
	//Textures and overlay files are located in a Resource Group called Cursor.
	mvpCursorLayer = Ogre::OverlayManager::getSingleton().getByName("CursorMain");
	mvpCursorNormal = Ogre::OverlayManager::getSingleton().getOverlayElement("CursorNormal");
	mvpCursorMoveBox = Ogre::OverlayManager::getSingleton().getOverlayElement("CursorMoveBox");
	mvpCursorMoveBox->hide();
	mvpCursorMoveBoxGrab = Ogre::OverlayManager::getSingleton().getOverlayElement("CursorMoveBoxGrab");
	mvpCursorMoveBoxGrab->hide();
	mvpCursorAim = Ogre::OverlayManager::getSingleton().getOverlayElement("CursorAim");
	mvpCursorAim->hide();
	
	//Start with the normal cursor
	mvpCurCursor = mvpCursorNormal;
	showCursor();
}


/*---------------------------------------------------------------------------------*/
void GuiHandler::changeCursor(int cursor)
{
	//Hide the current Cursor.
	mvpCurCursor->hide();

	//Set the new cursor.
	switch (cursor)
	{
	case Game::Cursor_Normal:
		mvpCurCursor = mvpCursorNormal;
		break;
	case Game::Cursor_MoveBox:
		mvpCurCursor = mvpCursorMoveBox;
		break;
	case Game::Cursor_MoveBoxGrab:
		mvpCurCursor = mvpCursorMoveBoxGrab;
		break;
	case Game::Cursor_Aim:
		mvpCurCursor = mvpCursorAim;
	}

	//Show the new cursor and update its position.
	mvpCurCursor->show();
	mvpCurCursor->setPosition(mvMouseX,mvMouseY);
}


/*---------------------------------------------------------------------------------*/
void GuiHandler::firstPerson(bool state)
{
	//Either shows or hides the First Person Gun GUI.
	mvFirstPerson = state;
	if (mvFirstPerson)
	{
		mvpFirstPersonLayer->show();
		updateCursorPos(mvpRenderWnd->getWidth()/2,mvpRenderWnd->getHeight()/2);
	}
	else
		mvpFirstPersonLayer->hide();
}

/*---------------------------------------------------------------------------------*/
void GuiHandler::hideCastingBar()
{
	//If the current casting bar has an icon, hide it.
	if (mvpCurrCastingBarIcon)
	{
		mvpCurrCastingBarIcon->hide();
		mvpCurrCastingBarIcon = 0;
	}

	//Hide and disable the casting bar.
	mvCastBar = false;
	mvpCastingBarLayer->hide();
}
/*---------------------------------------------------------------------------------*/
void GuiHandler::hideCursor()
{
	//Hide the Cursor overlay.
	mvpCursorLayer->hide();
}
/*---------------------------------------------------------------------------------*/
void GuiHandler::initGui()
{
	//Initiate all the In-Game GUI components.
	initActionBar();
	initCastingBar();
	initMiniMap();
	initFirstPersonShooter();
	initHP();
}
/*---------------------------------------------------------------------------------*/
bool GuiHandler::isLoaded(int powerUp)
{
//If a PowerUp is loaded, the regular ".PowerUp" in the ActionBarElement struct is visible.
switch (powerUp)		
	{
	case Game::PowerUp_SuperJump:
		return mvpActionBarSuperJump.PowerUp->isVisible();
		break;
	case Game::PowerUp_SuperSpeed:
		return mvpActionBarSuperSpeed.PowerUp->isVisible();
		break;
	case Game::PowerUp_MoveBoxMode:
		return mvpActionBarMoveBox.PowerUp->isVisible();
		break;
	case Game::PowerUp_GunModePistol:
		return mvpActionBarPistol.PowerUp->isVisible();
		break;
	case Game::PowerUp_GunModeMachineGun:
		return mvpActionBarMachineGun.PowerUp->isVisible();
		break;
	case Game::PowerUp_RocketBootsMode:
		return mvpActionBarRocketBoots.PowerUp->isVisible();
	};	
return false;
}

/*---------------------------------------------------------------------------------*/
void GuiHandler::loadCastingBar(int powerUp , float totalTime)
{
	//Depending on PowerUp, the casting bar has different text strings and icons.
	Ogre::String name;
	switch(powerUp)
	{
	case Game::PowerUp_SuperSpeed:
		mvpCurrCastingBarIcon = mvpCastingBarSuperSpeed;
		name = "Super Speed";
		break;
	case Game::PowerUp_MoveBox:
		mvpCurrCastingBarIcon = mvpCastingBarMoveBox;
		name = "Move Box";
		break;
	case Game::PowerUp_RocketBootsMode:
		mvpCurrCastingBarIcon = mvpCastingBarRocketBoots;
		name = "Rocket Boots";
	}
	
	//Enables casting bar.
	mvCastBar = true;
	mvpCastingBarLayer->show();
	mvpCurrCastingBarIcon->show();
	
	//mvpCastingBarCur is a small "bar" that shows current position on the casting bar.
	//This resets it to start at the same position as the casting bars right edge.
	//243 = width of casting bar
	//24 = left offset of casting bar. (the icon is 24 in width)
	mvpCastingBarCur->setLeft(24+243);

	//Updates the total time and the name of the PowerUp on the casting bar.
	mvCastingBarTotalTime = totalTime;
	mvpCastingBarText->setCaption(name);
	//For example "SuperSpeed      x / 3.0"
	//SuperSpeed =  mvpCastingBarText
	//3.0 = mvCastingBarTotalTime
}

/*---------------------------------------------------------------------------------*/
void GuiHandler::showCursor()
{
	//Show the cursor overlay
	mvpCursorLayer->show();
}
/*---------------------------------------------------------------------------------*/
void GuiHandler::showMuzzleFire(bool state)
{
	//Either shows or hides the First Person muzzle fire.
	if (state)
		mvpMuzzleFireFirstPerson->show();
	else
		mvpMuzzleFireFirstPerson->hide();
}

/*---------------------------------------------------------------------------------*/
void GuiHandler::toggleGameGUI(bool state)
{
	//Either shows or hides the In-Game GUI
	if (state)
	{
		mvpMiniMapLayer->show();
		//If the casting bar was enabled before, show it.
		if (mvCastBar)
			mvpCastingBarLayer->show();
		mvpActionBar->show();
		mvpHPLayer->show();
		if (mvFirstPerson)
			firstPerson(mvFirstPerson);
	}
	else
	{
		mvpMiniMapLayer->hide();
		mvpCastingBarLayer->hide();
		mvpActionBar->hide();
		mvpHPLayer->hide();
		if (mvFirstPerson)
			mvpFirstPersonLayer->hide();
	}
}

/*---------------------------------------------------------------------------------*/
void GuiHandler::updateActionBarElement(int powerUp,int ActionBarMode)
{
	//In the future: Make this more general maybe.

	//Changes an action bar slot. Some PowerUps are supposed to cancel others.
	switch (powerUp)		
	{
	case Game::PowerUp_SuperJump:
		changeActionBarElement(mvpActionBarSuperJump,ActionBarMode);
		//If SuperJump is set to active and RocketBoots is loaded, set Rocket Boots icon to normal (and not activ)
		if (ActionBarMode==Game::ActionBar_Active && isLoaded(Game::PowerUp_RocketBootsMode))
			changeActionBarElement(mvpActionBarRocketBoots,Game::ActionBar_Normal);
		break;

	case Game::PowerUp_SuperSpeed:
		changeActionBarElement(mvpActionBarSuperSpeed,ActionBarMode);
		//Same as SuperJump (line 213). SuperSpeed cancels RocketBoots and MoveBox.
		if (ActionBarMode==Game::ActionBar_Active && isLoaded(Game::PowerUp_RocketBootsMode))
			changeActionBarElement(mvpActionBarRocketBoots,Game::ActionBar_Normal);
		if (ActionBarMode==Game::ActionBar_Active && isLoaded(Game::PowerUp_MoveBoxMode))
			changeActionBarElement(mvpActionBarMoveBox,Game::ActionBar_Normal);
		break;

	case Game::PowerUp_MoveBoxMode:
		changeActionBarElement(mvpActionBarMoveBox,ActionBarMode);
		//Same as SuperJump (line 213). MoveBox cancels GunMode Pistol, GunMode MachineGun, SuperSpeed and RocketBoots.
		if (ActionBarMode==Game::ActionBar_Active && isLoaded(Game::PowerUp_GunModePistol))
			changeActionBarElement(mvpActionBarPistol,Game::ActionBar_Normal);
		if (ActionBarMode==Game::ActionBar_Active && isLoaded(Game::PowerUp_GunModeMachineGun))
			changeActionBarElement(mvpActionBarMachineGun,Game::ActionBar_Normal);
		if (ActionBarMode==Game::ActionBar_Active && isLoaded(Game::PowerUp_SuperSpeed))
			changeActionBarElement(mvpActionBarSuperSpeed,Game::ActionBar_Normal);
		if (ActionBarMode==Game::ActionBar_Active && isLoaded(Game::PowerUp_RocketBootsMode))
			changeActionBarElement(mvpActionBarRocketBoots,Game::ActionBar_Normal);
		break;

	case Game::PowerUp_GunModePistol:
		changeActionBarElement(mvpActionBarPistol,ActionBarMode);
		//Same as SuperJump (line 213). GunMode Pistol cancels GunMode MachineGun and MoveBox.
		if (ActionBarMode==Game::ActionBar_Active && isLoaded(Game::PowerUp_MoveBoxMode))
			changeActionBarElement(mvpActionBarMoveBox,Game::ActionBar_Normal);
		if (ActionBarMode==Game::ActionBar_Active && isLoaded(Game::PowerUp_GunModeMachineGun))
			changeActionBarElement(mvpActionBarMachineGun,Game::ActionBar_Normal);
		break;

	case Game::PowerUp_GunModeMachineGun:
		changeActionBarElement(mvpActionBarMachineGun,ActionBarMode);			
		//Same as SuperJump (line 213). GunMode Machine cancels GunMode Pistol and MoveBox.
		if (ActionBarMode==Game::ActionBar_Active && isLoaded(Game::PowerUp_MoveBoxMode))
			changeActionBarElement(mvpActionBarMoveBox,Game::ActionBar_Normal);
		if (ActionBarMode==Game::ActionBar_Active && isLoaded(Game::PowerUp_GunModePistol))
			changeActionBarElement(mvpActionBarPistol,Game::ActionBar_Normal);
		break;

	case Game::PowerUp_RocketBootsMode:
		changeActionBarElement(mvpActionBarRocketBoots,ActionBarMode);
		//Same as SuperJump (line 213). Rocket Boots cancels SuperSpeed,SuperJump and MoveBox.
		if (ActionBarMode==Game::ActionBar_Active && isLoaded(Game::PowerUp_SuperJump))
			changeActionBarElement(mvpActionBarSuperJump,Game::ActionBar_Normal);
		if (ActionBarMode==Game::ActionBar_Active && isLoaded(Game::PowerUp_SuperSpeed))
			changeActionBarElement(mvpActionBarSuperSpeed,Game::ActionBar_Normal);
		if (ActionBarMode==Game::ActionBar_Active && isLoaded(Game::PowerUp_MoveBoxMode))
			changeActionBarElement(mvpActionBarMoveBox,Game::ActionBar_Normal);
		break;
	};	
}
/*---------------------------------------------------------------------------------*/
void GuiHandler::updateCastingBar(float curTime)
{
	//Get the width in percents. relWidth is in the range 0 <= relWidth <= 1
	float relWidth = curTime/mvCastingBarTotalTime;
	
	//Depending on the curTime, we need to set differnet precisions to avoid 0.000001 etc
	int precision1 = 2;
	//Prolbems once curTime = 0.xx
	if (curTime<1)
		precision1 = 1;
	
	//Once the casting bar is below 0.1 seconds, the casting bar is reseted.
	//No differnece between 0.1 and 0.0, no one can notice any difference.
	if (curTime < 0.1)
	{
		curTime = 0;
		mvCastBar = false;
	}

	static Ogre::String text("/");
	if (relWidth <= 1)
	{
		//Updates the current casting bar text. " 2.3 / 3" for example.
		mvpCastingBarTextTime->setCaption(Ogre::StringConverter::toString(curTime,precision1)+text+Ogre::StringConverter::toString(mvCastingBarTotalTime,3));
		
		//243 = castingbar width
		//relWidth is in the range 0 <= relWidth <= 1
		mvpCastingBar->setWidth(243 * relWidth);
		
		//See ::loadCastingBar for more information about mvpCastingBarCur
		mvpCastingBarCur->setLeft(24 + 243 * relWidth);
	}
}
/*---------------------------------------------------------------------------------*/
void GuiHandler::updateCursorPos(const int x,const int y)
{
	/* The (0,0) position of a cursor is the top left corner. Every cursor has the size of
	32 x 32 pixels. If 16 is subtracted from each cursor position, the cursors center will be where the
	OIS cursor is.*/
	mvMouseX = x-16;
	mvMouseY = y-16;
	mvpCurCursor->setPosition(mvMouseX,mvMouseY);
#ifdef DEBUG
	//updateDebugCursorPos(x,y);
#endif
}
/*---------------------------------------------------------------------------------*/
void GuiHandler::updateHP(float hp)
{
	//Updates the HP-meter
	mvpHPText->setCaption(Ogre::StringConverter::toString(hp));
}
/*---------------------------------------------------------------------------------*/
void GuiHandler::updateMiniMap(double globalX, double globalZ)
{
	//In the future: Read World Size from the Settings class.
	#define WORLDSIZE 600

	//New coordinate system where x and z can have values between (-1) and (1).
	double x = (globalX) / (WORLDSIZE / 2);
	double z = (globalZ) / (WORLDSIZE / 2);

	#define MINIMAPWIDTH 0.24 
	/* On each side of the minimap there is 12% of the picture.
		________________________
	12% |	_________________   |
		|	|XXXXXXXXXXXXXXX|	|
	76%	|	|XXXXXXXXXXXXXXX|	|
		|	|XXXXXXXXXXXXXXX|	|
		|	|XXXXXXXXXXXXXXX|	|
	12%	|_______________________|
		12%        76%		 12%

	XXX = minimap
	the white space = black empty space on the minimap

	Scales the coordinate system so x and z can have values from (0) to (1-MINIMAPWIDTH=0.76)	*/
	x = (x + 1) / (2/(1-MINIMAPWIDTH));
	z = (z + 1) / (2/(1-MINIMAPWIDTH));

	//Set new UV coordinates on the minimap texture
	Ogre::Real uvXstart = 0.76 - z;
	Ogre::Real uvXend = uvXstart + 0.24;
	Ogre::Real uvYstart = x;
	Ogre::Real uvYend = uvYstart + 0.24;

	//Changes the UV coordinates of the MiniMap layer.
	Ogre::String value = Ogre::StringConverter::toString(uvXstart);
	value += " ";
	value += Ogre::StringConverter::toString(uvYstart);
	value += " ";
	value += Ogre::StringConverter::toString(uvXend);
	value += " ";
	value += Ogre::StringConverter::toString(uvYend);
	mvpMiniMap->setParameter("uv_coords" ,value );
}

/*---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*/
/*									PRIVATE										   */
/*---------------------------------------------------------------------------------*/
void GuiHandler::initActionBar()
{
	//Load Action Bar border.
	mvpActionBar = Ogre::OverlayManager::getSingletonPtr()->getByName("ActionBarOverlay");
	mvpActionBar->show();

	//Load all elements and only show the locked ones (initial state)
	mvpActionBarSuperJump.PowerUp = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupSuperJump");
	mvpActionBarSuperJump.PowerUpLocked = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupSuperJumpLocked");
	mvpActionBarSuperJump.Active = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupSuperJumpActive");
	changeActionBarElement(mvpActionBarSuperJump,Game::ActionBar_Locked);

	mvpActionBarSuperSpeed.PowerUp = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupSuperSpeed");
	mvpActionBarSuperSpeed.PowerUpLocked = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupSuperSpeedLocked");
	mvpActionBarSuperSpeed.Active = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupSuperSpeedActive");
	changeActionBarElement(mvpActionBarSuperSpeed,Game::ActionBar_Locked);

	mvpActionBarMoveBox.PowerUp = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupMoveBox");
	mvpActionBarMoveBox.PowerUpLocked = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupMoveBoxLocked");
	mvpActionBarMoveBox.Active = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupMoveBoxActive");
	changeActionBarElement(mvpActionBarMoveBox,Game::ActionBar_Locked);

	mvpActionBarPistol.PowerUp = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupPistol");
	mvpActionBarPistol.PowerUpLocked = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupPistolLocked");
	mvpActionBarPistol.Active = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupPistolActive");
	changeActionBarElement(mvpActionBarPistol,Game::ActionBar_Locked);

	mvpActionBarMachineGun.PowerUp = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupMachineGun");
	mvpActionBarMachineGun.PowerUpLocked = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupMachineGunLocked");
	mvpActionBarMachineGun.Active = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupMachineGunActive");
	changeActionBarElement(mvpActionBarMachineGun,Game::ActionBar_Locked);

	mvpActionBarRocketBoots.PowerUp = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupRocketBoots");
	mvpActionBarRocketBoots.PowerUpLocked = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupRocketBootsLocked");
	mvpActionBarRocketBoots.Active = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("ActionBarOverlay/pwrupRocketBootsActive");
	changeActionBarElement(mvpActionBarRocketBoots,Game::ActionBar_Locked);
}


/*---------------------------------------------------------------------------------*/
void GuiHandler::changeActionBarElement(ActionBarElements& ae, int ActionBarMode)
{
	/*Changes a slot in the Action Bar.
	
	Action Bar Mode == Game::ActionBar_Normal
	Show normal layer, hide locked and active.

	Action Bar Mode == Game::ActionBar_Active
	Show active layer and normal layer, hide locked.

	Action Bar Mode == Game::ActionBar_Locked
	Show locked layer, hide locked and normal.
	*/

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

/*---------------------------------------------------------------------------------*/
void GuiHandler::initCastingBar()
{
	//Initiate the casting bar overlays.
	mvpCastingBarLayer = Ogre::OverlayManager::getSingleton().getByName("GuiKarma/CastingBar");
	mvpCastingBar =	Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/CastingbarPanel/Texture"); 
	mvpCastingBarCur =Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/CastingBarPanel/TextureCurrent"); 
	mvpCastingBarText =	Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/CastingbarPanel/Text");
	mvpCastingBarTextTime = Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/CastingbarPanel/TextTime");

	//Initiate the casting bar icons. Default hidden.
	mvpCastingBarSuperSpeed = Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/CastingbarPanel/SuperSpeed");
	mvpCastingBarSuperSpeed->hide();
	mvpCastingBarMoveBox = Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/CastingbarPanel/MoveBox");
	mvpCastingBarMoveBox->hide();
	mvpCastingBarRocketBoots = Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/CastingbarPanel/RocketBoots");
	mvpCastingBarRocketBoots->hide();

	//At first, no icon is active.
	mvpCurrCastingBarIcon = 0;
}
/*---------------------------------------------------------------------------------*/
void GuiHandler::initMiniMap()
{
	//Initiate the MiniMap
	mvpMiniMapLayer = Ogre::OverlayManager::getSingleton().getByName("GuiKarma/MiniMap");
	mvpMiniMapLayer->show();
	mvpMiniMap = Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/MiniMapImage"); 
}
/*---------------------------------------------------------------------------------*/
void GuiHandler::initFirstPersonShooter()
{
	//Initiate the First Person GUI (Muzzle Fire and Gun). Starts hidden
	mvpMuzzleFireFirstPerson = Ogre::OverlayManager::getSingleton().getByName("GuiKarma/FirstPersonMuzzle");
	mvpMuzzleFireFirstPerson->hide();
	mvpFirstPersonLayer = Ogre::OverlayManager::getSingleton().getByName("GuiKarma/FirstPerson");
	mvpFirstPersonLayer->hide();
	mvpMiniMapLayer->setZOrder(1);
}


/*---------------------------------------------------------------------------------*/
void GuiHandler::initHP()
{
	//Initiate the HP-meter.
	mvpHPLayer = Ogre::OverlayManager::getSingleton().getByName("GuiKarma/HP");
	mvpHPLayer->show();	
	mvpHPText = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("GuiKarma/HPText");
}

/*---------------------------------------------------------------------------------*/
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