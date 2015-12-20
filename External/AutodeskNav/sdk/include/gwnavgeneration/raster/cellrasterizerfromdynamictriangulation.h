/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


	


// primary contact: GUAL - secondary contact: LASI
#ifndef GwNavGen_CellRasterizerFromDynamicTriangulation_H
#define GwNavGen_CellRasterizerFromDynamicTriangulation_H

#include "gwnavgeneration/raster/cellrasterizer.h"
#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/navmesh/celldesc.h"
#include "gwnavgeneration/raster/trianglerasterizer.h"
#include "gwnavgeneration/raster/singlestageraster.h"
#include "gwnavruntime/navmesh/dynamictriangulation_int.h"

namespace Kaim
{

class GeneratorSystem;
class InputCellBlob;
class DynamicTriangulation;

class CellRasterizerFromDynamicTriangulation : public CellRasterizer
{
public:
	CellRasterizerFromDynamicTriangulation(
			GeneratorSystem* sys, KyInt32 stride, DynamicTriangulation* triangulation,
			DynamicRasterCell* dynamicRaster, const PixelBox& pixelBox, const KyArrayDH_POD<KyUInt32>* modifiedTriangles, CellDesc& cellDesc
			)
		: CellRasterizer(dynamicRaster)
		, m_singleStageRaster(GeneratorMemory::TlsHeap())
		, m_triangleRasterizer(sys)
	{
		m_sys = sys;
		m_dynamicTriangulation = triangulation;
		m_pixelBox = pixelBox;
		m_modifiedTriangles = modifiedTriangles;
		m_singleStageRaster.Init(m_pixelBox);
		m_triangleRasterizer.Init((KyFloat32)stride * m_sys->GetNormalizedRasterPrecision(), stride * IntCoordSystem::IntPixelSize(), &m_singleStageRaster);
		m_cellDesc = cellDesc;
	}

public: 
	void SetDynamicTriangulation(DynamicTriangulation* triangulation)
	{
		m_dynamicTriangulation = triangulation;
	}
	void SetDynamicRasterCell(DynamicRasterCell* rasterCell)
	{
		m_dynamicRaster = rasterCell;
	}
	void SetModifiedTriangles(const KyArrayDH_POD<KyUInt32>* triangles)
	{
		m_modifiedTriangles = triangles;
	}

protected:
	virtual KyResult DoRasterize();

protected:
	DynamicTriangulation* m_dynamicTriangulation;
	PixelBox m_pixelBox;
	const KyArrayDH_POD<KyUInt32>* m_modifiedTriangles;
	SingleStageRaster m_singleStageRaster;
	TriangleRasterizer m_triangleRasterizer;
	CellDesc m_cellDesc;
	GeneratorSystem* m_sys;
};


}


#endif
