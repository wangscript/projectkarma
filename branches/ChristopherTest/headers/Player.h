#ifndef PLAYER_H
#define PLAYER_H

//***************************************** OGRE

#include "Ogre.h"

//***************************************** BULLET


#include "btBulletCollisionCommon.h"

#include "btBulletDynamicsCommon.h"


#include "BulletCollision/CollisionShapes/btConvexHullShape.h"


class btCollisionShape;

//*****************************************

class Player : btKinematicCharacterController
{
	public:

		Player(SceneManager* sceneMgr, btCollisionWorld* btColWorld, btPairCachingGhostObject* ghostObject,btConvexShape* convexShape,btScalar stepHeight, int upAxis)

		//******************************** OGRE

		Ogre::SceneManager*		m_pSceneMgr;
		
		Ogre::Entity*			m_pCharacterEntity;
		
		Ogre::SceneNode*		m_pCharacterNode;

		//******************************** BULLET

		btCollisionWorld*					m_pCollisionWorld;
		
		class btPairCachingGhostObject*		m_pGhostObject;
		
		btConvexShape*						m_pCharacterConvexShape;

		btScalar							characterHeight;
		btScalar							characterWidth;
		btScalar							stepHeight;

		void addEntity(Ogre::Entity* ent);

		Ogre::Entity * getEntity(void);
				

};




#endif