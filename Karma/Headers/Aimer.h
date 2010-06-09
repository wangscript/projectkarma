/*---------------------------------------------------------------------------------*/
/* File: Aimer.h																   */
/* Author: Per Karlsson, perkarlsson89@gmail.com								   */
/*																				   */
/* Description:	Aimer is a class that fire weapons and uses raycasting to          */
/* determine if the shot was a hit or not. It also has functions for manually      */
/* rotating bones in a characters skeleton.										   */
/*---------------------------------------------------------------------------------*/

#ifndef AIMER_H
#define AIMER_H

#include "Character.h"
#include "GameCommon.h"
#include "Chunks.h"

class Aimer
{
public:
	Aimer();
	Aimer::~Aimer();

	/* In case the bones were manually rotated, this resets their orientation to the original one */
	void setInitialOrientation();

	/* Making the bones in the characters skeleton to be either manually or automatically controlled.*/
	void setManuallyControlled(bool state);

	/* Continious fire means spraying. The cooldown between each shot is 0.2 seconds. 
	In the future: Instead of 0.2, read from settings.cfg*/
	void toggleContiniousFire(bool state){mtContinuousShoot=state;mtContinuousShootTimer = 0.2;};

	/* Updates the characters bones. This involves some hacks that just make the game works but I can't explain why really. */
	void updateBones(Ogre::SceneNode* aimNode);

	/* All Aimers use the same PhysX scene. */
	static void setPhysicsScene(NxOgre::Scene* s){mvpPhysicsScene = s;};


protected:
	Ogre::SceneNode				*mtvGunTrackNode;
	bool						 mtContinuousShoot;
	double						 mtContinuousShootTimer;
	Ogre::Entity				*mtpGun1Ent,*mtpGun2Ent;
	
	/* Adds a blood particle system from the Effects class. */
	void addBloodParticleSystem(bool addToChunk = false, GridData d = GridData(0,0));

	/* Adds two guns, one in each hand.
	rot = orientation offset.
	offset = position offset. */
	void addGun(const Ogre::String& hand1,Ogre::Quaternion rot1,Ogre::Vector3 offset1, const Ogre::String& hand2,Ogre::Quaternion rot2,Ogre::Vector3 offset2);

	/* Adds Muzzle Fire from the Effects class. For parameters, see addGun */
	void addMuzzleFire(const Ogre::String& hand1,Ogre::Quaternion rot1,Ogre::Vector3 offset1, const Ogre::String& hand2,Ogre::Quaternion rot2,Ogre::Vector3 offset2,bool addToChunk = false, GridData d = GridData(0,0));

	/* Destroys the node tracks for the bones in an animation (Only the bones that were assigned in the setBones) */
	void destroyNodeTracks(Ogre::Animation* animation);

	/* Fires a gun shot from the "start" position in the "dir" direction.
	If the shot is not supposed to hit, for example the NPC collision group, filterGroup= (1 << Game::CollisionGroup_NPC ).
	If the shot hits a character, the character will lose hp. How much HP it loses depends on if it was a normal shot or headshot.*/
	void fire(const Ogre::Vector3& start, const Ogre::Vector3& dir, int filterGroup);	

	/* Pointers to the bones in the characters skeleton. The parameter names should be self-explanatory. */
	void setBones(Ogre::Entity* charEnt,Ogre::String& strUpArmR, Ogre::String& strUpArmL, Ogre::String& strLowArmL, Ogre::String& strLowArmR,Ogre::String& strHead);

	/* Since the aimer class is created before some of the stuff in the Player class, some variables has to be initiated here. */
	void setNodes(Ogre::SceneNode* charNode, Ogre::SceneManager* sceneMgr,const Ogre::String& name = "");

private:		
	Blood						*mvpBlood;
	MuzzleFire					*mvpMuzzleFire1,*mvpMuzzleFire2;
	Ogre::Bone					*mtpBoneArmRight,*mtpBoneArmLeft,*mtpBoneLowArmRight,*mtpBoneLowArmLeft,*mtpBoneHead;
	Ogre::SceneNode				*mtvCharNode;
	Ogre::Entity				*mvpCharEnt;
	Ogre::SceneManager			*mvpSceneMgr;
	const Ogre::String			*mvpName;
	float						*mvpFireDistanceMax,*mvpFireDistanceMin,*mvpShotDmgNormal,*mvpShotDmgHeadshot,*mvpShotForce;
	static NxOgre::Scene*		mvpPhysicsScene;
	
	/* Takes a bone and makes it face the aimNode. The math here works but don't ask me why.
	This is a modified version from the : http://www.ogre3d.org/wiki/index.php/Make_A_Character_Look_At_The_Camera*/
	void RotateBone(Ogre::Bone* bone, Ogre::SceneNode* aimNode);
};

#endif