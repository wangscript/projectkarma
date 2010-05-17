#ifndef AIMER_H
#define AIMER_H

#include "Character.h"
#include "GameCommon.h"
#include "Chunks.h"

class Aimer
{
public:
	Aimer(){};
	Aimer::~Aimer();

	void setInitialOrientation();
	void setManuallyControlled(bool state);
	void updateBones(Ogre::SceneNode* aimNode);

	static void setPhysicsScene(NxOgre::Scene* s){mvpPhysicsScene = s;};

protected:
	Ogre::SceneNode* mtvGunTrackNode;
	
	void addBloodParticleSystem(bool addToChunk = false, GridData d = GridData(0,0));
	void addGun(const Ogre::String& hand1,Ogre::Quaternion rot1,Ogre::Vector3 offset1, const Ogre::String& hand2,Ogre::Quaternion rot2,Ogre::Vector3 offset2);
	void addMuzzleFire(const Ogre::String& hand1,Ogre::Quaternion rot1,Ogre::Vector3 offset1, const Ogre::String& hand2,Ogre::Quaternion rot2,Ogre::Vector3 offset2,bool addToChunk = false, GridData d = GridData(0,0));
	void destroyNodeTracks(Ogre::Animation* animation);
	void fire(const Ogre::Vector3& start, const Ogre::Vector3& dir, int filterGroup);	
	void setBones(Ogre::Entity* charEnt,Ogre::String& armL, Ogre::String& armR,Ogre::String& head);
	void setNodes(Ogre::SceneNode* charNode, Ogre::SceneManager* sceneMgr,const Ogre::String& name = "");

private:		
	Blood						*mvpBlood;
	MuzzleFire					*mvpMuzzleFire1,*mvpMuzzleFire2;
	Ogre::Bone					*mtpBoneArmRight,*mtpBoneArmLeft,*mtpBoneHead;
	Ogre::SceneNode				*mtvCharNode;
	Ogre::Entity				*mvpCharEnt;
	Ogre::SceneManager			*mvpSceneMgr;
	const Ogre::String			*mvpName;

	static NxOgre::Scene*		mvpPhysicsScene;

	void RotateBone(Ogre::Bone* bone, Ogre::SceneNode* aimNode);
};

#endif