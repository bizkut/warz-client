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
#include "morpheme/Nodes/mrNodeBlendN.h"
#include "morpheme/mrCoreTaskIDs.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeBlendNFindGeneratingNodeForSemantic(
  NodeID              NMP_UNUSED(callingNodeID),
  bool                NMP_UNUSED(fromParent),   // Was this query from a parent or child node.
  AttribDataSemantic  semantic,
  NodeDef*            nodeDef,
  Network*            NMP_UNUSED(network))
{
  NodeID result = INVALID_NODE_ID;

  switch (semantic)
  {
  case ATTRIB_SEMANTIC_UPDATE_TIME_POS:
  case ATTRIB_SEMANTIC_TRANSFORM_BUFFER:
  case ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER:
  case ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM:
  case ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER:
    result = nodeDef->getNodeID();
    break;
  default:
    break;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void nodeBlendNCalculateBlendWeightClamped(
  float                 interpolant,            // IN: Across any range.
  uint16_t              numSourceNodes,         // IN: The number of active source Nodes
  const float*          sourceWeights,          // IN: The array of weights (including the wrapping element)
  float&                blendWeight,            // OUT: Between the 2 active source Nodes.
  uint16_t&             newActiveIndex0,        // OUT: Index into array of source Nodes.
  uint16_t&             newActiveIndex1)        // OUT: Index into array of source Nodes.
{
  NMP_ASSERT(numSourceNodes > 1);
  NMP_ASSERT(sourceWeights);

  uint16_t lastWeightIndex = numSourceNodes - 1;
  float delta = sourceWeights[lastWeightIndex] - sourceWeights[0];

  // Scale factor for reflecting the weight sequence if required
  float signum = NMP::floatSelect(delta, 1.0f, -1.0f);

  // Note we determine the two connected sources in an adjusted space that assumes the
  // weights are increasing in value. This is achieved by applying the scaling factor to
  // the weight value to perform the necessary reflection.
  float interpAdj = signum * interpolant;
  float weight0Adj = signum * sourceWeights[0];
  float weight1Adj = signum * sourceWeights[lastWeightIndex];

  // Also note that the two connected sources bounding the interpolant and the corresponding
  // blend weight are dependent on the weight sequence and not the (reflected) space the
  // test is performed in.
  if (interpAdj <= weight0Adj)
  {
    // Less than the first weight
    newActiveIndex0 = 0;
    newActiveIndex1 = 1;
    blendWeight = 0.0f;
  }
  else if (interpAdj >= weight1Adj)
  {
    // Greater than the last weight
    newActiveIndex0 = lastWeightIndex - 1;
    newActiveIndex1 = lastWeightIndex;
    blendWeight = 1.0f;
  }
  else
  {
    // Work out which weight interval we are lying between.
    for (uint16_t i = 1; i < numSourceNodes; i++)
    {
      newActiveIndex1 = i;
      weight1Adj = signum * sourceWeights[i];
      if (interpAdj <= weight1Adj)
        break;
    }
    newActiveIndex0 = newActiveIndex1 - 1;
    weight0Adj = signum * sourceWeights[newActiveIndex0];

    // Calculate weighting between 2 source inputs.
    float interpDivision = weight1Adj - weight0Adj;
    if (interpDivision < ERROR_LIMIT)
      blendWeight = 0.0f;
    else
      blendWeight = (interpAdj - weight0Adj) / interpDivision;
    NMP_ASSERT(blendWeight >= 0.0f && blendWeight <= 1.0f);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void nodeBlendNCalculateBlendWeightWrapped(
  float                 interpolant,            // IN: Across any range.
  uint16_t              numSourceNodes,         // IN: The number of active source Nodes
  const float*          sourceWeights,          // IN: The array of weights (including the wrapping element)
  float&                blendWeight,            // OUT: Between the 2 active source Nodes.
  uint16_t&             newActiveIndex0,        // OUT: Index into array of source Nodes.
  uint16_t&             newActiveIndex1)        // OUT: Index into array of source Nodes.
{
  NMP_ASSERT(numSourceNodes > 1);
  NMP_ASSERT(sourceWeights);

  // Check for a narrow weight distribution
  float delta = sourceWeights[numSourceNodes] - sourceWeights[0];
  if (NMP::nmfabs(delta) < ERROR_LIMIT)
  {
    interpolant = 0.0f;
    newActiveIndex0 = 0;
    newActiveIndex1 = 1;
  }
  // Scale factor for reflecting the weight sequence if required
  float signum = NMP::floatSelect(delta, 1.0f, -1.0f);

  // Note we determine the two connected sources in an adjusted space that assumes the
  // weights are increasing in value. This is achieved by applying the scaling factor to
  // the weight value to perform the necessary reflection.
  float interpAdj = signum * interpolant;
  // Compute the wrapped value.
  float weight0Adj = signum * sourceWeights[0];
  float weight1Adj = signum * sourceWeights[numSourceNodes];
  NMP::wrapValue(interpAdj, weight0Adj, weight1Adj);

  // Work out which weight interval we are lying between.
  for (uint16_t i = 1; i <= numSourceNodes; i++)
  {
    newActiveIndex1 = i;
    weight1Adj = signum * sourceWeights[i];
    if (interpAdj <= weight1Adj)
      break;
  }
  newActiveIndex0 = newActiveIndex1 - 1;
  weight0Adj = signum * sourceWeights[newActiveIndex0];

  // Calculate weighting between 2 source inputs.
  float interpDivision = weight1Adj - weight0Adj;
  if (interpDivision < ERROR_LIMIT)
    blendWeight = 0.0f;
  else
    blendWeight = (interpAdj - weight0Adj) / interpDivision;
  NMP_ASSERT(blendWeight >= 0.0f && blendWeight <= 1.0f);

  // Handle wrapping of the last weight interval back to the start. Note that if wrapping
  // is enabled then a extra sample is appended to the weights during asset compilation to
  // encode the weight value at which the space is wrapped.
  if (newActiveIndex1 == numSourceNodes)
    newActiveIndex1 = 0;
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeBlendNUpdateConnections(
  NodeDef* nodeDef,
  Network* net)
{
  //------------------------
  // Calculate the blend weights  
  AttribDataBlendWeights* attribBlendWeights = nodeBlend2CreateBlendWeights(nodeDef, net);
  const AttribDataFloatArray* nodeChildWeights = nodeDef->getAttribData<AttribDataFloatArray>(ATTRIB_SEMANTIC_CHILD_NODE_WEIGHTS);
  NMP_ASSERT(
    (uint16_t)nodeChildWeights->m_numValues == nodeDef->getNumChildNodes() ||
    (uint16_t)nodeChildWeights->m_numValues == nodeDef->getNumChildNodes() + 1);
  bool wrapWeights = ((uint16_t)nodeChildWeights->m_numValues == nodeDef->getNumChildNodes() + 1);
  AnimSetIndex animSet = net->getOutputAnimSetIndex(nodeDef->getNodeID());

  AttribDataFloat* inputCPFloat = net->updateInputCPConnection<AttribDataFloat>(nodeDef->getInputCPConnection(0), animSet);

  BlendOptNodeConnections* activeNodeConnections = (BlendOptNodeConnections*)net->getActiveNodesConnections(nodeDef->getNodeID());
  NMP_ASSERT(net->getNumActiveChildren(nodeDef->getNodeID()) == 2);
  NodeID lastUpdateChild0 = activeNodeConnections->m_activeChildNodeIDs[0];
  NodeID lastUpdateChild1 = activeNodeConnections->m_activeChildNodeIDs[1];

  uint16_t childNodeIndices[2];
  float blendWeight;
  if (wrapWeights)
  {
    nodeBlendNCalculateBlendWeightWrapped(
      inputCPFloat->m_value,
      nodeDef->getNumChildNodes(),
      nodeChildWeights->m_values,
      blendWeight,
      childNodeIndices[0],
      childNodeIndices[1]);
  }
  else
  {
    nodeBlendNCalculateBlendWeightClamped(
      inputCPFloat->m_value,
      nodeDef->getNumChildNodes(),
      nodeChildWeights->m_values,
      blendWeight,
      childNodeIndices[0],
      childNodeIndices[1]);
  }

  // Set the blend weights and active connections
  NMP_ASSERT(childNodeIndices[0] < nodeDef->getNumChildNodes());
  NMP_ASSERT(childNodeIndices[1] < nodeDef->getNumChildNodes());
  activeNodeConnections->m_activeChildNodeIDs[0] = nodeDef->getChildNodeID(childNodeIndices[0]);
  activeNodeConnections->m_activeChildNodeIDs[1] = nodeDef->getChildNodeID(childNodeIndices[1]);

  NMP_USED_FOR_ASSERTS( NodeID deletedNodeID = )
    nodeBlend2UpdateConnectionsSetBlendWeightsCheckForOptimisation( attribBlendWeights, activeNodeConnections,
                                                 net, nodeDef, 
                                                 blendWeight, blendWeight, 
                                                 false );

  //------------------------
  // Delete any child nodes that have become inactive this frame.
  if ((lastUpdateChild0 != activeNodeConnections->m_activeChildNodeIDs[0]) &&
      (lastUpdateChild0 != activeNodeConnections->m_activeChildNodeIDs[1]))
  {
    NMP_ASSERT(lastUpdateChild0 != deletedNodeID);
    net->deleteNodeInstance(lastUpdateChild0);
  }
  if ((lastUpdateChild1 != activeNodeConnections->m_activeChildNodeIDs[0]) &&
      (lastUpdateChild1 != activeNodeConnections->m_activeChildNodeIDs[1]))
  {
    NMP_ASSERT(lastUpdateChild1 != deletedNodeID);
    net->deleteNodeInstance(lastUpdateChild1);
  }

  // Recurse to children.
  net->updateNodeInstanceConnections(activeNodeConnections->m_activeChildNodeIDs[0], animSet);
  net->updateNodeInstanceConnections(activeNodeConnections->m_activeChildNodeIDs[1], animSet);

  return nodeDef->getNodeID();
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
