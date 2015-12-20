/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_WorkingMemDeque_H
#define Navigation_WorkingMemDeque_H

#include "gwnavruntime/querysystem/workingmemcontainers/workingmemcontainerbase.h"

namespace Kaim
{

template <class T>
class WorkingMemDeque
{
public:
	WorkingMemDeque();

	WorkingMemDeque(WorkingMemory*workingMemory);

	void Init(WorkingMemory* workingMemory);

	T*   GetBuffer()     const;
	bool IsInitialized() const;
	bool IsEmpty()       const;

	bool IsFull();
	void MakeEmpty();

	KyResult PushFront(const T& x);
	KyResult PushBack(const T& x);

	void PopFront();
	void PopBack();

	void Front(T& item) const;
	void Back(T& item)  const;

	KyUInt32 GetHeadIdx()    const;
	KyUInt32 GetTailEndIdx() const;

	KyUInt32 GetNextItemIdx(KyUInt32 idx) const;
	KyUInt32 GetPrevItemIdx(KyUInt32 idx) const;

	T& GetItem(KyUInt32 idx);

	KyResult TryToResize();

	void ReleaseWorkingMemoryBuffer() { m_workingMemContainerBase.ReleaseBuffer(); }
public :
	WorkingMemContainerBase m_workingMemContainerBase;
	KyUInt32 m_headIdx;
	KyUInt32 m_tailIdx;
	KyUInt32 m_lastIdx; // capacity - 1
};

}

#include "gwnavruntime/querysystem/workingmemcontainers/workingmemdeque.inl"

#endif

