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
	static int killcount;

public:
	static NPCHandler getSingleton();
	static NPCHandler* getSingletonPtr();
	static void addKill(){++killcount;};
	static int getKills(){return killcount;};
	static void resetKills(){killcount = 0;};

	void addDeadNPC(NPC* npc);
	void destroyAll();
	void updateNPCsFromChunk(const std::vector<NPC*>& npcs,const double& timeSinceLastFrame,bool reset=false);
	void updateResetNPCs(const double& timeSinceLastFrame);
	void updateDeadNPCs(const double& timeSinceLastFrame);
};

#endif