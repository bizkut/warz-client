/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


namespace Kaim
{

template <typename T>
Pool<T>::Pool(MemoryHeap* heap, KyInt32 memStat, PoolChunkSize::Mode chunkSizeMode, KyUInt32 byteOrSlotCount) :
	m_chunks(heap),
	m_lastAllocChunkIdx(KyUInt32MAXVAL),
	m_count(0),
	m_chunkSizeMode(PoolChunkSize::WaitInit),
	m_heap(heap),
	m_memStat(memStat)
{
	Init(chunkSizeMode, byteOrSlotCount);
}


template <typename T>
void Pool<T>::Init(PoolChunkSize::Mode chunkSizeMode, KyUInt32 byteOrSlotCount)
{
	if (m_chunkSizeMode != PoolChunkSize::WaitInit)
		return; // if already init, do nothing

	if (chunkSizeMode == PoolChunkSize::WaitInit)
		return; // if want to wait the actual init, do nothing

	m_chunkSizeMode = chunkSizeMode;

	if (chunkSizeMode == PoolChunkSize::SlotCount)
		m_chunkByteSize = byteOrSlotCount * sizeof(T);
	else // (chunkSizeMode == PoolChunkSize::ByteSize)
		m_chunkByteSize = byteOrSlotCount;

	m_nbSlotsByChunk = m_chunkByteSize / sizeof(T);

	KY_DEBUG_ASSERTN(sizeof(T) >= 4, ("Pool<T> requires sizeof(T) >= 4"));
	KY_DEBUG_ASSERTN(m_chunkByteSize >= sizeof(T), ("chunkByteSize must be >= sizeof(T)"));
}


template <typename T>
Pool<T>::~Pool()
{
	for (KyUInt32 i = 0; i < m_chunks.GetCount(); ++i)
		delete m_chunks[i];
}


template <typename T>
void Pool<T>::NewSlot(Slot& slot)
{
	++m_count;

	// if it's not full, alloc from the chunk in which the last alloc occurred
	if (m_lastAllocChunkIdx < m_chunks.GetCount())
	{
		ChunkT* chunk = m_chunks[m_lastAllocChunkIdx];
		if (chunk->IsFull() == false)
		{
			slot.Set(m_lastAllocChunkIdx, chunk, chunk->New());
			return;
		}
	}

	// find a chunk that is not full
	for (KyUInt32 chunkIndex = 0; chunkIndex < m_chunks.GetCount(); ++chunkIndex)
	{
		ChunkT* chunk = m_chunks[chunkIndex];
		if (chunk->IsFull() == false)
		{
			m_lastAllocChunkIdx = chunkIndex;
			slot.Set(chunkIndex, chunk, chunk->New());
			return;
		}
	}

	// all chunks all full, create a new chunk
	KyUInt32 chunkIndex = m_chunks.GetCount();
	m_lastAllocChunkIdx = chunkIndex;
	ChunkT* chunk = PushBackNewChunk();
	slot.Set(chunkIndex, chunk, chunk->New());
}


template <typename T>
void Pool<T>::New_CompactKeyAndPtr(Key& key, T*& ptr)
{
	Slot slot;
	NewSlot(slot);

	Slot2CompactKey(slot, key);
	ptr = slot.m_value;
}


template <typename T>
void Pool<T>::Slot2CompactKey(const Slot& slot, Key& key)
{
	key.m_chunkIdx = (KyUInt16)slot.m_chunkIdx;
	key.m_idxInChunk = (KyUInt16)slot.m_chunk->GetIdxInChunk(slot.m_value);
}


template <typename T>
typename Pool<T>::Key Pool<T>::New_CompactKey()
{
	Slot slot;
	NewSlot(slot);
	Key key;
	Slot2CompactKey(slot, key);
	return key;
}


template <typename T>
typename Pool<T>::Key Pool<T>::New_CompactKey(const T& data)
{
	Slot slot;
	NewSlot(slot);
	*slot.m_value = data;
	Key key;
	Slot2CompactKey(slot, key);
	return key;
}


template <typename T>
typename Pool<T>::Handle Pool<T>::New_Handle()
{
	Slot slot;
	NewSlot(slot);
	return Handle(slot.m_chunkIdx, slot.m_value);
}


template <typename T>
T* Pool<T>::New_Ptr()
{
	Slot slot;
	NewSlot(slot);
	return slot.m_value;
}


template <typename T>
T* Pool<T>::New_Ptr(const T& data)
{
	Slot slot;
	NewSlot(slot);
	*slot.m_value = data;
	return slot.m_value;
}


template <typename T>
typename Pool<T>::Handle Pool<T>::New_Handle(const T& data)
{
	Slot slot;
	NewSlot(slot);
	*slot.m_value = data;
	return Handle(slot.m_chunkIdx, slot.m_value);
}


template <typename T>
KyUInt32 Pool<T>::Ptr2ChunkIdx(T* ptr) const
{
	for (KyUInt32 i = 0; i < m_chunks.GetCount(); ++i)
	{
		if (m_chunks[i]->IsPtrInChunk(ptr))
			return i;
	}
	return KyUInt32MAXVAL;
}


template <typename T>
typename Pool<T>::ChunkT* Pool<T>::PushBackNewChunk()
{
	ChunkT* newChunk = KY_HEAP_NEW(m_heap) ChunkT(m_heap, m_nbSlotsByChunk, m_memStat);
	m_chunks.PushBack(newChunk);
	return newChunk;
}


} // namespace Kaim


