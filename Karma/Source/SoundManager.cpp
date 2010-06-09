/*---------------------------------------------------------------------------------*/
/* File: SoundManager.cpp														   */
/* Author: Per Karlsson, perkarlsson89@gmail.com								   */
/*																				   */
/* Description:	SoundManager is a class that handles all the sound. It is		   */
/* initiated in the GameFramework and is accessable everywhere.			  		   */
/*---------------------------------------------------------------------------------*/

#include <SoundManager.h>

/*---------------------------------------------------------------------------------*/
/*									PUBLIC										   */
/*---------------------------------------------------------------------------------*/
SoundManager::SoundManager() 
{
	//Initiate the sound engine.
	mvpEngine = irrklang::createIrrKlangDevice();

	mvUpdateRocketSound = false;
	mvpRocketSound = 0;
	mvpRocketFadeSound = 0;
	mvpBgSound = 0;
}

/*---------------------------------------------------------------------------------*/
void SoundManager::initGameStateSound()
{
	//Load all sound effects for the Game State.
	mpGun = mvpEngine->addSoundSourceFromFile("../Media/Sounds/gun.ogg",irrklang::ESM_NO_STREAMING);
	mpBodyHit = mvpEngine->addSoundSourceFromFile("../Media/Sounds/bodyhit.ogg",irrklang::ESM_NO_STREAMING);
	mpSplash = mvpEngine->addSoundSourceFromFile("../Media/Sounds/splash.ogg",irrklang::ESM_NO_STREAMING);
	mpHeadShot = mvpEngine->addSoundSourceFromFile("../Media/Sounds/headshot.ogg",irrklang::ESM_NO_STREAMING);
	mpRocketBoots = mvpEngine->addSoundSourceFromFile("../Media/Sounds/rocketboots.ogg",irrklang::ESM_NO_STREAMING);
	mpRocketBootsFade = mvpEngine->addSoundSourceFromFile("../Media/Sounds/rocketbootsfade.ogg",irrklang::ESM_NO_STREAMING);	
	mpMoveBox = mvpEngine->addSoundSourceFromFile("../Media/Sounds/movebox.ogg",irrklang::ESM_NO_STREAMING);	
}

/*---------------------------------------------------------------------------------*/
void SoundManager::deleteSound()
{
	//Delete and remove the sound engine.
	mvpEngine->drop();
}

/*---------------------------------------------------------------------------------*/
void SoundManager::playBackgroundSound()
{
	//Play the ambient background sound.
	mvpBgSound = mvpEngine->play2D("../Media/Sounds/ambient.ogg",true,false,true);
}
/*---------------------------------------------------------------------------------*/
void SoundManager::playSound(const char* soundFileName, Ogre::Vector3 pos)
{
	//Plays a 3D sound in the world.
	irrklang::ISound *sound = mvpEngine->play3D(soundFileName, irrklang::vec3df(pos.x, pos.y, pos.z),false,false,true);
	sound->setMinDistance(10.0);
}


/*---------------------------------------------------------------------------------*/
void SoundManager::playSound(irrklang::ISoundSource* s, Ogre::Vector3 pos,bool loop)
{
	//Plays a 3D sound in the world. If loop is true, then loop the Rocket Boots sound.
	if (loop)
	{
		mvpRocketSound = mvpEngine->play3D(s, irrklang::vec3df(pos.x, pos.y, pos.z),loop,false,true);
		mvUpdateRocketSound = true;
	}
	else
		irrklang::ISound *sound = mvpEngine->play3D(s, irrklang::vec3df(pos.x, pos.y, pos.z),loop,false,true);

}

/*---------------------------------------------------------------------------------*/
void SoundManager::removeGameStateSounds()
{
	//Remove all the sounds from the Game State.
	mvpEngine->removeAllSoundSources();
}
/*---------------------------------------------------------------------------------*/
void SoundManager::stopBackgroundSound()
{
	//If there is a background sound, stop and delete it.
	if(mvpBgSound)
		mvpBgSound->stop();                
}
/*---------------------------------------------------------------------------------*/
void SoundManager::stopRocketSound(Ogre::Vector3 pos)
{
	//Stop the normal RocketBoots sound.
	mvpRocketSound->stop();

	//Play the Rocket Boots fade sound if it has never been initiated 
	if (mvpRocketFadeSound == 0)
		mvpRocketFadeSound=mvpEngine->play3D(mpRocketBootsFade, irrklang::vec3df(pos.x, pos.y, pos.z),false,false,true);

	//Play the Rocket Boots fade sound if the previous fade sound is finished.
	if (mvpRocketFadeSound->isFinished())
		mvpRocketFadeSound=mvpEngine->play3D(mpRocketBootsFade, irrklang::vec3df(pos.x, pos.y, pos.z),false,false,true);
}
/*---------------------------------------------------------------------------------*/
void SoundManager::updateListenerPos(Ogre::Vector3 pos)
{
	//Updates the listener position
	mvpEngine->setListenerPosition(irrklang::vec3df(pos.x, pos.y, pos.z), irrklang::vec3df(0,0,0));

	//If mvUpdateRocketSound = true, then update the position of the rocket boots sound.
	if (mvUpdateRocketSound)
	{
		if (!mvpRocketSound->isFinished())
			mvpRocketSound->setPosition(irrklang::vec3df(pos.x, pos.y, pos.z));
		if (mvpRocketFadeSound!= 0 && !mvpRocketFadeSound->isFinished())
		{
			mvUpdateRocketSound=false;
			mvpRocketFadeSound->setPosition(irrklang::vec3df(pos.x, pos.y, pos.z));
		}
	}
}
/*---------------------------------------------------------------------------------*/