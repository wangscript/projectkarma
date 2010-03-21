//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef TEST_STATE_HPP
#define TEST_STATE_HPP

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "AppState.hpp"

#include "DotSceneLoader.hpp"

#include <OgreSubEntity.h>
#include <OgreMaterialManager.h>
#include "Player.h"

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"

#include "BulletCollision/CollisionShapes/btConvexHullShape.h"

#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

class btKinematicCharacterController;

class btCollisionShape;

//|||||||||||||||||||||||||||||||||||||||||||||||

class TestState : public AppState
{
public:
	TestState();

	DECLARE_APPSTATE_CLASS(TestState)

	void enter();
	void createScene();
	void exit();
	bool pause();
	void resume();
	void initPhysics();

	void moveCamera();
	void getInput();

	bool keyPressed(const OIS::KeyEvent &keyEventRef);
	bool keyReleased(const OIS::KeyEvent &keyEventRef);

	bool mouseMoved(const OIS::MouseEvent &arg);
	bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id); 
	bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	void onLeftPressed(const OIS::MouseEvent &evt);
	bool onExitButtonGame(const CEGUI::EventArgs &args);

	void update(double timeSinceLastFrame);

	
	//************** BULLET
	btDiscreteDynamicsWorld*	m_pDynamicsWorld;

	btKinematicCharacterController* m_character;

	class btPairCachingGhostObject* m_ghostObject;

	btAlignedObjectArray<btCollisionShape*> m_collisionShapes;

	class btBroadphaseInterface*	m_overlappingPairCache;

	class btCollisionDispatcher*	m_dispatcher;

	btConstraintSolver*	m_constraintSolver;

	class btDefaultCollisionConfiguration* m_collisionConfiguration;

	class btTriangleIndexVertexArray*	m_indexVertexArrays;

	btVector3*	m_vertices;

	//*************************************

private:

	Player*						m_pKinematicPlayer;

	Ogre::SceneNode*			m_pCharacterNode;
	Ogre::Entity*				m_pCharacterEntity;

	Ogre::SceneNode*			m_pGroundNode;
	Ogre::Entity*				m_pGroundEntity;

	bool						m_bQuit;

	Ogre::Vector3				m_TranslateVector;
	Ogre::Real					m_MoveSpeed; 
	Ogre::Degree				m_RotateSpeed; 
	float						m_MoveScale; 
	Ogre::Degree				m_RotScale;

	Ogre::SceneNode*			m_pCurrentObject;
	Ogre::Entity*				m_pCurrentEntity;
	bool						m_bLMouseDown, m_bRMouseDown;
	

};

//|||||||||||||||||||||||||||||||||||||||||||||||

#endif

//|||||||||||||||||||||||||||||||||||||||||||||||