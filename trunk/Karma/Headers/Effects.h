#ifndef EFFECTS_H
#define EFFECTS_H

#include <Ogre.h>
#include <vector>

class Effects
{
public:
	Effects(const double& timerReset) : mtTimer(0.0),mtTimerReset(timerReset){};

	virtual void startTimer() = 0; //Abstract
	virtual void update(const double& timeSinceLastFrame);

	static void updateAll(const double& timeSinceLastFrame);
	static void Effects::addDynamicEffect(Effects* e);

protected:
	double mtTimer;
	const double mtTimerReset;

	static std::vector<Effects*> mtDynamicEffects;

	virtual void resetTimer() = 0; //Abstract
};

class MuzzleFire : public Effects
{
public:
	MuzzleFire(const double& timerReset,Ogre::SceneManager* sceneMgr,const Ogre::Real width,
		const Ogre::Real height, const Ogre::String& material,const Ogre::String& name);
	
	Ogre::BillboardSet* getBillboardSet(){return mvpMuzzleFire;};
	void startTimer();
	
private:
	Ogre::BillboardSet* mvpMuzzleFire;

	void setVisible(bool state);
	void resetTimer();
};

class Blood : public Effects
{
public:
	Blood(const double& timerReset,Ogre::SceneManager* sceneMgr,const Ogre::String& name,const Ogre::String& particleSystem);

	void setPosition(const Ogre::Vector3& p){mvpBloodNode->setPosition(p);};
	void startTimer();
private:
	Ogre::ParticleSystem* mvpBloodPS;
	Ogre::SceneNode* mvpBloodNode;

	void resetTimer();
};

class ManuallyControlledParticles : public Effects
{
public:
	ManuallyControlledParticles(Ogre::SceneManager *sceneMgr,const Ogre::String &name, const Ogre::String &material);

	Ogre::ParticleSystem* getParticleSystem(){return mvpPS;};
	void setManualReset(){resetTimer();};
	void startTimer();
	void update(const double& timeSinceLastFrame){};
	
private:
	Ogre::ParticleSystem* mvpPS;

	void resetTimer();
};

#endif
