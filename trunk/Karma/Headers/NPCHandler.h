#ifndef NPCHANDLER_H
#define NPCHANDLER_H

#include <vector>
#include "NPC.h"

class NPCHandler
{
private: 
	static NPCHandler singleton;
	
	NPCHandler() {};

	std::vector<NPC*> mvNPCsReset;
	std::vector<NPC*> mvNPCsDead;

public:
	static NPCHandler getSingleton();
	static NPCHandler* getSingletonPtr();

	void addDeadNPC(NPC* npc);
	void destroyAll();
	void updateNPCsFromChunk(const std::vector<NPC*>& npcs,const double& timeSinceLastFrame,bool reset=false);
	void updateResetNPCs(const double& timeSinceLastFrame);
	void updateDeadNPCs(const double& timeSinceLastFrame);
};

#endif