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
#include "morpheme/mrMirroredAnimMapping.h"
#include "morpheme/mrManager.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

#ifndef NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataMirroredAnimMapping::getMemoryRequirements(
  uint32_t numValues,
  uint32_t numEvents,
  uint32_t numTracks,
  uint32_t numBones)
{
  NMP::Memory::Format result(sizeof(AttribDataMirroredAnimMapping), MR_ATTRIB_DATA_ALIGNMENT);

  // Add space for the array of bone mappings.
  result += NMP::Memory::Format(sizeof(SimpleMapping) * numValues, NMP_NATURAL_TYPE_ALIGNMENT);

  // Add space for the array of rotation offsets
  result += NMP::Memory::Format(sizeof(NMP::Quat) * numBones, NMP_VECTOR_ALIGNMENT);

  // Add space for the array of event id mappings.
  result += NMP::Memory::Format(sizeof(SimpleMapping) * numEvents, NMP_NATURAL_TYPE_ALIGNMENT);

  // Add space for the array of track id mappings.
  result += NMP::Memory::Format(sizeof(SimpleMapping) * numTracks, NMP_NATURAL_TYPE_ALIGNMENT);

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataMirroredAnimMapping* AttribDataMirroredAnimMapping::init(
  NMP::Memory::Resource& resource,
  uint32_t               numValues,
  uint32_t               numEvents,
  uint32_t               numTracks,
  uint32_t               numBones,
  uint16_t               refCount)
{
  NMP::Memory::Format format(sizeof(AttribDataMirroredAnimMapping), MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(format);
  AttribDataMirroredAnimMapping* result = (AttribDataMirroredAnimMapping*) resource.ptr;
  resource.increment(format);

  result->setType(ATTRIB_TYPE_ANIM_MIRRORED_MAPPING);
  result->setRefCount(refCount);

  // Array of BoneMappings.
  result->m_numBoneMappings = numValues;
  format = NMP::Memory::Format(sizeof(SimpleMapping) * numValues, NMP_NATURAL_TYPE_ALIGNMENT);
  resource.align(format);
  result->m_boneMappings = (SimpleMapping*) resource.ptr;
  resource.increment(format);

  // Array of rotation offsets for the bones.
  result->m_numBones = numBones;
  format = NMP::Memory::Format(sizeof(NMP::Quat) * numBones, NMP_VECTOR_ALIGNMENT);
  resource.align(format);
  result->m_quatOffsets = (NMP::Quat*) resource.ptr;
  resource.increment(format);

  // Array of event id mappings.
  result->m_numEventIds = numEvents;
  format = NMP::Memory::Format(sizeof(SimpleMapping) * numEvents, NMP_NATURAL_TYPE_ALIGNMENT);
  resource.align(format);
  result->m_eventIds = (SimpleMapping*) resource.ptr;
  resource.increment(format);

  // Array of track id mappings.
  result->m_numTrackIds = numTracks;
  format = NMP::Memory::Format(sizeof(SimpleMapping) * numTracks, NMP_NATURAL_TYPE_ALIGNMENT);
  resource.align(format);
  result->m_trackIds = (SimpleMapping*) resource.ptr;
  resource.increment(format);

  // Make sure resource pointer has been moved on by the size returned by getMemoryRequirements.
  format = NMP::Memory::Format(0, MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(format);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataMirroredAnimMapping::locate(AttribData* target)
{
  AttribDataMirroredAnimMapping* result = (AttribDataMirroredAnimMapping*) target;
  AttribData::locate(target);
  NMP::endianSwap(result->m_axis);
  NMP::endianSwap(result->m_numBoneMappings);
  NMP::endianSwap(result->m_boneMappings);
  NMP::endianSwap(result->m_numTrackIds);
  NMP::endianSwap(result->m_trackIds);
  NMP::endianSwap(result->m_numEventIds);
  NMP::endianSwap(result->m_eventIds);
  NMP::endianSwap(result->m_numBones);
  NMP::endianSwap(result->m_quatOffsets);
  REFIX_PTR_RELATIVE(SimpleMapping, result->m_boneMappings, result);
  REFIX_PTR_RELATIVE(SimpleMapping, result->m_eventIds, result);
  REFIX_PTR_RELATIVE(SimpleMapping, result->m_trackIds, result);
  REFIX_PTR_RELATIVE(NMP::Quat, result->m_quatOffsets, result);

  // Fixup each of the actual values.
  for (uint32_t i = 0; i < result->m_numBoneMappings; ++i)
  {
    NMP::endianSwap(result->m_boneMappings[i]);
  }

  // Fixup each of the actual values.
  for (uint32_t i = 0; i < result->m_numEventIds; ++i)
  {
    NMP::endianSwap(result->m_eventIds[i]);
  }

  // Fixup each of the actual values.
  for (uint32_t i = 0; i < result->m_numTrackIds; ++i)
  {
    NMP::endianSwap(result->m_trackIds[i]);
  }

  // Fixup each of the actual values.
  for (uint32_t i = 0; i < result->m_numBones; ++i)
  {
    NMP::endianSwap(result->m_quatOffsets[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataMirroredAnimMapping::dislocate(AttribData* target)
{
  AttribDataMirroredAnimMapping* result = (AttribDataMirroredAnimMapping*) target;

  for (uint32_t i = 0; i < result->m_numBones; ++i)
  {
    NMP::endianSwap(result->m_quatOffsets[i]);
  }
  UNFIX_PTR_RELATIVE(NMP::Quat, result->m_quatOffsets, result);

  for (uint32_t i = 0; i < result->m_numTrackIds; ++i)
  {
    NMP::endianSwap(result->m_trackIds[i]);
  }
  UNFIX_PTR_RELATIVE(SimpleMapping, result->m_trackIds, result);

  for (uint32_t i = 0; i < result->m_numEventIds; ++i)
  {
    NMP::endianSwap(result->m_eventIds[i]);
  }
  UNFIX_PTR_RELATIVE(SimpleMapping, result->m_eventIds, result);

  for (uint32_t i = 0; i < result->m_numBoneMappings; ++i)
  {
    NMP::endianSwap(result->m_boneMappings[i]);
  }
  UNFIX_PTR_RELATIVE(SimpleMapping, result->m_boneMappings, result);

  NMP::endianSwap(result->m_axis);
  NMP::endianSwap(result->m_numBoneMappings);
  NMP::endianSwap(result->m_boneMappings);
  NMP::endianSwap(result->m_numTrackIds);
  NMP::endianSwap(result->m_trackIds);
  NMP::endianSwap(result->m_numEventIds);
  NMP::endianSwap(result->m_eventIds);
  NMP::endianSwap(result->m_numBones);
  NMP::endianSwap(result->m_quatOffsets);
  AttribData::dislocate(target);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AttribDataMirroredAnimMapping::relocate(AttribData* target, void* location)
{
  AttribDataMirroredAnimMapping* result = (AttribDataMirroredAnimMapping*) target;
  void* ptr = location;
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, MR_ATTRIB_DATA_ALIGNMENT));
  ptr = (void*)(((size_t)ptr) + sizeof(AttribDataMirroredAnimMapping));

  ptr = (void*) NMP::Memory::align(ptr, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_boneMappings = (SimpleMapping*) ptr;
  ptr = (void*)(((size_t)ptr) + (sizeof(SimpleMapping) * result->m_numBoneMappings));

  ptr = (void*) NMP::Memory::align(ptr, NMP_VECTOR_ALIGNMENT);
  result->m_quatOffsets = (NMP::Quat*) ptr;
  ptr = (void*)(((size_t)ptr) + (sizeof(NMP::Quat) * result->m_numBones));

  ptr = (void*) NMP::Memory::align(ptr, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_eventIds = (SimpleMapping*) ptr;
  ptr = (void*)(((size_t)ptr) + (sizeof(SimpleMapping) * result->m_numEventIds));

  ptr = (void*) NMP::Memory::align(ptr, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_trackIds = (SimpleMapping*) ptr;
  ptr = (void*)(((size_t)ptr) + (sizeof(SimpleMapping) * result->m_numTrackIds));
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AttribDataMirroredAnimMapping::findTrackIDMapping(uint32_t trackID) const
{
  for (uint32_t i = 0; i != m_numTrackIds; ++i)
  {
    uint32_t leftTrackId = getLeftTrackIdMapping(i);
    uint32_t rightTrackId = getRightTrackIdMapping(i);

    if (leftTrackId == trackID)
    {
      trackID = rightTrackId;
      break;
    }

    if (rightTrackId == trackID)
    {
      trackID = leftTrackId;
      break;
    }
  }
  return trackID;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AttribDataMirroredAnimMapping::findEventMappingID(uint32_t eventID) const
{
  for (uint32_t i = 0; i != m_numEventIds; ++i)
  {
    uint32_t leftEventId = getLeftEventIdMapping(i);
    uint32_t rightEventId = getRightEventIdMapping(i);

    if (leftEventId == eventID)
    {
      eventID = rightEventId;
      break;
    }

    if (rightEventId == eventID)
    {
      eventID = leftEventId;
      break;
    }
  }
  return eventID;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
