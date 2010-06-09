/*---------------------------------------------------------------------------------*/
/* File: Effects.h											   					   */
/* Author: Per Karlsson, perkarlsson89@gmail.com								   */
/*																				   */
/* Description:	Here all effects classes are collected. Info about each class	   */
/* is defined below in the file.												   */
/*---------------------------------------------------------------------------------*/

#ifndef EFFECTS_H
#define EFFECTS_H

#include <Ogre.h>
#include "Appstate.h"
#include <vector>

/* Effects is a superclass for all effects in the game with a timer. */
class Effects
{
public:
	/* mtTimerReset is how long the effect will be shown. */
	Effects(const double& timerReset) : mtTimer(0.0),mtTimerReset(timerReset){};

	/* Abstract function. Every effect has its own way of starting. Particle Systems start emitting, Billboards->setVisible etc etc */
	virtual void startTimer() = 0;

	/* Updates the timer of the effect. If the timer is bigger or equal with the mtTimerReset, then the reset function will be called. */
	virtual void update(const double& timeSinceLastFrame);

	/* Static function that updates all dynamic effects (effects that aren't bound to any specific chunk). */
	static void updateAll(const double& timeSinceLastFrame);

	/* Adds an effect to the mtDynamicEffects vector. */
	static void Effects::addDynamicEffect(Effects* e);

protected:
	double mtTimer;
	const double mtTimerReset;

	/* See ::updateAll. */
	static std::vector<Effects*> mtDynamicEffects;

	/* Abstract function. See ::startTimer(). Every effect has its own way of resetting the effect. */
	virtual void resetTimer() = 0; //Abstract
};

/* MuzzleFire is a simple Billboard (sprite, 2D texture) with a muzzle fire texture */
class MuzzleFire : public Effects
{
public:
	/*  
	timerReset = how long the Muzzle Fire will be visible.
	sceneMgr = the OGRE scenemanager
	width = width of the billboard
	height = height of the billboard
	material = material of the texture
	name = name of the billboardset
	*/
	MuzzleFire(const double& timerReset,Ogre::SceneManager* sceneMgr,const Ogre::Real width,
		const Ogre::Real height, const Ogre::String& material,const Ogre::String& name);
	
	/* Returns the BillboardSet of the Muzzle Fire. */
	Ogre::BillboardSet* getBillboardSet(){return mvpMuzzleFire;};

	/* Start the timer and makes the billboard visible. */
	void startTimer();
	
private:
	Ogre::BillboardSet* mvpMuzzleFire;

	/* Sets the billboard visible or invisible. */
	void setVisible(bool state);

	/* Resets the timer and hides the billboard. */
	void resetTimer();
};

/* The same as Muzzle Fire except this one only shows in First Person view. */
class MuzzleFireFirstPerson : public Effects
{
public:
	/* TimerReset = how long the effect will be shown. */
	MuzzleFireFirstPerson(const double& timerReset);
	
	/* see MuzzleFire::startTimer() */
	void startTimer();
	
private:
	/* MuzzleFire::resetTimer() */
	void resetTimer();
};

/* Blood is a particle system of redish textures. */
class Blood : public Effects
{
public:
	/* 
	timerReset = how long the Muzzle Fire will be visible.
	sceneMgr = the OGRE scenemanager.
	name = name of the character of the spawns the blood effect.
	particleSystem = name of the particleSystem ("blood" for example)
	*/
	Blood(const double& timerReset,Ogre::SceneManager* sceneMgr,const Ogre::String& name,const Ogre::String& particleSystem);

	/* setPosition is called everytime a character has been shot. The position of the particle system is the hit point in the world. */
	void setPosition(const Ogre::Vector3& p){mvpBloodNode->setPosition(p);};

	/* The particle system starts emitting particles. */
	void startTimer();
private:
	Ogre::ParticleSystem* mvpBloodPS;
	Ogre::SceneNode* mvpBloodNode;

	/* The particle system stops emitting. */
	void resetTimer();
};

/* ManuallyControlledParticles is a class for particle systems that don't have specific length.
PowerUp_MoveBox and PowerUp_RocketBoots are examples of such particle systems. The length here depends on the users input.*/ 
class ManuallyControlledParticles : public Effects
{
public:
	/*
	sceneMgr = OGRE scene manager.
	name = "name" of the particle system.
	particleSystem = name of the particleSystem template.
	*/
	ManuallyControlledParticles(Ogre::SceneManager *sceneMgr,const Ogre::String &name, const Ogre::String &particleSystem);

	/* Returns the particle system */ 
	Ogre::ParticleSystem* getParticleSystem(){return mvpPS;};

	/* Resets the particle system. */
	void setManualReset(){resetTimer();};

	/* Start emitting particles. */
	void startTimer();

	/* Needs to overwrite the update function so the resetTimer() isn't called. */
	void update(const double& timeSinceLastFrame){};
	
private:
	Ogre::ParticleSystem* mvpPS;

	/* Stops emitting particles. */
	void resetTimer();
};

/* BulletHoles is an alone class for creating bullet holes on the static enviroment.
A bullet hole is a simple plane with a texture on it.*/
class BulletHoles
{
private:
	/* Singleton, easier to use. */
	static BulletHoles singleton;
	BulletHoles(): mvBulletHoles(0){};

	int mvBulletHoles;
	Ogre::SceneManager* mvpSceneMgr;
public:
	/* Get the singleton */
	static BulletHoles getSingleton(){return singleton;};
	static BulletHoles* getSingletonPtr(){return &singleton;};

	/* Adds a plane with normal and of position. */
	void addBulletHole(const Ogre::Vector3 &normal,const Ogre::Vector3 &pos);

	/* Sets the OGRE scene manager. Needs to be done in the createScene() of every level. */
	void setSceneManager(Ogre::SceneManager* s) {mvpSceneMgr = s;};

	/* Resets the amount of bulletHoles. */
	void resetBulletHoles(){mvBulletHoles = 0;};
};
#endif
