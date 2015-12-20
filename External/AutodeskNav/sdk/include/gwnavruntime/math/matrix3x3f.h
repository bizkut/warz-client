/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_Matrix3x3f_H
#define Navigation_Matrix3x3f_H


#include "gwnavruntime/math/vec3f.h"


namespace Kaim
{


/// Column-major 3x3 matrix.
class Matrix3x3f
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	// ---------------------------------- Constructors ----------------------------------

	Matrix3x3f()
	{
		SetIdentity();
	}

	Matrix3x3f(
		KyFloat32 vX_x, KyFloat32 vX_y, KyFloat32 vX_z,
		KyFloat32 vY_x, KyFloat32 vY_y, KyFloat32 vY_z,
		KyFloat32 vZ_x, KyFloat32 vZ_y, KyFloat32 vZ_z)
	{
		Set(vX_x, vX_y, vX_z,
			vY_x, vY_y, vY_z,
			vZ_x, vZ_y, vZ_z);
	}

	Matrix3x3f(const Vec3f& vX, const Vec3f& vY, const Vec3f& vZ)
	{
		Set(vX, vY, vZ);
	}

	Matrix3x3f(const Vec3f& axis, KyFloat32 angleRad)
	{
		SetRotation(axis, angleRad);
	}

	static KY_INLINE Matrix3x3f Identity()
	{
		return Matrix3x3f(
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f);
	}

	KY_INLINE void Set(
		KyFloat32 vX_x, KyFloat32 vX_y, KyFloat32 vX_z,
		KyFloat32 vY_x, KyFloat32 vY_y, KyFloat32 vY_z,
		KyFloat32 vZ_x, KyFloat32 vZ_y, KyFloat32 vZ_z)
	{
		m_vX.Set(vX_x, vX_y, vX_z);
		m_vY.Set(vY_x, vY_y, vY_z);
		m_vZ.Set(vZ_x, vZ_y, vZ_z);
	}

	KY_INLINE void Set(const Vec3f& vX, const Vec3f& vY, const Vec3f& vZ)
	{
		m_vX = vX;
		m_vY = vY;
		m_vZ = vZ;
	}

	KY_INLINE void SetIdentity()
	{
		Set(1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f);
	}

	KY_INLINE const Vec3f& GetColumn(KyUInt32 index) const
	{
		switch (index)
		{
		case 0: return m_vX;
		case 1: return m_vY;
		case 2: return m_vZ;

		default:
			{
				KY_ASSERT(index > 2);
				return m_vX;
			}
		}
	}

	KY_INLINE Vec3f& GetColumn(KyUInt32 index)
	{
		switch (index)
		{
		case 0: return m_vX;
		case 1: return m_vY;
		case 2: return m_vZ;

		default:
			{
				KY_ASSERT(index > 2);
				return m_vX;
			}
		}
	}

	KY_INLINE Vec3f GetRow(KyUInt32 index) const
	{
		switch (index)
		{
		case 0 : return Vec3f(m_vX[0], m_vY[0], m_vZ[0]);
		case 1 : return Vec3f(m_vX[1], m_vY[1], m_vZ[1]);
		case 2 : return Vec3f(m_vX[2], m_vY[2], m_vZ[2]);

		default:
			KY_ASSERT(index > 2);
			return Vec3f::Zero();
		}
	}

	/// Set as angleRad rotation matrix around axis.
	void SetRotation(const Vec3f& axis, KyFloat32 angleRad);

	/// Returns the product of this matrix and v.
	KY_INLINE Vec3f operator*(const Vec3f& v) const
	{
		return Vec3f(
			m_vX.x * v.x + m_vY.x * v.y + m_vZ.x * v.z,
			m_vX.y * v.x + m_vY.y * v.y + m_vZ.y * v.z,
			m_vX.z * v.x + m_vY.z * v.y + m_vZ.z * v.z);
	}

	Matrix3x3f operator*(const Matrix3x3f& m) const;

	KY_INLINE void Transform(const Vec3f& v, Vec3f& transformed) const
	{
		transformed.x = m_vX.x * v.x + m_vY.x * v.y + m_vZ.x * v.z;
		transformed.y = m_vX.y * v.x + m_vY.y * v.y + m_vZ.y * v.z;
		transformed.z = m_vX.z * v.x + m_vY.z * v.y + m_vZ.z * v.z;
	}

	bool operator==(const Matrix3x3f& other)
	{
		return m_vX == other.m_vX && m_vY == other.m_vY && m_vZ == other.m_vZ;
	}

	bool operator!=(const Matrix3x3f& other)
	{
		return !operator==(other);
	}

	// ---------------------------------- Public Data Members ----------------------------------

public:
	Vec3f m_vX;
	Vec3f m_vY;
	Vec3f m_vZ;
};


KY_INLINE void SwapEndianness(Endianness::Target e, Matrix3x3f& self)
{
	SwapEndianness(e, self.m_vX);
	SwapEndianness(e, self.m_vY);
	SwapEndianness(e, self.m_vZ);
}


} // namespace Kaim

#endif // Navigation_Matrix3x3f_H
