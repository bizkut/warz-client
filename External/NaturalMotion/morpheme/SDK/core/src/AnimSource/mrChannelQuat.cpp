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
#include "NMPlatform/NMMemory.h"
#include "morpheme/AnimSource/mrChannelQuat.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// ChannelQuat functions
//----------------------------------------------------------------------------------------------------------------------
void ChannelQuat::relocate(uint8_t** data)
{
  NMP_ASSERT(NMP_IS_ALIGNED(*data, NMP_VECTOR_ALIGNMENT));
  if (m_numberOfKeyFrames != 0)
  {
    m_keyFrameArray = (NMP::Quat*) (*data);
    (*data) += (sizeof(NMP::Quat) * m_numberOfKeyFrames);
    (*data) = (uint8_t*) NMP::Memory::align(*data, NMP_VECTOR_ALIGNMENT);
  }
  else
  {
    m_keyFrameArray = NULL;
  }
}

#ifndef NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
void ChannelQuat::locate(uint8_t** data)
{
  NMP_ASSERT(NMP_IS_ALIGNED(*data, NMP_VECTOR_ALIGNMENT));

  NMP::endianSwap(m_numberOfKeyFrames);
  if (m_numberOfKeyFrames != 0)
  {
    NMP::endianSwapArray((float*) *data, m_numberOfKeyFrames * 4);
    m_keyFrameArray = (NMP::Quat*) (*data);
    (*data) += (sizeof(NMP::Quat) * m_numberOfKeyFrames);
    (*data) = (uint8_t*) NMP::Memory::align(*data, NMP_VECTOR_ALIGNMENT);
  }
  else
  {
    m_keyFrameArray = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelQuat::dislocate()
{
  if (m_numberOfKeyFrames != 0)
  {
    NMP::endianSwapArray((float*) m_keyFrameArray, m_numberOfKeyFrames * 4);
    NMP::endianSwap(m_numberOfKeyFrames);
    UNFIX_SWAP_PTR(NMP::Quat, m_keyFrameArray);
  }
  else
  {
    m_keyFrameArray = NULL;
  }
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void ChannelQuat::getQuat(
  const NMP::Quat& defaultValue,
  uint32_t         frameIndex,
  float            interpolant,
  NMP::Quat&       resultQuat) const
{
  // Check for no keyframe samples
  if (m_numberOfKeyFrames == 0)
  {
    resultQuat = defaultValue;
    return;
  }
  NMP_ASSERT(frameIndex < m_numberOfKeyFrames);

  // Check if we are aligned with a keyframe boundary
  NMP_ASSERT(interpolant >= 0.0f && interpolant < 1.0f);
  if (interpolant == 0.0f)
  {
    resultQuat = m_keyFrameArray[frameIndex];
  }
  else
  {
    // Safe since frameIndex must be less than the last frame index
    NMP_ASSERT(frameIndex < m_numberOfKeyFrames - 1);
    float fromDotTo = m_keyFrameArray[frameIndex].dot(m_keyFrameArray[frameIndex+1]);
    resultQuat.fastSlerp(m_keyFrameArray[frameIndex], m_keyFrameArray[frameIndex+1], interpolant, fromDotTo);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelQuat::getKeyFrameQuat(const NMP::Quat& defaultValue, uint32_t frameIndex, NMP::Quat& resultQuat) const
{
  // Check for no keyframe samples
  if (m_numberOfKeyFrames == 0)
  {
    resultQuat = defaultValue;
    return;
  }

  NMP_ASSERT(frameIndex < m_numberOfKeyFrames);
  resultQuat = m_keyFrameArray[frameIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Quat& ChannelQuat::operator[] (uint32_t i)
{
  NMP_ASSERT(i < m_numberOfKeyFrames);
  return m_keyFrameArray[i];
}

//----------------------------------------------------------------------------------------------------------------------
const NMP::Quat& ChannelQuat::operator[] (uint32_t i) const
{
  NMP_ASSERT(i < m_numberOfKeyFrames);
  return m_keyFrameArray[i];
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
