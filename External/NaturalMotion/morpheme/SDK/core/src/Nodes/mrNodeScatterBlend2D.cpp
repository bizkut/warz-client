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
#include "morpheme/Nodes/mrScatterBlend2DUtils.h"
#include "morpheme/Nodes/mrNodeScatterBlend2D.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/mrCoreTaskIDs.h"
//----------------------------------------------------------------------------------------------------------------------

// To enable profiling output.
#define NM_SB2D_PROFILINGx
#if defined(NM_SB2D_PROFILING)
#define NM_SB2D_BEGIN_PROFILING(name)    NM_BEGIN_PROFILING(name)
#define NM_SB2D_END_PROFILING()          NM_END_PROFILING()
#else // NM_SB2D_PROFILING
#define NM_SB2D_BEGIN_PROFILING(name)
#define NM_SB2D_END_PROFILING()
#endif // NM_SB2D_PROFILING

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
AttribDataBlendWeights* nodeScatterBlend2DCreateBlendWeights(
  const NodeDef* nodeDef,
  Network* net)
{
  FrameCount currFrameNo = net->getCurrentFrameNo();
  NMP_ASSERT(currFrameNo > 0);
  NodeBinEntry* networkEntry;
  AttribDataBlendWeights* attribBlendWeights = NULL;

  //------------------------
  // If not already existing, create an attribute on the Network to store the desired blend weights.
  networkEntry = net->getAttribDataNodeBinEntry(ATTRIB_SEMANTIC_BLEND_WEIGHTS, nodeDef->getNodeID(), INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
  if (!networkEntry)
  {
    AttribDataHandle handle = AttribDataBlendWeights::create(net->getPersistentMemoryAllocator(), 2);
    attribBlendWeights = (AttribDataBlendWeights*) handle.m_attribData;
    AttribAddress attribAddess(ATTRIB_SEMANTIC_BLEND_WEIGHTS, nodeDef->getNodeID(), INVALID_NODE_ID, currFrameNo);
    net->addAttribData(attribAddess, handle, LIFESPAN_FOREVER);
  }
  else
  {
    networkEntry->m_address.m_validFrame = currFrameNo;
    attribBlendWeights = networkEntry->getAttribData<AttribDataBlendWeights>();
  }
  NMP_ASSERT(attribBlendWeights);

  return attribBlendWeights;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataScatterBlend2DState* nodeScatterBlend2DCreateState(
  const NodeDef* nodeDef,
  Network* net)
{
  FrameCount currFrameNo = net->getCurrentFrameNo();
  NMP_ASSERT(currFrameNo > 0);
  NodeBinEntry* networkEntry;
  AttribDataScatterBlend2DState* attribDataState = NULL;

  //------------------------
  // If not already existing, create an attribute on the Network to store the desired blend weights.
  networkEntry = net->getAttribDataNodeBinEntry(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, nodeDef->getNodeID(), INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
  if (!networkEntry)
  {
    AttribDataHandle handle = AttribDataScatterBlend2DState::create(net->getPersistentMemoryAllocator());
    attribDataState = (AttribDataScatterBlend2DState*) handle.m_attribData;
    AttribAddress attribAddess(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, nodeDef->getNodeID(), INVALID_NODE_ID, currFrameNo);
    net->addAttribData(attribAddess, handle, LIFESPAN_FOREVER);
  }
  else
  {
    networkEntry->m_address.m_validFrame = currFrameNo;
    attribDataState = networkEntry->getAttribData<AttribDataScatterBlend2DState>();
  }
  NMP_ASSERT(attribDataState);

  return attribDataState;
}

//----------------------------------------------------------------------------------------------------------------------
void triangleGetBlendWeights(
  const float*  childNodeWeightsAlwaysBlend,  ///< IN: The input non optimal Barycentric blend weights
  float*        blendWeightsAlwaysBlend )     ///< OUT: The bi-linear blend weights for always blending behaviour
{
  NMP_ASSERT(childNodeWeightsAlwaysBlend);
  NMP_ASSERT(blendWeightsAlwaysBlend);

  // Convert the interpolated Barycentric weights into tri-linear blend weights.
  float alpha, beta;
  NMP::triangleBarycentricToBilinear(
    childNodeWeightsAlwaysBlend[0],
    childNodeWeightsAlwaysBlend[1],
    childNodeWeightsAlwaysBlend[2],
    alpha,
    beta);

  blendWeightsAlwaysBlend[0] = NMP::clampValue(alpha, 0.0f, 1.0f);
  blendWeightsAlwaysBlend[1] = NMP::clampValue(beta, 0.0f, 1.0f);
}

//----------------------------------------------------------------------------------------------------------------------
void triangleBlendWeightCheck(
  bool          alwaysBlendFlag,              ///< IN: Flag to enable blend optimisation
  const float*  childNodeWeightsAlwaysBlend,  ///< IN: The input non optimal Barycentric blend weights
  const NodeID* childNodeIDsAlwaysBlend,      ///< IN: The Node IDs of the non optimal active connected children
  const float*  blendWeightsAlwaysBlend,      ///< IN: The bi-linear blend weights for always blending behaviour
  uint16_t&     numBlendWeightsOpt,           ///< OUT: The number of optimised weights
  float*        blendWeightsOpt,              ///< OUT: The optimised blend weights
  uint16_t&     numChildNodeIDsOpt,           ///< OUT: The number of optimised active connected children
  NodeID*       childNodeIDsOpt,              ///< OUT: The optimised node IDs of the active connected children
  uint16_t&     numInvalidChildNodeIDs,       ///< OUT: The number of children node ids that are optimised away
  NodeID*       invalidChildNodeIDs )         ///< OUT: The optimised away node IDs
{
  NMP_ASSERT(childNodeWeightsAlwaysBlend);
  NMP_ASSERT(childNodeIDsAlwaysBlend);
  NMP_ASSERT(blendWeightsAlwaysBlend);
  NMP_ASSERT(blendWeightsOpt);
  NMP_ASSERT(childNodeIDsOpt);
  NMP_ASSERT(invalidChildNodeIDs);

  if (!alwaysBlendFlag)
  {
    // Compute the active set of Barycentric weights
    float weights[3];
    uint16_t count = 0;
    numInvalidChildNodeIDs = 0;
    for (uint16_t i = 0; i < 3; ++i)
    {
      // If the weight is effectively above zero we will 
      // process this node
      if (childNodeWeightsAlwaysBlend[i] > ERROR_LIMIT)
      {
        weights[count] = childNodeWeightsAlwaysBlend[i];
        childNodeIDsOpt[count] = childNodeIDsAlwaysBlend[i];
        ++count;
      }
      else
      {
        invalidChildNodeIDs[numInvalidChildNodeIDs] = childNodeIDsAlwaysBlend[i];
        ++numInvalidChildNodeIDs;
      }
    }
    NMP_ASSERT(count > 0);

    // Convert the active Barycentric weights into blend weights
    if (count == 3)
    {
      // Triangle blend - no optimisation
      numChildNodeIDsOpt = 3;
      numBlendWeightsOpt = 2;
      for (uint16_t i = 0; i < 2; ++i)
        blendWeightsOpt[i] = blendWeightsAlwaysBlend[i];
    }
    else if (count == 2)
    {
      // Edge blend - 1 node is optimised away
      numChildNodeIDsOpt = 2;
      numBlendWeightsOpt = 1;
      blendWeightsOpt[0] = NMP::clampValue(weights[1], 0.0f, 1.0f); // 1 - weights[0]
    }
    else
    {
      // Vertex
      numChildNodeIDsOpt = 1;
      numBlendWeightsOpt = 0;
    }
  }
  else
  {
    // Always blend - no optimisation.
    numInvalidChildNodeIDs = 0;
    numBlendWeightsOpt = 2;
    for (uint16_t i = 0; i < 2; ++i)
      blendWeightsOpt[i] = blendWeightsAlwaysBlend[i];
    numChildNodeIDsOpt = 3;
    for (uint16_t i = 0; i < 3; ++i)
      childNodeIDsOpt[i] = childNodeIDsAlwaysBlend[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Transforms
Task* nodeScatterBlend2DQueueBlendTransformBuffsInterpAttInterpPos(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NMP_ASSERT(net);
  const BlendOptNodeConnections* activeNodeConnections = (const BlendOptNodeConnections*)net->getActiveNodesConnections(nodeDef->getNodeID());
  NMP_ASSERT(
    activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs >= 1 &&
    activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs <= 3);

  Task* task;
  if (activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs == 3)
  {
    task = nodeBlendQueueTask(
      nodeDef,
      queue,
      net,
      dependentParameter,
      CoreTaskIDs::MR_TASKID_TRIANGLE_BLEND_TRANSFORMBUFFSINTERPATTINTERPPOS,
      ATTRIB_SEMANTIC_TRANSFORM_BUFFER,
      ATTRIB_TYPE_TRANSFORM_BUFFER,
      activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs,
      activeNodeConnections->m_trajectoryAndTransformsNodeIDs);
  }
  else if (activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs == 2)
  {
    task = nodeBlendQueueTask(
      nodeDef,
      queue,
      net,
      dependentParameter,
      CoreTaskIDs::MR_TASKID_BLEND2TRANSFORMBUFFSINTERPATTINTERPPOS,
      ATTRIB_SEMANTIC_TRANSFORM_BUFFER,
      ATTRIB_TYPE_TRANSFORM_BUFFER,
      activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs,
      activeNodeConnections->m_trajectoryAndTransformsNodeIDs);
  }
  else
  {
    task = queuePassThroughOnChildNodeID(
      activeNodeConnections->m_trajectoryAndTransformsNodeIDs[0],
      nodeDef,
      queue,
      net,
      dependentParameter);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
// Trajectory delta
Task* nodeScatterBlend2DQueueTrajectoryDeltaTransformInterpAttInterpPos(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NMP_ASSERT(net);
  const BlendOptNodeConnections* activeNodeConnections = (const BlendOptNodeConnections*)net->getActiveNodesConnections(nodeDef->getNodeID());
  NMP_ASSERT(
    activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs >= 1 &&
    activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs <= 3);

  Task* task;
  if (activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs == 3)
  {
    task = nodeBlendQueueTask(
      nodeDef,
      queue,
      net,
      dependentParameter,
      CoreTaskIDs::MR_TASKID_TRIANGLE_BLEND_TRAJECTORYDELTATRANSFORMSINTERPATTINTERPPOS,
      ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM,
      ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM,
      activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs,
      activeNodeConnections->m_trajectoryAndTransformsNodeIDs);
  }
  else if (activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs == 2)
  {
    task = nodeBlendQueueTask(
      nodeDef,
      queue,
      net,
      dependentParameter,
      CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSINTERPATTINTERPPOS,
      ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM,
      ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM,
      activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs,
      activeNodeConnections->m_trajectoryAndTransformsNodeIDs);
  }
  else
  {
    task = queuePassThroughOnChildNodeID(
      activeNodeConnections->m_trajectoryAndTransformsNodeIDs[0],
      nodeDef,
      queue,
      net,
      dependentParameter);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeScatterBlend2DQueueTrajectoryDeltaTransformInterpAttSlerpPos(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NMP_ASSERT(net);
  const BlendOptNodeConnections* activeNodeConnections = (const BlendOptNodeConnections*)net->getActiveNodesConnections(nodeDef->getNodeID());
  NMP_ASSERT(
    activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs >= 1 &&
    activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs <= 3);

  Task* task;
  if (activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs == 3)
  {
    task = nodeBlendQueueTask(
      nodeDef,
      queue,
      net,
      dependentParameter,
      CoreTaskIDs::MR_TASKID_TRIANGLE_BLEND_TRAJECTORYDELTATRANSFORMSINTERPATTSLERPPOS,
      ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM,
      ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM,
      activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs,
      activeNodeConnections->m_trajectoryAndTransformsNodeIDs);
  }
  else if (activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs == 2)
  {
    task = nodeBlendQueueTask(
      nodeDef,
      queue,
      net,
      dependentParameter,
      CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSINTERPATTSLERPPOS,
      ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM,
      ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM,
      activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs,
      activeNodeConnections->m_trajectoryAndTransformsNodeIDs);
  }
  else
  {
    task = queuePassThroughOnChildNodeID(
      activeNodeConnections->m_trajectoryAndTransformsNodeIDs[0],
      nodeDef,
      queue,
      net,
      dependentParameter);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
// Trajectory delta and transforms
Task* nodeScatterBlend2DQueueTrajectoryDeltaAndTransformsInterpPosInterpAttInterpTraj(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NMP_ASSERT(net);
  const BlendOptNodeConnections* activeNodeConnections = (const BlendOptNodeConnections*)net->getActiveNodesConnections(nodeDef->getNodeID());
  NMP_ASSERT(
    activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs >= 1 &&
    activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs <= 3);

  Task* task;
  if (activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs == 3)
  {
    task = nodeBlendQueueTask(
      nodeDef,
      queue,
      net,
      dependentParameter,
      CoreTaskIDs::MR_TASKID_TRIANGLE_BLEND_TRAJECTORYANDTRANSFORMSINTERPTRAJ,
      ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
      ATTRIB_TYPE_TRANSFORM_BUFFER,
      activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs,
      activeNodeConnections->m_trajectoryAndTransformsNodeIDs);
  }
  else if (activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs == 2)
  {
    task = nodeBlendQueueTask(
      nodeDef,
      queue,
      net,
      dependentParameter,
      CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSINTERPATTINTERPPOSINTERPTRAJ,
      ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
      ATTRIB_TYPE_TRANSFORM_BUFFER,
      activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs,
      activeNodeConnections->m_trajectoryAndTransformsNodeIDs);
  }
  else
  {
    task = queuePassThroughOnChildNodeID(
      activeNodeConnections->m_trajectoryAndTransformsNodeIDs[0],
      nodeDef,
      queue,
      net,
      dependentParameter);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
// Trajectory delta and transforms
Task* nodeScatterBlend2DQueueTrajectoryDeltaAndTransformsInterpPosInterpAttSlerpTraj(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NMP_ASSERT(net);
  const BlendOptNodeConnections* activeNodeConnections = (const BlendOptNodeConnections*)net->getActiveNodesConnections(nodeDef->getNodeID());
  NMP_ASSERT(
    activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs >= 1 &&
    activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs <= 3);

  Task* task;
  if (activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs == 3)
  {
    task = nodeBlendQueueTask(
      nodeDef,
      queue,
      net,
      dependentParameter,
      CoreTaskIDs::MR_TASKID_TRIANGLE_BLEND_TRAJECTORYANDTRANSFORMSSLERPTRAJ,
      ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
      ATTRIB_TYPE_TRANSFORM_BUFFER,
      activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs,
      activeNodeConnections->m_trajectoryAndTransformsNodeIDs);
  }
  else if (activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs == 2)
  {
    task = nodeBlendQueueTask(
      nodeDef,
      queue,
      net,
      dependentParameter,
      CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSINTERPATTINTERPPOSSLERPTRAJ,
      ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
      ATTRIB_TYPE_TRANSFORM_BUFFER,
      activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs,
      activeNodeConnections->m_trajectoryAndTransformsNodeIDs);
  }
  else
  {
    task = queuePassThroughOnChildNodeID(
      activeNodeConnections->m_trajectoryAndTransformsNodeIDs[0],
      nodeDef,
      queue,
      net,
      dependentParameter);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeScatterBlend2DQueueSampledEventsBuffers(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NMP_ASSERT(net);
  const NodeConnections* activeNodeConnections = net->getActiveNodesConnections(nodeDef->getNodeID());
  NMP_ASSERT(activeNodeConnections->m_numActiveChildNodes == 3);

  return nodeBlendQueueTask(      
    nodeDef,
    queue,
    net,
    dependentParameter,
    CoreTaskIDs::MR_TASKID_TRIANGLE_COMBINE_SAMPLEDEVENTSBUFFERS,
    ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER,
    ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER,
    activeNodeConnections->m_numActiveChildNodes,
    activeNodeConnections->m_activeChildNodeIDs);
}

//----------------------------------------------------------------------------------------------------------------------
// Update connections function for generating the sub-samples during offline compilation.
NodeID nodeScatterBlend2DOfflineUpdateConnections(
  NodeDef*           nodeDef,
  Network*           net)
{
  //------------------------
  // Calculate the blend weights
  AttribDataBlendWeights* attribBlendWeights = nodeScatterBlend2DCreateBlendWeights(nodeDef, net);
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
NodeID nodeScatterBlend2DUpdateConnections(
  NodeDef* nodeDef,
  Network* net)
{
  //------------------------
  // Calculate the blend weights
  AnimSetIndex animSet = net->getOutputAnimSetIndex(nodeDef->getNodeID());
  AttribDataBlendWeights* attribBlendWeights = nodeScatterBlend2DCreateBlendWeights(nodeDef, net);
  AttribDataScatterBlend2DState* attribDataState = nodeScatterBlend2DCreateState(nodeDef, net);
  BlendOptNodeConnections* activeNodeConnections = (BlendOptNodeConnections*)net->getActiveNodesConnections(nodeDef->getNodeID());
  AttribDataFloat* inputCPFloatX = net->updateInputCPConnection<AttribDataFloat>(nodeDef->getInputCPConnection(0), animSet);
  AttribDataFloat* inputCPFloatY = net->updateInputCPConnection<AttribDataFloat>(nodeDef->getInputCPConnection(1), animSet);
  const AttribDataScatterBlend2DDef* attribDef = nodeDef->getAttribData<AttribDataScatterBlend2DDef>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, animSet);
  const AttribDataBlendFlags* blendFlags = nodeDef->getAttribData<AttribDataBlendFlags>(ATTRIB_SEMANTIC_BLEND_FLAGS);

  // Recover the motion parameter vector. We also apply the unit scaling factor to convert
  // input control parameters in degrees to radians
  float motionParameterValue[2];
  motionParameterValue[0] = inputCPFloatX->m_value * attribDef->m_unitScale[0];
  motionParameterValue[1] = inputCPFloatY->m_value * attribDef->m_unitScale[1];

  // Compute the blend sources and weights from the desired input control
  NM_SB2D_BEGIN_PROFILING("scatterBlend2DComputeChildNodeWeightsForInputControl");
  ScatterBlend2DParameteriserInfo &result = attribDataState->m_parameteriserInfo;
  scatterBlend2DComputeChildNodeWeightsForInputControl(
    net,
    nodeDef,
    attribDef,
    motionParameterValue,
    result);
  NM_SB2D_END_PROFILING();

  // Convert the Barycentric weights into bi-linear blend weights and perform
  // any necessary blend source optimisation
  float    blendWeightsAlwaysBlend[2];
  uint16_t numBlendWeightsOpt;
  float    blendWeightsOpt[2];
  uint16_t numChildNodeIDsOpt;
  NodeID   childNodeIDsOpt[3];

  // will populate this array with node ids that have been optimised for both
  // trajectory/transforms and sampled events.
  uint16_t  numInvalidChildNodeIDs;
  NodeID    invalidChildNodeIDs[2];

  // get the blend weights to perform the optimisation check
  triangleGetBlendWeights( result.m_childNodeWeights, blendWeightsAlwaysBlend );

  // Optimise trajectory and transforms.
  triangleBlendWeightCheck( blendFlags->m_alwaysBlendTrajectoryAndTransforms,
                            result.m_childNodeWeights,
                            result.m_childNodeIDs,
                            blendWeightsAlwaysBlend,
                            numBlendWeightsOpt,
                            blendWeightsOpt,
                            numChildNodeIDsOpt,
                            childNodeIDsOpt,
                            numInvalidChildNodeIDs,
                            invalidChildNodeIDs );

  NMP_ASSERT( numChildNodeIDsOpt >= 1 && numChildNodeIDsOpt <=3 );
  NMP_ASSERT( numInvalidChildNodeIDs <=2 );
  NMP_ASSERT( (numChildNodeIDsOpt + numInvalidChildNodeIDs) == 3 );

  attribBlendWeights->setTrajectoryAndTransformsWeights(numBlendWeightsOpt, blendWeightsOpt);
  activeNodeConnections->setTrajectoryAndTransformsNodeIDs(numChildNodeIDsOpt, childNodeIDsOpt);

  //------------------------
  // Optimise sampled events.
  // as we do not have a separate event weight if the blend flags are 
  // the same we can re-use the trajectory and transforms result.
  if( blendFlags->m_alwaysBlendTrajectoryAndTransforms == blendFlags->m_alwaysCombineSampledEvents )
  {
    attribBlendWeights->setSampledEventsWeights(numBlendWeightsOpt, blendWeightsOpt);
    activeNodeConnections->setSampledEventsNodeIDs(numChildNodeIDsOpt, childNodeIDsOpt);

    //------------------------
    // If we are optimising both trajectory/transforms and events then we need to delete nodes that 
    // have both; otherwise we do not create a dependency on time and the time is not updated. 
    // The will lead to this transform source being out of sync with others connected.
    // By deleting; when the sync track is requested the node is recreated and the temporal data is
    // set correctly.
    if( blendFlags->m_alwaysBlendTrajectoryAndTransforms == false )
    {
      for( uint32_t i=0; i<numInvalidChildNodeIDs; ++i )
      {
        net->deleteNodeInstance(invalidChildNodeIDs[i]);
      }
    }
  }
  else
  {
    triangleBlendWeightCheck( blendFlags->m_alwaysCombineSampledEvents,
                              result.m_childNodeWeights,
                              result.m_childNodeIDs,
                              blendWeightsAlwaysBlend,
                              numBlendWeightsOpt,
                              blendWeightsOpt,
                              numChildNodeIDsOpt,
                              childNodeIDsOpt,
                              numInvalidChildNodeIDs,
                              invalidChildNodeIDs );

    NMP_ASSERT( numChildNodeIDsOpt >= 1 && numChildNodeIDsOpt <=3 );
    NMP_ASSERT( numInvalidChildNodeIDs <=2 );
    NMP_ASSERT( (numChildNodeIDsOpt + numInvalidChildNodeIDs) == 3 );

    attribBlendWeights->setSampledEventsWeights(numBlendWeightsOpt, blendWeightsOpt);
    activeNodeConnections->setSampledEventsNodeIDs(numChildNodeIDsOpt, childNodeIDsOpt);
  }

  // Sync event track and non sampled events are always blended
  attribBlendWeights->setEventsWeights(2, blendWeightsAlwaysBlend);

  //------------------------
  // Update the active node connections
  net->updateActiveNodeConnections( nodeDef->getNodeID(), 3, result.m_childNodeIDs );
  
  //------------------------
  // Recurse to children
  for (uint16_t i = 0; i < 3; ++i)
  {
    net->updateNodeInstanceConnections(activeNodeConnections->m_activeChildNodeIDs[i], animSet);
  }

  return nodeDef->getNodeID();
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
