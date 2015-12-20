/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_RayCanGoQuery_H
#define Navigation_RayCanGoQuery_H

#include "gwnavruntime/queries/utils/baseraycangoquery.h"
#include "gwnavruntime/database/database.h"
#include "gwnavruntime/querysystem/workingmemcontainers/workingmemarray.h"

namespace Kaim
{

/// The RayCanGoQuery class tests whether or not a ray (with a width of 0) can pass along a straight line segment from a
/// starting point to an ending point  without:
/// -	a collision with the outside border of the NavMesh.
/// -	crossing a forbidden NavTag (a triangle whose NavTag is considered as non-traversable by the traverseLogic).
template<class TraverseLogic>
class RayCanGoQuery : public BaseRayCanGoQuery
{
public:
	// ---------------------------------- Public Member Functions ----------------------------------

	RayCanGoQuery();
	virtual ~RayCanGoQuery() {}

	// ---------------------------------- Functions to set up the query ----------------------------------

	/// Binds the query with the specified Database, clears all the inputs and outputs and sets all
	/// other parameters to their default value (they may be changed by calling the corresponding write accessors.)
	/// \param database    A pointer to the Database on which the query must be performed.
	void BindToDatabase(Database* database);

	/// Sets an  instance of this class with minimum input requirements.
	/// Clears all the output, and let all other parameters unchanged. They
	/// may be changed by calling the corresponding write accessors.
	/// \param startPos    Sets the value of #m_startPos3f.
	/// \param destPos     Sets the value of #m_destPos3f. 
	void Initialize(const Vec3f& startPos, const Vec3f& destPos);

	// ---------------------------------- write accessors for query inputs ----------------------------------

	/// Set The NavMesh triangle that corresponds to #m_startPos3f. If a valid NavTrianglePtr is specified, this value will be used.
	/// If no value is specified, it will be automatically computed during the query.
	/// Call this method after Initialize(), during which it is cleared. 
	void SetStartTrianglePtr(const NavTrianglePtr& startTrianglePtr);

	/// Write accessor for #m_queryDynamicOutput.
	/// Call this method after Initialize(), during which #m_queryDynamicOutput is set to KY_NULL.
	void SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput);

	// ---------------------------------- write accessors for query parameters ----------------------------------

	/// Write accessor for #m_positionSpatializationRange.
	/// Call this method after BindToDatabase(), during which #m_positionSpatializationRange is set to its default value.
	void SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange);

	/// Write accessor for #m_computeCostMode.
	/// Call this method after BindToDatabase(), during which #m_computeCostMode is set to its default value.
	void SetComputeCostMode(ComputeCostMode computeCostMode);

	/// Write accessor for #m_marginMode.
	/// Call this method after BindToDatabase(), during which #m_marginMode is set to its default value.
	void SetMarginMode(RayCanGoMarginMode marginMode);

	/// Write accessor for #m_dynamicOutputMode.
	/// Call this method after BindToDatabase during which member #m_dynamicOutputMode is set to its default value.
	void SetDynamicOutputMode(DynamicOutputMode savingMode);

	// ---------------------------------- Query framework functions ----------------------------------

	virtual void Advance(WorkingMemory* workingMemory);

	/// Performs the query.
	/// \param workingMemory    Sandbox memory used to perform queries. If let to KY_NULL,
	///                          the #m_database internal WorkingMemory is used.
	void PerformQuery(WorkingMemory* workingMemory = KY_NULL);

	// ---------------------------------- Accessors ----------------------------------

	RayCanGoQueryResult   GetResult()             const;
	const Vec3f&          GetStartPos()           const;
	const Vec3f&          GetDestPos()            const;
	const NavTrianglePtr& GetStartTrianglePtr()   const;
	const NavTrianglePtr& GetDestTrianglePtr()    const;
	ComputeCostMode       GetComputeCostMode()    const;
	KyFloat32             GetComputedCost()       const;
	DynamicOutputMode     GetDynamicOutputMode()  const;
	QueryDynamicOutput*   GetQueryDynamicOutput() const;
	RayCanGoMarginMode    GetMarginMode()         const;

	const PositionSpatializationRange& GetPositionSpatializationRange() const;

public : // internal
	void PerformQueryWithInputCoordPos(WorkingMemory* workingMemory);
	void SetStartIntegerPos(const WorldIntegerPos& startIntegerPos);
	void SetDestIntegerPos(const WorldIntegerPos& destIntegerPos);
	const WorldIntegerPos& GetStartIntegerPos() const;
	const WorldIntegerPos& GetDestIntegerPos() const;
};

}

#include "gwnavruntime/queries/raycangoquery.inl"


#endif //Navigation_RayCanGoQuery_H

