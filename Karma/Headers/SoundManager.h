#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <irrklang.h>
#include <Ogre.h>

class SoundManager
{
protected:

	irrklang::ISoundEngine*		m_engine;
	irrklang::ISound*			m_BackgroundMusic;
	irrklang::ISound*			mpRocketSound;
	irrklang::ISound*			mpRocketFadeSound;
	irrklang::ISound*			bgSound;
	bool updateRocketSound;

public:
	irrklang::ISoundSource*		mpGun;
	irrklang::ISoundSource*		mpSplash;
	irrklang::ISoundSource*		mpBodyHit;
	irrklang::ISoundSource*		mpWood;
	irrklang::ISoundSource*		mpHeadShot;
	irrklang::ISoundSource*		mpRocketBoots;
	irrklang::ISoundSource*		mpRocketBootsFade;
	irrklang::ISoundSource*		mpMoveBox;
    SoundManager();

	void initGameStateSound();
	void removeGameStateSounds();
	void deleteSound();

	void playBackgroundSound();
	void stopBackgroundSound();

	void setBackgroundMusic(const char*);
	void setBackgroundMusicVolume(const float);
	void removeBackroundMusic();

	void playSound(const char* , Ogre::Vector3 );
	void playSound(irrklang::ISoundSource* sound, Ogre::Vector3 pos,bool loop=false);
	void play2D(irrklang::ISoundSource* sound,bool loop);
	void stopRocketSound(Ogre::Vector3 pos);
	void updateListenerPos(Ogre::Vector3 );
};

#endif