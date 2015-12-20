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
#include "assetProcessor/NodeBuilder.h"
#include "assetProcessor/NodeBuilderUtils.h"
#include "nmtl/algorithm.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

const AttribDataSemanticNameTable BUILT_IN_ATTRIB_DATA_SEMANTIC_NAMES[] ={
    { MR::ATTRIB_SEMANTIC_CP_BOOL, "bool" },
    { MR::ATTRIB_SEMANTIC_CP_UINT, "uint" },
    { MR::ATTRIB_SEMANTIC_CP_PHYSICS_OBJECT_POINTER, "physicsObjectID" },
    { MR::ATTRIB_SEMANTIC_CP_INT, "int" },
    { MR::ATTRIB_SEMANTIC_CP_FLOAT, "float" },
    { MR::ATTRIB_SEMANTIC_CP_VECTOR3,"vector3" },
    { MR::ATTRIB_SEMANTIC_CP_VECTOR4, "vector4" }
  };

const uint32_t NUM_BUILT_IN_ATTRIB_DATA_SEMANTICS = sizeof(BUILT_IN_ATTRIB_DATA_SEMANTIC_NAMES)/sizeof(AttribDataSemanticNameTable);

//----------------------------------------------------------------------------------------------------------------------
// NetworkDefCompilationInfo
//----------------------------------------------------------------------------------------------------------------------
NetworkDefCompilationInfo::NetworkDefCompilationInfo(
  const ME::NetworkDefExport*       netDefExport,
  const ME::AnimationLibraryExport* animLibraryExport,
  MR::SharedTaskFnTables*           taskQueuingFnTables,
  MR::SharedTaskFnTables*           outputCPTaskFnTables)
: m_taskQueuingFnTables(taskQueuingFnTables),
  m_outputCPTaskFnTables(outputCPTaskFnTables),
  m_semanticLookupTablesCount(0),
  m_semanticLookupTables(NULL)
{
  m_numAnimSets = animLibraryExport->getNumAnimationSets();

  //------------------------
  // setup the node related arrays
  uint32_t numNodes = netDefExport->getNumNodes();
  m_nodeDefCompilationInfo.resize(numNodes);
  for (uint32_t i = 0; i < numNodes; ++i)
  {
    uint32_t nodeID = netDefExport->getNode(i)->getNodeID();    
    m_nodeDefCompilationInfo[nodeID].initPinInfo();
  }
  m_nodeDefDependencyGraph.resize(numNodes);

  //------------------------
  // build the message distributor array
  const uint32_t numMessages = netDefExport->getNumMessages();

  // messageID should also be the the index into the array,
  // make sure the array is big enough to hold the highest message id.
  uint32_t maxMessageID = netDefExport->getNumMessages();

  for (uint32_t i = 0; i != numMessages; ++i)
  {
    const ME::MessageExport* messageExport = netDefExport->getMessage(i);

    MR::MessageID messageID = messageExport->getMessageID();
    if (messageID >= maxMessageID)
    {
      maxMessageID = messageID + 1;
    }
  }

  //------------------------
  // now initialise the message distributor info array, potentially with some gaps if the messageIDs were sparse
  m_messageInterest.resize(maxMessageID);

  for (uint32_t i = 0; i != numMessages; ++i)
  {
    const ME::MessageExport* messageExport = netDefExport->getMessage(i);

    MR::MessageID messageID = messageExport->getMessageID();
    MR::MessageType messageType = messageExport->getMessageType();

    m_messageInterest[messageID].m_messageID = messageID;
    m_messageInterest[messageID].m_messageType = messageType;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NetworkDefCompilationInfo::getNodeDataPinInfoMemory(uint32_t nodeID, const ME::NodeExport* nodeDefExport)
{
  NMP_VERIFY(nodeID < getNumNodes());
  size_t pins = m_nodeDefCompilationInfo[nodeID].m_inputPinInfo.size();

  for (size_t i = 0; i < pins; ++i)
  {
    InputPinInfo* info = &(m_nodeDefCompilationInfo[nodeID].m_inputPinInfo[i]);

    readCPConnectionDetails(
      this,
      nodeDefExport,
      info);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NetworkDefCompilationInfo::processNodeDataPinInfo(
  uint32_t               nodeID,
  NMP::Memory::Resource& resource,
  MR::NodeDef*           nodeDef,
  const ME::NodeExport*  nodeDefExport)
{
  NMP_VERIFY(nodeID < getNumNodes());
  size_t pins = m_nodeDefCompilationInfo[nodeID].m_inputPinInfo.size();

  for (size_t i = 0; i < pins; ++i)
  {
    InputPinInfo* info = &(m_nodeDefCompilationInfo[nodeID].m_inputPinInfo[i]);

    processCPConnectionDetails(
      nodeDefExport,
      info,
      resource,
      nodeDef);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NetworkDefCompilationInfo::~NetworkDefCompilationInfo()
{
  // Tidy up the node def compilation info.
  for (uint32_t nodeID = 0; nodeID < getNumNodes(); ++nodeID)
  {
    releaseNodeDef(nodeID);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// NodeDefBuilder
//----------------------------------------------------------------------------------------------------------------------
bool NodeDefBuilder::hasSemanticLookupTable()
{
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeDefBuilder::initSemanticLookupTable(NMP::Memory::Resource& NMP_UNUSED(memRes))
{
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NodeDefBuilder::initAttribEntry(
  MR::NodeDef*               nodeDef,
  MR::AttribDataSemantic     semantic,
  MR::AnimSetIndex           animSetIndex,
  MR::AttribData*            attribData,
  const NMP::Memory::Format& attribMemReqs)
{
  const MR::SemanticLookupTable* nodeTypeSemanticLookupTable = nodeDef->getSemanticLookupTable();
  NMP_ASSERT(nodeTypeSemanticLookupTable);

  uint32_t lookupIndex = nodeTypeSemanticLookupTable->getLookupIndex(semantic, animSetIndex);
  NMP_ASSERT(lookupIndex < (uint32_t)(nodeTypeSemanticLookupTable->getNumAttribsPerAnimSet() * (animSetIndex + 1)));

  MR::AttribDataHandle* attribDataArray = nodeDef->getAttribDataHandles();
  NMP_ASSERT(!attribDataArray[lookupIndex].m_attribData);
  attribDataArray[lookupIndex].set(attribData, attribMemReqs);

  return lookupIndex;
}

//----------------------------------------------------------------------------------------------------------------------
MR::PinIndex NodeDefBuilder::newPinAttribEntry(
  MR::NodeDef*               nodeDef,
  MR::AttribDataSemantic     semantic,
  bool                       perAnimSet)
{
  return nodeDef->newPinAttribDataInfo(perAnimSet, semantic);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeDefBuilder::addPinAttribAnimSetEntry(
  uint32_t                   pin,
  MR::NodeDef*               nodeDef,
  MR::AnimSetIndex           animSetIndex,
  MR::AttribData*            attribData,
  const NMP::Memory::Format& attribMemReqs)
{
  nodeDef->addPinAttribDataAnimSetEntry(pin, animSetIndex);
  uint32_t lookupIndex  = nodeDef->getPinAttribIndex(pin, animSetIndex);

  MR::AttribDataHandle* pinAttribDataArray = nodeDef->getPinAttribDataHandles();
  NMP_ASSERT(!pinAttribDataArray[lookupIndex].m_attribData);
  pinAttribDataArray[lookupIndex].set(attribData, attribMemReqs);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeDefBuilder::getCoreNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  uint32_t                    numChildren,
  uint32_t                    numInputCPConnections,
  uint32_t                    numAnimSetEntries,  // Although this Node may have anim set specific attrib data we still pass in the total number
                                                  // of anim sets. This parameter is used to reserve the space for possible anim set specific data. 
                                                  // Unused entries remain as NULL pointers. These are kept to make the modification simpler and debugging easier.
  const ME::NodeExport*       nodeDefExport)
{
  // Work out how many attrib data slots are required
  uint32_t numAttribDatas = 0;
  MR::SemanticLookupTable* semanticLookupTable = netDefCompilationInfo->findSemanticLookupTable(nodeDefExport->getTypeID());
  if (semanticLookupTable)
    numAttribDatas = semanticLookupTable->getNumAttribsPerAnimSet() * numAnimSetEntries;

  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  uint32_t numInternalMessageIDs = 0;
  uint32_t numExternalMessageIDs = 0;
  nodeDefDataBlock->readUInt(numInternalMessageIDs, "NumOnEnterInternalMessageIDs");
  nodeDefDataBlock->readUInt(numExternalMessageIDs, "NumOnEnterExternalMessageIDs");
  uint32_t numOnEnterMessages = numInternalMessageIDs + numExternalMessageIDs;
  uint32_t nodeID = nodeDefExport->getNodeID();
  NMP_VERIFY_MSG(numOnEnterMessages < 256, "Node %i has more than 255 onEnter messages!", nodeID);

  // Process dataPins
  netDefCompilationInfo->getNodeDataPinInfoMemory((MR::NodeID)nodeDefExport->getNodeID(), nodeDefExport);
  MR::DataPinInfo* nodeDataPinInfo = netDefCompilationInfo->getNodeDataPinInfo((MR::NodeID)nodeDefExport->getNodeID());

  // Do the computation.
  return  MR::NodeDef::getMemoryRequirements(
                            numChildren,
                            numInputCPConnections,
                            numAttribDatas,
                            numOnEnterMessages,
                            nodeDataPinInfo);
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeDefBuilder::initCoreNodeDef(
  NMP::Memory::Resource&      resource,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  uint32_t                    numChildren,
  uint16_t                    maxNumActiveChildNodes,
  uint32_t                    numInputCPConnections,
  uint8_t                     numOutputCPPins,
  uint16_t                    numAnimSetEntries,
  const ME::NodeExport*       nodeDefExport)
{
  uint16_t numAttribDatas = 0;
  MR::SemanticLookupTable* semanticLookupTable = netDefCompilationInfo->findSemanticLookupTable(nodeDefExport->getTypeID());
  if (semanticLookupTable)
    numAttribDatas = semanticLookupTable->getNumAttribsPerAnimSet() * numAnimSetEntries;

  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  uint32_t numOnEnterInternalMessageIDs = 0;
  uint32_t numOnEnterExternalMessageIDs = 0;
  nodeDefDataBlock->readUInt(numOnEnterInternalMessageIDs, "NumOnEnterInternalMessageIDs");
  nodeDefDataBlock->readUInt(numOnEnterExternalMessageIDs, "NumOnEnterExternalMessageIDs");
  uint32_t numOnEnterMessages = numOnEnterInternalMessageIDs + numOnEnterExternalMessageIDs;
  NMP_VERIFY_MSG(numOnEnterMessages < 256, "Node %i has more than 255 onEnter messages!", nodeDefExport->getNodeID());

  MR::DataPinInfo* nodeDataPinInfo = netDefCompilationInfo->getNodeDataPinInfo((MR::NodeID)nodeDefExport->getNodeID());
  MR::NodeDef* result = MR::NodeDef::init(
    resource,
    numChildren,
    numInputCPConnections,
    numAttribDatas,
    maxNumActiveChildNodes,
    numOutputCPPins,
    numOnEnterMessages,
    nodeDataPinInfo);

  result->setSemanticLookupTable(semanticLookupTable);
  result->setNodeTypeID(nodeDefExport->getTypeID());
  result->setNodeID((MR::NodeID)nodeDefExport->getNodeID());
  result->setParentNodeID((MR::NodeID)nodeDefExport->getDownstreamParentID());

  // Set up the messages themselves
  CHAR messageSlotName[256];
  uint32_t messageID;
  bool readResult;

  // OnEnterInternal
  for(uint32_t i = 0; i < numOnEnterInternalMessageIDs; i++)
  {
    // Get the message ID from the XML
    sprintf_s(messageSlotName, "OnEnterInternalMessageID_%d", i);
    readResult = nodeDefDataBlock->readUInt(messageID, messageSlotName);
    NMP_VERIFY_MSG(readResult, "Could not read onEnter internal message %i from node ID %i", i, nodeDefExport->getNodeID());

    // Write it into the message structure
    result->setOnEnterMessage(i, messageID, false);
  }

  // Continue iterating with the OnEnterExternals
  for(uint32_t i = 0; i < numOnEnterExternalMessageIDs; i++)
  {
    // Get the message ID from the XML
    sprintf_s(messageSlotName, "OnEnterExternalMessageID_%d", i);
    readResult = nodeDefDataBlock->readUInt(messageID, messageSlotName);
    NMP_VERIFY_MSG(readResult, "Could not read onEnter external message %i from node ID %i", i, nodeDefExport->getNodeID());

    // Write it into the message structure
    result->setOnEnterMessage(i + numOnEnterInternalMessageIDs, messageID, true);
  }

  if (nodeDefExport->isDownstreamParentMultiplyConnected())
  {
    result->setNodeFlags(MR::NodeDef::NODE_FLAG_OUTPUT_REFERENCED);
  }

  // connect declared dataPins
  netDefCompilationInfo->processNodeDataPinInfo(
    (MR::NodeID)nodeDefExport->getNodeID(), resource, result, nodeDefExport);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------
void NetworkDefCompilationInfo::registerMessageInterest(MR::MessageID messageID, MR::NodeID nodeID)
{
  // this node is trying to register interest in an invalid message
  NMP_VERIFY(messageID < (MR::MessageID)m_messageInterest.size());
  // cannot add a node to the list of interested nodes if the message distributor has been compiled.
  NMP_VERIFY(!isMessageDistributorCompiled(messageID));

  MessageInterest& messageInterest = m_messageInterest[messageID];

  // Only add this node ID if it isn't already in the list
  nmtl::pod_vector<MR::NodeID>::const_iterator begin = messageInterest.m_nodeIDs.begin();
  nmtl::pod_vector<MR::NodeID>::const_iterator end = messageInterest.m_nodeIDs.end();
  if (nmtl::find(begin, end, nodeID) == end)
  {
    messageInterest.m_nodeIDs.push_back(nodeID);
  }
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NetworkDefCompilationInfo::findSemanticLookupTable(MR::NodeType nodeType)
{
  NMP_VERIFY(m_semanticLookupTables && (m_semanticLookupTablesCount != 0));
  for (uint32_t i = 0; i < m_semanticLookupTablesCount; ++i)
  {
    MR::SemanticLookupTable* const table = m_semanticLookupTables[i];
    if (table->getNodeType() == nodeType)
    {
      return table;
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefCompilationInfo::setSemanticLookupTables(
  uint32_t                  semanticLookupTablesCount,
  MR::SemanticLookupTable** semanticLookupTables,
  NMP::Memory::Format       semanticLookupTablesMemReqs)
{
  NMP_VERIFY(!m_semanticLookupTables && (m_semanticLookupTablesCount == 0));

  m_semanticLookupTablesCount = semanticLookupTablesCount;
  m_semanticLookupTables = semanticLookupTables;
  m_semanticLookupTablesMemReqs = semanticLookupTablesMemReqs;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefCompilationInfo::detachSemanticLookupTables(
  uint32_t&                  semanticLookupTablesCount,
  MR::SemanticLookupTable**& semanticLookupTables,
  NMP::Memory::Format&       semanticLookupTablesMemReqs)
{
  NMP_VERIFY(m_semanticLookupTables && (m_semanticLookupTablesCount != 0));

  semanticLookupTablesCount = m_semanticLookupTablesCount;
  semanticLookupTables = m_semanticLookupTables;
  semanticLookupTablesMemReqs = m_semanticLookupTablesMemReqs;

  // Detach the data
  m_semanticLookupTablesCount = 0;
  m_semanticLookupTables = NULL;
  m_semanticLookupTablesMemReqs.set(0, NMP_NATURAL_TYPE_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefCompilationInfo::tidySemanicLookupTables()
{
  if (m_semanticLookupTables)
  {
    NMP::Memory::memFree(m_semanticLookupTables);
    m_semanticLookupTables = NULL;
  }
  m_semanticLookupTablesMemReqs.set(0, NMP_NATURAL_TYPE_ALIGNMENT);
  m_semanticLookupTablesCount = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefCompilationInfo::tidyMessageInterest()
{
  for (uint32_t messageID = 0; messageID < m_messageInterest.size(); messageID++)
  {
    NMP::Memory::memFree(m_messageInterest[messageID].m_messageDistributor);
    m_messageInterest[messageID].m_messageDistributor = NULL;
  }
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
