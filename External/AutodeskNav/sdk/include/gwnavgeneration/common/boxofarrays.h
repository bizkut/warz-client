/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_BoxOfArrays_H
#define GwNavGen_BoxOfArrays_H


#include "gwnavgeneration/common/growingsmallbuffer.h"


namespace Kaim
{

/*  So T must be a POD with a default constructor.
	T() constructor is called when InitColumn() is called.
	~T() destructor is never called. */
template <class T>
class BoxOfArrays
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	class Column
	{
		KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	public:
		void Init(KyUInt32 count_, T* values_); // T() constructor called
	public:
		KyUInt32 m_count;
		T* m_values;
	};

public:
	explicit BoxOfArrays(MemoryHeap* heap, KyUInt32 byteCountInChunk = 0) :
		m_countX(0), m_countY(0), m_columns(KY_NULL), m_pool(heap, byteCountInChunk), m_heap(heap) {}

	~BoxOfArrays() { Release(); }

	void Init(KyUInt32 countX, KyUInt32 countY);

	KyUInt32 CountX() const { return m_countX; }
	KyUInt32 CountY() const { return m_countY; }
	KyUInt32 ColumnsCount() const { return m_countX * m_countY; }

	T* InitColumn(const Vec2i& pos, KyUInt32 count)     { return InitColumn(pos.x, pos.y, count); }
	T* InitColumn(KyInt32 x, KyInt32 y, KyUInt32 count) { return InitColumn(y * m_countX + x, count); }
	T* InitColumn(KyInt32 columnIdx, KyUInt32 count);

	Column& GetColumn(const Vec2i& pos)    { return GetColumn(pos.x, pos.y);     }
	Column& GetColumn(KyInt32 x, KyInt32 y) { return GetColumn(y * m_countX + x); }
	Column& GetColumn(KyInt32 rowMajorIdx)  { return m_columns[rowMajorIdx];      }

	const Column& GetColumn(const Vec2i& pos)    const { return GetColumn(pos.x, pos.y);     }
	const Column& GetColumn(KyInt32 x, KyInt32 y) const { return GetColumn(y * m_countX + x); }
	const Column& GetColumn(KyInt32 rowMajorIdx)  const { return m_columns[rowMajorIdx];      }

	const Column* GetColumns() const { return m_columns; }

	void Clear();

	void Release();

	KyUInt32 ByteCountAllocated() const { return m_pool.ByteCountAllocated(); }

	BoxOfArrays<T>& operator=(const BoxOfArrays<T>& other);

private:
	KyUInt32 m_countX;
	KyUInt32 m_countY;
	Column* m_columns; // row major
	GrowingSmallBufferPool m_pool;
	MemoryHeap* m_heap;

private:
	BoxOfArrays<T>(const BoxOfArrays<T>& other);
};



// --------------------------------- inline implementation ---------------------------------

template <class T>
void BoxOfArrays<T>::Column::Init(KyUInt32 count_, T* values_)
{
	m_count = count_;
	m_values = values_;

	// call constructor on each element of the new buffer
	for (KyUInt32 i = 0; i < m_count; ++i)
		::new(m_values + i) T();
}


template <class T>
void BoxOfArrays<T>::Init(KyUInt32 countX, KyUInt32 countY)
{
	Clear();
	m_countX = countX;
	m_countY = countY;
	m_columns = KY_HEAP_MALLOC(m_heap, Column, countX * countY, MemStat_NavDataGen);
	memset(m_columns, 0, countX * countY * sizeof(Column)); // sets m_columns[i].count = 0 and m_columns[i].values = KY_NULL
}

template <class T>
T* BoxOfArrays<T>::InitColumn(KyInt32 columnIdx, KyUInt32 count)
{
	T* values = (T*)m_pool.GetNewBuffer(count * sizeof(T));
	GetColumn(columnIdx).Init(count, values);
	return values;
}

template <class T>
void BoxOfArrays<T>::Clear()
{
	if (m_columns != KY_NULL)
		KY_FREE(m_columns);
	m_columns = KY_NULL;

	m_countX = 0;
	m_countY = 0;

	m_pool.Clear();
}

template <class T>
void BoxOfArrays<T>::Release()
{
	Clear();
	m_pool.Release();
}


template <class T>
BoxOfArrays<T>& BoxOfArrays<T>::operator=(const BoxOfArrays<T>& other)
{
	Init(other.CountX(), other.CountY());

	const Column* otherColumns = other.GetColumns();
	KyUInt32 columnsCount = ColumnsCount();

	for (KyUInt32 columnIdx = 0; columnIdx < columnsCount; ++columnIdx)
	{
		const Column& otherColumn = otherColumns[columnIdx];
		T* values = InitColumn(columnIdx, otherColumn.m_count);
		memcpy(values, otherColumn.m_values, otherColumn.m_count * sizeof(T));
	}
	return *this;
}


}


#endif
