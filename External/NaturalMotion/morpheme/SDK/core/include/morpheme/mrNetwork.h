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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef MR_NETWORK_H
#define MR_NETWORK_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMatrix34.h"
#include "NMPlatform/NMFastHeapAllocator.h"
#include "morpheme/mrRig.h"
#include "morpheme/mrNetworkDef.h"
#include "morpheme/mrNodeBin.h"
#include "morpheme/mrDefines.h"
#include "morpheme/mrDispatcher.h"
#include "morpheme/mrCoreTaskIDs.h"
#include "morpheme/mrMessage.h"
#include "morpheme/mrCharacterControllerAttribData.h"
#include "morpheme/mrInstanceDebugInterface.h"
#include "morpheme/mrManager.h"
#ifdef MR_OUTPUT_DEBUGGING
  #include "NMPlatform/NMTimer.h"
#endif // MR_OUTPUT_DEBUGGING

//----------------------------------------------------------------------------------------------------------------------
// Turn on circular dependency tracing for debug builds here
#ifdef NM_DEBUG
#define MR_CIRCDEPS_DEBUGGINGx

#include "NMPlatform/NMVectorContainer.h"
#define TASK_PARAM_LIST_DEPTH 256
#endif // NM_DEBUG
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \defgroup NetworkModule Network Classes, Functions and Constants
/// \ingroup CoreModule
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

#define NODE_TASKS_QUEUE_BY_SEMANTICSzz

class CharacterControllerInterface;
class AttribDataCharacterProperties;

//----------------------------------------------------------------------------------------------------------------------
/// \struct MR::ConditionAttribEntry
/// \brief Entry for the condition attributes list
/// \ingroup NetworkModule
//----------------------------------------------------------------------------------------------------------------------
struct ConditionAttribEntry
{
  ConditionAttribEntry* m_next;
  AttribAddress         m_conditionAttrib;
  AttribDataType        m_attribType;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::NodeConnections
/// \brief Stores the current child and parent connections of and active node, as well as some other status information.
/// \ingroup NetworkModule
//----------------------------------------------------------------------------------------------------------------------
class NodeConnections
{
public:
  //---------------------------------------------------------------------
  /// \name   NodeConnections Initialisation
  /// \brief  Functions to determine the memory requirements for the NodeConnections
  ///         and initialise the memory.
  //---------------------------------------------------------------------
  //@{

  /// \brief Full memory requirements of a NodeConnections
  static NMP::Memory::Format getMemoryRequirements(
    uint16_t maxNumActiveChildNodes);

  /// \brief Memory requirements of a NodeConnections, excluding the memory requirements
  /// for the base NodeConnections class itself.
  static NMP::Memory::Format getMemoryRequirementsExcludeBaseMem(
    uint16_t maxNumActiveChildNodes);

  /// \brief Full initialisation of a NodeConnections.
  static NodeConnections* init(
    NMP::Memory::Resource& resource,
    uint16_t maxNumActiveChildNodes);

  /// \brief Initialisation of a NodeConnections. But assumes that incrementing the base
  /// NodeConnections class memory requirements has been done externally.
  static void initExcludeBaseMem(
    NodeConnections* target,
    NMP::Memory::Resource& resource,
    uint16_t maxNumActiveChildNodes);
  //@}

  //---------------------------------------------------------------------
  /// \name   NodeConnections management
  /// \brief  Functions to get and set information about the NodeConnections
  //---------------------------------------------------------------------
  //@{
  bool setFrom(const NodeConnections* source);

  void cleanActiveConnections(const NodeDef* nodeDef);

  bool hasActiveChildNodeID(NodeID nodeID) const;
  //@}

public:
  uint8_t                   m_justBecameActive:1;     ///< Set as true if the node was not active last frame, but is active this frame.
  uint8_t                   m_active:1;               ///<
  mutable bool              m_activeGatherTag;        ///< To avoid counting a node more than once while counting up the active nodes.
  NodeID                    m_activeParentNodeID;     ///< The active up stream dependent node.
                                                      ///<  Other nodes may reference us as a child (through the pass down system).
                                                      ///<  But we can only have upstream dependency on this nodes data.
  NodeID*                   m_activeChildNodeIDs;     ///< The active down stream dependent nodes.
                                                      ///<  We can have dependency on data from any or all of these nodes.
  uint16_t                  m_maxNumActiveChildNodes;
  uint16_t                  m_numActiveChildNodes;
};

#ifdef NM_HOST_64_BIT
NM_ASSERT_COMPILE_TIME(sizeof(NodeConnections) == 24);
#else
NM_ASSERT_COMPILE_TIME(sizeof(NodeConnections) == 12);
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \struct MR::BlendOptNodeConnections
/// \brief Stores the current child and parent connections of an active node. The
/// structure additionally stores child node connections required for blend optimisation
/// of the transforms and events data.
/// \ingroup NetworkModule
//----------------------------------------------------------------------------------------------------------------------
class BlendOptNodeConnections : public NodeConnections
{
public:
  //---------------------------------------------------------------------
  /// \name   BlendOptNodeConnections Initialisation
  /// \brief  Functions to determine the memory requirements for the NodeConnections
  ///         and initialise the memory.
  //---------------------------------------------------------------------
  //@{

  /// \brief Full memory requirements of a BlendNodeConnections
  static NMP::Memory::Format getMemoryRequirements(
    uint16_t maxNumActiveChildNodes);

  /// \brief Full initialisation of a BlendOptNodeConnections.
  static BlendOptNodeConnections* init(
    NMP::Memory::Resource& resource,
    uint16_t maxNumActiveChildNodes);
  //@}

  //---------------------------------------------------------------------
  /// \name   BlendOptNodeConnections management
  /// \brief  Functions to get and set information about the BlendNodeConnections
  //---------------------------------------------------------------------
  //@{
  NM_INLINE void setTrajectoryAndTransformsNodeIDs(
    uint16_t numNodeIDs,
    const NodeID* nodeIDs);
  
  NM_INLINE void setSampledEventsNodeIDs(
    uint16_t numNodeIDs,
    const NodeID* nodeIDs);
  //@}

public:
  uint16_t      m_trajectoryAndTransformsNumNodeIDs;
  uint16_t      m_sampledEventsNumNodeIDs;

  NodeID*       m_trajectoryAndTransformsNodeIDs;
  NodeID*       m_sampledEventsNodeIDs;
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BlendOptNodeConnections::setTrajectoryAndTransformsNodeIDs(
  uint16_t numNodeIDs,
  const NodeID* nodeIDs)
{
  NMP_ASSERT(numNodeIDs <= m_maxNumActiveChildNodes);
  m_trajectoryAndTransformsNumNodeIDs = numNodeIDs;
  for (uint16_t i = 0; i < numNodeIDs; ++i)
  {
    m_trajectoryAndTransformsNodeIDs[i] = nodeIDs[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BlendOptNodeConnections::setSampledEventsNodeIDs(
  uint16_t numNodeIDs,
  const NodeID* nodeIDs)
{
  NMP_ASSERT(numNodeIDs <= m_maxNumActiveChildNodes);
  m_sampledEventsNumNodeIDs = numNodeIDs;
  for (uint16_t i = 0; i < numNodeIDs; ++i)
  {
    m_sampledEventsNodeIDs[i] = nodeIDs[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
// OutputCPPin functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void OutputCPPin::deleteAttribData()
{
  if (m_attribDataHandle.m_attribData)
  {
    m_attribDataHandle.m_attribData->m_allocator->memFree(m_attribDataHandle.m_attribData);
  }
  m_attribDataHandle.set(NULL, NMP::Memory::Format());
  m_lastUpdateFrame = 0;
}

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::Network
/// \brief A MR::Network is an instance of a MR::NetworkDef.
/// \ingroup NetworkModule
///
/// A Network will generally correspond to 1 game character instance.
/// It contains all functionality for updating time, changing active state and evaluating all animation data
/// required by the character, including transforms.
/// Though its lifetime a Network instance references data within its NetworkDef, therefore an instance cannot exist
/// without its Def.
//----------------------------------------------------------------------------------------------------------------------
class Network
{
public:
  friend class NetworkRestorePoint;

  static Network* createAndInit(
    NetworkDef*                       netDef,
    Dispatcher*                       dispatcher,
    NMP::TempMemoryAllocator*         tempAllocator,
    NMP::MemoryAllocator*             persistentAllocator,
    MR::CharacterControllerInterface* characterController = 0);

  /// \brief Frees up all resources used by this Network
  void release();

  /// \brief calls release on this network and also frees our own memory.  For use with createAndInit.
  void releaseAndDestroy();

  /// \brief Calculate the memory required for an instance of this network def.
  static NMP::Memory::Format getMemoryRequirements(NetworkDef* netDef);

  /// \brief Initialise an instance of the network from the definition provided
  static Network* init(
    NMP::Memory::Resource&            resource,
    NetworkDef*                       netDef,
    Dispatcher*                       dispatcher,
    NMP::TempMemoryAllocator*         tempDataAllocator,
    NMP::MemoryAllocator*             persistentMemoryAllocator,
    MR::CharacterControllerInterface* characterController);

  bool updateNetworkTime(
    float updateTime,     ///< Update time value.
    bool  isAbsTime       ///< Are we setting time absolutely or updating time with a delta.
  );

  void setTaskQueue(TaskQueue* taskQueue) { m_taskQueue = taskQueue; }
  TaskQueue* getTaskQueue() { return m_taskQueue; }
  Dispatcher* getDispatcher() { return m_dispatcher; }

  /// \brief Put tasks on the queue for the requested attribute.
  bool queueTasksFor(
    TaskParameter* dependentParam   ///< The param of the above that says what attribute we're after
  );

  /// \brief Perform immediate update of the attribute data with this address.
  AttribData* updateOutputCPAttribute(
    NodeID   owningNodeID,
    PinIndex outputCPPinIndex,
    AnimSetIndex animSet);

  /// \brief prepare the network for a new update - also resets the attribute counter
  NM_INLINE void incFrameCounter();

  /// \brief returns the time step from the last update.  Note that this is only valid during network execution.
  NM_INLINE float getLastUpdateTimeStep();

  /// \brief Start a Network update. The task queue must be empty at this point. It updates the network
  /// connections and builds the task queue.
  void startUpdate(
    float updateTime,                 ///< Update time value.
    bool  absTime = false,            ///< Are we setting time or updating time with a delta.
    bool  queueTransforms = true,     ///< Do we want to create a transforms dependency from the root.
                                      ///<  Set this to false to update off-screen characters faster.
    bool  queueTrajectory = true,     ///< Do we want to create a trajectory dependency from the root.
                                      ///<  Set this to false if you characters movement relative to the world will not be
                                      ///<  controlled by the network.
    bool  queueSampledEvents = true,  ///< Do we want to create a sample events dependency from the root.
    bool  queueSyncEventTrack = false ///< Do we want to create a sync event track dependency from the root. NOTE: be sure that your root node always generates, or passes through, SyncEventTracks.
  );
  
  /// Continue the network update. Returns 0 when there are no more ready tasks - so either the network
  /// update is complete, or there is an error and task dependencies have not been fulfilled. If an external
  /// task is encountered it is returned. If the external task is owned by morpheme it will have been run,
  /// so the called doesn't need to call it, but may act on it (e.g. update physics or the character controller
  /// manager). If it's not owned by morpheme the caller can choose what to do with it (but should update the
  /// dependent tasks before continuing). The caller can look at the task ID to make this decision.
  ExecuteResult update(Task *&task);

  /// \brief End a Network update. Updating emitted control parameter nodes.
  void endUpdate();

  /// \brief Find the attrib data NodeBinEntry with this address.
  ///  It is legal that the NodeBinEntry we are looking for does not exist.
  NM_INLINE NodeBinEntry* getAttribDataNodeBinEntry(
    const AttribAddress& attributeAddress) const;
  NM_INLINE NodeBinEntry* getAttribDataNodeBinEntry(
    AttribDataSemantic semantic,
    NodeID             owningNodeID,
    NodeID             targetNodeID = INVALID_NODE_ID,
    FrameCount         validFrame = VALID_FOREVER,
    AnimSetIndex       animSetIndex = ANIMATION_SET_ANY) const;

  /// \brief Find the attrib data with this address. AttribData must exist.
  NM_INLINE AttribData* getAttribData(
    const AttribAddress& attributeAddress);
  NM_INLINE AttribData* getAttribData(
    AttribDataSemantic semantic,
    NodeID             owningNodeID,
    NodeID             targetNodeID = INVALID_NODE_ID,
    FrameCount         validFrame = VALID_FOREVER,
    AnimSetIndex       animSetIndex = ANIMATION_SET_ANY);
  

  /// \brief Find the attrib data with this address. AttribData must exist.
  ///  More secure templated versions that validate type and does a cast to the template type internally.
  template <class T>
  NM_INLINE T* getAttribData(
    const AttribAddress& attributeAddress);
  template <class T>
  NM_INLINE T* getAttribData(
    AttribDataSemantic semantic,
    NodeID             owningNodeID,
    NodeID             targetNodeID = INVALID_NODE_ID,
    FrameCount         validFrame = VALID_FOREVER,
    AnimSetIndex       animSetIndex = ANIMATION_SET_ANY);

  /// \brief Find the attrib data with this address.
  ///  It is legal that the AttribData we are looking for does not exist.
  NM_INLINE AttribData* getOptionalAttribData(
    const AttribAddress& attributeAddress);
  NM_INLINE AttribData* getOptionalAttribData(
    AttribDataSemantic semantic,
    NodeID             owningNodeID,
    NodeID             targetNodeID = INVALID_NODE_ID,
    FrameCount         validFrame = VALID_FOREVER,
    AnimSetIndex       animSetIndex = ANIMATION_SET_ANY);

  /// \brief Find the attrib data with this address.
  ///  It is legal that the AttribData we are looking for does not exist.
  ///  More secure templated versions that validate type and does a cast to the template type internally.
  template <class T>
  NM_INLINE T* getOptionalAttribData(
    const AttribAddress& attributeAddress);
  template <class T>
  NM_INLINE T* getOptionalAttribData(
    AttribDataSemantic semantic,
    NodeID             owningNodeID,
    NodeID             targetNodeID = INVALID_NODE_ID,
    FrameCount         validFrame = VALID_FOREVER,
    AnimSetIndex       animSetIndex = ANIMATION_SET_ANY);


  /// \brief If this node does not have this data this function recurses over any attached child filter nodes until
  ///  the it is found or until we reach a leaf node.
  ///  It is legal that the AttribData we are looking for does not exist.
  NM_INLINE NodeBinEntry* getAttribDataNodeBinEntryRecurseFilterNodes(
    const AttribAddress& attributeAddress,
    bool                 fromParent = true);
  NM_INLINE NodeBinEntry* getAttribDataNodeBinEntryRecurseFilterNodes(
    AttribDataSemantic semantic,
    NodeID             owningNodeID,
    NodeID             targetNodeID = INVALID_NODE_ID,
    FrameCount         validFrame = VALID_FOREVER,
    AnimSetIndex       animSetIndex = ANIMATION_SET_ANY,
    bool               fromParent = true);
  NM_INLINE AttribData* getAttribDataRecurseFilterNodes(
    const AttribAddress& attributeAddress,
    bool                 fromParent = true);
  NM_INLINE AttribData* getAttribDataRecurseFilterNodes(
    AttribDataSemantic semantic,
    NodeID             owningNodeID,
    NodeID             targetNodeID = INVALID_NODE_ID,
    FrameCount         validFrame = VALID_FOREVER,
    AnimSetIndex       animSetIndex = ANIMATION_SET_ANY,
    bool               fromParent = true);

  NM_INLINE bool addAttribData(
    const AttribAddress& attribAddress,
    AttribDataHandle&    attribDataHandle,
    uint16_t             lifespan);

  FrameCount getCurrentFrameNo() const { return m_currentFrameNo; }
  void setCurrentFrameNo(FrameCount currentFrameNo) { m_currentFrameNo = currentFrameNo; }

  NetworkDef* getNetworkDef() const { return m_netDef; }

  /// \brief Get the maximum number of channels (joints) defined in the network for all rigs in the anim library
  /// \return 0 if the network is not initialised properly
  uint32_t getMaxTransformCount() const;

  /// \brief Get the currently used Rig.
  /// \return The Rig.
  AnimRigDef* getActiveRig();

  /// \brief Get an animation set specific Rig.
  /// \return The Rig.
  AnimRigDef* getRig(AnimSetIndex animationSetIndex) const;

  /// \brief Get the currently used CharacterControllerDef.
  /// \return The CharacterControllerDef.
  CharacterControllerDef* getActiveCharacterControllerDef();

  /// \brief Get the transforms for all channels (joints) defined in the network and
  ///        fills in any missing channels with transforms from the bind pose.
  /// \return NULL if the network is not initialised properly
  ///
  /// Note: Be careful not to swap the animation set and then call this function it can result in the
  /// transforms from the wrong Rig Bind Pose being copied in to the output buffer.
  NMP::DataBuffer* getTransforms();

  /// \brief Get the number of channels (joints) defined in the network for the currently active rig
  /// \return NULL if the network is not initialised properly
  uint32_t getCurrentTransformCount();

  /// \brief Set the value of an output control parameter pin on a node.
  ///
  /// Interface control parameter nodes, as seen in connect, all currently have only 1 output pin
  /// so the index in this case should be CONTROL_PARAMETER_NODE_PIN_0.
  template <typename _AttribDataClass>
  NM_INLINE void setControlParameter(
    NodeID            owningNodeID,                                 ///< The CP node that this attribute belongs to.
    _AttribDataClass* attributeData,                                ///< The attribute data to set.
    PinIndex          outputPinIndex = CONTROL_PARAMETER_NODE_PIN_0 ///< The output pin we are setting.
  );

  /// \brief Get the value of the control parameter with the specified node ID. Includes type validation.
  /// \return Pointer to resulting attrib data.
  ///
  /// Interface control parameter nodes, as seen in connect, all currently have only 1 output pin
  /// so the index in this case should be CONTROL_PARAMETER_NODE_PIN_0.
  template <typename _AttribDataClass>
  NM_INLINE _AttribDataClass* getControlParameter(
    NodeID            owningNodeID,                                 ///< The CP node that this attribute belongs to.
    PinIndex          outputPinIndex = CONTROL_PARAMETER_NODE_PIN_0 ///< The output pin we are getting.
  );

  /// \brief Get the value of the control parameter with the specified node ID. Does not includes any type validation.
  /// \return Pointer to resulting attrib data.
  ///
  /// Interface control parameter nodes, as seen in connect, all currently have only 1 output pin
  /// so the index in this case should be CONTROL_PARAMETER_NODE_PIN_0.
  NM_INLINE AttribData* getControlParameter(
    NodeID            owningNodeID,                                 ///< The CP node that this attribute belongs to.
    PinIndex          outputPinIndex = CONTROL_PARAMETER_NODE_PIN_0 ///< The output pin we are getting.
  );

  /// \brief Call the update function of a connected input control parameter and get the result.
  ///
  /// Utility primarily for use within operator node control parameter update functions.
  NM_INLINE AttribData* updateInputCPConnection(const CPConnection* cpConnection, AnimSetIndex animSet);

  /// \brief Call the update function of an optionally connected input control parameter and get the result.
  ///
  /// Utility primarily for use within operator node control parameter update functions.
  NM_INLINE AttribData* updateOptionalInputCPConnection(const CPConnection* cpConnection, AnimSetIndex animSet);

  /// \brief Call the update function of a connected input control parameter and get the result cast to the template type.
  ///
  /// Utility primarily for use within operator node control parameter update functions.
  template <typename T> 
  NM_INLINE T* updateInputCPConnection(const CPConnection* cpConnection, AnimSetIndex animSet);

  /// \brief Call the update function of an optionally connected input control parameter and get the result cast to the template type.
  ///
  /// Utility primarily for use within operator node control parameter update functions.
  template <typename T> 
  NM_INLINE T* updateOptionalInputCPConnection(const CPConnection* cpConnection, AnimSetIndex animSet);

  /// \brief Get the number of active nodes this frame.
  uint32_t getActiveNodeCount() const;

  /// \brief Fill an array with the Node IDs of all of the active Nodes this frame.
  uint32_t getActiveNodes(NodeID* nodeIDs, uint32_t maxNodeIDs) const;

  /// \brief Get the number of external triggered node event messages this frame.
  uint32_t getTriggeredNodeEventMessageCount() const;

  /// \brief Fill an array with the MessageIDs of all of the external triggered node event messages this frame.
  uint32_t getTriggeredNodeEventMessages(MessageID* messageIDs, uint32_t maxMessageIDs) const;

  /// \brief Returns whether a emitted message was sent this frame.
  bool isTriggeredNodeEventMessageEmitted(const char * messageName) const;
  /// \brief Returns whether a emitted message was sent this frame.
  bool isTriggeredNodeEventMessageEmitted(MessageID messageID) const;

  /// \brief Clear the list of external triggered node event messages.
  void clearExternalTriggeredNodeEventMessages();

  /// \brief Find if a node is currently active or not this frame.
  NM_INLINE bool nodeIsActive(NodeID node) const;

  /// \brief Find if an operator or control parameter node is currently active or not this frame.
  NM_INLINE bool operatorCPNodeIsActive(NodeID nodeID) const;

  /// \brief Get the active parent node ID of the specified node.
  NM_INLINE NodeID getActiveParentNodeID(NodeID nodeID) const;

  /// \brief Set the active parent node ID of the specified node.
  NM_INLINE void setActiveParentNodeID(NodeID nodeID, NodeID parentNodeID);

  /// \brief How many active children are there on this node.
  NM_INLINE uint32_t getNumActiveChildren(NodeID nodeID) const;

  /// \brief Set the number of active children on this node.
  NM_INLINE void setNumActiveChildren(NodeID nodeID, uint32_t numActiveChildNodes);

  /// \brief Get the NodeID of an active child. Asserts if child index is greater than current child count.
  NM_INLINE NodeID getActiveChildNodeID(NodeID nodeID, uint32_t childIndex) const;

  /// \brief Set the NodeID of an active child slot. Asserts if child index is greater than current child count.
  NM_INLINE void setActiveChildNodeID(NodeID nodeID, uint32_t childIndex, NodeID newChildNodeID);

  /// \brief
  NM_INLINE NodeConnections* getActiveNodesConnections(NodeID nodeID);
  NM_INLINE const NodeConnections* getActiveNodesConnections(NodeID nodeID) const;

  /// \brief
  void updateActiveNodeConnections(
    NodeID nodeID,
    uint16_t numActiveChildNodes,
    const NodeID* activeChildNodeIDs);

  /// \brief Returns true if searchNodeID is a parent/grandparent etc of startNodeID BUT it won't traverse
  // pass-down hierarchies.
  NM_INLINE bool isAnActiveParentOrGrandParent(NodeID startNodeID, NodeID searchNodeID);

  /// \brief Searches all the active nodes to see if any contain a NodeTag that matches the nodeTag string parameter
  ///  if true is returned else false. Remember this is not quick as a buffer is grabbed to copy the active node pointers.
  ///  Unsurprisingly also uses a string compare.
  bool isNodeTagActive(const char * nodeTag) const;

  //------------- Animation Set Management -------------
  /// \brief Set the Animation Set to be used by this network from the list of available sets in the network definition.
  /// \return true on success, false on failure.
  bool setActiveAnimSetIndex(AnimSetIndex animationSetIndex);

  /// \brief Get the index of the currently used network output AnimationSet
  /// \return The AnimationSet index
  AnimSetIndex getActiveAnimSetIndex() const {
    NMP_ASSERT(m_activeAnimSetIndex);
    return (AnimSetIndex)m_activeAnimSetIndex->m_value;
  }

  /// \brief Get the index of a nodes output AnimationSet
  /// \return The AnimationSet index for the node
  AnimSetIndex getOutputAnimSetIndex(NodeID node) const { return getNodeBin(node)->getOutputAnimSetIndex(); }

  /// \brief Set the index of a nodes output AnimationSet
  void setOutputAnimSetIndex(NodeID node, AnimSetIndex animationSetIndex) { getNodeBin(node)->setOutputAnimSetIndex(animationSetIndex); }

  /// \brief Typedef for an animation set change callback
  ///
  /// This function definition describes the prototype for a an animation set change CB.  When the network changes
  ///  animation set, it will call the registered callback if one has been set.
  typedef void (AnimSetChangedCB)(Network* net, AnimSetIndex animationSetIndex);

  static void setActiveAnimSetChangedCallback(AnimSetChangedCB* cb) { sm_animSetChangedCB = cb; };
  static AnimSetChangedCB* getActiveAnimSetChangedCallback() { return sm_animSetChangedCB; };

  //------------- State Management -------------
  /// \brief Send the message with the given ID to the target node
  /// \return true if the message was handled.
  ///
  /// Sets messageIDs (if they exist) on the active state of this state machines.
  NM_INLINE bool sendMessage(
    NodeID targetNode,      ///< Id of node to receive the message
    const Message& message  ///< Message data send to the node.
    );

  /// \brief Clear all currently set messageIDs on the specified state machine.
  ///
  /// Only does clear if state machine is active, inactive ones should not have messageID conditions set anyway.
  void clearMessageStatusesOnStateMachine(
    NodeID    stateMachineNodeID  ///< State machine to amend the status of messageIDs within.
  );

  /// \brief Send the message with the given ID to all nodes that have declared an interest.  
  ///        This is the method always used by morpheme connect.
  ///
  /// \return the number of nodes that handled the message
  uint32_t broadcastMessage(
    const Message& message  ///< Message sent to interested receivers
  ); 
  uint32_t broadcastMessage(
    const Message& message,                 ///< Message sent to interested receivers
    const MessageDistributor* distributor   ///< We already know the distributor we need
    );                                      ///< no need to find this again.

  /// \brief  This method finds the correct preset message from amongst all the message distributors
  ///         (hence the message distributor also) retrieves this a pipes this through the method above.
  uint32_t broadcastPresetMessage(
    const MessageType messageType, 
    const char * messageName);

  /// \brief  Calls the method above passing in 'MESSAGE_TYPE_SCALE_CHARACTER' as the first parameter.
  uint32_t broadcastScaleCharacterMessage(const char * messageName);

  /// \brief Clear all currently set messageIDs on all active state machines.
  ///
  /// Only does clear for state machines as inactive ones should not have message conditions set anyway.
  void clearMessageStatusesOnAllStateMachines();

  // \brief Will return nothing if state machine is inactive.
  uint32_t getActiveMessageIDsFromActiveStateMachine(
    NodeID     stateMachineNodeID, ///< State machine to query.
    MessageID* messageIDs,         ///< Out.
    uint32_t   maxNumRequestIDs    ///< Size of messages array.
  );

  /// \brief Set the state of a specified state machine using the NodeID of one of its member states.
  void queueSetStateMachineStateByNodeID(
    NodeID    stateMachineNodeID,
    NodeID    targetStateNodeID
  );

  /// \brief Populate an array with all of the messages that can currently be responded to from all the active state machines.
  ///
  /// Some of the messageIDs IDs may be duplicated in the final result, if they are conditions on more than one transition from the active states.
  uint32_t getActiveMessageIDsFromAllActiveStateMachines(
    MessageID* messageIDs,         ///< Out.
    uint32_t   maxNumRequestIDs    ///< Size of messages array.
  );

  /// \brief return the node ID of the current state node of the given state machine.
  NodeID getStateMachineCurrentStateNodeId(NodeID stateMachineID);

  /// \brief Get the destination node ID of a transition node.
  NodeID getDestinationNodeIDFromATransition(NodeID transitionNodeID) const;

  //------------- Control Parameter Management -------------
  /// \brief Counts control parameter nodes that were updated in the last update connections faze.
  uint32_t getNumActiveControlParameterNodes();

  /// \brief Lists those control parameter nodes that were updated in the last update connections faze.
  uint32_t getActiveControlParameterNodeIDs(
    NodeID*   nodeIDs,      ///< Out.
    uint32_t  maxNumNodeIDs ///< Size of node IDs array.
  );

  /// \brief
  /// \return The ID of the Node that has a queuing function for the specified semantic or INVALID_NODE_ID if the semantic has
  ///         no queuing function.
  NM_INLINE NodeID nodeFindGeneratingNodeForSemantic(
    NodeID             nodeID,
    AttribDataSemantic semantic,
    bool               fromParent,
    NodeID             callingNodeID);

  /// \brief Looks in the Network then the NetworkDef for a piece of attribute data that may or may not already exist.
  NM_INLINE void TaskAddInputParam(
    Task*              task,
    uint32_t           index,
    AttribDataSemantic semantic,
    NodeID             owningNodeID,
    NodeID             targetNodeID,
    MR::TaskParamFlags taskParamFlags,
    FrameCount         validFrame,
    AnimSetIndex       animSetIndex = ANIMATION_SET_ANY);

  /// \brief Looks in the Network for a piece of attribute data that must already exist.
  NM_INLINE void TaskAddNetInputParam(
    Task*               task,
    uint32_t            index,
    AttribDataSemantic  semantic,
    NodeID              owningNodeID,
    NodeID              targetNodeID,
    FrameCount          validFrame,
    AnimSetIndex        animSetIndex = ANIMATION_SET_ANY);

  /// \brief Look in the Network for a piece of attribute data that must already exist.  The parameter is used as an 
  ///        input and output when the task executes.
  NM_INLINE void TaskAddNetInputOutputParam(
    Task*               task,
    uint32_t            index,
    AttribDataSemantic  semantic,
    AttribDataType      attribType,
    NodeID              targetNodeID,
    FrameCount          validFrame,
    AnimSetIndex        animSetIndex = ANIMATION_SET_ANY);

  /// \brief Look in the Network for an optional piece of attribute data. The parameter is used
  ///        as an input and output when the task executes.
  NM_INLINE void TaskAddOptionalNetInputOutputParam(
    Task*               task,
    uint32_t            index,
    AttribDataSemantic  semantic,
    AttribDataType      attribType,
    NodeID              targetNodeID,
    FrameCount          validFrame,
    AnimSetIndex        animSetIndex = ANIMATION_SET_ANY);

  /// \brief Looks in the Network for a piece of attribute data that may or may not exist.
  NM_INLINE void TaskAddOptionalNetInputParam(
    Task*               task,
    uint32_t            index,
    AttribDataSemantic  semantic,
    NodeID              owningNodeID,
    NodeID              targetNodeID,
    FrameCount          validFrame,
    AnimSetIndex        animSetIndex = ANIMATION_SET_ANY);

  /// \brief Looks in the Network for a piece of attribute data that may or may not exist.  This function
  ///  will accept an owning node ID of INVALID_NODE_ID.
  NM_INLINE void TaskAddInputParamWithOptionallyInvalidOwningNode(
    Task*               task,
    uint32_t            index,
    AttribDataSemantic  semantic,
    NodeID              owningNodeID,
    NodeID              targetNodeID,
    FrameCount          validFrame,
    AnimSetIndex        animSetIndex = ANIMATION_SET_ANY);


  /// \brief Look on the owning node; if not there and the owner is a filter recurse to a connection (parent or child)
  ///        and continue recursing any subsequent filter nodes.
  NM_INLINE void TaskAddOptionalNetInputParamRecurseFilterNodes(
    Task*               task,
    uint32_t            index,
    AttribDataSemantic  semantic,
    NodeID              owningNodeID,
    NodeID              targetNodeID,
    FrameCount          validFrame,
    AnimSetIndex        animSetIndex = ANIMATION_SET_ANY,
    bool                fromParent = true);

  /// \brief Look on the owning node; If not found and our parent passes through semantics of this type
  ///        look for it there, and recurse.
  NM_INLINE void TaskAddOptionalNetInputParamRecurseToParent(
    Task*               task,
    uint32_t            index,
    AttribDataSemantic  semantic,
    NodeID              owningNodeID,
    NodeID              targetNodeID,
    FrameCount          validFrame,
    AnimSetIndex        animSetIndex = ANIMATION_SET_ANY);


  /// \brief Look for a piece of AttribData on the specified node. If not found
  ///        and our parent passes through semantics of this type look for it there, and recurse.
  NodeBinEntry* getAttribDataNodeBinEntryRecurseToParent(
    AttribDataSemantic semantic,
    NodeID             owningNodeID,
    NodeID             targetNodeID,
    FrameCount         validFrame,
    AnimSetIndex       animSetIndex);

  /// \brief Looks in the NetworkDef for a piece of attribute data that must exist.
  NM_INLINE void TaskAddDefInputParam(
    Task*               task,
    uint32_t            index,
    AttribDataSemantic  semantic,
    NodeID              owningNodeID,
    AnimSetIndex        animSetIndex = 0);  // We never use ANIMATION_SET_ANY with def data, if its AnimSet independent it goes in the AnimSet 0 data.

  /// \brief Looks in the Network for a piece of attribute data that may or may not exist.
  NM_INLINE void TaskAddOptionalDefInputParam(
    Task*               task,
    uint32_t            index,
    AttribDataSemantic  semantic,
    NodeID              owningNodeID,
    AnimSetIndex        animSetIndex = 0);  // We never use ANIMATION_SET_ANY with def data, if its AnimSet independent it goes in the AnimSet 0 data.

  NM_INLINE void TaskAddInputParamOptionallyInvalidOwningNode(
    Task*               task,
    uint32_t            index,
    AttribDataSemantic  semantic,
    NodeID              owningNodeID,
    NodeID              targetNodeID,
    MR::TaskParamFlags  taskParamFlags,
    FrameCount          validFrame);
  
  NM_INLINE void TaskAddEmittedCPParam(
    Task*              task,
    uint32_t           index,
    AttribDataSemantic semantic,
    AttribDataType     attribType,
    NodeID             owningNodeID,
    PinIndex           outputCPPinIndex);

  NM_INLINE void TaskAddOutputParam(
    Task*              task,
    uint32_t           index,
    AttribDataSemantic semantic,
    AttribDataType     attribType,
    NodeID             targetNodeID,
    MR::TaskParamFlags taskParamFlags,
    FrameCount         validFrame,
    uint16_t           lifespan,
    AnimSetIndex       animSetIndex = ANIMATION_SET_ANY);

  NM_INLINE void TaskAddOutputReplaceParam(
    Task*              task,
    uint32_t           index,
    AttribDataSemantic semantic,
    AttribDataType     attribType,
    NodeID             targetNodeID,
    FrameCount         validFrame,
    AnimSetIndex       animSetIndex = ANIMATION_SET_ANY);

  NM_INLINE void TaskAddOutputParamZeroLifespan(
    Task*              task,
    uint32_t           index,
    AttribDataSemantic semantic,
    AttribDataType     attribType,
    NodeID             targetNodeID,
    FrameCount         validFrame,
    AnimSetIndex       animSetIndex = ANIMATION_SET_ANY);

  NM_INLINE void TaskAddParamAndDependency(
    Task*              task,
    uint32_t           index,
    AttribDataSemantic semantic,
    AttribDataType     attribType,
    NodeID             owningNodeID,
    NodeID             targetNodeID,
    MR::TaskParamFlags taskParamFlags,
    FrameCount         validFrame,
    AnimSetIndex       animSetIndex = ANIMATION_SET_ANY);

  /// \brief Determines if the specified AttribAddress is validly queuable on the root empty task.
  ///
  /// i.e. the data already exist on the Network or the NetworkDef or there is a queuing function for it on the root node.
  NM_INLINE bool rootTaskParamIsQueuable(
    AttribDataSemantic  semantic,
    NodeID              owningNodeID,
    NodeID              targetNodeID,
    FrameCount          validFrame,
    AnimSetIndex        animSetIndex = ANIMATION_SET_ANY);

  NM_INLINE void RootTaskAddParamAndDependency(
    Task*               task,
    uint32_t            index,
    AttribDataSemantic  semantic,
    AttribDataType      attribType,
    NodeID              owningNodeID,
    NodeID              targetNodeID,
    MR::TaskParamFlags  taskParamFlags,
    FrameCount          validFrame,
    AnimSetIndex        animSetIndex = ANIMATION_SET_ANY);

  /// \brief Look for a piece of AttribData on the specified output node and pin. It must exist.
  NM_INLINE void TaskAddInputCP(
    Task*               task,
    uint32_t            index,
    AttribDataSemantic  semantic,
    const CPConnection* cpConnection);  ///< Identifies the output pin on another node that we are using as input.

  /// \brief Look for a piece of AttribData on the specified output node and pin. The connection can optionally be invalid.
  NM_INLINE void TaskAddOptionalInputCP(
    Task*               task,
    uint32_t            index,
    AttribDataSemantic  semantic,
    const CPConnection* cpConnection);  ///< Identifies the output pin on another node that we are using as input.

  //------------- External access attibutes -------------
  struct PostUpdateAccessAttribEntry
  {
    PostUpdateAccessAttribEntry* m_prev;
    PostUpdateAccessAttribEntry* m_next;
    NodeID                       m_nodeID;
    AttribDataSemantic           m_semantic;
    uint16_t                     m_minLifespan;
    uint16_t                     m_refCount;    ///< Needed because, if several locations are interested in post update
                                                ///< access, removing the interest of 1 of these locations will cause
                                                ///< the entry to removed and the lifespan adjust will no longer happen.
  };

  /// \brief Find out if the specified piece of attribute data needs to be accessed post the queue execution.
  PostUpdateAccessAttribEntry* findPostUpdateAccessAttrib(NodeID nodeID, AttribDataSemantic semantic) const;

  /// \brief Find out if the specified piece of attribute data needs to be accessed post the queue execution.
  /// \returns maximum of the input existingLifespan and the m_minLifespan of any found existing entry.
  uint16_t getPostUpdateAccessAttribLifespan(NodeID nodeID, AttribDataSemantic semantic, uint16_t existingLifespan = 0);

  /// \brief Add an attribute to the list of attributes that need to be accessed post the queue execution.
  void addPostUpdateAccessAttrib(
    NodeID             nodeID, 
    AttribDataSemantic semantic, 
    uint16_t           minLifespan = 1,
    bool               fromParent = true);

  /// \brief Remove an attribute from the list of attributes that need to be accessed post the queue execution.
  bool removePostUpdateAccessAttrib(NodeID nodeID, AttribDataSemantic semantic);

  /// \brief Add to the list of attribute addresses that need queuing to ensure that conditions can be evaluated.
  ///
  /// Called when StateMachines conditions are updated.
  void addConditionAttrib(AttribAddress& address, AttribDataType attribType);

  /// \brief Does this attribute address already exist on the m_conditionAttribList.
  ///
  /// Note: Only checks that the m_owningNodeID and m_semantic are the same.
  bool findConditionAttrib(AttribAddress& address);

  //------------- Network Playback Management -------------
  /// \brief get the root node translation change since the last call to updateNetworkInstance.
  /// \return translation change
  NMP::Vector3 getTranslationChange();

  /// \brief get the root node orientation change since the last frame.
  /// \return orientation change or identity quaternion if not initialised properly.
  NMP::Quat getOrientationChange();

  /// \brief Call this nodes delete instance function. Each nodes delete function should cope with recursion to children.
  NM_INLINE void deleteNodeInstance(NodeID nodeID);

  /// \brief Call this nodes update connections function.
  NodeID updateNodeInstanceConnections(NodeID nodeID, AnimSetIndex animSet);

  /// Returns the active physics nodes and their number. Only valid during the network update after connections
  /// has been evaluated.
  uint32_t getActivePhysicsNodes(NodeID*& nodeIDs);

  /// \brief Returns true if a behaviour node is active somewhere in the network
  bool areBehaviourNodesActive() const;
  /// Returns the active Grouper nodes and their number. Only valid during the network update after connections
  /// has been evaluated.
  uint32_t getActiveGrouperNodes(NodeID*& nodeIDs);

  /// This returns true if the node is an active physics node, or if the node is a grouper that is actively converting
  /// its input to physics. It doesn't return true just because a node's children are physical, or if it's a grouper
  /// that isn't converting to physics
  bool isNodePhysical(const NodeDef* node) const;

  /// Returns true if the node or one of its active children is an active physics or grouper (which must itself either
  /// be an active physics node, or have a child that is) node.
  bool isNodeOrActiveChildPhysical(const NodeDef* node) const;
  bool isNodeOrActiveChildPhysical(NodeID nodeID) const;

  /// \enum  MR::Network::RootControlMethod
  /// \brief  Indication of how the network is expected to control the root of the character.
  /// \ingroup
  enum RootControlMethod
  {
    ROOT_CONTROL_ANIMATION,         ///< Animation will generate a trajectory that the application will
                                    ///<  likely pass into a character controller.
    ROOT_CONTROL_PHYSICS,           ///< The root movement is suggested by physics - in this case the application
                                    ///<  should not use a character controller
                                    ///<  (although it is free to do as it likes with the root).
    ROOT_CONTROL_PHYSICAL_ANIMATION ///< Means the same as ROOT_CONTROL_ANIMATION, except the animation is the
                                    ///<  drive for some physics method (e.g. soft/hard keyframing)
  };

  /// \brief Indicates how the root is controlled.
  RootControlMethod getRootControlMethod() const { return m_rootControlMethod; }

  /// \brief Sets how the root should be controlled.
  ///
  /// Will normally be called by the nodes/runtime at the start of the network evaluation.
  /// This doesn't just copy the value - it chooses a value based on the current value and the value passed in.
  /// Returns true if the new method is actually used
  bool setRootControlMethod(RootControlMethod method);

  //------------- Callback management -------------
  /// \brief The state machine callback function prototype.
  typedef void (StateMachineStateChangeCallback)(MR::NodeID stateMachineNodeID, MR::NodeID newRootNodeID);
  /// \brief The node activation/deactivation callback function prototype.
  typedef void (NodeActivationCallback)(MR::NodeID affectedNodeID);
  /// \brief The triggered event callback function prototype.
  typedef void (EventTriggeredCallback)(TriggeredDiscreteEventsBuffer* eventBuffer);

  /// \brief Supply a callback to the Network, which will be called whenever the specified state machine changes it's
  ///  root state.
  void setStateMachineStateChangeCB(MR::NodeID stateMachineNodeID, StateMachineStateChangeCallback* transitStartedCB);
  
  /// \brief Retrieve the stored callback which should be called whenever the specified state machine changes it's
  ///  root state.
  StateMachineStateChangeCallback* getStateMachineStateChangeCB(MR::NodeID stateMachineNodeID);

  /// \brief Supply a callback to the Network, which will be called whenever a node is activated
  void setNodeActivationCB(NodeActivationCallback* nodeActivationCallback);
  /// \brief Retrieve the stored callback which is called when a node is activated
  NodeActivationCallback* getNodeActivationCB();

  /// \brief Supply a callback to the Network, which will be called whenever a node is deactivated
  void setNodeDeactivationCB(NodeActivationCallback* nodeDeactivationCallback);
  /// \brief Retrieve the stored callback which is called when a node is deactivated
  NodeActivationCallback* getNodeDeactivationCB();

  /// \brief Supply a callback to the Network, which will be called whenever a triggered event is output in the current
  ///  frame of network update
  void setEventTriggeredCB(EventTriggeredCallback* eventTriggeredCallback);
  /// \brief Retrieve the stored callback which is called when triggered events are output from the network.
  EventTriggeredCallback* getEventTriggeredCB();

  //------------- Character controller -------------
  /// \brief Allow the application to set the character controller used by this network.
  ///
  /// (essential for some nodes to work)
  void setCharacterController(CharacterControllerInterface* cc);  

  /// Sets the achieved requested motion flag
  void setCharacterPropertiesAchievedRequestedMovement(bool status);
  bool getCharacterPropertiesAchievedRequestedMovement();

  /// Implementation should call this to let the runtime know how long the controller has been on the ground.
  /// +ve time indicates the time it's been on the ground.
  /// -ve time means the time it's been in the air.
  void setCharacterPropertiesGroundContactTime(float time);
  float getCharacterPropertiesGroundContactTime();

  /// Implementation should call this to let the runtime know whether it is using a single update
  /// step to update the character controllers and physics at the same time, rather than the
  /// (preferred) method of separate controller and physics update steps.
  void setCharacterPropertiesPhysicsAndCharacterControllerUpdate(PhysicsAndCharacterControllerUpdate physicsAndCharacterControllerUpdate);
  PhysicsAndCharacterControllerUpdate getCharacterPropertiesPhysicsAndCharacterControllerUpdate() const;

  /// \brief Access the the current world root transform of the Network/game character and information reflecting
  ///  the state of the Networks associated character controller, if there is one.
  AttribDataCharacterProperties* getCharacterPropertiesAttribData();
  const AttribDataCharacterProperties* getCharacterPropertiesAttribData() const;

  /// Allow updating of the world root/offset - should be done by the
  /// application after it calculates a new location based on the network
  /// trajectory deltas (i.e. after its character capsule movement). The
  /// transform position should be around the character's feet, with y-axis up
  /// and z-axis pointing "forwards", so that it matches approximately with the
  /// root calculated in PhysicsRig::getRoot. Note that the previous root
  /// transform gets copied internally so that morpheme is able calculate the
  /// velocity of the character root - this means that normally this update
  /// function should get called once per update with copyCurrentToPrevious set
  /// to true.
  void updateCharacterPropertiesWorldRootTransform(
    const NMP::Matrix34& worldRootTransform,
    bool                 copyCurrentToPrevious);

  /// This sets the current and old world root transforms - use this rather than
  /// update when "teleporting" the character to a new
  /// location to make sure that the root velocity is zeroed.
  void setCharacterPropertiesWorldRootTransform(
    const NMP::Matrix34& worldRootTransform);

  /// WorldRootTransform accessors.
  const NMP::Matrix34& getCharacterPropertiesWorldRootTransform() const;
  const NMP::Matrix34& getCharacterPropertiesPrevWorldRootTransform() const;

  /// \brief Get the application-owned character controller that is responsible for moving
  ///  the character root around according to the trajectory changes.
  CharacterControllerInterface* getCharacterController() { return m_characterController; }
  const CharacterControllerInterface* getCharacterController() const { return m_characterController; }

  /// \brief Set the temporary memory allocator that the network uses to allocate temp data.  Note that this function
  ///  must not be called during network update.
  NM_INLINE void setTempMemoryAllocator(NMP::TempMemoryAllocator* tempAllocator);

  // \brief Get the current temporary memory allocator.
  NMP::TempMemoryAllocator* getTempMemoryAllocator() const { return m_tempDataAllocator; }
  // \brief Get the current persistent memory allocator.
  NMP::MemoryAllocator* getPersistentMemoryAllocator() const { return m_persistentDataAllocator; }

  uint32_t freeAllTempData();

  /// \brief Wipe down the contents of this node in the network ready for reuse.
  ///
  /// Including:
  ///  + Deleting all any existing attribute data belonging to this node in the Network.
  ///  + Reset the nodes active connections structure.
  ///  + Clear this nodes queued tasks list.
  ///  + Set the node's last updated frame to -1
  void cleanNodeData(NodeID nodeID);

  /// \brief Delete any allocated AttribData in the output control parameter pins of this node (if any).
  void deleteNodeOutputCPAttribData(NodeID nodeID);

#ifdef NM_DEBUG
  /// \brief To allow easy examination of attribute data currently remaining in the network.
  void debugOutputAttribContents(int32_t priority, bool countsOnly = false);

  /// \brief Display a nodes attributes.
  uint32_t debugOutputNodeAttribData(NodeID nodeID, int32_t priority, bool countsOnly = false);

  /// \brief Display the contents of the supplied NodeBinEntry.
  void debugOutputAttributeInfo(int32_t priority, NodeBinEntry* entry);

  /// \brief Make sure that all the node attributes currently in the network have sensible contents.
  void debugValidateAttribContents(bool validateNoTempAttribs);

  /// \brief Make sure that a bit of data hanging around on an inavtive node is allowed to be there.
  void validateInactiveNodeAttribData(NodeID nodeID, NodeBinEntry* entry, bool& entryFail);

  /// \brief List the numbers, types and lifetimes of attribs associated with each node.
  /// This is intended for detecting the presence of memory leaks.  If the number of attributes
  /// for a node increases without limit over time, the most likely cause is a problem with
  /// its task scheduler setup.  To track this down, look up the node number in the XML network file
  /// (generated by connect for the asset compiler), then look up the task scheduling code for this
  /// node type in mrCommonTasks.cpp
  void attribNodeAccounting(int32_t priority);

  /// \brief Hierarchically validate connectivity, from this node down.
  void debugValidateConnectivity(NodeID nodeID);

  void debugValidateActiveNodes();

  /// \brief Make sure that dependency is flowing in the correct direction for this semantic.
  ///
  /// Incorrect flow direction can result in problems such as circular dependency.
  void debugValidateDependencySense(
    NodeID             dependentTaskOwningNodeID,
    NodeID             semanticOwningNodeID, 
    AttribDataSemantic semantic);
#endif

  /// \brief Update requests emitted from nodes in the network during previous frame.
  ///
  /// For all nodes that are flagged as being able to emit requests in the manifest ("NodeEmitsRequests"),
  /// find any 
  void updateEmittedMessages();

  /// \brief Update requests emitted from nodes in the network during previous frame.
  void updateEmittedMessages(NodeID nodeID);

  /// \brief Decrement attrib entry lifespans, remove any whose lifespan becomes zero, then iterates to children.
  void updateAttribDataLifespans(NodeID nodeID);

  /// \brief Call the immediate update function of every output control parameter node.
  void updateEmittedControlParameters();

  /// \brief Emit possible requests of each emitter node
  void updateRequestEmitterNodes();

  //////////////////////////////////////////////////////////////////////////
  // the following methods are needed inside startUpdate and are public only to provide an easy interface in the
  // case that one might want to change/control the provided update behaviour from the outside

  /// \brief builds a dummy root task that will be the parent of all other tasks created during network update
  void buildDummyRootTaskAndQueueDependents(
    bool queueTransforms,
    bool queueTrajectory,
    bool queueSampleEvents,
    bool queueSyncEventTrack = false);

  /// \brief finds and stores the physics and grouper nodes for quick traversal later during update, sets the
  /// m_behaviourNodesActive flag.
  void getPhysicsAndGrouperNodesAndBehavioursActive();

  // Recursive get active nodes.
  uint32_t getActiveNodeCountUnder(NodeID nodeID) const;
  uint32_t getActiveNodesUnder(NodeID nodeID, NodeID* nodeIDsArray) const;
  uint32_t resetGatherFlagsUnder(NodeID nodeID) const;

  /// \brief
  uint32_t getNumActiveControlParamAndOpNodes();

  /// \brief
  NM_INLINE NodeBin* getNodeBin(NodeID nodeID) const;

  /// \brief retrieves attrib data matching the address.
  /// \return the attrib data entry. NULL if the data doesn't exist.
  NodeBinEntry* findIgnoringValidFrame(const AttribAddress& attribAddress);

  /// \brief retrieves the semantic used to store the root trajectory delta.
  NM_INLINE AttribDataSemantic getTrajectoryDeltaSemantic() const;

  /// \brief retrieves the semantic used to store the root transforms.
  NM_INLINE AttribDataSemantic getTransformSemantic() const;

#if defined(MR_ATTRIB_DEBUG_BUFFERING)
  /// \brief
  void sendInstanceProfileTimingDebugOutput();
#endif // MR_ATTRIB_DEBUG_BUFFERING

#if defined(MR_CIRCDEPS_DEBUGGING)
  /// \brief Optional debug VectorContainer for tracing circular task dependencies
  typedef NMP::VectorContainer<MR::TaskParameter*> TaskParamList;

  /// \brief display dependency trace
  NM_INLINE void debugDepsTrace();
  /// \brief add a param to the list
  NM_INLINE void startTraceParam(MR::TaskParameter* param);
  /// \brief remove param from list if no circular dependency found
  NM_INLINE void endTraceParam(MR::TaskParameter* param);
  
#endif // MR_CIRCDEPS_DEBUGGING

#ifdef MR_OUTPUT_DEBUGGING
  /// \brief Profile timings for debug output.
  enum OutputDebuggingProfilerPoints
  {
    QueueTasks,                     ///< Time taken for task queuing this frame.
    TaskExecution,                  ///< Time taken for task execution this frame.
    UpdateEmittedControlParameters,
    UpdateRequestEmitterNodes,
    UpdateAttribDataLifespans,
    UpdateNodeInstanceConnections,
    NumOutputDebuggingProfilerPoints
  };
  static const char* m_profilePointNames[Network::NumOutputDebuggingProfilerPoints];

  NM_INLINE void outputDebugTimingReset();
  NM_INLINE void outputDebugTimingBegin();
  NM_INLINE void outputDebugTimingEnd(OutputDebuggingProfilerPoints profilePoint);

  /// \brief Send debug data indicating the task that was queued and the content of its parameter list.
  ///
  /// Note that this operation adds significant performance cost. (Important when profiling).
  void sendQueuedTaskDebugInfo(Task* queuedTask);
#endif // MR_OUTPUT_DEBUGGING

  bool isUsingCombinedTrajectoryAndTransforms() { return m_combinedTrajectoryAndTransforms; }

private:
  void markActiveNodesUnder(NodeID nodeID);

  /// Allocate storage for post update access attrib addresses.
  static void initPostUpdateAccessAttribEntries(Network* network, NMP::Memory::Resource& resource);

  /// Allocate and initialise node connections array.
  static NMP::Memory::Format computeMemoryRequirementsActiveNodeConnectionsArray(NetworkDef* netDef);
  static NodeConnections** initActiveNodeConnectionsArray(NMP::Memory::Resource& resource, const NetworkDef* netDef);

  /// \brief Find and store the active physics and physics grouper nodes and see if behaviours are active
  ///
  /// Starts with the specified NodeID and iterates through all of its child sub-trees, also updating m_behaviourNodesActive flag. 
  void getActivePhysicsNodesAndBehaviourInChildHierarchy(NodeID startingNodeID);

  /// \brief Find if a certain physics nodeID is already within the m_activePhysicsNodeIDs[].
  NM_INLINE bool containsActivePhysicsNodeID( NodeID nodeID ) const;
  /// \brief Find if a certain grouper nodeID is already within the m_activeGrouperNodeIDs[].
  NM_INLINE bool containsActiveGrouperNodeID( NodeID nodeID ) const;

  /// \brief Add a nodeID into the m_activePhysicsNodeIDs[]. Checks the bound of the array. Returns true on successfully adding.
  NM_INLINE bool addActivePhysicsNodeID( NodeID nodeID );
  /// \brief Add a nodeID into the m_activeGrouperNodeIDs[]. Checks the bound of the array. Returns true on successfully adding.
  NM_INLINE bool addActiveGrouperNodeID( NodeID nodeID );
  
  /// \brief Store the specified triggered NodeEventMessage in either the internal or external list, so it can be
  ///  applied correctly after network update.
  bool storeTriggeredEvent(NetworkDef::NodeEventMessage* messageToStore);

  NetworkDef*           m_netDef;
  TaskQueue*            m_taskQueue;
  Dispatcher*           m_dispatcher;

  /// The node attribute data
  NodeBin*              m_nodeBins;

  /// The current parent and child connectivity of every node.
  NodeConnections**     m_activeNodesConnections;

  /// Count of how many frames this Network has been updated for.
  FrameCount            m_currentFrameNo;

  static const uint32_t MAX_ACTIVE_PHYSICS_NODE_IDS = 32;
  NodeID                m_activePhysicsNodeIDs[MAX_ACTIVE_PHYSICS_NODE_IDS];
  uint32_t              m_numActivePhysicsNodes;

  // Set during update connectivity
  bool                  m_behaviourNodesActive;
  static const uint32_t MAX_ACTIVE_GROUPER_NODE_IDS = 32;
  NodeID                m_activeGrouperNodeIDs[MAX_ACTIVE_GROUPER_NODE_IDS];
  uint32_t              m_numActiveGrouperNodes;

  // Stored in the m_attribDatas array for access via address, and stored here as a pointer for quick access from queuing functions.
  AttribDataUInt*       m_activeAnimSetIndex;

  /// This frames list of attribute addresses that need queuing to ensure that conditions can be evaluated.
  /// Other dependencies may have already queued the update of these attributes, but having this list ensures that they are updated.
  ConditionAttribEntry* m_conditionAttribList;

  /// How the root is moved around
  RootControlMethod     m_rootControlMethod;

  /// Pointer to the character controller (should normally exist, but may not) owned by the application
  CharacterControllerInterface*  m_characterController;

  /// A list of attributes that must be available post the queue execution.
  /// Attributes in this list are created with a lifespan of, at minimum, 1.
  /// e.g. Root node network transforms output, child node transforms for debug viewing in connect and
  /// any other application requirement.
  /// State machine conditions also use this system to make sure that attributes that are checked against are maintained.
  PostUpdateAccessAttribEntry** m_postUpdateAccessAttribEntries; ///< size of m_numNodes.

  /// This memory allocator is used to allocate any attribdata that has a lifespan of 0 (i.e. temporary data generated during network execution)
  /// It's also used for any other temporary data that only exists during network execution.
  NMP::TempMemoryAllocator* m_tempDataAllocator;

  /// This memory allocator is used to allocate any attribdata that has a lifespan of 1 or more
  /// It is also used to allocate any other data that persists from frame to frame.
  NMP::MemoryAllocator*   m_persistentDataAllocator;

  /// This stores the update time step for the previous frame.  If the last timestep was absolute, zero is stored.
  float                  m_lastUpdateTimeStep;

  /// True if trajectory delta and transforms were combined into ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORMS this frame
  bool                   m_combinedTrajectoryAndTransforms;

#if defined(MR_CIRCDEPS_DEBUGGING)
  TaskParamList*         m_taskTrace;
#endif // MR_CIRCDEPS_DEBUGGING

#ifdef MR_OUTPUT_DEBUGGING
  /// Profile timings for debug output.
  NMP::Timer m_outDbgProfilingTimer;
  float      m_outDbgTimings[NumOutputDebuggingProfilerPoints];
#endif // MR_OUTPUT_DEBUGGING

public:
  uint32_t          m_peakUsedTempData;
  MR::ExecuteResult m_exeResult;

  static AnimSetChangedCB* sm_animSetChangedCB;

  /// Callback to be called every time a node is activated in the network
  NodeActivationCallback* m_nodeActivationCallback;
  /// Callback to be called every time a node is deactivated in the network
  NodeActivationCallback* m_nodeDeactivationCallback;
  /// Callback to be called every frame that a triggered event is output from the network root.
  EventTriggeredCallback* m_eventTriggeredCallback;

  typedef struct NodeEventMessageLink
  {
    NetworkDef::NodeEventMessage*     m_message;
    NodeEventMessageLink* m_next;
  } NodeEventMessageLink;

  /// Node event messages triggered this frame, which are to be sent to this network for next frame
  NodeEventMessageLink *m_triggeredInternalMessages;
  /// Node event messages triggered this frame, which are to be broadcast to other networks next frame
  MessageID *m_triggeredExternalMessages;
  uint32_t   m_numTriggeredExternalMessages;
  uint32_t   m_triggeredExternalMessagesSize;
};

//----------------------------------------------------------------------------------------------------------------------
/// Optional TaskParamList functions for tracing circular task dependencies
#if defined(MR_CIRCDEPS_DEBUGGING)

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Network::debugDepsTrace()
{
  NMP_STDOUT("--------------Network has circular task dependency-------------------");
  for (TaskParamList::iterator it = m_taskTrace->begin(); it != m_taskTrace->end(); ++it)
  {
    TaskParameter* param = *it;
    NMP_STDOUT(
      "    node %i: %s",
      param->m_attribAddress.m_owningNodeID,
      param->m_attribAddress.getAttribSemanticName());
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Network::startTraceParam(MR::TaskParameter* param)
{
#ifdef NMP_ENABLE_ASSERTS
  bool notFull =
#endif
    m_taskTrace->push_back(param);
  NMP_ASSERT_MSG(notFull, "Task trace buffer full, increase size of TASK_PARAM_LIST_DEPTH");
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Network::endTraceParam(MR::TaskParameter* param)
{
  // Check completed param wasn't already tasked
  for (TaskParamList::iterator it = m_taskTrace->begin(); it != m_taskTrace->end(); ++it)
  {
    if (param != *it) // ignore self
    {
      AttribData* data = (*it)->m_attribDataHandle.m_attribData;
      // is task param a dependee?
      if (param->m_attribDataHandle.m_attribData == data)
      {
        debugDepsTrace();
        NMP_ASSERT_MSG(param->m_attribDataHandle.m_attribData != data, "Task has circular dependency!");
      }
    }
  }

  // Pop last and continue
  m_taskTrace->pop_back();
}

#endif // MR_CIRCDEPS_DEBUGGING

//----------------------------------------------------------------------------------------------------------------------
/// Profile timings for debug output.
#ifdef MR_OUTPUT_DEBUGGING

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Network::outputDebugTimingReset()
{
  if (m_dispatcher &&
      m_dispatcher->getDebugInterface() &&
      m_dispatcher->getDebugInterface()->debugOutputsAreOn(DEBUG_OUTPUT_PROFILING))
  {
    m_outDbgProfilingTimer.enable(false);
    m_outDbgProfilingTimer.enable(true);
    for (uint32_t i = 0; i < Network::NumOutputDebuggingProfilerPoints; i ++)
      m_outDbgTimings[i] = 0.0f;

    m_dispatcher->getDebugInterface()->clearNodeTimings();
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Network::outputDebugTimingBegin()
{
  if (m_dispatcher &&
      m_dispatcher->getDebugInterface() &&
      m_dispatcher->getDebugInterface()->debugOutputsAreOn(DEBUG_OUTPUT_PROFILING))
  {
    m_outDbgProfilingTimer.start();
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Network::outputDebugTimingEnd(OutputDebuggingProfilerPoints profilePoint)
{
  if (m_dispatcher &&
      m_dispatcher->getDebugInterface() &&
      m_dispatcher->getDebugInterface()->debugOutputsAreOn(DEBUG_OUTPUT_PROFILING))
  {
    NMP_ASSERT(profilePoint < NumOutputDebuggingProfilerPoints);
    m_outDbgTimings[profilePoint] += m_outDbgProfilingTimer.stop();
  }
}

  #define OUTDBG_TIMING_RESET(_net)  { NMP_ASSERT(_net); _net->outputDebugTimingReset(); }
  #define OUTDBG_TIMING_BEGIN(_net)  { NMP_ASSERT(_net); _net->outputDebugTimingBegin(); }
  #define OUTDBG_TIMING_END(_net, _p_pt)  { NMP_ASSERT(_net); _net->outputDebugTimingEnd(_p_pt); }

#else // MR_OUTPUT_DEBUGGING

  #define OUTDBG_TIMING_RESET(_net)
  #define OUTDBG_TIMING_BEGIN(_net)
  #define OUTDBG_TIMING_END(_net, _p_pt)

#endif // MR_OUTPUT_DEBUGGING

//----------------------------------------------------------------------------------------------------------------------
// Network class functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NodeBin* Network::getNodeBin(NodeID nodeID) const
{
  NMP_ASSERT(m_netDef && (nodeID < m_netDef->getNumNodeDefs()));
  return m_nodeBins + nodeID;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataSemantic Network::getTrajectoryDeltaSemantic() const
{
  if (m_combinedTrajectoryAndTransforms)
  {
    return ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER;
  }
  else
  {
    return ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM;
  }
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataSemantic Network::getTransformSemantic() const
{
  if (m_combinedTrajectoryAndTransforms)
  {
    return ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER;
  }
  else
  {
    return ATTRIB_SEMANTIC_TRANSFORM_BUFFER;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Network::deleteNodeInstance(NodeID nodeID)
{
  // Get the node def.
  NMP_ASSERT(m_netDef);

  NodeDef* nodeDef = m_netDef->getNodeDef(nodeID);
  NMP_ASSERT(nodeDef);

  // Call the nodes delete function.
  if (!nodeDef->getNodeFlags().isSet(NodeDef::NODE_FLAG_OUTPUT_REFERENCED))
  {
    nodeDef->deleteNodeInstance(this);
    m_activeNodesConnections[nodeID]->m_active = false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Network::isAnActiveParentOrGrandParent(NodeID startNodeID, NodeID searchNodeID)
{
  NodeID currentNodeID = startNodeID;

  while (currentNodeID != 0)
  {
    NMP_ASSERT(currentNodeID != INVALID_NODE_ID);
    NodeConnections* connections = getActiveNodesConnections(currentNodeID);
    NMP_ASSERT(connections);
    currentNodeID = connections->m_activeParentNodeID;

    if (currentNodeID == searchNodeID)
      return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NodeBinEntry* Network::getAttribDataNodeBinEntryRecurseFilterNodes(
  const AttribAddress& attributeAddress,
  bool                 fromParent)
{
  return getAttribDataNodeBinEntryRecurseFilterNodes(
           attributeAddress.m_semantic,
           attributeAddress.m_owningNodeID,
           attributeAddress.m_targetNodeID,
           attributeAddress.m_validFrame,
           attributeAddress.m_animSetIndex,
           fromParent);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NodeBinEntry* Network::getAttribDataNodeBinEntryRecurseFilterNodes(
  AttribDataSemantic semantic,
  NodeID             owningNodeID,
  NodeID             targetNodeID,
  FrameCount         validFrame,
  AnimSetIndex       animSetIndex,
  bool               fromParent)
{
  NodeBinEntry* entry = getAttribDataNodeBinEntry(semantic, owningNodeID, targetNodeID, validFrame, animSetIndex);
  if (!entry)
  {
    // AttribData not found in this node so if we are a filter node look for it in our parent or child
    // dependent on the direction from which the query came.
    NMP_ASSERT(m_netDef);
    NodeDef* nodeDef = m_netDef->getNodeDef(owningNodeID);
    NMP_ASSERT(nodeDef);
    NodeDef::NodeFlags flags = nodeDef->getNodeFlags();
    if (flags.isSet(NodeDef::NODE_FLAG_IS_FILTER))
    {
      NodeConnections* connections = getActiveNodesConnections(owningNodeID);
      NMP_ASSERT(connections);

      targetNodeID = owningNodeID;
      if (fromParent)
      {
        if (connections->m_numActiveChildNodes > nodeDef->getPassThroughChildIndex())
        {
          owningNodeID = connections->m_activeChildNodeIDs[nodeDef->getPassThroughChildIndex()];
          NMP_ASSERT(owningNodeID != INVALID_NODE_ID);
          entry = getAttribDataNodeBinEntryRecurseFilterNodes(semantic, owningNodeID, targetNodeID, validFrame, animSetIndex);
        }
      }
      else
      {
        owningNodeID = connections->m_activeParentNodeID;
        NMP_ASSERT(owningNodeID != INVALID_NODE_ID);
        entry = getAttribDataNodeBinEntryRecurseFilterNodes(semantic, owningNodeID, targetNodeID, validFrame, animSetIndex, false);
      }
    }
  }

  return entry;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribData* Network::getAttribDataRecurseFilterNodes(
  const AttribAddress& attributeAddress,
  bool                 fromParent)
{
  NodeBinEntry* attribDataNodeBinEntry = getAttribDataNodeBinEntryRecurseFilterNodes(
      attributeAddress.m_semantic,
      attributeAddress.m_owningNodeID,
      attributeAddress.m_targetNodeID,
      attributeAddress.m_validFrame,
      attributeAddress.m_animSetIndex,
      fromParent);
  if (attribDataNodeBinEntry)
    return attribDataNodeBinEntry->m_attribDataHandle.m_attribData;

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribData* Network::getAttribDataRecurseFilterNodes(
  AttribDataSemantic semantic,
  NodeID             owningNodeID,
  NodeID             targetNodeID,
  FrameCount         validFrame,
  AnimSetIndex       animSetIndex,
  bool               fromParent)
{
  NodeBinEntry* attribDataNodeBinEntry = getAttribDataNodeBinEntryRecurseFilterNodes(
      semantic,
      owningNodeID,
      targetNodeID,
      validFrame,
      animSetIndex,
      fromParent);
  if (attribDataNodeBinEntry)
    return attribDataNodeBinEntry->getAttribData();

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NodeBinEntry* Network::getAttribDataNodeBinEntry(const MR::AttribAddress& attributeAddress) const
{
  return getAttribDataNodeBinEntry(
           attributeAddress.m_semantic,
           attributeAddress.m_owningNodeID,
           attributeAddress.m_targetNodeID,
           attributeAddress.m_validFrame,
           attributeAddress.m_animSetIndex);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NodeBinEntry* Network::getAttribDataNodeBinEntry(
  AttribDataSemantic semantic,
  NodeID             owningNodeID,
  NodeID             targetNodeID,
  FrameCount         validFrame,
  AnimSetIndex       animSetIndex) const
{
  NMP_ASSERT(owningNodeID < getNetworkDef()->getNumNodeDefs());
  return getNodeBin(owningNodeID)->getEntry(semantic, targetNodeID, validFrame, animSetIndex);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribData* Network::getAttribData(const AttribAddress& attributeAddress)
{
  NodeBinEntry* attribDataNodeBinEntry = getAttribDataNodeBinEntry(attributeAddress);
  NMP_ASSERT(attribDataNodeBinEntry);
  return attribDataNodeBinEntry->getAttribData();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribData* Network::getAttribData(
  AttribDataSemantic semantic,
  NodeID             owningNodeID,
  NodeID             targetNodeID,
  FrameCount         validFrame,
  AnimSetIndex       animSetIndex)
{
  NodeBinEntry* attribDataNodeBinEntry = getAttribDataNodeBinEntry(
      semantic,
      owningNodeID,
      targetNodeID,
      validFrame,
      animSetIndex);
  NMP_ASSERT(attribDataNodeBinEntry);
  return attribDataNodeBinEntry->getAttribData();
}

//----------------------------------------------------------------------------------------------------------------------
template <class T>
NM_INLINE T* Network::getAttribData(const AttribAddress& attributeAddress)
{
  NodeBinEntry* attribDataNodeBinEntry = getAttribDataNodeBinEntry(attributeAddress);
  NMP_ASSERT(attribDataNodeBinEntry);
  NMP_ASSERT(T::getDefaultType() == attribDataNodeBinEntry->getAttribData()->getType());
  return static_cast<T*>(attribDataNodeBinEntry->getAttribData());
}

//----------------------------------------------------------------------------------------------------------------------
template <class T>
NM_INLINE T* Network::getAttribData(
  AttribDataSemantic semantic,
  NodeID             owningNodeID,
  NodeID             targetNodeID,
  FrameCount         validFrame,
  AnimSetIndex       animSetIndex)
{
  NodeBinEntry* attribDataNodeBinEntry = getAttribDataNodeBinEntry(
      semantic,
      owningNodeID,
      targetNodeID,
      validFrame,
      animSetIndex);
  NMP_ASSERT(attribDataNodeBinEntry);
  NMP_ASSERT(T::getDefaultType() == attribDataNodeBinEntry->getAttribData()->getType());
  return static_cast<T*>(attribDataNodeBinEntry->getAttribData());
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribData* Network::getOptionalAttribData(const AttribAddress& attributeAddress)
{
  NodeBinEntry* attribDataNodeBinEntry = getAttribDataNodeBinEntry(attributeAddress);
  if (attribDataNodeBinEntry)
    return attribDataNodeBinEntry->getAttribData();
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribData* Network::getOptionalAttribData(
  AttribDataSemantic semantic,
  NodeID             owningNodeID,
  NodeID             targetNodeID,
  FrameCount         validFrame,
  AnimSetIndex       animSetIndex)
{
  NodeBinEntry* attribDataNodeBinEntry = getAttribDataNodeBinEntry(
      semantic,
      owningNodeID,
      targetNodeID,
      validFrame,
      animSetIndex);
  if (attribDataNodeBinEntry)
    return attribDataNodeBinEntry->getAttribData();
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
template <class T>
NM_INLINE T* Network::getOptionalAttribData(
  const AttribAddress& attributeAddress)
{
  NodeBinEntry* attribDataNodeBinEntry = getAttribDataNodeBinEntry(attributeAddress);
  if (attribDataNodeBinEntry)
  {
    NMP_ASSERT(T::getDefaultType() == attribDataNodeBinEntry->getAttribData()->getType());
    return static_cast<T*>(attribDataNodeBinEntry->getAttribData());
  }
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
template <class T>
NM_INLINE T* Network::getOptionalAttribData(
  AttribDataSemantic semantic,
  NodeID             owningNodeID,
  NodeID             targetNodeID,
  FrameCount         validFrame,
  AnimSetIndex       animSetIndex)
{
  NodeBinEntry* attribDataNodeBinEntry = getAttribDataNodeBinEntry(
      semantic,
      owningNodeID,
      targetNodeID,
      validFrame,
      animSetIndex);
  if (attribDataNodeBinEntry)
  {
    NMP_ASSERT(T::getDefaultType() == attribDataNodeBinEntry->getAttribData()->getType());
    return static_cast<T*>(attribDataNodeBinEntry->getAttribData());
  }
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Network::addAttribData(
  const AttribAddress& attribAddress,
  AttribDataHandle&    attribDataHandle,
  uint16_t             lifespan)
{
  NMP_ASSERT(lifespan > 0);  // We should never add zero lifespan attributes to the Network.
  NMP_ASSERT(!getAttribDataNodeBinEntry(attribAddress)); // We cannot add 2 bits of AttribData with the same address.

  NMP::MemoryAllocator* allocator = getPersistentMemoryAllocator();
  NodeBinEntry* newAD = (NodeBinEntry*)NMPAllocatorMemAlloc(allocator,
                          sizeof(NodeBinEntry), NMP_NATURAL_TYPE_ALIGNMENT);
  NMP_ASSERT(newAD);
  NMP_ASSERT(attribDataHandle.m_attribData->getType() != ATTRIB_TYPE_TRANSFORM_BUFFER || attribAddress.m_animSetIndex  != ANIMATION_SET_ANY);

  newAD->m_address = attribAddress;
  newAD->m_allocatorUsed = allocator;
  newAD->m_attribDataHandle = attribDataHandle;
  newAD->m_lifespan = lifespan;

  getNodeBin(attribAddress.m_owningNodeID)->add(newAD);

  newAD->getAttribData()->refCountIncrease();

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename _AttribDataClass>
NM_INLINE void Network::setControlParameter(
  NodeID            owningNodeID,   // The node that this attribute belongs to.
  _AttribDataClass* attributeData,  // The attribute data itself.
  PinIndex          outputPinIndex) // The output pin we are setting.
{
  NMP_ASSERT(attributeData);
  NMP_ASSERT(m_netDef->getNodeDef(owningNodeID)->getNodeFlags() & MR::NodeDef::NODE_FLAG_IS_CONTROL_PARAM);

  // Control param attribute data should already exists in the Network, so copy new values into the existing attribute
  NodeBin* nodeBin = getNodeBin(owningNodeID);
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(outputPinIndex);
  _AttribDataClass* outputCPAttribData = outputCPPin->getAttribData<_AttribDataClass>();

  // Copy the attribute data to the destination.
  _AttribDataClass::copy(attributeData, outputCPAttribData);
}

//----------------------------------------------------------------------------------------------------------------------
template <typename _AttribDataClass>
NM_INLINE _AttribDataClass* Network::getControlParameter(
  NodeID   owningNodeID,    // The node that this attribute belongs to.
  PinIndex outputPinIndex)  // The output pin we are getting.
{
  NMP_ASSERT(m_netDef->getNodeDef(owningNodeID)->getNodeFlags() & MR::NodeDef::NODE_FLAG_IS_CONTROL_PARAM);
  NodeBin* nodeBin = getNodeBin(owningNodeID);
  MR::OutputCPPin* const outputCPPin = nodeBin->getOutputCPPin(outputPinIndex);
  _AttribDataClass* const outputCPAttribData = outputCPPin->getAttribData<_AttribDataClass>();
  NMP_ASSERT(outputCPAttribData);
  return outputCPAttribData;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribData* Network::getControlParameter(
  NodeID   owningNodeID,    // The node that this attribute belongs to.
  PinIndex outputPinIndex)  // The output pin we are getting.
{
  NMP_ASSERT(m_netDef->getNodeDef(owningNodeID)->getNodeFlags() & MR::NodeDef::NODE_FLAG_IS_CONTROL_PARAM);
  NodeBin* nodeBin = getNodeBin(owningNodeID);
  MR::OutputCPPin* const outputCPPin = nodeBin->getOutputCPPin(outputPinIndex);
  AttribData* const outputCPAttribData = outputCPPin->getAttribData();
  NMP_ASSERT(outputCPAttribData);
  return outputCPAttribData;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribData* Network::updateInputCPConnection(const CPConnection* cpConnection, AnimSetIndex animSet)
{
  return updateOutputCPAttribute(cpConnection->m_sourceNodeID, cpConnection->m_sourcePinIndex, animSet);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribData* Network::updateOptionalInputCPConnection(const CPConnection* cpConnection, AnimSetIndex animSet)
{
  if (cpConnection->m_sourceNodeID == INVALID_NODE_ID)
    return NULL;

  return updateOutputCPAttribute(cpConnection->m_sourceNodeID, cpConnection->m_sourcePinIndex, animSet);
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T> 
NM_INLINE T* Network::updateInputCPConnection(const CPConnection* cpConnection, AnimSetIndex animSet)
{
  AttribData *result = updateOutputCPAttribute(cpConnection->m_sourceNodeID, cpConnection->m_sourcePinIndex, animSet);
  NMP_ASSERT(T::getDefaultType() == result->getType());
  return static_cast<T*>(result);
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T> 
NM_INLINE T* Network::updateOptionalInputCPConnection(const CPConnection* cpConnection, AnimSetIndex animSet)
{
  if (cpConnection->m_sourceNodeID == INVALID_NODE_ID)
    return NULL;

  AttribData *result = updateOutputCPAttribute(cpConnection->m_sourceNodeID, cpConnection->m_sourcePinIndex, animSet);
  NMP_ASSERT(T::getDefaultType() == result->getType());
  return static_cast<T*>(result);
}

//----------------------------------------------------------------------------------------------------------------------
/// Macros that ensure that functions that are only meant for debug are fully excluded from release builds.
#ifdef NM_DEBUG

  /// Enables detailed attribute data checking in debug during every Network::update.
  /// You may want to exclude this for better performance in debug.
  #define DEBUG_VALIDATE_NETWORK_ATTRIB_CONTENTS(net, validateNoTempAttribs) net->debugValidateAttribContents(validateNoTempAttribs);

  /// Make sure that all active connections on the network are valid.
  #define DEBUG_VALIDATE_NETWORK_CONNECTIVITY() debugValidateConnectivity(NETWORK_NODE_ID); debugValidateActiveNodes();

  /// Make sure that dependency is flowing in the correct direction for this semantic.
  #define DEBUG_VALIDATE_DEPENDENCY_SENSE(net, dependentTaskOwningNodeID, semanticOwningNodeID, semantic) \
                    net->debugValidateDependencySense(  \
                      dependentTaskOwningNodeID,        \
                      semanticOwningNodeID,             \
                      semantic);

#else // NM_DEBUG

  #define DEBUG_VALIDATE_NETWORK_ATTRIB_CONTENTS(net, validateNoTempAttribs) {}
  #define DEBUG_VALIDATE_NETWORK_CONNECTIVITY() {}
  #define DEBUG_VALIDATE_DEPENDENCY_SENSE(net, dependentTaskOwningNodeID, semanticOwningNodeID, semantic) {}

#endif // NM_DEBUG

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Network::setNodeActivationCB(Network::NodeActivationCallback* nodeActivationCallback)
{
  m_nodeActivationCallback = nodeActivationCallback;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Network::NodeActivationCallback* Network::getNodeActivationCB()
{
  return m_nodeActivationCallback;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Network::setNodeDeactivationCB(Network::NodeActivationCallback* nodeDeactivationCallback)
{
  m_nodeDeactivationCallback = nodeDeactivationCallback;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Network::NodeActivationCallback* Network::getNodeDeactivationCB()
{
  return m_nodeDeactivationCallback;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Network::setEventTriggeredCB(Network::EventTriggeredCallback* eventTriggeredCallback)
{
  m_eventTriggeredCallback = eventTriggeredCallback;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Network::EventTriggeredCallback* Network::getEventTriggeredCB()
{
  return m_eventTriggeredCallback;
}

//----------------------------------------------------------------------------------------------------------------------
// Dependency adding function
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool addDependency(Network* net, TaskParameter* taskParam)
{
  // See if we can get the attribdata from the network - it may already be calculated
  MR::NodeBinEntry* attr = net->getAttribDataNodeBinEntry(taskParam->m_attribAddress);
  if (attr)
  {
    // The attrib data was found in the bin entry so assign it to the task param.
    taskParam->m_attribDataHandle = attr->m_attribDataHandle;
  }
  else
  {
    // We assume that if the data can be stored in the network definition, then there won't be a way to calculate
    // it. Therefore check to see if there's a task first (since it's a quick check) before searching the network
    // definition.
    QueueAttrTaskFn fn = net->getNetworkDef()->getNodeDef(taskParam->m_attribAddress.m_owningNodeID)->getTaskQueueingFn(taskParam->m_attribAddress.m_semantic);
    if (fn)
    {
      // If we haven't found a way out of avoiding queuing this dependency then we better stick it on the list :(
      net->queueTasksFor(taskParam);
    }
    else
    {
      // See if we can get the attribdata from the network definition:
      AnimSetIndex animSet = taskParam->m_attribAddress.m_animSetIndex;
      if (animSet == ANIMATION_SET_ANY)
        animSet = 0;
      MR::AttribDataHandle* attrData = net->getNetworkDef()->getOptionalAttribDataHandle(
                          taskParam->m_attribAddress.m_semantic,
                          taskParam->m_attribAddress.m_owningNodeID,
                          animSet);
      NET_LOG_ASSERT_MESSAGE(
                          attrData,
                          "ERROR: Unable to find either queuing function or attrib data for dependent attribute\n       Dep Attrib %s (%i), owner %i, target %i, anim set index %i\n",
                          taskParam->m_attribAddress.getAttribSemanticName(),
                          taskParam->m_attribAddress.m_semantic,
                          taskParam->m_attribAddress.m_owningNodeID,
                          taskParam->m_attribAddress.m_targetNodeID,
                          taskParam->m_attribAddress.m_animSetIndex);

      // Mark the dependency's parameter as evaluated.
      // That's all we need to do in this case.
      taskParam->m_attribDataHandle = *attrData;      
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
// Parameter locating function
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool locateParam(Network* net, TaskParameter* taskParam)
{
  // See if we can get the AttribData from the network - it may already be calculated.
  MR::NodeBinEntry* attr = net->getAttribDataNodeBinEntry(taskParam->m_attribAddress);
  if (attr)
  {
    // Stick it in the param.
    taskParam->m_attribDataHandle = attr->m_attribDataHandle;
    return true;
  }
  else
  {
    // See if we can get the attribdata from the network definition:
    AnimSetIndex animSet = taskParam->m_attribAddress.m_animSetIndex;
    if (animSet == ANIMATION_SET_ANY)
      animSet = 0;
    MR::AttribDataHandle* attrData = net->getNetworkDef()->getOptionalAttribDataHandle(
                                                              taskParam->m_attribAddress.m_semantic,
                                                              taskParam->m_attribAddress.m_owningNodeID,
                                                              animSet);
    if (attrData)
    {
      // Stick it in the param.
      taskParam->m_attribDataHandle = *attrData;
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
// Call the update function of a connected input control parameter and get the resulting AttribData.
// Assign the m_value of the resulting AttribData to already defined variable (_attribValueName).
// Utility primarily for use within operator node control parameter update functions.
#define UPDATE_OPTIONAL_INPUT_CP_CONNECTION(_InputCPConnection, _attribValueName, _attribClass, _animSet)                      \
    NMP_ASSERT(net);                                                                                                           \
    if ((_InputCPConnection)->m_sourceNodeID != INVALID_NODE_ID)                                                               \
    {                                                                                                                          \
      _attribClass *inputCPAttrib = (_attribClass *)                                                                           \
         net->updateOutputCPAttribute((_InputCPConnection)->m_sourceNodeID, (_InputCPConnection)->m_sourcePinIndex, _animSet); \
      (_attribValueName) = inputCPAttrib->m_value;                                                                             \
    }

//----------------------------------------------------------------------------------------------------------------------
// Outputting an attribute address to the network log.
#if defined(MR_NETWORK_LOGGING)

  #define NET_LOG_OUTPUT_ATTRIB_ADDRESS(_priority, _attributeAddress, _networkDef)         \
        MR::NetworkDef *nDef = (_networkDef);                                              \
        if (nDef)                                                                          \
        {                                                                                  \
          NET_LOG_MESSAGE(                                                                 \
                    _priority,                                                             \
                    "Attrib %45s (%3i), owner %3i %48s, target %5i, anim set index %2i\n", \
                    _attributeAddress.getAttribSemanticName(),                             \
                    _attributeAddress.m_semantic,                                          \
                    _attributeAddress.m_owningNodeID,                                      \
                    nDef->getNodeNameFromNodeID(_attributeAddress.m_owningNodeID),         \
                    _attributeAddress.m_targetNodeID,                                      \
                    _attributeAddress.m_animSetIndex);                                     \
        }                                                                                  \
        else                                                                               \
        {                                                                                  \
          NET_LOG_MESSAGE(                                                                 \
                    _priority,                                                             \
                    "Attrib %48s (%3i), owner %3i, target %5i, anim set index %2i\n",      \
                    _attributeAddress.getAttribSemanticName(),                             \
                    _attributeAddress.m_semantic,                                          \
                    _attributeAddress.m_owningNodeID,                                      \
                    _attributeAddress.m_targetNodeID,                                      \
                    _attributeAddress.m_animSetIndex);                                     \
        }

#else // defined(MR_NETWORK_LOGGING)

  #define NET_LOG_OUTPUT_ATTRIB_ADDRESS(_priority, _attributeAddress, _networkDef)

#endif // defined(MR_NETWORK_LOGGING)

//----------------------------------------------------------------------------------------------------------------------
// Network class functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NodeID Network::nodeFindGeneratingNodeForSemantic(
  NodeID             nodeID,
  AttribDataSemantic semantic,
  bool               fromParent,
  NodeID             callingNodeID)
{
  NMP_ASSERT(m_netDef);
  NodeDef* nodeDef = m_netDef->getNodeDef(nodeID);
  NMP_ASSERT(nodeDef);
  FindGeneratingNodeForSemanticFn findGeneratingNodeForSemanticFn = nodeDef->getFindGeneratingNodeForSemanticFn();
  NMP_ASSERT(findGeneratingNodeForSemanticFn);

  return findGeneratingNodeForSemanticFn(callingNodeID, fromParent, semantic, nodeDef, this);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Network::sendMessage(NodeID targetNodeID, const Message& message)
{
  NMP_ASSERT(targetNodeID < m_netDef->getNumNodeDefs());
  NodeDef* nodeDef = m_netDef->getNodeDef(targetNodeID);

#if defined(MR_OUTPUT_DEBUGGING)
  NMP_ASSERT(m_dispatcher);
  InstanceDebugInterface* debugInterface = m_dispatcher->getDebugInterface();
  if (debugInterface)
    debugInterface->logStateMachineMessageEventMsg(message.m_id, targetNodeID, message.m_status);
#endif // MR_OUTPUT_DEBUGGING

  return nodeDef->handleMessage(this, message);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Network::nodeIsActive(NodeID nodeID) const
{
  NMP_ASSERT_MSG(!(m_netDef->getNodeDef(nodeID)->getNodeFlags().isSet(NodeDef::NODE_FLAG_IS_OPERATOR_NODE) ||
                   m_netDef->getNodeDef(nodeID)->getNodeFlags().isSet(NodeDef::NODE_FLAG_IS_CONTROL_PARAM)),
                "The node is a control parameter or an operator node. Use operatorCPNodeIsActive instead.");
  NMP_ASSERT(nodeID < m_netDef->getNumNodeDefs());
  return m_activeNodesConnections[nodeID]->m_active;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Network::operatorCPNodeIsActive(NodeID nodeID) const
{
  NMP_ASSERT_MSG(m_netDef->getNodeDef(nodeID)->getNodeFlags().isSet(NodeDef::NODE_FLAG_IS_OPERATOR_NODE) ||
                 m_netDef->getNodeDef(nodeID)->getNodeFlags().isSet(NodeDef::NODE_FLAG_IS_CONTROL_PARAM),
                "The node is not a control parameter or an operator node. Use nodeIsActive instead.");
  NMP_ASSERT(nodeID < m_netDef->getNumNodeDefs());
  NodeBin* nodeBin = getNodeBin(nodeID);
  return (nodeBin->getLastFrameUpdate() == getCurrentFrameNo());
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NodeID Network::getActiveParentNodeID(NodeID nodeID) const
{
  NMP_ASSERT(nodeID < m_netDef->getNumNodeDefs());
  return m_activeNodesConnections[nodeID]->m_activeParentNodeID;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Network::incFrameCounter()
{
  m_currentFrameNo++;
  // Initialize for building an array of attributes that need queuing for update because transit conditions require them.
  m_conditionAttribList = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float Network::getLastUpdateTimeStep()
{
  return m_lastUpdateTimeStep;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Network::setActiveParentNodeID(NodeID nodeID, NodeID parentNodeID)
{
  NMP_ASSERT((nodeID < m_netDef->getNumNodeDefs()) && (parentNodeID < m_netDef->getNumNodeDefs()));
  m_activeNodesConnections[nodeID]->m_activeParentNodeID = parentNodeID;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t Network::getNumActiveChildren(NodeID nodeID) const
{
  NMP_ASSERT(nodeID < m_netDef->getNumNodeDefs());
  return m_activeNodesConnections[nodeID]->m_numActiveChildNodes;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Network::setNumActiveChildren(NodeID nodeID, uint32_t numActiveChildNodes)
{
  NMP_ASSERT(nodeID < m_netDef->getNumNodeDefs());
  NMP_ASSERT(numActiveChildNodes <= m_activeNodesConnections[nodeID]->m_maxNumActiveChildNodes);
  m_activeNodesConnections[nodeID]->m_numActiveChildNodes = (uint16_t)numActiveChildNodes;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NodeID Network::getActiveChildNodeID(NodeID nodeID, uint32_t childIndex) const
{
  NMP_ASSERT(nodeID < m_netDef->getNumNodeDefs() && childIndex < m_activeNodesConnections[nodeID]->m_numActiveChildNodes);
  return m_activeNodesConnections[nodeID]->m_activeChildNodeIDs[childIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Network::setActiveChildNodeID(NodeID nodeID, uint32_t childIndex, NodeID newChildNodeID)
{
  NMP_ASSERT(nodeID < m_netDef->getNumNodeDefs() && childIndex < m_activeNodesConnections[nodeID]->m_numActiveChildNodes);
  NMP_ASSERT(newChildNodeID < m_netDef->getNumNodeDefs());
  m_activeNodesConnections[nodeID]->m_activeChildNodeIDs[childIndex] = newChildNodeID;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NodeConnections* Network::getActiveNodesConnections(NodeID nodeID)
{
  NMP_ASSERT(nodeID < m_netDef->getNumNodeDefs());
  return m_activeNodesConnections[nodeID];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const NodeConnections* Network::getActiveNodesConnections(NodeID nodeID) const
{
  NMP_ASSERT(nodeID < m_netDef->getNumNodeDefs());
  return m_activeNodesConnections[nodeID];
}

//----------------------------------------------------------------------------------------------------------------------
/// Utility functions to assist when building a Task. Used to:
///  1. Build a list of all parameters (attributes) that are needed for the task to operate.
///  2. Build a list of attribute addresses that we are dependent on having been updated before us.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Network::TaskAddInputCP(
  Task*                    task,
  uint32_t                 index,
  AttribDataSemantic       semantic,
  const CPConnection* cpConnection)  // Identifies the output pin on another node that we are using as input.
{
  NMP_ASSERT(cpConnection);
  NMP_ASSERT(index < task->m_numParams);

  task->m_params[index].init(
    TPARAM_FLAG_INPUT,
    semantic,
    0,
    cpConnection->m_sourceNodeID,
    INVALID_NODE_ID,
    getCurrentFrameNo(),
    ANIMATION_SET_ANY,
    LIFESPAN_FOREVER);

  // Get the AttribData from connected nodes output control param pin. It must already exist.
  NMP_ASSERT(cpConnection->m_sourceNodeID != INVALID_NODE_ID);
  NodeBin* nodeBin = getNodeBin(cpConnection->m_sourceNodeID);
  OutputCPPin* outputCPPin = nodeBin->getOutputCPPin(cpConnection->m_sourcePinIndex);

  // Stick it in the param.
  task->m_params[index].m_attribDataHandle = outputCPPin->m_attribDataHandle;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Network::TaskAddOptionalInputCP(
  Task*               task,
  uint32_t            index,
  AttribDataSemantic  semantic,
  const CPConnection* cpConnection)  // Identifies the output pin on another node that we are using as input.
{
  NMP_ASSERT(cpConnection);
  NMP_ASSERT(index < task->m_numParams);

  task->m_params[index].init(
    TPARAM_FLAG_INPUT | TPARAM_FLAG_OPTIONAL,
    semantic,
    0,
    cpConnection->m_sourceNodeID,
    INVALID_NODE_ID,
    getCurrentFrameNo(),
    ANIMATION_SET_ANY,
    LIFESPAN_FOREVER);

  if (cpConnection->m_sourceNodeID != INVALID_NODE_ID)
  {
    // Get the AttribData from connected nodes output control param pin. It must already exist.
    NodeBin* nodeBin = getNodeBin(cpConnection->m_sourceNodeID);
    OutputCPPin* outputCPPin = nodeBin->getOutputCPPin(cpConnection->m_sourcePinIndex);

    // Stick it in the param.
    task->m_params[index].m_attribDataHandle = outputCPPin->m_attribDataHandle;
  }

  NET_LOG_CONDITIONAL_MESSAGE(cpConnection->m_sourceNodeID == INVALID_NODE_ID, 98, "    Following task param is optional and empty...\n");
  NET_LOG_OUTPUT_TASK_PARAM(98, task, index);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Network::TaskAddInputParam(
  Task*               task,
  uint32_t            index,
  AttribDataSemantic  semantic,
  NodeID              owningNodeID,
  NodeID              targetNodeID,
  MR::TaskParamFlags  taskParamFlags,
  FrameCount          validFrame,
  AnimSetIndex        animSetIndex/*= ANIMATION_SET_ANY*/)
{
  NMP_ASSERT(index < task->m_numParams);
  NMP_ASSERT(owningNodeID != INVALID_NODE_ID);  // Use TaskAddInputParamWithOptionallyInvalidOwningNode if this
                                                // situation is actually valid.
  taskParamFlags.set(TPARAM_FLAG_INPUT);
  task->m_params[index].init(
    taskParamFlags,
    semantic,
    0,
    owningNodeID,
    targetNodeID,
    validFrame,
    animSetIndex);
  locateParam(this, &(task->m_params[index]));
  NET_LOG_CONDITIONAL_MESSAGE(owningNodeID == INVALID_NODE_ID, 98, "    Following task param is optional and empty...\n");
  NET_LOG_OUTPUT_TASK_PARAM(98, task, index);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Network::TaskAddNetInputParam(
  Task*               task,
  uint32_t            index,
  AttribDataSemantic  semantic,
  NodeID              owningNodeID,
  NodeID              targetNodeID,
  FrameCount          validFrame,
  AnimSetIndex        animSetIndex/*= ANIMATION_SET_ANY*/)
{
  NMP_ASSERT(index < task->m_numParams);
  NMP_ASSERT(owningNodeID != INVALID_NODE_ID);  // Use TaskAddInputParamWithOptionallyInvalidOwningNode if this
                                                // situation is actually valid.
  task->m_params[index].init(
    TPARAM_FLAG_INPUT,
    semantic,
    0,
    owningNodeID,
    targetNodeID,
    validFrame,
    animSetIndex);

  // Get the AttribData from the network. It must already exist because this is an input param that is not a dependent.
  MR::NodeBinEntry* attr = getAttribDataNodeBinEntry(task->m_params[index].m_attribAddress);
  NMP_ASSERT(attr);

  // Stick it in the param.
  task->m_params[index].m_attribDataHandle = attr->m_attribDataHandle;

  NET_LOG_CONDITIONAL_MESSAGE(owningNodeID == INVALID_NODE_ID, 98, "    Following task param is optional and empty...\n");
  NET_LOG_OUTPUT_TASK_PARAM(98, task, index);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Network::TaskAddNetInputOutputParam(
  Task*               task,
  uint32_t            index,
  AttribDataSemantic  semantic,
  AttribDataType      attribType,
  NodeID              targetNodeID,
  FrameCount          validFrame,
  AnimSetIndex        animSetIndex/*= ANIMATION_SET_ANY*/)
{
  NMP_ASSERT(index < task->m_numParams);
  NMP_ASSERT(attribType != ATTRIB_TYPE_TRANSFORM_BUFFER || animSetIndex != ANIMATION_SET_ANY);
  task->m_params[index].init(
    TPARAM_FLAG_INPUT | TPARAM_FLAG_OUTPUT,
    semantic,
    attribType,
    task->m_owningNodeID,
    targetNodeID,
    validFrame,
    animSetIndex,
    LIFESPAN_FOREVER);

  NET_LOG_OUTPUT_TASK_PARAM(98, task, index);

  // Get the AttribData from the network.
  NodeBinEntry* nodeAttribData = findIgnoringValidFrame(task->m_params[index].m_attribAddress);

  // Stick it in the param.
  if (nodeAttribData)
  {
    task->m_params[index].m_attribDataHandle = nodeAttribData->m_attribDataHandle;

    NMP_ASSERT(task->m_params[index].m_attribDataHandle.m_attribData);

    // Update node bin here to avoid a second search for the bin in Dispatcher::TaskParameters::createOutputAttribReplace...().
    nodeAttribData->m_lifespan = LIFESPAN_FOREVER;
    nodeAttribData->m_address.m_validFrame = getCurrentFrameNo();
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Network::TaskAddOptionalNetInputOutputParam(
  Task*               task,
  uint32_t            index,
  AttribDataSemantic  semantic,
  AttribDataType      attribType,
  NodeID              targetNodeID,
  FrameCount          validFrame,
  AnimSetIndex        animSetIndex/*= ANIMATION_SET_ANY*/)
{
  NMP_ASSERT(index < task->m_numParams);

  NMP_ASSERT(attribType != ATTRIB_TYPE_TRANSFORM_BUFFER || animSetIndex != ANIMATION_SET_ANY);
  task->m_params[index].init(
    TPARAM_FLAG_INPUT | TPARAM_FLAG_OUTPUT | TPARAM_FLAG_OPTIONAL,
    semantic,
    attribType,
    task->m_owningNodeID,
    targetNodeID,
    validFrame,
    animSetIndex,
    LIFESPAN_FOREVER);

  NET_LOG_OUTPUT_TASK_PARAM(98, task, index);

  // Get the AttribData from the network.
  NodeBinEntry* nodeAttribData = findIgnoringValidFrame(task->m_params[index].m_attribAddress);

  // Stick it in the param.
  if (nodeAttribData)
  {
    task->m_params[index].m_attribDataHandle = nodeAttribData->m_attribDataHandle;

    NMP_ASSERT(task->m_params[index].m_attribDataHandle.m_attribData);

    // Update node bin here to avoid a second search for the bin in Dispatcher::TaskParameters::createOutputAttribReplace...().
    nodeAttribData->m_lifespan = LIFESPAN_FOREVER;
    nodeAttribData->m_address.m_validFrame = getCurrentFrameNo();
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Network::TaskAddOptionalNetInputParam(
  Task*               task,
  uint32_t            index,
  AttribDataSemantic  semantic,
  NodeID              owningNodeID,
  NodeID              targetNodeID,
  FrameCount          validFrame,
  AnimSetIndex        animSetIndex/*= ANIMATION_SET_ANY*/)
{
  NMP_ASSERT(index < task->m_numParams);
  NMP_ASSERT(owningNodeID != INVALID_NODE_ID);  // Use TaskAddInputParamWithOptionallyInvalidOwningNode if this
                                                // situation is actually valid.
  task->m_params[index].init(
    TPARAM_FLAG_INPUT | TPARAM_FLAG_OPTIONAL,
    semantic,
    0,
    owningNodeID,
    targetNodeID,
    validFrame,
    animSetIndex);

  // Get the AttribData from the network. It may already exist because this is an input param that is not a dependent.
  MR::NodeBinEntry* attr = getAttribDataNodeBinEntry(task->m_params[index].m_attribAddress);
  if (attr)
  {
    // Stick it in the param.
    task->m_params[index].m_attribDataHandle = attr->m_attribDataHandle;
  }

  NET_LOG_CONDITIONAL_MESSAGE(owningNodeID == INVALID_NODE_ID, 98, "    Following task param is optional and empty...\n");
  NET_LOG_OUTPUT_TASK_PARAM(98, task, index);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Network::TaskAddInputParamWithOptionallyInvalidOwningNode(
  Task*               task,
  uint32_t            index,
  AttribDataSemantic  semantic,
  NodeID              owningNodeID,
  NodeID              targetNodeID,
  FrameCount          validFrame,
  AnimSetIndex        animSetIndex/*= ANIMATION_SET_ANY*/)
{
  NMP_ASSERT(index < task->m_numParams);

  task->m_params[index].init(
    TPARAM_FLAG_INPUT | TPARAM_FLAG_OPTIONAL,
    semantic,
    0,
    owningNodeID,
    targetNodeID,
    validFrame,
    animSetIndex);

  NET_LOG_CONDITIONAL_MESSAGE(owningNodeID == INVALID_NODE_ID, 98, "    Following task param is optional and empty...\n");
  NET_LOG_OUTPUT_TASK_PARAM(98, task, index);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Network::TaskAddOptionalNetInputParamRecurseToParent(
  Task*               task,
  uint32_t            index,
  AttribDataSemantic  semantic,
  NodeID              owningNodeID,
  NodeID              targetNodeID,
  FrameCount          validFrame,
  AnimSetIndex        animSetIndex/*= ANIMATION_SET_ANY*/)
{
  NMP_ASSERT(index < task->m_numParams);
  NMP_ASSERT(m_netDef);
  NMP_ASSERT(owningNodeID != INVALID_NODE_ID);  // Use TaskAddInputParamWithOptionallyInvalidOwningNode if this
                                                // situation is actually valid.
  task->m_params[index].init(
    TPARAM_FLAG_INPUT | TPARAM_FLAG_OPTIONAL,
    semantic,
    0,
    owningNodeID,
    targetNodeID,
    validFrame,
    animSetIndex);

  NodeBinEntry* attr = getAttribDataNodeBinEntryRecurseToParent(semantic, owningNodeID, targetNodeID, validFrame, animSetIndex);
  if (attr)
  {
    // Stick it in the param.
    task->m_params[index].m_attribDataHandle = attr->m_attribDataHandle;
  }

  NET_LOG_CONDITIONAL_MESSAGE(owningNodeID == INVALID_NODE_ID, 98, "    Following task param is optional and empty...\n");
  NET_LOG_OUTPUT_TASK_PARAM(98, task, index);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Network::TaskAddOptionalNetInputParamRecurseFilterNodes(
  Task*               task,
  uint32_t            index,
  AttribDataSemantic  semantic,
  NodeID              owningNodeID,
  NodeID              targetNodeID,
  FrameCount          validFrame,
  AnimSetIndex        animSetIndex/*= ANIMATION_SET_ANY*/,
  bool                fromParent/*= true*/)
{
  NMP_ASSERT(index < task->m_numParams);
  NMP_ASSERT(m_netDef);
  NMP_ASSERT(owningNodeID != INVALID_NODE_ID);  // Use TaskAddInputParamWithOptionallyInvalidOwningNode if this
                                                // situation is actually valid.
  task->m_params[index].init(
    TPARAM_FLAG_INPUT | TPARAM_FLAG_OPTIONAL,
    semantic,
    0,
    owningNodeID,
    targetNodeID,
    validFrame,
    animSetIndex,
    LIFESPAN_FOREVER);

  NodeBinEntry* attr = getAttribDataNodeBinEntryRecurseFilterNodes(
      semantic,
      owningNodeID,
      targetNodeID,
      validFrame,
      animSetIndex,
      fromParent);

  if (attr)
  {
    // Stick it in the param.
    task->m_params[index].m_attribDataHandle = attr->m_attribDataHandle;
  }

  NET_LOG_CONDITIONAL_MESSAGE(owningNodeID == INVALID_NODE_ID, 98, "    Following task param is optional and empty...\n");
  NET_LOG_OUTPUT_TASK_PARAM(98, task, index);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Network::TaskAddDefInputParam(
  Task*               task,
  uint32_t            index,
  AttribDataSemantic  semantic,
  NodeID              owningNodeID,
  AnimSetIndex        animSetIndex/*= 0*/)
{
  NMP_ASSERT(animSetIndex != ANIMATION_SET_ANY); // We never use ANIMATION_SET_ANY with def data, if its AnimSet independent it goes in the AnimSet 0 data.
  NMP_ASSERT(index < task->m_numParams);
  NMP_ASSERT(owningNodeID != INVALID_NODE_ID);  // Use TaskAddInputParamWithOptionallyInvalidOwningNode if this
                                                // situation is actually valid.
  task->m_params[index].init(
    TPARAM_FLAG_INPUT,
    semantic,
    0,
    owningNodeID,
    INVALID_NODE_ID,
    VALID_FOREVER,
    animSetIndex,
    LIFESPAN_FOREVER);

  // Locate the attribute for this param in the NetworkDef.
  MR::AttribDataHandle* attrData = getNetworkDef()->getAttribDataHandle(task->m_params[index].m_attribAddress);
  NMP_ASSERT(attrData); // We are looking specifically for an attribute in the NetworkDef; it must exist.
  task->m_params[index].m_attribDataHandle = *attrData;

  NET_LOG_CONDITIONAL_MESSAGE(owningNodeID == INVALID_NODE_ID, 98, "    Following task param is optional and empty...\n");
  NET_LOG_OUTPUT_TASK_PARAM(98, task, index);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Network::TaskAddOptionalDefInputParam(
  Task*               task,
  uint32_t            index,
  AttribDataSemantic  semantic,
  NodeID              owningNodeID,
  AnimSetIndex        animSetIndex/*= 0*/)
{
  NMP_ASSERT(animSetIndex != ANIMATION_SET_ANY); // We never use ANIMATION_SET_ANY with def data, if its AnimSet independent it goes in the AnimSet 0 data.
  NMP_ASSERT(index < task->m_numParams);
  NMP_ASSERT(owningNodeID != INVALID_NODE_ID);  // Use TaskAddInputParamWithOptionallyInvalidOwningNode if this
                                                // situation is actually valid.
  task->m_params[index].init(
    TPARAM_FLAG_INPUT | TPARAM_FLAG_OPTIONAL,
    semantic,
    0,
    owningNodeID,
    INVALID_NODE_ID,
    VALID_FOREVER,
    animSetIndex,
    LIFESPAN_FOREVER);

  // Locate the attribute for this param in the NetworkDef; may or may not exist.
  MR::AttribDataHandle* attrData = getNetworkDef()->getOptionalAttribDataHandle(task->m_params[index].m_attribAddress);
  if (attrData)
    task->m_params[index].m_attribDataHandle = *attrData;

  NET_LOG_CONDITIONAL_MESSAGE(owningNodeID == INVALID_NODE_ID, 98, "    Following task param is optional and empty...\n");
  NET_LOG_OUTPUT_TASK_PARAM(98, task, index);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Network::TaskAddInputParamOptionallyInvalidOwningNode(
  Task*               task,
  uint32_t            index,
  AttribDataSemantic  semantic,
  NodeID              owningNodeID,
  NodeID              targetNodeID,
  MR::TaskParamFlags  taskParamFlags,
  FrameCount          validFrame)
{
  NMP_ASSERT(index < task->m_numParams);
  taskParamFlags.set(TPARAM_FLAG_INPUT);
  task->m_params[index].init(
    taskParamFlags,
    semantic,
    0,
    owningNodeID,
    targetNodeID,
    validFrame,
    ANIMATION_SET_ANY);
  NMP_ASSERT(owningNodeID == INVALID_NODE_ID ? taskParamFlags.isSet(TPARAM_FLAG_OPTIONAL) : true);
  if (owningNodeID != INVALID_NODE_ID)
  {
    locateParam(this, &(task->m_params[index]));
  }
  NET_LOG_CONDITIONAL_MESSAGE(owningNodeID == INVALID_NODE_ID, 98, "    Following task param is optional and empty...\n");
  NET_LOG_OUTPUT_TASK_PARAM(98, task, index);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Network::TaskAddOutputParam(
  Task*              task,
  uint32_t           index,
  AttribDataSemantic semantic,
  AttribDataType     attribType,
  NodeID             targetNodeID,
  MR::TaskParamFlags taskParamFlags,
  FrameCount         validFrame,
  uint16_t           lifespan,
  AnimSetIndex       animSetIndex/*= ANIMATION_SET_ANY*/)
{
  NMP_ASSERT_MSG(lifespan > 0, "For creating output param with a zero lifespan use TaskAddOutputParamZeroLifespan");
  NMP_ASSERT(lifespan >= getPostUpdateAccessAttribLifespan(task->m_owningNodeID, semantic));
  NMP_ASSERT(animSetIndex == ANIMATION_SET_ANY || animSetIndex == getOutputAnimSetIndex(task->m_owningNodeID));
  NMP_ASSERT(index < task->m_numParams);
  NMP_ASSERT(!taskParamFlags.isSet(TPARAM_FLAG_INPUT));

  taskParamFlags.set(TPARAM_FLAG_OUTPUT);
  task->m_params[index].init(
    taskParamFlags,
    semantic,
    attribType,
    task->m_owningNodeID,
    targetNodeID,
    validFrame,
    animSetIndex,
    lifespan);

  NET_LOG_OUTPUT_TASK_PARAM(98, task, index);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Network::TaskAddOutputReplaceParam(
  Task*              task,
  uint32_t           index,
  AttribDataSemantic semantic,
  AttribDataType     attribType,
  NodeID             targetNodeID,
  FrameCount         validFrame,
  AnimSetIndex       animSetIndex/*= ANIMATION_SET_ANY*/)
{
  NMP_ASSERT(index < task->m_numParams);
  NMP_ASSERT(attribType != ATTRIB_TYPE_TRANSFORM_BUFFER || animSetIndex != ANIMATION_SET_ANY);
  MR::TaskParamFlags taskParamFlags;
  taskParamFlags.clearThenSet(TPARAM_FLAG_OUTPUT);
  task->m_params[index].init(
    taskParamFlags,
    semantic,
    attribType,
    task->m_owningNodeID,
    targetNodeID,
    validFrame,
    animSetIndex,
    LIFESPAN_FOREVER);
  NET_LOG_OUTPUT_TASK_PARAM(98, task, index);

  // Get the AttribData from the network.
  NodeBinEntry* nodeAttribData = findIgnoringValidFrame(task->m_params[index].m_attribAddress);

  // Stick it in the param.
  if (nodeAttribData)
  {
    task->m_params[index].m_attribDataHandle = nodeAttribData->m_attribDataHandle;

    NMP_ASSERT(task->m_params[index].m_attribDataHandle.m_attribData);

    // Update node bin here to avoid a second search for the bin in Dispatcher::TaskParameters::createOutputAttribReplace...().
    nodeAttribData->m_lifespan = LIFESPAN_FOREVER;
    nodeAttribData->m_address.m_validFrame = getCurrentFrameNo();
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Network::TaskAddEmittedCPParam(
  Task*              task,
  uint32_t           index,
  AttribDataSemantic semantic,
  AttribDataType     attribType,
  NodeID             owningNodeID,
  PinIndex           outputCPPinIndex)
{
  NMP_ASSERT(index < task->m_numParams);

  task->m_params[index].init(
    TPARAM_FLAG_EMITTED_CP,
    semantic,
    attribType,
    owningNodeID,
    outputCPPinIndex,
    VALID_FOREVER,
    ANIMATION_SET_ANY,
    LIFESPAN_FOREVER);
  NET_LOG_OUTPUT_TASK_PARAM(98, task, index);

  // Get the AttribData from the output control param pin (May not exist yet).
  NodeBin* nodeBin = getNodeBin(owningNodeID);
  OutputCPPin* outputPin = nodeBin->getOutputCPPin(outputCPPinIndex);

  // Stick it in the param.
  task->m_params[index].m_attribDataHandle = outputPin->m_attribDataHandle; 
}

//----------------------------------------------------------------------------------------------------------------------
/// If the attribute is needed for query by the app make sure it is given the correct lifespan.
NM_INLINE void Network::TaskAddOutputParamZeroLifespan(
  Task*              task,
  uint32_t           index,
  AttribDataSemantic semantic,
  AttribDataType     attribType,
  NodeID             targetNodeID,
  FrameCount         validFrame,
  AnimSetIndex       animSetIndex/*= ANIMATION_SET_ANY*/)
{
  NMP_ASSERT(index < task->m_numParams);
  uint16_t lifespan = getPostUpdateAccessAttribLifespan(task->m_owningNodeID, semantic);

#if defined(MR_ATTRIB_DEBUG_BUFFERING)
  // If debug output of tree node attributes is enabled
  //  and the semantic is flagged for debug output against this node
  //  then make sure this output semantic exists for at least 1 frame so that it can be examined by the debug output system.
  if (m_dispatcher &&
      m_dispatcher->getDebugInterface() &&
      m_dispatcher->getDebugInterface()->debugOutputsAreOn(DEBUG_OUTPUT_TREE_NODES) &&
      getNodeBin(task->m_owningNodeID)->isDebugOutputSemanticSet(semantic) &&
      (lifespan == 0))
  {
    lifespan = 1;
  }
#endif // MR_ATTRIB_DEBUG_BUFFERING

  NMP_ASSERT((animSetIndex == ANIMATION_SET_ANY && attribType == ATTRIB_TYPE_TRANSFORM_BUFFER) == false);
  NMP_ASSERT( animSetIndex == ANIMATION_SET_ANY || animSetIndex == getOutputAnimSetIndex(task->m_owningNodeID));

  task->m_params[index].init(
    TPARAM_FLAG_OUTPUT,
    semantic,
    attribType,
    task->m_owningNodeID,
    targetNodeID,
    validFrame,
    animSetIndex,
    lifespan);

  NET_LOG_OUTPUT_TASK_PARAM(98, task, index);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Network::TaskAddParamAndDependency(
  Task*               task,
  uint32_t            index,
  AttribDataSemantic  semantic,
  AttribDataType      attribType,
  NodeID              owningNodeID,
  NodeID              targetNodeID,
  MR::TaskParamFlags  taskParamFlags,
  FrameCount          validFrame,
  AnimSetIndex        animSetIndex/*= ANIMATION_SET_ANY*/)
{
  // Make sure that dependency is flowing in the correct direction for this semantic.
  // Requesting incorrect flow direction can result in problems such as circular dependency.
  DEBUG_VALIDATE_DEPENDENCY_SENSE(this, task->m_owningNodeID, owningNodeID, semantic);

  NMP_ASSERT(index < task->m_numParams);
  task->m_params[index].init(
    taskParamFlags,
    semantic,
    attribType,
    owningNodeID,
    targetNodeID,
    validFrame,
    animSetIndex);
  addDependency(this, &(task->m_params[index]));
  NET_LOG_OUTPUT_TASK_PARAM(98, task, index);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Network::rootTaskParamIsQueuable(
  AttribDataSemantic  semantic,
  NodeID              owningNodeID,
  NodeID              targetNodeID,
  FrameCount          validFrame,
  AnimSetIndex        animSetIndex/*= ANIMATION_SET_ANY*/)
{
  if (getAttribDataNodeBinEntry(semantic, owningNodeID, targetNodeID, validFrame, animSetIndex) ||
      m_netDef->getOptionalAttribDataHandle(semantic, owningNodeID) ||
      (nodeFindGeneratingNodeForSemantic(owningNodeID, semantic, true, NETWORK_NODE_ID) != INVALID_NODE_ID))
  {
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Network::RootTaskAddParamAndDependency(
  Task*               task,
  uint32_t            index,
  AttribDataSemantic  semantic,
  AttribDataType      attribType,
  NodeID              owningNodeID,
  NodeID              targetNodeID,
  MR::TaskParamFlags  taskParamFlags,
  FrameCount          validFrame,
  AnimSetIndex        animSetIndex/*= ANIMATION_SET_ANY*/)
{
  NMP_ASSERT(index < task->m_numParams);
  TaskParameter* param = &(task->m_params[index]);
  param->init(taskParamFlags, semantic, attribType, owningNodeID, targetNodeID, validFrame, animSetIndex);

  // If the AttribData we are dependent on already exists do not queue a task to create it.
  MR::NodeBinEntry* attr = getAttribDataNodeBinEntry(param->m_attribAddress);
  if (attr)
  {
    param->m_attribDataHandle = attr->m_attribDataHandle;
  }
  else
  {
    QueueAttrTaskFn fn = m_netDef->getNodeDef(owningNodeID)->getTaskQueueingFn(semantic);
    if (fn)
    {
      NMP_ASSERT(nodeFindGeneratingNodeForSemantic(
        param->m_attribAddress.m_owningNodeID,
        param->m_attribAddress.m_semantic,
        true, NETWORK_NODE_ID) != NETWORK_NODE_ID);
      // If we haven't found a way out of avoiding queuing this dependency then we better stick it on the list :(
      queueTasksFor(param);
    }
    else
    {
      AnimSetIndex animSet = param->m_attribAddress.m_animSetIndex;
      if (animSet == ANIMATION_SET_ANY)
        animSet = 0;
      MR::AttribDataHandle* attrData = m_netDef->getOptionalAttribDataHandle(
                                                        param->m_attribAddress.m_semantic,
                                                        param->m_attribAddress.m_owningNodeID,
                                                        animSet);
      NET_LOG_ASSERT_MESSAGE(
        attrData,
        "ERROR: Unable to find either queuing function or attrib data for dependent attribute\n       Dep Attrib %s (%i), owner %i, target %i, anim set index %i\n",
        param->m_attribAddress.getAttribSemanticName(),
        param->m_attribAddress.m_semantic,
        param->m_attribAddress.m_owningNodeID,
        param->m_attribAddress.m_targetNodeID,
        param->m_attribAddress.m_animSetIndex);

      // Mark the dependency's parameter as evaluated.
      // That's all we need to do in this case.
      param->m_attribDataHandle = *attrData;
    }
  }
  NET_LOG_OUTPUT_TASK_PARAM(98, task, index);
}
  
//----------------------------------------------------------------------------------------------------------------------
void Network::setTempMemoryAllocator(NMP::TempMemoryAllocator*  tempAllocator)
{
  // Do not change the temporary memory allocator while the network is being processed.
  NMP_ASSERT(m_exeResult == EXECUTE_RESULT_COMPLETE);
  m_tempDataAllocator = tempAllocator;
}

//----------------------------------------------------------------------------------------------------------------------
// Network node functions.
//----------------------------------------------------------------------------------------------------------------------
// Delete node instance function.
void nodeNetworkDeleteInstance(
  const NodeDef* node,
  Network* net
);

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeNetworkFindGeneratingNodeForSemantic(
  NodeID              callingNodeID,
  bool                fromParent,   // Was this query from a parent or child node.
  AttribDataSemantic  semantic,
  NodeDef*            node,
  Network*            net);

//----------------------------------------------------------------------------------------------------------------------
Task* nodeNetworkQueueUpdateCharacterController(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeNetworkQueueUpdatePhysics(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeNetworkQueueUpdateRoot(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeNetworkQueueMergePhysicsRigTransformBuffer(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

//----------------------------------------------------------------------------------------------------------------------
NMP::DataBuffer* getNodeTransformsBuffer(
  NodeID       nodeID,
  const Network*     network,
  FrameCount   validFrame,
  AnimSetIndex animSetIndex);

//----------------------------------------------------------------------------------------------------------------------
NodeID getParentStateMachineOrNetworkRootNodeID(
  NodeDef* node,
  Network* net,
  bool*    foundStateMachine = NULL);

/// \brief Emitted messages are stored in a bit field (32 bits).
typedef AttribDataUInt AttribDataEmittedMessages;

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::EmittedMessagesUtil
/// \brief A utility class to help when:
///           + Adding the necessary parameters to a task that needs to be able to emit messages.
///           + Accessing and managing this parameter data inside a node task function.
/// NOTE: This class should only be used in these 2 contexts.
//----------------------------------------------------------------------------------------------------------------------
template <uint32_t TASK_PARAMSTART_INDEX, typename REQUESTNAMES = EmittedMessageMap::Names>
class EmittedMessagesUtil
{
public:
  //------------------
  /// \brief Get the input task parameter data and cache necessary info internally.
  ///
  /// Only for use inside a task.
  EmittedMessagesUtil(Dispatcher::TaskParameters* parameters)
  {
    NMP_ASSERT(parameters);
    NMP_ASSERT(TASK_PARAMSTART_INDEX < parameters->m_numParameters);

    // Get any emitted messages that may have already been created by other tasks this frame.
    AttribDataEmittedMessages* emittedRequestsInputAttrib =
      parameters->createOutputAttribReplace<AttribDataEmittedMessages>(
        TASK_PARAMSTART_INDEX,
        ATTRIB_SEMANTIC_EMITTED_MESSAGES);

    m_parameters = (emittedRequestsInputAttrib ? parameters : 0);

    // If another task has already emitted messages on this node cache them here so that 
    //  we can add to them rather than just overwriting them.
    m_messageIDs = (emittedRequestsInputAttrib ? emittedRequestsInputAttrib->m_value : 0);
  };

  //------------------
  /// \brief Construct output message flags attribute parameter.
  ///
  /// Only for use inside a task.
  ~EmittedMessagesUtil()
  {
    if (m_parameters && m_messageIDs)
    {
      // Create the emitted messages output attrib.
      AttribDataEmittedMessages* emittedRequestsOutputAttrib =
        m_parameters->createOutputAttribReplace<AttribDataEmittedMessages>(
          TASK_PARAMSTART_INDEX,
          ATTRIB_SEMANTIC_EMITTED_MESSAGES);

      // Set the output emitted messages bit field from our temporarily cached value.
      emittedRequestsOutputAttrib->m_value = m_messageIDs;
    }
  }

  //------------------
  /// \brief Add the necessary parameters to a task that needs to be able to emit messages.
  ///
  /// Only for use inside a task queuing function.
  static uint32_t addTaskParams(Network* net, Task* task, NodeID nodeID)
  {
    // Input/Output of any exiting messages on this node created from other tasks this frame.
    net->TaskAddOptionalNetInputOutputParam(
      task,
      TASK_PARAMSTART_INDEX,
      ATTRIB_SEMANTIC_EMITTED_MESSAGES,
      ATTRIB_TYPE_UINT,
      nodeID,
      net->getCurrentFrameNo());

    return getTotalTaskParams();
  }

  NM_INLINE void set(REQUESTNAMES request)
  {
    m_messageIDs |= request;
  };

  NM_INLINE void unset(REQUESTNAMES request)
  {
    m_messageIDs &= ~request;
  };

  NM_INLINE static uint32_t getTotalTaskParams()
  {
    return TASK_PARAMSTART_INDEX + 1;
  };

private:

  Dispatcher::TaskParameters* m_parameters; ///< Temporarily store a pointer to this tasks parameters structure 
                                            ///<  so that we can automatically add a parameter later if necessary.
  uint32_t                    m_messageIDs; ///< Temporary working store for our 32bits of output request data.
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NETWORK_H
//----------------------------------------------------------------------------------------------------------------------
