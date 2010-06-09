/*---------------------------------------------------------------------------------*/
/* File: NPC.h																	   */
/* Author: Per Karlsson, perkarlsson89@gmail.com								   */
/*																				   */
/* Description:	NPC is the class between the Character and the derived NPC		   */
/* classes (i.e NPCAimer). It involves some standard functions for reseting		   */
/* the NPC.			  															   */
/*---------------------------------------------------------------------------------*/

#ifndef NPC_H
#define NPC_H

#include "Character.h"

/*  */
class NPC : public Character
{
public:
	NPC(Ogre::SceneManager* sceneMgr,NxOgre::Scene* physScene,OGRE3DRenderSystem* renderSystem, Ogre::String filename, 
		Ogre::String name, Ogre::Vector3 spawnPoint , float scale, float hp , float walkSpeed);
	
	/* Returns true if NPC is currently resetting */
	bool isReseting(){return mtReseting;};
	
	/* Instead of moving normally (often towards the Player), the NPC walks to its spawn point. */
	bool moveReset(const double& timeSinceLastFrame);
	
	/* Resets the character. Used when it is no longer in the active chunk area. */
	void reset();
	
	/* Static function to set the scene node that the player is attached to. */
	static void setPlayerNode(Ogre::SceneNode* s){playerNode = s;};
	
	/* Abstract function. All NPC subclasses need their own way of updating. */
	virtual void update(const double& timeSinceLastFrame) = 0;
		
protected:
	GridData mtChunk;
	float mtUpdateTimer;
	static Ogre::SceneNode* playerNode;
	bool mtReseting;
	
	/* Common method for dying that can be used for any NPC subclass. Is virtual in the superclass and can still be overwritten. */	
	void die();
};
#endif