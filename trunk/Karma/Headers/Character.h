/*---------------------------------------------------------------------------------*/
/* File: Character.h															   */
/* Author: Per Karlsson, perkarlsson89@gmail.com								   */
/*																				   */
/* Description:	Character is the superclass for all characters. It involves	       */
/* functions and member variables that is common for all characters.			   */
/*---------------------------------------------------------------------------------*/

#ifndef CHARACTER_H
#define CHARACTER_H

#include "GameCommon.h"

class Character
{
public:

	/*Constructor. Needs to be called after the physics are initiad.
	sceneMgr = The current OGRE scene manager.
	physScene = The current NxOgre physics scene.
	renderSystem = The NxOgre render system.
	filename = Name of the mesh file.
	name = Name of the character.
	spawnPoint = Where the character will spawn in the world.
	scale = If the character needs to be scaled.
	hp = Starting Hit Points.
	walkSpeed = How fast the character will walk.
	collisionGroup = What collision group the character belongs to.
	*/
	Character(Ogre::SceneManager* sceneMgr,NxOgre::Scene* physScene,OGRE3DRenderSystem* renderSystem, Ogre::String filename, 
		Ogre::String name, Ogre::Vector3 spawnPoint , float scale, float hp , float walkSpeed,int collisionGroup);
			
	/* Changes the animation in a animation state.
	name = name of the animation state
	time = time to be added to the timeline in the animation
	loop = shall the animation loop or not?			
	*/
	void changeAnimation(const Ogre::String& name,const double time,bool loop=true);

	/* Shows/hides the bounding capsule and sphere */
	void debugMode();

	/* Returns the character entity. */
	Ogre::Entity* getEntity(){return mtpCharEnt;};
	
	/* Returns the bounding capsule.  */
	OGRE3DBody* getHitBox(){return mtpHitBox;};

	/* Returns the bounding sphere. */
	OGRE3DBody* getHitBoxHead(){return mtpHitBoxHead;};
	
	/* Is the character dead? Returns true if so. */
	bool isDead(){return mtDying;};

	/* Is the character jumping? Returns true if so. */
	bool isJumping() {return mtJumping;};

	/* Virtual function that can be overwritten. Adds force in the Y-direction of the bounding capsule to make the character jump. */
	virtual void jump();

	/* Abstract function. Every character has its own way to move. */
	virtual void move(const double& timeSinceLastFrame) = 0;

	/* Sets Move Direction. Used for animation states (see GameEnums.h) */
	void setMoveDirection (const int& d){mtMoveDir = d;};

	/* Virtual function. Updates the character if it is dead. */
	virtual bool updateDead(const double& timeSinceLastFrame);	

	/* Virtual function. Updates the HP of the character.
	minusHP = hitpoints you want removed. Should be a positive value. */
	virtual	float updateHp(float minusHP);
	
	/* Updates the characters Node to the same position as the bounding capsule. */
	virtual void updatePosition();

	/* Convert a characters x and z to chunk coordinates. */
	GridData worldToChunk(const float& x, const float& z);

	/* Loop through all characters in the static mtDynamicCharacters std::vector. You send in a PhysX NxActor. 
	If that actor is the same actor as the bounding capsule's actor, then the character will be returned */
	static Character* loopDynamicCharacters(NxActor* actor);

protected:
	bool					 mtDying,mtJumping;
	float 					 mtHP,mtHPCur,mtWalkSpeed,mtDieRespawnTimer,*mtDieRespawnTime,*mtWorldSize,mtChunksWidth,mtFallingVelocity,*mtJumpForceY,mtCharOffsetY,*mtFreezeAfterDieTimer;
	int 					 mtMoveDir;
	NxMat33 				 mtHitBoxGlobalOrientation;
	NxOgre::Scene*			 mtpPhysicsScene;
	OGRE3DBody 				*mtpHitBox,*mtpHitBoxHead;
	Ogre::AnimationState	*mtpAnimationState;
	Ogre::SceneNode			*mtpCharNode, *mtpDieNode; 
	Ogre::Entity 			*mtpCharEnt;
	Ogre::Vector3			 mtSpawnPoint,*mtFaceDirection;
	Ogre::Real 				 *mtCharSpeedForward,*mtCharSpeedBackward,*mtCharSpeedStrafeLeft,*mtCharSpeedStrafeRight; 
	static std::vector<Character*> mtDynamicCharacters;	

	/* Virtal function. This simple version just resets the HP and sets mtDying to true. */
	virtual void die();

	/* Virtual function. The character respawns at its spawn point. */
	virtual void respawn();
	
	/* A function that rotates the character so it faces a specific direction.
	sceneNode = The scene node the character is attached to.
	dest = The vector we want the character to face.
	originalDir = The "forward" direction the character was initiated with.
	*/
	void rotateCharacter(Ogre::SceneNode* sceneNode,const Ogre::Vector3& dest,const Ogre::Vector3& originalDir);


};
#endif