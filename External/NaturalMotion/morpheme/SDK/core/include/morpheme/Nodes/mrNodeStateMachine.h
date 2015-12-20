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
#ifndef MR_NODE_STATE_MACHINE_H
#define MR_NODE_STATE_MACHINE_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrAttribData.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/TransitConditions/mrTransitCondition.h"
#include "morpheme/mrTransitDeadBlend.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

/// \brief Indicates whether to include connectivity information in the StateDef structures.
///
///  If you don't need this information then deactivating this define will save memory.
///  NOTE: changing this define will require a recompile of the RuntimeTarget and the AssetCompiler.
#define MR_INCLUDE_CONNECTIVITY

//----------------------------------------------------------------------------------------------------------------------
// Queuing function declarations
Task* stateMachineNodeQueuePassThrough(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

//----------------------------------------------------------------------------------------------------------------------
// Update connections function.
NodeID nodeStateMachineUpdateConnections(
  NodeDef* node,
  Network* net);

//----------------------------------------------------------------------------------------------------------------------
void nodeStateMachineInitInstance(
  NodeDef* node,
  Network* net);

//----------------------------------------------------------------------------------------------------------------------
/// Delete node instance function.
/// Deletes temporary attrib data (mostly pass through data), but does not delete our attrib state data as it is
/// permanent with the Network instance.
void nodeStateMachineDeleteInstance(
  const NodeDef* node,
  Network*       net
);

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeStateMachineFindGeneratingNodeForSemantic(
  NodeID             callingNodeID,
  bool               fromParent,   // Was this query from a parent or child node.
  AttribDataSemantic semantic,
  NodeDef*           node,
  Network*           network);

//----------------------------------------------------------------------------------------------------------------------
bool nodeStateMachineHandleMessages(
  const Message&  message,
  NodeID          node,
  Network*        network);

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::StateMachineInitData
/// \brief
//----------------------------------------------------------------------------------------------------------------------
class StateMachineInitData : public NodeInitData
{
public:
  static NMP::Memory::Format getMemoryRequirements();

  static StateMachineInitData* init(NMP::Memory::Resource& resource, NodeID subStateID);

  static bool locate(NodeInitData* target);
  static bool dislocate(NodeInitData* target);

  NM_INLINE NodeID getInitialSubStateID() const { return m_subStateID; }
  NM_INLINE void setInitialSubStateID(NodeID stateID) { m_subStateID = stateID; }

protected:

  NodeID m_subStateID;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::StateDef
/// \brief The definition of a State within a State Machine.
//----------------------------------------------------------------------------------------------------------------------
class StateDef
{
public:
  bool dislocate();
  bool locate();

  /// \brief The ID of the Node that represents this state.
  NM_INLINE NodeID getNodeID() const { return m_nodeID; }

  /// \brief How many conditions does this State use.
  NM_INLINE ConditionIndex getNumExitConditions() const { return m_numExitConditions; }

  /// \brief How many of the conditions in this State are used by breakouts.
  NM_INLINE ConditionIndex getNumExitBreakoutConditions() const { return m_numExitBreakoutConditions; }
  
  /// \brief Get the StateMachine index of an exit condition from its local StateDef index.
  NM_INLINE ConditionIndex getExitConditionStateMachineIndex(StateConditionIndex index) const;

  /// \brief Get the number of possible state changes from this State.
  NM_INLINE uint32_t getNumExitTransitionStates() const { return m_numExitTransitionStates; }

  /// \brief Get one of this States possible state changes via its local index.
  NM_INLINE StateID getExitTransitionStateID(uint32_t index) const;

  /// \brief Get the number of conditions that must be satisfied to start this transition state.
  ///
  /// Always non zero for transitions and always zero for steady states.
  NM_INLINE ConditionIndex getNumEntryConditions() const { return m_numEntryConditions; }

  /// \brief Get the StateMachine index of an entry condition from its local StateDef index.
  NM_INLINE ConditionIndex getEntryConditionStateMachineIndex(StateConditionIndex index) const;

#ifdef MR_INCLUDE_CONNECTIVITY
  /// \brief Only returns non INVALID_STATE_ID if this is a transition. May still be INVALID_STATE_ID if this is  a global transition.
  NM_INLINE StateID getTransitSourceStateID() const { return m_sourceStateID; }

  /// \brief Not INVALID_STATE_ID if this is a transition state, INVALID_STATE_ID otherwise.
  NM_INLINE StateID getTransitDestinationStateID() const { return m_destinationStateID; }
#endif // MR_INCLUDE_CONNECTIVITY

public:
  /// The node that will be instanced as the root node of a State Machine when this state is active.
  NodeID          m_nodeID;                   

  //--------------------------
  /// The condition set that must be satisfied to enter this transition state.
  ///  Only applies if this is a transition state.
  ///  Always non zero for transitions and always zero for steady states.
  ///  MORPH-21330: Move this data in to a separate structure and reference from here with a pointer.
  ///               Saving space, but check that further indirection does not slow things down.
  ConditionIndex  m_numEntryConditions;
  ConditionIndex* m_entryConditionIndexes;
#ifdef MR_INCLUDE_CONNECTIVITY
  StateID         m_sourceStateID;      ///< Only used if this is a transition state. INVALID_STATE_ID otherwise.
                                        ///<  Can still be INVALID_STATE_ID if this is a global transition.
  StateID         m_destinationStateID; ///< Used if this is a transition state. INVALID_STATE_ID otherwise.
#endif // MR_INCLUDE_CONNECTIVITY

  //--------------------------
  /// Collection of the conditions on all transitions from this state. Indexes in to StateMachines array.
  ///  Individual conditions specified here may be referenced by more than one exit transition state
  ///  in this StateDef. When this State is active all of these conditions are updated.
  ConditionIndex  m_numExitConditions;
  ConditionIndex* m_exitConditionIndexes;     

  /// Number of breakout conditions used by transitions from this state, stored first in the m_exitConditionIndexes array.
  ///  Used when this is a destination state.
  ConditionIndex  m_numExitBreakoutConditions;

  //--------------------------
  /// All possible transition state changes from this state.
  ///  Each initiated by a sub-set of the above exit conditions being satisfied.
  ///  Only when a full set of conditions is satisfied will a transition start.
  StateID         m_numExitTransitionStates;
  StateID*        m_exitTransitionStateIDs;
};

//----------------------------------------------------------------------------------------------------------------------
class AttribDataStateMachineDef : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();
  static AttribDataStateMachineDef* init(
    NMP::Memory::Resource& resource,
    uint16_t               refCount = 0);

  NM_INLINE AttribDataStateMachineDef() { setType(ATTRIB_TYPE_STATE_MACHINE_DEF); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_STATE_MACHINE_DEF; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

  NM_INLINE ConditionIndex getNumConditions() const { return m_numConditions; }
  NM_INLINE TransitConditionDef* getConditionDef(ConditionIndex index) const;

  /// \brief Searches through the array of states to find the one with this Node ID as its root.
  /// \return INVALID_STATE_ID if not found.
  StateID findStateIDFromNodeID(
    NodeID  nodeID        ///< Node ID of a state within this state machine.
  ) const;

  /// \brief Gets the root node ID of a state within this StateMachine. Fast array lookup.
  NodeID getNodeIDFromStateID(
    StateID stateID       ///< Index of state within this state machine.
  ) const;

  /// \brief Get a list of the message IDs for transitions from the specified network node ID in the state machine.
  /// \return number of messages found for the specified node.
  ///
  /// Asserts if maxResults is exceeded.
  uint32_t getMessageIDsFromNodeID(
    NodeID     nodeID,    ///< Is ID for the node that we want the messages for.
    MessageID* results,   ///< Out: Is the array to be filled in.
    uint32_t   maxResults ///< Number of messages found for the specified node.
  );

  /// \brief Get a list of the message IDs for transitions from the specified state index in the state machine.
  ///
  /// Asserts if maxResults is exceeded.
  uint32_t getMessageIDsFromStateID(
    StateID    stateID,
    MessageID* results,
    uint32_t   maxResults);

  /// \brief Get a list of the message IDs for breakout transitions from the specified state index in the state machine.
  ///
  /// Asserts if maxResults is exceeded.
  uint32_t getMessageIDsOnBreakoutsFromStateID(
    StateID    stateID,
    MessageID* results,
    uint32_t   maxResults);

  /// \brief Get a list of the message IDs on any global transitions in the state machine.
  ///
  /// Asserts if maxResults is exceeded.
  uint32_t getGlobalMessageIDs(
    MessageID* results,
    uint32_t   maxResults);

  /// \brief Check to see if message id is valid for given state.
  ///
  /// Asserts if maxResults is exceeded.
  bool isMessageIDValidFromStateID(
    StateID   stateID,
    MessageID messageID) const;

  /// \brief Find all the transitions from the specified source state that are listening for the specified message.
  ///
  /// Asserts if maxResults is exceeded.
  uint32_t findTransitionsListeningToMessage(
    StateID   sourceStateID,      ///< We will examine all transitions from this state.
    MessageID messageID,          ///< The message to search for.
    StateID*  transitionStateIDs, ///< Returned array filled with IDs of all transitions that are listening for this message.
    uint32_t  maxResults);        ///< Size of transitionStateIDs array.

  /// \brief Gets the default start state ID of this state machine.
  /// \return Index of state within this StateMachine.
  NM_INLINE StateID getDefaultStartingStateID() { return m_defaultStartingStateID; }

  NM_INLINE StateID getNumStates() const { return m_numStates; }
  NM_INLINE StateDef* getStateDef(StateID stateID) const;
  NM_INLINE StateDef* getGlobalStateDef() const { return m_globalStateDef; }

  StateID               m_defaultStartingStateID; ///< Index the into local array of available states.

  StateID               m_numStates;              ///< Array of all the possible states of this StateMachine.
  StateDef*             m_stateDefs;              ///< Includes steady states and transitional states.
  StateDef*             m_globalStateDef;         ///< Contains a set of state changes (transitions) that can be
                                                  ///<  triggered from any active state within the state machine.

  bool                  m_prioritiseGlobalTransitions; ///< Allows to prioritize global transitions over mormal ones.

  ConditionIndex        m_numConditions;          ///< Pointers to all Conditions used within this State Machine.
  TransitConditionDef** m_conditions;             ///< These conditions are not shared with other StateMachines and hence
                                                  ///< management of these conditions is entirely local to this StateMachineDef.
private:
  /// \brief If the specified condition is a MessegeID condition, copy its ID to the results array and increment the numFound.
  void getMessageIDFromCondition(
    StateConditionIndex stateCondIndex,
    StateDef*           state,
    uint32_t&           numFound,
    MessageID*          results,
    uint32_t            maxResults);
};

//----------------------------------------------------------------------------------------------------------------------
class AttribDataStateMachine : public AttribData
{
public:
  // Initialising from def data. Used on line.
  static NMP::Memory::Format getMemoryRequirements(
    AttribDataStateMachineDef* stateMachineAttribDef);
  static AttribDataStateMachine* init(
    NMP::Memory::Resource&     resource,
    AttribDataStateMachineDef* stateMachineAttribDef,
    Network*                   net,
    NodeID                     smNodeID,
    uint16_t                   refCount = 0);

  NM_INLINE AttribDataStateMachine() { setType(ATTRIB_TYPE_STATE_MACHINE); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_STATE_MACHINE; }

  /// \brief A specialist relocate function is required because the state machine stores a pointer to its originating AttribDataStateMachineDef.
  //         This function also sets m_onStateChangedCB to NULL.
  static void relocate(AttribData* target, const NetworkDef* netDef);
  
  /// \brief A specialist locate function that just calls the relocate and does no endian swapping.
  static void locate(AttribData* target, const NetworkDef* netDef);

#if defined(MR_OUTPUT_DEBUGGING)
  static uint32_t serializeTx(Network* net, MR::NodeID owningNodeID, AttribData* toOutput, void* outputBuffer, uint32_t outputBufferSize = 0);
#endif // MR_OUTPUT_DEBUGGING

  /// \brief Manage the update of the active state of a state machine.
  ///
  /// Includes the update of conditions on the active state and the management of the initialisation and completion of transitions.
  void updateConnections(
    const NodeDef* smNodeDef,
    Network*       net);
  
  /// \brief Get the node id of the active root node of this state machine.
  /// \return kINVALID_NODE_ID if current state not set.
  NM_INLINE NodeID getActiveNodeID(AttribDataStateMachineDef* smDef) const;

  /// \brief Get the state id of the active root node of this state machine.
  /// \return INVALID_STATE_ID if current state not set.
  NM_INLINE StateID getActiveStateID() const;

  /// \brief
  NM_INLINE StateID getTargetStateID() const { return m_targetStateID; }
  NM_INLINE void setTargetStateID(StateID stateID) { m_targetStateID = stateID; }

  NM_INLINE const AttribDataStateMachineDef* getDef() const { return m_def; }

  /// \brief Flag a message as having been set or cleared on the active state.
  /// \return false if this messageID does not exist.
  ///
  /// If setting, it does not start a transition immediately. All of a transition sets conditions need to be true for a
  /// transition to start.
  /// The transition node knows the ID of the destination node, triggers can potentially allow
  /// transition to > 1 destination node, through use in multiple transition nodes.
  bool setTransitionMessageID(
    MessageID                  messageID, ///< Id of messages to set the status of.
    bool                       status,    ///< Set this message on or off.
    AttribDataStateMachineDef* smDef,
    Network*                   net);

  /// \brief Queue up a wipe down of the existing state and initialization to a new state.
  ///
  /// This will happen on the next network update of connections.
  /// If the nodeID doesn't exist the queueSetStateByNodeID call will be ignored.
  /// If the state machine is already in this state it will still be re-set.
  NM_INLINE void queueSetStateByNodeID(
    NodeID                           nodeID,  ///< Node ID of a state within this state machine.
    const AttribDataStateMachineDef* smDef);

  /// \brief Queue up a wipe down of the existing state and initialization to a new state.
  ///
  /// This will happen on the next network update of connections.
  /// If the stateID doesn't exist the queueSetStateByStateID call will be ignored.
  /// If the state machine is already in this state it will still be re-set.
  NM_INLINE void queueSetStateByStateID(
    StateID                          stateID, ///< Index into the array of possible states.
    const AttribDataStateMachineDef* smDef);

  /// \brief Get a list of the MessageIDs for transitions from the currently active state in the state machine.
  ///
  /// Includes conditions on transitions from the current state and
  /// if the active state is a transition find the MessageIDs on any break out transitions on the destination.
  NM_INLINE uint32_t getMessageIDsFromActiveState(
    uint32_t*                  results,
    uint32_t                   maxResults,
    AttribDataStateMachineDef* smDef,
    NetworkDef*                netDef);

  /// \brief Get a list of the MessageIDs for transitions from the specified state in the state machine.
  ///
  /// Includes conditions on transitions from the current state and
  /// if the state is a transition find the MessageIDs on any break out transitions on the destination.
  uint32_t getMessageIDsFromState(
    StateID                    stateID,
    uint32_t*                  results,
    uint32_t                   maxResults,
    AttribDataStateMachineDef* smDef,
    NetworkDef*                netDef);

  /// \brief Clear all of the conditions on the active state.
  NM_INLINE void resetActiveStateConditions(
    AttribDataStateMachineDef* smDef);

  /// \brief Clear all the message conditions in the active state.
  NM_INLINE void resetActiveStateMessageConditions(
    AttribDataStateMachineDef* smDef);

  /// \brief Make all global transition conditions ready for re-use.
  NM_INLINE void resetGlobalConditions(
    AttribDataStateMachineDef* smDef);

protected:
  /// \brief Wipe down any existing state and initialise and set a new root state.
  /// If the state machine is already in this state it will still be re-set.
  /// \return NodeID of the new root node instance.
  NodeID setStateByStateID(
    StateID          stateID,        ///< Index into the array of possible states.
    const NodeDef*   smNodeDef,
    Network*         net,
    NodeConnections* nodeConnections);

  /// \brief Wipe down any existing state and initialise and set a new root state.
  /// Searches through array of states to find this requested root Node.
  /// \return NULL If not found.
  /// \return StateID of the new root node instance.
  uint32_t setStateByNodeID(
    NodeID                     nodeID, // Node ID of a state within this state machine.
    AttribDataStateMachineDef* smDef,
    const NodeDef*             smNodeDef,
    Network*                   net,
    NodeConnections*           nodeConnections);

  /// \brief If the active state is a zero length transition that was set up last frame we must complete the transition here.
  void completeActiveZeroLengthTransition(
    AttribDataStateMachineDef* smDef,
    NodeConnections*           nodeConnections,
    Network*                   net,
    NodeID                     smNodeID);

  /// \brief Has a condition within the StateMachine been satisfied.
  bool isConditionSatisfied(
    ConditionIndex             conditionIndex, ///< Index into StateMachines array of Conditions.
    AttribDataStateMachineDef* smDef) const;

  /// \brief Update and check the status of all conditions and state changes from the active state.
  /// \return If a transition condition set is satisfied returns the index of the destination transition state.
  /// Otherwise returns result of updateStateForBreakoutTransits.
  StateID updateActiveState(
    AttribDataStateMachineDef* smDef,
    Network*                   net,
    bool&                      modifyTransition,
    bool&                      breakoutSelfTransition);

  /// \brief Update and check the status of all conditions and state changes from the global state.
  /// \return If a transition condition set is satisfied returns the index of the destination transition state.
  StateID updateGlobalState(
    AttribDataStateMachineDef* smDef,
    Network*                   net,
    bool&                      breakoutGlobalSelfTransition);

  /// \brief Update the status of conditions and state changes in this state's destination node's transitions
  StateID updateStateForBreakoutTransits(
    StateDef*                  stateDef,
    AttribDataStateMachineDef* smDef,
    Network*                   net,
    bool&                      modifyTransition,  ///< Tells the caller that a break out from the root transition
                                                  ///<  has started and that he needs to reconfigure connections appropriately.
    bool&                      breakoutSelfTransition);

  /// \brief If the indicated state is a transition make sure any conditions on any breakouts from its destination are made ready for re-use.
  void resetConditionsForBreakoutTransitions(
    StateID                    stateID,
    AttribDataStateMachineDef* smDef,
    Network*                   net);

  /// \brief Make sure that condition dependencies from the current state are queued.
  NM_INLINE void queueConditionDeps(
    StateID                    stateID,     ///< ID of state in this state machine.
    AttribDataStateMachineDef* smDef,
    Network*                   net);

  /// \brief Make sure that condition dependencies from the global state are queued.
  NM_INLINE void queueGlobalConditionDeps(
    StateID                    activeStateID,
    AttribDataStateMachineDef* smDef,
    Network*                   net);

  /// \brief Make sure that condition dependencies from the current state are queued.
  void queueConditionDeps(
    StateDef*                  stateDef,
    AttribDataStateMachineDef* smDef,
    Network*                   net,
    NodeID                     smActiveNodeID);

  /// \brief Make all transition conditions in state ready for re-use.
  NM_INLINE void resetStateConditions(
    StateID                    stateID,
    AttribDataStateMachineDef* smDef);

  /// \brief Make all transition conditions in state ready for re-use.
  void resetStateConditions(
    StateDef*                  stateDef,
    AttribDataStateMachineDef* smDef);

  /// \brief Clear all the messageID conditions in state ready for re-use.
  NM_INLINE void resetStateMessageIDConditions(
    StateID                    stateID,
    AttribDataStateMachineDef* smDef);

  /// \brief Clear all the global request conditions in state ready for re-use.
  NM_INLINE void resetGlobalMessageIDConditions(
    AttribDataStateMachineDef* smDef);

  /// \brief Clear all the request conditions in state ready for re-use.
  void resetStateMessageIDConditions(
    StateDef*                  stateDef,
    AttribDataStateMachineDef* smDef);

  /// \brief Find all the TransitionConditionRequests with the given ID in the specified state and set or clear them.
  /// \return false if trigger not found.
  bool setTransitionMessageIDOnState(
    const StateDef* const      stateDef,  ///< The state to amend within this state machine.
    MessageID                  messageID, ///< Id of messages to set the status of.
    bool                       status,    ///< Set this message on or off.
    AttribDataStateMachineDef* smDef);

  /// \brief If the specified state is a transition find all the TransitionConditionRequests with the given ID
  ///        on its destination state and set or clear them.
  ///
  /// This is so that we can set messages on break out transitions.
  /// \return false if trigger not found.
  bool setTransitionMessageIDOnBreakOutTransition(
    StateDef*                  stateDef,  ///< The state to amend within this state machine.
    MessageID                  messageID, ///< Id of messages to set the status of.
    bool                       status,    ///< Set this message on or off.
    AttribDataStateMachineDef* smDef,
    Network*                   network);

  /// \brief Evaluates the conditions of this transition and determines if they have all been satisfied.
  /// \return Index of destination state in StateMachine if a stateChange is satisfied otherwise INVALID_STATE_ID.
  StateID evaluateExitConditionsAndTestForSatisfaction(
    Network*                   net,
    StateDef*                  sourceStateDef,        ///< This can be a steady state node, a transition or a global state.
    NodeID                     sourceActiveNodeID,    ///< In the case of global state transitions this ID will not be that of the sourceStateDef.
    uint32_t                   exitTransitStateIndex, ///< Index local to the StateDef.
    AttribDataStateMachineDef* smDef) const;

  // MORPH-21331: Refactor these members to be private
public:
  StateID            m_activeStateID; ///< Index into local array of available states.
  StateID            m_targetStateID; ///< Index the into local array of available states.
                                      ///<  Records any messageSetState calls.
                                      ///<  Set to INVALID_NODE_ID if no message has been set.
                                      ///<  This is initialised with the default starting state from the definition.
  TransitCondition** m_conditions;    ///< Some conditions may be used by more than one StateChangeDef within the
                                      ///<  StateMachine.
  NodeID             m_smNodeID;      ///< The ID of this state machines owning Node.
                                      ///<  This is only required when trying to locate or relocate this class because we store a pointer to corresponding def data.
  AttribDataStateMachineDef* m_def;   ///< Pointer back to the definition.
  Network::StateMachineStateChangeCallback* m_onStateChangedCB; ///< Pointer to function to be called when root state
                                                                ///<  changes.  May be NULL if notifications aren't
                                                                ///<  required by the application.
};

//----------------------------------------------------------------------------------------------------------------------
// StateDef functions.
//----------------------------------------------------------------------------------------------------------------------
// Get the index into the StateMachines array of Conditions from one of this States Conditions.
NM_INLINE ConditionIndex StateDef::getExitConditionStateMachineIndex(StateConditionIndex index) const
{
  NMP_ASSERT(index < m_numExitConditions);
  NMP_ASSERT(m_exitConditionIndexes);
  return m_exitConditionIndexes[index];
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE ConditionIndex StateDef::getEntryConditionStateMachineIndex(StateConditionIndex index) const
{
  NMP_ASSERT(index < m_numEntryConditions);
  NMP_ASSERT(m_entryConditionIndexes);
  return m_entryConditionIndexes[index];
}

//----------------------------------------------------------------------------------------------------------------------
// Get one of this States possible state changes.
NM_INLINE StateID StateDef::getExitTransitionStateID(uint32_t index) const
{
  NMP_ASSERT(index < m_numExitTransitionStates);
  NMP_ASSERT(m_exitTransitionStateIDs);
  return m_exitTransitionStateIDs[index];
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataStateMachineDef functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE TransitConditionDef* AttribDataStateMachineDef::getConditionDef(ConditionIndex index) const
{
  NMP_ASSERT(index < m_numConditions && m_conditions);
  return m_conditions[index];
}

//----------------------------------------------------------------------------------------------------------------------
// Searches through array of states to find the one with this Node ID as its root.
NM_INLINE StateID AttribDataStateMachineDef::findStateIDFromNodeID(NodeID nodeID) const   // Node ID of a state within this state machine.
{
  for (StateID i = 0; i < m_numStates; ++i)
  {
    if (m_stateDefs[i].getNodeID() == nodeID)
      return i;
  }
  return INVALID_STATE_ID;
}

//----------------------------------------------------------------------------------------------------------------------
// Gets the root network ID of a state within this StateMachine.
NM_INLINE NodeID AttribDataStateMachineDef::getNodeIDFromStateID(StateID stateID) const  // Index of state within this StateMachine.
{
  NMP_ASSERT(stateID < m_numStates);
  return m_stateDefs[stateID].getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE StateDef* AttribDataStateMachineDef::getStateDef(StateID stateID) const
{
  NMP_ASSERT(stateID < m_numStates);
  return &m_stateDefs[stateID];
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataStateMachine functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AttribDataStateMachine::queueConditionDeps(
  StateID                    stateID,
  AttribDataStateMachineDef* smDef,
  Network*                   net)
{
  NMP_ASSERT(smDef);
  queueConditionDeps(smDef->getStateDef(stateID), smDef, net, smDef->getStateDef(stateID)->getNodeID());
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AttribDataStateMachine::queueGlobalConditionDeps(
  StateID                    activeStateID,
  AttribDataStateMachineDef* smDef,
  Network*                   net)
{
  queueConditionDeps(smDef->getGlobalStateDef(), smDef, net, smDef->getStateDef(activeStateID)->getNodeID());
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AttribDataStateMachine::resetStateConditions(
  StateID                    stateID,
  AttribDataStateMachineDef* smDef)
{
  resetStateConditions(smDef->getStateDef(stateID), smDef);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AttribDataStateMachine::resetGlobalConditions(
  AttribDataStateMachineDef* smDef)
{
  resetStateConditions(smDef->getGlobalStateDef(), smDef);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AttribDataStateMachine::resetStateMessageIDConditions(
  StateID                    stateID,
  AttribDataStateMachineDef* smDef)
{
  resetStateMessageIDConditions(smDef->getStateDef(stateID), smDef);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AttribDataStateMachine::resetGlobalMessageIDConditions(
  AttribDataStateMachineDef* smDef)
{
  resetStateMessageIDConditions(smDef->getGlobalStateDef(), smDef);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NodeID AttribDataStateMachine::getActiveNodeID(
  AttribDataStateMachineDef* smDef) const
{
  if (m_activeStateID == INVALID_STATE_ID)
    return INVALID_NODE_ID;  // No root node state currently set.

  NMP_ASSERT(smDef && smDef->getStateDef(m_activeStateID));
  return smDef->getStateDef(m_activeStateID)->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE StateID AttribDataStateMachine::getActiveStateID() const
{
  return m_activeStateID;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AttribDataStateMachine::queueSetStateByNodeID(
  NodeID                     nodeID, // Node ID of a state within this state machine.
  const AttribDataStateMachineDef* smDef)
{
  NMP_ASSERT(smDef);
  m_targetStateID = smDef->findStateIDFromNodeID(nodeID);
  NMP_ASSERT(m_targetStateID != INVALID_STATE_ID); // Trying to set the state machines state using a NodeID
                                                   // that doesn't correspond to a state in the state machine.
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AttribDataStateMachine::queueSetStateByStateID(
  StateID                          stateID,                   // Index into the array of possible states.
  const AttribDataStateMachineDef* NMP_USED_FOR_ASSERTS(smDef))
{
  NMP_ASSERT(stateID < smDef->getNumStates());
  m_targetStateID = stateID;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t AttribDataStateMachine::getMessageIDsFromActiveState(
  uint32_t*                  results,
  uint32_t                   maxResults,
  AttribDataStateMachineDef* smDef,
  NetworkDef*                netDef)
{
  if (m_activeStateID == INVALID_STATE_ID)
    return 0;

  return getMessageIDsFromState(m_activeStateID, results, maxResults, smDef, netDef);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AttribDataStateMachine::resetActiveStateConditions(
  AttribDataStateMachineDef* smDef)
{
  NMP_ASSERT(smDef);
  if (m_activeStateID != INVALID_STATE_ID)
    resetStateConditions(m_activeStateID, smDef);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AttribDataStateMachine::resetActiveStateMessageConditions(
  AttribDataStateMachineDef* smDef)
{
  NMP_ASSERT(smDef);
  resetGlobalMessageIDConditions(smDef);
  if (m_activeStateID != INVALID_STATE_ID)
    resetStateMessageIDConditions(m_activeStateID, smDef);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_STATE_MACHINE_H
//----------------------------------------------------------------------------------------------------------------------
