#ifndef NPC_H
#define NPC_H

#include "Character.h"

class NPC : public Character
{
public:
	NPC(Ogre::SceneManager* sceneMgr,NxOgre::Scene* physScene,OGRE3DRenderSystem* renderSystem, Ogre::String filename, 
		Ogre::String name, Ogre::Vector3 spawnPoint , float scale, float hp , float walkSpeed);
	
	OGRE3DBody* getHitBox(){return mtpHitBox;};
	bool isReseting(){return mtReseting;};
	bool moveReset(const double& timeSinceLastFrame);
	void reset();
	static void setPlayerNode(Ogre::SceneNode* s){playerNode = s;};
	virtual void update(const double& timeSinceLastFrame) = 0;
		
protected:
	GridData mtChunk;
	float mtUpdateTimer;

	static Ogre::SceneNode* playerNode;
	
	void die();
	bool mtReseting;
};
#endif