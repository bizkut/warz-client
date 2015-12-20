/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: LAPA
#ifndef Navigation_Vec3f_H
#define Navigation_Vec3f_H

#include "gwnavruntime/math/vec2f.h"
#include <math.h>

namespace Kaim
{

/// This class defines a three-dimensional vector whose coordinates are stored using
/// floating-point numbers.
/// This class is frequently used by all Gameware Navigation components to refer to specific points in
/// three-dimensional space.
class Vec3f
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	// ---------------------------------- Creation & Initialization ----------------------------------

	/// Creates a vector with coordinates (0,0,0). 
	Vec3f() : x(0.0f), y(0.0f), z(0.0f) {}

	/// Creates a vector with the specified coordinates.
	Vec3f(KyFloat32 _x, KyFloat32 _y, KyFloat32 _z) : x(_x), y(_y), z(_z) {}

	/// Creates a vector with the specified coordinates.
	/// \param coords array of coordinates {x, y, z}
	explicit Vec3f(const KyFloat32* coords) { Set(coords); }

	/// Creates a vector with provided Vec2f horizontal coordinates and z = 0.
	explicit Vec3f(const Vec2f& v) { Set(v); }

	/// Creates a vector with provided Vec2f horizontal coordinates and the provided z.
	Vec3f(const Vec2f& v, KyFloat32 _z) { Set(v, _z); }

	/// Sets the coordinates.
	KY_INLINE void Set(KyFloat32 _x, KyFloat32 _y, KyFloat32 _z) { x = _x; y = _y; z = _z; }
	
	/// Sets the coordinates.
	/// \param coords array of coordinates {x, y, z}
	KY_INLINE void Set(const KyFloat32* coords) { x = coords[0]; y = coords[1]; z = coords[2]; }

	/// Sets x = v.x, y = v.y and z = 0.0f
	KY_INLINE void Set(const Vec2f& v) { x = v.x; y = v.y; z = 0.0f; }

	/// Sets x = v.x, y = v.y and z = z
	KY_INLINE void Set(const Vec2f& v, KyFloat32 _z) { x = v.x; y = v.y; z = _z; }


	// -------------------------- Operators --------------------------------

	/// Retrieves the size of the vector around one of its axes. Use [0] for the X axis, [1] for
	/// the Y axis, or [2] for the Z axis. 
	KY_INLINE KyFloat32& operator[](KyInt32 i) { return (&x)[i]; }
	
	/// Retrieves the size of the vector around one of its axes. Use [0] for the X axis, [1] for
	/// the Y axis, or [2] for the Z axis. 
	KY_INLINE KyFloat32 operator[](KyInt32 i) const { return (&x)[i]; }

	/// Returns true if this object contains the same coordinates as v. 
	KY_INLINE bool operator==(const Vec3f& v) const { return x == v.x && y == v.y && z == v.z; }
	
	/// Returns true if this object contains at least one different coordinate from v. 
	KY_INLINE bool operator!=(const Vec3f& v) const { return x != v.x || y != v.y || z != v.z; }
	
	/// Returns true if the size of this vector along all three of the X, Y and Z axes is less than that of v. 
	KY_INLINE bool operator<(const Vec3f& v) const;
	KY_INLINE bool operator>(const Vec3f& v) const;
	KY_INLINE bool operator<=(const Vec3f& v) const;
	KY_INLINE bool operator>=(const Vec3f& v) const;

	/// Multiplies the X, Y and Z coordinates of this vector by the specified value. 
	KY_INLINE Vec3f& operator*=(KyFloat32 s) { x *= s; y *= s; z *= s; return *this; }
	
	/// Divides the X, Y and Z coordinates of this vector by the specified value. 
	KY_INLINE Vec3f& operator/=(KyFloat32 d) { return operator*=(1.0f / d); }

	/// Adds the X coordinate of v to the X coordinate of this vector, adds the Y coordinate of v
	/// to the Y coordinate of this vector, and adds the Z coordinate of v to the Z coordinate of this vector. 
	KY_INLINE Vec3f& operator+=(const Vec3f& v) { x += v.x; y += v.y; z += v.z; return *this; }

	/// Subtracts the X coordinate of v from the X coordinate of this vector, subtracts the Y coordinate of v
	/// from the Y coordinate of this vector, and subtracts the Z coordinate of v from the Z coordinate of this vector. 
	KY_INLINE Vec3f& operator-=(const Vec3f& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }

	/// Multiplies the X, Y and Z coordinates of this vector by the specified value. 
	KY_INLINE Vec3f operator*(KyFloat32 s) const { return Vec3f(x * s, y * s, z * s); }

	/// Divides the X, Y and Z coordinates of this vector by the specified value. 
	KY_INLINE Vec3f operator/(KyFloat32 d) const { return operator*(1.0f / d); }

	/// Adds the X coordinate of v to the X coordinate of this vector, adds the Y coordinate of v
	/// to the Y coordinate of this vector, and adds the Z coordinate of v to the Z coordinate of this vector. 
	KY_INLINE Vec3f operator+(const Vec3f& v) const { return Vec3f(x + v.x, y + v.y, z + v.z); }

	/// Subtracts the X coordinate of v from the X coordinate of this vector, subtracts the Y coordinate of v
	/// from the Y coordinate of this vector, and subtracts the Z coordinate of v from the Z coordinate of this vector. 
	KY_INLINE Vec3f operator-(const Vec3f& v) const { return Vec3f(x - v.x, y - v.y, z - v.z); }

	/// Negates the X, Y and Z coordinates of this vector, effectively flipping it around the origin. 
	KY_INLINE Vec3f operator-() const { return Vec3f(-x, -y, -z); }

	/// Returns the dot product of this vector and v. 
	KY_INLINE KyFloat32 operator*(const Vec3f& v) const { return x * v.x + y * v.y + z * v.z; } // DOT PRODUCT

	/// Returns the cross product of this vector and v. 
	KY_INLINE Vec3f operator^(const Vec3f& v) const { return Vec3f( (y * v.z - z * v.y), (z * v.x - x * v.z), (x * v.y - y * v.x) ); } // CROSS PRODUCT


	// -------------------------- Operations with Vec2f --------------------------------

	/// Adds the X coordinate of v to the X coordinate of this vector
	/// and the Y coordinate of v to the Y coordinate of this vector. 
	KY_INLINE Vec3f& operator+=(const Vec2f& v) { x += v.x; y += v.y; return *this; }

	/// Subtracts the X coordinate of v from the X coordinate of this vector
	/// and the Y coordinate of v from the Y coordinate of this vector. 
	KY_INLINE Vec3f& operator-=(const Vec2f& v) { x -= v.x; y -= v.y; return *this; }

	/// Adds the X coordinate of v to the X coordinate of this vector
	/// and the Y coordinate of v to the Y coordinate of this vector. 
	KY_INLINE Vec3f operator+(const Vec2f& v) const { return Vec3f(x + v.x, y + v.y, z); }

	/// Subtracts the X coordinate of v from the X coordinate of this vector
	/// and the Y coordinate of v from the Y coordinate of this vector. 
	KY_INLINE Vec3f operator-(const Vec2f& v) const { return Vec3f(x - v.x, y - v.y, z); }


	// -------------------------- Main interface --------------------------------

	/// Calculates the cross product ofu and v, and stores the result in this vector. 
	KY_INLINE void Cross(const Vec3f& u, const Vec3f& v);

	/// Returns the square of the magnitude of the vector. 
	KY_INLINE KyFloat32 GetSquareLength() const { return x * x + y * y + z * z; }
	
	/// Returns the square of the magnitude of the vector on the plane of the (X,Y) axes. 
	KY_INLINE KyFloat32 GetSquareLength2d() const { return x * x + y * y; }
	
	/// Returns the magnitude of the vector. 
	KY_INLINE KyFloat32 GetLength() const { return sqrtf(x * x + y * y + z * z); }
	
	/// Returns the magnitude of the vector on the plane of the (X,Y) axes. 
	KY_INLINE KyFloat32 GetLength2d() const { return sqrtf(x * x + y * y); }

	/// Normalizes the vector, making it one unit in length without changing its orientation.
	/// \return The previous magnitude of this vector before normalization. 
	KY_INLINE KyFloat32 Normalize();
	
	/// Normalizes the vector, making it one unit in length without changing its orientation.
	/// This method does not change this object. Instead, it stores the computed normal in
	/// the normalized parameter.
	/// \param[out] normalized			Stores the normalized vector computed from this vector.
	/// \return The magnitude of this vector before normalization. 
	KY_INLINE KyFloat32 GetNormalized(Vec3f& normalized) const;

	KY_INLINE bool IsNormalized() const;

	/// Returns a Vec2f that contains the X and Y coordinates of this vector. The Z coordinate is ignored. 
	KY_INLINE Vec2f Get2d() const { return Vec2f(x, y); }
	
	/// Normalizes the (X,Y) coordinates of the vector, making it one unit in length without changing its orientation.
	/// This method ignores the Z coordinate of the vector. This method does not change this object. Instead, it 
	/// stores the computed normal in the normalized parameter.
	/// \param[out] normalized			Stores the normalized vector computed from X and Y coordinates of this vector.
	/// \return The magnitude of the vector on the plane of the (X,Y) axes before normalization. 
	KY_INLINE KyFloat32 GetNormalized2d(Vec2f& normalized) const;

	/// Returns true if the X, Y and Z coordinates of this vector are all 0. 
	KY_INLINE bool IsZero() const { return x== 0.0f && y == 0.0f && z == 0.0f; }


	// -------------------------- Static methods --------------------------------

	/// Returns a vector of zero size: (0,0,0). 
	static KY_INLINE Vec3f Zero() { return Vec3f(0.0f, 0.0f, 0.0f); }
	
	/// Returns the normalized orientation of the X axis. 
	static KY_INLINE Vec3f UnitX() { return Vec3f(1.0f, 0.0f, 0.0f); }
	
	/// Returns the normalized orientation of the Y axis. 
	static KY_INLINE Vec3f UnitY() { return Vec3f(0.0f, 1.0f, 0.0f); }
	
	/// Returns the normalized orientation of the Z axis. 
	static KY_INLINE Vec3f UnitZ() { return Vec3f(0.0f, 0.0f, 1.0f); }


	// ---------------------------------- Public Data Members ----------------------------------

	KyFloat32 x; ///< The size of the vector along the X axis. 
	KyFloat32 y; ///< The size of the vector along the Y axis. 
	KyFloat32 z; ///< The size of the vector along the Z axis. 
};

/// Swaps the endianness of the data in the specified object. This function allows this type of object to be serialized through the blob framework.
/// The first parameter specifies the target endianness, and the second parameter provides the object whose data is to be swapped. 
KY_INLINE void SwapEndianness(Endianness::Target e, Vec3f& self)
{
	SwapEndianness(e, self.x);
	SwapEndianness(e, self.y);
	SwapEndianness(e, self.z);
}


// ----------------------------------- global functions -----------------------------------
/// Multiplies the X, Y and Z coordinates of v by s.  
KY_INLINE Vec3f operator*(KyFloat32 s, const Vec3f& v) { return Vec3f(v.x * s, v.y * s, v.z * s); }

/// Returns the dot product of v1 and v2.  
KY_INLINE KyFloat32 DotProduct(const Vec3f& v1, const Vec3f& v2) { return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z); }

/// Returns the dot product of v1 and v2, ignoring Z coordinates.  
KY_INLINE KyFloat32 DotProduct2d(const Vec3f& v1, const Vec3f& v2) { return (v1.x * v2.x + v1.y * v2.y); }

/// Returns the X component of the cross product of v1 and v2.  
KY_INLINE KyFloat32 CrossProduct_x(const Vec3f& v1, const Vec3f& v2) { return v1.y * v2.z - v1.z * v2.y; }

/// Returns the Y component of the cross product of v1 and v2.  
KY_INLINE KyFloat32 CrossProduct_y(const Vec3f& v1, const Vec3f& v2) { return v1.z * v2.x - v1.x * v2.z; }

/// Returns the Z component of the cross product of v1 and v2.  
KY_INLINE KyFloat32 CrossProduct_z(const Vec3f& v1, const Vec3f& v2) { return v1.x * v2.y - v1.y * v2.x; }

/// Returns the cross product of v1 and v2.  
KY_INLINE Vec3f CrossProduct(const Vec3f& v1, const Vec3f& v2) { return Vec3f(CrossProduct_x(v1, v2), CrossProduct_y(v1, v2), CrossProduct_z(v1, v2)); }

/// Returns the normalized horizontal vector on the right of v. If v is vertical, this return Vec3f::Zero().  
KY_INLINE Vec3f GetRightDirection(const Vec3f& v) {
	Vec3f right = CrossProduct(v, Vec3f::UnitZ());
	right.Normalize();
	return right;
}

/// Returns the square of the distance between v1 and v2.  
KY_INLINE KyFloat32 SquareDistance(const Vec3f& v1, const Vec3f& v2) { return (v2 - v1).GetSquareLength(); }

/// Returns the square of the distance between v1 and v2, ignoring Z coordinates.  
KY_INLINE KyFloat32 SquareDistance2d(const Vec3f& v1, const Vec3f& v2)
{
	const KyFloat32 dx = v2.x - v1.x;
	const KyFloat32 dy = v2.y - v1.y;
	return dx * dx + dy * dy;
}

/// Returns the distance between v1 and v2.  
KY_INLINE KyFloat32 Distance(const Vec3f& v1, const Vec3f& v2) { return sqrtf(SquareDistance(v1, v2)); }

/// Returns the distance between v1 and v2, ignoring Z coordinates.  
KY_INLINE KyFloat32 Distance2d(const Vec3f& v1, const Vec3f& v2) { return sqrtf(SquareDistance2d(v1, v2)); }

/// Get Direction
KY_INLINE Vec3f GetDir(const Vec3f& A, const Vec3f& B)
{
	Vec3f AB = B - A;
	AB.Normalize();
	return AB;
}

/// Get Direction in 2d
KY_INLINE Vec2f GetDir2d(const Vec3f& A, const Vec3f& B)
{
	Vec2f AB(B.x - A.x, B.y - A.y);
	AB.Normalize();
	return AB;
}

// ----------------------------------- KY_INLINE implementation -----------------------------------

KY_INLINE bool Vec3f::operator<(const Vec3f& v) const
{
	if (x < v.x) return true;
	if (x > v.x) return false;
	if (y < v.y) return true;
	if (y > v.y) return false;
	return (z < v.z);
}

KY_INLINE bool Vec3f::operator>(const Vec3f& v) const
{
	if (x > v.x) return true;
	if (x < v.x) return false;
	if (y > v.y) return true;
	if (y < v.y) return false;
	return (z > v.z);
}

KY_INLINE bool Vec3f::operator<=(const Vec3f& v) const { return !operator>(v); }
KY_INLINE bool Vec3f::operator>=(const Vec3f& v) const { return !operator<(v); }

KY_INLINE void Vec3f::Cross(const Vec3f& u, const Vec3f& v)
{
	x = u.y * v.z - u.z * v.y;
	y = u.z * v.x - u.x * v.z;
	z = u.x * v.y - u.y * v.x;
}


KY_INLINE KyFloat32 Vec3f::Normalize()
{
	const KyFloat32 length = GetLength();
	if (length != 0.0f)
		(*this) /= length;
	return length;
}


KY_INLINE KyFloat32 Vec3f::GetNormalized(Vec3f& normalized) const
{
	// don't call Vec3f::Normalize() to avoid LHS
	const KyFloat32 length = GetLength();
	if (length != 0.0f)
	{
		const KyFloat32 invLength = 1.f / length;
		normalized.Set(x * invLength, y * invLength, z * invLength);
		return length;
	}
	else
	{
		normalized.Set(0.f, 0.f, 0.f);
		return 0.f;
	}
}

KY_INLINE bool Vec3f::IsNormalized() const
{
	const KyFloat32 sqLength = GetSquareLength();
	return (fabsf(sqLength - 1.0f) < 1e-6f);
}

KY_INLINE KyFloat32 Vec3f::GetNormalized2d(Vec2f& normalized2d) const
{
	// don't call Vec2f::Normalize() to avoid LHS
	const KyFloat32 length = GetLength2d();
	if (length != 0.0f)
	{
		const KyFloat32 invLength = 1.0f / length;
		normalized2d.Set(x * invLength, y * invLength);
		return length;
	}
	else
	{
		normalized2d.Set(0.0f, 0.0f);
		return 0.0f;
	}
}


template <class OSTREAM>
inline OSTREAM& operator<<(OSTREAM& os, const Vec3f& v)
{
	os << "{" << v.x << ";" << v.y << ";" << v.z << "}";
	return os;
}


} // namespace Kaim

#endif
