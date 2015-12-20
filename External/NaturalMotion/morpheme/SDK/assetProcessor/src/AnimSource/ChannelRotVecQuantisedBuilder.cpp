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
#include "morpheme/mrDefines.h"
#include "assetProcessor/AnimSource/ChannelRotVecQuantisedBuilder.h"
//----------------------------------------------------------------------------------------------------------------------

// Disable warnings constant conditionals
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning(disable : 4127)
#endif

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// ChannelRotVecQuantisedBuilder
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ChannelRotVecQuantisedBuilder::channelRotVecGetMemoryRequirements(uint32_t numKeyFrameSamples)
{
  NMP::Memory::Format result(0, NMP_NATURAL_TYPE_ALIGNMENT);
  result.size += (numKeyFrameSamples * sizeof(MR::chanRotVecQuantised));
  result.size = NMP::Memory::align(result.size, NMP_NATURAL_TYPE_ALIGNMENT);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelRotVecQuantisedBuilder::computeRotVecQuantisedInfo(
  uint32_t                     numKeyFrames,
  const NMP::Quat*             sourceQuatKeys,
  MR::chanRotVecQuantisedInfo& info)
{
  NMP::Vector3 v, minV, maxV;
  NMP_VERIFY(sourceQuatKeys);
  NMP_VERIFY(numKeyFrames > 0);

  //-------------------
  if (numKeyFrames == 1)
  {
    info.m_chanRotVecScales[0] = info.m_chanRotVecScales[1] = info.m_chanRotVecScales[2] = info.m_chanRotVecScales[3] = 0.0f;

    info.m_chanRotVecOffsets[0] = sourceQuatKeys[0].x;
    info.m_chanRotVecOffsets[1] = sourceQuatKeys[0].y;
    info.m_chanRotVecOffsets[2] = sourceQuatKeys[0].z;
    info.m_chanRotVecOffsets[3] = sourceQuatKeys[0].w;
    return;
  }

  // Compute the minimum and maximum of the rotation vector components (independently)
  v = sourceQuatKeys[0].toRotationVector(true);
  minV = v;
  maxV = v;

  for (uint32_t i = 1; i < numKeyFrames; ++i)
  {
    // Convert the quaternion into a tan quarter angle rotation vector
    v = sourceQuatKeys[i].toRotationVector(true);

    // Update the bound information
    minV.x = NMP::minimum(minV.x, v.x);
    minV.y = NMP::minimum(minV.y, v.y);
    minV.z = NMP::minimum(minV.z, v.z);
    maxV.x = NMP::maximum(maxV.x, v.x);
    maxV.y = NMP::maximum(maxV.y, v.y);
    maxV.z = NMP::maximum(maxV.z, v.z);
  }

  //-------------------
  // Compute the quantisation scaling factor
  MR::chanRotVecQuantised chanRotVec;
  chanRotVec.m_data[0] = 0xFFFF;
  chanRotVec.m_data[1] = 0xFFFF;
  chanRotVec.m_data[2] = 0xFFFF;
  NMP::Vector3 diffV = maxV - minV;

  // X Component
  if (diffV.x > MR::ERROR_LIMIT)
  {
    info.m_chanRotVecScales[0] = diffV.x / chanRotVec.m_data[0]; // 16 bits
  }
  else
  {
    info.m_chanRotVecScales[0] = 0.0f; // Essentially static
  }

  // Y Component
  if (diffV.y > MR::ERROR_LIMIT)
  {
    info.m_chanRotVecScales[1] = diffV.y / chanRotVec.m_data[1]; // 16 bits
  }
  else
  {
    info.m_chanRotVecScales[1] = 0.0f; // Essentially static
  }

  // Z Component
  if (diffV.z > MR::ERROR_LIMIT)
  {
    info.m_chanRotVecScales[2] = diffV.z / chanRotVec.m_data[2]; // 16 bits
  }
  else
  {
    info.m_chanRotVecScales[2] = 0.0f; // Essentially static
  }

  //-------------------
  info.m_chanRotVecScales[3] = 0.0f;
  // Set the quantisation offset factor
  info.m_chanRotVecOffsets[0] = minV.x;
  info.m_chanRotVecOffsets[1] = minV.y;
  info.m_chanRotVecOffsets[2] = minV.z;
  info.m_chanRotVecOffsets[3] = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelRotVecQuantisedBuilder::initChannelRotVec(
  uint32_t                           numKeyFrames,
  const NMP::Quat*                   sourceQuatKeys,
  const MR::chanRotVecQuantisedInfo& info,
  MR::ChannelRotVecQuantised&        channelRotVec,
  uint8_t**                          data)
{
  NMP::Vector3 v, minV, maxV;
  NMP_VERIFY(data);

  ChannelRotVecQuantisedBuilder& channelRotVecBuilder = (ChannelRotVecQuantisedBuilder&)channelRotVec;

  //-------------------
  // Check for no keyframe samples
  channelRotVecBuilder.m_numberOfKeyFrames = numKeyFrames;
  if (numKeyFrames == 0)
  {
    channelRotVecBuilder.m_keyFrameArray = NULL;
    return;
  }
  channelRotVecBuilder.m_keyFrameArray = (MR::chanRotVecQuantised*) * data;
  NMP_VERIFY(channelRotVecBuilder.m_keyFrameArray);
  NMP_VERIFY(NMP_IS_ALIGNED(*data, NMP_NATURAL_TYPE_ALIGNMENT));

  //-------------------
  // Compute the inverse scaling coefficients
  float invScale[3], invOffset[3];
  for (uint32_t i = 0; i < 3; ++i)
  {
    if (info.m_chanRotVecScales[i] > 0.0f)
    {
      invScale[i] = 1.0f / info.m_chanRotVecScales[i];
      invOffset[i] = -info.m_chanRotVecOffsets[i] * invScale[i];
    }
    else
    {
      invScale[i] = 0.0f;
      invOffset[i] = 0.0f;
    }
  }

  //-------------------
  // Compute the minimum and maximum component limits for clamping
  minV.x = info.m_chanRotVecOffsets[0];
  minV.y = info.m_chanRotVecOffsets[1];
  minV.z = info.m_chanRotVecOffsets[2];
  MR::chanRotVecQuantised chanRotVec;
  chanRotVec.m_data[0] = 0xFFFF;
  chanRotVec.m_data[1] = 0xFFFF;
  chanRotVec.m_data[2] = 0xFFFF;
  maxV.x = info.m_chanRotVecScales[0] * chanRotVec.m_data[0] + info.m_chanRotVecOffsets[0]; // 16 bits
  maxV.y = info.m_chanRotVecScales[1] * chanRotVec.m_data[1] + info.m_chanRotVecOffsets[1]; // 16 bits
  maxV.z = info.m_chanRotVecScales[2] * chanRotVec.m_data[2] + info.m_chanRotVecOffsets[2]; // 16 bits

  //-------------------
  // Quantise the quaternion data samples
  for (uint32_t i = 0; i < numKeyFrames; ++i)
  {
    // Convert the quaternion into a tan quarter angle rotation vector
    v = sourceQuatKeys[i].toRotationVector(true);

    // Clamp the rotation vector components to the representable range
    v.x = NMP::clampValue(v.x, minV.x, maxV.x);
    v.y = NMP::clampValue(v.y, minV.y, maxV.y);
    v.z = NMP::clampValue(v.z, minV.z, maxV.z);

    // Quantise the value
    MR::chanRotVecQuantised& value = channelRotVecBuilder.m_keyFrameArray[i];
    value.m_data[0] = (uint16_t)(invScale[0] * v.x + invOffset[0]);
    value.m_data[1] = (uint16_t)(invScale[1] * v.y + invOffset[1]);
    value.m_data[2] = (uint16_t)(invScale[2] * v.z + invOffset[2]);
  }

  //-------------------
  // Update the memory data pointer
  *data += (numKeyFrames * sizeof(MR::chanRotVecQuantised));
  *data = (uint8_t*)NMP::Memory::align(*data, NMP_NATURAL_TYPE_ALIGNMENT);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif
//----------------------------------------------------------------------------------------------------------------------
