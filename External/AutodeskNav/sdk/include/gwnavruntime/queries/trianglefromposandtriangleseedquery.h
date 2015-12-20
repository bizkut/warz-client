/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_TriangleFromPosAndTriangleSeed_H
#define Navigation_TriangleFromPosAndTriangleSeed_H

#include "gwnavruntime/querysystem/iquery.h"
#include "gwnavruntime/navmesh/worldintegerpos.h"
#include "gwnavruntime/navmesh/identifiers/navtriangleptr.h"



namespace Kaim
{

class NavHalfEdgeRawPtr;
template <class T> class WorkingMemArray;


/// Enumerates the possible results of a TriangleFromPosAndTriangleSeedQuery.
enum TriangleFromPosAndTriangleSeedResult
{
	NEARESTTRIANGLEFROMSEED_NOT_INITIALIZED = QUERY_NOT_INITIALIZED, ///< Indicates the query has not yet been initialized. 
	NEARESTTRIANGLEFROMSEED_NOT_PROCESSED   = QUERY_NOT_PROCESSED,   ///< Indicates the query has not yet been launched. 

	NEARESTTRIANGLEFROMSEED_DONE_SEED_INVALID,                       ///< Indicates an invalid NavTrianglePtr was provided as the seed point; the query cannot be processed. 
	NEARESTTRIANGLEFROMSEED_DONE_NO_TRIANGLE_FOUND,                  ///< Indicates that no triangle could be found. 
	NEARESTTRIANGLEFROMSEED_DONE_LACK_OF_WORKING_MEMORY,             ///< Indicates that insufficient working memory caused the query to stop. 

	NEARESTTRIANGLEFROMSEED_DONE_TRIANGLE_FOUND                      ///< Indicates the query was successfully completed; a triangle was found. 
};


/// The TriangleFromPosAndTriangleSeedQuery class finds the nearest triangle in the NavMesh that covers the same (X,Y)
/// coordinates as a specified position in the 3D space (#m_inputPos3f). To find the triangle, it starts with a specified seed
/// triangle (#m_seedTrianglePtr), and propagates outward into the NavMesh to check each triangle in the "crown" of the
/// seed triangle: that is, each triangle that shares at least one vertex with the seed triangle.
/// If a result is found, it is written to #m_result.
/// This query is used internally during other types of NavMesh queries to improve their results.
class TriangleFromPosAndTriangleSeedQuery : public IAtomicQuery
{
public:
	// ---------------------------------- Public Member Functions ----------------------------------

	static  QueryType GetStaticType() { return TypeTriangleFromPosAndTriangleSeed; }
	virtual QueryType GetType() const { return TypeTriangleFromPosAndTriangleSeed; }

	TriangleFromPosAndTriangleSeedQuery();
	virtual ~TriangleFromPosAndTriangleSeedQuery() {}

	// ---------------------------------- Functions to set up the query ----------------------------------

	/// Binds the query with the specified Database, clears all the inputs and outputs and sets all
	/// other parameters to their default value (they may be changed by calling the corresponding write accessors.)
	/// \param database    A pointer to the Database on which the query must be performed.
	void BindToDatabase(Database* database);

	/// resets an initialized instance of this class with minimum input requirements.
	/// Clears all the output, and let all other parameters unchanged. They
	/// may be changed by calling the corresponding write accessors.
	/// \param seedTrianglePtr    Sets the value of #m_seedTrianglePtr.
	/// \param pos                Sets the value of #m_inputPos3f. 
	void Initialize(const NavTrianglePtr& seedTrianglePtr, const Vec3f& pos);

	// ---------------------------------- Query framework functions ----------------------------------

	virtual void Advance(WorkingMemory* workingMemory);

	/// Performs the query.
	/// \param workingMemory    Sandbox memory used to perform queries. If let to KY_NULL,
	///                          the #m_database internal WorkingMemory is used.
	void PerformQuery(WorkingMemory* workingMemory = KY_NULL);

	// ---------------------------------- Accessors ----------------------------------

	TriangleFromPosAndTriangleSeedResult GetResult()            const;
	const Vec3f&                         GetInputPos()          const;
	const NavTrianglePtr&                GetSeedTrianglePtr()   const;
	const NavTrianglePtr&                GetResultTrianglePtr() const;

public: // internal
	void SetInputIntegerPos(const WorldIntegerPos& integerPos);
	const WorldIntegerPos& GetInputIntegerPos() const;
	void PerformQueryWithInputCoordPos(WorkingMemory* workingMemory);

private :
	void SetResult(TriangleFromPosAndTriangleSeedResult result);
	void GetNearestTrianglePtrFromTriangleSeed_Crown();
	bool IsPointInsideCurrentTriangle(const NavHalfEdgeRawPtr& currentOutComingEdge) const;

private:
	WorldIntegerPos m_inputIntegerPos;

	NavTrianglePtr m_seedTrianglePtr; ///< A NavTrianglePtr that identifies the triangle that will be the seed for the propagation. 

	Vec3f m_inputPos3f; ///< The position on the (X,Y) plane used to retrieve the nearest triangle. The altitude of this point is not considered. 

	/// Updated during processing to store the nearest triangle found to #m_inputPos3f. If no triangle is found,
	/// #m_resultTrianglePtr is set to an invalid NavTrianglePtr. 
	NavTrianglePtr m_resultTrianglePtr;

	TriangleFromPosAndTriangleSeedResult m_result; ///< Updated during processing to indicate the result of the query. 
};

}

#include "gwnavruntime/queries/trianglefromposandtriangleseedquery.inl"



#endif //Navigation_TriangleFromPosAndTriangleSeed_H

