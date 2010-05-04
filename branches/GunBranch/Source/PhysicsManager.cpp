#include <PhysicsManager.h>

PhysicsManager::PhysicsManager(Ogre::SceneManager* sceneMgr)
{
	mtpSceneMgr = sceneMgr;
	mtEntityCounter = 0;

	// Create the NxOgre physics world
	mtpPhysicsWorld = NxOgre::World::createWorld();

	// Create NxOgre physics scene
	NxOgre::SceneDescription sceneDesc;
	sceneDesc.mGravity = NxOgre::Vec3(0, -9.8f, 0);
	sceneDesc.mName = "GameScene";
	mtpPhysicsScene = mtpPhysicsWorld->createScene(sceneDesc);

	// Set some physical scene values
	mtpPhysicsScene->getMaterial(0)->setStaticFriction(GameFramework::getSingletonPtr()->mpSettings->mTemp1);
	mtpPhysicsScene->getMaterial(0)->setDynamicFriction(GameFramework::getSingletonPtr()->mpSettings->mTemp2);
	mtpPhysicsScene->getMaterial(0)->setRestitution(0);

	//For kinematic objects
	mtpPhysicsScene->getScene()->setDominanceGroupPair(1,2,NxConstraintDominance::NxConstraintDominance(1.0f, 0.0f));

	// Create NxOgre render system
	mtpPhysicsRenderSystem = new OGRE3DRenderSystem(mtpPhysicsScene);

	//Create NxOgre time controller
	mtpPhysicsTimeController = NxOgre::TimeController::getSingleton();

	//Load own resources for NxOgre
	NxOgre::ResourceSystem::getSingleton()->openArchive("nxogre", "file:nxogre");
	mtpPhysicsScene->createSceneGeometry(new NxOgre::PlaneGeometry(0, NxOgre::Vec3(0, 1, 0)), Matrix44_Identity);
}

PhysicsManager::~PhysicsManager()
{
}

void PhysicsManager::addStaticTriangleMesh(const Ogre::Vector3& pos ,char* filename)
{
	char nxOgreFileName[80] = "nxogre:";
	std::strcat(nxOgreFileName,filename);
	std::strcat(nxOgreFileName,".xns");
	std::cout << nxOgreFileName;
	NxOgre::Mesh* triangleMesh = NxOgre::MeshManager::getSingleton()->load(nxOgreFileName);
	NxOgre::TriangleGeometry* triangleGeometry = new NxOgre::TriangleGeometry(triangleMesh);
	mtpPhysicsScene->createSceneGeometry(triangleGeometry, NxOgre::Matrix44(NxOgre::Vec3(pos)));

	//@todo ska bort, bör finnas i dotscene.
	char ogreFileName[80] = "";
	std::strcat(ogreFileName,filename);
	std::strcat(ogreFileName,".mesh");
	Ogre::Entity* triangleEntity = mtpSceneMgr->createEntity("physicsEntity" + Ogre::StringConverter::toString(mtEntityCounter++), ogreFileName);
	Ogre::SceneNode* triangleNode = mtpSceneMgr->getRootSceneNode()->createChildSceneNode();
	triangleNode->attachObject(triangleEntity);
	triangleNode->setPosition(pos);
}

void PhysicsManager::addKinematicTriangleMesh(const std::vector<Ogre::Vector3> posList, const Ogre::Real speed,char* filename)
{
	char nxOgreFileName[80] = "nxogre:";
	std::strcat(nxOgreFileName,filename);
	std::strcat(nxOgreFileName,".xns");

	char ogreFileName[80] = "";
	std::strcat(ogreFileName,filename);
	std::strcat(ogreFileName,".mesh");
	NxOgre::Mesh* triangleMesh = NxOgre::MeshManager::getSingleton()->load(nxOgreFileName);
	NxOgre::TriangleGeometry* triangleGeometry = new NxOgre::TriangleGeometry(triangleMesh);

	struct_kinematicObject kinematicObject;
	NxOgre::RigidBodyDescription description;
	description.mBodyFlags |= NxOgre::Enums::BodyFlags_FreezeRotation; 
	description.mBodyFlags |= NxOgre::Enums::BodyFlags_DisableGravity;

	kinematicObject.body = mtpPhysicsRenderSystem->createBody(triangleGeometry,posList[0],ogreFileName,description);
	kinematicObject.body->getNxActor()->setDominanceGroup(2);
	kinematicObject.speed = speed;
	kinematicObject.deltaTime = 0;
	kinematicObject.time = 0;
	kinematicObject.curDir = NxOgre::Vec3(0,0,0);
	for (unsigned int i=0; i <posList.size(); i++)
	{
		kinematicObject.posQueue.push( NxOgre::Vec3(posList[i]) );
		std::cout << "IMPORT : " << posList[i].x << " " << posList[i].y << " " << posList[i].z <<"\n";	
	}
	mtKinematicObjects.push_back(kinematicObject);

}

void PhysicsManager::addKinematicCircle(const Ogre::Real angVel, const Ogre::Vector3 orgin, const Ogre::Real radius, char* filename)
{
	char nxOgreFileName[80] = "nxogre:";
	std::strcat(nxOgreFileName,filename);
	std::strcat(nxOgreFileName,".xns");

	char ogreFileName[80] = "";
	std::strcat(ogreFileName,filename);
	std::strcat(ogreFileName,".mesh");
	NxOgre::Mesh* triangleMesh = NxOgre::MeshManager::getSingleton()->load(nxOgreFileName);
	NxOgre::TriangleGeometry* triangleGeometry = new NxOgre::TriangleGeometry(triangleMesh);

	PhysicsManager::struct_kinematicObjectCircle kinematicObject;
	NxOgre::RigidBodyDescription description;
	description.mBodyFlags |= NxOgre::Enums::BodyFlags_FreezeRotation; 
	description.mBodyFlags |= NxOgre::Enums::BodyFlags_DisableGravity;

	NxOgre::Vec3 spawnPoint = NxOgre::Vec3(orgin);
	spawnPoint.x += radius;
	kinematicObject.body = mtpPhysicsRenderSystem->createBody(triangleGeometry,spawnPoint, ogreFileName,description);
	kinematicObject.body->getNxActor()->setDominanceGroup(2);
	kinematicObject.radius = radius;
	kinematicObject.degree = angVel * 2 * Ogre::Math::PI;
	kinematicObject.deltaTime = 0;
	kinematicObject.orgin = NxOgre::Vec3(orgin);

	mtKinematicObjectsCircle.push_back(kinematicObject);
}


void PhysicsManager::moveKinematicObjects(const double timeSinceLastFrame)
{
	for (unsigned int i=0; i < mtKinematicObjects.size(); i++)
	{	
		if (mtKinematicObjects[i].deltaTime >= mtKinematicObjects[i].time)
		{
			mtKinematicObjects[i].posQueue.push(mtKinematicObjects[i].posQueue.front());
			mtKinematicObjects[i].posQueue.pop();
			mtKinematicObjects[i].deltaTime = 0;
			mtKinematicObjects[i].curDir = mtKinematicObjects[i].posQueue.front() - mtKinematicObjects[i].body->getGlobalPosition();
			mtKinematicObjects[i].time = mtKinematicObjects[i].curDir.magnitude() / mtKinematicObjects[i].speed;
			mtKinematicObjects[i].curDir /= mtKinematicObjects[i].time;
			mtKinematicObjects[i].body->setLinearVelocity(mtKinematicObjects[i].curDir);

		}
		mtKinematicObjects[i].deltaTime += timeSinceLastFrame;
	}
}

void PhysicsManager::moveKinematicObjectsCircle(const double timeSinceLastFrame)
{
	for (unsigned int i=0; i < mtKinematicObjectsCircle.size(); i++)
	{	
		mtKinematicObjectsCircle[i].deltaTime += timeSinceLastFrame;
		if (mtKinematicObjectsCircle[i].deltaTime * mtKinematicObjectsCircle[i].degree >= 2 * Ogre::Math::PI)
		{
			mtKinematicObjectsCircle[i].deltaTime = 0;
		}
	
		NxOgre::Vec3 curPos =  mtKinematicObjectsCircle[i].body->getGlobalPosition();
		//Tables available
		NxOgre::Real x = mtKinematicObjectsCircle[i].radius * Ogre::Math::Cos(mtKinematicObjectsCircle[i].degree * mtKinematicObjectsCircle[i].deltaTime) + mtKinematicObjectsCircle[i].orgin.x ;
		NxOgre::Real z = mtKinematicObjectsCircle[i].radius * Ogre::Math::Sin(mtKinematicObjectsCircle[i].degree * mtKinematicObjectsCircle[i].deltaTime) + mtKinematicObjectsCircle[i].orgin.z ;
		NxOgre::Vec3 dirPos(x,curPos.y,z);
		mtKinematicObjectsCircle[i].body->setLinearVelocity(dirPos-curPos);
	}
}

void PhysicsManager::update(const double timeSinceLastFrame)
{
	//Steps the physics scene with timeSinceLastFrame seconds.
	moveKinematicObjects(timeSinceLastFrame);
	moveKinematicObjectsCircle(timeSinceLastFrame);
	mtpPhysicsTimeController->advance(timeSinceLastFrame);


}

void PhysicsManager::pause()
{
	mtpPhysicsTimeController->pause();
}

void PhysicsManager::resume()
{
	mtpPhysicsTimeController->resume();
}

void PhysicsManager::destroyPhysicsWorld()
{
	delete		mtpPhysicsRenderSystem;
	mtpPhysicsWorld->destroyWorld();
	mtpPhysicsWorld->destroySingletons();
}
