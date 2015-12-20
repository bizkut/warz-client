/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JAPA - secondary contact: NOBODY
#ifndef Navigation_RayCastQuery3D_H
#define Navigation_RayCastQuery3D_H

#include "gwnavruntime/querysystem/iquery.h"
#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/collision/collisiontypes.h"

namespace Kaim
{

/// Query that performs a 3D raycast against the physical geometry in a CollisionWorld.
/// As such, it does not represent passability like a RayCanGoQuery but whether there is a collision or not.
/// Provides a simple yes/no result (CollisionRayCastResult).

class CollisionRayCastQuery : public IAtomicQuery
{
public:
	static  QueryType GetStaticType() { return TypeCollisionRayCast; }
	virtual QueryType GetType() const { return TypeCollisionRayCast; }

	virtual void                 BuildQueryBlob(BaseBlobHandler* blobHandler);
	virtual void                 InitFromQueryBlob(World* world, void* blob);
	virtual Ptr<BaseBlobHandler> CreateQueryBlobHandler() { return CreateStaticQueryBlobHandler(); }
	static  Ptr<BaseBlobHandler> CreateStaticQueryBlobHandler();

	// ---------------------------------- Public Member Functions ----------------------------------

	CollisionRayCastQuery() : m_world(KY_NULL), m_result(RayDidNotHit) {}
	virtual ~CollisionRayCastQuery() {}

	/// Associates the query to this world, clears all the inputs and outputs and sets all
	/// other parameters to their default value (they may be changed by calling the corresponding write accessors.)
	/// \param database    A pointer to the Database on which the query must be performed.
	void BindToWorld(World* world);

	/// Sets an  instance of this class with minimum input requirements.
	/// Clears all the input and output, lets all configuration parameters unchanged. They
	/// may be changed by calling the corresponding write accessors.
	/// \param startPos    Sets the value of #m_startPos3f.
	/// \param destPos     Sets the value of #m_destPos3f. 
	void Initialize(const Vec3f& start, const Vec3f& dest);

	// ---------------------------------- Query framework functions ----------------------------------

	virtual void Advance(WorkingMemory* workingMemory);

	/// Performs the query.
	/// \param workingMemory    Sandbox memory used to perform queries.
	void PerformQuery(WorkingMemory* workingMemory = KY_NULL);

	// ---------------------------------- Accessors ----------------------------------

	CollisionRayCastResult  GetResult()                  const { return m_result; }
	const Vec3f&            GetStartPos()                const { return m_start;  }
	const Vec3f&            GetDestPos()                 const { return m_dest;   }

public:

	World*                    m_world;
	Vec3f                     m_start;
	Vec3f                     m_dest;

	CollisionRayCastResult    m_result;
};

}

#endif //Navigation_RayCastQuery3D_H

