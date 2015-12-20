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
#include "morpheme/Nodes/mrNodeBlendN.h"
#include "morpheme/Nodes/mrNodeBlend2x2.h"
#include "morpheme/Nodes/mrNodeBlendNxM.h"
#include "morpheme/mrCoreTaskIDs.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeBlendNxMUpdateConnections(
  NodeDef*           nodeDef,
  Network*           net)
{
  //------------------------
  // Calculate the blend weights  
  AttribDataBlendWeights* attribBlendWeights = nodeBlend2x2CreateBlendWeights(nodeDef, net);
  BlendOptNodeConnections* activeNodeConnections = (BlendOptNodeConnections*)net->getActiveNodesConnections(nodeDef->getNodeID());
  AnimSetIndex animSet = net->getOutputAnimSetIndex(nodeDef->getNodeID());

  const AttribDataBlendNxMDef* nodeChildWeights = nodeDef->getAttribData<AttribDataBlendNxMDef>(ATTRIB_SEMANTIC_CHILD_NODE_WEIGHTS);
  NMP_ASSERT(nodeChildWeights->m_numWeightsX >= 2);
  NMP_ASSERT(nodeChildWeights->m_numWeightsY >= 2);
  AttribDataFloat* inputCPFloatX = net->updateInputCPConnection<AttribDataFloat>(nodeDef->getInputCPConnection(0), animSet);
  AttribDataFloat* inputCPFloatY = net->updateInputCPConnection<AttribDataFloat>(nodeDef->getInputCPConnection(1), animSet);
  float blendWeightsAlwaysBlend[2];

  // Weights X
  uint16_t activeChildIndexX0;
  uint16_t activeChildIndexX1;
  if (nodeChildWeights->m_wrapWeightsX)
  {
    nodeBlendNCalculateBlendWeightWrapped(
      inputCPFloatX->m_value,
      nodeChildWeights->m_numWeightsX,
      nodeChildWeights->m_weightsX,
      blendWeightsAlwaysBlend[0],
      activeChildIndexX0,
      activeChildIndexX1);
  }
  else
  {
    nodeBlendNCalculateBlendWeightClamped(
      inputCPFloatX->m_value,
      nodeChildWeights->m_numWeightsX,
      nodeChildWeights->m_weightsX,
      blendWeightsAlwaysBlend[0],
      activeChildIndexX0,
      activeChildIndexX1);
  }

  // Weights Y
  uint16_t activeChildIndexY0;
  uint16_t activeChildIndexY1;
  if (nodeChildWeights->m_wrapWeightsY)
  {
    nodeBlendNCalculateBlendWeightWrapped(
      inputCPFloatY->m_value,
      nodeChildWeights->m_numWeightsY,
      nodeChildWeights->m_weightsY,
      blendWeightsAlwaysBlend[1],
      activeChildIndexY0,
      activeChildIndexY1);
  }
  else
  {
    nodeBlendNCalculateBlendWeightClamped(
      inputCPFloatY->m_value,
      nodeChildWeights->m_numWeightsY,
      nodeChildWeights->m_weightsY,
      blendWeightsAlwaysBlend[1],
      activeChildIndexY0,
      activeChildIndexY1);
  }

  // Recover the child node IDs for the blending quad
  uint16_t childNodeIndices[4];
  childNodeIndices[0] = nodeBlendNxMGetChildNodeIndex(activeChildIndexX0, activeChildIndexY0, nodeChildWeights->m_numWeightsX, nodeChildWeights->m_numWeightsY);
  childNodeIndices[1] = nodeBlendNxMGetChildNodeIndex(activeChildIndexX1, activeChildIndexY0, nodeChildWeights->m_numWeightsX, nodeChildWeights->m_numWeightsY);
  childNodeIndices[2] = nodeBlendNxMGetChildNodeIndex(activeChildIndexX0, activeChildIndexY1, nodeChildWeights->m_numWeightsX, nodeChildWeights->m_numWeightsY);
  childNodeIndices[3] = nodeBlendNxMGetChildNodeIndex(activeChildIndexX1, activeChildIndexY1, nodeChildWeights->m_numWeightsX, nodeChildWeights->m_numWeightsY);
  NodeID childNodeIDsAlwaysBlend[4];
  for (uint32_t i = 0; i < 4; ++i)
    childNodeIDsAlwaysBlend[i] = nodeDef->getChildNodeID(childNodeIndices[i]);

  //------------------------
  // Update the active node connections
  net->updateActiveNodeConnections( nodeDef->getNodeID(), 4, childNodeIDsAlwaysBlend );

  //------------------------
  // Set up for optimisation
  nodeBlend2x2UpdateConnectionsSetBlendWeightsCheckForOptimisation( attribBlendWeights,
                                                                    activeNodeConnections,
                                                                    net,
                                                                    nodeDef,
                                                                    blendWeightsAlwaysBlend );

  //------------------------
  // Recurse to children  
  for (uint16_t i = 0; i < 4; ++i)
  {
    net->updateNodeInstanceConnections(activeNodeConnections->m_activeChildNodeIDs[i], animSet);
  }

  return nodeDef->getNodeID();
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
