#ifndef SETTINGS_H
#define SETTINGS_H

#include <fstream>
#include <string>
#include <Ogre.h>


class Settings
{
public:
	Settings(Ogre::Log* l){mpLog = l;};
	~Settings(){};

	Ogre::Log* mpLog;

	float mTemp1;
	float mTemp2;
	float mTemp3;
	float mTemp4;
	float mTemp5;
	float mTemp6;
	
	float mCamZoom;
	float mCamRotate;
	float mCamVelocityY;
	float mCamVelocityXZ;
	float mCamHeightMaxY;
	float mCamHeightMinY;
	float mCamDistanceMax;
	float mCamDistanceMin;

	float mCharMassBoundCapsule;
	float mCharSpeedSuper;
	float mCharSpeedForward;
	float mCharSpeedBackward;
	float mCharSpeedStrafeLeft;
	float mCharSpeedStrafeRight;
	float mCharAdjustPosX;
	float mCharAdjustPosY;
	float mCharAdjustPosZ;
	float mCharScale;
	float mCharRotation;



	void loadSettingsFromFile()
	{
		using namespace std;
		ifstream f;
		f.open("../settings.txt", ios::in);

		if ( !f )
			mpLog->logMessage("Could not open settings.txt!");
		
		string s;
		stringstream ss;
		mpLog->logMessage("\n=======================\n=       SETTINGS       =\n=======================\n");

		if (f >> s && s=="mTemp1")
		{
			f >> mTemp1;
			mpLog->logMessage("mTemp1 = " + Ogre::StringConverter::toString(mTemp1));
		}
		else
			mpLog->logMessage("Could load settings for mTemp1");

		if (f >> s && s=="mTemp2")
		{
			f >> mTemp2;
			mpLog->logMessage("mTemp2 = " + Ogre::StringConverter::toString(mTemp2));
		}
		else
			mpLog->logMessage("Could load settings for mTemp2");

		if (f >> s && s=="mTemp3")
		{
			f >> mTemp3;
			mpLog->logMessage("mTemp3 = " + Ogre::StringConverter::toString(mTemp3));
		}
		else
			mpLog->logMessage("Could load settings for mTemp3");

		if (f >> s && s=="mTemp4")
		{
			f >> mTemp4;
			mpLog->logMessage("mTemp4 = " + Ogre::StringConverter::toString(mTemp4));
		}
		else
			mpLog->logMessage("Could load settings for mTemp4");

		if (f >> s && s=="mTemp5")
		{
			f >> mTemp5;
			mpLog->logMessage("mTemp5 = " + Ogre::StringConverter::toString(mTemp5));
		}
		else
			mpLog->logMessage("Could load settings for mTemp5");

		if (f >> s && s=="mTemp6")
		{
			f >> mTemp6;
			mpLog->logMessage("mTemp6 = " + Ogre::StringConverter::toString(mTemp6));
		}
		else
			mpLog->logMessage("Could load settings for mTemp6");

		if (f >> s && s=="mCamZoom")
		{
			f >> mCamZoom;
			mpLog->logMessage("mCamZoom = " + Ogre::StringConverter::toString(mCamZoom));
		}
		else
			mpLog->logMessage("Could load settings for mCamZoom");

		if (f >> s && s=="mCamRotate")
		{
			f >> mCamRotate;
			mpLog->logMessage("mCamRotate = " + Ogre::StringConverter::toString(mCamRotate));
		}
		else
			mpLog->logMessage("Could load settings for mCamRotate");

		if (f >> s && s=="mCamVelocityY")
		{
			f >> mCamVelocityY;
			mpLog->logMessage("mCamVelocityY = " + Ogre::StringConverter::toString(mCamVelocityY));
		}
		else
			mpLog->logMessage("Could load settings for mCamVelocityY");

		if (f >> s && s=="mCamVelocityXZ")
		{
			f >> mCamVelocityXZ;
			mpLog->logMessage("mCamVelocityXZ = " + Ogre::StringConverter::toString(mCamVelocityXZ));
		}
		else
			mpLog->logMessage("Could load settings for mCamVelocityXZ");
		
		if (f >> s && s=="mCamHeightMaxY")
		{
			f >> mCamHeightMaxY;
			mpLog->logMessage("mCamHeightMaxY = " + Ogre::StringConverter::toString(mCamHeightMaxY));
		}
		else
			mpLog->logMessage("Could load settings for mCamHeightMaxY");

		if (f >> s && s=="mCamHeightMinY")
		{
			f >> mCamHeightMinY;
			mpLog->logMessage("mCamHeightMinY = " + Ogre::StringConverter::toString(mCamHeightMinY));
		}
		else
			mpLog->logMessage("Could load settings for mCamHeightMinY");

		if (f >> s && s=="mCamDistanceMax")
		{
			f >> mCamDistanceMax;
			mpLog->logMessage("mCamDistanceMax = " + Ogre::StringConverter::toString(mCamDistanceMax));
		}
		else
			mpLog->logMessage("Could load settings for mCamDistanceMax");

				if (f >> s && s=="mCamDistanceMin")
		{
			f >> mCamDistanceMin;
			mpLog->logMessage("mCamDistanceMin = " + Ogre::StringConverter::toString(mCamDistanceMin));
		}
		else
			mpLog->logMessage("Could load settings for mCamDistanceMin");

		if (f >> s && s=="mCharMassBoundCapsule")
		{
			f >> mCharMassBoundCapsule;
			mpLog->logMessage("mCharMassBoundCapsule = " + Ogre::StringConverter::toString(mCharMassBoundCapsule));
		}
		else
			mpLog->logMessage("Could load settings for mCharMassBoundCapsule");

		if (f >> s && s=="mCharSpeedSuper")
		{
			f >> mCharSpeedSuper;
			mpLog->logMessage("mCharSpeedSuper = " + Ogre::StringConverter::toString(mCharSpeedSuper));
		}
		else
			mpLog->logMessage("Could load settings for mCharSpeedSuper");

		if (f >> s && s=="mCharSpeedForward")
		{
			f >> mCharSpeedForward;;;
			mpLog->logMessage("mCharSpeedForward = " + Ogre::StringConverter::toString(mCharSpeedForward));
		}
		else
			mpLog->logMessage("Could load settings for mCharSpeedForward");

		if (f >> s && s=="mCharSpeedBackward")
		{
			f >> mCharSpeedBackward;;;
			mpLog->logMessage("mCharSpeedBackward = " + Ogre::StringConverter::toString(mCharSpeedBackward));
		}
		else
			mpLog->logMessage("Could load settings for mCharSpeedBackward");

		if (f >> s && s=="mCharSpeedStrafeLeft")
		{
			f >> mCharSpeedStrafeLeft;
			mpLog->logMessage("mCharSpeedStrafeLeft = " + Ogre::StringConverter::toString(mCharSpeedStrafeLeft));
		}
		else
			mpLog->logMessage("Could load settings for mCharSpeedStrafeLeft");

		if (f >> s && s=="mCharSpeedStrafeRight")
		{
			f >> mCharSpeedStrafeRight;
			mpLog->logMessage("mCharSpeedStrafeRight = " + Ogre::StringConverter::toString(mCharSpeedStrafeRight));
		}
		else
			mpLog->logMessage("Could load settings for mCharSpeedStrafeRight");

		if (f >> s && s=="mCharAdjustPosX")
		{
			f >> mCharAdjustPosX;
			mpLog->logMessage("mCharAdjustPosX = " + Ogre::StringConverter::toString(mCharAdjustPosX));
		}
		else
			mpLog->logMessage("Could load settings for mCharAdjustPosX");

		if (f >> s && s=="mCharAdjustPosY")
		{
			f >> mCharAdjustPosY;
			mpLog->logMessage("mCharAdjustPosY = " + Ogre::StringConverter::toString(mCharAdjustPosY));
		}
		else
			mpLog->logMessage("Could load settings for mCharAdjustPosY");

		if (f >> s && s=="mCharAdjustPosZ")
		{
			f >> mCharAdjustPosZ;
			mpLog->logMessage("mCharAdjustPosZ = " + Ogre::StringConverter::toString(mCharAdjustPosZ));
		}
		else
			mpLog->logMessage("Could load settings for mCharAdjustPosZ");

		if (f >> s && s=="mCharScale")
		{
			f >> mCharScale;
			mpLog->logMessage("mCharScale = " + Ogre::StringConverter::toString(mCharScale));
		}
		else
			mpLog->logMessage("Could load settings for mCharScale");

		if (f >> s && s=="mCharRotation")
		{
			f >> mCharRotation;
			mpLog->logMessage("mCharRotation = " + Ogre::StringConverter::toString(mCharRotation));
		}
		else
			mpLog->logMessage("Could load settings for mCharRotation");

		mpLog->logMessage("Settings loaded from settings.txt!");
	}

};
#endif


