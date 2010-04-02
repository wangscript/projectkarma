#ifndef CAMERAHANDLER_H
#define CAMERAHANDLER_H

#include <Ogre.h>
#include <NxOgreOGRE3D.h>
#include <NxActor.h>
#include <NxScene.h>
#include <NxUserContactReport.h>
#include <OISMouse.h>
#define CAMERA_VELOCITY_Y 2
#define CAMERA_VELOCITY_XZ 3
#define CAMERA_MAX_Y 6
#define CAMERA_MIN_Y 1.1

class CameraHandler
{
	public:
		CameraHandler(Ogre::Camera* cam, Ogre::SceneNode* node, OGRE3DRenderSystem* renderSystem,Ogre::SceneManager* mSceneMgr);
		virtual ~CameraHandler();
		virtual void Rotate(const OIS::MouseEvent &arg);
		virtual void AdjustCamera();
		virtual void MoveCamera();
	protected:
		OGRE3DBody*		cameraSphere;
		Ogre::Camera*	camera;
		Ogre::SceneManager* sceneMgr;
		Ogre::Real	 mRotate; 
		Ogre::SceneNode* camNoCollisionNode;
		Ogre::SceneNode* camCollisionNode;
		Ogre::SceneNode* camHelperNode;
		Ogre::SceneNode* camOrginNode;
		Ogre::SceneNode* charNode;

};		

#endif 