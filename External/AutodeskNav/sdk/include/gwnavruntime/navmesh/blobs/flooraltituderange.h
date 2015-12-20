/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_FloorAltitudeRange_H
#define Navigation_FloorAltitudeRange_H

#include "gwnavruntime/navmesh/navmeshtypes.h"

namespace Kaim
{

/// Represents the range of altitudes covered by a single NavFloorBlob.
class FloorAltitudeRange
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
public:
	FloorAltitudeRange() { Clear(); }

	void Clear()
	{
		m_maxZ = -KyFloat32MAXVAL;
		m_minZ = KyFloat32MAXVAL;
	}

	/// Returns true if the specified coordinates are within the range of altitudes represented by this object.
	/// \param altitude			The altitude of the point to test.
	/// \param toleranceAboveFloor	Specifies an extra padding distance in meters that is added to the maximum altitude of this range  before the test.
	/// \param toleranceBelowFloor	Specifies an extra padding distance in meters that is subtracted from the minimum altitude of the range before the test.
	bool IsAltitudeInside(KyFloat32 altitude, KyFloat32 toleranceAboveFloor, KyFloat32 toleranceBelowFloor) const;

	/// Returns true if the range of altitudes represented by this object intersects the range represented by the specified FloorAltitudeRange. 
	bool DoesIntersect(const FloorAltitudeRange& other) const;

	/// Returns true if the range of altitudes represented by this object intersects the range zmin and zmax arguments. 
	bool DoesIntersect(KyFloat32 zmin, KyFloat32 zmax) const;

	void Update(KyFloat32 altitude);

	KyFloat32 GetAltDiff() const { return m_maxZ - m_minZ; }
public:
	KyFloat32 m_minZ;  ///< Stores the minimum altitude covered by this range. Do not modify.
	KyFloat32 m_maxZ;  ///< Stores the maximum altitude covered by this range. Do not modify.
};

/// Swaps the endianness of the data in the specified object. This function allows this type of object to be serialized through the blob framework. 
/// The first parameter specifies the target endianness, and the second parameter provides the object whose data is to be swapped.
KY_INLINE void SwapEndianness(Endianness::Target e, FloorAltitudeRange& self)
{
	SwapEndianness(e, self.m_minZ);
	SwapEndianness(e, self.m_maxZ);
}


KY_INLINE bool FloorAltitudeRange::IsAltitudeInside(KyFloat32 altitude, KyFloat32 toleranceAboveFloor, KyFloat32 toleranceBelowFloor) const
{
	const KyFloat32 operand1 = Fsel(altitude - (m_minZ - toleranceBelowFloor), 1.f, 0.f); // (operand1 == 1.f) <=> (altitude >= m_minZ - toleranceBelowFloor)
	const KyFloat32 operand2 = Fsel((m_maxZ + toleranceAboveFloor) - altitude, 1.f, 0.f); // (operand2 == 1.f) <=> (m_maxZ + toleranceAboveFloor >= altitude)

	return operand1 * operand2 > 0.f;
}

KY_INLINE bool FloorAltitudeRange::DoesIntersect(const FloorAltitudeRange& other) const
{
	return DoesIntersect(other.m_minZ, other.m_maxZ);
}

KY_INLINE bool FloorAltitudeRange::DoesIntersect(KyFloat32 zmin, KyFloat32 zmax) const
{
	const KyFloat32 operand1 = Fsel(zmax - m_minZ, 1.f, 0.f); // (operand1 == 1.f) <=> (other.m_maxZ >= m_minZ)
	const KyFloat32 operand2 = Fsel(m_maxZ - zmin, 1.f, 0.f); // (operand2 == 1.f) <=> (m_maxZ >= other.m_minZ)

	return operand1 * operand2 > 0.f;
}

KY_INLINE void FloorAltitudeRange::Update(KyFloat32 altitude)
{
	m_minZ = Kaim::Min(m_minZ, altitude);
	m_maxZ = Kaim::Max(m_maxZ, altitude);
}

}


#endif //Navigation_FloorAltitudeRange_H

