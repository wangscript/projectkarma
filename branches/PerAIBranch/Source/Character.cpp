#include "Character.h"

Character::Character(Ogre::SceneManager* sceneMgr, OGRE3DRenderSystem* renderSystem)
{
	mtCharMassBoundCapsule	= GameFramework::getSingletonPtr()->mpSettings->mCharMassBoundCapsule;
	mtCharSpeedSuper		= GameFramework::getSingletonPtr()->mpSettings->mCharSpeedSuper;
	mtCharSpeedForward		= GameFramework::getSingletonPtr()->mpSettings->mCharSpeedForward; 
	mtCharSpeedBackward		= GameFramework::getSingletonPtr()->mpSettings->mCharSpeedBackward; 
	mtCharSpeedStrafeLeft	= GameFramework::getSingletonPtr()->mpSettings->mCharSpeedStrafeLeft; 
	mtCharSpeedStrafeRight	= GameFramework::getSingletonPtr()->mpSettings->mCharSpeedStrafeRight; 
	mtCharAdjustPosX		= GameFramework::getSingletonPtr()->mpSettings->mCharAdjustPosX;
	mtCharAdjustPosY		= GameFramework::getSingletonPtr()->mpSettings->mCharAdjustPosY; 
	mtCharAdjustPosZ		= GameFramework::getSingletonPtr()->mpSettings->mCharAdjustPosZ; 
	mtCharScale				= GameFramework::getSingletonPtr()->mpSettings->mCharScale; 
	mtCharRotation			= GameFramework::getSingletonPtr()->mpSettings->mCharRotation; 

	//Create Character and attach it  @ todo kunna välja mesh i config filen
	Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().load("HaloBlender.mesh", "Karma",
		Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);
	// build tangent vectors for our mesh
	unsigned short src, dest;
	if (!mesh->suggestTangentVectorBuildParams(Ogre::VES_TANGENT, src, dest))
	{
		std::cout << "\n\n\n BUILDING !!!! \n\n\n\n";
		mesh->buildTangentVectors(Ogre::VES_TANGENT, src, dest);
		// this version cleans mirrored and rotated UVs but requires quality models
		// mesh->buildTangentVectors(VES_TANGENT, src, dest, true, true);
	}
	charEnt = sceneMgr->createEntity(mesh->getName(), mesh->getName());
	charEnt->setCastShadows(true);
	charEnt->setMaterialName("mc");
	charNode = sceneMgr->getSceneNode("CharNode");
	charNode->translate(Ogre::Vector3(mtCharAdjustPosX,mtCharAdjustPosY,mtCharAdjustPosZ));
	charNode->yaw( Ogre::Degree( mtCharRotation ) );//Ogre.mesh "felvänd"
	charNode->attachObject(charEnt);
	charNode->setScale(mtCharScale,mtCharScale,mtCharScale);

	//Pointers to bones in the skeleton. @ todo alla namn ska väl finnas i en fin config fil
	rightArmBone = charEnt->getSkeleton()->getBone("UpArm.R");
	leftArmBone = charEnt->getSkeleton()->getBone("UpArm.L");
	headBone = charEnt->getSkeleton()->getBone("Head");
	hipBone = charEnt->getSkeleton()->getBone("Hip");

	//Destroys (if any) keys in the walk animation
	Ogre::Animation* walkAnimation = charEnt->getSkeleton()->getAnimation(0);
	walkAnimation->destroyNodeTrack(hipBone->getHandle());
	walkAnimation->destroyNodeTrack(headBone->getHandle());
	walkAnimation->destroyNodeTrack(leftArmBone->getHandle());
	walkAnimation->destroyNodeTrack(rightArmBone->getHandle());

	//Initiate stuff
	m_AnimationState= NULL;
	jumping=false;
	mMoveDir = Move_None;
	mCamera = GameFramework::getSingletonPtr()->mpViewport->getCamera();
	mMoveBoxJoint = NULL;

	//Pointer to cam node. Going to be used a lot
	camCollisionNode = sceneMgr->getSceneNode("CamCollisionNode");
	camGunModeNode = sceneMgr->getSceneNode("CamMixHelperNode");
	firstPersonGunTrackerNode = sceneMgr->getSceneNode("FirstPersonGunTrackerNode");
	mixGunTrackerNode = sceneMgr->getSceneNode("CursorMovableGunTrackerNode");


	//Importing NxOgre skeleton for our bounding capsule
	NxOgre::Mesh* convexMesh  = NxOgre::MeshManager::getSingleton()->load("nxogre:capsule.xns");
	NxOgre::Convex* convex = new NxOgre::Convex(convexMesh);

	//Properties for bounding capsule
	NxOgre::RigidBodyDescription description;
	description.mMass = mtCharMassBoundCapsule;
	description.mBodyFlags |= NxOgre::Enums::BodyFlags_FreezeRotation; 

	//Add capsule to physics world and set Collision Flag to group 1
	mCapsule = renderSystem->createBody(convex, NxOgre::Vec3(0, 0, 0), "capsule.mesh", description);
	mCapsule->getEntity()->setVisible(false);
	//So it doesn't change position of kinematic objects
	mCapsule->getNxActor()->setDominanceGroup(1);
	NxShape* const* s = mCapsule->getNxActor()->getShapes();
	//@todo fix enums for groups. bit operations useful maybe :)
	(*s)->setGroup(1);

	//Add a kinematic controller for the move box powerup. Position,size,mesh doesn't really matter. Only a reference.
	mMoveBoxController = renderSystem->createKinematicBody(new NxOgre::Box(1, 1,1), NxOgre::Vec3(0, 0, 0), "cube.1m.mesh");
	//Collision Group 1
	NxShape* const* ss = mMoveBoxController->getNxActor()->getShapes();
	(*ss)->setGroup(1);
	//Not making any other dynamic objects move
	mMoveBoxController->getNxActor()->raiseActorFlag(NX_AF_DISABLE_COLLISION);
	//Invisible of course.
	mMoveBoxController->getEntity()->setVisible(false);

	//MUZZLE AND SHIT @ BIG TODO HÄR. ser för jävligt ut :D
	mSceneMgr = sceneMgr;
	muzzleTimer = 0;
	muzzle1 = sceneMgr->createBillboardSet("bSet1");
	Ogre::Billboard* bill1 = muzzle1->createBillboard(Ogre::Vector3(0,0,0));
	bill1->setDimensions(1,1);
	muzzle1->setMaterialName("muzze");
	muzzle1->setVisible(false);
	muzzle2 = sceneMgr->createBillboardSet("bSet2");
	Ogre::Billboard* bill2 = muzzle2->createBillboard(Ogre::Vector3(0,0,0));
	bill2->setDimensions(1,1);
	muzzle2->setMaterialName("muzze");
	muzzle2->setVisible(false);

	expoTimer = 0;
	explosion = sceneMgr->createBillboardSet("exp");
	Ogre::Billboard* exp = explosion->createBillboard(Ogre::Vector3(0,0,0));
	exp->setDimensions(3,3);
	explosion->setMaterialName("expo");
	explosion->setVisible(false);
	expoNode = sceneMgr->getRootSceneNode()->createChildSceneNode("expoNode");
	expoNode->attachObject(explosion);


	Ogre::Entity* gunENt1 = sceneMgr->createEntity("Pistol1", "pistol.mesh");
	gunENt1->setMaterialName("2 - Default");
	Ogre::Entity* gunENt2 = sceneMgr->createEntity("Pistol2", "pistol.mesh");
	gunENt2->setMaterialName("2 - Default");

	charEnt->attachObjectToBone("Hand.R",gunENt1,Ogre::Quaternion(1,0,0,0),Ogre::Vector3(0,0.4,0));
	charEnt->attachObjectToBone("Hand.L",gunENt2,Ogre::Quaternion(0,0,1,0),Ogre::Vector3(0,0.4,0) );
	charEnt->attachObjectToBone("Hand.L",muzzle1,Ogre::Quaternion(0,0,1,0),Ogre::Vector3(0,1,0) );
	charEnt->attachObjectToBone("Hand.R",muzzle2,Ogre::Quaternion(1,0,0,0),Ogre::Vector3(0,1,0));

	particleTimer = 0;
	bulleHoles = 0;

	Ogre::ParticleSystem* ps = mSceneMgr->createParticleSystem("Nimbus", "Examples/GreenyNimbus");
	ps->setKeepParticlesInLocalSpace(true);
	//ps->fastForward(25);
	ps->setVisible(false);
	charEnt->attachObjectToBone("Hand.R",ps,Ogre::Quaternion(1,0,0,0),Ogre::Vector3(0,0.7,-0.7));
	//END MUZZLE AND SHIT
}

Character::~Character() 
{
}

void Character::changeAnimation(const Ogre::String& name,const double time)
{
	{
		if (m_AnimationState)
		{
			m_AnimationState->setEnabled(false);
			m_AnimationState->setLoop(false);
		}

		Ogre::AnimationState* animState;

		if (jumping)
			animState = charEnt->getAnimationState("Jump");
		else
		{
			animState = charEnt->getAnimationState(name);
			animState->setLoop(true);
		}

		animState->setEnabled(true);
		m_AnimationState = animState;
		m_AnimationState->addTime(time);

		if (jumping && m_AnimationState->getTimePosition() >= m_AnimationState->getLength())
		{
			jumping=false;
			m_AnimationState->setTimePosition(0);
		}
	}
}

void Character::setPowerUp(const int& p, float time)
{
	mCastingBarTimer = 0.0f;
	mPowerUp = p;
	GameFramework::getSingletonPtr()->mpGui->hideCastingBar();
	if (time!=0)
	{
		mCastingBarTimer = float(time);
		GameFramework::getSingletonPtr()->mpGui->loadCastingBar("SuperSpeed",mCastingBarTimer);
	}
}

void Character::updateCastingBar(const double& timeSinceLastFrame)
{
	if (mCastingBarTimer > 0)
	{
		mCastingBarTimer -= float(timeSinceLastFrame);
		GameFramework::getSingletonPtr()->mpGui->updateCastingBar(mCastingBarTimer);
	}
	else
	{
		setPowerUp(PowerUp_None);
		GameFramework::getSingletonPtr()->mpGui->changeIngameUIIcon(Character::PowerUp_None);//på flera ställen fixah
		GameFramework::getSingletonPtr()->mpGui->hideCastingBar();
	}
}

void Character::jump()
{
	jumping = true;
	if (mPowerUp != PowerUp_SuperJump)
	{
		mCapsule->addForce(NxOgre::Vec3(0,30000,0),NxOgre::Enums::ForceMode_Force);
	}
	else
	{
		mCapsule->addForce(NxOgre::Vec3(0,75000,0),NxOgre::Enums::ForceMode_Force);
	}
}

bool Character::move(const double& timeSinceLastFrame)
{
	bool move;
	Ogre::Vector3 dirCamToChar;
	//Get the direction from the camera to the charcter
	if (mPowerUp != PowerUp_GunMode && mPowerUp != PowerUp_MoveBox)
		dirCamToChar =  charNode->_getDerivedPosition() - camCollisionNode->_getDerivedPosition();
	else
		dirCamToChar =  charNode->_getDerivedPosition() - camGunModeNode->_getDerivedPosition();
	//Update to debugger << DEBUG >>
	GameFramework::getSingletonPtr()->mpGui->updateDebugDirXYZ(dirCamToChar);
	//Setting the y scalar to zero. No movement in y axis.
	dirCamToChar.y = 0;
	//Movement velocity not dependent of the distance between Character and Camera.
	dirCamToChar.normalise();
	//Get the same orientation on the character as the viewing direction
	if (mPowerUp != PowerUp_GunMode || mPowerUp != PowerUp_MoveBox)
	{
		Ogre::Vector3 src = charNode->getOrientation()  * (Ogre::Vector3(0,0,-1));  //@todo! Detta blir annolunda med suitkillen
		Ogre::Quaternion quat = src.getRotationTo(dirCamToChar);
		charNode->rotate(quat);
	}

	//How fast to walk/run
	Ogre::Real walkSpeed;
	if (mMoveDir != Move_None)
	{
		//Check if the PowerUp SuperSpeed is enabled or not
		if (mPowerUp == PowerUp_SuperSpeed)
		{
			walkSpeed = mtCharSpeedSuper;
			Ogre::CompositorManager::getSingleton().setCompositorEnabled(GameFramework::getSingletonPtr()->mpViewport, "Motion Blur", true);
		}
		else
		{
			walkSpeed = 1;
			Ogre::CompositorManager::getSingleton().setCompositorEnabled(GameFramework::getSingletonPtr()->mpViewport, "Motion Blur", false);
		}

		//Set walking speed depending on the direction the player is going to walk. (Case better than if if if if?)
		//Also changes animation
		if  (mMoveDir == Move_Forward )
		{
			changeAnimation("Walk", timeSinceLastFrame);
			walkSpeed *= mtCharSpeedForward;
		}
		if  (mMoveDir == Move_Backward )
		{
			//changeAnimation("WalkBackwards", timeSinceLastFrame);
			changeAnimation("Walk", timeSinceLastFrame);
			walkSpeed *= mtCharSpeedBackward;
		}
		if  (mMoveDir == Move_StrafeLeft )
		{
			//changeAnimation("StrafeLeft", timeSinceLastFrame);
			changeAnimation("Walk", timeSinceLastFrame);
			walkSpeed *= mtCharSpeedStrafeLeft;
		}
		if  (mMoveDir == Move_StrafeRight )
		{
			//changeAnimation("StrafeRight", timeSinceLastFrame);
			changeAnimation("Walk", timeSinceLastFrame);
			walkSpeed *= mtCharSpeedStrafeRight;
		}

		//Character has moved! move = true
		move = true;
		//Adds linear velocity to the capsule. Linear Velocity on Y axis, same as previous(Only gravity can change it)
		mCapsule->setLinearVelocity(NxOgre::Vec3(dirCamToChar.x*walkSpeed ,mCapsule->getLinearVelocity().y,dirCamToChar.z*walkSpeed ));
	}
	else
	{    
		//No movement, set character animation to idle
		//changeAnimation("Idle2", timeSinceLastFrame);
		//move = false
		move = false;
	}

	return move;

}

void Character::updatePosition()
{
	//Get position of capsule and set the characters position to the capsule's position.
	NxOgre::Vec3 capsulePos = mCapsule->getGlobalPosition();
	//Move the "RootNode". CHARACTER_ADJUST_Y , translate node if orgin is not in origo
	charNode->getParentSceneNode()->setPosition(capsulePos.x ,capsulePos.y,capsulePos.z);

	GameFramework::getSingletonPtr()->mpGui->updateMiniMap(capsulePos.x ,capsulePos.z);
	//Update to debugger << DEBUG >>
	GameFramework::getSingletonPtr()->mpGui->updateDebugCharXYZ(capsulePos.as<Ogre::Vector3>());
}

void Character::debugMode()
{
	if (mCapsule->getEntity()->isVisible())
	{
		mCapsule->getEntity()->setVisible(false);
	}
	else
	{
		mCapsule->getEntity()->setVisible(true);
	}
}
void Character::moveBoxMoved(const OIS::MouseEvent &e)
{
	float screenWidth = GameFramework::getSingletonPtr()->mpRenderWnd->getWidth();
	float screenHeight = GameFramework::getSingletonPtr()->mpRenderWnd->getHeight();

	Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(e.state.X.abs/screenWidth,e.state.Y.abs/screenHeight);
	//t = 10. The ray direction * t. Moves the box from camera to a distance.
	//Since mMoveBoxController only is a reference, this will affect the moveBox().

	moveBoxControllerTranslate = mouseRay.getPoint(Ogre::Real (10));
	mMoveBoxController->moveGlobalPosition(moveBoxControllerTranslate); 
}
void Character::moveBoxPressed(const OIS::MouseEvent &e)
{
	//MoveBoxController
	//Get 2D screen mouse position and create cam2vp ray
	//CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition();
	float screenWidth = GameFramework::getSingletonPtr()->mpRenderWnd->getWidth();
	float screenHeight = GameFramework::getSingletonPtr()->mpRenderWnd->getHeight();

	Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(e.state.X.abs/screenWidth,e.state.Y.abs/screenHeight);
	//t = 10. The ray direction * t. Moves the box from camera to a distance.
	//Since mMoveBoxController only is a reference, this will affect the moveBox().



	/*@ todo. CEGUI finns ej. Hitta annat för muspekare

	//Get X and Y position of 2D CEGUI cursor
	CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition();
	//Create a ray from camera trough the CEGUI 2D Screen X & Y
	Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(mousePos.d_x/float(e.state.width), mousePos.d_y/float(e.state.height));
	//Get orgin and direction of ray*/
	NxOgre::Vec3 rayDirection(mouseRay.getDirection());
	NxOgre::Vec3 rayOrgin(mouseRay.getOrigin());
	//NxOgre Raycast. Only hitting dynamic objects . No intersecting with collision group 1
	//@todo fix collision groups
	NxOgre::RaycastHit rayCastResult = mMoveBoxController->getScene()->raycastClosestShape(NxOgre::Ray(rayOrgin,rayDirection),NxOgre::Enums::ShapesType_Dynamic,1);

	//True if it hits a Rigid Body
	if (rayCastResult.mRigidBody)
	{
		NxOgre::FixedJointDescription desciptionJoint;
		//Removes earlier joints (if there's any)
		if (mMoveBoxJoint != NULL)
		{
			mMoveBoxController->getScene()->destroyJoint(mMoveBoxJoint);
		}
		//Create Joint between the raycast hit and MoveBoxController
		mMoveBoxJoint = mMoveBoxController->getScene()->createFixedJoint( mMoveBoxController, rayCastResult.mRigidBody, desciptionJoint );
		mSceneMgr->getParticleSystem("Nimbus")->clear();
		mSceneMgr->getParticleSystem("Nimbus")->setEmitting(true);
		mSceneMgr->getParticleSystem("Nimbus")->setVisible(true);
		GameFramework::getSingletonPtr()->mpSound->playSound("Raspberry 06.wav", charNode->_getDerivedPosition());
		if (particleTimer == 0)
			particleTimer+=0.001;

		mCastingBarTimer = float(1.2);
		GameFramework::getSingletonPtr()->mpGui->loadCastingBar("Force",mCastingBarTimer);	
	}

}
void Character::releaseBox()
{
	//Destroys the joint between the Rigid body object and the BoxController
	mMoveBoxController->getScene()->destroyJoint(mMoveBoxJoint);
	mSceneMgr->getParticleSystem("Nimbus")->setEmitting(false);
	particleTimer = 0;
	mCastingBarTimer = 0;
	GameFramework::getSingletonPtr()->mpGui->hideCastingBar();
	mMoveBoxJoint = NULL;
}

void Character::updateMuzzleFlash(const double& timeSinceLastFrame)
{
	if (muzzleTimer>0)
		muzzleTimer += timeSinceLastFrame;
	//std::cout << muzzleInt<<;

	if (expoTimer>0)
	{
		expoTimer += timeSinceLastFrame;
		expoNode->setScale(expoNode->getScale() * 1.02);
	}

	if (muzzleTimer > 0.1)
	{
		muzzle1->setVisible(false);
		muzzle2->setVisible(false);
		muzzleTimer = 0;
	}

	if (particleTimer>0)
	{
		particleTimer += timeSinceLastFrame;
		mCastingBarTimer -= float(timeSinceLastFrame);
		GameFramework::getSingletonPtr()->mpGui->updateCastingBar(mCastingBarTimer);
	}



	if (expoTimer > 0.25)
	{
		explosion->setVisible(false);
		expoTimer = 0;
	}

	if (particleTimer > 1.2)
	{
		releaseBox();
		particleTimer = 0;
	}

	if (particleTimer > 1.0)
		mSceneMgr->getParticleSystem("Nimbus")->setEmitting(false);
}

void Character::toggleMuzzleFlash(const OIS::MouseEvent &e, bool center)
{
	if (muzzleTimer == 0)
	{
		muzzle1->setVisible(true);
		muzzle2->setVisible(true);
		muzzleTimer+=0.001;
		//update ska egentligen inte ske här @todo ska bööört
		GameFramework::getSingletonPtr()->mpSound->updateListenerPos(charNode->_getDerivedPosition());
		GameFramework::getSingletonPtr()->mpSound->playSound("Pistol Shot 22cal 01.wav", charNode->_getDerivedPosition());

		float screenWidth = GameFramework::getSingletonPtr()->mpRenderWnd->getWidth();
		float screenHeight = GameFramework::getSingletonPtr()->mpRenderWnd->getHeight();
		Ogre::Ray mouseRay;
		if (center==true)
			mouseRay = mCamera->getCameraToViewportRay(0.5,0.5);
		else
			mouseRay = mCamera->getCameraToViewportRay(e.state.X.abs/screenWidth,e.state.Y.abs/screenHeight);

		//t = 10. The ray direction * t. Moves the box from camera to a distance.
		//Since mMoveBoxController only is a reference, this will affect the moveBox().
		//Get orgin and direction of ray*/
		NxOgre::Vec3 rayDirection(mouseRay.getDirection());
		NxOgre::Vec3 rayOrgin(mouseRay.getOrigin());
		//NxOgre Raycast. Only hitting dynamic objects . No intersecting with collision group 1
		//@todo fix collision groups
		NxOgre::RaycastHit rayCastResult = mMoveBoxController->getScene()->raycastClosestShape(NxOgre::Ray(rayOrgin,rayDirection),NxOgre::Enums::ShapesType_All,1);
		expoNode->setPosition(rayCastResult.mWorldImpact.as<Ogre::Vector3>());
		GameFramework::getSingletonPtr()->mpSound->playSound("Explosion Deep Echo 02.wav", rayCastResult.mWorldImpact.as<Ogre::Vector3>());

		std::cout << "\n SKOTTET TRÄFFADE PA POSITION " << rayCastResult.mWorldImpact.x << " " << rayCastResult.mWorldImpact.y << " " << rayCastResult.mWorldImpact.z;
		std::cout << "\n DISTANS = " << rayCastResult.mDistance;
		//GameFramework::getSingletonPtr()->mpSound->playSound("Explosion Deep Echo 02.wav", rayCastResult.mWorldImpact.as<Ogre::Vector3>());
		if (rayCastResult.mDistance < 100)
		{
			if (rayCastResult.mRigidBody->isDynamic())
			{
				NxOgre::Vec3 force = rayCastResult.mWorldImpact - NxOgre::Vec3(charNode->_getDerivedPosition());
				force.normalise();
				force = force*1000;
				rayCastResult.mRigidBody->getNxActor()->addForce(NxVec3(force.x,force.y,force.z));
				GameFramework::getSingletonPtr()->mpSound->playSound("Wood Destruction 01.wav", rayCastResult.mWorldImpact.as<Ogre::Vector3>());
			}
			else
				createBulletHole(rayCastResult.mWorldNormal.as<Ogre::Vector3>(), rayCastResult.mWorldImpact.as<Ogre::Vector3>());
		}

	}

	if (expoTimer == 0)
	{
		explosion->setVisible(true);
		expoTimer+=0.001;
		expoNode->setScale(0.5,0.5,0.5);
	}



}

void Character::updateBones(const OIS::MouseEvent &arg)
{
	hipBone->setManuallyControlled(true);
	//hipBone->yaw(Ogre::Degree(-GameFramework::getSingletonPtr()->mpSettings->mCamRotate * arg.state.X.rel), Ogre::Node::TS_WORLD);
	//RotateBone(hipBone);

	rightArmBone->setManuallyControlled(true);
	RotateBone(rightArmBone);
	rightArmBone->pitch(Ogre::Degree(-90));

	leftArmBone->setManuallyControlled(true);
	RotateBone(leftArmBone);
	leftArmBone->pitch(Ogre::Degree(-90));

	headBone->setManuallyControlled(true);
	RotateBone(headBone);
	headBone->yaw(Ogre::Degree(180));
}

void Character::RotateBone(Ogre::Bone* bone)
{
	Ogre::Vector3 between = mixGunTrackerNode->_getDerivedPosition() - charNode->_getDerivedPosition();

	between = between;//what is the unit vector
	Ogre::Vector3 unitBetween = between.normalisedCopy();

	Ogre::Node * neckBone = bone->getParent();
	Ogre::Quaternion neckBoneWorldOrientation = charNode->_getDerivedOrientation() * neckBone->_getDerivedOrientation();
	Ogre::Vector3 headForward = unitBetween;
	Ogre::Vector3 neckUp;
	neckUp = neckBoneWorldOrientation.yAxis();
	Ogre::Vector3 headRight = neckUp.crossProduct(headForward);
	Ogre::Vector3 headUp = headForward.crossProduct(headRight);

	Ogre::Quaternion rot(-headRight,headUp,-headForward);
	rot.normalise(); //might have gotten messed up



	/*//DEBUG
	if (bone->getName() == "Head")
	{
	std::cout << bone->getName()<< std::endl << std::endl;
	std::cout<< "Vector Forward X : x= " << headForward.x << ". y= "  << headForward.y << ". z- "  << headForward.z << std::endl;
	std::cout<< "Vector NeckUp  Y : " << neckUp.x << " "  << neckUp.y << " "  << neckUp.z << std::endl;
	std::cout<< "headRight = neckUp X Forward" << std::endl;
	std::cout<< "Vector Right   Z : x= " << headRight.x << ". y= "  << headRight.y << ". z- "  << headRight.z << std::endl;
	std::cout<< "headUp = Forward X Right" << std::endl;
	std::cout<< "Vector Up   Y : x= " << headUp.x << ". y= "  << headUp.y << ". z- "  << headUp.z << std::endl;
	std::cout<< "quat rot(-right,up,-forward)" << std::endl;
	std::cout<< "quat rot	   : w= " << rot.w << ". x= "  << rot.x << ". y- "  << rot.y << ". z- "  << rot.z << std::endl;
	std::cout<< "quat neckBoneWorldOrientation.inverse()"<< std::endl;
	std::cout<< "quat neckOrien: w= " << neckBoneWorldOrientation.Inverse().w << ". x= "  << neckBoneWorldOrientation.Inverse().x << ". y- "  << neckBoneWorldOrientation.Inverse().y << ". z- "  << neckBoneWorldOrientation.Inverse().z << std::endl;
	rot = neckBoneWorldOrientation.Inverse()*rot;
	std::cout<< "rot = neckBoneWorldOrientation.Inverse()*rot;"<< std::endl;
	std::cout<< "quat nya rot  : w= " << rot.w << ". x= "  << rot.x << ". y- "  << rot.y << ". z- "  << rot.z  <<std::endl;
	bone->setOrientation(rot);*/

	rot = neckBoneWorldOrientation.Inverse()*rot;
	bone->setOrientation(rot);
}

void Character::createBulletHole(const Ogre::Vector3 &normal,const Ogre::Vector3 &pos)
{
	Ogre::Plane plane(normal,0.001);

	Ogre::String name = "bulletHole" + Ogre::StringConverter::toString(bulleHoles);
	Ogre::Vector3 upVector= normal.perpendicular();
	Ogre::MeshManager::getSingleton().createPlane(name,
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
		0.1,0.1,20,20,true,1,1,1,upVector);

	Ogre::Entity* ent = mSceneMgr->createEntity(name+"Ent", name);
	mSceneMgr->getRootSceneNode()->createChildSceneNode(pos)->attachObject(ent);
	ent->setMaterialName("BulletHole");
	ent->setCastShadows(false);
	bulleHoles++;
}