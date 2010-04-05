#include "CameraHandler.h"

CameraHandler::CameraHandler(Ogre::Camera* cam, Ogre::SceneNode* node, OGRE3DRenderSystem* renderSystem,Ogre::SceneManager* mSceneMgr )
{
	camera = cam;
	node->attachObject(camera);
	sceneMgr = mSceneMgr;
	mRotate = 0.10;
	camNoCollisionNode = sceneMgr->getSceneNode("CamNoCollisionNode");
	camCollisionNode = sceneMgr->getSceneNode("CamCollisionNode");
	camHelperNode = sceneMgr->getSceneNode("CamHelperNode");
	camOrginNode = sceneMgr->getSceneNode("CamOrginNode");
	charNode = sceneMgr->getSceneNode("CharNode");


	//Create sphere for collision
	//Properties
	NxOgre::RigidBodyDescription descriptionSphere;
	descriptionSphere.mBodyFlags |= NxOgre::Enums::BodyFlags_DisableGravity;
	descriptionSphere.mBodyFlags |= NxOgre::Enums::BodyFlags_FreezeRotation; 
	descriptionSphere.mMass = 100;
	descriptionSphere.mDensity = 0;
	descriptionSphere.mLinearDamping = 10;
	descriptionSphere.mAngularDamping = 10;

	/*Create it.
	cube1m mesh wont show anyway.
	NxOgre::Vec3(0,2,3) indicates the camera position*/
	cameraSphere = renderSystem->createBody(new NxOgre::Sphere(0.1), NxOgre::Vec3(0,2,3), "cube.1m.mesh", descriptionSphere);
	cameraSphere->getEntity()->setVisible(false);

	//Set Collision Flag to group 2
	NxShape* const* s2 = cameraSphere->getNxActor()->getShapes();
	(*s2)->setGroup(2);
}

CameraHandler::~CameraHandler() 
{
}

void CameraHandler::Rotate(const OIS::MouseEvent &arg)
{
    //Yaw the camera. Rotate around the Y axis
    //@todo add raycast so the camera cant get stuck behind stuff
    camOrginNode->yaw(Ogre::Degree(-mRotate * arg.state.X.rel), Ogre::Node::TS_LOCAL);

    /*It is not allowed to pitch the camera (rotate around Z axis) if it
    A. Rotates more than 90 degrees, making the world "flip"
    B. Position itself under the world.*/
    bool okToPitch = false;

    //If the relative mouse movement is negative, we only have to worry about Case B.
    if ( arg.state.Y.rel < 0 &&camNoCollisionNode->_getDerivedPosition().y > CAMERA_MIN_Y)
    {
        okToPitch = true;
    }
    //To get here the mouse movement must be positive. Case A.
    else if (camNoCollisionNode->_getDerivedPosition().y < CAMERA_MAX_Y)
    {
        okToPitch = true;
    }

    if (okToPitch)
    {
        camHelperNode->pitch(Ogre::Degree(-mRotate * arg.state.Y.rel), Ogre::Node::TS_LOCAL);
    }
}


void CameraHandler::AdjustCamera()
{
    //Get position of imaginary camera and the bounding sphere.
    Ogre::Vector3 noCollisionPos = camNoCollisionNode->_getDerivedPosition();
    NxOgre::Vec3 collisionSpherePos = cameraSphere->getGlobalPosition();

    //Get direction from bounding sphere to imaginary camera and set it as linear velocity
    NxOgre::Vec3 linearVelocityDirection;
    linearVelocityDirection.x = (noCollisionPos.x - collisionSpherePos.x) * CAMERA_VELOCITY_XZ;
    linearVelocityDirection.y = (noCollisionPos.y - collisionSpherePos.y) * CAMERA_VELOCITY_Y;
    linearVelocityDirection.z = (noCollisionPos.z - collisionSpherePos.z) * CAMERA_VELOCITY_XZ  ;
    cameraSphere->setLinearVelocity(linearVelocityDirection);
}

void CameraHandler::MoveCamera()
{
    //Get position of bounding sphere and set the camera's position
    NxOgre::Vec3 collisionSpherePos = cameraSphere->getGlobalPosition();
    camCollisionNode->setPosition(Ogre::Vector3(collisionSpherePos.x,collisionSpherePos.y,collisionSpherePos.z));

    //So the camera always will look at the character  
    Ogre::Vector3 directionToCharacter =  charNode->_getDerivedPosition() - camCollisionNode->_getDerivedPosition();
    camera->setDirection(directionToCharacter);
}

