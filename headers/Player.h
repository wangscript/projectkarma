#ifndef PLAYER_H
#define PLAYER_H
#include <btBulletCollisionCommon.h>
#include <Ogre.h>
#include <cmath>

#define BTVEC3_TO_OGREVEC3(v) \
    btVector3(v[0], v[1], v[2])
#define OGREVEC3_TO_BTVEC3(v) \
    Vector3(v[0], v[1], v[2])


using namespace Ogre;

class Player
{
public:
    // movement direction
    enum direction { FORWARD, BACKWARD, TURN_LEFT, TURN_RIGHT };
    
protected:
    //
    // OGRE
    //
    SceneManager* mSceneMgr;
    Vector3 latestValid;
		ManualObject* debugRayObject;
		    SceneNode* debugRayNode;
    Entity* charEnt;             // character entity
    SceneNode* charNode;         // character node
    Entity* contactEnt; 
    SceneNode* contactNode;   
    // last frame begin and frame end event
    FrameEvent evtFrameBegin, evtFrameEnd;
    
    //
    // BULLET
    //
    btCollisionWorld* mColWorld;

    // cylinder shape and object for part3
    btCylinderShape* mCylShape;    
    btCollisionObject* mCylColObj; 
    
    
public:
    Player(SceneManager* sceneMgr, btCollisionWorld* btColWorld);

    virtual ~Player();
    virtual void init();
	virtual bool vardetettvalidmove();
    virtual void move(Vector3 dir);
    virtual void updateFrameBegin(const FrameEvent& evt);
    virtual void updateFrameEnd(const FrameEvent& evt);
};


#endif /*PLAYER_H_*/
