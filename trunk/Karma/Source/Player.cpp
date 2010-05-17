#include "Player.h"
#include "Chunks.h"

Player::Player(Ogre::SceneManager* sceneMgr,NxOgre::Scene* physScene,OGRE3DRenderSystem* renderSystem, Ogre::String filename, 
			   Ogre::String name, Ogre::Vector3 spawnPoint ,  float hp , float walkSpeed)

			   : Character(sceneMgr,physScene,renderSystem,filename,name,spawnPoint,GameFramework::getSingletonPtr()->mpSettings->mCharScale,hp,walkSpeed,1),
	mtCharSpeedSuper(GameFramework::getSingletonPtr()->mpSettings->mCharSpeedSuper),
	mvpCamera(GameFramework::getSingletonPtr()->mpViewport->getCamera()),
	mtScreenWidth(GameFramework::getSingletonPtr()->mpRenderWnd->getWidth()),
	mtScreenHeight(GameFramework::getSingletonPtr()->mpRenderWnd->getHeight()),
	mvpMoveBoxJoint(NULL),
	mvPowerUp(Game::PowerUp_None)
{
	//The player is not assigned to any chunk and therefore added to the dynamic characters vector. Used for updating later on.
	mtDynamicCharacters.push_back(this);

	//Creating a lot of SceneNodes that the Player will need for the different Camera Modes.
	//For more information about have the nodes works, see documentation.
	Ogre::SceneNode* camOrginNode = sceneMgr->getSceneNode("RootNode")->createChildSceneNode("CamOrginNode", Ogre::Vector3(0, CAMERA_HEIGHT, 0));
	
	//First person nodes
	Ogre::SceneNode*  camFPnode = camOrginNode->createChildSceneNode("CamFirstPersonNode");
	float mCamGunTrackerDistanceZ = GameFramework::getSingletonPtr()->mpSettings->mCamGunTrackerDistanceZ;
	camFPnode->createChildSceneNode("FirstPersonGunTrackerNode", Ogre::Vector3(0, 0, mCamGunTrackerDistanceZ));

	//Third person nodes
	Ogre::SceneNode*  camHelperNode = camOrginNode->createChildSceneNode("CamHelperNode");
	camHelperNode->createChildSceneNode("CamNoCollisionNode", Ogre::Vector3(0, 0, CAMERA_DISTANCE));
	mvpCamCollisionNode = sceneMgr->getRootSceneNode()->createChildSceneNode("CamCollisionNode",Ogre::Vector3(0, 0, CAMERA_DISTANCE));	

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
	(*ss)->setGroup(4); //hack ¨@todo så inte de kan skjuta min låda
	//Not making any other dynamic objects move
	mvpMoveBoxController->getNxActor()->raiseActorFlag(NX_AF_DISABLE_COLLISION);
	//Invisible of course.
	mvpMoveBoxController->getEntity()->setVisible(false);

	//Aimer class

	//Binds nodes and bones
	setNodes(mtpCharNode, sceneMgr);
	setBones(mtpCharEnt,Ogre::String("UpArm.L"), Ogre::String("UpArm.R"),Ogre::String("Head"));

	//Some animations might have keyframes for these bones. If not destroyed, weird things will happen.
	destroyNodeTracks(mtpCharEnt->getSkeleton()->getAnimation(0)); //@todo 0 utbytt mot config

	//Add visual effects such as gun, muzzle fire,Rocket Boots, MoveBox particles and blood (for targets)
	addGun("Hand.R",Ogre::Quaternion(1,0,0,0), Ogre::Vector3(0,0.4,0),
		"Hand.L",Ogre::Quaternion(0,0,1,0)  ,Ogre::Vector3(0,0.4,0) );//@todo config...
	addMuzzleFire("Hand.R",Ogre::Quaternion(1,0,0,0), Ogre::Vector3(0,1.0,0),
		"Hand.L",Ogre::Quaternion(0,0,1,0)  ,Ogre::Vector3(0,1.0,0) );
	addBloodParticleSystem();	
	mvpMoveBoxParticles = new ManuallyControlledParticles(sceneMgr,name + Ogre::String("MoveBoxParticles"),"Examples/GreenyNimbus");
	mvpRocketBootsParticle1 = new ManuallyControlledParticles(sceneMgr,name + Ogre::String("RocketBoots1"),"rocketboots");
	mvpRocketBootsParticle2 = new ManuallyControlledParticles(sceneMgr,name + Ogre::String("RocketBoots2"),"rocketboots");

	//Attach the manually controlled particle systems to the bones
	mtpCharEnt->attachObjectToBone("Hand.R",mvpMoveBoxParticles->getParticleSystem(),Ogre::Quaternion(1,0,0,0),Ogre::Vector3(0,0.7,-0.7));
	mtpCharEnt->attachObjectToBone("LowLeg.L",mvpRocketBootsParticle1->getParticleSystem(),Ogre::Quaternion(1,0,0,0),Ogre::Vector3(0,2.4,0));
	mtpCharEnt->attachObjectToBone("LowLeg.R",mvpRocketBootsParticle2->getParticleSystem(),Ogre::Quaternion(1,0,0,0),Ogre::Vector3(0,2.4,0));
	
	//Optional, attach a sphere to the gunTrackerNode
	/*Ogre::Entity* sphereEnt = sceneMgr->createEntity("Sphere", "sphere.mesh");
	Ogre::SceneNode* node = sceneMgr->getSceneNode("GunTrackNode");
	node->attachObject(sphereEnt);
	node->setScale(0.001f, 0.001f, 0.001f);*/
}
/*---------------------------------------------------------------------------------*/
Player::~Player()
{
	delete mvpMoveBoxParticles;
	delete mvpRocketBootsParticle1;
	delete mvpRocketBootsParticle2;
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
		//setPowerUp(Game::PowerUp_None);
		GameFramework::getSingletonPtr()->mpGui->changeIngameUIIcon(Game::PowerUp_None);//på flera ställen fixah
		GameFramework::getSingletonPtr()->mpGui->hideCastingBar();
	}
}
/*---------------------------------------------------------------------------------*/
void Player::setPowerUp(const int& p,float time)
{
	/*mvCastingBarTimer = 0.0f;
	mvPowerUp = p;
	GameFramework::getSingletonPtr()->mpGui->hideCastingBar();*/

	//Not 100% done
	if (time!=0)
	{
		mvCastingBarTimer = float(time);
		GameFramework::getSingletonPtr()->mpGui->loadCastingBar("SuperSpeed",mvCastingBarTimer);
	}

	if (p == Game::PowerUp_None)
		mvPowerUp = Game::PowerUp_None;

	if (p == Game::PowerUp_SuperJump)
		mvPowerUp = (mvPowerUp | Game::PowerUp_SuperJump) & (~Game::PowerUp_RocketBoots);

	if (p == Game::PowerUp_RocketBoots)
		mvPowerUp = (mvPowerUp | Game::PowerUp_RocketBoots) & (~Game::PowerUp_SuperJump);

	if (p == Game::PowerUp_SuperSpeed)
		mvPowerUp = mvPowerUp | Game::PowerUp_SuperSpeed;

	if (p == Game::PowerUp_MoveBox)
		mvPowerUp = (mvPowerUp | Game::PowerUp_MoveBox) & (~Game::PowerUp_GunMode);			

	if (p == Game::PowerUp_GunMode)
		mvPowerUp = (mvPowerUp | Game::PowerUp_GunMode) & (~Game::PowerUp_MoveBox);		
	std::cout << "\nmPowerUP = " << mvPowerUp << "\n";
}
/*---------------------------------------------------------------------------------*/
void Player::move(const double& timeSinceLastFrame)
{
	Ogre::Vector3 dirCamToChar;
	//Get the direction from the camera to the charcter or character to GunTracker node
	//Depends on PowerUp
	if ((mvPowerUp & (Game::PowerUp_GunMode | Game::PowerUp_MoveBox)) == 0)
	{
		dirCamToChar =  mtpCharNode->_getDerivedPosition() - mvpCamCollisionNode->_getDerivedPosition();
	}
	else
		dirCamToChar =  mtpCharNode->_getDerivedPosition() - mvpCamGunModeNode->_getDerivedPosition();
	//Update to debugger << DEBUG >>
	GameFramework::getSingletonPtr()->mpGui->updateDebugDirXYZ(dirCamToChar);
	//Setting the y scalar to zero. No movement in y axis.
	dirCamToChar.y = 0;
	//Movement velocity not dependent of the distance between Character and Camera.
	dirCamToChar.normalise();
	
	//Get the same orientation on the character as the viewing direction
	rotateCharacter(mtpCharNode,dirCamToChar,mtFaceDirection);
	
	//If the mouse has moved and the bones need an update
	if (mvNeedUpdateBones)
	{
		updateBones(mtvGunTrackNode);
		mvNeedUpdateBones = false;
	}

	//How fast to walk/run
	if (mtMoveDir != Game::Move_None)
	{
		Ogre::Real walkSpeed = mtWalkSpeed;
		//Check if the PowerUp SuperSpeed is enabled or not
		if ((mvPowerUp & Game::PowerUp_SuperSpeed) != 0)
		{
			walkSpeed = mtCharSpeedSuper;
			Ogre::CompositorManager::getSingleton().setCompositorEnabled(GameFramework::getSingletonPtr()->mpViewport, "Motion Blur", true);
		}

		//Set walking speed depending on the direction the player is going to walk.
		//Also changes animation (At the moment only Walk(forward) available)
		if  (mtMoveDir == Game::Move_Forward )
		{
			changeAnimation("Walk", timeSinceLastFrame);
			walkSpeed *= mtCharSpeedForward;
		}
		if  (mtMoveDir == Game::Move_Backward )
		{
			//changeAnimation("WalkBackwards", timeSinceLastFrame);
			changeAnimation("Walk", timeSinceLastFrame);
			walkSpeed *= mtCharSpeedBackward;
		}
		if  (mtMoveDir == Game::Move_StrafeLeft )
		{
			//changeAnimation("StrafeLeft", timeSinceLastFrame);
			changeAnimation("Walk", timeSinceLastFrame);
			walkSpeed *= mtCharSpeedStrafeLeft;
		}
		if  (mtMoveDir == Game::Move_StrafeRight )
		{
			//changeAnimation("StrafeRight", timeSinceLastFrame);
			changeAnimation("Walk", timeSinceLastFrame);
			walkSpeed *= mtCharSpeedStrafeRight;
		}

		//Adds linear velocity to the capsule. Linear Velocity on Y axis, same as previous(Only gravity can change it)
		mtpHitBox->setLinearVelocity(NxOgre::Vec3(dirCamToChar.x*walkSpeed ,mtpHitBox->getLinearVelocity().y,dirCamToChar.z*walkSpeed ));
	}
	else
	{    
		//No movement, set character animation to idle
		//At the moment no Idle animation available
		//changeAnimation("Idle2", timeSinceLastFrame);
	}
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
	Ogre::Ray mouseRay = mvpCamera->getCameraToViewportRay(e.state.X.abs/mtScreenWidth,e.state.Y.abs/mtScreenHeight);

	//Get orgin and direction of ray
	NxOgre::Vec3 rayDirection(mouseRay.getDirection());
	NxOgre::Vec3 rayOrgin(mouseRay.getOrigin());

	//NxOgre Raycast. Only hitting dynamic objects . No intersecting with collision group 1
	//@todo fix collision groups
	NxOgre::RaycastHit rayCastResult = mvpMoveBoxController->getScene()->raycastClosestShape(NxOgre::Ray(rayOrgin,rayDirection),NxOgre::Enums::ShapesType_Dynamic,1);

	//True if it hits a Rigid Body
	if (rayCastResult.mRigidBody)
	{
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
		GameFramework::getSingletonPtr()->mpGui->loadCastingBar("Force",mvCastingBarTimer);	
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

	//Update to debugger << DEBUG >>
	GameFramework::getSingletonPtr()->mpGui->updateDebugCharXYZ(hitBoxPos.as<Ogre::Vector3>());
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
		//Update to debugger << DEBUG >>
		GameFramework::getSingletonPtr()->mpGui->updateCurChunk(mtChunk.x,mtChunk.y);

		//Updates the current active "Chunk"
		std::cout << "\nNew Chunk Pos! x: " << mtChunk.x << " Y: " << mtChunk.y;
		Chunks::getSingletonPtr()->setActiveChunkArea(mtChunk,change);
	}
}
/*---------------------------------------------------------------------------------*/
void Player::queueFire(const double& x,const double& y)
{
	//Sends a ray to the Aimer::fire();
	Ogre::Ray mouseRay = mvpCamera->getCameraToViewportRay(x,y);
	fire(mouseRay.getOrigin(), mouseRay.getDirection(),Game::CollisionGroup_Player | Game::CollisionGroup_MoveBoxReference);
}
/*---------------------------------------------------------------------------------*/
void Player::rocketBoots()
{
	//@todo ska inte bero på fps
	if (mtpHitBox->getLinearVelocity().y < -5)
		mtpHitBox->addForce(NxOgre::Vec3(0,1000*3,0));
	else
		mtpHitBox->addForce(NxOgre::Vec3(0,1000,0));
}
/*---------------------------------------------------------------------------------*/
void Player::startRocketBoots()
{
	//Starts the particle system
	mvpRocketBootsParticle1->startTimer();
	mvpRocketBootsParticle2->startTimer();
}
/*---------------------------------------------------------------------------------*/
void Player::resetRocketBoots()
{
	//Resets the particle system
	mvpRocketBootsParticle1->setManualReset();
	mvpRocketBootsParticle2->setManualReset();
}
/*---------------------------------------------------------------------------------*/