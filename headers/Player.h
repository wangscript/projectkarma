#ifndef PLAYER_H
#define PLAYER_H

//***************************************** OGRE

#include "Ogre.h"

//***************************************** BULLET


#include "btBulletCollisionCommon.h"

#include "btBulletDynamicsCommon.h"


#include "BulletCollision/CollisionShapes/btConvexHullShape.h"

#include "BulletDynamics/Character/btKinematicCharacterController.h"


class btCollisionShape;

class btKinematicCharacterController;

//*****************************************

using namespace Ogre;

class Player : public btKinematicCharacterController
{
	public:

		Player(SceneManager* sceneMgr, btDynamicsWorld* btDynWorld, btPairCachingGhostObject* ghostObject,btConvexShape* convexShape,btScalar stepHeight, int upAxis);
		
		~Player();

		//****************************************************** OGRE

		SceneManager*						m_pSceneMgr;
		
		Entity*								m_pCharacterEntity;
		
		SceneNode*							m_pCharacterNode;

		//****************************************************** BULLET

		btDynamicsWorld*					m_pDynamicsWorld;
		
		class btPairCachingGhostObject*		m_pGhostObject;		
		
		btConvexShape*						m_pCharacterConvexShape;

		btScalar							m_stepHeight;

		void addEntity(Ogre::Entity* ent);
		Entity * getEntity(void);		
		btConvexShape * setConvexShape(int characterHeight, int CharacterWidth);

	private:

		btScalar							m_characterHeight;
		btScalar							m_characterWidth;
};

#endif