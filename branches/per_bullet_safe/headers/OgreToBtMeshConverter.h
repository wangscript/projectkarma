#ifndef OGRE_TO_BT_MESH_CONVERTER_H
#define OGRE_TO_BT_MESH_CONVERTER_H

#include <Ogre.h>
#include <btBulletCollisionCommon.h>

void registerEntityAsCollider(Ogre::Entity* entity, btCollisionWorld* colWorld);
void registerAllEntitiesAsColliders(Ogre::SceneManager* sceneMgr, btCollisionWorld* colWorld);

#endif
