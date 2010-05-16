#include "Effects.h"
//Key to victory
std::vector<Effects*> Effects::mtDynamicEffects;

void Effects::update(const double& timeSinceLastFrame)
{
	mtTimer += timeSinceLastFrame;

	if (mtTimer >= mtTimerReset)
	{
		resetTimer();
		mtTimer = 0.0;
	}
}

void Effects::updateAll(const double& timeSinceLastFrame)
{
	for (std::vector<Effects*>::const_iterator it = (mtDynamicEffects).begin(); it!= (mtDynamicEffects).end(); it++)
	{
		if ((*it)->mtTimer > 0)
			(*it)->update(timeSinceLastFrame);
	}
}

void Effects::addDynamicEffect(Effects* e)
{
	mtDynamicEffects.push_back(e);
}

MuzzleFire::MuzzleFire(const double& timerReset,Ogre::SceneManager* sceneMgr,const Ogre::Real width, const Ogre::Real height, 
					   const Ogre::String& material,const Ogre::String& name) : Effects(timerReset)
{
	mvpMuzzleFire = sceneMgr->createBillboardSet(name);
	Ogre::Billboard* bbrdMuzzleFire1 = mvpMuzzleFire->createBillboard(Ogre::Vector3(0,0,0));
	bbrdMuzzleFire1->setDimensions(width,height);
	mvpMuzzleFire->setMaterialName(material);
	mvpMuzzleFire->setVisible(false);
}


void MuzzleFire::startTimer()
{
	setVisible(true);
	mtTimer+=0.001;
}

void MuzzleFire::resetTimer()
{
	setVisible(false);
}	

void MuzzleFire::setVisible(bool state)
{
	mvpMuzzleFire->setVisible(state);
}

Blood::Blood(const double& timerReset,Ogre::SceneManager *sceneMgr, const Ogre::String &name, const Ogre::String &particleSystem) : Effects(timerReset)
{
	mvpBloodPS = sceneMgr->createParticleSystem(name, "blood");
	mvpBloodPS->setKeepParticlesInLocalSpace(true);
	//ps->fastForward(25);
	mvpBloodPS->setVisible(false);
	mvpBloodNode = sceneMgr->getRootSceneNode()->createChildSceneNode("BloodNode");
	mvpBloodNode->attachObject(mvpBloodPS);
}

void Blood::startTimer()
{
	mvpBloodPS->clear();
	mvpBloodPS->setEmitting(true);
	mvpBloodPS->setVisible(true);
	mtTimer+=0.001;
}

void Blood::resetTimer()
{
	mvpBloodPS->setEmitting(false);
}

ManuallyControlledParticles::ManuallyControlledParticles(Ogre::SceneManager *sceneMgr,const Ogre::String &name, const Ogre::String &material) : Effects(0)
{
	mvpPS = sceneMgr->createParticleSystem(name, material); 
	mvpPS->setKeepParticlesInLocalSpace(true);
	mvpPS->setEmitting(false);
	mvpPS->setVisible(false);
}

void ManuallyControlledParticles::startTimer()
{
	mvpPS->clear();
	mvpPS->setEmitting(true);
	mvpPS->setVisible(true);
}

void ManuallyControlledParticles::resetTimer()
{
	mvpPS->setEmitting(false);
}
