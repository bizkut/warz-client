// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#ifndef NM_PHYSXCONFIGURE_H
#define NM_PHYSXCONFIGURE_H

namespace physx
{
class PxActor;
class PxAggregate;
class PxArticulation;
class PxArticulationJoint;
class PxArticulationLink;
class PxConvexMesh;
class PxConvexMeshDesc;
class PxConvexShapeDesc;
class PxD6Joint;
class PxFixedJoint;
class PxMaterial;
class PxPhysics;
class PxRigidActor;
class PxRigidBody;
class PxRigidDynamic;
class PxScene;
class PxTriangleMesh;
class PxTriangleMeshDesc;
class PxTriangleMeshShapeDesc;

typedef unsigned char PxClientID;

class PxVec3;
class PxTransform;
class PxQuat;
class PxProfileZoneManager;
namespace repx
{
class RepXCollection;
class RepXIdToRepXObjectMap;
}
}

// Typedef for objects that will only get passed as references/pointers
typedef physx::PxActor PhysXActor;
typedef physx::PxScene PhysXScene;
typedef physx::PxPhysics PhysXSDK;
typedef int PhysXTriangleMesh;        // int rather than void as the api uses references to some of these
typedef int PhysXTriangleMeshDesc;
typedef int PhysXTriangleMeshShapeDesc;
typedef int PhysXConvexMesh;
typedef int PhysXConvexMeshDesc;
typedef int PhysXConvexShapeDesc;

#endif // NM_PHYSXCONFIGURE_H

