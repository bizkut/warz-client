/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_IntermediateFilesConfig_H
#define GwNavGen_IntermediateFilesConfig_H


#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/kernel/SF_String.h"

namespace Kaim
{


/// The IntermediateFilesConfig class is used by GeneratorRunOptions to store configuration parameters that control
/// whether or not the Generator writes different types of intermediate data to files on disk, where, and how.
class IntermediateFilesConfig
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavDataGen)
public:
	IntermediateFilesConfig() {}

	// Add a CellPos for which we generate intermediate files
	void Add(const CellPos& pos) { m_cellsPositions.PushBack(pos); }

	bool DoesContain(const CellPos& pos) const { return m_cellsPositions.DoesContain(pos); }
	void Clear()                               {        m_cellsPositions.Clear(); }
	KyUInt32 GetCount() const                  { return m_cellsPositions.GetCount(); }
	const CellPos& Get(KyUInt32 index) const   { return m_cellsPositions[index]; }

	/// Returns true if at least one intermediate file is to be created
	bool IsEnabled() const {return m_cellsPositions.GetCount() != 0; }

	String GetFileName_RasterCell(const CellPos& cellPos) const        { return GetFileName(cellPos, "RasterCell"); }
	String GetFileName_NavRasterCell(const CellPos& cellPos) const     { return GetFileName(cellPos, "NavRasterCell"); }
	String GetFileName_BoundaryGraphCell(const CellPos& cellPos) const { return GetFileName(cellPos, "BoundaryGraphCell"); }

private:
	/// Returns the actual file name that should be used for the file that corresponds to the \c cellPos cell. 
	String GetFileName(const CellPos& cellPos, const String& extension) const;

public:
	KyArray<CellPos> m_cellsPositions; // Write intermediate files for these cell positions
};


} // namespace Kaim

#endif
