/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_CellRasterizer_H
#define GwNavGen_CellRasterizer_H


#include "gwnavruntime/navmesh/navmeshtypes.h"


namespace Kaim
{

class DynamicRasterCell;
class SingleStageRaster;

class CellRasterizer : public NewOverrideBase<MemStat_NavDataGen>
{
public:
	CellRasterizer(DynamicRasterCell* dynamicRaster) : m_dynamicRaster(dynamicRaster) {};
	virtual ~CellRasterizer() {}

	KyResult Rasterize();

protected:
	void Push(const SingleStageRaster& singleStageRaster);

protected:
	virtual KyResult DoRasterize() = 0;

protected:
	DynamicRasterCell* m_dynamicRaster;
};


}


#endif
