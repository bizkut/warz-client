/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_WorkingMemory_H
#define Navigation_WorkingMemory_H

#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/kernel/SF_Debug.h"

namespace Kaim
{

typedef KyUInt32 BufferIndex;
static const BufferIndex BufferIndex_Invalid = KyUInt32MAXVAL;
static const BufferIndex BufferIndex_Count = 13;

class AStarTraversalContext;
class PathRefinerContext;
class PathClamperContext;
class NavDataChangeIndexInGrid;
class SpatializedPointCollectorContext;
class ChannelArrayComputer;
class DynamicNavMeshContext;

class WorkingMemory
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_WorldFwk)
	KY_CLASS_WITHOUT_COPY(WorkingMemory)

public:
	struct MyLimitHandler : public MemoryHeap::LimitHandler
	{
		virtual ~MyLimitHandler() {}
		virtual bool OnExceedLimit(MemoryHeap* /*heap*/, UPInt /*overLimit*/) { return false; };
		virtual void OnFreeSegment(MemoryHeap* /*heap*/, UPInt /*freeingSize*/) { };
	};

	struct WorkingBuffer
	{
		WorkingBuffer() : m_memory(KY_NULL), m_memorySize(0), m_inUse(false) {}

		void* m_memory;
		KyUInt32 m_memorySize;
		bool m_inUse;
	};

	/*
		\param memoryLimitSize				specifies the local heap memory allocation limit
		\param granularityOfBufferResize	specifies how the buffer size increase if more memory is needed
	*/
	WorkingMemory(KyUInt32 memoryLimitSize = 4 * 1024 * 1024 /* 4 Mb */, KyUInt32 granularityOfBufferResize = 5 * 1024/* 5 Kb */);
	~WorkingMemory();

	void Init(KyUInt32 memoryLimitSize, KyUInt32 granularityOfBufferResize);
	void InitMemoryHeap(KyUInt32 memoryLimitSize);

	void SetNewLimit(KyUInt32 memoryLimitSize);

	void* ResizeBuffer(BufferIndex bufferIdx, KyUInt32 newMinByteSize = 0);

	void ReleaseAllMemoryBuffer();
	void ReleaseUnusedMemoryBuffer();
	void ReleaseMemory(void* memory)
	{
		/*KY_DEBUG_MESSAGEN(("release %x\n", (int)memory));*/
		KY_HEAP_FREE(m_memoryHeap, memory);
	}

	void InitBuffersToNull();

	BufferIndex TakeUsageOfFirstUnusedBufferIdx();
	void ReleaseBuffer(BufferIndex bufferIdx);

	void* GetBuffer(BufferIndex bufferIdx) const;
	KyUInt32 GetBufferSize(BufferIndex bufferIdx) const;

	KyUInt32 GetTotalAllocatedSize() const;

	// Used by astarQuery
	AStarTraversalContext*            GetOrCreateAStarTraversalContext();
	PathRefinerContext*               GetOrCreatePathRefinerContext();
	PathClamperContext*               GetOrCreatePathClamperContext();
	NavDataChangeIndexInGrid*         GetOrCreateNavDataChangeIndexInGrid();
	ChannelArrayComputer*             GetOrCreateChannelArrayComputer();
	DynamicNavMeshContext*            GetOrCreateDynamicNavMeshContext();
	SpatializedPointCollectorContext* GetOrCreateSpatializedPointCollectorContext();
private:
	void CreateAllPathfinderContexts();
	void  CreateChannelArrayComputer();
	void CreateDynamicNavMeshContext();
	void CreateSpatializedPointCollectorContext();
public:
	MemoryHeap* m_memoryHeap;
	MyLimitHandler m_myLimitHandler;
	KyUInt32 m_granularityOfBufferResize;
	WorkingBuffer m_workingBuffer[BufferIndex_Count];

public:
	AStarTraversalContext* m_astarContext;
	PathRefinerContext* m_refinerContext;
	PathClamperContext* m_clamperContext;
	NavDataChangeIndexInGrid* m_navDataChangeIndexInGrid;
	ChannelArrayComputer* m_channelArrayComputer;

	SpatializedPointCollectorContext* m_collectorContext;

	DynamicNavMeshContext* m_dynamicNavMeshContext;
};

KY_INLINE void WorkingMemory::ReleaseBuffer(BufferIndex bufferIdx)
{
	KY_ASSERT(bufferIdx < BufferIndex_Count);
	m_workingBuffer[bufferIdx].m_inUse = false;
}
KY_INLINE void* WorkingMemory::GetBuffer(BufferIndex bufferIdx) const
{
	KY_ASSERT(bufferIdx < BufferIndex_Count);
	return m_workingBuffer[bufferIdx].m_memory;
}

KY_INLINE KyUInt32 WorkingMemory::GetBufferSize(BufferIndex bufferIdx) const
{
	KY_ASSERT(bufferIdx < BufferIndex_Count);
	return m_workingBuffer[bufferIdx].m_memorySize;
}



KY_INLINE AStarTraversalContext*    WorkingMemory::GetOrCreateAStarTraversalContext()
{
	if (m_astarContext == KY_NULL)
		CreateAllPathfinderContexts();
	return m_astarContext;
}
KY_INLINE PathRefinerContext*       WorkingMemory::GetOrCreatePathRefinerContext()
{
	if (m_refinerContext == KY_NULL)
		CreateAllPathfinderContexts();
	return m_refinerContext;
}
KY_INLINE PathClamperContext*       WorkingMemory::GetOrCreatePathClamperContext()
{
	if (m_clamperContext == KY_NULL)
		CreateAllPathfinderContexts();
	return m_clamperContext;
}
KY_INLINE NavDataChangeIndexInGrid* WorkingMemory::GetOrCreateNavDataChangeIndexInGrid()
{
	if (m_navDataChangeIndexInGrid == KY_NULL)
		CreateAllPathfinderContexts();
	return m_navDataChangeIndexInGrid;
}
KY_INLINE ChannelArrayComputer*     WorkingMemory::GetOrCreateChannelArrayComputer()
{
	if (m_channelArrayComputer == KY_NULL)
		CreateChannelArrayComputer();
	return m_channelArrayComputer;
}
KY_INLINE DynamicNavMeshContext*    WorkingMemory::GetOrCreateDynamicNavMeshContext()
{
	if (m_dynamicNavMeshContext == KY_NULL)
		CreateDynamicNavMeshContext();
	return m_dynamicNavMeshContext;
}
KY_INLINE SpatializedPointCollectorContext* WorkingMemory::GetOrCreateSpatializedPointCollectorContext()
{
	if (m_collectorContext == KY_NULL)
		CreateSpatializedPointCollectorContext();
	return m_collectorContext;
}

}

#endif //Navigation_NavMeshQueryBuffer_H

