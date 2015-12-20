/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_DynamicRasterCellBlobBuilder_H
#define GwNavGen_DynamicRasterCellBlobBuilder_H


#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavgeneration/raster/rastercell.h"


namespace Kaim
{

class DynamicRasterCell;
class PixelAndCellGrid;


class DynamicRasterCellBlobBuilder : public BaseBlobBuilder<RasterCell>
{
public:
	DynamicRasterCellBlobBuilder(const DynamicRasterCell* dynamicRasterCell, const PixelAndCellGrid* pixelAndCellGrid);

private:
	virtual void DoBuild();
	KyFloat32 m_rasterPrecision;
	const DynamicRasterCell* m_dynamicRasterCell;
	const PixelAndCellGrid* m_pixelAndCellGrid; 
};


}


#endif

