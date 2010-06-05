#include <SoundManager.h>


SoundManager::SoundManager() 
{
	updateRocketSound = false;
		m_engine = irrklang::createIrrKlangDevice();
	mpRocketSound = 0;
	mpRocketFadeSound = 0;
	bgSound = 0;
}

void SoundManager::initGameStateSound()
{
	mpGun = m_engine->addSoundSourceFromFile("../Media/Sounds/gun.ogg",irrklang::ESM_NO_STREAMING);
	mpBodyHit = m_engine->addSoundSourceFromFile("../Media/Sounds/bodyhit.ogg",irrklang::ESM_NO_STREAMING);
	mpSplash = m_engine->addSoundSourceFromFile("../Media/Sounds/splash.ogg",irrklang::ESM_NO_STREAMING);
	mpHeadShot = m_engine->addSoundSourceFromFile("../Media/Sounds/headshot.ogg",irrklang::ESM_NO_STREAMING);
	mpRocketBoots = m_engine->addSoundSourceFromFile("../Media/Sounds/rocketboots.ogg",irrklang::ESM_NO_STREAMING);
	mpRocketBootsFade = m_engine->addSoundSourceFromFile("../Media/Sounds/rocketbootsfade.ogg",irrklang::ESM_NO_STREAMING);	
	mpMoveBox = m_engine->addSoundSourceFromFile("../Media/Sounds/movebox.ogg",irrklang::ESM_NO_STREAMING);	
}

void SoundManager::removeGameStateSounds()
{
	m_engine->removeAllSoundSources();
}

void SoundManager::playBackgroundSound()
{
		bgSound = m_engine->play2D("../Media/Sounds/ambient.ogg",true,false,true);
}

void SoundManager::stopBackgroundSound()
{
	if(bgSound)
		bgSound->stop();		
}

void SoundManager::deleteSound()
{
	m_engine->drop();
}

void SoundManager::setBackgroundMusic(const char* soundFileName)
{
	m_engine->play2D(soundFileName, true);
}

void SoundManager::setBackgroundMusicVolume(const float value)
{
	m_BackgroundMusic->setVolume(value);
}

void SoundManager::removeBackroundMusic()
{
	m_BackgroundMusic->drop();
}

void SoundManager::playSound(const char* soundFileName, Ogre::Vector3 pos)
{
	irrklang::ISound *sound = m_engine->play3D(soundFileName, irrklang::vec3df(pos.x, pos.y, pos.z),false,false,true);

	sound->setMinDistance(10.0);
}


void SoundManager::playSound(irrklang::ISoundSource* s, Ogre::Vector3 pos,bool loop)
{
	//sound->setMinDistance(10.0);
	if (loop)
	{
   		mpRocketSound = m_engine->play3D(s, irrklang::vec3df(pos.x, pos.y, pos.z),loop,false,true);
		updateRocketSound = true;
	}
	else
		irrklang::ISound *sound = m_engine->play3D(s, irrklang::vec3df(pos.x, pos.y, pos.z),loop,false,true);

}

void SoundManager::updateListenerPos(Ogre::Vector3 pos)
{
	m_engine->setListenerPosition(irrklang::vec3df(pos.x, pos.y, pos.z), irrklang::vec3df(0,0,0));
	if (updateRocketSound)
	{
		if (!mpRocketSound->isFinished())
			mpRocketSound->setPosition(irrklang::vec3df(pos.x, pos.y, pos.z));
		if (mpRocketFadeSound!= 0 && !mpRocketFadeSound->isFinished())
		{
			updateRocketSound=false;
			mpRocketFadeSound->setPosition(irrklang::vec3df(pos.x, pos.y, pos.z));
		}
	}
}

void SoundManager::play2D(irrklang::ISoundSource* sound,bool loop)
{
	mpRocketSound = m_engine->play2D(sound,loop,true);
}


void SoundManager::stopRocketSound(Ogre::Vector3 pos)
{
	//if(mpRocketSound)
	mpRocketSound->stop();
	//mpRocketSound = 0;
	
	if (mpRocketFadeSound == 0)
		mpRocketFadeSound=m_engine->play3D(mpRocketBootsFade, irrklang::vec3df(pos.x, pos.y, pos.z),false,false,true);

	if (mpRocketFadeSound->isFinished())
		mpRocketFadeSound=m_engine->play3D(mpRocketBootsFade, irrklang::vec3df(pos.x, pos.y, pos.z),false,false,true);
}
