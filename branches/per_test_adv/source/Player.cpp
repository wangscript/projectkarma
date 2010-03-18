#include "Player.h"
#include <iostream>
using namespace Ogre;
//
// Collects ray contact information. This object will pretty much
// only record the closest contact point.
//
struct  AllRayResultCallback : public btCollisionWorld::RayResultCallback
{
    AllRayResultCallback(const btVector3& rayFromWorld,
            const btVector3& rayToWorld)
    : m_rayFromWorld(rayFromWorld), m_rayToWorld(rayToWorld), m_closestHitFraction(1.0)
    {
    }

    btVector3   m_rayFromWorld;
    btVector3   m_rayToWorld;
    btVector3   m_hitNormalWorld;
    btVector3   m_hitPointWorld;
    float m_closestHitFraction;
        
    virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult,
        bool normalInWorldSpace)
    {
        // Is the new result closer than the last. If so, save as the
        // closest
        if (rayResult.m_hitFraction < m_closestHitFraction)
            m_closestHitFraction = rayResult.m_hitFraction;

        m_collisionObject = rayResult.m_collisionObject;
        if (normalInWorldSpace) {
            m_hitNormalWorld = rayResult.m_hitNormalLocal;
        } 
        else {
            ///need to transform normal into worldspace
            m_hitNormalWorld = m_collisionObject->getWorldTransform().getBasis()*rayResult.m_hitNormalLocal;
        }
        
        m_hitPointWorld.setInterpolate3(m_rayFromWorld,m_rayToWorld,m_closestHitFraction);
        return 1.f;
    }
};


Player::Player(SceneManager* sceneMgr, btCollisionWorld* btColWorld) : 
    mSceneMgr(sceneMgr), mColWorld(btColWorld) 
{
	mSceneMgr = sceneMgr; 
	mColWorld = btColWorld;
	latestValid = (0,0,30);
}
	
Player::~Player() 
{
}


void Player::init() 
{
    //
    // create your entities and scene nodes here
    //
    charEnt = mSceneMgr->createEntity("Ninja", "ninja.mesh");
	charEnt->setCastShadows(true);
	charNode = mSceneMgr->getSceneNode("NinjaNode");
	charNode->setScale(Vector3(0.03,0.03,0.03));
	charNode->attachObject(charEnt);
	charNode->showBoundingBox(true); 

    //mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);

      // make a light to see stuff with

//
    // For part 3 on the lab page, create your cylinder volume 
    // here.
    //
    // Basic steps:
    //
    // 1.) create the shape
    // 2.) create and set the collision object
    // 3.) add the collision object to the collision world
    //
    // Example:
    //
    //
	
	float scale = 0.03;
	Vector3 aabbExtents = charEnt->getBoundingBox().getHalfSize();
    btVector3 cylHalfExtents(aabbExtents[0] * scale * 0.8 , aabbExtents[1] *  scale * 0.01, aabbExtents[2] * scale*0.8);
    mCylShape = new btCylinderShape(cylHalfExtents);
    mCylColObj = new btCollisionObject();
    mCylColObj->setCollisionShape(mCylShape);
    //
    btTransform trans;
    trans.setIdentity();
    trans.setOrigin(btVector3(0.0, aabbExtents[1] *  scale , 30.0));
    mCylColObj->setWorldTransform(trans);
    mColWorld->addCollisionObject(mCylColObj, 1, 2);


        debugRayObject = new ManualObject("debugRay"); 
        debugRayNode = mSceneMgr->getSceneNode("NinjaNode")->createChildSceneNode("debugRayNode");

        MaterialPtr myManualObjectMaterial = MaterialManager::getSingleton().create("rayMaterial", "debugger"); 
        myManualObjectMaterial->setReceiveShadows(false); 
        myManualObjectMaterial->getTechnique(0)->setLightingEnabled(true); 
        myManualObjectMaterial->getTechnique(0)->getPass(0)->setDiffuse(0,0,1,0); 
        myManualObjectMaterial->getTechnique(0)->getPass(0)->setAmbient(0,0,1); 
        myManualObjectMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(0,0,1);

        debugRayObject->begin("rayMaterial", Ogre::RenderOperation::OT_LINE_LIST); 
        debugRayObject->position(0.0, 0.0, 0.0);
        debugRayObject->position(0.0, aabbExtents.y, 0.0);
        debugRayObject->end();
        debugRayNode->attachObject(debugRayObject);
}

// Move your character according to the direction specified
void Player::move(Vector3 dir)
{
    // You will not only need to move your scene node, but the
    // collision volumes as well. Then you will need to do some
    // collision checks to determine if the move is valid. If not, you
    // should move back to your last state in which you were not
    // colliding.

	charNode->translate(dir, Node::TS_LOCAL);
    



	//
    // For part 2 you will need to fire a ray using btCollisionWorld.
    // This is relatively simple.
    //
	 Vector3 tempFrom = charNode->_getDerivedPosition();
	 btVector3 from = btVector3(btScalar(tempFrom.x),btScalar(tempFrom.y +4 ),btScalar(tempFrom.z));
	btVector3 to = btVector3(btScalar(tempFrom.x),btScalar(tempFrom.y-50),btScalar(tempFrom.z));
   
    // ...
    //
	
    AllRayResultCallback myCback(from, to);
    mColWorld->rayTest(from, to, myCback);
    
	//AllRayResultCallback *ptr = new AllRayResultCallback(from, to);
	//ptr->AddSingleResult();
	//
    // You can get the point of closest collision through
    // 
	//Vector3 loltestet = myCback.m_hitPointWorld;
	
	charNode->setPosition(tempFrom.x, myCback.m_hitPointWorld.y()+0.05,tempFrom.z );
	mSceneMgr->getSceneNode("FakeNode")->setPosition(tempFrom.x, myCback.m_hitPointWorld.y()+10,tempFrom.z );

	float scale = 0.03;
	Vector3 aabbExtents = charEnt->getBoundingBox().getHalfSize();
	btTransform trans;
    trans.setIdentity();
	//Vector3 currposse = charNode->_getDerivedPosition();
	trans.setOrigin(btVector3(tempFrom.x, aabbExtents[1] *scale+( myCback.m_hitPointWorld.y()+0.05),tempFrom.z ));
    mCylColObj->setWorldTransform(trans);

    // 
    // after the call to rayTest
    //
    
    // For part 3 you can determine if you have made any contacts with
    // the environment through the following sample code. Note: this
    // contact information is generated from the
    // mColWorld->performDiscreteCollisionDetection() call in
    // MyFrameListener::frameStarted code and you are simply querying
    // the results.
    //
    //

    //
    // This will tell you the total number of geometry intersections
    // in the world. Given that the world meshes are set up as to only
    // contact with the player (see collision groups in the manual), 
    // all of the contact points generated
    // should be between our player and the world. To query the
    // contact point count specifically for our character, you can
    // look at the btCollisionObject in collision using 
    //
                
	 //btCollisionObject* obA = static_cast<btCollisionObject*>(contactManifold->getBody0());
     //btCollisionObject* obB = static_cast<btCollisionObject*>(contactManifold->getBody1());

				/*if (nContact>0)
				{
					//charNode->translate(-2 * dir, Node::TS_LOCAL);
					charNode->setPosition(latestValid.x, latestValid.y ,latestValid.z+0.4 );
//SceneMgr->getSceneNode("FakeNode")->setPosition(tempFrom.x, myCback.m_hitPointWorld.y()+10,tempFrom.z );
					float scale = 0.03;
				Vector3 aabbExtents = charEnt->getBoundingBox().getHalfSize();
				btTransform trans;
				trans.setIdentity();
				//Vector3 currposse = charNode->_getDerivedPosition();
				trans.setOrigin(btVector3(latestValid.x,latestValid.y + ( aabbExtents[1] *scale/ 2),latestValid.z+0.4));
				mCylColObj->setWorldTransform(trans);
				}
				else
				{
					latestValid = charNode->_getDerivedPosition();
				}*/

		//
    // and
    //
  // contactManifold->getBody1()
    //
    // Use the API reference guide to get more information on these
    // functions.
    //
    // If you set up everything correctly,
    // it should only be >0 when your characters cylinder volume is in
    // contact with the world.
}

bool Player::vardetettvalidmove()
{
	     int nContact = 0;
    int nManifolds = mColWorld->getDispatcher()->getNumManifolds();
    //
     for (int i = 0; i < nManifolds; ++i) {
         btPersistentManifold* contactManifold = mColWorld->getDispatcher()->getManifoldByIndexInternal(i);
         btCollisionObject* obA = static_cast<btCollisionObject*>(contactManifold->getBody0());
         btCollisionObject* obB = static_cast<btCollisionObject*>(contactManifold->getBody1());
         if (obA==mCylColObj || obB ==mCylColObj)
		 nContact += contactManifold->getNumContacts();
     }
	 if (nContact>0)
	 {
		 std::cout << "DONK! \n";
		 return false;
		 		 
	 }
	 else
	 {

		 return true;
	 }
}

void Player::updateFrameBegin(const FrameEvent& evt)
{
    evtFrameBegin = evt;
}

void Player::updateFrameEnd(const FrameEvent& evt)
{
    evtFrameEnd = evt;       
}

