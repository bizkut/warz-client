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
#include "morpheme/AnimSource/mrChannelPosQuantised.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// chanPosQuantisedInfo functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void chanPosQuantisedInfo::locate()
{
  NMP::endianSwapArray(m_chanPosScales, 4);
  NMP::endianSwapArray(m_chanPosOffsets, 4);
}

//----------------------------------------------------------------------------------------------------------------------
void chanPosQuantisedInfo::dislocate()
{
  NMP::endianSwapArray(m_chanPosScales, 4);
  NMP::endianSwapArray(m_chanPosOffsets, 4);
}

//----------------------------------------------------------------------------------------------------------------------
// ChannelPosQuantised functions.
//----------------------------------------------------------------------------------------------------------------------
void ChannelPosQuantised::locate(uint8_t** data)
{
  NMP_ASSERT(NMP_IS_ALIGNED(*data, NMP_NATURAL_TYPE_ALIGNMENT));

  NMP::endianSwap(m_numberOfKeyFrames);
  if (m_numberOfKeyFrames > 0)
  {
    NMP::endianSwapArray((uint32_t*)*data, m_numberOfKeyFrames);
    m_keyFrameArray = (chanPosQuantised*) (*data);
    (*data) += (sizeof(chanPosQuantised) * m_numberOfKeyFrames);
    (*data) = (uint8_t*) NMP::Memory::align(*data, NMP_NATURAL_TYPE_ALIGNMENT);
  }
  else
  {
    m_keyFrameArray = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelPosQuantised::dislocate()
{
  if (m_numberOfKeyFrames != 0)
  {
    NMP::endianSwapArray((uint32_t*) m_keyFrameArray, m_numberOfKeyFrames);
  }
  NMP::endianSwap(m_numberOfKeyFrames);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void ChannelPosQuantised::relocate(uint8_t** data)
{
  NMP_ASSERT(NMP_IS_ALIGNED(*data, NMP_NATURAL_TYPE_ALIGNMENT));
  if (m_numberOfKeyFrames > 0)
  {
    m_keyFrameArray = (chanPosQuantised*) (*data);
    (*data) += (sizeof(chanPosQuantised) * m_numberOfKeyFrames);
    (*data) = (uint8_t*) NMP::Memory::align(*data, NMP_NATURAL_TYPE_ALIGNMENT);
  }
  else
  {
    m_keyFrameArray = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelPosQuantised::getPos(
  const chanPosQuantisedInfo& info,
  uint32_t                    frameIndex,
  float                       interpolant,
  NMP::Vector3&               resultPos) const
{
  NMP::Vector3 unpackedPos0, unpackedPos1;

  // Check for no keyframe samples
  if (m_numberOfKeyFrames == 0)
  {
    resultPos.x = info.m_chanPosOffsets[0];
    resultPos.y = info.m_chanPosOffsets[1];
    resultPos.z = info.m_chanPosOffsets[2];
    return;
  }
  NMP_ASSERT(frameIndex < m_numberOfKeyFrames);

  // Check if we are aligned with a keyframe boundary
  NMP_ASSERT(interpolant >= 0.0f && interpolant < 1.0f);
  if (interpolant == 0.0f)
  {
    keyFramePosUnPack(info, m_keyFrameArray[frameIndex], resultPos);
  }
  else
  {
    // Safe since frameIndex must be less than the last frame index
    NMP_ASSERT(frameIndex < m_numberOfKeyFrames - 1);
    keyFramePosUnPack(info, m_keyFrameArray[frameIndex], unpackedPos0);
    keyFramePosUnPack(info, m_keyFrameArray[frameIndex+1], unpackedPos1);

    // Interpolation math
    resultPos.lerp(unpackedPos0, unpackedPos1, interpolant);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelPosQuantised::getKeyFramePos(
  const chanPosQuantisedInfo& info,
  uint32_t                    frameIndex,
  NMP::Vector3&               resultPos) const
{
  // Check for no keyframe samples
  if (m_numberOfKeyFrames == 0)
  {
    resultPos.x = info.m_chanPosOffsets[0];
    resultPos.y = info.m_chanPosOffsets[1];
    resultPos.z = info.m_chanPosOffsets[2];
    return;
  }
  NMP_ASSERT(frameIndex < m_numberOfKeyFrames);
  keyFramePosUnPack(info, m_keyFrameArray[frameIndex], resultPos);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void ChannelPosQuantised::keyFramePosUnPack(
  const chanPosQuantisedInfo& info,
  const chanPosQuantised&     packedPos,
  NMP::Vector3&               resultPos) const
{
  uint32_t tranX = ((packedPos.m_data >> 21) & 2047);  // 11 bits
  uint32_t tranY = ((packedPos.m_data >> 10) & 2047);  // 11 bits
  uint32_t tranZ = ((packedPos.m_data >> 0) & 1023);   // 10 bits
  resultPos.x = (tranX * info.m_chanPosScales[0]) + info.m_chanPosOffsets[0];
  resultPos.y = (tranY * info.m_chanPosScales[1]) + info.m_chanPosOffsets[1];
  resultPos.z = (tranZ * info.m_chanPosScales[2]) + info.m_chanPosOffsets[2];
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
