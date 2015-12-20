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
#include "morpheme/AnimSource/mrChannelPos.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// ChannelPos functions
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void ChannelPos::locate(uint8_t** data)
{
  NMP_ASSERT(NMP_IS_ALIGNED(*data, NMP_VECTOR_ALIGNMENT));

  NMP::endianSwap(m_numberOfKeyFrames);
  if (m_numberOfKeyFrames != 0)
  {
    NMP::endianSwapArray((float*)*data, m_numberOfKeyFrames * 4);
    m_keyFrameArray = (NMP::Vector3*) (*data);
    (*data) += (sizeof(NMP::Vector3) * m_numberOfKeyFrames);
    (*data) = (uint8_t*) NMP::Memory::align(*data, NMP_VECTOR_ALIGNMENT);
  }
  else
  {
    m_keyFrameArray = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelPos::dislocate()
{
  if (m_numberOfKeyFrames != 0)
  {
    NMP::endianSwapArray((float*) m_keyFrameArray, m_numberOfKeyFrames * 4);
    NMP::endianSwap(m_numberOfKeyFrames);
    UNFIX_SWAP_PTR(NMP::Vector3, m_keyFrameArray);
  }
  else
  {
    m_keyFrameArray = NULL;
  }
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void ChannelPos::relocate(uint8_t** data)
{
  NMP_ASSERT(NMP_IS_ALIGNED(*data, NMP_VECTOR_ALIGNMENT));
  if (m_numberOfKeyFrames != 0)
  {
    m_keyFrameArray = (NMP::Vector3*) (*data);
    (*data) += (sizeof(NMP::Vector3) * m_numberOfKeyFrames);
    (*data) = (uint8_t*) NMP::Memory::align(*data, NMP_VECTOR_ALIGNMENT);
  }
  else
  {
    m_keyFrameArray = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelPos::getPos(
  const NMP::Vector3& defaultValue,
  uint32_t            frameIndex,
  float               interpolant,
  NMP::Vector3&       resultPos) const
{
  // Check for no keyframe samples
  if (m_numberOfKeyFrames == 0)
  {
    resultPos = defaultValue;
    return;
  }
  NMP_ASSERT(frameIndex < m_numberOfKeyFrames);

  // Check if we are aligned with a keyframe boundary
  NMP_ASSERT(interpolant >= 0.0f && interpolant < 1.0f);
  if (interpolant == 0.0f)
  {
    resultPos = m_keyFrameArray[frameIndex];
  }
  else
  {
    // Safe since frameIndex must be less than the last frame index
    NMP_ASSERT(frameIndex < m_numberOfKeyFrames - 1);
    resultPos.lerp(m_keyFrameArray[frameIndex], m_keyFrameArray[frameIndex+1], interpolant);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelPos::getKeyFramePos(
  const NMP::Vector3& defaultValue,
  uint32_t            frameIndex,
  NMP::Vector3&       resultPos) const
{
  // Check for no keyframe samples
  if (m_numberOfKeyFrames == 0)
  {
    resultPos = defaultValue;
    return;
  }

  NMP_ASSERT(frameIndex < m_numberOfKeyFrames);
  resultPos = m_keyFrameArray[frameIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3& ChannelPos::operator[] (uint32_t i)
{
  NMP_ASSERT(i < m_numberOfKeyFrames);
  return m_keyFrameArray[i];
}

//----------------------------------------------------------------------------------------------------------------------
const NMP::Vector3& ChannelPos::operator[] (uint32_t i) const
{
  NMP_ASSERT(i < m_numberOfKeyFrames);
  return m_keyFrameArray[i];
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
