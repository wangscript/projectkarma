#ifndef NPC_H
#define NPC_H

#include "Character.h"

class NPC : public Character
{
public:
	NPC(Ogre::SceneManager* sceneMgr,NxOgre::Scene* physScene,OGRE3DRenderSystem* renderSystem, Ogre::String filename, 
		Ogre::String name, Ogre::Vector3 spawnPoint , float scale, float hp , float walkSpeed);
	void move(const double& timeSinceLastFrame);
	bool moveReset(const double& timeSinceLastFrame);
	static void setPlayerNode(Ogre::SceneNode* s){playerNode = s;};
	OGRE3DBody* getHitBox(){return mtpHitBox;};
	bool isReseting(){return mtReseting;};
	void reset();
protected:
	static Ogre::SceneNode* playerNode;
	GridData mtChunk;
	bool mtReseting;
	void die();
};
#endif