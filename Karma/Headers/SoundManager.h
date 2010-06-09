/*---------------------------------------------------------------------------------*/
/* File: SoundManager.h															   */
/* Author: Per Karlsson, perkarlsson89@gmail.com								   */
/*																				   */
/* Description:	SoundManager is a class that handles all the sound. It is		   */
/* initiated in the GameFramework and is accessable everywhere.			  		   */
/*---------------------------------------------------------------------------------*/

#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <irrklang.h>
#include <Ogre.h>

class SoundManager
{
private:
	irrklang::ISoundEngine		*mvpEngine;
	irrklang::ISound			*mvpRocketSound,*mvpRocketFadeSound,*mvpBgSound;
	bool						mvUpdateRocketSound;

public:
	irrklang::ISoundSource		*mpGun,*mpSplash,*mpBodyHit,*mpHeadShot,*mpRocketBoots,*mpRocketBootsFade,*mpMoveBox;

    SoundManager();

	/* Delete the sound engine. Called when the application is closed. */
	void deleteSound();

	/* Load all sound files used in the game state. */
	void initGameStateSound();
    
	/* Play the ambient background sound. */
	void playBackgroundSound();
    
	/* Plays a soundfile at a position in the world. */
	void playSound(const char* , Ogre::Vector3 );

	/* Plays a irrklang::ISound at a position in the world. */
	void playSound(irrklang::ISoundSource* sound, Ogre::Vector3 pos,bool loop=false);

	/* Delete all game state sounds from the memory. Called when a game level quits. */
	void removeGameStateSounds();

	/* Stops playing the ambient background sound. */
	void stopBackgroundSound();

	/* Stops the Rocket Boots sound */
	void stopRocketSound(Ogre::Vector3 pos);

	/* Update the listener position. Needs to called every frame in the Game Loop. */
	void updateListenerPos(Ogre::Vector3 );
};

#endif