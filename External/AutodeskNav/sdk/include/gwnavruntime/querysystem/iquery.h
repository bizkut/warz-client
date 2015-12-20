/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: JAPA - secondary contact: NOBODY
#ifndef Navigation_IQuery_H
#define Navigation_IQuery_H

#include "gwnavruntime/blob/baseblobhandler.h"
#include "gwnavruntime/basesystem/logger.h"
#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/kernel/SF_RefCount.h"
#include "gwnavruntime/kernel/SF_Debug.h"


namespace Kaim
{

class World;
class Database;
class WorkingMemory;
class QueryQueue;

/// Enumerates all the type of query.
/// If you write your own query, you have to add a corresponding QueryType here.
enum QueryType
{
	TypeRayCanGo = 0,
	TypeRayCast,
	TypeDiskCanGo,
	TypeDiskCast,
	TypeDiskCollision,
	TypeAStar,
	TypeTriangleFromPos,
	TypeMoveOnNavMesh,
	TypeSpatializedPointCollectorInAABB,
	TypeTriangleFromPosAndTriangleSeed,
	TypeNearestBorderHalfEdgeFromPos,
	TypeSegmentCast,
	TypeSegmentCanGo,
	TypeDiskExpansion,
	TypeInsidePosFromOutsidePos,
	TypeMultipleFloorTrianglesFromPos,
	TypeRayCanGoOnSegment,
	TypeRayCastOnSegment,
	TypeDynamicNavMesh,
	TypeFindFirstVisiblePositionOnPath,
	TypeBestGraphVertexPathFinderQuery,
	TypeMakeNavFloorStitchQuery,
	TypeCollisionRayCast,
	TypeRayCastFull3D,
	TypePathFromPolyline,
	TypeTagVolumesFromPos,
	TypeMultiDestinationPathFinderQuery,

	QueryType_FirstCustom
};

enum PerformQueryStat
{
	QueryStat_Generation,
	QueryStat_Spatialization,
	QueryStat_NavGraph,
	QueryStat_PathFinder,
	QueryStat_ProgressOnPath,
	QueryStat_PathValidityInterval,
	QueryStat_TargetOnPathIsStillVisible,
	QueryStat_TargetOnPathShortcutForward,
	QueryStat_TargetOnPathSearchBackward,
	QueryStat_Avoidance,
	QueryStat_Channel,
	QueryStat_LabEngine,
	
	QueryStat_Other, ///< can be sporadically to differentiate from PerformQueryStat_Unspecified

	PerformQueryStat_Count,
	PerformQueryStat_Unspecified = PerformQueryStat_Count,
};

/// Enumerates all the processing status a query can have.
enum QueryProcessStatus
{
	QueryNotStarted = 0,
	QueryInProcess,
	QueryDone,
	QueryCanceled,
	QueryResultMax,
};

/// Enumerates the different states a query may have in a QueryQueue.
enum QueryStatusInQueue
{
	QueryNotInQueue,
	QueryPushedAsCommand,
	QueryInQueryQueue,
};

enum QueryProcessMode
{
	QueryProcessMode_Sync,
	QueryProcessMode_ASync
};

/// Small interface class that you have to derive from and set to an IQuery to
/// get the IOnDone::OnDone() function called during the World::Update() (in
/// FlushQueries() step) if the query has been completed.
/// Note that this function is not called if the query is canceled.
class IOnDone : public RefCountBase<IOnDone, MemStat_Query>
{
public:
	// ---------------------------------- Public Member Functions ----------------------------------
	virtual ~IOnDone() {}
	virtual void OnDone() = 0;
};

/// Abstract class for all queries.
class IQuery : public RefCountBase<IQuery, MemStat_Query>
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Query)
public:
	IQuery();
	virtual ~IQuery();

	// ---------------------------------- Main API Functions ----------------------------------

	/// In the case of a query processed in a QueryQueue, this must be tested before
	/// initializing the query, to prevent modifying the query during the process.
	bool CanBeInitialized() const;

	/// Returns true if the query is currently registered in a QueryQueue.
	/// If true, you must not push it again in QueryQueue (even the same one).
	/// If false, you must not try to cancel it from the QueryQueue.
	bool IsInAQueryQueue() const;

	/// Returns true if the query has been processed, is no longer in a queue, and OnDone (if there is one) has been called.
	/// If true, it's guaranteed to be safe to read the results of this query.
    /// Will return false if the Query has been cancelled or has not yet been processed.
	bool IsSafeToReadResult() const;

	/// Process the query at once whether it is time-sliced or not.
	/// \param workingMemory    Sandbox memory used to perform queries. If let to KY_NULL,
	///                         the #m_database internal WorkingMemory is used.
	void PerformQueryBlocking(WorkingMemory* workingMemory = KY_NULL);

	/// Calls the OnDone function of the IOnDone object if provided.
	/// Called automatically by the QueryQueue during the World::Update()
	/// (in FlushQueries() step) when this query has been completed.
	/// Not called if this query is canceled.
	/// Note that the member #m_onDone is reset to KY_NULL at the end of this function.
	void OnDone();


	// Function that can be used to send a query to the NavigationLab.
	// Currently the implementation transform the query using a ScopedDisplayList,
	// and the display list is shown only at the frame SendVisualDebug is called,
	KyResult SendVisualDebug();


	void SetTraverseLogicUserData(void* traverseLogicUserData);
	void* GetTraverseLogicUserData() const;

	// ---------------------------------- Pure virtual functions - Query framework contract ----------------------------------

	virtual QueryType GetType() const = 0;

	/// This function is called by the QueryQueue to process one step on the query.
	/// It MUST NEVER, under any circumstances, take too long. As a guideline, consider 0.1ms as too long.
	/// It is under the responsibility of this function to update #m_processStatus. Obviously, if the
	/// query is atomic (i.e. not time-sliced), a call to Advance() will fully perform the query and set
	/// #m_processStatus to QueryDone. If the query is time-sliced, several calls to Advance() will be needed
	/// to complete the query; the first call should set #m_processStatus to Kaim::QueryInProcess and the last one
	/// to Kaim::QueryDone.
	/// \param workingMemory    Sandbox memory used to perform queries.
	virtual void Advance(WorkingMemory* workingMemory) = 0;

public: // internal
	// ---------------------------------- Internal Virtual functions for NavigationLab Remote Query processing ----------------------------------
	/// These functions are used by IQuery::SendVisualDebug()
	/// and also used by the NavigationLab to request remote execution: 
	///   using VisualDebugClient::Send(IQuery*) to ask for a query to be executed
	///   using VisualDebugServer::Send(IQuery*) to send back the query result.
	/// Queries that can be visual debug override these 3 functions.
	/// By convenience, such queries provide a static function with the following signature: /code
	///    static Ptr<BaseBlobHandler>  CreateStaticQueryBlobHandler();  /endcode

	virtual Ptr<BaseBlobHandler> CreateQueryBlobHandler() { return KY_NULL; }
	virtual void BuildQueryBlob(Kaim::BaseBlobHandler* /*blobHandler*/) {}
	virtual void InitFromQueryBlob(World* /*world*/, void* /*blob*/) {}

public: // internal
	static const char* GetQueryTypeName(QueryType queryType);

	// ---------------------------------- Mandatory initialization calls to be done by derived classes ----------------------------------
	
	/// Should be called by the derived class before Initializing the query
	/// It sets #m_database to the provided value.
	/// It sets #m_userData to KY_NULL;
	void BindToDatabase(Database* database);

	/// Should be called by the derived class before trying to perform the query
	/// or to push it in a QueryQueue.
	/// The query must have been bound to a Database before.
	/// It sets #m_processStatus to Kaim::QueryNotStarted. As some query framework
	/// components (such as QueryQueue) relies on these members, you cannot call SetUp()
	/// unless CanBeInitialized() returns true.
	void Initialize();

public:
	// ---------------------------------- Public Data Members ----------------------------------

	/// Modified by the query within Advance(). Do not modify.
	/// Before reading m_processStatus, always ensure IsInAQueryQueue() returns false.
	QueryProcessStatus m_processStatus; 
	Ptr<IOnDone> m_onDone;              ///< The optional IOnDone instance to be called. Must be set manually by the user. See IOnDone.

public: // Internal
	Database* m_database;               ///< The database on which the query will be performed. Set in Initialize(), do not directly modify.
	void* m_traverseLogicUserData;                   ///< 


	QueryQueue* m_queue;                ///< Updated by the QueryQueue. Do not modify.
	QueryStatusInQueue m_inQueueStatus; ///< Used by the QueryQueue. Do not modify.
	KyUInt32 m_queryInfoId;
};


/// Base class for all the queries that do not need to be time-sliced.
class IAtomicQuery : public IQuery
{
public:
	IAtomicQuery();
	virtual ~IAtomicQuery() {}

	void SetPerformQueryStat(PerformQueryStat performQueryStat);
public:
	PerformQueryStat m_performQueryStat;
};


/// Base class for all the queries that need to be time-sliced.
class ITimeSlicedQuery : public IQuery
{
public:
	ITimeSlicedQuery();
	virtual ~ITimeSlicedQuery();

	// ---------------------------------- Public Member Functions ----------------------------------

	/// Should be called by the derived class before trying to perform the query
	/// or to push it in a QueryQueue.
	/// This function calls IQuery::Initialize().
	void Initialize();


	// ---------------------------------- Pure virtual functions  ----------------------------------

	/// Called when a query is canceled in FlushCommands while its status is QueryInProcess
	/// to make sure that the WorkingMemory is left in valid state (typically, no
	/// WorkingMemory buffer should be in used after this call).
	virtual void ReleaseWorkingMemoryOnCancelDuringProcess(WorkingMemory* workingMemory) = 0;

public:
	KyUInt32 m_advanceCount;
	KyUInt32 m_lastAdvanceFrameIdx;
};

/// Enumerates the result codes that are common to all queries.
enum CommonQueryResult
{
	QUERY_NOT_INITIALIZED = 0, ///< Indicates that the query has not yet been initialized. 
	QUERY_NOT_PROCESSED = 1    ///< Indicates that the query has not yet been launched. 
};



KY_INLINE IQuery::IQuery() :
	m_processStatus(QueryNotStarted),
	m_database(KY_NULL),
	m_traverseLogicUserData(KY_NULL),
	m_queue(KY_NULL),
	m_inQueueStatus(QueryNotInQueue),
	m_queryInfoId(0)
{}

KY_INLINE IQuery::~IQuery() {}

KY_INLINE void IQuery::BindToDatabase(Database* database)
{
	KY_LOG_ERROR_IF(database == KY_NULL, ("The database you are bounding the query to is not valid."));
	KY_LOG_ERROR_IF(CanBeInitialized() == false, ("You cannot bind a query to a Database while it is registered in a QueryQueue."));
	m_database = database;
	m_traverseLogicUserData = KY_NULL;
}

KY_INLINE void IQuery::Initialize()
{
	KY_LOG_ERROR_IF(m_database == KY_NULL, ("This query must have been bound to a valid Database before calling Initialize"));
	KY_LOG_ERROR_IF(CanBeInitialized() == false, ("You cannot initialized a query while it is registered in a QueryQueue."));
	m_processStatus = QueryNotStarted;
}

KY_INLINE bool IQuery::CanBeInitialized() const { return m_inQueueStatus != QueryInQueryQueue; }
KY_INLINE bool IQuery::IsInAQueryQueue()  const { return m_inQueueStatus != QueryNotInQueue;   }
KY_INLINE bool IQuery::IsSafeToReadResult() const { return (IsInAQueryQueue() == false) && (m_processStatus == QueryDone); }

KY_INLINE void* IQuery::GetTraverseLogicUserData() const { return m_traverseLogicUserData; }
KY_INLINE void  IQuery::SetTraverseLogicUserData(void* traverseLogicUserData) { m_traverseLogicUserData = traverseLogicUserData; }

KY_INLINE void IQuery::OnDone()
{
	if (m_onDone != KY_NULL)
		m_onDone->OnDone();

	m_onDone = KY_NULL;
}

KY_INLINE IAtomicQuery::IAtomicQuery() : m_performQueryStat(PerformQueryStat_Unspecified) {}
KY_INLINE void IAtomicQuery::SetPerformQueryStat(PerformQueryStat performQueryStat) { m_performQueryStat = performQueryStat; }

KY_INLINE ITimeSlicedQuery::ITimeSlicedQuery() : m_advanceCount(0), m_lastAdvanceFrameIdx(0) {}
KY_INLINE ITimeSlicedQuery::~ITimeSlicedQuery() {}
KY_INLINE void ITimeSlicedQuery::Initialize()
{
	IQuery::Initialize();
	m_advanceCount = 0;
	m_lastAdvanceFrameIdx = 0;
}


}

#endif
