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
	typedef struct
	{
		std::vector<NPC*> staticNPCs;
		std::vector<Effects*> staticEffects;
		;
	} chunkData;

	static Chunks getSingleton();
	static Chunks* getSingletonPtr();

	void destroyAll();
	void addStaticNPC(NPC* npc,const GridData& d);
	void addTempNPC(NPC *npc);
	void addStaticEffects(Effects* effect,const GridData& d);
	void loopCurrentChunksMove( const double& timeSinceLastFrame);
	void loopCurrentChunksUpdatePosition();
	int getChunksWidth(){return mvChunksNumber;};
	void setActiveChunkArea(const GridData& d,bool diagonalMove);
	void updateTempNpcs(const double& timeSinceLastFrame);
	NPC* Chunks::searchForNPCinChunk(NxActor* actor);
	
private:	
	int mvChunksNumber;
	int mvGridArea;
	chunkData** mvStaticChunks;
	static int left,right,top,bot;

	std::vector<NPC*> mvTempNPCs;
	std::vector<Effects*> mvTempEffects;
	void resetNPCs(int l, int r, int t, int b);
	void loopChunks(const int l=left,const int right=right,const int t=top,const int b=bot,const double& timeSinceLastFrame=0.0,bool resetNPC = false);
	void loopCurrentChunk(void (NPC::*function)(),const int l=left,const int right=right,const int t=top,const int b=bot);
	void loopCurrentChunk(void (NPC::*function)(const double&d),const double& timeSinceLastFrame);
};
#endif