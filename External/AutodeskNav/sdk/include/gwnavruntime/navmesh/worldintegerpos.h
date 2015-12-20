/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_WorldIntegerPos_H
#define Navigation_WorldIntegerPos_H


#include "gwnavruntime/navmesh/navmeshtypes.h"


namespace Kaim
{


/// Utilities for dealing with NavData coordinates, which are expressed in a world space based on integers.
class WorldIntegerPos
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	WorldIntegerPos();
	WorldIntegerPos(const CellPos& cellPos, const CoordPos& posInCell);

	// ---------------------------------- Public Member Functions ----------------------------------

	void Set(const CellPos& cellPos, const CoordPos& posInCell);                                ///< Sets the coordinates of the vector to match the specified values.
	void Set(CellCoord cellCoordX, CellCoord cellCoordY, KyInt32 coordPosX, KyInt32 coordPosY); ///< Sets the coordinates of the vector to match the specified values.

	void Clear(); ///< Clears all information maintained by this object. 

	bool operator==(const WorldIntegerPos& rhs) const;
	bool operator!=(const WorldIntegerPos& rhs) const;
	bool operator<(const WorldIntegerPos& rhs) const;
	bool operator>(const WorldIntegerPos& rhs) const;

public: //internal
	void Add1OnCoordPosX(KyInt32 cellSizeInCoord);
	void Remove1OnCoordPosX(KyInt32 cellSizeInCoord);
	void Add1OnCoordPosY(KyInt32 cellSizeInCoord);
	void Remove1OnCoordPosY(KyInt32 cellSizeInCoord);

public:
	CellPos m_cellPos;
	CoordPos m_coordPosInCell;
};

/// Swaps the endianness of the data in the specified object. This function allows this type of object to be serialized through the blob framework.
/// The first parameter specifies the target endianness, and the second parameter provides the object whose data is to be swapped.  
KY_INLINE void SwapEndianness(Kaim::Endianness::Target e, WorldIntegerPos& self)
{
	SwapEndianness(e, self.m_cellPos);
	SwapEndianness(e, self.m_coordPosInCell);
}


KY_INLINE WorldIntegerPos::WorldIntegerPos() : m_cellPos(InvalidCellCoord, InvalidCellCoord), m_coordPosInCell(InvalidCoord, InvalidCoord) {}
KY_INLINE WorldIntegerPos::WorldIntegerPos(const CellPos& cellPos, const CoordPos& posInCell) : m_cellPos(cellPos), m_coordPosInCell(posInCell) {}

KY_INLINE void WorldIntegerPos::Clear()
{
	m_cellPos.Set(InvalidCellCoord, InvalidCellCoord);
	m_coordPosInCell.Set(InvalidCoord, InvalidCoord);
}

KY_INLINE void WorldIntegerPos::Set(const CellPos& cellPos, const CoordPos& posInCell) { m_cellPos = cellPos; m_coordPosInCell = posInCell; }
KY_INLINE void WorldIntegerPos::Set(CellCoord cellCoordX, CellCoord cellCoordY, KyInt32 coordPosX, KyInt32 coordPosY)
{
	m_cellPos.Set(cellCoordX, cellCoordY);
	m_coordPosInCell.Set(coordPosX, coordPosY);
}

KY_INLINE bool WorldIntegerPos::operator==(const WorldIntegerPos& rhs) const { return m_cellPos == rhs.m_cellPos && m_coordPosInCell == rhs.m_coordPosInCell; }
KY_INLINE bool WorldIntegerPos::operator!=(const WorldIntegerPos& rhs) const { return !operator==(rhs);/*x != v.x || y != v.y;*/ }
KY_INLINE bool WorldIntegerPos::operator<(const WorldIntegerPos& rhs)  const { return m_cellPos != rhs.m_cellPos ? m_cellPos < rhs.m_cellPos : m_coordPosInCell < rhs.m_coordPosInCell; }
KY_INLINE bool WorldIntegerPos::operator>(const WorldIntegerPos& rhs)  const { return !operator<(rhs) && operator!=(rhs); }

KY_INLINE void WorldIntegerPos::Add1OnCoordPosX(KyInt32 cellSizeInCoord)
{
	KY_DEBUG_ASSERTN(m_coordPosInCell.x > 0 || m_coordPosInCell.x <= cellSizeInCoord, ("Invalid Object"));
	KY_DEBUG_ASSERTN(m_coordPosInCell.y >= 0 || m_coordPosInCell.y < cellSizeInCoord, ("Invalid Object"));
	if (m_coordPosInCell.x == cellSizeInCoord)
	{
		m_coordPosInCell.x = 1;
		++m_cellPos.x;
	}
	else
		++m_coordPosInCell.x;
}

KY_INLINE void WorldIntegerPos::Remove1OnCoordPosX(KyInt32 cellSizeInCoord)
{
	KY_DEBUG_ASSERTN(m_coordPosInCell.x > 0 || m_coordPosInCell.x <= cellSizeInCoord, ("Invalid Object"));
	KY_DEBUG_ASSERTN(m_coordPosInCell.y >= 0 || m_coordPosInCell.y < cellSizeInCoord, ("Invalid Object"));
	if (m_coordPosInCell.x == 1)
	{
		m_coordPosInCell.x = cellSizeInCoord;
		--m_cellPos.x;
	}
	else
		--m_coordPosInCell.x;
}

KY_INLINE void WorldIntegerPos::Add1OnCoordPosY(KyInt32 cellSizeInCoord)
{
	KY_DEBUG_ASSERTN(m_coordPosInCell.x > 0 || m_coordPosInCell.x <= cellSizeInCoord, ("Invalid Object"));
	KY_DEBUG_ASSERTN(m_coordPosInCell.y >= 0 || m_coordPosInCell.y < cellSizeInCoord, ("Invalid Object"));
	if (m_coordPosInCell.y + 1 == cellSizeInCoord)
	{
		m_coordPosInCell.y = 0;
		++m_cellPos.y;
	}
	else
		++m_coordPosInCell.y;
}

KY_INLINE void WorldIntegerPos::Remove1OnCoordPosY(KyInt32 cellSizeInCoord)
{
	KY_DEBUG_ASSERTN(m_coordPosInCell.x > 0 || m_coordPosInCell.x <= cellSizeInCoord, ("Invalid Object"));
	KY_DEBUG_ASSERTN(m_coordPosInCell.y >= 0 || m_coordPosInCell.y < cellSizeInCoord, ("Invalid Object"));
	if (m_coordPosInCell.y == 0)
	{
		m_coordPosInCell.y = cellSizeInCoord - 1;
		--m_cellPos.y;
	}
	else
		--m_coordPosInCell.y;
}

} // namespace Kaim



#endif

