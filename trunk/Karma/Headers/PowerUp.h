#ifndef POWERUP_H
#define POWERUP_H

#include "Player.h"
#include "GameCommon.h"

class PowerUp: public NxOgre::Callback
{
public:
	struct struct_PowerUp
	{
		NxOgre::Volume* volume;
		Ogre::String	name;
	};

	PowerUp(NxOgre::Scene*,Ogre::SceneManager*,NxOgre::RigidBody*, Player*);
	void addPowerUp(const Ogre::Vector3 &p,const Ogre::String& s);
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