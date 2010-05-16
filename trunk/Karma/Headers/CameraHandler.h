#ifndef CAMERAHANDLER_H
#define CAMERAHANDLER_H

#include "Player.h"
#include "GameCommon.h"

class CameraHandler
{
	public:

		CameraHandler(Ogre::Camera* cam, OGRE3DRenderSystem* renderSystem,Ogre::SceneManager* mSceneMgr, Player* p);
		virtual ~CameraHandler();
		void Zoom(const OIS::MouseEvent &arg);
		void Rotate(const OIS::MouseEvent &arg);
		void AdjustCamera();
		void MoveCamera();
		void setCamMode(int x);
		int getCamMode(){return mtCamMode;};
		void setPlayerPtr(Player* x){mtpPlayer = x;};

	protected:
		OGRE3DBody*		cameraSphere;
		Ogre::Camera*	camera;
		Ogre::SceneManager* sceneMgr;
		Ogre::Real	 mtRotate; 
		Ogre::Real	mtZoom;
		int mtCamMode;
		Player* mtpPlayer;
		Ogre::Real mtCamCharYAdjust;
		Ogre::Real mtCamGunTrackerOffsetX,mtCamGunTrackerOffsetY,mtCamGunTrackerOffsetZ;

		Ogre::Real mtCamVelocityY; 
		Ogre::Real mtCamVelocityXZ;
		Ogre::Real mtCamHeightMaxY;
		Ogre::Real mtCamHeightMinY;
		Ogre::Real mtCamDistanceMax;
		Ogre::Real mtCamDistanceMin;
		Ogre::Real mtScreenWidth;
		Ogre::Real mtScreenHeight;

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

		void RotateFree(const OIS::MouseEvent &arg);
		void RotateThirdPerson(const OIS::MouseEvent &arg);
		void RotateFirstPerson(const OIS::MouseEvent &arg);
		void RotateMixCursorCenter(const OIS::MouseEvent &arg);
		void RotateMixCursorMovable(const OIS::MouseEvent &arg);
};		

#endif 