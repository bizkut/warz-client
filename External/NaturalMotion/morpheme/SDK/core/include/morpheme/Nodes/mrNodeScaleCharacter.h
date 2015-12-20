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
#ifndef MR_NODE_SCALE_CHARACTER_H
#define MR_NODE_SCALE_CHARACTER_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Queuing function declarations
Task* nodeScaleCharacterQueueUpdateTransforms(
  NodeDef        *node,
  TaskQueue      *queue,
  Network        *net,
  TaskParameter  *dependentParameter);

Task* nodeScaleCharacterQueueTrajectoryDeltaTransform(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter);

Task* nodeScaleCharacterQueueTrajectoryDeltaAndTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter);

//----------------------------------------------------------------------------------------------------------------------
// Update of any node data that must happen before queue building or execution.
NodeID nodeScaleCharacterUpdateConnections(
  NodeDef*            node,
  Network*            net);

//----------------------------------------------------------------------------------------------------------------------
// Handles a change of scale values
bool nodeScaleCharacterMessageHandler(
  const MR::Message&  message,
  MR::NodeID          nodeID,
  MR::Network*        net);

//----------------------------------------------------------------------------------------------------------------------
// Handle dislocating and locating scale messages
void endianSwapScaleCharacterMessage(void *data, uint32_t dataSize);

//----------------------------------------------------------------------------------------------------------------------
// Creates a custom scale character message that can then be broadcast to the network
MR::Message* createScaleCharacterMessage(
  NMP::Memory::Resource& memResource,
  MessageID messageID,
  bool advancedMessage,
  float* scaleValues,
  uint32_t numScaleValues);

//----------------------------------------------------------------------------------------------------------------------
// ScaleCharacterMessage inline functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool nodeScaleCharacterMessageLocate(MR::Message* message)
{
  endianSwapScaleCharacterMessage(message->m_data, message->m_dataSize);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool nodeScaleCharacterMessageDislocate(MR::Message* message)
{
  endianSwapScaleCharacterMessage(message->m_data, message->m_dataSize);
  return true;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_SCALE_CHARACTER_H
//----------------------------------------------------------------------------------------------------------------------

