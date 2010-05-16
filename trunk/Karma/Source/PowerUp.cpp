#include "PowerUp.h"

PowerUp::PowerUp(NxOgre::Scene* r, Ogre::SceneManager* m,NxOgre::RigidBody* c, Player* p)
{
	//Initate stuff. 
	//@todo If time, fix singletons. These variables are being linked quite a lot
	mScene = r;
	mSceneMgr = m;
	mCapsule = c;
	mPlayer = p;
}

PowerUp::~PowerUp()
{
}

void PowerUp::addPowerUp(Ogre::Vector3 &p, Ogre::String s)
{
	//Creating NxOgre Box Volume object. Local Callback method.
	NxOgre::Volume* mVolume = mScene->createVolume(new NxOgre::Box(1.5), NxOgre::Matrix44(NxOgre::Vec3(p)),
		this, NxOgre::Enums::VolumeCollisionType_All);

	//Stores information in struct_PowerUp. Useful later for check what PowerUpBox that was entered.
	struct_PowerUp pwrup;
	pwrup.volume = mVolume;
	pwrup.name = s;

	//Puts it at the end of the mPowerUps std::vector
	mPowerUps.push_back(pwrup);
}

void PowerUp::onVolumeEvent(NxOgre::Volume* volume, NxOgre::Shape* volumeShape, NxOgre::RigidBody* rigidBody,
							NxOgre::Shape* rigidBodyShape, unsigned int collisionEvent)
{
	//If the bounding capsule didn't trigger the event -> break
	if (rigidBody==mCapsule)
	{
		//For the PowerUp enums;
		int powerUpFlag = 1;

		//Creating iterator for the vector and loops through until it finds the correct PowerUp
		std::vector<struct_PowerUp>::iterator itr;
		for ( itr = mPowerUps.begin(); itr != mPowerUps.end(); ++itr )
		{
			//Information in the struct class!
			if (itr->volume == volume)
			{
				//Hide the Ogre Entity.
				mSceneMgr->getEntity(itr->name)->setVisible(false);
				//@todo Action!!!!!! Färger hit å dit tjarå
				mPlayer->setPowerUp(powerUpFlag);
				GameFramework::getSingletonPtr()->mpGui->changeIngameUIIcon(powerUpFlag);

				//When once collected, there is no need to keep looking for more events.
				itr->volume->removeCallback();
				break;
			}
			powerUpFlag++;
		}

	}
}