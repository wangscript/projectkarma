/*---------------------------------------------------------------------------------*/
/* File: CameraHandler.h														   */
/* Author: Per Karlsson, perkarlsson89@gmail.com								   */
/*																				   */
/* Description:	CameraHandler is a singleton class that takes care of all the	   */	
/* camera movements.															   */
/*	There are 4 different camera views:											   */
/*	1. Third Person (Uses physics).												   */
/*	2. Semi-Third Person with mouse movable.									   */
/*	3. Semi-Third Person with mouse locked.										   */
/*	4. First Person.															   */
/*---------------------------------------------------------------------------------*/

#ifndef CAMERAHANDLER_H
#define CAMERAHANDLER_H

#include "Player.h"
#include "GameCommon.h"

class CameraHandler
{
private:
	/*Initate the singleton */
	CameraHandler() {};
	static CameraHandler singleton;
public:
	/* Get the Singleton */
	static CameraHandler getSingleton();
	static CameraHandler* getSingletonPtr();

	/* Adds linear velocity to the camera's bounding sphere. It tries to get to the same position as the completely virtual node "CamNoCollisionNode" */
	void AdjustCamera();

	/* Returns the camera's bounding sphere. */
	OGRE3DBody* getCamSphere(){return mvpCamSphere;};

	/* Returns the current camera mode. See @GameEnums */
	int getCamMode(){return mvCamMode;};

	/* Function that initiates the Camera Handler. Needs to be called after the player class is created so that all nodes exist. */
	void initCameraHandler(Ogre::Camera* cam, OGRE3DRenderSystem* renderSystem,Ogre::SceneManager* sceneMgr, Player* p);
	
	/* Moves the node "CamCollisionNode" to the same position as the bounding spheres position. Also makes the camera look at the Player. */
	void MoveCamera();

	/* Everytime the mouse has moved since the previous frame, this function needs to be called. Depending on what the current camere mode is, 
	a camera movement will be executed. */
	void Rotate(const OIS::MouseEvent &arg);

	/* Sets the active camera mode. It will automatically deatach the camera to the current mode. */
	void setCamMode(int x);
	
	/* Sets the link to the Player class. */
	void setPlayerPtr(Player* x){mvpPlayer = x;};

	/* In third person view, zoom is available. Call this function if the mouse wheel has moved since previous frame. */
	void Zoom(const OIS::MouseEvent &arg);
	
private:
	int							 mvCamMode;
	Player						*mvpPlayer;
	float						 mvPitchTrackerThirdPerson,mvPitchTrackerSemiThirdPerson,mvPitchTrackerFirstPerson;
	OGRE3DBody					*mvpCamSphere;
	Ogre::Camera				*mvpCam;
	Ogre::SceneManager			*mvpSceneMgr;

	Ogre::Real					*mvpCamCharYAdjust,*mvpCamGunTrackerOffsetX,*mvpCamGunTrackerOffsetY,*mvpCamGunTrackerOffsetZ,
								*mvpCamVelocityY,*mvpCamVelocityXZ,*mvpCamHeightMaxY,*mvpCamHeightMinY,*mvpCamDistanceMax,
								*mvpCamDistanceMin,*mvpRotate,*mvpZoom,mvScreenWidth,mvScreenHeight;

	Ogre::SceneNode				*mvpCamNoCollisionNode,*mvpCamCollisionNode,*mvpCamHelperNode,*mvpCamOrginNode,*mvpCharNode,
								*mvpCamFirstPersonNode,*mvpFirstPersonGunTrackerNode,*mvpCamGunMoveNode,*mvpCamGunMoveHelperNode,
								*mvpCamGunCenterNode,*mvpCamGunCenterHelperNode,*mvpCursorMovableGunTrackerNode;

	/* When entering Third Person mode, the Player is no longer manually controlled(as it is in the other modes). Also sets the bones to their initial
	orientation (if they were rotated during the other camera modes). Attaches the camera to the corresponding node. */
	void enterThirdPersonMode();

	/* When entering Semi-Third Person with the mouse locked, it will lock the mouse cursor (GUI wise) to the center of the screen. The player will
	also be manually controlled. Attaches the camera to the corresponding node. */
	void enterMixCursorCenterMode();

	/* Same as enterMixCursorCenterMode() but also resets the position of the camera since rotation around Z isn't avaiable in this mode.*/
	void enterMixCursorMovableMode();

	/* Centers the mouse GUI position. Attaches the camera to the corresponding node. */
	void enterFirstPersonMode();

	/* Set the Player to have bones manually controlled and resets the position of the Gun Tracker Node */
	void enterGunMode();

	/* Rotation around Z. mtPitchTracker is a reference to a member variable to prevent the camera from flipping the world. Each camera mode 
	with pitch available has it own pitchTracker*/
	void Pitch(const OIS::MouseEvent &arg,Ogre::SceneNode* node,float& mtPitchTracker);

	/* Instructions for rotation in the First Person mode */
	void RotateFirstPerson(const OIS::MouseEvent &arg);

	/* Instructions for rotation in the "Free mode" (used for screenshotting minimaps) */
	void RotateFree(const OIS::MouseEvent &arg);

	/* Instructions for rotation in the Semi-Third Person with mouse locked mode */
	void RotateMixCursorCenter(const OIS::MouseEvent &arg);

	/* Instructions for rotation in the Semi-Third Person with mouse movable mode */
	void RotateMixCursorMovable(const OIS::MouseEvent &arg);

	/* Instructions for rotation in the Third Person mode */
	void RotateThirdPerson(const OIS::MouseEvent &arg);
};		

#endif 