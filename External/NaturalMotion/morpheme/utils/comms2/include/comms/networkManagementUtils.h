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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef MCOMMS_NETWORKMANAGEMENTUTILS_H
#define MCOMMS_NETWORKMANAGEMENTUTILS_H
//----------------------------------------------------------------------------------------------------------------------
#include "comms/mcomms.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

class Network;
class NetworkDef;

}

namespace NMP
{

class FastHeapAllocator;
class MemoryAllocator;
class PrioritiesLogger;

}

//----------------------------------------------------------------------------------------------------------------------
namespace MCOMMS
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief Create and initialise a fast heap allocator of a given size and alignment.
//----------------------------------------------------------------------------------------------------------------------
NMP::FastHeapAllocator* initFastHeapAllocator(const size_t dataSize, uint32_t alignment);
//----------------------------------------------------------------------------------------------------------------------
/// \brief Create and initialise a memory allocator.
/// If NMP_MEMORY_LOGGING is defined a NMP::LoggingMemoryAllocator is created, otherwise a NMP::FreelistMemoryAllocator
/// is created.
//----------------------------------------------------------------------------------------------------------------------
NMP::MemoryAllocator* initMemoryAllocator();

//----------------------------------------------------------------------------------------------------------------------
/// \brief Terminate a FastHeapAllocator created in initFastHeapAllocator().
/// If NMP_MEMORY_LOGGING is defined (*allocator)->printAllocations(); is called before freeing its memory.
//----------------------------------------------------------------------------------------------------------------------
void termFastHeapAllocator(NMP::FastHeapAllocator** allocator);
//----------------------------------------------------------------------------------------------------------------------
/// \brief Terminate a memory allocator created in initMemoryAllocator.
/// If NMP_MEMORY_LOGGING is defined (*allocator)->printPeakAllocations(); is called before freeing its memory.
//----------------------------------------------------------------------------------------------------------------------
void termMemoryAllocator(NMP::MemoryAllocator** allocator);

//----------------------------------------------------------------------------------------------------------------------
/// \brief Set a control parameters of a given type on a network.
//----------------------------------------------------------------------------------------------------------------------
bool setNetworkControlParameter(
  MR::Network*           network,
  commsNodeID            nodeID,
  const MR::NodeOutputDataType type,
  const void*            cparamData);

void deleteOrphanDurationTracks(
  MR::NetworkDef*        networkDef,
  MR::NodeID             nodeIndex,
  uint16_t               animSetIndex,
  NMP::PrioritiesLogger& logger,
  uint32_t               loggerPriority);

void deleteOrphanCurveTracks(
  MR::NetworkDef*        networkDef,
  MR::NodeID             nodeIndex,
  uint16_t               animSetIndex,
  NMP::PrioritiesLogger& logger,
  uint32_t               loggerPriority);

void deleteOrphanDiscreteTracks(
  MR::NetworkDef*        networkDef,
  MR::NodeID             nodeIndex,
  uint16_t               animSetIndex,
  NMP::PrioritiesLogger& logger,
  uint32_t               loggerPriority);

} // namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_NETWORKMANAGEMENTUTILS_H
//----------------------------------------------------------------------------------------------------------------------
