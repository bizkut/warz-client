// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
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
#include "morpheme/Nodes/mrScatterBlend1DUtils.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrNodeScatterBlend1D.h"
#include "morpheme/mrCoreTaskIDs.h"
//----------------------------------------------------------------------------------------------------------------------

// To enable profiling output.
#define NM_SB1D_PROFILINGx
#if defined(NM_SB1D_PROFILING)
#define NM_SB1D_BEGIN_PROFILING(name)    NM_BEGIN_PROFILING(name)
#define NM_SB1D_END_PROFILING()          NM_END_PROFILING()
#else // NM_SB1D_PROFILING
#define NM_SB1D_BEGIN_PROFILING(name)
#define NM_SB1D_END_PROFILING()
#endif // NM_SB1D_PROFILING

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeScatterBlend1DOfflineUpdateConnections(
  NodeDef*           nodeDef,
  Network*           net)
{
  //------------------------
  // Calculate the blend weights
  AttribDataBlendWeights* attribBlendWeights = nodeBlend2CreateBlendWeights(nodeDef, net);
  BlendOptNodeConnections* activeNodeConnections = (BlendOptNodeConnections*)net->getActiveNodesConnections(nodeDef->getNodeID());
  const AttribDataScatterBlendAnalysisDef* attribDef = nodeDef->getAttribData<AttribDataScatterBlendAnalysisDef>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, 0);

  // Set the blend weights and sources
  attribBlendWeights->setAllWeights(attribDef->m_numWeights, attribDef->m_weights);
  activeNodeConnections->setTrajectoryAndTransformsNodeIDs(attribDef->m_numNodeIDs, attribDef->m_nodeIDs);
  activeNodeConnections->setSampledEventsNodeIDs(attribDef->m_numNodeIDs, attribDef->m_nodeIDs);

  //------------------------
  // Update the active node connections
  activeNodeConnections->m_numActiveChildNodes = attribDef->m_numNodeIDs;
  for (uint16_t i = 0; i < attribDef->m_numNodeIDs; ++i)
  {
    activeNodeConnections->m_activeChildNodeIDs[i] = attribDef->m_nodeIDs[i];
  }

  //------------------------
  // Recurse to children
  AnimSetIndex animSet = net->getOutputAnimSetIndex(nodeDef->getNodeID());
  for (uint16_t i = 0; i < activeNodeConnections->m_numActiveChildNodes; ++i)
  {
    net->updateNodeInstanceConnections(activeNodeConnections->m_activeChildNodeIDs[i], animSet);
  }

  return nodeDef->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
// Update connections function.
NodeID nodeScatterBlend1DUpdateConnections(
  NodeDef*           nodeDef,
  Network*           net)
{
  //------------------------
  // Calculate the blend weights
  AnimSetIndex                        animSet               = net->getOutputAnimSetIndex(nodeDef->getNodeID());
  AttribDataBlendWeights*             attribBlendWeights    = nodeBlend2CreateBlendWeights(nodeDef, net);
  BlendOptNodeConnections*            activeNodeConnections = (BlendOptNodeConnections*)net->getActiveNodesConnections(nodeDef->getNodeID());
  AttribDataFloat*                    inputCPFloat          = net->updateInputCPConnection<AttribDataFloat>(nodeDef->getInputCPConnection(0), animSet);
  const AttribDataScatterBlend1DDef*  attribDef             = nodeDef->getAttribData<AttribDataScatterBlend1DDef>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, animSet);

  // Recover the motion parameter vector. We also apply the unit scaling factor to convert
  // input control parameters in degrees to radians
  float motionParameterValue = inputCPFloat->m_value * attribDef->m_unitScale;

  NMP_ASSERT(net->getNumActiveChildren(nodeDef->getNodeID()) == 2);
  NodeID lastUpdateChild0 = activeNodeConnections->m_activeChildNodeIDs[0];
  NodeID lastUpdateChild1 = activeNodeConnections->m_activeChildNodeIDs[1];

  // Compute the blend sources and weights from the desired input control
  NM_SB1D_BEGIN_PROFILING("scatterBlend1DComputeChildNodeWeightsForInputControl");
  ScatterBlend1DParameteriserInfo result;
  scatterBlend1DComputeChildNodeWeightsForInputControl(
    nodeDef,
    attribDef,
    motionParameterValue,
    result);
  NM_SB1D_END_PROFILING();

  // Set the blend weights and active connections
  activeNodeConnections->m_activeChildNodeIDs[0] = result.m_childNodeIDs[0];
  activeNodeConnections->m_activeChildNodeIDs[1] = result.m_childNodeIDs[1];

  NMP_USED_FOR_ASSERTS( NodeID nodeIDToDel = )
  nodeBlend2UpdateConnectionsSetBlendWeightsCheckForOptimisation( attribBlendWeights,
                                                                  activeNodeConnections,
                                                                  net,
                                                                  nodeDef,
                                                                  result.m_childNodeWeights[1],
                                                                  result.m_childNodeWeights[1],
                                                                  false );

  //------------------------
  // Delete any child nodes that have become inactive this frame.
  if ((lastUpdateChild0 != activeNodeConnections->m_activeChildNodeIDs[0]) &&
      (lastUpdateChild0 != activeNodeConnections->m_activeChildNodeIDs[1]))
  {
    NMP_ASSERT( lastUpdateChild0 != nodeIDToDel );
    net->deleteNodeInstance(lastUpdateChild0);
  }
  if ((lastUpdateChild1 != activeNodeConnections->m_activeChildNodeIDs[0]) &&
      (lastUpdateChild1 != activeNodeConnections->m_activeChildNodeIDs[1]))
  {
    NMP_ASSERT( lastUpdateChild1 != nodeIDToDel );
    net->deleteNodeInstance(lastUpdateChild1);
  }

  // Recurse to children.
  net->updateNodeInstanceConnections(activeNodeConnections->m_activeChildNodeIDs[0], animSet);
  net->updateNodeInstanceConnections(activeNodeConnections->m_activeChildNodeIDs[1], animSet);

  return nodeDef->getNodeID();
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
