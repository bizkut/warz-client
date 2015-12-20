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
#include "morpheme/AnimSource/mrAnimSource.h"
#include "morpheme/mrManager.h"

#ifdef NM_HOST_CELL_SPU
  #include "NMPlatform/ps3/NMSPUDMAController.h"
#endif // NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// AnimSourceBase functions
//----------------------------------------------------------------------------------------------------------------------
void AnimSourceBase::animComputeAtTime(
  float                 time,
  const AnimRigDef*     rig,
  const RigToAnimMap*   mapFromRigToAnim,
  uint32_t              outputSubsetSize,
  const uint16_t*       outputSubsetArray,
  NMP::DataBuffer*      outputTransformBuffer,
  NMP::MemoryAllocator* allocator) const
{
  NMP_ASSERT(m_funcTable);

  return m_funcTable->animComputeAtTime(
           this,
           time,
           rig,
           mapFromRigToAnim,
           outputSubsetSize,
           outputSubsetArray,
           outputTransformBuffer,
           allocator);
}

//----------------------------------------------------------------------------------------------------------------------
float AnimSourceBase::animGetDuration() const
{
  NMP_ASSERT(m_funcTable);

  return m_funcTable->animGetDuration(this);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AnimSourceBase::animGetNumChannelSets() const
{
  NMP_ASSERT(m_funcTable);

  return m_funcTable->animGetNumChannelSets(this);
}

//----------------------------------------------------------------------------------------------------------------------
const TrajectorySourceBase* AnimSourceBase::animGetTrajectorySourceData() const
{
  NMP_ASSERT(m_funcTable);

  return m_funcTable->animGetTrajectorySourceData(this);
}

//----------------------------------------------------------------------------------------------------------------------
const NMP::OrderedStringTable* AnimSourceBase::animGetChannelNameTable() const
{
  NMP_ASSERT(m_funcTable);

  return m_funcTable->animGetChannelNameTable(this);
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AnimSourceBase::locate()
{
  NMP_ASSERT_MSG(!m_isLocated, "Animation already located!");
  NMP::endianSwap(m_animType);
  NMP::endianSwap(m_animInstanceMemReqs);
  m_isLocated = true;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceBase::dislocate()
{
  NMP::endianSwap(m_animType);
  NMP::endianSwap(m_animInstanceMemReqs);
  // the asset compiler calls a single dislocate for the asset to safe it on disk.
  // this means m_isLocated gets initialized during asset compilation by dislocate
  m_isLocated = false;

  m_funcTable = NULL;
}

#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
// AttribDataSourceAnim functions
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataSourceAnim::locate(AttribData* target)
{
  AttribDataSourceAnim* result = (AttribDataSourceAnim*)target;

  AttribData::locate(result);

  NMP::endianSwap(result->m_registeredAnimFormatIndex);
  uint8_t formatIndex;
  #ifdef NMP_ENABLE_ASSERTS
  bool success =
  #endif
    Manager::getInstance().findAnimationFormatRegistryEntryIndex(result->m_registeredAnimFormatIndex, formatIndex);
  NMP_ASSERT(success);
  result->m_registeredAnimFormatIndex = formatIndex;

  // Note: We don't endian swap or unfix the animation ptr as it is currently rebound by the application at runtime.
  result->m_anim = NULL;
  NMP::endianSwap(result->m_animAssetID);

  // Rig to animation map.
  result->m_rigToAnimMap = NULL;
  NMP::endianSwap(result->m_rigToAnimMapAssetID);
  NMP::endianSwap(result->m_rigToAnimMapSize);

  NMP::endianSwap(result->m_startSyncEventIndex);

  NMP::endianSwap(result->m_clipStartSyncEventIndex);
  NMP::endianSwap(result->m_clipEndSyncEventIndex);

  NMP::endianSwap(result->m_clipStartFraction);
  NMP::endianSwap(result->m_clipEndFraction);
  NMP::endianSwap(result->m_sourceAnimDuration);

  NMP::endianSwap(result->m_transformAtStartPos);
  NMP::endianSwap(result->m_transformAtEndPos);
  NMP::endianSwap(result->m_transformAtStartQuat);
  NMP::endianSwap(result->m_transformAtEndQuat);

  NMP::endianSwap(result->m_syncEventTrackIndex);

  NMP::endianSwap(result->m_playBackwards);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataSourceAnim::dislocate(AttribData* target)
{
  AttribDataSourceAnim* result = (AttribDataSourceAnim*)target;

  AttribData::dislocate(result);

  result->m_registeredAnimFormatIndex =
    Manager::getInstance().getAnimationFormatRegistryEntry(result->m_registeredAnimFormatIndex)->m_animType;
  NMP::endianSwap(result->m_registeredAnimFormatIndex);

  // NB: We don't endian swap or unfix the animation ptr as it is currently
  //  rebound by the application at runtime.
  NMP::endianSwap(result->m_animAssetID);

  NMP::endianSwap(result->m_rigToAnimMapAssetID);
  NMP::endianSwap(result->m_rigToAnimMapSize);

  NMP::endianSwap(result->m_startSyncEventIndex);

  NMP::endianSwap(result->m_clipStartFraction);
  NMP::endianSwap(result->m_clipEndFraction);
  NMP::endianSwap(result->m_sourceAnimDuration);

  NMP::endianSwap(result->m_transformAtStartPos);
  NMP::endianSwap(result->m_transformAtEndPos);
  NMP::endianSwap(result->m_transformAtStartQuat);
  NMP::endianSwap(result->m_transformAtEndQuat);

  NMP::endianSwap(result->m_syncEventTrackIndex);

  NMP::endianSwap(result->m_playBackwards);
}
#endif //NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataSourceAnim::getMemoryRequirements()
{
  return NMP::Memory::Format(NMP::Memory::align(sizeof(AttribDataSourceAnim), MR_ATTRIB_DATA_ALIGNMENT), MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataSourceAnim* AttribDataSourceAnim::init(
  NMP::Memory::Resource&      resource,
  uint8_t                     registeredAnimFormatIndex,
  RuntimeAnimAssetID          animAssetID,
  RuntimeAssetID              rigToAnimMapAssetID,
  const NMP::Memory::Format&  rigToAnimMapMemoryFormat,
  uint16_t                    refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);

  // Placement new the resource to ensure the base constructor is called.
  NMP::Memory::Format memReq = getMemoryRequirements();
  AttribDataSourceAnim* result = (AttribDataSourceAnim*)resource.alignAndIncrement(memReq);

  result->setRefCount(refCount);
  result->setType(ATTRIB_TYPE_SOURCE_ANIM);
  result->m_anim = NULL;
  result->m_registeredAnimFormatIndex = registeredAnimFormatIndex;
  result->m_animAssetID = animAssetID;
  result->m_rigToAnimMap = NULL;
  result->m_rigToAnimMapAssetID = rigToAnimMapAssetID;
  result->m_rigToAnimMapSize = (uint32_t)rigToAnimMapMemoryFormat.size;

  return result;
}

#ifndef NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
bool AttribDataSourceAnim::setAnimation(AnimSourceBase* anim)
{
  // Animation.
  m_anim = anim;

  if (m_anim)
  {
    NMP_ASSERT(m_anim->isLocated());
    // Store the memory requirements of the referenced source animation.
    NMP::Memory::Format memReqs = m_anim->getInstanceMemoryRequirements();
    m_animSize = (uint32_t)memReqs.size;
    NMP_ASSERT((memReqs.size > 0) && (memReqs.alignment > 0) && (memReqs.alignment <= NMP_VECTOR_ALIGNMENT));
  }

  return true;
}

#else // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AttribDataSourceAnim::prepAnimForSPU(AttribData* target, NMP::MemoryAllocator* allocator)
{
  AttribDataSourceAnim* result = (AttribDataSourceAnim*)target;

  // Allocate local memory to store the referenced anim in.
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, NMP::Memory::Format(result->m_animSize, NMP_VECTOR_ALIGNMENT));
  NMP_ASSERT(resource.ptr);

  // DMA anim from system mem to locally allocated result->m_anim.
  NMP::SPUDMAController::dmaGet(resource.ptr, (uint32_t)result->m_anim, result->m_animSize);
  NMP::SPUDMAController::dmaWaitAll();

  // Set the new anim pointer.
  result->m_anim = (AnimSourceBase*)resource.ptr;
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataSourceAnim::prepRigToAnimMapForSPU(AttribData* target, NMP::MemoryAllocator* allocator)
{
  AttribDataSourceAnim* result = (AttribDataSourceAnim*)target;

  // Allocate local memory to store the referenced rig to anim map in.
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, NMP::Memory::Format(result->m_rigToAnimMapSize, MR_ATTRIB_DATA_ALIGNMENT));
  NMP_ASSERT(resource.ptr);

  // DMA rig from system mem to locally allocated result->m_rig.
  NMP::SPUDMAController::dmaGet(resource.ptr, (uint32_t)result->m_rigToAnimMap, result->m_rigToAnimMapSize);
  NMP::SPUDMAController::dmaWaitAll();

  // Set the new rig to anim map pointer.
  result->m_rigToAnimMap = (RigToAnimMap*)resource.ptr;

  // Sort out all internal pointers in the rig to anim map.
  result->m_rigToAnimMap->relocate();
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataSourceAnim::prepTrajectoryForSPU(AttribData* target, NMP::MemoryAllocator* allocator)
{
  AttribDataSourceAnim* result = (AttribDataSourceAnim*)target;

  // Allocate local memory to store the referenced anim in.
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, NMP::Memory::Format(result->m_trajectorySize, NMP_VECTOR_ALIGNMENT));
  NMP_ASSERT(resource.ptr);

  // DMA anim from system mem to locally allocated result->m_anim.
  NMP::SPUDMAController::dmaGet(resource.ptr, (uint32_t)result->m_sourceTrajectoryChannel, result->m_trajectorySize);
  NMP::SPUDMAController::dmaWaitAll();

  // Set the new trajectory channel pointer.
  result->m_sourceTrajectoryChannel = (TrajectorySourceBase*)resource.ptr;
}

#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
bool AttribDataSourceAnim::fixupRigToAnimMap()
{
  // Rig.
  m_rigToAnimMap = (RigToAnimMap*) MR::Manager::getInstance().getObjectPtrFromObjectID(m_rigToAnimMapAssetID);
  NMP_ASSERT(m_rigToAnimMap);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttribDataSourceAnim::unfixRigToAnimMap()
{
  m_rigToAnimMap = NULL;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataSourceAnim::setTrajectorySource(const TrajectorySourceBase* trajectorySource)
{
  m_sourceTrajectoryChannel = trajectorySource;
  if (trajectorySource)
  {
    // Store the memory requirements of the referenced source trajectory.
    NMP::Memory::Format memReqs = trajectorySource->getInstanceMemoryRequirements();
    m_trajectorySize = (uint32_t)memReqs.size;
    NMP_ASSERT((memReqs.size > 0) && (memReqs.alignment > 0) && (memReqs.alignment <= NMP_VECTOR_ALIGNMENT));
  }
  else // I don't think this is used if m_sourceTrajectoryChannel == NULL.
  {
    m_trajectorySize = 0;
  }
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
