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
	
	void die();
	void firstPersonMode(bool s=true);
	void semiThirdPersonMode(bool s=true);
	float getCastingBarTimer(){return mvCastingBarTimer;};
	Ogre::SceneNode* getPlayerNode(){return mtpCharNode;};	
	int getPowerUp(){return mvPowerUp;};
	void jump();
	void move(const double& timeSinceLastFrame);
	void moveBoxMoved(const OIS::MouseEvent &e);
	void moveBoxPressed(const OIS::MouseEvent &e);
	void queueFire(const double& x,const double& y);
	void queueRocketBoots();
	void releaseBox();
	void removePowerUp(){mvPowerUp = Game::PowerUp_None;};
	void resetRocketBoots();
	void rocketBoots(const double& timeSinceLastFrame);
	void setMousePosition(const int& x,const  int& y){mvMouseX=x;mvMouseX=y;};
	void setPowerUp(const int& p,float time = 0.0f);
	void setNeedUpdateBones(){mvNeedUpdateBones = true;};
	void setNeedUpdateRotation(bool s){mvNeedUpdateRotation=s;};
	void startRocketBoots();
	void updateCursorPos(int x,int y){mvMouseX = x;mvMouseY=y;};
	void updateCastingBar(const double& timeSinceLastFrame);
	void updateChunk(const NxOgre::Vec3& hitBoxPos);
	float updateHp(float minusHP);
	void updatePosition();

private:
	ManuallyControlledParticles	*mvpMoveBoxParticles,*mvpRocketBootsParticle1,*mvpRocketBootsParticle2;
	MuzzleFireFirstPerson		*mvpMuzzleFireFirstPerson;
	Ogre::SceneNode				*mvpCamCollisionNode,*mvpCamGunModeNode;
	OGRE3DKinematicBody			*mvpMoveBoxController;
	NxOgre::FixedJoint			*mvpMoveBoxJoint;
	Ogre::Camera				*mvpCamera;
	Ogre::Vector3				mvDirCamToChar;
	float						mvCastingBarTimer,mtCharSpeedSuper,mtScreenWidth,mtScreenHeight,mvRocketBootsTimer,mvQuatTimer,*mvpMaxSpeedBeforeDie,
								mvRocketBootsUpdateFreq,mvRocketBootsCastingBarUpFreq,*mvpRocketBootsForceY,*mvpRocketBootsDuration,
								mvUpdateBonesFreq,*mvpContiniousShootTimer,*mvpMoveBoxDuration,*mvpSuperSpeedDuration;
	int							mvPowerUp,mvMouseX,mvMouseY,mvPowerUpCastingBar;
	GridData					mtChunk;
	bool						mvNeedUpdateBones,mvNeedUpdateRotation,mvRocketBoots;

	void respawn();
};
#endif