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
#include "morpheme/Nodes/mrNodeCharacterControllerOverride.h"
#include "morpheme/TransitConditions/mrTransitConditionOnMessage.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
void nodeCCOverrideInitInstance(
  NodeDef* ccOverrideNodeDef,
  Network* net)
{
  NMP_ASSERT(ccOverrideNodeDef && net && net->getNetworkDef());

  // Get character controller override def data.
  NodeID ccOverrideNodeID = ccOverrideNodeDef->getNodeID();
  AttribDataCCOverrideConditionsDef* ccOverrideAttribDef = 
        ccOverrideNodeDef->getAttribData<AttribDataCCOverrideConditionsDef>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  
  // Get memory requirements for character controller override conditions attribute instance given the def.
  NMP::Memory::Format ccOverrideMemReqs = AttribDataCCOverrideConditions::getMemoryRequirements(ccOverrideAttribDef);
  NMP::MemoryAllocator* allocator = net->getPersistentMemoryAllocator();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, ccOverrideMemReqs);

  // Create the character controller override conditions attribute instance and add it to the network.
  AttribDataCCOverrideConditions* ccOverrideAttrib = AttribDataCCOverrideConditions::init(resource, ccOverrideAttribDef, net, ccOverrideNodeID, 0);
  ccOverrideAttrib->m_allocator = allocator;
  AttribAddress attribAddress(
                        ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
                        ccOverrideNodeID,
                        INVALID_NODE_ID,
                        VALID_FOREVER,
                        ANIMATION_SET_ANY);
  AttribDataHandle ccOverrideAttribHandle = { ccOverrideAttrib, ccOverrideMemReqs };
  net->addAttribData(attribAddress, ccOverrideAttribHandle, LIFESPAN_FOREVER);

  //---------------------------
  if (ccOverrideNodeDef->getNumReflexiveCPPins() > 0)
  {
    MR::nodeInitPinAttribDataInstance(ccOverrideNodeDef, net);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void nodeCCOverrideDeleteInstance(
  const NodeDef* ccOverrideNodeDef,
  Network*       net)
{
  NMP_ASSERT(net && ccOverrideNodeDef);
  NodeID thisNodeID = ccOverrideNodeDef->getNodeID();

  // Delete children first.
  NodeID childNodeID = ccOverrideNodeDef->getChildNodeID(0);
  net->deleteNodeInstance(childNodeID);

  // Delete all attrib data except our state data.
  NodeBin* thisNodeBin = net->getNodeBin(thisNodeID);
  AttribDataSemantic semantics[] = {ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE};
  thisNodeBin->deleteAllAttribDataExcluding(1, semantics);

  // Clear this nodes queued task list.
  thisNodeBin->clearQueuedTasks();

  // Set the node's last updated frame to -1, so that everything will get updated the first frame it's used again.
  thisNodeBin->setLastFrameUpdate(NOT_FRAME_UPDATED);

  // Reset our conditions states ready for reuse on reactivation of this node.
  AttribDataCCOverrideConditions* ccOverrideAttrib = net->getAttribData<AttribDataCCOverrideConditions>(
                                                                  ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
                                                                  thisNodeID);
  ccOverrideAttrib->resetConditions();
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeCCOverrideUpdateConnections(
  NodeDef* ccOverrideNodeDef,
  Network* net)
{
  // Get the character controller override conditions attribute instance from the network.
  NMP_ASSERT(net && ccOverrideNodeDef);
  NodeID ccOverrideNodeID = ccOverrideNodeDef->getNodeID();
  AttribDataCCOverrideConditions* ccOverrideAttrib = net->getAttribData<AttribDataCCOverrideConditions>(
                                                                  ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
                                                                  ccOverrideNodeID);
  AnimSetIndex myAnimSetIndex = net->getOutputAnimSetIndex(ccOverrideNodeDef->getNodeID());

  // Update anything connected to the activate pin.
  AttribDataBool* inputCPActivate = net->updateInputCPConnection<AttribDataBool>(ccOverrideNodeDef->getInputCPConnection(0), myAnimSetIndex);
  
  // Get the ID of the one child.
  NodeID childNodeID = ccOverrideNodeDef->getChildNodeID(0);
 
  // Get the Networks active character controller.
  CharacterControllerInterface* activeCC = net->getCharacterController();
  NMP_ASSERT(activeCC);

  //---------------------
  // Update the condition set.
  if (ccOverrideAttrib->updateConditions(childNodeID, net) && inputCPActivate->m_value)
  {
    // ---------------------
    // All conditions are satisfied, or the force activate pin is true,
    //  so apply the character controller override properties for the active anim set.

    //---------------------
    // Update anything connected to the override position pin.
    AttribDataVector3* inputCPPosition = net->updateOptionalInputCPConnection<AttribDataVector3>(ccOverrideNodeDef->getInputCPConnection(1), myAnimSetIndex);
    if (inputCPPosition)
    {
      // Apply the override position to the character controller.
      AttribDataVector3 positionOverrideAttrib;
      positionOverrideAttrib.m_value = inputCPPosition->m_value;
      activeCC->setRequestedPropertyOverride(
        CC_PROPERTY_TYPE_POSITION_ABSOLUTE,  // The type of property we are trying to set.
        &positionOverrideAttrib,             // The property value to apply.
        ccOverrideAttrib->m_def->m_priority, // The priority of this override vs other active overrides.
        net->getCurrentFrameNo());           // The frame on which this override is being requested.
    }

    //---------------------
    // Update anything connected to the override offset pin.
    AttribDataVector3* inputCPOffset = net->updateOptionalInputCPConnection<AttribDataVector3>(ccOverrideNodeDef->getInputCPConnection(2), myAnimSetIndex);
    if (inputCPOffset)
    {
      // Apply the override position to the character controller.
      AttribDataVector3 offsetOverrideAttrib;
      offsetOverrideAttrib.m_value = inputCPOffset->m_value;
      activeCC->setRequestedPropertyOverride(
        CC_PROPERTY_TYPE_POSITION_OFFSET,    // The type of property we are trying to set.
        &offsetOverrideAttrib,               // The property value to apply.
        ccOverrideAttrib->m_def->m_priority, // The priority of this override vs other active overrides.
        net->getCurrentFrameNo());           // The frame on which this override is being requested.
    }
    
    //---------------------
    // Get the property set for the current anim set.
    AttribDataCCOverridePropertiesDef* ccOverridePropertiesDef = 
            ccOverrideNodeDef->getAttribData<AttribDataCCOverridePropertiesDef>(
                                            ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET,
                                            myAnimSetIndex);
        
    //---------------------
    // Apply the override properties to the character controller.
    for (uint16_t i = 0; i < ccOverridePropertiesDef->getNumProperties(); ++i)
    {
      activeCC->setRequestedPropertyOverride(
                ccOverridePropertiesDef->getPropertyType(i), // The type of property we are trying to set.
                ccOverridePropertiesDef->getProperty(i),     // The property value to apply.
                ccOverrideAttrib->m_def->m_priority,         // The priority of this override vs other active overrides.
                net->getCurrentFrameNo());                   // The frame on which this override is being requested.
    }   
  }

  // Recurse update connections to our child. (Must update connections before doing any queuing).
  net->updateNodeInstanceConnections(childNodeID, myAnimSetIndex);

  // Queue the update of any bits of AttribData that our conditions are dependent on.
  //  This is because this data may not otherwise be updated through the root nodes dependencies.
  ccOverrideAttrib->queueConditionDeps(ccOverrideNodeDef->getNodeID(), net);

  return ccOverrideNodeID;
}

//----------------------------------------------------------------------------------------------------------------------
bool nodeCCOverrideHandleMessages(
  const Message& message,
  NodeID         ccOverrideNodeID,
  Network*       net)
{
  // Get the character controller override conditions attribute instance from the network.
  NMP_ASSERT(net);
  AttribDataCCOverrideConditions* ccOverrideConditions = net->getAttribData<AttribDataCCOverrideConditions>(
                                                                  ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
                                                                  ccOverrideNodeID);
  if (ccOverrideConditions->setMessage(message.m_id, message.m_status, net))
    return true;

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataCCOverrideConditions functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataCCOverrideConditions::getMemoryRequirements(
  AttribDataCCOverrideConditionsDef* ccOverrideAttribDef)
{
  NMP::Memory::Format result(sizeof(AttribDataCCOverrideConditions), MR_ATTRIB_DATA_ALIGNMENT);

  // Condition pointers array.
  NMP::Memory::Format memReqs(sizeof(TransitCondition*) * ccOverrideAttribDef->getNumConditions(), NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqs;

  // Condition instances themselves.
  for (uint32_t i = 0; i < ccOverrideAttribDef->getNumConditions(); ++i)
  {
    // Get condition def.
    TransitConditionDef* conditionDef = ccOverrideAttribDef->getConditionDef(i);
    NMP_ASSERT(conditionDef);
    result += conditionDef->condGetMemoryRequirements();
  }

  result.align();
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataCCOverrideConditions* AttribDataCCOverrideConditions::init(
  NMP::Memory::Resource&             resource,
  AttribDataCCOverrideConditionsDef* ccOverrideAttribDef,
  Network*                           net,
  NodeID                             nodeID,
  uint16_t                           refCount)
{
  NMP_ASSERT(getMemoryRequirements(ccOverrideAttribDef).alignment == MR_ATTRIB_DATA_ALIGNMENT);
  NMP::Memory::Format fmt(sizeof(AttribDataCCOverrideConditions), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataCCOverrideConditions* result = (AttribDataCCOverrideConditions*)resource.alignAndIncrement(fmt);

  result->setType(ATTRIB_TYPE_C_C_OVERRIDE_CONDITIONS);
  result->setRefCount(refCount);

  // Condition pointers array.
  fmt.set(sizeof(TransitCondition*) * ccOverrideAttribDef->m_numConditions, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_conditions = (TransitCondition**)resource.alignAndIncrement(fmt);

  // Create condition instances.
  for (uint32_t i = 0; i < ccOverrideAttribDef->m_numConditions; ++i)
  {
    // Get condition def.
    TransitConditionDef* conditionDef = ccOverrideAttribDef->m_conditions[i];
    NMP_ASSERT(conditionDef);
    result->m_conditions[i] = conditionDef->condInit(resource, net);
  }

  result->m_nodeID = nodeID;
  result->m_def = ccOverrideAttribDef;
  
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataCCOverrideConditions::relocate(AttribData* target, const NetworkDef* netDef)
{
  AttribDataCCOverrideConditions* result = static_cast<AttribDataCCOverrideConditions*>(target);
  NMP_ASSERT(NMP_IS_ALIGNED(target, MR_ATTRIB_DATA_ALIGNMENT));
  
  // Find the definition data that is associated with state machine.
  result->m_def = netDef->getAttribData<AttribDataCCOverrideConditionsDef>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, result->m_nodeID);

  NMP::Memory::Format memReqs = getMemoryRequirements(result->m_def);
  NMP::Memory::Resource resultRes = {result, memReqs};

  // AttribDataCCOverrideConditions class.
  resultRes.increment(sizeof(AttribDataCCOverrideConditions));
  
  // Condition pointers array.
  memReqs.set(sizeof(TransitCondition*) * result->m_def->m_numConditions, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_conditions = (TransitCondition**)resultRes.alignAndIncrement(memReqs);
  
  // Condition instances.
  for (uint32_t i = 0; i < result->m_def->getNumConditions(); ++i)
  {
    TransitConditionDef* conditionDef = result->m_def->m_conditions[i];

    memReqs = conditionDef->condGetMemoryRequirements();
    result->m_conditions[i] = (TransitCondition*)resultRes.alignAndIncrement(memReqs);
    
    conditionDef->condRelocate(result->m_conditions[i], result->m_conditions[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataCCOverrideConditions::locate(AttribData* target, const NetworkDef* netDef)
{
  relocate(target, netDef);
}

//----------------------------------------------------------------------------------------------------------------------
bool AttribDataCCOverrideConditions::updateConditions(
  NodeID   childNodeID,
  Network* net)
{
  bool result = true;
  
  // Update all conditions.
  for (uint32_t i = 0; i < m_def->getNumConditions(); ++i)
  {
    TransitConditionDef* tcDef = m_def->getConditionDef(i);
    NMP_ASSERT(tcDef && m_conditions[i]);
    tcDef->condUpdate(m_conditions[i], net, childNodeID);

    // Track whether all conditions have been satisfied.
    result &= m_conditions[i]->condIsSatisfied();
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AttribDataCCOverrideConditions::queueConditionDeps(
  NodeID   childNodeID,
  Network* net)
{
  // Queue any dependencies on our conditions.
  for (uint32_t i = 0; i < m_def->getNumConditions(); ++i)
  {
    TransitConditionDef* tcDef = m_def->getConditionDef(i);
    NMP_ASSERT(tcDef && m_conditions[i]);
    tcDef->condQueueDeps(m_conditions[i], net, childNodeID);
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool AttribDataCCOverrideConditions::setMessage(
  MessageID messageID,
  bool      status,
  Network*  NMP_UNUSED(net))
{
  bool hasBeenSet = false;
  for (uint32_t i = 0; i < m_def->getNumConditions(); ++i)
  {
    TransitConditionDef* tcDef = m_def->getConditionDef(i);
    NMP_ASSERT(tcDef && m_conditions[i]);
    
    if (tcDef->getType() == TRANSCOND_ON_MESSAGE_ID)
    {
      TransitConditionDefOnMessage* messageConditionDef = static_cast<TransitConditionDefOnMessage*> (tcDef);
      if (messageID == messageConditionDef->getMessageID())
      {
        NMP_ASSERT(m_conditions[i]->condGetType() == TRANSCOND_ON_MESSAGE_ID);
        TransitConditionOnRequest* messageCondition = static_cast<TransitConditionOnRequest*> (m_conditions[i]);
        TransitConditionDefOnMessage::instanceSetState(messageCondition, tcDef, status);
        // We have at least one message condition in this node.
        hasBeenSet = true;
      }
    }
  }

  return hasBeenSet;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataCCOverrideConditionsDef functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataCCOverrideConditionsDef::getMemoryRequirements()
{
  return NMP::Memory::Format(sizeof(AttribDataCCOverrideConditionsDef), MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataCCOverrideConditionsDef* AttribDataCCOverrideConditionsDef::init(
  NMP::Memory::Resource& resource,
  uint16_t               refCount)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataCCOverrideConditionsDef), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataCCOverrideConditionsDef* result = (AttribDataCCOverrideConditionsDef*)resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_C_C_OVERRIDE_CONDITIONS_DEF);
  result->setRefCount(refCount);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataCCOverrideConditionsDef::locate(AttribData* target)
{
  AttribDataCCOverrideConditionsDef* result = static_cast<AttribDataCCOverrideConditionsDef*>(target);
        
  // Locate the base attrib data class first.
  AttribData::locate(target);

  NMP::endianSwap(result->m_priority);
  NMP::endianSwap(result->m_numConditions);
  
  if (result->m_conditions)
  {
    REFIX_SWAP_PTR_RELATIVE(TransitConditionDef*, result->m_conditions, result);
  }  
  for (uint32_t i = 0; i < result->m_numConditions; ++i)
  {
    REFIX_SWAP_PTR_RELATIVE(TransitConditionDef, result->m_conditions[i], result);

    // Locate condition.
    TransitConditionDef::locate(result->m_conditions[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataCCOverrideConditionsDef::dislocate(AttribData* target)
{
  AttribDataCCOverrideConditionsDef* result = static_cast<AttribDataCCOverrideConditionsDef*>(target);

  // Dislocate the base attrib data class first.
  AttribData::dislocate(target);

  // Conditions.
  for (uint32_t i = 0; i < result->m_numConditions; ++i)
  {
    // Dislocate condition
    TransitConditionDef::dislocate(result->m_conditions[i]);

    UNFIX_SWAP_PTR_RELATIVE(TransitConditionDef, result->m_conditions[i], result);
  }
  if (result->m_conditions)
  {
    UNFIX_SWAP_PTR_RELATIVE(TransitConditionDef*, result->m_conditions, result);
  }
 
  NMP::endianSwap(result->m_numConditions);
  NMP::endianSwap(result->m_priority);
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataCCOverridePropertiesDef functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataCCOverridePropertiesDef::getMemoryRequirements()
{
  return NMP::Memory::Format(sizeof(AttribDataCCOverridePropertiesDef), MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataCCOverridePropertiesDef* AttribDataCCOverridePropertiesDef::init(
  NMP::Memory::Resource& resource,
  uint16_t               refCount)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataCCOverridePropertiesDef), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataCCOverridePropertiesDef* result = (AttribDataCCOverridePropertiesDef*)resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_C_C_OVERRIDE_PROPERTIES_DEF);
  result->setRefCount(refCount);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataCCOverridePropertiesDef::locate(AttribData* target)
{
  AttribDataCCOverridePropertiesDef* result = static_cast<AttribDataCCOverridePropertiesDef*>(target);

  MR::Manager& manager = MR::Manager::getInstance();

  // Locate the base attrib data class first.
  AttribData::locate(target);

  NMP::endianSwap(result->m_numProperties);

  if (result->m_numProperties)
  {
    REFIX_SWAP_PTR_RELATIVE(AttribData*, result->m_properties, result);
    REFIX_SWAP_PTR_RELATIVE(CCPropertyType, result->m_propertyTypes, result);
  }  
  // Locate each property AttribData.
  for (uint32_t i = 0; i < result->m_numProperties; ++i)
  {
    NMP::endianSwap(result->m_propertyTypes[i]);

    REFIX_SWAP_PTR_RELATIVE(AttribData, result->m_properties[i], result);
    AttribDataType type = result->m_properties[i]->getTypeUnlocated();

    AttribLocateFn locateFn = manager.getAttribLocateFn(type);
    NMP_ASSERT(locateFn);
    locateFn(result->m_properties[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataCCOverridePropertiesDef::dislocate(AttribData* target)
{
  AttribDataCCOverridePropertiesDef* result = static_cast<AttribDataCCOverridePropertiesDef*>(target);

  MR::Manager& manager = MR::Manager::getInstance();

  // Dislocate the base attrib data class first.
  AttribData::dislocate(target);
  
  // Dislocate each property AttribData.
  for (uint32_t i = 0; i < result->m_numProperties; ++i)
  {
    NMP::endianSwap(result->m_propertyTypes[i]);

    AttribDataType type = result->m_properties[i]->getType();
    AttribDislocateFn dislocateFn = manager.getAttribDislocateFn(type);
    NMP_ASSERT(dislocateFn);
    dislocateFn(result->m_properties[i]);

    UNFIX_SWAP_PTR_RELATIVE(AttribData, result->m_properties[i], result);
  }
  if (result->m_numProperties)
  {
    UNFIX_SWAP_PTR_RELATIVE(CCPropertyType, result->m_propertyTypes, result);
    UNFIX_SWAP_PTR_RELATIVE(AttribData*, result->m_properties, result);
  }  

  NMP::endianSwap(result->m_numProperties);
}
 
} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
