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
#include "morpheme/mrRigRetargetMapping.h"
#include "morpheme/Nodes/mrNodeRetarget.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/mrCoreTaskIDs.h"

#include "NMRetarget/NMFKRetarget.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// This structure must be kept in sync with the attributes serialised by morpheme connect
struct RetargetOffsetsMessageDataPerJoint
{
  /// \name Offset transform as a quat and a vector3
  /// @{
  float offsetRotationX;
  float offsetRotationY;
  float offsetRotationZ;
  float offsetRotationW;
  float offsetTranslationX;
  float offsetTranslationY;
  float offsetTranslationZ;
  /// @}

  /// \name Reference transform as a quat only (pos assumed to be zero)
  /// @{
  float referenceRotationX;
  float referenceRotationY;
  float referenceRotationZ;
  float referenceRotationW;
  /// @}

  /// \brief The joint index (or channel number) of the joint in question
  uint32_t jointIndex;
};

//----------------------------------------------------------------------------------------------------------------------
/// This structure must be kept in sync with the attributes serialised by morpheme connect
struct RetargetScaleMessage
{
  float rigRetargetScale;
};

//----------------------------------------------------------------------------------------------------------------------
Task *nodeRetargetQueueUpdateTransforms(
    NodeDef        *node,
    TaskQueue      *queue,
    Network        *net,
    TaskParameter  *dependentParameter)
{
  AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());    
  NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);
  AnimSetIndex inputAnimSetIndex = net->getOutputAnimSetIndex(activeChildNodeID);

  if (inputAnimSetIndex == activeAnimSetIndex)
  {
    return queuePassThroughChild0(node, queue, net, dependentParameter);
  }

  Task *task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_RETARGETTRANSFORMS,
    node->getNodeID(),
    5,
    dependentParameter,
    false,
    false);

#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    FrameCount currFrameNo = net->getCurrentFrameNo();
    NMP_ASSERT(currFrameNo > 0);

    // Input transforms
    net->TaskAddParamAndDependency(
      task, 0, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, activeChildNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);

    // Output transforms
    net->TaskAddOutputParamZeroLifespan(
      task, 1, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);

    // The input rig - bind pose needed
    net->TaskAddDefInputParam(task, 2, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, inputAnimSetIndex);

    // The output rig - bind pose needed
    net->TaskAddDefInputParam(task, 3, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);

    // Retarget persistent state - including the retarget solver
    net->TaskAddNetInputParam(
      task, 4, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, node->getNodeID(),
      INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task *nodeRetargetQueueTrajectoryDeltaTransform(
  NodeDef       *node,
  TaskQueue     *queue,
  Network       *net,
  TaskParameter *dependentParameter)
{
  AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
  NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);
  AnimSetIndex inputAnimSetIndex = net->getOutputAnimSetIndex(activeChildNodeID);

  if (inputAnimSetIndex == activeAnimSetIndex)
  {
    return queuePassThroughChild0(node, queue, net, dependentParameter);
  }

  Task *task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_RETARGET_TRAJECTORY_DELTA,
    node->getNodeID(),
    7,
    dependentParameter,
    false,
    false);

#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    uint32_t currFrameNo = net->getCurrentFrameNo();
    
    NMP_ASSERT(currFrameNo > 0);

    // Input trajectory delta
    net->TaskAddParamAndDependency(
      task, 0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM,
      node->getChildNodeID(0), INVALID_NODE_ID,
      TPARAM_FLAG_INPUT|TPARAM_FLAG_OPTIONAL, currFrameNo);

    // Output trajectory delta
    net->TaskAddOutputParamZeroLifespan(
      task, 1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM,
      ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, INVALID_NODE_ID, currFrameNo);

    // The input rig
    net->TaskAddDefInputParam(task, 2, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, inputAnimSetIndex);

    // The output rig
    net->TaskAddDefInputParam(task, 3, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);

    // The input rig's retarget mapping
    net->TaskAddDefInputParam(task, 4, ATTRIB_SEMANTIC_RIG_RETARGET_MAPPING, NETWORK_NODE_ID, inputAnimSetIndex);

    // The output rig's retarget mapping
    net->TaskAddDefInputParam(task, 5, ATTRIB_SEMANTIC_RIG_RETARGET_MAPPING, NETWORK_NODE_ID, activeAnimSetIndex);

    // The rig scale for the target rig, as runtime data so it can be modified via message
    net->TaskAddOptionalNetInputParam(
      task, 6, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE_2, node->getNodeID(),
      INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeRetargetQueueTrajectoryDeltaAndTransforms(
  NodeDef       *node,
  TaskQueue     *queue,
  Network       *net,
  TaskParameter *dependentParameter)
{
  AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
  NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);
  AnimSetIndex inputAnimSetIndex = net->getOutputAnimSetIndex(activeChildNodeID);

  if (inputAnimSetIndex == activeAnimSetIndex)
  {
    return queuePassThroughChild0(node, queue, net, dependentParameter);
  }

  Task *task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_RETARGET_TRAJECTORY_DELTA_AND_TRANSFORMS,
    node->getNodeID(),
    8,
    dependentParameter,
    false,
    false);

#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    uint32_t currFrameNo = net->getCurrentFrameNo();

    NMP_ASSERT(currFrameNo > 0);

    // Input trajectory delta
    net->TaskAddParamAndDependency(
      task, 0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER,
      node->getChildNodeID(0), INVALID_NODE_ID,
      TPARAM_FLAG_INPUT|TPARAM_FLAG_OPTIONAL, currFrameNo);

    // Output trajectory delta
    net->TaskAddOutputParamZeroLifespan(
      task, 1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
      ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);

    // The input rig
    net->TaskAddDefInputParam(task, 2, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, inputAnimSetIndex);

    // The output rig
    net->TaskAddDefInputParam(task, 3, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);

    // Retarget persistent state - including the retarget solver
    net->TaskAddNetInputParam(
      task, 4, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, node->getNodeID(),
      INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);

    // The input rig's retarget mapping
    net->TaskAddDefInputParam(task, 5, ATTRIB_SEMANTIC_RIG_RETARGET_MAPPING, NETWORK_NODE_ID, inputAnimSetIndex);

    // The output rig's retarget mapping
    net->TaskAddDefInputParam(task, 6, ATTRIB_SEMANTIC_RIG_RETARGET_MAPPING, NETWORK_NODE_ID, activeAnimSetIndex);

    // The rig scale for the target rig, as runtime data so it can be modified via message
    net->TaskAddOptionalNetInputParam(
      task, 7, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE_2, node->getNodeID(),
      INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
// Update of any node data that must happen before queue building or execution.
NodeID nodeRetargetUpdateConnections(
    NodeDef           *node,
    Network           *net)
{ 
  FrameCount currFrameNo = net->getCurrentFrameNo();
  NMP_ASSERT(currFrameNo > 0);
  NetworkDef* networkDef = net->getNetworkDef();
  NodeID nodeID = node->getNodeID();

  // Output animation set index
  AnimSetIndex targetAnimSet = net->getOutputAnimSetIndex(nodeID);

  // Input animation set index
  AttribDataUInt* attribInputAnimSet =
    networkDef->getAttribData<AttribDataUInt>(
    ATTRIB_SEMANTIC_ACTIVE_ANIM_SET_INDEX,
    nodeID,
    targetAnimSet);
  AnimSetIndex inputAnimSet = (AnimSetIndex)attribInputAnimSet->m_value;
  NMP_ASSERT(attribInputAnimSet);

  if (inputAnimSet != targetAnimSet)
  {
    //------------------------
    // If not already existing, create attributes on the Network to store the output anim set.
    NodeBinEntry* networkEntry =
      net->getAttribDataNodeBinEntry(ATTRIB_SEMANTIC_CP_UINT, nodeID, INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
    AttribDataUInt* attribOutputAnimSet;
    if (!networkEntry)
    {
      AttribDataHandle handle = AttribDataUInt::create(net->getPersistentMemoryAllocator(), targetAnimSet);
      attribOutputAnimSet = (AttribDataUInt*) handle.m_attribData;
      AttribAddress attribAddress(ATTRIB_SEMANTIC_CP_UINT, nodeID, INVALID_NODE_ID, currFrameNo);
      net->addAttribData(attribAddress, handle, LIFESPAN_FOREVER);
    }
    else
    {
      networkEntry->m_address.m_validFrame = currFrameNo;
      attribOutputAnimSet = networkEntry->getAttribData<AttribDataUInt>();
    }
    NMP_ASSERT(attribOutputAnimSet);
    attribOutputAnimSet->m_value = targetAnimSet;

    //------------------------
    // Prepare the retarget solver for a change of rigs
    // Retrieve the source and target rigs
    const AttribDataRig* sourceRigAttrib = networkDef->getAttribData<AttribDataRig>(
      ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, inputAnimSet);
    const AnimRigDef* sourceRig = sourceRigAttrib->m_rig;
    const AttribDataRig* targetRigAttrib = networkDef->getAttribData<AttribDataRig>(
      ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, targetAnimSet);
    const AnimRigDef* targetRig = targetRigAttrib->m_rig;

    // Retrieve the source and target rig retarget mapping data
    const AttribDataRigRetargetMapping* sourceRigRetargetMapping =
      networkDef->getAttribData<AttribDataRigRetargetMapping>(
      ATTRIB_SEMANTIC_RIG_RETARGET_MAPPING,
      NETWORK_NODE_ID,
      inputAnimSet);
    const AttribDataRigRetargetMapping* targetRigRetargetMapping =
      networkDef->getAttribData<AttribDataRigRetargetMapping>(
      ATTRIB_SEMANTIC_RIG_RETARGET_MAPPING,
      NETWORK_NODE_ID,
      targetAnimSet);

    // Retrieve the network's solver storage stats attrib
    const AttribDataRetargetStorageStats* retargetStorageStats =
      networkDef->getAttribData<AttribDataRetargetStorageStats>(
      ATTRIB_SEMANTIC_RETARGET_STORAGE_STATS,
      NETWORK_NODE_ID);

    // Create or retrieve the node's retarget state
    AttribDataRetargetState* retargetState = net->getOptionalAttribData<AttribDataRetargetState>(
      ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
      nodeID);
    bool validState = (retargetState != 0);
    if (!validState)
    {
      AttribDataHandle handle = AttribDataRetargetState::create(
        net->getPersistentMemoryAllocator(), retargetStorageStats->m_solverStorageStats);
      retargetState = static_cast<AttribDataRetargetState*>(handle.m_attribData);
      AttribAddress attribAddress(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, nodeID, INVALID_NODE_ID, VALID_FOREVER);
      net->addAttribData(attribAddress, handle, LIFESPAN_FOREVER);
    }

    // If newly created, or if the source or target rigs have changed, prepare the solver
    if (!validState ||
      inputAnimSet != retargetState->m_sourceAnimSetIndex ||
      targetAnimSet != retargetState->m_targetAnimSetIndex)
    {
      retargetState->m_retargetSolver->prepare(
        sourceRigRetargetMapping->m_retargetParams, sourceRig->getNumBones(), sourceRig->getHierarchy(),
        targetRigRetargetMapping->m_retargetParams, targetRig->getNumBones(), targetRig->getHierarchy());
      retargetState->m_sourceAnimSetIndex = inputAnimSet;
      retargetState->m_targetAnimSetIndex = targetAnimSet;
    }
  }

  //------------------------
  // Recurse to children.
  NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);
  net->updateNodeInstanceConnections(activeChildNodeID, (AnimSetIndex)inputAnimSet);

  return node->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
bool nodeRetargetUpdateOffsetsAndScaleMessageHandler(const MR::Message& message, MR::NodeID nodeId, MR::Network* net)
{
  if (message.m_type == MESSAGE_TYPE_RETARGET_UPDATE_SCALE)
  {
    // Retrieve the node's retarget state
    AttribDataRetargetState* retargetState = net->getOptionalAttribData<AttribDataRetargetState>(
      ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
      nodeId);
    NMP_VERIFY_MSG(retargetState != 0, "Retarget offsets message handler called before node state exists.");

    // Retrieve or create the 'live' target scale parameter
    AttribDataFloat* targetScale = net->getOptionalAttribData<AttribDataFloat>(
      ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE_2,
      nodeId);
    if (!targetScale)
    {
      AttribDataHandle handle = AttribDataFloat::create(net->getPersistentMemoryAllocator(), 1.0f);
      targetScale = static_cast<AttribDataFloat*>(handle.m_attribData);
      AttribAddress attribAddress(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE_2, nodeId, INVALID_NODE_ID, VALID_FOREVER);
      net->addAttribData(attribAddress, handle, LIFESPAN_FOREVER);
    }

    // Overwrite the databuffers with message transforms
    NMP::Memory::Format scaleMessageFormat(
      sizeof(RetargetScaleMessage), NMP_NATURAL_TYPE_ALIGNMENT);
    NMP::Memory::Resource memTracker;
    memTracker.format.size = message.m_dataSize;
    memTracker.format.alignment = scaleMessageFormat.alignment;
    memTracker.ptr = const_cast<void*>(message.m_data);
    if (memTracker.format.size == scaleMessageFormat.size)
    {
      memTracker.align(scaleMessageFormat);
      RetargetScaleMessage* scaleMessage = (RetargetScaleMessage*)memTracker.ptr;

      retargetState->m_retargetSolver->overwriteTargetScale(scaleMessage->rigRetargetScale);

      // Set the live target scale - used by the trajectory update task
      targetScale->m_value = scaleMessage->rigRetargetScale;
    }
    else
    {
      NMP_VERIFY_FAIL("Retarget scale message incorrectly formed - incorrect data size.")
    }
  }
  else if (message.m_type == MESSAGE_TYPE_RETARGET_UPDATE_OFFSETS)
  {
  #if NM_ENABLE_EXCEPTIONS || defined(NMP_ENABLE_ASSERTS)
    // Retrieve the network definition
    const NetworkDef* networkDef = net->getNetworkDef();

    // Get the output animation set index
    AnimSetIndex outputAnimSet = net->getOutputAnimSetIndex(nodeId);

    // Retrieve the target rig and get its length
    const AttribDataRig* targetRigAttrib =  networkDef->getAttribData<AttribDataRig>(
      ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, outputAnimSet);
    const AnimRigDef* targetRig = targetRigAttrib->m_rig;
    const uint32_t numJoints = targetRig->getNumBones();
  #endif

    // Retrieve the node's retarget state
    AttribDataRetargetState* retargetState = net->getOptionalAttribData<AttribDataRetargetState>(
      ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
      nodeId);
    NMP_VERIFY_MSG(retargetState != 0, "Retarget offsets message handler called before node state exists.");

    // Overwrite the databuffers with message transforms
    NMP::Memory::Format offsetMessageOneJointFormat(
      sizeof(RetargetOffsetsMessageDataPerJoint), NMP_NATURAL_TYPE_ALIGNMENT);
    NMP::Memory::Resource memTracker;
    memTracker.format.size = message.m_dataSize;
    memTracker.format.alignment = offsetMessageOneJointFormat.alignment;
    memTracker.ptr = const_cast<void*>(message.m_data);
    while (memTracker.format.size >= offsetMessageOneJointFormat.size)
    {
      memTracker.align(offsetMessageOneJointFormat);
      RetargetOffsetsMessageDataPerJoint* newOffsets = (RetargetOffsetsMessageDataPerJoint*)memTracker.ptr;

      // Convert message floats to transforms
      NMP::PosQuat offsetTransform;
      NMP::PosQuat referenceTransform;
      offsetTransform.m_quat = NMP::Quat(
        newOffsets->offsetRotationX, newOffsets->offsetRotationY,
        newOffsets->offsetRotationZ, newOffsets->offsetRotationW);
      offsetTransform.m_pos = NMP::Vector3(
        newOffsets->offsetTranslationX, newOffsets->offsetTranslationY, newOffsets->offsetTranslationZ);

      referenceTransform.m_quat = NMP::Quat(
        newOffsets->referenceRotationX, newOffsets->referenceRotationY,
        newOffsets->referenceRotationZ, newOffsets->referenceRotationW);
      referenceTransform.m_pos = NMP::Vector3Zero();

      // Insert into databuffers
      uint32_t& j = newOffsets->jointIndex;
      NMP_VERIFY_MSG(j < numJoints, "Retarget offsets message incorrectly formed - illegal joint index.");

      // Insert the new offset transforms into the retarget state
      retargetState->m_retargetSolver->overwriteTargetOffsetMP(j, offsetTransform, referenceTransform);

      // Move to next joint in message
      memTracker.increment(offsetMessageOneJointFormat);
    }

    // Check that we used up all the message data - this means that the data was correctly formed,
    // consisting of a whole number of per-joint message data objects.
    NMP_VERIFY_MSG(memTracker.format.size == 0, "Retarget offsets message incorrectly formed - unused data left over.")
  }
  else
  {
    NMP_VERIFY_FAIL("Retarget offsets: unexpected message type.");
  }

  return true;
}


} // namespace MR

//----------------------------------------------------------------------------------------------------------------------

