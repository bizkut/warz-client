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
#include "Morpheme/Nodes/mrNodeScaleCharacter.h"
#include "morpheme/mrRigRetargetMapping.h"
#include "morpheme/mrCoreTaskIDs.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
Task* nodeScaleCharacterQueueUpdateTransforms(
  NodeDef        *node,
  TaskQueue      *queue,
  Network        *net,
  TaskParameter  *dependentParameter)
{
  Task *task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_SCALECHARACTERTRANSFORMS,
    node->getNodeID(),
    3,
    dependentParameter);

#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    FrameCount currFrameNo = net->getCurrentFrameNo();
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    NMP_ASSERT(currFrameNo > 0);

    // Input and output transform buffers
    NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);
    net->TaskAddParamAndDependency(
      task, 0, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, activeChildNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddOutputParamZeroLifespan(
      task, 1, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);

    // The node's state data, storing the rig and joint scales.
    net->TaskAddOptionalNetInputParam(
      task, 2, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, node->getNodeID(),
      INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
  }
  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeScaleCharacterQueueTrajectoryDeltaTransform(
  NodeDef        *node,
  TaskQueue      *queue,
  Network        *net,
  TaskParameter  *dependentParameter)
{
  Task *task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_SCALECHARACTERTRAJECTORYDELTA,
    node->getNodeID(),
    4,
    dependentParameter);

#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
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

    // The rig - only need this to get the trajectory bone index
    net->TaskAddDefInputParam(task, 2, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);

    // The node's state data, storing the rig and joint scales.
    net->TaskAddOptionalNetInputParam(
      task, 3, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, node->getNodeID(),
      INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
  }
  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeScaleCharacterQueueTrajectoryDeltaAndTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task *task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_SCALECHARACTERTRAJECTORYDELTA_AND_TRANSFORMS,
    node->getNodeID(),
    4,
    dependentParameter);

#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
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

    // The rig - only need this to get the trajectory bone index
    net->TaskAddDefInputParam(task, 2, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);

    // The node's state data, storing the rig and joint scales.
    net->TaskAddOptionalNetInputParam(
      task, 3, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, node->getNodeID(),
      INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
  }
  return task;
}

//----------------------------------------------------------------------------------------------------------------------
// Update of any node data that must happen before queue building or execution.
NodeID nodeScaleCharacterUpdateConnections(
  NodeDef* node,
  Network* net)
{
  NMP_ASSERT(net->getCurrentFrameNo() > 0);

  // Create the node's state data if it doesn't exist
  AttribDataScaleCharacterState* state = net->getOptionalAttribData<AttribDataScaleCharacterState>(
    ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
    node->getNodeID());
  if (!state)
  {
    // Calculate the maximum number of bones that might be received in a scale message.
    uint32_t maxNumBones = 0;
    AnimSetIndex animSetCount = net->getNetworkDef()->getNumAnimSets();
    for (AnimSetIndex animSetIndex = 0; animSetIndex < animSetCount; ++animSetIndex)
    {
      AttribDataRig* rigAttrib = net->getNetworkDef()->getAttribData<AttribDataRig>(
        ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, animSetIndex);
      AnimRigDef* rigDef = rigAttrib->m_rig;

      maxNumBones = NMP::maximum(maxNumBones, rigDef->getNumBones());
    }

    AttribDataArrayCreateDesc createDesc(maxNumBones);
    AttribDataHandle handle = AttribDataScaleCharacterState::create(
      net->getPersistentMemoryAllocator(), &createDesc);
    state = static_cast<AttribDataScaleCharacterState*>(handle.m_attribData);
    AttribAddress attribAddress(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, node->getNodeID(), INVALID_NODE_ID, VALID_FOREVER);
    net->addAttribData(attribAddress, handle, LIFESPAN_FOREVER);
  }

  // Recurse to children.
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());
  NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);
  net->updateNodeInstanceConnections(activeChildNodeID, animSet);

  return node->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
bool nodeScaleCharacterMessageHandler(const MR::Message& message, MR::NodeID nodeId, MR::Network* net)
{
  NMP_VERIFY_MSG(message.m_type == MESSAGE_TYPE_SCALE_CHARACTER, "Scale character: unexpected message type.");
  NMP_VERIFY_MSG(message.m_dataSize > sizeof(bool), "Scale character: invalid message size.");

  if (net->nodeIsActive(nodeId))
  {
    // Retrieve the rig (which we need in order to determine the trajectory bone index and rig size)
    const AttribDataRig* rigAttrib =  net->getNetworkDef()->getAttribData<AttribDataRig>(
      ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, net->getOutputAnimSetIndex(nodeId));
    AnimRigDef* rigDef = rigAttrib->m_rig;
    uint32_t trajectoryIndex = rigDef->getTrajectoryBoneIndex();
    uint32_t numJoints = rigDef->getNumBones();

    // Retrieve the scale character state containing all the scales
    AttribDataScaleCharacterState* state = net->getAttribData<AttribDataScaleCharacterState>(
      ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, nodeId, INVALID_NODE_ID, VALID_FRAME_ANY_FRAME, ANIMATION_SET_ANY);
    NMP_ASSERT(state); // Fails if you call the handler before update connections

    // Get a memory resource for stepping through the message memory in a well-defined way
    NMP::Memory::Resource memTracker;
    memTracker.format.size = message.m_dataSize;
    memTracker.format.alignment = 4; // This message has floats in it but nothing more complex
    memTracker.ptr = const_cast<void*>(message.m_data);

    // Determine whether or not this is a full message with scales for every joint, or one with only
    // rig and trajectory scales.
    NMP::Memory::Format format(sizeof(bool), 1);
    memTracker.align(format); // This won't do anything here but it's good practice to do this properly
    bool advancedMessage = *(bool*)memTracker.ptr;
    memTracker.increment(format);

    // Get the array of scale values, which will either be numJoints long (advanced message) or two
    // floats long (simple message)
    format.set(sizeof(float), 4);
    memTracker.align(format);
    float* scales = (float*)memTracker.ptr;

    if (advancedMessage)
    {
      format *= numJoints;
      if(memTracker.format.size == format.size)
      {
        // Reset the rig scale to 1 and then set all the joint scales separately
        memcpy(state->m_jointScale, scales, format.size);
      }
      else
      {
        NMP_MSG("Scale character: invalid message size.");
      }
    }
    else
    {
      format *= 2;
      NMP_VERIFY_MSG(memTracker.format.size == format.size, "Scale character: invalid message size.");

      for (uint32_t j = 0; j < numJoints; ++j)
      {
        state->m_jointScale[j] = scales[1];
      }
      state->m_jointScale[trajectoryIndex] = scales[0];
    }

    // Check we used up the whole message and no more
    memTracker.increment(format);
    if(memTracker.format.size != 0)
    {
      NMP_MSG("Scale character: invalid message size.");
    }

    return true;
  }

  // return false if the node wasn't active.
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void endianSwapScaleCharacterMessage(void* data, uint32_t dataSize)
{
  // The first byte of the message contains the per joint flag with the remaining memory storing floats of scale values

  // Get a memory resource for stepping through the message memory in a well-defined way
  NMP::Memory::Resource memTracker;
  memTracker.format.size = dataSize;
  memTracker.format.alignment = 4; // This message has floats in it but nothing more complex
  memTracker.ptr = data;

  // Determine whether or not this is a full message with scales for every joint, or one with only
  // rig and trajectory scales.
  bool* perJoint = (bool*)data;

  memTracker.increment(NMP::Memory::Format(sizeof(bool), NMP_NATURAL_TYPE_ALIGNMENT));
  if (*perJoint)
  {
    float* scales = (float*)memTracker.ptr;
    uint32_t numFloats = (uint32_t)(dataSize - NMP::Memory::align(sizeof(bool), NMP_NATURAL_TYPE_ALIGNMENT)) / sizeof(float);
    NMP::endianSwapArray(scales, numFloats, sizeof(float));
  }
  else
  {
    NMP::endianSwap(memTracker.ptr, sizeof(float));
    memTracker.increment(sizeof(float));
    memTracker.align(NMP_NATURAL_TYPE_ALIGNMENT); 
    NMP::endianSwap(memTracker.ptr, sizeof(float));
  }
 
}

MR::Message* createScaleCharacterMessage(NMP::Memory::Resource& memResource, MessageID messageID, bool advancedMessage, float* scaleValues, uint32_t numScaleValues)
{
  // Check the memory resource is large enough to accommodate the number of scale values passed in
  NMP_ASSERT_MSG( ((memResource.format.size - (sizeof(MR::Message) + sizeof(bool))) / sizeof(float)) == numScaleValues,
    "Number of scale values requested is too many for memory resource");

  NMP::Memory::Format memReqs(sizeof(MR::Message), NMP_NATURAL_TYPE_ALIGNMENT);

  //------------------------
  // Construct the message
  MR::Message* message = (MR::Message*) memResource.alignAndIncrement(memReqs);
  message->m_dataSize = (uint32_t) NMP::Memory::align((numScaleValues * sizeof(float)) + sizeof(bool), NMP_NATURAL_TYPE_ALIGNMENT);
  message->m_id = messageID;
  message->m_type = MESSAGE_TYPE_SCALE_CHARACTER;
  message->m_status = true;

  //------------------------
  // Construct the message data

  // Write the per joint flag (advanced message) into the first byte of the message.
  memReqs.set(sizeof(bool), NMP_NATURAL_TYPE_ALIGNMENT);
  message->m_data = memResource.alignAndIncrement(memReqs);
  *(bool*)message->m_data = advancedMessage;

  // Write the floats into the remaining message memory
  memReqs.set(sizeof(float) * numScaleValues, NMP_NATURAL_TYPE_ALIGNMENT);
  float* perJointScales = (float*)memResource.alignAndIncrement(memReqs);

  if(advancedMessage)
  {
    // Copy our scale values directly into our message
    memcpy(perJointScales, scaleValues, memReqs.size);
  }
  else
  {
    // We are only interested in the trajectory scale and a single bone scaling value
    perJointScales[0] = scaleValues[0];
    perJointScales[1] = scaleValues[1];
  }

  return message;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------

