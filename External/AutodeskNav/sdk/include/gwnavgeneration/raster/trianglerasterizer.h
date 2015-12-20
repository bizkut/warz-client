/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


	


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_TriangleRasterizer_H
#define GwNavGen_TriangleRasterizer_H


#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavgeneration/raster/rastertriangle.h"


namespace Kaim
{

class Triangle3i;
class GeneratorSystem;
class RasterPoint;
class SingleStageRaster;


class TriangleRasterizer
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	TriangleRasterizer(GeneratorSystem* sys) : m_sys(sys), m_singleStageRaster(KY_NULL){}

	void Init(KyFloat32 pixelSizeInMeters, KyInt32 intPixelSize, SingleStageRaster* singleStageRaster);

	void RasterizeTriangle(const Triangle3i& triangle3i, KyUInt32 navTagIdx);

	KyFloat32 GetRasterPrecision() const { return m_rasterPrecision; }

private:
	void RasterizePoint(const RasterPoint& M);
	void RasterizeInterior();
	void WriteLine(Coord min_x, Coord max_x, Coord y);
	void WriteSinglePixelLine(Coord min_x, Coord max_x, PixelCoord py, Coord y);
	void WriteDoublePixelLine(Coord min_x, Coord max_x, Coord y);
	void FlagLineEnds();

private:
	GeneratorSystem* m_sys;
	KyInt32 m_intPixelSize;
	KyFloat32 m_rasterPrecision;
	SingleStageRaster* m_singleStageRaster;
	RasterTriangle m_triangle;
	CoordBox m_rasterCoordBox;
};


}


#endif
