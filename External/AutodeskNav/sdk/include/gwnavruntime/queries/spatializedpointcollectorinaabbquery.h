/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_SpatializedPointCollectorInAABBQuery_H
#define Navigation_SpatializedPointCollectorInAABBQuery_H


#include "gwnavruntime/querysystem/iquery.h"
#include "gwnavruntime/queries/querydynamicoutput.h"
#include "gwnavruntime/queries/utils/basespatializedpointcollectorinaabbquery.h"
#include "gwnavruntime/queries/utils/spatializedpointcollectorcontext.h"
#include "gwnavruntime/spatialization/spatializedpoint.h"
#include "gwnavruntime/database/database.h"


namespace Kaim
{

class SpatializedPointCollectorFilter_CollectAll
{
public:
	bool ShouldCollectSpatializedPoint(SpatializedPoint* /*spatializedPoint*/) { return true; }
};

class SpatializedPointCollectorFilter_SelectiveCollect
{
public:
	SpatializedPointCollectorFilter_SelectiveCollect()
	{
		for (KyUInt32 i = 0; i < SpatializedPointObjectType_Count; ++i)
			m_selection[i] = false;
	}

	void Select(SpatializedPointObjectType type) { m_selection[type] = true; }

	bool ShouldCollectSpatializedPoint(SpatializedPoint* spatializedPoint)
	{
		return m_selection[spatializedPoint->GetObjectType()];
	}

public:
	bool m_selection[SpatializedPointObjectType_Count];
};

/// The SpatializedPointCollectorInAABBQuery class retrieves all SpatializedPoints within an Axis-Aligned Bounding Box (AABB)
/// that can be reached navigating from a specified starting point, and stores them in a QueryDynamicOutput object.
/// The AABB is computed from startPos and extentBox provided in the BindToDatabase function.
template <class SpatializedPointCollectorFilter = SpatializedPointCollectorFilter_CollectAll>
class SpatializedPointCollectorInAABBQuery : public BaseSpatializedPointCollectorInAABBQuery
{
public:
	// ---------------------------------- Public Member Functions ----------------------------------

	SpatializedPointCollectorInAABBQuery();
	SpatializedPointCollectorInAABBQuery(const SpatializedPointCollectorFilter& collectorFilter);
	virtual ~SpatializedPointCollectorInAABBQuery() {}

	// ---------------------------------- Functions to set up the query ----------------------------------

	/// Binds the query with the specified Database, clears all the inputs and outputs and sets all
	/// other parameters to their default value (they may be changed by calling the corresponding write accessors.)
	/// \param database    A pointer to the Database on which the query must be performed.
	void BindToDatabase(Database* database);

	/// resets an initialized instance of this class with minimum input requirements.
	/// Clears all the output, and let all other parameters unchanged. They
	/// may be changed by calling the corresponding write accessors.
	/// \param startPos     The starting position for the query.
	/// \param extentBox    The extent Box3f relative to the starting position of the query. 
	void Initialize(const Vec3f& startPos, const Box3f& extentBox);

	// ---------------------------------- write accessors for query inputs ----------------------------------

	/// The NavMesh triangle that corresponds to the starting position. If a valid NavTrianglePtr is specified, this value will be used.
	/// If no value is specified, it will be automatically computed during the query. 
	/// Call this method after BindToDatabase(). 
	void SetStartTrianglePtr(const NavTrianglePtr& startTrianglePtr);

	/// Write accessor for #m_queryDynamicOutput.
	/// Call this method after Initialize(), during which #m_queryDynamicOutput is set to KY_NULL.
	void SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput);

	// ---------------------------------- write accessors for query parameters ----------------------------------

	/// Defines a range of altitudes around the starting position that will be used to retrieve a NavMesh triangle
	/// for the starting position.
	/// Call this method after BindToDatabase().
	void SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange);

	// ---------------------------------- Query framework functions ----------------------------------

	virtual void Advance(WorkingMemory* workingMemory);

	/// Performs the query.
	/// \param workingMemory    Sandbox memory used to perform queries. If let to KY_NULL,
	///                          the #m_database internal WorkingMemory is used.
	void PerformQuery(WorkingMemory* workingMemory = KY_NULL);

	// ------------------------------- Accessors ----------------------------------

	SpatializedPointCollectorInAABBQueryResult GetResult()                 const;
	const Vec3f&                               GetStartPos()               const;
	const Box3f&                               GetExtentBox()              const;
	const NavTrianglePtr&                      GetStartTrianglePtr()       const;
	QueryDynamicOutput*                        GetQueryDynamicOutput()     const;

	const PositionSpatializationRange& GetPositionSpatializationRange() const;

public:
	void PerformQueryWithInputCoordPos(WorkingMemory* workingMemory);
	void SetStartIntegerPos(const WorldIntegerPos& startIntegerPos);
	const WorldIntegerPos& GetStartIntegerPos() const;

private:
	void SetResult(SpatializedPointCollectorInAABBQueryResult result);

	void RunCollectorTraversal(SpatializedPointCollectorContext* traversalContext);
	KyResult CollectInNavFloor(SpatializedPointCollectorContext* traversalContext, const NavFloorRawPtr& navFloorRawPtr);
	KyResult CollectInNavGraphEdge(SpatializedPointCollectorContext* traversalContext, const NavGraphEdgeRawPtr& navGraphEdgeRawPtr);
	KyResult ProcessNavFloorNode(SpatializedPointCollectorContext* traversalContext, const NavFloorRawPtr& navFloorRawPtr);
	KyResult ProcessNavGraphEdgeNode(SpatializedPointCollectorContext* traversalContext, const NavGraphEdgeRawPtr& navGraphEdgeRawPtr);
public:
	SpatializedPointCollectorFilter m_spatializedPointCollectorFilter;
};

}

#include "gwnavruntime/queries/spatializedpointcollectorinaabbquery.inl"


#endif //Navigation_SpatializedPointCollectorInAABBQuery_H

