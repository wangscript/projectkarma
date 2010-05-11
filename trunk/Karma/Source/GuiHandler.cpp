#include <GuiHandler.h>

GuiHandler::GuiHandler()
{
}

GuiHandler::~GuiHandler()
{
}

void GuiHandler::initGui()
{
	mtpCursorLayer = Ogre::OverlayManager::getSingleton().getByName("GuiKarma/CursorMain");
	//mtpCursorLayer->setZOrder(400);
	mtpCursor = Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/CursorImage"); 
	showCursor();
	showCursor();
}

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

void GuiHandler::initIngameUI()
{
	mtpIngameUILayer = Ogre::OverlayManager::getSingleton().getByName("GuiKarma/IngameUIOverlay");
	mtpIngameUILayer->show();
}

void GuiHandler::showInGameUI()
{
	mtpIngameUILayer->show();
}

void GuiHandler::hideInGameUI()
{
	mtpIngameUILayer->hide();
}

void GuiHandler::changeIngameUIIcon(int pwrup)
{
	//@todo enums bättre 

	//Denna metoden laggar
	switch (pwrup)		
	{
	case 0: 
		mtpIngameUILayer->getChild("GuiKarma/IngameUI/Icon")->setMaterialName("PowerUp/None");
		break;
	case 1:
		mtpIngameUILayer->getChild("GuiKarma/IngameUI/Icon")->setMaterialName("PowerUp/SuperJump");
		break;
	case 2:
		mtpIngameUILayer->getChild("GuiKarma/IngameUI/Icon")->setMaterialName("PowerUp/SuperSpeed");
		break;
	case 3:
		mtpIngameUILayer->getChild("GuiKarma/IngameUI/Icon")->setMaterialName("PowerUp/MoveBox");
		break;
	}

}

void GuiHandler::initCastingBar()
{
	mtpCastingBarLayer = Ogre::OverlayManager::getSingleton().getByName("GuiKarma/CastingBar");
	mtpCastingBar =	Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/CastingbarPanel/Texture"); 
	mtpCastingBarText =	Ogre::OverlayManager::getSingleton().getOverlayElement("GuiKarma/CastingbarPanel/Text");
}

void GuiHandler::loadCastingBar(Ogre::String name, float totalTime)
{
	mtpCastingBarLayer->show();
	mtCastingBarTotalTime = totalTime;
	mtCastingBarString = name + ": ";
	mtpCastingBarText->setCaption(mtCastingBarString +  Ogre::StringConverter::toString(totalTime));
}

void GuiHandler::updateCastingBar(float curTime)
{
	mtpCastingBarText->setCaption(mtCastingBarString + Ogre::StringConverter::toString(curTime));
	float blabla = curTime/mtCastingBarTotalTime;
	std::cout << blabla << "\n";
	mtpCastingBar->setWidth(300 * blabla);
}

void GuiHandler::hideCastingBar()
{
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
	mtpCursor->setPosition(x-16,y-16);
	updateDebugCursorPos(x,y);
}

void GuiHandler::initMenuGUI()
{
	mtpMenuGUILayer = Ogre::OverlayManager::getSingleton().getByName("GuiKarma/Menu");
	mtpMenuGUILayer->show();
}

void GuiHandler::removeMenuGUI()
{
	mtpMenuGUILayer->hide();
}

void GuiHandler::showMiniMap()
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

