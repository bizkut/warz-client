/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Authors: James Park, Guillaume Aldebert

#ifndef Navigation_QueryQueueArray_H
#define Navigation_QueryQueueArray_H


#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/world/worldelement.h"

namespace Kaim
{

class QueryQueue;
class World;


enum QueryQueueArrayProcess
{
	QueryQueueArrayProcess_InWorldUpdate,
	QueryQueueArrayProcess_OutOfWorldUpdate,
};


/// QueryQueueArray processes queries in an asynchronous, time-sliced way.
/// QueryQueueArray has no locking, it is not thread safe.
/// QueryQueueArray does not need to be thread-safe because Queries always read the world state.
/// As a result, QueryQueueArray::Process() must not be called simultaneously with World::Update()
/// Yet, one thread calls: PushBack() and Cancel(), while another thread calls Process().
class QueryQueueArray : public WorldElement
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_QuerySystem)
	KY_CLASS_WITHOUT_COPY(QueryQueueArray)

public:
	static  WorldElementType GetStaticType()       { return TypeQueryQueueArray; }
	virtual WorldElementType GetType()       const { return TypeQueryQueueArray; }
	virtual const char*      GetTypeName()   const { return "QueryQueueArray";   }

	QueryQueueArray();
	virtual ~QueryQueueArray() {}

	void Init(World* world, KyUInt32 queueCount, QueryQueueArrayProcess queryQueueArrayProcessThread);

	void AddToWorld();
	void RemoveFromWorld();

	void ProcessQueues();

	void FlushQueries();

	void FlushCommands();

	KyUInt32 GetQueueCount() { return m_queues.GetCount(); }
	QueryQueue* GetQueue(KyUInt32 index) { return m_queues[index]; }

	KyUInt32 GetPendingOperationCount();

	bool IsProcessedInWorldUpdate() const { return m_queryQueueArrayProcessThread == QueryQueueArrayProcess_InWorldUpdate; }

	virtual void DoSendVisualDebug(VisualDebugServer& server, VisualDebugSendChangeEvent changeEvent); // Inherited from WorldElement

	void CancelAllQueriesAndClearCommands();
private:
	KyArray<Ptr<QueryQueue>, MemStat_QuerySystem> m_queues;
	QueryQueueArrayProcess m_queryQueueArrayProcessThread;
};

} // namespace Kaim

#endif // Navigation_QueryQueueArray_H


