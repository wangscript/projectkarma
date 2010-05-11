#ifndef CHARACTER_H
#define CHARACTER_H
#include <Ogre.h>
#include "AppState.h"//Singelton usage
#include <NxOgreOGRE3D.h>
#include <NxActor.h>
#include <NxScene.h>
#include <NxUserContactReport.h>

//Test
//WTF!?!?!?

class Character
{
public:

	enum PowerUp
	{
		PowerUp_None = 0, 
		PowerUp_SuperJump = 1,
		PowerUp_SuperSpeed = 2,
		PowerUp_MoveBox = 3,
		PowerUp_GunMode = 4
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

protected:
	Ogre::Entity* charEnt;           
	Ogre::SceneNode* charNode; 
	Ogre::SceneNode* camCollisionNode;
	Ogre::SceneNode* camGunModeNode;
	Ogre::SceneNode* firstPersonGunTrackerNode;
	Ogre::SceneNode* mixGunTrackerNode;

	OGRE3DBody* mCapsule;
	OGRE3DKinematicBody* mMoveBoxController;
	NxOgre::FixedJoint* mMoveBoxJoint;
	Ogre::AnimationState* m_AnimationState;
	Ogre::Camera* mCamera;
	NxOgre::Vec3 moveBoxControllerTranslate;
	float mCastingBarTimer;
	bool jumping;
	int mPowerUp;
	int mMoveDir;

	Ogre::BillboardSet* muzzle1;
	Ogre::BillboardSet* explosion;
	Ogre::BillboardSet* muzzle2;
	Ogre::SceneNode* expoNode;
	double muzzleTimer,expoTimer,particleTimer;
	int bulleHoles;
	void RotateBone(Ogre::Bone* bone);
	Ogre::SceneManager* mSceneMgr;
	void createBulletHole(const Ogre::Vector3 &normal,const Ogre::Vector3 &pos);
	
	Ogre::Bone * rightArmBone;
	Ogre::Bone * leftArmBone;
	Ogre::Bone * headBone;
	Ogre::Bone * hipBone;

	Ogre::Real 	mtCharMassBoundCapsule;
	Ogre::Real 	mtCharSpeedSuper;
	Ogre::Real 	mtCharSpeedForward; 
	Ogre::Real 	mtCharSpeedBackward; 
	Ogre::Real 	mtCharSpeedStrafeLeft; 
	Ogre::Real 	mtCharSpeedStrafeRight; 
	Ogre::Real 	mtCharAdjustPosX;
	Ogre::Real 	mtCharAdjustPosY; 
	Ogre::Real 	mtCharAdjustPosZ; 
	Ogre::Real 	mtCharScale; 
	Ogre::Real 	mtCharRotation; 

public:
	Character(Ogre::SceneManager*,OGRE3DRenderSystem*);
	~Character();
	void setPowerUp(const int& p,float time = 0.0f);
	void updateCastingBar(const double& timeSinceLastFrame);
	float getCastingBarTimer(){return mCastingBarTimer;};
	void removePowerUp(){mPowerUp = PowerUp_None;};
	OGRE3DBody* getCapsule(){return mCapsule;};
	int getPowerUp(){return mPowerUp;};
	Ogre::Entity* getEntity(){return charEnt;};
	bool move(const double& timeSinceLastFrame);
	void setMoveDirection (const int& d){mMoveDir = d;};
	void jump();
	void changeAnimation(const Ogre::String& name,const double time);
	bool isJumping() {return jumping;};
	void debugMode();
	void moveBoxMoved(const OIS::MouseEvent &e);
	void moveBoxPressed(const OIS::MouseEvent &e);
	void releaseBox();
	void updatePosition();

	void updateMuzzleFlash(const double& timeSinceLastFrame);
	void toggleMuzzleFlash(const OIS::MouseEvent &arg, bool center = false);
	void updateBones(const OIS::MouseEvent &arg);
};
#endif

