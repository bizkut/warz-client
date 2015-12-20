/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


namespace Kaim
{

template <typename T>
PoolKey<T> IterablePool<T>::OnNew::Do(Key compactKey)
{
	if (m_oldChunkCount != m_pool->GetChunkCount())
		m_pool->m_bitField.Resize(m_pool->GetChunkCount() * m_pool->GetNbSlotsByChunk());
	m_pool->m_bitField.SetBit(m_pool->GetGlobalIdx(compactKey));
	return compactKey;
}

template <typename T>
PoolHandle<T> IterablePool<T>::OnNew::Do(Handle handle)
{
	if (m_oldChunkCount != m_pool->GetChunkCount())
		m_pool->m_bitField.Resize(m_pool->GetChunkCount() * m_pool->GetNbSlotsByChunk());
	m_pool->m_bitField.SetBit(m_pool->GetGlobalIdx(handle));
	return handle;
}

template <typename T>
T* IterablePool<T>::OnNew::Do(T* ptr)
{
	if (m_oldChunkCount != m_pool->GetChunkCount())
		m_pool->m_bitField.Resize(m_pool->GetChunkCount() * m_pool->GetNbSlotsByChunk());
	m_pool->m_bitField.SetBit(m_pool->GetGlobalIdx(ptr));
	return ptr;
}

template <typename T>
IterablePool_Iterator<T>::IterablePool_Iterator() :
	m_iterablePool(KY_NULL),
	m_usedSlotsCount(KyUInt32MAXVAL), m_usedSlotsIdx(KyUInt32MAXVAL),
	m_chunkCount(KyUInt32MAXVAL),
	m_chunkIdx(KyUInt32MAXVAL), m_idxInChunk(KyUInt32MAXVAL), m_globalIdx(KyUInt32MAXVAL),
	m_ptr(KY_NULL)
{}


template <typename T>
void IterablePool_Iterator<T>::Init(const IterablePool<T>* iterablePool)
{
	m_iterablePool = iterablePool;
	m_usedSlotsCount = m_iterablePool->GetCount();
	m_usedSlotsIdx = 0;
	m_chunkCount = m_iterablePool->GetChunkCount();
	m_globalIdx = KyUInt32MAXVAL;
	m_ptr = KY_NULL;

	// trick to use Next() to get the first element
	m_chunkIdx = KyUInt32MAXVAL;
	m_idxInChunk = m_iterablePool->GetNbSlotsByChunk() - 1;
}


template <typename T>
bool IterablePool_Iterator<T>::Next()
{
	KyUInt32 nbSlotByChunk = m_iterablePool->GetNbSlotsByChunk();

	for (;;)
	{
		++m_globalIdx;
		++m_idxInChunk;
		if (m_idxInChunk == nbSlotByChunk)
		{
			++m_chunkIdx;
			m_idxInChunk = 0;
			if (m_chunkIdx == m_chunkCount)
				return false; // END
		}

		if (m_iterablePool->m_bitField.IsBitSet(m_globalIdx))
		{
			m_ptr = &m_iterablePool->Get(m_chunkIdx, m_idxInChunk);
			return true;
		}
	}
}

template <typename T>
void IterablePool<T>::Clear()
{
	if (m_basePool.GetCount() == 0)
		return;

	Iterator it(this);
	while (it.Next())
		m_basePool.Delete(Handle(it.m_chunkIdx, it.m_ptr));

	m_bitField.Clear();
}


} // namespace Kaim

