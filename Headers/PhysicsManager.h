#ifndef PHYSICSMANAGER_H
#define PHYSICSMANAGER_H

#include <NxOgre.h>
#include <NxOgreOGRE3D.h>
#include <NxActor.h>
#include <NxScene.h>
#include <NxUserContactReport.h>
#include "AppState.h"

class PhysicsManager
{
public:

	typedef struct
	{
		OGRE3DBody*					body;
		Ogre::Real					speed;
		Ogre::Real					time;
		Ogre::Real					deltaTime;
		NxOgre::Vec3				curDir;
		std::queue<NxOgre::Vec3>	posQueue;
	} struct_kinematicObject;
	
	typedef struct
	{
		OGRE3DBody*					body;
		Ogre::Real					radius;
		Ogre::Real					degree;
		Ogre::Real					deltaTime;
		NxOgre::Vec3				orgin;
	} struct_kinematicObjectCircle;


	PhysicsManager(Ogre::SceneManager*);
	~PhysicsManager();
	NxOgre::World* getPhysicsWorld(){return mtpPhysicsWorld;};
	NxOgre::Scene* getPhysicsScene(){return mtpPhysicsScene;};
	OGRE3DRenderSystem* getPhysicsRenderSystem() {return mtpPhysicsRenderSystem;};
	void addStaticTriangleMesh(const Ogre::Vector3&, char* filename); 
	void addKinematicTriangleMesh(const std::vector<Ogre::Vector3>, const Ogre::Real speed,char* filename);
	void addKinematicCircle(const Ogre::Real angVel, const Ogre::Vector3 orgin, const Ogre::Real radius, char* filename);
	void update(const double);
	void pause();
	void resume();
	void destroyPhysicsWorld();

protected:
	NxOgre::World*							mtpPhysicsWorld;
	NxOgre::Scene*							mtpPhysicsScene;
	NxOgre::TimeController*					mtpPhysicsTimeController;
	OGRE3DRenderSystem*						mtpPhysicsRenderSystem;
	std::vector<struct_kinematicObject>		mtKinematicObjects;
	std::vector<struct_kinematicObjectCircle>		mtKinematicObjectsCircle;

	void moveKinematicObjects(const double timeSinceLastFrame);
	void moveKinematicObjectsCircle(const double timeSinceLastFrame);

	Ogre::SceneManager*						mtpSceneMgr;
	int										mtEntityCounter	;

};

#endif