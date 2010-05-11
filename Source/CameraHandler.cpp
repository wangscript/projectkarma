#include "CameraHandler.h"

CameraHandler::CameraHandler(Ogre::Camera* cam, Ogre::SceneNode* node, OGRE3DRenderSystem* renderSystem,Ogre::SceneManager* mSceneMgr )
{
	camera = cam;
	node->attachObject(camera);
	sceneMgr = mSceneMgr;

	mtRotate = GameFramework::getSingletonPtr()->mpSettings->mCamRotate;
	mtZoom = GameFramework::getSingletonPtr()->mpSettings->mCamZoom;
	mtCamVelocityY = GameFramework::getSingletonPtr()->mpSettings->mCamVelocityY;
	mtCamVelocityXZ = GameFramework::getSingletonPtr()->mpSettings->mCamVelocityXZ;
	mtCamHeightMaxY = GameFramework::getSingletonPtr()->mpSettings->mCamHeightMaxY;
	mtCamHeightMinY = GameFramework::getSingletonPtr()->mpSettings->mCamHeightMinY;
	mtCamDistanceMax = GameFramework::getSingletonPtr()->mpSettings->mCamDistanceMax;
	mtCamDistanceMin = GameFramework::getSingletonPtr()->mpSettings->mCamDistanceMin;

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
	descriptionSphere.mLinearDamping = GameFramework::getSingletonPtr()->mpSettings->mTemp3;
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
void CameraHandler::Zoom(const OIS::MouseEvent &arg)
{
	bool okToZoom = false;
	//Direction from imaginary camera position to character
	Ogre::Vector3 directionToCharacter =  charNode->_getDerivedPosition() - camNoCollisionNode->_getDerivedPosition();
	//No direction in Y direction.
	directionToCharacter.y = 0;
	//To check if we're too close or too far
	Ogre::Real lengthToCharacter = directionToCharacter.length();

	//If zoom is positive, we only have to worry about going too close
	if (lengthToCharacter > mtCamDistanceMin && arg.state.Z.rel > 0)
	{
		okToZoom = true;
	}
	//If zoom is negative, we only have to worry about going too far
	else if (lengthToCharacter < mtCamDistanceMax && arg.state.Z.rel < 0)
	{
		okToZoom = true;
	}

	if (okToZoom)
	{
	//Always the same speed, not dependent of the distance
	directionToCharacter.normalise();
	//At the startup, the camera was translated @ Z axis.
	camNoCollisionNode->translate(Ogre::Vector3::NEGATIVE_UNIT_Z * mtZoom * arg.state.Z.rel, Ogre::Node::TS_LOCAL);
	}

}

void CameraHandler::Rotate(const OIS::MouseEvent &arg)
{
    //Yaw the camera. Rotate around the Y axis
    //@todo add raycast so the camera cant get stuck behind stuff
    camOrginNode->yaw(Ogre::Degree(-mtRotate * arg.state.X.rel), Ogre::Node::TS_LOCAL);

    /*It is not allowed to pitch the camera (rotate around Z axis) if it
    A. Rotates more than 90 degrees, making the world "flip"
    B. Position itself under the world.*/
    bool okToPitch = false;

    //If the relative mouse movement is negative, we only have to worry about Case B.
    if ( arg.state.Y.rel < 0 &&camNoCollisionNode->_getDerivedPosition().y > mtCamHeightMinY)
    {
        okToPitch = true;
    }
    //To get here the mouse movement must be positive. Case A.
    else if (camNoCollisionNode->_getDerivedPosition().y < mtCamHeightMaxY)
    {
        okToPitch = true;
    }

    if (okToPitch)
    {
        camHelperNode->pitch(Ogre::Degree(-mtRotate * arg.state.Y.rel), Ogre::Node::TS_LOCAL);
    }
}


void CameraHandler::AdjustCamera()
{
    //Get position of imaginary camera and the bounding sphere.
    Ogre::Vector3 noCollisionPos = camNoCollisionNode->_getDerivedPosition();
    NxOgre::Vec3 collisionSpherePos = cameraSphere->getGlobalPosition();

    //Get direction from bounding sphere to imaginary camera and set it as linear velocity
    NxOgre::Vec3 linearVelocityDirection;
    linearVelocityDirection.x = (noCollisionPos.x - collisionSpherePos.x) * mtCamVelocityXZ;
    linearVelocityDirection.y = (noCollisionPos.y - collisionSpherePos.y) * mtCamVelocityY;
    linearVelocityDirection.z = (noCollisionPos.z - collisionSpherePos.z) * mtCamVelocityXZ  ;
    cameraSphere->setLinearVelocity(linearVelocityDirection);
}

void CameraHandler::MoveCamera()
{
    //Get position of bounding sphere and set the camera's position
    NxOgre::Vec3 collisionSpherePos = cameraSphere->getGlobalPosition();
    camCollisionNode->setPosition(Ogre::Vector3(collisionSpherePos.x,collisionSpherePos.y,collisionSpherePos.z));
	//Update to debugger << DEBUG >>
	GameFramework::getSingletonPtr()->mpGui->updateDebugCamXYZ(collisionSpherePos.as<Ogre::Vector3>());

    //So the camera always will look at the character  
    Ogre::Vector3 directionToCharacter =  charNode->_getDerivedPosition() - camCollisionNode->_getDerivedPosition();
    camera->setDirection(directionToCharacter);
}

