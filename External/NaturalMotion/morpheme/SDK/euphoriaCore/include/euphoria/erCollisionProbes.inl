// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
// inline included by CollisionProbes.h
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
//----------------------------------------------------------------------------------------------------------------------

namespace ER
{

NM_INLINE NMP::Vector3 SweepResult::getContactPoint() const
{
  return m_contactPoint;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE int32_t SweepResult::getType() const
{
  return m_type;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE int64_t SweepResult::getShapeID() const
{
  return m_shapeID;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE int32_t SweepResult::getProbeID() const
{
  return m_probeID;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Vector3 SweepResult::getTriangleNormal() const
{
  NMP_ASSERT(m_type == SR_Triangle);
  return m_vectorData[0];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Vector3 SweepResult::getVertex(uint32_t n) const
{
  NMP_ASSERT(m_type == SR_Triangle);
  NMP_ASSERT(n < 3); // a triangle only has 3 vertices: 0, 1, 2
  return m_vectorData[1+n];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Vector3 SweepResult::getNeighbourNormal(uint32_t n) const
{
  NMP_ASSERT(m_type == SR_Triangle);
  NMP_ASSERT(n < 3); // a triangle only has 3 neighbours: 0, 1, 2
  return m_vectorData[4+n];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Matrix34 SweepResult::getBoxMatrix() const
{
  NMP_ASSERT(m_type == SR_Box);
  return NMP::Matrix34(m_vectorData[0], m_vectorData[1], m_vectorData[2], m_vectorData[3]);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Vector3 SweepResult::getBoxExtents() const
{
  NMP_ASSERT(m_type == SR_Box);
  return m_vectorData[4];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Vector3 SweepResult::getSpherePosition() const
{
  NMP_ASSERT(m_type == SR_Sphere);
  return m_vectorData[0];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float SweepResult::getSphereRadius() const
{
  NMP_ASSERT(m_type == SR_Sphere);
  return m_floatData;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Vector3 SweepResult::getCapsuleStart() const
{
  NMP_ASSERT(m_type == SR_Capsule);
  return m_vectorData[0];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Vector3 SweepResult::getCapsuleEnd() const
{
  NMP_ASSERT(m_type == SR_Capsule);
  return m_vectorData[1];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float SweepResult::getCapsuleRadius() const
{
  NMP_ASSERT(m_type == SR_Capsule);
  return m_floatData;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float SweepResult::getCapsuleLength() const
{
  NMP_ASSERT(m_type == SR_Capsule);
  return m_vectorData[2].x;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Vector3 SweepResult::getPlanePosition() const
{
  NMP_ASSERT(m_type == SR_Plane || m_type == SR_ContactPlane);
  return m_vectorData[0];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Vector3 SweepResult::getPlaneNormal() const
{
  NMP_ASSERT(m_type == SR_Plane || m_type == SR_ContactPlane);
  return m_vectorData[1];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Vector3 SweepResult::getCCCorner() const
{
  return m_vectorData[0];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Vector3 SweepResult::getCCFaceNormal(uint32_t i) const
{
  NMP_ASSERT(i < 3);
  return m_vectorData[i+1];
}
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float SweepResult::getCCEdgeLength(uint32_t i) const
{
  NMP_ASSERT(i < 3);
  return m_vectorData[4][i];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SweepResult::setShapeID(int64_t shapeID)
{
  m_shapeID = shapeID;
}

} // namespace ER

//----------------------------------------------------------------------------------------------------------------------
