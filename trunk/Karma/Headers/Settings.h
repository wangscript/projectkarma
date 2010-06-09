/*---------------------------------------------------------------------------------*/
/* File: Settings.h																   */
/* Author: Per Karlsson, perkarlsson89@gmail.com								   */
/*																				   */
/* Description:	The settings class unfortunately bad coded. It has lots of		   */
/* different member variables instead of a map containing them all. Anyway, it     */
/* is used to prevent re-compiling when testing new values for stuff.			   */
/*---------------------------------------------------------------------------------*/

#ifndef SETTINGS_H
#define SETTINGS_H

#include <fstream>
#include <string>
#include <Ogre.h>

using namespace std;
class Settings
{
private:
	Settings(){};
	static Settings singleton;

	vector<float*> vectorNumbers;
	vector<string*> vectorStrings;
public:
	static Settings getSingleton(){return singleton;};
	static Settings* getSingletonPtr(){return &singleton;};
	void setLog(Ogre::Log* l){mpLog = l;};
	Ogre::Log* mpLog;

	float  mTemp1; 
	float  mTemp2; 
	float  mTemp3;
	float  mTemp4; 
	float  mTemp5; 
	float  mTemp6; 

	string* mAimerGunMaterial; 
	string* mAimerMuzzleMaterial; 
	float* mAimerMuzzleTimer; 
	float* mAimerBloodTimer; 
	float  mAimerFireDistanceMax; 
	float  mAimerFireDistanceMin;
	float  mAimerShotDmgNormal; 
	float  mAimerShotDmgHeadshot; 
	float  mAimerShotForce; 
	float* mAimerOffsetVectorLX; 
	float* mAimerOffsetVectorLY; 
	float* mAimerOffsetVectorLZ; 
	float* mAimerOffsetQuatLW; 
	float* mAimerOffsetQuatLX; 
	float* mAimerOffsetQuatLY;
	float* mAimerOffsetQuatLZ; 
	float* mAimerOffsetVectorRX; 
	float* mAimerOffsetVectorRY; 
	float* mAimerOffsetVectorRZ; 
	float* mAimerOffsetQuatRW; 
	float* mAimerOffsetQuatRX; 
	float* mAimerOffsetQuatRY; 
	float* mAimerOffsetQuatRZ; 

	float* mChunksNumber;
	float* mChunksGridArea;

	float  mBulletHoleSize; 

	float* mAmbientLight; 
	float* mNearClipDistance; 
	float* mFarClipDistance; 
	float* mShadowFarDistance; 
	float* mDirectionalLightDirX; 
	float* mDirectionalLightDirY; 
	float* mDirectionalLightDirZ; 
	float* mFogColor; 
	float* mFogStart; 
	float* mFogEnd; 
	string* mSkyBox; 

	float  mNPCUpdateTimerFreq; 
	float  mNPCAimerRangeLong; 
	float  mNPCAimerRangeLongFreq; 
	float  mNPCAimerRangeShort; 
	float  mNPCAimerRangeShortFreq;
	float  mNPCAimerRandomness; 

	float* mPysicsFrictionStatic;
	float* mPysicsFrictionDynamic; 
	float* mPysicsRestitution;

	float* mPlayerSpawnPointX; 
	float* mPlayerSpawnPointY;
	float* mPlayerSpawnPointZ; 
	float* mPlayerParticleMoveBoxOffsetX; 
	float* mPlayerParticleMoveBoxOffsetY; 
	float* mPlayerParticleMoveBoxOffsetZ; 
	float* mPlayerParticleRocketBootsOffsetX;
	float* mPlayerParticleRocketBootsOffsetY; 
	float* mPlayerParticleRocketBootsOffsetZ; 
	float  mPlayerMaxFallSpeedBeforeDie; 
	float  mPlayerRocketBootsUpdateFreq; 
	float  mPlayerRocketBootsCastingBarUpFreq;
	float  mPlayerUpdateBonesFreq; 
	float  mPlayerRocketBootsForceY; 
	float  mPlayerContiniousShootTimer; 
	float  mPlayerMoveBoxDuration; 
	float  mPlayerRocketBootsDuration; 
	float  mPlayerSuperSpeedDuration;
	float  mPlayerSuperSpeed; 

	float  mCamZoom; 
	float  mCamRotate; 
	float  mCamVelocityY; 
	float  mCamVelocityXZ; 
	float  mCamHeightMaxY;
	float  mCamHeightMinY; 
	float  mCamDistanceMax; 
	float  mCamDistanceMin; 
	float  mCamCharYAdjust;
	float  mCamGunTrackerOffsetX; 
	float  mCamGunTrackerOffsetY; 
	float  mCamGunTrackerOffsetZ; 
	float* mCamMixHelperHeight; 
	float  mCamThirdDistance; 
	float  mCamThirdHeight; 
	float* mCamMass; 
	float* mCamLinearDamping; 
	float* mCamAngularDamping; 
	float* mCamRadius; 
	float  mCamStartPosX; 
	float  mCamStartPosY; 
	float  mCamStartPosZ; 

	float  mCharSpeedForward; 
	float  mCharSpeedBackward; 
	float  mCharSpeedStrafeLeft; 
	float  mCharSpeedStrafeRight; 
	float* mCharScale;
	float* mCharRotation; 
	float  mCharDieAngularDamping; 
	float  mCharRespawnTime;
	Ogre::Vector3 mCharFaceDirection;
	float* mCharCapsuleMass; 
	float* mCharSphereMass; 
	float  mCharSphereRadius; 
	float  mCharSpherePosY;
	float  mCharCapsuleHeight;
	float  mCharCapsuleRadius; 
	float  mCharJumpForceY; 
	float  mCharFreezeAfterDieTimer; 

	float* mTerrainSize; 
	float  mTerrainWorldSize; 
	float* mTerrainInputScale; 
	float* mTerrainMinBatchSize; 
	float* mTerrainMaxBatchSize; 
	float* mTerrainMaxPixelError; 
	float* mTerrainCompositeMapDistance; 
	float* mTerrainDefaultAnisotropy;
	float* mTerrainAdjustY;
	float* mTerrainTextureWorldSize;

	/* Load settings from the settings.cfg.  */
	void loadSettingsFromFile();

	/* Delete all pointers. */
	void deletePointers();

};
#endif


