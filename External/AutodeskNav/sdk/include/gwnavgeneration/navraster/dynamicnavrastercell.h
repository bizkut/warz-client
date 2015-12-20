/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LASI - secondary contact: GUAL
#ifndef GwNavGen_DynamicNavRasterCell_H
#define GwNavGen_DynamicNavRasterCell_H


#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavgeneration/navraster/navrastertypes.h"

#include "gwnavgeneration/common/growinglistpool.h"
#include "gwnavgeneration/common/growingsmallbuffer.h"
#include "gwnavgeneration/raster/dynamicrastercell.h"
#include "gwnavgeneration/navraster/navrastercell.h"
#include "gwnavruntime/navmesh/celldesc.h"
#include "gwnavruntime/base/timeprofiler.h"
#include "gwnavgeneration/navraster/navrastercellscanlinepainter.h"

namespace Kaim
{

class GeneratorSystem;
class BlobChunkBuilder;

class NavRasterWorkingPixel;

class NavRasterNeighborPixel 
{
public: 
	NavRasterNeighborPixel() : m_floorIdxOfNeighbor(NavRasterFloorIdx_Invalid), m_connectionType(NavRasterConnectionType_NO_TAG), m_neighborPtr(KY_NULL) {}
	void Clear();

	NavRasterFloorIdx m_floorIdxOfNeighbor; 
	NavRasterConnectionType m_connectionType; 
	NavRasterWorkingPixel* m_neighborPtr;
};

class NavRasterWorkingPixel
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	NavRasterWorkingPixel() { Clear(); }
	void Clear();

	// remove the pointer to the neighbor pixel and make sure we update
	// the symmetric neighbor connections as well. 
	void DisconnectFromNeighbors();

	void TagNeighbors(KyUInt32 tag);

	KyUInt32               GetFeatureMask(CardinalDir dir)          const;
	NavRasterFloorIdx      GethNeighborFloorIndex(CardinalDir dir)  const;
	NavRasterWorkingPixel* GethNeighborPixelPtr(CardinalDir dir)  const;
	bool HasAtLeastOneValidNeighborLink() const;

	KyFloat32 m_altitude;
	KyFloat32 m_distanceFromWall;
	KyFloat32 m_distanceFromHole;
	NavRasterFloorIdx m_floorIdx;
	KyUInt32 m_navTagIdx;
	KyUInt32 m_featureBitField;

	// connections are indexed as follow:
	//  +-----------+
	//  |     1     |
	//  |           |
	//  |2    +    0|
	//  |           |
	//  |     3     |
	//  +-----------+

	//0 corresponds to East (+1;0) and index increases CCW
	//Stores level index of neighbor position if reachable, 0xFFFFFFFF otherwise
	NavRasterNeighborPixel m_neighborPixels[4];
};

class DynamicNavRasterFinalColumn
{
public:
	DynamicNavRasterFinalColumn() : m_floorCount(0), m_elements(KY_NULL) {}
	KyUInt32 m_floorCount;
	NavRasterPixel* m_elements;
};

class DynamicNavRasterWorkingColumn
{
public:
	DynamicNavRasterWorkingColumn() : m_floorCount(0), m_elements(KY_NULL) {}
	KyUInt32 m_floorCount;
	NavRasterWorkingPixel* m_elements;
};

class DynamicNavRasterCell
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	DynamicNavRasterCell(GeneratorSystem* sys, const DynamicRasterCell* inputRasterCell, const CellDesc& cellDesc, KyArrayTLS_POD<KyUInt32>* connexIdxToNavTagIdxArray);
	~DynamicNavRasterCell() {}

	void CreateNavRaster();

	DynamicNavRasterFinalColumn GetFinalColumnFromLocalPixelPos(const PixelPos& localPixelPos) const;

	/* Write NavRaster file file if specified in the Generator RunOptions, for debug purpose. */
	KyResult WriteIntermediateNavRasterFile();

	bool IsEmpty() const { return m_isEmpty; }
	bool HasMultipleNavTags() const { return m_hasMultipleNavTags; }

	DynamicNavRasterWorkingColumn* GetWorkingNeighborColumn(const PixelPos& pos, CardinalDir dir);

	NavRasterWorkingPixel* GetWorkingNeighborPixel(const NavRasterWorkingPixel& pixel, NavRasterCardinalDir dir) const;

private:
	void ResettNeighborhoodInfo(const PixelPos& posInRaster, PixelPos* neighborPos, KyUInt32* neighborColumnIndex, 
		bool* isNeighborInside, KyUInt32* neighborNavTagIdx, NavRasterWorkingPixel** neighbors);
	void CreateUnpaintNavRaster();
	void BuildRaster();
	void FilterIsolatedSteps();
	void TagConnections();
	void ErodePixels();

	void MakeFinalColumns();

	void EnforceSymetry();

	void ComputeManhattanDistanceMap();
	void InitBorderPixelDistance();
	void UpdateManhattanDistForPixelsOnDir(const PixelPos& posInRaster, CardinalDir dir1, CardinalDir dir2);
	void UpdateChessBoardDistForPixelsOnDir(const PixelPos& posInRaster, NavRasterCardinalDir* navRasterDirs);

	void ComputeChessboardDistanceMap();

	void ReleaseWorkingColumns()
	{
		m_workingColumns.ClearAndRelease();
		m_workingColumnsPool.Release();
	}

	NavRasterWorkingPixel* GetWorkingDiagonalNeighbor(const NavRasterWorkingPixel& pixel, NavRasterCardinalDir dir) const;
	NavRasterWorkingPixel* GetWorkingDiagonalNeighbor(const NavRasterWorkingPixel& pixel, CardinalDir dir1, CardinalDir dir2) const;

public:
	GeneratorSystem* m_sys;
	CellDesc m_cellDesc;
	KyUInt32 m_highestFloorCount;

public:
	const DynamicRasterCell* m_inputRasterCell;

	PixelBox m_enlargedPixelBox;
	PixelBox m_navPixelBox;

	//coloring
	NavRasterCellScanlinePainter m_painter;

	// final columns
	KyArrayTLS<DynamicNavRasterFinalColumn> m_finalColumns;
	GrowingSmallBufferPool m_finalColumnsPool;

	KyArrayTLS<DynamicNavRasterWorkingColumn> m_workingColumns;
	GrowingSmallBufferPool m_workingColumnsPool;

	bool m_isEmpty;
	bool m_hasMultipleNavTags; //extracted from the rasterCell

	KyFloat32* m_navRasterMs;
	KyFloat32* m_paintingMs;

	friend class DynamicNavRasterCellBlobBuilder;
};

KY_INLINE void NavRasterNeighborPixel::Clear()
{
	m_neighborPtr = KY_NULL;
	m_connectionType = NavRasterConnectionType_NO_TAG;
	m_floorIdxOfNeighbor = NavRasterFloorIdx_Invalid;
}

KY_INLINE KyUInt32               NavRasterWorkingPixel::GetFeatureMask(CardinalDir dir)         const { return (0xFF << dir) && m_featureBitField;          }
KY_INLINE NavRasterFloorIdx      NavRasterWorkingPixel::GethNeighborFloorIndex(CardinalDir dir) const { return m_neighborPixels[dir].m_floorIdxOfNeighbor; }
KY_INLINE NavRasterWorkingPixel* NavRasterWorkingPixel::GethNeighborPixelPtr(CardinalDir dir)   const { return m_neighborPixels[dir].m_neighborPtr;         }

KY_INLINE DynamicNavRasterFinalColumn DynamicNavRasterCell::GetFinalColumnFromLocalPixelPos(const PixelPos& localPixelPos) const
{
	const KyInt32 idx = m_navPixelBox.GetRowMajorIndexFromLocalPos(localPixelPos);
	return m_finalColumns[idx];
}

KY_INLINE NavRasterWorkingPixel* DynamicNavRasterCell::GetWorkingNeighborPixel(const NavRasterWorkingPixel& pixel, NavRasterCardinalDir dir) const
{
	return dir < 4 ? pixel.GethNeighborPixelPtr(dir) : GetWorkingDiagonalNeighbor(pixel, dir);
}

KY_INLINE NavRasterWorkingPixel* DynamicNavRasterCell::GetWorkingDiagonalNeighbor(const NavRasterWorkingPixel& pos, NavRasterCardinalDir dir) const
{
	KY_ASSERT(dir >= 4 && dir < 8);

	switch(dir)
	{
	case NavRasterCardinalDir_NORTH_EAST:
		return GetWorkingDiagonalNeighbor(pos, CardinalDir_NORTH, CardinalDir_EAST);
	case NavRasterCardinalDir_NORTH_WEST:
		return GetWorkingDiagonalNeighbor(pos, CardinalDir_NORTH, CardinalDir_WEST);
	case NavRasterCardinalDir_SOUTH_WEST:
		return GetWorkingDiagonalNeighbor(pos, CardinalDir_SOUTH, CardinalDir_WEST);
	case NavRasterCardinalDir_SOUTH_EAST:
		return GetWorkingDiagonalNeighbor(pos, CardinalDir_SOUTH, CardinalDir_EAST);
	default: return KY_NULL;
	}
}
}

#endif
