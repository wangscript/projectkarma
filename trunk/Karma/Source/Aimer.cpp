#include "Aimer.h"

NxOgre::Scene*	Aimer::mvpPhysicsScene = 0;

Aimer::~Aimer()
{
	delete mvpMuzzleFire1;
	delete mvpMuzzleFire2;
	delete mvpBlood;
}

void Aimer::setNodes(Ogre::SceneNode* charNode,Ogre::SceneManager* sceneMgr,const Ogre::String& name)
{
	mtvCharNode = charNode;
	mtvGunTrackNode = sceneMgr->getSceneNode("CamOrginNode")->createChildSceneNode(name + Ogre::String("GunTrackNode"),
		Ogre::Vector3(0.0f,GameFramework::getSingletonPtr()->mpSettings->mCamCharYAdjust, 
		GameFramework::getSingletonPtr()->mpSettings->mCamGunTrackerDistanceZ ));
	mvpName = &name;
	mvpSceneMgr = sceneMgr;
}

void Aimer::setBones(Ogre::Entity* charEnt,Ogre::String& strArmR, Ogre::String& strArmL,Ogre::String& strHead)
{
	mvpCharEnt = charEnt;
	mtpBoneArmRight = charEnt->getSkeleton()->getBone(strArmR);
	mtpBoneArmLeft = charEnt->getSkeleton()->getBone(strArmL);
	mtpBoneHead = charEnt->getSkeleton()->getBone(strHead);
}
void Aimer::updateBones(const OIS::MouseEvent &arg)
{
	setManuallyControlled(true);
	RotateBone(mtpBoneArmRight);
	mtpBoneArmRight->pitch(Ogre::Degree(-90));

	RotateBone(mtpBoneArmLeft);
	mtpBoneArmLeft->pitch(Ogre::Degree(-90));

	RotateBone(mtpBoneHead);
	mtpBoneHead->yaw(Ogre::Degree(180));
}

void Aimer::destroyNodeTracks(Ogre::Animation* animation)
{
	animation->destroyNodeTrack(mtpBoneArmRight->getHandle());
	animation->destroyNodeTrack(mtpBoneArmLeft->getHandle());
	animation->destroyNodeTrack(mtpBoneHead->getHandle());
}

void Aimer::addGun(const Ogre::String& hand1,Ogre::Quaternion rot1,Ogre::Vector3 offset1, const Ogre::String& hand2,Ogre::Quaternion rot2,Ogre::Vector3 offset2)
{
	Ogre::Entity* gunEnt1 = mvpSceneMgr->createEntity(*mvpName + Ogre::String("Gun1"), "pistol.mesh");
	gunEnt1->setMaterialName("2 - Default");//@todo fixa
	Ogre::Entity* gunEnt2 = mvpSceneMgr->createEntity(*mvpName + Ogre::String("Gun2"), "pistol.mesh");
	gunEnt2->setMaterialName("2 - Default");//@todo fixa

	mvpCharEnt->attachObjectToBone(hand1,gunEnt1,rot1,offset1); 
	mvpCharEnt->attachObjectToBone(hand2,gunEnt2,rot2,offset2);
}

void Aimer::addMuzzleFire(const Ogre::String& hand1,Ogre::Quaternion rot1,Ogre::Vector3 offset1, const Ogre::String& hand2,Ogre::Quaternion rot2,Ogre::Vector3 offset2,bool addToChunk, GridData chunk)
{
	mvpMuzzleFire1 = new MuzzleFire(0.1,mvpSceneMgr,1,1,"muzze",*mvpName + Ogre::String("MuzzleFire1"));
	mvpMuzzleFire2 = new MuzzleFire(0.1,mvpSceneMgr,1,1,"muzze",*mvpName+ Ogre::String("MuzzleFire2"));

	mvpCharEnt->attachObjectToBone(hand1,mvpMuzzleFire1->getBillboardSet(),rot1,offset1); 
	mvpCharEnt->attachObjectToBone(hand2,mvpMuzzleFire2->getBillboardSet(),rot2,offset2);

	if (addToChunk)
	{
		Chunks::getSingletonPtr()->addStaticEffects(mvpMuzzleFire1,chunk);
		Chunks::getSingletonPtr()->addStaticEffects(mvpMuzzleFire2,chunk);
	}
	else
	{
		Effects::addDynamicEffect(mvpMuzzleFire1);
		Effects::addDynamicEffect(mvpMuzzleFire2);
	}
}

void Aimer::addBloodParticleSystem(bool addToChunk, GridData chunk)
{
	mvpBlood = new Blood(0.2,mvpSceneMgr,"BloodPS","blood");
	if (addToChunk)
		Chunks::getSingletonPtr()->addStaticEffects(mvpBlood,chunk);
	else
		Effects::addDynamicEffect(mvpBlood);
}

void Aimer::fire(const Ogre::Vector3& start, const Ogre::Vector3& dir,int filterGroup)
{
	mvpMuzzleFire1->startTimer();
	mvpMuzzleFire2->startTimer();
	GameFramework::getSingletonPtr()->mpSound->playSound(GameFramework::getSingletonPtr()->mpSound->mpGun,mtvCharNode->_getDerivedPosition());

	//GameFramework::getSingletonPtr()->mpSound->updateListenerPos(charNode->_getDerivedPosition());


	// float screenWidth = GameFramework::getSingletonPtr()->mpRenderWnd->getWidth();
	// float screenHeight = GameFramework::getSingletonPtr()->mpRenderWnd->getHeight();
	// Ogre::Ray mouseRay;
	//if (center==true)
	// mouseRay = mCamera->getCameraToViewportRay(0.5,0.5);
	// else
	//     mouseRay = mCamera->getCameraToViewportRay(e.state.X.abs/screenWidth,e.state.Y.abs/screenHeight);

	//t = 10. The ray direction * t. Moves the box from camera to a distance.
	//Since mMoveBoxController only is a reference, this will affect the moveBox().
	//Get orgin and direction of ray*/
	NxOgre::Vec3 rayDirection(dir);
	NxOgre::Vec3 rayOrgin(start);
	//NxOgre Raycast. Only hitting dynamic objects . No intersecting with collision group 1
	//@todo fix collision groups


	const NxU32 avoid = 1 << filterGroup;
	NxOgre::RaycastHit rayCastResult = mvpPhysicsScene->raycastClosestShape(NxOgre::Ray(rayOrgin,rayDirection),NxOgre::Enums::ShapesType_All,~avoid);
	// expoNode->setPosition(rayCastResult.mWorldImpact.as<Ogre::Vector3>());
	//  GameFramework::getSingletonPtr()->mpSound->playSound("Explosion Deep Echo 02.wav", rayCastResult.mWorldImpact.as<Ogre::Vector3>());

	//   std::cout << "\n SKOTTET TRÄFFADE PA POSITION " << rayCastResult.mWorldImpact.x << " " << rayCastResult.mWorldImpact.y << " " << rayCastResult.mWorldImpact.z;
	//   std::cout << "\n DISTANS = " << rayCastResult.mDistance;
	//GameFramework::getSingletonPtr()->mpSound->playSound("Explosion Deep Echo 02.wav", rayCastResult.mWorldImpact.as<Ogre::Vector3>());
	if (rayCastResult.mDistance < 100 && rayCastResult.mRigidBody->isDynamic())
	{
		Character* hitChar = Chunks::getSingletonPtr()->searchForNPCinChunk(rayCastResult.mRigidBody->getNxActor());
		if (!hitChar)
			hitChar = Character::loopDynamicCharacters(rayCastResult.mRigidBody->getNxActor());
		if (hitChar && !hitChar->isDead())
		{
			GameFramework::getSingletonPtr()->mpSound->playSound(GameFramework::getSingletonPtr()->mpSound->mpSplash,rayCastResult.mWorldImpact.as<Ogre::Vector3>());
			GameFramework::getSingletonPtr()->mpSound->playSound(GameFramework::getSingletonPtr()->mpSound->mpBodyHit,rayCastResult.mWorldImpact.as<Ogre::Vector3>());
			hitChar->updateHp(10.0);
			mvpBlood->setPosition(rayCastResult.mWorldImpact.as<Ogre::Vector3>());
			mvpBlood->startTimer();
		}
		else if (!hitChar)
			GameFramework::getSingletonPtr()->mpSound->playSound(GameFramework::getSingletonPtr()->mpSound->mpWood,rayCastResult.mWorldImpact.as<Ogre::Vector3>());
		std::cout << "TRAFF!!!!";
		NxOgre::Vec3 force = rayCastResult.mWorldImpact - NxOgre::Vec3(mtvCharNode->_getDerivedPosition());
		force.normalise();
		force = force*10000;
		rayCastResult.mRigidBody->getNxActor()->addForce(NxVec3(force.x,force.y,force.z));
	}
}

void Aimer::setInitialOrientation()
{
	mtpBoneArmRight->setOrientation(mtpBoneArmRight->getInitialOrientation());
	mtpBoneArmLeft->setOrientation(mtpBoneArmLeft->getInitialOrientation());
	mtpBoneHead->setOrientation(mtpBoneHead->getInitialOrientation());
}

void Aimer::setManuallyControlled(bool state)
{
	mtpBoneArmRight->setManuallyControlled(state);
	mtpBoneArmLeft->setManuallyControlled(state);
	mtpBoneHead->setManuallyControlled(state);
}

void Aimer::RotateBone(Ogre::Bone* bone)
{
	Ogre::Vector3 between = mtvGunTrackNode->_getDerivedPosition() - mtvCharNode->_getDerivedPosition();

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