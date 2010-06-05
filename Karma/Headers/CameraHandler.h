#ifndef CAMERAHANDLER_H
#define CAMERAHANDLER_H

#include "Player.h"
#include "GameCommon.h"

class CameraHandler
{
private: 
	static CameraHandler singleton;

	CameraHandler() {};
public:
	static CameraHandler getSingleton();
	static CameraHandler* getSingletonPtr();

	void initCameraHandler(Ogre::Camera* cam, OGRE3DRenderSystem* renderSystem,Ogre::SceneManager* mSceneMgr, Player* p);
	void Zoom(const OIS::MouseEvent &arg);
	void Rotate(const OIS::MouseEvent &arg);
	void AdjustCamera();
	void MoveCamera();
	void setCamMode(int x);
	int getCamMode(){return mtCamMode;};
	OGRE3DBody* getCameraSphere(){return cameraSphere;};
	void setPlayerPtr(Player* x){mtpPlayer = x;};

private:
	OGRE3DBody*		cameraSphere;
	Ogre::Camera*	camera;
	Ogre::SceneManager* sceneMgr;
	Ogre::Real	 *mvpRotate,*mvpZoom; 
	int mtCamMode;
	Player* mtpPlayer;
	float mtPitchTrackerThirdPerson,mtPitchTrackerSemiThirdPerson,mtPitchTrackerFirstPerson;

	Ogre::Real *mvpCamCharYAdjust,*mvpCamGunTrackerOffsetX,*mvpCamGunTrackerOffsetY,*mvpCamGunTrackerOffsetZ;
	Ogre::Real *mvpCamVelocityY,*mvpCamVelocityXZ,*mvpCamHeightMaxY,*mvpCamHeightMinY,*mvpCamDistanceMax,*mvpCamDistanceMin;
	Ogre::Real mtScreenWidth,mtScreenHeight;

	Ogre::SceneNode* camNoCollisionNode;
	Ogre::SceneNode* camCollisionNode;
	Ogre::SceneNode* camHelperNode;
	Ogre::SceneNode* camOrginNode;
	Ogre::SceneNode* charNode;
	Ogre::SceneNode* mtpCamFirstPersonNode;
	Ogre::SceneNode* mtpFirstPersonGunTrackerNode;
	Ogre::SceneNode* mtpCamGunMoveNode;
	Ogre::SceneNode* mtpCamGunMoveHelperNode;
	Ogre::SceneNode* mtpCamGunCenterNode;
	Ogre::SceneNode* mtpCamGunCenterHelperNode;
	Ogre::SceneNode* mtpCursorMovableGunTrackerNode;

	void enterThirdPersonMode();
	void enterMixCursorCenterMode();
	void enterMixCursorMovableMode();
	void enterFirstPersonMode();

	void Pitch(const OIS::MouseEvent &arg,Ogre::SceneNode* node,float& mtPitchTracker);
	void RotateFree(const OIS::MouseEvent &arg);
	void RotateThirdPerson(const OIS::MouseEvent &arg);
	void RotateFirstPerson(const OIS::MouseEvent &arg);
	void RotateMixCursorCenter(const OIS::MouseEvent &arg);
	void RotateMixCursorMovable(const OIS::MouseEvent &arg);
};		

#endif 