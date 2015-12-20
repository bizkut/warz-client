/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_Transform_H
#define Navigation_Transform_H


#include "gwnavruntime/math/matrix3x3f.h"


namespace Kaim
{

/// This class represents a three-dimensional 6 freedom degrees unit transform.
class Transform
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	// ---------------------------------- Public Member Functions ----------------------------------

	Transform(const Matrix3x3f& matrix = Matrix3x3f::Identity(), const Vec3f& position = Vec3f::Zero())
		: m_rotationMatrix(matrix)
		, m_translation(position)
	{}

	void SetIdentity()
	{
		m_translation = Vec3f::Zero();
		m_rotationMatrix.SetIdentity();
	}

	bool operator!=(const Transform& other)
	{
		return (
			(m_rotationMatrix != other.m_rotationMatrix) ||
			(m_translation != other.m_translation)
			);
	}

	bool operator==(const Transform& other)
	{
		return !(*this != other);
	}

	/// Compute vector coordinates in root coordinate system for the local vector v.
	/// It just applies rotation matrix transformation, ignoring translation.
	KY_INLINE Vec3f GetRootVector(const Vec3f& v) const { return m_rotationMatrix * v; }

	/// \copydoc GetRootVector()
	KY_INLINE void GetRootVector(const Vec3f& v, Vec3f& w) const { w = m_rotationMatrix * v; }

	/// Compute position coordinates in root coordinate system for the local position v.
	/// It applies rotation matrix transformation and then add the translation.
	KY_INLINE Vec3f GetRootCoordinates(const Vec3f& v) const { return m_translation + GetRootVector(v); }

	/// \copydoc GetRootCoordinates()
	KY_INLINE void GetRootCoordinates(const Vec3f& v, Vec3f& w) const { GetRootVector(v, w); w += m_translation; }


	// ---------------------------------- Public Data Members ----------------------------------

	Matrix3x3f m_rotationMatrix;
	Vec3f m_translation;
};

/// Swaps the endianness of the data in the specified object. This function allows this type of object to be serialized through the blob framework.
/// The first parameter specifies the target endianness, and the second parameter provides the object whose data is to be swapped. 
KY_INLINE void SwapEndianness(Endianness::Target e, Transform& self)
{
	SwapEndianness(e, self.m_rotationMatrix);
	SwapEndianness(e, self.m_translation);
}


} // namespace Kaim

#endif // Navigation_Transform_H
