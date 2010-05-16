#ifndef PLAYER_H
#define PLAYER_H

#include "Character.h"
#include "Aimer.h"

//@todo settings
#define CAMERA_HEIGHT			1.2
#define CAMERA_DISTANCE			3.0


class Player : public Character, public Aimer
{
public:
	Player(Ogre::SceneManager* sceneMgr,NxOgre::Scene* physScene,OGRE3DRenderSystem* renderSystem, Ogre::String filename, 
		Ogre::String name, Ogre::Vector3 spawnPoint, float hp , float walkSpeed);
	~Player();
	void updateCastingBar(const double& timeSinceLastFrame);
	float getCastingBarTimer(){return mvCastingBarTimer;};

	void setPowerUp(const int& p,float time = 0.0f);
	void removePowerUp(){mvPowerUp = Game::PowerUp_None;};
	int getPowerUp(){return mvPowerUp;};
	Ogre::SceneNode* getPlayerNode(){return mtpCharNode;};
	void updateChunk(const NxOgre::Vec3& hitBoxPos);

	void move(const double& timeSinceLastFrame);
	void moveBoxMoved(const OIS::MouseEvent &e);
	void moveBoxPressed(const OIS::MouseEvent &e);
	void releaseBox();
	void rocketBoots();
	void queueFire();
	void startRocketBoots();
	void resetRocketBoots();
	void updatePosition();
private:
	ManuallyControlledParticles*			mvpMoveBoxParticles;
	ManuallyControlledParticles*		mvpRocketBootsParticle1;
	ManuallyControlledParticles*		mvpRocketBootsParticle2;
	Ogre::SceneNode*			mvpCamCollisionNode;
	Ogre::SceneNode*			mvpCamGunModeNode;
	OGRE3DKinematicBody*		mvpMoveBoxController;
	NxOgre::FixedJoint*			mvpMoveBoxJoint;
	Ogre::Camera*				mvpCamera;
	float						mvCastingBarTimer;
	int							mvPowerUp;
	float						mtCharSpeedSuper;
	GridData					mtChunk;
};
#endif