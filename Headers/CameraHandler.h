#ifndef CAMERAHANDLER_H
#define CAMERAHANDLER_H

#include <Ogre.h>
#include <NxOgreOGRE3D.h>
#include "AppState.h"//Singelton usage
#include <NxActor.h>
#include <NxScene.h>
#include <NxUserContactReport.h>
#include <OISMouse.h>
#include "Character.h"

class CameraHandler
{
	public:

	enum CameraMode
	{
		Cam_ThirdPerson = 0, 
		Cam_MixCursorCenter = 1,
		Cam_MixCursorMovable = 2,
		Cam_FirstPerson = 3,
		Cam_FreeMode = 4
	};


		CameraHandler(Ogre::Camera* cam, Ogre::SceneNode* node, OGRE3DRenderSystem* renderSystem,Ogre::SceneManager* mSceneMgr);
		virtual ~CameraHandler();
		void Zoom(const OIS::MouseEvent &arg);
		void Rotate(const OIS::MouseEvent &arg);
		void AdjustCamera();
		void MoveCamera();
		void setCamMode(int x);
		int getCamMode(){return mtCamMode;};
		void setCharPtr(Character* x){mtpChar = x;};

	protected:
		OGRE3DBody*		cameraSphere;
		Ogre::Camera*	camera;
		Ogre::SceneManager* sceneMgr;
		Ogre::Real	 mtRotate; 
		Ogre::Real	mtZoom;
		int mtCamMode;
		Character* mtpChar;
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
		Ogre::SceneNode* mtpCamMixNode;
		Ogre::SceneNode* mtpCamMixHelperNode;
		Ogre::SceneNode* mtpCursorMovableGunTrackerNode;
		Ogre::SceneNode* mtpCursorCenterGunTrackerNode;

		void RotateFree(const OIS::MouseEvent &arg);
		void RotateThirdPerson(const OIS::MouseEvent &arg);
		void RotateFirstPerson(const OIS::MouseEvent &arg);
		void RotateMixCursorCenter(const OIS::MouseEvent &arg);
		void RotateMixCursorMovable(const OIS::MouseEvent &arg);
};		

#endif 