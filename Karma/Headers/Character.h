#ifndef CHARACTER_H
#define CHARACTER_H
#include <Ogre.h>
#include "AppState.h"//Singelton usage
#include <NxOgreOGRE3D.h>
#include <NxActor.h>
#include <NxScene.h>
#include <NxUserContactReport.h>


class Character
{
public:

	enum PowerUp
	{
		PowerUp_None = 0, 
		PowerUp_SuperJump = 1,
		PowerUp_SuperSpeed = 2,
		PowerUp_MoveBox = 3
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
	virtual ~Character();
	virtual void setPowerUp(const int& p,float time = 0.0f);
	virtual void updateCastingBar(const double& timeSinceLastFrame);
	virtual float getCastingBarTimer(){return mCastingBarTimer;};
	virtual void removePowerUp(){mPowerUp = PowerUp_None;};
	virtual NxOgre::RigidBody* getCapsule(){return mCapsule;};
	virtual int getPowerUp(){return mPowerUp;};
	virtual bool move(const double& timeSinceLastFrame);
	virtual void setMoveDirection (const int& d){mMoveDir = d;};
	virtual void jump();
	virtual void changeAnimation(const Ogre::String& name,const double time);
	virtual bool isJumping() {return jumping;};
	virtual void debugMode();
	virtual void moveBoxMoved(const OIS::MouseEvent &e);
	virtual void moveBoxPressed(const OIS::MouseEvent &e);
	virtual void releaseBox();
};
#endif

