/*---------------------------------------------------------------------------------*/
/* File: GameState.h															   */
/* Author: Per Karlsson, perkarlsson89@gmail.com								   */
/*																				   */
/* Description:	http://www.ogre3d.org/wiki/index.php/Advanced_Ogre_Framework	   */
/* Based on the example in the Advanced Ogre Framework by spacegaier.			   */
/*---------------------------------------------------------------------------------*/

#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "CameraHandler.h"
#include "Player.h"
#include "PowerUp.h"
#include "WorldManager.h"
#include "PhysicsManager.h"
#include "SceneLoader.h"
#include "NPCAimer.h"
#include "NPCHandler.h"
#include <stdlib.h>
#include <time.h>

#include "GameCommon.h"

class GameState : public AppState
{
public:
	DECLARE_APPSTATE_CLASS(GameState)
	
	/* Every time we enter a GameState we do the following:
	1. Create SceneManager
	2. Create a Camera
	3. Set input callbacks
	4. Load Sounds
	5. Create a scene.*/
	void enter();

	/* In createScene we do the following:
	1. Initiate Physics.
	2. Set SkyBox, Shadows and Directional Light.
	3. Create Motion Blur Effects.
	4. Create the grid for the Chunks class.
	5. Create the terrain.
	6. Create the Player.
	7. Create the Camera.
	8. Create PowerUps
	9. Add NPCs
	10. Add concent to the scene(Dynamic and static objects)*/
	void createScene();

	/* Delete Physics, Chunk system, Unload GameState sound and finally destroys Camera and Scene Manager. */
	void exit();

	/* Pause the Game State (Physics, AI etc) and hides the Game GUI. */
	bool pause();

	/* Resumes the earlier paused Game State. Shows the Game GUI. */
	void resume();

	/* In every frame we check for input from the keyboard here. */
	void getInput();

	/* Every time a key is pressed, this function is called. Good for activating new PowerUps. */
	bool keyPressed(const OIS::KeyEvent &keyEventRef);

	/* Every time a key is released, this function is called. Good for resetting move direction. */
	bool keyReleased(const OIS::KeyEvent &keyEventRef);

	/* This function is called every time the mouse moves. All camera modes has their own way
	of defining a rotation. The CameraHandler class takes care of this. */
	bool mouseMoved(const OIS::MouseEvent &arg);

	/* When the mouse is pressed, this function is called. Good for start shooting or creating joints in the MoveBox mode. */
	bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id); 

	/* See mousePressed. Just de-activates what mousePressed activates. */
	bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	/* This updates the whole game every frame. Called from the Game Loop. */
	void update(double timeSinceLastFrame);
private:

	/* Create Motion Blur effects. Taken from the OGRE samples. */
	void createMotionBlurEffects();

	bool                                    mvQuit,mvRightButton;
	PhysicsManager*							mvpPhysicsMgr;
	Player*									mvpPlayer;
	PowerUp*								mvpPowerUps;
	WorldManager*							mvpWorld;
};

#endif
