/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: NOBODY

#ifndef Navigation_PositionOnCircleArcSplineBlob_H
#define Navigation_PositionOnCircleArcSplineBlob_H

#include "gwnavruntime/pathfollower/circlearcblob.h"


namespace Kaim
{

class PositionOnCircleArcSpline;

class PositionOnCircleArcSplineBlob
{
	KY_ROOT_BLOB_CLASS(Runtime, PositionOnCircleArcSplineBlob, 0)

public:
	PositionOnCircleArcSplineBlob() {}

public:
	KyUInt32 m_visualDebugId;
	Vec3f    m_position;
	Vec2f    m_tangent;
	KyUInt32 m_arcIndex;
};

inline void SwapEndianness(Endianness::Target e, PositionOnCircleArcSplineBlob& self)
{
	SwapEndianness(e, self.m_visualDebugId);
	SwapEndianness(e, self.m_position);
	SwapEndianness(e, self.m_tangent);
	SwapEndianness(e, self.m_arcIndex);
}

class PositionOnCircleArcSplineBlobBuilder : public BaseBlobBuilder<PositionOnCircleArcSplineBlob>
{
	KY_CLASS_WITHOUT_COPY(PositionOnCircleArcSplineBlobBuilder)

public:
	PositionOnCircleArcSplineBlobBuilder(const PositionOnCircleArcSpline* positionOnCircleArcSpline, KyUInt32 visualDebugId) : m_positionOnCircleArcSpline(positionOnCircleArcSpline), m_visualDebugId(visualDebugId) {}

	virtual void DoBuild();

private:
	const PositionOnCircleArcSpline* m_positionOnCircleArcSpline;
	KyUInt32 m_visualDebugId;
};

} // namespace Kaim

#endif // Navigation_PositionOnCircleArcSplineBlob_H
