//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef TEST_STATE_HPP
#define TEST_STATE_HPP

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "AppState.hpp"

#include "DotSceneLoader.hpp"

#include <OgreSubEntity.h>
#include <OgreMaterialManager.h>

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

	void setBufferedMode();
	void setUnbufferedMode();

private:
	Ogre::SceneNode*			m_pSphereNode;
	Ogre::Entity*				m_pSphereEntity;

	Ogre::SceneNode*			m_pGroundNode;
	Ogre::Entity*			m_pGroundEntity;

	bool						m_bQuit;

	Ogre::Vector3				m_TranslateVector;
	Ogre::Real					m_MoveSpeed; 
	Ogre::Degree				m_RotateSpeed; 
	float						m_MoveScale; 
	Ogre::Degree				m_RotScale;

	//Ogre::RaySceneQuery*		m_pRSQ;
	Ogre::SceneNode*			m_pCurrentObject;
	Ogre::Entity*				m_pCurrentEntity;
	bool						m_bLMouseDown, m_bRMouseDown;
	
	CEGUI::Window*				m_pMainWnd;	
};

//|||||||||||||||||||||||||||||||||||||||||||||||

#endif

//|||||||||||||||||||||||||||||||||||||||||||||||