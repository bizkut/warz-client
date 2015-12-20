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
// inline included by NMPositionBasedRetarget.h
//----------------------------------------------------------------------------------------------------------------------

namespace NMRU
{

//----------------------------------------------------------------------------------------------------------------------
// PositionBasedRetarget::SourceJointParams construction and initialisation

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE PositionBasedRetarget::SourceJointParams* PositionBasedRetarget::SourceJointParams::init(NMP::Memory::Resource* memDesc)
{
  NMP_ASSERT(memDesc);
  NMP_ASSERT(memDesc->format.size >= (int32_t)sizeof(PositionBasedRetarget::SourceJointParams));

  // Initialise shell
  SourceJointParams* result = static_cast<SourceJointParams*>(memDesc->ptr);
  NMP::Memory::Resource internalMemTrack = *memDesc;
  NMP::Memory::Format shellFormat(sizeof(SourceJointParams), NMP_VECTOR_ALIGNMENT);
  internalMemTrack.increment(shellFormat);

  // Check size
  NMP_ASSERT(internalMemTrack.format.size == 0);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void PositionBasedRetarget::SourceJointParams::setDefaults()
{
  sourceJointBias = -1.0f;
  sourceJointPriority = 1.0f;
  sourceJointConfidence = 1.0f;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Memory::Format PositionBasedRetarget::SourceJointParams::getMemoryRequirements()
{
  // Shell
  NMP::Memory::Format result(sizeof(PositionBasedRetarget::SourceJointParams), NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void PositionBasedRetarget::SourceJointParams::endianSwap()
{
  NMP::endianSwap(sourceJointBias);
  NMP::endianSwap(sourceJointPriority);
  NMP::endianSwap(sourceJointConfidence);
}

//----------------------------------------------------------------------------------------------------------------------
// PositionBasedRetarget::TargetJointParams construction and initialisation

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE PositionBasedRetarget::TargetJointParams* PositionBasedRetarget::TargetJointParams::init(NMP::Memory::Resource* memDesc)
{
  NMP_ASSERT(memDesc);
  NMP_ASSERT(memDesc->format.size >= (int32_t)sizeof(PositionBasedRetarget::TargetJointParams));

  // Initialise shell
  TargetJointParams* result = static_cast<TargetJointParams*>(memDesc->ptr);
  NMP::Memory::Resource internalMemTrack = *memDesc;
  NMP::Memory::Format shellFormat(sizeof(TargetJointParams), NMP_VECTOR_ALIGNMENT);
  internalMemTrack.increment(shellFormat);

  // Check size
  NMP_ASSERT(internalMemTrack.format.size == 0);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void PositionBasedRetarget::TargetJointParams::setDefaults()
{
  offsetTransform = GeomUtils::identityPosQuat();
  isRigid = true;
  rotationWeight = 1.0f;
  positionWeight = 1.0f;
  isLimited = false;
  isHinge = false;
  limits = JointLimits::Params();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Memory::Format PositionBasedRetarget::TargetJointParams::getMemoryRequirements()
{
  // Shell
  NMP::Memory::Format result(sizeof(PositionBasedRetarget::TargetJointParams), NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void PositionBasedRetarget::TargetJointParams::endianSwap()
{
  NMP::endianSwap(offsetTransform.t);
  NMP::endianSwap(offsetTransform.q);
  NMP::endianSwap(isRigid);
  NMP::endianSwap(rotationWeight);
  NMP::endianSwap(positionWeight);
  NMP::endianSwap(isHinge);
  limits.endianSwap();
}

//----------------------------------------------------------------------------------------------------------------------
// PositionBasedRetarget::SourceParams construction and initialisation

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE PositionBasedRetarget::SourceParams* PositionBasedRetarget::SourceParams::init(NMP::Memory::Resource* memDesc, uint32_t numJoints)
{
  NMP_ASSERT(memDesc);
  NMP_ASSERT(memDesc->format.size >= (int32_t)sizeof(PositionBasedRetarget::SourceParams));

  // Initialise shell
  SourceParams* result = static_cast<SourceParams*>(memDesc->ptr);
  NMP::Memory::Resource internalMemTrack = *memDesc;
  NMP::Memory::Format shellFormat(sizeof(SourceParams), NMP_NATURAL_TYPE_ALIGNMENT);
  internalMemTrack.increment(shellFormat);

  // Initialise per-joint parameters
  NMP::Memory::Format jointParamsFormat = SourceJointParams::getMemoryRequirements();
  jointParamsFormat *= numJoints;
  internalMemTrack.align(jointParamsFormat);
  result->jointParams = static_cast<SourceJointParams*>(internalMemTrack.ptr);
  internalMemTrack.increment(jointParamsFormat);

  // Check size
  NMP_ASSERT(internalMemTrack.format.size == 0);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void PositionBasedRetarget::SourceParams::setDefaults(uint32_t numJoints)
{
  for (uint32_t j = 0; j < numJoints; ++j)
  {
    jointParams->setDefaults();
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Memory::Format PositionBasedRetarget::SourceParams::getMemoryRequirements(uint32_t numJoints)
{
  // Shell
  NMP::Memory::Format result(sizeof(PositionBasedRetarget::SourceParams), NMP_NATURAL_TYPE_ALIGNMENT);

  // Per-joint parameters
  NMP::Memory::Format jointParamsFormat = SourceJointParams::getMemoryRequirements();
  jointParamsFormat *= numJoints;
  result += jointParamsFormat;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void PositionBasedRetarget::SourceParams::dislocate(uint32_t numJoints)
{
  for (uint32_t j = 0; j < numJoints; ++j)
  {
    jointParams[j].endianSwap();
  }
  UNFIX_PTR(PositionBasedRetarget::SourceJointParams, jointParams);
  NMP::endianSwap(jointParams);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void PositionBasedRetarget::SourceParams::locate(uint32_t numJoints)
{
  NMP::endianSwap(jointParams);
  REFIX_PTR(PositionBasedRetarget::SourceJointParams, jointParams);
  for (uint32_t j = 0; j < numJoints; ++j)
  {
    jointParams[j].endianSwap();
  }
}

//----------------------------------------------------------------------------------------------------------------------
// PositionBasedRetarget::TargetParams construction and initialisation

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE PositionBasedRetarget::TargetParams* PositionBasedRetarget::TargetParams::init(NMP::Memory::Resource* memDesc, uint32_t numJoints)
{
  NMP_ASSERT(memDesc);
  NMP_ASSERT(memDesc->format.size >= (int32_t)sizeof(PositionBasedRetarget::TargetParams));

  // Initialise shell
  TargetParams* result = static_cast<TargetParams*>(memDesc->ptr);
  NMP::Memory::Resource internalMemTrack = *memDesc;
  NMP::Memory::Format shellFormat(sizeof(TargetParams), NMP_NATURAL_TYPE_ALIGNMENT);
  internalMemTrack.increment(shellFormat);

  // Initialise index map
  NMP::Memory::Format indexMapFormat(sizeof(int32_t), NMP_NATURAL_TYPE_ALIGNMENT);
  indexMapFormat *= numJoints;
  internalMemTrack.align(indexMapFormat);
  result->targetToSourceMap = static_cast<int32_t*>(internalMemTrack.ptr);
  internalMemTrack.increment(indexMapFormat);

  // Initialise per-joint parameters
  NMP::Memory::Format jointParamsFormat = TargetJointParams::getMemoryRequirements();
  jointParamsFormat *= numJoints;
  internalMemTrack.align(jointParamsFormat);
  result->jointParams = static_cast<TargetJointParams*>(internalMemTrack.ptr);
  internalMemTrack.increment(jointParamsFormat);

  // Check size
  NMP_ASSERT(internalMemTrack.format.size == 0);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void PositionBasedRetarget::TargetParams::setDefaults(uint32_t numJoints)
{
  rigScale = 1.0f;
  debugMode = kRetargetDebuggingOff;
  for (uint32_t j = 0; j < numJoints; ++j)
  {
    targetToSourceMap[j] = -1; // Default is no map
    jointParams[j].setDefaults();
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Memory::Format PositionBasedRetarget::TargetParams::getMemoryRequirements(uint32_t numJoints)
{
  // Shell
  NMP::Memory::Format result(sizeof(PositionBasedRetarget::TargetParams), NMP_NATURAL_TYPE_ALIGNMENT);

  // Index map
  NMP::Memory::Format indexMapFormat(sizeof(int32_t), NMP_NATURAL_TYPE_ALIGNMENT);
  indexMapFormat *= numJoints;
  result += indexMapFormat;

  // Per-joint parameters
  NMP::Memory::Format jointParamsFormat = TargetJointParams::getMemoryRequirements();
  jointParamsFormat *= numJoints;
  result += jointParamsFormat;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void PositionBasedRetarget::TargetParams::dislocate(uint32_t numJoints)
{
  for (uint32_t j = 0; j < numJoints; ++j)
  {
    jointParams[j].endianSwap();
    NMP::endianSwap(targetToSourceMap[j]);
  }
  UNFIX_PTR(PositionBasedRetarget::TargetJointParams, jointParams);
  UNFIX_PTR(int32_t, targetToSourceMap);
  NMP::endianSwap(jointParams);
  NMP::endianSwap(targetToSourceMap);
  NMP::endianSwap(debugMode);
  NMP::endianSwap(rigScale);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void PositionBasedRetarget::TargetParams::locate(uint32_t numJoints)
{
  NMP::endianSwap(rigScale);
  NMP::endianSwap(debugMode);
  NMP::endianSwap(targetToSourceMap);
  NMP::endianSwap(jointParams);
  REFIX_PTR(int32_t, targetToSourceMap);
  REFIX_PTR(PositionBasedRetarget::TargetJointParams, jointParams);
  for (uint32_t j = 0; j < numJoints; ++j)
  {
    NMP::endianSwap(targetToSourceMap[j]);
    jointParams[j].endianSwap();
  }
}

//----------------------------------------------------------------------------------------------------------------------
// PositionBasedRetarget::Solver construction and initialisation

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE PositionBasedRetarget::Solver* PositionBasedRetarget::Solver::init(
  NMP::Memory::Resource* memDesc,
  uint32_t numSourceJoints,
  uint32_t numTargetJoints)
{
  NMP_ASSERT(memDesc);
  NMP_ASSERT(memDesc->format == getMemoryRequirements(numSourceJoints, numTargetJoints));
  NMP_ASSERT(memDesc->format.size >= (int32_t)sizeof(PositionBasedRetarget::Solver));

  // Initialise shell
  Solver* result = static_cast<Solver*>(memDesc->ptr);
  NMP::Memory::Resource internalMemTrack = *memDesc;
  NMP::Memory::Format shellFormat(sizeof(Solver), NMP_NATURAL_TYPE_ALIGNMENT);
  internalMemTrack.increment(shellFormat);

  // Initialise values
  result->m_estimatedTargetRigScale = 1.0f;
  result->m_numSourceJoints = numSourceJoints;
  result->m_numTargetJoints = numTargetJoints;
  result->m_memoryRequirements = getMemoryRequirements(numSourceJoints, numTargetJoints);

  // Initialise source-to-target map
  NMP::Memory::Format intArrayFormat(sizeof(int32_t), NMP_NATURAL_TYPE_ALIGNMENT);
  intArrayFormat *= numSourceJoints;
  internalMemTrack.align(intArrayFormat);
  result->m_sourceToTargetMap = static_cast<int32_t*>(internalMemTrack.ptr);
  internalMemTrack.increment(intArrayFormat);
  // Initialise values to all unmapped
  for (uint32_t j = 0; j < numSourceJoints; ++j)
  {
    result->m_sourceToTargetMap[j] = -1;
  }

  // Initialise mapped hierarchy
  internalMemTrack.align(intArrayFormat);
  result->m_mappedHierarchy = static_cast<int32_t*>(internalMemTrack.ptr);
  internalMemTrack.increment(intArrayFormat);
  // Initialise to all unmapped
  for (uint32_t j = 0; j < numSourceJoints; ++j)
  {
    result->m_mappedHierarchy[j] = -1;
  }

  // Initialise reverse hierarchy
  NMP::Memory::Format intPointerArrayFormat(sizeof(int32_t*), NMP_NATURAL_TYPE_ALIGNMENT);
  intPointerArrayFormat *= numSourceJoints;
  internalMemTrack.align(intPointerArrayFormat);
  result->m_reverseHierarchy = static_cast<int32_t**>(internalMemTrack.ptr);
  internalMemTrack.increment(intPointerArrayFormat);
  // Array of the number of mapped children each mapped joint has
  internalMemTrack.align(intArrayFormat);
  result->m_numMappedChildren = static_cast<int32_t*>(internalMemTrack.ptr);
  internalMemTrack.increment(intArrayFormat);
  // Array of child data itself.  Maximum number of parent-child relationships you can have
  // is one fewer than the number of joints, so allocate enough space for that.
  uint32_t sizeReverseHierarchy = numSourceJoints == 0 ? 0 : numSourceJoints - 1;
  NMP::Memory::Format intArrayAltFormat(sizeof(int32_t*), NMP_NATURAL_TYPE_ALIGNMENT);
  intArrayAltFormat *= sizeReverseHierarchy;
  internalMemTrack.align(intArrayAltFormat);
  result->m_reverseHierarchyData = static_cast<int32_t*>(internalMemTrack.ptr);
  internalMemTrack.increment(intArrayAltFormat);
  // Initialise everything as unmapped
  for (uint32_t j = 0; j < numSourceJoints; ++j)
  {
    result->m_reverseHierarchy[j] = NULL;
    result->m_numMappedChildren[j] = -1;
  }

  // Working memory
  // Input transforms
  NMP::Memory::Format transformBufferFormat =
    NMP::DataBuffer::getPosQuatMemoryRequirements(numTargetJoints);
  internalMemTrack.align(transformBufferFormat);
  NMP::Memory::Resource transformBufferResource;
  transformBufferResource.format = transformBufferFormat;
  transformBufferResource.ptr = internalMemTrack.ptr;
  result->m_inputTransforms = NMP::DataBuffer::initPosQuat(transformBufferResource, transformBufferFormat, numTargetJoints);
  internalMemTrack.increment(transformBufferFormat);
  // Worldspace transforms
  internalMemTrack.align(transformBufferFormat);
  transformBufferResource.format = transformBufferFormat;
  transformBufferResource.ptr = internalMemTrack.ptr;
  result->m_worldTransforms = NMP::DataBuffer::initPosQuat(transformBufferResource, transformBufferFormat, numTargetJoints);
  internalMemTrack.increment(transformBufferFormat);
  // Target joint rotation confidence
  NMP::Memory::Format floatArrayFormat(sizeof(float), NMP_NATURAL_TYPE_ALIGNMENT);
  floatArrayFormat *= numTargetJoints;
  internalMemTrack.align(floatArrayFormat);
  result->m_targetRotationConfidence = static_cast<float*>(internalMemTrack.ptr);
  internalMemTrack.increment(floatArrayFormat);
  // Target joint position confidence
  internalMemTrack.align(floatArrayFormat);
  result->m_targetPositionConfidence = static_cast<float*>(internalMemTrack.ptr);
  internalMemTrack.increment(floatArrayFormat);

  // Check size
  NMP_ASSERT(internalMemTrack.format.size == 0);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Memory::Format PositionBasedRetarget::Solver::getMemoryRequirements(
  uint32_t numSourceJoints,
  uint32_t numTargetJoints)
{
  // Shell
  NMP::Memory::Format result(sizeof(PositionBasedRetarget::Solver), NMP_NATURAL_TYPE_ALIGNMENT);

  // Source-to-target map
  NMP::Memory::Format intArrayFormat(sizeof(int32_t), NMP_NATURAL_TYPE_ALIGNMENT);
  intArrayFormat *= numSourceJoints;
  result += intArrayFormat;

  // Mapped hierarchy
  result += intArrayFormat;

  // Reverse hierarchy
  NMP::Memory::Format intPointerArrayFormat(sizeof(int32_t*), NMP_NATURAL_TYPE_ALIGNMENT);
  intPointerArrayFormat *= numSourceJoints;
  result += intPointerArrayFormat;
  // No. mapped children
  result += intArrayFormat;
  // Reverse hierarchy data
  uint32_t sizeReverseHierarchy = numSourceJoints == 0 ? 0 : numSourceJoints - 1;
  NMP::Memory::Format intArrayAltFormat(sizeof(int32_t*), NMP_NATURAL_TYPE_ALIGNMENT);
  intArrayAltFormat *= sizeReverseHierarchy;
  result += intArrayAltFormat;

  // Working memory
  // Input transforms
  NMP::Memory::Format transformBufferFormat =
    NMP::DataBuffer::getPosQuatMemoryRequirements(numTargetJoints);
  result += transformBufferFormat;
  // Worldspace transforms
  result += transformBufferFormat;
  // Target joint confidences
  NMP::Memory::Format floatArrayFormat(sizeof(float), NMP_NATURAL_TYPE_ALIGNMENT);
  floatArrayFormat *= numTargetJoints;
  result += floatArrayFormat; // Rotation confidences
  result += floatArrayFormat; // Position confidences

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void PositionBasedRetarget::Solver::dislocate()
{
  // Nested array contents
  for (int32_t j = 0; j < (signed)m_numSourceJoints - 1; ++j)
  {
    NMP::endianSwap(m_reverseHierarchyData[j]);
  }
  // Array contents
  for (uint32_t j = 0; j < m_numTargetJoints; ++j)
  {
    NMP::endianSwap(m_targetPositionConfidence[j]);
    NMP::endianSwap(m_targetRotationConfidence[j]);
  }
  m_worldTransforms->dislocate();
  m_inputTransforms->dislocate();
  for (uint32_t j = 0; j < m_numSourceJoints; ++j)
  {
    // Leave NULL as NULL
    if (m_reverseHierarchy[j] != 0)
    {
      UNFIX_PTR(int32_t, m_reverseHierarchy[j]);
    }
    NMP::endianSwap(m_reverseHierarchy[j]);
    NMP::endianSwap(m_numMappedChildren[j]);
    NMP::endianSwap(m_mappedHierarchy[j]);
    NMP::endianSwap(m_sourceToTargetMap[j]);
  }

  // Pointers
  UNFIX_PTR(float, m_targetPositionConfidence);
  UNFIX_PTR(float, m_targetRotationConfidence);
  UNFIX_PTR(NMP::DataBuffer, m_worldTransforms);
  UNFIX_PTR(NMP::DataBuffer, m_inputTransforms);
  UNFIX_PTR(int32_t, m_reverseHierarchyData);
  UNFIX_PTR(int32_t, m_numMappedChildren);
  UNFIX_PTR(int32_t*, m_reverseHierarchy);
  UNFIX_PTR(int32_t, m_mappedHierarchy);
  UNFIX_PTR(int32_t, m_sourceToTargetMap);

  // Data
  NMP::endianSwap(m_targetPositionConfidence);
  NMP::endianSwap(m_targetRotationConfidence);
  NMP::endianSwap(m_worldTransforms);
  NMP::endianSwap(m_inputTransforms);
  NMP::endianSwap(m_reverseHierarchyData);
  NMP::endianSwap(m_numMappedChildren);
  NMP::endianSwap(m_reverseHierarchy);
  NMP::endianSwap(m_mappedHierarchy);
  NMP::endianSwap(m_sourceToTargetMap);
  NMP::endianSwap(m_numTargetJoints);
  NMP::endianSwap(m_numSourceJoints);
  NMP::endianSwap(m_estimatedTargetRigScale);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void PositionBasedRetarget::Solver::locate()
{
  // Data
  NMP::endianSwap(m_estimatedTargetRigScale);
  NMP::endianSwap(m_numSourceJoints);
  NMP::endianSwap(m_numTargetJoints);
  NMP::endianSwap(m_sourceToTargetMap);
  NMP::endianSwap(m_mappedHierarchy);
  NMP::endianSwap(m_reverseHierarchyData);
  NMP::endianSwap(m_reverseHierarchy);
  NMP::endianSwap(m_numMappedChildren);
  NMP::endianSwap(m_inputTransforms);
  NMP::endianSwap(m_worldTransforms);
  NMP::endianSwap(m_targetRotationConfidence);
  NMP::endianSwap(m_targetPositionConfidence);

  // Pointers
  REFIX_PTR(int32_t, m_sourceToTargetMap);
  REFIX_PTR(int32_t, m_mappedHierarchy);
  REFIX_PTR(int32_t*, m_reverseHierarchy);
  REFIX_PTR(int32_t, m_numMappedChildren);
  REFIX_PTR(int32_t, m_reverseHierarchyData);
  REFIX_PTR(NMP::DataBuffer, m_inputTransforms);
  REFIX_PTR(NMP::DataBuffer, m_worldTransforms);
  REFIX_PTR(float, m_targetRotationConfidence);
  REFIX_PTR(float, m_targetPositionConfidence);

  // Array contents
  for (uint32_t j = 0; j < m_numSourceJoints; ++j)
  {
    NMP::endianSwap(m_sourceToTargetMap[j]);
    NMP::endianSwap(m_mappedHierarchy[j]);
    NMP::endianSwap(m_numMappedChildren[j]);
    NMP::endianSwap(m_reverseHierarchy[j]);
    // Leave NULL as NULL
    if (m_reverseHierarchy[j] != 0)
    {
      REFIX_PTR(int32_t, m_reverseHierarchy[j]);
    }
  }
  m_inputTransforms->locate();
  m_worldTransforms->locate();
  for (uint32_t j = 0; j < m_numTargetJoints; ++j)
  {
    NMP::endianSwap(m_targetRotationConfidence[j]);
    NMP::endianSwap(m_targetPositionConfidence[j]);
  }

  // Nested array contents
  for (int32_t j = 0; j < (signed)m_numSourceJoints - 1; ++j)
  {
    NMP::endianSwap(m_reverseHierarchyData[j]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void PositionBasedRetarget::Solver::relocate()
{
  NMP::Memory::Resource memTrack;
  memTrack.format = m_memoryRequirements;
  memTrack.ptr = this;
  NMP_ASSERT(NMP_IS_ALIGNED(memTrack.ptr, memTrack.format.alignment));

  // Shell
  NMP::Memory::Format format(sizeof(Solver), NMP_NATURAL_TYPE_ALIGNMENT);
  memTrack.align(format);
  memTrack.increment(format);

  // Source-to-target map
  format.set(sizeof(int32_t), NMP_NATURAL_TYPE_ALIGNMENT);
  format *= m_numSourceJoints;
  memTrack.align(format);
  m_sourceToTargetMap = static_cast<int32_t*>(memTrack.ptr);
  memTrack.increment(format);

  // Mapped hierarchy
  memTrack.align(format);
  m_mappedHierarchy = static_cast<int32_t*>(memTrack.ptr);
  memTrack.increment(format);

  // Reverse hierarchy
  format.set(sizeof(int32_t*), NMP_NATURAL_TYPE_ALIGNMENT);
  format *= m_numSourceJoints;
  memTrack.align(format);
  m_reverseHierarchy = static_cast<int32_t**>(memTrack.ptr);
  memTrack.increment(format);
  // Num mapped children array
  format.set(sizeof(int32_t), NMP_NATURAL_TYPE_ALIGNMENT);
  format *= m_numSourceJoints;
  memTrack.align(format);
  m_numMappedChildren = static_cast<int32_t*>(memTrack.ptr);
  memTrack.increment(format);
  // Reverse hierarchy data
  uint32_t sizeReverseHierarchy = m_numSourceJoints == 0 ? 0 : m_numSourceJoints - 1;
  format.set(sizeof(int32_t*), NMP_NATURAL_TYPE_ALIGNMENT);
  format *= sizeReverseHierarchy;
  memTrack.align(format);
  m_reverseHierarchyData = static_cast<int32_t*>(memTrack.ptr);
  int32_t* whichChild = m_reverseHierarchyData;
  for (uint32_t j = 0; j < m_numSourceJoints; ++j)
  {
    m_reverseHierarchy[j] = NULL;
    int32_t numMappedChildren = m_numMappedChildren[j];
    if (numMappedChildren > 0)
    {
      m_reverseHierarchy[j] = whichChild;
      whichChild += numMappedChildren;
    }
  }
  memTrack.increment(format);

  // Working memory
  // Input transforms
  format = NMP::DataBuffer::getPosQuatMemoryRequirements(m_numTargetJoints);
  memTrack.align(format);
  m_inputTransforms = static_cast<NMP::DataBuffer*>(memTrack.ptr);
  m_inputTransforms->relocate();
  memTrack.increment(format);
  // Worldspace transforms
  memTrack.align(format);
  m_worldTransforms = static_cast<NMP::DataBuffer*>(memTrack.ptr);
  m_worldTransforms->relocate();
  memTrack.increment(format);
  // Target joint rotation confidence
  format.set(sizeof(float), NMP_NATURAL_TYPE_ALIGNMENT);
  format *= m_numTargetJoints;
  memTrack.align(format);
  m_targetRotationConfidence = static_cast<float*>(memTrack.ptr);
  memTrack.increment(format);
  // Target joint position confidence
  memTrack.align(format);
  m_targetPositionConfidence = static_cast<float*>(memTrack.ptr);
  memTrack.increment(format);

  // Check size
  NMP_ASSERT(memTrack.format.size == 0);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void PositionBasedRetarget::Solver::calculateOffsets(
  const SourceParams* sourceParams,
  const NMP::DataBuffer* sourceTransforms,
  const TargetParams* targetParams,
  NMP::DataBuffer* targetTransforms,
  const int32_t* targetHierarchy,
  const NMRU::GeomUtils::PosQuat& rootTransform,
  float scaleMultiplier /* = 1.0f */)
{
  retarget(
    sourceParams,
    sourceTransforms,
    targetParams,
    targetTransforms,
    targetHierarchy,
    rootTransform,
    scaleMultiplier,
    false, // Don't ignore input joints
    true);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool PositionBasedRetarget::Solver::prepare(
  const TargetParams* targetParams,
  uint32_t numTargetJoints,
  const int32_t* targetHierarchy)
{
  return prepare(targetParams->targetToSourceMap, numTargetJoints, targetHierarchy);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float PositionBasedRetarget::Solver::getEstimatedRigScale() const
{
  return m_estimatedTargetRigScale;
}

} // end of namespace NMRU
