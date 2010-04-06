#ifndef CHARACTER_H
#define CHARACTER_H
#include <Ogre.h>
#include "AppState.hpp"//Singelton usage
#include <NxOgreOGRE3D.h>
#include <NxActor.h>
#include <NxScene.h>
#include <NxUserContactReport.h>

#define BOUNDING_CAPSULE_MASS	80
#define SUPERSPEED				5
#define FORWARD_SPEED			3
#define BACKWARD_SPEED			-1
#define STRAFE_LEFT_SPEED		1.5
#define STRAFE_RIGHT_SPEED		1.5
#define CHARACTER_ADJUST_Y		0.5
#define CHARACTER_ADJUST_X		-0.2
#define CHARACTER_ADJUST_Z		-0.1
#define CHARACTER_SCALE			0.2 //For ogre.mesh 0.2 is good
#define CHARACTER_ROTATION		180 //Turns the character 180 degrees

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
	bool jumping;
	int mPowerUp;
	int mMoveDir;

public:
	Character(Ogre::SceneManager*,OGRE3DRenderSystem*);
	virtual ~Character();
	virtual void setPowerUp(const int& p){mPowerUp = p;};
	virtual void removePowerUp(){mPowerUp = PowerUp_None;};
	virtual int getPowerUp(){return mPowerUp;};
	virtual bool move(const double& timeSinceLastFrame);
	virtual void setMoveDirection (const int& d){mMoveDir = d;};
	virtual void jump();
	virtual void changeAnimation(const Ogre::String& name,const double time);
	virtual bool isJumping() {return jumping;};
	virtual void debugMode();
	virtual void moveBox(const OIS::MouseEvent &e);
	virtual void releaseBox();
};
#endif

