/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: MAMU - secondary contact: NOBODY
#ifndef Navigation_QueryQueueBlob_H
#define Navigation_QueryQueueBlob_H

#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/world/runtimeblobcategory.h"

namespace Kaim
{

class QueryQueue;

class QueryQueueBlob
{
	KY_ROOT_BLOB_CLASS(Runtime, QueryQueueBlob, 0)
public:
	QueryQueueBlob()
		: m_queueIndex(0)
		, m_elementId(0)
		, m_budgetInMilliseconds(0.0f)
		, m_workingMemoryMaxSizeInBytes(0)
	{}

	KyUInt32        m_queueIndex;
	KyUInt32        m_elementId;
	KyUInt32        m_queueArrayProcess;
	KyFloat32       m_budgetInMilliseconds;
	KyUInt32        m_workingMemoryMaxSizeInBytes;
	BlobArray<char> m_queueName;

	// Stats
	KyUInt32        m_queriesPopped;
	KyUInt32        m_queriesInQueue;
	KyFloat32       m_timeSpent;
	KyFloat32       m_maxTimeSpent;
	KyFloat32       m_averageTimeSpent;
	KyUInt32        m_allocatedWorkingMemoryInBytes;
};

inline void SwapEndianness(Endianness::Target e, QueryQueueBlob& self)
{
	SwapEndianness(e, self.m_queueIndex);
	SwapEndianness(e, self.m_elementId);
	SwapEndianness(e, self.m_queueArrayProcess);
	SwapEndianness(e, self.m_budgetInMilliseconds);
	SwapEndianness(e, self.m_workingMemoryMaxSizeInBytes);
	SwapEndianness(e, self.m_queueName);

	SwapEndianness(e, self.m_queriesPopped);
	SwapEndianness(e, self.m_queriesInQueue);
	SwapEndianness(e, self.m_timeSpent);
	SwapEndianness(e, self.m_maxTimeSpent);
	SwapEndianness(e, self.m_averageTimeSpent);
	SwapEndianness(e, self.m_allocatedWorkingMemoryInBytes);
}

class QueryQueueBlobBuilder : public BaseBlobBuilder< QueryQueueBlob >
{
	KY_CLASS_WITHOUT_COPY(QueryQueueBlobBuilder)
public:
	QueryQueueBlobBuilder(const QueryQueue* queryQueue, KyUInt32 queryQueueArrayIndex, KyUInt32 queryQueueIndex, KyUInt32 queueArrayProcess)
		: m_queryQueue(queryQueue), m_queryQueueArrayIndex(queryQueueArrayIndex), m_queryQueueIndex(queryQueueIndex), m_queueArrayProcess(queueArrayProcess) {}

private:
	virtual void DoBuild();

	const QueryQueue* m_queryQueue;
	KyUInt32     m_queryQueueArrayIndex;
	KyUInt32     m_queryQueueIndex;
	KyUInt32     m_queueArrayProcess;
};

} // namespace Kaim

#endif // Navigation_QueryQueueBlob_H
