/*---------------------------------------------------------------------------------*/
/* File: NPCAimer.cpp															   */
/* Author: Per Karlsson, perkarlsson89@gmail.com								   */
/*																				   */
/* Description:	NPCAimer is a class used for NPCs that can shoot. Inherits from    */
/* both the NPC and Aimer class.			  									   */
/*---------------------------------------------------------------------------------*/

#include "NPCAimer.h"

/*---------------------------------------------------------------------------------*/
/*									PUBLIC										   */
/*---------------------------------------------------------------------------------*/
NPCAimer::NPCAimer(Ogre::SceneManager* sceneMgr,NxOgre::Scene* physScene,OGRE3DRenderSystem* renderSystem, Ogre::String filename, 
				   Ogre::String name, Ogre::Vector3 spawnPoint , float scale, float hp , float walkSpeed) : NPC
				   (sceneMgr,physScene,renderSystem,filename,name,spawnPoint,scale,hp,walkSpeed),
			mvShootFreq(0),
			mvShootTimer(0),
			mvDirNPCtoChar(Ogre::Vector3(0,0,0)),
			mvpRangeLong(&Settings::getSingletonPtr()->mNPCAimerRangeLong),
			mvpRangeShort(&Settings::getSingletonPtr()->mNPCAimerRangeShort),
			mvpRangeLongFreq(&Settings::getSingletonPtr()->mNPCAimerRangeLongFreq),
			mvpRangeShortFreq(&Settings::getSingletonPtr()->mNPCAimerRangeShortFreq),
			mvpRandomFactor(&Settings::getSingletonPtr()->mNPCAimerRandomness)
{
	//See Player::Player() for more info about the following lines.
	setNodes(mtpCharNode, sceneMgr,name);
	setBones(mtpCharEnt,Ogre::String("UpArm.L"), Ogre::String("UpArm.R"),Ogre::String("LowArm.L"),Ogre::String("LowArm.R"),Ogre::String("Head"));
	destroyNodeTracks(mtpCharEnt->getSkeleton()->getAnimation(0));

	Ogre::Vector3 offsetLeft(*Settings::getSingletonPtr()->mAimerOffsetVectorLX,*Settings::getSingletonPtr()->mAimerOffsetVectorLY,*Settings::getSingletonPtr()->mAimerOffsetVectorLZ);
	Ogre::Vector3 offsetRight(*Settings::getSingletonPtr()->mAimerOffsetVectorRX,*Settings::getSingletonPtr()->mAimerOffsetVectorRY,*Settings::getSingletonPtr()->mAimerOffsetVectorRZ);
	Ogre::Quaternion orientationLeft(*Settings::getSingletonPtr()->mAimerOffsetQuatLW,*Settings::getSingletonPtr()->mAimerOffsetQuatLX,*Settings::getSingletonPtr()->mAimerOffsetQuatLY,*Settings::getSingletonPtr()->mAimerOffsetQuatLZ);
	Ogre::Quaternion orientationRight(*Settings::getSingletonPtr()->mAimerOffsetQuatRW,*Settings::getSingletonPtr()->mAimerOffsetQuatRX,*Settings::getSingletonPtr()->mAimerOffsetQuatRY,*Settings::getSingletonPtr()->mAimerOffsetQuatRZ);

	addGun("Hand.R",orientationRight, offsetRight,
		"Hand.L",orientationLeft  ,offsetLeft );
	addMuzzleFire("Hand.R",orientationRight, offsetRight,
		"Hand.L",orientationLeft  ,offsetLeft);

	addBloodParticleSystem();
	setManuallyControlled(true);

	//Special attributes for the NPCAimer class.
	mvUpdateTimerFreq = 1 / Settings::getSingletonPtr()->mNPCUpdateTimerFreq;
	mtpCharEnt->setMaterialName("NPCAimer");

}
/*---------------------------------------------------------------------------------*/
void NPCAimer::update(const double& timeSinceLastFrame)
{
	if (mtReseting || mtDying)
		return;

	updatePosition();
	mvDirNPCtoChar = playerNode->_getDerivedPosition()  - mtpCharNode->_getDerivedPosition();

	mtUpdateTimer+=timeSinceLastFrame;
	if (mtUpdateTimer > mvUpdateTimerFreq)
	{
	mtUpdateTimer = 0;
	updateBones(playerNode);
	rotateCharacter(mtpCharNode,mvDirNPCtoChar - Ogre::Vector3(0,mvDirNPCtoChar.y,0),*mtFaceDirection);
	}

	//Is the NPC inside the "long" range? 
	//Yes -> Continue
	//No -> move
	if (!(mvDirNPCtoChar.x > -(*mvpRangeLong) && mvDirNPCtoChar.x < (*mvpRangeLong)&& mvDirNPCtoChar.z > -(*mvpRangeLong)&& mvDirNPCtoChar.z < (*mvpRangeLong)))
	{
		//No
		move(timeSinceLastFrame);
		mvShootFreq = 0;
	}
	else
	{
		//YES
		mvShootFreq = *mvpRangeLongFreq;

		//Is inside the "short" range ?
		// Yes -> ShootFreq fast
		// No -> Move
		if (!(mvDirNPCtoChar.x > -(*mvpRangeShort) && mvDirNPCtoChar.x < (*mvpRangeShort)&& mvDirNPCtoChar.z > -(*mvpRangeShort)&& mvDirNPCtoChar.z < (*mvpRangeShort)))
		{
			//No
			move(timeSinceLastFrame);
		}
		else
		{
			mvShootFreq = *mvpRangeShortFreq;
		}			
	}

	//Update the shootTimer if shootFrequency is enabled
	if (mvShootFreq != 0)
	{
		mvShootTimer += timeSinceLastFrame;
		queueFire();
	}

}
/*---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*/
/*									PRIVATE										   */
/*---------------------------------------------------------------------------------*/
void NPCAimer::die()
{
	std::cout << "Dead";

	NPCHandler::addKill();

	if (NPCHandler::getKills() == 9)
	GameFramework::getSingletonPtr()->mpSound->playSound("../Media/Sounds/godlike.ogg", playerNode->_getDerivedPosition());		
	else if(NPCHandler::getKills() == 6)
	GameFramework::getSingletonPtr()->mpSound->playSound("../Media/Sounds/ultrakill.ogg", playerNode->_getDerivedPosition());
	else if (NPCHandler::getKills() == 3)
	GameFramework::getSingletonPtr()->mpSound->playSound("../Media/Sounds/killingspree.ogg", playerNode->_getDerivedPosition());

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
void NPCAimer::move(const double& timeSinceLastFrame)
{
	//Animate the NPC
	changeAnimation("Walk", timeSinceLastFrame);

	//Move the Capsule
	Ogre::Vector3 dirNPCtoChar = mvDirNPCtoChar - mvDirNPCtoChar.y;
	dirNPCtoChar.normalise();
	mtpHitBox->setLinearVelocity(NxOgre::Vec3(dirNPCtoChar.x*mtWalkSpeed ,mtpHitBox->getLinearVelocity().y,dirNPCtoChar.z*mtWalkSpeed ));		
}
/*---------------------------------------------------------------------------------*/
void NPCAimer::queueFire()
{
	//If the mvShootTimer is bigger than the current frequency, shoot!
	if (mvShootTimer > mvShootFreq)
	{
		/*Offset the shot (Not being used 2010-05-03)
		Ogre::Vector3 destination = mtpCharNode->_getDerivedPosition() +
			Ogre::Vector3(-(*mvpRandomFactor)+ (std::rand() % int((*mvpRandomFactor)*2)),
			-(*mvpRandomFactor) + (std::rand() % int((*mvpRandomFactor)*2)),
			-(*mvpRandomFactor) + (std::rand() % int((*mvpRandomFactor)*2)));*/

		Ogre::Vector3 startPosition = mtpCharNode->_getDerivedPosition() + Ogre::Vector3(0,0.5,0);

		//Aimer::Fire. No collision with the MoveBoxReference or NPC collision group.
		fire(startPosition, mvDirNPCtoChar.normalisedCopy(),(1 << Game::CollisionGroup_NPC)|(1 << Game::CollisionGroup_MoveBoxReference)); 
		mvShootTimer = 0;
	}

}
/*---------------------------------------------------------------------------------*/
