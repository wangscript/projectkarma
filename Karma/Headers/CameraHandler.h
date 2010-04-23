#ifndef CAMERAHANDLER_H
#define CAMERAHANDLER_H

#include <Ogre.h>
#include <NxOgreOGRE3D.h>
#include "AppState.h"//Singelton usage
#include <NxActor.h>
#include <NxScene.h>
#include <NxUserContactReport.h>
#include <OISMouse.h>

class CameraHandler
{
	public:
		CameraHandler(Ogre::Camera* cam, Ogre::SceneNode* node, OGRE3DRenderSystem* renderSystem,Ogre::SceneManager* mSceneMgr);
		virtual ~CameraHandler();
		void Zoom(const OIS::MouseEvent &arg);
		void Rotate(const OIS::MouseEvent &arg);
		void AdjustCamera();
		void MoveCamera();
	protected:
		OGRE3DBody*		cameraSphere;
		Ogre::Camera*	camera;
		Ogre::SceneManager* sceneMgr;
		Ogre::Real	 mtRotate; 
		Ogre::Real	mtZoom;
		Ogre::Real mtCamVelocityY; 
		Ogre::Real mtCamVelocityXZ;
		Ogre::Real mtCamHeightMaxY;
		Ogre::Real mtCamHeightMinY;
		Ogre::Real mtCamDistanceMax ;
		Ogre::Real mtCamDistanceMin ;

		Ogre::SceneNode* camNoCollisionNode;
		Ogre::SceneNode* camCollisionNode;
		Ogre::SceneNode* camHelperNode;
		Ogre::SceneNode* camOrginNode;
		Ogre::SceneNode* charNode;
};		

#endif 