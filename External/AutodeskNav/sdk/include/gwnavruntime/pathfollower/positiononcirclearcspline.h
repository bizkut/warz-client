/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_PositionOnCircleArcSpline_H
#define Navigation_PositionOnCircleArcSpline_H

#include "gwnavruntime/math/vec3f.h"

namespace Kaim
{

class CircleArcSpline;
class CircleArc;

class PositionOnCircleArcSpline
{
public:
	PositionOnCircleArcSpline()
	{
		Invalidate();
	}

	void Reset(const CircleArcSpline* circleArcSpline);

	void Invalidate();
	bool IsValid() const;

	const Vec3f&     GetPosition() const { return m_position; }
	const Vec2f&     GetTangent()  const { return m_tangent; }

	const CircleArc* GetCurrentCircleArc()        const;
	KyFloat32        GetTurningRadius()           const;
	KyFloat32        GetDistanceFromStart()       const;
	KyFloat32        GetDistanceToEnd()           const;
	KyFloat32        GetDistanceToCurrentArcEnd() const;

	void MoveForward(KyFloat32 distance);
	void MoveBackward(KyFloat32 distance);
	KyResult MoveToNextTurn();

public:
	const CircleArcSpline* m_circleArcSpline;
	KyUInt32         m_arcIndex;
	Vec3f            m_position;
	Vec2f            m_tangent;
};

}

#endif // Navigation_PositionOnCircleArcSpline_H
