// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrNodeBlendAll.h"
#include "morpheme/mrCoreTaskIDs.h"
//----------------------------------------------------------------------------------------------------------------------

#define MR_NODE_BLEND_ALL_FORCEINLINE NM_FORCEINLINE

namespace MR
{
//----------------------------------------------------------------------------------------------------------------------
MR_NODE_BLEND_ALL_FORCEINLINE
NodeID nodeBlendAllGetNodeIDIfOnlyOneActive(NodeDef* node, Network* net)
{
  // Helper function to test if the only input is active
  // If so, return the only active node so its transforms can be passed straight through.
  // This avoids doing an unnecessary blend.

  NodeID onlyActiveNode = (net->getNumActiveChildren(node->getNodeID()) == 1) ? 
    net->getActiveChildNodeID(node->getNodeID(), 0) : INVALID_NODE_ID;

  return onlyActiveNode;
}

//----------------------------------------------------------------------------------------------------------------------
MR_NODE_BLEND_ALL_FORCEINLINE
static Task* nodeBlendAllQueueTask(
  TaskID             taskID,
  AttribDataSemantic taskDataSemantic,
  AttribDataType     attribType,
  NodeDef*           node,
  TaskQueue*         queue,
  Network*           net,
  TaskParameter*     dependentParameter,
  bool               managesOwnDMA)
{
  NodeConnections* nodeConnections = net->getActiveNodesConnections(node->getNodeID());
  NMP_ASSERT(nodeConnections);

  Task* task = queue->createNewTaskOnQueue(
                 taskID,
                 node->getNodeID(),
                 nodeConnections->m_numActiveChildNodes + 2,
                 dependentParameter, 
                 false, 
                 true,   
                 managesOwnDMA);  // Manages its own DMA

#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    FrameCount currFrameNo = net->getCurrentFrameNo();
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    NMP_ASSERT(currFrameNo > 0);

    net->TaskAddOutputParamZeroLifespan(task, 0, taskDataSemantic, attribType, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);

    uint32_t param = 0;
    // inputs
    for (uint32_t i = 0; i < nodeConnections->m_numActiveChildNodes; i++)
    {
      net->TaskAddParamAndDependency(
        task,
        ++param,
        taskDataSemantic,
        attribType,
        nodeConnections->m_activeChildNodeIDs[i],
        INVALID_NODE_ID,
        TPARAM_FLAG_INPUT,
        currFrameNo); 
    }

    // Array of normalised blend weights
    net->TaskAddNetInputParam(task, ++param, ATTRIB_SEMANTIC_CP_FLOAT, node->getNodeID(), INVALID_NODE_ID, currFrameNo);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlendAllQueueBlendTransformBuffsInterpAttInterpPos(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  const NodeID activeNode = nodeBlendAllGetNodeIDIfOnlyOneActive(node, net);

  if (activeNode != INVALID_NODE_ID)
  {
    return queuePassThroughOnChildNodeID(
      activeNode,
      node,
      queue,
      net,
      dependentParameter);
  }
  else
  {
    return nodeBlendAllQueueTask(
           CoreTaskIDs::MR_TASKID_BLENDALLTRANSFORMBUFFSINTERPATTINTERPPOS,
           ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER,
           node, queue, net, dependentParameter, true);
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlendAllQueueSampledEventsBuffers(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeBlendAllQueueTask(
           CoreTaskIDs::MR_TASKID_COMBINEALLSAMPLEDEVENTSBUFFERS,
           ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER,
           node, queue, net, dependentParameter, false);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlendAllQueueTrajectoryDeltaTransformInterpAttInterpPos(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  const NodeID activeNode = nodeBlendAllGetNodeIDIfOnlyOneActive(node, net);

  if (activeNode != INVALID_NODE_ID)
  {
    return queuePassThroughOnChildNodeID(
      activeNode,
      node,
      queue,
      net,
      dependentParameter);
  }
  else
  {
    return nodeBlendAllQueueTask(
      CoreTaskIDs::MR_TASKID_BLENDALLTRAJECTORYDELTATRANSFORMSINTERPATTINTERPPOS,
      ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM,
      node, queue, net, dependentParameter, false);
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlendAllQueueTrajectoryDeltaTransformInterpAttSlerpPos(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  const NodeID activeNode = nodeBlendAllGetNodeIDIfOnlyOneActive(node, net);

  if (activeNode != INVALID_NODE_ID)
  {
    return queuePassThroughOnChildNodeID(
      activeNode,
      node,
      queue,
      net,
      dependentParameter);
  }
  else
  {
    return nodeBlendAllQueueTask(
      CoreTaskIDs::MR_TASKID_BLENDALLTRAJECTORYDELTATRANSFORMSINTERPATTSLERPPOS,
      ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM,
      node, queue, net, dependentParameter, false);
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlendAllQueueTrajectoryDeltaAndTransformsSlerpTraj(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  const NodeID activeNode = nodeBlendAllGetNodeIDIfOnlyOneActive(node, net);

  if (activeNode != INVALID_NODE_ID)
  {
    return queuePassThroughOnChildNodeID(
      activeNode,
      node,
      queue,
      net,
      dependentParameter);
  }
  else
  {
    return nodeBlendAllQueueTask(
      CoreTaskIDs::MR_TASKID_BLENDALLTRAJECTORYDELTAANDTRANSFORMSSLERPPOS,
      ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER,
      node, queue, net, dependentParameter, true);
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlendAllQueueTrajectoryDeltaAndTransformsInterpTraj(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  const NodeID activeNode = nodeBlendAllGetNodeIDIfOnlyOneActive(node, net);

  if (activeNode != INVALID_NODE_ID)
  {
    return queuePassThroughOnChildNodeID(
      activeNode,
      node,
      queue,
      net,
      dependentParameter);
  }
  else
  {
    return nodeBlendAllQueueTask(
      CoreTaskIDs::MR_TASKID_BLENDALLTRAJECTORYDELTAANDTRANSFORMSINTERPPOS,
      ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER,
      node, queue, net, dependentParameter, true);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void normaliseWeights(
  AttribDataFloat**     blendCP,
  const uint16_t        numConnections,
  AttribDataFloatArray* attribBlendWeights,
  float                 totalWeight)
{
  uint32_t index = 0;
  if (totalWeight == 0.0f)
  {
    totalWeight = 1.0f / (float)numConnections;
    for (int32_t i = 0; i < numConnections; ++i)
    {
      attribBlendWeights->m_values[i] = totalWeight;
    }
  }
  else
  {
    for (int32_t i = 0; i < numConnections; ++i)
    {
      if (blendCP[i]->m_value!= 0.0f)
      {
        attribBlendWeights->m_values[index] = blendCP[i]->m_value / totalWeight;
        ++index;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Update connections function.
NodeID nodeBlendAllUpdateConnections(
  NodeDef* node,
  Network* net)
{
  FrameCount currFrameNo = net->getCurrentFrameNo();
  NMP_ASSERT(currFrameNo > 0);
  const AnimSetIndex outputSetIndex = net->getOutputAnimSetIndex(node->getNodeID());

  // Get temp allocator.
  NMP::TempMemoryAllocator* tempAllocator = net->getTempMemoryAllocator();
  NMP_ASSERT(tempAllocator);
  NMP::TempMemoryAllocator* childAllocator = tempAllocator->createChildAllocator();

  //------------------------
  // Calculate number of connected pins
  uint16_t numChildrenTotal = node->getNumChildNodes();
    
  //------------------------
  // Get these nodes connections structure to fill in.
  NodeConnections* nodeConnections = net->getActiveNodesConnections(node->getNodeID());
  NMP_ASSERT(nodeConnections);

  //------------------------
  // Cache the last frames active child list before updating.
  uint32_t numActiveChildrenLastUpdate = nodeConnections->m_numActiveChildNodes;
  NMP::Memory::Format memReq(sizeof(NodeID) * numActiveChildrenLastUpdate, NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::Resource memRes = NMPAllocatorAllocateFromFormat(childAllocator, memReq);
  NodeID* activeChildrenLastUpdate = (NodeID*) memRes.ptr;
  for (uint32_t i = 0; i < numActiveChildrenLastUpdate; ++i)
  {
    activeChildrenLastUpdate[i] = nodeConnections->m_activeChildNodeIDs[i];
  }

  //------------------------
  // Update the connected control parameters.
  memReq.set(sizeof(AttribDataFloat*) * numChildrenTotal, NMP_NATURAL_TYPE_ALIGNMENT);
  memRes = NMPAllocatorAllocateFromFormat(childAllocator, memReq);
  AttribDataFloat** blendCP = (AttribDataFloat**)memRes.ptr;
  float totalWeight = 0.0f;
  
  uint16_t numActiveSources = 0;
  for (uint32_t i = 0; i < numChildrenTotal; ++i)
  {
    blendCP[i] = net->updateInputCPConnection<AttribDataFloat>(node->getInputCPConnection((MR::PinIndex)i), outputSetIndex);
    if (blendCP[i]->m_value != 0.0f)
    {
      nodeConnections->m_activeChildNodeIDs[numActiveSources] = node->getChildNodeID(i);
      totalWeight += blendCP[i]->m_value;
      ++numActiveSources;
    }
  }

  // If every blendWeight has value 0.0f it means that the sources will be evenly distributed.
  // Therefore each blendWeight = 1.0f/numConnections
  if (numActiveSources == 0)
  {
    for (uint32_t i = 0; i < numChildrenTotal; ++i)
    {
      nodeConnections->m_activeChildNodeIDs[i] = node->getChildNodeID(i);
    }
    numActiveSources = numChildrenTotal;
  }
  nodeConnections->m_numActiveChildNodes = numActiveSources;

  //------------------------
  // If not already existing, create an attribute on the Network to store the normalised array of cp weights.
  NodeBinEntry* networkEntry = net->getAttribDataNodeBinEntry(ATTRIB_SEMANTIC_CP_FLOAT, node->getNodeID(), INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
  AttribDataFloatArray* attribBlendWeights;
  if (!networkEntry)
  {
    AttribDataHandle handle = AttribDataFloatArray::create(net->getPersistentMemoryAllocator(), (uint32_t)numChildrenTotal);
    attribBlendWeights = (AttribDataFloatArray*) handle.m_attribData;
    AttribAddress attribAddess(ATTRIB_SEMANTIC_CP_FLOAT, node->getNodeID(), INVALID_NODE_ID, currFrameNo);
    net->addAttribData(attribAddess, handle, LIFESPAN_FOREVER);
  }
  else
  {
    networkEntry->m_address.m_validFrame = currFrameNo;
    attribBlendWeights = networkEntry->getAttribData<AttribDataFloatArray>();
  }
  NMP_ASSERT(attribBlendWeights);
  normaliseWeights(blendCP, numChildrenTotal, attribBlendWeights, totalWeight);

  //--------------------------------------------
  // Delete any child nodes that have become inactive this frame.
  bool becomeInactive = true;
  for (uint32_t i = 0; i < numActiveChildrenLastUpdate; i++)
  {
    for (uint32_t j = 0; j < numActiveSources; j++)
    {
      if (activeChildrenLastUpdate[i] == nodeConnections->m_activeChildNodeIDs[j])
      {
        becomeInactive = false;
        break;     
      }
    }
    if (becomeInactive)
    {
      net->deleteNodeInstance(activeChildrenLastUpdate[i]);
    }
    becomeInactive = true;
  }

  //--------------------------------------------
  // Release working memory
  childAllocator->memFree(activeChildrenLastUpdate);
  childAllocator->memFree(blendCP);
  tempAllocator->destroyChildAllocator(childAllocator);

  //--------------------------------------------
  // Recurse to children.
  for (uint32_t i = 0; i < numActiveSources; i++)
  {
    net->updateNodeInstanceConnections(nodeConnections->m_activeChildNodeIDs[i], outputSetIndex);
  }

  return node->getNodeID();
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
