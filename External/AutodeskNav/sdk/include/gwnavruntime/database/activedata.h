/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_ActiveData_H
#define Navigation_ActiveData_H

#include "gwnavruntime/database/activecell.h"
#include "gwnavruntime/navmesh/navcell.h"
#include "gwnavruntime/navgraph/navgraph.h"


namespace Kaim
{

class Database;

/// This class gathers all the navigation data that are currently active in a Database. This is comprised
/// of a grid of NavFloors and a set of NavGraph.
/// You may not need to use the ActiveData directly in your code. However, if you do,
/// you can retrieve it by calling Database::GetActiveData() at runtime
class ActiveData
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_WorldFwk)
	KY_CLASS_WITHOUT_COPY(ActiveData)

public: // Internal
	ActiveData(Database* database);
	~ActiveData();

	void Clear();

public:
	// ---------------------------------- Public Member Functions ----------------------------------

	/// Retrieves a CellBox that encloses all the navigation data in this ActiveData. That does not mean
	///  that in all the CellPos of this CellBox there is activeData.
	 const CellBox& GetCellBox() const;

	KyUInt32 GetNavGraphCount() const;
	NavGraph* GetNavGraph(NavGraphIdxInActiveData idx) const;

	/// Return the index in the buffer of ActiveCell of this CellPos
	/// Returns CellIdxInActiveData_Invalid if GetCellBox().IsInside(cellPos) == false.
	CellIdxInActiveData GetCellIdxInActiveDataFromCellPos(const CellPos& cellPos) const;

	/// Returns the index in the buffer of ActiveCell of this CellPos
	/// \pre GetCellBox().IsInside(cellPos) == true
	CellIdxInActiveData GetCellIdxInActiveDataFromCellPos_Unsafe(const CellPos& cellPos) const;

	/// Returns true if at least one NavFloor is active at the input CellPos
	bool IsActiveNavFloorAtThisCellPos(const CellPos& cellPos) const;

	ActiveCell* GetActiveCellFromCellPos(const CellPos& cellPos) const;
	ActiveCell& GetActiveCellFromCellPos_Unsafe(const CellPos& cellPos) const;
	ActiveCell& GetActiveCellFromIndex_Unsafe(CellIdxInActiveData idxInActiveData) const;

public: // Internal
	Database* m_database;

	ActiveCell* m_bufferOfActiveCell; /*< For internal use. Do not modify. */
	KyUInt32 m_sizeOfCellBuffer; /*< For internal use. Do not modify. */

	KyArrayPOD<NavGraph*, MemStat_NavData> m_navGraphs; /*< For internal use. Do not modify. */
	KyUInt32 m_navGraphChangeIdx;

	CellBox m_cellBox; /*< A bounding box that encloses all NavCells currently added to the ActiveData. Do not modify. */
};

}

#include "gwnavruntime/database/activedata.inl"



#endif //Navigation_ActiveData_H

