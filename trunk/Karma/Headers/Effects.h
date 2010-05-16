#ifndef EFFECTS_H
#define EFFECTS_H

#include <Ogre.h>
#include <vector>

class Effects
{
public:
	Effects(const double& timerReset) : mtTimer(0.0),mtTimerReset(timerReset){};
	virtual void update(const double& timeSinceLastFrame);
	
	static void updateAll(const double& timeSinceLastFrame);
	virtual void startTimer() = 0;
	static void Effects::addDynamicEffect(Effects* e);
protected:

	static std::vector<Effects*> mtDynamicEffects;
	virtual void resetTimer() = 0;
	double mtTimer;
	const double mtTimerReset;
};

class MuzzleFire : public Effects
{
public:
	MuzzleFire(const double& timerReset,Ogre::SceneManager* sceneMgr,const Ogre::Real width,
		const Ogre::Real height, const Ogre::String& material,const Ogre::String& name);
	void startTimer();
	Ogre::BillboardSet* getBillboardSet(){return mvpMuzzleFire;};
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
	void startTimer();
	void update(const double& timeSinceLastFrame){};
	void setManualReset(){resetTimer();};
	Ogre::ParticleSystem* getParticleSystem(){return mvpPS;};
private:
	Ogre::ParticleSystem* mvpPS;
	void resetTimer();
};

#endif
