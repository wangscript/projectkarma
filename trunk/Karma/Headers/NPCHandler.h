/*---------------------------------------------------------------------------------*/
/* File: NPCHandler.h															   */
/* Author: Per Karlsson, perkarlsson89@gmail.com								   */
/*																				   */
/* Description:	NPCHandler keeps track of every NPC. This class is being called    */
/* from the Chunks when a certain "chunk" need its NPCs updated. It also has a     */
/* vector for all the temporary resetting NPCs and another one for the dead NPCs.  */
/*---------------------------------------------------------------------------------*/

#ifndef NPCHANDLER_H
#define NPCHANDLER_H

#include <vector>
#include "NPC.h"

class NPCHandler
{
private: 
	std::vector<NPC*> mvNPCsReset;
	std::vector<NPC*> mvNPCsDead;
	static int killcount;
	
	/* Singleton stuff */
	static NPCHandler singleton;
	NPCHandler() {};

public:
	/* Get the singleton. */
	static NPCHandler getSingleton();
	static NPCHandler* getSingletonPtr();

	/* Functions for adding and resetting the kill counter. Used to count how many NPCAimers that has died.*/
	static void addKill(){++killcount;};
	static int getKills(){return killcount;};
	static void resetKills(){killcount = 0;};

	/* Adds a pointer to the dead NPC in the mvNPCsDead. */
	void addDeadNPC(NPC* npc);

	/* Updates the NPCs in the mvNPCsDead vector. If a NPC is ready to be respawned, it is removed from the vector.*/
	void updateDeadNPCs(const double& timeSinceLastFrame);

	/* 	This function is called from the Chunks::loopChunks
	If the "reset" is false, it updates all the NPCs in the vector.
	If reset = true, it means that all elements in the vector needs to be added in the mvNPCsReset vector.*/
	void updateNPCsFromChunk(const std::vector<NPC*>& npcs,const double& timeSinceLastFrame,bool reset=false);
	
	/* Updates the NPCs in the mvNPCsReset vector. If a NPC has reached its spawn point, it is removed from the vector. */
	void updateResetNPCs(const double& timeSinceLastFrame);

};

#endif