#include "Player.h"

Player::Player(Ogre::SceneManager* sceneMgr, OGRE3DRenderSystem* renderSystem)
{
		//Create Character and attach it 
		charEnt = sceneMgr->createEntity("Char", "Ogre.mesh");
		charEnt->setCastShadows(true);
		charNode = sceneMgr->getSceneNode("CharNode");
		charNode->yaw( Ogre::Degree( CHARACTER_ROTATION ) );
		charNode->attachObject(charEnt);
		charNode->setScale(CHARACTER_SCALE,CHARACTER_SCALE,CHARACTER_SCALE);
		m_AnimationState= NULL;
		jumping=false;
		camCollisionNode = sceneMgr->getSceneNode("CamCollisionNode");


		NxOgre::Mesh* convexMesh  = NxOgre::MeshManager::getSingleton()->load("media:capsule.xns");
		NxOgre::Convex* convex = new NxOgre::Convex(convexMesh);
		//Info
		NxOgre::RigidBodyDescription description;
		float mass = 80;
		description.mMass = mass;
		//description.mBodyFlags |= NxOgre::Enums::BodyFlags_DisableGravity;
		description.mBodyFlags |= NxOgre::Enums::BodyFlags_FreezeRotation; 
		//description.mBodyFlags |= NxOgre::Enums::BodyFlags_FreezePositionY;
		mCapsule = renderSystem->createBody(convex, NxOgre::Vec3(0, 0, 0), "capsule.mesh", description);
		NxShape* const* s = mCapsule->getNxActor()->getShapes();
		(*s)->setGroup(1);
}
Player::~Player() 
{
}

void Player::changeAnimation(const Ogre::String& name,const double time)
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
		float animationspeed = 0.001;
		m_AnimationState->addTime(time*animationspeed);

		if (jumping && m_AnimationState->getTimePosition() >= m_AnimationState->getLength())
		{
			jumping=false;
			m_AnimationState->setTimePosition(0);
		}
	}
}

void Player::move(const double timeSinceLastFrame)
{
			//Behöver uppdateras. Mycket
			
			Ogre::Vector3 dirre =  charNode->_getDerivedPosition() - camCollisionNode->_getDerivedPosition();
	
			dirre.y=0;
			dirre.normalise();
			Ogre::Vector3 src = charNode->getOrientation()  * (Ogre::Vector3(0,0,1));
			Ogre::Quaternion quat = src.getRotationTo(dirre);
			charNode->rotate(quat);
			
			changeAnimation("Walk", timeSinceLastFrame);
			float scale = 3;
			mCapsule->setLinearVelocity(NxOgre::Vec3(dirre.x*scale,mCapsule->getLinearVelocity().y,dirre.z*scale));
			std::cout << dirre.x << " " << dirre.y<< " " <<dirre.z << "\n";
			NxOgre::Vec3 capPos = mCapsule->getGlobalPosition();
			charNode->getParentSceneNode()->setPosition(capPos.x,capPos.y+0.5,capPos.z);   
}