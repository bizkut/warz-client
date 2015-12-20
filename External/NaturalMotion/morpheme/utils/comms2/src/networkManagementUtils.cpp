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
#include "comms/networkManagementUtils.h"
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
#include "morpheme/mrManager.h"

#ifdef NMP_MEMORY_LOGGING
  #include "NMPlatform/NMLoggingMemoryAllocator.h"
#endif // NMP_MEMORY_LOGGING

#include "NMPlatform/NMFreelistMemoryAllocator.h"
#include "NMPlatform/NMPrioritiesLogger.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MCOMMS
{

//----------------------------------------------------------------------------------------------------------------------
NMP::FastHeapAllocator* initFastHeapAllocator(const size_t dataSize, uint32_t alignment)
{
  NMP::Memory::Format allocatorFormat = NMP::FastHeapAllocator::getMemoryRequirements((uint32_t)dataSize, alignment);
  NMP::Memory::Resource resource = NMPMemoryAllocateFromFormat(allocatorFormat);

  NMP_ASSERT(resource.ptr);

  return NMP::FastHeapAllocator::init(resource, (uint32_t)dataSize, alignment);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::MemoryAllocator* initMemoryAllocator()
{
  NMP::Memory::Format allocatorFormat;
  NMP::Memory::Resource resource;

#ifdef NMP_MEMORY_LOGGING
  allocatorFormat = NMP::LoggingMemoryAllocator::getMemoryRequirements();
  resource = NMPMemoryAllocateFromFormat(allocatorFormat);
  NMP_ASSERT(resource.ptr);

  return NMP::LoggingMemoryAllocator::init(resource);
#else
  allocatorFormat = NMP::FreelistMemoryAllocator::getMemoryRequirements();
  resource = NMPMemoryAllocateFromFormat(allocatorFormat);
  NMP_ASSERT(resource.ptr);

  return NMP::FreelistMemoryAllocator::init(resource);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void termFastHeapAllocator(NMP::FastHeapAllocator** allocator)
{
  if (*allocator)
  {
#ifdef NMP_MEMORY_LOGGING
    (*allocator)->printAllocations();
#endif
    (*allocator)->term();
    NMP::Memory::memFree(*allocator);
    *allocator = 0;
  }
}
//----------------------------------------------------------------------------------------------------------------------
void termMemoryAllocator(NMP::MemoryAllocator** allocator)
{
  if (*allocator)
  {
#ifdef NMP_MEMORY_LOGGING
    ((NMP::LoggingMemoryAllocator*)(*allocator))->printPeakAllocations();
    ((NMP::LoggingMemoryAllocator*)(*allocator))->printCurrentAllocations();
#endif
    (*allocator)->term();
    NMP::Memory::memFree(*allocator);
    *allocator = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool setNetworkControlParameter(
    MR::Network*                 network,
    commsNodeID                  nodeID,
    const MR::NodeOutputDataType type,
    const void*                  cparamData)
{
  if (network)
  {
    switch (type)
    {
    case NODE_OUTPUT_DATA_TYPE_FLOAT:
      {
        MR::AttribDataFloat floatAttribData;
        floatAttribData.m_value = (*(const float*)cparamData);
        network->setControlParameter((MR::NodeID)nodeID, &floatAttribData);
      }
      break;

    case NODE_OUTPUT_DATA_TYPE_VECTOR3:
      {
        MR::AttribDataVector3 vector3AttribData;
        const float* const floatData = (const float*)cparamData;
        vector3AttribData.m_value.set(floatData[0], floatData[1], floatData[2]);
        network->setControlParameter((MR::NodeID)nodeID, &vector3AttribData);
      }
      break;

    case NODE_OUTPUT_DATA_TYPE_VECTOR4:
      {
        MR::AttribDataVector4 vector4AttribData;
        const float* const floatData = (const float*)cparamData;
        vector4AttribData.m_value.setXYZW(floatData[0], floatData[1], floatData[2], floatData[3]);
        network->setControlParameter((MR::NodeID)nodeID, &vector4AttribData);
      }
      break;

    case NODE_OUTPUT_DATA_TYPE_BOOL:
      {
        MR::AttribDataBool boolAttribData;
        boolAttribData.m_value = (*(const bool*)cparamData);
        network->setControlParameter((MR::NodeID)nodeID, &boolAttribData);
      }
      break;

    case NODE_OUTPUT_DATA_TYPE_INT:
      {
        MR::AttribDataInt intAttribData;
        intAttribData.m_value = (*(const int32_t*)cparamData);
        network->setControlParameter((MR::NodeID)nodeID, &intAttribData);
      }
      break;

    case NODE_OUTPUT_DATA_TYPE_UINT:
      {
        MR::AttribDataUInt uintAttribData;
        uintAttribData.m_value = (*(const uint32_t*)cparamData);
        network->setControlParameter((MR::NodeID)nodeID, &uintAttribData);
      }
      break;

    case NODE_OUTPUT_DATA_TYPE_PHYSICS_OBJECT_POINTER:
      {
        // Note that the calling function should have done the conversion to the actual pointer.
        MR::AttribDataPhysicsObjectPointer ptrAttribData; 
        ptrAttribData.m_value = (void*) cparamData; 
        network->setControlParameter((MR::NodeID)nodeID, &ptrAttribData); 
      }
      break;

    default:
      NMP_ASSERT_FAIL();
      return false;
    }

    return true;
  }
  return false;
}

//---------------------------------------------------------------------------------------------------------------------
void deleteOrphanDurationTracks(
  MR::NetworkDef*        networkDef,
  MR::NodeID             nodeIndex,
  uint16_t               animSetIndex,
  NMP::PrioritiesLogger& logger,
  uint32_t               loggerPriority)
{
  MR::AttribDataSourceEventTrackSet* sourceEventTracks =
    networkDef->getAttribData<MR::AttribDataSourceEventTrackSet>(
    MR::ATTRIB_SEMANTIC_SOURCE_EVENT_TRACKS,
    nodeIndex,
    animSetIndex);
  if (sourceEventTracks)
  {
    uint32_t eventTrackCount = sourceEventTracks->m_numDurEventTracks;
    MR::EventTrackDefDuration** durationEventTracks = sourceEventTracks->m_sourceDurEventTracks;
    for (uint32_t i = 0; i < eventTrackCount; i++)
    {
      MR::EventTrackDefDuration* durationTrack = durationEventTracks[i];

      // Ignore null entries in the AnimSet
      if (durationTrack == 0)
        continue;

      if (MR::Manager::getInstance().getObjectRefCount(durationTrack) == 0)
      {
        NM_LOG_MESSAGE(
          logger,
          loggerPriority,
          "    Deleting referenced EventTrackDefDuration - AssetID: %p.\n",
          MR::Manager::getInstance().getObjectIDFromObjectPtr(durationTrack));
        MR::Manager::getInstance().unregisterObject(durationTrack);
        NMP::Memory::memFree(durationTrack);
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void deleteOrphanCurveTracks(
  MR::NetworkDef*        networkDef,
  MR::NodeID             nodeIndex,
  uint16_t               animSetIndex,
  NMP::PrioritiesLogger& logger,
  uint32_t               loggerPriority)
{
  MR::AttribDataSourceEventTrackSet* sourceEventTracks =
    networkDef->getAttribData<MR::AttribDataSourceEventTrackSet>(
    MR::ATTRIB_SEMANTIC_SOURCE_EVENT_TRACKS,
    nodeIndex,
    animSetIndex);
  if (sourceEventTracks)
  {
    uint32_t eventTrackCount = sourceEventTracks->m_numCurveEventTracks;
    MR::EventTrackDefCurve** curveEventTracks = sourceEventTracks->m_sourceCurveEventTracks;
    for (uint32_t i = 0; i < eventTrackCount; i++)
    {
      MR::EventTrackDefCurve* curveTrack = curveEventTracks[i];

      // Ignore null entries in the AnimSet
      if (curveTrack == 0)
        continue;

      if (MR::Manager::getInstance().getObjectRefCount(curveTrack) == 0)
      {
        NM_LOG_MESSAGE(
          logger,
          loggerPriority,
          "    Deleting referenced EventTrackDefCurve - AssetID: %p.\n",
          MR::Manager::getInstance().getObjectIDFromObjectPtr(curveTrack));
        MR::Manager::getInstance().unregisterObject(curveTrack);
        NMP::Memory::memFree(curveTrack);
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void deleteOrphanDiscreteTracks(
  MR::NetworkDef* networkDef,
  MR::NodeID nodeIndex,
  uint16_t animSetIndex,
  NMP::PrioritiesLogger& logger,
  uint32_t loggerPriority)
{
  MR::AttribDataSourceEventTrackSet* sourceEventTracks =
    networkDef->getAttribData<MR::AttribDataSourceEventTrackSet>(
    MR::ATTRIB_SEMANTIC_SOURCE_EVENT_TRACKS,
    nodeIndex,
    animSetIndex);
  if (sourceEventTracks)
  {
    uint32_t eventTrackCount = sourceEventTracks->m_numDiscreteEventTracks;
    MR::EventTrackDefDiscrete** discreteEventTracks = sourceEventTracks->m_sourceDiscreteEventTracks;
    for (uint32_t i = 0; i < eventTrackCount; i++)
    {
      MR::EventTrackDefDiscrete* discreteTrack = discreteEventTracks[i];

      // Ignore null entries in the AnimSet
      if (discreteTrack == NULL)
        continue;

      if (MR::Manager::getInstance().getObjectRefCount(discreteTrack) == 0)
      {
        NM_LOG_MESSAGE(
          logger,
          loggerPriority,
          "    Deleting referenced EventTrackDefDiscrete - AssetID: %p.\n",
          MR::Manager::getInstance().getObjectIDFromObjectPtr(discreteTrack));
        MR::Manager::getInstance().unregisterObject(discreteTrack);
        NMP::Memory::memFree(discreteTrack);
      }
    }
  }
}

} // namespace MCOMMS
