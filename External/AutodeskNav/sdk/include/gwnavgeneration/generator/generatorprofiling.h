/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_GeneratorProfiling_H
#define GwNavGen_GeneratorProfiling_H


#include "gwnavgeneration/generator/cellgeneratorprofiling.h"
#include "gwnavruntime/basesystem/logger.h"
#include "gwnavruntime/basesystem/logstream.h"



namespace Kaim
{


class CellProfiling
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	CellProfiling() : m_totalMs(0.0f) {}

	void Integrate(KyFloat32 milliseconds, const CellPos& cellPos)
	{
		m_totalMs += milliseconds;

		if (milliseconds > m_maxMs)
		{
			m_maxMs = milliseconds;
			m_maxCellPos = cellPos;
		}
	}

public:
	KyFloat32 m_totalMs;
	KyFloat32 m_maxMs;
	CellPos m_maxCellPos;
};


template <class OSTREAM>
inline OSTREAM& operator<<(OSTREAM& os, const Kaim::CellProfiling& c)
{
	os << " total(s)=" << c.m_totalMs * 0.001f << " max(ms)=" << c.m_maxMs << " @ " << c.m_maxCellPos;
	return os;
}


class GeneratorProfiling
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	GeneratorProfiling() {}

	void Integrate(const CellGeneratorProfiling& cellProfiling, const CellPos& cellPos)
	{
		m_raster.Integrate(           cellProfiling.m_rasterMs           , cellPos);
		m_navRaster.Integrate(        cellProfiling.m_navRasterMs        , cellPos);
		m_navRasterPainting.Integrate(cellProfiling.m_navRasterPaintingMs, cellPos);
		m_heightField.Integrate(      cellProfiling.m_heightFieldMs      , cellPos);
		m_boundaries.Integrate(       cellProfiling.m_boundariesMs       , cellPos);
		m_navCell.Integrate(          cellProfiling.m_navCellMs          , cellPos);
	}

public:
	CellProfiling m_raster;
	CellProfiling m_navRaster;
	CellProfiling m_navRasterPainting;
	CellProfiling m_heightField;
	CellProfiling m_boundaries;
	CellProfiling m_navCell;
};


template<class OSTREAM>
inline OSTREAM& operator<<(OSTREAM& os, const GeneratorProfiling& profiling)
{
	os << KY_LOG_SMALL_TITLE_BEGIN("", "Generator Profiling");
	os << "raster              " << profiling.m_raster            << Endl;
	os << "navRaster           " << profiling.m_navRaster         << Endl;
	os << "navRasterPainting   " << profiling.m_navRasterPainting << Endl;
	os << "heightField         " << profiling.m_heightField       << Endl;
	os << "boundaries          " << profiling.m_boundaries        << Endl;
	os << "navCellBlob         " << profiling.m_navCell           << Endl;
	os << KY_LOG_SMALL_TITLE_END("", "Generator Profiling");
	return os;
}


}


#endif

