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
#include "morpheme/mrRigToAnimMap.h"
#include "morpheme/mrManager.h"

#ifdef NM_HOST_CELL_SPU
  #include "NMPlatform/ps3/NMSPUDMAController.h"
#endif // NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// MapPair
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void MapPair::locate()
{
  NMP::endianSwap(m_rigChannelIndex);
  NMP::endianSwap(m_animChannelIndex);
}

//----------------------------------------------------------------------------------------------------------------------
void MapPair::dislocate()
{
  NMP::endianSwap(m_rigChannelIndex);
  NMP::endianSwap(m_animChannelIndex);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
// RigToAnimEntryMap
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format RigToAnimEntryMap::getMemoryRequirements(uint32_t numEntries)
{
  // Header
  NMP::Memory::Format result(sizeof(RigToAnimEntryMap), NMP_NATURAL_TYPE_ALIGNMENT);

  // Rig to anim map table
  NMP::Memory::Format memReqsRAMTable(sizeof(MapPair) * numEntries, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsRAMTable;
  
  // Multiple of the alignment
  result.size = NMP::Memory::align(result.size, NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
RigToAnimEntryMap* RigToAnimEntryMap::init(NMP::Memory::Resource& resource, uint32_t numEntries)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(RigToAnimEntryMap), NMP_NATURAL_TYPE_ALIGNMENT);
  RigToAnimEntryMap* result = (RigToAnimEntryMap*) resource.alignAndIncrement(memReqsHdr);

  result->m_numEntries = numEntries;

  // Rig to anim map table
  NMP::Memory::Format memReqsTable(sizeof(MapPair) * numEntries, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_entries = (MapPair*) resource.alignAndIncrement(memReqsTable);
  for(uint32_t i = 0; i < numEntries; ++i)
    result->m_entries[i].clear();

  // Multiple of the alignment
  resource.align(NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
RigToAnimEntryMap* RigToAnimEntryMap::relocate(void*& ptr)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(RigToAnimEntryMap), NMP_NATURAL_TYPE_ALIGNMENT);
  RigToAnimEntryMap* result = (RigToAnimEntryMap*) NMP::Memory::alignAndIncrement(ptr, memReqsHdr);
  
  // Rig to anim map table
  NMP::Memory::Format memReqsTable(sizeof(MapPair) * result->m_numEntries, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_entries = (MapPair*) NMP::Memory::alignAndIncrement(ptr, memReqsTable);
  
  return result;
}

#ifndef NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
void RigToAnimEntryMap::locate()
{
  // Header
  NMP::endianSwap(m_numEntries);

  // Rig to anim map table
  REFIX_SWAP_PTR(MapPair, m_entries);
  for (uint32_t i = 0; i < m_numEntries; ++i)
    m_entries[i].locate();
}

//----------------------------------------------------------------------------------------------------------------------
void RigToAnimEntryMap::dislocate()
{
  // Rig to anim map table
  for (uint32_t i = 0; i < m_numEntries; ++i)
    m_entries[i].dislocate();
  UNFIX_SWAP_PTR(MapPair, m_entries);

  // Header
  NMP::endianSwap(m_numEntries);
}
#endif // NM_HOST_CELL_SPU

#ifndef NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
bool RigToAnimEntryMap::isEqual(const MR::RigToAnimEntryMap* compare) const
{
  NMP_ASSERT(compare);
  // Check the number of entries for the rig to anim channel maps
  if (compare->m_numEntries != m_numEntries)
    return false;

  // Check that the contents of the rig to anim map table are identical
  if (memcmp(compare->m_entries, m_entries, sizeof(MapPair) * m_numEntries) != 0)
    return false;

  return true;
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
bool RigToAnimEntryMap::findEntryIndexForRigChannelIndex(const uint16_t rigChannelIndex, uint32_t& entryIndex) const
{
  NMP_ASSERT(m_numEntries && m_entries);
  int32_t startIndex;
  if (rigChannelIndex < m_numEntries)
    startIndex = (uint32_t) rigChannelIndex;
  else
    startIndex = m_numEntries - 1;

  if (m_entries[startIndex].m_rigChannelIndex == rigChannelIndex)
  {
    entryIndex = startIndex;
    return true;
  }

  if (m_entries[startIndex].m_rigChannelIndex < rigChannelIndex)
  {
    // Search up array
    for (int32_t i = startIndex + 1; i < (int32_t) m_numEntries; ++i)
    {
      uint16_t key = m_entries[i].m_rigChannelIndex;
      if (key == rigChannelIndex)
      {
        entryIndex = i;
        return true;
      }
      else if (key > rigChannelIndex)
      {
        return false;
      }
    }
  }
  else
  {
    // Search down array
    for (int32_t i = startIndex - 1; i >= 0; i--)
    {
      uint16_t key = m_entries[i].m_rigChannelIndex;
      if (key == rigChannelIndex)
      {
        entryIndex = i;
        return true;
      }
      else if (key < rigChannelIndex)
      {
        return false;
      }
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool RigToAnimEntryMap::findEntryIndexForAnimChannelIndex(const uint16_t animChannelIndex, uint32_t& entryIndex) const
{
  int32_t startIndex;

  NMP_ASSERT(m_numEntries && m_entries);
  if (animChannelIndex < m_numEntries)
    startIndex = (uint32_t) animChannelIndex;
  else
    startIndex = m_numEntries;

  if (m_entries[startIndex].m_animChannelIndex == animChannelIndex)
  {
    entryIndex = startIndex;
    return true;
  }

  if (m_entries[startIndex].m_animChannelIndex < animChannelIndex)
  {
    // Search up array
    for (int32_t i = startIndex + 1; i < (int32_t) m_numEntries; ++i)
    {
      if (m_entries[i].m_animChannelIndex == animChannelIndex)
      {
        entryIndex = i;
        return true;
      }
    }
    // Search down array
    for (int32_t i = startIndex - 1; i >= 0; i--)
    {
      if (m_entries[i].m_animChannelIndex == animChannelIndex)
      {
        entryIndex = i;
        return true;
      }
    }
  }
  else
  {
    // Search down array
    for (int32_t i = startIndex - 1; i >= 0; i--)
    {
      if (m_entries[i].m_animChannelIndex == animChannelIndex)
      {
        entryIndex = i;
        return true;
      }
    }
    // Search up array
    for (int32_t i = startIndex + 1; i < (int32_t) m_numEntries; ++i)
    {
      if (m_entries[i].m_animChannelIndex == animChannelIndex)
      {
        entryIndex = i;
        return true;
      }
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
// Look up bone in the rig and find the index that it maps to in the animation.
// Returns -1 if the rig bone is not found or does not have an equivalent entry in the animation.
int16_t RigToAnimEntryMap::getAnimChannelIndexFromRigBoneName(const char* rigBoneName, const AnimRigDef* rig) const
{
  uint32_t rigBoneIndex;
  uint16_t animChannelIndex = 0;

  NMP_ASSERT(rigBoneName && rig);

  rigBoneIndex = rig->getBoneIndexFromName(rigBoneName);
  if (rigBoneIndex == AnimRigDef::BONE_NAME_NOT_FOUND)
    return -1;

  if (getAnimIndexForRigIndex((uint16_t) rigBoneIndex, animChannelIndex))
    return animChannelIndex;

  return -1; // No anim channel exists for this rig bone.
}

#ifndef NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
void RigToAnimEntryMap::sortByRigChannels()
{
  for (uint32_t i = 0; i < m_numEntries - 1; ++i)
  {
    for (uint32_t j = i + 1; j < m_numEntries; ++j)
    {
      if (m_entries[j].m_rigChannelIndex < m_entries[i].m_rigChannelIndex)
      {
        NMP::nmSwap(m_entries[i].m_rigChannelIndex, m_entries[j].m_rigChannelIndex);
        NMP::nmSwap(m_entries[i].m_animChannelIndex, m_entries[j].m_animChannelIndex);
      }
    }
  }
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
// AnimToRigTableMap
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnimToRigTableMap::getMemoryRequirements(uint32_t numAnimChannels)
{
  // Header
  NMP::Memory::Format result(sizeof(AnimToRigTableMap), NMP_NATURAL_TYPE_ALIGNMENT);

  // Anim to rig map:
  // An extra entry is allocated to allow for faster animation decompression.
  //
  // - Invalid rig channels (i.e. padding) are mapped to output buffer channel 0,
  //   since this is overwritten at the end of decompression.
  //
  // - The m_rigIndices array is offset by 1 position to accommodate a -1 entry, so that invalid
  //   anim channel indices (-1) can be handled simply without the need for expensive checks.
  //
  // See Also:
  // MR::CompToAnimChannelMap::getMemoryRequirements,
  // MR::CompToAnimChannelMap::init,
  // MR::SectionDataNSA::getMemoryReqsMeanAndSetVec3.
  NMP::Memory::Format memReqsTable(sizeof(uint16_t) * (numAnimChannels + 1), NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsTable;

  // Multiple of the alignment
  result.size = NMP::Memory::align(result.size, NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnimToRigTableMap* AnimToRigTableMap::init(NMP::Memory::Resource& resource, uint32_t numAnimChannels)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(AnimToRigTableMap), NMP_NATURAL_TYPE_ALIGNMENT);
  AnimToRigTableMap* result = (AnimToRigTableMap*) resource.alignAndIncrement(memReqsHdr);
  
  result->m_numAnimChannels = numAnimChannels;
  result->m_numUsedEntries = 0;
  result->m_numAnimChannelsForLOD = 0;

  // Anim to rig map
  // An extra entry is allocated to allow faster animation decompression.
  // See getMemoryRequirements()
  NMP::Memory::Format memReqsTable(sizeof(uint16_t) * (numAnimChannels + 1), NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_animToRigEntries = ((uint16_t*) resource.alignAndIncrement(memReqsTable)) + 1;

  // By default empty entries point to rig bone number 0, which is unused.
  result->m_animToRigEntries[-1] = 0;
  for(uint32_t i = 0; i < numAnimChannels; ++i)
    result->m_animToRigEntries[i] = 0;

  // Multiple of the alignment
  resource.align(NMP_NATURAL_TYPE_ALIGNMENT);
  
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnimToRigTableMap* AnimToRigTableMap::relocate(void*& ptr)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(AnimToRigTableMap), NMP_NATURAL_TYPE_ALIGNMENT);
  AnimToRigTableMap* result = (AnimToRigTableMap*) NMP::Memory::alignAndIncrement(ptr, memReqsHdr);
  
  // Anim to rig map
  // An extra entry is allocated to allow faster animation decompression.
  // See getMemoryRequirements()
  NMP::Memory::Format memReqsTable(sizeof(uint16_t) * (result->m_numAnimChannels + 1), NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_animToRigEntries = ((uint16_t*) NMP::Memory::alignAndIncrement(ptr, memReqsTable)) + 1;
  
  // Multiple of the alignment
  ptr = (void*) NMP::Memory::align(ptr, NMP_NATURAL_TYPE_ALIGNMENT);
  
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimToRigTableMap::locate()
{
  // Header
  NMP::endianSwap(m_numAnimChannels);
  NMP::endianSwap(m_numUsedEntries);
  NMP::endianSwap(m_numAnimChannelsForLOD);

  // Anim to rig map
  REFIX_SWAP_PTR(uint16_t, m_animToRigEntries);
  // The m_rigIndices is offset by 1 position, to have a -1 entry.
  // Check MR::RigToAnimMap::getMemoryRequirements and MR::RigToAnimMap::init.
  NMP::endianSwapArray(m_animToRigEntries - 1, m_numAnimChannels + 1);

}

//----------------------------------------------------------------------------------------------------------------------
void AnimToRigTableMap::dislocate()
{
  // Anim to rig map
  // The m_rigIndices is offset by 1 position, to have a -1 entry.
  // Check MR::RigToAnimMap::getMemoryRequirements and MR::RigToAnimMap::init.
  NMP::endianSwapArray(m_animToRigEntries - 1, m_numAnimChannels + 1);
  UNFIX_SWAP_PTR(uint16_t, m_animToRigEntries);

  // Header
  NMP::endianSwap(m_numAnimChannelsForLOD);
  NMP::endianSwap(m_numUsedEntries);
  NMP::endianSwap(m_numAnimChannels);
}

#ifndef NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
bool AnimToRigTableMap::isEqual(const MR::AnimToRigTableMap* compare) const
{
  NMP_ASSERT(compare);
  // Check the number of entries for the rig to anim channel maps
  if (compare->m_numAnimChannels != m_numAnimChannels)
    return false;

  // Check that the contents of the rig to anim map table are identical. The -1 entry is always 0.
  if (memcmp(compare->m_animToRigEntries, m_animToRigEntries, sizeof(uint16_t) * m_numAnimChannels) != 0)
    return false;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimToRigTableMap::setEntryInfo()
{
  m_numUsedEntries = 0;
  m_numAnimChannelsForLOD = 0;
  for (uint32_t i = 0; i < m_numAnimChannels; ++i)
  {
    if (m_animToRigEntries[i] != 0)
    {
      m_numUsedEntries++;
      m_numAnimChannelsForLOD = (uint16_t)(i + 1);
    }
  }
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
// RigToAnimMap
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format RigToAnimMap::getMemoryRequirements(
  uint32_t numRigBones,
  const NMP::Memory::Format& memReqsRigToAnimMapData)
{
  // Header
  NMP::Memory::Format result(sizeof(RigToAnimMap), MR_ATTRIB_DATA_ALIGNMENT);

  // Channel used flags
  NMP::Memory::Format memReqsUsedFlags = NMP::BitArray::getMemoryRequirements(numRigBones);
  result += memReqsUsedFlags;

  // Rig to anim map data
  result += memReqsRigToAnimMapData;

  // Ensure a multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
RigToAnimMap* RigToAnimMap::init(
  NMP::Memory::Resource& resource,
  RigToAnimMapType rigToAnimMapType,
  uint32_t numRigBones,
  const NMP::Memory::Format& memReqsRigToAnimMapData)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(RigToAnimMap), MR_ATTRIB_DATA_ALIGNMENT);
  RigToAnimMap *result = (RigToAnimMap*) resource.alignAndIncrement(memReqsHdr);

  result->m_rigToAnimMapType = rigToAnimMapType;

  // Channel used flags
  result->m_usedFlags = NMP::BitArray::init(resource, numRigBones);

  // Rig to anim map data
  result->m_memReqsRigToAnimMapData = memReqsRigToAnimMapData;
  result->m_rigToAnimMapData = resource.alignAndIncrement(memReqsRigToAnimMapData);

  // Ensure a multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}


#ifndef NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
void RigToAnimMap::locate()
{
  // Header
  NMP::endianSwap(m_rigToAnimMapType);

  // Rig channel used flags
  REFIX_SWAP_PTR(NMP::BitArray, m_usedFlags);
  m_usedFlags->locate();
  
  // Rig to anim map data
  REFIX_SWAP_PTR(void, m_rigToAnimMapData);
  switch(m_rigToAnimMapType)
  {
  case MapPairs:
    ((RigToAnimEntryMap*)m_rigToAnimMapData)->locate();
    break;
  
  case AnimToRig:
    ((AnimToRigTableMap*)m_rigToAnimMapData)->locate();
    break;
   
  default:
    NMP_ASSERT_FAIL(); // Unknown rig to anim map type
  }
}

//----------------------------------------------------------------------------------------------------------------------
void RigToAnimMap::dislocate()
{
  // Rig to anim map data
  switch(m_rigToAnimMapType)
  {
  case MapPairs:
    ((RigToAnimEntryMap*)m_rigToAnimMapData)->dislocate();
    break;

  case AnimToRig:
    ((AnimToRigTableMap*)m_rigToAnimMapData)->dislocate();
    break;

  default:
    NMP_ASSERT_FAIL(); // Unknown rig to anim map type
  }
  UNFIX_SWAP_PTR(void, m_rigToAnimMapData);

  // Rig channel used flags
  m_usedFlags->dislocate();
  UNFIX_SWAP_PTR(NMP::BitArray, m_usedFlags);

  // Header
  NMP::endianSwap(m_rigToAnimMapType);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void RigToAnimMap::relocate()
{
  // Verify alignment
  NMP_ASSERT_MSG(
    NMP_IS_ALIGNED(this, MR_ATTRIB_DATA_ALIGNMENT),
    "Rig to anim maps must be aligned to %d bytes.",
    MR_ATTRIB_DATA_ALIGNMENT);
  
  // Header
  void* ptr = this;
  NMP::Memory::Format memReqsHdr(sizeof(RigToAnimMap), MR_ATTRIB_DATA_ALIGNMENT);
  NMP::Memory::alignAndIncrement(ptr, memReqsHdr);
  
  // Channel used flags
  m_usedFlags = NMP::BitArray::relocate(ptr);

  // Rig to anim map data
  switch(m_rigToAnimMapType)
  {
  case MapPairs:
    m_rigToAnimMapData = (void*) RigToAnimEntryMap::relocate(ptr);
    break;

  case AnimToRig:
    m_rigToAnimMapData = (void*) AnimToRigTableMap::relocate(ptr);
    break;

  default:
    NMP_ASSERT_FAIL(); // Unknown rig to anim map type
  }
}

#ifndef NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
bool RigToAnimMap::isEqual(const MR::RigToAnimMap* compare) const
{
  NMP_ASSERT(compare);
  if (compare->m_rigToAnimMapType != m_rigToAnimMapType)
    return false;

  if (compare->getNumRigBones() != getNumRigBones())
    return false;

  switch(m_rigToAnimMapType)
  {
  case MapPairs:
    return ((RigToAnimEntryMap*)m_rigToAnimMapData)->isEqual((const RigToAnimEntryMap*)compare->m_rigToAnimMapData);

  case AnimToRig:
    return ((AnimToRigTableMap*)m_rigToAnimMapData)->isEqual((const AnimToRigTableMap*)compare->m_rigToAnimMapData);

  default:
    NMP_ASSERT_FAIL(); // Unknown rig to anim map type
  }
  
  return false;
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
// AttribDataRigToAnimMap functions
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataRigToAnimMap::locate(AttribData* target)
{
  AttribDataRigToAnimMap* result = (AttribDataRigToAnimMap*)target;

  AttribData::locate(result);

  // Rig to animation map.
  result->m_rigToAnimMap = NULL;
  NMP::endianSwap(result->m_rigToAnimMapAssetID);
  NMP::endianSwap(result->m_numRigJoints);
  NMP::endianSwap(result->m_rigToAnimMapMemReqs);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataRigToAnimMap::dislocate(AttribData* target)
{
  AttribDataRigToAnimMap* result = (AttribDataRigToAnimMap*)target;

  AttribData::dislocate(result);

  NMP::endianSwap(result->m_rigToAnimMapAssetID);
  NMP::endianSwap(result->m_numRigJoints);
  NMP::endianSwap(result->m_rigToAnimMapMemReqs);
}
#endif // NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataRigToAnimMap::getMemoryRequirements()
{
  return NMP::Memory::Format(NMP::Memory::align(sizeof(AttribDataRigToAnimMap), MR_ATTRIB_DATA_ALIGNMENT), MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataRigToAnimMap* AttribDataRigToAnimMap::init(
  NMP::Memory::Resource& resource,
  RuntimeAssetID         rigToAnimMapAssetID,
  uint32_t               numRigJoints,
  NMP::Memory::Format&   rigToAnimMapMemoryFormat)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataRigToAnimMap* result = (AttribDataRigToAnimMap*)resource.ptr;
  resource.increment(sizeof(AttribDataRigToAnimMap));
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  result->setType(ATTRIB_TYPE_RIG_TO_ANIM_MAP);
  result->m_rigToAnimMap = NULL;
  result->m_rigToAnimMapAssetID = rigToAnimMapAssetID;
  result->m_numRigJoints = numRigJoints;
  result->m_rigToAnimMapMemReqs = rigToAnimMapMemoryFormat;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttribDataRigToAnimMap::fixupRigToAnimMap()
{
  // Rig.
  m_rigToAnimMap = (RigToAnimMap*) MR::Manager::getInstance().getObjectPtrFromObjectID(m_rigToAnimMapAssetID);
  NMP_ASSERT(m_rigToAnimMap);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttribDataRigToAnimMap::unfixRigToAnimMap()
{
  m_rigToAnimMap = NULL;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_HOST_CELL_SPU
void AttribDataRigToAnimMap::prepForSPU(AttribData* target, NMP::MemoryAllocator* allocator)
{
  AttribDataRigToAnimMap* result = (AttribDataRigToAnimMap*)target;

  // Allocate local memory to store the referenced rig to anim map in.
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result->m_rigToAnimMapMemReqs);
  NMP_ASSERT(resource.ptr);

  // DMA rig from system mem to locally allocated result->m_rig.
  NMP::SPUDMAController::dmaGet(resource.ptr, (uint32_t)result->m_rigToAnimMap, result->m_rigToAnimMapMemReqs.size);
  NMP::SPUDMAController::dmaWaitAll();

  // Set the new rig to anim map pointer.
  result->m_rigToAnimMap = (RigToAnimMap*)resource.ptr;

  // Sort out all internal pointers in the rig to anim map.
  result->m_rigToAnimMap->relocate();
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
bool locateRigToAnimMap(uint32_t NMP_USED_FOR_ASSERTS(assetType), void* assetMemory)
{
  NMP_ASSERT(assetType == MR::Manager::kAsset_RigToAnimMap);
  MR::RigToAnimMap* rigToAnimMap = (MR::RigToAnimMap*)assetMemory;
  rigToAnimMap->locate();
  return true;
}
#endif

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
