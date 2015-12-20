/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_Pool_H
#define Navigation_Pool_H

#include "gwnavruntime/containers/poolchunk.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/kernel/SF_Debug.h"

namespace Kaim
{


// PoolHandle
// - allows Delete(Handle handle) in O(1)
// - has sizeof() = 8 on 32 bits
// - has sizeof() = 16 on 64 bits
// - points directly to the value
template <typename T>
class PoolHandle
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	KY_INLINE PoolHandle() : m_chunkIdx(KyUInt32MAXVAL), m_ptr(KY_NULL) {}
	KY_INLINE PoolHandle(KyUInt32 chunkIdx, T* value) : m_chunkIdx(chunkIdx), m_ptr(value) {}
	KY_INLINE PoolHandle(const PoolHandle& rhs) : m_chunkIdx(rhs.m_chunkIdx), m_ptr(rhs.m_ptr) {}

	KY_INLINE PoolHandle& operator=(const PoolHandle& rhs) { m_chunkIdx = rhs.m_chunkIdx; m_ptr = rhs.m_ptr; return *this; }

	KY_INLINE T& Get() const  { return *m_ptr; }
	KY_INLINE T& operator*()  { return *m_ptr; }
	KY_INLINE T* operator->() { return  m_ptr; }

	KY_INLINE void Invalidate() { m_chunkIdx = KyUInt32MAXVAL; m_ptr = KY_NULL; }
	KY_INLINE bool IsValid() const { return m_ptr != KY_NULL; }

	KY_INLINE bool operator==(const PoolHandle& rhs) const { return m_ptr == rhs.m_ptr; }
	KY_INLINE bool operator!=(const PoolHandle& rhs) const { return m_ptr != rhs.m_ptr; }

public:
	KyUInt32 m_chunkIdx;
	T* m_ptr;
};


// PoolKey
// - allows Delete(Key compactKey) in O(1)
// - has sizeof() = 4
// - requires 2 indirections to get the value
template <typename T>
class PoolKey
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	KY_INLINE PoolKey() : m_chunkIdx(KyUInt16MAXVAL), m_idxInChunk(KyUInt16MAXVAL) {}

	KY_INLINE PoolKey(KyUInt32 chunkIdx, KyUInt32 idxInChunk) :
		m_chunkIdx(KyUInt16(chunkIdx)), m_idxInChunk(KyUInt16(idxInChunk)) {}

	KY_INLINE PoolKey(const PoolKey& rhs) : m_chunkIdx(rhs.m_chunkIdx), m_idxInChunk(rhs.m_idxInChunk) {}

	KY_INLINE PoolKey& operator=(const PoolKey& rhs) { m_chunkIdx = rhs.m_chunkIdx; m_idxInChunk = rhs.m_idxInChunk; return *this; }

	KY_INLINE void Invalidate() { m_chunkIdx = KyUInt16MAXVAL; m_idxInChunk = KyUInt16MAXVAL; }
	KY_INLINE bool IsValid() const { return m_chunkIdx != KyUInt16MAXVAL && m_idxInChunk != KyUInt16MAXVAL; }

	KY_INLINE bool operator==(const PoolKey& rhs) const { return m_chunkIdx == rhs.m_chunkIdx && m_idxInChunk == rhs.m_idxInChunk; }
	KY_INLINE bool operator!=(const PoolKey& rhs) const { return !(*this == rhs); }

public:
	KyUInt16 m_chunkIdx;
	KyUInt16 m_idxInChunk;
};


struct PoolChunkSize
{
	enum Mode { ByteSize, SlotCount, WaitInit };
};


template <typename T>
class Pool
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_CLASS_WITHOUT_COPY(Pool)
	typedef PoolChunk<T> ChunkT;

public:
	typedef PoolHandle<T> Handle;
	typedef PoolKey<T>    Key;

public:
	explicit Pool(MemoryHeap* heap, KyInt32 memStat, PoolChunkSize::Mode chunkSizeMode = PoolChunkSize::ByteSize, KyUInt32 byteOrSlotCount = 256);

	void Init(PoolChunkSize::Mode chunkSizeMode, KyUInt32 byteOrSlotCount);

	~Pool();

	KY_INLINE KyUInt32 GetCount() const { return m_count; }

	// -------- Key access --------
	KY_INLINE Key New_CompactKey();
	KY_INLINE Key New_CompactKey(const T& data);

	KY_INLINE void New_CompactKeyAndPtr(Key& key, T*& ptr); 

	KY_INLINE const T& Get(Key key) const { return *m_chunks[key.m_chunkIdx]->Get(key.m_idxInChunk); }
	KY_INLINE T&       Get(Key key)       { return *m_chunks[key.m_chunkIdx]->Get(key.m_idxInChunk); }

	KY_INLINE void Delete(Key key) { --m_count; m_chunks[key.m_chunkIdx]->Delete(key.m_idxInChunk); }

	// -------- Handle access --------
	KY_INLINE Handle New_Handle();
	KY_INLINE Handle New_Handle(const T& data);
	KY_INLINE void Delete(const Handle& handle) { --m_count; return m_chunks[handle.m_chunkIdx]->Delete(handle.m_ptr); }

	// -------- ptr access Delete() is in O(nbChunks) --------
	KY_INLINE T* New_Ptr();
	KY_INLINE T* New_Ptr(const T& data);
	KY_INLINE void Delete(T* ptr) { --m_count; return m_chunks[Ptr2ChunkIdx(ptr)]->Delete(ptr); }

	// --------
	KY_INLINE Key Handle2CompactKey(const Handle& h) const { return Key(h.m_chunkIdx, Ptr2IdxInChunk(h.m_chunkIdx, h.m_ptr)); }
	KY_INLINE Handle CompactKey2Handle(Key key) const { return Handle(key.m_chunkIdx, m_chunks[key.m_chunkIdx]->Get(key.m_idxInChunk)); }

	KY_INLINE KyUInt32 GetChunkCount() const { return m_chunks.GetCount(); }
	KY_INLINE KyUInt32 GetNbSlotsByChunk() const { return m_nbSlotsByChunk; }

	KyUInt32 Ptr2ChunkIdx(T* ptr) const; // O(NbChunks)
	KY_INLINE KyUInt32 Ptr2IdxInChunk(KyUInt32 chunkIdx, T* ptr) const { return m_chunks[chunkIdx]->GetIdxInChunk(ptr); }

	KY_INLINE T& Get(KyUInt32 chunkIdx, KyUInt32 idxInChunk) const { return *m_chunks[chunkIdx]->Get(idxInChunk); }

	KY_INLINE KyUInt32 GetGlobalIdx(const Handle& handle) { return handle.m_chunkIdx  * m_nbSlotsByChunk + Ptr2IdxInChunk(handle.m_chunkIdx, handle.m_ptr); }
	KY_INLINE KyUInt32 GetGlobalIdx(Key key)              { return key.m_chunkIdx     * m_nbSlotsByChunk + key.m_idxInChunk; }
	KY_INLINE KyUInt32 GetGlobalIdx(T* ptr)               { KyUInt32 chunkIdx = Ptr2ChunkIdx(ptr); return chunkIdx * m_nbSlotsByChunk + Ptr2IdxInChunk(chunkIdx, ptr); }

private:
	ChunkT* PushBackNewChunk();

	struct Slot
	{
		void Set(KyUInt32 chunkIdx, ChunkT* chunk, KyUInt32 idxInChunk) { m_chunkIdx = chunkIdx; m_idxInchunk = idxInChunk; m_chunk = chunk; m_value = chunk->Get(idxInChunk); }
		KyUInt32 m_chunkIdx; KyUInt32 m_idxInchunk; ChunkT* m_chunk; T* m_value;
	};
	void NewSlot(Slot& slot);
	void Slot2CompactKey(const Slot& slot, Key& key);

private:
	KyArrayDH_POD<ChunkT*> m_chunks;
	KyUInt32 m_chunkByteSize;
	KyUInt32 m_lastAllocChunkIdx;
	KyUInt32 m_nbSlotsByChunk;
	KyUInt32 m_count;
	PoolChunkSize::Mode m_chunkSizeMode;
	MemoryHeap* m_heap;
	KyInt32 m_memStat;
};

} // namespace Kaim

#include "gwnavruntime/containers/pool.inl"

#endif
