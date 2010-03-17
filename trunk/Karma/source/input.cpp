#include "input.h"
#include "OgreStringConverter.h"
#include "simulation.h"

InputHandler::InputHandler(Simulation *sim, unsigned long hWnd)  {

	OIS::ParamList pl;
	pl.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString(hWnd)));

	m_hWnd = hWnd;
	m_ois = OIS::InputManager::createInputSystem( pl );
	mMouse = static_cast<OIS::Mouse*>(m_ois->createInputObject( OIS::OISMouse, true ));
	mKeyboard = static_cast<OIS::Keyboard*>(m_ois->createInputObject( OIS::OISKeyboard, true));
	mMouse->setEventCallback(this);
	mKeyboard->setEventCallback(this);

	m_simulation = sim;
}

InputHandler::~InputHandler() {
	if (mMouse)
		m_ois->destroyInputObject(mMouse);
	if (mKeyboard)
		m_ois->destroyInputObject(mKeyboard);

	OIS::InputManager::destroyInputSystem(m_ois);
}

void InputHandler::capture() {
	mMouse->capture();
	mKeyboard->capture();
}

void  InputHandler::setWindowExtents(int width, int height){
	//Set Mouse Region.. if window resizes, we should alter this to reflect as well
	const OIS::MouseState &ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}


// MouseListener
bool InputHandler::mouseMoved(const OIS::MouseEvent &evt) {
	return true;
}

bool InputHandler::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID btn) {
	return true;
}

bool InputHandler::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID btn) {
	return true;
}


// KeyListener
bool InputHandler::keyPressed(const OIS::KeyEvent &evt) {
	return true;
}

bool InputHandler::keyReleased(const OIS::KeyEvent &evt) {
	if (evt.key == OIS::KC_ESCAPE)
		m_simulation->requestStateChange(SHUTDOWN);

	return true;
}



// JoyStickListener
bool InputHandler::buttonPressed(const OIS::JoyStickEvent &evt, int index) {
	return true;
}

bool InputHandler::buttonReleased(const OIS::JoyStickEvent &evt, int index) {
	return true;
}

bool InputHandler::axisMoved(const OIS::JoyStickEvent &evt, int index) {
	return true;
}

bool InputHandler::povMoved(const OIS::JoyStickEvent &evt, int index) {
	return true;
}
