/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_Triangle3i_H
#define Navigation_Triangle3i_H

#include "gwnavruntime/math/vec3i.h"


namespace Kaim
{


/// This class represents a triangle in three-dimensional space, whose dimensions are stored
/// using integer values.
/// This class is used primarily during NavData generation.
class Triangle3i
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	// ---------------------------------- Public Member Functions ----------------------------------

	/// Sets the coordinates of the three points that make up the triangle.
	/// \param _a			The coordinates of the first corner of the triangle.
	/// \param _b			The coordinates of the second corner of the triangle.
	/// \param _c			The coordinates of the third corner of the triangle. 
	void Set(const Vec3i& _a, const Vec3i& _b, const Vec3i& _c) { A = _a; B = _b; C = _c; }

	/// Retrieves the coordinates of the specified corner of the triangle. Use only [0], [1], or [2]. 
	Vec3i& operator[](KyInt32 i) { return (&A)[i]; }
	
	/// Retrieves the coordinates of the specified corner of the triangle. Use only [0], [1], or [2]. 
	const Vec3i& operator[](KyInt32 i) const { return (&A)[i]; }

	/// Returns true if the (X,Y) coordinates of the specified point lie inside the (X,Y) extents of the triangle,
	/// or exactly on one edge of the triangle. 
	KyUInt32 IsInside2d(const Vec2i& M) const { return M.IsInsideTriangle(Vec2i(A.x, A.y), Vec2i(B.x, B.y), Vec2i(C.x, C.y)); }

	/// Returns true if the (X,Y) coordinates of the specified point lie inside the (X,Y) extents of the triangle,
	/// or exactly on one edge of the triangle. This method assumes that the plane of the triangle is not horizontal. 
	bool IsInside2d_NoFlat(const Vec2i& M) const { return M.IsInsideNotColinearTriangle(Vec2i(A.x, A.y), Vec2i(B.x, B.y), Vec2i(C.x, C.y)); }

	/// Returns true if the (X,Y) coordinates of the specified point lie strictly inside the (X,Y) extents of the triangle. 
	bool IsStrictlyInside2d(const Vec2i& M) const { return M.IsStrictlyInsideTriangle(Vec2i(A.x, A.y), Vec2i(B.x, B.y), Vec2i(C.x, C.y)); }


	// ---------------------------------- Public Data Members ----------------------------------

	Vec3i A; ///< The position of the first corner of the triangle. 
	Vec3i B; ///< The position of the second corner of the triangle. 
	Vec3i C; ///< The position of the third corner of the triangle. 
};

/// Swaps the endianness of the data in the specified object. This function allows this type of object to be serialized through the blob framework.
/// The first parameter specifies the target endianness, and the second parameter provides the object whose data is to be swapped.   
inline void SwapEndianness(Endianness::Target e, Triangle3i& self)
{
	SwapEndianness(e, self.A);
	SwapEndianness(e, self.B);
	SwapEndianness(e, self.C);
}


}


#endif

