/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_BoxOfLists_H
#define GwNavGen_BoxOfLists_H


#include "gwnavgeneration/common/growinglistpool.h"


namespace Kaim
{


template <class T, KyUInt32 NbEl = 8>
class BoxOfLists
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavDataGen)
public:
	typedef GrowingListPool<T, NbEl> Pool;
	typedef typename Pool::List Column;

public:
	explicit BoxOfLists(MemoryHeap* heap, KyUInt32 byteCountInChunk = 0) :
		m_countX(0), m_countY(0), m_columns(KY_NULL), m_pool(heap, byteCountInChunk), m_heap(heap) {}

	void Init(KyUInt32 countX, KyUInt32 countY);

	KyUInt32 CountX() { return m_countX; }
	KyUInt32 CountY() { return m_countY; }
	KyUInt32 ColumnsCount() { return m_countX * m_countY; }

	T* GetNew(KyInt32 x, KyInt32 y) { return GetNew(y * m_countX + x); }
	T* GetNew(KyInt32 rowMajorIdx) { return GetOrCreateColumn(rowMajorIdx)->GetNew(); }

	Column* GetOrCreateColumn(KyInt32 x, KyInt32 y) { return GetOrCreateColumn(y * m_countX + x); }
	Column* GetOrCreateColumn(KyInt32 rowMajorIdx);

	const Column* GetColumn(KyInt32 x, KyInt32 y) const { return m_columns[y * m_countX + x]; }
	const Column* GetColumn(KyInt32 rowMajorIdx) const { return m_columns[rowMajorIdx]; }

	void Clear();

	void Release();

	KyUInt32 ByteCountAllocated() const { return m_pool.m_byteCountAllocated; }

	KyUInt32 ElementCount() const { return m_pool.ElementCount(); }

private:
	KyUInt32 m_countX;
	KyUInt32 m_countY;
	Column** m_columns; // row major
	Pool m_pool;
	MemoryHeap* m_heap;
};



// --------------------------------- inline implementation ---------------------------------

template <class T, KyUInt32 NbEl>
inline void BoxOfLists<T, NbEl>::Init(KyUInt32 countX, KyUInt32 countY)
{
	Clear();
	m_countX = countX;
	m_countY = countY;
	m_columns = KY_HEAP_MALLOC(m_heap, Column*,  countX * countY, Stat_Default_Mem);
	memset(m_columns, 0, countX * countY * sizeof(Column*));
}


template <class T, KyUInt32 NbEl>
inline typename BoxOfLists<T, NbEl>::Column* BoxOfLists<T, NbEl>::GetOrCreateColumn(KyInt32 rowMajorIdx)
{
	Column*& column = m_columns[rowMajorIdx];
	if (column == KY_NULL)
		column = m_pool.GetNewList();
	return column;
}


template <class T, KyUInt32 NbEl>
inline void BoxOfLists<T, NbEl>::Clear()
{
	if (m_columns != KY_NULL)
		KY_FREE(m_columns);
	m_columns = KY_NULL;

	m_countX = 0;
	m_countY = 0;

	m_pool.Clear();
}


template <class T, KyUInt32 NbEl>
inline void BoxOfLists<T, NbEl>::Release()
{
	Clear();
	m_pool.Release();
}


}


#endif
