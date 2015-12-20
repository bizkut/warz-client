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
#include "morpheme/Nodes/mrNodeTransit.h"
#include "physics/Nodes/mrNodePhysicsTransit.h"
#include "morpheme/mrCoreTaskIDs.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/mrBlendOps.h"
#include "physics/mrPhysics.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
Task* nodeTransitQueueFeatherBlend2ToPhysicsTransformBuffs(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_FEATHERBLENDTOPHYSICSTRANSFORMBUFFS,
                 node->getNodeID(),
                 6,
                 dependentParameter,
                 false,
                 false);
  if (task)
  {
    FrameCount currFrameNo = net->getCurrentFrameNo();
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    NMP_ASSERT(currFrameNo > 0);

    NodeID activeChildNodeID0 = net->getActiveChildNodeID(node->getNodeID(), 0);
    NodeID activeChildNodeID1 = net->getActiveChildNodeID(node->getNodeID(), 1);

    net->TaskAddNetInputParam(task, 0, ATTRIB_SEMANTIC_DEAD_BLEND_STATE, node->getNodeID(), INVALID_NODE_ID, VALID_FOREVER);

    net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, activeChildNodeID0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddParamAndDependency(task, 2, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, activeChildNodeID1, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);

    net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_BLEND_WEIGHTS, ATTRIB_TYPE_BLEND_WEIGHTS, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);

    net->TaskAddDefInputParam(task, 4, ATTRIB_SEMANTIC_ANIM_TO_PHYSICS_MAP, NETWORK_NODE_ID, net->getOutputAnimSetIndex(node->getNodeID()));

    net->TaskAddOutputParamZeroLifespan(task, 5, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodePhysicsTransitQueueTransformBuffs(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  // We need to decide what type of blend to perform.
  if (isDeadBlending(node, net))
  {
    // Dead Blend
    return nodeQueuePhysicalDeadBlendUpdateTransforms(
             node,
             queue,
             net,
             dependentParameter);
  }
  else if (isBlendingToPhysics(node, net))
  {
    AttribDataDeadBlendDef*   deadBlendDef   = node->getAttribData<AttribDataDeadBlendDef>(ATTRIB_SEMANTIC_DEAD_BLEND_DEF);
    AttribDataDeadBlendState* deadBlendState = getDeadBlendState(node, net);
    // We currently support the default deadblend only.
    NMP_ASSERT(deadBlendDef && deadBlendState);

    // Check to see if there's an anim to physics rig map.
    MR::AttribDataHandle* physicsRigMapAttrData = net->getNetworkDef()->getOptionalAttribDataHandle(
                                                                            ATTRIB_SEMANTIC_ANIM_TO_PHYSICS_MAP,
                                                                            NETWORK_NODE_ID,
                                                                            net->getOutputAnimSetIndex(node->getNodeID()));

    // Here the user decides with a static attribute if a blend2 transition
    // is desired on the physical bones or on the non physical ones only (feather blend)
    // Note that we default to a normal blend if there's no physics rig in the active anim set.
    if (deadBlendDef->m_blendToDestinationPhysicsBones || physicsRigMapAttrData == NULL)
    {
      // Standard blend 2
      return nodeTransitQueueBlend2TransformBuffs(
               node,
               queue,
               net,
               dependentParameter,
               deadBlendState->m_blendMode);
    }
    else
    {
      return nodeTransitQueueFeatherBlend2ToPhysicsTransformBuffs(
               node,
               queue,
               net,
               dependentParameter);
    }
  }
  else
  {
    // Look up for blending attributes in the transition definition.
    AttribDataHandle* attrib = net->getNetworkDef()->getAttribDataHandle(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, node->getNodeID());
    NMP_ASSERT(attrib && attrib->m_attribData);
    NMP_ASSERT(attrib->m_attribData->getType() == ATTRIB_TYPE_TRANSIT_DEF);

    AttribDataTransitDef* transitDef = (AttribDataTransitDef*)attrib->m_attribData;
    return nodeTransitQueueBlend2TransformBuffs(
             node,
             queue,
             net,
             dependentParameter,
             transitDef->m_blendMode);
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodePhysicsTransitSyncEventsQueueTransformBuffs(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  // We need to decide what type of blend to perform.
  if (isDeadBlending(node, net))
  {
    // Dead Blend
    return nodeQueuePhysicalDeadBlendUpdateTransforms(
             node,
             queue,
             net,
             dependentParameter);
  }
  else if (isBlendingToPhysics(node, net))
  {
    AttribDataDeadBlendDef*   deadBlendDef   = node->getAttribData<AttribDataDeadBlendDef>(ATTRIB_SEMANTIC_DEAD_BLEND_DEF);
    AttribDataDeadBlendState* deadBlendState = getDeadBlendState(node, net);
    // We currently support the default deadblend only.
    NMP_ASSERT(deadBlendDef && deadBlendState);

    // Check to see if there's an anim to physics rig map.
    MR::AttribDataHandle* physicsRigMapAttrData = net->getNetworkDef()->getOptionalAttribDataHandle(
                                                                          ATTRIB_SEMANTIC_ANIM_TO_PHYSICS_MAP,
                                                                          NETWORK_NODE_ID,
                                                                          net->getOutputAnimSetIndex(node->getNodeID()));

    // Here the user decides with a static attribute if a blend2 transition
    // is desired on the physical bones or on the non physical ones only (feather blend)
    // Note that we default to a normal blend if there's no physics rig in the active anim set.
    if (deadBlendDef->m_blendToDestinationPhysicsBones || physicsRigMapAttrData == NULL)
    {
      // Standard blend 2
      return nodeTransitQueueBlend2TransformBuffs(
               node,
               queue,
               net,
               dependentParameter,
               deadBlendState->m_blendMode);
    }
    else
    {
      return nodeTransitQueueFeatherBlend2ToPhysicsTransformBuffs(
               node,
               queue,
               net,
               dependentParameter);
    }
  }
  else
  {
    // Look up for blending attributes in the transition definition.
    AttribDataHandle* attrib = net->getNetworkDef()->getAttribDataHandle(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, node->getNodeID());
    NMP_ASSERT(attrib && attrib->m_attribData);
    NMP_ASSERT(attrib->m_attribData->getType() == ATTRIB_TYPE_TRANSIT_SYNC_EVENTS_DEF);
    AttribDataTransitSyncEventsDef* transitDef = (AttribDataTransitSyncEventsDef*)attrib->m_attribData;

    return nodeTransitQueueBlend2TransformBuffs(
             node,
             queue,
             net,
             dependentParameter,
             transitDef->m_blendMode);
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeQueuePhysicalDeadBlendUpdateTransforms(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  MR::PhysicsRig* physicsRig = getPhysicsRig(net);
  bool physicallyLimitDeadBlend = physicsRig && (physicsRig->getNumJoints() > 0);

  uint32_t numTaskParameters = (physicallyLimitDeadBlend ? 10 : 8);
  // Queue the task that computes the dead blend on the transforms.
  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_PHYSICALDEADBLENDTRANSFORMBUFFS,
                 node->getNodeID(),
                 numTaskParameters,
                 dependentParameter,
                 false,
                 false);
  if (task)
  {
    FrameCount currFrameNo = net->getCurrentFrameNo();
    uint16_t activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    NMP_ASSERT(currFrameNo > 0);
    NodeID activeParentNodeID = net->getActiveParentNodeID(node->getNodeID());

    // Build list of all parameters (attributes) that are needed for the task to operate.
    // Also build a list of attribute addresses that we are dependent on having been updated before us.

    // Destination node transforms we want to blend with.
    NodeID activeChildNodeID0 = net->getActiveChildNodeID(node->getNodeID(), 0);
    net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, activeChildNodeID0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);

    // Dead blend state data.
    net->TaskAddNetInputParam(task, 1, ATTRIB_SEMANTIC_DEAD_BLEND_STATE, node->getNodeID(), INVALID_NODE_ID, VALID_FOREVER);

    // Dead blend def data.
    net->TaskAddDefInputParam(task, 2, ATTRIB_SEMANTIC_DEAD_BLEND_DEF, node->getNodeID());

    // Use the delta time to compute the dead blend extrapolation.
    net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_UPDATE_TIME_POS, ATTRIB_TYPE_UPDATE_PLAYBACK_POS, activeParentNodeID, node->getNodeID(), TPARAM_FLAG_INPUT, currFrameNo);

    // Use the weighting value from the transition update time.
    net->TaskAddParamAndDependency(task, 4, ATTRIB_SEMANTIC_BLEND_WEIGHTS, ATTRIB_TYPE_BLEND_WEIGHTS, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);

    // Dead blend transform rates. Use "0" as target node to make the address unique.
    net->TaskAddNetInputOutputParam(task, 5, ATTRIB_SEMANTIC_TRANSFORM_RATES, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);

    // Dead blend data which includes the current transforms (blended during the transition).
    net->TaskAddNetInputOutputParam(task, 6, ATTRIB_SEMANTIC_DEAD_BLEND_TRANSFORMS_STATE, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);

    // The dead blend transition outputs transforms.
    net->TaskAddOutputParamZeroLifespan(task, 7, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);

    // Optional physics rig parameters
    if (physicallyLimitDeadBlend)
    {
      // Physics rig
      net->TaskAddOptionalNetInputParam(task, 8, ATTRIB_SEMANTIC_PHYSICS_RIG, NETWORK_NODE_ID, INVALID_NODE_ID, currFrameNo);
      // Animation to physics map information.
      net->TaskAddOptionalDefInputParam(task, 9, ATTRIB_SEMANTIC_ANIM_TO_PHYSICS_MAP, NETWORK_NODE_ID, activeAnimSetIndex);
    }
  }

  return task;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
