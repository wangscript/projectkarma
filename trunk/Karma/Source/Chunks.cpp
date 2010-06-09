/*---------------------------------------------------------------------------------*/
/* File: Chunks.cpp											   					   */
/* Author: Per Karlsson, perkarlsson89@gmail.com								   */
/*																				   */
/* Description:	Chunks is a class that divides the world into "Chunks", smaller    */
/* pieces. The player will always be in a chunk and only NPCs and effects in an    */
/* active chunk area around that chunk will only be updated each frame.			   */
/*---------------------------------------------------------------------------------*/

#include "Chunks.h"

/*---------------------------------------------------------------------------------*/
/*									STATIC										   */
/*---------------------------------------------------------------------------------*/
int Chunks::left = 0;
int Chunks::right = 0;
int Chunks::top = 0;
int Chunks::bot = 0;
/*---------------------------------------------------------------------------------*/
Chunks Chunks::singleton;
/*---------------------------------------------------------------------------------*/
Chunks::Chunks(){};
/*---------------------------------------------------------------------------------*/
Chunks Chunks::getSingleton()
{
	return singleton;
}
/*---------------------------------------------------------------------------------*/
Chunks* Chunks::getSingletonPtr()
{
	return &singleton;
}
/*---------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------*/
/*									PUBLIC										   */
/*---------------------------------------------------------------------------------*/
void Chunks::addStaticEffects(Effects *effect,const GridData& gridCoords)
{
	//Adds an effect to the staticEffects Vector.
	mvStaticChunks[gridCoords.x][gridCoords.y].staticEffects.push_back(effect);
}
/*---------------------------------------------------------------------------------*/
void Chunks::addStaticNPC(NPC *npc,const GridData& gridCoords)
{
	//Adds NPC to a chunk.
	mvStaticChunks[gridCoords.x][gridCoords.y].staticNPCs.push_back(npc);
	std::cout << "NPC was added at mvStaticChunks["<< gridCoords.x <<"]["<< gridCoords.y<< "]\n";
}
/*---------------------------------------------------------------------------------*/
void Chunks::createGrid()
{
	//Gridsize
	mvChunksNumber = *Settings::getSingletonPtr()->mChunksNumber;
	//Size of each grid
	mvGridArea = *Settings::getSingletonPtr()->mChunksGridArea;

	//New two dimensionall array
	mvStaticChunks = new chunkData*[mvChunksNumber];
	for( int i = 0 ; i < mvChunksNumber ; i++ )
		mvStaticChunks[i] = new chunkData[mvChunksNumber];
}
/*---------------------------------------------------------------------------------*/
void Chunks::destroyAll()
{
	//Destroy all chunkData
	for( int i = 0 ; i < mvChunksNumber ; i++ )
	{
		for( int j = 0 ; j < mvChunksNumber ; j++ )
		{
			for(std::vector<NPC*>::const_iterator it = mvStaticChunks[i][j].staticNPCs.begin();
				it != mvStaticChunks[i][j].staticNPCs.end(); it++)
				delete (*it);
		}
	}

	//Destroy the pointers
	for( int i = 0 ; i < mvChunksNumber ; i++ )
		delete [] mvStaticChunks[i] ;
	delete [] mvStaticChunks ;
}
/*---------------------------------------------------------------------------------*/
void Chunks::loopCurrentChunksUpdate(const double& timeSinceLastFrame)
{
	//Updates NPC in active Chunk area.
	loopChunks(left,right,top,bot,timeSinceLastFrame,false);
}
/*---------------------------------------------------------------------------------*/
void Chunks::resetAllNPcs()
{
	loopChunks(left,right,top,bot,0.0,true);
}
/*---------------------------------------------------------------------------------*/
NPC* Chunks::searchForNPCinChunk(NxActor* actor,bool& headShot)
{
	//Loops current active chunk area
	int i=left;
	int j=top;
	while (i<=right)
	{
		while (j<=bot)
		{
			for(std::vector<NPC*>::const_iterator it = mvStaticChunks[i][j].staticNPCs.begin();
				it != mvStaticChunks[i][j].staticNPCs.end(); it++)
			{
				//Check if the NxActor is the same as the characters HitBoxes
				//If normal HitBox was hit, headshot = false
				if ((*it)->getHitBox()->getNxActor() == actor )
					return *it;
				//If HitBoxHead was hit, boom Headshot!
				else if ((*it)->getHitBoxHead()->getNxActor() == actor)
				{
					headShot = true;
					return *it;
				}
			}
			j++;
		}
		j=top;
		i++;
	}
	return 0;
}
/*---------------------------------------------------------------------------------*/
void Chunks::setActiveChunkArea(const GridData& d, bool diagonalMove)
{
	//Set new borders to the active Chunk Area

	//Checks if the area is outside the world, i.e Chunk(-1,-1) doesn't exist.
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


	//Check in what the direction the move was
	//If only change in only X or Y
	if (!diagonalMove)
	{
		//Movement in +X
		if (left < newLeft)
		{
			resetNPCs(left,left,top,bot);
			std::cout << "\nMovement +X. ResetNPC from left:" << left << " to right:" << left << " and top:" << top <<" to bot:"<<bot;
		}
		//Movement in -X
		else if (right > newRight)
		{
			resetNPCs(right,right,top,bot);
			resetNPCs(left,left,top,bot);
			std::cout << "\nMovement -X. ResetNPC from left:" << right << " to right:" << right << " and top:" << top <<" to bot:"<<bot;
		}
		//Movement in +Y
		else if (top < newTop)
		{	
			resetNPCs(left,right,top,top);
			std::cout << "\nMovement +Y. ResetNPC from left:" << left << " to right:" << right << " and top:" << top <<" to bot:"<<top;
		}
		//Movement in -Y
		else if (bot < newBot)
		{	
			resetNPCs(left,right,bot,bot);
			std::cout << "\nMovement -Y. ResetNPC from left:" << left << " to right:" << right << " and top:" << bot <<" to bot:"<<bot;
		}
	}
	else
	{
		//Movement in +X & -Y
		if (left < newLeft && bot > newBot)
		{
			std::cout << "\nMovement +X -Y. ResetNPC from left:" << left << " to right:" << left << " and top:" << top <<" to bot:"<<bot<< " and\n"
				<< "left:" << left+1 << " to right:" << right << " and top:" << bot <<" to bot:"<<bot;
			resetNPCs(left,left,top,bot);
			resetNPCs(left+1,right,bot,bot);
		}
		//Movement in -X & -Y
		else if (right > newRight && bot > newBot)
		{
			std::cout << "\nMovement +X -Y. ResetNPC from left:" << right << " to right:" << right << " and top:" << top <<" to bot:"<<bot<< " and\n"
				<< "left:" << left << " to right:" << right-1 << " and top:" << bot <<" to bot:"<<bot;
			resetNPCs(right,right,top,bot);
			resetNPCs(left,right-1,bot,bot);		
		}
		//Movement in +X & +Y
		else if (left < newLeft && top < newTop)
		{
			std::cout << "\nMovement +X -Y. ResetNPC from left:" << left << " to right:" << left << " and top:" << top <<" to bot:"<<bot<< " and\n"
				<< "left:" << left+1 << " to right:" << right << " and top:" << top <<" to bot:"<<top;
			resetNPCs(left,left,top,bot);
			resetNPCs(left+1,right,top,top);
		}
		//Movement in -X & +Y
		else if (right > newRight && top < newTop)
		{
			std::cout << "\nMovement +X -Y. ResetNPC from left:" << right << " to right:" << right << " and top:" << top <<" to bot:"<<bot<< " and\n"
				<< "left:" << left << " to right:" << right-1 << " and top:" << top <<" to bot:"<<top;
			resetNPCs(right,right,top,bot);		
			resetNPCs(left,right-1,top,top);
		}
	}

	//Sets the new border once the NPCs are reseted.
	left = newLeft;
	right = newRight;
	top = newTop;
	bot = newBot;
	std::cout << "\nNew ChunkBorder. L:" << left << " R:" << right << " T:" << top << " B:" << bot;
}

/*---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*/
/*									PRIVATE										   */
/*---------------------------------------------------------------------------------*/
void Chunks::resetNPCs(const int l,const int r,const int t,const int b)
{
	//Add NPCS to the reset vector in NPCHandler
	loopChunks(l,r,t,b,0.0,true);
}
/*---------------------------------------------------------------------------------*/
void Chunks::loopChunks(const int l,const int r,const int t,const int b,const double& timeSinceLastFrame,bool resetNPC)
{
	//Loops active chunk area and 
	//If resetNPC = true, add NPCs to the NPCHander::mvResetNPCs
	//If resetNPC = false(default), update the NPC
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
/*---------------------------------------------------------------------------------*/
