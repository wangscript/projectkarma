#ifndef PLAYER_H
#define PLAYER_H
#include <Ogre.h>
#include <NxOgreOGRE3D.h>
#include <NxActor.h>
#include <NxScene.h>
#include <NxUserContactReport.h>
#define CHARACTER_SCALE			0.2 //For ogre.mesh 0.2 is good
#define CHARACTER_ROTATION		180 //Turns the character 180 degrees

class Player
{
public:
	Player(Ogre::SceneManager*,OGRE3DRenderSystem*);
	virtual ~Player();
	virtual void changeAnimation(const Ogre::String& name,const double time);
	virtual void setJumping() {jumping=true;};
	virtual bool isJumping() {return jumping;};
	virtual void move(const double );
	

protected:
	Ogre::Entity* charEnt;           
	Ogre::SceneNode* charNode; 
	Ogre::SceneNode* camCollisionNode;
   OGRE3DBody* mCapsule;
   Ogre::AnimationState* m_AnimationState; 
   bool jumping;
	
};
#endif

