/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_StatusInGridBase_H
#define Navigation_StatusInGridBase_H

#include "gwnavruntime/querysystem/workingmemcontainers/workingmemcontainerbase.h"
#include "gwnavruntime/database/activedata.h"
#include "gwnavruntime/containers/bitfieldutils.h"

namespace Kaim
{

class StatusInGridBase
{
public:
	typedef KyUInt32 StatusWord;

	struct IndexedStatus
	{
		KY_INLINE StatusWord* GetBufferOfStatus() const { return (StatusWord*)((char*)this + m_offSetToStatus); }

		KY_INLINE bool IsValid() const { return m_offSetToStatus != KyUInt32MAXVAL; }

		KY_INLINE bool IsNodeOpen(KyUInt32 triangleIdx) const { return BitFieldUtils::IsBitSet(GetBufferOfStatus(), triangleIdx); }
		KY_INLINE void OpenNode(KyUInt32 triangleIdx) const { BitFieldUtils::SetBit(GetBufferOfStatus(), triangleIdx); }

		KY_INLINE void SetBufferOfStatusWord(StatusWord* buffer) { m_offSetToStatus = (KyUInt32)((char*)buffer - (char*)(this)); }

	private:
		KyUInt32 m_offSetToStatus;
	};

	template <class UnderlyingClass>
	struct IntermediateStatusAccessor
	{
		KY_INLINE bool IsValid() const { return m_offSetToUnderlyingClass != KyUInt32MAXVAL; }
		KY_INLINE UnderlyingClass* GetUnderlyingObjectBuffer() const { return (UnderlyingClass*)((char*)this + m_offSetToUnderlyingClass); }
		KY_INLINE UnderlyingClass* GetUnderlyingObject(KyUInt32 objectVertexIdx) const { return &GetUnderlyingObjectBuffer()[objectVertexIdx]; }

		KY_INLINE void SetBufferOfUnderlyingObjects(UnderlyingClass* buffer) { m_offSetToUnderlyingClass = (KyUInt32)((char*)buffer - (char*)(this)); }
	private:
		KyUInt32 m_offSetToUnderlyingClass;
	};

public:
	StatusInGridBase() {}

	StatusInGridBase(WorkingMemory* workingMemory, const CellBox& cellBox) : m_cellBox(cellBox)
	{
		m_workingMemContainerBase.Init(workingMemory);
		m_currentOffsetFromBuffer = 0;
	}

	void Init(WorkingMemory* workingMemory, const CellBox& cellBox)
	{
		m_workingMemContainerBase.Init(workingMemory);
		m_cellBox = cellBox;
		m_currentOffsetFromBuffer = 0;
	}

	void ReleaseWorkingMemoryBuffer() { m_workingMemContainerBase.ReleaseBuffer(); }

	KY_INLINE KyUInt32 GetAvailableSizeInBytes() const { return m_workingMemContainerBase.GetBufferSize() - m_currentOffsetFromBuffer; }

	KY_INLINE bool IsEnoughPlaceForAllocation(KyUInt32 sizeInBytes)
	{
		while (GetAvailableSizeInBytes() < sizeInBytes)
		{
			if (TryToResize() == false)
				return false;
		}

		return true;
	}

	bool TryToResize()
	{
		void* oldBuffer = GetBuffer();
		KyUInt32 oldSize = m_workingMemContainerBase.GetBufferSize();

		void* newBuffer = m_workingMemContainerBase.Resize();
		if (newBuffer == KY_NULL)
			return false;

		memcpy(newBuffer, oldBuffer, oldSize);
#if defined(KY_BUILD_DEBUG)
		memset(oldBuffer, 0xAB, oldSize);
#endif
		m_workingMemContainerBase.ReleaseMemory((void*)oldBuffer);
		return true;
	}

	void* AllocateInBufferAndMemset(KyUInt32 sizeOfOneElementInBytes, KyUInt32 numberOfElements, KyUInt8 memsetValue);
	StatusWord* AllocateStatusWords(KyUInt32 numberOfNodes);

	void* GetBuffer() const;
	bool IsInitialized() const;
public:
	WorkingMemContainerBase m_workingMemContainerBase;
	KyUInt32 m_currentOffsetFromBuffer;
	CellBox m_cellBox;
};

KY_INLINE void* StatusInGridBase::GetBuffer() const { return m_workingMemContainerBase.GetBuffer(); }
KY_INLINE bool StatusInGridBase::IsInitialized() const { return m_workingMemContainerBase.IsInitialized(); }

KY_INLINE void* StatusInGridBase::AllocateInBufferAndMemset(KyUInt32 sizeOfOneElementInBytes, KyUInt32 numberOfElements, KyUInt8 memSetValue)
{
	KY_DEBUG_ASSERTN(sizeOfOneElementInBytes %4  == 0, ("this function cannot be called with other values multiple of 4"));

	const KyUInt32 totalSizeToNewOffSet = sizeOfOneElementInBytes * numberOfElements;
	if (IsEnoughPlaceForAllocation(totalSizeToNewOffSet) == false)
		return KY_NULL;

	void* returnValue = (void*)((char*)GetBuffer() + m_currentOffsetFromBuffer);

	memset((char*)GetBuffer() + m_currentOffsetFromBuffer, memSetValue, totalSizeToNewOffSet);

	m_currentOffsetFromBuffer += totalSizeToNewOffSet;
	return returnValue;
}

KY_INLINE StatusInGridBase::StatusWord* StatusInGridBase::AllocateStatusWords(KyUInt32 numberOfNodes)
{
	return (StatusInGridBase::StatusWord*)AllocateInBufferAndMemset(sizeof(StatusInGridBase::StatusWord), BitFieldUtils::GetWordsCount(numberOfNodes), 0);
}

}


#endif

