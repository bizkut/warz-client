/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_Vec2L_H
#define Navigation_Vec2L_H


#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/base/endianness.h"
#include "gwnavruntime/math/cardinaldir.h"
#include <math.h>


namespace Kaim
{


/// This class defines a two-dimensional vector whose coordinates are stored using
/// 64-bit integers.
/// This class is typically used to identify a cell within a regular two-dimensional grid.
class Vec2LL
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	// ---------------------------------- Creation & Initialization ----------------------------------

	/// Creates a vector with coordinates (0,0). 
	Vec2LL() : x(0), y(0) {}

	/// Creates a vector with the specified coordinates.
	/// \param _x			The size of the vector on the X axis.
	/// \param _y			The size of the vector on the Y axis. 
	Vec2LL(KyInt64 _x, KyInt64 _y) : x(_x), y(_y) {}

	/// Creates a vector with the specified coordinates.
	/// \param coords		An array of two integers that contains the size of the
	/// 					vector along the X and Y axes respectively. 
	explicit Vec2LL(KyInt64* coords) { Set(coords); }

	/// Sets the coordinates of the vector to match the specified values.
	/// \param _x			The size of the vector on the X axis.
	/// \param _y			The size of the vector on the Y axis. 
	KY_INLINE void Set(KyInt64 _x, KyInt64 _y) { x = _x; y = _y; }

	/// Sets the coordinates of the vector to match the specified values.
	/// \param coords		An array of two floating-point numbers that contains the size of the
	/// 					vector along the X and Y axes respectively. 
	KY_INLINE void Set(KyInt64* coords) { x = coords[0]; y = coords[1]; }


	// -------------------------- Operators --------------------------------

	/// Retrieves the size of the vector around one of its axes. Use [0] for the X axis, or [1] for
	/// the Y axis. 
	KY_INLINE KyInt64 operator[](KyInt64 idx) const { return (&x)[idx]; }

	/// Retrieves the size of the vector around one of its axes. Use [0] for the X axis, or [1] for
	/// the Y axis. Non-const and returned as a reference to allow modification of the member. 
	KY_INLINE KyInt64& operator[](KyInt64 idx) { return (&x)[idx]; }

	/// Returns true if this object contains the same coordinates as v. 
	KY_INLINE bool operator==(const Vec2LL& v) const { return x == v.x && y == v.y; }

	/// Returns true if this object contains at least one different coordinate from v. 
	KY_INLINE bool operator!=(const Vec2LL& v) const { return !operator==(v);/*x != v.x || y != v.y;*/ }

	/// Returns true if the size of this vector along both the X and Y axes is less than that of v. 
	KY_INLINE bool operator<(const Vec2LL& v) const
	{
		if (x < v.x) return true;
		if (x > v.x) return false;
		return (y < v.y);
	}

	/// Multiplies both the X and Y coordinates of this vector by the specified value. 
	KY_INLINE Vec2LL& operator*=(KyInt64 s)      { x *= s;   y *= s;   return *this; }
	
	/// Divides both the X and Y coordinates of this vector by the specified value. 
	KY_INLINE Vec2LL& operator/=(KyInt64 d)      { x /= d;   y /= d;   return *this; }
	
	/// Addst to both the X and Y coordinates of this vector. 
	KY_INLINE Vec2LL& operator+=(KyInt64 t)      { x += t;   y += t;   return *this; }
	
	/// Subtractst from both the X and Y coordinates of this vector. 
	KY_INLINE Vec2LL& operator-=(KyInt64 t)      { x -= t;   y -= t;   return *this; }
	
	/// Adds the X coordinate of v to the X coordinate of this vector, and adds the Y coordinate of v
	/// to the Y coordinate of this vector. 
	KY_INLINE Vec2LL& operator+=(const Vec2LL& v) { x += v.x; y += v.y; return *this; }
	
	/// Subtracts the X coordinate of v from the X coordinate of this vector, and subtracts the Y coordinate of v
	/// from the Y coordinate of this vector. 
	KY_INLINE Vec2LL& operator-=(const Vec2LL& v) { x -= v.x; y -= v.y; return *this; }

	/// Multiplies both the X and Y coordinates of the vector by the specified value. 
	KY_INLINE Vec2LL operator*(KyInt64 s)      const { return Vec2LL(x * s  , y * s  ); }
	
	/// Divides both the X and Y coordinates of the vector by the specified value. 
	KY_INLINE Vec2LL operator/(KyInt64 d)      const { return Vec2LL(x / d  , y / d  ); }
	
	/// Adds the X coordinate of v to the X coordinate of this vector, and adds the Y coordinate of v
	/// to the Y coordinate of this vector. 
	KY_INLINE Vec2LL operator+(const Vec2LL& v) const { return Vec2LL(x + v.x, y + v.y); }
	
	/// Subtracts the X coordinate of v from the X coordinate of this vector, and subtracts the Y coordinate of v
	/// from the Y coordinate of this vector. 
	KY_INLINE Vec2LL operator-(const Vec2LL& v) const { return Vec2LL(x - v.x, y - v.y); }
	
	/// Negates the X and Y coordinates of this vector, effectively flipping it around the origin. 
	KY_INLINE Vec2LL operator-()               const { return Vec2LL(-x     , -y     ); }

	/// Returns the dot product of this vector and v. 
	KY_INLINE KyInt64 operator*(const Vec2LL& v) const { return x * v.x + y * v.y; } // DOT PROD

	/// Returns the magnitude on the Z axis of the cross product between this vector and v. 
	KY_INLINE KyInt64 operator^(const Vec2LL& v) const { return CrossProd(v); } // CROSS PROD 2D aka PERP PROD


	// -------------------------- Main interface --------------------------------

	/// Returns the magnitude on the Z axis of the cross product between this vector and v. 
	KY_INLINE KyInt64 CrossProd(const Vec2LL& v) const { return x * v.y - y * v.x; } // CROSS PROD 2D aka PERP PROD

	/// Returns the square of the magnitude of the vector. 
	KY_INLINE KyInt64 GetSquareLength() const { return x * x + y * y; }

	/// Rotates this vector 90 degrees counter-clockwise (negating the Y coordinate). 
	KY_INLINE Vec2LL PerpCCW() const{return Vec2LL(y, -x);}

	/// Rotates this vector 90 degrees clockwise (negating the X coordinate). 
	KY_INLINE Vec2LL PerpCW() const{return Vec2LL(-y, x);}

	KY_INLINE void SetAdditionResult(const Vec2LL& v1, const Vec2LL& v2) { x = v1.x + v2.x; y = v1.y + v2.y; }


	// -------------------------- Static methods --------------------------------

	/// Returns the normalized orientation of the X axis. 
	static KY_INLINE Vec2LL UnitX() { return Vec2LL(1, 0); }
	
	/// Returns the normalized orientation of the Y axis. 
	static KY_INLINE Vec2LL UnitY() { return Vec2LL(0, 1); }


	// ---------------------------------- Public Data Members ----------------------------------

	KyInt64 x; ///< The size of the vector along the X axis. 
	KyInt64 y; ///< The size of the vector along the Y axis. 
};

inline Vec2LL operator * (KyInt64 lhs, const Vec2LL& rhs) { return rhs * lhs; }

/// Returns the dot product of v1 and v2.  
KY_INLINE KyInt64 DotProduct(const Vec2LL& v1, const Vec2LL& v2) { return v1 * v2; }

/// Returns the Z component of the cross product of v1 and v2.  
KY_INLINE KyInt64 CrossProduct(const Vec2LL& v1, const Vec2LL& v2) { return v1.CrossProd(v2); }


template <class OSTREAM>
inline OSTREAM& operator<<(OSTREAM& os, const Kaim::Vec2LL& v)
{
	os << "{" << v.x << ";" << v.y << "}";
	return os;
}

} // namespace Kaim



#endif

