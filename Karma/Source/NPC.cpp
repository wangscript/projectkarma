/*---------------------------------------------------------------------------------*/
/* File: NPC.cpp																   */
/* Author: Per Karlsson, perkarlsson89@gmail.com								   */
/*																				   */
/* Description:	NPC is the class between the Character and the derived NPC		   */
/* classes (i.e NPCAimer). It involves some standard functions for reseting		   */
/* the NPC.			  															   */
/*---------------------------------------------------------------------------------*/

#include "NPC.h"
#include "Chunks.h"

/*---------------------------------------------------------------------------------*/
/*									STATIC										   */
/*---------------------------------------------------------------------------------*/
Ogre::SceneNode* NPC::playerNode = 0;
/*---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*/
/*									PUBLIC										   */
/*---------------------------------------------------------------------------------*/
NPC::NPC(Ogre::SceneManager* sceneMgr,NxOgre::Scene* physScene,OGRE3DRenderSystem* renderSystem, Ogre::String filename, 
		 Ogre::String name, Ogre::Vector3 spawnPoint, float scale, float hp , float walkSpeed)
		 : Character(sceneMgr,physScene,renderSystem,filename,name,spawnPoint,scale,hp,walkSpeed,Game::CollisionGroup_NPC),
	mtReseting(false),
	mtUpdateTimer(0)
{
	//Calculate what chunk the NPC belongs to
	mtChunk = worldToChunk(spawnPoint.x,spawnPoint.z);

	//Adds the NPC to its chunk.
	std::cout << "NPC at CHUNK("<<mtChunk.x <<","<<mtChunk.y<< ")\n";
	Chunks::getSingletonPtr()->addStaticNPC(this,mtChunk);
}
/*---------------------------------------------------------------------------------*/
bool NPC::moveReset(const double& timeSinceLastFrame)
{
	//Updates the position here. Not as accurate as the method Player uses, but works fine.
	//See GameState::update to see how Player need two calls each update. One before and after physics timestep.
	updatePosition();

	//Get the direction to the spawnpoint
	Ogre::Vector3 dirNPCtoSpawnPoint = mtSpawnPoint - mtpCharNode->_getDerivedPosition();

	//Instead of length, use this approximation to check if the NPC is in range close enough of the spawn point
	//If so, the reset is done.
	if ((dirNPCtoSpawnPoint.x > -0.1 && dirNPCtoSpawnPoint.x < 0.1) && (dirNPCtoSpawnPoint.z > -0.1 && dirNPCtoSpawnPoint.z < 0.1))
	{
		mtReseting = false;
		//Returns true and tells the NPCHandler to delete it from a temporary Vector of reseting NPCs.
		return true;
	}
	//No movement in Y direciton
	dirNPCtoSpawnPoint.y = 0;
	//Movement velocity not dependent of the distance between Character and Camera.
	dirNPCtoSpawnPoint.normalise();

	//Make the NPC always face the spawn point when moving
	rotateCharacter(mtpCharNode,dirNPCtoSpawnPoint,*mtFaceDirection);

	//Animare and move the box
	changeAnimation("Walk", timeSinceLastFrame);
	mtpHitBox->setLinearVelocity(NxOgre::Vec3(dirNPCtoSpawnPoint.x*mtWalkSpeed ,mtpHitBox->getLinearVelocity().y,dirNPCtoSpawnPoint.z*mtWalkSpeed ));	
	
	//Returning false == no reset yet.
	return false;
}
/*---------------------------------------------------------------------------------*/
void NPC::reset()
{
	//If not reseting already, active reset.
	if (!mtReseting)
	{
		std::cout << "\nReseting: " << mtpCharEnt->getName() << ". Orginal chunk = " << mtChunk.x << "," << mtChunk.y;
		mtReseting = true;
	}
}
/*---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*/
/*									PROTECTED									   */
/*---------------------------------------------------------------------------------*/
void NPC::die()
{
	std::cout << "Dead";

	//Play a die sound
	GameFramework::getSingletonPtr()->mpSound->playSound("../Media/Sounds/die.ogg", mtpCharNode->_getDerivedPosition());

	//Active the dead variable
	mtDying=true;
	
	//Attach the entity to the dieNode instead of charNode. The die node will fall to the ground along with the body capsule.
	mtpCharNode->detachObject(mtpCharEnt);
	mtpDieNode->attachObject(mtpCharEnt);

	//Clearing PhysX body flags of the capsule to make it fall!
	//Important to re-enable these once the capsule is active again
	mtpHitBox->getNxActor()->clearBodyFlag(NX_BF_FROZEN_ROT_X);
	mtpHitBox->getNxActor()->clearBodyFlag(NX_BF_FROZEN_ROT_Z);
	mtpHitBox->getNxActor()->setAngularDamping(1);

	//Makes the NPC face the player when it dies.
	Ogre::Vector3 dir=  playerNode->_getDerivedPosition() - mtpCharNode->_getDerivedPosition();
	dir.y = 0;
	rotateCharacter(mtpDieNode,dir,*mtFaceDirection);

	//Adds the dead NPC to a vector in the NPC Manager
	NPCHandler::getSingletonPtr()->addDeadNPC(this);
}
/*---------------------------------------------------------------------------------*/