/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: LASI

namespace Kaim
{


namespace CoordSystem_Internal
{
	inline void SetAxis(CoordSystem::ClientAxis axis, KyInt32& axisIdx, KyFloat32& axisSign)
	{
		axisIdx = axis / 2;
		axisSign = (axis % 2) ? -1.0f : 1.0f;
	}

	inline CoordSystem::ClientAxis GetAxis(KyInt32 axisIdx, KyFloat32 axisSign)
	{
		if (axisSign > 0.0f) // axisSign is 1.f or -1.f
			return (CoordSystem::ClientAxis)(axisIdx * 2);
		else
			return (CoordSystem::ClientAxis)((axisIdx * 2) + 1);
	}
}


inline void CoordSystem::Setup(
	KyFloat32 oneMeterInClientUnits,
	ClientAxis clientAxisForNavigationX,
	ClientAxis clientAxisForNavigationY,
	ClientAxis clientAxisForNavigationZ)
{
	SetOneMeterInClientUnits(oneMeterInClientUnits);

	SetClientAxisForNavigationX(clientAxisForNavigationX);
	SetClientAxisForNavigationY(clientAxisForNavigationY);
	SetClientAxisForNavigationZ(clientAxisForNavigationZ);
}


inline void CoordSystem::Setup(const CoordSystemConfig& config)
{
	Setup(
		config.m_oneMeterInClientUnits,
		(ClientAxis)config.m_clientAxisForNavigationX,
		(ClientAxis)config.m_clientAxisForNavigationY,
		(ClientAxis)config.m_clientAxisForNavigationZ);
}


inline void CoordSystem::SetOneMeterInClientUnits(KyFloat32 oneMeterInClientUnits)
{
	m_oneMeter = oneMeterInClientUnits;
	m_inv_oneMeter = 1.0f / oneMeterInClientUnits;
}

inline void CoordSystem::SetClientAxisForNavigationX(ClientAxis clientAxisForNavigationX) { CoordSystem_Internal::SetAxis(clientAxisForNavigationX, m_navigationX_idx, m_navigationX_sign); }
inline void CoordSystem::SetClientAxisForNavigationY(ClientAxis clientAxisForNavigationY) { CoordSystem_Internal::SetAxis(clientAxisForNavigationY, m_navigationY_idx, m_navigationY_sign); }
inline void CoordSystem::SetClientAxisForNavigationZ(ClientAxis clientAxisForNavigationZ) { CoordSystem_Internal::SetAxis(clientAxisForNavigationZ, m_navigationZ_idx, m_navigationZ_sign); }

inline CoordSystem::ClientAxis CoordSystem::GetClientAxisForNavigationX() { return CoordSystem_Internal::GetAxis(m_navigationX_idx, m_navigationX_sign); }
inline CoordSystem::ClientAxis CoordSystem::GetClientAxisForNavigationY() { return CoordSystem_Internal::GetAxis(m_navigationY_idx, m_navigationY_sign); }
inline CoordSystem::ClientAxis CoordSystem::GetClientAxisForNavigationZ() { return CoordSystem_Internal::GetAxis(m_navigationZ_idx, m_navigationZ_sign); }


// ------------------------------------ Client <-> Gameware Navigation: Scalars ------------------------------------

inline KyFloat32 CoordSystem::ClientToNavigation_Dist(KyFloat32 clientDist) const { return clientDist * m_inv_oneMeter; }
inline KyFloat32 CoordSystem::NavigationToClient_Dist(KyFloat32 navigationDist) const { return navigationDist * m_oneMeter; }

inline KyFloat32 CoordSystem::ClientToNavigation_SquareDist(KyFloat32 clientSquareDist) const { return clientSquareDist * m_inv_oneMeter * m_inv_oneMeter; }
inline KyFloat32 CoordSystem::NavigationToClient_SquareDist(KyFloat32 navigationSquareDist) const { return navigationSquareDist * m_oneMeter * m_oneMeter; }


// ------------------ Client <-> Gameware Navigation: Vectors ------------------

inline void CoordSystem::ClientToNavigation_Pos(const Vec3f& clientPos, Vec3f& navigationPos) const
{
	navigationPos.x = ClientToNavigation_Dist(m_navigationX_sign * clientPos[m_navigationX_idx]);
	navigationPos.y = ClientToNavigation_Dist(m_navigationY_sign * clientPos[m_navigationY_idx]);
	navigationPos.z = ClientToNavigation_Dist(m_navigationZ_sign * clientPos[m_navigationZ_idx]);
}
inline void CoordSystem::NavigationToClient_Pos(const Vec3f& navigationPos, Vec3f& clientPos) const
{
	clientPos[m_navigationX_idx] = NavigationToClient_Dist(m_navigationX_sign * navigationPos.x);
	clientPos[m_navigationY_idx] = NavigationToClient_Dist(m_navigationY_sign * navigationPos.y);
	clientPos[m_navigationZ_idx] = NavigationToClient_Dist(m_navigationZ_sign * navigationPos.z);
}
inline Vec3f CoordSystem::ClientToNavigation_Pos(const Vec3f& clientPos) const
{
	Vec3f navigationPos;
	ClientToNavigation_Pos(clientPos, navigationPos);
	return navigationPos;
}
inline Vec3f CoordSystem::NavigationToClient_Pos(const Vec3f& navigationPos) const
{
	Vec3f clientPos;
	NavigationToClient_Pos(navigationPos, clientPos);
	return clientPos;
}


inline void CoordSystem::ClientToNavigation_Normal(const Vec3f& clientNormal, Vec3f& navigationNormal) const
{
	navigationNormal.x = m_navigationX_sign * clientNormal[m_navigationX_idx];
	navigationNormal.y = m_navigationY_sign * clientNormal[m_navigationY_idx];
	navigationNormal.z = m_navigationZ_sign * clientNormal[m_navigationZ_idx];
}
inline void CoordSystem::NavigationToClient_Normal(const Vec3f& navigationNormal, Vec3f& clientNormal) const
{
	clientNormal[m_navigationX_idx] = m_navigationX_sign * navigationNormal.x;
	clientNormal[m_navigationY_idx] = m_navigationY_sign * navigationNormal.y;
	clientNormal[m_navigationZ_idx] = m_navigationZ_sign * navigationNormal.z;
}
inline Vec3f CoordSystem::ClientToNavigation_Normal(const Vec3f& clientNormal) const
{
	Vec3f navigationNormal;
	ClientToNavigation_Normal(clientNormal, navigationNormal);
	return navigationNormal;
}
inline Vec3f CoordSystem::NavigationToClient_Normal(const Vec3f& navigationNormal) const
{
	Vec3f clientNormal;
	NavigationToClient_Normal(navigationNormal, clientNormal);
	return clientNormal;
}

inline void CoordSystem::ClientToNavigation_AngularVelocity(const Vec3f& clientAngularVelocity, Vec3f& navigationAngularVelocity) const
{
	ClientToNavigation_Normal(clientAngularVelocity, navigationAngularVelocity); // Axis direction has to be converted, length express the rotation speed in rad/s
}
inline void CoordSystem::NavigationToClient_AngularVelocity(const Vec3f& navigationAngularVelocity, Vec3f& clientAngularVelocity) const
{
	NavigationToClient_Normal(navigationAngularVelocity, clientAngularVelocity); // Axis direction has to be converted, length express the rotation speed in rad/s
}
inline Vec3f CoordSystem::ClientToNavigation_AngularVelocity(const Vec3f& clientAngularVelocity) const
{
	Vec3f navigationAngularVelocity;
	ClientToNavigation_AngularVelocity(clientAngularVelocity, navigationAngularVelocity);
	return navigationAngularVelocity;
}
inline Vec3f CoordSystem::NavigationToClient_AngularVelocity(const Vec3f& navigationAngularVelocity) const
{
	Vec3f clientAngularVelocity;
	NavigationToClient_AngularVelocity(navigationAngularVelocity, clientAngularVelocity);
	return clientAngularVelocity;
}

inline void CoordSystem::ClientToNavigation_Extents(const Vec3f& clientExtents, Vec3f& navigationExtents) const
{
	KY_ASSERT(clientExtents[0] >= 0.0f);
	KY_ASSERT(clientExtents[1] >= 0.0f);
	KY_ASSERT(clientExtents[2] >= 0.0f);
	navigationExtents.x = ClientToNavigation_Dist(clientExtents[m_navigationX_idx]);
	navigationExtents.y = ClientToNavigation_Dist(clientExtents[m_navigationY_idx]);
	navigationExtents.z = ClientToNavigation_Dist(clientExtents[m_navigationZ_idx]);
}
inline void CoordSystem::NavigationToClient_Extents(const Vec3f& navigationExtents, Vec3f& clientExtents) const
{
	KY_ASSERT(navigationExtents[0] >= 0.0f);
	KY_ASSERT(navigationExtents[1] >= 0.0f);
	KY_ASSERT(navigationExtents[2] >= 0.0f);
	clientExtents[m_navigationX_idx] = NavigationToClient_Dist(navigationExtents.x);
	clientExtents[m_navigationY_idx] = NavigationToClient_Dist(navigationExtents.y);
	clientExtents[m_navigationZ_idx] = NavigationToClient_Dist(navigationExtents.z);
}
inline Vec3f CoordSystem::ClientToNavigation_Extents(const Vec3f& clientExtents) const
{
	Vec3f navigationExtents;
	ClientToNavigation_Extents(clientExtents, navigationExtents);
	return navigationExtents;
}
inline Vec3f CoordSystem::NavigationToClient_Extents(const Vec3f& navigationExtents) const
{
	Vec3f clientExtents;
	NavigationToClient_Extents(navigationExtents, clientExtents);
	return clientExtents;
}


// ------------------ Client <-> Gameware Navigation: Triangles ------------------

inline void CoordSystem::ClientToNavigation_Triangle(const Triangle3f& navigationTriangle, Triangle3f& clientTriangle) const
{
	clientTriangle.A = ClientToNavigation_Pos(navigationTriangle.A);

	if (IsRightHanded())
	{
		clientTriangle.B = ClientToNavigation_Pos(navigationTriangle.B);
		clientTriangle.C = ClientToNavigation_Pos(navigationTriangle.C);
	}
	else
	{
		clientTriangle.C = ClientToNavigation_Pos(navigationTriangle.B);
		clientTriangle.B = ClientToNavigation_Pos(navigationTriangle.C);
	}
}


inline void CoordSystem::NavigationToClient_Triangle(const Triangle3f& navigationTriangle, Triangle3f& clientTriangle) const
{
	clientTriangle.A = NavigationToClient_Pos(navigationTriangle.A);

	if (IsRightHanded())
	{
		clientTriangle.B = NavigationToClient_Pos(navigationTriangle.B);
		clientTriangle.C = NavigationToClient_Pos(navigationTriangle.C);
	}
	else
	{
		clientTriangle.C = NavigationToClient_Pos(navigationTriangle.B);
		clientTriangle.B = NavigationToClient_Pos(navigationTriangle.C);
	}
}


// ------------------ Client <-> Gameware Navigation: Boxes ------------------

inline void CoordSystem::ClientToNavigation_Box(const Vec3f& clientMin, const Vec3f& clientMax, Box3f& navigationBox) const
{
	navigationBox.Clear();
	navigationBox.ExpandByVec3(ClientToNavigation_Pos(clientMin));
	navigationBox.ExpandByVec3(ClientToNavigation_Pos(clientMax));
}
inline void CoordSystem::ClientToNavigation_Box(const Box3f& clientBox, Box3f& navigationBox) const
{
	ClientToNavigation_Box(clientBox.m_min, clientBox.m_max, navigationBox);
}
inline void CoordSystem::NavigationToClient_Box(const Box3f& navigationBox, Vec3f& clientMin, Vec3f& clientMax) const
{
	Box3f clientBox;
	clientBox.ExpandByVec3(NavigationToClient_Pos(navigationBox.m_min));
	clientBox.ExpandByVec3(NavigationToClient_Pos(navigationBox.m_max));
	clientMin = clientBox.m_min;
	clientMax = clientBox.m_max;
}
inline void CoordSystem::NavigationToClient_Box(const Box3f& navigationBox, Box3f& clientBox) const
{
	clientBox.Clear();
	clientBox.ExpandByVec3(NavigationToClient_Pos(navigationBox.m_min));
	clientBox.ExpandByVec3(NavigationToClient_Pos(navigationBox.m_max));
}


// ------------------ Client <-> Gameware Navigation: Matrixes & Transforms ------------------

inline void CoordSystem::ClientToNavigation_Matrix(const Matrix3x3f& clientMatrix, Matrix3x3f& navigationMatrix) const
{
	const Vec3f& c_Nx = clientMatrix.GetColumn(m_navigationX_idx);
	const Vec3f& c_Ny = clientMatrix.GetColumn(m_navigationY_idx);
	const Vec3f& c_Nz = clientMatrix.GetColumn(m_navigationZ_idx);

	navigationMatrix.m_vX[0] = c_Nx[m_navigationX_idx];
	navigationMatrix.m_vX[1] = c_Nx[m_navigationY_idx] * m_navigationX_sign * m_navigationY_sign;
	navigationMatrix.m_vX[2] = c_Nx[m_navigationZ_idx] * m_navigationX_sign * m_navigationZ_sign;

	navigationMatrix.m_vY[0] = c_Ny[m_navigationX_idx] * m_navigationY_sign * m_navigationX_sign;
	navigationMatrix.m_vY[1] = c_Ny[m_navigationY_idx];
	navigationMatrix.m_vY[2] = c_Ny[m_navigationZ_idx] * m_navigationY_sign * m_navigationZ_sign;

	navigationMatrix.m_vZ[0] = c_Nz[m_navigationX_idx] * m_navigationZ_sign * m_navigationX_sign;
	navigationMatrix.m_vZ[1] = c_Nz[m_navigationY_idx] * m_navigationZ_sign * m_navigationY_sign;
	navigationMatrix.m_vZ[2] = c_Nz[m_navigationZ_idx];
}

inline void CoordSystem::NavigationToClient_Matrix(const Matrix3x3f& navigationMatrix, Matrix3x3f& clientMatrix) const
{
	Vec3f& c_Nx = clientMatrix.GetColumn(m_navigationX_idx);
	Vec3f& c_Ny = clientMatrix.GetColumn(m_navigationY_idx);
	Vec3f& c_Nz = clientMatrix.GetColumn(m_navigationZ_idx);

	c_Nx[m_navigationX_idx] = navigationMatrix.m_vX[0];
	c_Nx[m_navigationY_idx] = navigationMatrix.m_vX[1] * m_navigationX_sign * m_navigationY_sign;
	c_Nx[m_navigationZ_idx] = navigationMatrix.m_vX[2] * m_navigationX_sign * m_navigationZ_sign;

	c_Ny[m_navigationX_idx] = navigationMatrix.m_vY[0] * m_navigationY_sign * m_navigationX_sign;
	c_Ny[m_navigationY_idx] = navigationMatrix.m_vY[1];
	c_Ny[m_navigationZ_idx] = navigationMatrix.m_vY[2] * m_navigationY_sign * m_navigationZ_sign;

	c_Nz[m_navigationX_idx] = navigationMatrix.m_vZ[0] * m_navigationZ_sign * m_navigationX_sign;
	c_Nz[m_navigationY_idx] = navigationMatrix.m_vZ[1] * m_navigationZ_sign * m_navigationY_sign;
	c_Nz[m_navigationZ_idx] = navigationMatrix.m_vZ[2];
}

inline void CoordSystem::ClientToNavigation_Tranform(const Transform& clientTranform, Transform& navigationTranform) const
{
	ClientToNavigation_Matrix(clientTranform.m_rotationMatrix, navigationTranform.m_rotationMatrix);
	ClientToNavigation_Pos(clientTranform.m_translation, navigationTranform.m_translation);
}
inline void CoordSystem::NavigationToClient_Tranform(const Transform& navigationTranform, Transform& clientTranform) const
{
	NavigationToClient_Matrix(navigationTranform.m_rotationMatrix, clientTranform.m_rotationMatrix);
	NavigationToClient_Pos(navigationTranform.m_translation, clientTranform.m_translation);
}


// ------------------ Other Member Functions ------------------


inline bool CoordSystem::operator==(const CoordSystem& other) const
{
	return m_navigationX_idx == other.m_navigationX_idx && m_navigationX_sign == other.m_navigationX_sign
		&& m_navigationY_idx == other.m_navigationY_idx && m_navigationY_sign == other.m_navigationY_sign
		&& m_navigationZ_idx == other.m_navigationZ_idx && m_navigationZ_sign == other.m_navigationZ_sign
		&& m_oneMeter == other.m_oneMeter;
}

inline bool CoordSystem::operator!=(const CoordSystem& other) const
{
	return !operator==(other);
}

inline bool CoordSystem::IsRightHanded() const
{
	Vec3f X(0.f, 0.f, 0.f);
	Vec3f Y(0.f, 0.f, 0.f);
	Vec3f Z(0.f, 0.f, 0.f);

	X[m_navigationX_idx] = 1.f * m_navigationX_sign;
	Y[m_navigationY_idx] = 1.f * m_navigationY_sign;
	Z[m_navigationZ_idx] = 1.f * m_navigationZ_sign;

	// calculate determinant
	const KyFloat32 det = X[0] * (Y[1] * Z[2] - Y[2] * Z[1])
		- X[1] * (Y[0] * Z[2] - Y[2] * Z[0])
		+ X[2] * (Y[0] * Z[1] - Y[1] * Z[0]);

	return det > 0.f;
}

}

