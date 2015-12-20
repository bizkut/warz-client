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
#include "NMPlatform/NMPlatform.h"
#include "assetProcessor/AnimSource/TransformsAccumulator.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// TransformsAccumulator
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format TransformsAccumulator::getMemoryRequirements(uint32_t numRigBones)
{
  NMP_VERIFY(numRigBones >= 1);

  // Header
  NMP::Memory::Format memReqs(sizeof(TransformsAccumulator), NMP_NATURAL_TYPE_ALIGNMENT);

  // Channel spaces
  NMP::Memory::Format memReqsSpace(sizeof(chanSpaceType) * numRigBones, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqs += memReqsSpace; // Quat
  memReqs += memReqsSpace; // Pos

  // Quat channels
  NMP::Memory::Format memReqsQuat(sizeof(NMP::Quat) * numRigBones, NMP_VECTOR_ALIGNMENT);
  memReqs += memReqsQuat;

  // Pos channels
  NMP::Memory::Format memReqsPos(sizeof(NMP::Vector3) * numRigBones, NMP_VECTOR_ALIGNMENT);
  memReqs += memReqsPos;

  return memReqs;
}

//----------------------------------------------------------------------------------------------------------------------
TransformsAccumulator* TransformsAccumulator::init(
  NMP::Memory::Resource&  resource,
  uint32_t                numRigBones)
{
  NMP_VERIFY(numRigBones >= 1);
  
  // Header
  NMP::Memory::Format memReqs(sizeof(TransformsAccumulator), NMP_NATURAL_TYPE_ALIGNMENT);
  TransformsAccumulator* result = (TransformsAccumulator*) resource.alignAndIncrement(memReqs);
  NMP_VERIFY(result);
  result->m_numChannelSets = numRigBones;

  // Channel spaces
  NMP::Memory::Format memReqsSpace(sizeof(chanSpaceType) * numRigBones, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_channelQuatSpace = (chanSpaceType*) resource.alignAndIncrement(memReqsSpace);
  result->m_channelPosSpace = (chanSpaceType*) resource.alignAndIncrement(memReqsSpace);
  
  // Quat channels
  NMP::Memory::Format memReqsQuat(sizeof(NMP::Quat) * numRigBones, NMP_VECTOR_ALIGNMENT);
  result->m_channelQuat = (NMP::Quat*) resource.alignAndIncrement(memReqsQuat);
  
  // Pos channels
  NMP::Memory::Format memReqsPos(sizeof(NMP::Vector3) * numRigBones, NMP_VECTOR_ALIGNMENT);
  result->m_channelPos = (NMP::Vector3*) resource.alignAndIncrement(memReqsPos);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void TransformsAccumulator::convertToWorldSpace(const NMP::Hierarchy& hierarchy)
{
  NMP_VERIFY(m_numChannelSets >= 1);

  // Ensure that the top root is set as worldspace transforms
  m_channelQuatSpace[0] = World;
  m_channelPosSpace[0] = World;

  // Accumulate over the rig hierarchy
  for (uint32_t rigBoneIndex = 1; rigBoneIndex < m_numChannelSets; ++rigBoneIndex)
  {
    uint32_t rigParentIndex = hierarchy.getParentIndex(rigBoneIndex);
    NMP_VERIFY(rigParentIndex < rigBoneIndex && rigParentIndex != INVALID_HIERARCHY_INDEX);

    // Quat part of the transform
    NMP_VERIFY(m_channelQuatSpace[rigParentIndex] == World);
    if (m_channelQuatSpace[rigBoneIndex] == Local)
    {
      m_channelQuat[rigBoneIndex] = m_channelQuat[rigParentIndex] * m_channelQuat[rigBoneIndex];
      m_channelQuatSpace[rigBoneIndex] = World;
    }

    // Pos part of the transform
    NMP_VERIFY(m_channelPosSpace[rigParentIndex] == World);
    if (m_channelPosSpace[rigBoneIndex] == Local)
    {
      m_channelPos[rigBoneIndex] = m_channelQuat[rigParentIndex].rotateVector(m_channelPos[rigBoneIndex]);
      m_channelPos[rigBoneIndex] += m_channelPos[rigParentIndex];
      m_channelPosSpace[rigBoneIndex] = World;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TransformsAccumulator::convertToLocalSpace(const NMP::Hierarchy& hierarchy)
{
  // Convert all transforms to worldspace first
  convertToWorldSpace(hierarchy);

  NMP::Quat invQuat;
  NMP::Vector3 dv;

  // Unaccumulate over the rig hierarchy
  for (uint32_t rigBoneIndex = m_numChannelSets - 1; rigBoneIndex > 0; --rigBoneIndex)
  {
    uint32_t rigParentIndex = hierarchy.getParentIndex(rigBoneIndex);
    NMP_VERIFY(rigParentIndex < rigBoneIndex && rigParentIndex != INVALID_HIERARCHY_INDEX);

    // Quat
    invQuat = ~m_channelQuat[rigParentIndex];
    m_channelQuat[rigBoneIndex] = invQuat * m_channelQuat[rigBoneIndex];
    m_channelQuatSpace[rigBoneIndex] = Local;

    // Pos
    dv = m_channelPos[rigBoneIndex] - m_channelPos[rigParentIndex];
    m_channelPos[rigBoneIndex] = invQuat.rotateVector(dv);
    m_channelPosSpace[rigBoneIndex] = Local;
  }
}

//----------------------------------------------------------------------------------------------------------------------
float TransformsAccumulator::worldspacePosSquaredError(const TransformsAccumulator& rhs, uint32_t channel)
{
  NMP_VERIFY(m_numChannelSets == rhs.m_numChannelSets);
  NMP_VERIFY(channel < m_numChannelSets);
  NMP_VERIFY(m_channelPosSpace[channel] == World);
  NMP_VERIFY(rhs.m_channelPosSpace[channel] == World);

  NMP::Vector3 dx = m_channelPos[channel] - rhs.m_channelPos[channel];
  return dx.magnitudeSquared();
}

//----------------------------------------------------------------------------------------------------------------------
float TransformsAccumulator::worldspacePosSquaredError(const TransformsAccumulator& rhs)
{
  NMP_VERIFY(m_numChannelSets == rhs.m_numChannelSets);
  NMP::Vector3 dx;
  float err = 0.0f;

  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    NMP_VERIFY(m_channelPosSpace[i] == World);
    NMP_VERIFY(rhs.m_channelPosSpace[i] == World);
    dx = m_channelPos[i] - rhs.m_channelPos[i];
    err += dx.magnitudeSquared();
  }

  return err;
}

//----------------------------------------------------------------------------------------------------------------------
float TransformsAccumulator::worldspacePosError(const TransformsAccumulator& rhs, uint32_t channel)
{
  NMP_VERIFY(m_numChannelSets == rhs.m_numChannelSets);
  NMP_VERIFY(channel < m_numChannelSets);
  NMP_VERIFY(m_channelPosSpace[channel] == World);
  NMP_VERIFY(rhs.m_channelPosSpace[channel] == World);

  NMP::Vector3 dx = m_channelPos[channel] - rhs.m_channelPos[channel];
  return dx.magnitude();
}

//----------------------------------------------------------------------------------------------------------------------
float TransformsAccumulator::worldspacePosError(const TransformsAccumulator& rhs)
{
  NMP_VERIFY(m_numChannelSets == rhs.m_numChannelSets);
  NMP::Vector3 dx;
  float err = 0.0f;

  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    NMP_VERIFY(m_channelPosSpace[i] == World);
    NMP_VERIFY(rhs.m_channelPosSpace[i] == World);
    dx = m_channelPos[i] - rhs.m_channelPos[i];
    err += dx.magnitude();
  }

  return err;
}

//----------------------------------------------------------------------------------------------------------------------
void TransformsAccumulator::setTransformsIdentity(chanSpaceType space)
{
  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    // Pos
    m_channelPos[i].setToZero();
    m_channelPosSpace[i] = space;

    // Quat
    m_channelQuat[i].identity();
    m_channelQuatSpace[i] = space;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TransformsAccumulator::setTransforms(const NMP::DataBuffer* transformBuffer, chanSpaceType space)
{
  NMP_VERIFY(transformBuffer);
  uint32_t numRigBones = transformBuffer->getLength();
  const NMP::Vector3* posKeys = transformBuffer->getPosQuatChannelPos(0);
  NMP_VERIFY(posKeys);
  const NMP::Quat* quatKeys = transformBuffer->getPosQuatChannelQuat(0);
  NMP_VERIFY(quatKeys);

  for (uint32_t i = 0; i < numRigBones; ++i)
  {
    // Pos
    m_channelPos[i] = posKeys[i];
    m_channelPosSpace[i] = space;

    // Quat
    m_channelQuat[i] = quatKeys[i];
    m_channelQuatSpace[i] = space;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TransformsAccumulator::diff(const TransformsAccumulator& rhs)
{
  NMP::Quat q;
  NMP_VERIFY(m_numChannelSets == rhs.m_numChannelSets);
  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    // Quat
    if (m_channelQuat[i].dot(rhs.m_channelQuat[i]) > 0)
      m_channelQuat[i] -= rhs.m_channelQuat[i];
    else
      m_channelQuat[i] += rhs.m_channelQuat[i];

    // Pos
    m_channelPos[i] -= rhs.m_channelPos[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TransformsAccumulator::maxAbsQuatElements(NMP::Quat& quat) const
{
  quat.setXYZW(0.0f, 0.0f, 0.0f, 0.0f);

  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    quat.x = NMP::maximum(quat.x, fabs(m_channelQuat[i].x));
    quat.y = NMP::maximum(quat.y, fabs(m_channelQuat[i].y));
    quat.z = NMP::maximum(quat.z, fabs(m_channelQuat[i].z));
    quat.w = NMP::maximum(quat.w, fabs(m_channelQuat[i].w));
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TransformsAccumulator::maxAbsPosElements(NMP::Vector3& pos) const
{
  pos.setToZero();

  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    pos.x = NMP::maximum(pos.x, fabs(m_channelPos[i].x));
    pos.y = NMP::maximum(pos.y, fabs(m_channelPos[i].y));
    pos.z = NMP::maximum(pos.z, fabs(m_channelPos[i].z));
  }
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
