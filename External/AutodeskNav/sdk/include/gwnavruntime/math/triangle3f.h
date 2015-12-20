/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: MAMU - secondary contact: NOBODY
#ifndef Navigation_Triangle3f_H
#define Navigation_Triangle3f_H

#include "gwnavruntime/math/vec3f.h"


namespace Kaim
{


/// This class represents a triangle in three-dimensional space, whose dimensions are stored using float values.
/// This class is used primarily for sending blobs containing triangle information.
class Triangle3f
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	// ---------------------------------- Public Member Functions ----------------------------------

	/// Sets the coordinates of the three points that make up the triangle.
	/// \param _a			The coordinates of the first corner of the triangle.
	/// \param _b			The coordinates of the second corner of the triangle.
	/// \param _c			The coordinates of the third corner of the triangle. 
	void Set(const Vec3f& _a, const Vec3f& _b, const Vec3f& _c) { A = _a; B = _b; C = _c; }

	/// Retrieves the coordinates of the specified corner of the triangle. Use only [0], [1], or [2]. 
	Vec3f& operator[](KyInt32 i) { return (&A)[i]; }
	
	/// Retrieves the coordinates of the specified corner of the triangle. Use only [0], [1], or [2]. 
	const Vec3f& operator[](KyInt32 i) const { return (&A)[i]; }


	// ---------------------------------- Public Data Members ----------------------------------

	Vec3f A; ///< The position of the first corner of the triangle. 
	Vec3f B; ///< The position of the second corner of the triangle. 
	Vec3f C; ///< The position of the third corner of the triangle. 
};

/// Swaps the endianness of the data in the specified object. This function allows this type of object to be serialized through the blob framework.
/// The first parameter specifies the target endianness, and the second parameter provides the object whose data is to be swapped.   
inline void SwapEndianness(Endianness::Target e, Triangle3f& self)
{
	SwapEndianness(e, self.A);
	SwapEndianness(e, self.B);
	SwapEndianness(e, self.C);
}


}


#endif

