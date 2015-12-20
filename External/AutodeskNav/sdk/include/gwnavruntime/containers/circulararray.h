/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_CircularArray_H
#define Navigation_CircularArray_H

#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/base/typetraits.h"
#include "gwnavruntime/math/fastmath.h"

namespace Kaim
{

template <class T, int SID=Stat_Default_Mem>
class CircularArray
{
	KY_DEFINE_NEW_DELETE_OPERATORS(SID)
	KY_CLASS_WITHOUT_COPY(CircularArray)
public:
	CircularArray(MemoryHeap* heap = KY_NULL);

	~CircularArray();

	KyUInt32 GetCount()    const;
	KyUInt32 GetCapacity() const;

	bool IsEmpty() const;
	bool IsFull()  const;

	void PushBack(const T& item);
	void PushFront(const T& item);

	void PopFront();
	void PopNFirstElements(KyUInt32 popCount);
	void PopBack();


	const T& GetBack() const;
	      T& GetBack();

	const T& GetFront() const;
	      T& GetFront();

	      T& operator [] (KyUInt32 index);
	const T& operator [] (KyUInt32 index) const;

	void Clear();
	void Reserve(KyUInt32 size);

protected:
	void ReserveInHeap(KyUInt32 size);

private:
	void AutoReserve() { if (IsFull()) Reserve(Max<KyUInt32>(4, 2 * GetCapacity())); }
	void ReleaseMemory();
	void IncreaseBeginIdx();
	void IncreaseEndIdx();
	void DecreaseBeginIdx();
	void DecreaseEndIdx();
private:
	T* m_buffer;
	MemoryHeap* const m_heap;
	KyUInt32 m_lastValidIdx; // capacity == m_lastValidIdx + 1;
	KyUInt32 m_beginIdx;
	KyUInt32 m_endIdx;
	KyUInt32 m_count;
};

} // namespace Kaim

#include "gwnavruntime/containers/circulararray.inl"

#endif // Navigation_CircularArray_H
