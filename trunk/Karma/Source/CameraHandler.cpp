/*---------------------------------------------------------------------------------*/
/* File: CameraHandler.cpp														   */
/* Author: Per Karlsson, perkarlsson89@gmail.com								   */
/*																				   */
/* Description:	CameraHandler is a singleton class that takes care of all the	   */	
/* camera movements.															   */
/*	There are 4 different camera views:											   */
/*	1. Third Person (Uses physics).												   */
/*	2. Semi-Third Person with mouse movable.									   */
/*	3. Semi-Third Person with mouse locked.										   */
/*	4. First Person.															   */
/*---------------------------------------------------------------------------------*/

#include "CameraHandler.h"

/*---------------------------------------------------------------------------------*/
/*									STATIC										   */
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

/*---------------------------------------------------------------------------------*/
/*									PUBLIC										   */
/*---------------------------------------------------------------------------------*/
void CameraHandler::AdjustCamera()
{
	//Get position of imaginary camera and the bounding sphere.
	Ogre::Vector3 noCollisionPos = mvpCamNoCollisionNode->_getDerivedPosition();
	NxOgre::Vec3 collisionSpherePos = mvpCamSphere->getGlobalPosition();

	//Get direction from bounding sphere to imaginary camera and set it as linear velocity
	NxOgre::Vec3 linearVelocityDirection;
	linearVelocityDirection.x = (noCollisionPos.x - collisionSpherePos.x) * *mvpCamVelocityXZ;
	linearVelocityDirection.y = (noCollisionPos.y - collisionSpherePos.y) * *mvpCamVelocityY;
	linearVelocityDirection.z = (noCollisionPos.z - collisionSpherePos.z) * *mvpCamVelocityXZ  ;
	mvpCamSphere->setLinearVelocity(linearVelocityDirection);
}

/*---------------------------------------------------------------------------------*/
void CameraHandler::initCameraHandler(Ogre::Camera* cam, OGRE3DRenderSystem* renderSystem,Ogre::SceneManager* mmvpSceneMgr , Player* p)
{
	mvpCam = cam;
	mvpSceneMgr = mmvpSceneMgr;

	//Gett settings from the settings class.
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

	mvScreenWidth = GameFramework::getSingletonPtr()->mpRenderWnd->getWidth();
	mvScreenHeight = GameFramework::getSingletonPtr()->mpRenderWnd->getHeight();

	//All the nodes that is going to be used
	mvpCamNoCollisionNode = mvpSceneMgr->getSceneNode("CamNoCollisionNode");
	mvpCamCollisionNode = mvpSceneMgr->getSceneNode("CamCollisionNode");
	mvpCamHelperNode = mvpSceneMgr->getSceneNode("CamHelperNode");
	mvpCamOrginNode = mvpSceneMgr->getSceneNode("CamOrginNode");
	mvpCharNode = mvpSceneMgr->getSceneNode("CharNode");
	mvpCamFirstPersonNode = mvpSceneMgr->getSceneNode("CamFirstPersonNode");
	mvpFirstPersonGunTrackerNode = mvpSceneMgr->getSceneNode("FirstPersonGunTrackerNode");
	mvpCamGunMoveNode = mvpSceneMgr->getSceneNode("CamMoveNode");
	mvpCamGunMoveHelperNode = mvpSceneMgr->getSceneNode("CamMoveHelperNode");
	mvpCamGunCenterNode = mvpSceneMgr->getSceneNode("CamCenterNode");
	mvpCamGunCenterHelperNode = mvpSceneMgr->getSceneNode("CamCenterHelperNode");
	mvpCursorMovableGunTrackerNode = mvpSceneMgr->getSceneNode("GunTrackNode");

	setPlayerPtr(p);
	mvCamMode = NULL;
	setCamMode(Game::Cam_ThirdPerson);
	mvPitchTrackerThirdPerson = 0;
	mvPitchTrackerSemiThirdPerson = 0;
	mvPitchTrackerFirstPerson = 0;

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
	mvpCamSphere = renderSystem->createBody(new NxOgre::Sphere(*Settings::getSingletonPtr()->mCamRadius),
		NxOgre::Vec3(Settings::getSingletonPtr()->mCamStartPosX,Settings::getSingletonPtr()->mCamStartPosY,Settings::getSingletonPtr()->mCamStartPosZ)+NxOgre::Vec3(Ogre::Vector3
			(*Settings::getSingletonPtr()->mPlayerSpawnPointX,*Settings::getSingletonPtr()->mPlayerSpawnPointY,*Settings::getSingletonPtr()->mPlayerSpawnPointZ)), 
		"cube.1m.mesh", descriptionSphere);
	mvpCamSphere->getEntity()->setVisible(false);

	//Set Collision Flag to group 2
	NxShape* const* s2 = mvpCamSphere->getNxActor()->getShapes();
	(*s2)->setGroup(2);
}
/*---------------------------------------------------------------------------------*/
void CameraHandler::MoveCamera()
{
	//Get position of bounding sphere and set the camera's position
	NxOgre::Vec3 collisionSpherePos = mvpCamSphere->getGlobalPosition();
	mvpCamCollisionNode->setPosition(Ogre::Vector3(collisionSpherePos.x,collisionSpherePos.y,collisionSpherePos.z));
#ifdef DEBUG	
	//Update to debugger << DEBUG >>
	GameFramework::getSingletonPtr()->mpGui->updateDebugCamXYZ(collisionSpherePos.as<Ogre::Vector3>());
#endif

	//So the camera always will look at the character  
	Ogre::Vector3 directionToCharacter =  mvpCharNode->_getDerivedPosition() + Ogre::Vector3(0,1.2,0) - mvpCamCollisionNode->_getDerivedPosition(); //@todo 1.2 random?
	mvpCam->setDirection(directionToCharacter);
}
/*---------------------------------------------------------------------------------*/
void CameraHandler::Rotate(const OIS::MouseEvent &arg)
{
	//Calls the correct rotation methods.
	switch (mvCamMode)		
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

	//Needed for raycasting in the Player class
	mvpPlayer->updateCursorPos(arg.state.X.abs,arg.state.Y.abs);
}

/*---------------------------------------------------------------------------------*/
void CameraHandler::setCamMode(int camMode)
{
	//Detaches the Ogre::Camera from the previous camera mode.
	switch (mvCamMode)		
	{
	case Game::Cam_MixCursorMovable:
		mvpCamGunMoveHelperNode->detachAllObjects();
	case Game::Cam_MixCursorCenter:
		mvpCamGunCenterHelperNode->detachAllObjects();
		//Hides the gun.
		mvpPlayer->semiThirdPersonMode(false);
		break;
	case Game::Cam_ThirdPerson:
		//mvpCamHelperNode->setInitialState();
		//mvpCamOrginNode->setInitialState();
		//mvpSceneMgr->getSceneNode("RootNode")->setInitialState();
		mvpCamCollisionNode->detachAllObjects();
		break;
	case Game::Cam_FirstPerson:
		mvpCamFirstPersonNode->detachAllObjects();
		//Makes things visible again (Character mesh and so on)
		mvpPlayer->firstPersonMode(false);
		break;
	}

	//New camera mode.
	mvCamMode = camMode;

	//Enter the new camera mode.
	switch (mvCamMode)		
	{
	case Game::Cam_MixCursorMovable:
		if ((mvpPlayer->getPowerUp() & Game::PowerUp_MoveBoxMode) == 0)
			mvpPlayer->semiThirdPersonMode(true);
		enterMixCursorMovableMode();
		break;
	case Game::Cam_MixCursorCenter:
		mvpPlayer->semiThirdPersonMode(true);
		enterMixCursorCenterMode();
		break;
	case Game::Cam_ThirdPerson:
		enterThirdPersonMode();
		break;
	case Game::Cam_FirstPerson:
		enterFirstPersonMode();
		break;
	case Game::Cam_FreeMode:
		mvpSceneMgr->getSceneNode("CamFree")->attachObject(mvpCam);
		break;
	}

}
/*---------------------------------------------------------------------------------*/
void CameraHandler::Zoom(const OIS::MouseEvent &arg)
{
	bool okToZoom = false;
	//Direction from imaginary camera position to character
	Ogre::Vector3 directionToCharacter =  mvpCharNode->_getDerivedPosition() - mvpCamNoCollisionNode->_getDerivedPosition();
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
		mvpCamNoCollisionNode->translate(Ogre::Vector3::NEGATIVE_UNIT_Z * (*mvpZoom) * arg.state.Z.rel, Ogre::Node::TS_LOCAL);
	}

}


/*---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*/
/*									PRIVATE										   */
/*---------------------------------------------------------------------------------*/
void CameraHandler::enterFirstPersonMode()
{
	//Attach the camera to the correct node.
	mvpCamFirstPersonNode->attachObject(mvpCam);

	//The GUI cursor icon will always be in the center.
	GameFramework::getSingletonPtr()->mpGui->updateCursorPos(GameFramework::getSingletonPtr()->mpRenderWnd->getWidth()/2,GameFramework::getSingletonPtr()->mpRenderWnd->getHeight()/2);		

	//Tells the player class to hide some stuff when entering first person camera mode.
	mvpPlayer->firstPersonMode();

	//See ::enterMixCursorMovableMode();
	enterGunMode();

}
/*---------------------------------------------------------------------------------*/
void CameraHandler::enterGunMode()
{
	//Gun mode requires some of the bones in the Player to be manually controlled.
	mvpPlayer->setManuallyControlled(true);

	//Resets the Aimer tracker Node but keeps the distance (z) and height (y).
	Ogre::Vector3  pos= mvpCursorMovableGunTrackerNode->getPosition();
	mvpCursorMovableGunTrackerNode->setPosition(0,pos.y,pos.z);
}
/*---------------------------------------------------------------------------------*/
void CameraHandler::enterMixCursorCenterMode()
{
	//So the camera will always look in the correct direction.
	Ogre::Vector3 directionToCharacter = mvpCharNode->_getDerivedPosition() - mvpCamGunMoveHelperNode->_getDerivedPosition();
	//To prevent the character from always being in the center of the scene, y = 0.
	directionToCharacter.y = 0;
	mvpCam->setDirection(directionToCharacter);	

	//The GUI cursor icon will always be in the center.
	GameFramework::getSingletonPtr()->mpGui->updateCursorPos(GameFramework::getSingletonPtr()->mpRenderWnd->getWidth()/2,GameFramework::getSingletonPtr()->mpRenderWnd->getHeight()/2);		

	//Attach the camera to the correct node.
	mvpCamGunCenterHelperNode->attachObject(mvpCam);

	//This camera mode is only available with the Gun Mode
	enterGunMode();
}
/*---------------------------------------------------------------------------------*/
void CameraHandler::enterMixCursorMovableMode()
{
	//So the camera will always look in the correct direction.

	Ogre::Vector3 directionToCharacter = mvpCharNode->_getDerivedPosition() - mvpCamGunMoveHelperNode->_getDerivedPosition();
	//If the this node (it is being used in other camera modes) has been rotated around the Z axis, it will have a new height.
	//In this camera mode, rotation around Z won't be possible, so we will have to reset it to it's inital position.
	mvpCamGunMoveHelperNode->setInitialState();	
	directionToCharacter.y = 0;


	//Attach the camera to the correct node.
	mvpCamGunMoveHelperNode->attachObject(mvpCam);

	mvpCam->setDirection(directionToCharacter);	

	//This camera mode is available for the PowerUp_MoveBox(See GameEnums.h) and PowerUp_GunMode.
	//enterGunMode makes manual control of bones in the player mesh available, something needed for both of these powerups.
	enterGunMode();
}
/*---------------------------------------------------------------------------------*/
void CameraHandler::enterThirdPersonMode()
{
	//Attach the camera to the correct node.
	mvpCamCollisionNode->attachObject(mvpCam);

	//If some bones were rotated during the previus camera mode, they will be reseted.
	mvpPlayer->setManuallyControlled(false);
	mvpPlayer->setInitialOrientation();
}


/*---------------------------------------------------------------------------------*/
void CameraHandler::Pitch(const OIS::MouseEvent &arg,Ogre::SceneNode* node,float& mtPitchTracker)
{
	/*It is not allowed to pitch the camera (rotate around Z axis) if it
	A. Rotates more than 90 degrees, making the world "flip"
	B. Position itself under the world.
	
	Not necessary to check to more than +-70 degrees (instead of 90)
	*/
	bool okToPitch = false;

	//If the relative mouse movement is negative, we only have to worry about Case B.
	if ( arg.state.Y.rel < 0 && mtPitchTracker < 70 )
		okToPitch = true;
	//Case A.
	else if ( arg.state.Y.rel > 0 && mtPitchTracker > -70)
		okToPitch = true;

	if (okToPitch)
	{
		node->pitch(Ogre::Degree(-(*mvpRotate) * arg.state.Y.rel), Ogre::Node::TS_LOCAL);
		mtPitchTracker+= -(*mvpRotate) * arg.state.Y.rel;
	}
}

/*---------------------------------------------------------------------------------*/
void CameraHandler::RotateFirstPerson(const OIS::MouseEvent &arg)
{
	//Yaw and pitch FirstPerson Nodes
	mvpCamOrginNode->yaw(Ogre::Degree(-(*mvpRotate) * arg.state.X.rel), Ogre::Node::TS_LOCAL);
	Pitch(arg,mvpCamFirstPersonNode,mvPitchTrackerFirstPerson);

	//So the camera always looks at the Gun Tracker Node.
	Ogre::Vector3 charPosition = mvpCharNode->_getDerivedPosition();
	charPosition.y += *mvpCamCharYAdjust;
	Ogre::Vector3 directionToGunTracker =  mvpFirstPersonGunTrackerNode->_getDerivedPosition() - charPosition  ;
	mvpCam->setDirection(directionToGunTracker);

	//Tells the player class that the bones needs an update in the game loop (Maybe not needed if only single player and no multiplayer).
	mvpPlayer->setNeedUpdateBones();
}
/*---------------------------------------------------------------------------------*/
void CameraHandler::RotateFree(const OIS::MouseEvent &arg)
{
	//This view is for screenshotting map overview only :)
	Ogre::Vector3 dirre =   - mvpSceneMgr->getSceneNode("CamFree")->_getDerivedPosition();
	mvpCam->setDirection(dirre);
	mvpSceneMgr->getSceneNode("CamFree")->translate(arg.state.X.rel,arg.state.Z.rel,arg.state.Y.rel);
}
/*---------------------------------------------------------------------------------*/
void CameraHandler::RotateMixCursorCenter(const OIS::MouseEvent &arg)
{
	//Only changes the the Y value of the GunTrackerNode
	Ogre::Real x = 0;
	Ogre::Real y = *mvpCamGunTrackerOffsetY - 2 - (2**mvpCamGunTrackerOffsetX*arg.state.Y.abs)/mvScreenWidth;
	mvpCursorMovableGunTrackerNode->setPosition(x,y,*mvpCamGunTrackerOffsetZ);

	//Yaw and pitch nodes
	mvpCamOrginNode->yaw(Ogre::Degree(-(*mvpRotate) * arg.state.X.rel), Ogre::Node::TS_LOCAL);
	Pitch(arg,mvpCamGunCenterNode,mvPitchTrackerSemiThirdPerson);

	//So the camera always will look at the same direction as the vector "Camere to Player"
	Ogre::Vector3 charPosition = mvpCharNode->_getDerivedPosition();
	charPosition.y += *mvpCamCharYAdjust;
	Ogre::Vector3 directionToCharacter =  charPosition - mvpCamGunCenterHelperNode->_getDerivedPosition();
	mvpCam->setDirection(directionToCharacter);
	
	//Tells the player class that the bones needs an update in the game loop.
	mvpPlayer->setNeedUpdateBones();
}
/*---------------------------------------------------------------------------------*/
void CameraHandler::RotateMixCursorMovable(const OIS::MouseEvent &arg)
{
	//The player class needs the mouse position for raycasting.
	mvpPlayer->updateCursorPos(arg.state.X.abs,arg.state.Y.abs);

	//Changing the gun tracker node that the character "aims" at.
	Ogre::Real x = -*mvpCamGunTrackerOffsetX + (2**mvpCamGunTrackerOffsetX*arg.state.X.abs)/mvScreenWidth;
	Ogre::Real y = *mvpCamGunTrackerOffsetY - 2 - (2**mvpCamGunTrackerOffsetY*arg.state.Y.abs)/mvScreenWidth;
	mvpCursorMovableGunTrackerNode->setPosition(x,y,*mvpCamGunTrackerOffsetZ);

	//Only yaw (rotation around y) available in this rotate mode.
	mvpCamOrginNode->yaw(Ogre::Degree(-(*mvpRotate) * arg.state.X.rel), Ogre::Node::TS_LOCAL);

	//So the camera always will look at the same direction as the vector "Camere to Player"
	Ogre::Vector3 directionToCharacter =  mvpCharNode->_getDerivedPosition() - mvpCamGunMoveHelperNode->_getDerivedPosition();
	directionToCharacter.y= 0;
	mvpCam->setDirection(directionToCharacter);
	
	//Tells the player class that the bones needs an update in the game loop.
	mvpPlayer->setNeedUpdateBones();

	//Updates the mouse position in the GUI
	GameFramework::getSingletonPtr()->mpGui->updateCursorPos(arg.state.X.abs,arg.state.Y.abs );
}

/*---------------------------------------------------------------------------------*/
void CameraHandler::RotateThirdPerson(const OIS::MouseEvent &arg)
{
	//In the future: Implement raycast, so the cameras bound spehere always can see the Player and not get stuck between stuff.

	//Yaw the camera. Rotate around the Y axis
	mvpCamOrginNode->yaw(Ogre::Degree(-(*mvpRotate) * arg.state.X.rel), Ogre::Node::TS_LOCAL);
	//Pitch the camera. Rotate around X axis
	Pitch(arg,mvpCamHelperNode,mvPitchTrackerThirdPerson);

	//Update GUI cursor position
	GameFramework::getSingletonPtr()->mpGui->updateCursorPos(arg.state.X.abs,arg.state.Y.abs);
}
/*---------------------------------------------------------------------------------*/