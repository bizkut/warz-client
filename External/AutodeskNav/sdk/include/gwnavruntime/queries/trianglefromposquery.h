/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_TriangleFromPosQuery_H
#define Navigation_TriangleFromPosQuery_H


#include "gwnavruntime/querysystem/iquery.h"
#include "gwnavruntime/navmesh/worldintegerpos.h"
#include "gwnavruntime/navmesh/identifiers/navtriangleptr.h"
#include "gwnavruntime/database/positionspatializationrange.h"


namespace Kaim
{

class ActiveCell;
class NavCell;
class NavFloorBlob;
template <class T> class WorkingMemArray;


/// Enumerates the possible results of a TriangleFromPosQuery.
enum TriangleFromPosQueryResult
{
	TRIANGLEFROMPOS_NOT_INITIALIZED = QUERY_NOT_INITIALIZED, ///< Indicates the query has not yet been initialized. 
	TRIANGLEFROMPOS_NOT_PROCESSED   = QUERY_NOT_PROCESSED,   ///< Indicates the query has not yet been launched. 

	TRIANGLEFROMPOS_DONE_NO_TRIANGLE_FOUND,                  ///< Indicates that the point provided is outside the navigable boundaries of the NavMesh. 
	TRIANGLEFROMPOS_DONE_LACK_OF_WORKING_MEMORY,             ///< Indicates that insufficient working memory caused the query to stop. 

	TRIANGLEFROMPOS_DONE_TRIANGLE_FOUND                      ///< Indicates that the query was completed successfully; a triangle was found. 
};

/// Enumerates the possible ways a TriangleFromPosQuery can choose the triangle it retrieves.
enum TriangleFromPosQueryType
{
	NEARESTTRIANGLE_FIND_NEAREST,       ///< Instructs the query to retrieve the nearest triangle to the point provided, regardless of whether it lies above or below the point. 
	NEARESTTRIANGLE_FIND_NEAREST_BELOW, ///< Instructs the query to retrieve the first triangle below the point provided. 
	NEARESTTRIANGLE_FIND_NEAREST_ABOVE  ///< Instructs the query to retrieve the first triangle above the point provided. 
};

/// The TriangleFromPosQuery class takes a position in the 3D space (#m_inputPos3f), and finds the nearest triangle in the
///  NavMesh that covers the same (X,Y) coordinates. The search can look above and below the position at the same time, or
///  it can be limited to above- or below-only.
/// If a triangle is found, it is written to #m_resultTrianglePtr.
class TriangleFromPosQuery : public IAtomicQuery
{
public:
	static  QueryType GetStaticType() { return TypeTriangleFromPos; }
	virtual QueryType GetType() const { return TypeTriangleFromPos; }
	virtual void BuildQueryBlob(BaseBlobHandler* blobHandler);
	virtual void InitFromQueryBlob(World* world, void* blob);
	virtual Ptr<BaseBlobHandler> CreateQueryBlobHandler() { return CreateStaticQueryBlobHandler(); }
	static Ptr<BaseBlobHandler> CreateStaticQueryBlobHandler();

	// ---------------------------------- Public Member Functions ----------------------------------

	TriangleFromPosQuery();
	virtual ~TriangleFromPosQuery() {}

	// ---------------------------------- Functions to set up the query ----------------------------------

	/// Binds the query with the specified Database, clears all the inputs and outputs and sets all
	/// other parameters to their default value (they may be changed by calling the corresponding write accessors.)
	/// \param database    A pointer to the Database on which the query must be performed.
	void BindToDatabase(Database* database);

	/// resets an initialized instance of this class with minimum input requirements.
	/// Clears all the output, and let all other parameters unchanged. They
	/// may be changed by calling the corresponding write accessors.
	/// \param pos         Sets the value of #m_inputPos3f. 
	void Initialize(const Vec3f& pos);

	// ---------------------------------- write accessors for query parameters ----------------------------------

	/// Write accessor for #m_positionSpatializationRange.
	/// Call this method after BindToDatabase(), during which #m_positionSpatializationRange is set to its default value. 
	void SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange);

	/// Write accessor for #m_queryType.
	/// Call this method after BindToDatabase(), during which member #m_queryType is set to its default value. 
	void SetQueryType(TriangleFromPosQueryType queryType);


	// ---------------------------------- Query framework functions ----------------------------------

	virtual void Advance(WorkingMemory* workingMemory);

	/// Performs the query.
	/// \param workingMemory    Sandbox memory used to perform queries. If let to KY_NULL,
	///                          the #m_database internal WorkingMemory is used.
	void PerformQuery(WorkingMemory* workingMemory = KY_NULL);

	// ---------------------------------- Accessors ----------------------------------

	TriangleFromPosQueryResult GetResult()                          const;
	const Vec3f&               GetInputPos()                        const;
	const NavTrianglePtr&      GetResultTrianglePtr()               const;
	TriangleFromPosQueryType   GetQueryType()                       const;
	KyFloat32                  GetAltitudeOfProjectionInTriangle()  const;

	const PositionSpatializationRange& GetPositionSpatializationRange() const;

public: // internal
	void SetInputIntegerPos(const WorldIntegerPos& integerPos);
	const WorldIntegerPos& GetInputIntegerPos() const;
	void PerformQueryWithInputCoordPos(WorkingMemory* workingMemory);

private :
	void SetResult(TriangleFromPosQueryResult result);
	void GetNearestTrianglePtrFromPos(WorkingMemory* workingMemory);
	KyResult FilterNavFloorsFromAltitudeRange(WorkingMemArray<NavFloor*>& floorIndicesToTest, ActiveCell& activeCell);
	void ProcessFilteredNavFloors(const WorkingMemArray<NavFloor*>& filteredMetaFloorIndices,
		const Vec3f& pos3fInCell, KyFloat32 multiplicatorValidyForQueryType);
	void FindTriangleInFloor(const Vec3f& posInCell, NavFloor* navFloor, const NavFloorBlob* floorBlob,
		bool& triangleFound, KyFloat32& minDist, KyFloat32 multiplicatorValidyForQueryType);

private:
	WorldIntegerPos m_inputIntegerPos;

	Vec3f m_inputPos3f; ///< The position on the (X,Y) plane used to retrieve the nearest triangle. 

	PositionSpatializationRange m_positionSpatializationRange; ///< The altitude tolerance around #m_inputPos3f that will be searched for triangles. 

	/// An element from the #TriangleFromPosQueryType enumeration that indicates whether to look for
	/// triangles only above #m_inputPos3f (#NEARESTTRIANGLE_FIND_NEAREST_ABOVE), only below #m_inputPos3f 
	/// (#NEARESTTRIANGLE_FIND_NEAREST_BELOW), or to look for only the nearest triangle above and below (#NEARESTTRIANGLE_FIND_NEAREST). 
	TriangleFromPosQueryType m_queryType;

	/// Updated during processing to store the nearest triangle found to #m_inputPos3f. If no triangle is found,
	/// #m_resultTrianglePtr is set to an invalid NavTrianglePtr. 
	NavTrianglePtr m_resultTrianglePtr;

	/// Updated during processing to indicate the projected altitude of #m_inputPos3f on the triangle stored in #m_resultTrianglePtr.
	KyFloat32 m_altitudeInTriangle;

	TriangleFromPosQueryResult m_result; ///< Updated during processing to indicate the result of the query. 
};

}

#include "gwnavruntime/queries/trianglefromposquery.inl"



#endif //Navigation_TriangleFromPosQuery_H

