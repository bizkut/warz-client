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
#ifndef MR_TRANSIT_DEADBLEND_H
#define MR_TRANSIT_DEADBLEND_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

class Network;
class NodeDef;

typedef uint32_t TransitDeadBlendType;

//----------------------------------------------------------------------------------------------------------------------
bool initialiseDeadBlend(NodeDef* node, Network* net);

//----------------------------------------------------------------------------------------------------------------------
bool initialiseTrajectoryDeltaAndTransformsDeadBlend(NodeDef* node, Network* net);

//----------------------------------------------------------------------------------------------------------------------
bool isDeadBlending(NodeDef* node, Network* net);

//----------------------------------------------------------------------------------------------------------------------
bool isBlendingToPhysics(NodeDef* node, Network* net);

//----------------------------------------------------------------------------------------------------------------------
bool isDeadBlendingOrBlendingToPhysics(NodeDef* node, Network* net);

//----------------------------------------------------------------------------------------------------------------------
NodeID getPhysicalNodeID(NodeDef* node, Network* net);

//----------------------------------------------------------------------------------------------------------------------
uint32_t getNodeRootControlMethod(NodeDef* node, Network* net);

//----------------------------------------------------------------------------------------------------------------------
AttribDataDeadBlendState* getDeadBlendState(NodeDef* node, Network* net);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_TRANSIT_DEADBLEND_H
//----------------------------------------------------------------------------------------------------------------------
