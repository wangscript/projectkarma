#ifndef GUIHANDLER_H
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