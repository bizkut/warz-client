/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_TagVolumesFromPosQuery_H
#define Navigation_TagVolumesFromPosQuery_H

#include "gwnavruntime/querysystem/iquery.h"
#include "gwnavruntime/navmesh/worldintegerpos.h"
#include "gwnavruntime/database/positionspatializationrange.h"
#include "gwnavruntime/queries/querydynamicoutput.h"

namespace Kaim
{

class ActiveCell;
class NavCell;
class NavFloorBlob;
template <class T> class WorkingMemArray;


/// Enumerates the possible results of a TagVolumesFromPosQuery.
enum TagVolumesFromPosQueryResult
{
	TAGVOLUMESROMPOS_NOT_INITIALIZED = QUERY_NOT_INITIALIZED, ///< Indicates the query has not yet been initialized. 
	TAGVOLUMESROMPOS_NOT_PROCESSED   = QUERY_NOT_PROCESSED,   ///< Indicates the query has not yet been launched. 

	TAGVOLUMESROMPOS_DONE_NO_TAGVOLUMES_FOUND,                  ///< Indicates that the point provided is outside the navigable boundaries of the NavMesh. 
	TAGVOLUMESROMPOS_DONE_LACK_OF_WORKING_MEMORY,             ///< Indicates that insufficient working memory caused the query to stop. 

	TAGVOLUMESROMPOS_DONE_TAGVOLUMES_FOUND                      ///< Indicates that the query was completed successfully; a triangle was found. 
};


// Note that this query returns the TagVolumes that are integrated.
// Added tagVolumes that are not integrated yet or already fully deIntegrated won't be retrieved.
class TagVolumesFromPosQuery : public IAtomicQuery
{
public:
	static  QueryType GetStaticType() { return TypeTagVolumesFromPos; }
	virtual QueryType GetType() const { return TypeTagVolumesFromPos; }

	// ---------------------------------- Public Member Functions ----------------------------------

	TagVolumesFromPosQuery();
	virtual ~TagVolumesFromPosQuery() {}

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

	/// Write accessor for #m_queryDynamicOutput.
	/// Call this method after Initialize(), during which #m_queryDynamicOutput is set to KY_NULL.
	void SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput);

	/// Write accessor for #m_positionSpatializationRange.
	/// Call this method after BindToDatabase(), during which #m_positionSpatializationRange is set to its default value. 
	void SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange);

	// ---------------------------------- Query framework functions ----------------------------------

	virtual void Advance(WorkingMemory* workingMemory);

	/// Performs the query.
	/// \param workingMemory    Sandbox memory used to perform queries. If let to KY_NULL,
	///                          the #m_database internal WorkingMemory is used.
	void PerformQuery(WorkingMemory* workingMemory = KY_NULL);

	// ---------------------------------- Accessors ----------------------------------

	TagVolumesFromPosQueryResult GetResult()                           const;
	const Vec3f&                 GetInputPos()                         const;
	KyFloat32                    GetAltitudeOfProjectionInTagVolumes() const;
	QueryDynamicOutput*          GetQueryDynamicOutput()               const;

	const PositionSpatializationRange& GetPositionSpatializationRange() const;

public: // internal
	void SetInputIntegerPos(const WorldIntegerPos& integerPos);
	const WorldIntegerPos& GetInputIntegerPos() const;
	void PerformQueryWithInputCoordPos(WorkingMemory* workingMemory);

private :
	void SetResult(TagVolumesFromPosQueryResult result);
	void GetTagVolumesFromPos(WorkingMemory* workingMemory);
	
private:
	WorldIntegerPos m_inputIntegerPos;

	Vec3f m_inputPos3f; ///< The position on the (X,Y) plane used to retrieve the nearest triangle. 

	/// A ref-counted Ptr to a QueryDynamicOutput instance, used to store the TagVolumes found.
	/// You can keep the Ptr for reuse and reassign it to another query by calling SetQueryDynamicOutput() after
	/// the BindToDatabase(), but in this case, be careful: the query may swap the underlying instance when processing
	/// if the provided buffer is to short. In this case, it is more sure to always update the Ptr when the query process is over.
	Ptr<QueryDynamicOutput> m_queryDynamicOutput;

	PositionSpatializationRange m_positionSpatializationRange; ///< The altitude tolerance around #m_inputPos3f that will be searched for triangles. 

	TagVolumesFromPosQueryResult m_result; ///< Updated during processing to indicate the result of the query. 
};

}

#include "gwnavruntime/queries/tagvolumesfromposquery.inl"



#endif //Navigation_TagVolumesFromPosQuery_H

