/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#include "bulletcollisioninterface.h"
#include "btBulletCollisionCommon.h"

#include "gwnavruntime/kernel/SF_String.h"
#include "gwnavruntime/collision/indexedmesh.h"
#include "gwnavruntime/collision/heightfield.h"
#include "gwnavruntime/kernel/SF_File.h"
#include "gwnavruntime/collision/collisiondata.h"
#include "gwnavruntime/visualsystem/displaylist.h"
#include "gwnavruntime/world/world.h"

#include "internals.h"
#include "bulletblobs.h"
#include "customallocator.h"

//#pragma optimize ("", off)

CollisionInterface::CollisionInterface(Kaim::World* world) : m_navigationWorld(world), 
	m_world(KY_NULL), m_dispatcher(KY_NULL), m_broadphase(KY_NULL), m_config(KY_NULL), m_debugDraw(KY_NULL)
{
	Init();
}

void CollisionInterface::Init()
{
#if 0
	if (Kaim::BlobRegistry::GetDescriptor(btStridingMeshInterfaceBlob::GetBlobTypeId(), false) == KY_NULL)
	{
		CollisionSystemBlobCategoryBuilder basicBlobCategoryBuilder;
		Kaim::BlobRegistry::AddCategory(&basicBlobCategoryBuilder);
	}
#endif

	//btAlignedAllocSetCustom(CustomAllocFunc, CustomFreeFunc);
	//btAlignedAllocSetCustomAligned(CustomAlignedAlloc, CustomAlignedFreeFunc);

	if (m_navigationWorld)
	{
		m_config      = new btDefaultCollisionConfiguration;
		m_dispatcher  = new btCollisionDispatcher(m_config);
		m_broadphase  = new btDbvtBroadphase;

		m_world       = new btCollisionWorld(m_dispatcher, m_broadphase, m_config);

#ifndef KY_BUILD_SHIPPING
		m_debugDraw = new CollisionInterfaceDebugDraw;
		m_world->setDebugDrawer(m_debugDraw);
#endif
	}
}

CollisionInterface::~CollisionInterface()
{
	if (m_debugDraw)
		delete m_debugDraw;
	m_debugDraw = NULL;

	if (m_world)
	{
		for (KyInt32 i = m_world->getNumCollisionObjects()-1; i>=0 ; i--)
		{
			btCollisionObject* obj = m_world->getCollisionObjectArray()[i];
			m_world->removeCollisionObject( obj );
		}
		delete m_world;
	}
	m_world = NULL;

	for (Kaim::Hash<Kaim::CollisionData*, StaticMesh>::Iterator it = m_staticMeshes.Begin(); it != m_staticMeshes.End(); ++it)
	{
		delete it->Second.m_collisionObject;
		delete it->Second.m_bvhTriMeshShape;
		delete it->Second.m_indexVertexArray;
	}
	m_staticMeshes.Clear();

	if(m_broadphase)
		delete m_broadphase;
	m_broadphase = NULL;

	if(m_dispatcher)
		delete m_dispatcher;
	m_dispatcher = NULL;

	if(m_config)
		delete m_config;
	m_config = NULL;
}

KyResult CollisionInterface::AddCollisionData(Kaim::Ptr<Kaim::CollisionData> collisionData)
{
	if (collisionData->m_blobAggregate == KY_NULL)
		return KY_ERROR;

	Kaim::BlobAggregate::Collection<Kaim::IndexedMeshArrayBlob> meshInterfaceBlobs = collisionData->m_blobAggregate->GetCollection<Kaim::IndexedMeshArrayBlob>();

	for (KyUInt32 meshInterfaceBlobIndex = 0; meshInterfaceBlobIndex < meshInterfaceBlobs.GetCount(); ++meshInterfaceBlobIndex)
	{
		const Kaim::IndexedMeshArrayBlob& currentMeshInterface = *meshInterfaceBlobs.GetBlob(meshInterfaceBlobIndex);
		btTriangleIndexVertexArray* indexVertexArray = new btTriangleIndexVertexArray;
		for (KyUInt32 i= 0; i < currentMeshInterface.m_meshParts.GetCount(); ++i)
		{
			const Kaim::IndexedMeshBlob& meshBlob = currentMeshInterface.m_meshParts.GetValues()[i];

			btIndexedMesh mesh;
			mesh.m_triangleIndexStride = meshBlob.GetIndexStride();
			mesh.m_vertexStride = meshBlob.GetVertexStride();
			mesh.m_numTriangles = meshBlob.GetTriangleCount();
			mesh.m_numVertices  = meshBlob.m_vertices.GetCount();
			mesh.m_triangleIndexBase = meshBlob.GetIndexBase();
			mesh.m_vertexBase        = meshBlob.GetVertexBase();
			indexVertexArray->addIndexedMesh(mesh);
		}

		bool useQuantizedAabbCompression = true;
		btBvhTriangleMeshShape* bvhShape = new btBvhTriangleMeshShape(indexVertexArray, useQuantizedAabbCompression);
		bvhShape->setMargin(0.05f);

		btCollisionObject* colObject = new btCollisionObject;
		colObject->setCollisionShape(bvhShape);
		btTransform startTransform;
		startTransform.setIdentity();
		colObject->setWorldTransform(startTransform);
		m_world->addCollisionObject(colObject);

		StaticMesh staticMesh;
		staticMesh.m_indexVertexArray = indexVertexArray;
		staticMesh.m_bvhTriMeshShape = bvhShape;
		staticMesh.m_collisionObject = colObject;

		m_staticMeshes.Add(collisionData, staticMesh);
	}

	return KY_SUCCESS;
}

KyResult CollisionInterface::RemoveCollisionData(Kaim::Ptr<Kaim::CollisionData> collisionData )
{
	Kaim::Hash<Kaim::CollisionData*, StaticMesh>::Iterator it = m_staticMeshes.Find(collisionData);
	if (it.IsEnd() == false)
	{
		m_world->removeCollisionObject(it->Second.m_collisionObject);

		delete it->Second.m_collisionObject;
		delete it->Second.m_bvhTriMeshShape;
		delete it->Second.m_indexVertexArray;

		m_staticMeshes.Remove(collisionData);

		return KY_SUCCESS;
	}
	return KY_ERROR;
}

KyResult CollisionInterface::Update()
{
	if (m_world && m_debugDraw)
	{
		Kaim::ScopedDisplayList displayList(m_navigationWorld, Kaim::DisplayList_Enable);
		displayList.InitSingleFrameLifespan("VisibilityInterface", "Visibility"); 
		m_debugDraw->BeginFrame(&displayList);
		m_world->debugDrawWorld();
		m_debugDraw->EndFrame();
	}

	return KY_SUCCESS;
}

// Perform RayCast from a to b, return true if no hit.
Kaim::CollisionRayCastResult CollisionInterface::RayCast(const Kaim::Vec3f& a, const Kaim::Vec3f& b)
{
	btCollisionWorld::ClosestRayResultCallback result(Vec3fTobtVector(a), Vec3fTobtVector(b));
	m_world->rayTest(result.m_rayFromWorld, result.m_rayToWorld, result);
		
	return result.hasHit() ? Kaim::RayHit : Kaim::RayDidNotHit;
}

KyResult CollisionInterface::ConvertIndexedMeshToCollisionData(const Kaim::IndexedMesh& inputMesh, Kaim::Ptr<Kaim::CollisionData> collisionData)
{
	if (inputMesh.GetTriangleCount() == 0)
		return KY_ERROR;

	if (collisionData == KY_NULL || collisionData->m_blobAggregate == KY_NULL)
		return KY_ERROR;

	btTriangleIndexVertexArray indexVertexArray( 
		inputMesh.GetTriangleCount(), (int*)inputMesh.GetIndicesDataPtr(), inputMesh.GetIndexStride(),
		inputMesh.GetVertexCount(), (KyFloat32*)inputMesh.GetVerticesDataPtr(), inputMesh.GetVertexStride() );

	bool useQuantizedAabbCompression = true;
	btBvhTriangleMeshShape bvhShape(&indexVertexArray, useQuantizedAabbCompression);
	bvhShape.setMargin(0.05f);

	Kaim::Ptr<Kaim::BlobHandler<Kaim::IndexedMeshArrayBlob> > handler = *KY_NEW Kaim::BlobHandler<Kaim::IndexedMeshArrayBlob>;
	btStridingMeshInterfaceBlobBuilder blobBuilder(bvhShape.getMeshInterface());
	blobBuilder.Build(*handler);
	collisionData->m_blobAggregate->AddBlob(handler);

	return KY_SUCCESS;
}

Kaim::Box3f GetAABB(btCollisionWorld* world)
{
	Kaim::Box3f aabb;
	for (KyInt32 i = 0; i < world->getNumCollisionObjects(); ++i)
	{
		btVector3 aabbMin;
		btVector3 aabbMax;
		btCollisionObject* object = world->getCollisionObjectArray()[i];
		btCollisionShape* shape = object->getCollisionShape();
		shape->getAabb(object->getWorldTransform(), aabbMin, aabbMax);
		aabb.ExpandByVec3(btVectorToVec3f(aabbMin));
		aabb.ExpandByVec3(btVectorToVec3f(aabbMax));
	}
	return aabb;
}

KyResult CollisionInterface::SaveToHeightField(Kaim::HeightField& heightField, KyFloat32 tileSize)
{
	if (m_world)
	{
		// Lets get the AABB.
		Kaim::Box3f aabb = GetAABB(m_world);
		
		const KyUInt32 xExtentsInPixels = KyInt32(/*ceilf*/(aabb.SizeX() / tileSize)) + 1;
		const KyUInt32 yExtentsInPixels = KyInt32(/*ceilf*/(aabb.SizeY() / tileSize)) + 1;
		
		Kaim::HeightFieldConfig config;
		config.m_origin = Kaim::Vec2f(aabb.m_min.x, aabb.m_min.y);
		config.m_tileSize = tileSize;
		config.m_xAltitudeCount = xExtentsInPixels;
		config.m_yAltitudeCount = yExtentsInPixels;

		heightField.Initialize(config);

		for (KyUInt32 y = 0; y < yExtentsInPixels; ++y)
		{
			for (KyUInt32 x = 0; x < xExtentsInPixels; ++x)
			{
				// Perform a raycast for each point in the grid
				// use aabb.max.z + delta for start
				// use aabb.min.x - delta for end
				const btVector3 rayStart(x*tileSize + aabb.m_min.x, y*tileSize + aabb.m_min.y, aabb.m_max.z + 1.0f);
				const btVector3 rayEnd(rayStart.x(), rayStart.y(), aabb.m_min.z - 1.0f);
				btCollisionWorld::ClosestRayResultCallback result(rayStart, rayEnd);
				m_world->rayTest(result.m_rayFromWorld, result.m_rayToWorld, result);
				if (result.hasHit() == false)
				{
					btSphereShape sphereShape(0.05f);
					btCollisionWorld::ClosestConvexResultCallback cb(rayStart, rayEnd);
					btMatrix3x3 identity; identity.setIdentity();
					btTransform fromTrans(identity, rayStart);
					btTransform toTrans(identity, rayEnd);
					m_world->convexSweepTest(&sphereShape, fromTrans, toTrans, cb);

					if (cb.hasHit() == false)
					{
						heightField.Clear();
						return KY_ERROR;
					}

					// Set Altitude for this position.
					heightField.SetAltitude(Kaim::PixelPos(x, y), cb.m_hitPointWorld.z());
				}
				else
				{
					// Set Altitude for this position.
					heightField.SetAltitude(Kaim::PixelPos(x, y), result.m_hitPointWorld.z());
				}
			}
		}
		return KY_SUCCESS;
	}
	return KY_ERROR;
}
