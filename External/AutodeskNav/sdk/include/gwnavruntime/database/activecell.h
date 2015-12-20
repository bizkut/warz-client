/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_ActiveCell_H
#define Navigation_ActiveCell_H

#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/database/activenavfloorcollection.h"

namespace Kaim
{

class NavFloor;

/// This object gathers all the active NavFloors at a CellPos.
/// Accessible from ActiveData.
class ActiveCell
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
public:
	ActiveCell();
	~ActiveCell();
	void Clear();
	void CopyFrom(ActiveCell& other);

	// ---------------------------------- Public Member Functions ----------------------------------

	KyUInt32 GetActiveNavFloorsCount() const;
	NavFloor** GetActiveNavFloorBuffer() const;

public : // Internal
	ActiveNavFloorCollection m_activeNavFloors;
	KyUInt32 m_navDataChangeIdx;
};


KY_INLINE ActiveCell::ActiveCell() : m_navDataChangeIdx(0) {}
KY_INLINE ActiveCell::~ActiveCell()
{
	Clear();
}

KY_INLINE KyUInt32 ActiveCell::GetActiveNavFloorsCount() const { return m_activeNavFloors.GetCount();   }
KY_INLINE NavFloor** ActiveCell::GetActiveNavFloorBuffer() const { return m_activeNavFloors.GetNavFloors(); }

KY_INLINE void ActiveCell::CopyFrom(ActiveCell& other)
{
	m_navDataChangeIdx = other.m_navDataChangeIdx;
	m_activeNavFloors.GetOwnershipOfData(other.m_activeNavFloors);
}

KY_INLINE void ActiveCell::Clear()
{
	m_activeNavFloors.ForceClear();
	m_navDataChangeIdx = 0;
}

}

#endif //Navigation_ActiveCell_H

