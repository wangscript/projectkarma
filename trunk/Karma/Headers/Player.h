/*---------------------------------------------------------------------------------*/
/* File: Player.h																   */
/* Author: Per Karlsson, perkarlsson89@gmail.com								   */
/*																				   */
/* Description:	Player is a class that inherit from the Character class and		   */
/* represents the character that the user controls.			  					   */
/*---------------------------------------------------------------------------------*/

#ifndef PLAYER_H
#define PLAYER_H

#include "Character.h"
#include "Aimer.h"

#define CAMERA_HEIGHT			1.2
#define CAMERA_DISTANCE			3.0

/*  */
class Player : public Character, public Aimer
{
public:
	/* See character class constructor for info about the parameters. */
	Player(Ogre::SceneManager* sceneMgr,NxOgre::Scene* physScene,OGRE3DRenderSystem* renderSystem, Ogre::String filename, 
		Ogre::String name, Ogre::Vector3 spawnPoint, float hp , float walkSpeed);
	~Player();
	
	/* If the mouse button is pressed, the weapons are supposed to "spray" bullets. This function fix so a new bullet isn't
	shot every frame.*/
	void continuousShootTimer(const double& timeSinceLastFrame);

	/* The Player dies. 
	1. Resets the camera mode to Third Person.
	2. Resets the kill count.
	3. Resets all active NPCs.
	*/
	void die();

	/* If jumping or in Rocket Boots mode, the character dies if it falls from a too high position.
	This also resets the mtJumping if needed(if it is true). */
	void dieFromFall();

	/* In first person mode, the character and the guns will be hidden. */
	void firstPersonMode(bool s=true);
	
	/* Returns the current time on the casting bar timer. */
	float getCastingBarTimer(){return mvCastingBarTimer;};
	
	/* Returns the OGRE scene node that the Player is attached to.  */
	Ogre::SceneNode* getPlayerNode(){return mtpCharNode;};	
	
	/* Return current PowerUp(s). */
	int getPowerUp(){return mvPowerUp;};
	
	/* Just like the Jet Pack in the game Soldat, the Jet Pack power slighly increases every frame. If Rocket Boots is used,
	the user will eventually run out of Jet Pack power and this makes sure that the Power "regenerates".*/
	void increaseRocketBootsTimer(const double& timeSinceLastFrame);

	/* Makes the character jump by adding force on the bounding capsule in the Y direction. */
	void jump();
	
	/* The main update function. Since this functin is called every frame in the Game Loop, all functions that change
	the player characters behaviour is called from here. */
	void move(const double& timeSinceLastFrame);
	
	/* Moves the hidden kinematic controller that the jointed box is connected to. */
	void moveBoxMoved(const OIS::MouseEvent &e);
	
	/* Checks if any box were "grabbed" by using raycasting. If so, then it creates a join connection between
	that box and the kinematic moveBox controller that was created in the constructor.*/
	void moveBoxPressed(const OIS::MouseEvent &e);
	
	/* Sends informatin to the Aimer::Fire function. */
	void queueFire(const double& x,const double& y);
	
	/* Set mvRocketBoots to true. This is called from the game state when the space bar is pressed in Rocket Boots mode.  */
	void queueRocketBoots(){mvRocketBoots = true;};
	
	/* Destroys the earlier created joint (see moveBoxPressed). Also resets the casting bar. */
	void releaseBox();
	
	/* Stops the Rocket Boots particle systems from emitting and also disables the sound. */
	void resetRocketBoots();
	
	/* Adds force on the Bounding Capsule in the Y direction to simulate a Jet Pack effect. */
	void rocketBoots(const double& timeSinceLastFrame);
	
	/* Either shows or hides the guns */
	void semiThirdPersonMode(bool s=true);
	
	/* Adds a new PowerUp. If the PowerUp has a casting bar, "time" sets the length of it. */
	void setPowerUp(const int& p,float time = 0.0f);
	
	/* Sets the variable mvNeedUpdateBones to true´. Used in the CameraHandler class to tell the Player
	class in the move function to update the bones.*/
	void setNeedUpdateBones(){mvNeedUpdateBones = true;};
	
	/* See setNeedUpdateBones. In third person mode, there are times where you don't want to 
	rotate the character (when right mouse button isn't pressed for example). */
	void setNeedUpdateRotation(bool s){mvNeedUpdateRotation=s;};
	
	/* Plays Rocket Boots sound and starts emitting particles in the particle systems. */
	void startRocketBoots();
	
	/* Set the mouse position. Used in raytracing. */
	void updateCursorPos(int x,int y){mvMouseX = x;mvMouseY=y;};
	
	/* Updates the casting bar. */
	void updateCastingBar(const double& timeSinceLastFrame);
	
	/* Check if the player has entered a new Chunk since last frame. If so, it tells the Chunks class. */
	void updateChunk(const NxOgre::Vec3& hitBoxPos);
	
	/* Updates the HP of the Player. Updates the HP GUI. */
	float updateHp(float minusHP);
	
	/* Updates the position of the Player scene node to the same position as the Bounding capsules position.
	This function is called every frame _after_ the physics simulation. */
	void updatePosition();

private:
	ManuallyControlledParticles	*mvpMoveBoxParticles,*mvpRocketBootsParticle1,*mvpRocketBootsParticle2;
	MuzzleFireFirstPerson		*mvpMuzzleFireFirstPerson;
	Ogre::SceneNode				*mvpCamCollisionNode,*mvpCamGunModeNode;
	OGRE3DKinematicBody			*mvpMoveBoxController;
	NxOgre::FixedJoint			*mvpMoveBoxJoint;
	Ogre::Camera				*mvpCamera;
	Ogre::Vector3				mvDirCamToChar;
	float						mvCastingBarTimer,mvCharSpeedSuper,mvScreenWidth,mvScreenHeight,mvRocketBootsTimer,mvQuatTimer,*mvpMaxSpeedBeforeDie,
								mvRocketBootsUpdateFreq,mvRocketBootsCastingBarUpFreq,*mvpRocketBootsForceY,*mvpRocketBootsDuration,
								mvUpdateBonesFreq,*mvpContiniousShootTimer,*mvpMoveBoxDuration,*mvpSuperSpeedDuration;
	int							mvPowerUp,mvMouseX,mvMouseY,mvPowerUpCastingBar;
	GridData					mvChunk;
	bool						mvNeedUpdateBones,mvNeedUpdateRotation,mvRocketBoots;
	
	/* Moves the player and the camera to the position where they entered the new chunk. Also resets the HP-meter.*/
	void respawn();
};
#endif