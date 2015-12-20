/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_WorkingMemArray_H
#define Navigation_WorkingMemArray_H

#include "gwnavruntime/querysystem/workingmemcontainers/workingmemcontainerbase.h"

namespace Kaim
{

template <class T>
class WorkingMemArray
{
public:
	WorkingMemArray();

	WorkingMemArray(WorkingMemory* workingMemory);

	void Init(WorkingMemory* workingMemory);

	bool IsInitialized()   const; // Check whether the container has been successfully initialized from a buffer of the WorkingMemory

	T*       GetBuffer()   const;
	KyUInt32 GetCount()    const;
	KyUInt32 GetCapacity() const;
	bool     IsEmpty()     const;

	void MakeEmpty();
	KyResult SetMinimumCapacity(KyUInt32 minCapacity); /* \pre IsInitialized() == true */
	bool IsFull();                                     /* \pre IsInitialized() == true */

	KyResult PushBack(const T& x);                  /* \pre IsInitialized() == true */
	void PushBack_UnSafe(const T& x);               /* \pre IsInitialized() == true && IsFull() == false */

	KyResult PopBack();                                /* \pre IsInitialized() == true && IsEmpty() == false */

	const T& operator[](UPInt idx) const;        /* \pre IsInitialized() == true && idx < GetCount() */
	T& operator[](UPInt idx);                    /* \pre IsInitialized() == true && idx < GetCount() */

	// newSize == 0 means no specific is wanted, only try to increase the buffer with a normal way
	KyResult TryToResize(KyUInt32 newBytesSize = 0); /* \pre IsInitialized() == true */

	void ReleaseWorkingMemoryBuffer() { m_workingMemContainerBase.ReleaseBuffer(); }
public:
	WorkingMemContainerBase m_workingMemContainerBase;
	KyUInt32 m_itemCount;
	KyUInt32 m_capacity;
};

}

#include "gwnavruntime/querysystem/workingmemcontainers/workingmemarray.inl"


#endif //Navigation_WorkingMemArray_H

