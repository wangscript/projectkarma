#include "NPCAimer.h"

NPCAimer::NPCAimer(Ogre::SceneManager* sceneMgr,NxOgre::Scene* physScene,OGRE3DRenderSystem* renderSystem, Ogre::String filename, 
				   Ogre::String name, Ogre::Vector3 spawnPoint , float scale, float hp , float walkSpeed) : NPC
				   (sceneMgr,physScene,renderSystem,filename,name,spawnPoint,scale,hp,walkSpeed),
			mvShootFreq(0),
			mvShootTimer(0),
			mvNeedsMoveUpdate(false)
{
	setNodes(mtpCharNode, sceneMgr,name);
	setBones(mtpCharEnt,Ogre::String("UpArm.L"), Ogre::String("UpArm.R"),Ogre::String("Head"));
	destroyNodeTracks(mtpCharEnt->getSkeleton()->getAnimation(0)); //@todo 0 utbytt mot config
	addGun("Hand.R",Ogre::Quaternion(1,0,0,0), Ogre::Vector3(0,0.4,0),
		"Hand.L",Ogre::Quaternion(0,0,1,0)  ,Ogre::Vector3(0,0.4,0) );//@todo config...

	addMuzzleFire("Hand.R",Ogre::Quaternion(1,0,0,0), Ogre::Vector3(0,1.0,0),
		"Hand.L",Ogre::Quaternion(0,0,1,0)  ,Ogre::Vector3(0,1.0,0) );
	addBloodParticleSystem();
	setManuallyControlled(true);

	//BIG TODO. effekterna ska in i chunksen
}
/*---------------------------------------------------------------------------------*/
void NPCAimer::update(const double& timeSinceLastFrame)
{
	//Long = 5, Short = 3 @ todo
	if (mtReseting || mtDying)
		return;

	updatePosition();
	updateBones(playerNode);
	Ogre::Vector3 dirNPCtoChar = playerNode->_getDerivedPosition()  - mtpCharNode->_getDerivedPosition();
	rotateCharacter(mtpCharNode,dirNPCtoChar - Ogre::Vector3(0,dirNPCtoChar.y,0),mtFaceDirection);

	//Is the NPC inside the "long" range? 
	//Yes -> Continue
	//No -> move
	if (!(dirNPCtoChar.x > -5 && dirNPCtoChar.x < 5&& dirNPCtoChar.z > -5&& dirNPCtoChar.z < 5))
	{
		//No
		move(timeSinceLastFrame);
		mvShootFreq = 0;
	}
	else
	{
		//YES
		mvShootFreq = 1.5; //@todo vadå 3

		//Is inside the "short" range ?
		// Yes -> ShootFreq fast
		// No -> Move
		if (!(dirNPCtoChar.x > -3 && dirNPCtoChar.x < 3&& dirNPCtoChar.z > -3&& dirNPCtoChar.z < 3))
		{
			//No
			move(timeSinceLastFrame);
		}
		else
		{
			mvShootFreq = 0.5;
		}			
	}

	if (mvShootFreq != 0)
	{
		mvShootTimer += timeSinceLastFrame;
		queueFire(dirNPCtoChar.normalisedCopy());
	}

}
/*---------------------------------------------------------------------------------*/
void NPCAimer::queueFire(const Ogre::Vector3& dirNPCtoChar)
{
	if (mvShootTimer > mvShootFreq)
	{
		fire(mtpCharNode->_getDerivedPosition() + Ogre::Vector3(0,1.3,0), dirNPCtoChar,3); //3an är filter group @ todo ska fixas
		mvShootTimer = 0;
	}

}
/*---------------------------------------------------------------------------------*/
void NPCAimer::move(const double& timeSinceLastFrame)
{
	mvNeedsMoveUpdate=true;
	Ogre::Vector3 dirNPCtoChar = playerNode->_getDerivedPosition() - mtpCharNode->_getDerivedPosition();
	if (dirNPCtoChar.length() < 2)
		mvNeedsMoveUpdate=false;
	dirNPCtoChar.y = 0;
	//Movement velocity not dependent of the distance between Character and Camera.
	dirNPCtoChar.normalise();
	//Make the NPC always face the player when moving
	changeAnimation("Walk", timeSinceLastFrame);
	if (mvNeedsMoveUpdate)
		mtpHitBox->setLinearVelocity(NxOgre::Vec3(dirNPCtoChar.x*mtWalkSpeed ,mtpHitBox->getLinearVelocity().y,dirNPCtoChar.z*mtWalkSpeed ));		
	mvNeedsMoveUpdate = true;
}
/*---------------------------------------------------------------------------------*/
