/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LASI - secondary contact: NOBODY
#ifndef Navigation_IntegerSegmentUtils_H
#define Navigation_IntegerSegmentUtils_H

#include "gwnavruntime/navmesh/intersections.h"


namespace Kaim
{

// Order cuts depending on their distance from edge start
class CutSorter
{
public:
	CutSorter(const Vec2i& edgeStart) : m_edgeStart(edgeStart) {}
	bool operator() (const Vec2i& cut1, const Vec2i& cut2) const
	{
		return (cut1 - m_edgeStart).GetSquareLength() > (cut2 - m_edgeStart).GetSquareLength();
	}

public:
	Vec2i m_edgeStart;
};

namespace IntegerSegment
{
	static KyInt32 RoundedDivide(KyInt64 num, KyInt64 denum);

	// Generic template functions
	// returns true iff pt coordinate is between start and end
	template <typename T> //T E { Vec{2,3}i, Vec2LL } 
	static bool Between(const T& pt, const T& edgeStart, const T& edgeEnd);

	// returns true if [ edgeStart, edgeEnd ] is vertical
	template <typename T> //T E { Vec{2,3}i, Vec2LL } 
	static bool IsVertical(const T& edgeStart, const T& edgeEnd);
	
	// returns true if [ edgeStart, edgeEnd ] is horizontal
	template <typename T> //T E { Vec{2,3}i, Vec2LL } 
	static bool IsHorizontal(const T& edgeStart, const T& edgeEnd);

	// returns true if edge1 with derivative (dx1, dy1) has a slope inferior to edge2 (dx2, dy2)
	template <typename T> //T E { Int32/64 } 
	static bool LessSlopeUnit(T dx1, T dy1, T dx2, T dy2);

	template <typename T>  //T E { Vec{2,3}i, Vec2LL }
	static bool LessSlope(const Vec2i& a, const Vec2i& b, const Vec2i& c);
	
	template <typename T>  //T E { KyInt32, KyInt64 }
	static bool ArePointsAligned(const Vec2i& a, const Vec2i& b, const Vec2i& c);

	
	//return -1 below, 0 on and 1 above line
	template <typename T>  
	static KyInt32 OnAboveOrBelow(const Vec2i& pt, const Vec2i& edgeStart, const Vec2i& edgeEnd);

	// NB1: When relevant, the intersection point is snapped to the lower left corner of the pixel where the 
	// "Real" (precise) intersection occurs. This is not a simple integer conversion by truncation.
	bool SegmentVsSegment2d_hp(const Vec2i& a, const Vec2i& b, const Vec2i& c, const Vec2i& d, Vec2i& intersection);

	bool SegmentVsSegment2d_rounded(const Vec2i& a, const Vec2i& b, const Vec2i& c, const Vec2i& d, Vec2i& intersection);
	bool SegmentVsSegment2dWithBoxes_rounded(const Vec2i& a, const Vec2i& b, const Vec2i& c, const Vec2i& d, const Box2i& boxAB, const Box2i& boxCD, Vec2i& intersection);

	template <typename T> 
	bool SegmentVsSegment2d(const T& a, const T& b, const T& c, const T& d);

	bool EvalPosAtZ(const Vec2i& a, const Vec2i& b, KyFloat32 altA, KyFloat32 altB, KyFloat32 targetAlt, Vec2i& pos);
} // namespace IntegerSegment

} //namespace Kaim


#include "gwnavruntime/math/integersegmentutils.inl"


#endif // Navigation_IntegerSegmentUtils_H
