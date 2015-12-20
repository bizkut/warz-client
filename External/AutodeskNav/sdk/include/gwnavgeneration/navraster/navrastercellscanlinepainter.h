/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LASI - secondary contact: NONE
#ifndef GwNavGen_NavRasterCellScanlinePainter_H
#define GwNavGen_NavRasterCellScanlinePainter_H


#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavgeneration/containers/tlsarray.h"
#include "gwnavgeneration/common/generatormemory.h"
#include "gwnavruntime/navmesh/blobs/flooraltituderange.h"
#include "gwnavgeneration/navraster/navrastertypes.h"
#include "gwnavgeneration/navraster/navrastercell.h"

namespace Kaim
{

class DynamicNavRasterCell;
class NavRasterPixel;
class NavRasterWorkingPixel;
class DynamicNavRasterFinalColumn;

class NavRasterCellScanlinePainter
{
public:
	enum SearchResult   { SEARCH_RESULT_FOUND, NO_SEARCH_RESULT };
	class ScanPixel
	{
	public:
		ScanPixel() : m_navPixel(KY_NULL), m_pos(KyInt32MAXVAL,KyInt32MAXVAL), m_floorIdx(KyUInt32MAXVAL) {}
		ScanPixel(NavRasterPixel* columnElement, const PixelPos& pos, KyUInt32 floor) :
			m_navPixel(columnElement), m_pos(pos), m_floorIdx(floor) {}

		NavRasterPixel* m_navPixel;
		PixelPos m_pos;
		KyUInt32 m_floorIdx;
	};

	class ColorBoundary
	{
	public:
		ColorBoundary() {}

		ScanPixel m_fisrtBoundaryPixel;
		ScanPixel m_lastBoundaryPixel;
		CardinalDir m_boundaryDir;
	};

	class ColorData
	{
	public:
		ColorData(PixelColor color = PixelColor_Unset, KyUInt32 connectedComponentIdx = KyUInt32MAXVAL) :
		  m_color(color), m_pixelCount(0), m_connectedComponentIdx(connectedComponentIdx),
			  m_boundaryPixelBorderCount(0) , m_indexInBinaryHeap(BinaryHeapInvalidReference)  {}
		void UpdateColorDataFromNewPixel(NavRasterPixel* pixel, const PixelPos& pos);
	public:
		PixelColor m_color;
		PixelBox m_box;
		KyUInt32 m_pixelCount;
		KyUInt32 m_connectedComponentIdx;
		KyUInt32 m_boundaryPixelBorderCount;
		KyUInt32 m_indexInBinaryHeap;
		FloorAltitudeRange m_altRange;
	};

public: //keep it simple

	NavRasterCellScanlinePainter() : m_navRaster(KY_NULL), m_paintingMs(KY_NULL) {}

	void Clear();
	KyResult Paint();

	void SetNavRasterCell(DynamicNavRasterCell* cell) {  m_navRaster = cell;       }
	void SetConnexIdxToNavTagIdxArray(KyArrayTLS_POD<KyUInt32>* connexIdxToNavTagIdxArray) { m_connexIdxToNavTagIdxArray = connexIdxToNavTagIdxArray ;}
	void SetPaintingMs(KyFloat32* paintingMs)         { m_paintingMs = paintingMs; }

	KyUInt32 GetNbColorsUsed() const { return m_colorCount; }
private:
	// 1 -
	void Init();

	// 2 -
	void RemoveSmallNavTagArea();

	// 3 -
	void ComputeConnectedComponent();

	// 4 - 
	void ComputeColorsByPropagation();

	// 5 - 
	void MergeColors();

	// 6 - 
	void TagConnex();

private:
	void PropagateInSouthWestBox();
	void PropagateInSouthEastBox();
	void PropagateInNorthWestBox();
	void PropagateInNorthEastBox();

	void PropagateFromCornerColumn(const PixelBox& paintBox, const PixelPos& cornerPos, CardinalDir expandLineDir, CardinalDir lineMoveDir);
	void PropagateFromCornerPixel(const PixelBox& paintBox, const ScanPixel& seedPixel, CardinalDir expandLineDir, CardinalDir lineMoveDir);
	PixelBox ComputeLocalPaintBoxForCorner(const PixelBox& paintBox, const ScanPixel& corner, CardinalDir expandLineDir, CardinalDir lineMoveDir);

	void PropagateFromBoxBorder(const PixelBox& paintBox, const PixelPos& startPos, CardinalDir expandLineDir, CardinalDir lineMoveDir);
	void PropagateFromBorderPixel(const PixelBox& paintBox, const ScanPixel& seedPixel, CardinalDir expandLineDir, CardinalDir lineMoveDir);
	PixelBox ComputeLocalPaintBoxForBorder(const PixelBox& paintBox, const ScanPixel& seedPixel, CardinalDir expandLineDir, CardinalDir lineMoveDir);

	void DisconnectOutsidePixelsAtBoxBounds(const PixelBox& paintBox, const ScanPixel& corner, CardinalDir dirAlongBorder,
		CardinalDir dirToOutside, PixelColor color);

	void PropagateFromAllRemainingUncoloredPixels();
	void PropagateFromUncoloredNonBorderPixel(const ScanPixel& startPixel);

	void ColorCellBoundaryPixelLineInBox(const PixelBox& localPaintBox, const ScanPixel& startPixel, PixelColor color,
		CardinalDir expandDir, CardinalDir borderDir, ScanPixel& lastPixelOnLine);

	bool TwoNeighborPixelHaveSameColor(const ScanPixel& pixel1, const ScanPixel& pixel2, CardinalDir dir, PixelColor currentColor);
	bool CanPropagateToPixel(PixelPos& pixelPos, KyUInt32 currentFloorIdx, KyUInt32 propagationIdx);

	void FindLowerConnectedPixel(const ScanPixel& startPixel, ScanPixel& lowerPixel);
	void CastPixelUntillBoundary(const ScanPixel& startScanPixel, CardinalDir castDir);

	PixelPos CastCellBoundaryPixel(const PixelBox& bbox, const ScanPixel& borderPixel, CardinalDir castDir, CardinalDir borderDir);
	void CastPixelCheckingColor(const ScanPixel& scanLine, PixelColor color, const PixelBox& bbox, CardinalDir castdir);
	void PaintLastPixelCastResultAndOutsideNeighbors(PixelColor color, CardinalDir borderDir);

	bool IsConnectedInDir(const ScanPixel& pixel, CardinalDir dir, ScanPixel& result);
	void GetNeighborScanPixel(const ScanPixel& borderPixel, CardinalDir neighborDir, ScanPixel& neighborPixel);
	void GetNeighborScanPixel(const PixelBox& bbox, const ScanPixel& borderPixel, CardinalDir neighborDir, ScanPixel& neighborPixel);

	PixelColor GetNewColor(KyUInt32 connectedComponentIdx);
	void ColorPixel(const ScanPixel& pixel, PixelColor color);

	bool CanSetColorToPixel(PixelPos& pixelPos, KyUInt32 floor, PixelColor newColor);
	bool IsColorPresentInOtherFloorOfColumn(const DynamicNavRasterFinalColumn& column, KyUInt32 ignoredFloorIdx, PixelColor newColor);
	bool IsColorPresentInOtherFloorOfColumn(const DynamicNavRasterFinalColumn& column, KyUInt32 ignoredFloorIdx1, KyUInt32 ignoredFloorIdx2, PixelColor newColor);

	bool CanPaintNeighborLine(const ScanPixel& startScanPixel, const ScanPixel& endScanPixel, PixelColor color, const PixelBox& bbox,
		CardinalDir lineDir, CardinalDir expandDir, ScanPixel& borderLineStartScanPixel, ScanPixel& borderLineEndScanPixel,
		KyUInt32& neighborLineCount, KyFloat32& altRangeDiffWithNeighborLine);

	// 4 -
	void DetectForbiddenColorMergeAndConnectedColors();
	void ComputeMergedColors();
	void ApplyFinalColorToPixels();

	class ScanPixelSorterByAlt
	{
	public:
		bool operator() (const ScanPixel& pixel1, const ScanPixel& pixel2) const
		{
			return pixel1.m_navPixel->m_altitude < pixel2.m_navPixel->m_altitude;
		}
	};

	void TagNavTagConnectedComponent(const PixelBox& safenavTagRemovalBox, const ScanPixel& startPixel, KyUInt32 connectedComponentIdx, KyUInt32 minPixelCount);
	void TagConnectedComponent(const ScanPixel& startPixel, KyUInt32 connectedComponentIdx);
	void TagConnexConnectedComponentInFloor(const ScanPixel& startPixel, KyUInt32 currentConnexIdx);
private:
	DynamicNavRasterCell* m_navRaster;
	KyFloat32* m_paintingMs;
	KyUInt32 m_colorCount;
	KyArrayTLS<ScanPixel> m_popagationStatck; // cache the array between floodFills to avoid multiple resizes
	KyArrayTLS_POD<ScanPixel> m_navTagArea;
	KyArrayTLS_POD<ScanPixel> m_navTagAreaNeighbors;
	KyArrayTLS<ColorData> m_colorData;

	BitFieldTLS m_forbiddenColorMerge;
	BitFieldTLS m_connectedColors;
	KyArrayTLS_POD<PixelColor> m_finalColor; // m_finalColor[color] = finalColor after merge !

	KyArrayTLS<ScanPixel> m_castForwardResult;
	KyArrayTLS<ScanPixel> m_castBackwardResult;
	KyArrayTLS<ScanPixel> m_onePixelPerFinalColor;

	BinaryHeapTls<ScanPixel, ScanPixelSorterByAlt> m_orderedPixels;
	KyArrayTLS_POD<KyUInt32>* m_connexIdxToNavTagIdxArray;

	KyInt32 m_midPixelPosX;
	KyInt32 m_midPixelPosY;
	KyUInt32 m_currentPropagationId;
};


KY_INLINE bool NavRasterCellScanlinePainter::IsConnectedInDir(const ScanPixel& pixel, CardinalDir dir, ScanPixel& result)
{
	GetNeighborScanPixel(pixel, dir, result);
	return result.m_navPixel != KY_NULL;
}

KY_INLINE PixelColor NavRasterCellScanlinePainter::GetNewColor(KyUInt32 connectedComponentIdx)
{
	PixelColor color = m_colorData.GetCount();
	m_colorData.PushBack(ColorData(color, connectedComponentIdx));
	return color;
}
KY_INLINE void NavRasterCellScanlinePainter::ColorPixel(const ScanPixel& pixel, PixelColor color)
{
	KY_ASSERT(pixel.m_navPixel != KY_NULL);
	KY_ASSERT(pixel.m_navPixel->m_floorColor == PixelColor_Unset);
	KY_ASSERT(color < m_colorData.GetCount());

	pixel.m_navPixel->m_floorColor = color;
	m_colorData[color].UpdateColorDataFromNewPixel(pixel.m_navPixel, pixel.m_pos);
}
KY_INLINE void NavRasterCellScanlinePainter::ColorData::UpdateColorDataFromNewPixel(NavRasterPixel* currentPixel, const PixelPos& pos)
{
	KY_LOG_ERROR_IF(currentPixel->m_floorColor != m_color, ("Wrong color data for this pixel"));
	++m_pixelCount;
	m_altRange.Update(currentPixel->m_altitude);
	m_box.ExpandByVec2(pos);
	for(KyUInt32 i = 0; i < 4; ++i)
	{
		if (currentPixel->m_neighborFloorIdx[i] == NavRasterFloorIdx_Invalid)
			++m_boundaryPixelBorderCount;
	}
}


}

#endif
