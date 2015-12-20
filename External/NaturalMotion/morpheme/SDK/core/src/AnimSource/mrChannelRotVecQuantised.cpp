// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMathUtils.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMMemory.h"
#include "morpheme/AnimSource/mrChannelRotVecQuantised.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// chanRotVecQuantisedInfo functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void chanRotVecQuantisedInfo::locate()
{
  NMP::endianSwapArray(m_chanRotVecScales, 4);
  NMP::endianSwapArray(m_chanRotVecOffsets, 4);
}

//----------------------------------------------------------------------------------------------------------------------
void chanRotVecQuantisedInfo::dislocate()
{
  NMP::endianSwapArray(m_chanRotVecScales, 4);
  NMP::endianSwapArray(m_chanRotVecOffsets, 4);
}

//----------------------------------------------------------------------------------------------------------------------
// ChannelRotVecQuantised functions.
//----------------------------------------------------------------------------------------------------------------------
void ChannelRotVecQuantised::locate(uint8_t** data)
{
  NMP_ASSERT(NMP_IS_ALIGNED(*data, NMP_NATURAL_TYPE_ALIGNMENT));

  NMP::endianSwap(m_numberOfKeyFrames);
  if (m_numberOfKeyFrames > 0)
  {
    NMP::endianSwapArray((uint16_t*) *data, m_numberOfKeyFrames * 3);
    m_keyFrameArray = (chanRotVecQuantised*) (*data);
    (*data) += (sizeof(chanRotVecQuantised) * m_numberOfKeyFrames);
    (*data) = (uint8_t*) NMP::Memory::align(*data, NMP_NATURAL_TYPE_ALIGNMENT);
  }
  else
  {
    m_keyFrameArray = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelRotVecQuantised::dislocate()
{
  if (m_numberOfKeyFrames != 0)
  {
    NMP::endianSwapArray((uint16_t*) m_keyFrameArray, m_numberOfKeyFrames * 3);
  }
  NMP::endianSwap(m_numberOfKeyFrames);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void ChannelRotVecQuantised::relocate(uint8_t** data)
{
  NMP_ASSERT(NMP_IS_ALIGNED(*data, NMP_NATURAL_TYPE_ALIGNMENT));

  if (m_numberOfKeyFrames > 0)
  {
    m_keyFrameArray = (chanRotVecQuantised*) (*data);
    (*data) += (sizeof(chanRotVecQuantised) * m_numberOfKeyFrames);
    (*data) = (uint8_t*) NMP::Memory::align(*data, NMP_NATURAL_TYPE_ALIGNMENT);
  }
  else
  {
    m_keyFrameArray = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelRotVecQuantised::getQuat(
  const chanRotVecQuantisedInfo& info,
  uint32_t                       frameIndex,
  float                          interpolant,
  NMP::Quat&                     resultQuat) const
{
  NMP::Quat unpackedQuat0, unpackedQuat1;

  // Check for no keyframe samples
  if (m_numberOfKeyFrames == 0)
  {
    keyFrameQuatUnPack(info, resultQuat);
    return;
  }
  NMP_ASSERT(frameIndex < m_numberOfKeyFrames);

  // Check if we are aligned with a keyframe boundary
  NMP_ASSERT(interpolant >= 0.0f && interpolant < 1.0f);
  if (interpolant == 0.0f)
  {
    keyFrameQuatUnPack(info, m_keyFrameArray[frameIndex], resultQuat);
  }
  else
  {
    // Safe since frameIndex must be less than the last frame index
    NMP_ASSERT(frameIndex < m_numberOfKeyFrames - 1);
    keyFrameQuatUnPack(info, m_keyFrameArray[frameIndex], unpackedQuat0);
    keyFrameQuatUnPack(info, m_keyFrameArray[frameIndex+1], unpackedQuat1);

    // Interpolation math
    float fromDotTo = unpackedQuat0.dot(unpackedQuat1);
    if (fromDotTo < 0.0f)
    {
      unpackedQuat1.negate();
      fromDotTo = -fromDotTo;
    }
    resultQuat.fastSlerp(unpackedQuat0, unpackedQuat1, interpolant, fromDotTo);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelRotVecQuantised::getKeyFrameQuat(
  const chanRotVecQuantisedInfo& info,
  uint32_t                       frameIndex,
  NMP::Quat&                     resultQuat) const
{
  // Check for no keyframe samples
  if (m_numberOfKeyFrames == 0)
  {
    keyFrameQuatUnPack(info, resultQuat);
    return;
  }

  NMP_ASSERT(frameIndex < m_numberOfKeyFrames);
  keyFrameQuatUnPack(info, m_keyFrameArray[frameIndex], resultQuat);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void ChannelRotVecQuantised::keyFrameQuatUnPack(
  const chanRotVecQuantisedInfo& info,
  const chanRotVecQuantised&     packedQuat,
  NMP::Quat&                     resultQuat) const
{
  NMP::Vector3 v;
  v.x = ((float)packedQuat.m_data[0] * info.m_chanRotVecScales[0]) + info.m_chanRotVecOffsets[0];
  v.y = ((float)packedQuat.m_data[1] * info.m_chanRotVecScales[1]) + info.m_chanRotVecOffsets[1];
  v.z = ((float)packedQuat.m_data[2] * info.m_chanRotVecScales[2]) + info.m_chanRotVecOffsets[2];

  // Convert tan quarter angle rotation vector into a quaternion
  float mag2 = v.x * v.x + v.y * v.y + v.z * v.z;
  float opm2 = 1 + mag2;
  float fac = 2 / opm2;
  resultQuat.w = (1 - mag2) / opm2;
  resultQuat.x = v.x * fac;
  resultQuat.y = v.y * fac;
  resultQuat.z = v.z * fac;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void ChannelRotVecQuantised::keyFrameQuatUnPack(
  const chanRotVecQuantisedInfo& info,
  NMP::Quat&                     resultQuat) const
{
  // not packed
  resultQuat.x = info.m_chanRotVecOffsets[0];
  resultQuat.y = info.m_chanRotVecOffsets[1];
  resultQuat.z = info.m_chanRotVecOffsets[2];
  resultQuat.w = info.m_chanRotVecOffsets[3];
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
