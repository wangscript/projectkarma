#include "NPCHandler.h"

/*---------------------------------------------------------------------------------*/
NPCHandler NPCHandler::singleton;
/*---------------------------------------------------------------------------------*/
NPCHandler NPCHandler::getSingleton()
{
	return singleton;
}
/*---------------------------------------------------------------------------------*/
NPCHandler* NPCHandler::getSingletonPtr()
{
	return &singleton;
}
/*---------------------------------------------------------------------------------*/
void NPCHandler::updateNPCsFromChunk(const std::vector<NPC*>& npcs,const double& timeSinceLastFrame,bool reset)
{
	//This function is called from the Chunks::loopChunks
	//If the "reset" is false, it updates all the NPCs in the vector
	if (!reset)
	{
	for(std::vector<NPC*>::const_iterator it = npcs.begin();
		it != npcs.end(); it++)
		(*it)->update(timeSinceLastFrame);
	}
	//If reset = true, it means that all elements in the vector needs to be added in the mvNPCsReset vector.
	else
	{
		for(std::vector<NPC*>::const_iterator it = npcs.begin();
		it != npcs.end(); it++)
		{
			(*it)->reset();
			mvNPCsReset.push_back(*it);
		}
	}
}
/*---------------------------------------------------------------------------------*/
void NPCHandler::updateResetNPCs(const double& timeSinceLastFrame)
{
	//Updates the NPCs in the vector mvNPCsReset
	std::vector<std::vector<NPC*>::iterator> deleteIt;
	for (std::vector<NPC*>::iterator it = mvNPCsReset.begin(); it != mvNPCsReset.end();it++)
	{
			//If the NPC is reseted(has reached its spawn point), it is queued to be deleted
			if ((*it)->moveReset(timeSinceLastFrame))
			{
				deleteIt.push_back(it);
				std::cout << "\n resetNPCs are queued to be deleted!";
			}
	}

	//Delete queued NPCs.
	for (std::vector<std::vector<NPC*>::iterator>::iterator it = deleteIt.begin(); it!= deleteIt.end(); it++)
	{
		mvNPCsReset.erase(*it);
		std::cout << "\n resetNPCs  were successfully deleted!";
	}
}
/*---------------------------------------------------------------------------------*/
void NPCHandler::addDeadNPC(NPC* npc)
{
	//From NPC::die()
	mvNPCsDead.push_back(npc);
}	
/*---------------------------------------------------------------------------------*/
void NPCHandler::updateDeadNPCs(const double& timeSinceLastFrame)
{
	//Updates the NPCs in the vector mvNPCsDead
	std::vector<std::vector<NPC*>::iterator> deleteIt;
	for (std::vector<NPC*>::iterator it = mvNPCsDead.begin(); it != mvNPCsDead.end();it++)
	{
			//If the NPC is respawned, it is queued to be deleted
			if ((*it)->updateDead(timeSinceLastFrame))
			{
				deleteIt.push_back(it);
				std::cout << "\n deadNPCs are queued to be deleted!";
			}
	}
	//Delete queued NPCs.
	for (std::vector<std::vector<NPC*>::iterator>::iterator it = deleteIt.begin(); it!= deleteIt.end(); it++)
	{
		mvNPCsDead.erase(*it);
		std::cout << "\n deadNPCs were successfully deleted!";
	}
}
/*---------------------------------------------------------------------------------*/
