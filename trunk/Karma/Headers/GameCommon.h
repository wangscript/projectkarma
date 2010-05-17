#ifndef GAMECOMMON_H
#define GAMECOMMON_H

#include <Ogre.h>
#include <OISMouse.h>
#include <NxOgreOGRE3D.h>
#include "AppState.h"//Singelton usage
#include <NxActor.h>
#include <NxScene.h>
#include <NxUserContactReport.h>
#include <NxOgreCommon.h>
#include <NxOgreRigidBodyDynamicFunctions.h>
#include "SoundManager.h"
#include "GuiHandler.h"
#include "Effects.h"

namespace Game
{
	enum CameraMode
	{
		Cam_ThirdPerson = 0, 
		Cam_MixCursorCenter = 1,
		Cam_MixCursorMovable = 2,
		Cam_FirstPerson = 3,
		Cam_FreeMode = 4
	};

	enum MoveDirection
	{
		Move_None = 0,
		Move_Forward = 1,
		Move_StrafeLeft =2,
		Move_StrafeRight =3,
		Move_Backward = 4,
		Move_ForwardLeft = 5,
		Move_ForwardRight=6
	};

	enum PowerUp
	{
		PowerUp_None = (1 << 0), 
		PowerUp_SuperJump = (1 << 1), 
		PowerUp_SuperSpeed = (1 << 2), 
		PowerUp_MoveBox = (1 << 3), 
		PowerUp_GunMode = (1 << 4), 
		PowerUp_RocketBoots = (1 << 5), 
	};

	enum CollisionGroups
	{
		CollisionGroup_Player = (1 << 0),
		CollisionGroup_MoveBoxReference = (1 << 1),
		CollisionGroup_NPC = (1 << 2),
		CollisioNGroup_DynamicObjects = (1 << 3),
	};
}

class GridData
{
public:
	GridData(){};
	GridData(int x1,int y2) {x = x1;y=y2;};

	int x,y;
};

#endif