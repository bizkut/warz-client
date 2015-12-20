/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_PoolChunk_H
#define Navigation_PoolChunk_H

#include "gwnavruntime/basesystem/logger.h" // KY_DEBUG_ASSERT
#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/kernel/SF_Debug.h"

namespace Kaim
{

template<typename T>
class PoolChunk
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	explicit PoolChunk(MemoryHeap* heap, KyUInt32 slotCount, KyUInt32 memStat);

	~PoolChunk();

	KyUInt32 New();
	KyUInt32 New(const T& data);

	KY_INLINE void Delete(T* ptr) { Delete(GetIdxInChunk(ptr)); }
	void Delete(KyUInt32 index);

	const T* Get(KyUInt32 index) const { return ((T*)m_buffer) + index; }
	T*       Get(KyUInt32 index)       { return ((T*)m_buffer) + index; }

	KY_INLINE bool IsFull()  const { return (m_nbFreeSlots == 0); }
	KY_INLINE bool IsEmpty() const { return (m_nbFreeSlots == GetNbSlots()); }

	KY_INLINE bool IsPtrInChunk(const T* ptr) const { return ((T*)m_buffer) <= ptr && ptr < (((T*)m_buffer) + m_nbTotalSlots); }

	KY_INLINE KyUInt32 GetNbSlots() const { return m_nbTotalSlots; }
	KY_INLINE KyUInt32 GetNbUsedSlots() const { return m_nbTotalSlots - m_nbFreeSlots; }

	KyUInt32 GetIdxInChunk(const T* ptr) const { return KyUInt32(ptr - (T*)m_buffer); }

	bool SuperSlow_IsSlotFree(KyUInt32 index) const;

private:

	KyUInt32 GetNextFreeSlot(KyUInt32 index) const                   { return *((KyUInt32*) (m_buffer + (sizeof(T) * index))); }
	void     SetNextFreeSlot(KyUInt32 index, KyUInt32 freeSlotIndex) { *((KyUInt32*) (m_buffer + (sizeof(T) * index))) = freeSlotIndex; }

	KyUInt32 GetFreeSlotAndMakeUsed();
	void MakeSlotFree(KyUInt32 index);

private:
	char*    m_buffer;
	KyUInt32 m_nbFreeSlots;
	KyUInt32 m_nbTotalSlots;
	KyUInt32 m_firstFreeSlot;
};



template<typename T>
PoolChunk<T>::PoolChunk(MemoryHeap* heap, KyUInt32 slotCount, KyUInt32 memStat) :
	m_buffer((char*)KY_HEAP_ALLOC(heap, (sizeof(T) * slotCount), memStat)),
	m_nbFreeSlots(slotCount),
	m_nbTotalSlots(slotCount),
	m_firstFreeSlot(0)
{
	KY_DEBUG_ASSERTN(sizeof (T) >= 4, ("Type should be bigger to enable some functionalities"));
	KY_DEBUG_ASSERTN(slotCount != 0, ("Bad parameter"));

	KY_UNUSED(memStat);

	for (KyUInt32 i = 0; i < slotCount; ++i)
		SetNextFreeSlot(i, i + 1);
}


template<typename T>
PoolChunk<T>::~PoolChunk()
{
	if (IsEmpty() == false)
	{
		KY_LOG_WARNING(("Bad usage of PoolChunk : you should call 'Delete' for each 'New'."));
	}
	KY_FREE(m_buffer);
}


template<typename T>
KyUInt32 PoolChunk<T>::New()
{
	KY_ASSERT(IsFull() == false);

	const KyUInt32 index = GetFreeSlotAndMakeUsed();
	T* mem = Get(index);

	new (mem) T;
	return index;
}

template<typename T>
KyUInt32 PoolChunk<T>::New(const T& data)
{
	KY_ASSERT(IsFull() == false);

	const KyUInt32 index = GetFreeSlotAndMakeUsed();
	T* mem = Get(index);

	new (mem) T(data);
	return index;
}

template<typename T>
void PoolChunk<T>::Delete(KyUInt32 index)
{
	KY_ASSERT(index < m_nbTotalSlots);
	//KY_DEBUG_ASSERTN(!IsFree(index), ("Delete multiple time the same element")); // slow
	Get(index)->~T();
	MakeSlotFree(index);
}


template<typename T>
bool PoolChunk<T>::SuperSlow_IsSlotFree(KyUInt32 index) const
{
	KyUInt32 freeSlotIdx = m_firstFreeSlot;

	for (KyUInt32 i = 0; i != m_nbFreeSlots; ++i)
	{
		if (freeSlotIdx == index)
			return true;
		freeSlotIdx = GetNextFreeSlot(freeSlotIdx);
	}
	return false;
}


template<typename T>
KyUInt32 PoolChunk<T>::GetFreeSlotAndMakeUsed()
{
	const KyUInt32 res = m_firstFreeSlot;
	m_firstFreeSlot = GetNextFreeSlot(m_firstFreeSlot);
	--m_nbFreeSlots;
	return res;
}


template<typename T>
void PoolChunk<T>::MakeSlotFree(KyUInt32 index)
{
	SetNextFreeSlot(index, m_firstFreeSlot);
	m_firstFreeSlot = index;
	++m_nbFreeSlots;
}


} // namespace Kaim

#endif // Navigation_PoolChunk_H
