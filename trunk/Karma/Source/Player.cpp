#include "Player.h"
#include "Chunks.h"
#include "CameraHandler.h"

Player::Player(Ogre::SceneManager* sceneMgr,NxOgre::Scene* physScene,OGRE3DRenderSystem* renderSystem, Ogre::String filename, 
			   Ogre::String name, Ogre::Vector3 spawnPoint ,  float hp , float walkSpeed)

			   : Character(sceneMgr,physScene,renderSystem,filename,name,spawnPoint,*Settings::getSingletonPtr()->mCharScale,hp,walkSpeed,Game::CollisionGroup_Player),
	mtCharSpeedSuper(Settings::getSingletonPtr()->mPlayerSuperSpeed),
	mvpCamera(GameFramework::getSingletonPtr()->mpViewport->getCamera()),
	mtScreenWidth(GameFramework::getSingletonPtr()->mpRenderWnd->getWidth()),
	mtScreenHeight(GameFramework::getSingletonPtr()->mpRenderWnd->getHeight()),
	mvpMoveBoxJoint(NULL),
	mvPowerUp(Game::PowerUp_None),
	mvPowerUpCastingBar(Game::PowerUp_None),
	mvMouseX(0),
	mvMouseY(0),
	mvRocketBootsTimer(0),
	mvRocketBoots(false),
	mvNeedUpdateBones(false),
	mvDirCamToChar(Ogre::Vector3(0,0,-1)),
	mvQuatTimer(0),
	mvpMaxSpeedBeforeDie(&Settings::getSingletonPtr()->mPlayerMaxFallSpeedBeforeDie),
	mvpRocketBootsForceY(&Settings::getSingletonPtr()->mPlayerRocketBootsForceY),
	mvpRocketBootsDuration(&Settings::getSingletonPtr()->mPlayerRocketBootsDuration),
	mvpContiniousShootTimer(&Settings::getSingletonPtr()->mPlayerContiniousShootTimer),
	mvpMoveBoxDuration(&Settings::getSingletonPtr()->mPlayerMoveBoxDuration),
	mvpSuperSpeedDuration(&Settings::getSingletonPtr()->mPlayerSuperSpeedDuration)
{
	//The player is not assigned to any chunk and therefore added to the dynamic characters vector. Used for updating later on.
	mtDynamicCharacters.push_back(this);

	//Creating a lot of SceneNodes that the Player will need for the different Camera Modes.
	//For more information about have the nodes works, see documentation.
	Ogre::SceneNode* camOrginNode = sceneMgr->getSceneNode("RootNode")->createChildSceneNode("CamOrginNode", Ogre::Vector3(0, CAMERA_HEIGHT, 0));
	
	//First person nodes
	Ogre::SceneNode*  camFPnode = camOrginNode->createChildSceneNode("CamFirstPersonNode");
	float mCamGunTrackerOffsetZ = Settings::getSingletonPtr()->mCamGunTrackerOffsetZ;
	camFPnode->createChildSceneNode("FirstPersonGunTrackerNode", Ogre::Vector3(0, 0, mCamGunTrackerOffsetZ));

	//Third person nodes
	Ogre::SceneNode*  camHelperNode = camOrginNode->createChildSceneNode("CamHelperNode");
	camHelperNode->createChildSceneNode("CamNoCollisionNode", Ogre::Vector3(0, 0, CAMERA_DISTANCE));
	mvpCamCollisionNode = sceneMgr->getRootSceneNode()->createChildSceneNode("CamCollisionNode",mtSpawnPoint + Ogre::Vector3(0, 0, CAMERA_DISTANCE));	

	// Semi-third person nodes
	Ogre::SceneNode* camMovableNode = camOrginNode->createChildSceneNode("CamMoveNode");
	mvpCamGunModeNode = camMovableNode->createChildSceneNode("CamMoveHelperNode", Ogre::Vector3(0.0f, 0.0f, 2.5f));
	Ogre::SceneNode* camCenterNode = camOrginNode->createChildSceneNode("CamCenterNode");
	camCenterNode->createChildSceneNode("CamCenterHelperNode", Ogre::Vector3(0.0f, 0.0f, 2.5f));
	
	//Update position of the Entity
	updatePosition();

	//Add a kinematic controller for the move box powerup. Position,size,mesh doesn't really matter. Only a reference.
	mvpMoveBoxController = renderSystem->createKinematicBody(new NxOgre::Box(1, 1,1), NxOgre::Vec3(0, 0, 0), "cube.1m.mesh");
	//Collision Group 1
	NxShape* const* ss = mvpMoveBoxController->getNxActor()->getShapes();
	(*ss)->setGroup(Game::CollisionGroup_MoveBoxReference); //hack ¨@todo så inte de kan skjuta min låda
	//Not making any other dynamic objects move
	mvpMoveBoxController->getNxActor()->raiseActorFlag(NX_AF_DISABLE_COLLISION);
	//Invisible of course.
	mvpMoveBoxController->getEntity()->setVisible(false);

	//Aimer class

	//Binds nodes and bones
	setNodes(mtpCharNode, sceneMgr);
	setBones(mtpCharEnt,Ogre::String("UpArm.L"), Ogre::String("UpArm.R"),Ogre::String("LowArm.L"),Ogre::String("LowArm.R"),Ogre::String("Head"));

	//Some animations might have keyframes for these bones. If not destroyed, weird things will happen.
	destroyNodeTracks(mtpCharEnt->getSkeleton()->getAnimation(Game::Animation_WalkGun));	
	destroyNodeTracks(mtpCharEnt->getSkeleton()->getAnimation(Game::Animation_RocketBoots));
	destroyNodeTracks(mtpCharEnt->getSkeleton()->getAnimation(Game::Animation_Jump));

	//Add visual effects such as gun, muzzle fire,Rocket Boots, MoveBox particles and blood (for targets)
	
	Ogre::Vector3 offsetLeft(*Settings::getSingletonPtr()->mAimerOffsetVectorLX,*Settings::getSingletonPtr()->mAimerOffsetVectorLY,*Settings::getSingletonPtr()->mAimerOffsetVectorLZ);
	Ogre::Vector3 offsetRight(*Settings::getSingletonPtr()->mAimerOffsetVectorRX,*Settings::getSingletonPtr()->mAimerOffsetVectorRY,*Settings::getSingletonPtr()->mAimerOffsetVectorRZ);
	Ogre::Quaternion orientationLeft(*Settings::getSingletonPtr()->mAimerOffsetQuatLW,*Settings::getSingletonPtr()->mAimerOffsetQuatLX,*Settings::getSingletonPtr()->mAimerOffsetQuatLY,*Settings::getSingletonPtr()->mAimerOffsetQuatLZ);
	Ogre::Quaternion orientationRight(*Settings::getSingletonPtr()->mAimerOffsetQuatRW,*Settings::getSingletonPtr()->mAimerOffsetQuatRX,*Settings::getSingletonPtr()->mAimerOffsetQuatRY,*Settings::getSingletonPtr()->mAimerOffsetQuatRZ);

	addGun("Hand.R",orientationRight , offsetRight,
		"Hand.L",orientationLeft  ,offsetLeft );
	addMuzzleFire("Hand.R",orientationRight, offsetRight,
		"Hand.L",orientationLeft  ,offsetLeft);

	addBloodParticleSystem();	
	mvpMoveBoxParticles = new ManuallyControlledParticles(sceneMgr,name + Ogre::String("MoveBoxParticles"),"Examples/GreenyNimbus");
	mvpRocketBootsParticle1 = new ManuallyControlledParticles(sceneMgr,name + Ogre::String("RocketBoots1"),"rocketboots");
	mvpRocketBootsParticle2 = new ManuallyControlledParticles(sceneMgr,name + Ogre::String("RocketBoots2"),"rocketboots");
	mvpMuzzleFireFirstPerson = new MuzzleFireFirstPerson(0.1);
	Effects::addDynamicEffect(mvpMuzzleFireFirstPerson);

	//Attach the manually controlled particle systems to the bones
	mtpCharEnt->attachObjectToBone("Hand.R",mvpMoveBoxParticles->getParticleSystem(),Ogre::Quaternion(1,0,0,0),
		Ogre::Vector3(*Settings::getSingletonPtr()->mPlayerParticleMoveBoxOffsetX,*Settings::getSingletonPtr()->mPlayerParticleMoveBoxOffsetY,*Settings::getSingletonPtr()->mPlayerParticleMoveBoxOffsetZ));
	Ogre::Vector3 rocketBootsOffset(*Settings::getSingletonPtr()->mPlayerParticleRocketBootsOffsetX,*Settings::getSingletonPtr()->mPlayerParticleRocketBootsOffsetY,*Settings::getSingletonPtr()->mPlayerParticleRocketBootsOffsetZ);
	mtpCharEnt->attachObjectToBone("LowLeg.L",mvpRocketBootsParticle1->getParticleSystem(),Ogre::Quaternion(1,0,0,0),rocketBootsOffset);
	mtpCharEnt->attachObjectToBone("LowLeg.R",mvpRocketBootsParticle2->getParticleSystem(),Ogre::Quaternion(1,0,0,0),rocketBootsOffset);
	
#ifdef DEBUG
	//Optional, attach a sphere to the gunTrackerNode
	Ogre::Entity* sphereEnt = sceneMgr->createEntity("Sphere", "sphere.mesh");
	Ogre::SceneNode* node = sceneMgr->getSceneNode("GunTrackNode");
	node->attachObject(sphereEnt);
	node->setScale(0.001f, 0.001f, 0.001f);
#endif

	//Update frequencies
	mvRocketBootsUpdateFreq = 1 / Settings::getSingletonPtr()->mPlayerRocketBootsUpdateFreq;
	mvRocketBootsCastingBarUpFreq = 1 / Settings::getSingletonPtr()->mPlayerRocketBootsCastingBarUpFreq;
	mvUpdateBonesFreq = 1 / Settings::getSingletonPtr()->mPlayerUpdateBonesFreq;
}
/*---------------------------------------------------------------------------------*/
Player::~Player()
{
	delete mvpMoveBoxParticles;
	delete mvpRocketBootsParticle1;
	delete mvpRocketBootsParticle2;
	delete mvpMuzzleFireFirstPerson;
}
/*---------------------------------------------------------------------------------*/
void Player::firstPersonMode(bool s)
{
	GameFramework::getSingletonPtr()->mpGui->firstPerson(s);
	mtpCharEnt->setVisible(!s);
	mvpGun1Ent->setVisible(!s);
	mvpGun2Ent->setVisible(!s);
}
/*---------------------------------------------------------------------------------*/
void Player::semiThirdPersonMode(bool s)
{
	mvpGun1Ent->setVisible(s);
	mvpGun2Ent->setVisible(s);
}
/*---------------------------------------------------------------------------------*/
void Player::updateCastingBar(const double& timeSinceLastFrame)
{
	//Not 100% done yet
	if (mvCastingBarTimer > 0)
	{
		mvCastingBarTimer -= float(timeSinceLastFrame);
		GameFramework::getSingletonPtr()->mpGui->updateCastingBar(mvCastingBarTimer);
	}
	else
	{
		if ((mvPowerUp & Game::PowerUp_SuperSpeed) != 0)
		{
			GameFramework::getSingletonPtr()->mpGui->updateActionBarElement(Game::PowerUp_SuperSpeed,Game::ActionBar_Normal);
		}
		mvPowerUp = mvPowerUp ^ mvPowerUpCastingBar;
		GameFramework::getSingletonPtr()->mpGui->hideCastingBar();
		mvCastingBarTimer = 0;
		if ((mvPowerUp & Game::PowerUp_MoveBoxMode) != 0)
		{
			releaseBox();
		}
	}	
}
/*---------------------------------------------------------------------------------*/
void Player::jump()
{
	//Just not jumping already, add force on the positve Y-axis
	if (!mtJumping)
	{
		std::cout << "\nJumping!\n";
		mtpCharEnt->getAnimationState("Jump")->setTimePosition(0);
		mtJumping = true;
		if ((mvPowerUp & Game::PowerUp_SuperJump) != 0)
			mtpHitBox->addForce(NxOgre::Vec3(0,3* *mtJumpForceY,0),NxOgre::Enums::ForceMode_Force);
		else
			mtpHitBox->addForce(NxOgre::Vec3(0,*mtJumpForceY,0),NxOgre::Enums::ForceMode_Force);		
	}	
}
/*---------------------------------------------------------------------------------*/
void Player::setPowerUp(const int& p,float time)
{
	mvCastingBarTimer = 0;
	if ((mvPowerUp & Game::PowerUp_SuperSpeed) != 0)
		mvPowerUp = mvPowerUp & (~Game::PowerUp_SuperSpeed);

	GameFramework::getSingletonPtr()->mpGui->updateActionBarElement(p,Game::ActionBar_Active);
	GameFramework::getSingletonPtr()->mpGui->hideCastingBar();

	if (mvpMoveBoxJoint != NULL && p != Game::PowerUp_MoveBox)
		releaseBox();

	//Not 100% done
	if (time!=0)
	{
		GameFramework::getSingletonPtr()->mpGui->hideCastingBar();
		mvPowerUpCastingBar = p;
		mvCastingBarTimer = float(time);
		GameFramework::getSingletonPtr()->mpGui->loadCastingBar(p,mvCastingBarTimer);
	}

		if (p == Game::PowerUp_None)
		mvPowerUp = Game::PowerUp_None;

	if (p == Game::PowerUp_SuperJump && (mvPowerUp & Game::PowerUp_SuperJump) == 0)
		mvPowerUp = (mvPowerUp | Game::PowerUp_SuperJump) & (~Game::PowerUp_RocketBootsMode);
	else if (p == Game::PowerUp_SuperJump)
	{
		mvPowerUp = mvPowerUp & (~Game::PowerUp_SuperJump);
		GameFramework::getSingletonPtr()->mpGui->updateActionBarElement(p,Game::ActionBar_Normal);
	}

	if (p == Game::PowerUp_RocketBootsMode)
	{
		mvPowerUp = (mvPowerUp | Game::PowerUp_RocketBootsMode) & (~Game::PowerUp_SuperJump);
		mvCastingBarTimer = 1.5f;
	}

	if (p == Game::PowerUp_SuperSpeed)
		mvPowerUp = (mvPowerUp | Game::PowerUp_SuperSpeed) & (~Game::PowerUp_RocketBootsMode);

	if (p == Game::PowerUp_MoveBoxMode)
	{
		mvPowerUp = (mvPowerUp | Game::PowerUp_MoveBoxMode) & (~Game::PowerUp_GunMode);			
		changeAnimation("WalkGun", 0.02);
		semiThirdPersonMode(false);
	}

	if (p == Game::PowerUp_GunMode)
	{
		mvPowerUp = (mvPowerUp | Game::PowerUp_GunMode) & (~Game::PowerUp_MoveBoxMode);		
			changeAnimation("WalkGun", 0.02);
	}

	std::cout << "\nmPowerUP = " << mvPowerUp << "\n";
}
/*---------------------------------------------------------------------------------*/
void Player::move(const double& timeSinceLastFrame)
{
	if (mvContinuousShoot)
	{
		if (mvContinuousShootTimer == 0.2)
		{
			if (CameraHandler::getSingletonPtr()->getCamMode() == Game::Cam_MixCursorCenter || CameraHandler::getSingletonPtr()->getCamMode() == Game::Cam_FirstPerson)
				queueFire(0.5,0.5);
			else
				queueFire(double(mvMouseX)/mtScreenWidth,double(mvMouseY)/mtScreenHeight);
		}
		mvContinuousShootTimer -= timeSinceLastFrame;
		if (mvContinuousShootTimer <= 0.0)
			mvContinuousShootTimer = 0.2;
	}


	
	if (mtJumping || (mvPowerUp & Game::PowerUp_RocketBootsMode) != 0)
	{
		float speedY = mtpHitBox->getLinearVelocity().y;
		if (speedY < 0)
			mtFallingSpeed = speedY;
		else if (mtFallingSpeed < 0)
		{
			if (speedY > 0)
			{
				mtJumping=false;		
				if (mtFallingSpeed < *mvpMaxSpeedBeforeDie )
					updateHp(100);
				if (mtpAnimationState)
				mtpAnimationState->setTimePosition(0);
				mtFallingSpeed = 0;
			}
		}
	}
	
	if (mvRocketBoots)
	{
		if (mvCastingBarTimer > 0)
		{
		rocketBoots(timeSinceLastFrame);
		mvCastingBarTimer -= float(timeSinceLastFrame);
		GameFramework::getSingletonPtr()->mpGui->updateCastingBar(mvCastingBarTimer);	
		}
		else
		resetRocketBoots();
	}

	if ((mvPowerUp & Game::PowerUp_RocketBootsMode) != 0)
	{
		if (mvCastingBarTimer < 1.5)
		{
		mvCastingBarTimer += float(timeSinceLastFrame) * mvRocketBootsCastingBarUpFreq ;
		GameFramework::getSingletonPtr()->mpGui->updateCastingBar(mvCastingBarTimer);
		}
	}


	if (mvNeedUpdateRotation)
	{
	//Get the direction from the camera to the charcter or character to GunTracker node
	//Depends on PowerUp
		if ((mvPowerUp & (Game::PowerUp_GunMode | Game::PowerUp_MoveBoxMode)) == 0)
		{
			mvDirCamToChar =  mtpCharNode->_getDerivedPosition() - mvpCamCollisionNode->_getDerivedPosition();
		}
		else
			mvDirCamToChar =  mtpCharNode->_getDerivedPosition() - mvpCamGunModeNode->_getDerivedPosition();
	}
	mvNeedUpdateRotation = false;
	
#ifdef DEBUG
	//Update to debugger << DEBUG >>
	GameFramework::getSingletonPtr()->mpGui->updateDebugDirXYZ(mvDirCamToChar);
#endif
	//Setting the y scalar to zero. No movement in y axis.
	mvDirCamToChar.y = 0;
	//Movement velocity not dependent of the distance between Character and Camera.
	mvDirCamToChar.normalise();
	
	//Get the same orientation on the character as the viewing direction
	rotateCharacter(mtpCharNode,mvDirCamToChar,*mtFaceDirection);
	//If the mouse has moved and the bones need an update
	if (mvNeedUpdateBones)
	{
		mvQuatTimer+=timeSinceLastFrame;
		if (mvQuatTimer >= mvUpdateBonesFreq)
		{
			mvQuatTimer=0;
			updateBones(mtvGunTrackNode);
			mvNeedUpdateBones = false;
		}	

	}

	//How fast to walk/run
	if (mtMoveDir != Game::Move_None)
	{
		Ogre::Real walkSpeed = mtWalkSpeed;
		Ogre::Real animationspeed = 1;
		//Check if the PowerUp SuperSpeed is enabled or not
		if ((mvPowerUp & Game::PowerUp_SuperSpeed) != 0)
		{
			walkSpeed = mtCharSpeedSuper;
			animationspeed = 5;
			Ogre::CompositorManager::getSingleton().setCompositorEnabled(GameFramework::getSingletonPtr()->mpViewport, "Motion Blur", true);
		}
		else
			Ogre::CompositorManager::getSingleton().setCompositorEnabled(GameFramework::getSingletonPtr()->mpViewport, "Motion Blur", false);

		//Set walking speed depending on the direction the player is going to walk.
		//Also changes animation (At the moment only Walk(forward) available)
		if  (mtMoveDir == Game::Move_Forward )
		{
			walkSpeed *= *mtCharSpeedForward;
		}
		if  (mtMoveDir == Game::Move_Backward )
		{
			//changeAnimation("WalkBackwards", timeSinceLastFrame);
			walkSpeed *= *mtCharSpeedBackward;
		}
		if  (mtMoveDir == Game::Move_StrafeLeft )
		{
			//changeAnimation("StrafeLeft", timeSinceLastFrame);
			walkSpeed *= *mtCharSpeedStrafeLeft;
		}
		if  (mtMoveDir == Game::Move_StrafeRight )
		{
			//changeAnimation("StrafeRight", timeSinceLastFrame);
			walkSpeed *= *mtCharSpeedStrafeRight;
		}
		if((mvPowerUp & Game::PowerUp_RocketBootsMode) != 0)
			changeAnimation("RocketBoots", timeSinceLastFrame);
		else if((mvPowerUp & Game::PowerUp_GunMode) != 0 || (mvPowerUp & Game::PowerUp_MoveBoxMode) != 0 )
			changeAnimation("WalkGun", timeSinceLastFrame*animationspeed);
		else
			changeAnimation("Walk", timeSinceLastFrame*animationspeed);

		//Adds linear velocity to the capsule. Linear Velocity on Y axis, same as previous(Only gravity can change it)
		mtpHitBox->setLinearVelocity(NxOgre::Vec3(mvDirCamToChar.x*walkSpeed ,mtpHitBox->getLinearVelocity().y,mvDirCamToChar.z*walkSpeed ));
	}
	else
	{    
		//No movement, set character animation to idle
		//At the moment no Idle animation available
		//changeAnimation("Idle2", timeSinceLastFrame);
	}

	if (mtJumping)
		changeAnimation("Jump", timeSinceLastFrame,false);
}
/*---------------------------------------------------------------------------------*/
void Player::moveBoxMoved(const OIS::MouseEvent &e)
{
	//Ray from camera to viewport and mouse position
	Ogre::Ray mouseRay = mvpCamera->getCameraToViewportRay(e.state.X.abs/mtScreenWidth,e.state.Y.abs/mtScreenHeight);
	
	//t = 10. The ray direction * t. Moves the box from camera to a distance.
	//Since mMoveBoxController only is a reference, this will affect the moveBox().
	mvpMoveBoxController->moveGlobalPosition(mouseRay.getPoint(Ogre::Real (10))); 
}
/*---------------------------------------------------------------------------------*/
void Player::moveBoxPressed(const OIS::MouseEvent &e)
{	
	//Ray from camera to viewport and mouse position
	Ogre::Ray mouseRay = mvpCamera->getCameraToViewportRay(double(mvMouseX)/mtScreenWidth,double(mvMouseY)/mtScreenHeight);

	//Get orgin and direction of ray
	NxOgre::Vec3 rayDirection(mouseRay.getDirection());
	NxOgre::Vec3 rayOrgin(mouseRay.getOrigin());

	//NxOgre Raycast. Only hitting dynamic objects . No intersecting with collision group 1
	//@todo fix collision groups
	NxOgre::RaycastHit rayCastResult = mvpMoveBoxController->getScene()->raycastClosestShape(NxOgre::Ray(rayOrgin,rayDirection),NxOgre::Enums::ShapesType_Dynamic,1);

	//True if it hits a Rigid Body
	if (rayCastResult.mRigidBody)
	{
		GameFramework::getSingletonPtr()->mpGui->changeCursor(Game::Cursor_MoveBoxGrab);

		GameFramework::getSingletonPtr()->mpSound->playSound(GameFramework::getSingletonPtr()->mpSound->mpMoveBox,mtpCharNode->_getDerivedPosition());
		//Start the timer for the particle system
		mvpMoveBoxParticles->startTimer();
		NxOgre::FixedJointDescription desciptionJoint;
		//Removes earlier joints (if there's any)
		if (mvpMoveBoxJoint != NULL)
		{
			mvpMoveBoxController->getScene()->destroyJoint(mvpMoveBoxJoint);
		}
		//Create Joint between the raycast hit and MoveBoxController
		mvpMoveBoxJoint = mvpMoveBoxController->getScene()->createFixedJoint( mvpMoveBoxController, rayCastResult.mRigidBody, desciptionJoint );
		setPowerUp(Game::PowerUp_MoveBox,2.0f);
	}
}
/*---------------------------------------------------------------------------------*/
void Player::releaseBox()
{
	//Destroys the joint between the Rigid body object and the BoxController
	mvpMoveBoxController->getScene()->destroyJoint(mvpMoveBoxJoint);
	

	//Manually stops the particle system
	mvpMoveBoxParticles->setManualReset();
	
	//Resets casting bartimer
	mvCastingBarTimer = 0;
	GameFramework::getSingletonPtr()->mpGui->hideCastingBar();
	GameFramework::getSingletonPtr()->mpGui->changeCursor(Game::Cursor_MoveBox);
	
	//Makes the pointer point to nothing.
	mvpMoveBoxJoint = NULL;

}
/*---------------------------------------------------------------------------------*/
void Player::updatePosition()
{
	//Get position of capsule and set the characters position to the capsule's position.
	NxOgre::Vec3 hitBoxPos = mtpHitBox->getGlobalPosition();
	//Moves the RootNode(The one with all other nodes attached to it)
	//CHARACTER_ADJUST_Y , translate node if orgin is not in origo
	mtpCharNode->getParentSceneNode()->setPosition(hitBoxPos.x ,hitBoxPos.y-0.675,hitBoxPos.z);
	
	//What chunk that is active
	updateChunk(hitBoxPos);

	//Sets The 3D sound listener to always be at the Player.
	GameFramework::getSingletonPtr()->mpSound->updateListenerPos(hitBoxPos.as<Ogre::Vector3>());
	
	//Updates the minimap
	GameFramework::getSingletonPtr()->mpGui->updateMiniMap(hitBoxPos.x ,hitBoxPos.z);

	#ifdef DEBUG
	//Update to debugger << DEBUG >>
	GameFramework::getSingletonPtr()->mpGui->updateDebugCharXYZ(hitBoxPos.as<Ogre::Vector3>());
	#endif
}
/*---------------------------------------------------------------------------------*/
void Player::updateChunk(const NxOgre::Vec3& hitBoxPos)
{
	//Get world position to Chunk Grid Coordinate System
	GridData chunk = worldToChunk(hitBoxPos.x,hitBoxPos.z);
	
	//Check if there has been any changes
	int change = -1;
	if (mtChunk.x != chunk.x)
	{
		mtChunk.x = chunk.x;
		++change;
	}
	if (mtChunk.y != chunk.y)
	{
		mtChunk.y = chunk.y;
		++change;
	}

	//change = 0, only movement in either x or y. change = 1, movement in both.
	if (change > -1)
	{
		//So that the player respawns in the right chunk
		mtSpawnPoint = mtpCharNode->_getDerivedPosition();

#ifdef DEBUG
		//Update to debugger << DEBUG >>
		GameFramework::getSingletonPtr()->mpGui->updateCurChunk(mtChunk.x,mtChunk.y);
#endif
		//Updates the current active "Chunk"
		std::cout << "\nNew Chunk Pos! x: " << mtChunk.x << " Y: " << mtChunk.y;
		Chunks::getSingletonPtr()->setActiveChunkArea(mtChunk,change);
	}
}
/*---------------------------------------------------------------------------------*/
void Player::queueFire(const double& x,const double& y)
{
	//@todo If CameraHandler->getCamMdoe == firstPerson
	if (CameraHandler::getSingletonPtr()->getCamMode() == Game::Cam_FirstPerson)
		mvpMuzzleFireFirstPerson->startTimer();
	
	//Sends a ray to the Aimer::fire();
	Ogre::Ray mouseRay = mvpCamera->getCameraToViewportRay(x,y);
	fire(mouseRay.getOrigin(), mouseRay.getDirection(),( (1 << Game::CollisionGroup_Player) | (1 << Game::CollisionGroup_MoveBoxReference)));
}
/*---------------------------------------------------------------------------------*/
void Player::queueRocketBoots()
{
	mvRocketBoots = true;
}

void Player::rocketBoots(const double& timeSinceLastFrame)
{
	mvRocketBoots = false;

	mvRocketBootsTimer+=timeSinceLastFrame;
	
	if(mvRocketBootsTimer > 1/30)
	{
		mvRocketBootsTimer = 0;
		mtpHitBox->addForce(NxOgre::Vec3(0,2000,0),NxOgre::Enums::ForceMode_Force);
	}
}
/*---------------------------------------------------------------------------------*/
void Player::startRocketBoots()
{
	//Starts the particle system
	if (mvCastingBarTimer>0)
	{
	GameFramework::getSingletonPtr()->mpSound->playSound(GameFramework::getSingletonPtr()->mpSound->mpRocketBoots,mtpCharNode->_getDerivedPosition(),true);
	mvpRocketBootsParticle1->startTimer();
	mvpRocketBootsParticle2->startTimer();
	}
}
/*---------------------------------------------------------------------------------*/
void Player::resetRocketBoots()
{
	//Resets the particle system
	mvpRocketBootsParticle1->setManualReset();
	mvpRocketBootsParticle2->setManualReset();
	GameFramework::getSingletonPtr()->mpSound->stopRocketSound(mtpCharNode->_getDerivedPosition());
}
/*---------------------------------------------------------------------------------*/
void Player::die()
{
	std::cout << "Dead";
	
	//Change to third person camera
	CameraHandler::getSingletonPtr()->setCamMode(Game::Cam_ThirdPerson);

	NPCHandler::resetKills();

	//Play a die sound
	GameFramework::getSingletonPtr()->mpSound->playSound("../Media/Sounds/die.ogg", mtpCharNode->_getDerivedPosition());

	//Active the dead variable
	mtDying=true;
	
	setInitialOrientation();
	setManuallyControlled(false);

	//Attach the entity to the dieNode instead of charNode. The die node will fall to the ground along with the body capsule.
	mtpCharNode->detachObject(mtpCharEnt);
	mtpDieNode->attachObject(mtpCharEnt);

	//Clearing PhysX body flags of the capsule to make it fall!
	//Important to re-enable these once the capsule is active again
	mtpHitBox->getNxActor()->clearBodyFlag(NX_BF_FROZEN_ROT_X);
	mtpHitBox->getNxActor()->clearBodyFlag(NX_BF_FROZEN_ROT_Z);
	mtpHitBox->getNxActor()->setAngularDamping(1);

	Chunks::getSingletonPtr()->resetAllNPcs();
}
/*---------------------------------------------------------------------------------*/
float Player::updateHp(float minusHP)
{
	//Removes hitpoints. If less than zero, the character dies.
	mtHPCur -= minusHP;
	GameFramework::getSingletonPtr()->mpGui->updateHP(mtHPCur);
#ifdef DEBUG
	GameFramework::getSingletonPtr()->mpGui->updateDebugHP(mtHPCur);
#endif
	if (mtHPCur <= 0)
		die();
	return mtHPCur;
}
/*---------------------------------------------------------------------------------*/
void Player::respawn()
{
			//Manually moving the physics actor to their spawn points.
		mtpHitBox->setGlobalPosition(NxOgre::Vec3(mtSpawnPoint + Ogre::Vector3(0,mtCharOffsetY,0)));
		CameraHandler::getSingletonPtr()->getCameraSphere()->setGlobalPosition(NxOgre::Vec3(mtSpawnPoint + Ogre::Vector3(0,mtCharOffsetY,0)));
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
		GameFramework::getSingletonPtr()->mpGui->updateHP(mtHPCur);

		//Finally updates the position of the character to the capsule
		updatePosition();
}