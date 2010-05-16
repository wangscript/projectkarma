#include "Character.h"

std::vector<Character*> Character::mtDynamicCharacters;

Character::Character(Ogre::SceneManager* sceneMgr,NxOgre::Scene* physScene,OGRE3DRenderSystem* renderSystem, Ogre::String filename, 
		  Ogre::String name, Ogre::Vector3 spawnPoint , float scale, float hp , float walkSpeed, int collisionGroup)
{
	Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().load(filename, "Karma",
		Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);
	// build tangent vectors for our mesh
	unsigned short src, dest;
	if (!mesh->suggestTangentVectorBuildParams(Ogre::VES_TANGENT, src, dest))
	{
		mesh->buildTangentVectors(Ogre::VES_TANGENT, src, dest);
		// this version cleans mirrored and rotated UVs but requires quality models
		// mesh->buildTangentVectors(VES_TANGENT, src, dest, true, true);
	}
	mtpPhysicsScene = physScene;

	mtSpawnPoint = spawnPoint;
	Ogre::SceneNode* rootNode = sceneMgr->getRootSceneNode()->createChildSceneNode(name+ Ogre::String("RootNode"),spawnPoint);
	mtpCharNode = rootNode->createChildSceneNode(name + Ogre::String("CharNode"));

	mtpCharEnt = sceneMgr->createEntity(name+Ogre::String("Entity"), mesh->getName());
	mtpCharEnt->setCastShadows(true);//@todo
	mtpCharEnt->setMaterialName("mc");//@todo
	mtpCharNode->attachObject(mtpCharEnt);
	mtpCharNode->setScale(scale,scale,scale);

	//Initiate stuff
	mtpAnimationState= NULL;
	mtJumping=false;
	mtMoveDir = Game::Move_None;
	mtHP = hp;
	mtHPCur = hp;
	mtWalkSpeed = walkSpeed;
	mtFaceDirection = Ogre::Vector3(0,0,-1);
	mtDieRespawnTimer = 0.0;
	mtDieRespawnTime = 6.0;//@todo fixa

	//@todo inte bara capsule som suger ost
	//Importing NxOgre skeleton for our bounding capsule
	NxOgre::Mesh* convexMesh  = NxOgre::MeshManager::getSingleton()->load("nxogre:capsule.xns");
	NxOgre::Convex* convex = new NxOgre::Convex(convexMesh);

	//Properties for bounding capsule
	NxOgre::RigidBodyDescription description;
	//description.mMass = mtCharMassBoundCapsule; @todo
	description.mMass = 80;
	description.mBodyFlags |= NxOgre::Enums::BodyFlags_FreezeRotation; 

	//Add capsule to physics world and set Collision Flag to group 1
	mtpHitBox = renderSystem->createBody(convex, NxOgre::Vec3(spawnPoint), "capsule.mesh", description);
	
	mtpHitBox->getEntity()->setVisible(false);
	//So it doesn't change position of kinematic objects
	mtpHitBox->getNxActor()->setDominanceGroup(1);
	NxShape* const* s = mtpHitBox->getNxActor()->getShapes();
	(*s)->setGroup(collisionGroup);
}

Character* Character::loopDynamicCharacters(NxActor* actor)
{
	for (std::vector<Character*>::const_iterator it = mtDynamicCharacters.begin(); it != mtDynamicCharacters.end();it++)
	{
		if (actor == (*it)->mtpHitBox->getNxActor())
			return *it;		
	}
	return 0;
}
void Character::changeAnimation(const Ogre::String& name,const double time,bool loop)
{
	{
		if (mtpAnimationState)
		{
			mtpAnimationState->setEnabled(false);
			mtpAnimationState->setLoop(false);
		}

		Ogre::AnimationState* animState;

		if (mtJumping)
			animState = mtpCharEnt->getAnimationState("Jump");
		else
		{
			animState = mtpCharEnt->getAnimationState(name);
			animState->setLoop(loop);
		}

		animState->setEnabled(true);
		mtpAnimationState = animState;
		mtpAnimationState->addTime(time);

		if (mtJumping && mtpAnimationState->getTimePosition() >= mtpAnimationState->getLength())
		{
			mtJumping=false;
			mtpAnimationState->setTimePosition(0);
		}
	}
}

void Character::jump()
{
	mtJumping = true;
	mtpHitBox->addForce(NxOgre::Vec3(0,30000,0),NxOgre::Enums::ForceMode_Force);
}

void Character::debugMode()
{
	if (mtpHitBox->getEntity()->isVisible())
	{
		mtpHitBox->getEntity()->setVisible(false);
	}
	else
	{
		mtpHitBox->getEntity()->setVisible(true);
	}
}

void Character::updatePosition()
{
	//Get position of capsule and set the characters position to the capsule's position.
	NxOgre::Vec3 hitBoxPos = mtpHitBox->getGlobalPosition();
	//Move the "RootNode". CHARACTER_ADJUST_Y , translate node if orgin is not in origo
	mtpCharNode->getParentSceneNode()->setPosition(hitBoxPos.x ,hitBoxPos.y,hitBoxPos.z);
}

float Character::updateHp(float minusHP)
{
	mtHPCur -= minusHP;
	if (mtHPCur <= 0)
		die();
	return mtHPCur;
}

void Character::die()
{
	std::cout << "Dead, respawning";
	mtHPCur = mtHP;
	mtDying=true;	
	//mtpHitBox->getNxActor()->raiseActorFlag(NxActorFlag::);
	//mtpHitBox->setGlobalPosition(NxOgre::Vec3(mtSpawnPoint));
}

void Character::rotateCharacter(Ogre::SceneNode* sceneNode,const Ogre::Vector3& dest, const Ogre::Vector3& originalDir)
{
	Ogre::Vector3 src = sceneNode->getOrientation()  * originalDir;
	Ogre::Quaternion quat = src.getRotationTo(dest);
	sceneNode->rotate(quat);
}

bool Character::updateDead(const double& timeSinceLastFrame)
{
	changeAnimation("Die",timeSinceLastFrame/5,false); // /10 för snabb i blender@todo
	mtDieRespawnTimer+=timeSinceLastFrame;
	if (mtDieRespawnTimer >= mtDieRespawnTime)
	{
		mtpAnimationState->setTimePosition(0);
		std::cout << "Respawning";
		mtpHitBox->setGlobalPosition(NxOgre::Vec3(mtSpawnPoint));
		mtDieRespawnTimer = 0.0;
		mtDying=false;
		mtHPCur = mtHP;
		return true;
	}
	return false;
}