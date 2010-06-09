/*---------------------------------------------------------------------------------*/
/* File: NPCAimer.h																   */
/* Author: Per Karlsson, perkarlsson89@gmail.com								   */
/*																				   */
/* Description:	NPCAimer is a class used for NPCs that can shoot. Inherits from    */
/* both the NPC and Aimer class.			  									   */
/*---------------------------------------------------------------------------------*/

#ifndef NPCAIMER_H
#define NPCAIMER_H

#include "NPC.h"
#include "Aimer.h"

class NPCAimer : public NPC, public Aimer
{
public:
	NPCAimer(Ogre::SceneManager* sceneMgr,NxOgre::Scene* physScene,OGRE3DRenderSystem* renderSystem, Ogre::String filename, 
		Ogre::String name, Ogre::Vector3 spawnPoint , float scale, float hp , float walkSpeed);
	
	/* Updates the NPCs position and orientation. A simple AI schedule is being used here to determine what to do. */
	void update(const double& timeSinceLastFrame);
	
private:
	float				mvShootFreq,mvShootTimer,mvUpdateTimerFreq,*mvpRangeLong,*mvpRangeShort, *mvpRangeLongFreq, *mvpRangeShortFreq, *mvpRandomFactor;
	Ogre::Vector3		mvDirNPCtoChar;
	
	/* See NPC::die(). This function also plays sound files if multiple NPCAimers have been killed.
	3 kills = Killing spree.
	6 kills = Ultra kill.
	9 kills = Godlike.
	*/
	void die();

	/* Moves the NPC by adding velocity to the PhysX actor. Also changes the animation. */
	void move(const double& timeSinceLastFrame);
	
	/* If in range and the shooting cooldown is okay, fire weapon by calling functions from Aimer.*/
	void queueFire();
};
#endif