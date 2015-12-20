/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


	


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_BoundaryTypes_H
#define GwNavGen_BoundaryTypes_H


#include "gwnavruntime/navmesh/navmeshtypes.h"


namespace Kaim
{
	// world coordinates
	typedef KyInt32 BoundaryCoord;
	typedef Vec2i BoundaryPos;
	typedef Box2i BoundaryBox;

	// relative to NavPixelBox
	typedef KyInt32 NavBoundaryCoord;
	typedef Vec2i NavBoundaryPos;

	// relative to NavPixelBox
	typedef KyInt32 NavPixelCoord;
	typedef Vec2i NavPixelPos;
	typedef Vec3i NavPixelPos3d;

	// relative to exclusivePixelBox
	typedef KyInt32 ExclBoundaryCoord;
	typedef Vec2i ExclBoundaryPos;
	typedef Vec3i ExclBoundaryVertexPos; // x and y are relative to exclusivePixelBox

	enum BoundaryEdgeType
	{
		BoundaryEdgeType_CellLink_East  = 0,
		BoundaryEdgeType_CellLink_North = 1,
		BoundaryEdgeType_CellLink_West  = 2,
		BoundaryEdgeType_CellLink_South = 3,
		BoundaryEdgeType_FloorLink      = 4, //Represent the outer edges of a 2d floor
		BoundaryEdgeType_ConnexLink     = 5, //Represent and edge that separate 2 navTag in the same layer
		BoundaryEdgeType_Wall           = 6,
		BoundaryEdgeType_Hole           = 7,
		BoundaryEdgeType_Unset = KyUInt32MAXVAL
	};

	KY_INLINE bool IsBoundaryEdgeOnFloorOrCellLink(BoundaryEdgeType type) { return type < BoundaryEdgeType_ConnexLink;  }

	typedef KyInt32 BoundaryVertexStaticStatus;
	static const BoundaryVertexStaticStatus BoundaryVertexStaticStatus_Dynamic = 0;
	static const BoundaryVertexStaticStatus BoundaryVertexStaticStatus_Static  = 1;
	static const BoundaryVertexStaticStatus BoundaryVertexStaticStatus_Narrow  = 2;
	static const BoundaryVertexStaticStatus BoundaryVertexStaticStatus_FeatureDiscontinuity  = 3;

	typedef KyUInt32 BoundaryIsVisitedStatus;
	static const BoundaryIsVisitedStatus BoundaryIsVisitedStatus_Unvisited         = 0;
	static const BoundaryIsVisitedStatus BoundaryIsVisitedStatus_Visited           = 1;
	static const BoundaryIsVisitedStatus BoundaryIsVisitedStatus_NoNeedToBeVisited = 2;

	typedef KyUInt32 BoundaryPolylineCycleStatus;
	static const BoundaryPolylineCycleStatus PolylineCycleStatus_NotCycle = 0;
	static const BoundaryPolylineCycleStatus PolylineCycleStatus_Cycle    = 1;
	static const BoundaryPolylineCycleStatus PolylineCycleStatus_Unset = KyUInt32MAXVAL;

	typedef KyUInt32 BoundarySide;
	static const BoundarySide BoundarySide_Left   = 0;
	static const BoundarySide BoundarySide_Right  = 1;
	static const BoundarySide BoundarySide_Unset = KyUInt32MAXVAL;

	typedef KyUInt32 ContourWinding;
	static const ContourWinding ContourWinding_CCW   = 0;
	static const ContourWinding ContourWinding_CW    = 1;
	static const ContourWinding ContourWinding_Unset = KyUInt32MAXVAL;

	typedef KyUInt32 BoundaryOrder;
	static const BoundaryOrder BoundaryOrder_Straight = 0;
	static const BoundaryOrder BoundaryOrder_Reverse  = 1;
	static const BoundaryOrder BoundaryOrder_Unset    = KyUInt32MAXVAL;
}


#endif
