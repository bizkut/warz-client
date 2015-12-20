/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef KyRuntimeGlue_CollisionInterface_H
#define KyRuntimeGlue_CollisionInterface_H

#include "gwnavruntime/collision/icollisioninterface.h"
#include "gwnavruntime/kernel/SF_Hash.h"

namespace Kaim
{
	class IndexedMesh;
	class CollisionData;
	class World;
	class HeightField;
}

class btCollisionWorld;
class btDispatcher;
class btBroadphaseInterface;
class btCollisionConfiguration;
class CollisionInterfaceDebugDraw;
class btTriangleIndexVertexArray;
class btBvhTriangleMeshShape;
class btCollisionObject;

/// Implementation of ICollisionInterface that uses Bullet.
class CollisionInterface : public Kaim::ICollisionInterface
{
public:
	explicit CollisionInterface(Kaim::World* world = KY_NULL);

	virtual ~CollisionInterface();

	// The data within collisionData will be added to the world.
	virtual KyResult AddCollisionData(Kaim::Ptr<Kaim::CollisionData> collisionData);

	// The data within collisionData will be removed from the world.
	virtual KyResult RemoveCollisionData(Kaim::Ptr<Kaim::CollisionData> collisionData);

	// Perform any processing that may be necessary.
	virtual KyResult Update();

	// Perform RayCast from a to b, return true if no hit.
	virtual Kaim::CollisionRayCastResult RayCast(const Kaim::Vec3f& a, const Kaim::Vec3f& b);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Functions that are not derived.
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// The data referenced by inputMesh will be stored in collisionData in a format understood by the CollisionInterface.
	KyResult ConvertIndexedMeshToCollisionData(const Kaim::IndexedMesh& inputMesh, Kaim::Ptr<Kaim::CollisionData> collisionData);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// The data in the CollisionWorld will be transformed into a heightfield with the given tilesize.
	KyResult SaveToHeightField(Kaim::HeightField& heightField, KyFloat32 tileSize);

private:
	struct StaticMesh
	{
		btTriangleIndexVertexArray* m_indexVertexArray;
		btBvhTriangleMeshShape*     m_bvhTriMeshShape;
		btCollisionObject*          m_collisionObject;
	};

	void Init();

	Kaim::World*                                  m_navigationWorld;
	Kaim::Hash<Kaim::CollisionData*, StaticMesh>        m_staticMeshes;
	btCollisionWorld*                             m_world;
	btDispatcher*                                 m_dispatcher;
	btBroadphaseInterface*                        m_broadphase;
	btCollisionConfiguration*                     m_config;
	CollisionInterfaceDebugDraw*                  m_debugDraw;
};

#endif