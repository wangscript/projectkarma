/*---------------------------------------------------------------------------------*/
/* File: GuiHandler.h															   */
/* Author: Per Karlsson, perkarlsson89@gmail.com								   */
/*																				   */
/* Description:	GuiHandler is a class takes care of all the in-game GUI			   */
/* (and the cursor).															   */
/*---------------------------------------------------------------------------------*/

#ifndef GUIHANDLER_H
#define GUIHANDLER_H

#include <Ogre.h>
#include "Debug.h"
#include "GameEnums.h"

class GuiHandler
{
public:
	/* renderWnd  = the current OGRE render window. */
	GuiHandler(Ogre::RenderWindow* renderWnd);

	/* A struct containing pointers to all overlay elements in an actionbar slot. */
	struct ActionBarElements
	{
		Ogre::OverlayElement	*PowerUp,*PowerUpLocked,*Active;
	};	

	/* Changes the cursor. Following cursors are available:
	Game::Cursor_Normal
	Game::Cursor_MoveBox 
	Game::Cursor_MoveBoxGrab
	Game::Cursor_Aim	*/
	void changeCursor(int cursor);
	
	/* Either shows or hide the First Person gun. If the First Person gun is to be showed,
	then the cursor is placed in the middle of the screen.*/
	void firstPerson(bool state);
	
	/* Hides the casting bar. */
	void hideCastingBar();
	
	/* Hides the cursor. */
	void hideCursor();
	
	/* Initiate the GUI. Following components are loaded here:
	Action bar, Casting Bar, MiniMap, First Person Gun, HP-meter.*/
	void initGui();
	
	/* If a PowerUp is loaded, this returns true, else false. A PowerUp is loaded
	once the player has intersected with the representive barrel in game.*/
	bool isLoaded(int powerUp);
	
	/* Loads the casting bar for PowerUps that requires once. TotalTime is how long (seconds)
	the bar will be shown. */
	void loadCastingBar(int powerUp, float totalTime);
	
	/* Shows the cursor. */
	void showCursor();
	
	/* Either shows or hides Muzzle Fire in First Person mode. */
	void showMuzzleFire(bool state);
	
	/* Either shows or hides the In-Game Gui. Used when switching from Menu and game and vice versa. */
	void toggleGameGUI(bool state);
	
	/* Updates an action bar slot. Avaiable action bar modes are:
	Game::ActionBar_Normal
	Game::ActionBar_Locked
	Game::ActionBar_Active */
	void updateActionBarElement(int powerUp,int ActionBarMode);
	
	/* Updates the casting bar (making it smaller). */
	void updateCastingBar(float curTime);
	
	/* Updates the position of the cursor on the screen. */
	void updateCursorPos(const int x,const int y);
	
	/* Update the HP-meter. hp is a postive value. */
	void updateHP(float hp);
	
	/* Updates the position on the MiniMap. GlobalX and GlobalZ are the OGRE world
	position coordinates of the player.*/
	void updateMiniMap(double globalX, double globalZ);
	
private:
	bool					 mvFirstPerson,mvCastBar;
	int						 mvMouseX,mvMouseY;
	float					 mvCastingBarTotalTime;
	Ogre::RenderWindow		*mvpRenderWnd;


	Ogre::Overlay			*mvpCursorLayer,*mvpMiniMapLayer,*mvpMuzzleFireFirstPerson,*mvpFirstPersonLayer,
							*mvpHPLayer,*mvpCastingBarLayer,*mvpActionBar;

	Ogre::OverlayElement	*mvpCursorNormal,*mvpCursorMoveBox, *mvpCursorMoveBoxGrab,*mvpCursorAim,*mvpCurCursor,
							*mvpMiniMap,*mvpCastingBar,*mvpCastingBarCur,*mvpCastingBarText,*mvpHPText,*mvpCastingBarTextTime,
							*mvpCastingBarSuperSpeed,*mvpCastingBarMoveBox,*mvpCastingBarRocketBoots,*mvpCurrCastingBarIcon;

	ActionBarElements		 mvpActionBarSuperJump,mvpActionBarSuperSpeed,mvpActionBarMoveBox,mvpActionBarPistol,mvpActionBarMachineGun,
							 mvpActionBarRocketBoots; 

	/* Initiates the HP-meter. */
	void initHP();

	/* Initiates the Action Bar. */
	void initActionBar();

	/* Initiates the First Person Shooter GUI. Starts hidden. */
	void initFirstPersonShooter();

	/* Initiates the MiniMap */
	void initMiniMap();

	/* Initiates the Casting Bar. */
	void initCastingBar();

	/* Changes an action bar element. See ::updateActionBarElement for available Action Bar modes. */
	void changeActionBarElement(ActionBarElements& ae, int ActionBarMode);
	
#ifdef DEBUG
public:
	void initDebugGui();
	void updateDebugFPS(Ogre::String&);
	void updateDebugCharXYZ(Ogre::Vector3&);
	void updateDebugCamXYZ(Ogre::Vector3&);
	void updateDebugDirXYZ(Ogre::Vector3&);
	void updateDebugTriangles(Ogre::String& s);
	void updateDebugCursorPos(const int x,const int y);
	void updateCurChunk(const int x, const int y);
	void updateDebugHP(float x);	
private:
	Ogre::Overlay* mtpDebugLayer;
	Ogre::OverlayElement* debugFPS;
	Ogre::OverlayElement* debugCharX;
	Ogre::OverlayElement* debugCharY;
	Ogre::OverlayElement* debugCharZ;
	Ogre::OverlayElement* debugCamX;
	Ogre::OverlayElement* debugCamY;
	Ogre::OverlayElement* debugCamZ;
	Ogre::OverlayElement* debugDirX;
	Ogre::OverlayElement* debugDirY;
	Ogre::OverlayElement* debugDirZ;
	Ogre::OverlayElement* debugMouseX;
	Ogre::OverlayElement* debugMouseY;
	Ogre::OverlayElement* debugTriangles;
	Ogre::OverlayElement* debugChunkX;
	Ogre::OverlayElement* debugChunkY;
	Ogre::OverlayElement* debugHP;
#endif

};

#endif#ifndef GUIHANDLER_H
#define GUIHANDLER_H

#include <Ogre.h>
#include "Debug.h"
#include "GameEnums.h"

class GuiHandler
{
public:

	GuiHandler(Ogre::RenderWindow* renderWnd);

	void initGui();

	struct ActionBarElements
	{
		Ogre::OverlayElement	*PowerUp,*PowerUpLocked,*Active;
	};
	void toggleGameGUI(bool state);

	void updateActionBarElement(int powerUp,int ActionBarMode);
	bool isLoaded(int powerUp);
	void loadCastingBar(int powerUp, float totalTime);
	void updateCastingBar(float curTime);
	void hideCastingBar();

	void showMuzzleFire(bool state);
	void firstPerson(bool state);
	void updateHP(float hp);
	void updateMiniMap(double globalX, double globalZ);

	void showCursor();
	void changeCursor(int cursor);
	void hideCursor();
	void updateCursorPos(const int x,const int y);

private:
	bool mvFirstPerson,mvCastBar;
	int mvMouseX,mvMouseY;
	Ogre::OverlayElement *mtpCursorNormal,*mtpCursorMoveBox, *mtpCursorMoveBoxGrab,*mtpCursorAim,*mtpCurCursor;
	Ogre::Overlay* mtpCursorLayer;
	Ogre::Overlay*	mtpMiniMapLayer;
	Ogre::Overlay* mtpMuzzleFireFirstPerson;
	Ogre::Overlay* mtpFirstPersonLayer;
	Ogre::Overlay* mtpHPLayer;
	Ogre::RenderWindow* mvpRenderWnd;
	
	
	Ogre::Overlay* mtpCastingBarLayer;
	Ogre::OverlayElement* mtpMiniMap;
	Ogre::OverlayElement* mtpCastingBar;
	Ogre::OverlayElement* mtpCastingBarCur;
	Ogre::OverlayElement* mtpCastingBarText;
	Ogre::OverlayElement* mtpHPText;
	Ogre::OverlayElement* mtpCastingBarTextTime;
	Ogre::OverlayElement* mtpCastingBarSuperSpeed,*mtpCastingBarMoveBox,*mtpCastingBarRocketBoots;
	Ogre::OverlayElement* mtpCurrCastingBarIcon;
	float mtCastingBarTotalTime;

	//Action Bar
	Ogre::Overlay* mvActionBar;
	ActionBarElements mvActionBarSuperJump; 
	ActionBarElements mvActionBarSuperSpeed;
	ActionBarElements mvActionBarMoveBox;
	ActionBarElements mvActionBarPistol;
	ActionBarElements mvActionBarMachineGun;
	ActionBarElements mvActionBarRocketBoots;
	
	void initHP();
	void initActionBar();
	void initFirstPersonShooter();
	void initMiniMap();
	void initCastingBar();
	void changeActionBarElement(ActionBarElements& ae, int ActionBarMode);
	

#ifdef DEBUG

public:
	void initDebugGui();
	void updateDebugFPS(Ogre::String&);
	void updateDebugCharXYZ(Ogre::Vector3&);
	void updateDebugCamXYZ(Ogre::Vector3&);
	void updateDebugDirXYZ(Ogre::Vector3&);
	void updateDebugTriangles(Ogre::String& s);
	void updateDebugCursorPos(const int x,const int y);
	void updateCurChunk(const int x, const int y);
	void updateDebugHP(float x);	
private:
	Ogre::Overlay* mtpDebugLayer;
	Ogre::OverlayElement* debugFPS;
	Ogre::OverlayElement* debugCharX;
	Ogre::OverlayElement* debugCharY;
	Ogre::OverlayElement* debugCharZ;
	Ogre::OverlayElement* debugCamX;
	Ogre::OverlayElement* debugCamY;
	Ogre::OverlayElement* debugCamZ;
	Ogre::OverlayElement* debugDirX;
	Ogre::OverlayElement* debugDirY;
	Ogre::OverlayElement* debugDirZ;
	Ogre::OverlayElement* debugMouseX;
	Ogre::OverlayElement* debugMouseY;
	Ogre::OverlayElement* debugTriangles;
	Ogre::OverlayElement* debugChunkX;
	Ogre::OverlayElement* debugChunkY;
	Ogre::OverlayElement* debugHP;
#endif

};

#endif