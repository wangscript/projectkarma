/*---------------------------------------------------------------------------------*/
/* File: PhysicsManager.h														   */
/* Author: Per Karlsson, perkarlsson89@gmail.com								   */
/*																				   */
/* Description:	PhysicsManager is class that handles all the Physics in the game   */
/* by using the PhysX wrapper NxOgre.			  								   */
/*---------------------------------------------------------------------------------*/

#ifndef PHYSICSMANAGER_H
#define PHYSICSMANAGER_H

#include "GameCommon.h"

class PhysicsManager
{
public:

	/* A struct for a kinematic object that has a vector of positions it moves between. 
	body = The OGRE3DBody(See NxOgre) of the kinematic object.
	speed = How fast the object moves.
	time = The time it will take for the object to travel from one position to another.
	deltaTime = a timer that will be added each frame. Is reseted once the object has moved from once position to another.
	curDir = The temporary direction of movement.
	posQueue = A vector of destinations(Vector3 positions). The objects moves between these.
	*/
	typedef struct
	{
		OGRE3DBody*					body;
		Ogre::Real					speed;
		Ogre::Real					time;
		Ogre::Real					deltaTime;
		NxOgre::Vec3				curDir;
		std::queue<NxOgre::Vec3>	posQueue;
	} struct_kinematicObject;
	
	/* A struct for a kinematic object that moves in a circular path.
	body = The OGRE3DBody(See NxOgre) of the kinematic object.
	radius = Radius of the circular path.
	degree = How fast the object moves (angular velocity * 2 *PI).
	deltaTime = a timer that will be added each frame. Is reseted and restarted every 360 degrees.
	orgin = Position of the orgin. The object moves around this point.
	*/
	typedef struct
	{
		OGRE3DBody*					body;
		Ogre::Real					radius;
		Ogre::Real					degree;
		Ogre::Real					deltaTime;
		NxOgre::Vec3				orgin;
	} struct_kinematicObjectCircle;

	PhysicsManager(Ogre::SceneManager*);
	
	/* Adds a dynamic RigidBody to the OGRE and PhysX scene.
	pos = Start position in the world.
	filename = The filename of the mesh and PhysX skeleton. For example if the mesh file name is Robot.mesh, then filename = "Robot".
	mass = Mass of the object.
	collisionGroup = CollisionGroup. Game::CollisioNGroup_DynamicObjects preferable. */
	void addDynamicRigidBody(const Ogre::Vector3& pos, char* filename, Ogre::Real mass, int collisionGroup);

	/* Adds a kinematic object that moves in a circle (See struct_kinematicObjectCircle).
	angVel = See degree in struct_kinematicObjectCircle.
	orgin = See struct_kinematicObjectCircle.
	radius = See struct_kinematicObjectCircle.
	filename = The filename of the mesh and PhysX skeleton. For example if the mesh file name is Robot.mesh, then filename = "Robot". */
	void addKinematicCircle(const Ogre::Real angVel, const Ogre::Vector3 orgin, const Ogre::Real radius, const char* filename);

	/* Adds a kinematic object that moves between a set of positions (See struct_kinematicObject)
	posList = A vector of positions that the object will travel between.
	speed = The movement speed.
	filename = The filename of the mesh and PhysX skeleton. For example if the mesh file name is Robot.mesh, then filename = "Robot". */
	void addKinematicTriangleMesh(const std::vector<Ogre::Vector3> posList, const Ogre::Real speed,const char* filename);

	/* Adds a static PhysX skeleton to a position in the world.
	filename = The filename of thePhysX skeleton. For example if the mesh file name is Robot.mesh, then filename = "Robot". 
	This will generate Robot.nxs inside this function, so do NOT add the ".nxs" yourself. */
	void addStaticTriangleMesh(const Ogre::Vector3&, const char* filename); 

	/* Destroys the physics world. Needs to be called when a game level quits. */
	void destroyPhysicsWorld();

	/* Retunrs the current Physics Render System */
	OGRE3DRenderSystem* getPhysicsRenderSystem() {return mvpPhysicsRenderSystem;};

	/* Returns the NxOgre scene. */
	NxOgre::Scene* getPhysicsScene(){return mvpPhysicsScene;};

	/* Returns the NxOgre world. */
	NxOgre::World* getPhysicsWorld(){return mvpPhysicsWorld;};

	/* Pauses all physics simulations. Called when pausing the game level (and entering Menu State). */
	void pause();

	/* See pause(). Resumes all simulations. */
	void resume();

	/* Updates all the Physics in the game. Needs to be called every frame in the Game Loop. */
	void update(const double);

private:
	NxOgre::World*									mvpPhysicsWorld;
	NxOgre::Scene*									mvpPhysicsScene;
	NxOgre::TimeController*							mvpPhysicsTimeController;
	OGRE3DRenderSystem*								mvpPhysicsRenderSystem;
	std::vector<struct_kinematicObject>				mvKinematicObjects;
	std::vector<struct_kinematicObjectCircle>		mvKinematicObjectsCircle;
	Ogre::SceneManager*								mvpSceneMgr;
	int												mvEntityCounter	;

	/* Internal function to move all the regular kinematic Objects. */
	void moveKinematicObjects(const double timeSinceLastFrame);

	/* Internal function to move all kinematics that moves in a circular path. */
	void moveKinematicObjectsCircle(const double timeSinceLastFrame);

	/* Adds ".mesh" at the end of "filename". */
	Ogre::String OgreFilename(const char* filename);

	/* Adds nxogre: at the start of the "filename" char array and ".nxs" at the end. */
	Ogre::String PhysxFilename(const char* filename);
};

#endif