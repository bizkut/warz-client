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
#include "morpheme/Nodes/mrNodeBlend2.h"
#include "morpheme/Nodes/mrNodeBlend2SyncEvents.h"
#include "morpheme/Nodes/mrNodeSubtractiveBlend.h"
#include "morpheme/Nodes/mrNodeSubtractiveBlendSyncEvents.h"
#include "morpheme/mrCoreTaskIDs.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeSubtractiveBlendSyncEventsUpdateConnections(
  NodeDef*           nodeDef,
  Network*           net)
{
  //------------------------
  // Calculate the blend weights  
  AttribDataBlendWeights* attribBlendWeights = nodeBlend2CreateBlendWeights(nodeDef, net);

  NodeID childNodeIDs[2];
  childNodeIDs[0] = nodeDef->getChildNodeID(0);
  childNodeIDs[1] = nodeDef->getChildNodeID(1);
  attribBlendWeights->setAllWeights(1.0f);

  //------------------------
  // This node re-uses the Blend2 Sampled Events Buffers tasks.
  // So we need to set the sampled events blend optimisation info.
  BlendOptNodeConnections* activeNodeConnections = static_cast<BlendOptNodeConnections*>( net->getActiveNodesConnections(nodeDef->getNodeID()) );
  activeNodeConnections->setSampledEventsNodeIDs(2,childNodeIDs);

  //------------------------
  // Update the active node connections
  net->updateActiveNodeConnections(
    nodeDef->getNodeID(),
    2,
    childNodeIDs);

  //------------------------
  // Recurse to children
  NMP_ASSERT(activeNodeConnections->m_numActiveChildNodes == 2);
  AnimSetIndex animSet = net->getOutputAnimSetIndex(nodeDef->getNodeID());
  for (uint16_t i = 0; i < activeNodeConnections->m_numActiveChildNodes; ++i)
  {
    net->updateNodeInstanceConnections(activeNodeConnections->m_activeChildNodeIDs[i], animSet);
  }

  return nodeDef->getNodeID();
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
