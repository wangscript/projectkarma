/*---------------------------------------------------------------------------------*/
/* File: PowerUp.cpp															   */
/* Author: Per Karlsson, perkarlsson89@gmail.com								   */
/*																				   */
/* Description:	PowerUp is a class that handles the PowerUps barrels that are	   */
/* located in the world.			  											   */
/*---------------------------------------------------------------------------------*/

#include "PowerUp.h"

PowerUp::PowerUp(NxOgre::Scene* r, Ogre::SceneManager* m,NxOgre::RigidBody* c, Player* p)
{
	//Initate stuff. 
	mvpScene = r;
	mvpSceneMgr = m;
	mvpCapsule = c;
}

void PowerUp::addPowerUp(const Ogre::Vector3 &p,const Ogre::String& s)
{
	//Creates the entity and attaches it to a node.
	Ogre::SceneNode* pwrUpNode = mvpSceneMgr->getRootSceneNode()->createChildSceneNode();
	Ogre::Entity* pwrUpEnt = mvpSceneMgr->createEntity(s, "PowerUpBarrel.mesh");
	pwrUpNode->attachObject(pwrUpEnt);
	pwrUpNode->setPosition(p);

	//Set the correct texture by adding an extra layer
	pwrUpEnt->setMaterialName(s);
	Ogre::MaterialPtr mat = (Ogre::MaterialPtr)Ogre::MaterialManager::getSingleton().getByName(s);
	Ogre::Pass *pass = mat->getTechnique(0)->createPass();
	pass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
	Ogre::TextureUnitState *texState = pass->createTextureUnitState(s + Ogre::String(".png"));

	//Creating NxOgre Box Volume object. Local Callback method.
	NxOgre::Volume* mVolume = mvpScene->createVolume(new NxOgre::Box(1.5), NxOgre::Matrix44(NxOgre::Vec3(p)),
		this, NxOgre::Enums::VolumeCollisionType_OnEnter);

	//Stores information in struct_PowerUp. Useful later for check what PowerUpBox that was entered.
	struct_PowerUp pwrup;
	pwrup.volume = mVolume;
	pwrup.name = s;

	//Puts it at the end of the mvPowerUps std::vector
	mvPowerUps.push_back(pwrup);
}

void PowerUp::onVolumeEvent(NxOgre::Volume* volume, NxOgre::Shape* volumeShape, NxOgre::RigidBody* rigidBody,
							NxOgre::Shape* rigidBodyShape, unsigned int collisionEvent)
{
	//If the bounding capsule didn't trigger the event -> break
	if (rigidBody==mvpCapsule)
	{
		//For the PowerUp enums;
		int powerUpFlag = 1;

		//Creating iterator for the vector and loops through until it finds the correct PowerUp
		std::vector<struct_PowerUp>::iterator itr;
		for ( itr = mvPowerUps.begin(); itr != mvPowerUps.end(); ++itr )
		{
			//Information in the struct class!
			if (itr->volume == volume)
			{
				//Hide the Ogre Entity.
				mvpSceneMgr->getEntity(itr->name)->setVisible(false);

				//Update Action Bar GUI.
				GameFramework::getSingletonPtr()->mpGui->updateActionBarElement(1 << powerUpFlag,Game::ActionBar_Normal);

				//Play sound.
				GameFramework::getSingletonPtr()->mpSound->playSound("../Media/Sounds/powerup.ogg", itr->volume->getGlobalPosition().as<Ogre::Vector3>());
				
				//When once collected, there is no need to keep looking for more events.
				itr->volume->removeCallback();
				break;
			}
			powerUpFlag++;
		}
	}
}