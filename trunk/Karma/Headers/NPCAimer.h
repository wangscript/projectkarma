#ifndef NPCAIMER_H
#define NPCAIMER_H

#include "NPC.h"
#include "Aimer.h"

class NPCAimer : public NPC, public Aimer
{
public:
	NPCAimer(Ogre::SceneManager* sceneMgr,NxOgre::Scene* physScene,OGRE3DRenderSystem* renderSystem, Ogre::String filename, 
		Ogre::String name, Ogre::Vector3 spawnPoint , float scale, float hp , float walkSpeed);

	void update(const double& timeSinceLastFrame);

private:
	bool	 mvNeedsMoveUpdate;
	double	 mvShootFreq,mvShootTimer;

	void move(const double& timeSinceLastFrame);
	void queueFire(const Ogre::Vector3& dirNPCtoChar);
};
#endif