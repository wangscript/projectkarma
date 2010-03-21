//*****************************

#include "TestState.h"
#include "Ogre.h"

#include "Player.h"

//*****************************

using namespace Ogre;

//*****************************

static int gForward = 0;
static int gBackward = 0;
static int gLeft = 0;
static int gRight = 0;
static int gJump = 0;

btVector3 forwardDir;
btVector3 upDir;
btVector3 strafeDir;
btVector3 walkDirection;
btScalar walkVelocity;
btScalar walkSpeed;
btTransform xform;
btTransform startTransform;


TestState::TestState()
:
m_indexVertexArrays(0),
m_vertices(0)

{
	m_MoveSpeed			= 0.1;
	m_RotateSpeed		= 0.3;

	m_pCurrentObject	= 0;
	


}

//******************************

void TestState::enter()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Entering TestState...");

	m_pSceneMgr = OgreFramework::getSingletonPtr()->m_pRoot->createSceneManager(ST_GENERIC,"TestSceneMgr");
	m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.7,0.7,0.7));
	
	m_pCamera = m_pSceneMgr->createCamera("TestCamera");
	m_pCamera->setPosition(Vector3(5,60,-100));
	m_pCamera->lookAt(Vector3(0,0,0));
	m_pCamera->setNearClipDistance(5);
	
	m_pCamera->setAspectRatio(Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualWidth() /
		Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualHeight())));
	
	OgreFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);

	OgreFramework::getSingletonPtr()->m_pKeyboard->setEventCallback(this);
	OgreFramework::getSingletonPtr()->m_pMouse->setEventCallback(this);

	m_bLMouseDown = m_bRMouseDown = false;
	m_bQuit = false;

	createScene();

	initPhysics();

}

void TestState::initPhysics()
{
	//init physics here
	btCollisionShape* groundShape = new btBoxShape(btVector3(100,3,100));	
	m_collisionShapes.push_back(groundShape);

	btCollisionObject* colObj = new btCollisionObject();
	colObj->setCollisionShape(groundShape);
	
	m_collisionConfiguration = new btDefaultCollisionConfiguration();	
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
	btVector3 worldMin(-1000,-1000,-1000);
	btVector3 worldMax(1000,1000,1000);
	btAxisSweep3* sweepBP = new btAxisSweep3(worldMin,worldMax);
	m_overlappingPairCache = sweepBP;

	m_constraintSolver = new btSequentialImpulseConstraintSolver();
	m_pDynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher,m_overlappingPairCache,m_constraintSolver,m_collisionConfiguration);


	m_pDynamicsWorld->addCollisionObject(colObj);

	startTransform.setIdentity ();
	//startTransform.setOrigin (btVector3(0.0, 4.0, 0.0));
	startTransform.setOrigin (btVector3(0,100,0));
	//startTransform.setOrigin(btVector3(0,300,0));


	m_ghostObject = new btPairCachingGhostObject();
	m_ghostObject->setWorldTransform(startTransform);
	sweepBP->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
	btScalar characterHeight=1.75;
	btScalar characterWidth =1.75;
	btConvexShape* capsule = new btCapsuleShape(characterWidth,characterHeight);
	m_ghostObject->setCollisionShape (capsule);
	m_ghostObject->setCollisionFlags (btCollisionObject::CF_CHARACTER_OBJECT);

	btScalar stepHeight = btScalar(0.35);
	m_character = new btKinematicCharacterController (m_ghostObject,capsule,stepHeight);
	
	m_pDynamicsWorld->addCollisionObject(m_ghostObject,btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter|btBroadphaseProxy::DefaultFilter);

	m_pDynamicsWorld->addAction(m_character);


}

//*********************************************

bool TestState::pause()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Pausing TestState...");

	return true;

}

//********************************************

void TestState::resume()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Resuming TestState...");

	OgreFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);

	m_bQuit = false;

}

//********************************************

void TestState::exit()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Leaving TestState...");

	m_pSceneMgr->destroyCamera(m_pCamera);
	if (m_pSceneMgr)
		OgreFramework::getSingletonPtr()->m_pRoot->destroySceneManager(m_pSceneMgr);
	
	//cleanup in the reverse order of creation/initialization
	if (m_character)
	{
		m_pDynamicsWorld->removeCollisionObject(m_ghostObject);
	}
	//remove the rigidbodies from the dynamics world and delete them
	int i;
	for (i=m_pDynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
	{
		btCollisionObject* obj = m_pDynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		m_pDynamicsWorld->removeCollisionObject( obj );
		delete obj;
	}

	//delete collision shapes
	for (int j=0;j<m_collisionShapes.size();j++)
	{
		btCollisionShape* shape = m_collisionShapes[j];
		delete shape;
	}

	delete m_indexVertexArrays;
	delete m_vertices;

	//delete dynamics world
	delete m_pDynamicsWorld;

	//delete solver
	delete m_constraintSolver;

	//delete broadphase
	delete m_overlappingPairCache;

	//delete dispatcher
	delete m_dispatcher;

	delete m_collisionConfiguration;

}

//*******************************************

void TestState::createScene()
{

	m_pSceneMgr->setSkyBox(true,"Examples/SpaceSkyBox");

	m_pSceneMgr->createLight("Light")->setPosition(75,75,75);

// 	Plane plane(Vector3::UNIT_Y, 0);
// 
// 	Ogre::MeshManager::getSingleton().createPlane("ground",
// 		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
// 		1500,1500,20,20,true,1,5,5,Vector3::UNIT_Z);
// 
// 	m_pGroundEntity = m_pSceneMgr->createEntity("GroundEntity", "ground");
// 	m_pGroundNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("GroundNode");
// 	m_pGroundNode->attachObject(m_pGroundEntity);
// 
// 	m_pGroundEntity->setMaterialName("Examples/Rockwall");
// 	m_pGroundEntity->setCastShadows(false);



	btQuaternion rot = startTransform.getRotation();	
	btVector3 pos = startTransform.getOrigin();
	
	m_pCharacterEntity = m_pSceneMgr->createEntity("OgreEntity", "ogrehead.mesh");
	m_pCharacterNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("OgreHeadNode",Ogre::Vector3(pos.x(),pos.y(),pos.z()),Ogre::Quaternion(rot.x(),rot.y(),rot.z(),rot.w()));
	m_pCharacterNode->attachObject(m_pCharacterEntity);
	m_pCharacterEntity->setCastShadows(true);

}

//************************************************

bool TestState::keyPressed(const OIS::KeyEvent &keyEventRef)
{

	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_ESCAPE))
	{
		m_bQuit = true;
		return true;
	}
	
	return true;
}

//*************************************************

bool TestState::keyReleased(const OIS::KeyEvent &keyEventRef)
{
	OgreFramework::getSingletonPtr()->keyReleased(keyEventRef);

	return true;
}

//***************************************************

bool TestState::mouseMoved(const OIS::MouseEvent &evt)
{
	if(m_bRMouseDown)
	{
		m_pCamera->yaw(Degree(evt.state.X.rel * -0.1));
		m_pCamera->pitch(Degree(evt.state.Y.rel * -0.1));
	}

	return true;
}

//***********************************************

bool TestState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	if(id == OIS::MB_Left)
	{
		onLeftPressed(evt);
		m_bLMouseDown = true;

	} 
	else if(id == OIS::MB_Right)
	{
		m_bRMouseDown = true;
	} 

	return true;
}

//************************************************

bool TestState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	if(id == OIS::MB_Left)
	{
		m_bLMouseDown = false;

		OgreFramework::getSingletonPtr()->m_pGUISystem->injectMouseButtonUp(CEGUI::LeftButton);
	} 
	else if(id == OIS::MB_Right)
	{
		m_bRMouseDown = false;
	}

	return true;
}

//***********************************************

void TestState::onLeftPressed(const OIS::MouseEvent &evt)
{
	//Implement stuff to do
}

//***********************************************

bool TestState::onExitButtonGame(const CEGUI::EventArgs &args)
{
	m_bQuit = true;
	return true;
}

//***********************************************

void TestState::moveCamera()
{
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_LSHIFT)) 
	m_pCamera->moveRelative(m_TranslateVector);
	m_pCamera->moveRelative(m_TranslateVector / 10);
}

//***********************************************

void TestState::getInput()
{

	//Left
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_A))
	{
		//m_TranslateVector.x = -m_MoveScale;
		btMatrix3x3 orn = m_ghostObject->getWorldTransform().getBasis();
		orn *= btMatrix3x3(btQuaternion(btVector3(0,1,0),0.01));
		m_ghostObject->getWorldTransform ().setBasis(orn);

	}
	
	//Right
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_D))
	{
		//m_TranslateVector.x = m_MoveScale;
		btMatrix3x3 orn = m_ghostObject->getWorldTransform().getBasis();
		orn *= btMatrix3x3(btQuaternion(btVector3(0,1,0),-0.01));
		m_ghostObject->getWorldTransform ().setBasis(orn);
	}
	
	//Forward
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_W))
	{
		//m_TranslateVector.z = -m_MoveScale;
		walkDirection += forwardDir;
	}
	
	//Backwards
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_S))
	{
		//m_TranslateVector.z = m_MoveScale;
		walkDirection -= forwardDir;
	}
	
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_LEFT))
	{
		m_pCamera->yaw(m_RotScale);
	}
	
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_RIGHT))
	{
		m_pCamera->yaw(-m_RotScale);
	}
	
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_UP))
	{
		m_pCamera->pitch(m_RotScale);
	}

	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_DOWN))
	{
		m_pCamera->pitch(-m_RotScale);
	}
}

//************************************************

void TestState::update(double timeSinceLastFrame)
{
	if(m_bQuit == true)
	{
		this->popAppState();
		return;
	}

	m_MoveScale = m_MoveSpeed   * timeSinceLastFrame;
	m_RotScale  = m_RotateSpeed * timeSinceLastFrame;

	m_TranslateVector = Vector3::ZERO;

	getInput();
	moveCamera();

	if (m_pDynamicsWorld)
	{
		//during idle mode, just run 1 simulation step maximum
		int maxSimSubSteps = 2; //m_idle ? 1 : 2;
// 		if (m_idle)
// 			dt = 1.0/420.f;

		///set walkDirection for our character
		
		xform = m_ghostObject->getWorldTransform ();

		forwardDir = xform.getBasis()[2];
		//	printf("forwardDir=%f,%f,%f\n",forwardDir[0],forwardDir[1],forwardDir[2]);
		upDir = xform.getBasis()[1];
		strafeDir = xform.getBasis()[0];
		forwardDir.normalize ();
		upDir.normalize ();
		strafeDir.normalize ();

		walkDirection = btVector3(0.0, 0.0, 0.0);
		walkVelocity = btScalar(1.1) * 4.0; // 4 km/h -> 1.1 m/s
		walkSpeed = walkVelocity * timeSinceLastFrame;

		m_character->setWalkDirection(walkDirection*walkSpeed);
		

		int numSimSteps = m_pDynamicsWorld->stepSimulation(timeSinceLastFrame,maxSimSubSteps);
	

		btQuaternion rot = xform.getRotation();
		m_pCharacterNode->setOrientation(rot.w(), rot.x(), rot.y(), rot.z());
		btVector3 pos = xform.getOrigin();
		m_pCharacterNode->setPosition(pos.x(), pos.y(), pos.z());

	}

}