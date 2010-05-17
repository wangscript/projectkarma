#include "Effects.h"
/*---------------------------------------------------------------------------------*/
std::vector<Effects*> Effects::mtDynamicEffects;
/*---------------------------------------------------------------------------------*/
void Effects::update(const double& timeSinceLastFrame)
{
	//Updates the timer
	mtTimer += timeSinceLastFrame;

	//If timer > the reset timer, call resetTimer and reset the timer.
	if (mtTimer >= mtTimerReset)
	{
		resetTimer();
		mtTimer = 0.0;
	}
}
/*---------------------------------------------------------------------------------*/
void Effects::updateAll(const double& timeSinceLastFrame)
{
	//Updates all effects in the static vector mtDynamicEffects
	for (std::vector<Effects*>::const_iterator it = (mtDynamicEffects).begin(); it!= (mtDynamicEffects).end(); it++)
	{
		if ((*it)->mtTimer > 0)
			(*it)->update(timeSinceLastFrame);
	}
}
/*---------------------------------------------------------------------------------*/
void Effects::addDynamicEffect(Effects* e)
{
	//Add to the dynamic effects
	mtDynamicEffects.push_back(e);
}
/*---------------------------------------------------------------------------------*/
/*							Muzzle Fire
/*---------------------------------------------------------------------------------*/
MuzzleFire::MuzzleFire(const double& timerReset,Ogre::SceneManager* sceneMgr,const Ogre::Real width, const Ogre::Real height, 
					   const Ogre::String& material,const Ogre::String& name) : Effects(timerReset)
{
	//Creates the needed billboardset and billboards.
	//Billboard = Sprite = 2D image that is always oriented towards the viewer.
	mvpMuzzleFire = sceneMgr->createBillboardSet(name);
	Ogre::Billboard* bbrdMuzzleFire1 = mvpMuzzleFire->createBillboard(Ogre::Vector3(0,0,0));
	bbrdMuzzleFire1->setDimensions(width,height);
	mvpMuzzleFire->setMaterialName(material);
	mvpMuzzleFire->setVisible(false);
}
/*---------------------------------------------------------------------------------*/
void MuzzleFire::startTimer()
{
	setVisible(true);
	mtTimer+=0.001;
}
/*---------------------------------------------------------------------------------*/
void MuzzleFire::resetTimer()
{
	setVisible(false);
}	
/*---------------------------------------------------------------------------------*/
void MuzzleFire::setVisible(bool state)
{
	mvpMuzzleFire->setVisible(state);
}
/*---------------------------------------------------------------------------------*/
/*									Blood
/*---------------------------------------------------------------------------------*/
Blood::Blood(const double& timerReset,Ogre::SceneManager *sceneMgr, const Ogre::String &name, const Ogre::String &particleSystem) : Effects(timerReset)
{
	//Creates the particle system.
	mvpBloodPS = sceneMgr->createParticleSystem(name +Ogre::String("BloodPS"), "blood");
	//Not using woorld coordinates.
	mvpBloodPS->setKeepParticlesInLocalSpace(true);
	mvpBloodPS->setVisible(false);
	//Attach it to a node.
	mvpBloodNode = sceneMgr->getRootSceneNode()->createChildSceneNode(name + Ogre::String("BloodNode"));
	mvpBloodNode->attachObject(mvpBloodPS);
}
/*---------------------------------------------------------------------------------*/
void Blood::startTimer()
{
	//Clear all existing particles
	mvpBloodPS->clear();
	//Starts emitting
	mvpBloodPS->setEmitting(true);
	mvpBloodPS->setVisible(true);
	mtTimer+=0.001;
}
/*---------------------------------------------------------------------------------*/
void Blood::resetTimer()
{
	//Stops emitting particles
	mvpBloodPS->setEmitting(false);
}
/*---------------------------------------------------------------------------------*/
/*					Manually Controlled Particles
/*---------------------------------------------------------------------------------*/
ManuallyControlledParticles::ManuallyControlledParticles(Ogre::SceneManager *sceneMgr,const Ogre::String &name, const Ogre::String &material) : Effects(0)
{
	//See Blood::Blood
	mvpPS = sceneMgr->createParticleSystem(name, material); 
	mvpPS->setKeepParticlesInLocalSpace(true);
	mvpPS->setEmitting(false);
	mvpPS->setVisible(false);
}
/*---------------------------------------------------------------------------------*/
void ManuallyControlledParticles::startTimer()
{
	//See Blood::startTimer
	mvpPS->clear();
	mvpPS->setEmitting(true);
	mvpPS->setVisible(true);
}
/*---------------------------------------------------------------------------------*/
void ManuallyControlledParticles::resetTimer()
{
	mvpPS->setEmitting(false);
}
/*---------------------------------------------------------------------------------*/