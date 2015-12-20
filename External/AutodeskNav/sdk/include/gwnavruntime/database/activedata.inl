/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY

namespace Kaim
{

KY_INLINE ActiveData::ActiveData(Database* database) :
	m_database(database),
	m_bufferOfActiveCell(KY_NULL),
	m_sizeOfCellBuffer(0),
	m_navGraphChangeIdx(0)
{}

KY_INLINE ActiveData::~ActiveData() { Clear(); }

KY_INLINE const CellBox& ActiveData::GetCellBox()                             const { return m_cellBox;              }
KY_INLINE KyUInt32       ActiveData::GetNavGraphCount()                       const { return m_navGraphs.GetCount(); }
KY_INLINE NavGraph*      ActiveData::GetNavGraph(NavGraphIdxInActiveData idx) const { return m_navGraphs[idx];       }

KY_INLINE CellIdxInActiveData ActiveData::GetCellIdxInActiveDataFromCellPos_Unsafe(const CellPos& cellPos) const
{
	KY_DEBUG_ASSERTN(m_sizeOfCellBuffer != 0 && m_cellBox.IsInside(cellPos),
		("You should not call the function if you are not sure that navdata have been loaded \
		  and that the input cellPos in the cellBox of loaded NavData"));

	return m_cellBox.GetRowMajorIndex(cellPos);
}

KY_INLINE CellIdxInActiveData ActiveData::GetCellIdxInActiveDataFromCellPos(const CellPos& cellPos) const
{
	return m_cellBox.FastIsInside(cellPos) ? GetCellIdxInActiveDataFromCellPos_Unsafe(cellPos) : CellIdxInActiveData_Invalid;
}

KY_INLINE ActiveCell& ActiveData::GetActiveCellFromIndex_Unsafe(CellIdxInActiveData idxInActiveData) const
{
	KY_DEBUG_ASSERTN(idxInActiveData != CellIdxInActiveData_Invalid, ("You should not call the function if you are not sure that navdata have been loaded and that the input idxInActiveData in valid"));
	return m_bufferOfActiveCell[idxInActiveData];
}

KY_INLINE ActiveCell& ActiveData::GetActiveCellFromCellPos_Unsafe(const CellPos& cellPos) const
{
	return GetActiveCellFromIndex_Unsafe(GetCellIdxInActiveDataFromCellPos_Unsafe(cellPos));
}

KY_INLINE bool ActiveData::IsActiveNavFloorAtThisCellPos(const CellPos& cellPos) const
{
	return m_cellBox.FastIsInside(cellPos) && GetActiveCellFromCellPos_Unsafe(cellPos).GetActiveNavFloorsCount() != 0;
}

KY_INLINE ActiveCell* ActiveData::GetActiveCellFromCellPos(const CellPos& cellPos) const
{
	return m_cellBox.FastIsInside(cellPos) ? &GetActiveCellFromCellPos_Unsafe(cellPos) : KY_NULL;
}

KY_INLINE void ActiveData::Clear()
{
	m_sizeOfCellBuffer = 0;
	m_cellBox.Clear();
	m_navGraphs.Clear();
}

}