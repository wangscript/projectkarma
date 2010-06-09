/*---------------------------------------------------------------------------------*/
/* File: GameEnums.h											   				   */
/* Author: Per Karlsson, perkarlsson89@gmail.com								   */
/*																				   */
/* Description:	Enums for CameraMode,MoveDirection, PowerUps, CollisionGroups      */
/* Action Bar, Cursor and Animation. Makes the code "Cleaner".					   */
/*---------------------------------------------------------------------------------*/

#ifndef GAMEENUMS_H
#define GAMEENUMS_H

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
		PowerUp_MoveBoxMode = (1 << 3),
		PowerUp_GunModePistol = (1 << 4),
		PowerUp_GunModeMachineGun = (1 << 5),
		PowerUp_RocketBootsMode = (1 << 6),
		PowerUp_MoveBox =  (1 << 7),
		PowerUp_GunMode = (1 << 8),
		PowerUp_RocketBoots = (1 << 9)
	};

	enum CollisionGroups
	{
		CollisionGroup_Player = 1,
		CollisionGroup_MoveBoxReference = 2,
		CollisionGroup_NPC = 3,
		CollisioNGroup_DynamicObjects = 4
	};

	enum ActionBar
	{
		ActionBar_Normal = 0, 
		ActionBar_Locked = 1, 
		ActionBar_Active = 2
	};

	enum Cursor
	{
		Cursor_Normal = 0,
		Cursor_MoveBox = 1,
		Cursor_MoveBoxGrab = 2,
		Cursor_Aim = 3
	};

	enum Animation
	{
		Animation_Die = 0,
		Animation_Jump = 1,
		Animation_RocketBoots = 2,
		Animation_Walk = 3,
		Animation_WalkGun = 4
	};
}
#endif