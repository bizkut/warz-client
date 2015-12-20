/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_SpatializedSectorInput_H
#define GwNavGen_SpatializedSectorInput_H

#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/base/kyguid.h"
#include "gwnavruntime/kernel/SF_String.h"
#include "gwnavgeneration/common/densegrid.h"
#include "gwnavgeneration/input/dynamicinputcell.h"


namespace Kaim
{

class GeneratorSystem;
class Triangle3i;
class TaggedTriangle3i;
class GeneratorSectorConfig;
class SectorInputData;

/*
cells are indexed this way
  |     |     |     |     |
--+-----+-----+-----+-----+--
  |-2, 1|-1, 1| 0, 1| 1, 1|
  |     |     |     |     |
--+-----+-----+-----+-----+--
  |-2, 0|-1, 0| 0, 0| 1, 0|
  |     |     |     |     |
--+-----+-----#-----+-----+--
  |-2,-1|-1,-1| 0,-1| 1,-1|
  |     |     |     |     |
--+-----+-----+-----+---- +--
  |-2,-2|-1,-2| 0,-2| 1,-2|
  |     |     |     |     |
--+-----+-----+-----+-----+--
  |     |     |     |     |

a cell contains at least all triangles that intersect with
[cell_min - enlargement, cell_max + enlargement]
  |     |     |     |
  |   +---------+   |
--+---|-+-----+-|---+--
  |   | |     | |   |
  |   | |     | |   |
--+---|-+-----+-|---+--
  |   +---------+   |
  |     |     |     |
              >-<
               enlargement
*/

class DynInputCellArrayAtTilePos
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavDataGen)
public:
	DynInputCellArrayAtTilePos(KyInt32 x, KyInt32 y) : m_x(x), m_y(y) {}
	KyArrayPOD<DynamicInputCell*> m_dynInputCells;
	KyInt32 m_x;
	KyInt32 m_y;
};

// tile part filenames at tilePos (x, y)
class TilePartFileNameArrayAtTilePos
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavDataGen)
public:
	TilePartFileNameArrayAtTilePos(KyInt32 x, KyInt32 y) : m_x(x), m_y(y) {}
	KyArray<String> m_filenames;
	KyInt32 m_x;
	KyInt32 m_y;
};

// Spatializes the input triangles into m_dynInputCellGrid.
// SpatializedSectorInput is a member of ClientInputConsumer. So there is one SpatializedSectorInput per Sector.
// Note that ClientInputConsumer is instantiated on the stack in GeneratorSectorBuilder::ProduceInputs().
// SpatializedSectorInput is just a temporary data, once production/consumption of the inputs are done,
// inputs reside in SectorInput (for each Sector).
// if Tiling is enabled
//     DynamicInputCells are spatialized into m_dynInputCellArrayTileGrid;
//     When a given number of triangles are pushed, m_dynInputCellGrid and m_dynInputCellArrayTileGrid
//     are copied to tilePart files referenced by m_tilePartFileNameArrayTileGrid
class SpatializedSectorInput
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavDataGen)

public:
	SpatializedSectorInput(GeneratorSystem* sys);

	~SpatializedSectorInput() { ClearAll(); }

	void SetName(const String& name) { m_sectorName = name; }

	void SetMaxTriangleCountBeforeFlush(KyUInt32 maxTriangleCountBeforeFlush) { m_maxTriangleCountBeforeFlush = maxTriangleCountBeforeFlush; }

	void ClearAll()
	{
		ClearCellAndTileGrids();
		ClearFlushedTiledFileNames();
	}

	void ClearCellAndTileGrids();

	void ClearFlushedTiledFileNames();

	void PushTriangle(const DynamicTaggedTriangle3i& inputTriangle);

	void PushTagVolume(const TagVolume& volume);

	DynamicInputCell* GetCell(CellCoord x, CellCoord y) { return m_dynInputCellGrid.Get(x, y); }

	DynamicInputCell** GetCells() { return m_dynInputCellGrid.GetElements(); }

	KyUInt32 GetCellsCount() const { return m_dynInputCellGrid.GetElementsCount(); }

	KyResult FlushTiles();

	void RemovePartsTempDirectory();

private:
	String GetTilePartDirName();
	String GetTilePartFileName(KyInt32 x, KyInt32 y, KyUInt32 partIndex);

public:
	GeneratorSystem* m_sys;

	KyUInt32 m_nbTrianglesPushed;
	KyUInt32 m_nbVolumesPushed;
	KyUInt32 m_maxTriangleCountBeforeFlush;

	DenseGridAndArrayOfRawPtr<DynamicInputCell> m_dynInputCellGrid;

	DenseGridAndArrayOfRawPtr<DynInputCellArrayAtTilePos>     m_dynInputCellArrayTileGrid;     // grid of m_nbCellsByInputTile * cellSize
	DenseGridAndArrayOfRawPtr<TilePartFileNameArrayAtTilePos> m_tilePartFileNameArrayTileGrid; // grid of m_nbCellsByInputTile * cellSize. Saves the parts saved so far.

	KyGuid m_sectorGuid;
	String m_sectorName;
};

}

#endif
