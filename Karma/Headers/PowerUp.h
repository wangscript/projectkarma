/*---------------------------------------------------------------------------------*/
/* File: PowerUp.h																   */
/* Author: Per Karlsson, perkarlsson89@gmail.com								   */
/*																				   */
/* Description:	PowerUp is a class that handles the PowerUps barrels that are	   */
/* located in the world.			  											   */
/*---------------------------------------------------------------------------------*/

#ifndef POWERUP_H
#define POWERUP_H

#include "Player.h"
#include "GameCommon.h"

class PowerUp: public NxOgre::Callback
{
public:

	/* A struct that binds a NxOgre volume to string, so Ogre knows what Entity to hide later. */
	struct struct_PowerUp
	{
		NxOgre::Volume* volume;
		Ogre::String	name;
	};

	PowerUp(NxOgre::Scene*,Ogre::SceneManager*,NxOgre::RigidBody*, Player*);

	/* Adds a PowerUp barrel at position p and with material s. */ 
	void addPowerUp(const Ogre::Vector3 &p,const Ogre::String& s);

	/* The event that triggers everytime the player runs into a PowerUp Barrel. */ 
	void onVolumeEvent(NxOgre::Volume* volume, NxOgre::Shape* volumeShape, NxOgre::RigidBody* rigidBody,
		NxOgre::Shape* rigidBodyShape, unsigned int collisionEvent);

private:

	std::vector<struct_PowerUp>		mvPowerUps;
	NxOgre::Scene*					mvpScene;
	Ogre::SceneManager*				mvpSceneMgr;
	NxOgre::RigidBody*				mvpCapsule;
};

#endif