#ifndef POWERUP_H
#define POWERUP_H
#include <Ogre.h>
#include <NxOgre.h>
#include <NxOgreOGRE3D.h>
#include "Character.h"

class PowerUp: public NxOgre::Callback
{
public:
	typedef struct
	{
		NxOgre::Volume* volume;
		Ogre::String	name;
	} struct_PowerUp;

	enum PowerUpEnums
	{
		PowerUp_None = 0, 
		PowerUp_SuperJump = 1,
		PowerUp_SuperSpeed = 2,
		PowerUp_MoveBox = 3
	};

	PowerUp(NxOgre::Scene*,Ogre::SceneManager*,NxOgre::RigidBody*, Character*);
	virtual ~PowerUp();
	virtual void addPowerUp(Ogre::Vector3&,Ogre::String);
	virtual void onVolumeEvent(NxOgre::Volume* volume, NxOgre::Shape* volumeShape, NxOgre::RigidBody* rigidBody,
		NxOgre::Shape* rigidBodyShape, unsigned int collisionEvent);

protected:

	std::vector<struct_PowerUp>		mPowerUps;
	NxOgre::Scene*					mScene;
	Ogre::SceneManager*				mSceneMgr;
	NxOgre::RigidBody*				mCapsule;
	Character*						mCharacter;
};

#endif