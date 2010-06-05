#ifndef CHARACTER_H
#define CHARACTER_H

#include "GameCommon.h"

class Character
{
public:
	Character(Ogre::SceneManager* sceneMgr,NxOgre::Scene* physScene,OGRE3DRenderSystem* renderSystem, Ogre::String filename, 
		Ogre::String name, Ogre::Vector3 spawnPoint , float scale, float hp , float walkSpeed,int collisionGroup);

			void changeAnimation(const Ogre::String& name,const double time,bool loop=true);
			void debugMode();
			Ogre::Entity* getEntity(){return mtpCharEnt;};
			OGRE3DBody* getHitBox(){return mtpHitBox;};
			OGRE3DBody* getHitBoxHead(){return mtpHitBoxHead;};
			bool isDead(){return mtDying;};
			bool isJumping() {return mtJumping;};
	virtual void jump();
	virtual void move(const double& timeSinceLastFrame) = 0; //Abstract
			void setMoveDirection (const int& d){mtMoveDir = d;};
	virtual bool updateDead(const double& timeSinceLastFrame);	
	virtual	float updateHp(float minusHP);
	virtual void updatePosition();
			GridData worldToChunk(const float& x, const float& z);
	
	static Character* loopDynamicCharacters(NxActor* actor);

protected:
	bool					 mtDying,mtJumping;
	float 					 mtHP,mtHPCur,mtWalkSpeed,mtDieRespawnTimer,*mtDieRespawnTime,*mtWorldSize,mtChunksWidth,mtFallingSpeed,*mtJumpForceY,mtCharOffsetY,*mtFreezeAfterDieTimer;
	int 					 mtMoveDir;
	NxMat33 				 mtHitBoxGlobalOrientation;
	NxOgre::Scene*			 mtpPhysicsScene;
	OGRE3DBody 				*mtpHitBox,*mtpHitBoxHead;
	Ogre::AnimationState	*mtpAnimationState;
	Ogre::SceneNode			*mtpCharNode, *mtpDieNode; 
	Ogre::Entity 			*mtpCharEnt;
	Ogre::Vector3			 mtSpawnPoint,*mtFaceDirection;
	Ogre::Real 				 *mtCharSpeedForward,*mtCharSpeedBackward,*mtCharSpeedStrafeLeft,*mtCharSpeedStrafeRight; 
			
	virtual void die();
	virtual void respawn();
			void rotateCharacter(Ogre::SceneNode* sceneNode,const Ogre::Vector3& dest,const Ogre::Vector3& originalDir);

	static std::vector<Character*> mtDynamicCharacters;	
};
#endif