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
#include "morpheme/Nodes/mrNodeFeatherBlend2SyncEvents.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/mrCoreTaskIDs.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeFeatherBlend2SyncEventsUpdateConnectionsFixBlendWeight(
  NodeDef*           nodeDef,
  Network*           net)
{
  //------------------------
  // Calculate the blend weights
  AnimSetIndex animSet = net->getOutputAnimSetIndex(nodeDef->getNodeID());

  AttribDataBlendWeights* attribBlendWeights = nodeBlend2CreateBlendWeights(nodeDef, net);
  BlendOptNodeConnections* activeNodeConnections = (BlendOptNodeConnections*)net->getActiveNodesConnections(nodeDef->getNodeID());
  const float blendWeight = 1.0f; // Fix the blend weight

  // Get the events blend weight
  float blendWeightEvents = blendWeight;
  if (nodeDef->getNumInputCPConnections() > 0)
  {
    // There is no connected blend weight so the events blend weight is located at index 0
    const AttribDataFloatArray* nodeChildWeights = nodeDef->getAttribData<AttribDataFloatArray>(ATTRIB_SEMANTIC_CHILD_NODE_WEIGHTS);
    AttribDataFloat* inputCPFloat0 = net->updateInputCPConnection<AttribDataFloat>(nodeDef->getInputCPConnection(0), animSet);
    blendWeightEvents = nodeBlend2CalculateBlendWeight(inputCPFloat0->m_value, nodeChildWeights->m_values);
  }

  //------------------------
  // Sets members of attribBlendWeights and activeNodeConnections. Indicating the number of and which nodes are active for 
  // the calculation of trajectory and transform and events. These are used for the optimisation of trajectory/transforms and 
  // events where only one child is active.
  nodeBlend2UpdateConnectionsSetBlendWeightsCheckForOptimisation( attribBlendWeights, 
                                                                  activeNodeConnections,
                                                                  net, 
                                                                  nodeDef,
                                                                  blendWeight, 
                                                                  blendWeightEvents,
                                                                  true );

  //------------------------
  // Recurse to children
  NMP_ASSERT(nodeDef->hasChildNodeID(activeNodeConnections->m_activeChildNodeIDs[0]));
  NMP_ASSERT(nodeDef->hasChildNodeID(activeNodeConnections->m_activeChildNodeIDs[1]));
  net->updateNodeInstanceConnections(activeNodeConnections->m_activeChildNodeIDs[0], animSet);
  net->updateNodeInstanceConnections(activeNodeConnections->m_activeChildNodeIDs[1], animSet);

  return nodeDef->getNodeID();
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
