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
#include "assetProcessor/AnimSource/ChannelSetTableBuilder.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// ChannelSetTable
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ChannelSetTable::getMemoryRequirements(
  uint32_t numChannelSets,
  uint32_t numKeyFrames)
{
  // Header
  NMP::Memory::Format memReqs(sizeof(ChannelSetTable), NMP_NATURAL_TYPE_ALIGNMENT);

  // Pointers array
  NMP::Memory::Format memReqsQuatPtrs(sizeof(NMP::Quat*) * numChannelSets, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqs += memReqsQuatPtrs;
  NMP::Memory::Format memReqsPosPtrs(sizeof(NMP::Vector3*) * numChannelSets, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqs += memReqsPosPtrs;

  // Quat channels
  uint32_t numChannelSetEntries = numKeyFrames * numChannelSets;
  NMP::Memory::Format memReqsChanQuat(sizeof(NMP::Quat) * numChannelSetEntries, NMP_VECTOR_ALIGNMENT);
  memReqs += memReqsChanQuat;

  // Pos channels
  NMP::Memory::Format memReqsChanPos(sizeof(NMP::Vector3) * numChannelSetEntries, NMP_VECTOR_ALIGNMENT);
  memReqs += memReqsChanPos;

  return memReqs;
}

//----------------------------------------------------------------------------------------------------------------------
ChannelSetTable* ChannelSetTable::init(
  NMP::Memory::Resource& resource,
  uint32_t              numChannelSets,
  uint32_t              numKeyFrames)
{
  // Header
  NMP::Memory::Format memReqs(sizeof(ChannelSetTable), NMP_NATURAL_TYPE_ALIGNMENT);
  ChannelSetTable* result = (ChannelSetTable*) resource.alignAndIncrement(memReqs);
  NMP_VERIFY(result);

  result->m_numChannelSets = numChannelSets;
  result->m_numKeyFrames = numKeyFrames;

  // Pointers array
  NMP::Memory::Format memReqsQuatPtrs(sizeof(NMP::Quat*) * numChannelSets, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_channelQuat = (NMP::Quat**) resource.alignAndIncrement(memReqsQuatPtrs);
  NMP::Memory::Format memReqsPosPtrs(sizeof(NMP::Vector3*) * numChannelSets, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_channelPos = (NMP::Vector3**) resource.alignAndIncrement(memReqsPosPtrs);

  // Quat channels
  NMP::Memory::Format memReqsChanQuatStride(sizeof(NMP::Quat) * numKeyFrames, NMP_VECTOR_ALIGNMENT);
  for (uint32_t i = 0; i < numChannelSets; ++i)
  {
    result->m_channelQuat[i] = (NMP::Quat*) resource.alignAndIncrement(memReqsChanQuatStride);
  }

  // Pos channels
  NMP::Memory::Format memReqsChanPosStride(sizeof(NMP::Vector3) * numKeyFrames, NMP_VECTOR_ALIGNMENT);
  for (uint32_t i = 0; i < numChannelSets; ++i)
  {
    result->m_channelPos[i] = (NMP::Vector3*) resource.alignAndIncrement(memReqsChanPosStride);

    // Initialise all elements to 0.0f. If this isn't done the w component is often left uninitialised and assets
    // that use channel set tables may not be binary invariant.
    for (uint32_t j = 0; j < numKeyFrames; ++j)
    {
      result->m_channelPos[i][j].setToZero();
    }
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::setChannelQuat(uint32_t channel, const NMP::Quat* quatKeys)
{
  NMP_VERIFY(channel < m_numChannelSets);
  NMP_VERIFY(quatKeys);
  NMP::Quat* dst = m_channelQuat[channel];
  NMP_VERIFY(dst);

  for (uint32_t i = 0; i < m_numKeyFrames; ++i)
  {
    dst[i] = quatKeys[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::setChannelQuatKeys(uint32_t channel, const NMP::Quat& quatKey)
{
  NMP_VERIFY(channel < m_numChannelSets);
  NMP::Quat* dst = m_channelQuat[channel];
  NMP_VERIFY(dst);

  for (uint32_t i = 0; i < m_numKeyFrames; ++i)
  {
    dst[i] = quatKey;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::setChannelPos(uint32_t channel, const NMP::Vector3* posKeys)
{
  NMP_VERIFY(channel < m_numChannelSets);
  NMP_VERIFY(posKeys);
  NMP::Vector3* dst = m_channelPos[channel];
  NMP_VERIFY(dst);

  for (uint32_t i = 0; i < m_numKeyFrames; ++i)
  {
    dst[i] = posKeys[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::setChannelPosKeys(uint32_t channel, const NMP::Vector3& posKey)
{
  NMP_VERIFY(channel < m_numChannelSets);
  NMP::Vector3* dst = m_channelPos[channel];
  NMP_VERIFY(dst);

  for (uint32_t i = 0; i < m_numKeyFrames; ++i)
  {
    dst[i] = posKey;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::getQuatKeysAtFrame(uint32_t frame, NMP::Quat* quatKeys) const
{
  NMP_VERIFY(quatKeys);
  NMP_VERIFY(frame < m_numKeyFrames);

  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    quatKeys[i] = m_channelQuat[i][frame];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::setQuatKeysAtFrame(uint32_t frame, const NMP::Quat* quatKeys)
{
  NMP_VERIFY(quatKeys);
  NMP_VERIFY(frame < m_numKeyFrames);

  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    m_channelQuat[i][frame] = quatKeys[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::getPosKeysAtFrame(uint32_t frame, NMP::Vector3* posKeys) const
{
  NMP_VERIFY(posKeys);
  NMP_VERIFY(frame < m_numKeyFrames);

  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    posKeys[i] = m_channelPos[i][frame];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::setPosKeysAtFrame(uint32_t frame, const NMP::Vector3* posKeys)
{
  NMP_VERIFY(posKeys);
  NMP_VERIFY(frame < m_numKeyFrames);

  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    m_channelPos[i][frame] = posKeys[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::getQuatKey(uint32_t channel, uint32_t frame, NMP::Quat& quatKey) const
{
  NMP_VERIFY(channel < m_numChannelSets);
  NMP_VERIFY(frame < m_numKeyFrames);
  quatKey = m_channelQuat[channel][frame];
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::setQuatKey(uint32_t channel, uint32_t frame, const NMP::Quat& quatKey)
{
  NMP_VERIFY(channel < m_numChannelSets);
  NMP_VERIFY(frame < m_numKeyFrames);
  m_channelQuat[channel][frame] = quatKey;
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::getPosKey(uint32_t channel, uint32_t frame, NMP::Vector3& posKey) const
{
  NMP_VERIFY(channel < m_numChannelSets);
  NMP_VERIFY(frame < m_numKeyFrames);
  posKey = m_channelPos[channel][frame];
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::setPosKey(uint32_t channel, uint32_t frame, const NMP::Vector3& posKey)
{
  NMP_VERIFY(channel < m_numChannelSets);
  NMP_VERIFY(frame < m_numKeyFrames);
  m_channelPos[channel][frame] = posKey;
}

//----------------------------------------------------------------------------------------------------------------------
bool ChannelSetTable::isChannelQuatUnchanging(uint32_t channel, float quatEps) const
{
  NMP_VERIFY(channel < m_numChannelSets);
  NMP_VERIFY(quatEps >= 0.0f);
  if (m_numKeyFrames <= 1)
    return true;

  NMP::Quat* quatKeys = m_channelQuat[channel];
  NMP_VERIFY(quatKeys);
  NMP::Quat dq;
  for (uint32_t i = 1; i < m_numKeyFrames; ++i)
  {
    dq = quatKeys[i] - quatKeys[0];
    dq.x = fabs(dq.x);
    dq.y = fabs(dq.y);
    dq.z = fabs(dq.z);
    dq.w = fabs(dq.w);
    if (dq.x > quatEps || dq.y > quatEps || dq.z > quatEps || dq.w > quatEps)
    {
      return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ChannelSetTable::isChannelPosUnchanging(uint32_t channel, float posEps) const
{
  NMP_VERIFY(channel < m_numChannelSets);
  NMP_VERIFY(posEps >= 0.0f);
  if (m_numKeyFrames <= 1)
    return true;

  const NMP::Vector3* posKeys = m_channelPos[channel];
  NMP_VERIFY(posKeys);
  NMP::Vector3 dp;
  for (uint32_t i = 1; i < m_numKeyFrames; ++i)
  {
    dp = posKeys[i] - posKeys[0];
    dp.x = fabs(dp.x);
    dp.y = fabs(dp.y);
    dp.z = fabs(dp.z);
    if (dp.x > posEps || dp.y > posEps || dp.z > posEps)
    {
      return false;
    }
  }
  
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::copy(const ChannelSetTable& src)
{
  NMP_VERIFY(m_numChannelSets == src.m_numChannelSets);
  NMP_VERIFY(m_numKeyFrames == src.m_numKeyFrames);

  if (&src != this)
  {
    for (uint32_t i = 0; i < m_numChannelSets; ++i)
    {
      const NMP::Quat* srcQuatKeys = src.m_channelQuat[i];
      const NMP::Vector3* srcPosKeys = src.m_channelPos[i];
      NMP::Quat* dstQuatKeys = m_channelQuat[i];
      NMP::Vector3* dstPosKeys = m_channelPos[i];

      for (uint32_t k = 0; k < m_numKeyFrames; ++k)
      {
        dstQuatKeys[k] = srcQuatKeys[k];
        dstPosKeys[k] = srcPosKeys[k];
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
ChannelSetTable& ChannelSetTable::operator=(const ChannelSetTable& src)
{
  copy(src);
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::resampleChannelSets(const ChannelSetTable& src)
{
  NMP_VERIFY(m_numChannelSets == src.m_numChannelSets);

  if (m_numKeyFrames != src.m_numKeyFrames)
  {
    NMP_VERIFY(m_numKeyFrames >= 2);
    NMP_VERIFY(src.m_numKeyFrames >= 2);
    
    // Compute the interpolation factor so that the first and last frames match
    uint32_t resampledFrameEnd = m_numKeyFrames - 1;
    uint32_t srcFrameEnd = src.m_numKeyFrames - 1;
    float resampleFac = (float)srcFrameEnd / (float)resampledFrameEnd;
  
    // Resample the channel set transforms
    for (uint32_t i = 0; i < resampledFrameEnd; ++i) // All but last frame
    {
      // Compute the bilinear interpolation coefficients
      float animFrameValue = i * resampleFac; // in source
      uint32_t animFrameIndex = (uint32_t)animFrameValue;
      float interpolant = animFrameValue - (float)animFrameIndex;

      for (uint32_t k = 0; k < m_numChannelSets; ++k)
      {
        NMP::Quat* resampledQuatKeys = m_channelQuat[k];
        NMP::Vector3* resampledPosKeys = m_channelPos[k];
        const NMP::Quat* animQuatKeys = src.m_channelQuat[k];
        const NMP::Vector3* animPosKeys = src.m_channelPos[k];

        // Interpolation math
        float fromDotTo = animQuatKeys[animFrameIndex].dot(animQuatKeys[animFrameIndex+1]);
        resampledQuatKeys[i].fastSlerp(animQuatKeys[animFrameIndex], animQuatKeys[animFrameIndex+1], interpolant, fromDotTo);
        resampledPosKeys[i].lerp(animPosKeys[animFrameIndex], animPosKeys[animFrameIndex+1], interpolant);

        // Important: Ensure quaternion is properly normalised (Do Not Remove Me)
        resampledQuatKeys[i].normalise();
      }
    }

    // Last frame interpolates the end pose exactly. Note that we enforce this explicitly since in the
    // above loop the last frame may not have an exactly zero interpolant due to numerical error
    for (uint32_t k = 0; k < m_numChannelSets; ++k)
    {
      NMP::Quat* resampledQuatKeys = m_channelQuat[k];
      NMP::Vector3* resampledPosKeys = m_channelPos[k];
      const NMP::Quat* animQuatKeys = src.m_channelQuat[k];
      const NMP::Vector3* animPosKeys = src.m_channelPos[k];

      // Data sample
      resampledQuatKeys[resampledFrameEnd] = animQuatKeys[srcFrameEnd];
      resampledPosKeys[resampledFrameEnd] = animPosKeys[srcFrameEnd];

      // Important: Ensure quaternion is properly normalised (Do Not Remove Me)
      resampledQuatKeys[resampledFrameEnd].normalise();
    }
  }
  else
  {
    copy(src);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::conditionChannelQuat(uint32_t channel)
{
  NMP_VERIFY(m_numKeyFrames > 0);
  NMP::Quat* dstKeys = getChannelQuat(channel);

  // Start off in the positive hemisphere
  dstKeys[0].normalise();
  if (dstKeys[0].w < 0.0f)
    dstKeys[0].negate();

  // Condition the quats
  for (uint32_t i = 1; i < m_numKeyFrames; ++i)
  {
    dstKeys[i].normalise();
    if (dstKeys[i].dot(dstKeys[i-1]) < 0.0f)
      dstKeys[i].negate();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::normaliseChannelQuat(uint32_t channel)
{
  NMP_VERIFY(m_numKeyFrames > 0);
  NMP::Quat* dstKeys = getChannelQuat(channel);

  // normalise the quats
  for (uint32_t i = 0; i < m_numKeyFrames; ++i)
    dstKeys[i].normalise();
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::conditionChannelQuat()
{
  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    conditionChannelQuat(i);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::normaliseChannelQuat()
{
  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    normaliseChannelQuat(i);
  }
}

//----------------------------------------------------------------------------------------------------------------------
float ChannelSetTable::maxAngleChannelQuat(uint32_t channel)
{
  NMP_VERIFY(m_numKeyFrames > 0);
  NMP::Quat* dstKeys = getChannelQuat(channel);
  NMP::Quat qi;
  float angle, maxAngle;

  maxAngle = 0.0f;
  for (uint32_t i = 0; i < m_numKeyFrames; ++i)
  {
    qi = dstKeys[i];
    qi.normalise();
    angle = qi.angle();
    maxAngle = NMP::maximum(maxAngle, angle);
  }

  return maxAngle;
}

//----------------------------------------------------------------------------------------------------------------------
float ChannelSetTable::maxAngleChannelQuat()
{
  float angle, maxAngle;

  maxAngle = 0.0f;
  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    angle = maxAngleChannelQuat(i);
    maxAngle = NMP::maximum(maxAngle, angle);
  }

  return maxAngle;
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::applyToChannelQuat(uint32_t channel, const NMP::Quat* quatKeys)
{
  NMP::Quat* dstKeys = getChannelQuat(channel);
  for (uint32_t i = 0; i < m_numKeyFrames; ++i)
  {
    dstKeys[i] = quatKeys[i] * dstKeys[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::inverseChannelQuat(uint32_t channel)
{
  NMP::Quat* quatKeys = getChannelQuat(channel);
  for (uint32_t i = 0; i < m_numKeyFrames; ++i)
  {
    quatKeys[i].conjugate();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::applyToChannelPos(uint32_t channel, const NMP::Vector3* posKeys)
{
  NMP::Vector3* dstKeys = getChannelPos(channel);
  for (uint32_t i = 0; i < m_numKeyFrames; ++i)
  {
    dstKeys[i] += posKeys[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::inverseChannelPos(uint32_t channel)
{
  NMP::Vector3* dstKeys = getChannelPos(channel);
  for (uint32_t i = 0; i < m_numKeyFrames; ++i)
  {
    dstKeys[i] = -dstKeys[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::applyToChannelQuat(const ChannelSetTable& src)
{
  NMP_VERIFY(src.m_numChannelSets == m_numChannelSets);
  NMP_VERIFY(src.m_numKeyFrames == m_numKeyFrames);

  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    applyToChannelQuat(i, src.getChannelQuat(i));
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::inverseChannelQuat()
{
  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    inverseChannelQuat(i);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::applyToChannelPos(const ChannelSetTable& src)
{
  NMP_VERIFY(src.m_numChannelSets == m_numChannelSets);
  NMP_VERIFY(src.m_numKeyFrames == m_numKeyFrames);

  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    applyToChannelPos(i, src.getChannelPos(i));
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::inverseChannelPos()
{
  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    inverseChannelPos(i);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::computeDeltaTransforms(const ChannelSetTable& from, const ChannelSetTable& to)
{
  NMP_VERIFY(from.m_numChannelSets == m_numChannelSets);
  NMP_VERIFY(to.m_numChannelSets == m_numChannelSets);
  NMP_VERIFY(from.m_numKeyFrames == m_numKeyFrames);
  NMP_VERIFY(to.m_numKeyFrames == m_numKeyFrames);

  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    NMP::Vector3* dstPosKeys = getChannelPos(i);
    const NMP::Vector3* fromPosKeys = from.getChannelPos(i);
    const NMP::Vector3* toPosKeys = to.getChannelPos(i);
    NMP::Quat* dstQuatKeys = getChannelQuat(i);
    const NMP::Quat* fromQuatKeys = from.getChannelQuat(i);
    const NMP::Quat* toQuatKeys = to.getChannelQuat(i);

    for (uint32_t k = 0; k < m_numKeyFrames; ++k)
    {
      dstPosKeys[k] = (toPosKeys[k] - fromPosKeys[k]);
      dstQuatKeys[k] = (toQuatKeys[k] * ~fromQuatKeys[k]);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::scaleChannelPos(uint32_t channel, float scale)
{
  NMP::Vector3* posKeys = getChannelPos(channel);
  if (scale == 1.0f)
    return;
  for (uint32_t i = 0; i < m_numKeyFrames; ++i)
  {
    posKeys[i] *= scale;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::scaleChannelPos(float scale)
{
  if (scale == 1.0f)
    return;
    
  for (uint32_t k = 0; k < m_numChannelSets; ++k)
  {
    NMP::Vector3* posKeys = getChannelPos(k);
    for (uint32_t i = 0; i < m_numKeyFrames; ++i)
    {
      posKeys[i] *= scale;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::linearTransformChannelPos(uint32_t channel, const NMP::Vector3& a, const NMP::Vector3& b)
{
  NMP::Vector3* dstKeys = getChannelPos(channel);
  for (uint32_t i = 0; i < m_numKeyFrames; ++i)
  {
    dstKeys[i].multiplyElements(a);
    dstKeys[i] += b;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::linearTransformChannelPos(const NMP::Vector3& a, const NMP::Vector3& b)
{
  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    linearTransformChannelPos(i, a, b);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::writeChannelSet(uint32_t channelIndex, FILE* file) const
{
  NMP_VERIFY(file);
  const NMP::Quat* chanQuat = getChannelQuat(channelIndex);
  const NMP::Vector3* chanPos = getChannelPos(channelIndex);

  fprintf(file, "---------------------------------------------------------------------\n");

  for (uint32_t i = 0; i < m_numKeyFrames; ++i)
  {
    fprintf(
      file,
      "frame = %4d, quat = [%3.5f, %3.5f, %3.5f, %3.5f], pos = [%3.5f, %3.5f, %3.5f]\n",
      i,
      chanQuat[i].x, chanQuat[i].y, chanQuat[i].z, chanQuat[i].w,
      chanPos[i].x, chanPos[i].y, chanPos[i].z);
  }

  fprintf(file, "---------------------------------------------------------------------\n\n");
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::writeFrame(uint32_t frame, FILE* file) const
{
  NMP_VERIFY(file);
  NMP::Quat quatKey;
  NMP::Vector3 posKey;

  fprintf(file, "---------------------------------------------------------------------\n");

  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    getQuatKey(i, frame, quatKey);
    getPosKey(i, frame, posKey);

    fprintf(
      file,
      "chanSet = %4d, quat = [%3.5f, %3.5f, %3.5f, %3.5f], pos = [%3.5f, %3.5f, %3.5f]\n",
      i,
      quatKey.x, quatKey.y, quatKey.z, quatKey.w,
      posKey.x, posKey.y, posKey.z);
  }

  fprintf(file, "---------------------------------------------------------------------\n\n");
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::writeTableByChannelSets(FILE* file) const
{
  NMP_VERIFY(file);
  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    fprintf(file, "---------------------------------------------------------------------\n");
    fprintf(file, "Channel = %4d\n", i);

    writeChannelSet(i, file);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::writeTableByFrames(FILE* file) const
{
  NMP_VERIFY(file);
  for (uint32_t i = 0; i < m_numKeyFrames; ++i)
  {
    fprintf(file, "---------------------------------------------------------------------\n");
    fprintf(file, "Frame = %4d\n", i);

    writeFrame(i, file);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::writeMaxPosErrors(FILE* file) const
{
  NMP_VERIFY(file);
  NMP::Vector3 posKey;
  float mag, magAll;

  fprintf(file, "\n---------------------- Max Pos Errors (at each frame) ---------------------\n");

  magAll = 0.0f;
  for (uint32_t i = 0; i < m_numKeyFrames; ++i)
  {
    mag = 0.0f;
    for (uint32_t k = 0; k < m_numChannelSets; ++k)
    {
      getPosKey(k, i, posKey);
      mag = NMP::maximum(posKey.magnitude(), mag);
    }

    fprintf(file, "%f\n", mag);

    magAll = NMP::maximum(mag, magAll);
  }

  fprintf(file, "\n---------------------- Max Pos Error (over all frames) ---------------------\n");
  fprintf(file, "%f\n", magAll);
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTable::writeMeanPosErrors(FILE* file) const
{
  NMP_VERIFY(file);
  NMP::Vector3 posKey;
  float sum, mean;
  float sumAll, meanAll;

  fprintf(file, "\n---------------------- Mean Pos Errors (at each frame) ---------------------\n");

  sumAll = 0.0f;
  meanAll = 0.0f;
  for (uint32_t i = 0; i < m_numKeyFrames; ++i)
  {
    sum = 0.0f;
    for (uint32_t k = 0; k < m_numChannelSets; ++k)
    {
      getPosKey(k, i, posKey);
      sum += posKey.magnitude();
    }

    mean = sum / m_numChannelSets;
    fprintf(file, "%f\n", mean);

    sumAll += sum;
  }

  meanAll = sumAll / (m_numKeyFrames * m_numChannelSets);

  fprintf(file, "\n---------------------- Mean Pos Error (over all frames) ---------------------\n");
  fprintf(file, "%f\n", meanAll);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
