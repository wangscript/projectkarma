///////////////////////////
//Player.h   Christopher //
///////////////////////////


#include "Player.h"

#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"

using namespace Ogre;


Player::Player(SceneManager* sceneMgr, btDynamicsWorld* btDynWorld, btPairCachingGhostObject* ghostObject,btConvexShape* convexShape,btScalar stepHeight, int upAxis)
				: btKinematicCharacterController(ghostObject,convexShape,stepHeight,upAxis)
{
	
	m_pSceneMgr =			sceneMgr;
	m_pDynamicsWorld =		btDynWorld;
	
	m_pCharacterEntity =	0;
	m_pCharacterNode =		0;
	
}

Player::~Player()
{
	delete m_pCharacterNode;
	delete m_pCharacterConvexShape;
	delete m_pCharacterEntity;
	delete m_pCharacterConvexShape;
}

void Player::addEntity(Ogre::Entity* ent)
{
	m_pCharacterEntity = m_pSceneMgr->createEntity("PlayerEntity","ogrehead.mesh");
}

Entity * Player::getEntity()
{
	if (m_pCharacterEntity != 0)
		return m_pCharacterEntity;
}

btConvexShape * Player::setConvexShape(int characterHeight, int characterWidth)
{
	m_characterHeight = characterHeight;
	m_characterHeight = characterWidth;
	m_pCharacterConvexShape = new btCapsuleShape(m_characterWidth,m_characterHeight);
}