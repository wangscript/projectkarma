#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <irrklang.h>
#include <Ogre.h>

class SoundManager
{
protected:

	irrklang::ISoundEngine*		m_engine;
	irrklang::ISound*			m_BackgroundMusic;
	

public:
	irrklang::ISoundSource*		mpGun;
	irrklang::ISoundSource*		mpSplash;
	irrklang::ISoundSource*		mpBodyHit;
	irrklang::ISoundSource*		mpWood;
    SoundManager();
    ~SoundManager();

    void initSound();
	void deleteSound();

	void setBackgroundMusic(const char*);
	void setBackgroundMusicVolume(const float);
	void removeBackroundMusic();

	void playSound(const char* , Ogre::Vector3 );
	void playSound(irrklang::ISoundSource* sound, Ogre::Vector3 pos);
	void updateListenerPos(Ogre::Vector3 );
};

#endif