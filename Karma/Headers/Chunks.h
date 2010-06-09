/*---------------------------------------------------------------------------------*/
/* File: Chunks.h											   					   */
/* Author: Per Karlsson, perkarlsson89@gmail.com								   */
/*																				   */
/* Description:	Chunks is a class that divides the world into "Chunks", smaller    */
/* pieces. The player will always be in a chunk and only NPCs and effects in an    */
/* active chunk area around that chunk will only be updated each frame.			   */
/*---------------------------------------------------------------------------------*/

#ifndef CHUNKS_H
#define CHUNKS_H

#include "GameCommon.h"
#include "NPC.h"
#include "NPCHandler.h"
#include <cmath>

class Chunks
{
private:
	/* Singleton. */
	static Chunks singleton;
	Chunks();

public:
	/* Get Singleton. */
	static Chunks getSingleton();
	static Chunks* getSingletonPtr();

	/* Each chunk has this struct, a list of NPCs and Effects */
	struct chunkData
	{
		std::vector<NPC*> staticNPCs;
		std::vector<Effects*> staticEffects;
	};

	/* Adds a effect to a chunk. Static here means that it can not change chunk(dynamic)*/
	void addStaticEffects(Effects* effect,const GridData& d);
	
	/* Adds a NPC to a chunk. Static here means that it is bound to a chunk its neighbour area */
	void addStaticNPC(NPC* npc,const GridData& d);
	
	/* Creates the Chunks grid. This needs to be initated early in the createScene of each level. */
	void createGrid();
	
	/*  Removes all the pointers in the vectors of each Chunk. Needs to be called every time a game level quits.*/
	void destroyAll();
	
	/* Returns how many blocks each side has. For example 20 x 20. */
	int getChunksWidth(){return mvChunksNumber;};
	
	/* Updates NPC in active Chunk area. */
	void loopCurrentChunksUpdate( const double& timeSinceLastFrame);
	
	/* Resets all the NPCs. Useful when the Player dies and the NPCs no longer has to search for it. */
	void resetAllNPcs();
	
	/* Compares the actor with the bounding volumes of the characters in the active chunk area.
	headshot is being set to true if the shot hits the bounding sphere of a character.*/
	NPC* searchForNPCinChunk(NxActor* actor, bool& headshot);
	
	/* Set the active chunk area.  diagonalMove is true if the new chunk area is a movement in both X and Y. (x and z in OGRE world)*/
	void setActiveChunkArea(const GridData& d,bool diagonalMove);

private:	
	int mvChunksNumber,mvGridArea;
	chunkData** mvStaticChunks;

	static int left,right,top,bot;

	/* Loops a chunk area and updates the NPCs in it. If resetNPC == true then it resets the NPC in the area instead of updating them. */
	void loopChunks(const int l=left,const int right=right,const int t=top,const int b=bot,const double& timeSinceLastFrame=0.0,bool resetNPC = false);
	
	/* Resets NPCs in a specific chunk area. */
	void resetNPCs(int l, int r, int t, int b);
};
#endif