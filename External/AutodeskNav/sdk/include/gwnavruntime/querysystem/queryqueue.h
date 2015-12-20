/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: JAPA - secondary contact: GUAL
#ifndef Navigation_QueryQueue_H
#define Navigation_QueryQueue_H

#include "gwnavruntime/containers/circulararray.h"
#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/querysystem/workingmemory.h"
#include "gwnavruntime/kernel/SF_String.h"
#include "gwnavruntime/kernel/SF_RefCount.h"
#include "gwnavruntime/world/floatstat.h"
#include "gwnavruntime/world/worldelement.h"


namespace Kaim
{

class VisualDebugServer;
class World;
class Database;
class IQuery;

/// This class is used by the Visual Debug system to profileQueryQueue.
class QueryQueueStats
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_QuerySystem)
public:
	QueryQueueStats()
		: m_msSpentInProcessStat("Spent in Process (ms)")
		, m_nbQueriesPopped(0)
	{}

	FloatStat m_msSpentInProcessStat; // ms spent in Process(), in the last frame
	KyUInt32  m_nbQueriesPopped;  // nb queries done or cancelled in the last frame
};

/// Class used to provide QueryQueue initialization parameters.
class QueryQueueConfig
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_QuerySystem)
public:
	QueryQueueConfig() :
		m_budgetInMilliseconds(1.0f),
		m_workingMemoryMaxSize(4 * 1024 * 1024), // 4MB
		m_workingMemoryReallocSize(5 * 1024) // 5KB
	{}

	/// The name of the QueryQueue. This is an optional parameter, not expected by Gameware Navigation, but it may be usefull for
	/// debugging and ease-of-use.
	String m_name;
	KyFloat32 m_budgetInMilliseconds;    ///< The time budget per frame of the QueryQueue. Default value is 1 ms.
	KyUInt32 m_workingMemoryMaxSize;     ///< The amount of memory in bytes the WorkingMemory of this QueryQueue is allowed to allocate. Default value is 1 MB.
	KyUInt32 m_workingMemoryReallocSize; ///< The additional size of memory in bytes that will be allocated on ReAlloc in the WorkingMemory. Default value is 5 KB.
};

/// class use internal by the QueryQueue to postpone Push/Cancel of Queries until next FlushCommands() step of the World::Update().
class QueryQueueCommand
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_QuerySystem)
public:
	enum Type { Push = 0, Cancel };
	QueryQueueCommand(Type type, IQuery* query) : m_type(type), m_query(query) {}
	Type m_type;
	Ptr<IQuery> m_query;
};


/// QueryQueue processes queries in an asynchronous, time-sliced way, within a QueryQueueArray.
/// Each QueryQueue has its own WorkingMemory, time budget...
class QueryQueue : public RefCountBase<QueryQueue, MemStat_QuerySystem>
{
	KY_CLASS_WITHOUT_COPY(QueryQueue)

public:
	QueryQueue();
	virtual ~QueryQueue();

	void Init(const QueryQueueConfig& config);

	/// Must be called from the main thread.
	/// \pre    The query must be initialized and should not be already present in a QueryQueue.
	void PushBack(IQuery* query); 

	/// Must be called from the main thread.
	/// \pre    The query should have been pushed in this queue and not flushed yet.
	void Cancel(IQuery* query);

	/// Process the Queries in the QueryQueue until the budget is spent.
	/// The process can be called in a worker thread as long as Process() and World::Update are mutally exclusive.
	void Process();

	/// When QueryQueue is part of A QueryQueueArray and this QueryQueue runs within the World::Update(), QueryQueue::FlushCommands() is called at the beginning of World::Update().
	/// When QueryQueue is part of A QueryQueueArray and this QueryQueue runs outside the World::Update(), QueryQueue::FlushCommands() is called at the end of World::Update().
	void FlushCommands();

	/// When QueryQueue is part of A QueryQueueArray, QueryQueue::FlushQueries() is called in the World::Update() just after the World::ProcessQueuesInWorldUpdate() step.
	/// It calls IQuery::OnDone() on all the queries that finished.
	void FlushQueries();

	void SetBudgetMs(KyFloat32 budgetMs);
	KyFloat32 GetBudgetMs() const;

	void SetWorkingMemoryMaxSizeInBytes(KyUInt32 sizeInBytes);
	KyUInt32 GetWorkingMemoryMaxSizeInBytes() const;
	KyUInt32 GetWorkingMemoryCurrentSize() const;

	// Get pending Commands
	KyUInt32 GetCommandCount() const;
	QueryQueueCommand& GetCommand(KyUInt32 index);
	const QueryQueueCommand& GetCommand(KyUInt32 index) const;
	
	// Get pending Queries
	KyUInt32 GetQueryCount() const;
	IQuery* GetQuery(KyUInt32 index) const;

	KyUInt32 GetPendingOperationCount() const; ///< GetCommandCount() + GetQueryCount()

	const QueryQueueStats& GetStats() const; ///< Previous value of GetMsSpentInProcess(). Call this for statitics on the time spent in Process().

	const String& GetName() const { return m_name; }

	// Call only in WorldUpdate()
	void Clear();
	void CancelAllQueriesAndClearCommands(); // O(GetCount())

	/// When QueryQueue is part of A QueryQueueArray, call within QueryQueueArray::DoSendVisualDebug().
	void DoSendVisualDebug(VisualDebugServer& visualDebugServer, VisualDebugSendChangeEvent changeEvent, KyUInt32 queueIndex, KyUInt32 elementId, KyUInt32 queueArrayProcessMode);

protected:
	void ClearCommandForThisQuery(IQuery* query);  // O(GetCommandCount())
	void CancelQuery(IQuery* query);               // O(GetQueryCount())
	void YieldProcess(KyFloat64 msSpent);
	IQuery* FindFirstNonNullQuery();
	IQuery* FindNextNonNullQuery();

protected:
	CircularArray<QueryQueueCommand, MemStat_QuerySystem> m_commands; ///< Postpone PushBack() and Cancel() until next ConsumeCommands().
	CircularArray<Ptr<IQuery>, MemStat_QuerySystem > m_queries;       ///< Queries to process, this includes queries that have been canceled.
	KyUInt32 m_firstQueryToProcessIndex;                              ///< Current index of query to Process. Usefull for successive call to Process. For Internal Use.
	WorkingMemory m_workingMemory;                                    ///< Sandbox memory used to perform queries; allocates less memory, and in a thread-local manner.
	KyFloat32 m_msProcessBudget;                                      ///< The time in milliseconds this queue is allowed to use in a call to Process().
	KyFloat32 m_msSpentInProcess;                                     ///< Aggregates the actual time spent in Process(), set to zero in FlushQueries().
	QueryQueueStats m_stats;                                          ///< Remember statistics of this QueryQueue. For Visual Debug.
	String m_name;                                                    ///< The name of the QueryQueue.
};


KY_INLINE QueryQueue::QueryQueue()
	: m_firstQueryToProcessIndex(0)
	, m_msProcessBudget(1.0f)
	, m_msSpentInProcess(0.0f)
{}

KY_INLINE QueryQueue::~QueryQueue() { Clear(); }

KY_INLINE void                     QueryQueue::SetBudgetMs(KyFloat32 budgetMs)  { m_msProcessBudget = budgetMs;               }
KY_INLINE void                     QueryQueue::YieldProcess(KyFloat64 msSpent)  { m_msSpentInProcess += (KyFloat32)msSpent;   }
KY_INLINE KyFloat32                QueryQueue::GetBudgetMs()              const { return m_msProcessBudget;                   }
KY_INLINE const QueryQueueStats&   QueryQueue::GetStats()                 const { return m_stats;                             }
KY_INLINE KyUInt32                 QueryQueue::GetCommandCount()          const { return m_commands.GetCount();               }
KY_INLINE QueryQueueCommand&       QueryQueue::GetCommand(KyUInt32 index)       { return m_commands[index];                   }
KY_INLINE const QueryQueueCommand& QueryQueue::GetCommand(KyUInt32 index) const { return m_commands[index];                   }
KY_INLINE KyUInt32                 QueryQueue::GetQueryCount()            const { return m_queries.GetCount();                }
KY_INLINE IQuery*                  QueryQueue::GetQuery(KyUInt32 index)   const { return m_queries[index];                    }
KY_INLINE KyUInt32                 QueryQueue::GetPendingOperationCount() const { return GetQueryCount() + GetCommandCount(); }

} // namespace Kaim

#endif // Navigation_QueryQueue_H
