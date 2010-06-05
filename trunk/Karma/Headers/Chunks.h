#ifndef CHUNKS_H
#define CHUNKS_H

#include "GameCommon.h"
#include "NPC.h"
#include "NPCHandler.h"
#include <cmath>

class Chunks
{
private:
	static Chunks singleton;
	Chunks();

public:
	static Chunks getSingleton();
	static Chunks* getSingletonPtr();

	struct chunkData
	{
		std::vector<NPC*> staticNPCs;
		std::vector<Effects*> staticEffects;
	};

	void addStaticEffects(Effects* effect,const GridData& d);
	void addStaticNPC(NPC* npc,const GridData& d);
	void createGrid();
	void destroyAll();
	int getChunksWidth(){return mvChunksNumber;};
	void loopCurrentChunksUpdate( const double& timeSinceLastFrame);
	void resetAllNPcs();
	NPC* searchForNPCinChunk(NxActor* actor, bool& headshot);
	void setActiveChunkArea(const GridData& d,bool diagonalMove);
	
private:	
	int mvChunksNumber,mvGridArea;
	chunkData** mvStaticChunks;

	static int left,right,top,bot;

	void loopChunks(const int l=left,const int right=right,const int t=top,const int b=bot,const double& timeSinceLastFrame=0.0,bool resetNPC = false);
	void resetNPCs(int l, int r, int t, int b);
};
#endif