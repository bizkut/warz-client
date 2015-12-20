
//----------------------------------------------------------------------------------------------------------------------
#include "Morpheme/Nodes/mrNodeEmitMessageOnDiscreteEvent.h"
#include "Morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/mrCoreTaskIDs.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
Task* nodeEmitMessageOnDiscreteEventQueueSampledEventsBuffers(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NodeID nodeID = node->getNodeID();
  NodeConnections* nodeConnections = net->getActiveNodesConnections(nodeID);
  NMP_ASSERT(nodeConnections);
  NodeID activeChildNodeID = nodeConnections->m_activeChildNodeIDs[0];
  NMP_ASSERT(activeChildNodeID != INVALID_NODE_ID);

  // Examine sampled events buffer and pass through.
  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_SAMPLEDEVENTSBUFFEREMITMESSAGEANDPASSTHROUGH,
                 nodeID,
                 EmittedMessagesUtil<3>::getTotalTaskParams(),
                 dependentParameter,
                 false,
                 false);

#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task) //
#endif
  {
    FrameCount currFrameNo = net->getCurrentFrameNo();
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());

    net->TaskAddParamAndDependency(
      task, 
      0, 
      ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, 
      ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, 
      activeChildNodeID, 
      INVALID_NODE_ID, 
      TPARAM_FLAG_INPUT, 
      currFrameNo, 
      activeAnimSetIndex);

    net->TaskAddOutputParamZeroLifespan(
      task,
      1,
      ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER,
      ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER,
      INVALID_NODE_ID,
      currFrameNo);

    net->TaskAddOptionalDefInputParam(
      task,
      2,
      ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,
      nodeID);

    EmittedMessagesUtil<3>::addTaskParams(net, task, nodeID);
  }

  return task;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------

