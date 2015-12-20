/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_MoveOnNavMeshQuery_H
#define Navigation_MoveOnNavMeshQuery_H

#include "gwnavruntime/queries/utils/basemoveonnavmeshquery.h"
#include "gwnavruntime/queries/diskcastquery.h"
#include "gwnavruntime/queries/utils/queryutils.h"
#include "gwnavruntime/queries/utils/breadthfirstsearchedgecollisioncollector.h"
#include "gwnavruntime/queries/utils/breadthfirstsearchtraversal.h"
#include "gwnavruntime/navmesh/intersections.h"
#include "gwnavruntime/navmesh/identifiers/navhalfedgeptr.h"
#include "gwnavruntime/database/database.h"
#include "gwnavruntime/visualsystem/displaylist.h"

namespace Kaim
{

class NavHalfEdgeRawPtr;


/// This class is used to get move bots on the NavMesh without physics engine.
template<class TraverseLogic>
class MoveOnNavMeshQuery : public BaseMoveOnNavMeshQuery
{
public:
	// ---------------------------------- Public Member Functions ----------------------------------

	static  QueryType GetStaticType() { return TypeMoveOnNavMesh; }
	virtual QueryType GetType() const { return TypeMoveOnNavMesh; }

	MoveOnNavMeshQuery();
	virtual ~MoveOnNavMeshQuery() {}

	// ---------------------------------- Functions to set up the query ----------------------------------

	/// Binds the query with the specified Database, clears all the inputs and outputs and sets all
	/// other parameters to their default value (they may be changed by calling the corresponding write accessors.)
	/// \param database    A pointer to the Database on which the query must be performed.
	void BindToDatabase(Database* database);

	/// Sets an  instance of this class with minimum input requirements.
	/// Clears all the output, and let all other parameters unchanged. They
	/// may be changed by calling the corresponding write accessors.
	/// \param startPos           Sets the value of #m_startPos3f. 
	/// \param normalizedDir2d    Sets the value of #m_normalizedDir2d.
	/// \param maxDist            Sets the value of #m_moveDistance. 
	void Initialize(const Vec3f& startPos, const Vec2f& normalizedDir2d, KyFloat32 maxDist);

	// ---------------------------------- write accessors for query inputs ----------------------------------

	/// Set the NavMesh triangle that corresponds to #m_startPos3f. If a valid NavTrianglePtr is specified, this value will be used.
	/// If no value is specified, it will be automatically computed during the query. 
	/// Call this method after Initialize(), during which it is cleared. 
	void SetStartTrianglePtr(const NavTrianglePtr& startTrianglePtr);

	// ---------------------------------- write accessors for query parameters ----------------------------------

	/// Set the minimal distance the arrival position should be from any NavMesh border.
	/// Call this method after BindToDatabase(), during which #m_distanceToBoundary is set to its default value.
	void SetDistanceToBoundary(KyFloat32 distanceToBoundary);

	// ---------------------------------- Query framework functions ----------------------------------

	virtual void Advance(WorkingMemory* workingMemory);

	/// Performs the query.
	/// \param workingMemory    Sandbox memory used to perform queries. If let to KY_NULL,
	///                          the #m_database internal WorkingMemory is used.
	void PerformQuery(WorkingMemory* workingMemory = KY_NULL);

	// ---------------------------------- Accessors ----------------------------------

	MoveOnNavMeshQueryResult GetResult()             const;
	const Vec3f&             GetStartPos()           const;
	const Vec3f&             GetArrivalPos()         const;
	const Vec2f&             GetNormalizedDir2d()    const;
	const NavTrianglePtr&    GetStartTrianglePtr()   const;
	KyFloat32                GetDistanceToBoundary() const;
	KyFloat32                GetDistance()           const;
	KyFloat32                GetAltitudeTolerance()  const;

public://internal
	void PerformQueryWithInputCoordPos(WorkingMemory* workingMemory);
	void SetStartIntegerPos(const WorldIntegerPos& startIntegerPos);
	const WorldIntegerPos& GetStartIntegerPos() const;

private:
	void SetDefaults();
	void SetResult(MoveOnNavMeshQueryResult result);

	bool CollectNearbyNavMeshBorders(WorkingMemory* workingMemory, WorkingMemArray<NavHalfEdgeRawPtr>& nearbyEdges);
	bool SolveCase(WorkingMemory* workingMemory, const WorkingMemArray<NavHalfEdgeRawPtr>& nearbyEdges);

};

} // namespace Kaim

#include "gwnavruntime/queries/moveonnavmeshquery.inl"



#endif //Navigation_MoveOnNavMeshQuery_H

