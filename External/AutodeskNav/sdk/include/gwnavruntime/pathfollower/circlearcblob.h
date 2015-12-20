/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: NOBODY

#ifndef Navigation_CircleArcBlob_H
#define Navigation_CircleArcBlob_H

#include "gwnavruntime/world/runtimeblobcategory.h"
#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/math/vec3f.h"

namespace Kaim
{

class CircleArc;

class CircleArcBlob
{
public:
	CircleArcBlob() {}

public:
	Vec3f     m_startPosition;
	Vec3f     m_midPoint;
	Vec3f     m_endPosition;
	Vec3f     m_center;
	KyFloat32 m_radius;
	KyUInt32  m_direction; // cast this to RotationDirection
	KyUInt32  m_startSectionIdx;
	KyUInt32  m_endSectionIdx;

	KyFloat32 m_length;
	KyFloat32 m_rotationAngle;
	Vec2f     m_startPositionOnUnitCircle;
	Vec2f     m_endPositionOnUnitCircle;
};

inline void SwapEndianness(Endianness::Target e, CircleArcBlob& self)
{
	SwapEndianness(e, self.m_startPosition);
	SwapEndianness(e, self.m_midPoint);
	SwapEndianness(e, self.m_endPosition);
	SwapEndianness(e, self.m_center);
	SwapEndianness(e, self.m_radius);
	SwapEndianness(e, self.m_direction);
	SwapEndianness(e, self.m_startSectionIdx);
	SwapEndianness(e, self.m_endSectionIdx);
	SwapEndianness(e, self.m_length);
}

class CircleArcBlobBuilder : public BaseBlobBuilder<CircleArcBlob>
{
	KY_CLASS_WITHOUT_COPY(CircleArcBlobBuilder)

public:
	CircleArcBlobBuilder(const CircleArc* circleArc) : m_circleArc(circleArc) {}

	virtual void DoBuild();

private:
	const CircleArc* m_circleArc;
};

} // namespace Kaim

#endif // Navigation_CircleArcBlob_H
