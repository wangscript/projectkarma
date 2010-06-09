/*---------------------------------------------------------------------------------*/
/* File: Aimer.cpp																   */
/* Author: Per Karlsson, perkarlsson89@gmail.com								   */
/*																				   */
/* Description:	Aimer is a class that fire weapons and uses raycasting to          */
/* determine if the shot was a hit or not. It also has functions for manually      */
/* rotating bones in a characters skeleton.										   */
/*---------------------------------------------------------------------------------*/

#include "Aimer.h"

/*---------------------------------------------------------------------------------*/
/*									STATIC										   */
/*---------------------------------------------------------------------------------*/
NxOgre::Scene*	Aimer::mvpPhysicsScene = 0;
/*---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*/
/*									PUBLIC										   */
/*---------------------------------------------------------------------------------*/
Aimer::Aimer():
mtContinuousShoot(false),
mvpFireDistanceMax(&Settings::getSingletonPtr()->mAimerFireDistanceMax),
mvpFireDistanceMin(&Settings::getSingletonPtr()->mAimerFireDistanceMin),
mvpShotDmgNormal(&Settings::getSingletonPtr()->mAimerShotDmgNormal),
mvpShotDmgHeadshot(&Settings::getSingletonPtr()->mAimerShotDmgHeadshot),
mvpShotForce(&Settings::getSingletonPtr()->mAimerShotForce)
{}
/*---------------------------------------------------------------------------------*/
Aimer::~Aimer()
{
	delete mvpMuzzleFire1;
	delete mvpMuzzleFire2;
	delete mvpBlood;
}
/*---------------------------------------------------------------------------------*/
void Aimer::setInitialOrientation()
{
	//Set the bones to their initial orientation. Used when going from manually controlled to controlled
	mtpBoneArmRight->setOrientation(mtpBoneArmRight->getInitialOrientation());
	mtpBoneArmLeft->setOrientation(mtpBoneArmLeft->getInitialOrientation());
	mtpBoneHead->setOrientation(mtpBoneHead->getInitialOrientation());
}
/*---------------------------------------------------------------------------------*/
void Aimer::setManuallyControlled(bool state)
{
	//Activte/Deactivates ManuallyControlled. Going to/from GunMode.
	mtpBoneArmRight->setManuallyControlled(state);
	mtpBoneArmLeft->setManuallyControlled(state);
	mtpBoneHead->setManuallyControlled(state);
}
/*---------------------------------------------------------------------------------*/
void Aimer::updateBones(Ogre::SceneNode* aimNode)
{
	//Sets the bones to be manually controlled.
	setManuallyControlled(true);

	//Rotates the bone and makes them face the aimNode
	//Hacks :(
	RotateBone(mtpBoneArmRight,aimNode);
	mtpBoneArmRight->pitch(Ogre::Degree(-90));//haaaacks

	RotateBone(mtpBoneArmLeft,aimNode);
	mtpBoneArmLeft->pitch(Ogre::Degree(-90));//haaaacks

	RotateBone(mtpBoneHead,aimNode);
	mtpBoneHead->yaw(Ogre::Degree(180));//haaaacks
}
/*---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*/
/*									PROTECTED									   */
/*---------------------------------------------------------------------------------*/
void Aimer::addBloodParticleSystem(bool addToChunk, GridData chunk)
{
	//Each Aimer has a Blood Particle System that will be started when an enemy has been hitted.
	mvpBlood = new Blood(0.2,mvpSceneMgr,*mvpName,"blood");

	//See Aimer::addMuzzleFire
	if (addToChunk)
		Chunks::getSingletonPtr()->addStaticEffects(mvpBlood,chunk);
	else
		Effects::addDynamicEffect(mvpBlood);
}
/*---------------------------------------------------------------------------------*/
void Aimer::addGun(const Ogre::String& hand1,Ogre::Quaternion rot1,Ogre::Vector3 offset1, const Ogre::String& hand2,Ogre::Quaternion rot2,Ogre::Vector3 offset2)
{
	//Adds two pistols the hands. Not so general. @todo...
	mtpGun1Ent= mvpSceneMgr->createEntity(*mvpName + Ogre::String("Gun1"), "pistol.mesh");
	mtpGun1Ent->setMaterialName("gunmaterial");//@todo fixa
	mtpGun2Ent = mvpSceneMgr->createEntity(*mvpName + Ogre::String("Gun2"), "pistol.mesh");
	mtpGun2Ent->setMaterialName("gunmaterial");//@todo fixa

	mvpCharEnt->attachObjectToBone(hand1,mtpGun1Ent,rot1,offset1); 
	mvpCharEnt->attachObjectToBone(hand2,mtpGun2Ent,rot2,offset2);

	mtpGun1Ent->setVisible(false);
	mtpGun2Ent->setVisible(false);
}
/*---------------------------------------------------------------------------------*/
void Aimer::addMuzzleFire(const Ogre::String& hand1,Ogre::Quaternion rot1,Ogre::Vector3 offset1, const Ogre::String& hand2,Ogre::Quaternion rot2,Ogre::Vector3 offset2,bool addToChunk, GridData chunk)
{
	//Creates two Effects: muzzle Fire
	mvpMuzzleFire1 = new MuzzleFire(0.1,mvpSceneMgr,1,1,"muzze",*mvpName + Ogre::String("MuzzleFire1"));
	mvpMuzzleFire2 = new MuzzleFire(0.1,mvpSceneMgr,1,1,"muzze",*mvpName+ Ogre::String("MuzzleFire2"));

	//Attach them to the bones
	mvpCharEnt->attachObjectToBone(hand1,mvpMuzzleFire1->getBillboardSet(),rot1,offset1); 
	mvpCharEnt->attachObjectToBone(hand2,mvpMuzzleFire2->getBillboardSet(),rot2,offset2);

	//If added from NPC, add to Chunk and static effects
	if (addToChunk)
	{
		Chunks::getSingletonPtr()->addStaticEffects(mvpMuzzleFire1,chunk);
		Chunks::getSingletonPtr()->addStaticEffects(mvpMuzzleFire2,chunk);
	}
	//If added from a Dynamic character, i.e Player, add to dynamic effects vector.
	else
	{
		Effects::addDynamicEffect(mvpMuzzleFire1);
		Effects::addDynamicEffect(mvpMuzzleFire2);
	}
}
/*---------------------------------------------------------------------------------*/
void Aimer::destroyNodeTracks(Ogre::Animation* animation)
{
	//Destroy the node tracks for an animation. Must be done if the animation has key nodes for the bones in this animation
	std::cout << "\nDestroying" << animation->getName() <<"\n";
	animation->destroyNodeTrack(mtpBoneArmRight->getHandle());
	animation->destroyNodeTrack(mtpBoneArmLeft->getHandle());
	animation->destroyNodeTrack(mtpBoneLowArmRight->getHandle());
	animation->destroyNodeTrack(mtpBoneLowArmLeft->getHandle());
	animation->destroyNodeTrack(mtpBoneHead->getHandle());
}
/*---------------------------------------------------------------------------------*/
void Aimer::fire(const Ogre::Vector3& start, const Ogre::Vector3& dir,int filterGroup)
{
	//Show both muzzle fires
	mvpMuzzleFire1->startTimer();
	mvpMuzzleFire2->startTimer();

	//Play pistol sound.
	GameFramework::getSingletonPtr()->mpSound->playSound(GameFramework::getSingletonPtr()->mpSound->mpGun,mtvCharNode->_getDerivedPosition());

	//Create PhysX Ray
	NxOgre::Vec3 rayDirection(dir);
	NxOgre::Vec3 rayOrgin(start);

	//NxOgre Raycast. ~filterGroup means it will not hit the CollisionGroups in filterGroup
	NxOgre::RaycastHit rayCastResult = mvpPhysicsScene->raycastClosestShape(NxOgre::Ray(rayOrgin,rayDirection),NxOgre::Enums::ShapesType_All,~filterGroup);

	//If the distance is less than 100m and the hit is a dynamic rigidbody
	if (rayCastResult.mDistance < *mvpFireDistanceMax && rayCastResult.mRigidBody->isDynamic())
	{
		bool headshot = false;

		//Search through the chunk to see if a  NPC was hit or not
		Character* hitChar = Chunks::getSingletonPtr()->searchForNPCinChunk(rayCastResult.mRigidBody->getNxActor(),headshot);
		//If not, loop the dynamicCharacters.
		if (!hitChar)
		{
			std::cout << "\nIngen Character hittades i chunk. Letar nu bland dynamiska";
			hitChar = Character::loopDynamicCharacters(rayCastResult.mRigidBody->getNxActor());
		}
		//If the hitChar pointer is != 0 and the character is not dead
		if (hitChar && !hitChar->isDead())
		{
			//Play splash and bodyhit sound
			GameFramework::getSingletonPtr()->mpSound->playSound(GameFramework::getSingletonPtr()->mpSound->mpSplash,rayCastResult.mWorldImpact.as<Ogre::Vector3>());
			GameFramework::getSingletonPtr()->mpSound->playSound(GameFramework::getSingletonPtr()->mpSound->mpBodyHit,rayCastResult.mWorldImpact.as<Ogre::Vector3>());

			//Activate Blood particle system
			mvpBlood->setPosition(rayCastResult.mWorldImpact.as<Ogre::Vector3>());
			mvpBlood->startTimer();

			//If the mtpHitBoxHead was hit, then headshot = true!
			if (headshot)
			{
				GameFramework::getSingletonPtr()->mpSound->playSound(GameFramework::getSingletonPtr()->mpSound->mpHeadShot,mtvCharNode->_getDerivedPosition());
				hitChar->updateHp(*mvpShotDmgHeadshot);
			}
			//Normal shot. Less cool.
			else
				hitChar->updateHp(*mvpShotDmgNormal);
		}
		//If the rigidbody wasn't a character
		else if (!hitChar)
			//GameFramework::getSingletonPtr()->mpSound->playSound(GameFramework::getSingletonPtr()->mpSound->mpWood,rayCastResult.mWorldImpact.as<Ogre::Vector3>());

			std::cout << "TRAFF!!!!";

		//Adds force
		NxOgre::Vec3 force = rayCastResult.mWorldImpact - NxOgre::Vec3(mtvCharNode->_getDerivedPosition());
		force.normalise();
		force = force * *mvpShotForce;
		rayCastResult.mRigidBody->getNxActor()->addForce(NxVec3(force.x,force.y,force.z));
	}
	//If the hit wasnt dynamic, i.e the terrain
	else if (rayCastResult.mDistance > *mvpFireDistanceMin)
		BulletHoles::getSingletonPtr()->addBulletHole(rayCastResult.mWorldNormal.as<Ogre::Vector3>(),rayCastResult.mWorldImpact.as<Ogre::Vector3>());

	std::cout << rayCastResult.mWorldImpact.x << " " <<rayCastResult.mWorldImpact.y  << " " << rayCastResult.mWorldImpact.z;
}


/*---------------------------------------------------------------------------------*/
void Aimer::setBones(Ogre::Entity* charEnt,Ogre::String& strUpArmR, Ogre::String& strUpArmL, Ogre::String& strLowArmL, Ogre::String& strLowArmR,Ogre::String& strHead)
{
	//Pointers to the entity and the bones.
	mvpCharEnt = charEnt;
	mtpBoneArmRight = charEnt->getSkeleton()->getBone(strUpArmR);
	mtpBoneArmLeft = charEnt->getSkeleton()->getBone(strUpArmL);
	mtpBoneLowArmRight = charEnt->getSkeleton()->getBone(strLowArmR);
	mtpBoneLowArmLeft = charEnt->getSkeleton()->getBone(strLowArmL);
	mtpBoneHead = charEnt->getSkeleton()->getBone(strHead);
}



/*---------------------------------------------------------------------------------*/
void Aimer::setNodes(Ogre::SceneNode* charNode,Ogre::SceneManager* sceneMgr,const Ogre::String& name)
{
	//Links stuff from child
	mtvCharNode = charNode;
	mvpName = &name;
	mvpSceneMgr = sceneMgr;
	mtContinuousShoot = false;
	mtContinuousShootTimer = 0.2;

	//This node is unique for Aimer. Used for aiming
	mtvGunTrackNode = sceneMgr->getSceneNode("CamOrginNode")->createChildSceneNode(name + Ogre::String("GunTrackNode"),
		Ogre::Vector3(0.0f,Settings::getSingletonPtr()->mCamCharYAdjust, 
		Settings::getSingletonPtr()->mCamGunTrackerOffsetZ ));
}
/*---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*/
/*									PRIVATE										   */
/*---------------------------------------------------------------------------------*/
void Aimer::RotateBone(Ogre::Bone* bone, Ogre::SceneNode* aimNode)
{
	//Ok, don't understand this one 100%
	//This is a modified version from the : http://www.ogre3d.org/wiki/index.php/Make_A_Character_Look_At_The_Camera
	// But with minor tweaks.. Some of the math is just .....

	Ogre::Vector3 between = aimNode->_getDerivedPosition() - mtvCharNode->_getDerivedPosition();

	between = between;//what is the unit vector
	Ogre::Vector3 unitBetween = between.normalisedCopy();

	Ogre::Node * neckBone = bone->getParent();
	Ogre::Quaternion neckBoneWorldOrientation = mtvCharNode->_getDerivedOrientation() * neckBone->_getDerivedOrientation();
	Ogre::Vector3 headForward = unitBetween;
	Ogre::Vector3 neckUp;
	neckUp = neckBoneWorldOrientation.yAxis();
	Ogre::Vector3 headRight = neckUp.crossProduct(headForward);
	Ogre::Vector3 headUp = headForward.crossProduct(headRight);

	Ogre::Quaternion rot(-headRight,headUp,-headForward);
	rot.normalise(); //might have gotten messed up

	rot = neckBoneWorldOrientation.Inverse()*rot;
	bone->setOrientation(rot);
}
/*---------------------------------------------------------------------------------*/