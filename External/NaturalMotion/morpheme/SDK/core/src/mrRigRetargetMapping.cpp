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
#include "morpheme/mrRigRetargetMapping.h"
#include "morpheme/mrManager.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// AttribDataRigRetargetMapping functions.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataRigRetargetMapping::getMemoryRequirements(
  uint32_t numJoints)
{
  // Get size of shell
  NMP::Memory::Format result(sizeof(AttribDataRigRetargetMapping), MR_ATTRIB_DATA_ALIGNMENT);

  // Add space for the retarget params object
  result += NMRU::FKRetarget::Params::getMemoryRequirements(numJoints);

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataRigRetargetMapping *AttribDataRigRetargetMapping::init(
  NMP::Memory::Resource &resource,
  uint32_t numJoints,
  uint16_t refCount /* = 0 */)
{
  // Initialise shell
  NMP::Memory::Format format(sizeof(AttribDataRigRetargetMapping), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataRigRetargetMapping *result = (AttribDataRigRetargetMapping*)resource.alignAndIncrement(format);

  // Set default values for base class
  result->setType(ATTRIB_TYPE_RIG_RETARGET_MAPPING);
  result->setRefCount(refCount);

  // Set default values for shell
  result->m_numJoints = numJoints;

  // Initialise the low-level retarget params object
  format = NMRU::FKRetarget::Params::getMemoryRequirements(numJoints);
  resource.align(format);
  NMP::Memory::Resource retargetParamsResource;
  retargetParamsResource.format = format;
  retargetParamsResource.ptr = resource.ptr;
  result->m_retargetParams = NMRU::FKRetarget::Params::init(&retargetParamsResource, numJoints);
  resource.increment(format);
  // Set defaults for retarget params object
  result->m_retargetParams->setDefaults(numJoints);

  // Make sure resource pointer has been moved on by the size returned by getMemoryRequirements.
  format = NMP::Memory::Format(0, MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(format);

  return result;
}

// Hide these from SPU to save space
#ifndef NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AttribDataRigRetargetMapping::locate(AttribData *target)
{
  AttribDataRigRetargetMapping *result = (AttribDataRigRetargetMapping*) target;

  // Locate base object data
  AttribData::locate(target);

  // Locate local data
  NMP::endianSwap(result->m_numJoints);

  // Locate pointers
  NMP::endianSwap(result->m_retargetParams);

  // Refix pointers
  REFIX_PTR_RELATIVE(NMRU::FKRetarget::Params, result->m_retargetParams, result);

  // Locate trailing memory
  result->m_retargetParams->locate(result->m_numJoints);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataRigRetargetMapping::dislocate(AttribData *target)
{
  AttribDataRigRetargetMapping *result = (AttribDataRigRetargetMapping*) target;

  // Dislocate trailing objects
  result->m_retargetParams->dislocate(result->m_numJoints);

  // Unfix pointers
  UNFIX_PTR_RELATIVE(NMRU::FKRetarget::Params, result->m_retargetParams, result);

  // Dislocate pointers
  NMP::endianSwap(result->m_retargetParams);

  // Dislocate local data
  NMP::endianSwap(result->m_numJoints);

  // Dislocate base object data
  AttribData::dislocate(target);
}

#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AttribDataRigRetargetMapping::relocate(AttribData *target, void* location)
{
  AttribDataRigRetargetMapping *result = (AttribDataRigRetargetMapping*) target;

#ifdef NM_HOST_CELL_SPU
  spu_printf("AttribDataRigRetargetMapping::relocate()\n");
#endif

  NMP_ASSERT(NMP_IS_ALIGNED(target, MR_ATTRIB_DATA_ALIGNMENT));
  NMP_ASSERT(NMP_IS_ALIGNED(location, MR_ATTRIB_DATA_ALIGNMENT));

  NMP::Memory::Format memReqs = getMemoryRequirements(result->m_numJoints);
  NMP::Memory::Resource resultRes = {result, memReqs};
  NMP::Memory::Resource locationRes = {location, memReqs};

  // AttribDataRigRetargetMapping class.
  resultRes.increment(sizeof(AttribDataRigRetargetMapping));
  locationRes.increment(sizeof(AttribDataRigRetargetMapping));

  // Retarget params object
  memReqs = NMRU::FKRetarget::Params::getMemoryRequirements(result->m_numJoints);
  NMRU::FKRetarget::Params* retargetParams = (NMRU::FKRetarget::Params*)resultRes.alignAndIncrement(memReqs);
  result->m_retargetParams = (NMRU::FKRetarget::Params*)locationRes.alignAndIncrement(memReqs);
  retargetParams->relocate(result->m_retargetParams, result->m_numJoints);

  // Pad up to a multiple of the alignment requirement.
  resultRes.align(MR_ATTRIB_DATA_ALIGNMENT);
  locationRes.align(MR_ATTRIB_DATA_ALIGNMENT);

  // Check we used all the memory
  NMP_ASSERT(resultRes.format.size == 0);
  NMP_ASSERT(locationRes.format.size == 0);
}


//----------------------------------------------------------------------------------------------------------------------
// AttribDataRetargetStorageStats functions.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataRetargetStorageStats::getMemoryRequirements()
{
  // Get size of shell
  NMP::Memory::Format result(sizeof(AttribDataRetargetStorageStats), MR_ATTRIB_DATA_ALIGNMENT);

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataRetargetStorageStats *AttribDataRetargetStorageStats::init(
  NMP::Memory::Resource &resource,
  uint16_t refCount /* = 0 */)
{
  // Initialise shell
  NMP::Memory::Format format(sizeof(AttribDataRetargetStorageStats), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataRetargetStorageStats *result = (AttribDataRetargetStorageStats*)resource.alignAndIncrement(format);

  // Set default values for base class
  result->setType(ATTRIB_TYPE_RETARGET_STORAGE_STATS);
  result->setRefCount(refCount);

  // Set default values for shell
  result->m_solverStorageStats.largestRigSize = 0;
  result->m_solverStorageStats.intermediateRigSize = 0;
  result->m_solverStorageStats.mostMappedJoints = 0;
  result->m_solverStorageStats.longestSequence = 1;
  result->m_solverStorageStats.mostMultiplexPackages = 0;

  // Make sure resource pointer has been moved on by the size returned by getMemoryRequirements.
  format = NMP::Memory::Format(0, MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(format);

  return result;
}

// Hide these from SPU to save space
#ifndef NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AttribDataRetargetStorageStats::locate(AttribData *target)
{
  AttribDataRetargetStorageStats *result = (AttribDataRetargetStorageStats*) target;

  // Locate base object data
  AttribData::locate(target);

  // Locate local data
  NMP::endianSwap(result->m_solverStorageStats);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataRetargetStorageStats::dislocate(AttribData *target)
{
  AttribDataRetargetStorageStats *result = (AttribDataRetargetStorageStats*) target;

  // Dislocate local data
  NMP::endianSwap(result->m_solverStorageStats);

  // Dislocate base object data
  AttribData::dislocate(target);
}

#endif // NM_HOST_CELL_SPU


//----------------------------------------------------------------------------------------------------------------------
// AttribDataRetargetState functions.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataRetargetState::getMemoryRequirements(
  const NMRU::FKRetarget::Solver::StorageStats solverStorageStats)
{
  // Get size of shell
  NMP::Memory::Format result(sizeof(AttribDataRetargetState), MR_ATTRIB_DATA_ALIGNMENT);

  // Add space for the retarget solver object
  result += NMRU::FKRetarget::Solver::getMemoryRequirements(solverStorageStats);

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataRetargetState *AttribDataRetargetState::init(
  NMP::Memory::Resource &resource,
  const NMRU::FKRetarget::Solver::StorageStats solverStorageStats,
  uint16_t refCount /* = 0 */)
{
  // Initialise shell
  NMP::Memory::Format format(sizeof(AttribDataRetargetState), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataRetargetState* result = (AttribDataRetargetState*)resource.alignAndIncrement(format);

  // Set default values for base class
  result->setType(ATTRIB_TYPE_RETARGET_STATE);
  result->setRefCount(refCount);

  // Set default values for shell
  result->m_sourceAnimSetIndex = 0;
  result->m_targetAnimSetIndex = 0;

  // Initialise the low-level retarget params object
  format = NMRU::FKRetarget::Solver::getMemoryRequirements(solverStorageStats);
  resource.align(format);
  NMP::Memory::Resource retargetSolverResource;
  retargetSolverResource.format = format;
  retargetSolverResource.ptr = resource.ptr;
  result->m_retargetSolver = NMRU::FKRetarget::Solver::init(
    &retargetSolverResource, solverStorageStats);
  resource.increment(format);

  // Make sure resource pointer has been moved on by the size returned by getMemoryRequirements.
  format = NMP::Memory::Format(0, MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(format);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataRetargetState::create(
  NMP::MemoryAllocator *allocator,
  const NMRU::FKRetarget::Solver::StorageStats solverStorageStats,
  uint16_t refCount /* = 0 */)
{
  AttribDataHandle result;

  NMP::Memory::Format memReqs = getMemoryRequirements(solverStorageStats);
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  result.m_attribData = init(resource, solverStorageStats, refCount);
  result.m_format = memReqs;

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

// Hide these from SPU to save space
#ifndef NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AttribDataRetargetState::locate(AttribData *target)
{
  AttribDataRetargetState *result = (AttribDataRetargetState*) target;

  // Locate base object data
  AttribData::locate(target);

  // Locate local data
  NMP::endianSwap(result->m_sourceAnimSetIndex);
  NMP::endianSwap(result->m_targetAnimSetIndex);

  // Locate pointers
  NMP::endianSwap(result->m_retargetSolver);

  // Refix pointers
  REFIX_PTR_RELATIVE(NMRU::FKRetarget::Solver, result->m_retargetSolver, result);

  // Locate trailing memory
  result->m_retargetSolver->locate();
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataRetargetState::dislocate(AttribData *target)
{
  AttribDataRetargetState *result = (AttribDataRetargetState*) target;

  // Dislocate trailing objects
  result->m_retargetSolver->dislocate();

  // Unfix pointers
  UNFIX_PTR_RELATIVE(NMRU::FKRetarget::Solver, result->m_retargetSolver, result);

  // Dislocate pointers
  NMP::endianSwap(result->m_retargetSolver);

  // Dislocate local data
  NMP::endianSwap(result->m_sourceAnimSetIndex);
  NMP::endianSwap(result->m_targetAnimSetIndex);

  // Dislocate base object data
  AttribData::dislocate(target);
}

#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AttribDataRetargetState::relocate(AttribData *target, void* location)
{
  AttribDataRetargetState *result = (AttribDataRetargetState*) target;
  NMP_ASSERT(NMP_IS_ALIGNED(target, MR_ATTRIB_DATA_ALIGNMENT));
  NMP_ASSERT(NMP_IS_ALIGNED(location, MR_ATTRIB_DATA_ALIGNMENT));
  size_t offset = NMP::Memory::align(sizeof(AttribDataRetargetState), MR_TRANSFORM_ATTRIB_ALIGNMENT);

  // Relocate solver object
  NMRU::FKRetarget::Solver* retargetSolver = (NMRU::FKRetarget::Solver*)(((size_t)result) + offset);
  result->m_retargetSolver = (NMRU::FKRetarget::Solver*)(((size_t)location) + offset);
  NMP::Memory::Format format = retargetSolver->getInstanceMemoryRequirements();
  retargetSolver->relocate(result->m_retargetSolver);
  offset += NMP::Memory::align(format.size, format.alignment);
}


//----------------------------------------------------------------------------------------------------------------------
// AttribDataScaleCharacterState functions.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataScaleCharacterState::getMemoryRequirements(
  uint32_t numJoints)
{
  // Get size of shell
  NMP::Memory::Format result(sizeof(AttribDataScaleCharacterState), MR_ATTRIB_DATA_ALIGNMENT);

  // Add space for the joint scales array
  result += NMP::Memory::Format(sizeof(float) * numJoints, NMP_NATURAL_TYPE_ALIGNMENT);

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataScaleCharacterState *AttribDataScaleCharacterState::init(
  NMP::Memory::Resource &resource,
  uint32_t numJoints,
  uint16_t refCount /* = 0 */)
{
  // Initialise shell
  NMP::Memory::Format format(sizeof(AttribDataScaleCharacterState), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataScaleCharacterState* result = (AttribDataScaleCharacterState*)resource.alignAndIncrement(format);

  // Set default values for base class
  result->setType(ATTRIB_TYPE_SCALECHARACTER_STATE);
  result->setRefCount(refCount);

  // Set default values for shell
  result->m_numJoints = numJoints;

  // Initialise the joint scales array
  format.set(sizeof(float) * numJoints, NMP_NATURAL_TYPE_ALIGNMENT);
  resource.align(format);
  NMP::Memory::Resource jointScaleResource;
  jointScaleResource.format = format;
  jointScaleResource.ptr = resource.ptr;
  result->m_jointScale = (float*)jointScaleResource.ptr;
  //   Set to sensible defaults
  for (uint32_t j = 0; j < numJoints; ++j)
  {
    result->m_jointScale[j] = 1.0f;
  }
  resource.increment(format);

  // Make sure resource pointer has been moved on by the size returned by getMemoryRequirements.
  format = NMP::Memory::Format(0, MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(format);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataScaleCharacterState::create(
  NMP::MemoryAllocator* allocator,
  AttribDataCreateDesc* desc,
  uint16_t refCount)
{
  AttribDataHandle result;
  uint32_t numJoints = ((AttribDataArrayCreateDesc*)desc)->m_numEntries;

  NMP::Memory::Format memReqs = getMemoryRequirements(numJoints);
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  result.m_attribData = init(resource, numJoints, refCount);
  result.m_format = memReqs;

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

// Hide these from SPU to save space
#ifndef NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AttribDataScaleCharacterState::locate(AttribData *target)
{
  AttribDataScaleCharacterState* result = (AttribDataScaleCharacterState*)target;

  // Locate base object data
  AttribData::locate(target);

  // Locate local data
  NMP::endianSwap(result->m_numJoints);

  // Locate and refix pointers
  REFIX_SWAP_PTR_RELATIVE(float, result->m_jointScale, result);

  // Locate trailing memory
  for (uint32_t j = 0; j < result->m_numJoints; ++j)
  {
    NMP::endianSwap(result->m_jointScale[j]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataScaleCharacterState::dislocate(AttribData *target)
{
  AttribDataScaleCharacterState* result = (AttribDataScaleCharacterState*)target;

  // Dislocate trailing objects
  for (uint32_t j = 0; j < result->m_numJoints; ++j)
  {
    NMP::endianSwap(result->m_jointScale[j]);
  }

  // Unfix and dislocate pointers
  UNFIX_SWAP_PTR_RELATIVE(float, result->m_jointScale, result);

  // Dislocate local data
  NMP::endianSwap(result->m_numJoints);

  // Dislocate base object data
  AttribData::dislocate(target);
}

#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AttribDataScaleCharacterState::relocate(AttribData *target, void* location)
{
  AttribDataScaleCharacterState* result = (AttribDataScaleCharacterState*)target;

  // Skip shell
  NMP::Memory::Format format(sizeof(AttribDataScaleCharacterState), NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::Resource memRes;
  memRes.format = format;
  memRes.ptr = location;
  memRes.alignAndIncrement(format);

  // Relocate joint scale array
  result->m_jointScale = (float*)memRes.ptr;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
