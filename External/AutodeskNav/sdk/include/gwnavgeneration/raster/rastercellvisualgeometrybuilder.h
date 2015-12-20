/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_RasterCellVisualGeometryBuilder_H
#define GwNavGen_RasterCellVisualGeometryBuilder_H

#include "gwnavruntime/visualsystem/ivisualgeometrybuilder.h"

namespace Kaim
{

class VisualGeometryBuilder;
class RasterCell;


class RasterCellVisualGeometryBuilder : public IVisualGeometryBuilder
{
public:
	RasterCellVisualGeometryBuilder(RasterCell* rasterCell) : m_rasterCell(rasterCell) {}

	virtual void DoBuild();
public:
	RasterCell* m_rasterCell;
};


}


#endif // GwNavGen_RasterCellVisualGeometryBuilder_H
