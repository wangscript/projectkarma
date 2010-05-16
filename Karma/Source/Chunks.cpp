#include "Chunks.h"
int Chunks::left = 0;
int Chunks::right = 0;
int Chunks::top = 0;
int Chunks::bot = 0;
Chunks Chunks::singleton;


Chunks::Chunks()
{
	mvChunksNumber = 20;
	mvGridArea = 1;

	mvStaticChunks = new chunkData*[mvChunksNumber];
	for( int i = 0 ; i < mvChunksNumber ; i++ )
		mvStaticChunks[i] = new chunkData[mvChunksNumber];
}

Chunks Chunks::getSingleton()
{
	return singleton;
}

Chunks* Chunks::getSingletonPtr()
{
	return &singleton;
}

void Chunks::destroyAll()
{
	for( int i = 0 ; i < mvChunksNumber ; i++ )
	{
		for( int j = 0 ; j < mvChunksNumber ; j++ )
		{
			for(std::vector<NPC*>::const_iterator it = mvStaticChunks[i][j].staticNPCs.begin();
				it != mvStaticChunks[i][j].staticNPCs.end(); it++)
				delete (*it);
		}
	}

	for( int i = 0 ; i < mvChunksNumber ; i++ )
		delete [] mvStaticChunks[i] ;
	delete [] mvStaticChunks ;
}

void Chunks::addStaticNPC(NPC *npc,const GridData& gridCoords)
{
	mvStaticChunks[gridCoords.x][gridCoords.y].staticNPCs.push_back(npc);
	std::cout << "NPC was added at mvStaticChunks["<< gridCoords.x <<"]["<< gridCoords.y<< "]\n";
}

void Chunks::addTempNPC(NPC *npc)
{
	mvTempNPCs.push_back(npc);
}

void Chunks::updateTempNpcs(const double& timeSinceLastFrame)
{
	std::vector<std::vector<NPC*>::iterator> deleteIt;
	for (std::vector<NPC*>::iterator it = mvTempNPCs.begin(); it != mvTempNPCs.end();it++)
	{
			if ((*it)->moveReset(timeSinceLastFrame))
			{
				deleteIt.push_back(it);
				std::cout << "\n tempNPCs are queued to be deleted!";
			}
	}
	for (std::vector<std::vector<NPC*>::iterator>::iterator it = deleteIt.begin(); it!= deleteIt.end(); it++)
	{
		mvTempNPCs.erase(*it);
		std::cout << "\n tempNPCs in Chunks were successfully deleted!";
	}
}

void Chunks::addStaticEffects(Effects *effect,const GridData& gridCoords)
{
	mvStaticChunks[gridCoords.x][gridCoords.y].staticEffects.push_back(effect);
}

void Chunks::loopCurrentChunksMove(const double& timeSinceLastFrame)
{
	loopCurrentChunk(&NPC::move,timeSinceLastFrame);
}

void Chunks::setActiveChunkArea(const GridData& d, bool diagonalMove)
{
	int newLeft,newRight,newTop,newBot;
	if (d.x-mvGridArea < 0)
		newLeft = d.x;
	else
		newLeft = d.x-mvGridArea;

	if (d.x+mvGridArea > mvChunksNumber-1)
		newRight = d.x;
	else
		newRight = d.x + mvGridArea;

	if (d.y-mvGridArea < 0)
		newTop = d.y;
	else
		newTop = d.y-mvGridArea;

	if (d.y+mvGridArea > mvChunksNumber-1)
		newBot = d.y;
	else
		newBot = d.y + mvGridArea;

	if (!diagonalMove)
	{
		if (left < newLeft)//förflyttning + i x-led
		{
			resetNPCs(left,left,top,bot);
			std::cout << "\nMovement +X. ResetNPC from left:" << left << " to right:" << left << " and top:" << top <<" to bot:"<<bot;
		}
		else if (right > newRight)//förflyttning - i x-led
		{
			resetNPCs(right,right,top,bot);
			resetNPCs(left,left,top,bot);
			std::cout << "\nMovement -X. ResetNPC from left:" << right << " to right:" << right << " and top:" << top <<" to bot:"<<bot;
		}
		else if (top < newTop)//förflyttning + i y-led
		{	
			resetNPCs(left,right,top,top);
			std::cout << "\nMovement +Y. ResetNPC from left:" << left << " to right:" << right << " and top:" << top <<" to bot:"<<top;
		}
		else if (bot < newBot)//förflyttning - i y-led
		{	
			resetNPCs(left,right,bot,bot);
			std::cout << "\nMovement -Y. ResetNPC from left:" << left << " to right:" << right << " and top:" << bot <<" to bot:"<<bot;
		}
	}
	else
	{
		if (left < newLeft && bot > newBot)//förflyttning + i x-led och - i y-led
		{
			std::cout << "\nMovement +X -Y. ResetNPC from left:" << left << " to right:" << left << " and top:" << top <<" to bot:"<<bot<< " and\n"
				<< "left:" << left+1 << " to right:" << right << " and top:" << bot <<" to bot:"<<bot;
			resetNPCs(left,left,top,bot);
			resetNPCs(left+1,right,bot,bot);
		}
		else if (right > newRight && bot > newBot)//förflyttning - i x-led och - i y-led
		{
			std::cout << "\nMovement +X -Y. ResetNPC from left:" << right << " to right:" << right << " and top:" << top <<" to bot:"<<bot<< " and\n"
				<< "left:" << left << " to right:" << right-1 << " and top:" << bot <<" to bot:"<<bot;
			resetNPCs(right,right,top,bot);
			resetNPCs(left,right-1,bot,bot);		
		}
		else if (left < newLeft && top < newTop)//förflyttning + i x-led och + i y-led
		{
			std::cout << "\nMovement +X -Y. ResetNPC from left:" << left << " to right:" << left << " and top:" << top <<" to bot:"<<bot<< " and\n"
				<< "left:" << left+1 << " to right:" << right << " and top:" << top <<" to bot:"<<top;
			resetNPCs(left,left,top,bot);
			resetNPCs(left+1,right,top,top);
		}	
		else if (right > newRight && top < newTop)//förflyttning - i x-led och + i y-led
		{
			std::cout << "\nMovement +X -Y. ResetNPC from left:" << right << " to right:" << right << " and top:" << top <<" to bot:"<<bot<< " and\n"
				<< "left:" << left << " to right:" << right-1 << " and top:" << top <<" to bot:"<<top;
			resetNPCs(right,right,top,bot);		
			resetNPCs(left,right-1,top,top);
		}
	}

	left = newLeft;
	right = newRight;
	top = newTop;
	bot = newBot;
	std::cout << "\nNew ChunkBorder. L:" << left << " R:" << right << " T:" << top << " B:" << bot;
}
void Chunks::loopCurrentChunksUpdatePosition()
{
	loopCurrentChunk(&NPC::updatePosition);
}

void Chunks::resetNPCs(const int l,const int r,const int t,const int b)
{
	loopChunks(l,r,t,b,0.0,true);
}

void Chunks::loopCurrentChunk(void (NPC::*function)(),const int l,const int r,const int t,const int b)
{
	int i=l;
	int j=t;
	while (i<=r)
	{
		while (j<=b)
		{
			for(std::vector<NPC*>::const_iterator it = mvStaticChunks[i][j].staticNPCs.begin();
				it != mvStaticChunks[i][j].staticNPCs.end(); it++)
				((*it)->*function)();
			j++;
		}
		j=t;
		i++;
	}	
}

void Chunks::loopChunks(const int l,const int r,const int t,const int b,const double& timeSinceLastFrame,bool resetNPC)
{
	int i=l;
	int j=t;
	while (i<=r)
	{
		while (j<=b)
		{
			NPCHandler::getSingletonPtr()->updateNPCsFromChunk(mvStaticChunks[i][j].staticNPCs,timeSinceLastFrame,resetNPC);
			j++;
		}
		j=t;
		i++;
	}		
}

void Chunks::loopCurrentChunk(void (NPC::*function)(const double&),const double& timeSinceLastFrame)
{
	int i=left;
	int j=top;
	while (i<=right)
	{
		while (j<=bot)
		{
			NPCHandler::getSingletonPtr()->updateNPCsFromChunk(mvStaticChunks[i][j].staticNPCs,timeSinceLastFrame);
			j++;
		}
		j=top;
		i++;
	}	
}

NPC* Chunks::searchForNPCinChunk(NxActor* actor)
{
	int i=left;
	int j=top;
	while (i<=right)
	{
		while (j<=bot)
		{
			for(std::vector<NPC*>::const_iterator it = mvStaticChunks[i][j].staticNPCs.begin();
				it != mvStaticChunks[i][j].staticNPCs.end(); it++)
			{
				if ((*it)->getHitBox()->getNxActor() == actor)
					return *it;
			}
			j++;
		}
		j=top;
		i++;
	}
	return 0;
}