#ifndef GUIHANDLER_H
#define GUIHANDLER_H

#include <Ogre.h>

class GuiHandler
{
protected:

	Ogre::OverlayContainer* mtpCursor;
	Ogre::Overlay* mtpCursorLayer;
	Ogre::Overlay* mtpDebugLayer;
	Ogre::Overlay* mtpIngameUILayer;
	Ogre::Overlay* mtpMenuGUILayer;
	
	Ogre::Overlay* mtpCastingBarLayer;
	Ogre::OverlayElement* mtpCastingBar;
	Ogre::OverlayElement* mtpCastingBarText;
	Ogre::String mtCastingBarString;
	Ogre::Real mtCastingBarTotalTime;


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

public:

	GuiHandler();
	~GuiHandler();

	void initGui();

	void initDebugGui();
	void updateDebugFPS(Ogre::String&);
	void updateDebugCharXYZ(Ogre::Vector3&);
	void updateDebugCamXYZ(Ogre::Vector3&);
	void updateDebugDirXYZ(Ogre::Vector3&);
	void updateDebugTriangles(Ogre::String& s);
	void updateDebugCursorPos(const int x,const int y);

	void initIngameUI();
	void changeIngameUIIcon(int pwrup);
	void showInGameUI();
	void hideInGameUI();
	void initCastingBar();
	void loadCastingBar(Ogre::String name, float totalTime);
	void updateCastingBar(float curTime);
	void hideCastingBar();

	void initMenuGUI();
	void removeMenuGUI();

	void showCursor();
	void hideCursor();
	void updateCursorPos(const int x,const int y);
};

#endif