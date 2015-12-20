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
// inline included by NMFKRetarget.h
//----------------------------------------------------------------------------------------------------------------------

namespace NMRU
{

//----------------------------------------------------------------------------------------------------------------------
// FKRetarget::PerJointParams construction and initialisation

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE FKRetarget::PerJointParams* FKRetarget::PerJointParams::init(NMP::Memory::Resource* memDesc)
{
  NMP_ASSERT(memDesc);
  NMP_ASSERT(memDesc->format.size >= (int32_t)sizeof(FKRetarget::PerJointParams));

  // Initialise shell
  PerJointParams* result = static_cast<PerJointParams*>(memDesc->ptr);
  NMP::Memory::Resource internalMemTrack = *memDesc;
  NMP::Memory::Format shellFormat(sizeof(PerJointParams), NMP_VECTOR_ALIGNMENT);
  internalMemTrack.increment(shellFormat);

  // Check size
  NMP_ASSERT(internalMemTrack.format.size == 0);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void FKRetarget::PerJointParams::setDefaults()
{
  offsetTransform = GeomUtils::identityPosQuat();
  referenceTransform = GeomUtils::identityPosQuat();
  isRigid = true;
  rotationWeight = 1.0f;
  positionWeight = 1.0f;
  bias = 0.0f;
  mirrorFlags.clearAll();
  isLimited = false;
  limits = JointLimits::Params();
  redistributeClampedMotion = true;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Memory::Format FKRetarget::PerJointParams::getMemoryRequirements()
{
  // Shell
  NMP::Memory::Format result(sizeof(FKRetarget::PerJointParams), NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void FKRetarget::PerJointParams::endianSwap()
{
  NMP::endianSwap(offsetTransform);
  NMP::endianSwap(referenceTransform);
  NMP::endianSwap(isRigid);
  NMP::endianSwap(rotationWeight);
  NMP::endianSwap(positionWeight);
  NMP::endianSwap(bias);
  NMP::endianSwap(mirrorFlags);
  NMP::endianSwap(isLimited);
  limits.endianSwap();
  NMP::endianSwap(redistributeClampedMotion);
}

//----------------------------------------------------------------------------------------------------------------------
// FKRetarget::Params construction and initialisation

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE FKRetarget::Params* FKRetarget::Params::init(NMP::Memory::Resource* memDesc, uint32_t numJoints)
{
  NMP_ASSERT(memDesc);
  NMP_ASSERT(memDesc->format.size >= (int32_t)sizeof(FKRetarget::Params));

  // Initialise shell
  Params* result = static_cast<Params*>(memDesc->ptr);
  NMP::Memory::Resource internalMemTrack = *memDesc;
  NMP::Memory::Format shellFormat(sizeof(Params), NMP_NATURAL_TYPE_ALIGNMENT);
  internalMemTrack.increment(shellFormat);

  // Initialise index map
  NMP::Memory::Format indexMapFormat(sizeof(int32_t), NMP_NATURAL_TYPE_ALIGNMENT);
  indexMapFormat *= numJoints;
  internalMemTrack.align(indexMapFormat);
  result->indexMap = static_cast<int32_t*>(internalMemTrack.ptr);
  internalMemTrack.increment(indexMapFormat);

  // Initialise joint reference index array
  NMP::Memory::Format referenceIndexFormat(sizeof(int32_t), NMP_NATURAL_TYPE_ALIGNMENT);
  referenceIndexFormat *= numJoints;
  internalMemTrack.align(referenceIndexFormat);
  result->jointReferenceIndex = static_cast<int32_t*>(internalMemTrack.ptr);
  internalMemTrack.increment(referenceIndexFormat);

  // Initialise per-joint parameters
  // Initialise array with the result of init of the first member, then just call init
  // for the other members - in case the init function is doing some important pointer
  // resolution.
  NMP::Memory::Format onePerJointParamsFormat = PerJointParams::getMemoryRequirements();
  NMP::Memory::Resource perJointParamsResource;
  internalMemTrack.align(onePerJointParamsFormat);
  perJointParamsResource.format = onePerJointParamsFormat;
  perJointParamsResource.ptr = internalMemTrack.ptr;
  result->perJointParams = PerJointParams::init(&perJointParamsResource);
  internalMemTrack.increment(onePerJointParamsFormat);
  for (uint32_t j = 1; j < numJoints; ++j)
  {
    internalMemTrack.align(onePerJointParamsFormat);
    perJointParamsResource.ptr = internalMemTrack.ptr;
    PerJointParams::init(&perJointParamsResource);
    internalMemTrack.increment(onePerJointParamsFormat);
  }

  // Check size
  NMP_ASSERT(internalMemTrack.format.size == 0);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void FKRetarget::Params::setDefaults(uint32_t numJoints)
{
  rigScale = 1.0f;
  for (uint32_t j = 0; j < numJoints; ++j)
  {
    indexMap[j] = -1; // Default is no map
    jointReferenceIndex[j] = j; // Referenced to its own input transform by default
    perJointParams[j].setDefaults();
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Memory::Format FKRetarget::Params::getMemoryRequirements(uint32_t numJoints)
{
  // Shell
  NMP::Memory::Format result(sizeof(FKRetarget::Params), NMP_NATURAL_TYPE_ALIGNMENT);

  // Index map
  NMP::Memory::Format indexMapFormat(sizeof(uint32_t), NMP_NATURAL_TYPE_ALIGNMENT);
  indexMapFormat *= numJoints;
  result += indexMapFormat;

  // Reference joints
  NMP::Memory::Format referenceIndexFormat(sizeof(int32_t), NMP_NATURAL_TYPE_ALIGNMENT);
  referenceIndexFormat *= numJoints;
  result += referenceIndexFormat;

  // Per-joint parameters
  // The multiply operator would pad the final member up to an alignment boundary, whereas
  // we will added members one by one so the last member may not be padded.  So we must duplicate
  // this by using += rather than *= to ensure we request the right amount of memory.
  NMP::Memory::Format perJointParamsFormat = PerJointParams::getMemoryRequirements();
  perJointParamsFormat *= numJoints;
  result += perJointParamsFormat;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// FKRetarget::Params serialisation

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void FKRetarget::Params::locate(uint32_t numJoints)
{
  // Locate local data
  NMP::endianSwap(rigScale);

  // Locate pointers
  NMP::endianSwap(indexMap);
  NMP::endianSwap(jointReferenceIndex);
  NMP::endianSwap(perJointParams);

  // Refix pointers
  REFIX_PTR_RELATIVE(int32_t, indexMap, this);
  REFIX_PTR_RELATIVE(int32_t, jointReferenceIndex, this);
  REFIX_PTR_RELATIVE(PerJointParams, perJointParams, this);

  // Locate trailing memory
  for (uint32_t j = 0; j < numJoints; ++j)
  {
    NMP::endianSwap(indexMap[j]);
    NMP::endianSwap(jointReferenceIndex[j]);
    perJointParams[j].endianSwap();
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void FKRetarget::Params::dislocate(uint32_t numJoints)
{
  // Dislocate trailing memory
  for (uint32_t j = 0; j < numJoints; ++j)
  {
    NMP::endianSwap(indexMap[j]);
    NMP::endianSwap(jointReferenceIndex[j]);
    perJointParams[j].endianSwap();
  }

  // Unfix pointers
  UNFIX_PTR_RELATIVE(int32_t, indexMap, this);
  UNFIX_PTR_RELATIVE(int32_t, jointReferenceIndex, this);
  UNFIX_PTR_RELATIVE(PerJointParams, perJointParams, this);

  // Dislocate pointers
  NMP::endianSwap(indexMap);
  NMP::endianSwap(jointReferenceIndex);
  NMP::endianSwap(perJointParams);

  // Dislocate local data
  NMP::endianSwap(rigScale);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void FKRetarget::Params::relocate(void* location, uint32_t numJoints)
{
  NMP::Memory::Resource memTrack;
  NMP::Memory::Format format = getMemoryRequirements(numJoints);
  memTrack.format = format;
  memTrack.ptr = location;

  // Skip Shell
  memTrack.align(format);
  format.set(sizeof(FKRetarget::Params), NMP_NATURAL_TYPE_ALIGNMENT);
  memTrack.increment(format);

  // Index map
  format.set(sizeof(uint32_t), NMP_NATURAL_TYPE_ALIGNMENT);
  format *= numJoints;
  memTrack.align(format);
  indexMap = (int32_t*)memTrack.ptr;
  memTrack.increment(format);

  // Reference joints
  memTrack.align(format);
  jointReferenceIndex = (int32_t*)memTrack.ptr;
  memTrack.increment(format);

  // Per-joint parameters
  format = PerJointParams::getMemoryRequirements();
  format *= numJoints;
  memTrack.align(format);
  perJointParams = (PerJointParams*)memTrack.ptr;
  memTrack.increment(format);

  // Check we used all the memory expected
#ifdef NM_HOST_CELL_SPU
  NMP_SPU_ASSERT(memTrack.format.size == 0);
#else
  NMP_ASSERT(memTrack.format.size == 0);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
// FKRetarget::Solver construction and initialisation
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE FKRetarget::Solver* FKRetarget::Solver::init(
  NMP::Memory::Resource* memDesc,
  const StorageStats& storageStats)
{
  NMP_ASSERT(memDesc);
  NMP_ASSERT(memDesc->format.size >= (int32_t)sizeof(FKRetarget::Solver));
  NMP_ASSERT(storageStats.mostMappedJoints <= storageStats.largestRigSize);
  NMP_ASSERT(storageStats.longestSequence >= 1);

  // Initialise shell
  Solver* result = static_cast<Solver*>(memDesc->ptr);
  NMP::Memory::Resource internalMemTrack = *memDesc;
  {
    NMP::Memory::Format shellFormat(sizeof(Solver), NMP_NATURAL_TYPE_ALIGNMENT);
    internalMemTrack.increment(shellFormat);
  }

  // Initialise values
  result->m_intermediateRigSize = storageStats.intermediateRigSize;
  result->m_largestRigSize = storageStats.largestRigSize;
  result->m_memoryRequirements = getMemoryRequirements(storageStats);

  // Initialise intermediate rig data
  {
    NMP::Memory::Format intermediateDataFormat(sizeof(IntermediateJointData), NMP_NATURAL_TYPE_ALIGNMENT);
    intermediateDataFormat *= storageStats.intermediateRigSize;
    internalMemTrack.align(intermediateDataFormat);
    result->m_intermediateRigData = static_cast<IntermediateJointData*>(internalMemTrack.ptr);
    internalMemTrack.increment(intermediateDataFormat);
  }

  // Initialise values
  for (uint32_t j = 0; j < storageStats.intermediateRigSize; ++j)
  {
    result->m_intermediateRigData[j].sourceIndex = -1;
    result->m_intermediateRigData[j].targetIndex = -1;
    result->m_intermediateRigData[j].sourceSequenceLength = 0;
    result->m_intermediateRigData[j].targetSequenceLength = 0;
    result->m_intermediateRigData[j].isRootJoint = false;
  }

  // Get and store multiplex-packaged size
  //   Get most possible individual entries.  The extra are possible unused entries in the packages.
  result->m_mpMaxLength = calculateMaxNumPackages(storageStats);

  // Initialise multiplex-packaged mappings
  {
    NMP::Memory::Format mappingsFormat(sizeof(MPIndexMap), NMP_NATURAL_TYPE_ALIGNMENT);
    mappingsFormat *= result->m_mpMaxLength;
    // Source mappings
    internalMemTrack.align(mappingsFormat);
    result->m_mpSourceMappings = static_cast<MPIndexMap*>(internalMemTrack.ptr);
    internalMemTrack.increment(mappingsFormat);
    // Target mappings
    internalMemTrack.align(mappingsFormat);
    result->m_mpTargetMappings = static_cast<MPIndexMap*>(internalMemTrack.ptr);
    internalMemTrack.increment(mappingsFormat);
  }

  // Initialise multiplex-packaged geometrical parameters
  {
    NMP::Memory::Format geomFormat(sizeof(MPGeometry), NMP_VECTOR_ALIGNMENT);
    // Add one extra package to store identity matrices for defaults
    geomFormat *= result->m_mpMaxLength + 1;
    // Source geom
    internalMemTrack.align(geomFormat);
    result->m_mpSourceGeom = static_cast<MPGeometry*>(internalMemTrack.ptr);
    //  We actually want to point to the second member, so the first is index -1
    result->m_mpSourceGeom++;
    internalMemTrack.increment(geomFormat);
    // Target geom
    internalMemTrack.align(geomFormat);
    result->m_mpTargetGeom = static_cast<MPGeometry*>(internalMemTrack.ptr);
    //  We actually want to point to the second member, so the first is index -1
    result->m_mpTargetGeom++;
    internalMemTrack.increment(geomFormat);
  }

  // Initialise multiplex-packaged parameters for other purposes
  {
    NMP::Memory::Format paramsFormat(sizeof(MPParams), NMP_VECTOR_ALIGNMENT);
    paramsFormat *= result->m_mpMaxLength;
    internalMemTrack.align(paramsFormat);
    result->m_mpParams = static_cast<MPParams*>(internalMemTrack.ptr);
    internalMemTrack.increment(paramsFormat);
  }

  // Check size
  NMP_ASSERT(internalMemTrack.format.size == 0);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Memory::Format FKRetarget::Solver::getMemoryRequirements(
  const StorageStats& storageStats)
{
  // Shell
  NMP::Memory::Format result(sizeof(FKRetarget::Solver), NMP_NATURAL_TYPE_ALIGNMENT);

  // Intermediate rig
  {
    NMP::Memory::Format intermediateDataFormat(sizeof(IntermediateJointData), NMP_NATURAL_TYPE_ALIGNMENT);
    intermediateDataFormat *= storageStats.intermediateRigSize;
    result += intermediateDataFormat;
  }

  // Get and store multiplex-packaged size
  //   Get most possible individual entries.  The extra are possible unused entries in the packages.
  uint32_t mpMaxLength = calculateMaxNumPackages(storageStats);

  // Multiplex-packaged mappings
  {
    NMP::Memory::Format mappingsFormat(sizeof(MPIndexMap), NMP_NATURAL_TYPE_ALIGNMENT);
    mappingsFormat *= mpMaxLength;
    result += mappingsFormat;
    result += mappingsFormat;
  }

  // Multiplex-packaged geometrical parameters
  {
    NMP::Memory::Format geomFormat(sizeof(MPGeometry), NMP_VECTOR_ALIGNMENT);
    // Add one extra package to store identity matrices for defaults
    geomFormat *= mpMaxLength + 1;
    result += geomFormat;
    result += geomFormat;
  }

  // Multiplex-packaged parameters for other purposes
  {
    NMP::Memory::Format paramsFormat(sizeof(MPParams), NMP_VECTOR_ALIGNMENT);
    paramsFormat *= mpMaxLength;
    result += paramsFormat; // Just one because we don't always need for both source and target
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void FKRetarget::Solver::locate()
{
  // Locate local data
  NMP::endianSwap(m_sourceInverseRigScale);
  NMP::endianSwap(m_targetRigScale);
  NMP::endianSwap(m_intermediateRigSize);
  NMP::endianSwap(m_largestRigSize);
  NMP::endianSwap(m_memoryRequirements);
  NMP::endianSwap(m_mpMaxLength);
  NMP::endianSwap(m_mpSourceLength);
  NMP::endianSwap(m_mpTargetLength);
  NMP::endianSwap(m_mpFirstNToMPackageInSource);
  NMP::endianSwap(m_mpFirstNToMPackageInTarget);
  NMP::endianSwap(m_applyJointWeights);

  // Locate and refix pointers
  REFIX_SWAP_PTR(IntermediateJointData, m_intermediateRigData);
  REFIX_SWAP_PTR(MPIndexMap, m_mpSourceMappings);
  REFIX_SWAP_PTR(MPIndexMap, m_mpTargetMappings);
  REFIX_SWAP_PTR(MPGeometry, m_mpSourceGeom);
  REFIX_SWAP_PTR(MPGeometry, m_mpTargetGeom);

  // Locate pointed-to data
  for (uint32_t j = 0; j < m_intermediateRigSize; ++j)
  {
    NMP::endianSwap(m_intermediateRigData[j].sourceIndex);
    NMP::endianSwap(m_intermediateRigData[j].targetIndex);
    NMP::endianSwap(m_intermediateRigData[j].sourceSequenceLength);
    NMP::endianSwap(m_intermediateRigData[j].targetSequenceLength);
    NMP::endianSwap(m_intermediateRigData[j].isRootJoint);
  }

  endianSwapMPData();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void FKRetarget::Solver::dislocate()
{
  // Dislocate pointed-to data
  for (uint32_t j = 0; j < m_intermediateRigSize; ++j)
  {
    NMP::endianSwap(m_intermediateRigData[j].sourceIndex);
    NMP::endianSwap(m_intermediateRigData[j].targetIndex);
    NMP::endianSwap(m_intermediateRigData[j].sourceSequenceLength);
    NMP::endianSwap(m_intermediateRigData[j].targetSequenceLength);
    NMP::endianSwap(m_intermediateRigData[j].isRootJoint);
  }

  endianSwapMPData();

  // Unfix and dislocate pointers
  UNFIX_SWAP_PTR(IntermediateJointData, m_intermediateRigData);
  UNFIX_SWAP_PTR(MPIndexMap, m_mpSourceMappings);
  UNFIX_SWAP_PTR(MPIndexMap, m_mpTargetMappings);
  UNFIX_SWAP_PTR(MPGeometry, m_mpSourceGeom);
  UNFIX_SWAP_PTR(MPGeometry, m_mpTargetGeom);

  // Dislocate local data
  NMP::endianSwap(m_sourceInverseRigScale);
  NMP::endianSwap(m_targetRigScale);
  NMP::endianSwap(m_intermediateRigSize);
  NMP::endianSwap(m_largestRigSize);
  NMP::endianSwap(m_memoryRequirements);
  NMP::endianSwap(m_mpMaxLength);
  NMP::endianSwap(m_mpSourceLength);
  NMP::endianSwap(m_mpTargetLength);
  NMP::endianSwap(m_mpFirstNToMPackageInSource);
  NMP::endianSwap(m_mpFirstNToMPackageInTarget);
  NMP::endianSwap(m_applyJointWeights);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void FKRetarget::Solver::endianSwapMPData()
{
  for (uint32_t j = 0; j < m_mpMaxLength; ++j)
  {
    // Twice, once for source, once for target
    MPIndexMap* map = &(m_mpSourceMappings[j]);
    MPGeometry* geom = &(m_mpSourceGeom[j]);
    for (uint32_t i = 0; i < 2; ++i)
    {
      // Mapping
      for (uint32_t el = 0; el < 4; ++el)
      {
        NMP::endianSwap(map->jointIndex[el]);
        NMP::endianSwap(map->jointParent[el]);
        NMP::endianSwap(map->jointPartnerBlendFrom[el]);
        NMP::endianSwap(map->jointPartnerBlendTo[el]);
        NMP::endianSwap(map->blendWeights[el]);
        NMP::endianSwap(map->rotationWeights[el]);
        NMP::endianSwap(map->positionWeights[el]);
        NMP::endianSwap(map->valid[el]);
        NMP::endianSwap(map->parentValid[el]);
        NMP::endianSwap(map->blendFromValid[el]);
      }
      NMP::endianSwap(map->childPackage);
      NMP::endianSwap(map->parentPackage);
      NMP::endianSwap(map->endPackage);
      NMP::endianSwap(map->rootPackage);
      NMP::endianSwap(map->hasDirectMapping);
      NMP::endianSwap(map->allUnitPositionWeights);
      NMP::endianSwap(map->allUnitRotationWeights);
      //  Shift to target
      map = &(m_mpTargetMappings[j]);

      // Geometry
      NMP::endianSwap(geom->offset.t);
      NMP::endianSwap(geom->offset.q);
      NMP::endianSwap(geom->reference.t);
      NMP::endianSwap(geom->reference.q);
      NMP::endianSwap(geom->parentReference.t);
      NMP::endianSwap(geom->parentReference.q);
      NMP::endianSwap(geom->validMask);
      //  Shift to target
      geom = &(m_mpTargetGeom[j]);
    }

    //  Other params
    m_mpParams[j].limits.endianSwap();
    NMP::endianSwap(m_mpParams[j].applyLimitsMask);
    NMP::endianSwap(m_mpParams[j].isRigidMask);
    NMP::endianSwap(m_mpParams[j].sourceMirrorMasks);
    NMP::endianSwap(m_mpParams[j].targetMirrorMasks);
    NMP::endianSwap(m_mpParams[j].isLimited);
    NMP::endianSwap(m_mpParams[j].redistributeMask);
    NMP::endianSwap(m_mpParams[j].doRedistribution);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void FKRetarget::Solver::relocate(void* location)
{
  NMP::Memory::Resource memTrack;
  memTrack.format = m_memoryRequirements;
  memTrack.ptr = location;
  NMP_ASSERT(NMP_IS_ALIGNED(memTrack.ptr, memTrack.format.alignment));

  // Shell
  NMP::Memory::Format format(sizeof(FKRetarget::Solver), NMP_NATURAL_TYPE_ALIGNMENT);
  memTrack.align(format);
  memTrack.increment(format);

  // Intermediate rig
  format = NMP::Memory::Format(sizeof(IntermediateJointData), NMP_NATURAL_TYPE_ALIGNMENT);
  format *= m_intermediateRigSize;
  memTrack.align(format);
  m_intermediateRigData = static_cast<IntermediateJointData*>(memTrack.ptr);
  memTrack.increment(format);

  // Multiplex-packaged mappings
  format = NMP::Memory::Format(sizeof(MPIndexMap), NMP_NATURAL_TYPE_ALIGNMENT);
  format *= m_mpMaxLength;
  //  Source mappings
  memTrack.align(format);
  m_mpSourceMappings = static_cast<MPIndexMap*>(memTrack.ptr);
  memTrack.increment(format);
  //  Target mappings
  memTrack.align(format);
  m_mpTargetMappings = static_cast<MPIndexMap*>(memTrack.ptr);
  memTrack.increment(format);

  // Multiplex-packaged geometry
  format = NMP::Memory::Format(sizeof(MPGeometry), NMP_VECTOR_ALIGNMENT);
  //  Add one extra package to store identity matrices for defaults
  format *= m_mpMaxLength + 1;
  //  Source geom
  memTrack.align(format);
  m_mpSourceGeom = static_cast<MPGeometry*>(memTrack.ptr);
  //   We actually want to point to the second member, so the first is index -1
  m_mpSourceGeom++;
  memTrack.increment(format);
  //  Target geom
  memTrack.align(format);
  m_mpTargetGeom = static_cast<MPGeometry*>(memTrack.ptr);
  //   We actually want to point to the second member, so the first is index -1
  m_mpTargetGeom++;
  memTrack.increment(format);

  // Multiplex-packaged params
  format = NMP::Memory::Format(sizeof(MPParams), NMP_VECTOR_ALIGNMENT);
  format *= m_mpMaxLength;
  memTrack.align(format);
  m_mpParams = static_cast<MPParams*>(memTrack.ptr);
  memTrack.increment(format);

  // Check we used all the memory
  NMP_ASSERT(memTrack.format.size == 0);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t FKRetarget::Solver::getNumSourcePackages()
{
  return m_mpSourceLength;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t FKRetarget::Solver::getNumTargetPackages()
{
  return m_mpTargetLength;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t FKRetarget::Solver::calculateMaxNumPackages(const StorageStats& storageStats)
{
  uint32_t maxPerJointPackageSpace = storageStats.mostMappedJoints;
  maxPerJointPackageSpace += 3 * storageStats.longestSequence;
  maxPerJointPackageSpace += 3 * (storageStats.longestSequence - 1);
  uint32_t result = (maxPerJointPackageSpace / 4);
  result += (maxPerJointPackageSpace % 4 == 0) ? 0 : 1;

  // You can precalculate this value and store it with the storage stats, this can save memory because
  // the above calculation is a conservative estimate based on assuming the most inefficient possible
  // multiplex packaging.
  if (storageStats.mostMultiplexPackages != 0)
  {
    // This will sanity check to make certain you won't over-allocate
    return NMP::minimum(storageStats.mostMultiplexPackages, result);
  }
  return result;
}

} // end of namespace NMRU
