#include "Settings.h"

Settings Settings::singleton;

void Settings::loadSettingsFromFile()
{
	float temp;
	ifstream f;
	vectorNumbers.clear();
	vectorStrings.clear();
	f.open("settings.cfg", ios::in);

	if ( !f )
		mpLog->logMessage("Could not open settings.txt!");

	string s;
	stringstream ss;
	mpLog->logMessage("\n=======================\n=       SETTINGS       =\n=======================\n");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mTemp1")
	{
		f >> mTemp1;
		mpLog->logMessage("mTemp1 = " + Ogre::StringConverter::toString(mTemp1));
	}
	else
		mpLog->logMessage("Could load settings for mTemp1");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mTemp2")
	{
		f >> mTemp2;
		mpLog->logMessage("mTemp2 = " + Ogre::StringConverter::toString(mTemp2));
	}
	else
		mpLog->logMessage("Could load settings for mTemp2");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mTemp3")
	{
		f >> mTemp3;
		mpLog->logMessage("mTemp3 = " + Ogre::StringConverter::toString(mTemp3));
	}
	else
		mpLog->logMessage("Could load settings for mTemp3");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mTemp4")
	{
		f >> mTemp4;
		mpLog->logMessage("mTemp4 = " + Ogre::StringConverter::toString(mTemp4));
	}
	else
		mpLog->logMessage("Could load settings for mTemp4");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mTemp5")
	{
		f >> mTemp5;
		mpLog->logMessage("mTemp5 = " + Ogre::StringConverter::toString(mTemp5));
	}
	else
		mpLog->logMessage("Could load settings for mTemp5");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mTemp6")
	{
		f >> mTemp6;
		mpLog->logMessage("mTemp6 = " + Ogre::StringConverter::toString(mTemp6));
	}
	else
		mpLog->logMessage("Could load settings for mTemp6");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mAimerGunMaterial")
	{
		f >> s;
		mAimerGunMaterial = new string(s);
		vectorStrings.push_back(mAimerGunMaterial);
		mpLog->logMessage("mAimerGunMaterial = " + Ogre::StringConverter::toString(s.c_str()));
	}
	else
		mpLog->logMessage("Could load settings for mAimerGunMaterial");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mAimerMuzzleMaterial")
	{
		f >> s;
		mAimerMuzzleMaterial = new string(s);
		vectorStrings.push_back(mAimerMuzzleMaterial);
		mpLog->logMessage("mAimerMuzzleMaterial = " + Ogre::StringConverter::toString(s.c_str()));
	}
	else
		mpLog->logMessage("Could load settings for mAimerMuzzleMaterial");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mAimerMuzzleTimer")
	{
		f >> temp;
		mAimerMuzzleTimer = new float(temp);
		vectorNumbers.push_back(mAimerMuzzleTimer);
		mpLog->logMessage("mAimerMuzzleTimer = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mAimerMuzzleTimer");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mAimerBloodTimer")
	{
		f >> temp;
		mAimerBloodTimer = new float(temp);
		vectorNumbers.push_back(mAimerBloodTimer);
		mpLog->logMessage("mAimerBloodTimer = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mAimerBloodTimer");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mAimerFireDistanceMax")
	{
		f >> mAimerFireDistanceMax;
		mpLog->logMessage("mAimerFireDistanceMax = " + Ogre::StringConverter::toString(mAimerFireDistanceMax));
	}
	else
		mpLog->logMessage("Could load settings for mAimerFireDistanceMax");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mAimerFireDistanceMin")
	{
		f >> mAimerFireDistanceMin;
		mpLog->logMessage("mAimerFireDistanceMin = " + Ogre::StringConverter::toString(mAimerFireDistanceMin));
	}
	else
		mpLog->logMessage("Could load settings for mAimerFireDistanceMin");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mAimerShotDmgNormal")
	{
		f >> mAimerShotDmgNormal;
		mpLog->logMessage("mAimerShotDmgNormal = " + Ogre::StringConverter::toString(mAimerShotDmgNormal));
	}
	else
		mpLog->logMessage("Could load settings for mAimerShotDmgNormal");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mAimerShotDmgHeadshot")
	{
		f >> mAimerShotDmgHeadshot;
		mpLog->logMessage("mAimerShotDmgHeadshot = " + Ogre::StringConverter::toString(mAimerShotDmgHeadshot));
	}
	else
		mpLog->logMessage("Could load settings for mAimerShotDmgHeadshot");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mAimerShotForce")
	{
		f >> mAimerShotForce;
		mpLog->logMessage("mAimerShotForce = " + Ogre::StringConverter::toString(mAimerShotForce));
	}
	else
		mpLog->logMessage("Could load settings for mAimerShotForce");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mAimerOffsetVectorLX")
	{
		f >> temp;
		mAimerOffsetVectorLX = new float(temp);
		vectorNumbers.push_back(mAimerOffsetVectorLX);
		mpLog->logMessage("mAimerOffsetVectorLX = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mAimerOffsetVectorLX");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mAimerOffsetVectorLY")
	{
		f >> temp;
		mAimerOffsetVectorLY = new float(temp);
		vectorNumbers.push_back(mAimerOffsetVectorLY);
		mpLog->logMessage("mAimerOffsetVectorLY = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mAimerOffsetVectorLY");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mAimerOffsetVectorLZ")
	{
		f >> temp;
		mAimerOffsetVectorLZ = new float(temp);
		vectorNumbers.push_back(mAimerOffsetVectorLZ);
		mpLog->logMessage("mAimerOffsetVectorLZ = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mAimerOffsetVectorLZ");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mAimerOffsetQuatLW")
	{
		f >> temp;
		mAimerOffsetQuatLW = new float(temp);
		vectorNumbers.push_back(mAimerOffsetQuatLW);
		mpLog->logMessage("mAimerOffsetQuatLW = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mAimerOffsetQuatLW");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mAimerOffsetQuatLX")
	{
		f >> temp;
		mAimerOffsetQuatLX = new float(temp);
		vectorNumbers.push_back(mAimerOffsetQuatLX);
		mpLog->logMessage("mAimerOffsetQuatLX = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mAimerOffsetQuatLX");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mAimerOffsetQuatLY")
	{
		f >> temp;
		mAimerOffsetQuatLY = new float(temp);
		vectorNumbers.push_back(mAimerOffsetQuatLY);
		mpLog->logMessage("mAimerOffsetQuatLY = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mAimerOffsetQuatLY");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mAimerOffsetQuatLZ")
	{
		f >> temp;
		mAimerOffsetQuatLZ = new float(temp);
		vectorNumbers.push_back(mAimerOffsetQuatLZ);
		mpLog->logMessage("mAimerOffsetQuatLZ = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mAimerOffsetQuatLZ");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mAimerOffsetVectorRX")
	{
		f >> temp;
		mAimerOffsetVectorRX = new float(temp);
		vectorNumbers.push_back(mAimerOffsetVectorRX);
		mpLog->logMessage("mAimerOffsetVectorRX = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mAimerOffsetVectorRX");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mAimerOffsetVectorRY")
	{
		f >> temp;
		mAimerOffsetVectorRY = new float(temp);
		vectorNumbers.push_back(mAimerOffsetVectorRY);
		mpLog->logMessage("mAimerOffsetVectorRY = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mAimerOffsetVectorRY");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mAimerOffsetVectorRZ")
	{
		f >> temp;
		mAimerOffsetVectorRZ = new float(temp);
		vectorNumbers.push_back(mAimerOffsetVectorRZ);
		mpLog->logMessage("mAimerOffsetVectorRZ = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mAimerOffsetVectorRZ");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mAimerOffsetQuatRW")
	{
		f >> temp;
		mAimerOffsetQuatRW = new float(temp);
		vectorNumbers.push_back(mAimerOffsetQuatRW);
		mpLog->logMessage("mAimerOffsetQuatRW = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mAimerOffsetQuatRW");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mAimerOffsetQuatRX")
	{
		f >> temp;
		mAimerOffsetQuatRX = new float(temp);
		vectorNumbers.push_back(mAimerOffsetQuatRX);
		mpLog->logMessage("mAimerOffsetQuatRX = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mAimerOffsetQuatRX");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mAimerOffsetQuatRY")
	{
		f >> temp;
		mAimerOffsetQuatRY = new float(temp);
		vectorNumbers.push_back(mAimerOffsetQuatRY);
		mpLog->logMessage("mAimerOffsetQuatRY = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mAimerOffsetQuatRY");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mAimerOffsetQuatRZ")
	{
		f >> temp;
		mAimerOffsetQuatRZ = new float(temp);
		vectorNumbers.push_back(mAimerOffsetQuatRZ);
		mpLog->logMessage("mAimerOffsetQuatRZ = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mAimerOffsetQuatRZ");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mChunksNumber")
	{
		f >> temp;
		mChunksNumber = new float(temp);
		vectorNumbers.push_back(mChunksNumber);
		mpLog->logMessage("mChunksNumber = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mChunksNumber");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mChunksGridArea")
	{
		f >> temp;
		mChunksGridArea = new float(temp);
		vectorNumbers.push_back(mChunksGridArea);
		mpLog->logMessage("mChunksGridArea = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mChunksGridArea");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mBulletHoleSize")
	{
		f >> mBulletHoleSize;
		mpLog->logMessage("mBulletHoleSize = " + Ogre::StringConverter::toString(mBulletHoleSize));
	}
	else
		mpLog->logMessage("Could load settings for mBulletHoleSize");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mAmbientLight")
	{
		f >> temp;
		mAmbientLight = new float(temp);
		vectorNumbers.push_back(mAmbientLight);
		mpLog->logMessage("mAmbientLight = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mAmbientLight");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mNearClipDistance")
	{
		f >> temp;
		mNearClipDistance = new float(temp);
		vectorNumbers.push_back(mNearClipDistance);
		mpLog->logMessage("mNearClipDistance = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mNearClipDistance");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mFarClipDistance")
	{
		f >> temp;
		mFarClipDistance = new float(temp);
		vectorNumbers.push_back(mFarClipDistance);
		mpLog->logMessage("mFarClipDistance = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mFarClipDistance");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mShadowFarDistance")
	{
		f >> temp;
		mShadowFarDistance = new float(temp);
		vectorNumbers.push_back(mShadowFarDistance);
		mpLog->logMessage("mShadowFarDistance = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mShadowFarDistance");	
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mDirectionalLightDirX")
	{
		f >> temp;
		mDirectionalLightDirX = new float(temp);
		vectorNumbers.push_back(mDirectionalLightDirX);
		mpLog->logMessage("mDirectionalLightDirX = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mDirectionalLightDirX");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mDirectionalLightDirY")
	{
		f >> temp;
		mDirectionalLightDirY = new float(temp);
		vectorNumbers.push_back(mDirectionalLightDirY);
		mpLog->logMessage("mDirectionalLightDirY = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mDirectionalLightDirY");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mDirectionalLightDirZ")
	{
		f >> temp;
		mDirectionalLightDirZ = new float(temp);
		vectorNumbers.push_back(mDirectionalLightDirZ);
		mpLog->logMessage("mDirectionalLightDirZ = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mDirectionalLightDirZ");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mFogColor")
	{
		f >> temp;
		mDirectionalLightDirZ = new float(temp);
		vectorNumbers.push_back(mDirectionalLightDirZ);
		mpLog->logMessage("mFogColor = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mFogColor");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mFogStart")
	{
		f >> temp;
		mFogStart = new float(temp);
		vectorNumbers.push_back(mFogStart);
		mpLog->logMessage("mFogStart = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mFogStart");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mFogEnd")
	{
		f >> temp;
		mFogEnd = new float(temp);
		vectorNumbers.push_back(mFogEnd);
		mpLog->logMessage("mFogEnd = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mFogEnd");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mSkyBox")
	{
		f >> s;
		mSkyBox = new string(s);
		vectorStrings.push_back(mSkyBox);
		mpLog->logMessage("mSkyBox = " + Ogre::StringConverter::toString(s.c_str()));
	}
	else
		mpLog->logMessage("Could load settings for mSkyBox");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mNPCUpdateTimerFreq")
	{
		f >> mNPCUpdateTimerFreq;
		mpLog->logMessage("mNPCUpdateTimerFreq = " + Ogre::StringConverter::toString(mNPCUpdateTimerFreq));
	}
	else
		mpLog->logMessage("Could load settings for mNPCUpdateTimerFreq");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mNPCAimerRangeLong")
	{
		f >> mNPCAimerRangeLong;
		mpLog->logMessage("mNPCAimerRangeLong = " + Ogre::StringConverter::toString(mNPCAimerRangeLong));
	}
	else
		mpLog->logMessage("Could load settings for mNPCAimerRangeLong");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mNPCAimerRangeLongFreq")
	{
		f >> mNPCAimerRangeLongFreq;
		mpLog->logMessage("mNPCAimerRangeLongFreq = " + Ogre::StringConverter::toString(mNPCAimerRangeLongFreq));
	}
	else
		mpLog->logMessage("Could load settings for mNPCAimerRangeLongFreq");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mNPCAimerRangeShort")
	{
		f >> mNPCAimerRangeShort;
		mpLog->logMessage("mNPCAimerRangeShort = " + Ogre::StringConverter::toString(mNPCAimerRangeShort));
	}
	else
		mpLog->logMessage("Could load settings for mNPCAimerRangeShort");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mNPCAimerRangeShortFreq")
	{
		f >> mNPCAimerRangeShortFreq;
		mpLog->logMessage("mNPCAimerRangeShortFreq = " + Ogre::StringConverter::toString(mNPCAimerRangeShortFreq));
	}
	else
		mpLog->logMessage("Could load settings for mNPCAimerRangeShortFreq");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mNPCAimerRandomness")
	{
		f >> mNPCAimerRandomness;
		mpLog->logMessage("mNPCAimerRandomness = " + Ogre::StringConverter::toString(mNPCAimerRandomness));
	}
	else
		mpLog->logMessage("Could load settings for mNPCAimerRandomness");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mPysicsFrictionStatic")
	{
		f >> temp;
		mPysicsFrictionStatic = new float(temp);
		vectorNumbers.push_back(mPysicsFrictionStatic);
		mpLog->logMessage("mPysicsFrictionStatic = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mPysicsFrictionStatic");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mPysicsFrictionDynamic")
	{
		f >> temp;
		mPysicsFrictionDynamic = new float(temp);
		vectorNumbers.push_back(mPysicsFrictionDynamic);
		mpLog->logMessage("mPysicsFrictionDynamic = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mPysicsFrictionDynamic");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mPysicsRestitution")
	{
		f >> temp;
		mPysicsRestitution = new float(temp);
		vectorNumbers.push_back(mPysicsRestitution);
		mpLog->logMessage("mPysicsRestitution = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mPysicsRestitution");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mPlayerSpawnPointX")
	{
		f >> temp;
		mPlayerSpawnPointX = new float(temp);
		vectorNumbers.push_back(mPlayerSpawnPointX);
		mpLog->logMessage("mPlayerSpawnPointX = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mPlayerSpawnPointX");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mPlayerSpawnPointY")
	{
		f >> temp;
		mPlayerSpawnPointY = new float(temp);
		vectorNumbers.push_back(mPlayerSpawnPointY);
		mpLog->logMessage("mPlayerSpawnPointY = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mPlayerSpawnPointY");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mPlayerSpawnPointZ")
	{
		f >> temp;
		mPlayerSpawnPointZ = new float(temp);
		vectorNumbers.push_back(mPlayerSpawnPointZ);
		mpLog->logMessage("mPlayerSpawnPointZ = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mPlayerSpawnPointZ");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mPlayerParticleMoveBoxOffsetX")
	{
		f >> temp;
		mPlayerParticleMoveBoxOffsetX = new float(temp);
		vectorNumbers.push_back(mPlayerParticleMoveBoxOffsetX);
		mpLog->logMessage("mPlayerParticleMoveBoxOffsetX = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mPlayerParticleMoveBoxOffsetX");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mPlayerParticleMoveBoxOffsetY")
	{
		f >> temp;
		mPlayerParticleMoveBoxOffsetY = new float(temp);
		vectorNumbers.push_back(mPlayerParticleMoveBoxOffsetY);
		mpLog->logMessage("mPlayerParticleMoveBoxOffsetY = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mPlayerParticleMoveBoxOffsetY");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mPlayerParticleMoveBoxOffsetZ")
	{
		f >> temp;
		mPlayerParticleMoveBoxOffsetZ = new float(temp);
		vectorNumbers.push_back(mPlayerParticleMoveBoxOffsetZ);
		mpLog->logMessage("mPlayerParticleMoveBoxOffsetZ = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mPlayerParticleMoveBoxOffsetZ");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mPlayerParticleRocketBootsOffsetX")
	{
		f >> temp;
		mPlayerParticleRocketBootsOffsetX = new float(temp);
		vectorNumbers.push_back(mPlayerParticleRocketBootsOffsetX);
		mpLog->logMessage("mPlayerParticleRocketBootsOffsetX = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mPlayerParticleRocketBootsOffsetX");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mPlayerParticleRocketBootsOffsetY")
	{
		f >> temp;
		mPlayerParticleRocketBootsOffsetY = new float(temp);
		vectorNumbers.push_back(mPlayerParticleRocketBootsOffsetY);
		mpLog->logMessage("mPlayerParticleRocketBootsOffsetY = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mPlayerParticleRocketBootsOffsetY");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mPlayerParticleRocketBootsOffsetZ")
	{
		f >> temp;
		mPlayerParticleRocketBootsOffsetZ = new float(temp);
		vectorNumbers.push_back(mPlayerParticleRocketBootsOffsetZ);
		mpLog->logMessage("mPlayerParticleRocketBootsOffsetZ = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mPlayerParticleRocketBootsOffsetZ");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mPlayerMaxFallSpeedBeforeDie")
	{
		f >> mPlayerMaxFallSpeedBeforeDie;
		mpLog->logMessage("mPlayerMaxFallSpeedBeforeDie = " + Ogre::StringConverter::toString(mPlayerMaxFallSpeedBeforeDie));
	}
	else
		mpLog->logMessage("Could load settings for mPlayerMaxFallSpeedBeforeDie");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mPlayerRocketBootsUpdateFreq")
	{
		f >> mPlayerRocketBootsUpdateFreq;
		mpLog->logMessage("mPlayerRocketBootsUpdateFreq = " + Ogre::StringConverter::toString(mPlayerRocketBootsUpdateFreq));
	}
	else
		mpLog->logMessage("Could load settings for mPlayerRocketBootsUpdateFreq");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mPlayerRocketBootsCastingBarUpFreq")
	{
		f >> mPlayerRocketBootsCastingBarUpFreq;
		mpLog->logMessage("mPlayerRocketBootsCastingBarUpFreq = " + Ogre::StringConverter::toString(mPlayerRocketBootsCastingBarUpFreq));
	}
	else
		mpLog->logMessage("Could load settings for mPlayerRocketBootsCastingBarUpFreq");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mPlayerUpdateBonesFreq")
	{
		f >> mPlayerUpdateBonesFreq;
		mpLog->logMessage("mPlayerUpdateBonesFreq = " + Ogre::StringConverter::toString(mPlayerUpdateBonesFreq));
	}
	else
		mpLog->logMessage("Could load settings for mPlayerUpdateBonesFreq");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mPlayerRocketBootsForceY")
	{
		f >> mPlayerRocketBootsForceY;
		mpLog->logMessage("mPlayerRocketBootsForceY = " + Ogre::StringConverter::toString(mPlayerRocketBootsForceY));
	}
	else
		mpLog->logMessage("Could load settings for mPlayerRocketBootsForceY");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mPlayerContiniousShootTimer")
	{
		f >> mPlayerContiniousShootTimer;
		mpLog->logMessage("mPlayerContiniousShootTimer = " + Ogre::StringConverter::toString(mPlayerContiniousShootTimer));
	}
	else
		mpLog->logMessage("Could load settings for mPlayerContiniousShootTimer");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mPlayerMoveBoxDuration")
	{
		f >> mPlayerMoveBoxDuration;
		mpLog->logMessage("mPlayerMoveBoxDuration = " + Ogre::StringConverter::toString(mPlayerMoveBoxDuration));
	}
	else
		mpLog->logMessage("Could load settings for mPlayerMoveBoxDuration");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mPlayerRocketBootsDuration")
	{
		f >> mPlayerRocketBootsDuration;
		mpLog->logMessage("mPlayerRocketBootsDuration = " + Ogre::StringConverter::toString(mPlayerRocketBootsDuration));
	}
	else
		mpLog->logMessage("Could load settings for mPlayerRocketBootsDuration");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mPlayerSuperSpeedDuration")
	{
		f >> mPlayerSuperSpeedDuration;
		mpLog->logMessage("mPlayerSuperSpeedDuration = " + Ogre::StringConverter::toString(mPlayerSuperSpeedDuration));
	}
	else
		mpLog->logMessage("Could load settings for mPlayerSuperSpeedDuration");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mPlayerSuperSpeed")
	{
		f >> mPlayerSuperSpeed;
		mpLog->logMessage("mPlayerSuperSpeed = " + Ogre::StringConverter::toString(mPlayerSuperSpeed));
	}
	else
		mpLog->logMessage("Could load settings for mPlayerSuperSpeed");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCamZoom")
	{
		f >> mCamZoom;
		mpLog->logMessage("mCamZoom = " + Ogre::StringConverter::toString(mCamZoom));
	}
	else
		mpLog->logMessage("Could load settings for mCamZoom");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCamRotate")
	{
		f >> mCamRotate;
		mpLog->logMessage("mCamRotate = " + Ogre::StringConverter::toString(mCamRotate));
	}
	else
		mpLog->logMessage("Could load settings for mCamRotate");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCamVelocityY")
	{
		f >> mCamVelocityY;
		mpLog->logMessage("mCamVelocityY = " + Ogre::StringConverter::toString(mCamVelocityY));
	}
	else
		mpLog->logMessage("Could load settings for mCamVelocityY");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCamVelocityXZ")
	{
		f >> mCamVelocityXZ;
		mpLog->logMessage("mCamVelocityXZ = " + Ogre::StringConverter::toString(mCamVelocityXZ));
	}
	else
		mpLog->logMessage("Could load settings for mCamVelocityXZ");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCamHeightMaxY")
	{
		f >> mCamHeightMaxY;
		mpLog->logMessage("mCamHeightMaxY = " + Ogre::StringConverter::toString(mCamHeightMaxY));
	}
	else
		mpLog->logMessage("Could load settings for mCamHeightMaxY");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCamHeightMinY")
	{
		f >> mCamHeightMinY;
		mpLog->logMessage("mCamHeightMinY = " + Ogre::StringConverter::toString(mCamHeightMinY));
	}
	else
		mpLog->logMessage("Could load settings for mCamHeightMinY");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCamDistanceMax")
	{
		f >> mCamDistanceMax;
		mpLog->logMessage("mCamDistanceMax = " + Ogre::StringConverter::toString(mCamDistanceMax));
	}
	else
		mpLog->logMessage("Could load settings for mCamDistanceMax");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCamDistanceMin")
	{
		f >> mCamDistanceMin;
		mpLog->logMessage("mCamDistanceMin = " + Ogre::StringConverter::toString(mCamDistanceMin));
	}
	else
		mpLog->logMessage("Could load settings for mCamDistanceMin");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCamCharYAdjust")
	{
		f >> mCamCharYAdjust;
		mpLog->logMessage("mCamCharYAdjust = " + Ogre::StringConverter::toString(mCamCharYAdjust));
	}
	else
		mpLog->logMessage("Could load settings for mCamCharYAdjust");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCamGunTrackerOffsetX")
	{
		f >> mCamGunTrackerOffsetX;
		mpLog->logMessage("mCamGunTrackerOffsetX = " + Ogre::StringConverter::toString(mCamGunTrackerOffsetX));
	}
	else
		mpLog->logMessage("Could load settings for mCamGunTrackerOffsetX");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCamGunTrackerOffsetY")
	{
		f >> mCamGunTrackerOffsetY;
		mpLog->logMessage("mCamGunTrackerOffsetY = " + Ogre::StringConverter::toString(mCamGunTrackerOffsetY));
	}
	else
		mpLog->logMessage("Could load settings for mCamGunTrackerOffsetY");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCamGunTrackerOffsetZ")
	{
		f >> mCamGunTrackerOffsetZ;
		mpLog->logMessage("mCamGunTrackerOffsetZ = " + Ogre::StringConverter::toString(mCamGunTrackerOffsetZ));
	}
	else
		mpLog->logMessage("Could load settings for mCamGunTrackerOffsetZ");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCamMixHelperHeight")
	{
		f >> temp;
		mCamMixHelperHeight = new float(temp);
		vectorNumbers.push_back(mCamMixHelperHeight);
		mpLog->logMessage("mCamMixHelperHeight = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mCamMixHelperHeight");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCamThirdDistance")
	{
		f >> mCamThirdDistance;
		mpLog->logMessage("mCamThirdDistance = " + Ogre::StringConverter::toString(mCamThirdDistance));
	}
	else
		mpLog->logMessage("Could load settings for mCamThirdDistance");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCamThirdHeight")
	{
		f >> mCamThirdHeight;
		mpLog->logMessage("mCamThirdHeight = " + Ogre::StringConverter::toString(mCamThirdHeight));
	}
	else
		mpLog->logMessage("Could load settings for mCamThirdHeight");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCamMass")
	{
		f >> temp;
		mCamMass = new float(temp);
		vectorNumbers.push_back(mCamMass);
		mpLog->logMessage("mCamMass = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mCamMass");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCamLinearDamping")
	{
		f >> temp;
		mCamLinearDamping = new float(temp);
		vectorNumbers.push_back(mCamLinearDamping);
		mpLog->logMessage("mCamLinearDamping = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mCamLinearDamping");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCamAngularDamping")
	{
		f >> temp;
		mCamAngularDamping = new float(temp);
		vectorNumbers.push_back(mCamAngularDamping);
		mpLog->logMessage("mCamAngularDamping = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mCamAngularDamping");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCamRadius")
	{
		f >> temp;
		mCamRadius = new float(temp);
		vectorNumbers.push_back(mCamRadius);
		mpLog->logMessage("mCamRadius = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mCamRadius");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCamStartPosX")
	{
		f >> mCamStartPosX;
		mpLog->logMessage("mCamStartPosX = " + Ogre::StringConverter::toString(mCamStartPosX));
	}
	else
		mpLog->logMessage("Could load settings for mCamStartPosX");
	/*-----------------------------------------------------------------------------------------------*/		
	if (f >> s && s=="mCamStartPosY")
	{
		f >> mCamStartPosY;
		mpLog->logMessage("mCamStartPosY = " + Ogre::StringConverter::toString(mCamStartPosY));
	}
	else
		mpLog->logMessage("Could load settings for mCamStartPosY");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCamStartPosZ")
	{
		f >> mCamStartPosZ;
		mpLog->logMessage("mCamStartPosZ = " + Ogre::StringConverter::toString(mCamStartPosZ));
	}
	else
		mpLog->logMessage("Could load settings for mCamStartPosZ");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCharSpeedForward")
	{
		f >> mCharSpeedForward;
		mpLog->logMessage("mCharSpeedForward = " + Ogre::StringConverter::toString(mCharSpeedForward));
	}
	else
		mpLog->logMessage("Could load settings for mCharSpeedForward");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCharSpeedBackward")
	{
		f >> mCharSpeedBackward;
		mpLog->logMessage("mCharSpeedBackward = " + Ogre::StringConverter::toString(mCharSpeedBackward));
	}
	else
		mpLog->logMessage("Could load settings for mCharSpeedBackward");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCharSpeedStrafeLeft")
	{
		f >> mCharSpeedStrafeLeft;
		mpLog->logMessage("mCharSpeedStrafeLeft = " + Ogre::StringConverter::toString(mCharSpeedStrafeLeft));
	}
	else
		mpLog->logMessage("Could load settings for mCharSpeedStrafeLeft");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCharSpeedStrafeRight")
	{
		f >> mCharSpeedStrafeRight;
		mpLog->logMessage("mCharSpeedStrafeRight = " + Ogre::StringConverter::toString(mCharSpeedStrafeRight));
	}
	else
		mpLog->logMessage("Could load settings for mCharSpeedStrafeRight");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCharScale")
	{
		f >> temp;
		mCharScale = new float(temp);
		vectorNumbers.push_back(mCharScale);
		mpLog->logMessage("mCharScale = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mCharScale");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCharRotation")
	{
		f >> temp;
		mCharRotation = new float(temp);
		vectorNumbers.push_back(mCharRotation);
		mpLog->logMessage("mCharRotation = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mCharRotation");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCharDieAngularDamping")
	{
		f >> mCharDieAngularDamping;
		mpLog->logMessage("mCharDieAngularDamping = " + Ogre::StringConverter::toString(mCharDieAngularDamping));
	}
	else
		mpLog->logMessage("Could load settings for mCharDieAngularDamping");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCharRespawnTime")
	{
		f >> mCharRespawnTime;
		mpLog->logMessage("mCharRespawnTime = " + Ogre::StringConverter::toString(mCharRespawnTime));
	}
	else
		mpLog->logMessage("Could load settings for mCharRespawnTime");
	/*-----------------------------------------------------------------------------------------------*/
	float x,y,z;

	if (f >> s && s=="mCharFaceDirectionX")
	{
		f >> x;
		mpLog->logMessage("mCharFaceDirectionX = " + Ogre::StringConverter::toString(x));
	}
	else
		mpLog->logMessage("Could load settings for mCharFaceDirectionX");
	if (f >> s && s=="mCharFaceDirectionY")
	{
		f >> y;
		mpLog->logMessage("mCharFaceDirectionY = " + Ogre::StringConverter::toString(y));
	}
	else
		mpLog->logMessage("Could load settings for mCharFaceDirectionY");
	if (f >> s && s=="mCharFaceDirectionZ")
	{
		f >> z;
		mpLog->logMessage("mCharFaceDirectionZ = " + Ogre::StringConverter::toString(z));
	}
	else
		mpLog->logMessage("Could load settings for mCharFaceDirectionZ");

	mCharFaceDirection = Ogre::Vector3(x,y,z);
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCharCapsuleMass")
	{
		f >> temp;
		mCharCapsuleMass = new float(temp);
		vectorNumbers.push_back(mCharCapsuleMass);
		mpLog->logMessage("mCharCapsuleMass = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mCharCapsuleMass");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCharSphereMass")
	{
		f >> temp;
		mCharSphereMass = new float(temp);
		vectorNumbers.push_back(mCharSphereMass);
		mpLog->logMessage("mCharSphereMass = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mCharSphereMass");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCharSphereRadius")
	{
		f >> mCharSphereRadius;
		mpLog->logMessage("mCharSphereRadius = " + Ogre::StringConverter::toString(mCharSphereRadius));
	}
	else
		mpLog->logMessage("Could load settings for mCharSphereRadius");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCharSpherePosY")
	{
		f >> mCharSpherePosY;
		mpLog->logMessage("mCharSpherePosY = " + Ogre::StringConverter::toString(mCharSpherePosY));
	}
	else
		mpLog->logMessage("Could load settings for mCharSpherePosY");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCharCapsuleHeight")
	{
		f >> mCharCapsuleHeight;
		mpLog->logMessage("mCharCapsuleHeight = " + Ogre::StringConverter::toString(mCharCapsuleHeight));
	}
	else
		mpLog->logMessage("Could load settings for mCharCapsuleHeight");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCharCapsuleRadius")
	{
		f >> mCharCapsuleRadius;
		mpLog->logMessage("mCharCapsuleRadius = " + Ogre::StringConverter::toString(mCharCapsuleRadius));
	}
	else
		mpLog->logMessage("Could load settings for mCharCapsuleRadius");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCharJumpForceY")
	{
		f >> mCharJumpForceY;
		mpLog->logMessage("mCharJumpForceY = " + Ogre::StringConverter::toString(mCharJumpForceY));
	}
	else
		mpLog->logMessage("Could load settings for mCharJumpForceY");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mCharFreezeAfterDieTimer")
	{
		f >> mCharFreezeAfterDieTimer;
		mpLog->logMessage("mCharFreezeAfterDieTimer = " + Ogre::StringConverter::toString(mCharFreezeAfterDieTimer));
	}
	else
		mpLog->logMessage("Could load settings for mCharFreezeAfterDieTimer");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mTerrainSize")
	{
		f >> temp;
		mTerrainSize = new float(temp);
		vectorNumbers.push_back(mTerrainSize);
		mpLog->logMessage("mTerrainSize = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mTerrainSize");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mTerrainWorldSize")
	{
		f >> mTerrainWorldSize;
		mpLog->logMessage("mTerrainWorldSize = " + Ogre::StringConverter::toString(mTerrainWorldSize));
	}
	else
		mpLog->logMessage("Could load settings for mTerrainWorldSize");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mTerrainInputScale")
	{
		f >> temp;
		mTerrainInputScale = new float(temp);
		vectorNumbers.push_back(mTerrainInputScale);
		mpLog->logMessage("mTerrainInputScale = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mTerrainInputScale");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mTerrainMinBatchSize")
	{
		f >> temp;
		mTerrainMinBatchSize = new float(temp);
		vectorNumbers.push_back(mTerrainMinBatchSize);
		mpLog->logMessage("mTerrainMinBatchSize = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mTerrainMinBatchSize");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mTerrainMaxBatchSize")
	{
		f >> temp;
		mTerrainMaxBatchSize = new float(temp);
		vectorNumbers.push_back(mTerrainMaxBatchSize);
		mpLog->logMessage("mTerrainMaxBatchSize = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mTerrainMaxBatchSize");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mTerrainMaxPixelError")
	{
		f >> temp;
		mTerrainMaxPixelError = new float(temp);
		vectorNumbers.push_back(mTerrainMaxPixelError);
		mpLog->logMessage("mTerrainMaxPixelError = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mTerrainMaxPixelError");
	/*-----------------------------------------------------------------------------------------------*/	
	if (f >> s && s=="mTerrainCompositeMapDistance")
	{
		f >> temp;
		mTerrainCompositeMapDistance = new float(temp);
		vectorNumbers.push_back(mTerrainCompositeMapDistance);
		mpLog->logMessage("mTerrainCompositeMapDistance = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mTerrainCompositeMapDistance");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mTerrainDefaultAnisotropy")
	{
		f >> temp;
		mTerrainDefaultAnisotropy = new float(temp);
		vectorNumbers.push_back(mTerrainDefaultAnisotropy);
		mpLog->logMessage("mTerrainDefaultAnisotropy = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mTerrainDefaultAnisotropy");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mTerrainAdjustY")
	{
		f >> temp;
		mTerrainAdjustY = new float(temp);
		vectorNumbers.push_back(mTerrainAdjustY);
		mpLog->logMessage("mTerrainAdjustY = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mTerrainAdjustY");
	/*-----------------------------------------------------------------------------------------------*/
	if (f >> s && s=="mTerrainTextureWorldSize")
	{
		f >> temp;
		mTerrainTextureWorldSize = new float(temp);
		vectorNumbers.push_back(mTerrainTextureWorldSize);
		mpLog->logMessage("mTerrainTextureWorldSize = " + Ogre::StringConverter::toString(temp));
	}
	else
		mpLog->logMessage("Could load settings for mTerrainTextureWorldSize");
	/*-----------------------------------------------------------------------------------------------*/
	mpLog->logMessage("Settings loaded from settings.txt!");
}

void Settings::deletePointers()
{
	for (unsigned int i=0;i<vectorNumbers.size();i++)
		delete vectorNumbers[i];

	for (unsigned int i=0;i<vectorStrings.size();i++)
		delete vectorStrings[i];
}