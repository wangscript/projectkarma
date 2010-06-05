#include "CameraHandler.h"
/*---------------------------------------------------------------------------------*/
CameraHandler CameraHandler::singleton;
/*---------------------------------------------------------------------------------*/
CameraHandler CameraHandler::getSingleton()
{
	return singleton;
}
/*---------------------------------------------------------------------------------*/
CameraHandler* CameraHandler::getSingletonPtr()
{
	return &singleton;
}
/*---------------------------------------------------------------------------------*/
void CameraHandler::initCameraHandler(Ogre::Camera* cam, OGRE3DRenderSystem* renderSystem,Ogre::SceneManager* mSceneMgr , Player* p)
{
	camera = cam;
	sceneMgr = mSceneMgr;

	mvpRotate = &Settings::getSingletonPtr()->mCamRotate;
	mvpZoom = &Settings::getSingletonPtr()->mCamZoom;
	mvpCamVelocityY = &Settings::getSingletonPtr()->mCamVelocityY;
	mvpCamVelocityXZ = &Settings::getSingletonPtr()->mCamVelocityXZ;
	mvpCamHeightMaxY = &Settings::getSingletonPtr()->mCamHeightMaxY;
	mvpCamHeightMinY = &Settings::getSingletonPtr()->mCamHeightMinY;
	mvpCamDistanceMax = &Settings::getSingletonPtr()->mCamDistanceMax;
	mvpCamDistanceMin = &Settings::getSingletonPtr()->mCamDistanceMin;
	mvpCamCharYAdjust = &Settings::getSingletonPtr()->mCamCharYAdjust;
	mvpCamGunTrackerOffsetX = &Settings::getSingletonPtr()->mCamGunTrackerOffsetX;
	mvpCamGunTrackerOffsetY = &Settings::getSingletonPtr()->mCamGunTrackerOffsetY;
	mvpCamGunTrackerOffsetZ = &Settings::getSingletonPtr()->mCamGunTrackerOffsetZ;

	mtScreenWidth = GameFramework::getSingletonPtr()->mpRenderWnd->getWidth();
	mtScreenHeight = GameFramework::getSingletonPtr()->mpRenderWnd->getHeight();
	camNoCollisionNode = sceneMgr->getSceneNode("CamNoCollisionNode");
	camCollisionNode = sceneMgr->getSceneNode("CamCollisionNode");
	camHelperNode = sceneMgr->getSceneNode("CamHelperNode");
	camOrginNode = sceneMgr->getSceneNode("CamOrginNode");
	charNode = sceneMgr->getSceneNode("CharNode");
	mtpCamFirstPersonNode = sceneMgr->getSceneNode("CamFirstPersonNode");
	mtpFirstPersonGunTrackerNode = sceneMgr->getSceneNode("FirstPersonGunTrackerNode");
	mtpCamGunMoveNode = sceneMgr->getSceneNode("CamMoveNode");
	mtpCamGunMoveHelperNode = sceneMgr->getSceneNode("CamMoveHelperNode");
	mtpCamGunCenterNode = sceneMgr->getSceneNode("CamCenterNode");
	mtpCamGunCenterHelperNode = sceneMgr->getSceneNode("CamCenterHelperNode");
	mtpCursorMovableGunTrackerNode = sceneMgr->getSceneNode("GunTrackNode");

	setPlayerPtr(p);
	mtCamMode = NULL;
	setCamMode(Game::Cam_ThirdPerson);
	mtPitchTrackerThirdPerson = 0;
	mtPitchTrackerSemiThirdPerson = 0;
	mtPitchTrackerFirstPerson = 0;

	//Create sphere for collision
	//Properties
	NxOgre::RigidBodyDescription descriptionSphere;
	descriptionSphere.mBodyFlags |= NxOgre::Enums::BodyFlags_DisableGravity;
	descriptionSphere.mBodyFlags |= NxOgre::Enums::BodyFlags_FreezeRotation; 
	descriptionSphere.mMass = *Settings::getSingletonPtr()->mCamMass;
	descriptionSphere.mDensity = 0;
	descriptionSphere.mLinearDamping = *Settings::getSingletonPtr()->mCamLinearDamping;
	descriptionSphere.mAngularDamping = *Settings::getSingletonPtr()->mCamAngularDamping;

	/*Create it.
	cube1m mesh wont show anyway.
	NxOgre::Vec3(0,2,3) indicates the camera position*/ //@todo spawn at player
	cameraSphere = renderSystem->createBody(new NxOgre::Sphere(*Settings::getSingletonPtr()->mCamRadius), 
		NxOgre::Vec3(Settings::getSingletonPtr()->mCamStartPosX,Settings::getSingletonPtr()->mCamStartPosY,Settings::getSingletonPtr()->mCamStartPosZ)+NxOgre::Vec3(Ogre::Vector3(-201.93,11,160.26)), 
		"cube.1m.mesh", descriptionSphere);
	cameraSphere->getEntity()->setVisible(false);

	//Set Collision Flag to group 2
	NxShape* const* s2 = cameraSphere->getNxActor()->getShapes();
	(*s2)->setGroup(2);
}
/*---------------------------------------------------------------------------------*/
void CameraHandler::Rotate(const OIS::MouseEvent &arg)
{
	switch (mtCamMode)		
	{
	case Game::Cam_ThirdPerson:
		RotateThirdPerson(arg);
		break;
	case Game::Cam_MixCursorCenter:
		RotateMixCursorCenter(arg);
		break;
	case Game::Cam_MixCursorMovable:
		RotateMixCursorMovable(arg);
		break;
	case Game::Cam_FirstPerson:
		RotateFirstPerson(arg);
		break;
	case Game::Cam_FreeMode:
		RotateFree(arg);
		break;
	}
	mtpPlayer->updateCursorPos(arg.state.X.abs,arg.state.Y.abs);
}

/*---------------------------------------------------------------------------------*/
void CameraHandler::RotateFree(const OIS::MouseEvent &arg)
{
	//This view is for screenshotting map overview only :)
	Ogre::Vector3 dirre =   - sceneMgr->getSceneNode("CamFree")->_getDerivedPosition();
    camera->setDirection(dirre);
   sceneMgr->getSceneNode("CamFree")->translate(arg.state.X.rel,arg.state.Z.rel,arg.state.Y.rel);
}
/*---------------------------------------------------------------------------------*/
void CameraHandler::Pitch(const OIS::MouseEvent &arg,Ogre::SceneNode* node,float& mtPitchTracker)
{
	/*It is not allowed to pitch the camera (rotate around Z axis) if it
    A. Rotates more than 90 degrees, making the world "flip"
    B. Position itself under the world.*/
    bool okToPitch = false;

    //If the relative mouse movement is negative, we only have to worry about Case B.
	if ( arg.state.Y.rel < 0 && mtPitchTracker < 70 )
        okToPitch = true;
    //To get here the mouse movement must be positive. Case A.
    else if ( arg.state.Y.rel > 0 && mtPitchTracker > -70)
        okToPitch = true;

    if (okToPitch)
    {
        node->pitch(Ogre::Degree(-(*mvpRotate) * arg.state.Y.rel), Ogre::Node::TS_LOCAL);
		mtPitchTracker+= -(*mvpRotate) * arg.state.Y.rel;
    }
}

void CameraHandler::RotateThirdPerson(const OIS::MouseEvent &arg)
{
   //Yaw the camera. Rotate around the Y axis
    //@todo add raycast so the camera cant get stuck behind stuff
    camOrginNode->yaw(Ogre::Degree(-(*mvpRotate) * arg.state.X.rel), Ogre::Node::TS_LOCAL);
	Pitch(arg,camHelperNode,mtPitchTrackerThirdPerson);
	GameFramework::getSingletonPtr()->mpGui->updateCursorPos(arg.state.X.abs,arg.state.Y.abs);
}
/*---------------------------------------------------------------------------------*/
void CameraHandler::RotateFirstPerson(const OIS::MouseEvent &arg)
{
	//Yaw and pitch FirstPerson Nodes
	camOrginNode->yaw(Ogre::Degree(-(*mvpRotate) * arg.state.X.rel), Ogre::Node::TS_LOCAL);
	Pitch(arg,mtpCamFirstPersonNode,mtPitchTrackerFirstPerson);

	//Kanske inte behöver detta varje frame! Undersök!
	Ogre::Vector3 charPosition = charNode->_getDerivedPosition();
	charPosition.y += *mvpCamCharYAdjust;
    Ogre::Vector3 directionToGunTracker =  mtpFirstPersonGunTrackerNode->_getDerivedPosition() - charPosition  ;
    camera->setDirection(directionToGunTracker);
	mtpPlayer->setNeedUpdateBones();
}
/*---------------------------------------------------------------------------------*/
void CameraHandler::RotateMixCursorCenter(const OIS::MouseEvent &arg)
{
	//Only changes the the Y value of the GunTrackerNode
	Ogre::Real x = 0;
	Ogre::Real y = *mvpCamGunTrackerOffsetY - 2 - (2**mvpCamGunTrackerOffsetX*arg.state.Y.abs)/mtScreenWidth;
	mtpCursorMovableGunTrackerNode->setPosition(x,y,*mvpCamGunTrackerOffsetZ);

	//Yaw and pitch nodes
	camOrginNode->yaw(Ogre::Degree(-(*mvpRotate) * arg.state.X.rel), Ogre::Node::TS_LOCAL);
	Pitch(arg,mtpCamGunCenterNode,mtPitchTrackerSemiThirdPerson);

	//So the camera always will look at the character  
	Ogre::Vector3 charPosition = charNode->_getDerivedPosition();
	charPosition.y += *mvpCamCharYAdjust;
    Ogre::Vector3 directionToCharacter =  charPosition - mtpCamGunCenterHelperNode->_getDerivedPosition();
    camera->setDirection(directionToCharacter);
	mtpPlayer->setNeedUpdateBones();
}

/*---------------------------------------------------------------------------------*/
void CameraHandler::RotateMixCursorMovable(const OIS::MouseEvent &arg)
{
	mtpPlayer->setMousePosition(arg.state.X.abs,arg.state.Y.abs);

	//Changing the gun tracker node that the character "aims" at.
	Ogre::Real x = -*mvpCamGunTrackerOffsetX + (2**mvpCamGunTrackerOffsetX*arg.state.X.abs)/mtScreenWidth;
	Ogre::Real y = *mvpCamGunTrackerOffsetY - 2 - (2**mvpCamGunTrackerOffsetY*arg.state.Y.abs)/mtScreenWidth;
	mtpCursorMovableGunTrackerNode->setPosition(x,y,*mvpCamGunTrackerOffsetZ);

	//Only yaw available in this rotate mode.
	camOrginNode->yaw(Ogre::Degree(-(*mvpRotate) * arg.state.X.rel), Ogre::Node::TS_LOCAL);

	//So the camera always will look at the character  
    Ogre::Vector3 directionToCharacter =  charNode->_getDerivedPosition() - mtpCamGunMoveNode->_getDerivedPosition();
	directionToCharacter.y= 0;
    camera->setDirection(directionToCharacter);
	mtpPlayer->setNeedUpdateBones();
	GameFramework::getSingletonPtr()->mpGui->updateCursorPos(arg.state.X.abs,arg.state.Y.abs );
}

/*---------------------------------------------------------------------------------*/
void CameraHandler::setCamMode(int camMode)
{
	switch (mtCamMode)		
	{
	case Game::Cam_MixCursorMovable:
		mtpCamGunMoveHelperNode->detachAllObjects();
	case Game::Cam_MixCursorCenter:
		mtpCamGunCenterHelperNode->detachAllObjects();
		mtpPlayer->semiThirdPersonMode(false);
		break;
	case Game::Cam_ThirdPerson:
		camCollisionNode->detachAllObjects();
		break;
	case Game::Cam_FirstPerson:
		mtpCamFirstPersonNode->detachAllObjects();
		mtpPlayer->firstPersonMode(false);
		break;
	}

	mtCamMode = camMode;

	switch (mtCamMode)		
	{
	case Game::Cam_MixCursorMovable:
		if ((mtpPlayer->getPowerUp() & Game::PowerUp_MoveBoxMode) == 0)
		mtpPlayer->semiThirdPersonMode(true);
		enterMixCursorMovableMode();
		break;
	case Game::Cam_MixCursorCenter:
		mtpPlayer->semiThirdPersonMode(true);
		enterMixCursorCenterMode();
		break;
	case Game::Cam_ThirdPerson:
		enterThirdPersonMode();
		break;
	case Game::Cam_FirstPerson:
		enterFirstPersonMode();
		break;
	case Game::Cam_FreeMode:
		sceneMgr->getSceneNode("CamFree")->attachObject(camera);
		break;
	}

}
/*---------------------------------------------------------------------------------*/
void CameraHandler::enterThirdPersonMode()
{
	camCollisionNode->attachObject(camera);
	mtpPlayer->setManuallyControlled(false);
	mtpPlayer->setInitialOrientation();
}
/*---------------------------------------------------------------------------------*/
void CameraHandler::enterMixCursorCenterMode()
{
	Ogre::Vector3 directionToCharacter = charNode->_getDerivedPosition() - mtpCamGunMoveHelperNode->_getDerivedPosition();
	directionToCharacter.y = 0;
	mtpCamGunCenterHelperNode->attachObject(camera);
	mtpPlayer->setManuallyControlled(true);
	camera->setDirection(directionToCharacter);	
	Ogre::Vector3  pos= mtpCursorMovableGunTrackerNode->getPosition();
	mtpCursorMovableGunTrackerNode->setPosition(0,pos.y,pos.z);
	GameFramework::getSingletonPtr()->mpGui->updateCursorPos(GameFramework::getSingletonPtr()->mpRenderWnd->getWidth()/2,GameFramework::getSingletonPtr()->mpRenderWnd->getHeight()/2);		
}
/*---------------------------------------------------------------------------------*/
void CameraHandler::enterMixCursorMovableMode()
{
	Ogre::Vector3 directionToCharacter = charNode->_getDerivedPosition() - mtpCamGunMoveHelperNode->_getDerivedPosition();
	mtpCamGunMoveHelperNode->setInitialState();
	mtpCamGunMoveHelperNode->attachObject(camera);
	directionToCharacter.y = 0;
	camera->setDirection(directionToCharacter);	

	Ogre::Vector3  pos= mtpCursorMovableGunTrackerNode->getPosition();
	mtpCursorMovableGunTrackerNode->setPosition(0,pos.y,pos.z);

	mtpPlayer->setManuallyControlled(true);
	
}
/*---------------------------------------------------------------------------------*/
void CameraHandler::enterFirstPersonMode()
{
	Ogre::Vector3  pos= mtpCursorMovableGunTrackerNode->getPosition();
	mtpCursorMovableGunTrackerNode->setPosition(0,pos.y,pos.z);
	GameFramework::getSingletonPtr()->mpGui->updateCursorPos(GameFramework::getSingletonPtr()->mpRenderWnd->getWidth()/2,GameFramework::getSingletonPtr()->mpRenderWnd->getHeight()/2);		
	mtpCamFirstPersonNode->attachObject(camera);
	mtpPlayer->setManuallyControlled(true);
	mtpPlayer->firstPersonMode();
}
/*---------------------------------------------------------------------------------*/
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
	if (lengthToCharacter > *mvpCamDistanceMin && arg.state.Z.rel > 0)
	{
		okToZoom = true;
	}
	//If zoom is negative, we only have to worry about going too far
	else if (lengthToCharacter < *mvpCamDistanceMax && arg.state.Z.rel < 0)
	{
		okToZoom = true;
	}

	if (okToZoom)
	{
	//Always the same speed, not dependent of the distance
	directionToCharacter.normalise();
	//At the startup, the camera was translated @ Z axis.
	camNoCollisionNode->translate(Ogre::Vector3::NEGATIVE_UNIT_Z * (*mvpZoom) * arg.state.Z.rel, Ogre::Node::TS_LOCAL);
	}

}

/*---------------------------------------------------------------------------------*/
void CameraHandler::AdjustCamera()
{
    //Get position of imaginary camera and the bounding sphere.
    Ogre::Vector3 noCollisionPos = camNoCollisionNode->_getDerivedPosition();
    NxOgre::Vec3 collisionSpherePos = cameraSphere->getGlobalPosition();

    //Get direction from bounding sphere to imaginary camera and set it as linear velocity
    NxOgre::Vec3 linearVelocityDirection;
    linearVelocityDirection.x = (noCollisionPos.x - collisionSpherePos.x) * *mvpCamVelocityXZ;
    linearVelocityDirection.y = (noCollisionPos.y - collisionSpherePos.y) * *mvpCamVelocityY;
    linearVelocityDirection.z = (noCollisionPos.z - collisionSpherePos.z) * *mvpCamVelocityXZ  ;
    cameraSphere->setLinearVelocity(linearVelocityDirection);
}

/*---------------------------------------------------------------------------------*/
void CameraHandler::MoveCamera()
{
    //Get position of bounding sphere and set the camera's position
    NxOgre::Vec3 collisionSpherePos = cameraSphere->getGlobalPosition();
    camCollisionNode->setPosition(Ogre::Vector3(collisionSpherePos.x,collisionSpherePos.y,collisionSpherePos.z));
#ifdef DEBUG	
	//Update to debugger << DEBUG >>
	GameFramework::getSingletonPtr()->mpGui->updateDebugCamXYZ(collisionSpherePos.as<Ogre::Vector3>());
#endif

    //So the camera always will look at the character  
	Ogre::Vector3 directionToCharacter =  charNode->_getDerivedPosition() + Ogre::Vector3(0,1.2,0) - camCollisionNode->_getDerivedPosition(); //@todo 1.2 random?
    camera->setDirection(directionToCharacter);
}
/*---------------------------------------------------------------------------------*/