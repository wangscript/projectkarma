#include <SoundManager.h>


SoundManager::SoundManager()
{
		
}
SoundManager::~SoundManager()
{
}

void SoundManager::initSound()
{
	updateRocketSound = false;
	m_engine = irrklang::createIrrKlangDevice();
	mpGun = m_engine->addSoundSourceFromFile("Pistol Shot 22cal 01.wav",irrklang::ESM_NO_STREAMING);
	mpBodyHit = m_engine->addSoundSourceFromFile("bodyhit.wav",irrklang::ESM_NO_STREAMING);
	mpSplash = m_engine->addSoundSourceFromFile("splash.wav",irrklang::ESM_NO_STREAMING);
	mpWood = m_engine->addSoundSourceFromFile("Wood Destruction 01.wav",irrklang::ESM_NO_STREAMING);
	mpHeadShot = m_engine->addSoundSourceFromFile("headshot.wav",irrklang::ESM_NO_STREAMING);
	mpRocketBoots = m_engine->addSoundSourceFromFile("rocketboots.wav",irrklang::ESM_NO_STREAMING);
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
	mpRocketSound->setPosition(irrklang::vec3df(pos.x, pos.y, pos.z));
}

void SoundManager::play2D(irrklang::ISoundSource* sound,bool loop)
{
	mpRocketSound= m_engine->play2D(sound,loop,true);
}

void SoundManager::stopPlaySound(irrklang::ISoundSource* sound)
{
	mpRocketSound->stop();
	updateRocketSound = false;
}
