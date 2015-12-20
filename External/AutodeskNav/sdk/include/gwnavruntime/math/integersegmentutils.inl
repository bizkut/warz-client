/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/
// primary contact: LASI - secondary contact: NOBODY

namespace Kaim
{
namespace IntegerSegment
{

// Generic template functions

// returns true iff pt coordinate is between start and end
template <typename T>
static KY_INLINE bool Between(const T& pt, const T& edgeStart, const T& edgeEnd) { return pt < edgeEnd && edgeStart < pt; }

// returns true if [ edgeStart, edgeEnd ] is vertical
template <typename T>
static KY_INLINE bool IsVertical(const T& edgeStart, const T& edgeEnd) { return edgeStart.x == edgeEnd.x; }
// returns true if [ edgeStart, edgeEnd ] is vertical
template <typename T>
static KY_INLINE bool IsHorizontal(const T& edgeStart, const T& edgeEnd) { return edgeStart.y == edgeEnd.y; }

// if alpha is the angle of the first vector V1 (modified such as V1.x >=0) with the X axis
// if Beta is the angle of the second vector V2 (modified such as V2.x >=0) with the X axis
// return alpha < Beta
// that means Angle(V2, V1) > 0
template <typename T> //T E { Int32/64 } 
static KY_INLINE bool LessSlopeUnit(T ab_x, T ab_y, T ac_x, T ac_y)
{
	if(ab_x == 0) 
	{
		KY_ASSERT(ab_y != 0);
		return false; //if the first slope is vertical (the max) the first cannot be strictly less
	}

	if(ac_x == 0) 
	{
		KY_ASSERT(ac_y != 0);
		return true;
	}

	//reflect x and y slopes to right hand side half plane
	// change vectors so that we have dx > 0 & dy > 0
	if(ab_x < 0)
	{
		ab_y = -ab_y;
		ab_x = -ab_x;
	}

	if(ac_x < 0)
	{
		ac_y = -ac_y;
		ac_x = -ac_x;
	}

	// CrossProd(V1, V2) > 0
	return (ab_x * ac_y - ac_x * ab_y) > 0;
}

template <typename T>  //T E { Int32 / Int64 }
static KY_INLINE bool LessSlope(const Vec2i& a, const Vec2i& b, const Vec2i& c)
{
	const T ab_x = b.x - a.x;
	const T ab_y = b.y - a.y;
	const T ac_x = c.x - a.x;
	const T ac_y = c.y - a.y;
	return LessSlopeUnit<T>(ab_x, ab_y, ac_x, ac_y);
}

template <typename T> //Int32 / Int64
static KY_INLINE bool ArePointsAligned(const Vec2i& a, const Vec2i& b, const Vec2i& c)
{
	const T crossProd = CrossProductT<T>(b-a, c-a);
	return crossProd == 0;
}


//returns
//  -1 if pt is strictly below the edge [edgeStart, edgeEnd]
//   0 if pt is on the edge [edgeStart, edgeEnd]
//   1 if pt is strictly above the edge [edgeStart, edgeEnd]
template <typename T>  //Int32 / Int64
static KY_INLINE KyInt32 OnAboveOrBelow(const Vec2i& pt, const Vec2i& edgeStart, const Vec2i& edgeEnd)
{
	KY_LOG_ERROR_IF(edgeEnd < edgeStart, ("start and end of the edge should be sorted"));
	const Vec2i& a = pt;
	const Vec2i& b = edgeStart;
	const Vec2i& c = edgeEnd;

	const T crossProd = CrossProductT<T>(c-b, a-b);
	if (crossProd == 0)
		return 0; // points are aligned

	return (KyInt32)Sel<T>(crossProd, 1, -1);
}


// Returns the result of the division num/denum, rounded to the closest integer. 
// example: 1/3 returns 0
//        :-1/3 returns 0
//        : 1/2 returns 0
//        :-1/2 returns -1
//        : 2/3 returns 1
//        :-2/3 returns -2 and so on. 
static KY_INLINE KyInt32 RoundedDivide(KyInt64 num, KyInt64 denum)
{
	KyInt64 doubleNum = num * 2; 
	KyInt64 truncatedResult = doubleNum/denum; 
	// On x86 the remainder is a by-product of the division itself so any half-decent compiler should be able to just use it (and not perform a div again). 
	// This is probably done on other architectures too.
	KyInt64 remainder = doubleNum%denum;
	// Determine if result is >0 BEFORE dividing. Otherwise If truncated result is 0, there is no way to know the real sign
	// of the result. 
	KyInt64 sign = (num ^ denum); //sign bit will have the correct value with a XOR
	KyInt64 truncationValue = Lsel(sign, 1, -1); // 1 if >= 0; -1 otherwise
	truncationValue = remainder == 0 && truncationValue > 0 ? truncationValue - 1 : truncationValue;
	// here truncation value is: 
	// * 1 if the sign is > 0 and remainder != 0
	// * 0 if the sign is < 0 or the sign > 0 && remainder = 0
	// * -1 if the sign is < 0 and the remainder is 0
	return (KyInt32)( (truncatedResult + truncationValue) / 2);
}

// TODO: Return INT instead of FLOAT...
// cast from int to float to int = BAD - will LHS on PPC... 
static inline KyFloat32 EvalAtXforY_float(KyInt32 xIn, const Vec2i& pt, const Vec2i& other_pt) 
{ 
	if(pt.y == other_pt.y)
		return (KyFloat32)pt.y;

	const KyInt32 x1 = pt.x;
	const KyInt32 y1 = pt.y;
	const KyInt32 dx01 = xIn - x1;

	if(dx01 == 0)
		return (KyFloat32)y1;

	const KyInt32 x2 = other_pt.x;
	const KyInt32 y2 = other_pt.y;

	const KyInt32 dx21 = x2 - x1;
	const KyInt32 dy21 = y2 - y1;
	return (KyFloat32)y1 + (((KyFloat32)dx01 * (KyFloat32)dy21) / (KyFloat32)dx21);
}

 //Returns the Y value of a point at a given X on line [pt;other_pt].
 //Result is rounded to the closest int inferior to the actual position. 
 //eg: 
 //y_float = 1.2f => 1
 //y_float = 0.8f => 0
 //y_float = -0.1f => -1
 //y_float = -0.9f => -1
 //y_float = -1.0001f => -2
 //Computation is carried out in 64 bit and should not overflow. 
 //If input values are not too big (< KyInt16MaxVal), _Int32 method should be preferred. 
static inline KyInt32 EvalAtXforY_Int64(KyInt32 xIn, const Vec2i& pt, const Vec2i& other_pt) 
{ 
	KY_ASSERT(pt != other_pt);
	const KyInt32 pointsAtSameY = (pt.y ^ other_pt.y); // == 0 if pt.y == other_pt.y
	const KyInt32 pointAtX = (pt.x ^ xIn); // == 0 if pt.x == xIn
	if((pointsAtSameY * pointAtX) == 0)
	{
		// if pointsAtSameY == 0, the line [pt;other_pt] is horizontal, the intersection of this line and the axis X == xIn is pt.y
		// if pointAtX == 0, pt.x == xIn so the intersection is on pt.y
		return pt.y;
	}

	if (pt.x == other_pt.x)
	{
		// the edge is vertical and pt.x != xIn, so there is no intersection
		return KyInt32MAXVAL - 1;
	}

	const KyInt64 x1 = pt.x;
	const KyInt64 y1 = pt.y;
	const KyInt64 x2 = other_pt.x;
	const KyInt64 y2 = other_pt.y;
	const KyInt64 dx01 = xIn - x1;
	const KyInt64 dx21 = x2 - x1;
	const KyInt64 dy21 = y2 - y1;
	const KyInt64 num = dx01*dy21;
	const KyInt64 denum = dx21;
	KY_ASSERT(dx01 != 0);
	KY_ASSERT(dy21 != 0);
	KY_ASSERT(denum != 0);
	
	const KyInt64 truncatedResult = num/denum; 
	// On x86 the remainder is a by-product of the division itself so any half-decent compiler should be able to just use it (and not perform a div again). 
	// This is probably done on other architectures too.
	const KyInt64 remainder = num%denum;

	// Determine if result is >0 BEFORE dividing. Otherwise If truncated result is 0, there is no way to know the real sign
	// of the result. 
	KyInt64 sign = num ^ denum; //sign bit will have the correct value with a XOR
	KyInt64 truncationValue = (sign >> 63) & (remainder != 0); // 1 iff sign < 0 && remainder is different from 0. = 0 otherwise
	KY_ASSERT(truncatedResult + y1 <= KyInt32MAXVAL && truncatedResult + y1 > -KyInt32MAXVAL);
	return (KyInt32)(truncatedResult + y1 - truncationValue);
}

//Returns the Y value of a point at a given X on line [pt;other_pt].
//Result is rounded to the closest int from the actual position. 
//eg: 
//y_float = 1.2f => 1
//y_float = 0.8f => 1
//y_float = -0.1f => 0
//y_float = -0.9f => -1
//y_float = -1.0001f => -1
//Computation is carried out in 64 bit and should not overflow. 
static inline KyInt32 EvalAtXforY_Int64_rounded(KyInt32 xIn, const Vec2i& pt, const Vec2i& other_pt) 
{
	KY_ASSERT(pt.x != other_pt.x); // this function should not be called if [pt;other_pt] is vertical
	const KyInt32 pointsAtSameY = (pt.y ^ other_pt.y); // == 0 if pt.y == other_pt.y
	const KyInt32 pointAtX = (pt.x ^ xIn); // == 0 if pt.x == xIn
	if((pointsAtSameY * pointAtX) == 0)
	{
		// if pointsAtSameY == 0, the line [pt;other_pt] is horizontal, the intersection of this line and the axis X == xIn is pt.y
		// if pointAtX == 0, pt.x == xIn so the intersection is on pt.y
		return pt.y;
	}

	const KyInt64 x1 = pt.x;
	const KyInt64 y1 = pt.y;
	const KyInt64 x2 = other_pt.x;
	const KyInt64 y2 = other_pt.y;
	const KyInt64 dx01 = xIn - x1;
	const KyInt64 dx21 = x2 - x1;
	const KyInt64 dy21 = y2 - y1;
	const KyInt64 num = dx01*dy21;
	const KyInt64 denum = dx21;
	KY_ASSERT(dx01 != 0);
	KY_ASSERT(dy21 != 0);
	KY_ASSERT(denum != 0);
	const KyInt64 roundedResult = RoundedDivide(num, denum); 
	return (KyInt32)(roundedResult + y1);
}

// TODO: Return INT instead of FLOAT...
// cast from int to float to int = BAD - will LHS on PPC... 
static inline KyFloat32 EvalAtYforX_float(KyInt32 yIn, const Vec2i& pt, const Vec2i& other_pt) 
{ 
	KyInt32	/*evalAtXforYret,*/ x1, y1, dy01, dy21, 
		dx21, x2, y2;
	if(pt.x == other_pt.x)
		return (KyFloat32)pt.x;
	x1 = pt.x;
	y1 = pt.y;
	dy01 = yIn - y1;

	if(dy01 == 0)
		return (KyFloat32)x1;

	x2 = other_pt.x;
	y2 = other_pt.y;

	dx21 = x2 - x1;
	dy21 = y2 - y1;

	return (((KyFloat32)dy01 * (KyFloat32)dx21) / (KyFloat32)dy21 + (KyFloat32)x1);
}

//Returns the X value of a point at a given Y on line [pt;other_pt].
//Result is rounded to the closest int inferior to the actual position. 
//eg: 
//y_float = 1.2f => 1
//y_float = 0.8f => 0
//y_float = -0.1f => -1
//y_float = -0.9f => -1
//y_float = -1.0001f => -2
//Computation is carried out in 64 bit and should not overflow. 
//If input values are not too big (< KyInt16MaxVal), _Int32 method should be preferred. 
static inline KyInt32 EvalAtYforX_Int64(KyInt32 yIn, const Vec2i pt, const Vec2i other_pt) 
{ 
	if(pt.x == other_pt.x)
		return (KyInt32)pt.x;

	const KyInt64 x1 = (KyInt64)pt.x;
	const KyInt64 y1 = (KyInt64)pt.y;
	const KyInt64 dy01 = yIn - y1;

	if(dy01 == 0)
		return pt.x;

	const KyInt64 x2 = (KyInt64)other_pt.x;
	const KyInt64 y2 = (KyInt64)other_pt.y;

	const KyInt64 dx21 = x2 - x1;
	const KyInt64 dy21 = y2 - y1;

	const KyInt64 num = dy01*dx21;
	const KyInt64 denum = dy21;
	KyInt64 sign = num ^ denum; //sign bit will have the correct value with a XOR
	if (denum == 0) 
		return KyInt32MAXVAL - 1;

	// On x86 the remainder is a by-product of the division itself so any half-decent compiler
	// should be able to just use it (and not perform a div again). This is probably done on other architectures too.
	const KyInt64 truncatedResult = num/denum; 
	const KyInt64 remainder       = num%denum;

	// Determine if result is >0 BEFORE dividing. Otherwise If truncated result is 0, there is no way to know the real sign
	// of the result. 
	const KyInt64 truncationValue = (sign >> 63) & (remainder != 0); // 1 if sign < 0 && remainder is different from 0. = 0 otherwise
	KY_ASSERT(truncatedResult + x1 <= KyInt32MAXVAL && truncatedResult + x1 > -KyInt32MAXVAL);
	return (KyInt32)(truncatedResult + x1 - truncationValue);
}

inline KyInt32 GetSign(KyInt32 i) { return Isel(i, 1, -1); }

// Returns the closest Int whose value is equal or below the exact result of numerator/denominator
// 1/10 will return 0
// -1/10, -9/10 and obviously -10/10 will return -1 
// and so on... 
inline KyInt64 DivideTruncatedBelow(KyInt64 numerator, KyInt64 denominator)
{

	KyInt64 rawResult = numerator/denominator;
	// On x86 the remainder is a by-product of the division itself so any half-decent compiler should be able to just use it (and not perform a div again). 
	// This is probably done on other architectures too.
	KyInt64 remainder = numerator%denominator;
	// keep the absolute value of remainder
	remainder = Lsel(remainder, remainder, -remainder);
	// Determine if result is >0 BEFORE dividing. Otherwise If truncated result is 0, there is no way to know the real sign
	// of the result. 
	KyInt64 sign = Lsel(numerator, 1, -1) * Lsel(denominator, 1, -1);
	KY_ASSERT(rawResult <= KyInt32MAXVAL && rawResult > -KyInt32MAXVAL);
	return (KyInt32)Lsel(sign*remainder, rawResult, rawResult - 1);
}


template <typename T>  //T E { Vec{2,3}i, Vec2LL }
bool SegmentVsSegment2d(const Vec2i& a, const Vec2i& b, const Vec2i& c, const Vec2i& d)
{
	Box2i edge1Box, edge2Box;
	edge1Box.ExpandByVec2(a);
	edge1Box.ExpandByVec2(b);

	edge2Box.ExpandByVec2(c);
	edge2Box.ExpandByVec2(d);
/*
	Vec2i edge1 = b-a;
	Vec2i edge2 = d-c;
*/
	if (Intersections::AABBVsAABB2d(edge1Box, edge2Box))
	{
		if(a == c) 
		{
			if(b != d && ArePointsAligned<T>(a, b, d)) 
			{
				return true;
			} 
			else 
			{
				return false;
			}
		}
		if(a == d) 
		{
			if(b != c && ArePointsAligned<T>(a, b, c)) 
			{
				return true;
			}
			else 
			{
				return false;
			}
		}
		if(b == c) 
		{
			if(a != d && ArePointsAligned<T>(b, a, d))
			{
				return true;
			}
			else 
			{
				return false;
			}
		}
		if(b == d) 
		{
			if(a != c && ArePointsAligned<T>(b, a, c))
			{
				return true;
			} 
			else 
			{
				return false;
			}
		}
		int oab1 = OnAboveOrBelow<T>(a, c, d);
		if(oab1 == 0 && Between<Vec2i>(a, c, d)) return true; 
		int oab2 = OnAboveOrBelow<T>(b, c, d);
		if(oab2 == 0 && Between<Vec2i>(b, c, d)) return true; 
		if(oab1 == oab2 && oab1 != 0) return false; // both points of he1 are on same side of he2
		int oab3 = OnAboveOrBelow<T>(c, a, b);
		if(oab3 == 0 && Between<Vec2i>(c, a, b)) return true; 
		int oab4 = OnAboveOrBelow<T>(d, a, b);
		if(oab4 == 0 && Between<Vec2i>(d, a, b)) return true; 
		if(oab3 == oab4) return false; // both points of he2 are on same side of he1
		return true; // they must cross
	}
	if(IsVertical<Vec2i>(a, b) && IsVertical<Vec2i>(c, d) && a.x == c.x)
	{
		return edge1Box.Min().y < edge2Box.Max().y && edge1Box.Max().y > edge2Box.Min().y;
	}
	if(IsHorizontal<Vec2i>(a, b) && IsHorizontal<Vec2i>(c, d) && a.y == c.y)
	{
		return edge1Box.Min().x < edge2Box.Max().x && edge1Box.Max().x > edge2Box.Min().x;
	}
	return false;
}

} // namespace IntegerSegment
} //namespace Kaim


