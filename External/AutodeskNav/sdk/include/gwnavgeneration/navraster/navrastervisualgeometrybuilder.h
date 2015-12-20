/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LASI - secondary contact: GUAL
#ifndef GwNavGen_NavRasterCellVisualGeometryBuilder_H
#define GwNavGen_NavRasterCellVisualGeometryBuilder_H

#include "gwnavruntime/visualsystem/ivisualgeometrybuilder.h"
#include "gwnavruntime/navmesh/navmeshtypes.h"

namespace Kaim
{

class VisualGeometryBuilder;
class NavRasterCell;
class NavRasterPixel;

class NavRasterCellVisualGeometryBuilder : public IVisualGeometryBuilder
{
public:
	NavRasterCellVisualGeometryBuilder(NavRasterCell* navRasterCell) : m_navRasterCell(navRasterCell) {}

	virtual void DoBuild();

private:
	const NavRasterPixel* GetNeighbor(const NavRasterPixel& pixel, const PixelPos& pos, KyInt32 dir);

public:
	NavRasterCell* m_navRasterCell;
};


}


#endif // GwNavGen_NavRasterCellVisualGeometryBuilder_H
