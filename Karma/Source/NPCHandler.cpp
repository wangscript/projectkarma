#include "NPCHandler.h"

NPCHandler NPCHandler::singleton;

NPCHandler NPCHandler::getSingleton()
{
	return singleton;
}

NPCHandler* NPCHandler::getSingletonPtr()
{
	return &singleton;
}

void NPCHandler::updateNPCsFromChunk(const std::vector<NPC*>& npcs,const double& timeSinceLastFrame,bool reset)
{
	if (!reset)
	{
	for(std::vector<NPC*>::const_iterator it = npcs.begin();
		it != npcs.end(); it++)
		(*it)->move(timeSinceLastFrame);
	}
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

void NPCHandler::updateResetNPCs(const double& timeSinceLastFrame)
{
	std::vector<std::vector<NPC*>::iterator> deleteIt;
	for (std::vector<NPC*>::iterator it = mvNPCsReset.begin(); it != mvNPCsReset.end();it++)
	{
			if ((*it)->moveReset(timeSinceLastFrame))
			{
				deleteIt.push_back(it);
				std::cout << "\n resetNPCs are queued to be deleted!";
			}
	}
	for (std::vector<std::vector<NPC*>::iterator>::iterator it = deleteIt.begin(); it!= deleteIt.end(); it++)
	{
		mvNPCsReset.erase(*it);
		std::cout << "\n resetNPCs  were successfully deleted!";
	}
}

void NPCHandler::addDeadNPC(NPC* npc)
{
	mvNPCsDead.push_back(npc);
}	

void NPCHandler::updateDeadNPCs(const double& timeSinceLastFrame)
{
	std::vector<std::vector<NPC*>::iterator> deleteIt;
	for (std::vector<NPC*>::iterator it = mvNPCsDead.begin(); it != mvNPCsDead.end();it++)
	{
			if ((*it)->updateDead(timeSinceLastFrame))
			{
				deleteIt.push_back(it);
				std::cout << "\n deadNPCs are queued to be deleted!";
			}
	}
	for (std::vector<std::vector<NPC*>::iterator>::iterator it = deleteIt.begin(); it!= deleteIt.end(); it++)
	{
		mvNPCsDead.erase(*it);
		std::cout << "\n deadNPCs were successfully deleted!";
	}
}

