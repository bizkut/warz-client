/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_ICollisionInterface_H
#define Navigation_ICollisionInterface_H

#include "gwnavruntime/base/types.h"
#include "gwnavruntime/kernel/SF_RefCount.h"
#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/collision/collisiontypes.h"

namespace Kaim
{
	class Vec3f;
	class CollisionData;
}

namespace Kaim
{

/// An abstract interface for an object that can perform collision queries.
class ICollisionInterface : public RefCountBaseV<ICollisionInterface, MemStat_WorldFwk>
{
public:
	virtual ~ICollisionInterface() {}

	// The data within collisionData will be added to the world.
	virtual KyResult AddCollisionData(Ptr<CollisionData> collisionData) = 0;

	// The data within collisionData will be removed from the world.
	virtual KyResult RemoveCollisionData(Ptr<CollisionData> collisionData) = 0;

	// Perform any processing that may be necessary.
	virtual KyResult Update() = 0;

	// Perform RayCast from a to b, return CollisionQueryDidNotHit if no hit.
	virtual CollisionRayCastResult RayCast(const Vec3f& a, const Vec3f& b) = 0;
};

} // Kaim

#endif // Navigation_ICollisionInterface_H
