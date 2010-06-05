#include "Character.h"
#include "Chunks.h"

std::vector<Character*> Character::mtDynamicCharacters;
/*---------------------------------------------------------------------------------*/
Character::Character(Ogre::SceneManager* sceneMgr,NxOgre::Scene* physScene,OGRE3DRenderSystem* renderSystem, Ogre::String filename, 
					 Ogre::String name, Ogre::Vector3 spawnPoint , float scale, float hp , float walkSpeed, int collisionGroup) : 
	mtpPhysicsScene(physScene),	
	mtSpawnPoint(spawnPoint),
	mtpAnimationState(NULL),
	mtJumping(false),
	mtDying(false),
	mtMoveDir(Game::Move_None),
	mtHP(hp),
	mtHPCur(mtHP),
	mtWalkSpeed(walkSpeed),
	mtFaceDirection(&Settings::getSingletonPtr()->mCharFaceDirection),
	mtDieRespawnTimer(0.0f),
	mtDieRespawnTime(&Settings::getSingletonPtr()->mCharRespawnTime),
	mtFreezeAfterDieTimer(&Settings::getSingletonPtr()->mCharFreezeAfterDieTimer),
	mtFallingSpeed(0.0f),
	mtJumpForceY(&Settings::getSingletonPtr()->mCharJumpForceY),
	mtCharSpeedForward(&Settings::getSingletonPtr()->mCharSpeedForward), 
	mtCharSpeedBackward(&Settings::getSingletonPtr()->mCharSpeedBackward), 
	mtCharSpeedStrafeLeft(&Settings::getSingletonPtr()->mCharSpeedStrafeLeft),
	mtCharSpeedStrafeRight(&Settings::getSingletonPtr()->mCharSpeedStrafeRight),
	mtWorldSize(&Settings::getSingletonPtr()->mTerrainWorldSize),
	mtChunksWidth(Chunks::getSingletonPtr()->getChunksWidth()) 	
{
	//Loads the mesh and calculates tangents if needed.
	Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().load(filename, "Game",
		Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);
	unsigned short src, dest;
	if (!mesh->suggestTangentVectorBuildParams(Ogre::VES_TANGENT, src, dest))
		mesh->buildTangentVectors(Ogre::VES_TANGENT, src, dest);

	//Every character has unique Root- and CharNodes.
	Ogre::SceneNode* rootNode = sceneMgr->getRootSceneNode()->createChildSceneNode(name+ Ogre::String("RootNode"),spawnPoint);
	mtpCharNode = rootNode->createChildSceneNode(name + Ogre::String("CharNode"));
	
	//Attach mesh as an entity to charNode
	mtpCharEnt = sceneMgr->createEntity(name+Ogre::String("Entity"), mesh->getName());
	mtpCharEnt->setMaterialName("mc");//@todo
	mtpCharNode->attachObject(mtpCharEnt);
	mtpCharNode->setScale(scale,scale,scale);

	//Properties for bounding capsule
	NxOgre::RigidBodyDescription descriptionCapsule;
	descriptionCapsule.mMass = *Settings::getSingletonPtr()->mCharCapsuleMass;
	descriptionCapsule.mBodyFlags |= NxOgre::Enums::BodyFlags_FreezeRotation;

	//Add capsule to physics world and set Collision Group
	float radius = Settings::getSingletonPtr()->mCharCapsuleRadius;
	float height = Settings::getSingletonPtr()->mCharCapsuleHeight;
	mtCharOffsetY = (height+radius*2)/2;
	mtpHitBox = renderSystem->createBody(new NxOgre::Capsule(radius,height),
		NxOgre::Vec3(spawnPoint + Ogre::Vector3(0,mtCharOffsetY,0)), "CharCapsule.mesh", descriptionCapsule);
	mtHitBoxGlobalOrientation = mtpHitBox->getNxActor()->getGlobalOrientation();
	NxShape* const* s = mtpHitBox->getNxActor()->getShapes();
	(*s)->setGroup(collisionGroup);
	//So that it doesn't change position of kinematic objects
	mtpHitBox->getNxActor()->setDominanceGroup(1);

	//These nodes are being used when the character dies and falls to the ground
	mtpDieNode = mtpHitBox->getSceneNode()->createChildSceneNode(name + Ogre::String("DieNode"),Ogre::Vector3(0,-(height+radius*2)/2,0));
	mtpDieNode->setScale(scale,scale,scale);

	//Properties for bounding sphere that's going to visualize the head.	
	NxOgre::RigidBodyDescription descriptionSphere;
	descriptionSphere.mMass = *Settings::getSingletonPtr()->mCharSphereMass;
	descriptionSphere.mBodyFlags |= NxOgre::Enums::BodyFlags_FreezeRotation;
	descriptionSphere.mBodyFlags |= NxOgre::Enums::BodyFlags_DisableGravity;
	mtpHitBoxHead = renderSystem->createBody(new NxOgre::Sphere(Settings::getSingletonPtr()->mCharSphereRadius), NxOgre::Vec3(spawnPoint + Ogre::Vector3(0,Settings::getSingletonPtr()->mCharSpherePosY,0)), "CharSphere.mesh", descriptionSphere);//@todo fixa sphere

	mtpHitBoxHead->getNxActor()->setDominanceGroup(1);
	NxShape* const* s2 = mtpHitBoxHead->getNxActor()->getShapes();
	(*s2)->setGroup(collisionGroup);
	std::cout << "\n\n\nCollisionGRoup : " << collisionGroup;

	mtpHitBox->getEntity()->setVisible(false);	
	mtpHitBoxHead->getEntity()->setVisible(false);

	#ifdef DEBUG_SHOW_HITBOX
	mtpHitBox->getEntity()->setVisible(true);	
	mtpHitBoxHead->getEntity()->setVisible(true);
	#endif

	//Create a joint between head and body so that we will only have to update the body.
	NxOgre::FixedJointDescription desciptionJoint;
	NxOgre::Joint* joint = mtpHitBox->getScene()->createFixedJoint( mtpHitBoxHead, mtpHitBox, desciptionJoint );
}
/*---------------------------------------------------------------------------------*/
Character* Character::loopDynamicCharacters(NxActor* actor)
{
	//Sometimes we might want to loop through characters that don't have been assigned a "chunk".
	//If the PhysX actor is the same as the paramter, the character is returned otherwise an empty pointer.
	for (std::vector<Character*>::const_iterator it = mtDynamicCharacters.begin(); it != mtDynamicCharacters.end();it++)
	{
		if (actor == (*it)->mtpHitBox->getNxActor())
			return *it;		
	}
	return 0;
}
/*---------------------------------------------------------------------------------*/
void Character::changeAnimation(const Ogre::String& name,const double time,bool loop)
{
	//Setting these to false if there is an AnimationState, in case a new State will be loaded
	if (mtpAnimationState)
	{
		mtpAnimationState->setEnabled(false);
		mtpAnimationState->setLoop(false);
	}

	//Load the animationState. If the Character is jumping,override with the jumping animation
	Ogre::AnimationState* animState;
		animState = mtpCharEnt->getAnimationState(name);
		//Some animation don't want to be looped
		animState->setLoop(loop);

	animState->setEnabled(true);
	//Switch (to a maybe new) state; 
	mtpAnimationState = animState;
	//Add time so the animation forwards.
	mtpAnimationState->addTime(time);

	//Special case. If jumping and the jumping animation is done, reset mtJumping.
	//if (mtJumping && mtpCharEnt->getAnimationState("Jump")->getTimePosition() >= mtpCharEnt->getAnimationState("Jump")->getLength())
	//{
		//mtpAnimationState->setTimePosition(0);
	//}
}
/*---------------------------------------------------------------------------------*/
void Character::jump()
{
	//Just not jumping already, add force on the positve Y-axis
	if (!mtJumping)
	{
		std::cout << "\nJumping!\n";
		mtJumping = true;
		mtpHitBox->addForce(NxOgre::Vec3(0,*mtJumpForceY,0),NxOgre::Enums::ForceMode_Force);
	}
}
/*---------------------------------------------------------------------------------*/
void Character::debugMode()
{
	//Toggles DebugMode on and off
	if (mtpHitBox->getEntity()->isVisible())
	{
		mtpHitBox->getEntity()->setVisible(false);
		mtpHitBoxHead->getEntity()->setVisible(false);
	}
	else
	{
		mtpHitBox->getEntity()->setVisible(true);
		mtpHitBoxHead->getEntity()->setVisible(true);
	}
}
/*---------------------------------------------------------------------------------*/
void Character::updatePosition()
{
	//Get position of capsule and set the characters position to the capsule's position.
	NxOgre::Vec3 hitBoxPos = mtpHitBox->getGlobalPosition();
	//Move the "RootNode". mtCharOffsetY , translate node if orgin is not in origo
	mtpCharNode->getParentSceneNode()->setPosition(hitBoxPos.x ,hitBoxPos.y-mtCharOffsetY,hitBoxPos.z);
}
/*---------------------------------------------------------------------------------*/
float Character::updateHp(float minusHP)
{
	//Removes hitpoints. If less than zero, the character dies.
	mtHPCur -= minusHP;
	if (mtHPCur <= 0)
		die();
	return mtHPCur;
}
/*---------------------------------------------------------------------------------*/
void Character::die()
{
	//Is a virtual function, can (and should) be overwritten
	std::cout << "Dead, respawning";
	mtHPCur = mtHP;
	mtDying=true;
}
/*---------------------------------------------------------------------------------*/
void Character::rotateCharacter(Ogre::SceneNode* sceneNode,const Ogre::Vector3& dest, const Ogre::Vector3& originalDir)
{
	//Makes the character face a certain direction, i.e the camToChar direction or maybe charToGunTrack direction (first Person)
	
	//Get offset from original orientation
	Ogre::Vector3 src = sceneNode->getOrientation()  * originalDir;
	//A quaternion that represents the difference
	Ogre::Quaternion quat = src.getRotationTo(dest);
	//Rotate!
	sceneNode->rotate(quat);
}
/*---------------------------------------------------------------------------------*/
bool Character::updateDead(const double& timeSinceLastFrame)
{
	//Update the "Die" animation. Currently no animation added 2010-05-17
	//changeAnimation("Die",timeSinceLastFrame,false); // /10 för snabb i blender@todo

	//Update the respawntimer
	mtDieRespawnTimer+=timeSinceLastFrame;

	//If the capsule ever gets positive velocity on the Y axis after "mtFreezeAfterDieTimer" seconds, it means
	//that it has hit the ground already. Friction can then make it shake on the ground.
	//Solution: setPosition = frozen.
	if (mtpHitBox->getLinearVelocity().y > 0 && mtDieRespawnTimer>*mtFreezeAfterDieTimer)
		mtpHitBox->getNxActor()->raiseBodyFlag(NX_BF_FROZEN);

	//If the timwer has reached the respawn time => Respawn
	if (mtDieRespawnTimer >= *mtDieRespawnTime)
	{
		std::cout << "Respawning";
		respawn();

		//Return respawn = true
		return true;
	}
	//Not respawned yet, return false,
	return false;
}

void Character::respawn()
{
		//Manually moving the physics actor to their spawn points.
		mtpHitBox->setGlobalPosition(NxOgre::Vec3(mtSpawnPoint + Ogre::Vector3(0,mtCharOffsetY,0)));
		mtpHitBoxHead->setGlobalPosition(NxOgre::Vec3(mtSpawnPoint + Ogre::Vector3(0,Settings::getSingletonPtr()->mCharSpherePosY,0)));
		//Important to set the orientation to the original one (since it is now lies on the ground)
		mtpHitBox->getNxActor()->setGlobalOrientation(mtHitBoxGlobalOrientation);

		//Detach the character from the dieNode and attach to the normal mtpCharNode.
		//For more information about DieNode, see NPC::Die()
		mtpCharEnt->detachFromParent();
		mtpCharNode->attachObject(mtpCharEnt);
		
		//Make capsule able to move again
		mtpHitBox->getNxActor()->clearBodyFlag(NX_BF_FROZEN_POS);
		
		//Reset timers and HP
		mtDieRespawnTimer = 0.0;
		mtDying=false;
		mtHPCur = mtHP;

		//Finally updates the position of the character to the capsule
		updatePosition();
}

GridData Character::worldToChunk(const float& x, const float& z)
{
	//Move the coordinate system to only have positive values
	double newX = x + *mtWorldSize/2.0;
	double newY = -z + *mtWorldSize/2.0;
	
	//Scales the coordinate system
	newX = (newX*mtChunksWidth)/ *mtWorldSize;
	newY = (newY*mtChunksWidth)/ *mtWorldSize;

	//Rounds the double to an int.
	newX = std::floor(newX + 0.5);
	newY = std::floor(newY + 0.5);
	return GridData(newX,newY);
}

