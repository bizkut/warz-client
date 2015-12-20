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
#include "NodeBehaviourBuilder.h"
#include "assetProcessor/NodeBuilderUtils.h"
#include "physics/Nodes/mrNodePhysics.h"
#include "physics/Nodes/mrNodePhysicsGrouper.h"
#include "morpheme/Nodes/mrNodeFilterTransforms.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/mrCommonTaskQueuingFns.h"
#include "euphoria/Nodes/erNodeBehaviour.h"

#include "morpheme/mrNetworkDef.h"
#include "morpheme/mrAttribData.h"
#include "euphoria/erAttribData.h"
#include <tchar.h>
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeBehaviourBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeBehaviourBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_BEHAVIOUR);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_CP_FLOAT);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_CP_BOOL);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  semanticLookupTable->addLookupIndex(ER::ATTRIB_SEMANTIC_BEHAVIOUR_PARAMETERS);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_EMITTED_MESSAGES_MAP);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
static MR::AttribDataSemanticEnum convertToSemanticEnum(const std::string& str)
{
  if (str == "ATTRIB_SEMANTIC_CP_BOOL")
    return MR::ATTRIB_SEMANTIC_CP_BOOL;
  else if (str == "ATTRIB_SEMANTIC_CP_UINT")
    return MR::ATTRIB_SEMANTIC_CP_UINT;
  else if (str == "ATTRIB_SEMANTIC_CP_PHYSICS_OBJECT_POINTER")
    return MR::ATTRIB_SEMANTIC_CP_PHYSICS_OBJECT_POINTER;
  else if (str == "ATTRIB_SEMANTIC_CP_INT")
    return MR::ATTRIB_SEMANTIC_CP_INT;
  else if (str == "ATTRIB_SEMANTIC_CP_FLOAT")
    return MR::ATTRIB_SEMANTIC_CP_FLOAT;
  else if (str == "ATTRIB_SEMANTIC_CP_VECTOR3")
    return MR::ATTRIB_SEMANTIC_CP_VECTOR3;
  else if (str == "ATTRIB_SEMANTIC_CP_VECTOR4")
    return MR::ATTRIB_SEMANTIC_CP_VECTOR4;  
  else 
  {
    NMP_ASSERT_FAIL_MSG("Unable to convert %s to AttribDataSemanticEnum", str.c_str());
    return MR::ATTRIB_SEMANTIC_NA;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NodeBehaviourBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();

  uint32_t numAnimSets = animLibraryExport->getNumAnimationSets();
  NMP_VERIFY(numAnimSets > 0);

  //---------------------------
  uint32_t numBehaviourControlParameterInputs = 0;
  nodeDefDataBlock->readUInt(numBehaviourControlParameterInputs, "numBehaviourControlParameterInputs");
  
  uint32_t numBehaviourNodeAnimationInputs = 0;
  nodeDefDataBlock->readUInt(numBehaviourNodeAnimationInputs, "numBehaviourNodeAnimationInputs");

  //---------------------------
  // Animation set dependent attribute data
  MR::NodeID nodeID;
  CHAR paramName[256];
  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < numAnimSets; ++animSetIndex)
  {
    // Animation inputs
    for (uint32_t i = 0; i < numBehaviourNodeAnimationInputs; ++i)
    {
      sprintf_s(paramName, "BehaviourNodeAnimationInput_%d", i);
      if (readNodeID(nodeDefDataBlock, paramName, nodeID, true))
      {
        if (nodeID != MR::INVALID_NODE_ID)
        {
          childNodeIDs.push_back(nodeID);
        }
      }
    }
  }

  //---------------------------
  // Control parameters
  for (uint32_t i = 0; i < numBehaviourControlParameterInputs; ++i)
  {
    sprintf_s(paramName, "BehaviourControlParameterInputID_%d", i);
    readDataPinChildNodeID(nodeDefDataBlock, paramName, childNodeIDs, true);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NodeBehaviourBuilder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  // Process the control parameters
  CHAR paramName[256];
  uint32_t numInputCPs = 0;
  const ME::DataBlockExport* dataBlock = nodeDefExport->getDataBlock();
  dataBlock->readUInt(numInputCPs, "numBehaviourControlParameterInputs");

  for (uint32_t i = 0; i < numInputCPs; ++i)
  {      
    // Get the datatype, e.g. float, int etc.
    std::string controlParameterType;
    sprintf_s(paramName, "BehaviourControlParameterInputType_%d", i);
#if defined(NMP_ENABLE_ASSERTS) || NM_ENABLE_EXCEPTIONS
    bool readResult =
#endif
      dataBlock->readString(controlParameterType, paramName);
    NMP_VERIFY(readResult);
    MR::AttribDataSemanticEnum semantic = convertToSemanticEnum(controlParameterType);

    // An optionally connected input CP.
    sprintf_s(paramName, "BehaviourControlParameterInputID_%d", i);
    declareDataPin(netDefCompilationInfo, nodeDefExport, paramName, (MR::PinIndex)i, true, (MR::AttribDataSemantic)semantic);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeBehaviourBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();

  uint32_t numAnimSets = animLibraryExport->getNumAnimationSets();
  NMP_VERIFY(numAnimSets > 0);

  // Note that the parameter names are all autogenerated so we don't need to worry about them being
  // too long.
  CHAR paramName[256];

  //---------------------------
  uint32_t numChildNodes = 0;
  uint32_t numInputCPs = 0;
  nodeDefDataBlock->readUInt(numChildNodes, "numBehaviourNodeAnimationInputs");
  nodeDefDataBlock->readUInt(numInputCPs, "numBehaviourControlParameterInputs");

  // Count the active Animation inputs (i.e. those that are connected)
  uint32_t numActiveChildNodes = 0;
  for (uint32_t i = 0; i < numChildNodes; ++i)
  {
    int32_t childNodeID = MR::INVALID_NODE_ID;
    sprintf_s(paramName, "BehaviourNodeAnimationInput_%d", i);
    nodeDefDataBlock->readInt(childNodeID, paramName);
    if ((MR::NodeID)childNodeID != MR::INVALID_NODE_ID)
      ++numActiveChildNodes;
  }

  uint32_t numBehaviourControlParameterOutputs = 0;
  nodeDefDataBlock->readUInt(numBehaviourControlParameterOutputs, "numBehaviourControlParameterOutputs");

  uint32_t numMessageSlots = 0xFFFFFFFF;
  nodeDefDataBlock->readUInt(numMessageSlots, "NumMessageSlots");
  NMP_VERIFY((numMessageSlots != 0xFFFFFFFF) && (numMessageSlots < 33));

  numBehaviourControlParameterOutputs =
    (numMessageSlots > 0) ? numBehaviourControlParameterOutputs + 1 : numBehaviourControlParameterOutputs;

  //---------------------------
  // Allocate space for the NodeDef itself.
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo,
    numActiveChildNodes,
    numInputCPs,
    numAnimSets,
    nodeDefExport);

  //---------------------------
  // Add space for behaviour setup attribute data.
  result += MR::AttribDataFloat::getMemoryRequirements();
  result += MR::AttribDataBool::getMemoryRequirements();
  result += ER::AttribDataBehaviourSetup::getMemoryRequirements(numChildNodes);

  //---------------------------
  // Behaviour parameters - per anim set
  for (uint32_t animSetIndex = 0 ; animSetIndex < numAnimSets ; ++animSetIndex)
  {
    uint32_t numInts = 0;
    uint32_t numFloats = 0;
    uint32_t numUInt64s = 0;
    nodeDefDataBlock->readUInt(numInts,   "numBehaviourInts");
    nodeDefDataBlock->readUInt(numFloats, "numBehaviourFloats");
    nodeDefDataBlock->readUInt(numUInt64s, "numBehaviourUInt64s");
    result += ER::AttribDataBehaviourParameters::getMemoryRequirements(
      numInts,
      numFloats,
      numUInt64s,
      numInputCPs,
      numBehaviourControlParameterOutputs);
  }

  //---------------------------
  // Emitted messages
  if (numMessageSlots > 0)
  {
    result += MR::AttribDataEmittedMessagesMap::getMemoryRequirements(numMessageSlots);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeBehaviourBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_BEHAVIOUR, "Expecting behaviour node type");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();

  uint32_t numAnimSets = animLibraryExport->getNumAnimationSets();
  NMP_VERIFY(numAnimSets > 0);

  uint32_t numMessageSlots = 0xFFFFFFFF;
  nodeDefDataBlock->readUInt(numMessageSlots, "NumMessageSlots");
  NMP_VERIFY(numMessageSlots != 0xFFFFFFFF);
  NMP_VERIFY(numMessageSlots < 33); // Runtime can only currently deal with a maximum of 32 emitted message slots from any one node.

  // Note that the parameter names are all autogenerated so we don't need to worry about them being
  // too long.
  CHAR paramName[256];

  //---------------------------
  uint32_t numChildNodes = 0;
  uint32_t numInputCPs = 0;
  nodeDefDataBlock->readUInt(numChildNodes, "numBehaviourNodeAnimationInputs");
  nodeDefDataBlock->readUInt(numInputCPs, "numBehaviourControlParameterInputs");

  // Count the active Animation inputs (i.e. those that are connected)
  uint32_t numActiveChildNodes = 0;
  for (uint32_t i = 0; i < numChildNodes; ++i)
  {
    int32_t childNodeID = MR::INVALID_NODE_ID;
    sprintf_s(paramName, "BehaviourNodeAnimationInput_%d", i);
    nodeDefDataBlock->readInt(childNodeID, paramName);
    if ((MR::NodeID)childNodeID != MR::INVALID_NODE_ID)
      ++numActiveChildNodes;
  }

  uint32_t numBehaviourControlParameterOutputs = 0;
  nodeDefDataBlock->readUInt(numBehaviourControlParameterOutputs, "numBehaviourControlParameterOutputs");

  // Add a dummy output control parameter to store the emittedRequestUpdate function
  // We leave numBehaviourControlParameterOutputs untouched and use this when initialising the behaviour
  // node def.  This prevents the behaviour node from trying to handle the emitted message entry in
  // the output control param list as a control param.
  uint32_t numOutputControlParams = numBehaviourControlParameterOutputs;
  if (numMessageSlots > 0)
  {
    ++numOutputControlParams;
  }

  //---------------------------
  // Initialise the NodeDef itself
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,
    netDefCompilationInfo,
    numActiveChildNodes,                            // numChildren
    (uint16_t)numActiveChildNodes,                  // max num active child nodes
    numInputCPs,                                    // numInputCPConnections
    (uint8_t)numOutputControlParams,                // numOutputCPPins
    (uint16_t)numAnimSets,                          // numAnimSetEntries
    nodeDefExport);

  MR::NodeDef::NodeFlags nodeFlags(0);

  // Flag node as being physical and as a filter node if we have a connected child.
  if (numChildNodes > 0)
  {
    nodeDef->setPassThroughChildIndex(0);
    nodeFlags.set(MR::NodeDef::NODE_FLAG_IS_FILTER);
  }

  nodeFlags.set(MR::NodeDef::NODE_FLAG_IS_PHYSICAL);
  nodeFlags.set(MR::NodeDef::NODE_FLAG_IS_BEHAVIOURAL);
  nodeFlags.set(MR::NodeDef::NODE_FLAG_REQUIRES_SEPARATE_TRAJECTORY_AND_TRANSFORMS);

  nodeDef->setNodeFlags(nodeFlags);

  //---------------------------
  // Initialise the attrib datas.

  //---------------------------
  // Set up a constant input to use if a weight is not connected. Also used for initialisationFraction
  // Add a constant blend weight attribute because the node is not connected to a float control param.
  MR::AttribDataFloat* fixedWeightAttribData = MR::AttribDataFloat::init(memRes, 1.0f, MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_CP_FLOAT,                   // semantic
    0,                                              // animSetIndex
    fixedWeightAttribData,                          // attribData
    MR::AttribDataFloat::getMemoryRequirements());  // attribMemReqs

  // Set up a constant input to use for waiting for tasks.
  MR::AttribDataBool* taskWaitAttribData = MR::AttribDataBool::init(memRes, true, MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_CP_BOOL,                    // semantic
    0,                                              // animSetIndex
    taskWaitAttribData,                             // attribData
    MR::AttribDataBool::getMemoryRequirements());   // attribMemReqs

  ER::AttribDataBehaviourSetup* behaviourSetup = ER::AttribDataBehaviourSetup::init(memRes, numChildNodes, MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,                // semantic
    0,                                                    // animSetIndex
    behaviourSetup,                                         // attribData
    ER::AttribDataBehaviourSetup::getMemoryRequirements(numChildNodes)); // attribMemReqs

  //---------------------------
  // Animation set dependent attribute data
  ER::AttribDataBehaviourParameters* behaviourParameters = 0;
  bool hasAnimationInputConnection = false;
  uint32_t iChildNodeID = 0;
  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < numAnimSets; ++animSetIndex)
  {
    iChildNodeID = 0;

    //---------------------------
    // create and initialise behaviour parameters
    uint32_t numInts = 0;
    uint32_t numFloats = 0;
    uint32_t numUint64s = 0;
    nodeDefDataBlock->readUInt(numInts,   "numBehaviourInts");
    nodeDefDataBlock->readUInt(numFloats, "numBehaviourFloats");
    nodeDefDataBlock->readUInt(numUint64s, "numBehaviourUInt64s");

    uint32_t numInputCPInts = 0;
    uint32_t numInputCPFloats = 0;
    uint32_t numInputCPUint64s = 0;
    uint32_t numInputCPVector3s = 0;
    nodeDefDataBlock->readUInt(numInputCPInts,   "numInputCPInts");
    nodeDefDataBlock->readUInt(numInputCPFloats, "numInputCPFloats");
    nodeDefDataBlock->readUInt(numInputCPUint64s, "numInputCPUInt64s");
    nodeDefDataBlock->readUInt(numInputCPVector3s, "numInputCPVector3s");

    // create the param attrib data
    behaviourParameters = ER::AttribDataBehaviourParameters::init(
      memRes,
      numInts,
      numFloats,
      numUint64s,
      numInputCPInts,
      numInputCPFloats,
      numInputCPUint64s,
      numInputCPVector3s,
      numInputCPs,
      numBehaviourControlParameterOutputs,
      MR::IS_DEF_ATTRIB_DATA);
    initAttribEntry(
      nodeDef,
      ER::ATTRIB_SEMANTIC_BEHAVIOUR_PARAMETERS,                 // semantic
      animSetIndex,                                             // animSetIndex
      behaviourParameters,                                      // attribData
      ER::AttribDataBehaviourParameters::getMemoryRequirements(
        numInts,
        numFloats,
        numUint64s,
        numInputCPs,
        numBehaviourControlParameterOutputs));                  // attribMemReqs

    // ints
    {
      int32_t value;
      for (uint32_t i = 0; i < numInts; ++i)
      {
        sprintf_s(paramName, "Int_%d_%d", i, animSetIndex);
        if (nodeDefDataBlock->readInt(value, paramName))
        {
          behaviourParameters->m_ints->m_values[i] = value;
        }
        else
        {
          behaviourParameters->m_ints->m_values[i] = 0;
        }
      }
    }
    // floats
    {
      float value;
      for (uint32_t i = 0; i < numFloats; ++i)
      {
        sprintf_s(paramName, "Float_%d_%d", i, animSetIndex);
        if (nodeDefDataBlock->readFloat(value, paramName))
        {
          behaviourParameters->m_floats->m_values[i] = value;
        }
        else
        {
          behaviourParameters->m_floats->m_values[i] = 0.0f;
        }
      }
    }
    // uint64s
    {
      uint64_t value;
      for (uint32_t i = 0; i < numUint64s; ++i)
      {
        sprintf_s(paramName, "UInt64_%d_%d", i, animSetIndex);
        if (nodeDefDataBlock->readUInt64(value, paramName))
        {
          behaviourParameters->m_uint64s->m_values[i] = value;
        }
        else
        {
          behaviourParameters->m_uint64s->m_values[i] = 0;
        }
      }
    }

    // Animation inputs
    int32_t childNodeID;
    for (uint32_t i = 0; i < numChildNodes; ++i)
    {
      sprintf_s(paramName, "BehaviourNodeAnimationInput_%d", i);
#if defined(NMP_ENABLE_ASSERTS) || NM_ENABLE_EXCEPTIONS
      bool readResult =
#endif
        nodeDefDataBlock->readInt(childNodeID, paramName);
      NMP_VERIFY(readResult);

      // Add the animation input to the child node array, but only if it's a valid child
      if ((MR::NodeID)childNodeID != MR::INVALID_NODE_ID)
      {
        NMP_VERIFY(iChildNodeID < nodeDef->getNumChildNodes());
        nodeDef->setChildNodeID(iChildNodeID, (MR::NodeID) childNodeID);
        ++iChildNodeID;

        hasAnimationInputConnection = true;
      }

      // Also always store it so that the behaviour tasks know which inputs are valid.
      behaviourSetup->m_childNodeIDs->m_values[i] = childNodeID;
    }
    NMP_VERIFY(iChildNodeID == nodeDef->getNumChildNodes());

    // Read the control parameter input type data
    std::string controlParameterType;
    for (uint32_t i = 0; i < numInputCPs; ++i)
    {
      // This is the datatype, e.g. float, int etc.  It maps to ER::AttribDataBehaviourParameters::ControlParameterType
      sprintf_s(paramName, "BehaviourControlParameterInputType_%d", i);
#if defined(NMP_ENABLE_ASSERTS) || NM_ENABLE_EXCEPTIONS
      bool readResult =
#endif
        nodeDefDataBlock->readString(controlParameterType, paramName);
      NMP_VERIFY(readResult);
      behaviourParameters->m_controlParameterInputsTypes->m_values[i] = convertToSemanticEnum(controlParameterType);
    }

    // output control parameters
    for (uint32_t i = 0; i < numBehaviourControlParameterOutputs; ++i)
    {
      sprintf_s(paramName, "BehaviourControlParameterOutputType_%d", i);
#if defined(NMP_ENABLE_ASSERTS) || NM_ENABLE_EXCEPTIONS
      bool readResult =
#endif
        nodeDefDataBlock->readString(controlParameterType, paramName);
      NMP_VERIFY(readResult);
      behaviourParameters->m_controlParameterOutputsTypes->m_values[i] = convertToSemanticEnum(controlParameterType);
    }

    //---------------------------
    // initialise the attrib data properly
    behaviourSetup->m_behaviourID = -1;
#ifdef NMP_ENABLE_ASSERTS
    bool readResult = nodeDefDataBlock->readInt(behaviourSetup->m_behaviourID, "BehaviourID");
    NMP_VERIFY_MSG(readResult, "Could not find behaviour ID for behaviour node %i", nodeDefExport->getNodeID());
#else
    nodeDefDataBlock->readInt(behaviourSetup->m_behaviourID, "BehaviourID");
#endif
  }

  //---------------------------
  // Initialise the emitted requests map attrib data.
  if (numMessageSlots > 0)
  {
    MR::AttribDataEmittedMessagesMap* emittedRequestMapAttribData =
      MR::AttribDataEmittedMessagesMap::init(memRes, numMessageSlots, MR::IS_DEF_ATTRIB_DATA);

    //---------------------------
    // Initialise each map entry
    for (uint32_t i = 0; i < numMessageSlots; ++i)
    {
      initMessageMapEntry(i, emittedRequestMapAttribData, netDefExport, nodeDefDataBlock);
    }

    // Add to the nodes AttribData array.
    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_EMITTED_MESSAGES_MAP,                                  // semantic
      0,                                                                         // animSetIndex
      emittedRequestMapAttribData,                                               // attribData
      MR::AttribDataEmittedMessagesMap::getMemoryRequirements(numMessageSlots)); // attribMemReqs
  }

  //---------------------------
  // Initialise the task function tables
  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    behaviourParameters,
    processor->getMessageLogger(),
    numMessageSlots > 0,
    hasAnimationInputConnection);

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeBehaviourBuilder::initTaskQueuingFns(
  MR::NodeDef*                nodeDef,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  ER::AttribDataBehaviourParameters* behaviourParameters,
  NMP::BasicLogger*           logger,
  bool                        emitMessages,
  bool                        hasAnimationInputConnection)
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
    // Most queuing messages get passed on to our child.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, i, FN_NAME(queuePassThroughChild0), logger);
  }

  nodeDef->setTaskQueuingFnId(
    taskQueuingFns,
    MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER,
    ER_FN_NAME(nodeBehaviourQueueUpdateTransformsPostPhysics),
    logger);
  nodeDef->setTaskQueuingFnId(
    taskQueuingFns,
    MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM,
    ER_FN_NAME(nodeBehaviourQueueUpdateTrajectory),
    logger);
  nodeDef->setTaskQueuingFnId(
    taskQueuingFns,
    MR::ATTRIB_SEMANTIC_PRE_PHYSICS_TRANSFORMS,
    ER_FN_NAME(nodeBehaviourQueueUpdateTransformsPrePhysics),
    logger);
  nodeDef->setTaskQueuingFnId(
    taskQueuingFns,
    MR::ATTRIB_SEMANTIC_OUTPUT_MASK,
    FN_NAME(grouperQueuePassThroughFromParent),
    logger);

  if (!hasAnimationInputConnection)
  {
    nodeDef->setTaskQueuingFnId(
      taskQueuingFns,
      MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER,
      FN_NAME(queueInitSampledEventsTrack),
      logger);
    nodeDef->setTaskQueuingFnId(
      taskQueuingFns,
      MR::ATTRIB_SEMANTIC_SYNC_EVENT_TRACK,
      FN_NAME(queueInitUnitLengthSyncEventTrack),
      logger);
    nodeDef->setTaskQueuingFnId(
      taskQueuingFns,
      MR::ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET,
      FN_NAME(queueInitEmptyEventTrackDurationSet),
      logger);
  }

  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);

  if (behaviourParameters && behaviourParameters->m_controlParameterOutputsTypes->m_numValues > 0)
  {
    MR::OutputCPTask* outputCPTaskFns = (MR::OutputCPTask*)MR::SharedTaskFnTables::createSharedTaskFnTable();

    uint32_t numPins = behaviourParameters->m_controlParameterOutputsTypes->m_numValues;
    for (uint8_t iPin = 0 ; iPin < numPins ; ++iPin)
    {
      MR::AttribDataSemanticEnum attribType = 
        attribType = (MR::AttribDataSemanticEnum) (behaviourParameters->m_controlParameterOutputsTypes->m_values[iPin]);
      nodeDef->setOutputCPTaskFnId(outputCPTaskFns, iPin, attribType, ER_FN_NAME(nodeBehaviorOutputCPUpdate), logger);
    }
    if (emitMessages)
    {
      nodeDef->setOutputCPTaskFnId(
        outputCPTaskFns,
        (uint8_t)numPins,
        MR::ATTRIB_SEMANTIC_EMITTED_MESSAGES,
        ER_FN_NAME(nodeBehaviourEmitMessageUpdate),
        logger);
    }
    nodeDef->registerOutputCPTasks(outputCPTaskFnTables, outputCPTaskFns);
  }
  else
  {
    if (emitMessages)
    {
      MR::OutputCPTask* outputCPTaskFns = (MR::OutputCPTask*)MR::SharedTaskFnTables::createSharedTaskFnTable();
      nodeDef->setOutputCPTaskFnId(
        outputCPTaskFns,
        0,
        MR::ATTRIB_SEMANTIC_EMITTED_MESSAGES,
        ER_FN_NAME(nodeBehaviourEmitMessageUpdate),
        logger);
      nodeDef->registerOutputCPTasks(outputCPTaskFnTables, outputCPTaskFns);
    }
    else
    {
      nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);
    }
  }

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);

  //------------------------------------
  // Other manager registered functions.
  //---------------------------
  if (nodeDef->getNumReflexiveCPPins() > 0)
  {
    nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeInitPinAttribDataInstance), logger);
  }
  nodeDef->setDeleteNodeInstanceId(ER_FN_NAME(nodeBehaviourDeleteInstance), logger);
  nodeDef->setUpdateConnectionsFnId(ER_FN_NAME(nodeBehaviourUpdateConnections), logger);
  nodeDef->setFindGeneratingNodeForSemanticFnId(ER_FN_NAME(nodeBehaviourFindGeneratingNodeForSemantic), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
