///////////////////////////
//Player.h   Christopher //
///////////////////////////


#include "Player.h"

#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"

Player::Player(SceneManager* sceneMgr, btCollisionWorld* btColWorld, btPairCachingGhostObject* ghostObject,btConvexShape* convexShape,btScalar stepHeight, int upAxis)
				: btKinematicCharacterController(ghostObject,convexShape,stepHeight,upAxis)
{
	
	m_pSceneMgr =			sceneMgr;
	m_pCollisionWorld =		btColWorld;
	
	m_pCharacterEntity =	0;
	m_pCharacterNode =		0;
	
}

void Player::addEntity(Ogre::Entity* ent)
{
	m_pSceneMgr->createEntity(m_pCharacterEntity);
}

Ogre::Entity * Player::getEntity()
{
	if (m_pCharacterEntity != 0)
		return m_pCharacterEntity;
}