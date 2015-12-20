/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_RayCast3dQuery_H
#define Navigation_RayCast3dQuery_H


#include "gwnavruntime/querysystem/iquery.h"
#include "gwnavruntime/navmesh/identifiers/navtriangleptr.h"
#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/navmesh/worldintegerpos.h"

namespace Kaim
{

class NavFloor;
class Box3f;

/// Enumerates the possible results of a RayCast3dQuery.
enum RayCast3dQueryResult
{
	RAYCAST3D_NOT_INITIALIZED = QUERY_NOT_INITIALIZED, ///< Indicates the query has not yet been initialized. 
	RAYCAST3D_NOT_PROCESSED   = QUERY_NOT_PROCESSED,   ///< Indicates the query has not yet been launched. 

	RAYCAST3D_DONE_NO_HIT,                  ///< Indicates that the point provided is outside the navigable boundaries of the NavMesh.
	RAYCAST3D_DONE_HIT                      ///< Indicates that the query was completed successfully; a triangle was found. 
};

class RayCastFull3dQuery : public IAtomicQuery
{
public:
	static  QueryType GetStaticType() { return TypeRayCastFull3D; }
	virtual QueryType GetType() const { return TypeRayCastFull3D; }

	// ---------------------------------- Public Member Functions ----------------------------------

	RayCastFull3dQuery();
	virtual ~RayCastFull3dQuery() {}

	// ---------------------------------- Functions to set up the query ----------------------------------

	/// Binds the query with the specified Database, clears all the inputs and outputs and sets all
	/// other parameters to their default value (they may be changed by calling the corresponding write accessors.)
	/// \param database    A pointer to the Database on which the query must be performed.
	void BindToDatabase(Database* database);

	/// resets an initialized instance of this class with minimum input requirements.
	/// Clears all the output, and let all other parameters unchanged. They
	/// may be changed by calling the corresponding write accessors.
	/// \param pos         Sets the value of #m_inputPos3f. 
	void Initialize(const Vec3f& startPos, const Vec3f& destPos);

	// ---------------------------------- Query framework functions ----------------------------------

	virtual void Advance(WorkingMemory* workingMemory)
	{
		PerformQuery(workingMemory);
		m_processStatus = QueryDone;
	}
	/// Performs the query.
	/// \param workingMemory    Sandbox memory used to perform queries. If let to KY_NULL,
	///                          the #m_database internal WorkingMemory is used.
	void PerformQuery(WorkingMemory* workingMemory = KY_NULL);

	// ---------------------------------- Accessors ----------------------------------

	RayCast3dQueryResult GetResult() const { return m_result; }

public: // internal
	void SetResult(RayCast3dQueryResult result) { m_result = result; }
	class RayCastData
	{
	public:
		RayCastData(const Vec3f& startPos, const Vec3f& destPos) : m_dir(destPos-startPos), m_dirInv(1.f, 1.f, 1.f)
		{
			m_dirIsNullOnAxis[0] = false;
			m_dirIsNullOnAxis[1] = false;
			m_dirIsNullOnAxis[2] = false;
		}
	public:
		Vec3f m_dir;
		Vec3f m_dirInv;
		CoordBox64 m_coordBoxOfTest;
		bool m_dirIsNullOnAxis[3];
	};

	void CollideAgainsttriangle(const Vec3f& a, const Vec3f& b, const Vec3f& c, KyFloat32& currentCollisionDistance);
	void RayCast3DAgainstNavFloor(NavFloor* navFloor, const RayCastData& rayCastData, const CoordPos64& cellOrigin, KyFloat32& currentCollisionDistance);
	bool RayVersusBox3f(const RayCastData& rayCastData, const Box3f& box);
public:
	Vec3f m_startPos3f;
	Vec3f m_destPos3f;
	Vec3f m_collisionPoint3f;

	/// Updated during processing to store the nearest triangle found to #m_inputPos3f. If no triangle is found,
	/// #m_resultTrianglePtr is set to an invalid NavTrianglePtr. 
	NavTrianglePtr m_resultTrianglePtr;

	RayCast3dQueryResult m_result; ///< Updated during processing to indicate the result of the query. 
};

}



#endif //Navigation_RayCast3dQuery_H

