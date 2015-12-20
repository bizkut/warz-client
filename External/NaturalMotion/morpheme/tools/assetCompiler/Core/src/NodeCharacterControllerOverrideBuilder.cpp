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
#include <tchar.h>
#include "NetworkDefBuilder.h"
#include "NodeCharacterControllerOverrideBuilder.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrNodeCharacterControllerOverride.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/TransitConditions/mrTransitConditionOnMessage.h"
#include "morpheme/TransitConditions/mrTransitConditionDiscreteEventTriggered.h"
#include "morpheme/TransitConditions/mrTransitConditionNodeActive.h"
#include "morpheme/TransitConditions/mrTransitConditionInSyncEventRange.h"
#include "assetProcessor/TransitConditionDefBuilder.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// Reading condition data from xml.
//----------------------------------------------------------------------------------------------------------------------
// Condition: On message ID.
bool isMessageConditionActive(
  const ME::DataBlockExport* nodeDefDataBlock)
{
  bool enabled = false;
  nodeDefDataBlock->readBool(enabled, "WhenMessageBroadcastEnabled");
  return enabled;
}

//----------------------------------------------------------------------------------------------------------------------
void initOnMessageCondition(
  MR::NodeDef*                           nodeDef,
  NetworkDefCompilationInfo*             netDefCompilationInfo,
  AssetProcessor*                        processor,
  NMP::Memory::Resource&                 memRes,
  const ME::DataBlockExport*             nodeDefDataBlock,
  MR::AttribDataCCOverrideConditionsDef* conditions,
  uint32_t&                              conditionIndex)
{
  bool enabled = isMessageConditionActive(nodeDefDataBlock);
  if (enabled)
  {
    TransitConditionDefBuilder* conditionBuilder = processor->getTransitConditionDefBuilder(TRANSCOND_ON_MESSAGE_ID);
    NMP_VERIFY(conditionBuilder);
    NMP::Memory::Format memReqs = conditionBuilder->getMemoryRequirements(NULL, NULL, NULL);
    MR::TransitConditionDefOnMessage* result = static_cast<MR::TransitConditionDefOnMessage*> (memRes.alignAndIncrement(memReqs));
    result->setType(TRANSCOND_ON_MESSAGE_ID);
    
    bool invertCondition = false;
    nodeDefDataBlock->readBool(invertCondition, "WhenMessageBroadcastInvert");
    result->setInvertFlag(invertCondition);

    uint32_t messageID;
    nodeDefDataBlock->readUInt(messageID, "WhenMessageBroadcastMessage");
    result->setMessageID((MR::MessageID) messageID);

    // Register this nodes interest in this message.
    if (messageID != MR::INVALID_MESSAGE_ID)
    {
      netDefCompilationInfo->registerMessageInterest(messageID, nodeDef->getNodeID());
    }

    // Set and increment condition count.
    conditions->m_conditions[conditionIndex] = result;
    ++conditionIndex;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Condition: In sync event range.
bool isInSyncEventRangeConditionActive(
  const ME::DataBlockExport* nodeDefDataBlock)
{
  bool enabled = false;
  nodeDefDataBlock->readBool(enabled, "WhenInEventRangeEnabled");
  return enabled;
}

//----------------------------------------------------------------------------------------------------------------------
void initInSyncEventRangeCondition(
  MR::NodeDef*                           nodeDef,
  AssetProcessor*                        processor,
  NMP::Memory::Resource&                 memRes,
  const ME::DataBlockExport*             nodeDefDataBlock,
  MR::AttribDataCCOverrideConditionsDef* conditions,
  uint32_t&                              conditionIndex)
{
  bool enabled = isInSyncEventRangeConditionActive(nodeDefDataBlock);
  if (enabled)
  {
    TransitConditionDefBuilder* conditionBuilder = processor->getTransitConditionDefBuilder(TRANSCOND_IN_SYNC_EVENT_RANGE_ID);
    NMP_VERIFY(conditionBuilder);
    NMP::Memory::Format memReqs = conditionBuilder->getMemoryRequirements(NULL, NULL, NULL);
    MR::TransitConditionDefInSyncEventRange* result = static_cast<MR::TransitConditionDefInSyncEventRange*> (memRes.alignAndIncrement(memReqs));
    result->setType(TRANSCOND_IN_SYNC_EVENT_RANGE_ID);
 
    bool invertCondition = false;
    nodeDefDataBlock->readBool(invertCondition, "WhenInEventRangeInvert");
    result->setInvertFlag(invertCondition);

    float eventRangeStart = 0.0f;
    nodeDefDataBlock->readFloat(eventRangeStart, "ActivationStartEvent");
    result->setEventRangeStart(eventRangeStart);
        
    float eventRangeEnd = 0.0f;
    nodeDefDataBlock->readFloat(eventRangeEnd, "ActivationEndEvent");
    result->setEventRangeEnd(eventRangeEnd);

    // Initialise event pos attrib address to look for.
    MR::AttribAddress attribAddress(
                        MR::ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS,
                        nodeDef->getChildNodeID(0),
                        MR::INVALID_NODE_ID,
                        MR::VALID_FRAME_ANY_FRAME,
                        MR::ANIMATION_SET_ANY);
    result->setSourceNodeSyncEventPlaybackPosAttribAddress(attribAddress);

    // Initialise sync event track attrib address to look for.
    attribAddress.m_semantic = MR::ATTRIB_SEMANTIC_SYNC_EVENT_TRACK;
    result->setSourceNodeSyncEventTrackAttribAddress(attribAddress);

    // Set and increment condition count.
    conditions->m_conditions[conditionIndex] = result;
    ++conditionIndex;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Condition: In sub state.
bool isInSubStateConditionActive(
  const ME::DataBlockExport* nodeDefDataBlock)
{
  bool enabled = false;
  nodeDefDataBlock->readBool(enabled, "WhenInSpecificSubStateEnabled");
  return enabled;
}

//----------------------------------------------------------------------------------------------------------------------
void initInSubStateCondition(
  AssetProcessor*                        processor,
  NMP::Memory::Resource&                 memRes,
  const ME::DataBlockExport*             nodeDefDataBlock,
  MR::AttribDataCCOverrideConditionsDef* conditions,
  uint32_t&                              conditionIndex)
{
  bool enabled = isInSubStateConditionActive(nodeDefDataBlock);
  if (enabled)
  {
    TransitConditionDefBuilder* conditionBuilder = processor->getTransitConditionDefBuilder(TRANSCOND_NODE_ACTIVE_ID);
    NMP_VERIFY(conditionBuilder);
    NMP::Memory::Format memReqs = conditionBuilder->getMemoryRequirements(NULL, NULL, NULL);
    MR::TransitConditionDefNodeActive* result = static_cast<MR::TransitConditionDefNodeActive*> (memRes.alignAndIncrement(memReqs));
    result->setType(TRANSCOND_NODE_ACTIVE_ID);
    
    bool invertCondition = false;
    nodeDefDataBlock->readBool(invertCondition, "WhenInSpecificSubStateInvert");
    result->setInvertFlag(invertCondition);

    MR::NodeID nodeID;
    readNodeID(nodeDefDataBlock, "ActivationSubStateNodeID", nodeID);
    result->setNodeID((MR::NodeID)nodeID);

    // Set and increment condition count.
    conditions->m_conditions[conditionIndex] = result;
    ++conditionIndex;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Reading properties data from xml.
//----------------------------------------------------------------------------------------------------------------------
// Property: Override scale.
bool isOverrideScaleActive(
  const ME::DataBlockExport* nodeDefDataBlock)
{
  bool enabled = false;
  nodeDefDataBlock->readBool(enabled, "EnableScale");
  return enabled;
}

//----------------------------------------------------------------------------------------------------------------------
void initOverrideScaleProperties(
  NMP::Memory::Resource&                 memRes,
  const ME::DataBlockExport*             nodeDefDataBlock,
  MR::AttribDataCCOverridePropertiesDef* properties,
  uint32_t&                              propertiesIndex,
  MR::AnimSetIndex                       animSetIndex)
{
  bool enabled = isOverrideScaleActive(nodeDefDataBlock);

  if (enabled)
  {
    char attributeName[256];
    sprintf_s(attributeName, "ScaleVertical_%d", animSetIndex);
    float scaleVertical = 1.0f;
    nodeDefDataBlock->readFloat(scaleVertical, attributeName);
    properties->m_properties[propertiesIndex] = MR::AttribDataFloat::init(memRes, scaleVertical, MR::IS_DEF_ATTRIB_DATA);
    properties->m_propertyTypes[propertiesIndex] = MR::CC_PROPERTY_TYPE_VERTICAL_SCALE;
    ++propertiesIndex;

    sprintf_s(attributeName, "ScaleHorizontal_%d", animSetIndex);
    float scaleHorizontal = 1.0f;
    nodeDefDataBlock->readFloat(scaleHorizontal, attributeName);
    properties->m_properties[propertiesIndex] = MR::AttribDataFloat::init(memRes, scaleHorizontal, MR::IS_DEF_ATTRIB_DATA);
    properties->m_propertyTypes[propertiesIndex] = MR::CC_PROPERTY_TYPE_HORIZONTAL_SCALE;
    ++propertiesIndex;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Property: Override offset.
bool isOverrideOffsetActive(
  const ME::DataBlockExport* nodeDefDataBlock)
{
  bool enabled = false;
  nodeDefDataBlock->readBool(enabled, "EnableOffset");
  return enabled;
}

//----------------------------------------------------------------------------------------------------------------------
void initOverrideOffsetProperties(
  NMP::Memory::Resource&                 memRes,
  const ME::DataBlockExport*             nodeDefDataBlock,
  MR::AttribDataCCOverridePropertiesDef* properties,
  uint32_t&                              propertiesIndex,
  MR::AnimSetIndex                       animSetIndex)
{
  bool enabled = isOverrideOffsetActive(nodeDefDataBlock);
  
  if (enabled)
  {
    // We only have a position offset for now.
    char attributeName[256];
    NMP::Vector3 translation;
    sprintf_s(attributeName, "OffsetPosition_%dX", animSetIndex);
    nodeDefDataBlock->readFloat(translation.x, attributeName);
    sprintf_s(attributeName, "OffsetPosition_%dY", animSetIndex);
    nodeDefDataBlock->readFloat(translation.y, attributeName);
    sprintf_s(attributeName, "OffsetPosition_%dZ", animSetIndex);
    nodeDefDataBlock->readFloat(translation.z, attributeName);
    properties->m_properties[propertiesIndex] = MR::AttribDataVector3::init(memRes, translation, MR::IS_DEF_ATTRIB_DATA);
    properties->m_propertyTypes[propertiesIndex] = MR::CC_PROPERTY_TYPE_POSITION_OFFSET;
    ++propertiesIndex;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Property: Override motion.
bool isOverrideMotionActive(
  const ME::DataBlockExport* nodeDefDataBlock)
{
  bool enabled = false;
  nodeDefDataBlock->readBool(enabled, "EnableOverrideMotion");
  return enabled;
}

//----------------------------------------------------------------------------------------------------------------------
void initOverrideMotionProperties(
  NMP::Memory::Resource&                 memRes,
  const ME::DataBlockExport*             nodeDefDataBlock,
  MR::AttribDataCCOverridePropertiesDef* properties,
  uint32_t&                              propertiesIndex,
  MR::AnimSetIndex                       animSetIndex)
{
  bool enabled = isOverrideMotionActive(nodeDefDataBlock);

  if (enabled)
  {
    char attributeName[256];
    int32_t verticalState = -1;
    sprintf_s(attributeName, "MotionVertical_%d", animSetIndex);
    nodeDefDataBlock->readInt(verticalState, attributeName);

    // We only currently have state override for the vertical component of movement.
    properties->m_properties[propertiesIndex] = MR::AttribDataUInt::init(memRes, (uint32_t)verticalState, MR::IS_DEF_ATTRIB_DATA);
    properties->m_propertyTypes[propertiesIndex] = MR::CC_PROPERTY_TYPE_VERTICAL_MOVEMENT_STATE;
    ++propertiesIndex;
  }  
}

//----------------------------------------------------------------------------------------------------------------------
// NodeCCOverrideBuilder functions.
//----------------------------------------------------------------------------------------------------------------------
bool NodeCCOverrideBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeCCOverrideBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_C_C_OVERRIDE);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeCCOverrideBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  // Set only child node ID.
  MR::NodeID sourceNodeID;
  readNodeID(nodeDefDataBlock, "SourceNodeID", sourceNodeID);
  childNodeIDs.push_back(sourceNodeID);

  // control params
  readDataPinChildNodeID(nodeDefDataBlock, "Activate", childNodeIDs, true);
  readDataPinChildNodeID(nodeDefDataBlock, "OverridePosition", childNodeIDs, true);
  readDataPinChildNodeID(nodeDefDataBlock, "OverrideOffset", childNodeIDs, true);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeCCOverrideBuilder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  declareDataPin(netDefCompilationInfo, nodeDefExport, "Activate", 0, true, MR::ATTRIB_SEMANTIC_CP_BOOL);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "OverridePosition", 1, true, MR::ATTRIB_SEMANTIC_CP_VECTOR3);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "OverrideOffset", 2, true, MR::ATTRIB_SEMANTIC_CP_VECTOR3);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeCCOverrideBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  int32_t numAnimSets;
  nodeDefDataBlock->readInt(numAnimSets, "NumAnimSets");
  NMP_VERIFY(numAnimSets > 0);
  
  //---------------------------
  // Allocate space for the NodeDef itself.
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
                                          netDefCompilationInfo, 
                                          1,              // numChildren
                                          3,              // numInputCPConnections
                                          numAnimSets,    // numAnimSetEntries
                                          nodeDefExport);
  //---------------------------
  // Attribute data memory requirements.

  // Animation set agnostic conditions.
  result += calculateConditionsMemoryRequirements(nodeDefDataBlock, processor);
  
  // Animation set specific properties.
  for (MR::AnimSetIndex i = 0; i < (MR::AnimSetIndex)numAnimSets; ++i)
  {
    result += calculatePropertiesMemoryRequirements(nodeDefDataBlock);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeCCOverrideBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_C_C_OVERRIDE, "Expecting node type C_C_OVERRIDE");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  int16_t numAnimSets = readNumAnimSets(nodeDefDataBlock);
 
  //---------------------------
  // Initialise the NodeDef itself
  MR::NodeDef* nodeDef = initCoreNodeDef(
                              memRes,                  
                              netDefCompilationInfo,   
                              1,                // numChildren
                              1,                // max num active child nodes
                              3,                // numInputCPConnections
                              0,                // numOutputCPPins
                              numAnimSets,      // numAnimSetEntries                     
                              nodeDefExport);

  // Flag node as a filter.
  nodeDef->setPassThroughChildIndex(0);
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_FILTER);

  //---------------------------
  // Set child node ID.
  MR::NodeID sourceNodeID;
  readNodeID(nodeDefDataBlock, "SourceNodeID", sourceNodeID);
  nodeDef->setChildNodeID(0, sourceNodeID);

  //---------------------------
  // Initialise the attrib datas.

  // Animation set agnostic conditions.
  initConditions(netDefCompilationInfo, nodeDef, nodeDefDataBlock, processor, memRes);

  // Animation set specific properties.
  for (MR::AnimSetIndex i = 0; i < (MR::AnimSetIndex)numAnimSets; ++i)
  {
    initProperties(nodeDef, nodeDefDataBlock, memRes, i);
  }

  //---------------------------
  // Initialise the nodedef.
  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeCCOverrideBuilder::initTaskQueuingFns(
  MR::NodeDef*                nodeDef,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  NMP::BasicLogger*           logger)
{
  //------------------------------------
  // Shared task function tables
  NMP_ASSERT(netDefCompilationInfo);
  MR::SharedTaskFnTables* taskQueuingFnTables = netDefCompilationInfo->getTaskQueuingFnTables();
  MR::SharedTaskFnTables* outputCPTaskFnTables = netDefCompilationInfo->getOutputCPTaskFnTables();
  MR::QueueAttrTaskFn* taskQueuingFns = (MR::QueueAttrTaskFn*)MR::SharedTaskFnTables::createSharedTaskFnTable();

  const uint32_t numEntries = MR::Manager::getInstance().getNumRegisteredAttribSemantics();
  for (uint32_t i = 0; i < numEntries; ++i)
  {
    // We have no queued tasks, all our processing is done in update connections.
    // All queuing requests get passed on to our child.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, i, FN_NAME(queuePassThroughChild0), logger);
  }

  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);

  //------------------------------------
  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeCCOverrideDeleteInstance), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeCCOverrideUpdateConnections), logger);
  nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(filterNodeFindGeneratingNodeForSemantic), logger);
  nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeCCOverrideInitInstance), logger);
  nodeDef->setMessageHandlerFnId(FN_NAME(nodeCCOverrideHandleMessages), logger);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeCCOverrideBuilder::calculateConditionsMemoryRequirements(
  const ME::DataBlockExport* nodeDefDataBlock,
  AssetProcessor*            processor)
{
  //---------------------------
  // Space for the AttribData container.
  NMP::Memory::Format result = MR::AttribDataCCOverrideConditionsDef::getMemoryRequirements();

  //---------------------------
  // Calculate the number of conditions.
  uint32_t numConditions = 0;
  if (isMessageConditionActive(nodeDefDataBlock))
    ++numConditions;
  if (isInSyncEventRangeConditionActive(nodeDefDataBlock))
    ++numConditions;
  if (isInSubStateConditionActive(nodeDefDataBlock))
    ++numConditions;

  //---------------------------
  // Space for the conditions pointer array.
  result += NMP::Memory::Format(sizeof(MR::TransitConditionDef*) * numConditions, NMP_NATURAL_TYPE_ALIGNMENT);
  
  //---------------------------
  // The conditions themselves.
  TransitConditionDefBuilder* conditionBuilder = NULL;
  if (isMessageConditionActive(nodeDefDataBlock))
  {
    conditionBuilder = processor->getTransitConditionDefBuilder(TRANSCOND_ON_MESSAGE_ID);
    NMP_VERIFY(conditionBuilder);
    result += conditionBuilder->getMemoryRequirements(NULL, NULL, NULL);
  }

  if (isInSyncEventRangeConditionActive(nodeDefDataBlock))
  {
    conditionBuilder = processor->getTransitConditionDefBuilder(TRANSCOND_IN_SYNC_EVENT_RANGE_ID);
    NMP_VERIFY(conditionBuilder);
    result += conditionBuilder->getMemoryRequirements(NULL, NULL, NULL);
  }

  if (isInSubStateConditionActive(nodeDefDataBlock))
  {
    conditionBuilder = processor->getTransitConditionDefBuilder(TRANSCOND_NODE_ACTIVE_ID);
    NMP_VERIFY(conditionBuilder);
    result += conditionBuilder->getMemoryRequirements(NULL, NULL, NULL);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeCCOverrideBuilder::initConditions(
  NetworkDefCompilationInfo* netDefCompilationInfo,
  MR::NodeDef*               nodeDef,
  const ME::DataBlockExport* nodeDefDataBlock,
  AssetProcessor*            processor,
  NMP::Memory::Resource&     memRes)
{
  // Container class.
  MR::AttribDataCCOverrideConditionsDef* conditions = MR::AttribDataCCOverrideConditionsDef::init(memRes, MR::IS_DEF_ATTRIB_DATA);

  //---------------------------
  initAttribEntry(
          nodeDef,
          MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,                           // semantic
          0,                                                               // animSetIndex
          conditions,                                                      // attribData
          MR::AttribDataCCOverrideConditionsDef::getMemoryRequirements()); // attribMemReqs


  //---------------------------
  // Set the priority of this CC override.
  int32_t priority;
  nodeDefDataBlock->readInt(priority, "Priority");
  NMP_ASSERT((priority > -MR::MAX_CC_OVERRIDE_PRIORITY) && (priority < MR::MAX_CC_OVERRIDE_PRIORITY))
  conditions->m_priority = (MR::CCOverridePriority) priority;

  //---------------------------
  // Calculate the number of conditions.
  conditions->m_numConditions = 0;
  if (isMessageConditionActive(nodeDefDataBlock))
    ++conditions->m_numConditions;
  if (isInSyncEventRangeConditionActive(nodeDefDataBlock))
    ++conditions->m_numConditions;
  if (isInSubStateConditionActive(nodeDefDataBlock))
    ++conditions->m_numConditions;

  //---------------------------
  // The conditions pointer array.
  if (conditions->m_numConditions)
  {
    NMP::Memory::Format memReqs = NMP::Memory::Format(sizeof(MR::TransitConditionDef*) * conditions->m_numConditions, NMP_NATURAL_TYPE_ALIGNMENT);
    conditions->m_conditions = static_cast<MR::TransitConditionDef **> (memRes.alignAndIncrement(memReqs));
  }
  else
  {
    conditions->m_conditions = NULL;
  }

  //---------------------------
  // The conditions themselves.
  uint32_t conditionIndex = 0;
  initOnMessageCondition(
                      nodeDef,
                      netDefCompilationInfo,
                      processor,
                      memRes,
                      nodeDefDataBlock,
                      conditions,
                      conditionIndex);
  initInSyncEventRangeCondition(
                      nodeDef,
                      processor,
                      memRes,
                      nodeDefDataBlock,
                      conditions,
                      conditionIndex);
  initInSubStateCondition(
                      processor,
                      memRes,
                      nodeDefDataBlock,
                      conditions,
                      conditionIndex);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeCCOverrideBuilder::calculatePropertiesMemoryRequirements(
  const ME::DataBlockExport* nodeDefDataBlock)
{
  //---------------------------
  // Space for the AttribData container.
  NMP::Memory::Format result = MR::AttribDataCCOverridePropertiesDef::getMemoryRequirements();

  //---------------------------
  // Calculate the number of parameters.
  uint32_t numProperties = 0;
  if (isOverrideScaleActive(nodeDefDataBlock))
    numProperties += 2; // Height, Radius.
  if (isOverrideOffsetActive(nodeDefDataBlock))
    ++numProperties;    // Translation
  if (isOverrideMotionActive(nodeDefDataBlock))
    ++numProperties;    // State.
  
  //---------------------------
  // Space for the property types array.
  result += NMP::Memory::Format(sizeof(MR::CCPropertyType) * numProperties, NMP_NATURAL_TYPE_ALIGNMENT);

  //---------------------------
  // Space for the property pointers array.
  result += NMP::Memory::Format(sizeof(MR::AttribData*) * numProperties, NMP_NATURAL_TYPE_ALIGNMENT);

 
  //---------------------------
  // The properties themselves.  
  if (isOverrideScaleActive(nodeDefDataBlock))
  {
    // Height, Radius.
    result += MR::AttribDataFloat::getMemoryRequirements();
    result += MR::AttribDataFloat::getMemoryRequirements();
  }

  if (isOverrideOffsetActive(nodeDefDataBlock))
  {
    // Translation.
    result += MR::AttribDataVector3::getMemoryRequirements();
    result += MR::AttribDataUInt::getMemoryRequirements();
  }

  if (isOverrideMotionActive(nodeDefDataBlock))
  {
    // State.
    result += MR::AttribDataUInt::getMemoryRequirements();
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeCCOverrideBuilder::initProperties(
  MR::NodeDef*               nodeDef,
  const ME::DataBlockExport* nodeDefDataBlock,
  NMP::Memory::Resource&     memRes,
  MR::AnimSetIndex           animSetIndex)
{
  // Container class.
  MR::AttribDataCCOverridePropertiesDef* properties = MR::AttribDataCCOverridePropertiesDef::init(memRes, MR::IS_DEF_ATTRIB_DATA);

  //---------------------------
  initAttribEntry(
            nodeDef,
            MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET,                  // semantic
            animSetIndex,                                                    // animSetIndex
            properties,                                                      // attribData
            MR::AttribDataCCOverridePropertiesDef::getMemoryRequirements()); // attribMemReqs

  //---------------------------
  // Calculate the number of parameters.
  properties->m_numProperties = 0;
  if (isOverrideScaleActive(nodeDefDataBlock))
    properties->m_numProperties += 2; // Height, Radius.
  if (isOverrideOffsetActive(nodeDefDataBlock))
    ++properties->m_numProperties;    // Translation.
  if (isOverrideMotionActive(nodeDefDataBlock))
    ++properties->m_numProperties;    // State.

  //---------------------------
  if (properties->m_numProperties)
  {
    // The property types array.
    NMP::Memory::Format memReqs = NMP::Memory::Format(sizeof(MR::CCPropertyType) * properties->m_numProperties, NMP_NATURAL_TYPE_ALIGNMENT);
    properties->m_propertyTypes = static_cast<MR::CCPropertyType*> (memRes.alignAndIncrement(memReqs));

    //The property pointers array.
    memReqs = NMP::Memory::Format(sizeof(MR::AttribData*) * properties->m_numProperties, NMP_NATURAL_TYPE_ALIGNMENT);
    properties->m_properties = static_cast<MR::AttribData**> (memRes.alignAndIncrement(memReqs));
  }
  else
  {
    properties->m_propertyTypes = NULL;
    properties->m_properties = NULL;
  }

  //---------------------------
  // The properties themselves.  
  uint32_t propertiesIndex = 0;
  initOverrideScaleProperties(
                  memRes,
                  nodeDefDataBlock,
                  properties,
                  propertiesIndex,
                  animSetIndex);
  initOverrideOffsetProperties(
                  memRes,
                  nodeDefDataBlock,
                  properties,
                  propertiesIndex,
                  animSetIndex);
  initOverrideMotionProperties(
                  memRes,
                  nodeDefDataBlock,
                  properties,
                  propertiesIndex,
                  animSetIndex);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
