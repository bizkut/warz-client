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
#include "morpheme/Nodes/mrNodeStateMachine.h"
#include "morpheme/mrCoreTaskIDs.h"
#include "morpheme/mrNetworkDef.h"
#include "morpheme/TransitConditions/mrTransitConditionOnMessage.h"
#include "sharedDefines/mCoreDebugInterface.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// AttribDataStateMachineDef functions.
//----------------------------------------------------------------------------------------------------------------------
void AttribDataStateMachineDef::locate(AttribData* target)
{
  AttribDataStateMachineDef* result = static_cast<AttribDataStateMachineDef*>(target);

  // Locate the base attrib data class first.
  AttribData::locate(target);

  NMP::endianSwap(result->m_defaultStartingStateID);
  NMP::endianSwap(result->m_numStates);
  NMP::endianSwap(result->m_prioritiseGlobalTransitions);
  NMP::endianSwap(result->m_numConditions);

  // Conditions.
  if (result->m_numConditions > 0)
  {
    NMP::endianSwap(result->m_conditions);
    REFIX_PTR_RELATIVE(TransitConditionDef*, result->m_conditions, result);
    for (uint32_t i = 0; i < result->m_numConditions; ++i)
    {
      NMP::endianSwap(result->m_conditions[i]);
      REFIX_PTR_RELATIVE(TransitConditionDef, result->m_conditions[i], result);

      // Locate condition.
      TransitConditionDef::locate(result->m_conditions[i]);
    }
  }

  // StateDefs.
  NMP_ASSERT(result->m_numStates > 0);
  NMP::endianSwap(result->m_stateDefs);
  REFIX_PTR_RELATIVE(StateDef, result->m_stateDefs, result);
  for (uint32_t i = 0; i < result->m_numStates; ++i)
  {
    result->m_stateDefs[i].locate();
  }

  // ActiveStateDef.
  NMP::endianSwap(result->m_globalStateDef);
  REFIX_PTR_RELATIVE(StateDef, result->m_globalStateDef, result);
  result->m_globalStateDef->locate();
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataStateMachineDef::dislocate(AttribData* target)
{
  AttribDataStateMachineDef* result = static_cast<AttribDataStateMachineDef*>(target);

  // Dislocate the base attrib data class first.
  AttribData::dislocate(target);

  // Conditions.
  for (uint32_t i = 0; i < result->m_numConditions; ++i)
  {
    // Dislocate condition
    TransitConditionDef::dislocate(result->m_conditions[i]);

    UNFIX_PTR_RELATIVE(TransitConditionDef, result->m_conditions[i], result);
    NMP::endianSwap(result->m_conditions[i]);
  }
  if (result->m_numConditions > 0)
  {
    UNFIX_PTR_RELATIVE(TransitConditionDef*, result->m_conditions, result);
    NMP::endianSwap(result->m_conditions);
  }

  // StateDefs.
  for (uint32_t i = 0; i < result->m_numStates; ++i)
  {
    result->m_stateDefs[i].dislocate();
  }
  NMP_ASSERT(result->m_numStates > 0);
  UNFIX_PTR_RELATIVE(StateDef, result->m_stateDefs, result);
  NMP::endianSwap(result->m_stateDefs);
  
  // ActiveStateDef.
  result->m_globalStateDef->dislocate();
  UNFIX_PTR_RELATIVE(StateDef, result->m_globalStateDef, result);
  NMP::endianSwap(result->m_globalStateDef);

  NMP::endianSwap(result->m_defaultStartingStateID);
  NMP::endianSwap(result->m_numStates);
  NMP::endianSwap(result->m_prioritiseGlobalTransitions);
  NMP::endianSwap(result->m_numConditions);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataStateMachineDef::getMemoryRequirements()
{
  // Note: Only includes the immediate attribute data structure size.
  // This function is only used in the asset processing stage.
  NMP::Memory::Format result = NMP::Memory::Format(NMP::Memory::align(sizeof(AttribDataStateMachineDef), MR_ATTRIB_DATA_ALIGNMENT), MR_ATTRIB_DATA_ALIGNMENT);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataStateMachineDef* AttribDataStateMachineDef::init(NMP::Memory::Resource& resource, uint16_t refCount)
{
  // Note: This function is used in the asset processing stage. The majority of the initialisation happens outside
  // of this function in the StateMachineNodeBuilder so that the vast majority of this code is not included
  // where not required in the release code.
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataStateMachineDef* result = (AttribDataStateMachineDef*)resource.ptr;
  resource.increment(sizeof(AttribDataStateMachineDef));
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  result->setType(ATTRIB_TYPE_STATE_MACHINE_DEF);
  result->setRefCount(refCount);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AttribDataStateMachineDef::getMessageIDsFromNodeID(
  NodeID     nodeID,     // Is ID for the node that we want the requests for.
  MessageID* results,    // Out: Is the array to be filled in.
  uint32_t   maxResults) // Number of requests found for the specified node.
{
  // Find this Network index in the array of states.
  StateID stateID = findStateIDFromNodeID(nodeID);
  NMP_ASSERT(stateID != INVALID_STATE_ID);  // This Node ID does not exist in the available states.

  return getMessageIDsFromStateID(stateID, results, maxResults);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataStateMachineDef::getMessageIDFromCondition(
  StateConditionIndex stateCondIndex,
  StateDef*           state,
  uint32_t&           numFound,
  MessageID*          results,
  uint32_t            maxResults)
{
  uint32_t stateMachineCondIndex = state->getExitConditionStateMachineIndex(stateCondIndex);
  NMP_ASSERT(stateMachineCondIndex < m_numConditions);
  TransitConditionDef* cond = m_conditions[stateMachineCondIndex];
  NMP_ASSERT(cond);

  if (cond && cond->getType() == TRANSCOND_ON_MESSAGE_ID)
  {
    NMP_ASSERT(numFound < maxResults);
    if (numFound < maxResults) // Check to make sure we don't overrun the array length.
    {
      TransitConditionDefOnMessage* rqCond = static_cast<TransitConditionDefOnMessage*> (cond);
      results[numFound] = rqCond->getMessageID();
    }
    numFound++;
  }
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AttribDataStateMachineDef::getMessageIDsFromStateID(
  StateID    stateID,
  MessageID* results,
  uint32_t  maxResults)
{
  uint32_t numFound = 0;

  NMP_ASSERT(stateID < m_numStates);
  StateDef* state = &(m_stateDefs[stateID]);

  // Loop through all conditions on this state searching for MessageID conditions.
  for (StateConditionIndex i = 0; i < state->getNumExitConditions(); ++i)
  {
    getMessageIDFromCondition(i, state, numFound, results, maxResults);
  }
  return numFound;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AttribDataStateMachineDef::findTransitionsListeningToMessage(
  StateID   sourceStateID,
  MessageID messageID,
  StateID*  transitionStateIDs,
  uint32_t  maxResults)
{
  uint32_t numFound = 0;

  NMP_ASSERT(sourceStateID < m_numStates);
  StateDef* sourceState = &(m_stateDefs[sourceStateID]);
  
  // Loop through all the transitions on this state looking for ones that use this messageID.
  StateID   transitStateID;
  StateDef* transitState = NULL;
  for (StateID i = 0; i < sourceState->getNumExitTransitionStates(); ++i)
  {
    transitStateID = sourceState->getExitTransitionStateID(i);
    NMP_ASSERT(transitStateID < m_numStates);
    transitState = &(m_stateDefs[transitStateID]);
    
    // Loop through all the conditions on this transition searching for the messageID.
    for (StateConditionIndex j = 0; j < transitState->getNumEntryConditions(); ++j)
    {
      ConditionIndex stateMachineCondIndex = transitState->getEntryConditionStateMachineIndex(j);
      NMP_ASSERT(stateMachineCondIndex < m_numConditions);
      TransitConditionDef* cond = m_conditions[stateMachineCondIndex];
      NMP_ASSERT(cond);
      if (cond->getType() == TRANSCOND_ON_MESSAGE_ID)
      {
        TransitConditionDefOnMessage* rqCond = static_cast<TransitConditionDefOnMessage*> (cond);
        if (rqCond->getMessageID() == messageID)
        {
          // This transition is using the specified message
          NMP_ASSERT(numFound < maxResults);
          if (numFound < maxResults) // Check to make sure we don't overrun the array length.
          {
            transitionStateIDs[numFound] = transitStateID;
          }
          numFound++;
          break;
        }
      }
    }
  }

  NMP_ASSERT_MSG(numFound <= maxResults, "There are more transition state IDs than fit in the array");

  return numFound;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AttribDataStateMachineDef::getMessageIDsOnBreakoutsFromStateID(
  StateID    stateID,
  MessageID* results,
  uint32_t   maxResults)
{
  uint32_t numFound = 0;

  NMP_ASSERT(stateID < m_numStates);
  StateDef* state = &(m_stateDefs[stateID]);
  
  // Loop through all breakout conditions on this state searching for MessageID conditions.
  // Not breakout conditions appear first in the
  for (StateConditionIndex i = 0; i < state->getNumExitBreakoutConditions(); ++i)
  {
    getMessageIDFromCondition(i, state, numFound, results, maxResults);
  }
  return numFound;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AttribDataStateMachineDef::getGlobalMessageIDs(
  MessageID* results,
  uint32_t   maxResults)
{
  uint32_t numFound = 0;

  // Update the status of all breakout conditions in the group.
  for (StateConditionIndex i = 0; i < m_globalStateDef->getNumExitConditions(); ++i)
  {
    getMessageIDFromCondition(i, m_globalStateDef, numFound, results, maxResults);
  }
  return numFound;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttribDataStateMachineDef::isMessageIDValidFromStateID(
  StateID   stateID,
  MessageID messageID) const
{
  NMP_ASSERT(stateID < m_numStates);
  StateDef* state = &(m_stateDefs[stateID]);

  for (StateConditionIndex i = 0; i < state->getNumExitConditions(); i++)
  {
    ConditionIndex condIndex = state->getExitConditionStateMachineIndex(i);
    NMP_ASSERT(condIndex < m_numConditions);
    TransitConditionDef* cond = m_conditions[condIndex];
    NMP_ASSERT(cond);

    if (cond && cond->getType() == TRANSCOND_ON_MESSAGE_ID)
    {
      TransitConditionDefOnMessage* rqCond = static_cast<TransitConditionDefOnMessage*> (cond);
      if (messageID == rqCond->getMessageID())
      {
        return true;
      }
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
// StateDef functions.
//----------------------------------------------------------------------------------------------------------------------
bool StateDef::locate()
{
  NMP::endianSwap(m_numEntryConditions);
  NMP::endianSwap(m_numExitConditions);
  NMP::endianSwap(m_numExitBreakoutConditions);
  NMP::endianSwap(m_numExitTransitionStates);
  NMP::endianSwap(m_nodeID);

  // Exit transition conditions.
  if (m_numExitConditions > 0)
  {
    REFIX_SWAP_PTR(ConditionIndex, m_exitConditionIndexes);
    NMP::endianSwapArray(m_exitConditionIndexes, m_numExitConditions);
  }

  // Entry transition conditions.
  if (m_numEntryConditions > 0)
  {
    REFIX_SWAP_PTR(ConditionIndex, m_entryConditionIndexes);
    NMP::endianSwapArray(m_entryConditionIndexes, m_numEntryConditions);
  }

  // Exit state changes.
  if (m_numExitTransitionStates > 0)
  {
    REFIX_SWAP_PTR(StateID, m_exitTransitionStateIDs);
    NMP::endianSwapArray(m_exitTransitionStateIDs, m_numExitTransitionStates);
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool StateDef::dislocate()
{
  // Exit transition conditions.
  if (m_numExitConditions > 0)
  {
    NMP::endianSwapArray(m_exitConditionIndexes, m_numExitConditions);
    UNFIX_SWAP_PTR(ConditionIndex, m_exitConditionIndexes);
  }

  // Entry transition conditions.
  if (m_numEntryConditions > 0)
  {
    NMP::endianSwapArray(m_entryConditionIndexes, m_numEntryConditions);
    UNFIX_SWAP_PTR(ConditionIndex, m_entryConditionIndexes);
  }

  // Exit state changes.
  if (m_numExitTransitionStates > 0)
  {
    NMP::endianSwapArray(m_exitTransitionStateIDs, m_numExitTransitionStates);
    UNFIX_SWAP_PTR(StateID, m_exitTransitionStateIDs);
  }

  NMP::endianSwap(m_numEntryConditions);
  NMP::endianSwap(m_numExitConditions);
  NMP::endianSwap(m_numExitBreakoutConditions);
  NMP::endianSwap(m_numExitTransitionStates);
  NMP::endianSwap(m_nodeID);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataStateMachine functions.
//----------------------------------------------------------------------------------------------------------------------
void AttribDataStateMachine::locate(AttribData* target, const NetworkDef* netDef)
{
  relocate(target, netDef);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataStateMachine::relocate(AttribData* target, const NetworkDef* netDef)
{
  void* location = target;

  AttribDataStateMachine* result = (AttribDataStateMachine*)target;
  NMP_ASSERT(NMP_IS_ALIGNED(target, MR_ATTRIB_DATA_ALIGNMENT));
  NMP_ASSERT(NMP_IS_ALIGNED(location, MR_ATTRIB_DATA_ALIGNMENT));
    
  // Find the definition data that is associated with state machine.
  result->m_def = netDef->getAttribData<AttribDataStateMachineDef>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, result->m_smNodeID);
  result->m_onStateChangedCB = NULL;
    
  NMP::Memory::Format memReqs = getMemoryRequirements(result->m_def);
  NMP::Memory::Resource resultRes = {result, memReqs};
  NMP::Memory::Resource locationRes = {location, memReqs};

  // AttribDataStateMachine class.
  resultRes.increment(sizeof(AttribDataStateMachine));
  locationRes.increment(sizeof(AttribDataStateMachine));

  // Condition pointers array.
  memReqs.set(sizeof(TransitCondition*) * result->m_def->m_numConditions, NMP_NATURAL_TYPE_ALIGNMENT);
  TransitCondition** conditions = (TransitCondition**)resultRes.alignAndIncrement(memReqs);
  result->m_conditions = (TransitCondition**)locationRes.alignAndIncrement(memReqs);

  // Condition instances.
  for (uint32_t i = 0; i < result->m_def->getNumConditions(); ++i)
  {
    TransitConditionDef* conditionDef = result->m_def->m_conditions[i];

    memReqs = conditionDef->condGetMemoryRequirements();
    TransitCondition* conditions_i = (TransitCondition*)resultRes.alignAndIncrement(memReqs);
    conditions[i] = (TransitCondition*)locationRes.alignAndIncrement(memReqs);

    conditionDef->condRelocate(conditions_i, conditions[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataStateMachine::getMemoryRequirements(AttribDataStateMachineDef* stateMachineAttribDef)
{
  NMP::Memory::Format result(sizeof(AttribDataStateMachine), MR_ATTRIB_DATA_ALIGNMENT);

  // Condition pointers array.
  NMP::Memory::Format memReqs(sizeof(TransitCondition*) * stateMachineAttribDef->m_numConditions, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqs;

  // Condition instances themselves.
  for (uint32_t i = 0; i < stateMachineAttribDef->m_numConditions; ++i)
  {
    // Get condition def.
    TransitConditionDef* conditionDef = stateMachineAttribDef->m_conditions[i];
    NMP_ASSERT(conditionDef);
    result += conditionDef->condGetMemoryRequirements();
  }

  result.align();
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataStateMachine* AttribDataStateMachine::init(
  NMP::Memory::Resource&     resource,
  AttribDataStateMachineDef* stateMachineAttribDef,
  Network*                   net,
  NodeID                     smNodeID,
  uint16_t                   refCount)
{
  NMP_ASSERT(getMemoryRequirements(stateMachineAttribDef).alignment == MR_ATTRIB_DATA_ALIGNMENT);
  NMP::Memory::Format fmt(sizeof(AttribDataStateMachine), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataStateMachine* result = (AttribDataStateMachine*)resource.alignAndIncrement(fmt);

  result->setType(ATTRIB_TYPE_STATE_MACHINE);
  result->setRefCount(refCount);

  // Set the default starting state index for this state machine.
  result->m_activeStateID = INVALID_STATE_ID;
  result->m_targetStateID = stateMachineAttribDef->m_defaultStartingStateID;

  // Condition pointers array.
  fmt.set(sizeof(TransitCondition*) * stateMachineAttribDef->m_numConditions, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_conditions = (TransitCondition**)resource.alignAndIncrement(fmt);

  // Create condition instances.
  for (uint32_t i = 0; i < stateMachineAttribDef->m_numConditions; ++i)
  {
    // Get condition def.
    TransitConditionDef* conditionDef = stateMachineAttribDef->m_conditions[i];
    NMP_ASSERT(conditionDef);
    result->m_conditions[i] = conditionDef->condInit(resource, net);
  }
  
  // Second phase of condition initialisation that makes sure any data that the condition is monitoring has the correct lifespan (using post update access)
  //  Without this the required data can be deleted before the condition gets a chance to look at it.
  //  Done as a second phase because some transition (active state) can have multiple source nodes.
  NMP::Memory::Resource nullResource;
  nullResource.ptr = NULL;
  for (StateID i = 0; i < stateMachineAttribDef->m_numStates; ++i)
  {
    StateDef* sourceState = &(stateMachineAttribDef->m_stateDefs[i]);
    
    for (StateConditionIndex j = 0; j < sourceState->getNumExitConditions(); ++j)
    {
      ConditionIndex conditionIndex = sourceState->getExitConditionStateMachineIndex(j);
      NMP_ASSERT(conditionIndex < stateMachineAttribDef->getNumConditions());
      TransitConditionDef* conditionDef = stateMachineAttribDef->getConditionDef(conditionIndex);

      conditionDef->condInit(nullResource, net, sourceState->getNodeID());
    }
  }

  // Also do any necessary AttribData lifespan adjustment for global transition conditions.
  StateDef* globalState = stateMachineAttribDef->m_globalStateDef;
  for (StateConditionIndex i = 0; i < globalState->getNumExitConditions(); ++i)
  {
    ConditionIndex conditionIndex = globalState->getExitConditionStateMachineIndex(i);
    NMP_ASSERT(conditionIndex < stateMachineAttribDef->getNumConditions());
    TransitConditionDef* conditionDef = stateMachineAttribDef->getConditionDef(conditionIndex);

    for (StateID j = 0; j < stateMachineAttribDef->m_numStates; ++j)
    {
      conditionDef->condInit(nullResource, net, stateMachineAttribDef->getNodeIDFromStateID(j));
    }
  }
  
  result->m_smNodeID = smNodeID;
  result->m_def = stateMachineAttribDef;
  result->m_onStateChangedCB = NULL;
  
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttribDataStateMachine::setTransitionMessageID(
  MessageID                  messageID, // Id of messages to set the status of.
  bool                       status,    // Set this message on or off.
  AttribDataStateMachineDef* smDef,
  Network*                   net)
{
  NMP_ASSERT(smDef && net);
  if (m_activeStateID == INVALID_STATE_ID)
    return false; // No root node state currently set.

  StateDef* stateDef = smDef->getStateDef(m_activeStateID);
  StateDef* globalStateDef = smDef->getGlobalStateDef();

  bool hasBeenSet = false;
  hasBeenSet |= setTransitionMessageIDOnState(stateDef, messageID, status, smDef);
  hasBeenSet |= setTransitionMessageIDOnBreakOutTransition(stateDef, messageID, status, smDef, net);
  hasBeenSet |= setTransitionMessageIDOnState(globalStateDef, messageID, status, smDef);
  return hasBeenSet;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AttribDataStateMachine::setStateByNodeID(
  NodeID                     nodeID, // Node ID of a state within this state machine.
  AttribDataStateMachineDef* smDef,
  const NodeDef*             smNodeDef,
  Network*                   net,
  NodeConnections*           nodeConnections)
{
  // Find this Network index in the array of states.
  NMP_ASSERT(smDef && smNodeDef && net);
  StateID stateID = smDef->findStateIDFromNodeID(nodeID);
  NMP_ASSERT(stateID != INVALID_STATE_ID);  // This Node ID does not exist in the available states.
  setStateByStateID(stateID, smNodeDef, net, nodeConnections);
  return stateID;
}

//----------------------------------------------------------------------------------------------------------------------
// Wipe down any existing state and initialise and set a new root state.
NodeID AttribDataStateMachine::setStateByStateID(
  StateID          stateID,   // Index into the array of possible states.
  const NodeDef*   smNodeDef,
  Network*         net,
  NodeConnections* nodeConnections)
{
  // Get SM def.
  NMP_ASSERT(smNodeDef && net);
  NetworkDef* netDef = net->getNetworkDef();
  AttribDataHandle* handle = netDef->getAttribDataHandle(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, smNodeDef->getNodeID());
  NMP_ASSERT(handle);
  AttribDataStateMachineDef* smDef = (AttribDataStateMachineDef*)handle->m_attribData;
  NMP_ASSERT(smDef);

  // delete the previous state
  if (m_activeStateID != INVALID_STATE_ID)
  {
    net->deleteNodeInstance(smDef->getStateDef(m_activeStateID)->getNodeID());

    // Make all transition conditions in state ready for re-use
    resetActiveStateConditions(smDef);

    // Make all global transition conditions ready for re-use.
    resetGlobalConditions(smDef); 
  }

  // Initialise the new state.
  StateDef* stateDef = smDef->getStateDef(stateID);
  NMP_ASSERT(stateDef);
  nodeConnections->m_activeChildNodeIDs[0] = stateDef->getNodeID();

  // Set the parent of the new root state to be the state machine
  net->setActiveParentNodeID(nodeConnections->m_activeChildNodeIDs[0], smNodeDef->getNodeID());

  // Store the new state index.
  m_activeStateID = stateID;

  return nodeConnections->m_activeChildNodeIDs[0];
}

//----------------------------------------------------------------------------------------------------------------------
// Has a condition within the StateMachine been satisfied.
bool AttribDataStateMachine::isConditionSatisfied(
  ConditionIndex             conditionIndex,
  AttribDataStateMachineDef* NMP_USED_FOR_ASSERTS(smDef)) const // Index into StateMachines array of Conditions.
{
  NMP_ASSERT(conditionIndex < smDef->getNumConditions());
  NMP_ASSERT(m_conditions[conditionIndex]);
  return m_conditions[conditionIndex]->condIsSatisfied();
}

//----------------------------------------------------------------------------------------------------------------------
// Evaluates the conditions of this transition and determines if they have all been satisfied.
// returns: Index of destination state in StateMachine if a stateChange is satisfied otherwise INVALID_STATE_ID.
StateID AttribDataStateMachine::evaluateExitConditionsAndTestForSatisfaction(
  Network*                   net,
  StateDef*                  sourceStateDef,      // This can be a steady state node, a transition or a global state.
  NodeID                     sourceActiveNodeID,  // In the case of global state transitions this ID will not be that of the sourceStateDef.
  uint32_t                   exitTransitStateIndex,
  AttribDataStateMachineDef* smDef) const
{
  bool conditionsSatisfied = true;

  NMP_ASSERT(sourceStateDef);
  NMP_ASSERT(exitTransitStateIndex < sourceStateDef->getNumExitTransitionStates());
  StateID exitTransitStateID = sourceStateDef->getExitTransitionStateID(exitTransitStateIndex);
  StateDef* exitTransitState = smDef->getStateDef(exitTransitStateID);
  
  for (StateConditionIndex i = 0; conditionsSatisfied && (i < exitTransitState->getNumEntryConditions()); ++i)
  {
    ConditionIndex conditionIndex = exitTransitState->getEntryConditionStateMachineIndex(i);
    NMP_ASSERT(conditionIndex < smDef->getNumConditions());
    TransitConditionDef* tcDef = smDef->getConditionDef(conditionIndex);
    NMP_ASSERT(tcDef && m_conditions[conditionIndex]);

    conditionsSatisfied &= tcDef->condUpdate(m_conditions[conditionIndex], net, sourceActiveNodeID);
  }

  if (conditionsSatisfied)
    return exitTransitStateID;

  return INVALID_STATE_ID;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttribDataStateMachine::setTransitionMessageIDOnBreakOutTransition(
  StateDef*                  stateDef,  // The state to amend within this state machine.
  MessageID                  messageID, // Id of messages to set the status of.
  bool                       status,    // Set this message on or off.
  AttribDataStateMachineDef* smDef,
  Network*                   network)
{
  NMP_ASSERT(stateDef && smDef && network);
  bool retval = false;

  // Get the state's node definition
  const NodeID nodeID = stateDef->getNodeID();
  const NodeDef* const nodeDef = network->getNetworkDef()->getNodeDef(nodeID);
  NMP_ASSERT(nodeDef && (nodeDef->getNodeID() == nodeID));

  // Only consider break out transitions if the state we are considering is a transition.
  if (nodeDef->getNodeFlags() & NodeDef::NODE_FLAG_IS_TRANSITION)
  {
    // Get the transition's destination state. Note that transition could be dead blending and hence have only 1 child.
    NodeID destNodeID = network->getDestinationNodeIDFromATransition(nodeID);
    StateID destStateID = smDef->findStateIDFromNodeID(destNodeID);

    retval = setTransitionMessageIDOnState(smDef->getStateDef(destStateID), messageID, status, smDef);
  }
  return retval;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttribDataStateMachine::setTransitionMessageIDOnState(
  const StateDef* const      stateDef,  // The state to amend within this state machine.
  MessageID                  messageID, // Id of messages to set the status of.
  bool                       status,    // Set this message on or off.
  AttribDataStateMachineDef* smDef)
{
  bool retval = false;

  NMP_ASSERT(stateDef && smDef);

  for (StateConditionIndex i = 0; i < stateDef->getNumExitConditions(); ++i)
  {
    ConditionIndex conditionIndex = stateDef->getExitConditionStateMachineIndex(i);
    TransitConditionDef* conditionDef = smDef->getConditionDef(conditionIndex);
    NMP_ASSERT(conditionDef);
    if (conditionDef->getType() == TRANSCOND_ON_MESSAGE_ID)
    {
      TransitConditionDefOnMessage* messageConditionDef = static_cast<TransitConditionDefOnMessage*> (conditionDef);
      if (messageID == messageConditionDef->getMessageID())
      {
        NMP_ASSERT(m_conditions[conditionIndex] && m_conditions[conditionIndex]->condGetType() == TRANSCOND_ON_MESSAGE_ID);
        TransitConditionOnRequest* messageCondition = static_cast<TransitConditionOnRequest*> (m_conditions[conditionIndex]);
        TransitConditionDefOnMessage::instanceSetState(messageCondition, conditionDef, status);
        // We have at least one requested transition in this node.
        retval = true;
      }
    }
  }

  return retval;
}

//----------------------------------------------------------------------------------------------------------------------
namespace
{

//----------------------------------------------------------------------------------------------------------------------
// For the given already active testNodeID, find if a transition in its parent hierarchy can start dead blending so that
//  we can activate a new instance of the testNodeID.
// 
// 1.     parent(transit,canDeadBlend)
//        |                   |
//    testNode(source)       (dest)
//
// 2a.             grandParent(transit,canDeadBlend)
//                 |          |
//         parent(transit)   (dest)
//         |             |
//    (source)       testNode(dest)
//
// 2b.             grandParent(transit,canDeadBlend)
//                 |                    |
//     parent(transit,deadBlending)    (dest)
//                |
//            testNode
//
// 3a.       grandParent(stateMachine)
//                 |
//         parent(transit)
//         |             |
//    (source)        testNode(dest)
//
// 3b.       grandParent(stateMachine)
//                 |
//        parent(transit,deadBlending)
//                 |
//             testNode
//
// 4.          parent(stateMachine)
//                 |
//             testNode(transit)
//                 |
//        1 unspecified active child
//
NodeID* parentTransitionCanStartDeadBlending(
  const NodeID            testNodeID,
  const NetworkDef* const networkDef,
  Network*                network)
{
  // node is currently active, check for dead blend breakout back to active state
  NodeID parentNodeID = network->getActiveParentNodeID(testNodeID);
  const NodeDef* parentNodeDef = networkDef->getNodeDef(parentNodeID);
  NodeDef::NodeFlags parentNodeFlags = parentNodeDef->getNodeFlags();

  if (parentNodeFlags & NodeDef::NODE_FLAG_IS_TRANSITION)
  {
    // Parent of testNode is a transition.
    NodeConnections* parentsConnections = network->getActiveNodesConnections(parentNodeID);
    if ((parentsConnections->m_numActiveChildNodes == 2) &&
        (parentsConnections->m_activeChildNodeIDs[0] == testNodeID))
    {
      if (parentNodeFlags & NodeDef::NODE_FLAG_IS_TRANSITION_CAN_DEAD_BLEND)
      {
        // (1.) testNode is the source of an active dead blendable transition that has 2 children.
        return &parentsConnections->m_activeChildNodeIDs[0];
      }
    }
    else
    {
      // testNode node is the destination of an active transition.
      // Check the transitions parent transition.
      NodeID grandParentNodeID = parentsConnections->m_activeParentNodeID;
      NMP_ASSERT(grandParentNodeID != INVALID_NODE_ID);
      const NodeDef* grandParentNodeDef = networkDef->getNodeDef(grandParentNodeID);
      NodeConnections* grandParentsConnections = network->getActiveNodesConnections(grandParentNodeID);

      if (grandParentsConnections->m_activeChildNodeIDs[0] == parentNodeID)
      {
        // testNodes parent transition is the source of an active transition
        NodeDef::NodeFlags grandParentNodeFlags = grandParentNodeDef->getNodeFlags();
        if ((grandParentNodeFlags & NodeDef::NODE_FLAG_IS_TRANSITION_CAN_DEAD_BLEND) || // it is dead blendable.
            (grandParentNodeFlags & NodeDef::NODE_FLAG_IS_STATE_MACHINE))               // Breakout self-transition
        {
          // (2a.), (2b.), (3a), (3b)
          return &grandParentsConnections->m_activeChildNodeIDs[0];
        }
      }
    }
  }
  else if (parentNodeFlags & NodeDef::NODE_FLAG_IS_STATE_MACHINE)
  {
    const NodeDef* testNodeDef = networkDef->getNodeDef(testNodeID);
    NodeDef::NodeFlags testNodeFlags = testNodeDef->getNodeFlags();

    if (testNodeFlags & NodeDef::NODE_FLAG_IS_TRANSITION)
    {
      NodeConnections* testNodeConnections = network->getActiveNodesConnections(testNodeID);
      bool deadblendTransitionSource = (testNodeConnections->m_numActiveChildNodes == 2) &&
                                       (testNodeConnections->m_activeChildNodeIDs[0] == testNodeID);
      if (!deadblendTransitionSource)
      {
        NodeConnections* parentsConnections = network->getActiveNodesConnections(parentNodeID);
        if (parentsConnections->m_activeChildNodeIDs[0] == testNodeID)
        {
          // (4.)
          return &parentsConnections->m_activeChildNodeIDs[0]; // Breakout self-transition
        }
      }
    }
  }

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
// Update the status of all conditions and state changes in the global state.
StateID AttribDataStateMachine::updateGlobalState(
  AttribDataStateMachineDef* smDef,
  Network*                   net,
  bool&                      breakoutGlobalSelfTransition)
{
  StateDef* globalStateDef = smDef->getGlobalStateDef();
  StateDef* activeStateDef = smDef->getStateDef(m_activeStateID);
  NMP_ASSERT(globalStateDef && activeStateDef);
  NetworkDef* networkDef = net->getNetworkDef();
  NMP_ASSERT(activeStateDef->getNodeID() != INVALID_NODE_ID);
  NodeDef* activeStateNodeDef = networkDef->getNodeDef(activeStateDef->getNodeID());
  
  // Find the first set (if any) whose conditions are all satisfied.
  for (uint32_t i = 0; i < globalStateDef->getNumExitTransitionStates(); ++i)
  {
    StateID destinationStateID = evaluateExitConditionsAndTestForSatisfaction(net, globalStateDef, activeStateDef->getNodeID(), i, smDef);

    if (destinationStateID != INVALID_STATE_ID)
    {
      const NodeID destStateNodeID = smDef->getNodeIDFromStateID(destinationStateID);
      NodeDef* destStateNodeDef = networkDef->getNodeDef(destStateNodeID);

      // Check if transition from a transition
      if (activeStateNodeDef &&
          (destStateNodeDef->getNodeFlags() & NodeDef::NODE_FLAG_IS_TRANSITION) &&
          (activeStateNodeDef->getNodeFlags() & NodeDef::NODE_FLAG_IS_TRANSITION))
      {
        const NodeID nodeID = activeStateNodeDef->getNodeID();

        NodeBin* nodeBin = net->getNodeBin(nodeID);
        NodeBin* destNodeBin = net->getNodeBin(destStateNodeID);
        if (destNodeBin->getLastFrameUpdate() == nodeBin->getLastFrameUpdate()) 
        {
          // Destination transition node is already active. See if we can dead blend a parent transition.
          NodeID* deadBlendNodePtr =
            parentTransitionCanStartDeadBlending(destStateNodeID, networkDef, net);
          if (deadBlendNodePtr)
          {
            NodeID deadBlendNode = *deadBlendNodePtr;
            NodeID activeParentNodeID = net->getActiveParentNodeID(deadBlendNode);

            if (networkDef->getNodeDef(activeParentNodeID)->getNodeFlags() & NodeDef::NODE_FLAG_IS_TRANSITION)
            {
              // Destination is target of transition, notify of dead blend initiation
              // Set dead blending transit source to be target of child transition
              NodeConnections* deadBlendNodeConnections = net->getActiveNodesConnections(activeParentNodeID);
              *deadBlendNodePtr =
                deadBlendNodeConnections->m_activeChildNodeIDs[deadBlendNodeConnections->m_numActiveChildNodes-1];
              deadBlendNodeConnections->m_activeChildNodeIDs[0] = *deadBlendNodePtr;

              // Remove reference to the destination node
              deadBlendNodeConnections->m_numActiveChildNodes--;
            }

            // Detect breakout self-transition
            if (nodeID == deadBlendNode)
            {
              breakoutGlobalSelfTransition = true;
            }
  
            // Delete currently referenced node instance
            net->deleteNodeInstance(deadBlendNode);
                                   
            // Dead blend active state
            resetGlobalConditions(smDef);
            return destinationStateID;
          }
        }
        else if (net->getNodeBin(destStateNodeDef->getChildNodeID(1))->getLastFrameUpdate()  == nodeBin->getLastFrameUpdate())
        {
          // Destination of the destination transition is already active. See if we can dead blend its current parent transition.
          // Check if active state dead blend is permitted
          NodeID* deadBlendNodePtr =
            parentTransitionCanStartDeadBlending(destStateNodeDef->getChildNodeID(1), networkDef, net);

          if (deadBlendNodePtr)
          {
            NodeID deadBlendNode = *deadBlendNodePtr;
            NodeID activeParentNodeID = net->getActiveParentNodeID(deadBlendNode);

            if (networkDef->getNodeDef(activeParentNodeID)->getNodeFlags() & NodeDef::NODE_FLAG_IS_TRANSITION)
            {
              // Destination is target of transition, notify of dead blend initiation
              // Set dead blending transit source to be target of child transition
              NodeConnections* deadBlendNodeConnections = net->getActiveNodesConnections(activeParentNodeID);
              *deadBlendNodePtr =
                deadBlendNodeConnections->m_activeChildNodeIDs[deadBlendNodeConnections->m_numActiveChildNodes-1];
              deadBlendNodeConnections->m_activeChildNodeIDs[0] = *deadBlendNodePtr;

              // Remove reference to the destination node
              deadBlendNodeConnections->m_numActiveChildNodes--;
            }

            // Detect breakout self-transition
            if (nodeID == deadBlendNode)
            {
              breakoutGlobalSelfTransition = true;
            }

            // Delete currently referenced node instance
            net->deleteNodeInstance(deadBlendNode);

            // Dead blend active state
            resetGlobalConditions(smDef);
            return destinationStateID;
          }

        }
        else
        {
          // Both the destination transition and its destination are not currently active so its safe to go ahead with the transition.
          resetGlobalConditions(smDef);
          return destinationStateID;
        }       
      }
      else
      {
        // Not a transition from transition
        resetGlobalConditions(smDef);
        return destinationStateID;
      }
    }
  }

  return INVALID_STATE_ID;
}

//----------------------------------------------------------------------------------------------------------------------
StateID AttribDataStateMachine::updateActiveState(
  AttribDataStateMachineDef* smDef,
  Network*                   net,
  bool&                      modifyTransition,
  bool&                      breakoutSelfTransition)
{
  modifyTransition = false;
  breakoutSelfTransition = false;
  
  NMP_ASSERT(m_activeStateID != GLOBAL_STATE_ID);
  StateDef* activeStateDef = smDef->getStateDef(m_activeStateID);
  NMP_ASSERT(activeStateDef);
  NetworkDef* netDef = net->getNetworkDef();
  NMP_ASSERT(activeStateDef->getNodeID() != INVALID_NODE_ID);
  NodeDef* activeStateNodeDef = netDef->getNodeDef(activeStateDef->getNodeID());
  

  //------------------------
  // Evaluate the conditions of each transition and find the first (if any) whose conditions are all satisfied.
  for (uint32_t i = 0; i < activeStateDef->getNumExitTransitionStates(); ++i)
  {
    StateID destinationStateID = evaluateExitConditionsAndTestForSatisfaction(net, activeStateDef, activeStateDef->getNodeID(), i, smDef);

    if (destinationStateID != INVALID_STATE_ID)
    {
      const NodeID destStateNodeID = smDef->getNodeIDFromStateID(destinationStateID);
      NodeDef* destStateNodeDef = netDef->getNodeDef(destStateNodeID);

      // Check if transition from a transition
      if (activeStateNodeDef &&
          (destStateNodeDef->getNodeFlags() & NodeDef::NODE_FLAG_IS_TRANSITION) &&
          (activeStateNodeDef->getNodeFlags() & NodeDef::NODE_FLAG_IS_TRANSITION))
      {
        // Allow transition only if the destination node is not already active
        NodeBin* destChild1NodeBin = net->getNodeBin(destStateNodeDef->getChildNodeID(1));
        NodeBin* activeNodeBin = net->getNodeBin(activeStateNodeDef->getNodeID());
        if (destChild1NodeBin->getLastFrameUpdate() !=
            activeNodeBin->getLastFrameUpdate())
        {
          // Destination state / node is not currently active
          return destinationStateID;
        }
        else
        {
          // Check if active state dead blend is permitted
          NodeID* deadBlendNodePtr =
            parentTransitionCanStartDeadBlending(destStateNodeDef->getChildNodeID(1), netDef, net);

          if (deadBlendNodePtr)
          {
            NodeID deadBlendNode = *deadBlendNodePtr;
            NodeID activeParentNodeID = net->getActiveParentNodeID(deadBlendNode);
            NodeID requestRootNodeID = activeStateNodeDef->getNodeID();

            if (netDef->getNodeDef(activeParentNodeID)->getNodeFlags() & NodeDef::NODE_FLAG_IS_TRANSITION)
            {
              // Destination is target of transition, notify of dead blend initiation
              // Set dead blending transit source to be target of child transition
              NodeConnections* deadBlendNodeConnections = net->getActiveNodesConnections(activeParentNodeID);
              *deadBlendNodePtr =
                deadBlendNodeConnections->m_activeChildNodeIDs[deadBlendNodeConnections->m_numActiveChildNodes-1];
              deadBlendNodeConnections->m_activeChildNodeIDs[0] = *deadBlendNodePtr;

              // Remove reference to the destination node
              deadBlendNodeConnections->m_numActiveChildNodes--;
            }

            // Detect breakout self-transition
            if (requestRootNodeID == deadBlendNode)
            {
              requestRootNodeID = activeParentNodeID;
              breakoutSelfTransition = true;
            }

            // Delete currently referenced node instance
            net->deleteNodeInstance(deadBlendNode);

            // Dead blend active state
            return destinationStateID;
          }
        }
      }
      else
      {
        // Not a transition from transition
        return destinationStateID;
      }
    }
  }

  //------------------------
  // If the current state is a transition then we need to update any breakout transition conditions on our destination.
  return updateStateForBreakoutTransits(activeStateDef, smDef, net, modifyTransition, breakoutSelfTransition);
}

//----------------------------------------------------------------------------------------------------------------------
// Update the status of conditions and state changes in this state's destination node's transitions
StateID AttribDataStateMachine::updateStateForBreakoutTransits(
  StateDef*                  stateDef,
  AttribDataStateMachineDef* smDef,
  Network*                   net,
  bool&                      modifyTransition,
  bool&                      breakoutSelfTransition)
{
  // Get the node definition
  NetworkDef* netDef = net->getNetworkDef();
  const NodeID nodeID = stateDef->getNodeID();
  const NodeDef* const nodeDef = netDef->getNodeDef(nodeID);
  NMP_ASSERT(nodeDef && (nodeDef->getNodeID() == nodeID));

  if (!nodeDef->getNodeFlags().isSet(NodeDef::NODE_FLAG_IS_TRANSITION))
  {
    // The active root state is not a transition so there cant be a break out.
    return INVALID_STATE_ID;
  }

  // Get the transition's destination state. Note that transition could be dead blending and hence have only 1 child.
  uint32_t numActiveChildren = net->getNumActiveChildren(nodeID);
  NMP_ASSERT((numActiveChildren == 1) || (numActiveChildren == 2));
  NodeID destNodeID = net->getActiveChildNodeID(nodeID, numActiveChildren - 1);
  const StateID destStateID = smDef->findStateIDFromNodeID(destNodeID);
  StateDef* const destStateDef = smDef->getStateDef(destStateID);
  NMP_ASSERT(destStateDef);

  if (destStateDef->getNumExitBreakoutConditions() == 0)
  {
    // There are no breakout conditions on the destination so there are no breakout transitions.
    return INVALID_STATE_ID;
  }

  // Iterate through the transitions from the destination evaluate their conditions and find the find the first (if any) whose conditions are all satisfied.
  for (uint32_t i = 0; i < destStateDef->getNumExitTransitionStates(); ++i)
  {
    // First, look further down the chain to catch problem configurations

    // From the destination state, get the destination node
    const StateID destNodesTransitionStateID = destStateDef->getExitTransitionStateID(i);
    const NodeID destNodesTransitionID = smDef->getNodeIDFromStateID(destNodesTransitionStateID);   
    const NodeDef* const destNodesTransitionDef = netDef->getNodeDef(destNodesTransitionID);
    NMP_ASSERT(destNodesTransitionDef);

    if (!destNodesTransitionDef->getNodeFlags().isSet(NodeDef::NODE_FLAG_IS_BREAKOUT_TRANSITION))
    {
      // Only consider breakout transitions.
      continue;
    }

    // Do not allow breakout transitions from a transition that does not output events to one that does.
    if ((destNodesTransitionDef->getNodeTypeID() == NODE_TYPE_TRANSIT_SYNC_EVENTS || destNodesTransitionDef->getNodeTypeID() == NODE_TYPE_TRANSIT_SYNC_EVENTS_PHYSICS) &&
        (nodeDef->getNodeTypeID() == NODE_TYPE_TRANSIT || nodeDef->getNodeTypeID() == NODE_TYPE_TRANSIT_PHYSICS))
    {
      continue;
    }

    const StateID destinationStateID = evaluateExitConditionsAndTestForSatisfaction(net, destStateDef, destStateDef->getNodeID(), i, smDef);

    if (destinationStateID != INVALID_STATE_ID)
    {
      // Get the destination node of this transition
      const NodeID destNodesTransitionsDestNode = destNodesTransitionDef->getChildNodeID(1);

      NodeBin* nodeBin = net->getNodeBin(nodeID);
      NodeBin* destTransitionNodeBin = net->getNodeBin(destNodesTransitionID);
      if (destTransitionNodeBin->getLastFrameUpdate() == nodeBin->getLastFrameUpdate())
      {
        // Destination transition node is already active. See if we can dead blend a parent transition.
        NodeID* deadBlendNodePtr =
          parentTransitionCanStartDeadBlending(destNodesTransitionID, netDef, net);

        if (deadBlendNodePtr)
        {
          NodeID deadBlendNode = *deadBlendNodePtr;
          NodeID activeParentNodeID = net->getActiveParentNodeID(deadBlendNode);

          if (netDef->getNodeDef(activeParentNodeID)->getNodeFlags() & NodeDef::NODE_FLAG_IS_TRANSITION)
          {
            // Destination is target of transition, notify of dead blend initiation
            // Set dead blending transit source to be target of child transition
            NodeConnections* deadBlendNodeConnections = net->getActiveNodesConnections(activeParentNodeID);
            *deadBlendNodePtr =
              deadBlendNodeConnections->m_activeChildNodeIDs[deadBlendNodeConnections->m_numActiveChildNodes-1];
            deadBlendNodeConnections->m_activeChildNodeIDs[0] = *deadBlendNodePtr;

            // Remove reference to the destination node
            deadBlendNodeConnections->m_numActiveChildNodes--;  
          }

          // Detect breakout self-transition
          if (nodeID == deadBlendNode)
          {
            breakoutSelfTransition = true;
          }

          // Delete currently referenced node instance
          net->deleteNodeInstance(deadBlendNode);
        }
        else
          // Dead blend not valid
          continue;
      }
      else if (net->getNodeBin(destNodesTransitionsDestNode)->getLastFrameUpdate() == nodeBin->getLastFrameUpdate())
      {
        // The destination state is currently active, check its parent transition can dead blend.
        NodeID* deadBlendNodePtr =
          parentTransitionCanStartDeadBlending(destNodesTransitionsDestNode, netDef, net);

        if (deadBlendNodePtr)
        {
          NodeID deadBlendNode = *deadBlendNodePtr;
          NodeID activeParentNodeID = net->getActiveParentNodeID(deadBlendNode);

          if (netDef->getNodeDef(activeParentNodeID)->getNodeFlags() & NodeDef::NODE_FLAG_IS_TRANSITION)
          {
            // Destination is target of transition, notify of dead blend initiation
            // Set dead blending transit source to be target of child transition
            NodeConnections* deadBlendNodeConnections = net->getActiveNodesConnections(activeParentNodeID);

            // Destination could be child 1 or 0 if transition is dead blending
            *deadBlendNodePtr =
              deadBlendNodeConnections->m_activeChildNodeIDs[deadBlendNodeConnections->m_numActiveChildNodes-1];
            deadBlendNodeConnections->m_activeChildNodeIDs[0] = *deadBlendNodePtr;

            // Remove reference to the destination node
            deadBlendNodeConnections->m_numActiveChildNodes--;
          }

          // Detect breakout self-transition
          if (nodeID == deadBlendNode)
          {
            breakoutSelfTransition = true;
          }

          // Delete currently referenced node instance
          net->deleteNodeInstance(deadBlendNode);
        }
        else
          // Dead blend not valid
          continue;
      }

      modifyTransition = true;
      return destinationStateID;
    }
  }

  return INVALID_STATE_ID;
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataStateMachine::resetConditionsForBreakoutTransitions(
  StateID                    stateID,
  AttribDataStateMachineDef* smDef,
  Network*                   net)
{
  // Get the node definition
  StateDef* stateDef = smDef->getStateDef(stateID);
  const NodeID nodeID = stateDef->getNodeID();
  const NetworkDef* const netDef = net->getNetworkDef();
  const NodeDef* const nodeDef = netDef->getNodeDef(nodeID);
  NMP_ASSERT(nodeDef && (nodeDef->getNodeID() == nodeID));

  // Is it a transition node?
  if (nodeDef->getNodeFlags().isSet(NodeDef::NODE_FLAG_IS_TRANSITION))
  {
    // Get the transition's destination state. Note that transition could be dead blending and hence have only 1 child.
    uint32_t numActiveChildren = net->getNumActiveChildren(nodeID);
    NMP_ASSERT((numActiveChildren == 1) || (numActiveChildren == 2));
    NodeID destNodeID = net->getActiveChildNodeID(nodeID, numActiveChildren - 1);
    StateID destStateID = smDef->findStateIDFromNodeID(destNodeID);
    resetStateConditions(destStateID, smDef);
  }
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AttribDataStateMachine::getMessageIDsFromState(
  StateID                    stateID,
  uint32_t*                  results,
  uint32_t                   maxResults,
  AttribDataStateMachineDef* smDef,
  NetworkDef*                netDef)
{
  NMP_ASSERT(stateID != INVALID_STATE_ID);

  //------------
  // Find the MessageIDs from the current state.
  uint32_t numMessageIDs = smDef->getMessageIDsFromStateID(stateID, results, maxResults);
  
  // Get the node definition of the state.
  const NodeID nodeID = smDef->getStateDef(stateID)->getNodeID();
  const NodeDef* const nodeDef = netDef->getNodeDef(nodeID);
  NMP_ASSERT(nodeDef && (nodeDef->getNodeID() == nodeID));

  //------------
  // If the state is a transition find the MessageIDs on any break out transitions on the destination.
  if (nodeDef->getNodeFlags().isSet(NodeDef::NODE_FLAG_IS_TRANSITION))
  {
    // Get the transition's destination state.
    uint32_t numChildren = nodeDef->getNumChildNodes();
    NMP_ASSERT(numChildren > 0);
    NodeID destNodeID = nodeDef->getChildNodeID(numChildren - 1);
    StateID destStateID = smDef->findStateIDFromNodeID(destNodeID);

    // Find any messages on breakouts.
    numMessageIDs += smDef->getMessageIDsOnBreakoutsFromStateID(destStateID, &(results[numMessageIDs]), (maxResults - numMessageIDs));
  }

  //------------
  // Find the MessageIDs from the global state.
  numMessageIDs += smDef->getGlobalMessageIDs(&(results[numMessageIDs]), (maxResults - numMessageIDs));

  return numMessageIDs;
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataStateMachine::queueConditionDeps(
  StateDef*                  stateDef,
  AttribDataStateMachineDef* smDef,
  Network*                   net,
  NodeID                     smActiveNodeID)
{
  NMP_ASSERT(stateDef);

  // Update the status of all conditions in the group.
  for (StateConditionIndex i = 0; i < stateDef->getNumExitConditions(); ++i)
  {
    ConditionIndex stateConditionIndex = stateDef->getExitConditionStateMachineIndex(i);
    TransitConditionDef* tcDef = smDef->getConditionDef(stateConditionIndex);
    NMP_ASSERT(tcDef && m_conditions[stateConditionIndex]);
    tcDef->condQueueDeps(m_conditions[stateConditionIndex], net, smActiveNodeID);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataStateMachine::resetStateConditions(StateDef* stateDef, AttribDataStateMachineDef* smDef)
{
  NMP_ASSERT(stateDef);
  // Update the status of all conditions in the group.
  for (StateConditionIndex i = 0; i < stateDef->getNumExitConditions(); ++i)
  {
    ConditionIndex stateConditionIndex = stateDef->getExitConditionStateMachineIndex(i);
    TransitConditionDef* tcDef = smDef->getConditionDef(stateConditionIndex);
    NMP_ASSERT(tcDef && m_conditions[stateConditionIndex]);
    tcDef->condReset(m_conditions[stateConditionIndex]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataStateMachine::resetStateMessageIDConditions(
  StateDef*                  stateDef,
  AttribDataStateMachineDef* smDef)
{
  NMP_ASSERT(stateDef);

  // Update the status of all conditions in the group.
  for (StateConditionIndex i = 0; i < stateDef->getNumExitConditions(); ++i)
  {
    ConditionIndex stateConditionIndex = stateDef->getExitConditionStateMachineIndex(i);
    TransitConditionDef* tcDef = smDef->getConditionDef(stateConditionIndex);
    NMP_ASSERT(tcDef && m_conditions[stateConditionIndex]);

    if (tcDef->getType() == TRANSCOND_ON_MESSAGE_ID)
    {
      tcDef->condReset(m_conditions[stateConditionIndex]);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataStateMachine::completeActiveZeroLengthTransition(
   AttribDataStateMachineDef* smDef,
   NodeConnections*           nodeConnections,
   Network*                   net,
   NodeID                     smNodeID)
{
  // If the active state is a zero length transition that was set up last frame we must complete the transition here.
  NodeID childNodeID = nodeConnections->m_activeChildNodeIDs[0];
  NodeDef* childNodeDef = net->getNetworkDef()->getNodeDef(childNodeID);
  if (childNodeDef->getNodeFlags().isSet(NodeDef::NODE_FLAG_IS_TRANSITION_ZERO))
  {
      // Nodes with this flag set are transitions with a transition time 
      // of 0.0f seconds.

    NodeConnections* childNodeConnections = net->getActiveNodesConnections(childNodeID);
    if (childNodeConnections->m_numActiveChildNodes == 2)
    {
      // Active state is the destination.
      m_activeStateID = smDef->findStateIDFromNodeID(childNodeConnections->m_activeChildNodeIDs[1]);
        // Make the state machines child the zero time transitions destination.
      nodeConnections->m_activeChildNodeIDs[0] = childNodeConnections->m_activeChildNodeIDs[1];
        // Make the zero time transitions destinations parent the state machine
      net->getActiveNodesConnections(childNodeConnections->m_activeChildNodeIDs[1])->m_activeParentNodeID = smNodeID;
      if (childNodeConnections->m_activeChildNodeIDs[0] != childNodeConnections->m_activeChildNodeIDs[1])
      {
          // Unless this transitions source and destination are the same we no longer need the source
        net->deleteNodeInstance(childNodeConnections->m_activeChildNodeIDs[0]);
      }

        // cleanNodeData deletes the attrib data and refresh the activeConnections for this node.
      net->cleanNodeData(childNodeID);
    }
    else
    {
      // There is only 1 child of the transition (deadblending), set the only child to be the active state of the state machine and delete the transition.
      NMP_ASSERT(childNodeConnections->m_numActiveChildNodes == 1);
      m_activeStateID = smDef->findStateIDFromNodeID(childNodeConnections->m_activeChildNodeIDs[0]);
      nodeConnections->m_activeChildNodeIDs[0] = childNodeConnections->m_activeChildNodeIDs[0];
      net->getActiveNodesConnections(childNodeConnections->m_activeChildNodeIDs[0])->m_activeParentNodeID = smNodeID;
      net->cleanNodeData(childNodeID);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataStateMachine::updateConnections(
  const NodeDef* smNodeDef,
  Network*       net)
{
  NMP_ASSERT(smNodeDef && net);

  AttribDataStateMachineDef* smDef = smNodeDef->getAttribData<AttribDataStateMachineDef>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  NMP_ASSERT(smDef);

  //------------------------
  // Get this state machine nodes connections structure.
  NodeID smNodeID = smNodeDef->getNodeID();
  NodeConnections* nodeConnections = net->getActiveNodesConnections(smNodeID);
  NMP_ASSERT(nodeConnections && (nodeConnections->m_numActiveChildNodes == 1));
    
  //------------------------
  // If the active state is a zero length transition that was set up last frame we must complete the transition here.
  completeActiveZeroLengthTransition(smDef, nodeConnections, net, smNodeID);
  
  //------------------------
  if (m_targetStateID != INVALID_STATE_ID)
  {
    // We have been asked to set a new active state so deal with it here.
    setStateByStateID(m_targetStateID, smNodeDef, net, nodeConnections);
    m_targetStateID = INVALID_STATE_ID;
  }

  //------------------------
  // Update all conditions in the current group and find the first transition set, if any,
  // who's conditions are all satisfied.
  StateID newStateID = INVALID_STATE_ID;
  bool modifyTransition = false;
  bool breakoutSelfTransition = false;
  NodeID activeNodeID = smDef->getNodeIDFromStateID(m_activeStateID);
  
  // Prioritize global transitions if a global transition has been set with a
  // higher priority than state transitions
  if (smDef->m_prioritiseGlobalTransitions)
  {
    //------------------------
    // Update conditions common to any active state and find the first transition set, if any,
    // who's conditions are all satisfied.
    newStateID = updateGlobalState(smDef, net, breakoutSelfTransition);

    //------------------------
    // Update all conditions in the current group and find the first transition set, if any,
    // who's conditions are all satisfied.
    if (newStateID == INVALID_STATE_ID)
    {
      newStateID = updateActiveState(smDef, net, modifyTransition, breakoutSelfTransition);
    }
  }
  else
  {
    //------------------------
    // Update all conditions in the current group and find the first transition set, if any,
    // who's conditions are all satisfied.
    newStateID = updateActiveState(smDef, net, modifyTransition, breakoutSelfTransition);

    //------------------------
    // Update conditions common to any active state and find the first transition set, if any,
    // who's conditions are all satisfied.
    if (newStateID == INVALID_STATE_ID)
    {
      newStateID = updateGlobalState(smDef, net, breakoutSelfTransition);
    }
  }

  //------------------------
  if (newStateID != INVALID_STATE_ID)
  {
    // Reaches here when a transition becomes active.
    // The if statement below suggests that this is not the only way
    // this code can be reached.

    // Worth pointing out that when a transition finishes the code does NOT arrive here.

    // Make sure our transition conditions in the source are made ready for re-use.
    resetStateConditions(m_activeStateID, smDef);

    // If the active state is a transition make sure any conditions on any breakouts from its destination are made ready for re-use.
    resetConditionsForBreakoutTransitions(m_activeStateID, smDef, net);

    // If the destination is a transition start it up.
    NodeID transitNodeID = smDef->getNodeIDFromStateID(newStateID);
    NodeDef* transitNodeDef = net->getNetworkDef()->getNodeDef(transitNodeID);
    
    if (transitNodeDef->getNodeFlags() & NodeDef::NODE_FLAG_IS_TRANSITION)
    {
      if(m_onStateChangedCB)
      {
        m_onStateChangedCB(smNodeID, transitNodeID);
      }

      NodeID transitDefSourceID = transitNodeDef->getChildNodeID(0);
      NodeID transitDefDestID   = transitNodeDef->getChildNodeID(1);

      if (transitDefSourceID == INVALID_NODE_ID || transitDefDestID == INVALID_NODE_ID)
      {
        // An active state transit occurred. Patch in this transition.

        // set the transit node's active child count
        if (net->getNumActiveChildren(transitNodeID) < 2)
          net->setNumActiveChildren(transitNodeID, 2);

        // set the transit node's first (source) active child
        if (transitDefSourceID == INVALID_NODE_ID)
          net->setActiveChildNodeID(transitNodeID, 0, breakoutSelfTransition ? transitDefDestID : activeNodeID);
        else
          net->setActiveChildNodeID(transitNodeID, 0, transitDefSourceID);
          

        // set the transit node's second (destination) active child
        NMP_ASSERT(transitDefDestID != INVALID_NODE_ID);
        net->setActiveChildNodeID(transitNodeID, 1, transitDefDestID);

        // fix up the active node's parent to be the transit node
        net->setActiveParentNodeID(activeNodeID, transitNodeID);
      }
      else if (modifyTransition)
      {
        // A breakout transit has occurred. Patch in this transition.

        // Reset conditions (Including messages) in breakout transition state.
        uint32_t numActiveChildren = net->getNumActiveChildren(activeNodeID);
        NMP_ASSERT((numActiveChildren == 1) || (numActiveChildren == 2));
        NodeID destNodeID = net->getActiveChildNodeID(activeNodeID, numActiveChildren - 1);
        StateID breakoutTransitStateID = smDef->findStateIDFromNodeID(destNodeID);
        resetStateConditions(breakoutTransitStateID, smDef);

        // Is self-transition?
        if (transitDefSourceID == transitDefDestID)
        {
          // Self-transition
          // Set the parent of the source and destination node to be the transition node.
          NodeID sourceNodeID = net->getActiveChildNodeID(transitNodeDef->getNodeID(), 0);
          net->setActiveParentNodeID(sourceNodeID, transitNodeID);
        }
        else
        {
          // Patch in this transition.
          // set the breakout transit node's active child to be the current active state's node
          net->setActiveChildNodeID(transitNodeID, 0, activeNodeID);

          // similarly, fix up the active node's parent to be the transit node
          net->setActiveParentNodeID(activeNodeID, transitNodeID);
        }
      }
      else
      {
        // Set the parent of the source and destination node to be the transition node.
        NodeID sourceNodeID = net->getActiveChildNodeID(transitNodeDef->getNodeID(), 0);
        NMP_ASSERT(sourceNodeID == smDef->getNodeIDFromStateID(m_activeStateID));
        net->setActiveParentNodeID(sourceNodeID, transitNodeID);
      }

      NodeID destNodeID = net->getActiveChildNodeID(transitNodeDef->getNodeID(), 1);
      net->setActiveParentNodeID(destNodeID, transitNodeID);
    }

    // Set the new root state.
    m_activeStateID = newStateID;
    NMP_ASSERT(m_activeStateID < smDef->getNumStates());

    // Set the parent of the new root state to be the state machine
    nodeConnections->m_activeChildNodeIDs[0] = smDef->getNodeIDFromStateID(m_activeStateID);
    net->setActiveParentNodeID(nodeConnections->m_activeChildNodeIDs[0], smNodeID);
  }

  //------------------------
  // Recurse connections to children.
  AnimSetIndex animSet = net->getOutputAnimSetIndex(smNodeID);
  NodeID rootNodeID = net->updateNodeInstanceConnections(nodeConnections->m_activeChildNodeIDs[0], animSet);
  if (rootNodeID != nodeConnections->m_activeChildNodeIDs[0])
  {
    // When a transition is active (ie. the root of the state machine) and a message is sent 
    // the message is written into the transitions' destination. This is to allow for the activation
    // of breakout transitions. These messages should be reset once the transition concludes.
    // Also the transition itself (which has just finished) needs its messages reset.
    MR::NodeID prevActiveChildNodeID = nodeConnections->m_activeChildNodeIDs[0];
    if( net->getNetworkDef()->getNodeDef(prevActiveChildNodeID)->getNodeFlags() & NodeDef::NODE_FLAG_IS_TRANSITION )
    {
      StateID transitionStateID = smDef->findStateIDFromNodeID(prevActiveChildNodeID);
      resetStateMessageIDConditions(transitionStateID, smDef);

      NodeID transitionDestNodeID = net->getDestinationNodeIDFromATransition(prevActiveChildNodeID);
      StateID destStateID = smDef->findStateIDFromNodeID(transitionDestNodeID);
      resetStateMessageIDConditions(destStateID, smDef);
    }

    // The application can register a callback when the root of the state machine changes.
    if(m_onStateChangedCB)
    {
      m_onStateChangedCB(smNodeID, rootNodeID);
    } 

    m_activeStateID = smDef->findStateIDFromNodeID(rootNodeID); // Store the new state index.
    NMP_ASSERT(m_activeStateID != INVALID_STATE_ID);            // Check this Node ID exists in the available states.
    nodeConnections->m_activeChildNodeIDs[0] = rootNodeID;      // Store the new root node.
    net->setActiveParentNodeID(rootNodeID, smNodeID);           // Set the parent of the new root state to be the state machine
  }

  //------------------------
  // Queue the update of any bits of AttribData that our active conditions are dependent on.
  // This is because this data may not not otherwise be updated through the the root nodes dependencies.
  queueConditionDeps(m_activeStateID, smDef, net);  // Conditions from active state.
  queueGlobalConditionDeps(m_activeStateID, smDef, net);  // Globally active conditions.
}

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
uint32_t AttribDataStateMachine::serializeTx(
  Network*    net,
  MR::NodeID  owningNodeID,
  AttribData* toOutput,
  void*       outputBuffer,
  uint32_t    NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  NMP_ASSERT(toOutput);
  AttribDataStateMachine* source = (AttribDataStateMachine*)toOutput;

  // Calculate output data size.
  uint32_t dataSize = sizeof(AttribStateMachineOutputData);
  
  NodeDef* smNodeDef = net->getNetworkDef()->getNodeDef(owningNodeID);
  NMP_ASSERT(smNodeDef);
  AttribDataStateMachineDef* smDef = smNodeDef->getAttribData<AttribDataStateMachineDef>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  NMP_ASSERT(smDef);


  NMP_ASSERT(smDef);
  StateDef* stateDef = NULL;
  if (source->getActiveStateID() != INVALID_STATE_ID)
  {
    stateDef = smDef->getStateDef(source->getActiveStateID());
    NMP_ASSERT(stateDef);
    dataSize += (sizeof(AttribStateMachineOutputData::PossibleTransition) * stateDef->getNumExitTransitionStates());
    for (uint32_t i = 0; i < stateDef->getNumExitTransitionStates(); ++i)
    {
      StateID transitStateID = stateDef->getExitTransitionStateID(i);
      StateDef* transitionState = smDef->getStateDef(transitStateID);
      NMP_ASSERT(transitionState);
      dataSize += (transitionState->getNumEntryConditions() * sizeof(AttribStateMachineOutputData::Condition));
    }
  }

  if (outputBuffer)
  {
    // Write output data to the buffer.
    NMP_ASSERT(outputBufferSize >= dataSize && smDef);
    AttribStateMachineOutputData* output = (AttribStateMachineOutputData*) outputBuffer;
    output->m_activeStateID = source->getActiveStateID();
    output->m_active = net->nodeIsActive(owningNodeID);
    if (output->m_activeStateID != INVALID_STATE_ID)
    {
      output->m_activeNodeID = stateDef->getNodeID();
      output->m_numActiveConditions = stateDef->getNumExitConditions();
      output->m_numPossibleTransitions = stateDef->getNumExitTransitionStates();
      AttribStateMachineOutputData::PossibleTransition* outputTransitions = NULL;
      if (output->m_numPossibleTransitions)
        outputTransitions = (AttribStateMachineOutputData::PossibleTransition*) ((char*)outputBuffer + sizeof(AttribStateMachineOutputData));

      // Each possible transition.
      AttribStateMachineOutputData::Condition* currentCondition = (AttribStateMachineOutputData::Condition*)
          ((char*)outputTransitions + (sizeof(AttribStateMachineOutputData::PossibleTransition) * output->m_numPossibleTransitions));
      for (uint32_t i = 0; i < output->m_numPossibleTransitions; ++i)
      {
        StateID transitStateID = stateDef->getExitTransitionStateID(i);
        StateDef* transitionState = smDef->getStateDef(transitStateID);
        NMP_ASSERT(transitionState);
        outputTransitions[i].m_transitionStateID = transitStateID;
        outputTransitions[i].m_transitionNodeID = transitionState->getNodeID();
        outputTransitions[i].m_numConditions = transitionState->getNumEntryConditions();

        AttribStateMachineOutputData::Condition* conditions = 0;
        if (outputTransitions[i].m_numConditions)
          conditions = currentCondition;

        for (StateConditionIndex j = 0; j < outputTransitions[i].m_numConditions; ++j)
        {
          ConditionIndex conditionIndex = transitionState->getEntryConditionStateMachineIndex(j);
          TransitCondition* condition = source->m_conditions[conditionIndex];
          conditions[j].m_satisfied = condition->condIsSatisfied();
          conditions[j].m_type = condition->condGetType();
          conditions[j].endianSwap();
        }
        currentCondition = &(conditions[outputTransitions[i].m_numConditions]);

        // Dislocate the transition data.
        if (outputTransitions[i].m_numConditions)
          UNFIX_PTR_RELATIVE(AttribStateMachineOutputData::Condition, conditions, &(outputTransitions[i]));
        outputTransitions[i].m_conditionsPtrOffset = (uint32_t)((ptrdiff_t)conditions);
        NMP::netEndianSwap(outputTransitions[i].m_conditionsPtrOffset);
        NMP::netEndianSwap(outputTransitions[i].m_transitionStateID);
        NMP::netEndianSwap(outputTransitions[i].m_transitionNodeID);
        NMP::netEndianSwap(outputTransitions[i].m_numConditions);
      }

      // Dislocate the transitions array ptr.
      if (output->m_numPossibleTransitions)
        UNFIX_PTR_RELATIVE(AttribStateMachineOutputData::PossibleTransition, outputTransitions, output);
      output->m_possibleTransitionPtrOffset = (uint32_t)((ptrdiff_t)outputTransitions);
    }
    else
    {
      output->m_activeNodeID = INVALID_NODE_ID;
      output->m_numActiveConditions = 0;
      output->m_numPossibleTransitions = 0;
      output->m_possibleTransitionPtrOffset = 0;
    }

    output->m_targetStateID = source->getTargetStateID();
    output->m_targetNodeID = INVALID_NODE_ID;
    if (output->m_targetStateID != INVALID_STATE_ID)
      output->m_targetNodeID = smDef->getStateDef(output->m_targetStateID)->getNodeID();

    NMP::netEndianSwap(output->m_possibleTransitionPtrOffset);
    NMP::netEndianSwap(output->m_active);
    NMP::netEndianSwap(output->m_activeStateID);
    NMP::netEndianSwap(output->m_activeNodeID);
    NMP::netEndianSwap(output->m_targetStateID);
    NMP::netEndianSwap(output->m_targetNodeID);
    NMP::netEndianSwap(output->m_numActiveConditions);
    NMP::netEndianSwap(output->m_numPossibleTransitions);
  }
  return dataSize;
}

#endif // MR_OUTPUT_DEBUGGING

//----------------------------------------------------------------------------------------------------------------------
// AttribDataStateMachineInitData functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format StateMachineInitData::getMemoryRequirements()
{
  NMP::Memory::Format result(
    NMP::Memory::align(sizeof(StateMachineInitData), NMP_NATURAL_TYPE_ALIGNMENT),
    NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
StateMachineInitData* StateMachineInitData::init(NMP::Memory::Resource& resource, NodeID subStateID)
{
  NMP::Memory::Format memReqs(
    NMP::Memory::align(sizeof(StateMachineInitData), NMP_NATURAL_TYPE_ALIGNMENT),
    NMP_NATURAL_TYPE_ALIGNMENT);
  resource.align(memReqs);

  StateMachineInitData* result = (StateMachineInitData*)resource.ptr;

  result->m_type = NODE_INIT_DATA_TYPE_STATE_MACHINE;
  result->m_subStateID = subStateID;

  resource.increment(memReqs);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool StateMachineInitData::locate(NodeInitData* target)
{
  StateMachineInitData* result = (StateMachineInitData*)target;

  NodeInitData::locate(target);

  NMP::endianSwap(result->m_subStateID);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool StateMachineInitData::dislocate(NodeInitData* target)
{
  StateMachineInitData* result = (StateMachineInitData*)target;

  NodeInitData::dislocate(target);

  NMP::endianSwap(result->m_subStateID);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
// State Machine NodeDef functions.
//----------------------------------------------------------------------------------------------------------------------
Task* stateMachineNodeQueuePassThrough(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  // There must always be a dependent task even when this is the root node in the network.
  NMP_ASSERT(node);

  // Queue the task that will create a reference to the input parameter.
  TaskID createRefTask = CoreTaskIDs::getCreateReferenceToInputTaskID(dependentParameter->m_attribAddress.m_semantic);
  Task* task = queue->createNewTaskOnQueue(
                 createRefTask,
                 node->getNodeID(),
                 2,
                 dependentParameter,
                 false,
                 true,
                 false,
                 true);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    // Get active child and parent node IDs.
    const NodeConnections* connections = net->getActiveNodesConnections(node->getNodeID());
    NMP_ASSERT(connections && (connections->m_numActiveChildNodes == 1));
    NodeID activeChildNodeID = connections->m_activeChildNodeIDs[0];
    NodeID activeParentNodeID = connections->m_activeParentNodeID;

#ifdef NMP_ENABLE_ASSERTS
    NodeBinEntry* networkEntry = net->getAttribDataNodeBinEntry(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, node->getNodeID());
    NMP_ASSERT(networkEntry);
    AttribDataStateMachine* stateMachine = networkEntry->getAttribData<AttribDataStateMachine>();
    NetworkDef* netDef = net->getNetworkDef();
    AttribDataHandle* handle = netDef->getAttribDataHandle(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, node->getNodeID());
    NMP_ASSERT(handle);
    AttribDataStateMachineDef* stateMachineDef = (AttribDataStateMachineDef*)handle->m_attribData;
    NMP_ASSERT(stateMachine && stateMachineDef);
    NMP_ASSERT(stateMachine->getActiveNodeID(stateMachineDef) ==  activeChildNodeID)
#endif

    // Dependency must come directly from a parent or child.
    AttribSemanticSense sense = Manager::getInstance().getAttributeSemanticSense(dependentParameter->m_attribAddress.m_semantic);
    NMP_ASSERT(sense != ATTRIB_SENSE_NONE);
    // If the attrib flows from parent to child, the request must come from the parent.
    bool fromParent = (sense == ATTRIB_SENSE_FROM_CHILD_TO_PARENT);

    // Create a reference lifespan of 0 unless specifically asked otherwise.
    FrameCount currFrameNo = net->getCurrentFrameNo();

    // Ensure that if a physics node exists it can access our output history
    uint16_t outputLifespan = net->getPostUpdateAccessAttribLifespan(node->getNodeID(), dependentParameter->m_attribAddress.m_semantic, 3);
       
    if (fromParent)
    {
      // Add dependency on our child attrib data.
      net->TaskAddParamAndDependency(
        task,
        0,
        dependentParameter->m_attribAddress.m_semantic,
        dependentParameter->m_attribType,
        activeChildNodeID,
        node->getNodeID(),
        TPARAM_FLAG_INPUT,
        currFrameNo,
        dependentParameter->m_attribAddress.m_animSetIndex);
    }
    else
    {
      // Add dependency on our parent attrib data.
      net->TaskAddParamAndDependency(
        task,
        0,
        dependentParameter->m_attribAddress.m_semantic,
        dependentParameter->m_attribType,
        activeParentNodeID,
        node->getNodeID(),
        TPARAM_FLAG_INPUT,
        currFrameNo,
        dependentParameter->m_attribAddress.m_animSetIndex);
    }

    // Add our output attrib.
    net->TaskAddOutputParam(
      task,
      1,
      dependentParameter->m_attribAddress.m_semantic,
      dependentParameter->m_attribType,
      INVALID_NODE_ID,
      TPARAM_FLAG_NONE,
      currFrameNo,
      outputLifespan,
      net->getOutputAnimSetIndex(node->getNodeID()));
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
// Update connections function.
NodeID nodeStateMachineUpdateConnections(
  NodeDef*            smNodeDef,
  Network*            net)
{
  NMP_ASSERT(net && smNodeDef);

  // Get the state machine attribute from the network.
  AttribDataStateMachine* sm = net->getAttribData<AttribDataStateMachine>(
                                        ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
                                        smNodeDef->getNodeID());
  sm->updateConnections(smNodeDef, net);

  return smNodeDef->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeStateMachineFindGeneratingNodeForSemantic(
  NodeID              NMP_UNUSED(callingNodeID),
  bool                fromParent,
  AttribDataSemantic  semantic,
  NodeDef*            node,
  Network*            net)
{
  NodeID result = INVALID_NODE_ID;

  NMP_ASSERT(semantic != ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE);

  // Pass on to parent or child appropriately.
  if (fromParent)
  {
    AttribDataStateMachine* sm = net->getAttribData<AttribDataStateMachine>(
                                   ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
                                   node->getNodeID());
   
    // Get the active child node id.
    NodeID activeChildNodeID;
    if (sm->m_targetStateID == INVALID_STATE_ID)
    {
      // State machine is probably active.
      activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);
    }
    else
    {
      // This node may be inactive so pass the query through to the node that will be activated on
      // the next update of connections.
      AttribDataStateMachineDef* smDef = node->getAttribData<AttribDataStateMachineDef>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
      activeChildNodeID = smDef->getNodeIDFromStateID(sm->m_targetStateID);
    }

    // Redirect to child.
    result = net->nodeFindGeneratingNodeForSemantic(activeChildNodeID, semantic, fromParent, node->getNodeID());
  }
  else
  {
    NodeID activeParentNodeID = net->getActiveParentNodeID(node->getNodeID());
    result = net->nodeFindGeneratingNodeForSemantic(activeParentNodeID, semantic, fromParent, node->getNodeID());
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void nodeStateMachineDeleteInstance(
  const NodeDef* node,
  Network*       net)
{
  // nodeShareDeleteInstanceWithChildren deletes all child nodes.
  // In a state machine node, that causes each state to be deleted multiple times,
  // since each state node is a child of the state machine itself, but also a child
  // of all the transitions to and from it. For performance reasons
  // this function should only delete the active child.
  NodeID thisNodeID = node->getNodeID();
  NodeConnections* thisNodeConnections = net->getActiveNodesConnections(thisNodeID);
  net->deleteNodeInstance(thisNodeConnections->m_activeChildNodeIDs[0]);

  // Delete all attrib data except our state data.
  NodeBin* nodeBin = net->getNodeBin(thisNodeID);
  const AttribDataSemantic excludeSemantics[1] = {ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE};
  nodeBin->deleteAllAttribDataExcluding(1, excludeSemantics);

  // Clear this nodes queued task list.
  nodeBin->clearQueuedTasks();

  // Get the state machine attributes from the network and networkDef.
  AttribDataStateMachine* sm = net->getAttribData<AttribDataStateMachine>(
    ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
    thisNodeID);
  AttribDataStateMachineDef* smDef = node->getAttribData<AttribDataStateMachineDef>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  // Make all transition conditions in state ready for re-use
  sm->resetActiveStateConditions(smDef);

  // Make all global transition conditions ready for re-use.
  sm->resetGlobalConditions(smDef); 

  // Set the default starting state index for this state machine.
  sm->m_activeStateID = INVALID_STATE_ID;
  sm->m_targetStateID = smDef->m_defaultStartingStateID;

  // Reset the nodes active connections structure for re-use.
  StateDef* defaultStateDef = smDef->getStateDef(smDef->m_defaultStartingStateID);
  thisNodeConnections->m_activeChildNodeIDs[0] = defaultStateDef->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
void nodeStateMachineInitInstance(NodeDef* node, Network* net)
{
  NMP_ASSERT(node && net && net->getNetworkDef());
  
  // Get each state machine def.
  NodeID stateMachineNodeID = node->getNodeID();
  AttribDataStateMachineDef* smDef = node->getAttribData<AttribDataStateMachineDef>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
 
  // Get memory requirements for state machine instance given the state machine def.
  NMP::Memory::Format smMemReqs = AttribDataStateMachine::getMemoryRequirements(smDef);
  NMP::MemoryAllocator* allocator = net->getPersistentMemoryAllocator();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, smMemReqs);

  // Create the state machine attribute instance and add it to the network.
  AttribDataStateMachine* stateMachineAttrib = AttribDataStateMachine::init(resource, smDef, net, stateMachineNodeID, 0);
  stateMachineAttrib->m_allocator = allocator;
  AttribAddress attribAddress(
    ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
    stateMachineNodeID,
    INVALID_NODE_ID,
    VALID_FOREVER,
    ANIMATION_SET_ANY);
  AttribDataHandle stateMachineAttribHandle = { stateMachineAttrib, smMemReqs };
  net->addAttribData(attribAddress, stateMachineAttribHandle, LIFESPAN_FOREVER);
}

//----------------------------------------------------------------------------------------------------------------------
bool nodeStateMachineHandleMessages(const Message& message, NodeID smNodeID, Network* net)
{
  // Get state machine.
  AttribDataStateMachine* sm = net->getAttribData<AttribDataStateMachine>(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, smNodeID);

  // Get state machine def.
  AttribDataStateMachineDef* smDef = net->getNetworkDef()->getAttribData<AttribDataStateMachineDef>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, smNodeID);

  if (sm->setTransitionMessageID(message.m_id, message.m_status, smDef, net))
  {
    return true;
  }

  return false;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
