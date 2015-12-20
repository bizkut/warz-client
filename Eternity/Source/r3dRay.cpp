#include "r3dPCH.h"
#include "r3d.h"
#include "r3dRay.h"

//////////////////////////////////////////////////////////////////////////

BOOL r3dRay::IsIntersect(const r3dBoundBox & p)
{
	r3dVector vCenter    = p.Org + p.Size * 0.5f;
	r3dVector vExtents   = p.Size * 0.5f;
	r3dVector vDiff      = org - vCenter;
	r3dVector AbsDiff    = vDiff;

	if (AbsDiff.x<0)AbsDiff.x = -AbsDiff.x;
	if (AbsDiff.y<0)AbsDiff.y = -AbsDiff.y;
	if (AbsDiff.z<0)AbsDiff.z = -AbsDiff.z;

	r3dVector vCheck     = vDiff * dir;

	if (AbsDiff.x > vExtents.x && vCheck.x >= 0.0f) return false;
	if (AbsDiff.y > vExtents.y && vCheck.y >= 0.0f) return false;
	if (AbsDiff.z > vExtents.z && vCheck.z >= 0.0f) return false;

	r3dVector vAbsDir   = dir;
	r3dVector vAbsCross = dir.Cross (vDiff);
	if (vAbsDir.x<0)vAbsDir.x = -vAbsDir.x;
	if (vAbsDir.y<0)vAbsDir.y = -vAbsDir.y;
	if (vAbsDir.z<0)vAbsDir.z = -vAbsDir.z;
	if (vAbsCross.x<0)vAbsCross.x = -vAbsCross.x;
	if (vAbsCross.y<0)vAbsCross.y = -vAbsCross.y;
	if (vAbsCross.z<0)vAbsCross.z = -vAbsCross.z;

	vCheck = (r3dVector(vExtents.y, vExtents.x, vExtents.x) *
		r3dVector(vAbsDir.z,  vAbsDir.z,  vAbsDir.y)) +
		(r3dVector(vExtents.z, vExtents.z, vExtents.y) * 
		r3dVector(vAbsDir.y,  vAbsDir.x,  vAbsDir.x));

	if (vAbsCross.x > vCheck.x) return false;
	if (vAbsCross.y > vCheck.y) return false;
	if (vAbsCross.z > vCheck.z) return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////

BOOL r3dRay::IsIntersect(const r3dSphere & s, r3dPoint3D *outIntersectionPoint) const
{
	r3dPoint3D eo = s.vCenter - org;
	float v = eo.Dot(dir);

	float dd = eo.Dot(eo);
	float disc = s.fRange * s.fRange - (dd - v * v);
	if (disc < 0)
		return FALSE;

	return TRUE;
}