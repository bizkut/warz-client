/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_AsyncQueryDispatcher_H
#define Navigation_AsyncQueryDispatcher_H


#include "gwnavruntime/querysystem/queryqueuearray.h"
#include "gwnavruntime/querysystem/queryqueue.h"


namespace Kaim
{

class Bot;

/// Typically there is one QueryQueue for each AsyncQueryDispatchId
enum AsyncQueryDispatchId
{
	AsyncQueryDispatchId_Default        = 0,
	AsyncQueryDispatchId_PathFinder     = 1,
	AsyncQueryDispatchId_DynamicNavMesh = 2,
	

	AsyncQueryDispatchId_Count
};


enum AsyncQueryDispatcherType
{
	AsyncQueryDispatcherType_Unknown = 0,
	AsyncQueryDispatcherType_Default,
	AsyncQueryDispatcherType_FirstCustom
};

/// World::PushAsyncQuery(IQuery* query) pushes the Query in the QueryQueue.
/// IAsyncQueryDispatcher chooses a QueryQueue from those available in the World.
class IAsyncQueryDispatcher : public RefCountBase<IAsyncQueryDispatcher, MemStat_QuerySystem>
{
public:
	IAsyncQueryDispatcher() : m_world(KY_NULL) {}

	void SetWorld(World* world) { m_world = world; }
	World* GetWorld() { return m_world; }

	virtual void AddQueryQueuesToWorld() = 0;
	virtual void RemoveQueryQueuesFromWorld() = 0;

	virtual QueryQueue* GetQueue(IQuery* query, AsyncQueryDispatchId asyncQueryDispatchId = AsyncQueryDispatchId_Default, Bot* bot = KY_NULL) = 0;

	virtual ~IAsyncQueryDispatcher() {}

protected:
	World* m_world;
};


class AsyncQueryDispatcherInitConfig
{
public:
	// By default, only one queryQueue is created in the mainThread
	AsyncQueryDispatcherInitConfig()
		: m_createWorkerQueryQueueArray(false)
		, m_useWorkerQueryQueueByDefault(false)
		, m_usePathFinderQueryQueue(true)
		, m_useDynamicNavMeshQueryQueue(true)
	{}

	bool m_createWorkerQueryQueueArray;
	bool m_useWorkerQueryQueueByDefault;

	bool m_usePathFinderQueryQueue;     // if false create only one queue: m_defaultQueryQueueConfig
	bool m_useDynamicNavMeshQueryQueue; // if false create only two queues: m_defaultQueryQueueConfig and m_pathFinderQueryQueueConfig

	QueryQueueConfig m_defaultQueryQueueConfig;        // will be queue[0]
	QueryQueueConfig m_pathFinderQueryQueueConfig;     // will be queue[1]
	QueryQueueConfig m_dynamicNavMeshQueryQueueConfig; // will be queue[2]
};

/// Default implementation of IAsyncQueryDispatcher
class AsyncQueryDispatcher : public IAsyncQueryDispatcher
{
public:
	AsyncQueryDispatcher(const AsyncQueryDispatcherInitConfig& initConfig);

	virtual ~AsyncQueryDispatcher();

	virtual void AddQueryQueuesToWorld();
	virtual void RemoveQueryQueuesFromWorld();

	virtual QueryQueue* GetQueue(IQuery* query, AsyncQueryDispatchId asyncQueryDispatchId = AsyncQueryDispatchId_Default, Bot* bot = KY_NULL);

	// change on the fly 
	void UseWorker(bool useWorkerQueryQueue) { m_useWorkerQueryQueue = useWorkerQueryQueue; }

private:
	void InitQueryQueueArray(QueryQueueArray* queryQueueArray, QueryQueueArrayProcess queryQueueArrayProcessThread);
	QueryQueue* GetQueueInArray(QueryQueueArray* queryQueueArray, AsyncQueryDispatchId asyncQueryDispatchId);
	void RemoveQueryQueuesFromWorldImpl();

private:
	AsyncQueryDispatcherInitConfig m_initConfig;
	Ptr<QueryQueueArray> m_mainQueryQueueArray;
	Ptr<QueryQueueArray> m_workerQueryQueueArray;
	bool m_useWorkerQueryQueue;
};


} // namespace Kaim

#endif

