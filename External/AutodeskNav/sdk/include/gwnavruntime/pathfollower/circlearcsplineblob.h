/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: NOBODY

#ifndef Navigation_CircleArcSplineBlob_H
#define Navigation_CircleArcSplineBlob_H

#include "gwnavruntime/pathfollower/circlearcblob.h"


namespace Kaim
{

class CircleArcSpline;

class CircleArcSplineBlob
{
	KY_ROOT_BLOB_CLASS(Runtime, CircleArcSplineBlob, 0)

public:
	CircleArcSplineBlob() {}

	KyUInt32 GetSectionCount() const { return m_sections.GetCount(); }
	Vec3f GetStart() const;
	Vec3f GetEnd() const;

public:
	KyUInt32 m_visualDebugId;
	BlobArray<CircleArcBlob> m_sections;
};

inline void SwapEndianness(Endianness::Target e, CircleArcSplineBlob& self)
{
	SwapEndianness(e, self.m_visualDebugId);
	SwapEndianness(e, self.m_sections);
}

class CircleArcSplineBlobBuilder : public BaseBlobBuilder<CircleArcSplineBlob>
{
	KY_CLASS_WITHOUT_COPY(CircleArcSplineBlobBuilder)

public:
	CircleArcSplineBlobBuilder(const CircleArcSpline* circleArcSpline, KyUInt32 visualDebugId) : m_circleArcSpline(circleArcSpline), m_visualDebugId(visualDebugId) {}

	virtual void DoBuild();

private:
	const CircleArcSpline* m_circleArcSpline;
	KyUInt32 m_visualDebugId;
};

} // namespace Kaim

#endif
