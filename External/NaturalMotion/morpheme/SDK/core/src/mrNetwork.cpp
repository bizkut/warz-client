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
#include "morpheme/mrNetwork.h"
#include "morpheme/mrNetworkLogger.h"
#include "morpheme/mrTaskQueue.h"
#include "morpheme/mrManager.h"
#include "morpheme/mrCoreTaskIDs.h"
#include "morpheme/Nodes/mrNodeStateMachine.h"
#include "morpheme/nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/mrCharacterControllerAttribData.h"
#include "morpheme/mrCharacterControllerInterface.h"
#include "NMPlatform/NMProfiler.h"
#include "NMPlatform/NMFreelistMemoryAllocator.h"

#if defined(NM_HOST_CELL_PPU)
  #include "morpheme/mrDispatcherPS3.h"
#endif

#include "morpheme/mrRuntimeNodeInspector.h"

//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
const char* Network::m_profilePointNames[Network::NumOutputDebuggingProfilerPoints] =
{
  "Task Queuing",
  "Task Execution" ,
  "Emitted Control Params",
  "Update Request Emitter Nodes",
  "Attrib Data Lifespans",
  "Connections"
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void beginNodeFuncDebugHook(Network* net, NodeDef* nodeDef, MR::NodeID& oldDebugNodeID)
{
  Dispatcher* dispatcher = net->getDispatcher();
  InstanceDebugInterface* debugInterface = dispatcher->getDebugInterface();
  if (debugInterface)
  {
    // could check if debug draw is enabled here but it's only setting a NodeID so not worth the extra work
    MR::NodeID currentDebugNodeID = nodeDef->getNodeID();
    oldDebugNodeID = debugInterface->setCurrentNodeID(currentDebugNodeID);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void endNodeFuncDebugHook(Network* net, MR::NodeID oldDebugNodeID)
{
  Dispatcher* dispatcher = net->getDispatcher();
  InstanceDebugInterface* debugInterface = dispatcher->getDebugInterface();
  if (debugInterface)
  {
    // could check if debug draw is enabled here but it's only setting a NodeID so not worth the extra work
    debugInterface->setCurrentNodeID(oldDebugNodeID);
  }
}

#endif // MR_OUTPUT_DEBUGGING

Network::AnimSetChangedCB* Network::sm_animSetChangedCB = NULL;

/// This define disables the internal caching of the state of request conditions between network updates.
/// In some situations clearing request conditions after each update allows an application to more easily track and
/// maintain the state of conditions itself.
#define MR_RESET_SM_MESSAGE_STATUSES_POST_UPDATEx

//----------------------------------------------------------------------------------------------------------------------
// MR::NodeConnections
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeConnections::getMemoryRequirements(uint16_t maxNumActiveChildNodes)
{
  // Header
  NMP::Memory::Format result(sizeof(NodeConnections), NMP_NATURAL_TYPE_ALIGNMENT);

  // Active connections
  NMP::Memory::Format memReqsActiveCtns = NodeConnections::getMemoryRequirementsExcludeBaseMem(maxNumActiveChildNodes);
  result += memReqsActiveCtns;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeConnections::getMemoryRequirementsExcludeBaseMem(
  uint16_t maxNumActiveChildNodes)
{
  // Active connections
  NMP::Memory::Format result(sizeof(NodeID) * maxNumActiveChildNodes, NMP_NATURAL_TYPE_ALIGNMENT);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NodeConnections* NodeConnections::init(NMP::Memory::Resource& resource, uint16_t maxNumActiveChildNodes)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(NodeConnections), NMP_NATURAL_TYPE_ALIGNMENT);
  NodeConnections* result = (NodeConnections*) resource.alignAndIncrement(memReqsHdr);

  // Active connections
  NodeConnections::initExcludeBaseMem(result, resource, maxNumActiveChildNodes);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeConnections::initExcludeBaseMem(
  NodeConnections* target,
  NMP::Memory::Resource& resource,
  uint16_t maxNumActiveChildNodes)
{
  NMP_ASSERT(target);

  // Active connections
  NMP::Memory::Format memReqsActiveCtns(sizeof(NodeID) * maxNumActiveChildNodes, NMP_NATURAL_TYPE_ALIGNMENT);
  target->m_activeChildNodeIDs = (NodeID*) resource.alignAndIncrement(memReqsActiveCtns);
  target->m_maxNumActiveChildNodes = maxNumActiveChildNodes;
}

//----------------------------------------------------------------------------------------------------------------------
bool NodeConnections::setFrom(const NodeConnections* source)
{
  NMP_ASSERT(source->m_maxNumActiveChildNodes == m_maxNumActiveChildNodes);

  m_activeParentNodeID = source->m_activeParentNodeID;
  m_numActiveChildNodes = source->m_numActiveChildNodes;
  m_justBecameActive = source->m_justBecameActive;
  m_active = source->m_active;
  m_activeGatherTag = source->m_activeGatherTag;

  for (uint32_t i = 0; i < m_maxNumActiveChildNodes; i++)
  {
    m_activeChildNodeIDs[i] = source->m_activeChildNodeIDs[i];
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeConnections::cleanActiveConnections(const NodeDef* nodeDef)
{
  NMP_ASSERT(nodeDef)
  m_activeParentNodeID = nodeDef->getParentNodeID();

  m_numActiveChildNodes = 0;
  m_justBecameActive = false;
  m_active = false;
  uint32_t numChildNodes = nodeDef->getNumChildNodes();
  for (uint32_t k = 0; k < numChildNodes && m_numActiveChildNodes < m_maxNumActiveChildNodes; ++k)
  {
    if (nodeDef->getChildNodeID(k) != INVALID_NODE_ID)
    {
      m_activeChildNodeIDs[m_numActiveChildNodes] = nodeDef->getChildNodeID(k);
      ++m_numActiveChildNodes;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool NodeConnections::hasActiveChildNodeID(NodeID nodeID) const
{
  for( int32_t i=0; i<m_numActiveChildNodes; ++i)
  {
    if( m_activeChildNodeIDs[i] == nodeID )
    {
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format BlendOptNodeConnections::getMemoryRequirements(
  uint16_t maxNumActiveChildNodes)
{
  // Header
  NMP::Memory::Format result(sizeof(BlendOptNodeConnections), NMP_NATURAL_TYPE_ALIGNMENT);

  // Active connections
  NMP::Memory::Format memReqsActiveCtns = NodeConnections::getMemoryRequirementsExcludeBaseMem(maxNumActiveChildNodes);
  result += memReqsActiveCtns;

  // Blend optimisation connected node IDs
  NMP::Memory::Format memReqsNodeIDs(sizeof(NodeID) * maxNumActiveChildNodes, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsNodeIDs; // transforms and events.
  result += memReqsNodeIDs; // sampled events.

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
BlendOptNodeConnections* BlendOptNodeConnections::init(
  NMP::Memory::Resource& resource,
  uint16_t maxNumActiveChildNodes)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(BlendOptNodeConnections), NMP_NATURAL_TYPE_ALIGNMENT);
  BlendOptNodeConnections* result = (BlendOptNodeConnections*) resource.alignAndIncrement(memReqsHdr);

  // Active connections
  NodeConnections::initExcludeBaseMem(result, resource, maxNumActiveChildNodes);

  // Connected node IDs
  NMP::Memory::Format memReqsNodeIDs(sizeof(NodeID) * maxNumActiveChildNodes, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_trajectoryAndTransformsNodeIDs = (NodeID*)resource.alignAndIncrement(memReqsNodeIDs);
  result->m_sampledEventsNodeIDs = (NodeID*)resource.alignAndIncrement(memReqsNodeIDs);

  result->m_trajectoryAndTransformsNumNodeIDs = 0;
  result->m_sampledEventsNumNodeIDs = 0;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// MR::Network
//----------------------------------------------------------------------------------------------------------------------
Network* Network::createAndInit(
  NetworkDef*                       netDef,
  Dispatcher*                       dispatcher,
  NMP::TempMemoryAllocator*         tempAllocator,
  NMP::MemoryAllocator*             persistentAllocator,
  MR::CharacterControllerInterface* characterController)
{
  NMP::Memory::Format networkFormat = Network::getMemoryRequirements(netDef);

  NMP::Memory::Resource resource = NMPMemoryAllocateFromFormat(networkFormat);
  NMP_ASSERT(resource.ptr);

  Network* net = Network::init(
                   resource,
                   netDef,
                   dispatcher,
                   tempAllocator,
                   persistentAllocator,
                   characterController);

  return net;
}

//----------------------------------------------------------------------------------------------------------------------
void Network::release()
{
  for (NodeID nodeID = 0; nodeID < m_netDef->getNumNodeDefs(); nodeID++)
  {
    NodeDef* nodeDef = m_netDef->getNodeDef(nodeID);
    if (!nodeDef) // Check for partially built network
      continue;

    //--------
    // Delete all the attrib data in this node.
    NodeBin* nodeBin = getNodeBin(nodeID);
    nodeBin->deleteAllAttribData();
    nodeBin->clearQueuedTasks();

    //--------
    // Delete all the post access attribs
    PostUpdateAccessAttribEntry* currentEntry = m_postUpdateAccessAttribEntries[nodeID];
    while (currentEntry)
    {
      PostUpdateAccessAttribEntry* entryToFree = currentEntry;
      currentEntry = currentEntry->m_next;
      getPersistentMemoryAllocator()->memFree(entryToFree);
    }
    m_postUpdateAccessAttribEntries[nodeID] = NULL;

    //--------
    // Delete all output control param attrib data.
    deleteNodeOutputCPAttribData(nodeID);
  }

#if defined(MR_CIRCDEPS_DEBUGGING)
  m_persistentDataAllocator->memFree(m_taskTrace);
  m_taskTrace = NULL;
#endif // MR_CIRCDEPS_DEBUGGING
  
  m_persistentDataAllocator->memFree(m_triggeredExternalMessages);
  m_triggeredExternalMessages = NULL;
  m_numTriggeredExternalMessages = 0;
  m_triggeredExternalMessagesSize = 0;

  return;
}

//----------------------------------------------------------------------------------------------------------------------
void Network::releaseAndDestroy()
{
  release();

  NMP::Memory::memFree(this);

  return;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format Network::getMemoryRequirements(NetworkDef* netDef)
{
  NMP::Memory::Format result(sizeof(Network), NMP_VECTOR_ALIGNMENT);

  // Space for the AttribData NodeBins
  NMP::Memory::Format nodeBins = NodeBin::getMemoryRequirements(netDef);
  result += nodeBins;

  // Space for the structures holding the active child and parent node IDs.
  NMP::Memory::Format nodeConnections = computeMemoryRequirementsActiveNodeConnectionsArray(netDef);
  result += nodeConnections;

  // Space for the task queue
  NMP::Memory::Format taskQueue = TaskQueue::getMemoryRequirements();
  result += taskQueue;

  // Space for external access attributes
  NMP::Memory::Format postUpdateAccess = NMP::Memory::Format(
      NMP::Memory::align(sizeof(PostUpdateAccessAttribEntry*), NMP_NATURAL_TYPE_ALIGNMENT) * netDef->getNumNodeDefs(),
      NMP_NATURAL_TYPE_ALIGNMENT);
  result += postUpdateAccess;

  NET_LOG_LINE_DIVIDE(4);
  NET_LOG_MESSAGE(4, "Network Instance Memory Requirements\n");
  NET_LOG_MESSAGE_INDENT(2, 4, "Node Bins          \t%i\n", nodeBins.size);
  NET_LOG_MESSAGE_INDENT(2, 4, "Node Connections   \t%i\n", nodeConnections.size);
  NET_LOG_MESSAGE_INDENT(2, 4, "Task Queue         \t%i\n", taskQueue.size);
  NET_LOG_MESSAGE_INDENT(2, 4, "Post Update Access \t%i\n", postUpdateAccess.size);
  NET_LOG_MESSAGE_INDENT(2, 4, "Total (inc misc)   \t%i\n", result.size);
  NET_LOG_LINE_DIVIDE(4);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
Network* Network::init(
  NMP::Memory::Resource&            resource,
  NetworkDef*                       netDef,
  Dispatcher*                       dispatcher,
  NMP::TempMemoryAllocator*         tempDataAllocator,
  NMP::MemoryAllocator*             persistentMemoryAllocator,
  MR::CharacterControllerInterface* characterController)
{
  // call default constructor in-place
  Network* result = new(resource.ptr) Network();

  resource.increment(NMP::Memory::Format(sizeof(Network), NMP_VECTOR_ALIGNMENT));

  result->m_netDef = netDef;

  result->m_currentFrameNo = 0;

  result->m_characterController = 0;
  result->m_numActivePhysicsNodes = 0;
  result->m_numActiveGrouperNodes = 0;
  result->m_behaviourNodesActive = false;
  result->m_rootControlMethod = ROOT_CONTROL_ANIMATION;
  result->m_combinedTrajectoryAndTransforms = netDef->canCombineTrajectoryAndTransformSemantics();

  //---------------------
  // Initialize attribute data owned by the Network.
  result->m_nodeBins = NodeBin::init(resource, result->getNetworkDef());

  result->m_conditionAttribList = NULL;

  // We have been provided a memory allocator for temporary working memory
  NMP_ASSERT(tempDataAllocator);
  result->m_tempDataAllocator = tempDataAllocator;

  // We have been provided a memory allocator for persistent memory
  NMP_ASSERT(persistentMemoryAllocator);
  result->m_persistentDataAllocator = persistentMemoryAllocator;

  //---------------------
  // Init the structures holding the active child and parent node IDs.
  result->m_activeNodesConnections = initActiveNodeConnectionsArray(resource, netDef);

  //---------------------
  // Initialise the task queue.
  NMP::Memory::Format tqFmt = TaskQueue::getMemoryRequirements();
  resource.align(tqFmt);
  result->m_taskQueue = TaskQueue::init(resource, result);
  resource.increment(tqFmt);

  result->m_dispatcher = dispatcher;

  //---------------------
  // Initialise storage for addresses of bits of attribute data that are required to exist post network update.
  initPostUpdateAccessAttribEntries(result, resource);

  //---------------------
  // Add the update time pos and active animation set attributes
  AttribDataHandle handle = AttribDataUpdatePlaybackPos::create(
                              result->m_persistentDataAllocator,
                              false,  // time in secs
                              true,   // abs time
                              0.0f);  // time value
  AttribAddress attribAddress(
    ATTRIB_SEMANTIC_UPDATE_TIME_POS,
    NETWORK_NODE_ID,
    INVALID_NODE_ID,
    result->getCurrentFrameNo(),
    ANIMATION_SET_ANY);
  result->addAttribData(attribAddress, handle, LIFESPAN_FOREVER);

  handle = AttribDataUInt::create(result->m_persistentDataAllocator, 0, 0);
  result->m_activeAnimSetIndex = (AttribDataUInt*) handle.m_attribData;
  result->setActiveAnimSetIndex((AnimSetIndex)result->m_activeAnimSetIndex->m_value);
  
  attribAddress.init(ATTRIB_SEMANTIC_ACTIVE_ANIM_SET_INDEX, NETWORK_NODE_ID, INVALID_NODE_ID, VALID_FOREVER, ANIMATION_SET_ANY);
  result->addAttribData(attribAddress, handle, LIFESPAN_FOREVER);
  result->setOutputAnimSetIndex(NETWORK_NODE_ID, result->getActiveAnimSetIndex());

  //---------------------
  // Call the initialisation function for any nodes that have one
  for (uint32_t i = 0; i < netDef->getNumNodeDefs(); ++i)
  {
    NodeDef* nodeDef = netDef->getNodeDef((NodeID) i);
    if (nodeDef)
    {
      nodeDef->initNodeInstance(result);
    }
  }

  result->setCharacterController(characterController);

  //---------------------
#if defined(MR_CIRCDEPS_DEBUGGING)
   result->m_taskTrace = TaskParamList::create(TASK_PARAM_LIST_DEPTH, result->m_persistentDataAllocator);
#endif // MR_CIRCDEPS_DEBUGGING

  result->m_peakUsedTempData = 0;
  result->m_lastUpdateTimeStep = 0.0f;
  result->m_exeResult = EXECUTE_RESULT_COMPLETE;

  result->m_nodeActivationCallback = NULL;
  result->m_nodeDeactivationCallback = NULL;
  result->m_eventTriggeredCallback = NULL;

  result->m_triggeredInternalMessages = NULL;
  result->m_triggeredExternalMessages = NULL;
  result->m_numTriggeredExternalMessages = 0;
  result->m_triggeredExternalMessagesSize = 0;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void Network::initPostUpdateAccessAttribEntries(Network* net, NMP::Memory::Resource& resource)
{
  NMP_ASSERT(net && net->m_netDef);
  uint32_t numNodes = net->m_netDef->getNumNodeDefs();
  NMP_ASSERT(numNodes > 0);
  NMP::Memory::Format memReq(NMP::Memory::align(sizeof(PostUpdateAccessAttribEntry*), NMP_NATURAL_TYPE_ALIGNMENT) * numNodes, NMP_NATURAL_TYPE_ALIGNMENT);
  resource.align(memReq);
  net->m_postUpdateAccessAttribEntries = (PostUpdateAccessAttribEntry**) resource.ptr;
  resource.increment(memReq);
  for (uint32_t i = 0; i < numNodes; ++i)
  {
    net->m_postUpdateAccessAttribEntries[i] = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Network::deleteNodeOutputCPAttribData(NodeID nodeID)
{
  NMP_ASSERT(nodeID < m_netDef->getNumNodeDefs());
  NodeDef* nodeDef = m_netDef->getNodeDef(nodeID);
  NMP_ASSERT (nodeDef);

  NodeBin* nodeBin = getNodeBin(nodeID);
  for (PinIndex i = 0; i < nodeDef->getNumOutputCPPins(); ++i)
  {
    nodeBin->getOutputCPPin(i)->deleteAttribData();
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format Network::computeMemoryRequirementsActiveNodeConnectionsArray(NetworkDef* netDef)
{
  NMP_ASSERT(netDef);
  uint32_t numNodes = netDef->getNumNodeDefs();
  NMP_ASSERT(numNodes > 0);

  NMP::Memory::Format result(sizeof(NodeConnections*) * numNodes, NMP_NATURAL_TYPE_ALIGNMENT);
  for (NodeID i = 0; i < numNodes; ++i)
  {
    const NodeDef* nodeDef = netDef->getNodeDef(i);
    if (!nodeDef) // Check for partially built network
      continue;

    // Check for a blend node that supports blend optimisation
    if (nodeDef->getNodeFlags().isSet(NodeDef::NODE_FLAG_IS_BLEND_OPT_NODE))
    {
      result += BlendOptNodeConnections::getMemoryRequirements(nodeDef->getMaxNumActiveChildNodes());
    }
    else
    {
      result += NodeConnections::getMemoryRequirements(nodeDef->getMaxNumActiveChildNodes());
    }
  }

  // Multiple of the alignment
  result.size = NMP::Memory::align(result.size, NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NodeConnections** Network::initActiveNodeConnectionsArray(NMP::Memory::Resource& resource, const NetworkDef* netDef)
{
  NMP_ASSERT(netDef);
  uint32_t numNodes = netDef->getNumNodeDefs();
  NMP_ASSERT(numNodes > 0);

  NMP::Memory::Format memReqs(sizeof(NodeConnections*) * numNodes, NMP_NATURAL_TYPE_ALIGNMENT);
  NodeConnections** activeNodesConnections = (NodeConnections**) resource.alignAndIncrement(memReqs);

  for (NodeID i = 0; i < numNodes; ++i)
  {
    activeNodesConnections[i] = NULL;
    const NodeDef* nodeDef = netDef->getNodeDef(i);
    if (!nodeDef) // Check for partially built network
      continue;

    // Check for a blend node that supports blend optimisation
    if (nodeDef->getNodeFlags().isSet(NodeDef::NODE_FLAG_IS_BLEND_OPT_NODE))
    {
      activeNodesConnections[i] = BlendOptNodeConnections::init(resource, nodeDef->getMaxNumActiveChildNodes());
    }
    else
    {
      activeNodesConnections[i] = NodeConnections::init(resource, nodeDef->getMaxNumActiveChildNodes());
    }

    // Clean the active connections
    activeNodesConnections[i]->cleanActiveConnections(nodeDef);
  }

  // Multiple of the alignment
  resource.align(NMP_NATURAL_TYPE_ALIGNMENT);

  return activeNodesConnections;
}

//----------------------------------------------------------------------------------------------------------------------
void Network::getPhysicsAndGrouperNodesAndBehavioursActive()
{
  NodeID startingNodeID = getNetworkDef()->getRootNodeID();

  m_numActivePhysicsNodes = 0;
  m_numActiveGrouperNodes = 0;
  m_behaviourNodesActive = false;

  if (getNetworkDef()->isPhysical())
  {
    getActivePhysicsNodesAndBehaviourInChildHierarchy(startingNodeID);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Network::getActivePhysicsNodesAndBehaviourInChildHierarchy(NodeID startingNodeID)
{
  const NodeDef* const node = getNetworkDef()->getNodeDef(startingNodeID);

  //-----------------------------------------------
  // Is this a physics node?
  if (node->getNodeFlags() & NodeDef::NODE_FLAG_IS_PHYSICAL)
  {
    // Prevent duplicates which can be caused by multiply connected pass down physics outputs
    if( containsActivePhysicsNodeID( startingNodeID ) )
    {
      return; // we can return here as we have visited this node previously 
              // and would have already recurse over its children.
    }

    // store this nodeID - this error checks as well.
    if( addActivePhysicsNodeID( startingNodeID ) )
    {
      // We successfully added this nodeID
      // if a behavioural node store that we have behaviour nodes active
      if (node->getNodeFlags() & NodeDef::NODE_FLAG_IS_BEHAVIOURAL)
      {
        m_behaviourNodesActive = true;
      }
    }
  }

  //-----------------------------------------------
  // Is this a physics group node?
  if (node->getNodeFlags() & NodeDef::NODE_FLAG_IS_PHYSICS_GROUPER)
  {
    // It appears that the active grouper node ids can be duplicated in the same way as
    // the active node ids (See above - 'which can be caused by multiply connected pass down physics outputs')
    if( containsActiveGrouperNodeID( startingNodeID ) )
    {
      return; // we can return here as we have visited this node previously 
              // and would have already recurse over its children.
    }

    // store this nodeID - this error checks as well.
    if( addActiveGrouperNodeID( startingNodeID ) == false )
    {
      // We successfully added this nodeID
      // if a behavioural node - store that we have behaviour nodes active
      if (node->getNodeFlags() & NodeDef::NODE_FLAG_IS_BEHAVIOURAL)
      {
        m_behaviourNodesActive = true;
      }
    }
  }

  //-----------------------------------------------
  // recurse through the children
  const NodeConnections* const nc = getActiveNodesConnections(startingNodeID);
  for (uint32_t i = 0; i < nc->m_numActiveChildNodes; ++i)
  {
    if (nc->m_activeChildNodeIDs[i] != INVALID_NODE_ID)
    {
      getActivePhysicsNodesAndBehaviourInChildHierarchy(nc->m_activeChildNodeIDs[i]);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Network::containsActivePhysicsNodeID( NodeID nodeID ) const
{
  for( uint32_t i = 0; i < m_numActivePhysicsNodes; ++i )
  {
    if(m_activePhysicsNodeIDs[i] == nodeID)
      return true; 
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Network::containsActiveGrouperNodeID( NodeID nodeID ) const
{
  for( uint32_t i = 0; i < m_numActiveGrouperNodes; ++i )
  {
    if(m_activeGrouperNodeIDs[i] == nodeID)
      return true; 
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Network::addActivePhysicsNodeID( NodeID nodeID )
{
  NMP_ASSERT_MSG( (getNetworkDef()->getNodeDef(nodeID)->getNodeFlags() & NodeDef::NODE_FLAG_IS_PHYSICAL) != 0, 
    "Adding an ActivePhysicsNodeID of a node that is not a physics node" );

  if (m_numActivePhysicsNodes < MAX_ACTIVE_PHYSICS_NODE_IDS)
  {
    m_activePhysicsNodeIDs[m_numActivePhysicsNodes++] = nodeID;
    return true;
  }
  NET_LOG_ERROR_MESSAGE("Found too many active nodes of type NODE_FLAG_IS_PHYSICAL.\n");
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Network::addActiveGrouperNodeID( NodeID nodeID )
{
  NMP_ASSERT_MSG( (getNetworkDef()->getNodeDef(nodeID)->getNodeFlags() & NodeDef::NODE_FLAG_IS_PHYSICS_GROUPER) != 0, 
    "Adding an ActiveGrouperNodeID of a node that is not a grouper node" );

  if (m_numActiveGrouperNodes < MAX_ACTIVE_GROUPER_NODE_IDS)
  {
    m_activeGrouperNodeIDs[m_numActiveGrouperNodes++] = nodeID;
    return true;
  }
  NET_LOG_ERROR_MESSAGE("Found too many active nodes of type NODE_FLAG_IS_PHYSICS_GROUPER.\n");
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool Network::isNodeTagActive(const char * nodeTag) const
{
  uint32_t numActiveNodes                   = getActiveNodeCount();
  NMP::TempMemoryAllocator* childAllocator  = m_tempDataAllocator->createChildAllocator();
  NodeID* activeNodes                       = (NodeID*)NMPAllocatorMemAlloc(childAllocator, numActiveNodes * sizeof(NodeID), NMP_NATURAL_TYPE_ALIGNMENT);

  getActiveNodes(activeNodes, numActiveNodes);

  bool tagFound = false;
  const MR::NodeTagTable * tagTable = getNetworkDef()->getNodeTagTable();
  for (uint32_t i = 0; i < numActiveNodes; ++i)
  {
    // This is doing a strcmp but it is over the number of active nodes.
    // This is probably more efficient than using findTag(const char *) 
    // and then using is index as findTag does a strcmp over all the tags 
    // in the network.
    if(tagTable->nodeHasTag(*(activeNodes + i), nodeTag))
    {
      tagFound = true;
      break;
    }
  }

  m_tempDataAllocator->destroyChildAllocator(childAllocator);

  return tagFound;
}

//----------------------------------------------------------------------------------------------------------------------
bool Network::queueTasksFor(
  TaskParameter* dependentParam)// The TaskParam in the above task that depends on this task.
{
#if defined(MR_OUTPUT_DEBUGGING)
  // Start task queuing timing.
  NMP::Timer queueTimer;
  if (m_dispatcher->getDebugInterface() && m_dispatcher->getDebugInterface()->debugOutputsAreOn(DEBUG_OUTPUT_PROFILING))
    queueTimer.start();
#endif // COMMS_DEBUGGING_TEST

  // Print out that we have been called.
  NET_LOG_MESSAGE(99, "------------\n%3i. Network::queueTasksFor: ", m_taskQueue->m_numQueuedTasks);
  NET_LOG_OUTPUT_ATTRIB_ADDRESS(99, dependentParam->m_attribAddress, getNetworkDef());

  //-------------------
  // Update connections must have already have been called on every node that is queuing tasks.
  NMP_ASSERT(getNodeBin(dependentParam->m_attribAddress.m_owningNodeID)->getLastFrameUpdate() == m_currentFrameNo);

#if defined(MR_OUTPUT_DEBUGGING)
  MR::NodeID oldDebugNodeID = INVALID_NODE_ID;
  beginNodeFuncDebugHook(this, m_netDef->getNodeDef(dependentParam->m_attribAddress.m_owningNodeID), oldDebugNodeID);
#endif // MR_OUTPUT_DEBUGGING

  //-------------------
#if defined(MR_CIRCDEPS_DEBUGGING)
  startTraceParam(dependentParam);
#endif // MR_CIRCDEPS_DEBUGGING

  //-------------------
  // This line will actually stick the tasks required to generate the requested attribute on the queue.
  // The returned deps will tell us what else we need to put on the queue in order to do this task.
  // We pass a reference to deps, which gets filled in with what the ops that are queued here require.
  Task* queuedTask = m_netDef->getNodeDef(dependentParam->m_attribAddress.m_owningNodeID)->putAttributeOnQueue(
                       getTaskQueue(),
                       this,
                       dependentParam);

  //-------------------
#if defined(MR_CIRCDEPS_DEBUGGING)
  endTraceParam(dependentParam);
#endif // MR_CIRCDEPS_DEBUGGING

#if defined(MR_OUTPUT_DEBUGGING)
  endNodeFuncDebugHook(this, oldDebugNodeID);

  // Finish and record task queuing timing.
  if (m_dispatcher->getDebugInterface() && m_dispatcher->getDebugInterface()->debugOutputsAreOn(DEBUG_OUTPUT_PROFILING))
    m_dispatcher->getDebugInterface()->addNodeTiming(dependentParam->m_attribAddress.m_owningNodeID, MR::NTTaskQueueing, queueTimer.stop());
#endif // MR_OUTPUT_DEBUGGING

  //-------------------
  // We might not have created a task to queue if the item was already on the queue for some reason.
  if (queuedTask)
  {
#if defined(MR_OUTPUT_DEBUGGING)
    sendQueuedTaskDebugInfo(queuedTask);
#endif // MR_OUTPUT_DEBUGGING

#ifdef NM_DEBUG
    NMP_ASSERT(m_dispatcher);
    queuedTask->validateTaskParams(
      m_dispatcher->getTaskName(queuedTask->m_taskid),
      m_netDef->getNodeNameFromNodeID(queuedTask->m_owningNodeID));

#ifdef NMP_ENABLE_ASSERTS
    queuedTask->checkForTaskParamMerges(m_dispatcher->getTaskName(queuedTask->m_taskid));
#endif//NMP_ENABLE_ASSERTS

#endif // NM_DEBUG

    getTaskQueue()->addToTaskList(queuedTask);
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
void Network::sendQueuedTaskDebugInfo (Task* queuedTask)
{
  OUTDBG_TIMING_END(this, Network::QueueTasks);

  NMP_ASSERT(m_dispatcher);
  InstanceDebugInterface* debugInterface = m_dispatcher->getDebugInterface();

  if (debugInterface && debugInterface->debugOutputsAreOn(DEBUG_OUTPUT_TASK_QUEUING))
  {
    debugInterface->beginDispatcherTaskExecute(
      queuedTask->m_taskid,
      m_dispatcher->getTaskName(queuedTask->m_taskid),
      queuedTask->m_owningNodeID);

    for (uint32_t i = 0; i < queuedTask->m_numParams; i++)
    {
      TaskParameter* param = &(queuedTask->m_params[i]);

      debugInterface->addDispatcherTaskExecuteParameter(
        queuedTask->m_taskid,
        param->m_attribAddress.getAttribSemanticName(),
        param->m_attribAddress.m_owningNodeID,
        param->m_attribAddress.m_targetNodeID,
        param->m_attribAddress.m_validFrame,
        param->m_taskParamFlags.isSet(TPARAM_FLAG_INPUT),
        param->m_taskParamFlags.isSet(TPARAM_FLAG_OUTPUT),
        param->m_lifespan);
    }

    debugInterface->endDispatcherTaskExecute();
  }

  OUTDBG_TIMING_BEGIN(this);
}
#endif // MR_OUTPUT_DEBUGGING

//----------------------------------------------------------------------------------------------------------------------
AttribData* Network::updateOutputCPAttribute(
  NodeID   owningNodeID,
  PinIndex outputCPPinIndex,
  AnimSetIndex animSet)
{
  AttribData* result = NULL;

  // Print out that we have been called.
#if defined(MR_NETWORK_LOGGING)
  NET_LOG_MESSAGE(
    99,
    "------------\n Network::updateOutputCPAttribute:\n NodeID: %d, OutputPinIndex: %d",
    owningNodeID,
    outputCPPinIndex);
#endif // MR_NETWORK_LOGGING

  NodeBin* nodeBin = getNodeBin(owningNodeID);
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(outputCPPinIndex);

  // Only call control param update functions if the connected pin has not been updated this frame.
  if ((outputCPPin->m_lastUpdateFrame == m_currentFrameNo) || 
      (outputCPPin->m_lastUpdateFrame == VALID_FOREVER))      // User input control param values remain valid even when not set every frame.
  {
    // AttribData has already been updated this frame, just return it.
    result = outputCPPin->getAttribData();
    NET_LOG_MESSAGE(99, "  Not processing, valid attrib already exists\n");
  }
  else
  {
    // AttibData for this pin needs updating the output control param update function for this pin.  
    NodeDef* owningNodeDef = m_netDef->getNodeDef(owningNodeID);

    if (nodeBin->m_outputAnimSet != animSet)
    {
      setOutputAnimSetIndex(owningNodeID, animSet);
      // update pinAttribData if animSet changed
      nodeUpdatePinAttribDataInstance(owningNodeDef, this);
    }

#if defined(MR_OUTPUT_DEBUGGING)
    MR::NodeID oldDebugNodeID = INVALID_NODE_ID;
    beginNodeFuncDebugHook(this, owningNodeDef, oldDebugNodeID);
#endif // MR_OUTPUT_DEBUGGING

    result = owningNodeDef->updateOutputCPAttribute(this, outputCPPinIndex);

#if defined(MR_OUTPUT_DEBUGGING)
    endNodeFuncDebugHook(this, oldDebugNodeID);
#endif // MR_OUTPUT_DEBUGGING

    // Flag this node and connection as having been updated this frame.
    outputCPPin->m_lastUpdateFrame = m_currentFrameNo;
    nodeBin->setLastFrameUpdate(m_currentFrameNo);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Network::freeAllTempData()
{
  DEBUG_VALIDATE_NETWORK_ATTRIB_CONTENTS(this, true);

  // The task queue uses the temp allocator so let it know we're wiping down.
  getTaskQueue()->freeTempData();

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
// There is no direct access to member variables in this method for two reasons:
//  1. this strengthens the architecture and encapsulation of the network instance.
//  2. it is easier to move the code outside the actual network instance if an update customization is needed.
void Network::startUpdate(
  float updateTime,                     // Update time value.
  bool  absTime             /*=false*/, // Are we setting time or updating time with a delta.
  bool  queueTransforms     /*=true */, // Do we want to create a transforms dependency from the root.
  bool  queueTrajectory     /*=true */, // Do we want to create a transforms dependency from the root.
  bool  queueSampledEvents  /*=true */, // Do we want to create a transforms dependency from the root.
  bool  queueSyncEventTrack /*=false*/) // Do we want to create a sync event track dependency from the root. 
                                        // NOTE: be sure that your root node always generates, or passes through, SyncEventTracks.
{
  OUTDBG_TIMING_RESET(this);

  incFrameCounter();

  DEBUG_VALIDATE_NETWORK_ATTRIB_CONTENTS(this, false);

  NET_LOG_MESSAGE(100, "\n");
  NET_LOG_MESSAGE_COLOUR(NMP::LOG_COLOUR_BRIGHT_CYAN, 100, "**********************************************************************************************************************\n");
  NET_LOG_MESSAGE_COLOUR(NMP::LOG_COLOUR_BRIGHT_CYAN, 100, "**********************************************************************************************************************\n");
  NET_LOG_MESSAGE_COLOUR(NMP::LOG_COLOUR_BRIGHT_CYAN, 100, "Network::update called! Frame No: %i\n", getCurrentFrameNo());

  setRootControlMethod(ROOT_CONTROL_ANIMATION);

  // Clear last frame's node event messages
  clearExternalTriggeredNodeEventMessages();

  // Process any emitted requests from previous frame.
  updateEmittedMessages();

  OUTDBG_TIMING_BEGIN(this)
  NM_BEGIN_PROFILING("updateAttribDataLifespans");
  // Decrement the lifespan of all attrib data that aren't persistent.
  // If an attribData's lifespan hits 0 delete it.
  updateAttribDataLifespans(NETWORK_NODE_ID);
  NM_END_PROFILING(); // "updateAttribDataLifespans"
  OUTDBG_TIMING_END(this, Network::UpdateAttribDataLifespans);

  // Provide the dispatcher with some allocators
  Dispatcher* dispatcher = getDispatcher();
  dispatcher->setPersistentMemoryAllocator(getPersistentMemoryAllocator());
  dispatcher->setTempMemoryAllocator(getTempMemoryAllocator());

  // Set up the task queue for a new dispatch
  dispatcher->setTaskQueue(getTaskQueue());
  getTaskQueue()->startQueuing();

  // This call will set the Time AttribData on the Network node to the correct timestep
  updateNetworkTime(updateTime, absTime);

  NET_LOG_MESSAGE(100, "***********************************************************************\n  QUEUE TASKS\n");

  //------------------
  getNodeTransformsBuffer(m_netDef->getRootNodeID(), this, m_currentFrameNo - 1, (AnimSetIndex)m_activeAnimSetIndex->m_value);

  // Update the network connections first.
  OUTDBG_TIMING_BEGIN(this)
  NM_BEGIN_PROFILING("updateNodeInstanceConnections");
  updateNodeInstanceConnections(m_netDef->getRootNodeID(), getActiveAnimSetIndex());
  NodeBin* nodeBin = getNodeBin(NETWORK_NODE_ID);
  nodeBin->setLastFrameUpdate(getCurrentFrameNo());
  getPhysicsAndGrouperNodesAndBehavioursActive();
  NM_END_PROFILING(); // "updateNodeInstanceConnections"
  OUTDBG_TIMING_END(this, Network::UpdateNodeInstanceConnections);

  // If the user has set a node deactivation callback, find any nodes which de-activated this frame and report them to
  // the callback.
  FrameCount lastFrameNumber = getCurrentFrameNo() - 1;
  if (m_nodeDeactivationCallback)
  {
    for (MR::NodeID i = 0; i < m_netDef->getNumNodeDefs(); i++)
    {
      NodeBin* deactivationNodeBin = getNodeBin(i);
      if (deactivationNodeBin->getLastFrameUpdate() == lastFrameNumber)
      {
        // Ignore deactivation callbacks for operator nodes.
        // (operator nodes update with the previous frame as the current frame)
        const NodeDef* const deactivationNodeBinDef = m_netDef->getNodeDef(i);
        if (!(deactivationNodeBinDef->getNodeFlags() & NodeDef::NODE_FLAG_IS_OPERATOR_NODE))
        {
          m_nodeDeactivationCallback(i);
        }
      }
    }
  }

  for (uint32_t i = 0; i < m_netDef->getNumMultiplyConnectedNodes(); i++)
  {
    NodeID nodeID = m_netDef->getMultiplyConnectedNodeID(i);

    // Have to check if the node exists because sometimes, during closest anim node compilation, the network is evaluated and they don't exist.
    if (m_activeNodesConnections[nodeID])
    {
      m_activeNodesConnections[nodeID]->m_active = false;
    }
  }
  markActiveNodesUnder(NETWORK_NODE_ID);
  for (uint32_t i = 0; i < m_netDef->getNumMultiplyConnectedNodes(); i++)
  {
    NodeID nodeID = m_netDef->getMultiplyConnectedNodeID(i);
    NodeDef* nodeDef = m_netDef->getNodeDef(nodeID);

    // Have to check if the node exists because sometimes, during closest anim node compilation, the network is evaluated and they don't exist.
    if (nodeDef && !nodeIsActive(nodeID))
    {
      nodeDef->deleteNodeInstance(this);
    }
  }

  // We also need to check if there are any onExit messages to be sent, and fire them if so.
  NetworkDef::NodeEventOnExitMessage *nodeEventOnExitMessages = m_netDef->getNodeEventOnExitMessages();
  for (uint32_t i = 0; i < m_netDef->getNumNodeEventOnExitMessages(); i++)
  {
    if (getNodeBin(nodeEventOnExitMessages[i].m_nodeID)->getLastFrameUpdate() == lastFrameNumber)
    {
      // This node was deactivated this frame - cache the message as requested.
      storeTriggeredEvent(&nodeEventOnExitMessages[i].m_nodeEventMessage);
    }
  }

  // Make sure that all active connections are valid.
  DEBUG_VALIDATE_NETWORK_CONNECTIVITY();

  //------------------
  OUTDBG_TIMING_BEGIN(this);
  // Make a dummy task for everything to be rooted to.
  // Dummy task must be dependent on attrib data from the root node that we will be asking to queue tasks for.
  buildDummyRootTaskAndQueueDependents(
    queueTransforms,
    queueTrajectory,
    queueSampledEvents,
    queueSyncEventTrack);
  OUTDBG_TIMING_END(this, Network::QueueTasks);

  NM_BEGIN_PROFILING("finishQueuing");
  getTaskQueue()->finishQueuing();
  NM_END_PROFILING(); // "finishQueuing"

  DEBUG_VALIDATE_NETWORK_ATTRIB_CONTENTS(this, false);

  m_exeResult = MR::EXECUTE_RESULT_STARTED;
}

//----------------------------------------------------------------------------------------------------------------------
ExecuteResult Network::update(Task *&task)
{
  //------------------
  // This call will pass the generated queue for this network to the dispatcher.  The dispatcher will then begin
  //  execution.
  NET_LOG_MESSAGE_COLOUR(NMP::LOG_COLOUR_BRIGHT_YELLOW, 107, "***********************************************************************\n  DISPATCHING THE QUEUE\n");

  NM_BEGIN_PROFILING("Dispatcher execute");
  task = NULL;
  Task* resultTask = NULL;

  ExecuteResult result;
  {
    OUTDBG_TIMING_BEGIN(this);
    result = m_dispatcher->execute(getTaskQueue(), resultTask);
    OUTDBG_TIMING_END(this, Network::TaskExecution);
  }
  NM_END_PROFILING(); // "Dispatcher execute"

  m_exeResult = result;

  if (result == EXECUTE_RESULT_COMPLETE)
  {
    // Dispatcher has finished (queue is blocked or empty)
#if defined(MR_RESET_SM_MESSAGE_STATUSES_POST_UPDATE)
    clearMessageStatusesOnAllStateMachines();
#endif // MR_RESET_SM_MESSAGE_STATUSES_POST_UPDATE

    // Deal with any internal messages which were triggered by nodes
    NodeEventMessageLink *nextMessage = m_triggeredInternalMessages;
    while(nextMessage)
    {
      NMP_ASSERT_MSG(!nextMessage->m_message->m_external, "External message found in internal message list!");

      // Broadcast the message
      Message requestMessage(nextMessage->m_message->m_msgID, MESSAGE_TYPE_REQUEST, true, 0, 0);
      broadcastMessage(requestMessage);
      
      // Iterate!
      nextMessage = nextMessage->m_next;
    }

    // We can just set the triggered internal messages to NULL as they will all be freed when the temp allocator resets
    m_triggeredInternalMessages = NULL;

    // Free any temporary data that was used in computation
    NM_BEGIN_PROFILING("freeTempData");

    uint32_t lastUsedBytes = freeAllTempData();
    if (lastUsedBytes > m_peakUsedTempData)
      m_peakUsedTempData = lastUsedBytes;

    NM_END_PROFILING(); // "freeTempData"

    // If the user has registered a triggered event callback, fire it if there have been any triggered events this
    //  frame.
    if (m_eventTriggeredCallback)
    {
      // Get the sampled events buffer for this frame.
      MR::AttribDataSampledEvents* sampledEventsAttribData = (MR::AttribDataSampledEvents *)
        getAttribData(MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER,
        m_netDef->getRootNodeID(),
        INVALID_NODE_ID,
        m_currentFrameNo,
        getActiveAnimSetIndex());

      if (sampledEventsAttribData)
      {
        if (sampledEventsAttribData->m_discreteBuffer->getNumTriggeredEvents() != 0)
        {
          m_eventTriggeredCallback(sampledEventsAttribData->m_discreteBuffer);
        }
      }
    }
  }
  else if (result == EXECUTE_RESULT_BLOCKED &&
           (resultTask->m_taskid == MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATECHARACTERCONTROLLER ||
            resultTask->m_taskid == MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATEPHYSICS ||
            resultTask->m_taskid == MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATEROOT))
  {
    // execute our own function
    Dispatcher::TaskFunction* taskFn = m_dispatcher->getTaskFunctionPointer(resultTask);
    NMP_ASSERT(taskFn);

    // Go ahead and call it with the task's parameters
    Dispatcher::TaskParameters taskParameters = { &(resultTask->m_params[0]), resultTask->m_numParams, m_dispatcher };
    taskFn(&taskParameters);
    m_taskQueue->m_numUnprocessedTasks--;

    // Note, these tasks are dummy markers, external task execution is required before further progress
    // can be made with this network, so they need to remain EXECUTE_RESULT_BLOCKED here.
  }

  task = resultTask;

  return m_exeResult;
}

//----------------------------------------------------------------------------------------------------------------------
void Network::endUpdate()
{
  OUTDBG_TIMING_BEGIN(this);
  NM_BEGIN_PROFILING("updateEmittedControlParameters");
  // Call the immediate update function of every emitted control parameter node.
  updateEmittedControlParameters();
  NM_END_PROFILING(); // "updateEmittedControlParameters"
  OUTDBG_TIMING_END(this, Network::UpdateEmittedControlParameters);

  OUTDBG_TIMING_BEGIN(this);
  NM_BEGIN_PROFILING("updateRequestEmitterNodes");
  // Call the immediate update function of every request emitter node.
  updateRequestEmitterNodes();
  NM_END_PROFILING(); // "updateRequestEmitterNodes"
  OUTDBG_TIMING_END(this, Network::UpdateRequestEmitterNodes);
}

//----------------------------------------------------------------------------------------------------------------------
void Network::buildDummyRootTaskAndQueueDependents(
  bool queueTransforms,
  bool queueTrajectory,
  bool queueSampleEvents,
  bool queueSyncEventTrack /* = false */)
{
  // Queue the tasks required to generate attribute data from the root node.
  NM_BEGIN_PROFILING("queuingTasks");

#if defined(MR_OUTPUT_DEBUGGING)
  // Start task queuing timing.
  NMP::Timer queueTimer;
  if (m_dispatcher->getDebugInterface() && m_dispatcher->getDebugInterface()->debugOutputsAreOn(DEBUG_OUTPUT_PROFILING))
    queueTimer.start();
#endif // COMMS_DEBUGGING_TEST

  //------------------
  // Work out the number of basic attributes to queue on the network root node.
  uint32_t numAttribsToQueue = 0;

  // If the network can combine trajectory and transforms and we've been requested to queue trajectory and transforms
  // the use the combined semantic
  if (queueTransforms && queueTrajectory && m_netDef->canCombineTrajectoryAndTransformSemantics())
  {
    m_combinedTrajectoryAndTransforms = true;
  }
  else
  {
    m_combinedTrajectoryAndTransforms = false;
  }

  bool queueTrajectoryAndTransforms = false;
  if (m_combinedTrajectoryAndTransforms)
  {
    if (rootTaskParamIsQueuable(ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, m_netDef->getRootNodeID(), INVALID_NODE_ID, getCurrentFrameNo()))
    {
      ++numAttribsToQueue;
      queueTrajectoryAndTransforms = true;
    }
  }
  else
  {
    if (queueTransforms && rootTaskParamIsQueuable(ATTRIB_SEMANTIC_TRANSFORM_BUFFER, m_netDef->getRootNodeID(), INVALID_NODE_ID, getCurrentFrameNo()))
      ++numAttribsToQueue;
    else
      queueTransforms = false;

    if (queueTrajectory && rootTaskParamIsQueuable(ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, m_netDef->getRootNodeID(), INVALID_NODE_ID, getCurrentFrameNo()))
      ++numAttribsToQueue;
    else
      queueTrajectory = false;
  }

  if (queueSampleEvents && rootTaskParamIsQueuable(ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, m_netDef->getRootNodeID(), INVALID_NODE_ID, getCurrentFrameNo()))
    ++numAttribsToQueue;
  else
    queueSampleEvents = false;

  // Queue sync event track
  if (queueSyncEventTrack && rootTaskParamIsQueuable(ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, m_netDef->getRootNodeID(), INVALID_NODE_ID, getCurrentFrameNo()))
  {
    ++numAttribsToQueue;
  }
  else
  {
    queueSyncEventTrack = false;
  }


  //------------------
  // Only queue the update of state machine transition condition attrib forTarget if their connections has been updated this frame.
  ConditionAttribEntry* indexAttrib = m_conditionAttribList;
  while (indexAttrib)
  {
    NodeID ownerNode = indexAttrib->m_conditionAttrib.m_owningNodeID;
    if (nodeIsActive(ownerNode))
      numAttribsToQueue++;
    indexAttrib = indexAttrib->m_next;
  }

  //----------------------------------------------------------------------------------------------------------------------
#if defined(MR_CIRCDEPS_DEBUGGING)
  NMP_ASSERT(m_taskTrace);
  m_taskTrace->clear();
#endif // MR_CIRCDEPS_DEBUGGING

  //------------------
  // Make a dummy task for everything to be rooted to.
  // Dummy task must be dependent on attrib data from the root node that we will be asking to queue tasks for.
  Task* dummyTask = m_taskQueue->createNewRootTaskOnQueue(
                      CoreTaskIDs::MR_TASKID_EMPTYTASK, // ID of the task to add to the queue.
                      NETWORK_NODE_ID,                  // Task owning Node ID.
                      numAttribsToQueue);               // Num task parameters.

#if defined(MR_OUTPUT_DEBUGGING)
  // Finish and record task queuing timing.
  if (m_dispatcher->getDebugInterface() && m_dispatcher->getDebugInterface()->debugOutputsAreOn(DEBUG_OUTPUT_PROFILING))
    m_dispatcher->getDebugInterface()->addNodeTiming(NETWORK_NODE_ID, MR::NTTaskQueueing, queueTimer.stop());
#endif // COMMS_DEBUGGING_TEST

  //------------------
  // Init params for root node. Dummy task dependent on root nodes attrib data.
  uint32_t rootNodeAttribCount = 0;

  if (m_combinedTrajectoryAndTransforms)
  {
    if (queueTrajectoryAndTransforms)
    {
      RootTaskAddParamAndDependency(
        dummyTask,
        rootNodeAttribCount,
        ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
        ATTRIB_TYPE_TRANSFORM_BUFFER,
        m_netDef->getRootNodeID(),
        INVALID_NODE_ID,
        TPARAM_FLAG_INPUT,
        getCurrentFrameNo());
      ++rootNodeAttribCount;
    }
  }
  else
  {
    if (queueTransforms)
    {
      RootTaskAddParamAndDependency(
        dummyTask,
        rootNodeAttribCount,
        ATTRIB_SEMANTIC_TRANSFORM_BUFFER,
        ATTRIB_TYPE_TRANSFORM_BUFFER,
        m_netDef->getRootNodeID(),
        INVALID_NODE_ID,
        TPARAM_FLAG_INPUT,
        getCurrentFrameNo());
      ++rootNodeAttribCount;
    }
    if (queueTrajectory)
    {
      RootTaskAddParamAndDependency(
        dummyTask,
        rootNodeAttribCount,
        ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM,
        ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM,
        m_netDef->getRootNodeID(),
        INVALID_NODE_ID,
        TPARAM_FLAG_INPUT,
        getCurrentFrameNo());
      ++rootNodeAttribCount;
    }
  }
  if (queueSampleEvents)
  {
    RootTaskAddParamAndDependency(
      dummyTask,
      rootNodeAttribCount,
      ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER,
      ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER,
      m_netDef->getRootNodeID(),
      INVALID_NODE_ID,
      TPARAM_FLAG_INPUT,
      getCurrentFrameNo());
    ++rootNodeAttribCount;
  }
  if (queueSyncEventTrack)
  {
    RootTaskAddParamAndDependency(
      dummyTask,
      rootNodeAttribCount,
      ATTRIB_SEMANTIC_SYNC_EVENT_TRACK,
      ATTRIB_TYPE_SYNC_EVENT_TRACK,
      m_netDef->getRootNodeID(),
      INVALID_NODE_ID,
      TPARAM_FLAG_INPUT,
      getCurrentFrameNo());
    ++rootNodeAttribCount;
  }

  //------------------
  // Queue update of AttribData dependencies from state machine transition conditions.
  indexAttrib = m_conditionAttribList;
  while (indexAttrib)
  {
    NodeID ownerNode = indexAttrib->m_conditionAttrib.m_owningNodeID;
    if (nodeIsActive(ownerNode))
    {
      RootTaskAddParamAndDependency(
        dummyTask,
        rootNodeAttribCount,
        indexAttrib->m_conditionAttrib.m_semantic,
        indexAttrib->m_attribType,
        indexAttrib->m_conditionAttrib.m_owningNodeID,
        indexAttrib->m_conditionAttrib.m_targetNodeID,
        TPARAM_FLAG_INPUT,
        getCurrentFrameNo(),
        getOutputAnimSetIndex(ownerNode));
      ++rootNodeAttribCount;
    }
    indexAttrib = indexAttrib->m_next;
  }

  //----------------------------------------------------------------------------------------------------------------------
#if defined(MR_CIRCDEPS_DEBUGGING)
  NMP_ASSERT(m_taskTrace->size() == 0);
#endif

  //------------------
#ifdef MR_TASK_QUEUE_POST_ORDER_DEPTH_FIRST
  getTaskQueue()->addToTaskList(dummyTask);
#endif

  NM_END_PROFILING(); // "queuingTasks"
}

//----------------------------------------------------------------------------------------------------------------------
bool Network::updateNetworkTime(float updateTime, bool isAbsTime)
{
  // Get the last frame's time
  NodeBinEntry* networkUpdateTimeEntry = getAttribDataNodeBinEntry(
      ATTRIB_SEMANTIC_UPDATE_TIME_POS,
      NETWORK_NODE_ID,
      INVALID_NODE_ID,
      this->getCurrentFrameNo() - 1);
  AttribDataUpdatePlaybackPos* networkUpdateTime = networkUpdateTimeEntry->getAttribData<AttribDataUpdatePlaybackPos>();

  // Cache the last update time in case it is needed by any calculations this frame
  m_lastUpdateTimeStep = networkUpdateTime->m_isAbs ? 0.0f : networkUpdateTime->m_value;

  networkUpdateTime->m_isFraction = false;
  networkUpdateTime->m_isAbs = isAbsTime;
  networkUpdateTime->m_value = updateTime;

  networkUpdateTimeEntry->m_address.m_validFrame = this->getCurrentFrameNo();
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void Network::updateActiveNodeConnections(
  NodeID nodeID,
  uint16_t numActiveChildNodes,
  const NodeID* activeChildNodeIDs)
{
  NodeConnections* nodeConnections = getActiveNodesConnections(nodeID);
  NMP_ASSERT(nodeConnections);

  //------------------------
  // Delete any child nodes that have become inactive this frame.
  for (uint16_t i = 0; i < nodeConnections->m_numActiveChildNodes; ++i)
  {
    NodeID lastUpdateChild = nodeConnections->m_activeChildNodeIDs[i];
    bool foundNode = false;

    // Find the last update node in the list of this frames active child nodes
    for (uint16_t j = 0; j < numActiveChildNodes; ++j)
    {
      if (lastUpdateChild == activeChildNodeIDs[j])
      {
        foundNode = true;
        break;
      }
    }

    // Delete the inactive node
    if (!foundNode)
    {
      deleteNodeInstance(lastUpdateChild);
    }
  }

  //------------------------
  // Update this nodes active child nodes
  nodeConnections->m_numActiveChildNodes = numActiveChildNodes;
  for (uint16_t i = 0; i < numActiveChildNodes; ++i)
  {
    nodeConnections->m_activeChildNodeIDs[i] = activeChildNodeIDs[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool Network::setActiveAnimSetIndex(AnimSetIndex animationSetIndex)
{
  m_activeAnimSetIndex->m_value = animationSetIndex;

  // MORPH-21369: Deal with purging the attrib data list in the network for any attributes that
  // were created for the previous animation set.

  if (sm_animSetChangedCB)
  {
    sm_animSetChangedCB(this, animationSetIndex);
  }

  setOutputAnimSetIndex(NETWORK_NODE_ID, animationSetIndex);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void Network::queueSetStateMachineStateByNodeID(
  NodeID stateMachineNodeID,
  NodeID targetStateNodeID)
{
  NMP_ASSERT(m_netDef);

  // Get state machine.
  NodeBinEntry* smEntry = getAttribDataNodeBinEntry(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, stateMachineNodeID);
  NMP_ASSERT(smEntry);
  AttribDataStateMachine* sm = smEntry->getAttribData<AttribDataStateMachine>();
  NMP_ASSERT(sm);

  // Get state machine def.
  AttribDataHandle* handle = m_netDef->getAttribDataHandle(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, stateMachineNodeID);
  NMP_ASSERT(handle);
  AttribDataStateMachineDef* smDef = (AttribDataStateMachineDef*)handle->m_attribData;
  NMP_ASSERT(smDef && smDef->getType() == ATTRIB_TYPE_STATE_MACHINE_DEF);

  sm->queueSetStateByNodeID(targetStateNodeID, smDef);
}

//----------------------------------------------------------------------------------------------------------------------
void Network::clearMessageStatusesOnStateMachine(
  NodeID stateMachineNodeID)  // State machine to amend the status of requests within.
{
  NMP_ASSERT(m_netDef);

  // Get state machine.
  NodeBinEntry* smEntry = getAttribDataNodeBinEntry(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, stateMachineNodeID);
  // It's possible that this state machine has not been constructed yet, if for example we are updating the network
  //  in the asset compiler during a node builder's execution.
  if (!smEntry)
  {
    // Nothing can be done for this SM at the moment, so we return.
    return;
  }

  AttribDataStateMachine* sm = smEntry->getAttribData<AttribDataStateMachine>();
  NMP_ASSERT(sm);

  // Get state machine def.
  AttribDataHandle* handle = m_netDef->getAttribDataHandle(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, stateMachineNodeID);
  NMP_ASSERT(handle);
  AttribDataStateMachineDef* smDef = (AttribDataStateMachineDef*)handle->m_attribData;
  NMP_ASSERT(smDef && smDef->getType() == ATTRIB_TYPE_STATE_MACHINE_DEF);

  sm->resetActiveStateMessageConditions(smDef);
}

//----------------------------------------------------------------------------------------------------------------------
void Network::clearMessageStatusesOnAllStateMachines()
{
  NMP_ASSERT(m_netDef);
  const NodeIDsArray* stateMachineNodeIDs = m_netDef->getStateMachineNodeIDs();
  NMP_ASSERT(stateMachineNodeIDs);

  for (uint32_t i = 0; i < stateMachineNodeIDs->getNumEntries(); ++i)
  {
    NodeID smNodeID = stateMachineNodeIDs->getEntry(i);
    clearMessageStatusesOnStateMachine(smNodeID);
  }
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Network::broadcastScaleCharacterMessage(const char * messageName)
{
  return broadcastPresetMessage( MESSAGE_TYPE_SCALE_CHARACTER, messageName );
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Network::broadcastPresetMessage(const MessageType messageType, const char * messageName)
{
  const Message * message = NULL;
  const MessageDistributor * distributor = m_netDef->getMessageDistributorAndPresetMessage(messageType, messageName, &message);
  if( message != NULL )
  {
    return broadcastMessage( *message, distributor );
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Network::broadcastMessage(const Message& message)
{
  const MessageDistributor * distributor = m_netDef->getMessageDistributor(message.m_id);
  
  return broadcastMessage(message, distributor);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Network::broadcastMessage(const Message& message, const MessageDistributor* distributor)
{
  uint32_t sentMessageCount = 0;

  if (distributor)
  {
    // Send the message to all nodes interested in this message type
    for (uint32_t nodeIndex = 0; nodeIndex < distributor->m_numNodeIDs; ++nodeIndex)
    {
      const MR::NodeID nodeID = distributor->m_nodeIDs[nodeIndex];
      NMP_ASSERT(nodeID < m_netDef->getNumNodeDefs());
      NodeDef* nodeDef = m_netDef->getNodeDef(nodeID);
      if (nodeDef->handleMessage(this, message))
      {
        ++sentMessageCount;
      }
    }

#if defined(MR_OUTPUT_DEBUGGING)
    NMP_ASSERT(m_dispatcher);
    InstanceDebugInterface* debugInterface = m_dispatcher->getDebugInterface();
    if (debugInterface)
      debugInterface->logStateMachineMessageEventMsg(message.m_id, INVALID_NODE_ID, message.m_status);
#endif // MR_OUTPUT_DEBUGGING
  }

  return sentMessageCount;
}

//----------------------------------------------------------------------------------------------------------------------
// Will return nothing if state machine is inactive.
uint32_t Network::getActiveMessageIDsFromActiveStateMachine(
  NodeID     stateMachineNodeID, // State machine to query.
  MessageID* messageIDs,         // Out.
  uint32_t   maxNumRequestIDs)   // Size of messages array.
{
  // Get state machine state attribute data
  NodeBinEntry* stateMachineDataEntry = getAttribDataNodeBinEntry(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, stateMachineNodeID);
  NMP_ASSERT(stateMachineDataEntry);
  AttribDataStateMachine* stateMachineData = stateMachineDataEntry->getAttribData<AttribDataStateMachine>();

  // Get state machine definition attribute data
  AttribDataHandle* handle = getNetworkDef()->getAttribDataHandle(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, stateMachineNodeID);
  NMP_ASSERT(handle);
  NMP_ASSERT(handle->m_attribData->getType() == ATTRIB_TYPE_STATE_MACHINE_DEF);
  AttribDataStateMachineDef* stateMachineDef = (AttribDataStateMachineDef*)handle->m_attribData;

  return stateMachineData->getMessageIDsFromActiveState(messageIDs, maxNumRequestIDs, stateMachineDef, m_netDef);
}

//----------------------------------------------------------------------------------------------------------------------
// Some of the message IDs may be duplicated in the final result, if they can trigger transitions from more than one active state.
uint32_t Network::getActiveMessageIDsFromAllActiveStateMachines(
  MessageID* messageIDs,         // Out.
  uint32_t   maxNumRequestIDs)   // Size of messages array.
{
  NodeBinEntry* stateMachineDataEntry;
  AttribDataHandle* handle;
  AttribDataStateMachine* stateMachineData;
  AttribDataStateMachineDef* stateMachineDef;
  uint32_t messageCount = 0;
  uint32_t adjustedMaxNumRequestIDs = 0;

  const NodeIDsArray* stateMachineNodeIDs = m_netDef->getStateMachineNodeIDs();
  NMP_ASSERT(stateMachineNodeIDs);

  for (uint32_t i = 0; i < stateMachineNodeIDs->getNumEntries(); ++i)
  {
    NodeID smNodeID = stateMachineNodeIDs->getEntry(i);
    if (nodeIsActive(smNodeID))
    {
      // Get state machine state attribute data
      stateMachineDataEntry = getAttribDataNodeBinEntry(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, smNodeID);
      NMP_ASSERT(stateMachineDataEntry);
      stateMachineData = stateMachineDataEntry->getAttribData<AttribDataStateMachine>();

      // Get state machine definition attribute data
      handle = getNetworkDef()->getAttribDataHandle(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, smNodeID);
      NMP_ASSERT(handle);
      NMP_ASSERT(handle->m_attribData->getType() == ATTRIB_TYPE_STATE_MACHINE_DEF);
      stateMachineDef = (AttribDataStateMachineDef*) handle->m_attribData;
      
      // Work out how many slots we have remaining in our output array.
      if (messageCount < maxNumRequestIDs)
        adjustedMaxNumRequestIDs = maxNumRequestIDs - messageCount;
      else
        adjustedMaxNumRequestIDs = 0;

      // Get the active messages from the state machine.
      messageCount += stateMachineData->getMessageIDsFromActiveState(
                        &(messageIDs[messageCount]),
                        adjustedMaxNumRequestIDs,
                        stateMachineDef,
                        m_netDef);
    }
  }

  NMP_ASSERT_MSG(messageCount <= maxNumRequestIDs, "There are more message IDs than fit in the array");

  return messageCount;
}

//----------------------------------------------------------------------------------------------------------------------
NodeID Network::getStateMachineCurrentStateNodeId(NodeID stateMachineNodeID)
{
  // Get state machine state attribute data
  NodeBinEntry* stateMachineDataEntry = getAttribDataNodeBinEntry(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, stateMachineNodeID);
  NMP_ASSERT(stateMachineDataEntry);
  AttribDataStateMachine* stateMachineData = stateMachineDataEntry->getAttribData<AttribDataStateMachine>();

  // Get state machine definition attribute data
  AttribDataHandle* handle = getNetworkDef()->getAttribDataHandle(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, stateMachineNodeID);
  NMP_ASSERT(handle);
  NMP_ASSERT(handle->m_attribData->getType() == ATTRIB_TYPE_STATE_MACHINE_DEF);
  AttribDataStateMachineDef* stateMachineDef = (AttribDataStateMachineDef*)handle->m_attribData;

  return stateMachineData->getActiveNodeID(stateMachineDef);
}

//----------------------------------------------------------------------------------------------------------------------
NodeID Network::getDestinationNodeIDFromATransition(NodeID nodeID) const
{
  NMP_USED_FOR_ASSERTS( const NodeDef* const nodeDef = getNetworkDef()->getNodeDef(nodeID); )
    NMP_ASSERT(nodeDef && (nodeDef->getNodeFlags() & NodeDef::NODE_FLAG_IS_TRANSITION));

  // Note that transition could be dead blending and hence have only 1 child.
  uint32_t numActiveChildren = getNumActiveChildren(nodeID);
  NMP_ASSERT((numActiveChildren == 1) || (numActiveChildren == 2));
  NodeID destNodeID = getActiveChildNodeID(nodeID, numActiveChildren - 1);
  return destNodeID;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Network::getNumActiveControlParameterNodes()
{
  uint32_t numControlParams = 0;

  for (NodeID i = 0; i < m_netDef->getNumNodeDefs(); ++i)
  {
    NodeDef* nodeDef = m_netDef->getNodeDef(i);
    NodeDef::NodeFlags nodeFlags = nodeDef->getNodeFlags();
    NodeBin* nodeBin = getNodeBin(i);
    if (nodeFlags.isSet(NodeDef::NODE_FLAG_IS_CONTROL_PARAM) && (nodeBin->getLastFrameUpdate() == m_currentFrameNo))
    {
      ++numControlParams;
    }
  }

  return numControlParams;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Network::getActiveControlParameterNodeIDs(
  NodeID*   nodeIDs,                              // Out.
  uint32_t  NMP_USED_FOR_ASSERTS(maxNumNodeIDs)) // Size of node IDs array.
{
  uint32_t numControlParams = 0;

  for (NodeID i = 0; i < m_netDef->getNumNodeDefs(); ++i)
  {
    NodeDef* nodeDef = m_netDef->getNodeDef(i);
    NodeDef::NodeFlags nodeFlags = nodeDef->getNodeFlags();
    NodeBin* nodeBin = getNodeBin(i);
    if (nodeFlags.isSet(NodeDef::NODE_FLAG_IS_CONTROL_PARAM) && (nodeBin->getLastFrameUpdate() == m_currentFrameNo))
    {
      NMP_ASSERT(numControlParams < maxNumNodeIDs);
      nodeIDs[numControlParams] = i;
      ++numControlParams;
    }
  }

  return numControlParams;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Network::getNumActiveControlParamAndOpNodes()
{
  uint32_t count = 0;

  for (NodeID i = 0; i < m_netDef->getNumNodeDefs(); ++i)
  {
    NodeDef* nodeDef = m_netDef->getNodeDef(i);
    NodeDef::NodeFlags nodeFlags = nodeDef->getNodeFlags();
    if ((nodeFlags.isSet(NodeDef::NODE_FLAG_IS_CONTROL_PARAM) || nodeFlags.isSet(NodeDef::NODE_FLAG_IS_OPERATOR_NODE)) &&
        operatorCPNodeIsActive(i))
    {
      ++count;
    }
  }

  return count;
}

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)

void Network::sendInstanceProfileTimingDebugOutput()
{
  InstanceDebugInterface* debugInterface = m_dispatcher->getDebugInterface();

  //-----------------------------------------
  // transmit back profile point data
  if (debugInterface && debugInterface->debugOutputsAreOn(DEBUG_OUTPUT_PROFILING))
  {
    for (uint32_t i = 0; i < Network::NumOutputDebuggingProfilerPoints; i++)
    {
      debugInterface->logProfilePointTiming(m_profilePointNames[i], m_outDbgTimings[i]);
    }
  }
}

#endif // MR_OUTPUT_DEBUGGING

//----------------------------------------------------------------------------------------------------------------------
uint32_t Network::getMaxTransformCount() const
{
  return getNetworkDef()->getMaxBoneCount();
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Network::getCurrentTransformCount()
{
  return getNetworkDef()->getRig(getActiveAnimSetIndex())->getNumBones();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::DataBuffer* Network::getTransforms()
{
  return getNodeTransformsBuffer(m_netDef->getRootNodeID(), this, m_currentFrameNo, (AnimSetIndex)m_activeAnimSetIndex->m_value);
}

//----------------------------------------------------------------------------------------------------------------------
AnimRigDef* Network::getActiveRig()
{
  return getNetworkDef()->getRig(getActiveAnimSetIndex());
}

//----------------------------------------------------------------------------------------------------------------------
AnimRigDef* Network::getRig(AnimSetIndex animationSetIndex) const
{
  return getNetworkDef()->getRig(animationSetIndex);
}

//----------------------------------------------------------------------------------------------------------------------
CharacterControllerDef* Network::getActiveCharacterControllerDef()
{
  return getNetworkDef()->getCharacterControllerDef(getActiveAnimSetIndex());
}

//----------------------------------------------------------------------------------------------------------------------
Network::PostUpdateAccessAttribEntry* Network::findPostUpdateAccessAttrib(NodeID nodeID, AttribDataSemantic semantic) const
{
  NMP_ASSERT(m_netDef && (nodeID < m_netDef->getNumNodeDefs()));
  PostUpdateAccessAttribEntry* currentEntry = m_postUpdateAccessAttribEntries[nodeID];

  while (currentEntry)
  {
    if (semantic == currentEntry->m_semantic)
    {
      return currentEntry;
    }
    currentEntry = currentEntry->m_next;
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
void Network::addPostUpdateAccessAttrib(NodeID nodeID, AttribDataSemantic semantic, uint16_t minLifespan, bool fromParent)
{
  NMP_ASSERT(m_netDef && (nodeID < m_netDef->getNumNodeDefs()));

  // Find if this entry already and early out if it does.
  PostUpdateAccessAttribEntry* currentEntry = m_postUpdateAccessAttribEntries[nodeID];
  while (currentEntry)
  {
    if (semantic == currentEntry->m_semantic)
    {
      // This entry already exists; update its lifespan and refCount then exit.
      currentEntry->m_minLifespan = NMP::maximum(currentEntry->m_minLifespan, minLifespan);
      currentEntry->m_refCount++;
      return;
    }
    currentEntry = currentEntry->m_next;
  }

  // Add a new entry as it does not already exist.
  PostUpdateAccessAttribEntry* newEntry = (PostUpdateAccessAttribEntry*)
    NMPAllocatorMemAlloc(getPersistentMemoryAllocator(), sizeof(PostUpdateAccessAttribEntry), NMP_NATURAL_TYPE_ALIGNMENT);
  NMP_ASSERT(newEntry);
  newEntry->m_nodeID = nodeID;
  newEntry->m_semantic = semantic;
  newEntry->m_prev = NULL;
  newEntry->m_minLifespan = minLifespan;
  newEntry->m_refCount = 1;

  // Insert at the beginning.
  PostUpdateAccessAttribEntry* currentHead = m_postUpdateAccessAttribEntries[nodeID];
  newEntry->m_next = currentHead;
  if (currentHead)
    currentHead->m_prev = newEntry;
  m_postUpdateAccessAttribEntries[nodeID] = newEntry;

  // If this node is a filter then recurse to parent or child appropriately.
  NMP_ASSERT(m_netDef);
  NodeDef* nodeDef = m_netDef->getNodeDef(nodeID);
  NMP_ASSERT(nodeDef);
  if (nodeDef->getNodeFlags().isSet(NodeDef::NODE_FLAG_IS_FILTER))
  {
    NodeConnections* connections = getActiveNodesConnections(nodeID);
    NMP_ASSERT(connections);

    if (fromParent)
    {
      if (connections->m_numActiveChildNodes > 0)
      {
        addPostUpdateAccessAttrib(
                              connections->m_activeChildNodeIDs[nodeDef->getPassThroughChildIndex()],
                              semantic,
                              minLifespan,
                              fromParent);
      }
    }
    else
    {
      addPostUpdateAccessAttrib(connections->m_activeParentNodeID, semantic, minLifespan, fromParent);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Find out if the specified piece of attribute data needs to be accessed post the queue execution.
// returns maximum of the input existingLifespan and the m_minLifespan of any found existing entry.
uint16_t Network::getPostUpdateAccessAttribLifespan(NodeID nodeID, AttribDataSemantic semantic, uint16_t existingLifespan/*=0*/)
{
  // Try and find an entry with matching nodeID and semantic.
  PostUpdateAccessAttribEntry* currentEntry = m_postUpdateAccessAttribEntries[nodeID];
  while (currentEntry)
  {
    if (semantic == currentEntry->m_semantic)
    {
      // This entry exists.
      existingLifespan = NMP::maximum(currentEntry->m_minLifespan, existingLifespan);
      break;
    }
    currentEntry = currentEntry->m_next;
  }

  return existingLifespan;
}

//----------------------------------------------------------------------------------------------------------------------
bool Network::removePostUpdateAccessAttrib(NodeID nodeID, AttribDataSemantic semantic)
{
  // Try and find this entry in the nodes list.
  PostUpdateAccessAttribEntry* currentEntry = m_postUpdateAccessAttribEntries[nodeID];
  while (currentEntry)
  {
    if (semantic == currentEntry->m_semantic)
    {
      // Entry exists.
      // Only remove when the reference count has reached 0.
      currentEntry->m_refCount--;
      if (currentEntry->m_refCount == 0)
      {
        if (currentEntry->m_next)
          currentEntry->m_next->m_prev = currentEntry->m_prev;
        if (currentEntry->m_prev)
          currentEntry->m_prev->m_next = currentEntry->m_next;
        else
          m_postUpdateAccessAttribEntries[nodeID] = currentEntry->m_next;

        getPersistentMemoryAllocator()->memFree(currentEntry);

        return true;
      }
    }
    currentEntry = currentEntry->m_next;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool Network::findConditionAttrib(AttribAddress& address)
{
  ConditionAttribEntry* currentEntry = m_conditionAttribList;
  while (currentEntry)
  {
    if ((currentEntry->m_conditionAttrib.m_owningNodeID == address.m_owningNodeID) &&
        (currentEntry->m_conditionAttrib.m_semantic == address.m_semantic))
    {
      return true;
    }
    currentEntry = currentEntry->m_next;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void Network::addConditionAttrib(AttribAddress& address, AttribDataType attribType)
{
  if (!findConditionAttrib(address)) // Avoid adding the same attrib multiple times.
  {
    NMP::TempMemoryAllocator* allocator = getTempMemoryAllocator();
    ConditionAttribEntry* newCondAttrib = (ConditionAttribEntry*)NMPAllocatorMemAlloc(allocator,
                                            sizeof(ConditionAttribEntry), NMP_NATURAL_TYPE_ALIGNMENT);
    NMP_ASSERT(newCondAttrib);
    newCondAttrib->m_conditionAttrib = address;
    newCondAttrib->m_attribType = attribType;
    newCondAttrib->m_next = m_conditionAttribList; // Insert at the beginning
    m_conditionAttribList = newCondAttrib;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 Network::getTranslationChange()
{
  AttribDataSemantic trajectorySemantic = getTrajectoryDeltaSemantic();
  NodeBinEntry* deltaTrajectoryEntry = getAttribDataNodeBinEntry(
    trajectorySemantic,
    m_netDef->getRootNodeID(),
    INVALID_NODE_ID,
    m_currentFrameNo,
    (uint16_t)m_activeAnimSetIndex->m_value);

  if (deltaTrajectoryEntry)
  {
    if (trajectorySemantic == ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM)
    {
      return deltaTrajectoryEntry->getAttribData<AttribDataTrajectoryDeltaTransform>()->m_deltaPos;
    }
    else if (trajectorySemantic == ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER)
    {
      if (deltaTrajectoryEntry->getAttribData<AttribDataTransformBuffer>()->m_transformBuffer->hasChannel(0))
        return *deltaTrajectoryEntry->getAttribData<AttribDataTransformBuffer>()->m_transformBuffer->getPosQuatChannelPos(0);
      else
        return NMP::Vector3(NMP::Vector3::InitZero);
    }
    NMP_ASSERT_FAIL_MSG("Un-expected delta trajectory semantic");
  }

  return NMP::Vector3(NMP::Vector3::InitZero);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Quat Network::getOrientationChange()
{
  AttribDataSemantic trajectorySemantic = getTrajectoryDeltaSemantic();
  NodeBinEntry* deltaTrajectoryEntry = getAttribDataNodeBinEntry(
    trajectorySemantic,
    m_netDef->getRootNodeID(),
    INVALID_NODE_ID,
    m_currentFrameNo,
    (uint16_t)m_activeAnimSetIndex->m_value);

  if (deltaTrajectoryEntry)
  {
    if (trajectorySemantic == ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM)
    {
      return deltaTrajectoryEntry->getAttribData<AttribDataTrajectoryDeltaTransform>()->m_deltaAtt;
    }
    else if (trajectorySemantic == ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER)
    {
      if (deltaTrajectoryEntry->getAttribData<AttribDataTransformBuffer>()->m_transformBuffer->hasChannel(0))
      return *deltaTrajectoryEntry->getAttribData<AttribDataTransformBuffer>()->m_transformBuffer->getPosQuatChannelQuat(0);
      else
        return NMP::Quat(NMP::Quat::kIdentity);
    }
    NMP_ASSERT_FAIL_MSG("Un-expected delta trajectory semantic");
  }

  return NMP::Quat(NMP::Quat::kIdentity);
}

//----------------------------------------------------------------------------------------------------------------------
void Network::markActiveNodesUnder(NodeID nodeID)
{
  NMP_ASSERT(m_activeNodesConnections[nodeID]->m_active || m_netDef->getNodeDef(nodeID)->getNodeFlags().isSet(NodeDef::NODE_FLAG_OUTPUT_REFERENCED) || nodeID == NETWORK_NODE_ID);
  m_activeNodesConnections[nodeID]->m_active = true;
  {
    const uint32_t activeChildCount = getNumActiveChildren(nodeID);
    for (uint32_t childIndex = 0; childIndex < activeChildCount; ++childIndex)
    {
      NodeID childID = getActiveChildNodeID(nodeID, childIndex);
      markActiveNodesUnder(childID);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Network::getActiveNodeCountUnder(NodeID nodeID) const
{
  uint32_t activeNodeCount = 0;
  NMP_ASSERT(nodeIsActive(nodeID));
  {
    ++activeNodeCount;
    const uint32_t activeChildCount = getNumActiveChildren(nodeID);
    for (uint32_t childIndex = 0; childIndex < activeChildCount; ++childIndex)
    {
      NodeID childID = getActiveChildNodeID(nodeID, childIndex);
      const NodeConnections* connections = getActiveNodesConnections(childID);
      if (!connections->m_activeGatherTag) // To avoid counting a node more than once.
      {
        connections->m_activeGatherTag = true;
        activeNodeCount += getActiveNodeCountUnder(childID);
      }
    }
  }

  return activeNodeCount;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Network::getActiveNodesUnder(NodeID nodeID, NodeID* nodeIDsArray) const
{
  uint32_t activeNodeCount = 0;
  NMP_ASSERT(nodeIsActive(nodeID));
  {
    ++activeNodeCount;
    *nodeIDsArray = nodeID;
    const uint32_t activeChildCount = getNumActiveChildren(nodeID);
    for (uint32_t childIndex = 0; childIndex < activeChildCount; ++childIndex)
    {
      NodeID childID = getActiveChildNodeID(nodeID, childIndex);
      const NodeConnections* connections = getActiveNodesConnections(childID);
      if (!connections->m_activeGatherTag) // To avoid counting a node more than once.
      {
        connections->m_activeGatherTag = true;
        activeNodeCount += getActiveNodesUnder(childID, nodeIDsArray + activeNodeCount);
      }
    }
  }

  return activeNodeCount;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Network::resetGatherFlagsUnder(NodeID nodeID) const
{
  uint32_t activeNodeCount = 0;
  NMP_ASSERT(nodeIsActive(nodeID));
  {
    ++activeNodeCount;
    const uint32_t activeChildCount = getNumActiveChildren(nodeID);
    for (uint32_t childIndex = 0; childIndex < activeChildCount; ++childIndex)
    {
      NodeID childID = getActiveChildNodeID(nodeID, childIndex);
      const NodeConnections* connections = getActiveNodesConnections(childID);
      if (connections->m_activeGatherTag) // To avoid counting a node more than once.
      {
        connections->m_activeGatherTag = false;
        activeNodeCount += resetGatherFlagsUnder(childID);
      }
    }
  }

  return activeNodeCount;
}

//----------------------------------------------------------------------------------------------------------------------
// Get the number of active nodes this frame.
uint32_t Network::getActiveNodeCount() const
{
  uint32_t activeNodeCount = getActiveNodeCountUnder(NETWORK_NODE_ID);
#ifdef NMP_ENABLE_ASSERTS
  uint32_t resetFlags =
#endif
    resetGatherFlagsUnder(NETWORK_NODE_ID);
  NMP_ASSERT(activeNodeCount == resetFlags);

  return activeNodeCount;
}

//----------------------------------------------------------------------------------------------------------------------
// Copy active node IDs into the provided buffer.
uint32_t Network::getActiveNodes(NodeID* nodeIDsArray, uint32_t NMP_USED_FOR_ASSERTS(arraySize)) const
{
  uint32_t activeNodeCount = getActiveNodesUnder(NETWORK_NODE_ID, nodeIDsArray);
  NMP_ASSERT(activeNodeCount <= arraySize);
#ifdef NMP_ENABLE_ASSERTS
  uint32_t resetFlags =
#endif
    resetGatherFlagsUnder(NETWORK_NODE_ID);
  NMP_ASSERT(activeNodeCount == resetFlags);

  return activeNodeCount;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Network::getTriggeredNodeEventMessageCount() const
{
  return m_numTriggeredExternalMessages;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Network::getTriggeredNodeEventMessages(MessageID* messageIDsArray, uint32_t NMP_USED_FOR_ASSERTS(arraySize)) const
{
  uint32_t messageCount = getTriggeredNodeEventMessageCount();
  NMP_ASSERT(messageCount <= arraySize);

  for (uint32_t messageIndex = 0; messageIndex < messageCount; messageIndex++)
  {
    NMP::Memory::memcpy(messageIDsArray, m_triggeredExternalMessages, messageCount * sizeof(MessageID));
  }

  return messageCount;
}

//----------------------------------------------------------------------------------------------------------------------
bool Network::isTriggeredNodeEventMessageEmitted(MessageID messageID) const
{
  NMP_ASSERT(messageID != INVALID_MESSAGE_ID)
  uint32_t messageCount = getTriggeredNodeEventMessageCount();
  for(uint32_t i=0; i<messageCount; ++i)
  {
    if( messageID == *(m_triggeredExternalMessages+i))
      return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool Network::isTriggeredNodeEventMessageEmitted(const char * messageName) const
{
  MessageID messageID = getNetworkDef()->getMessageIDFromMessageName(messageName);

  return isTriggeredNodeEventMessageEmitted(messageID);
}

//----------------------------------------------------------------------------------------------------------------------
void Network::clearExternalTriggeredNodeEventMessages()
{
  m_numTriggeredExternalMessages = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void Network::cleanNodeData(NodeID nodeID)
{
  NMP_ASSERT(m_netDef);
  NMP_ASSERT(nodeID < m_netDef->getNumNodeDefs());

  // Empty the AttribData bin.
  NodeBin* nodeBin = getNodeBin(nodeID);
  nodeBin->deleteAllAttribData();

  // Clear this nodes queued task list.
  nodeBin->clearQueuedTasks();

  // Reset the nodes active connections structure for re-use.
  m_activeNodesConnections[nodeID]->cleanActiveConnections(m_netDef->getNodeDef(nodeID));
}

//----------------------------------------------------------------------------------------------------------------------
void Network::updateEmittedMessages()
{
  NMP_ASSERT(m_netDef);
  const NodeIDsArray* messageEmitterNodeIDs = m_netDef->getMessageEmitterNodeIDs();
  NMP_ASSERT(messageEmitterNodeIDs);

  // iterate through nodes in the message emitter list
  for (uint32_t i = 0; i < messageEmitterNodeIDs->getNumEntries(); i++)
  {
    NodeID nodeID = messageEmitterNodeIDs->getEntry(i);
    updateEmittedMessages(nodeID);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Network::updateEmittedMessages(NodeID nodeID)
{
  // retrieve any emitted messages from the previous frame
  AttribAddress emittedRequestsAttribAddress(ATTRIB_SEMANTIC_EMITTED_MESSAGES, nodeID, nodeID, VALID_FRAME_ANY_FRAME);
  NodeBinEntry* emittedRequestsEntry = getAttribDataNodeBinEntry(emittedRequestsAttribAddress);

  if (emittedRequestsEntry)
  {
    AttribDataEmittedMessages* emittedRequestsAttrib = emittedRequestsEntry->getAttribData<AttribDataEmittedMessages>();
       
    // Retrieve the message mapping for the emitted messages.  
    //  Nodes that emit messages must have a map that to convert local ids to global message ids.
    AttribDataEmittedMessagesMap* messagesMap = m_netDef->getAttribData<AttribDataEmittedMessagesMap>(ATTRIB_SEMANTIC_EMITTED_MESSAGES_MAP, nodeID);

    uint32_t nodeNumRequestMaps = messagesMap->getEmittedMessageMapLength();
    uint32_t messageIDBit = 1;

    // Check all messages emitted by this node.
    // MORPH-21370: optimize by counting leading zeros, early out for no bits left.
    for (uint32_t message = 0; message < nodeNumRequestMaps; message++)
    {
      uint32_t emittedRequests = emittedRequestsAttrib->m_value;
      if (emittedRequests & messageIDBit)
      {
        const EmittedMessageMap* emittedMessageMapEntry = messagesMap->getEmittedMessageEntry(message);
        NodeID stateMachineNodeID = emittedMessageMapEntry->stateMachineNodeID;
        MessageID messageID = emittedMessageMapEntry->messageID;
        if (stateMachineNodeID != INVALID_NODE_ID)
        {
          // request or reset?
          if (messageID != INVALID_MESSAGE_ID)
          {
            bool status = messagesMap->isEntryUsed(message);

            if (stateMachineNodeID == NETWORK_NODE_ID)
            {
              // 'broadcast' message
              Message requestMessage(messageID, MESSAGE_TYPE_REQUEST, status, 0, 0);
              broadcastMessage(requestMessage);
            }
            else
            {
              // message for specific state machine
              Message requestMessage(messageID, MESSAGE_TYPE_REQUEST, status, 0, 0);
              sendMessage(stateMachineNodeID, requestMessage);
            }
          }
          else
          {
            if (stateMachineNodeID == NETWORK_NODE_ID)
            {
              // 'broadcast' reset
              clearMessageStatusesOnAllStateMachines();
            }
            else
            {
              // reset a specific state machine
              clearMessageStatusesOnStateMachine(stateMachineNodeID);
            }
          }
        }
        else
        {
          if (messageID != INVALID_MESSAGE_ID)
          {
            // External message.
            NetworkDef::NodeEventMessage eventMessage = {true, messageID};
            storeTriggeredEvent(&eventMessage);
          }
        }
      }
      messageIDBit = messageIDBit << 1;
    }

    // Clear the messages so they're not applied next frame
    emittedRequestsAttrib->m_value = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Network::updateEmittedControlParameters()
{
  // don't try and update anything on the first frame as nothing else will have been updated yet.
  NMP_ASSERT(m_currentFrameNo > 0);

  NMP_ASSERT(m_netDef);
  const EmittedControlParamsInfo* emittedControlParamsInfo = m_netDef->getEmittedControlParamsInfo();
  NMP_ASSERT(emittedControlParamsInfo);

  for (uint32_t i = 0; i < emittedControlParamsInfo->getNumEmittedControlParams(); ++i)
  {
    const EmittedControlParamsInfo::EmittedControlParamData& emittedControlParamData = emittedControlParamsInfo->getEmittedControlParamData(i);

    // Force the update of this emitted control params attrib data.
    NodeDef* owningNodeDef = m_netDef->getNodeDef(emittedControlParamData.m_nodeID);

#if defined(MR_OUTPUT_DEBUGGING)
    MR::NodeID oldDebugNodeID = INVALID_NODE_ID;
    beginNodeFuncDebugHook(this, owningNodeDef, oldDebugNodeID);
#endif // MR_OUTPUT_DEBUGGING

    owningNodeDef->updateOutputCPAttribute(this, 0); // Note: If we ever want user input control parameters to have more
    //       than 1 output pin we will have to include a PinIndex in the
    //       EmittedControlParamData structure (0 assumes only 1).

#if defined(MR_OUTPUT_DEBUGGING)
    endNodeFuncDebugHook(this, oldDebugNodeID);
#endif // MR_OUTPUT_DEBUGGING
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Network::updateRequestEmitterNodes()
{
  // don't try and update anything on the first frame as nothing else will have been updated yet.
  NMP_ASSERT(m_currentFrameNo > 0);

  const NodeIDsArray* messageEmitterNodes = m_netDef->getMessageEmitterNodeIDs();
  for (uint32_t i = 0; i < messageEmitterNodes->getNumEntries(); ++i)
  {
    NodeDef* owningNodeDef = m_netDef->getNodeDef(messageEmitterNodes->getEntry(i));

    // Guard against updating null nodes during sub-network evaluation during asset compilation
    if (owningNodeDef)
    {
#if defined(MR_OUTPUT_DEBUGGING)
      MR::NodeID oldDebugNodeID = INVALID_NODE_ID;
      beginNodeFuncDebugHook(this, owningNodeDef, oldDebugNodeID);
#endif // MR_OUTPUT_DEBUGGING

      owningNodeDef->updateEmittedMessages(this, owningNodeDef->getNumOutputCPPins() - owningNodeDef->getNumReflexiveCPPins() -1);

#if defined(MR_OUTPUT_DEBUGGING)
      endNodeFuncDebugHook(this, oldDebugNodeID);
#endif // MR_OUTPUT_DEBUGGING
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Network::updateAttribDataLifespans(NodeID nodeID)
{
  NMP_ASSERT(nodeID != INVALID_NODE_ID && nodeID < m_netDef->getNumNodeDefs());

  if (getNodeBin(nodeID)->updateAttribDataLifespans(m_currentFrameNo))
  {
    // Iterate to children.
    for (uint32_t i = 0; i < m_activeNodesConnections[nodeID]->m_numActiveChildNodes; ++i)
    {
      NodeID childNodeID = m_activeNodesConnections[nodeID]->m_activeChildNodeIDs[i];
      NMP_ASSERT(childNodeID != NETWORK_NODE_ID && (childNodeID != nodeID));
      updateAttribDataLifespans(childNodeID);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
NodeBinEntry* Network::findIgnoringValidFrame(const AttribAddress& attribAddress)
{
  NodeBinEntry* result = NULL;
  NodeBinEntry* nodeAttribData = getNodeBin(attribAddress.m_owningNodeID)->getEntries();

  while (nodeAttribData)
  {
    if (
      (nodeAttribData->m_address.m_semantic == attribAddress.m_semantic) &&  // It's the right type
      (
        (nodeAttribData->m_address.m_targetNodeID == attribAddress.m_targetNodeID) ||
        (nodeAttribData->m_address.m_targetNodeID == INVALID_NODE_ID)) &&  // It's targeting the right node (or is for all nodes that ask for this owner's attribute of this type)
      (nodeAttribData->m_address.m_animSetIndex == attribAddress.m_animSetIndex))
    {
      result = nodeAttribData;
      break;
    }

    nodeAttribData = nodeAttribData->m_next;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NodeBinEntry* Network::getAttribDataNodeBinEntryRecurseToParent(
  AttribDataSemantic semantic,
  NodeID             owningNodeID,
  NodeID             targetNodeID,
  FrameCount         validFrame,
  AnimSetIndex       animSetIndex)
{
  NodeBinEntry* entry = getAttribDataNodeBinEntry(semantic, owningNodeID, targetNodeID, validFrame, animSetIndex);
  if (!entry)
  {
    // AttribData not found in this node so if our parent passes it through look for it there, and recurse.
    targetNodeID = owningNodeID;
    owningNodeID = getActiveParentNodeID(owningNodeID);
    NMP_ASSERT(owningNodeID != INVALID_NODE_ID);

    NMP_ASSERT(m_netDef);
    NodeDef* nodeDef = m_netDef->getNodeDef(owningNodeID);
    NMP_ASSERT(nodeDef);
    QueueAttrTaskFn queuingFunc = nodeDef->getTaskQueueingFn(semantic);

    // Only pass on the query to our parent if the semantic we are looking for is passed through
    if (queuingFunc == queuePassThroughChild0 || queuingFunc == queuePassThroughChild1 || queuingFunc == stateMachineNodeQueuePassThrough)
    {
      entry = getAttribDataNodeBinEntryRecurseToParent(semantic, owningNodeID, targetNodeID, validFrame, animSetIndex);
    }
  }

  return entry;
}

//----------------------------------------------------------------------------------------------------------------------
void Network::setCharacterController(CharacterControllerInterface* cc)
{
  m_characterController = cc;
  
  NodeBinEntry* nodeBinEntry = getAttribDataNodeBinEntry(ATTRIB_SEMANTIC_CHARACTER_PROPERTIES, NETWORK_NODE_ID);
  if (!nodeBinEntry)
  {
    AttribDataHandle handle = AttribDataCharacterProperties::create(getPersistentMemoryAllocator());
    AttribAddress attribAddress(ATTRIB_SEMANTIC_CHARACTER_PROPERTIES, NETWORK_NODE_ID);
    addAttribData(attribAddress, handle, LIFESPAN_FOREVER);
  }
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataCharacterProperties* Network::getCharacterPropertiesAttribData()
{
  AttribDataCharacterProperties* attribcc = getAttribData<AttribDataCharacterProperties>(
                                                ATTRIB_SEMANTIC_CHARACTER_PROPERTIES,
                                                NETWORK_NODE_ID);
  return attribcc;
}

//----------------------------------------------------------------------------------------------------------------------
const AttribDataCharacterProperties* Network::getCharacterPropertiesAttribData() const
{
  NodeBinEntry* nodeBinEntry = getAttribDataNodeBinEntry(ATTRIB_SEMANTIC_CHARACTER_PROPERTIES, NETWORK_NODE_ID);
  NMP_ASSERT(nodeBinEntry);
  const AttribDataCharacterProperties* attribcc = nodeBinEntry->getAttribData<AttribDataCharacterProperties>();
  return attribcc;
}

//----------------------------------------------------------------------------------------------------------------------
bool Network::getCharacterPropertiesAchievedRequestedMovement()
{
  return getCharacterPropertiesAttribData()->m_achievedRequestedMovement;
}

//----------------------------------------------------------------------------------------------------------------------
void Network::setCharacterPropertiesAchievedRequestedMovement(bool status)
{
  getCharacterPropertiesAttribData()->m_achievedRequestedMovement = status;
}

//----------------------------------------------------------------------------------------------------------------------
void Network::setCharacterPropertiesGroundContactTime(float time)
{
  getCharacterPropertiesAttribData()->m_groundContactTime = time;
}

//----------------------------------------------------------------------------------------------------------------------
void Network::setCharacterPropertiesPhysicsAndCharacterControllerUpdate(
  PhysicsAndCharacterControllerUpdate physicsAndCharacterControllerUpdate)
{
  getCharacterPropertiesAttribData()->m_physicsAndCharacterControllerUpdate = physicsAndCharacterControllerUpdate;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsAndCharacterControllerUpdate Network::getCharacterPropertiesPhysicsAndCharacterControllerUpdate() const
{
  return (PhysicsAndCharacterControllerUpdate) getCharacterPropertiesAttribData()->m_physicsAndCharacterControllerUpdate;
}

//----------------------------------------------------------------------------------------------------------------------
float Network::getCharacterPropertiesGroundContactTime()
{
  return getCharacterPropertiesAttribData()->m_groundContactTime;
}

//----------------------------------------------------------------------------------------------------------------------
void Network::updateCharacterPropertiesWorldRootTransform(
  const NMP::Matrix34& worldRootTransform,
  bool                 copyCurrentToPrevious)
{
  AttribDataCharacterProperties* attribcc = getCharacterPropertiesAttribData();
  if (copyCurrentToPrevious)
    attribcc->m_prevWorldRootTransform = attribcc->m_worldRootTransform;
  attribcc->m_worldRootTransform = worldRootTransform;
}

//----------------------------------------------------------------------------------------------------------------------
void Network::setCharacterPropertiesWorldRootTransform(
  const NMP::Matrix34& worldRootTransform)
{
  AttribDataCharacterProperties* attribcc = getCharacterPropertiesAttribData();
  attribcc->m_prevWorldRootTransform = worldRootTransform;
  attribcc->m_worldRootTransform = worldRootTransform;
}

//----------------------------------------------------------------------------------------------------------------------
const NMP::Matrix34& Network::getCharacterPropertiesWorldRootTransform() const
{
  const AttribDataCharacterProperties* attribcc = getCharacterPropertiesAttribData();
  return attribcc->m_worldRootTransform;
}

//----------------------------------------------------------------------------------------------------------------------
const NMP::Matrix34& Network::getCharacterPropertiesPrevWorldRootTransform() const
{
  const AttribDataCharacterProperties* attribcc = getCharacterPropertiesAttribData();
  return attribcc->m_prevWorldRootTransform;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Network::getActivePhysicsNodes(NodeID*& nodeIDs)
{
  nodeIDs = m_activePhysicsNodeIDs;
  return m_numActivePhysicsNodes;
}

//----------------------------------------------------------------------------------------------------------------------
bool Network::areBehaviourNodesActive() const
{
  return m_behaviourNodesActive;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Network::getActiveGrouperNodes(NodeID*& nodeIDs) {
  nodeIDs = m_activeGrouperNodeIDs;
  return m_numActiveGrouperNodes;
}

//----------------------------------------------------------------------------------------------------------------------
bool Network::isNodeOrActiveChildPhysical(NodeID nodeID) const
{
  MR::NodeDef* nodeDef = m_netDef->getNodeDef(nodeID);
  NMP_ASSERT(nodeDef);
  
  return isNodeOrActiveChildPhysical(nodeDef);
}

//----------------------------------------------------------------------------------------------------------------------
bool Network::isNodeOrActiveChildPhysical(const NodeDef* node) const
{
  if (node->getNodeFlags() & NodeDef::NODE_FLAG_NO_PHYSICAL_CHILDREN)
  {
    // This node and all the nodes underneath it are not physical, so it's impossible for there to be an active physics
    //  node.
    return false;
  }

  if (node->getNodeFlags() & (NodeDef::NODE_FLAG_IS_PHYSICAL | NodeDef::NODE_FLAG_IS_PHYSICS_GROUPER))
  {
    // This node itself is physical
    return true;
  }

  // We haven't found a conclusive result.  This means that there are potentially-active physics nodes below this one.
  // So, we have to iterate down to see if they are active.
  const NodeConnections* nc = getActiveNodesConnections(node->getNodeID());
  for (uint32_t i = 0; i < nc->m_numActiveChildNodes; ++i)
  {
    const NodeDef* child = getNetworkDef()->getNodeDef(nc->m_activeChildNodeIDs[i]);
    if (isNodeOrActiveChildPhysical(child))
      return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool Network::isNodePhysical(const NodeDef* node) const
{
  if (node->getNodeFlags() & NodeDef::NODE_FLAG_IS_PHYSICAL)
    return true;
  if (node->getNodeFlags() & NodeDef::NODE_FLAG_IS_PHYSICS_GROUPER)
  {
    // return true if the grouper is needing to actively convert - i.e. its children are all non-physics
    const NodeConnections* nc = getActiveNodesConnections(node->getNodeID());
    for (uint32_t i = 0 ; i < nc->m_numActiveChildNodes ; ++i)
    {
      const NodeDef* child = getNetworkDef()->getNodeDef(nc->m_activeChildNodeIDs[i]);
      if (!isNodeOrActiveChildPhysical(child))
        return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool Network::setRootControlMethod(RootControlMethod method)
{
  // anim would only be set at the start of the sequence where the root control method is evaluated
  if (method == ROOT_CONTROL_ANIMATION)
  {
    m_rootControlMethod = method;
    return true;
  }

  if (method >= m_rootControlMethod)
  {
    m_rootControlMethod = method;
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
// Call this node's update connections function.
// Returns the node ID of the new root of this network sub-tree; in the majority of cases this is constant, it only
// really becomes important with transition node types.
NodeID Network::updateNodeInstanceConnections(NodeID nodeID, AnimSetIndex animSet)
{
  // output AnimSetIndex must be past through
  NMP_ASSERT(animSet != ANIMATION_SET_ANY);
  bool wasActive = m_activeNodesConnections[nodeID]->m_active;
  m_activeNodesConnections[nodeID]->m_active = true;

  // Update connections can only happen once a per-frame per-node instance - although pass-down
  // pins may result in this getting called more than once.
  NodeBin* nodeBin = getNodeBin(nodeID);
  if (nodeBin->getLastFrameUpdate() == m_currentFrameNo)
    return nodeID;

  // If the node is updated, but it wasn't during the previous update, m_justBecameActive will be true.
  NMP_ASSERT(m_currentFrameNo > 0);
  m_activeNodesConnections[nodeID]->m_justBecameActive = !wasActive;

  // If we have any OnEnter messages on this node, let's trigger those
  MR::NodeDef* owningNodeDef = m_netDef->getNodeDef(nodeID);

  if (m_activeNodesConnections[nodeID]->m_justBecameActive)
  {
    // If the user has registered a node activation callback and this node just became active, notify via the callback.
    if (m_nodeActivationCallback)
    {
      m_nodeActivationCallback(nodeID);
    }

    if (owningNodeDef->getNumOnEnterMessages())
    {
      for (uint32_t i = 0; i < owningNodeDef->getNumOnEnterMessages(); i++)
      {
        storeTriggeredEvent(&owningNodeDef->getOnEnterMessages()[i]);
      }
    }
  }

  m_activeNodesConnections[nodeID]->m_activeGatherTag = false;
  nodeBin->setLastFrameUpdate(m_currentFrameNo);

  if (nodeBin->m_outputAnimSet != animSet)
  {
    setOutputAnimSetIndex(nodeID, animSet);
    // update pinAttribData if animSet changed
    nodeUpdatePinAttribDataInstance(owningNodeDef, this);
  }

#if defined(MR_OUTPUT_DEBUGGING)
  MR::NodeID oldDebugNodeID = INVALID_NODE_ID;
  beginNodeFuncDebugHook(this, owningNodeDef, oldDebugNodeID);
#endif // MR_OUTPUT_DEBUGGING

  NodeID newRootID = owningNodeDef->updateConnections(this);

#if defined(MR_OUTPUT_DEBUGGING)
  endNodeFuncDebugHook(this, oldDebugNodeID);
#endif // MR_OUTPUT_DEBUGGING

  return newRootID;
}

//----------------------------------------------------------------------------------------------------------------------
bool Network::storeTriggeredEvent(NetworkDef::NodeEventMessage* messageToStore)
{
  if (messageToStore->m_external)
  {
    if (m_numTriggeredExternalMessages == m_triggeredExternalMessagesSize)
    {
      m_triggeredExternalMessagesSize = NMP::maximum(2 * m_triggeredExternalMessagesSize, 16U);
      MessageID* triggeredMessages =
        (MessageID*)m_persistentDataAllocator->memAlloc(m_triggeredExternalMessagesSize * sizeof(MessageID),
        NMP_NATURAL_TYPE_ALIGNMENT NMP_MEMORY_TRACKING_ARGS);
      NMP::Memory::memcpy(triggeredMessages, m_triggeredExternalMessages, m_numTriggeredExternalMessages * sizeof(MessageID));
      m_persistentDataAllocator->memFree(m_triggeredExternalMessages);
      m_triggeredExternalMessages = triggeredMessages;
    }

    m_triggeredExternalMessages[m_numTriggeredExternalMessages] = messageToStore->m_msgID;
    m_numTriggeredExternalMessages++;
  }
  else
  {
    // Internal Message
    NodeEventMessageLink* newLink = (NodeEventMessageLink*)m_tempDataAllocator->
      memAlloc(sizeof(NodeEventMessageLink), NMP_NATURAL_TYPE_ALIGNMENT NMP_MEMORY_TRACKING_ARGS);

    newLink->m_next = m_triggeredInternalMessages;
    newLink->m_message = messageToStore;
    m_triggeredInternalMessages = newLink;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_DEBUG

void Network::debugValidateAttribContents(bool validateNoTempAttribs)
{
  NMP_ASSERT(getNetworkDef());
  for (NodeID i = 0; i < getNetworkDef()->getNumNodeDefs(); ++i)
  {
    NodeBinEntry* entry = getNodeBin(i)->getEntries();
    uint32_t nodeAttribsCount = 0;
    while (entry)
    {
      bool entryFail = false;

      const uint32_t numEntries = MR::Manager::getInstance().getNumRegisteredAttribSemantics();

      if (entry->m_address.m_semantic >= numEntries)
      {
        NET_LOG_ERROR_MESSAGE("Error in Node Attribute in Network: entry->m_address.m_semantic >= ATTRIB_SEMANTIC_MAX\n");
        entryFail = true;
      }

      if (entry->m_address.m_owningNodeID >= getNetworkDef()->getNumNodeDefs())
      {
        NET_LOG_ERROR_MESSAGE("Error in Node Attribute in Network: entry->m_address.m_owningNodeID >= getNetworkDef()->getNumNodeDefs()\n");
        entryFail = true;
      }

      if ((entry->m_address.m_targetNodeID >= getNetworkDef()->getNumNodeDefs()) &&
          (entry->m_address.m_targetNodeID != INVALID_NODE_ID))
      {
        NET_LOG_ERROR_MESSAGE("Error in Node Attribute in Network: (entry->m_address.m_targetNodeID >= getNetworkDef()->getNumNodeDefs()) && (entry->m_address.m_targetNodeID != INVALID_NODE_ID)\n");
        entryFail = true;
      }

      if ((entry->m_address.m_animSetIndex >= getNetworkDef()->getNumAnimSets()) &&
          (entry->m_address.m_animSetIndex != ANIMATION_SET_ANY))
      {
        NET_LOG_ERROR_MESSAGE("Error in Node Attribute in Network: (entry->m_address.m_animSetIndex >= getNetworkDef()->getNumAnimSets()) && (entry->m_address.m_animSetIndex != ANIMATION_SET_ANY)\n");
        entryFail = true;
      }

      if (entry->getAttribData() == NULL)
      {
        NET_LOG_ERROR_MESSAGE("Error in Node Attribute in Network: entry->getAttribData() == NULL\n");
        entryFail = true;
      }

      if (entry->getAttribData()->getRefCount() <= 0)
      {
        NET_LOG_ERROR_MESSAGE("Error in Node Attribute in Network: entry->getAttribData()->getRefCount() <= 0\n");
        entryFail = true;
      }

      if (entry->getAttribData()->getRefCount() >= 500 && entry->getAttribData()->getRefCount() != IS_DEF_ATTRIB_DATA) // A sensible, very high limit.
      {
        NET_LOG_ERROR_MESSAGE("Error in Node Attribute in Network: entry->getAttribData()->getRefCount() >= 500\n");
        entryFail = true;
      }

      MR::Manager& manager = MR::Manager::getInstance();
      if (entry->getAttribData()->getType() > manager.getHighestRegisteredAttribDataType())
      {
        NET_LOG_ERROR_MESSAGE("Error in Node Attribute in Network: Attribute type not registered with the Manager\n");
        entryFail = true;
      }

      if (validateNoTempAttribs)
      {
        if (entry->m_lifespan == 0)
        {
          NET_LOG_ERROR_MESSAGE("Error in Node Attribute in Network: Attribute with zero lifespan remaining.\n");
          entryFail = true;
        }

        if ((entry->getAttribData()->m_allocator != getPersistentMemoryAllocator()) &&
            (entry->getAttribData()->m_allocator != NULL)) // Statically allocated attrib data?
        {
          NET_LOG_ERROR_MESSAGE("Error in Node Attribute in Network: (entry->getAttribData()->m_allocator != getPersistentMemoryAllocator()) && (entry->getAttribData()->m_allocator != NULL)\n");
          entryFail = true;
        }
      }
      else
      {
        if ((entry->getAttribData()->m_allocator != getTempMemoryAllocator()) &&
            (entry->getAttribData()->m_allocator != getPersistentMemoryAllocator()) &&
            (entry->getAttribData()->m_allocator != NULL)) // Statically allocated attrib data?
        {
          NET_LOG_ERROR_MESSAGE("Error in Node Attribute in Network: (entry->getAttribData()->m_allocator != getTempMemoryAllocator()) && (entry->getAttribData()->m_allocator != getPersistentMemoryAllocator()) && (entry->getAttribData()->m_allocator != NULL)\n");
          entryFail = true;
        }
      }

      validateInactiveNodeAttribData(i, entry, entryFail);

      if (entryFail)
      {
        NMP_ASSERT(entry);
        NMP_ASSERT(entry->getAttribData());
        debugOutputAttributeInfo(NMP::LOG_PRIORITY_ALWAYS, entry);
        NMP_ASSERT_FAIL();
      }

      ++nodeAttribsCount;
      if (nodeAttribsCount >= 400) // A sensible, high limit.
      {
        NET_LOG_ERROR_MESSAGE("To many attributes in the Network Node bin for Node id %d\n", i);
        debugOutputNodeAttribData(i, NMP::LOG_PRIORITY_ALWAYS);
        NMP_ASSERT_FAIL();
      }

      entry = entry->m_next;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Network::validateInactiveNodeAttribData(NodeID nodeID, NodeBinEntry* entry, bool& entryFail)
{
  NodeBin* nodeBin = getNodeBin(nodeID);
  if ((nodeBin->getLastFrameUpdate() != m_currentFrameNo) &&
      (nodeBin->getLastFrameUpdate() != m_currentFrameNo - 1))
  {
    // None LIFESPAN_FOREVER data hanging around on inactive nodes can potentially be a big waste of memory.
    if (entry->m_lifespan != LIFESPAN_FOREVER)
    {
      // Find if any node in our parent hierarchy is multiply-referenced.
      //  Don't error on multiply-referenced nodes having data hanging around when they are not active,
      //  as long as their true parent is still active.
      //  Only true parents can cause the deletion of multiply-referenced children,
      //  otherwise one of the referencers might delete the node before another referencer has finished with it.
      bool isMultiplyConnected = false;
      NodeDef* multiplyRefedNodeDef = m_netDef->getNodeDef(nodeID);
      do
      {
        if (multiplyRefedNodeDef->getNodeFlags() & NodeDef::NODE_FLAG_OUTPUT_REFERENCED)
          isMultiplyConnected = true;
        else
          multiplyRefedNodeDef = multiplyRefedNodeDef->getParentNodeDef();
      }
      while (!isMultiplyConnected && multiplyRefedNodeDef->getNodeID() != 0);

      if (isMultiplyConnected)
      {
        // We or our parent are multiply-referenced, make sure the first true ancestor that isn't multiply referenced is active.
        NodeID trueParentNodeID = getActiveParentNodeID(multiplyRefedNodeDef->getNodeID());
        while (trueParentNodeID > 0 &&
          m_netDef->getNodeDef(trueParentNodeID)->getNodeFlags() & NodeDef::NODE_FLAG_OUTPUT_REFERENCED)
        {
          trueParentNodeID = getActiveParentNodeID(trueParentNodeID);
        }

        NMP_ASSERT(trueParentNodeID < m_netDef->getNumNodeDefs());
        NodeBin* parentNodeBin = getNodeBin(trueParentNodeID);
        if ((parentNodeBin->getLastFrameUpdate() != m_currentFrameNo) &&
            (parentNodeBin->getLastFrameUpdate() != m_currentFrameNo - 1))
        {
          NET_LOG_ERROR_MESSAGE("Attribute that is not LIFESPAN_FOREVER exists in an inactive node (Node is multiply-referenced and its true parent is not active).\n");
          entryFail = true;
        }
      }
      else
      {
        // We are not multiply-referenced so throw an error.
        NET_LOG_ERROR_MESSAGE("Attribute that is not LIFESPAN_FOREVER exists in an inactive node.\n");
        entryFail = true;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Network::attribNodeAccounting(int32_t MR_USED_FOR_NETWORK_LOGGING(priority))
{
  const static uint32_t MAXIMUM_SENSIBLE_NUMBER_OF_NODE_ATTRIBS = 100;
  NET_LOG_MESSAGE(priority, "\n\n-- Network::attribNodeAccounting --\n");
  NET_LOG_MESSAGE(priority, "-- The following node attribute counts should not increase indefinitely over time.\n");
  NET_LOG_MESSAGE(priority, "-- If they do, it indicates a memory leak bug in the node queueing code.\n\n");
  for (uint32_t nodeID = 0; nodeID < m_netDef->getNumNodeDefs(); nodeID++)
  {
    uint32_t entryCount = 0;
    NodeBinEntry* nodeBinEntry = getNodeBin((NodeID)nodeID)->getEntries();
    while (nodeBinEntry != NULL)
    {
      NET_LOG_MESSAGE(
        priority,
        " - attrib has lifeSpan %i, type %s\n",
        nodeBinEntry->m_lifespan,
        nodeBinEntry->m_address.getAttribSemanticName());
      nodeBinEntry = nodeBinEntry->m_next;
      entryCount++;
    }
    NET_LOG_MESSAGE(priority, "node %i has %i attribs\n", nodeID, entryCount);
    NMP_ASSERT(entryCount < MAXIMUM_SENSIBLE_NUMBER_OF_NODE_ATTRIBS); // This node may be leaking attribs.
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Network::debugOutputAttributeInfo(int32_t MR_USED_FOR_NETWORK_LOGGING(priority), NodeBinEntry* MR_USED_FOR_NETWORK_LOGGING(entry))
{
  NET_LOG_MESSAGE(priority, "                                                    Attrib,               owner, target, anim set,  lifespan, ref count\n");
  
  NET_LOG_MESSAGE(
    priority,
    "    %48s (%3i),   %40s (%3i),    %3i,      %2i,     %5i,     %5i\n",
    entry->m_address.getAttribSemanticName(),
    entry->m_address.m_semantic,
    m_netDef->getNodeNameFromNodeID(entry->m_address.m_owningNodeID),
    entry->m_address.m_owningNodeID,
    entry->m_address.m_targetNodeID,
    entry->m_address.m_animSetIndex,
    entry->m_lifespan,
    entry->getAttribData()->getRefCount());

}

//----------------------------------------------------------------------------------------------------------------------
void Network::debugOutputAttribContents(int32_t MR_USED_FOR_NETWORK_LOGGING(priority), bool countsOnly)
{
  NET_LOG_MESSAGE(priority, "  --------------\n  Attributes in Network\n");
  NMP_ASSERT(m_netDef);
  uint32_t numEntries = 0;

  for (NodeID i = 0; i < m_netDef->getNumNodeDefs(); ++i)
  {
    NodeBin* nodeBin = getNodeBin(i);
    if (nodeBin->getEntries()) // Only output info for nodes that actually have any attributes.
    {
      NMP_ASSERT(m_netDef->getNodeDef(i));
      NET_LOG_MESSAGE(
        priority,
        "   Node ID: %d, Name: %s, Last frame update: %d\n",
        i,
        m_netDef->getNodeNameFromNodeID(i),
        nodeBin->getLastFrameUpdate());
#if defined(MR_NETWORK_LOGGING)
      numEntries += debugOutputNodeAttribData(i, priority, countsOnly);
#endif
    }
  }
  NET_LOG_MESSAGE(priority, "Total Num Attrib Data = %5i\n", numEntries);
}

//----------------------------------------------------------------------------------------------------------------------
// Display a nodes attributes.
uint32_t Network::debugOutputNodeAttribData(NodeID nodeID, int32_t MR_USED_FOR_NETWORK_LOGGING(priority), bool countsOnly)
{
  NodeBin* nodeBin = getNodeBin(nodeID);
  NodeBinEntry* entry = nodeBin->getEntries();
  uint32_t numEntries = 0;

  if (!countsOnly)
  NET_LOG_MESSAGE(priority, "                                                    Attrib, target, anim set,  lifespan, ref count\n");
  while (entry)
  {
    NMP_ASSERT(entry->getAttribData());
    if (!countsOnly)
    NET_LOG_MESSAGE(
      priority,
      "    %48s (%3i),    %3i,       %2i,     %5i,     %5i\n",
      entry->m_address.getAttribSemanticName(),
      entry->m_address.m_semantic,
      entry->m_address.m_targetNodeID,
      entry->m_address.m_animSetIndex,
      entry->m_lifespan,
      entry->getAttribData()->getRefCount());
    entry = entry->m_next;
    numEntries++;
  }

  NET_LOG_MESSAGE(priority, "Num Attrib Data = %5i\n", numEntries);
  return numEntries;
}

//----------------------------------------------------------------------------------------------------------------------
void Network::debugValidateConnectivity(NodeID nodeID)
{
  NMP_ASSERT(nodeID != INVALID_NODE_ID && nodeID < m_netDef->getNumNodeDefs());

  NMP_ASSERT(getNodeBin(nodeID)->getLastFrameUpdate() == m_currentFrameNo)

  // Iterate to children.
  for (uint32_t i = 0; i < m_activeNodesConnections[nodeID]->m_numActiveChildNodes; ++i)
  {
    debugValidateConnectivity(m_activeNodesConnections[nodeID]->m_activeChildNodeIDs[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Network::debugValidateActiveNodes()
{
  uint32_t numActiveNodes = getActiveNodeCount();
  NMP::TempMemoryAllocator* childAllocator = m_tempDataAllocator->createChildAllocator();

  NodeID* activeNodes = (NodeID*)NMPAllocatorMemAlloc(childAllocator, numActiveNodes * sizeof(NodeID), NMP_NATURAL_TYPE_ALIGNMENT);

  getActiveNodes(activeNodes, numActiveNodes);

  for (uint32_t i = 0; i != m_netDef->getNumNodeDefs(); ++i)
  {
    // Have to check if the node exists because sometimes, during closest anim node compilation, the network is evaluated and they don't exist.
    NodeDef* nodeDef = m_netDef->getNodeDef((NodeID)i);
    if (nodeDef)
    {
      bool isActive = false;
      for (uint32_t j = 0; !isActive && j != numActiveNodes; ++j)
      {
        isActive = (i == activeNodes[j]);
      }
      bool isNodeOperatorCP = nodeDef->getNodeFlags().isSet(NodeDef::NODE_FLAG_IS_CONTROL_PARAM) || nodeDef->getNodeFlags().isSet(NodeDef::NODE_FLAG_IS_OPERATOR_NODE);
      bool isNodeActive = isNodeOperatorCP ? false : nodeIsActive((NodeID)i);
      NMP_ASSERT((isActive && isNodeActive) || (!isActive && !isNodeActive));
    }
  }

  m_tempDataAllocator->destroyChildAllocator(childAllocator);
}

//----------------------------------------------------------------------------------------------------------------------
void Network::debugValidateDependencySense(
  NodeID             dependentTaskOwningNodeID,
  NodeID             semanticOwningNodeID, 
  AttribDataSemantic semantic)
{
  NMP_ASSERT( dependentTaskOwningNodeID != INVALID_NODE_ID );

  // There are many circumstances when a node task requires data that is owned by itself as opposed to through 
  // it's normal dependency sense. This is the node effectively asking for it's own data 
  //
  // Importantly also - the dependency sense is to prevent circular dependencies. This case will not produce one.
  if( dependentTaskOwningNodeID == semanticOwningNodeID )
  {
      return;
  }

  // Perform the standard debug dependency check.
  bool dependencySenseIsParentToChild = isAnActiveParentOrGrandParent(dependentTaskOwningNodeID, semanticOwningNodeID);
  AttribSemanticSense semanticSense = Manager::getInstance().getAttributeSemanticSense(semantic);
  switch (semanticSense)
  {
    case ATTRIB_SENSE_FROM_PARENT_TO_CHILD:
      NMP_ASSERT_MSG(
        (dependencySenseIsParentToChild == true) || 
        (dependentTaskOwningNodeID == NETWORK_NODE_ID),
        "A dependency has been set on child semantic %s but the sense of the semantic is parent to child.",
        Manager::getInstance().getAttributeSemanticName(semantic));
      break;
    case ATTRIB_SENSE_FROM_CHILD_TO_PARENT:
      NMP_ASSERT_MSG(
        dependencySenseIsParentToChild == false,
        "A dependency has been set on parent semantic %s but the sense of the semantic is child to parent.",
        Manager::getInstance().getAttributeSemanticName(semantic));
      break;
    case ATTRIB_SENSE_NONE:
      // No validation when there is no stated sense on the semantic because this data cannot cause a dependency flow up or down the network.
      // The semanticOwningNodeID should have a queuing function that deals with creation of this semantic.
      break;
    default:
      NMP_ASSERT_FAIL_MSG("Unknown sense.");
      break;
  } 
}

#endif // NM_DEBUG

//----------------------------------------------------------------------------------------------------------------------
// Callback management functions.
//----------------------------------------------------------------------------------------------------------------------
void Network::setStateMachineStateChangeCB(
  NodeID stateMachineNodeID,
  Network::StateMachineStateChangeCallback *stateMachineStateChangeCB)
{
  // Get the state machine attribute from the network.
  NodeBinEntry* networkEntry = getAttribDataNodeBinEntry(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, stateMachineNodeID);
  NMP_ASSERT(networkEntry);
  AttribDataStateMachine* sm = networkEntry->getAttribData<AttribDataStateMachine>();
  NMP_ASSERT(sm);
  sm->m_onStateChangedCB = stateMachineStateChangeCB;
}

//----------------------------------------------------------------------------------------------------------------------
Network::StateMachineStateChangeCallback* Network::getStateMachineStateChangeCB(NodeID stateMachineNodeID)
{
  // Get the state machine attribute from the network.
  NodeBinEntry* networkEntry = getAttribDataNodeBinEntry(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, stateMachineNodeID);
  NMP_ASSERT(networkEntry);
  AttribDataStateMachine* sm = networkEntry->getAttribData<AttribDataStateMachine>();
  NMP_ASSERT(sm);
  return sm->m_onStateChangedCB;
}

//----------------------------------------------------------------------------------------------------------------------
// Network as Node functions.
//----------------------------------------------------------------------------------------------------------------------
// Delete node instance function.
void nodeNetworkDeleteInstance(
  const NodeDef* node,
  Network*       net)
{
  // Recurse to our children first.
  NMP_ASSERT(net && net->getNetworkDef());
  net->deleteNodeInstance(net->getNetworkDef()->getRootNodeID());

  // Delete all our own existing attribute data.
  net->cleanNodeData(node->getNodeID());
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeNetworkFindGeneratingNodeForSemantic(
  NodeID             NMP_UNUSED(callingNodeID),
  bool               fromParent,   // Was this query from a parent or child node.
  AttribDataSemantic semantic,
  NodeDef*           NMP_UNUSED(node),
  Network*           net)
{
  NodeID result = INVALID_NODE_ID;

  if (semantic == ATTRIB_SEMANTIC_UPDATE_TIME_POS)
  {
    // Update time pos in the only semantic we output ourselves.
    result = NETWORK_NODE_ID;
  }
  else
  {
    // Pass on to parent or child appropriately.
    if (fromParent)
    {
      NMP_ASSERT(net && net->getNetworkDef());
      NodeID rootNodeID = net->getNetworkDef()->getRootNodeID();
      result = net->nodeFindGeneratingNodeForSemantic(rootNodeID, semantic, fromParent, NETWORK_NODE_ID);
    }
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
namespace
{

//----------------------------------------------------------------------------------------------------------------------
void addDependancyOnPrePhysicsTransforms(
  Network*      net,
  Task*         task,
  uint32_t      index)
{
  NodeID* nodeIDs = 0;
  // Physics nodes
  uint32_t numPhysicsNodes = net->getActivePhysicsNodes(nodeIDs);
  for (uint32_t iNode = 0; iNode < numPhysicsNodes; ++iNode)
  {
    NodeDef* node = net->getNetworkDef()->getNodeDef(nodeIDs[iNode]);
    FrameCount currFrameNo = net->getCurrentFrameNo();
    net->TaskAddParamAndDependency(task, index, ATTRIB_SEMANTIC_PRE_PHYSICS_TRANSFORMS, ATTRIB_TYPE_BOOL, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    ++index;
  }
  // Grouper nodes
  uint32_t numGrouperNodes = net->getActiveGrouperNodes(nodeIDs);
  for (uint32_t iNode = 0; iNode < numGrouperNodes; ++iNode)
  {
    NodeDef* node = net->getNetworkDef()->getNodeDef(nodeIDs[iNode]);
    FrameCount currFrameNo = net->getCurrentFrameNo();
    net->TaskAddParamAndDependency(task, index, ATTRIB_SEMANTIC_PRE_PHYSICS_TRANSFORMS, ATTRIB_TYPE_BOOL, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    ++index;
  }
}

//----------------------------------------------------------------------------------------------------------------------
int32_t addPhysicsDependancyOnChildTransforms(
  Network*      net,
  Task*         task,
  uint32_t      index,
  AnimSetIndex  activeAnimSetIndex,
  bool          justCountParams)
{
  FrameCount currFrameNo = net->getCurrentFrameNo();
  NodeID* nodeIDs = 0;
  int32_t count = 0;

  // true physics nodes
  {
    uint32_t numPhysicsNodes = net->getActivePhysicsNodes(nodeIDs);
    for (uint32_t iNode = 0; iNode < numPhysicsNodes; ++iNode)
    {
      NodeID nodeID = nodeIDs[iNode];
      AttribDataHandle* handle = net->getNetworkDef()->getOptionalAttribDataHandle(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, nodeID);
      // handle is zero for PerformanceBehaviour nodes
      if (handle && handle->m_attribData->getType() == AttribDataPhysicsSetup::getDefaultType())
      {
        AttribDataPhysicsSetup* physicsSetup = (AttribDataPhysicsSetup*) handle->m_attribData;
        if (physicsSetup->m_hasAnimationChild)
        {
          const NodeConnections* nc = net->getActiveNodesConnections(nodeID);
          NMP_ASSERT(nc->m_numActiveChildNodes >= 1);
          if (physicsSetup->m_rootControlMethod != Network::ROOT_CONTROL_PHYSICS ||
              physicsSetup->m_useActiveAnimationAsKeyframeAnchor)
          {
            count += 2;
            if (!justCountParams)
            {
              net->TaskAddParamAndDependency(task, index, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, nc->m_activeChildNodeIDs[0], INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
              ++index;
              net->TaskAddParamAndDependency(task, index, ATTRIB_SEMANTIC_OUTPUT_MASK, ATTRIB_TYPE_BOOL_ARRAY, nodeID, 0, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
              ++index;
            }
          }
        }
      }
    }
  }

  // physically active grouper nodes
  uint32_t numGrouperNodes = net->getActiveGrouperNodes(nodeIDs);
  for (uint32_t iNode = 0; iNode < numGrouperNodes; ++iNode)
  {
    NodeID nodeID = nodeIDs[iNode];
    const NodeConnections* nc = net->getActiveNodesConnections(nodeID);
    for (uint32_t iChild = 0; iChild < nc->m_numActiveChildNodes; ++iChild)
    {
      NodeID childNodeID = nc->m_activeChildNodeIDs[iChild];
      if (!net->isNodeOrActiveChildPhysical(net->getNetworkDef()->getNodeDef(childNodeID)))
      {
        count += 2;
        if (!justCountParams)
        {
          net->TaskAddParamAndDependency(task, index, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, childNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
          ++index;
          net->TaskAddParamAndDependency(task, index, ATTRIB_SEMANTIC_OUTPUT_MASK, ATTRIB_TYPE_BOOL_ARRAY, nodeID, childNodeID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
          ++index;
        }
      }
    }
  }

  return count;
}
 
//----------------------------------------------------------------------------------------------------------------------
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeNetworkQueueUpdateCharacterController(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  NET_LOG_ENTER_FUNC();
  FrameCount currFrameNo = net->getCurrentFrameNo();

  bool addExtraDependencies = net->getRootControlMethod() == Network::ROOT_CONTROL_PHYSICS;
  NodeID* junk;
  uint32_t nPhysics = net->getActivePhysicsNodes(junk);
  uint32_t nGrouper = net->getActiveGrouperNodes(junk);
  uint32_t numParams = 1 + (addExtraDependencies ? nPhysics + nGrouper : 1);
  NMP_ASSERT(currFrameNo > 0);

  Task* task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_NETWORKUPDATECHARACTERCONTROLLER, // ID of the task to add to the queue.
    node->getNodeID(),                                       // Task owning Node ID.
    numParams,                                               // List of all parameters (attributes) that are needed for the task to operate.
    dependentParameter,                                      //
    true);                                                   // external
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_CHARACTER_CONTROLLER_UPDATED, ATTRIB_TYPE_BOOL, INVALID_NODE_ID, currFrameNo);
    if (addExtraDependencies)
    {
      // If physics-controlled root:
      // add a dependency on every active physics node's pre-physics step. Just do this to make sure
      // the pre-physics task gets executed before us - we don't need the result, but when the CC manager runs
      // it will disable the CC for this network, and the pre-physics task needs to have completed before that.
      addDependancyOnPrePhysicsTransforms(net, task, 1);
    }
    else
    {
      // If animation-controlled root:
      // depend on the whole networks trajectory output
      net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, net->getNetworkDef()->getRootNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    }
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeNetworkQueueUpdatePhysics(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NET_LOG_ENTER_FUNC();
  FrameCount currFrameNo = net->getCurrentFrameNo();
  NodeID*    junk;
  bool       addExtraDependencies = net->getRootControlMethod() != Network::ROOT_CONTROL_PHYSICS;
  uint32_t   numParams = 2 + (addExtraDependencies ? (net->getActivePhysicsNodes(junk) + net->getActiveGrouperNodes(junk)) : 0);

  NMP_ASSERT(currFrameNo > 0);

  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_NETWORKUPDATEPHYSICS,
                 node->getNodeID(),
                 numParams,
                 dependentParameter,
                 true);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_PHYSICS_UPDATED, ATTRIB_TYPE_BOOL, INVALID_NODE_ID, currFrameNo);
    net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_CHARACTER_CONTROLLER_UPDATED, ATTRIB_TYPE_BOOL, 0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);

    if (addExtraDependencies)
    {
      // If animation-controlled root:
      // add a dependency on every active physics node's pre-physics step
      addDependancyOnPrePhysicsTransforms(net, task, 2);
    }
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeNetworkQueueUpdateRoot(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NET_LOG_ENTER_FUNC();
  FrameCount currFrameNo = net->getCurrentFrameNo();
  NMP_ASSERT(currFrameNo > 0);
  bool       addExtraDependencies = net->getRootControlMethod() == Network::ROOT_CONTROL_PHYSICS;
  uint32_t   numParams = addExtraDependencies ? 3 : 2;

  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_NETWORKUPDATEROOT,
                 node->getNodeID(),
                 numParams,
                 dependentParameter,
                 true);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_ROOT_UPDATED, ATTRIB_TYPE_BOOL, INVALID_NODE_ID, currFrameNo);
    net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_PHYSICS_UPDATED, ATTRIB_TYPE_BOOL, 0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);

    if (addExtraDependencies)
    {
      // depend on the whole networks trajectory output - this will implicitly make us depend on the
      // physics node's post-physics trajectory task
      net->TaskAddParamAndDependency(task, 2, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, net->getNetworkDef()->getRootNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    }
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeNetworkQueueMergePhysicsRigTransformBuffer(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  NET_LOG_ENTER_FUNC();
  FrameCount currFrameNo = net->getCurrentFrameNo();
  AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());

  NMP_ASSERT(currFrameNo > 0);

  Task* task = 0;
  uint32_t numParams = 2 + addPhysicsDependancyOnChildTransforms(net, task, 2, activeAnimSetIndex, true);

  task = queue->createNewTaskOnQueue(
           CoreTaskIDs::MR_TASKID_NETWORKMERGEPHYSICSRIGTRANSFORMBUFFERS,
           node->getNodeID(),
           numParams,
           dependentParameter,
           false);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    net->TaskAddDefInputParam(task, 0, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);
    net->TaskAddOutputParamZeroLifespan(task, 1, ATTRIB_SEMANTIC_MERGED_PHYSICS_RIG_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
    addPhysicsDependancyOnChildTransforms(net, task, 2, activeAnimSetIndex, false);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::DataBuffer* getNodeTransformsBuffer(NodeID nodeID, const Network* net, FrameCount validFrame, AnimSetIndex animSetIndex)
{
  //---------------------
  // Both semantics need to be searched because we don't know if trajectory and transforms were combined in the frame
  // that is being queried.

  NMP_ASSERT(animSetIndex != ANIMATION_SET_ANY);

  NodeBinEntry* transformsAttrEntry = net->getAttribDataNodeBinEntry(
    MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER,
    nodeID,
    MR::INVALID_NODE_ID,
    validFrame);
  if (transformsAttrEntry)
  {
    NMP_ASSERT(transformsAttrEntry->m_address.m_animSetIndex != ANIMATION_SET_ANY);

    AttribDataTransformBuffer* transformsAttr = (AttribDataTransformBuffer*)transformsAttrEntry->getAttribData();
    if (animSetIndex != transformsAttrEntry->m_address.m_animSetIndex)
    {
      // The animation set has changed. Create a new one and map the old one to it.
      AnimRigDef* targetRigDef = net->getRig(animSetIndex);

      NMP::Memory::Format internalBuffMemReqs;
      NMP::Memory::Format buffMemReqs;
      uint32_t numRigJoints = targetRigDef->getNumBones();
      AttribDataTransformBuffer::getPosQuatMemoryRequirements(numRigJoints, buffMemReqs, internalBuffMemReqs);
      AttribDataTransformBuffer* targetTransformsAttr = AttribDataTransformBuffer::createPosQuat(transformsAttrEntry->m_allocatorUsed, buffMemReqs, internalBuffMemReqs, numRigJoints, 1);
      net->getNetworkDef()->mapCopyTransformBuffers(transformsAttr->m_transformBuffer, transformsAttrEntry->m_address.m_animSetIndex, targetTransformsAttr->m_transformBuffer, animSetIndex);

      transformsAttrEntry->removeReference();
      transformsAttrEntry->m_address.m_animSetIndex = animSetIndex;
      transformsAttrEntry->m_attribDataHandle.m_attribData = targetTransformsAttr;
      transformsAttrEntry->m_attribDataHandle.m_format = buffMemReqs;

      return targetTransformsAttr->m_transformBuffer;
    }
    
    return transformsAttr->m_transformBuffer;
  }

  transformsAttrEntry = net->getAttribDataNodeBinEntry(
    MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
    nodeID,
    MR::INVALID_NODE_ID,
    validFrame);
  if (transformsAttrEntry)
  {
    NMP_ASSERT(transformsAttrEntry->m_address.m_animSetIndex != ANIMATION_SET_ANY);

    AttribDataTransformBuffer* transformsAttr = (AttribDataTransformBuffer*)transformsAttrEntry->getAttribData();
    if (animSetIndex != transformsAttrEntry->m_address.m_animSetIndex)
    {
      // The animation set has changed. Create a new one and map the old one to it.
      AnimRigDef* targetRigDef = net->getRig(animSetIndex);

      NMP::Memory::Format internalBuffMemReqs;
      NMP::Memory::Format buffMemReqs;
      uint32_t numRigJoints = targetRigDef->getNumBones();
      AttribDataTransformBuffer::getPosQuatMemoryRequirements(numRigJoints, buffMemReqs, internalBuffMemReqs);
      AttribDataTransformBuffer* targetTransformsAttr = AttribDataTransformBuffer::createPosQuat(transformsAttrEntry->m_allocatorUsed, buffMemReqs, internalBuffMemReqs, numRigJoints, 1);
      net->getNetworkDef()->mapCopyTransformBuffers(transformsAttr->m_transformBuffer, transformsAttrEntry->m_address.m_animSetIndex, targetTransformsAttr->m_transformBuffer, animSetIndex);

      transformsAttrEntry->removeReference();
      transformsAttrEntry->m_address.m_animSetIndex = animSetIndex;
      transformsAttrEntry->m_attribDataHandle.m_attribData = targetTransformsAttr;
      transformsAttrEntry->m_attribDataHandle.m_format = buffMemReqs;

      return targetTransformsAttr->m_transformBuffer;
    }

    return transformsAttr->m_transformBuffer;
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
NodeID getParentStateMachineOrNetworkRootNodeID(NodeDef* node, Network* net, bool* foundStateMachine)
{
  NetworkDef* netDef = net->getNetworkDef();
  NodeID networkRootID = netDef->getRootNodeID();
  NodeDef* n = node;
  if (foundStateMachine)
    *foundStateMachine = true;
  while (netDef != (n = net->getNetworkDef()->getNodeDef(net->getActiveParentNodeID(n->getNodeID()))))
  {
    if (n->getNodeFlags() & NodeDef::NODE_FLAG_IS_STATE_MACHINE)
      return n->getNodeID();
  }
  if (foundStateMachine)
    *foundStateMachine = false;
  return networkRootID;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
