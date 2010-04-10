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
	virtual ~GuiHandler();

	virtual void initGui();

	virtual void initDebugGui();
	virtual void updateDebugFPS(Ogre::String&);
	virtual void updateDebugCharXYZ(Ogre::Vector3&);
	virtual void updateDebugCamXYZ(Ogre::Vector3&);
	virtual void updateDebugDirXYZ(Ogre::Vector3&);
	virtual void updateDebugTriangles(Ogre::String& s);
	virtual void updateDebugCursorPos(const int x,const int y);

	virtual void initIngameUI();
	virtual void changeIngameUIIcon(int pwrup);

	virtual void initCastingBar();
	virtual void loadCastingBar(Ogre::String name, float totalTime);
	virtual void updateCastingBar(float curTime);
	virtual void hideCastingBar();

	virtual void showCursor();
	virtual void hideCursor();
	virtual void updateCursorPos(const int x,const int y);
};

#endif