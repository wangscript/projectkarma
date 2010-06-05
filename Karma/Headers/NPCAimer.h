#ifndef NPCAIMER_H
#define NPCAIMER_H

#include "NPC.h"
#include "Aimer.h"

class NPCAimer : public NPC, public Aimer
{
public:
	NPCAimer(Ogre::SceneManager* sceneMgr,NxOgre::Scene* physScene,OGRE3DRenderSystem* renderSystem, Ogre::String filename, 
		Ogre::String name, Ogre::Vector3 spawnPoint , float scale, float hp , float walkSpeed);
	
	void die();
	void update(const double& timeSinceLastFrame);
	
private:
	float				mvShootFreq,mvShootTimer,mvUpdateTimerFreq,*mvpRangeLong,*mvpRangeShort, *mvpRangeLongFreq, *mvpRangeShortFreq, *mvpRandomFactor;

	Ogre::Vector3		mtDirNPCtoChar;

	void move(const double& timeSinceLastFrame);
	void queueFire();
};
#endif