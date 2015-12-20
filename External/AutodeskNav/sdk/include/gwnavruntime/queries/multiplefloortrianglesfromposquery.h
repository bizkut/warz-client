/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_TrianglesFromPosQuery_H
#define Navigation_TrianglesFromPosQuery_H

#include "gwnavruntime/querysystem/iquery.h"
#include "gwnavruntime/queries/querydynamicoutput.h"
#include "gwnavruntime/navmesh/worldintegerpos.h"

namespace Kaim
{

/// Enumerates the possible results of a MultipleFloorTrianglesFromPosQuery.
enum MultipleFloorTrianglesFromPosQueryResult
{
	MULTIPLEFLOORTRIANGLESFROMPOS_NOT_INITIALIZED = QUERY_NOT_INITIALIZED, ///< Indicates the query has not yet been initialized. 
	MULTIPLEFLOORTRIANGLESFROMPOS_NOT_PROCESSED   = QUERY_NOT_PROCESSED,   ///< Indicates the query has not yet been launched. 

	MULTIPLEFLOORTRIANGLESFROMPOS_DONE_NO_TRIANGLE_FOUND,                  ///< Indicates that the point provided is outside the navigable boundaries of the NavMesh. 
	MULTIPLEFLOORTRIANGLESFROMPOS_DONE_MAX_MEMORY_REACHED,                 ///< Indicates the query was completed successfully, but not all the triangles found have been stored because insufficient memory is available.

	MULTIPLEFLOORTRIANGLESFROMPOS_DONE_TRIANGLES_FOUND                     ///< Indicates the query was completed successfully, as at least one triangle was found. 
};

/// Enumerates the possible ways that a MultipleFloorTrianglesFromPosQuery chooses which triangles to retrieve.
enum MultipleFloorTrianglesFromPosQueryType
{
	MULTIPLEFLOORTRIANGLESFROMPOS_FIND_ALL,                    ///< Instructs the query to retrieve all triangles at the same (X,Y) coordinates as the point provided, regardless of whether they lie above or below the point. 
	MULTIPLEFLOORTRIANGLESFROMPOS_FIND_ALL_BELOW,              ///< Instructs the query to retrieve all triangles that lie below the point provided. 
	MULTIPLEFLOORTRIANGLESFROMPOS_FIND_ALL_ABOVE,              ///< Instructs the query to retrieve all triangles that lie above the point provided. 
	MULTIPLEFLOORTRIANGLESFROMPOS_FIND_NEAREST_ABOVE_AND_BELOW ///< Instructs the query to retrieve the first triangle that lies above the point provided, and the first triangle that lies below the point provided. 
};

/// The MultipleFloorTrianglesFromPosQuery class takes a position in 3D space (#m_inputPos3f), and finds the triangles in the
/// NavMesh that lie at the same (X,Y) coordinates. The search can look above and below the position at the same time,
/// or it can be limited to above- or below-only.
/// If the search finds one or more triangles, they are written into a QueryDynamicOutput object.
class MultipleFloorTrianglesFromPosQuery : public IAtomicQuery
{
public:
	// ---------------------------------- Public Member Functions ----------------------------------

	static  QueryType GetStaticType() { return TypeMultipleFloorTrianglesFromPos; }
	virtual QueryType GetType() const { return TypeMultipleFloorTrianglesFromPos; }

	MultipleFloorTrianglesFromPosQuery();
	virtual ~MultipleFloorTrianglesFromPosQuery() {}

	// ---------------------------------- Functions to set up the query ----------------------------------

	/// Binds the query with the specified Database, clears all the inputs and outputs and sets all
	/// other parameters to their default value (they may be changed by calling the corresponding write accessors.)
	/// \param database    A pointer to the Database on which the query must be performed.
	void BindToDatabase(Database* database);

	/// Sets an  instance of this class with minimum input requirements.
	/// Clears all the output, and let all other parameters unchanged. They
	/// may be changed by calling the corresponding write accessors.
	/// \param pos         Sets the value of #m_inputPos3f. 
	void Initialize(const Vec3f& pos);

	// ---------------------------------- write accessors for query inputs ----------------------------------

	/// Write accessor for #m_queryDynamicOutput.
	/// Call this method after Initialize(), during which #m_queryDynamicOutput is set to KY_NULL.
	void SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput);

	// ---------------------------------- write accessors for query parameters ----------------------------------

	/// Write accessor for #m_queryType.
	/// Call this method after BindToDatabase(), during which #m_queryType is set to its default value. 
	void SetQueryType(MultipleFloorTrianglesFromPosQueryType queryType);

	// ---------------------------------- Query framework functions ----------------------------------

	virtual void Advance(WorkingMemory* workingMemory);

	/// Performs the query.
	/// \param workingMemory    Sandbox memory used to perform queries. If let to KY_NULL,
	///                          the #m_database internal WorkingMemory is used.
	void PerformQuery(WorkingMemory* workingMemory = KY_NULL);

	// ---------------------------------- Accessors ----------------------------------

	MultipleFloorTrianglesFromPosQueryResult GetResult()             const;
	const Vec3f&                             GetInputPos()           const;
	MultipleFloorTrianglesFromPosQueryType   GetQueryType()          const;
	QueryDynamicOutput*                      GetQueryDynamicOutput() const;

public: // internal
	void SetInputIntegerPos(const WorldIntegerPos& integerPos);
	const WorldIntegerPos& GetInputIntegerPos() const;
	void PerformQueryWithInputCoordPos(WorkingMemory* workingMemory);

private:
	void SetResult(MultipleFloorTrianglesFromPosQueryResult result);

private:
	WorldIntegerPos m_inputIntegerPos;

	Vec3f m_inputPos3f; ///< The position on the (X,Y) plane for which the triangles will be retrieved. 

	/// An element from the #MultipleFloorTrianglesFromPosQueryType enumeration that indicates whether to look for
	/// triangles only above #m_inputPos3f (#MULTIPLEFLOORTRIANGLESFROMPOS_FIND_ALL_ABOVE), only below #m_inputPos3f 
	/// (#MULTIPLEFLOORTRIANGLESFROMPOS_FIND_ALL_BELOW), both above and below (#MULTIPLEFLOORTRIANGLESFROMPOS_FIND_ALL), or to look for 
	/// only the nearest triangle above and below (MULTIPLEFLOORTRIANGLESFROMPOS_FIND_NEAREST_ABOVE_AND_BELOW). 
	MultipleFloorTrianglesFromPosQueryType m_queryType;

	MultipleFloorTrianglesFromPosQueryResult m_result; ///< Updated during processing to indicate the result of the query. 

	/// A ref-counted Ptr to a QueryDynamicOutput instance, used to store the data encountered during the query process.
	/// You can keep the Ptr for reuse and reassign it to another query by calling SetQueryDynamicOutput() after the BindToDatabase(), but in this case, be careful: the query
	/// may swap the underlying instance when processing if the provided buffer is to short. In this case, it is
	/// more sure to always update the Ptr when the query process is over.
	Ptr<QueryDynamicOutput> m_queryDynamicOutput;
};

}

#include "gwnavruntime/queries/multiplefloortrianglesfromposquery.inl"


#endif //Navigation_TrianglesFromPosQuery_H

