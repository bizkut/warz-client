/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_IterablePool_H
#define Navigation_IterablePool_H

#include "gwnavruntime/containers/pool.h"
#include "gwnavruntime/containers/bitfield.h"
#include "gwnavruntime/containers/kyarray.h"

namespace Kaim
{


template <typename T> class IterablePool;

// MyPool<MyObject>::Iterator it(myPool);
// while (it.Next())
// { MyObject& o = it.Get(); }
template<typename T>
class IterablePool_Iterator
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	KY_INLINE IterablePool_Iterator();
	KY_INLINE IterablePool_Iterator(const IterablePool<T>* iterablePool) { Init(iterablePool); }
	KY_INLINE void Init(const IterablePool<T>* iterablePool);
	KY_INLINE bool Next();
	KY_INLINE T& Get() const { return *m_ptr; }

public:
	const IterablePool<T>* m_iterablePool;
	KyUInt32 m_usedSlotsCount;
	KyUInt32 m_usedSlotsIdx;
	KyUInt32 m_chunkCount;
	KyUInt32 m_chunkIdx;
	KyUInt32 m_idxInChunk;
	KyUInt32 m_globalIdx;
	T* m_ptr;
};


// Same Pool but
// - has a BitField to indicate what slots are filled/free, this allows to iterate on all elements in the Pool
// - destructor deletes all elements that are in the Pool
template <typename T>
class IterablePool
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_CLASS_WITHOUT_COPY(IterablePool)

public:
	typedef PoolHandle<T>  Handle;
	typedef PoolKey<T>     Key;
	typedef IterablePool_Iterator<T> Iterator;

private:
	// struct that allows to update the bitfield easily when a new element is added
	struct OnNew
	{
		OnNew(IterablePool* pool) : m_pool(pool), m_oldChunkCount(pool->GetChunkCount()) {}
		Key Do(Key compactKey);
		Handle Do(Handle handle);
		T* Do(T* ptr);
		IterablePool* m_pool;
		KyUInt32 m_oldChunkCount;
	};

public:
	explicit IterablePool(MemoryHeap* heap, KyInt32 memStat, PoolChunkSize::Mode chunkSizeMode = PoolChunkSize::ByteSize, KyUInt32 byteOrSlotCount = 256) :
		m_basePool(heap, memStat, chunkSizeMode, byteOrSlotCount) {}

	void Init(PoolChunkSize::Mode chunkSizeMode, KyUInt32 byteOrSlotCount) { m_basePool.Init(chunkSizeMode, byteOrSlotCount); }

	~IterablePool() { Clear(); }

	KY_INLINE KyUInt32 GetCount() const { return m_basePool.GetCount(); }

	void Clear();

	// -------- Key access --------
	KY_INLINE Key New_CompactKey()              { OnNew onNew(this); return onNew.Do(m_basePool.New_CompactKey());     }
	KY_INLINE Key New_CompactKey(const T& data) { OnNew onNew(this); return onNew.Do(m_basePool.New_CompactKey(data)); }

	KY_INLINE void New_CompactKeyAndPtr(Key& key, T*& ptr) { OnNew onNew(this); m_basePool.New_CompactKeyAndPtr(key, ptr); onNew.Do(key); }

	KY_INLINE const T& Get(Key key) const { return m_basePool.Get(key); }
	KY_INLINE T&       Get(Key key)       { return m_basePool.Get(key); }

	KY_INLINE void Delete(Key key)        { m_basePool.Delete(key); m_bitField.UnsetBit(GetGlobalIdx(key)); }

	// -------- Handle access --------
	KY_INLINE Handle New_Handle()                { OnNew onNew(this); return onNew.Do(m_basePool.New_Handle());     }
	KY_INLINE Handle New_Handle(const T& data)   { OnNew onNew(this); return onNew.Do(m_basePool.New_Handle(data)); }
	KY_INLINE void Delete(const Handle& handle)  { m_basePool.Delete(handle); m_bitField.UnsetBit(GetGlobalIdx(handle)); }

	// -------- ptr access Delete() is in O(nbChunks) --------
	KY_INLINE T* New_Ptr()                       { OnNew onNew(this); return onNew.Do(m_basePool.New_Ptr()); }
	KY_INLINE T* New_Ptr(const T& data)          { OnNew onNew(this); return onNew.Do(m_basePool.New_Ptr()); }
	KY_INLINE void Delete(T* ptr)                { m_basePool.Delete(ptr); m_bitField.UnsetBit(GetGlobalIdx(ptr)); }

	// -------- internal stuff --------
	KY_INLINE Key Handle2CompactKey(const Handle& h) const { return m_basePool.Handle2CompactKey(h); }
	KY_INLINE Handle CompactKey2Handle(Key key) const      { return m_basePool.CompactKey2Handle(key); }

	KY_INLINE KyUInt32 GetChunkCount() const     { return m_basePool.GetChunkCount(); }
	KY_INLINE KyUInt32 GetNbSlotsByChunk() const { return m_basePool.GetNbSlotsByChunk(); }

	KY_INLINE KyUInt32 Ptr2ChunkIdx(T* ptr) const { return m_basePool.Ptr2ChunkIdx(ptr); } // O(NbChunks)
	KY_INLINE KyUInt32 Ptr2IdxInChunk(KyUInt32 chunkIdx, T* ptr) const { return m_basePool.Ptr2IdxInChunk(chunkIdx, ptr); }

	KY_INLINE T& Get(KyUInt32 chunkIdx, KyUInt32 idxInChunk) const { return m_basePool.Get(chunkIdx, idxInChunk); }

	KY_INLINE KyUInt32 GetGlobalIdx(const Handle& handle) { return m_basePool.GetGlobalIdx(handle); }
	KY_INLINE KyUInt32 GetGlobalIdx(Key key)              { return m_basePool.GetGlobalIdx(key);    }
	KY_INLINE KyUInt32 GetGlobalIdx(T* ptr)               { return m_basePool.GetGlobalIdx(ptr);    }

	// internal
	KY_INLINE bool Next(Iterator& it) const;

public:
	Pool<T> m_basePool;
	BitField m_bitField;
};

} // namespace Kaim

#include "gwnavruntime/containers/iterablepool.inl"

#endif // Navigation_IterablePool_H
