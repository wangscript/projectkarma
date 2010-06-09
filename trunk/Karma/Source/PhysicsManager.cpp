/*---------------------------------------------------------------------------------*/
/* File: PhysicsManager.cpp														   */
/* Author: Per Karlsson, perkarlsson89@gmail.com								   */
/*																				   */
/* Description:	PhysicsManager is class that handles all the Physics in the game   */
/* by using the PhysX wrapper NxOgre.			  								   */
/*---------------------------------------------------------------------------------*/

#include <PhysicsManager.h>

/*---------------------------------------------------------------------------------*/
/*									PUBLIC										   */
/*---------------------------------------------------------------------------------*/
PhysicsManager::PhysicsManager(Ogre::SceneManager* sceneMgr)
{
	mvpSceneMgr = sceneMgr;
	mvEntityCounter = 0;

	// Create the NxOgre physics world
	mvpPhysicsWorld = NxOgre::World::createWorld();

	// Create NxOgre physics scene
	NxOgre::SceneDescription sceneDesc;
	sceneDesc.mGravity = NxOgre::Vec3(0, -9.8f, 0);
	sceneDesc.mName = "GameScene";
	mvpPhysicsScene = mvpPhysicsWorld->createScene(sceneDesc);

	// Set some physical scene values
	mvpPhysicsScene->getMaterial(0)->setStaticFriction(*Settings::getSingletonPtr()->mPysicsFrictionStatic);
	mvpPhysicsScene->getMaterial(0)->setDynamicFriction(*Settings::getSingletonPtr()->mPysicsFrictionDynamic);
	mvpPhysicsScene->getMaterial(0)->setRestitution(*Settings::getSingletonPtr()->mPysicsRestitution);

	//For kinematic objects
	mvpPhysicsScene->getScene()->setDominanceGroupPair(1,2,NxConstraintDominance::NxConstraintDominance(1.0f, 0.0f));

	// Create NxOgre render system
	mvpPhysicsRenderSystem = new OGRE3DRenderSystem(mvpPhysicsScene);

	//Create NxOgre time controller
	mvpPhysicsTimeController = NxOgre::TimeController::getSingleton();

	//Load resource system for NxOgre
	NxOgre::ResourceSystem::getSingleton()->openArchive("nxogre", "file:nxogre");
}

/*---------------------------------------------------------------------------------*/
void PhysicsManager::addDynamicRigidBody(const Ogre::Vector3& pos, char* filename, Ogre::Real mass, int collisionGroup)
{
	//Load the PhysX skeleton
	NxOgre::Mesh* triangleMesh = NxOgre::MeshManager::getSingleton()->load(PhysxFilename(filename).c_str());
	NxOgre::TriangleGeometry* triangleGeometry = new NxOgre::TriangleGeometry(triangleMesh);

	//Properties for the rigid body.
	NxOgre::RigidBodyDescription description;
	description.mMass = mass;
	
	//Adds the body to both OGRE and PhysX.
	OGRE3DBody* body = 	mvpPhysicsRenderSystem->createBody(triangleGeometry,pos,OgreFilename(filename),description);

	//Set the collision group
	NxShape* const* shapes = body ->getNxActor()->getShapes();
	(*shapes)->setGroup(collisionGroup);

}

/*---------------------------------------------------------------------------------*/
void PhysicsManager::addKinematicCircle(const Ogre::Real angVel, const Ogre::Vector3 orgin, const Ogre::Real radius, const char* filename)
{
	//Load the PhysX skeleton
	NxOgre::Mesh* triangleMesh = NxOgre::MeshManager::getSingleton()->load(PhysxFilename(filename).c_str());
	NxOgre::TriangleGeometry* triangleGeometry = new NxOgre::TriangleGeometry(triangleMesh);

	//Creates a struct_kinematicObjectCircle and raises the flag that disable rotation and gravity.
	PhysicsManager::struct_kinematicObjectCircle kinematicObject;
	NxOgre::RigidBodyDescription description;
	description.mBodyFlags |= NxOgre::Enums::BodyFlags_FreezeRotation; 
	description.mBodyFlags |= NxOgre::Enums::BodyFlags_DisableGravity;

	//Offsets the starting position by moving the radius distance on the X-axis.
	NxOgre::Vec3 spawnPoint = NxOgre::Vec3(orgin);
	spawnPoint.x += radius;

	//Adds the struct_kinematicObjectCircle to both OGRE and PhysX.
	kinematicObject.body = mvpPhysicsRenderSystem->createBody(triangleGeometry,spawnPoint, OgreFilename(filename),description);

	//So that the object won't be pushed away by the character.
	kinematicObject.body->getNxActor()->setDominanceGroup(2);

	//See struct_kinematicObjectCircle definition.
	kinematicObject.radius = radius;
	kinematicObject.degree = angVel * 2 * Ogre::Math::PI;
	kinematicObject.deltaTime = 0;
	kinematicObject.orgin = NxOgre::Vec3(orgin);

	//Finally adds the struct_kinematicObjectCircle to the mvKinematicObjectsCircle vector.
	mvKinematicObjectsCircle.push_back(kinematicObject);
}


/*---------------------------------------------------------------------------------*/
void PhysicsManager::addKinematicTriangleMesh(const std::vector<Ogre::Vector3> posList, const Ogre::Real speed,const char* filename)
{
	//Load the PhysX skeleton
	NxOgre::Mesh* triangleMesh = NxOgre::MeshManager::getSingleton()->load(PhysxFilename(filename).c_str());
	NxOgre::TriangleGeometry* triangleGeometry = new NxOgre::TriangleGeometry(triangleMesh);

	//Creates a struct_kinematicObject and raises the flag that disable rotation and gravity.
	struct_kinematicObject kinematicObject;
	NxOgre::RigidBodyDescription description;
	description.mBodyFlags |= NxOgre::Enums::BodyFlags_FreezeRotation; 
	description.mBodyFlags |= NxOgre::Enums::BodyFlags_DisableGravity;

	//Adds the struct_kinematicObject to both OGRE and PhysX.
	kinematicObject.body = mvpPhysicsRenderSystem->createBody(triangleGeometry,posList[0],OgreFilename(filename),description);

	//So that the object won't be pushed away by the character.
	kinematicObject.body->getNxActor()->setDominanceGroup(2);

	//See struct_kinematicObject definition.
	kinematicObject.speed = speed;
	kinematicObject.deltaTime = 0;
	kinematicObject.time = 0;
	kinematicObject.curDir = NxOgre::Vec3(0,0,0);
	for (unsigned int i=0; i <posList.size(); i++)
	{
		kinematicObject.posQueue.push( NxOgre::Vec3(posList[i]) );
		std::cout << "IMPORT : " << posList[i].x << " " << posList[i].y << " " << posList[i].z <<"\n";	
	}

	//Finally adds the struct_kinematicObject to the mvKinematicObjects vector.
	mvKinematicObjects.push_back(kinematicObject);
}

/*---------------------------------------------------------------------------------*/
void PhysicsManager::addStaticTriangleMesh(const Ogre::Vector3& pos ,const char* filename)
{
	//Loads the PhysX skeleton and creates a static object in the Physics world at "pos".
	NxOgre::Mesh* triangleMesh = NxOgre::MeshManager::getSingleton()->load(PhysxFilename(filename).c_str());
	NxOgre::TriangleGeometry* triangleGeometry = new NxOgre::TriangleGeometry(triangleMesh);
	mvpPhysicsScene->createSceneGeometry(triangleGeometry, NxOgre::Matrix44(NxOgre::Vec3(pos)));
}
/*---------------------------------------------------------------------------------*/
void PhysicsManager::destroyPhysicsWorld()
{
	//Delete all the physics.
	delete		mvpPhysicsRenderSystem;
	mvpPhysicsWorld->destroyWorld();
	mvpPhysicsWorld->destroySingletons();
}
/*---------------------------------------------------------------------------------*/
void PhysicsManager::pause()
{
	//Pauses the PhysX scene.
	mvpPhysicsTimeController->pause();
}

/*---------------------------------------------------------------------------------*/
void PhysicsManager::resume()
{
	//Resumes the PhysX scene.
	mvpPhysicsTimeController->resume();
}
/*---------------------------------------------------------------------------------*/
void PhysicsManager::update(const double timeSinceLastFrame)
{
	//Changes moving directions of the kinematic objects (might not be needed for regular kinematics objects)
	moveKinematicObjects(timeSinceLastFrame);
	moveKinematicObjectsCircle(timeSinceLastFrame);

	//Steps the physics scene with timeSinceLastFrame seconds.
	mvpPhysicsTimeController->advance(timeSinceLastFrame);
}
/*---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*/
/*									PRIVATE										   */
/*---------------------------------------------------------------------------------*/
void PhysicsManager::moveKinematicObjects(const double timeSinceLastFrame)
{
	//Loops trough all regular kinematicObjects
	for (unsigned int i=0; i < mvKinematicObjects.size(); i++)
	{	
		//If deltaTime > time, the object has reached its position in the queue and news a new moving direction.
		if (mvKinematicObjects[i].deltaTime >= mvKinematicObjects[i].time)
		{
			//Adds the current position to the back.
			mvKinematicObjects[i].posQueue.push(mvKinematicObjects[i].posQueue.front());

			//Pop the front.
			mvKinematicObjects[i].posQueue.pop();

			//Reset deltatime
			mvKinematicObjects[i].deltaTime = 0;

			//Calculate the new moving direction.
			mvKinematicObjects[i].curDir = mvKinematicObjects[i].posQueue.front() - mvKinematicObjects[i].body->getGlobalPosition();

			//Calculate how long it will take to reach the new position
			mvKinematicObjects[i].time = mvKinematicObjects[i].curDir.magnitude() / mvKinematicObjects[i].speed;
			mvKinematicObjects[i].curDir /= mvKinematicObjects[i].time;

			//Add velocity in the moving direction
			mvKinematicObjects[i].body->setLinearVelocity(mvKinematicObjects[i].curDir);
		}

		//Add time to deltaTime.
		mvKinematicObjects[i].deltaTime += timeSinceLastFrame;
	}
}

/*---------------------------------------------------------------------------------*/
void PhysicsManager::moveKinematicObjectsCircle(const double timeSinceLastFrame)
{
	//Loop trough all kinematicObjects with a circular moving path
	for (unsigned int i=0; i < mvKinematicObjectsCircle.size(); i++)
	{	
		//Add time to deltaTime.
		mvKinematicObjectsCircle[i].deltaTime += timeSinceLastFrame;

		//If deltaTime * degree > 2 * PI, The object has moved 360 degrees.
		if (mvKinematicObjectsCircle[i].deltaTime * mvKinematicObjectsCircle[i].degree >= 2 * Ogre::Math::PI)
		{
			mvKinematicObjectsCircle[i].deltaTime = 0;
		}

		//Every frame the object needs a new moving direction.
		NxOgre::Vec3 curPos =  mvKinematicObjectsCircle[i].body->getGlobalPosition();
		NxOgre::Real x = mvKinematicObjectsCircle[i].radius * Ogre::Math::Cos(mvKinematicObjectsCircle[i].degree * mvKinematicObjectsCircle[i].deltaTime) + mvKinematicObjectsCircle[i].orgin.x ;
		NxOgre::Real z = mvKinematicObjectsCircle[i].radius * Ogre::Math::Sin(mvKinematicObjectsCircle[i].degree * mvKinematicObjectsCircle[i].deltaTime) + mvKinematicObjectsCircle[i].orgin.z ;
		NxOgre::Vec3 dirPos(x,curPos.y,z);
		mvKinematicObjectsCircle[i].body->setLinearVelocity(dirPos-curPos);
	}
}
/*---------------------------------------------------------------------------------*/
Ogre::String PhysicsManager::PhysxFilename(const char* filename)
{
	//Adds nxogre: at the start of the "filename" char array and ".nxs" at the end.
	char nxOgreFileName[80] = "nxogre:";
	std::strcat(nxOgreFileName,filename);
	std::strcat(nxOgreFileName,".nxs");
	std::cout << "INUTI ÄR DEN = " << nxOgreFileName;
	return Ogre::String(nxOgreFileName);
}

/*---------------------------------------------------------------------------------*/
Ogre::String PhysicsManager::OgreFilename(const char* filename)
{
	//Adds ".mesh" at the end of "filename".
	char ogreFileName[80] = "";
	std::strcat(ogreFileName,filename);
	std::strcat(ogreFileName,".mesh");
	std::cout << "INUTI ÄR DEN = " << ogreFileName;
	return Ogre::String(ogreFileName);
}
/*---------------------------------------------------------------------------------*/