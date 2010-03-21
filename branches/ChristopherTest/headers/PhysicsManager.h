

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"


class PhysicsManager
{
	public:
	void initPhysics();
	

	btAlignedObjectArray<btCollisionShape*>		_pCollisionShapes;
	btBroadphaseInterface*						_pOverlappingPairChache;
	btCollisionDispatcher*						_pDispatcher;
	
	
	btVector3									_btVecWorldMin;
	btVector3									_btVecWorldMax;
	btAxisSweep3*								_sweepBP;
	


	private:
};