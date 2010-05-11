#include "CameraHandler.h"

CameraHandler::CameraHandler(Ogre::Camera* cam, Ogre::SceneNode* node, OGRE3DRenderSystem* renderSystem,Ogre::SceneManager* mSceneMgr )
{
	camera = cam;
	node->attachObject(camera);
	sceneMgr = mSceneMgr;
	mtCamMode = Cam_ThirdPerson;

	mtRotate = GameFramework::getSingletonPtr()->mpSettings->mCamRotate;
	mtZoom = GameFramework::getSingletonPtr()->mpSettings->mCamZoom;
	mtCamVelocityY = GameFramework::getSingletonPtr()->mpSettings->mCamVelocityY;
	mtCamVelocityXZ = GameFramework::getSingletonPtr()->mpSettings->mCamVelocityXZ;
	mtCamHeightMaxY = GameFramework::getSingletonPtr()->mpSettings->mCamHeightMaxY;
	mtCamHeightMinY = GameFramework::getSingletonPtr()->mpSettings->mCamHeightMinY;
	mtCamDistanceMax = GameFramework::getSingletonPtr()->mpSettings->mCamDistanceMax;
	mtCamDistanceMin = GameFramework::getSingletonPtr()->mpSettings->mCamDistanceMin;
	mtCamCharYAdjust = GameFramework::getSingletonPtr()->mpSettings->mCamCharYAdjust;
	mtCamGunTrackerOffsetX = GameFramework::getSingletonPtr()->mpSettings->mCamGunTrackerOffsetX;
	mtCamGunTrackerOffsetY = GameFramework::getSingletonPtr()->mpSettings->mCamGunTrackerOffsetY;
	mtCamGunTrackerOffsetZ = GameFramework::getSingletonPtr()->mpSettings->mCamGunTrackerOffsetZ;

	mtScreenWidth = GameFramework::getSingletonPtr()->mpRenderWnd->getWidth();
	mtScreenHeight = GameFramework::getSingletonPtr()->mpRenderWnd->getHeight();

	camNoCollisionNode = sceneMgr->getSceneNode("CamNoCollisionNode");
	camCollisionNode = sceneMgr->getSceneNode("CamCollisionNode");
	camHelperNode = sceneMgr->getSceneNode("CamHelperNode");
	camOrginNode = sceneMgr->getSceneNode("CamOrginNode");
	charNode = sceneMgr->getSceneNode("CharNode");
	mtpCamFirstPersonNode = sceneMgr->getSceneNode("CamFirstPersonNode");
	mtpFirstPersonGunTrackerNode = sceneMgr->getSceneNode("FirstPersonGunTrackerNode");
	mtpCamMixNode = sceneMgr->getSceneNode("CamMixNode");
	mtpCamMixHelperNode = sceneMgr->getSceneNode("CamMixHelperNode");
	mtpCursorMovableGunTrackerNode = sceneMgr->getSceneNode("CursorMovableGunTrackerNode");
	mtpCursorCenterGunTrackerNode = sceneMgr->getSceneNode("CursorCenterGunTrackerNode");


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


void CameraHandler::Rotate(const OIS::MouseEvent &arg)
{
	switch (mtCamMode)		
	{
	case Cam_ThirdPerson:
		RotateThirdPerson(arg);
		break;
	case Cam_MixCursorCenter:
		RotateMixCursorCenter(arg);
		break;
	case Cam_MixCursorMovable:
		RotateMixCursorMovable(arg);
		break;
	case Cam_FirstPerson:
		RotateFirstPerson(arg);
		break;
	case Cam_FreeMode:
		RotateFree(arg);
		break;
	}
}

void CameraHandler::RotateFree(const OIS::MouseEvent &arg)
{
	//This view is for screenshotting map overview only :)
	Ogre::Vector3 dirre =   Ogre::Vector3(-115,GameFramework::getSingletonPtr()->mpSettings->mTerrainAdjustY,12) - sceneMgr->getSceneNode("CamFree")->_getDerivedPosition();
	camera->setDirection(dirre);
	sceneMgr->getSceneNode("CamFree")->translate(arg.state.X.rel,arg.state.Z.rel,arg.state.Y.rel);
}

void CameraHandler::RotateThirdPerson(const OIS::MouseEvent &arg)
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
	GameFramework::getSingletonPtr()->mpGui->updateCursorPos(arg.state.X.abs,arg.state.Y.abs );
}
void CameraHandler::RotateFirstPerson(const OIS::MouseEvent &arg)
{
	//Yaw and pitch FirstPerson Nodes
	camOrginNode->yaw(Ogre::Degree(-mtRotate * arg.state.X.rel), Ogre::Node::TS_LOCAL);
	mtpCamFirstPersonNode->pitch(Ogre::Degree(-mtRotate * arg.state.Y.rel), Ogre::Node::TS_LOCAL);

	//Kanske inte behöver detta varje frame! Undersök!
	Ogre::Vector3 charPosition = charNode->_getDerivedPosition();
	charPosition.y += mtCamCharYAdjust;
    Ogre::Vector3 directionToGunTracker =  mtpFirstPersonGunTrackerNode->_getDerivedPosition() - charPosition  ;
    camera->setDirection(directionToGunTracker);
	mtpChar->updateBones(arg);
}
void CameraHandler::RotateMixCursorCenter(const OIS::MouseEvent &arg)
{
	//Only changes the the Y value of the GunTrackerNode
	Ogre::Real x = 0;
	Ogre::Real y = mtCamGunTrackerOffsetY - 2 - (2*mtCamGunTrackerOffsetX*arg.state.Y.abs)/mtScreenWidth;
	mtpCursorMovableGunTrackerNode->setPosition(x,y,mtCamGunTrackerOffsetZ);

	//Yaw and pitch nodes
	camOrginNode->yaw(Ogre::Degree(-mtRotate * arg.state.X.rel), Ogre::Node::TS_LOCAL);
	mtpCamMixNode->pitch(Ogre::Degree(-mtRotate * arg.state.Y.rel), Ogre::Node::TS_LOCAL);


	//So the camera always will look at the character  
	Ogre::Vector3 charPosition = charNode->_getDerivedPosition();
	charPosition.y += mtCamCharYAdjust;
    Ogre::Vector3 directionToCharacter =  charPosition - mtpCamMixHelperNode->_getDerivedPosition();
    camera->setDirection(directionToCharacter);
	mtpChar->updateBones(arg);
}

void CameraHandler::RotateMixCursorMovable(const OIS::MouseEvent &arg)
{
	//Changing the gun tracker node that the character "aims" at.
	Ogre::Real x = -mtCamGunTrackerOffsetX + (2*mtCamGunTrackerOffsetX*arg.state.X.abs)/mtScreenWidth;
	Ogre::Real y = mtCamGunTrackerOffsetY - 2 - (2*mtCamGunTrackerOffsetY*arg.state.Y.abs)/mtScreenWidth;
	mtpCursorMovableGunTrackerNode->setPosition(x,y,mtCamGunTrackerOffsetZ);

	//Only yaw available in this rotate mode.
	camOrginNode->yaw(Ogre::Degree(-mtRotate * arg.state.X.rel), Ogre::Node::TS_LOCAL);

	//So the camera always will look at the character  
    Ogre::Vector3 directionToCharacter =  charNode->_getDerivedPosition() - mtpCamMixNode->_getDerivedPosition();
	directionToCharacter.y= 0;
    camera->setDirection(directionToCharacter);
	mtpChar->updateBones(arg);
	GameFramework::getSingletonPtr()->mpGui->updateCursorPos(arg.state.X.abs,arg.state.Y.abs );
}

void CameraHandler::setCamMode(int camMode)
{
	switch (mtCamMode)		
	{
	case Cam_MixCursorMovable:
		mtpCamMixHelperNode->detachAllObjects();
		break;
	case Cam_MixCursorCenter:
		mtpCamMixHelperNode->detachAllObjects();
		break;
	case Cam_ThirdPerson:
		camCollisionNode->detachAllObjects();
		break;
	case Cam_FirstPerson:
		mtpCamFirstPersonNode->detachAllObjects();
		mtpChar->getEntity()->setVisible(true);
		Ogre::OverlayManager::getSingleton().getByName("GuiKarma/FirstPerson")->hide();
		break;
	}

	mtCamMode = camMode;

 	if(mtCamMode == Cam_MixCursorMovable )
	{
		Ogre::Vector3 directionToCharacter = charNode->_getDerivedPosition() - mtpCamMixHelperNode->_getDerivedPosition();
		directionToCharacter.y = 0;
		Ogre::Vector3  pos= mtpCursorMovableGunTrackerNode->getPosition();
		mtpCursorMovableGunTrackerNode->setPosition(0,pos.y,pos.z);
		
		mtpCamMixHelperNode->attachObject(camera);
		camera->setDirection(directionToCharacter);	
	}

 	if(mtCamMode == Cam_MixCursorCenter)
	{
		Ogre::Vector3 directionToCharacter = charNode->_getDerivedPosition() - mtpCamMixHelperNode->_getDerivedPosition();
		directionToCharacter.y = 0;
		Ogre::Vector3  pos= mtpCursorMovableGunTrackerNode->getPosition();
		mtpCursorMovableGunTrackerNode->setPosition(0,pos.y,pos.z);

		mtpCamMixHelperNode->attachObject(camera);
		camera->setDirection(directionToCharacter);	
		GameFramework::getSingletonPtr()->mpGui->updateCursorPos(400,240);
	}


	if(mtCamMode == Cam_ThirdPerson)
	{
		camCollisionNode->attachObject(camera);
		mtpChar->getEntity()->getSkeleton()->getBone("UpArm.R")->setOrientation(mtpChar->getEntity()->getSkeleton()->getBone("UpArm.R")->getInitialOrientation());
		mtpChar->getEntity()->getSkeleton()->getBone("UpArm.L")->setOrientation(mtpChar->getEntity()->getSkeleton()->getBone("UpArm.L")->getInitialOrientation());
		mtpChar->getEntity()->getSkeleton()->getBone("Head")->setOrientation(mtpChar->getEntity()->getSkeleton()->getBone("Head")->getInitialOrientation());
		mtpChar->getEntity()->getSkeleton()->getBone("Hip")->setOrientation(mtpChar->getEntity()->getSkeleton()->getBone("Hip")->getInitialOrientation());
	}

	if(mtCamMode == Cam_FirstPerson)
	{
		Ogre::Overlay* overlay = Ogre::OverlayManager::getSingleton().getByName("GuiKarma/FirstPerson");
		overlay->show();
		Ogre::Vector3  pos= mtpCursorMovableGunTrackerNode->getPosition();
		mtpCursorMovableGunTrackerNode->setPosition(0,pos.y,pos.z);
		GameFramework::getSingletonPtr()->mpGui->updateCursorPos(400,240);
		mtpCamFirstPersonNode->attachObject(camera);
		mtpChar->getEntity()->setVisible(false);
	}
	if(mtCamMode == Cam_FreeMode)
	{
		sceneMgr->getSceneNode("CamFree")->attachObject(camera);
		camera->setDirection(Ogre::Vector3(0.0f,-1.0f, 0.0f));
	}
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

