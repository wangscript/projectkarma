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

	float getCastingBarTimer(){return mvCastingBarTimer;};
	Ogre::SceneNode* getPlayerNode(){return mtpCharNode;};	
	int getPowerUp(){return mvPowerUp;};
	void move(const double& timeSinceLastFrame);
	void moveBoxMoved(const OIS::MouseEvent &e);
	void moveBoxPressed(const OIS::MouseEvent &e);
	void queueFire(const double& x,const double& y);
	void releaseBox();
	void removePowerUp(){mvPowerUp = Game::PowerUp_None;};
	void resetRocketBoots();
	void rocketBoots();
	void setPowerUp(const int& p,float time = 0.0f);
	void setNeedUpdateBones(){mvNeedUpdateBones = true;};
	void startRocketBoots();
	void updateCastingBar(const double& timeSinceLastFrame);
	void updateChunk(const NxOgre::Vec3& hitBoxPos);
	void updatePosition();

private:
	ManuallyControlledParticles	*mvpMoveBoxParticles,*mvpRocketBootsParticle1,*mvpRocketBootsParticle2;
	Ogre::SceneNode				*mvpCamCollisionNode,*mvpCamGunModeNode;
	OGRE3DKinematicBody			*mvpMoveBoxController;
	NxOgre::FixedJoint			*mvpMoveBoxJoint;
	Ogre::Camera				*mvpCamera;
	float						mvCastingBarTimer,mtCharSpeedSuper,mtScreenWidth,mtScreenHeight;
	int							mvPowerUp;
	GridData					mtChunk;
	bool						mvNeedUpdateBones;
};
#endif