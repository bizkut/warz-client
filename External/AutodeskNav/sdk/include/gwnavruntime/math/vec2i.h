/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_Vec2i_H
#define Navigation_Vec2i_H


#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/base/endianness.h"
#include "gwnavruntime/math/cardinaldir.h"
#include <math.h>

namespace Kaim
{


/// This class defines a two-dimensional vector whose coordinates are stored using
/// 32-bit integers.
/// This class is typically used to identify a cell within a regular two-dimensional grid.
class Vec2i
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	// ---------------------------------- Creation & Initialization ----------------------------------

	/// Creates a vector with coordinates (0,0). 
	Vec2i() : x(0), y(0) {}

	/// Creates a vector with the specified coordinates.
	/// \param _x			The size of the vector on the X axis.
	/// \param _y			The size of the vector on the Y axis. 
	Vec2i(KyInt32 _x, KyInt32 _y) : x(_x), y(_y) {}

	/// Creates a vector with the specified coordinates.
	/// \param coords		An array of two integers that contains the size of the
	/// 					vector along the X and Y axes respectively. 
	explicit Vec2i(KyInt32* coords) { Set(coords); }

	/// Sets the coordinates of the vector to match the specified values.
	/// \param _x			The size of the vector on the X axis.
	/// \param _y			The size of the vector on the Y axis. 
	KY_INLINE void Set(KyInt32 _x, KyInt32 _y) { x = _x; y = _y; }

	/// Sets the coordinates of the vector to match the specified values.
	/// \param coords		An array of two floating-point numbers that contains the size of the
	/// 					vector along the X and Y axes respectively. 
	KY_INLINE void Set(KyInt32* coords) { x = coords[0]; y = coords[1]; }


	// -------------------------- Operators --------------------------------

	/// Retrieves the size of the vector around one of its axes. Use [0] for the X axis, or [1] for
	/// the Y axis. 
	KY_INLINE KyInt32 operator[](KyInt32 idx) const { return (&x)[idx]; }

	/// Retrieves the size of the vector around one of its axes. Use [0] for the X axis, or [1] for
	/// the Y axis. Non-const and returned as a reference to allow modification of the member. 
	KY_INLINE KyInt32& operator[](KyInt32 idx) { return (&x)[idx]; }

	/// Returns true if this object contains the same coordinates as v. 
	KY_INLINE bool operator==(const Vec2i& v) const { return ((x ^ v.x) | (y ^ v.y)) == 0/*x == v.x && y == v.y*/; }

	/// Returns true if this object contains at least one different coordinate from v. 
	KY_INLINE bool operator!=(const Vec2i& v) const { return !operator==(v);/*x != v.x || y != v.y;*/ }

	/// order by x first: (1,9)<(2,8).
	KY_INLINE bool operator<(const Vec2i& v) const { return (x != v.x) ? x < v.x : y < v.y; }

	/// order by x first: (1,9)<=(2,8).
	KY_INLINE bool operator<=(const Vec2i& v) const { return (x != v.x) ? x < v.x : y <= v.y; }

	/// order by x first: (2,8)>(1,9).
	KY_INLINE bool operator>(const Vec2i& v) const { return (x != v.x) ? x > v.x : y > v.y; }

	/// order by x first: (2,8)>=(1,9).
	KY_INLINE bool operator>=(const Vec2i& v) const { return (x != v.x) ? x < v.x : y >= v.y; }

	/// Multiplies both the X and Y coordinates of this vector by the specified value. 
	KY_INLINE Vec2i& operator*=(KyInt32 s)      { x *= s;   y *= s;   return *this; }
	
	/// Divides both the X and Y coordinates of this vector by the specified value. 
	KY_INLINE Vec2i& operator/=(KyInt32 d)      { x /= d;   y /= d;   return *this; }
	
	/// Addst to both the X and Y coordinates of this vector. 
	KY_INLINE Vec2i& operator+=(KyInt32 t)      { x += t;   y += t;   return *this; }
	
	/// Subtractst from both the X and Y coordinates of this vector. 
	KY_INLINE Vec2i& operator-=(KyInt32 t)      { x -= t;   y -= t;   return *this; }
	
	/// Adds the X coordinate of v to the X coordinate of this vector, and adds the Y coordinate of v
	/// to the Y coordinate of this vector. 
	KY_INLINE Vec2i& operator+=(const Vec2i& v) { x += v.x; y += v.y; return *this; }
	
	/// Subtracts the X coordinate of v from the X coordinate of this vector, and subtracts the Y coordinate of v
	/// from the Y coordinate of this vector. 
	KY_INLINE Vec2i& operator-=(const Vec2i& v) { x -= v.x; y -= v.y; return *this; }

	/// Multiplies both the X and Y coordinates of the vector by the specified value. 
	KY_INLINE Vec2i operator*(KyInt32 s)      const { return Vec2i(x * s  , y * s  ); }
	
	/// Divides both the X and Y coordinates of the vector by the specified value. 
	KY_INLINE Vec2i operator/(KyInt32 d)      const { return Vec2i(x / d  , y / d  ); }
	
	/// Adds the X coordinate of v to the X coordinate of this vector, and adds the Y coordinate of v
	/// to the Y coordinate of this vector. 
	KY_INLINE Vec2i operator+(const Vec2i& v) const { return Vec2i(x + v.x, y + v.y); }
	
	/// Subtracts the X coordinate of v from the X coordinate of this vector, and subtracts the Y coordinate of v
	/// from the Y coordinate of this vector. 
	KY_INLINE Vec2i operator-(const Vec2i& v) const { return Vec2i(x - v.x, y - v.y); }
	
	/// Negates the X and Y coordinates of this vector, effectively flipping it around the origin. 
	KY_INLINE Vec2i operator-()               const { return Vec2i(-x     , -y     ); }

	/// Returns the dot product of this vector and v. 
	KY_INLINE KyInt32 operator*(const Vec2i& v) const { return x * v.x + y * v.y; } // DOT PROD

	/// Returns the magnitude on the Z axis of the cross product between this vector and v. 
	KY_INLINE KyInt32 operator^(const Vec2i& v) const { return CrossProd(v); } // CROSS PROD 2D aka PERP PROD

	// -------------------------- Main interface --------------------------------

	/// Returns the magnitude on the Z axis of the cross product between this vector and v. 
	KY_INLINE KyInt32 CrossProd(const Vec2i& v) const { return x * v.y - y * v.x; } // CROSS PROD 2D aka PERP PROD

	/// Indicates whether or not the polyline formed by (A to B to this vector ) outlines a triangle in
	/// counter-clockwise order when seen from above.
	/// \return	Twice the area of the triangle outlined by (A to B to this vector ).
	/// 		-	If this value is positive, (A to B to this vector ) outlines a triangle in counter-clockwise order.
	/// 		-	If this value is negative, (A to B to this vector ) outlines a triangle in clockwise order. 
	/// 		-	If this value is 0, then A,B and this vector are all colinear. 
	KY_INLINE KyInt32 Orient2d(const Vec2i& A, const Vec2i& B) const { return (A.x - x) * (B.y - y) - (B.x - x) * (A.y - y); } // > 0 if (MA,MB) is CCW

	/// Rotates this vector 90 degrees counter-clockwise (negating the Y coordinate). 
	KY_INLINE Vec2i PerpCCW() const{return Vec2i(y, -x);}

	/// Rotates this vector 90 degrees clockwise (negating the X coordinate). 
	KY_INLINE Vec2i PerpCW() const{return Vec2i(-y, x);}

	/// Returns true if this vector is on the left side when moving from A to B, or if this vector
	/// is on the line between A and B. 
	KY_INLINE bool IsOnLeftSide(const Vec2i& A, const Vec2i& B) const { return Orient2d(A, B) >= 0; }

	/// Returns true if this vector is on the left side when moving from A to B, but not if this vector
	/// is on the line between A and B. 
	KY_INLINE bool IsStrictlyOnLeftSide(const Vec2i& A, const Vec2i& B) const { return Orient2d(A, B) > 0; }

	/// Returns the square of the magnitude of the vector. 
	KY_INLINE KyInt32 GetSquareLength() const { return x * x + y * y; }


	// -------------------------- Neighbors --------------------------------

	/// Returns a vector that identifies the next grid cell to the East: i.e. in the positive direction of the X axis. 
	KY_INLINE Vec2i NeighborEast() const { return Vec2i(x + 1, y    ); }

	/// Set theneighbor vector coordinates to identify the next grid cell to the East: i.e. in the positive direction of the X axis. 
	KY_INLINE void NeighborEast(Vec2i& neighbor) const { neighbor.Set(x + 1, y    ); }
	
	/// Returns a vector that identifies the next grid cell to the West: i.e. in the negative direction of the X axis. 
	KY_INLINE Vec2i NeighborWest() const { return Vec2i(x - 1, y    ); }

	/// Set theneighbor vector coordinates to identify the next grid cell to the West: i.e. in the negative direction of the X axis. 
	KY_INLINE void NeighborWest(Vec2i& neighbor) const { neighbor.Set(x - 1, y    ); }
	
	/// Returns a vector that identifies the next grid cell to the North: i.e. in the positive direction of the Y axis. 
	KY_INLINE Vec2i NeighborNorth() const { return Vec2i(x    , y + 1); }

	/// Set theneighbor vector coordinates to identify the next grid cell to the North: i.e. in the positive direction of the Y axis. 
	KY_INLINE void NeighborNorth(Vec2i& neighbor) const { neighbor.Set(x    , y + 1); }
	
	/// Returns a vector that identifies the next grid cell to the South: i.e. in the negative direction of the Y axis. 
	KY_INLINE Vec2i NeighborSouth() const { return Vec2i(x    , y - 1); }

	/// Set theneighbor vector coordinates to identify the next grid cell to the South: i.e. in the negative direction of the Y axis. 
	KY_INLINE void NeighborSouth(Vec2i& neighbor) const { neighbor.Set(x    , y - 1); }

	/// Returns a vector that identifies the next grid cell in the direction of the specified CardinalDir. 
	KY_INLINE Vec2i Neighbor(CardinalDir dir) const
	{
		Vec2i returnCellPos;
		Neighbor(dir, returnCellPos);
		return returnCellPos;
	}

	/// Set the neighbor vector coordinates to identify the next grid cell in the direction of the specified CardinalDir. 
	KY_INLINE void Neighbor(CardinalDir dir, Vec2i& neighbor) const
	{
		static const Vec2i s_cardinalDirToNeighbor[4] =
		{
			/*CardinalDir_EAST*/Vec2i(1, 0),
			/*CardinalDir_NORTH*/Vec2i(0, 1),
			/*CardinalDir_WEST*/Vec2i(-1, 0),
			/*CardinalDir_SOUTh*/Vec2i(0, -1)
		};

		neighbor.x = x + s_cardinalDirToNeighbor[dir].x;
		neighbor.y = y + s_cardinalDirToNeighbor[dir].y;
	}

	
	/// Returns a vector that identifies the next grid cell to the North-East: 
	/// i.e. in the positive direction of the X and Y axes. 
	KY_INLINE Vec2i NeighborNorthEast()  const { return Vec2i(x + 1, y + 1); }
	
	/// Returns a vector that identifies the next grid cell to the North-West: 
	/// i.e. in the positive direction of the Y axis and the negative direction of the X axis. 
	KY_INLINE Vec2i NeighborNorthWest()  const { return Vec2i(x - 1, y + 1); }
	
	/// Returns a vector that identifies the next grid cell to the South-East: 
	/// i.e. in the positive direction of the X axis and the negative direction of the Y axis. 
	KY_INLINE Vec2i NeighborSouthEast()  const { return Vec2i(x + 1, y - 1); }
	
	/// Returns a vector that identifies the next grid cell to the South-West: 
	/// i.e. in the negative direction of the X and Y axes. 
	KY_INLINE Vec2i NeighborSouthWest()  const { return Vec2i(x - 1, y - 1); }


	// -------------------------- Triangle Inclusion Checkers --------------------------------

	/// Returns true if this vector is inside the clockwise triangle formed by theA,B and
	///C parameters, or if this vector is on any edge between A,B and
	///C. 
	KY_INLINE bool IsInsideTriangle(const Vec2i& A, const Vec2i& B, const Vec2i& C) const;

	/// Returns true if this vector is inside the clockwise triangle formed by theA,B and
	///C parameters, or if this vector is on any edge between A,B and
	///C. This method assumes that the three points that make up the triangle are not colinear: i.e.
	/// they do not lie all in a straight line. 
	KY_INLINE bool IsInsideNotColinearTriangle(const Vec2i& A, const Vec2i& B, const Vec2i& C) const;

	/// Returns true if this vector is inside the clockwise triangle formed by theA,B and
	///C parameters, but not on any edge between A,B andC.  
	KY_INLINE bool IsStrictlyInsideTriangle(const Vec2i& A, const Vec2i& B, const Vec2i& C) const;

	// -------------------------- Static methods --------------------------------

	/// Returns the normalized orientation of the X axis. 
	static KY_INLINE Vec2i UnitX() { return Vec2i(1, 0); }
	
	/// Returns the normalized orientation of the Y axis. 
	static KY_INLINE Vec2i UnitY() { return Vec2i(0, 1); }


	// ---------------------------------- Public Data Members ----------------------------------

	KyInt32 x; ///< The size of the vector along the X axis. 
	KyInt32 y; ///< The size of the vector along the Y axis. 
};

inline Vec2i operator * (KyInt32 lhs, const Vec2i& rhs) { return rhs * lhs; }

/// Swaps the endianness of the data in the specified object. This function allows this type of object to be serialized through the blob framework. 
/// The first parameter specifies the target endianness, and the second parameter provides the object whose data is to be swapped.
KY_INLINE void SwapEndianness(Kaim::Endianness::Target e, Vec2i& self)
{
	SwapEndianness(e, self.x);
	SwapEndianness(e, self.y);
}

KY_INLINE KyInt32 SquareDistance(const Vec2i& v1, const Vec2i& v2)
{
	const KyInt32 dx = v2.x - v1.x;
	const KyInt32 dy = v2.y - v1.y;
	return dx * dx + dy * dy;
}

KY_INLINE bool Vec2i::IsInsideTriangle(const Vec2i& A, const Vec2i& B, const Vec2i& C) const
{
	if (A.Orient2d(B, C) == 0) // flat triangle
	{
		if (A.x == B.x) // vertical
		{
			// not vertical
			if (y < A.y && y < B.y && y < C.y) 
				return false; // M is below of ABC
			if (y > A.y && y > B.y && y > C.y)
				return false; // M is above of ABC
			return true; // M is on ABC
		}
		else // not vertical
		{
			if (x < A.x && x < B.x && x < C.x) 
				return false; // M is "left" of ABC
			if (x > A.x && x > B.x && x > C.x)
				return false; // M is "right" of ABC
			return true; // M is on ABC
		}
	}

	return IsInsideNotColinearTriangle(A, B, C);
}


KY_INLINE bool Vec2i::IsInsideNotColinearTriangle(const Vec2i& A, const Vec2i& B, const Vec2i& C) const
{
	return IsOnLeftSide(A, B) && IsOnLeftSide(B, C) && IsOnLeftSide(C, A);
}


KY_INLINE bool Vec2i::IsStrictlyInsideTriangle(const Vec2i& A, const Vec2i& B, const Vec2i& C) const
{
	return IsStrictlyOnLeftSide(A, B) && IsStrictlyOnLeftSide(B, C) && IsStrictlyOnLeftSide(C, A);
}

/// Returns the dot product of v1 and v2.  
KY_INLINE KyInt32 DotProduct(const Vec2i& v1, const Vec2i& v2) { return v1 * v2; }

/// Returns the Z component of the cross product of v1 and v2.  
KY_INLINE KyInt32 CrossProduct(const Vec2i& v1, const Vec2i& v2) { return v1.CrossProd(v2); }


template <typename T> KY_INLINE T CrossProductT(const Vec2i& v1, const Vec2i& v2) { return (T)(v1.x) * (T)(v2.y) - (T)(v1.y) * (T)(v2.x); }
template <typename T> KY_INLINE T DotProductT(const Vec2i& v1, const Vec2i& v2) { return (T)(v1.x) * (T)(v2.x) + (T)(v1.y) * (T)(v2.y); }

template <class OSTREAM>
inline OSTREAM& operator<<(OSTREAM& os, const Kaim::Vec2i& v)
{
	os << "{" << v.x << ";" << v.y << "}";
	return os;
}

} // namespace Kaim



#endif

