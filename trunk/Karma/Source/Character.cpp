#include "Character.h"

Character::Character(Ogre::SceneManager* sceneMgr, OGRE3DRenderSystem* renderSystem)
{
	//Create Character and attach it 
	charEnt = sceneMgr->createEntity("Char", "Ogre.mesh");
	charEnt->setCastShadows(true);
	charNode = sceneMgr->getSceneNode("CharNode");
	charNode->translate(Ogre::Vector3(CHARACTER_ADJUST_X,CHARACTER_ADJUST_Y,CHARACTER_ADJUST_Z));
	charNode->yaw( Ogre::Degree( CHARACTER_ROTATION ) );//Ogre.mesh "felvänd"
	charNode->attachObject(charEnt);
	charNode->setScale(CHARACTER_SCALE,CHARACTER_SCALE,CHARACTER_SCALE);

	//Initiate stuff
	m_AnimationState= NULL;
	jumping=false;
	mMoveDir = Move_None;
	mCamera = GameFramework::getSingletonPtr()->mpViewport->getCamera();
	mMoveBoxJoint = NULL;

	//Pointer to cam node. Going to be used a lot
	camCollisionNode = sceneMgr->getSceneNode("CamCollisionNode");

	//Importing NxOgre skeleton for our bounding capsule
	NxOgre::Mesh* convexMesh  = NxOgre::MeshManager::getSingleton()->load("nxogre:capsule.xns");
	NxOgre::Convex* convex = new NxOgre::Convex(convexMesh);

	//Properties for bounding capsule
	NxOgre::RigidBodyDescription description;
	description.mMass = BOUNDING_CAPSULE_MASS;
	description.mBodyFlags |= NxOgre::Enums::BodyFlags_FreezeRotation; 

	//Add capsule to physics world and set Collision Flag to group 1
	mCapsule = renderSystem->createBody(convex, NxOgre::Vec3(0, 0, 0), "capsule.mesh", description);
	mCapsule->getEntity()->setVisible(false);
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
	
	if (mPowerUp == PowerUp_MoveBox)
	{

	}

	//Get the direction from the camera to the charcter
	Ogre::Vector3 dirCamToChar =  charNode->_getDerivedPosition() - camCollisionNode->_getDerivedPosition();
	//Update to debugger << DEBUG >>
	GameFramework::getSingletonPtr()->mpGui->updateDebugDirXYZ(dirCamToChar);
	//Setting the y scalar to zero. No movement in y axis.
	dirCamToChar.y = 0;
	//Movement velocity not dependent of the distance between Character and Camera.
	dirCamToChar.normalise();
	//Get the same orientation on the character as the viewing direction
	Ogre::Vector3 src = charNode->getOrientation()  * (Ogre::Vector3(0,0,1));
	Ogre::Quaternion quat = src.getRotationTo(dirCamToChar);
	charNode->rotate(quat);
	//How fast to walk/run
	Ogre::Real walkSpeed;
	if (mMoveDir != Move_None)
	{
		//Check if the PowerUp SuperSpeed is enabled or not
		if (mPowerUp == PowerUp_SuperSpeed)
		{
			walkSpeed = SUPERSPEED;
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
			walkSpeed *= FORWARD_SPEED;
		}
		if  (mMoveDir == Move_Backward )
		{
			//changeAnimation("WalkBackwards", timeSinceLastFrame);
			changeAnimation("Walk", timeSinceLastFrame);
			walkSpeed *= BACKWARD_SPEED;
		}
		if  (mMoveDir == Move_StrafeLeft )
		{
			//changeAnimation("StrafeLeft", timeSinceLastFrame);
			changeAnimation("Walk", timeSinceLastFrame);
			walkSpeed *= STRAFE_LEFT_SPEED;
		}
		if  (mMoveDir == Move_StrafeRight )
		{
			//changeAnimation("StrafeRight", timeSinceLastFrame);
			changeAnimation("Walk", timeSinceLastFrame);
			walkSpeed *= STRAFE_RIGHT_SPEED;
		}

		//Character has moved! move = true
		move = true;
	}
	else
	{    
		//No movement, set character animation to idle
		changeAnimation("Idle2", timeSinceLastFrame);
		walkSpeed = 0;
		//move = false
		move = false;
	}
	//Adds linear velocity to the capsule. Linear Velocity on Y axis, same as previous(Only gravity can change it)
	mCapsule->setLinearVelocity(NxOgre::Vec3(dirCamToChar.x*walkSpeed ,mCapsule->getLinearVelocity().y,dirCamToChar.z*walkSpeed ));

	//Get position of capsule and set the characters position to the capsule's position.
	NxOgre::Vec3 capsulePos = mCapsule->getGlobalPosition();
	//Move the "RootNode". CHARACTER_ADJUST_Y , translate node if orgin is not in origo
	charNode->getParentSceneNode()->setPosition(capsulePos.x ,capsulePos.y,capsulePos.z);
	//Update to debugger << DEBUG >>
	GameFramework::getSingletonPtr()->mpGui->updateDebugCharXYZ(capsulePos.as<Ogre::Vector3>());


	return move;

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
	}

}
void Character::releaseBox()
{
	//Destroys the joint between the Rigid body object and the BoxController
	mMoveBoxController->getScene()->destroyJoint(mMoveBoxJoint);
	mMoveBoxJoint = NULL;
}

