/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_CellGeneratorProfiling_H
#define GwNavGen_CellGeneratorProfiling_H


#include "gwnavruntime/navmesh/navmeshtypes.h"


namespace Kaim
{


class CellGeneratorProfiling
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	CellGeneratorProfiling()
		: m_enabled(false)
		, m_rasterMs(0.0f)
		, m_navRasterMs(0.0f)
		, m_navRasterPaintingMs(0.0f)
		, m_heightFieldMs(0.0f)
		, m_boundariesMs(0.0f)
		, m_navCellMs(0.0f)
	{}

	KyFloat32* GetRasterMs()             { return m_enabled ? &m_rasterMs             : KY_NULL; }
	KyFloat32* GetNavRasterMs()          { return m_enabled ? &m_navRasterMs          : KY_NULL; }
	KyFloat32* GetNavRasterPaintingMs()  { return m_enabled ? &m_navRasterPaintingMs  : KY_NULL; }
	KyFloat32* GetHeightFieldMs()        { return m_enabled ? &m_heightFieldMs        : KY_NULL; }
	KyFloat32* GetBoundariesMs()         { return m_enabled ? &m_boundariesMs         : KY_NULL; }
	KyFloat32* GetNavCellMs()            { return m_enabled ? &m_navCellMs            : KY_NULL; }

public:
	bool m_enabled;
	KyFloat32 m_rasterMs;
	KyFloat32 m_navRasterMs;
	KyFloat32 m_navRasterPaintingMs;
	KyFloat32 m_heightFieldMs;
	KyFloat32 m_boundariesMs;
	KyFloat32 m_navCellMs;
};


}


#endif

