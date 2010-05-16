#include "NPC.h"
#include "Chunks.h"

Ogre::SceneNode* NPC::playerNode = 0;

NPC::NPC(Ogre::SceneManager* sceneMgr,NxOgre::Scene* physScene,OGRE3DRenderSystem* renderSystem, Ogre::String filename, 
		 Ogre::String name, Ogre::Vector3 spawnPoint, float scale, float hp , float walkSpeed)
: Character(sceneMgr,physScene,renderSystem,filename,name,spawnPoint,scale,hp,walkSpeed)
{
	mtReseting = false;
	mtDying = false;
	//flytta coordinat
	double worldSize = GameFramework::getSingletonPtr()->mpSettings->mTerrainWorldSize;
	double x = spawnPoint.x + worldSize/2.0;
	double y = spawnPoint.z - worldSize/2.0;
	//skala
	x = (x*Chunks::getSingletonPtr()->getChunksWidth())/worldSize;
	y = -(y*Chunks::getSingletonPtr()->getChunksWidth())/worldSize;
	//avrunda
	mtChunk.x = std::floor(x + 0.5);
	mtChunk.y = std::floor(y + 0.5);

	std::cout << "NPC at CHUNK("<<mtChunk.x <<","<<mtChunk.y<< ")\n";
	Chunks::getSingletonPtr()->addStaticNPC(this,mtChunk);
}

void NPC::move(const double& timeSinceLastFrame)
{
	if (mtReseting || mtDying)
		return;
	updatePosition();
	bool move=true;
	Ogre::Vector3 dirNPCtoChar = playerNode->_getDerivedPosition() - mtpCharNode->_getDerivedPosition();
	if (dirNPCtoChar.length() < 2)
		move=false;
	dirNPCtoChar.y = 0;
	//Movement velocity not dependent of the distance between Character and Camera.
	dirNPCtoChar.normalise();
	//Make the NPC always face the player when moving
	rotateCharacter(mtpCharNode,dirNPCtoChar,mtFaceDirection);
	changeAnimation("Walk", timeSinceLastFrame);
	if (move)
		mtpHitBox->setLinearVelocity(NxOgre::Vec3(dirNPCtoChar.x*mtWalkSpeed ,mtpHitBox->getLinearVelocity().y,dirNPCtoChar.z*mtWalkSpeed ));		
}

bool NPC::moveReset(const double& timeSinceLastFrame)
{
	updatePosition();
	Ogre::Vector3 dirNPCtoSpawnPoint = mtSpawnPoint - mtpCharNode->_getDerivedPosition();
	if ((dirNPCtoSpawnPoint.x > -0.1 && dirNPCtoSpawnPoint.x < 0.1) && (dirNPCtoSpawnPoint.z > -0.1 && dirNPCtoSpawnPoint.z < 0.1))
	{
		mtReseting = false;
		return true;
	}
	dirNPCtoSpawnPoint.y = 0;
	//Movement velocity not dependent of the distance between Character and Camera.
	dirNPCtoSpawnPoint.normalise();
	//Make the NPC always face the player when moving
	rotateCharacter(mtpCharNode,dirNPCtoSpawnPoint,mtFaceDirection);
	changeAnimation("Walk", timeSinceLastFrame);
	mtpHitBox->setLinearVelocity(NxOgre::Vec3(dirNPCtoSpawnPoint.x*mtWalkSpeed ,mtpHitBox->getLinearVelocity().y,dirNPCtoSpawnPoint.z*mtWalkSpeed ));	
	return false;
}

void NPC::reset()
{
	if (!mtReseting)
	{
		std::cout << "\nReseting: " << mtpCharEnt->getName() << ". Orginal chunk = " << mtChunk.x << "," << mtChunk.y;
		mtReseting = true;
		//Chunks::addTempNPC(this);
	}
}

void NPC::die()
{
	std::cout << "Dead";
	GameFramework::getSingletonPtr()->mpSound->playSound("die.wav", mtpCharNode->_getDerivedPosition());
	mtDying=true;
/*		const NxU32 avoid = 1 << 3;
	NxOgre::RaycastHit rayCastResult = mtpPhysicsScene->raycastClosestShape(NxOgre::Ray(NxOgre::Vec3(mtpCharNode->_getDerivedPosition() + Ogre::Vector3(0,3,0) ),NxOgre::Vec3(0,-1,0)),NxOgre::Enums::ShapesType_All,~avoid);
		Ogre::Vector3 hitNormal = rayCastResult.mWorldNormal.as<Ogre::Vector3>();
		hitNormal.x = 0;
		rotateCharacter(mtpCharNode,hitNormal,Ogre::Vector3(0,1,0));*/

	//mtpCharNode->detachObject(mtpCharEnt);
	//mtpDieNode->attachObject(mtpCharEnt);
	//rotateCharacter(mtpDieNode,playerNode->_getDerivedPosition() - mtpCharNode->_getDerivedPosition(),mtFaceDirection);
	//	mtpHitBox->getNxActor()->clearBodyFlag(NX_BF_FROZEN_ROT); 
		//std::cout << "Normal underneath the dead character: " << hitNormal.x << " " << hitNormal.y << " " << hitNormal.z;
	NPCHandler::getSingletonPtr()->addDeadNPC(this);
	//mtpHitBox->getNxActor()->raiseActorFlag(NxActorFlag::);
}