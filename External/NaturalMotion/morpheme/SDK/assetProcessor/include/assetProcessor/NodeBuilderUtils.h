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
#ifndef MR_OUTPUT_CONTROL_PARAM_NODE_BUILDER_UTILS_H
#define MR_OUTPUT_CONTROL_PARAM_NODE_BUILDER_UTILS_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
#include "morpheme/mrNodeDef.h"
#include "morpheme/mrNetworkDef.h"
#include "assetProcessor/NodeBuilder.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// Reads an int32_t value from the data block (which is what connect writes), validates and converts int16_t.
NM_INLINE int16_t readNumAnimSets(const ME::DataBlockExport* dataBlock)
{
  int32_t serialisedValue = -1;
  dataBlock->readInt(serialisedValue, "NumAnimSets");
  NMP_VERIFY(serialisedValue > 0 && serialisedValue < 0xFFFF);
  return (uint16_t) serialisedValue;
}

//----------------------------------------------------------------------------------------------------------------------
// Reads an int32_t value from the data block (which is what connect writes), validates and converts to a MR::NodeID.
NM_INLINE bool readNodeID(
  const ME::DataBlockExport* dataBlock,        // We are reading from.
  const char*                nodeIDLabel,      // We are searching for.
  MR::NodeID&                outNodeID,        // Output
  bool                       optional = false) // The nodeIDLabel can legitimately not exist.
{
  outNodeID = MR::INVALID_NODE_ID;
  int32_t serialisedNodeID = -1;

  if(dataBlock->readNetworkNodeId(serialisedNodeID, nodeIDLabel))
  {
    // Only use 16bits for storing node IDs.
    outNodeID = (MR::NodeID)serialisedNodeID;

#ifdef NMP_ENABLE_ASSERTS
    // For functional pins, if invalid, check optional
    if (serialisedNodeID == MR::INVALID_NODE_ID)
    {
      NMP_VERIFY(optional); // If we haven't found the label then it must be optional.
    }
#endif
    return true;
  }

#ifdef NMP_ENABLE_ASSERTS
  // If undefined, check optional
  NMP_VERIFY(optional); // If we haven't found the label then it must be optional.
#else
  (void)optional;
#endif

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
// Reads an int32_t value for the NodeID from the data block if an id has been serialized.  This ID is added to the list 
// of childNodeIds if it is a valid ID. 
NM_INLINE bool readDataPinChildNodeID(
  const ME::DataBlockExport* dataBlock,        // We are reading from.
  const char*                nodeIDLabel,      // We are searching for.
  std::vector<MR::NodeID>&   childNodeIDs,     // Output
  bool                       optional = false) // The nodeIDLabel can legitimately not exist.
{
  MR::NodeID nodeID;
  if(readNodeID(dataBlock, nodeIDLabel, nodeID, optional))
  {
    if(nodeID != MR::INVALID_NODE_ID)
    {
      childNodeIDs.push_back(nodeID);
    }
    return true;
  }
  return false;
}


//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool hasElement(
  const ME::DataBlockExport* dataBlock,  // We are reading from.
  const char*                label)      // We are searching for.
{
  return ((ME::DataBlockExportXML*)dataBlock)->hasElement(label);
}

//----------------------------------------------------------------------------------------------------------------------
// Reads an int32_t value for the NodeID from the data block if an id has been serialized.  This ID is added to the list 
// of childNodeIds if it is a valid ID. 
NM_INLINE bool readVariableTypeDataPinChildNodeID(
  const ME::DataBlockExport* dataBlock,        // We are reading from.
  const char*                nodeIDLabel,      // We are searching for.
  std::vector<MR::NodeID>&   childNodeIDs,     // Output
  bool                       optional = false) // The nodeIDLabel can legitimately not exist.
{
  CHAR paramName[256];

  for (uint32_t semanticCandidate = 0; semanticCandidate < NUM_BUILT_IN_ATTRIB_DATA_SEMANTICS; ++semanticCandidate)
  {
    sprintf_s(paramName, "%s_%s", nodeIDLabel, BUILT_IN_ATTRIB_DATA_SEMANTIC_NAMES[semanticCandidate].m_typeName);
    MR::NodeID nodeID;
    if(readNodeID(dataBlock, paramName, nodeID, true))
    {
      if(nodeID != MR::INVALID_NODE_ID)
      {
        childNodeIDs.push_back(nodeID);
      }
      return true;
    }
  }

#ifdef NMP_ENABLE_ASSERTS
  // If undefined, check optional
  NMP_VERIFY(optional); // If we haven't found the label then it must be optional.
#else
  (void)optional;
#endif

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void declareDataPin(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const char*                 nodeIDLabel,      // We are searching for.
  MR::PinIndex                inputPinIndex,
  bool                        optional = false, // The nodeIDLabel can legitimately not exist.
  MR::AttribDataSemantic      semantic = MR::ATTRIB_SEMANTIC_NA)
{
  // Determine if this pin's attrib data is anim set specific
  uint32_t numAnimSets = 1;
  bool readPerAnimSetData = false;
  CHAR paramName[256];
  sprintf_s(paramName, "%s_%d", nodeIDLabel, 0);

  ME::DataBlockExportXML* dataBlock = (ME::DataBlockExportXML*)nodeDefExport->getDataBlock();
  if (dataBlock->hasElement(paramName))
  {
    // There is anim-set specific versions of this nodeIDLabel, so we should make an attribute per anim set
    numAnimSets = netDefCompilationInfo->getNumAnimSets();
    readPerAnimSetData = true;
  }

  netDefCompilationInfo->setNodeDataPinInfo(nodeDefExport->getNodeID(), nodeIDLabel, semantic, inputPinIndex, numAnimSets, readPerAnimSetData, optional);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void declareVariableTypeDataPin(
   NetworkDefCompilationInfo*  netDefCompilationInfo,
   const ME::NodeExport*       nodeDefExport,
   const char*                 nodeIDLabel,      // We are searching for.
   MR::PinIndex                inputPinIndex,
   bool                        optional, // The nodeIDLabel can legitimately not exist.
   MR::AttribDataSemantic*     possibleSemantics,
   uint32_t                    numPossibleSemantics
   )
{
  CHAR paramName[256];

  ME::DataBlockExportXML* dataBlock = (ME::DataBlockExportXML*)nodeDefExport->getDataBlock();
  for (uint32_t semanticCandidate = 0; semanticCandidate < numPossibleSemantics; ++semanticCandidate)
  {
    MR::AttribDataSemantic curSemantic = possibleSemantics[semanticCandidate];
    for(uint32_t builtInIndex = 0; builtInIndex <  NUM_BUILT_IN_ATTRIB_DATA_SEMANTICS; ++ builtInIndex)
    {
      if(BUILT_IN_ATTRIB_DATA_SEMANTIC_NAMES[builtInIndex].m_semantic == curSemantic)
      {
        // Determine if this pin's attrib data is anim set specific
        uint32_t numAnimSets = 1;
        bool readPerAnimSetData = false;
        // test without animSets
        sprintf_s(paramName, "%s_%s",nodeIDLabel, BUILT_IN_ATTRIB_DATA_SEMANTIC_NAMES[builtInIndex].m_typeName);
        if (dataBlock->hasElement(paramName))
        {
          netDefCompilationInfo->setNodeDataPinInfo(
            nodeDefExport->getNodeID(),
            paramName, 
            curSemantic,
            inputPinIndex,
            numAnimSets,
            readPerAnimSetData,
            optional);
          return;
        }
        else
        {
          // test with animSets
          sprintf_s(paramName, "%s_%s_%d", nodeIDLabel,BUILT_IN_ATTRIB_DATA_SEMANTIC_NAMES[builtInIndex].m_typeName, 0);
          if (dataBlock->hasElement(paramName))
          {
            // There is anim-set specific versions of this nodeIDLabel, so we should make an attribute per anim set
            numAnimSets = netDefCompilationInfo->getNumAnimSets();
            readPerAnimSetData = true;

            sprintf_s(paramName, "%s_%s",nodeIDLabel, BUILT_IN_ATTRIB_DATA_SEMANTIC_NAMES[builtInIndex].m_typeName);

            netDefCompilationInfo->setNodeDataPinInfo(
              nodeDefExport->getNodeID(),
              paramName, 
              curSemantic,
              inputPinIndex,
              numAnimSets,
              readPerAnimSetData,
              optional);

            return;
          }
        }
      }
    }
  }
  NMP_ASSERT_FAIL_MSG("None of the allowed data types were encountered on a declared pin.");
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool readInputPinConnectionDetails(
  const ME::DataBlockExport* nodeDefDataBlock,
  const char*                connectionLabel,            // How the connection has been labeled in the xml.
  MR::NodeDef*               nodeDef,                    // The node def we are initialising.
  MR::PinIndex               inputCpPinIndex,            // The index of the input control param connection that we are initialising.
  bool                       optionalConnection)         // This connection can legitimately not exist.
{
  int32_t serialisedCPNodeID = -1; // Value set when not connected
  int32_t connectedPinIndex = MR::INVALID_PIN_INDEX;
  MR::NodeID connectedCPNodeID = MR::INVALID_NODE_ID;

  if(nodeDefDataBlock->readNetworkNodeId(serialisedCPNodeID, connectionLabel))
  {
    connectedCPNodeID = (MR::NodeID)serialisedCPNodeID;
    nodeDefDataBlock->readIntAttribute(connectedPinIndex, connectionLabel, "pinIndex");
    NMP_VERIFY(connectedPinIndex != MR::INVALID_PIN_INDEX);
    NMP_VERIFY(connectedPinIndex < 0xFF); // Currently only have 8bits for storing count of input control params.

    // set the connection
    nodeDef->setInputCPConnection(inputCpPinIndex, connectedCPNodeID, (MR::PinIndex&)connectedPinIndex);

    return true;
  }
#ifdef NMP_ENABLE_ASSERTS
  else
  {
    NMP_VERIFY_MSG(
      optionalConnection,
      "Non optional control param input is not connected. NodeID: %d, InputCPPinID: %d, ConnectionLabel: %s",
      nodeDef->getNodeID(),
      inputCpPinIndex,
      connectionLabel);
  }
#else
  (void) optionalConnection;
#endif

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
// Read a 32 bit uint from the data block but check the result does not exceed a 8 bit uint and return as an 8 bit uint.
NM_INLINE uint8_t readUInt8FromUInt32(
  const ME::DataBlockExport* dataBlock,   // We are reading from.
  const char*                label)       // What the uint is labeled with in the data block.
{
  uint32_t result = 0;
  dataBlock->readUInt(result, label);
  NMP_VERIFY(result < 0xFF);
  return (uint8_t &) result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void readCPConnectionDetails(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  InputPinInfo*               info)
{
  const ME::DataBlockExport* dataBlock = nodeDefExport->getDataBlock();
  int32_t serialisedNodeID = -1;

  if(dataBlock->readNetworkNodeId(serialisedNodeID, info->m_nodeIDLabel))
  {
    // just a node
  }
  else if (dataBlock->readUndefined(info->m_nodeIDLabel))
  {
    // Found nothing, better be optional
#ifdef NMP_ENABLE_ASSERTS
    NMP_VERIFY(info->m_optional);
#else
    (void)info->m_optional;
#endif
  }
  else
  {
    // It's a reflexive pin
    MR::DataPinInfo* nodeDataPinInfo = netDefCompilationInfo->getNodeDataPinInfo(nodeDefExport->getNodeID());
    nodeDataPinInfo->m_numPinAttribData += info->m_numAnimSets; 
    nodeDataPinInfo->m_reflexiveCPCount += 1;
    
    // get memory requirements
    switch (info->m_semantic)
    {
    case MR::ATTRIB_SEMANTIC_CP_BOOL:
      nodeDataPinInfo->m_pinAtttribDataMemory += MR::AttribDataBool::getMemoryRequirements() * info->m_numAnimSets;

      break;
    case MR::ATTRIB_SEMANTIC_CP_UINT:
      nodeDataPinInfo->m_pinAtttribDataMemory += MR::AttribDataUInt::getMemoryRequirements() * info->m_numAnimSets;

      break;
    case MR::ATTRIB_SEMANTIC_CP_PHYSICS_OBJECT_POINTER:
      nodeDataPinInfo->m_pinAtttribDataMemory += MR::AttribDataPhysicsObjectPointer::getMemoryRequirements() * info->m_numAnimSets;

      break;
    case MR::ATTRIB_SEMANTIC_CP_INT:
      nodeDataPinInfo->m_pinAtttribDataMemory += MR::AttribDataInt::getMemoryRequirements() * info->m_numAnimSets;

      break;
    case MR::ATTRIB_SEMANTIC_CP_FLOAT:
      nodeDataPinInfo->m_pinAtttribDataMemory += MR::AttribDataFloat::getMemoryRequirements() * info->m_numAnimSets;

      break;
    case MR::ATTRIB_SEMANTIC_CP_VECTOR3:
      nodeDataPinInfo->m_pinAtttribDataMemory += MR::AttribDataVector3::getMemoryRequirements() * info->m_numAnimSets;

      break;  
    case MR::ATTRIB_SEMANTIC_CP_VECTOR4:
      nodeDataPinInfo->m_pinAtttribDataMemory += MR::AttribDataVector4::getMemoryRequirements() * info->m_numAnimSets;
      
          break;
        case MR::ATTRIB_SEMANTIC_NA:
          NMP_ASSERT_FAIL_MSG("Semantic not specified for Pin AttribData.");

          break;
        default:
          NMP_ASSERT_FAIL_MSG("Semantic not supported as Pin AttribData.");
          break;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool processCPConnectionDetails(
  const ME::NodeExport*  nodeDefExport,
  InputPinInfo*          info,
  NMP::Memory::Resource& memRes,
  MR::NodeDef*           nodeDef)
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  int32_t serialisedCPNodeID = -1; // Value set when not connected
  int32_t connectedPinIndex = MR::INVALID_PIN_INDEX;
  MR::NodeID connectedCPNodeID = MR::INVALID_NODE_ID;

  if(nodeDefDataBlock->readNetworkNodeId(serialisedCPNodeID, info->m_nodeIDLabel))
  {
    connectedCPNodeID = (MR::NodeID)serialisedCPNodeID;
    nodeDefDataBlock->readIntAttribute(connectedPinIndex, info->m_nodeIDLabel, "pinIndex");
    NMP_VERIFY(connectedPinIndex != MR::INVALID_PIN_INDEX);
    NMP_VERIFY(connectedPinIndex < 0xFF); // Currently only have 8bits for storing count of input control params.

    // set the connection
    nodeDef->setInputCPConnection(info->m_pinIndex, connectedCPNodeID, (MR::PinIndex&)connectedPinIndex);

    return true;
  }
  else if (nodeDefDataBlock->readUndefined(info->m_nodeIDLabel))
  {
#ifdef NMP_ENABLE_ASSERTS
    NMP_VERIFY_MSG(
      info->m_optional,
      "Non optional control param input is not connected. NodeID: %d, InputCPPinID: %d, ConnectionLabel: %s",
      nodeDef->getNodeID(),
      info->m_pinIndex,
      info->m_nodeIDLabel);
#else
    (void) info->m_optional;
#endif

    // Undefined optional connections are initialized as invalid
    return false;
  }
  else
  {
    // A reflexive CP pin, requires pinAttribData
    MR::PinIndex pin = NodeDefBuilder::newPinAttribEntry(
      nodeDef,
      info->m_semantic,                              // semantic, 
      info->m_numAnimSets > 1 ? true : false);       // perAnimSet

    CHAR paramName[256];
    strcpy_s(paramName, 256, info->m_nodeIDLabel);

    for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < info->m_numAnimSets; ++animSetIndex)
    {
      MR::AttribData* attrib = NULL;
      bool readResult = false;
      NMP::Memory::Format attribMem(0, MR_ATTRIB_DATA_ALIGNMENT);
      if(info->m_readPerAnimSetData)
      {
        sprintf_s(paramName, "%s_%d", info->m_nodeIDLabel, animSetIndex);
      }

      // Add attribute.
      switch (info->m_semantic)
      {
      case MR::ATTRIB_SEMANTIC_CP_BOOL:
        {
          bool paramValue = false;
          readResult = nodeDefDataBlock->readBool(paramValue, paramName);
          attrib = MR::AttribDataBool::init(memRes, paramValue, MR::IS_DEF_ATTRIB_DATA);
          attribMem = MR::AttribDataBool::getMemoryRequirements();
        }
        break;
      case MR::ATTRIB_SEMANTIC_CP_UINT:
        {
          uint32_t paramValue = 0;
          readResult = nodeDefDataBlock->readUInt(paramValue, paramName);
          attrib = MR::AttribDataUInt::init(memRes, paramValue, MR::IS_DEF_ATTRIB_DATA);
          attribMem = MR::AttribDataUInt::getMemoryRequirements();
        }
        break;
      case MR::ATTRIB_SEMANTIC_CP_PHYSICS_OBJECT_POINTER:
        {
          int32_t paramValue = 0;
          readResult = nodeDefDataBlock->readInt(paramValue, paramName);
          attrib = MR::AttribDataPhysicsObjectPointer::init(memRes, (void*)paramValue, MR::IS_DEF_ATTRIB_DATA);
          attribMem = MR::AttribDataPhysicsObjectPointer::getMemoryRequirements();
        }
        break;
      case MR::ATTRIB_SEMANTIC_CP_INT:
        {
          int32_t paramValue = 0;
          readResult = nodeDefDataBlock->readInt(paramValue, paramName);
          attrib = MR::AttribDataInt::init(memRes, paramValue, MR::IS_DEF_ATTRIB_DATA);
          attribMem = MR::AttribDataInt::getMemoryRequirements();
        }
        break;
      case MR::ATTRIB_SEMANTIC_CP_FLOAT:
        {
          float paramValue = 0.0f;
          readResult = nodeDefDataBlock->readFloat(paramValue, paramName);
          attrib = MR::AttribDataFloat::init(memRes, paramValue, MR::IS_DEF_ATTRIB_DATA);
          attribMem = MR::AttribDataFloat::getMemoryRequirements();
        }
        break;
      case MR::ATTRIB_SEMANTIC_CP_VECTOR3:
        {
          NMP::Vector3 paramValue(NMP::Vector3::InitZero);
          readResult = nodeDefDataBlock->readVector3(paramValue, paramName);
          paramValue.w = 0.0f;
          attrib = MR::AttribDataVector3::init(memRes, paramValue, MR::IS_DEF_ATTRIB_DATA);
          attribMem = MR::AttribDataVector3::getMemoryRequirements();
        }
        break;
      case MR::ATTRIB_SEMANTIC_CP_VECTOR4:
        {
          NMP::Quat paramValue(NMP::Quat::kIdentity);
          readResult = nodeDefDataBlock->readVector4(paramValue, paramName);
          attrib = MR::AttribDataVector4::init(memRes, paramValue, MR::IS_DEF_ATTRIB_DATA);
          attribMem = MR::AttribDataVector4::getMemoryRequirements();
        }
        break;
      case MR::ATTRIB_SEMANTIC_NA:
        {
          NMP_ASSERT_FAIL_MSG("Semantic not specified for Pin AttribData.");
        }
        break;
      default:
        NMP_ASSERT_FAIL_MSG("No Pin AttribData handler provided for semantic");
      }

      // Check that the requested attribute was actually read correctly.
      NMP_VERIFY_MSG(readResult, "Parameter %s was unable to be read!", paramName);

      // add pin attrib data
      NodeDefBuilder::addPinAttribAnimSetEntry(
        pin,
        nodeDef,
        animSetIndex,  // animSetIndex,
        attrib,        // attribData,  
        attribMem);    // attribMemReqs
    }

    // Set the connection
    MR::PinAttribDataInfo* padi = nodeDef->getPinAttribDataInfo(pin);
    nodeDef->setInputCPConnection(info->m_pinIndex, nodeDef->getNodeID(), padi->m_pin);

    return true;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void getEmittedControlParamNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::DataBlockExport*  nodeDefDatablock,
  uint32_t                    inputPathsCount)
{
  // Directly connected child nodes (could be an operator node)
  CHAR paramName[256];
  uint32_t nodeID;
  for (uint32_t i = 0; i < inputPathsCount; ++i)
  {
    sprintf_s(paramName, "InputPathConnectedNodeID_%d", i);
    nodeID = 0xFFFFFFFF;
    nodeDefDatablock->readUInt(nodeID, paramName);
    childNodeIDs.push_back((MR::NodeID) nodeID);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// If this is an Emitted control parameter init an array of connected children and set some flags.
NM_INLINE void initEmittedControlParamData(
  MR::NodeDef*               nodeDef,
  const char*                name,
  const ME::DataBlockExport* nodeDefDataBlock,
  uint32_t                   inputPathsCount)
{
  // We only have 8 bits for storing CP input connections, see m_numInputCPConnections in MR::NodeDef.
  NMP_VERIFY_MSG(
    inputPathsCount < 0xFF,
    "Emitted Control Parameter '%s' has more than 256 input connections.",
    name);

  // Flag node as an operator if it is an emitted control param (We are a control param node with an update).
  if (inputPathsCount > 0)
  {
    nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_OPERATOR_NODE);
  }

  // m_inputCPConnections: Stores the child paths that can potentially be active.
  // m_childNodeIDs: Stores the IDs of the nodes that must be active for the associated child path to be updated.
  //
  // Emitted control parameters can be connected to more than one outputting node. 
  //  A sensible use case is when we are connected to nodes that are within separate (independent) states of a state machine.
  //  Note that currently if > 1 connected node is active at a time then the first active connection found will perform the update.
  // m_inputCPConnections and m_childNodeIDs may store different IDs because, for example, 
  //  we may be connected to an operator node that is connected to more than 1 tree node but we only want to get output from it when
  //  1 of the these in particular is active.
  CHAR paramName[256];
  uint32_t activeNodeID;
  for (MR::PinIndex i = 0; i < inputPathsCount; ++i)
  {
    sprintf_s(paramName, "InputPathConnectedNodeID_%d", i);
    readInputPinConnectionDetails(nodeDefDataBlock, paramName, nodeDef, i, false);

    sprintf_s(paramName, "InputPathActiveNodeID_%d", i);
    activeNodeID = 0xFFFFFFFF;
    nodeDefDataBlock->readUInt(activeNodeID, paramName);
    nodeDef->setChildNodeID(i, (MR::NodeID) activeNodeID);  
  }
}

//----------------------------------------------------------------------------------------------------------------------
// If there are specified active node input paths on the control parameter this must be an emitted control parameter.
#if defined(NM_DEBUG)
NM_INLINE void validateEmittedControlParam(uint32_t inputPathsCount, const ME::DataBlockExport* nodeDefDatablock)
{
  if (inputPathsCount)
  {
    bool isEmittedControlParameter = false;
    nodeDefDatablock->readBool(isEmittedControlParameter, "IsEmittedControlParameter");
    NMP_VERIFY(isEmittedControlParameter);
  }
}
  #define VALIDATE_OUTPUT_CONTROL_PARAMS(_inputPathsCount, _nodeDefDatablock) validateEmittedControlParam(_inputPathsCount, _nodeDefDatablock);
#else // NM_DEBUG
  #define VALIDATE_OUTPUT_CONTROL_PARAMS(_inputPathsCount, _nodeDefDatablock)
#endif // NM_DEBUG

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void initMessageMapEntry(
  uint32_t                          mapIndex,
  MR::AttribDataEmittedMessagesMap* emittedMessageMapAttribData,
  const ME::NetworkDefExport*       netDefExport,
  const ME::DataBlockExport*        nodeDefDataBlock)
{
  CHAR paramName[256];

  // Actions (0 = "Not Specified", 1 = "Set", 2 = "Clear", 3 = "ClearAll", 4 = "External")
  sprintf_s(paramName, "ActionID_%d", mapIndex);
  uint32_t actionID = 0xFFFFFFFF;
  nodeDefDataBlock->readUInt(actionID, paramName);
  NMP_VERIFY(actionID < MR::AttribDataEmittedMessagesMap::NUM_REQUEST_TYPES);

  MR::MessageID messageID = MR::INVALID_MESSAGE_ID;
  MR::NodeID targetNodeID = MR::NETWORK_NODE_ID;
  if (actionID != MR::AttribDataEmittedMessagesMap::UNUSED) 
  {
    // Message ID.
    if (actionID != MR::AttribDataEmittedMessagesMap::RESET) // If clearing all a specific message ID is irrelevant.
    {
      sprintf_s(paramName, "EmittedMessageID_%d", mapIndex);
      nodeDefDataBlock->readUInt(messageID, paramName);
      NMP_VERIFY(messageID != MR::INVALID_MESSAGE_ID);
    }

    if (actionID != MR::AttribDataEmittedMessagesMap::EXTERNAL)
    {
      // NodeID of target state machine (or NETWORK_NODE_ID for broadcast).
      sprintf_s(paramName, "Broadcast_%d", mapIndex);
      bool broadcast = false;
      nodeDefDataBlock->readBool(broadcast, paramName);
      if (broadcast)
      {
        targetNodeID = MR::NETWORK_NODE_ID; // Broadcasting to all state machines.
      }
      else
      {
        // Try and resolve the exported state machine path to a specific node.
        // MORPH-21328: This is currently done by trying to find the node's name.  To be truly secure
        // this needs to done using the full path, but we do not have this information available to
        // search in the asset compiler. The best solution is to export a target node ID from the node's
        // lua manifest file, however resolving to an ID may cause problems of circular dependency in connect.
        std::string targetNodePath;
        sprintf_s(paramName, "TargetNodePath_%d", mapIndex);
        nodeDefDataBlock->readString(targetNodePath, paramName);

        uint32_t numNetworkNodes = netDefExport->getNumNodes();
        const ME::NodeExport* currentNodeDefExport;

        for(MR::NodeID j = 0; j < numNetworkNodes; ++j)
        {
          currentNodeDefExport = netDefExport->getNode(j);
          NMP_VERIFY(currentNodeDefExport);
          uint32_t nodeType = currentNodeDefExport->getTypeID();
          if (nodeType ==  NODE_TYPE_STATE_MACHINE)  // Only check nodes that are flagged as state machines.
          {
            const char* currentNodeName = currentNodeDefExport->getName();
            if (targetNodePath == currentNodeName)
            {
              targetNodeID = j;
            }
          }
        }   
      }

      NMP_VERIFY(targetNodeID < netDefExport->getNumNodes());
    }
  }

  // Actually initialise the message mapping slot.
  emittedMessageMapAttribData->setEmittedMessage(mapIndex, (MR::AttribDataEmittedMessagesMap::RequestType)actionID, targetNodeID, messageID);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_OUTPUT_CONTROL_PARAM_NODE_BUILDER_UTILS_H
//----------------------------------------------------------------------------------------------------------------------
