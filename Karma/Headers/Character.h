#ifndef CHARACTER_H
#define CHARACTER_H

#include "GameCommon.h"

class Character
{
public:
	Character(Ogre::SceneManager* sceneMgr,NxOgre::Scene* physScene,OGRE3DRenderSystem* renderSystem, Ogre::String filename, 
		Ogre::String name, Ogre::Vector3 spawnPoint , float scale, float hp , float walkSpeed,int collisionGroup=3);
	void setMoveDirection (const int& d){mtMoveDir = d;};
	virtual void jump();
	virtual void move(const double& timeSinceLastFrame) = 0;
	void changeAnimation(const Ogre::String& name,const double time,bool loop=true);
	bool isJumping() {return mtJumping;};
	void debugMode();
	Ogre::Entity* getEntity(){return mtpCharEnt;};
	OGRE3DBody* getHitBox(){return mtpHitBox;};
	virtual void updatePosition();
	float updateHp(float minusHP);
	static Character* loopDynamicCharacters(NxActor* actor);
	bool isDead(){return mtDying;};
	virtual bool updateDead(const double& timeSinceLastFrame);

protected:

	void rotateCharacter(Ogre::SceneNode* sceneNode,const Ogre::Vector3& dest,const Ogre::Vector3& originalDir);

	NxOgre::Scene*			mtpPhysicsScene;
	double					mtDieRespawnTimer,mtDieRespawnTime;
	bool					mtDying;
	float 					mtHPCur;
	float 					mtHP;
	Ogre::Real 				mtWalkSpeed;
	bool					mtJumping;
	int 					mtMoveDir;
	Ogre::AnimationState* 	mtpAnimationState;
	OGRE3DBody* 			mtpHitBox;
	Ogre::SceneNode*		mtpCharNode; 
	Ogre::SceneNode*		mtpDieNode;
	Ogre::Entity* 			mtpCharEnt;
	Ogre::Vector3			mtSpawnPoint;
	Ogre::Vector3			mtFaceDirection;
	static std::vector<Character*> mtDynamicCharacters;

	Ogre::Real 	mtCharSpeedForward; 
	Ogre::Real 	mtCharSpeedBackward; 
	Ogre::Real 	mtCharSpeedStrafeLeft; 
	Ogre::Real 	mtCharSpeedStrafeRight; 

	virtual void die();
};
#endif