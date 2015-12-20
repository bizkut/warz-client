/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_GrowingPool_H
#define GwNavGen_GrowingPool_H


#include "gwnavgeneration/common/generatormemory.h"
#include "gwnavruntime/containers/kyarray.h"


namespace Kaim
{

/*
// To browse the GrowingPool use the following code
MyClass* chunk = KY_NULL;
KyUInt32 countInChunk = 0;
for (KyUInt32 chunkIdx = 0; chunkIdx < pool.GetChunkCount(); ++chunkIdx)
{
	pool.GetChunk(chunkIdx, chunk, countInChunk);
	for (KyUInt32 idx = 0; idx < countInChunk; ++idx)
	{
		MyClass& obj = chunk[idx];
	}
}
*/
template <class T>
class GrowingPool
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	explicit GrowingPool(MemoryHeap* heap, KyUInt32 byteCountInChunk = 0);

	~GrowingPool() { Release(); }

	// Does call T constructor. 
	T* GetNew();

	void Clear();

	void Release();

	KyUInt32 GetElementCount()    const { return m_elementCount;       }
	KyUInt32 GetChunkCount()      const { return m_chunkCount;         }
	KyUInt32 ByteCountAllocated() const { return m_byteCountAllocated; }

	void GetChunk(KyUInt32 idx, T*& chunk, KyUInt32& countInChunk);


private:
	KyUInt32 CalculateElementCountFromByteCount(KyInt32 byteCount);
	KyUInt32 CalculateWordCount(KyInt32 elementCount);

private:
	KyArrayDH_POD<T*> m_chunks;
	KyUInt32 m_chunkCount;
	KyUInt32 m_elementCount;
	KyUInt32 m_maxWordCountInChunk;
	KyUInt32 m_maxElementCountInChunk;
	KyUInt32 m_elementCountInLastChunk;
	KyUInt32 m_byteCountAllocated;
	MemoryHeap* m_heap;
};


template<class T>
inline GrowingPool<T>::GrowingPool(MemoryHeap* heap, KyUInt32 byteCountInChunk) : m_chunks(heap)
{
	if (byteCountInChunk <= 0)
		byteCountInChunk = 512 * 1024; // 512 KB by default

	m_chunks.Resize(16);
	for (KyUInt32 i = 0 ; i < m_chunks.GetCount(); ++i)
		m_chunks[i] = KY_NULL;

	m_chunkCount = 0;
	m_elementCount = 0;
	m_maxElementCountInChunk = CalculateElementCountFromByteCount(byteCountInChunk);
	m_maxWordCountInChunk = CalculateWordCount(m_maxElementCountInChunk);
	m_elementCountInLastChunk = m_maxElementCountInChunk;
	m_byteCountAllocated = 0;
	m_heap = heap;
}


template<class T>
inline T* GrowingPool<T>::GetNew()
{
	if (m_elementCountInLastChunk == m_maxElementCountInChunk) // we reached the end of the current chunk
	{
		KyUInt32 oldSize = m_chunks.GetCount();
		if (m_chunkCount >= oldSize) // no more chunks in m_chunks, resize it
		{
			m_chunks.Resize(oldSize * 2);
			for (KyUInt32 i = oldSize; i < m_chunks.GetCount(); ++i)
				m_chunks[i] = KY_NULL;
		}

		if (m_chunks[m_chunkCount] == KY_NULL) // need to allocate a new chunk
		{
			m_chunks[m_chunkCount] = (T*)KY_HEAP_MALLOC(m_heap, KyUInt32, m_maxWordCountInChunk, MemStat_NavDataGen);
			m_byteCountAllocated += m_maxWordCountInChunk * sizeof(KyUInt32);
		}

		++m_chunkCount;
		m_elementCountInLastChunk = 0;
	}

	++m_elementCount;
	
	T* ptr = &m_chunks[m_chunkCount - 1][m_elementCountInLastChunk];
	::new(ptr) T;

	++m_elementCountInLastChunk;
	return ptr;
}


template<class T>
inline void GrowingPool<T>::Clear()
{
	// call destructor on elements
	T* chunk = KY_NULL;
	KyUInt32 countInChunk = 0;

	for (KyUInt32 chunkIdx = 0; chunkIdx < GetChunkCount(); ++chunkIdx)
	{
		GetChunk(chunkIdx, chunk, countInChunk);
		for (KyUInt32 idx = 0; idx < countInChunk; ++idx)
			chunk[idx].~T();
	}

	m_elementCount = 0;
	m_chunkCount = 0;
	m_elementCountInLastChunk = m_maxElementCountInChunk;
}


template<class T>
inline void GrowingPool<T>::Release()
{
	Clear();

	for (KyUInt32 i = 0; i < m_chunks.GetCount(); ++i)
		KY_FREE(m_chunks[i]);

	m_chunks.ClearAndRelease();
	m_byteCountAllocated = 0;
}


template<class T>
KY_INLINE void GrowingPool<T>::GetChunk(KyUInt32 idx, T*& chunk, KyUInt32& countInChunk)
{
	chunk = m_chunks[idx];
	countInChunk = (idx != m_chunkCount - 1) ? m_maxElementCountInChunk : m_elementCountInLastChunk;
}


template<class T>
KY_INLINE KyUInt32 GrowingPool<T>::CalculateElementCountFromByteCount(KyInt32 byteCount)
{
	return (byteCount - 1) / (KyInt32)sizeof (T) + 1;
}

template<class T>
KY_INLINE KyUInt32 GrowingPool<T>::CalculateWordCount(KyInt32 elementCount)
{
	return (elementCount * (KyInt32)sizeof (T) - 1) / sizeof(KyUInt32) + 1;
}

} // namespace Kaim

#endif
