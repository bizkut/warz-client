/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_GrowingListPool_H
#define GwNavGen_GrowingListPool_H

#include "gwnavruntime/base/types.h"
#include "gwnavgeneration/common/growingpool.h"
#include "gwnavruntime/basesystem/logger.h"


namespace Kaim
{

// forward declare
template<class T, KyUInt32 NbEl> class GrowingListPool;


// ---------------------------------------------------------------------
template<class T, KyUInt32 NbEl = 8>
class GrowingListNode
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	GrowingListNode() : m_next(KY_NULL) {}

public:
	T m_values[NbEl];
	GrowingListNode<T, NbEl>* m_next;
};


// ---------------------------------------------------------------------
template<class T, KyUInt32 NbEl = 8>
class GrowingList
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	typedef GrowingListPool<T, NbEl> Pool;
	typedef GrowingListNode<T, NbEl> Node;

public:
	GrowingList();

	void Init(Pool* pool);

	T* GetNew();

	void PushBack(const T& value) { *GetNew() = value; }

	KyInt32 Count() const { return m_count; }

	void ToArray(T* values) const;

private:
	Pool* m_pool;
	KyUInt32 m_count;
	Node* m_first;
	Node* m_last;
	KyUInt32 m_countInLast;
};


// ---------------------------------------------------------------------
template<class T, KyUInt32 NbEl = 8>
class GrowingListPool
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	typedef GrowingListNode<T, NbEl> Node;
	typedef GrowingList<T, NbEl> List;

	friend class GrowingList<T, NbEl>;
public:
	explicit GrowingListPool(MemoryHeap* heap, KyUInt32 byteCountInChunk = 0) :
		m_lists(heap, byteCountInChunk), m_nodes(heap, byteCountInChunk), m_elementCount(0), m_heap(heap) {}

	List* GetNewList();

	KyUInt32 GetListChunkCount() { return m_lists.GetChunkCount(); }

	void Clear();

	void Release();

	KyUInt32 ByteCountAllocated() const;

	KyUInt32 ElementCount() const { return m_elementCount; }

private:
	Node* GetNewNode();
private:
	GrowingPool<List> m_lists;
	GrowingPool<Node> m_nodes;
	KyUInt32 m_elementCount;
	MemoryHeap* m_heap;
};


template<class T, KyUInt32 NbEl>
GrowingList<T, NbEl>::GrowingList()
{
	Init(KY_NULL);
}


template<class T, KyUInt32 NbEl>
void GrowingList<T, NbEl>::Init(Pool* pool)
{
	m_pool = pool;
	m_count = 0;
	m_first = KY_NULL;
	m_last = KY_NULL;
	m_countInLast = 0;
}


template<class T, KyUInt32 NbEl>
void GrowingList<T, NbEl>::ToArray(T* values) const
{
	if (m_count == 0)
		return;

	KyUInt32 idx = 0;
	for (GrowingListNode<T, NbEl>* node = m_first; node != m_last; node = node->m_next)
	{
		for (KyUInt32 i = 0; i < NbEl; ++i, ++idx)
			values[idx] = node->m_values[i];
	}
	for (KyUInt32 i = 0; i < m_countInLast; ++i, ++idx)
		values[idx] = m_last->m_values[i];
}


template<class T, KyUInt32 NbEl>
T* GrowingList<T, NbEl>::GetNew()
{
	if (m_first == KY_NULL)
	{
		m_last = m_first = m_pool->GetNewNode();
	}
	else if (m_countInLast == NbEl)
	{
		GrowingListNode<T, NbEl>* newNode = m_pool->GetNewNode();
		m_last->m_next = newNode;
		m_last = newNode;
		m_countInLast = 0;
	}


	KY_DEBUG_ASSERTN(m_countInLast >= 0 && m_countInLast < NbEl, ("Can not get a new element"));

	T* value = &m_last->m_values[m_countInLast];

	++m_countInLast;
	++m_count;
	++m_pool->m_elementCount;

	return value;
}


template<class T, KyUInt32 NbEl>
GrowingList<T, NbEl>* GrowingListPool<T, NbEl>::GetNewList()
{
	List* list = m_lists.GetNew();
	list->Init(this);
	return list;
}


template<class T, KyUInt32 NbEl>
GrowingListNode<T, NbEl>* GrowingListPool<T, NbEl>::GetNewNode()
{
	Node* node = m_nodes.GetNew();
	return node;
}


template<class T, KyUInt32 NbEl>
void GrowingListPool<T, NbEl>::Clear()
{
	m_lists.Clear();
	m_nodes.Clear();
	m_elementCount = 0;
}

template<class T, KyUInt32 NbEl>
void GrowingListPool<T, NbEl>::Release()
{
	m_lists.Release();
	m_nodes.Release();
	m_elementCount = 0;
}


template<class T, KyUInt32 NbEl>
KyUInt32 GrowingListPool<T, NbEl>::ByteCountAllocated() const
{
	return m_lists.ByteCountAllocated() + m_nodes.ByteCountAllocated();
}


}


#endif
