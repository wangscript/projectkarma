#ifndef POWERUP_H
#define POWERUP_H

#include "Player.h"
#include "GameCommon.h"

class PowerUp: public NxOgre::Callback
{
public:
	typedef struct
	{
		NxOgre::Volume* volume;
		Ogre::String	name;
	} struct_PowerUp;

	PowerUp(NxOgre::Scene*,Ogre::SceneManager*,NxOgre::RigidBody*, Player*);
	~PowerUp();
	void addPowerUp(Ogre::Vector3&,Ogre::String);
	void onVolumeEvent(NxOgre::Volume* volume, NxOgre::Shape* volumeShape, NxOgre::RigidBody* rigidBody,
		NxOgre::Shape* rigidBodyShape, unsigned int collisionEvent);

protected:

	std::vector<struct_PowerUp>		mPowerUps;
	NxOgre::Scene*					mScene;
	Ogre::SceneManager*				mSceneMgr;
	NxOgre::RigidBody*				mCapsule;
	Player*							mPlayer;
};

#endif