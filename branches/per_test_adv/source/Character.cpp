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

	//Pointer to cam node. Going to be used a lot
	camCollisionNode = sceneMgr->getSceneNode("CamCollisionNode");

	//Importing NxOgre skeleton for our bounding capsule
	NxOgre::Mesh* convexMesh  = NxOgre::MeshManager::getSingleton()->load("media:capsule.xns");
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
			animState = charEnt->getAnimationState("Walk");
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
	//Get the direction from the camera to the charcter
	Ogre::Vector3 dirCamToChar =  charNode->_getDerivedPosition() - camCollisionNode->_getDerivedPosition();
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
			Ogre::CompositorManager::getSingleton().setCompositorEnabled(OgreFramework::getSingletonPtr()->m_pViewport, "Motion Blur", true);
		}
		else
		{
			walkSpeed = 1;
			Ogre::CompositorManager::getSingleton().setCompositorEnabled(OgreFramework::getSingletonPtr()->m_pViewport, "Motion Blur", false);
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

