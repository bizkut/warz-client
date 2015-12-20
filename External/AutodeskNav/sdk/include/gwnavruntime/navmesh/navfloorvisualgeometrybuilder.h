/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_NavFloorVisualGeometryBuilder_H
#define Navigation_NavFloorVisualGeometryBuilder_H

#include "gwnavruntime/visualsystem/ivisualgeometrybuilder.h"
#include "gwnavruntime/database/databasegenmetrics.h"
#include "gwnavruntime/navmesh/navfloor.h"

namespace Kaim
{

class NavFloor;

/* This class of VisualRepresentation defines the way a NavFloor should be rendered using triangles. */
class NavFloorVisualGeometryBuilder : public IVisualGeometryBuilder
{
public:
	NavFloorVisualGeometryBuilder(NavFloor* navFloor, KyInt32 cellSizeInPixel, KyFloat32 integerPrecision);

	virtual void DoBuild();

private:
	void RenderInvalidLink(const NavHalfEdgeIdx edgeIdx, const NavFloorBlob& currentFloorBlob, const CoordPos64 cellOrigin);

public:
	NavFloor* m_navFloor;
	KyFloat32 m_integerPrecision;
	KyInt32 m_cellSizeInPixel;
	KyInt32 m_cellSizeInCoord;
};

class NavFloorBlobVisualGeometryBuilder : public IVisualGeometryBuilder
{
public:
	NavFloorBlobVisualGeometryBuilder(const NavFloorBlob* navFloorBlob, KyUInt32 floorIdx, KyFloat32 integerPrecision,
		KyInt32 cellSizeInCoord, const CellPos& cellPos);

	virtual void DoBuild();

	void RenderEdge(NavHalfEdgeType edgeType, const CoordPos64& start64, const CoordPos64& end64, const Vec3f& start, const Vec3f& end);
	void RenderStitchDataBlob(const CoordPos64& cellOrigin);
public:
	const NavFloorBlob* m_navFloorBlob;
	KyFloat32 m_integerPrecision;
	KyInt32 m_cellSizeInCoord;
	KyUInt32 m_floorIdx;
	CellPos m_cellPos;
	const NavFloor1To1StitchDataBlob* m_stitchDataBlob;
};

}

#endif // Navigation_NavFloorVisualGeometryBuilder_H
