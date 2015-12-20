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
// inline NodeDef, SemanticLookupTable and SharedTaskFnTables functions, included by mrNetworkDef.h
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const char* NodeDef::getName() const
{
  NMP_ASSERT(m_owningNetworkDef);
  return m_owningNetworkDef->getNodeNameFromNodeID(m_nodeID);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const NodeDef* NodeDef::getParentNodeDef() const
{
  NMP_ASSERT(m_owningNetworkDef);
  if (m_parentNodeID == INVALID_NODE_ID)
    return NULL;
  return m_owningNetworkDef->getNodeDef(m_parentNodeID);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NodeDef* NodeDef::getParentNodeDef()
{
  NMP_ASSERT(m_owningNetworkDef);
  if (m_parentNodeID == INVALID_NODE_ID)
    return NULL;
  return m_owningNetworkDef->getNodeDef(m_parentNodeID);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NodeID NodeDef::getChildNodeID(uint32_t childNodeIndex) const
{
  NMP_ASSERT(childNodeIndex < m_numChildNodeIDs);
  return m_childNodeIDs[childNodeIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool NodeDef::hasChildNodeID(NodeID nodeID) const
{
  for (uint32_t i=0; i<m_numChildNodeIDs; ++i)
  {
    if(m_childNodeIDs[i] == nodeID)
      return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const NodeDef* NodeDef::getChildNodeDef(uint32_t childNodeIndex) const
{
  NMP_ASSERT(childNodeIndex < m_numChildNodeIDs);
  NMP_ASSERT(m_owningNetworkDef);
  return m_owningNetworkDef->getNodeDef(m_childNodeIDs[childNodeIndex]);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NodeDef* NodeDef::getChildNodeDef(uint32_t childNodeIndex)
{
  NMP_ASSERT(childNodeIndex < m_numChildNodeIDs);
  NMP_ASSERT(m_owningNetworkDef);
  return m_owningNetworkDef->getNodeDef(m_childNodeIDs[childNodeIndex]);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NodeDef::setChildNodeID(uint32_t childNodeIndex, NodeID nodeID)
{
  NMP_ASSERT(childNodeIndex < m_numChildNodeIDs);
  m_childNodeIDs[childNodeIndex] = nodeID;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NodeID NodeDef::getInputCPConnectionSourceNodeID(uint32_t inputPinIndex) const
{
  NMP_ASSERT(inputPinIndex < m_numInputCPConnections);
  return  m_inputCPConnections[inputPinIndex].m_sourceNodeID;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE PinIndex NodeDef::getInputCPConnectionSourcePinIndex(uint32_t inputPinIndex) const
{
  NMP_ASSERT(inputPinIndex < m_numInputCPConnections);
  return m_inputCPConnections[inputPinIndex].m_sourcePinIndex;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const NodeDef* NodeDef::getInputCPConnectionSourceNodeDef(uint32_t inputPinIndex) const
{
  NMP_ASSERT(inputPinIndex < m_numInputCPConnections);
  NMP_ASSERT(m_owningNetworkDef);
  return m_owningNetworkDef->getNodeDef(m_inputCPConnections[inputPinIndex].m_sourceNodeID);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NodeDef* NodeDef::getInputCPConnectionSourceNodeDef(uint32_t inputPinIndex)
{
  NMP_ASSERT(inputPinIndex < m_numInputCPConnections);
  NMP_ASSERT(m_owningNetworkDef);
  return m_owningNetworkDef->getNodeDef(m_inputCPConnections[inputPinIndex].m_sourceNodeID);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NodeDef::setInputCPConnection(
  PinIndex inputPinIndex,         // My input pin index.
  NodeID   sourceNodeID,          // The node we are connected to.
  PinIndex sourceOutputPinIndex)  // The output pin we are connected to on the source node
{
  NMP_ASSERT(inputPinIndex < m_numInputCPConnections);
  m_inputCPConnections[inputPinIndex].set(sourceNodeID, sourceOutputPinIndex);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const CPConnection* NodeDef::getInputCPConnection(PinIndex inputPinIndex) const // My input pin index.
{
  NMP_ASSERT(inputPinIndex < m_numInputCPConnections);
  return &(m_inputCPConnections[inputPinIndex]);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE MR::PinAttribDataInfo* NodeDef::getPinAttribDataInfo(uint32_t index) const
{
  NMP_ASSERT(index < m_numReflexiveCPPins);
  return &m_nodePinAttribDataInfo[index];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE MR::PinIndex NodeDef::newPinAttribDataInfo(bool perAnimSet, AttribDataSemantic semantic)
{
  NMP_ASSERT(m_lastPinAttribDataOffset < m_numPinAttribDataHandles);
  NMP_ASSERT(m_lastPinAttribDataIndex < m_numReflexiveCPPins);

  PinAttribDataInfo* padi = &m_nodePinAttribDataInfo[m_lastPinAttribDataIndex];
  NMP_ASSERT(padi->m_semantic == ATTRIB_SEMANTIC_NA);
  padi->set(m_lastPinAttribDataOffset, perAnimSet, semantic);
  
  return m_lastPinAttribDataIndex++;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NodeDef::addPinAttribDataAnimSetEntry(uint32_t NMP_USED_FOR_ASSERTS(pin), MR::AnimSetIndex NMP_USED_FOR_ASSERTS(animSetIndex))
{
#ifdef NMP_ENABLE_ASSERTS
  PinAttribDataInfo* padi = &m_nodePinAttribDataInfo[pin];
#endif
  NMP_ASSERT(animSetIndex == 0 || padi->m_perAnimSet == true);
  NMP_ASSERT(m_lastPinAttribDataIndex == pin + 1);
  NMP_ASSERT(m_lastPinAttribDataOffset - m_nodePinAttribDataInfo[pin].m_offset == animSetIndex);
  
  m_lastPinAttribDataOffset++;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NodeDef::getPinAttribIndex(uint32_t pin, AnimSetIndex animSetIndex) const
{
  PinAttribDataInfo* padi = &m_nodePinAttribDataInfo[pin];
  uint32_t lookupIndex = padi->m_offset;
  if(padi->m_perAnimSet)
  {
    NMP_ASSERT(animSetIndex != ANIMATION_SET_ANY); // ANIMATION_SET_ANY AttribData is stored in the array for AnimSet 0.
    lookupIndex += (uint32_t)animSetIndex;
  }
  NMP_ASSERT(lookupIndex < m_numPinAttribDataHandles);
  
  return lookupIndex;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribDataHandle* NodeDef::getPinAttribDataHandle(uint32_t index, AnimSetIndex animSetIndex) const
{
  uint32_t lookupIndex = getPinAttribIndex(index, animSetIndex);
  return &(m_nodePinAttribDataHandles[lookupIndex]);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribDataHandle* NodeDef::getAttribDataHandle(
  AttribDataSemantic semantic,
  AnimSetIndex       animSetIndex) const
{
  NMP_ASSERT(animSetIndex != ANIMATION_SET_ANY); // ANIMATION_SET_ANY AttribData is stored in the array for AnimSet 0.
  NMP_ASSERT(m_semanticLookupTable);
  uint32_t semanticLookupIndex = m_semanticLookupTable->getLookupIndex(semantic, animSetIndex);
  NMP_ASSERT(semanticLookupIndex < m_numAttribDataHandles);
  NMP_ASSERT(m_nodeAttribDataHandles[semanticLookupIndex].m_attribData);
  return &(m_nodeAttribDataHandles[semanticLookupIndex]);
}

//----------------------------------------------------------------------------------------------------------------------
// The same as getAttribDataHandle with animSetIndex = ANIMATION_SET_ANY.
// ANIMATION_SET_ANY AttribData is stored in the array for AnimSet 0.
NM_INLINE AttribDataHandle* NodeDef::getAttribDataHandle(AttribDataSemantic semantic) const
{
  NMP_ASSERT(m_semanticLookupTable);
  uint32_t semanticLookupIndex = m_semanticLookupTable->getLookupIndex(semantic, 0);
  NMP_ASSERT(semanticLookupIndex < m_numAttribDataHandles);
  NMP_ASSERT(m_nodeAttribDataHandles[semanticLookupIndex].m_attribData);
  return &(m_nodeAttribDataHandles[semanticLookupIndex]);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribData* NodeDef::getAttribData(
  AttribDataSemantic semantic,
  AnimSetIndex       animSetIndex) const
{
  AttribDataHandle* handle = getAttribDataHandle(semantic, animSetIndex);
  NMP_ASSERT(handle && handle->m_attribData);
  return handle->m_attribData;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribData* NodeDef::getAttribData(
  AttribDataSemantic semantic) const
{
  AttribDataHandle* handle = getAttribDataHandle(semantic);
  NMP_ASSERT(handle && handle->m_attribData);
  return handle->m_attribData;
}

//----------------------------------------------------------------------------------------------------------------------
template <class T>
NM_INLINE T* NodeDef::getPinAttribData(
  uint32_t     index,
  AnimSetIndex animSetIndex) const
{
  AttribDataHandle* handle = getPinAttribDataHandle(index, animSetIndex);
  NMP_ASSERT(handle && handle->m_attribData);
  NMP_ASSERT(T::getDefaultType() == handle->m_attribData->getType());
  return static_cast<T*>(handle->m_attribData);
}

//----------------------------------------------------------------------------------------------------------------------
template <class T>
NM_INLINE T* NodeDef::getAttribData(
  AttribDataSemantic semantic,
  AnimSetIndex       animSetIndex) const
{
  AttribDataHandle* handle = getAttribDataHandle(semantic, animSetIndex);
  NMP_ASSERT(handle && handle->m_attribData);
  NMP_ASSERT(T::getDefaultType() == handle->m_attribData->getType());
  return static_cast<T*>(handle->m_attribData);
}

//----------------------------------------------------------------------------------------------------------------------
template <class T>
NM_INLINE T* NodeDef::getAttribData(
  AttribDataSemantic semantic) const
{
  AttribDataHandle* handle = getAttribDataHandle(semantic);
  NMP_ASSERT(handle && handle->m_attribData);
  NMP_ASSERT(T::getDefaultType() == handle->m_attribData->getType());
  return static_cast<T*>(handle->m_attribData);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribDataHandle* NodeDef::getOptionalAttribDataHandle(
  AttribDataSemantic semantic,
  AnimSetIndex       animSetIndex) const
{
  NMP_ASSERT(animSetIndex != ANIMATION_SET_ANY); // ANIMATION_SET_ANY AttribData is stored in the array for AnimSet 0.
  NMP_ASSERT(m_semanticLookupTable);
  uint32_t semanticLookupIndex = m_semanticLookupTable->getOptionalLookupIndex(semantic, animSetIndex);
  if (semanticLookupIndex == SemanticLookupTable::INVALID_LOOKUP_INDEX)
    return NULL;
  NMP_ASSERT(semanticLookupIndex < m_numAttribDataHandles);
  if(m_nodeAttribDataHandles[semanticLookupIndex].m_attribData == NULL) 
    return NULL;// We now don't have attrib data for nodes for anim sets they are not valid for.
  return &(m_nodeAttribDataHandles[semanticLookupIndex]);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribDataHandle* NodeDef::getOptionalAttribDataHandle(
  AttribDataSemantic semantic) const
{
  NMP_ASSERT(m_semanticLookupTable);
  uint32_t semanticLookupIndex = m_semanticLookupTable->getOptionalLookupIndex(semantic, 0);
  if (semanticLookupIndex == SemanticLookupTable::INVALID_LOOKUP_INDEX)
    return NULL;
  NMP_ASSERT(semanticLookupIndex < m_numAttribDataHandles);
  if(m_nodeAttribDataHandles[semanticLookupIndex].m_attribData == NULL) 
    return NULL;// We now don't have attrib data for nodes for anim sets they are not valid for.
  return &(m_nodeAttribDataHandles[semanticLookupIndex]);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE QueueAttrTaskFn NodeDef::getTaskQueueingFn(AttribDataSemantic nodeAttribSemantic)
{
  return m_taskQueuingFns[nodeAttribSemantic];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE OutputCPTask NodeDef::getOutputCPTask(PinIndex outputCPPinIndex)
{
  return m_outputCPTasks[outputCPPinIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE FindGeneratingNodeForSemanticFn NodeDef::getFindGeneratingNodeForSemanticFn() const
{
  return m_findGeneratingNodeForSemanticFn;
}

//----------------------------------------------------------------------------------------------------------------------
// Q: Do we need to pass the network in here?
// A:: If we depend on control params to make queuing decisions then yes
//     (as we can't get to their state without the network).
NM_INLINE Task* NodeDef::putAttributeOnQueue(
  TaskQueue*               queue,
  Network*                 net,
  TaskParameter*           dependentParam)
{
  NET_LOG_ASSERT_MESSAGE(
    m_taskQueuingFns[dependentParam->m_attribAddress.m_semantic],
    "ERROR: No queuing op for attribute %i on node %i\n",
    dependentParam->m_attribAddress.m_semantic,
    getNodeID());

  return m_taskQueuingFns[dependentParam->m_attribAddress.m_semantic](
    this,
    queue,
    net,
    dependentParam);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribData* NodeDef::updateOutputCPAttribute(
  Network* net,
  PinIndex outputCPPinIndex)
{
  NET_LOG_ASSERT_MESSAGE(
    m_outputCPTasks[outputCPPinIndex],
    "\nERROR: No output control param task update function for pin index %i on node %i\n",
    outputCPPinIndex,
    getNodeID());
  return m_outputCPTasks[outputCPPinIndex](this, outputCPPinIndex, net);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribData* NodeDef::updateEmittedMessages(
  Network* net,
  PinIndex emitMessagePinIndex)
{
  NET_LOG_ASSERT_MESSAGE(
    m_outputCPTasks[emitMessagePinIndex],
    "\nERROR: No output control param task update function for pin index %i on node %i\n",
    emitMessagePinIndex,
    getNodeID());
  return m_outputCPTasks[emitMessagePinIndex](this, emitMessagePinIndex, net);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool NodeDef::handleMessage(Network* net, const Message& message)
{
  if (m_messageHandlerFn)
  {
    return m_messageHandlerFn(message, m_nodeID, net);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NodeID NodeDef::updateConnections(Network* net)
{
  NMP_ASSERT(m_updateNodeConnectionsFn);
  return m_updateNodeConnectionsFn(this, net);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NodeDef::initNodeInstance(Network* net)
{
  if (m_initNodeInstanceFn)
    m_initNodeInstanceFn(this, net);
}

//----------------------------------------------------------------------------------------------------------------------
// SemanticLookupTable
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE uint32_t SemanticLookupTable::getLookupIndex(
  AttribDataSemantic semantic,
  AnimSetIndex       animSetIndex) const
{
  NMP_ASSERT(semantic < m_numSemantics);
  NMP_ASSERT(m_semanticLookup[semantic] != INVALID_LOOKUP_INDEX);
  return ((uint32_t)animSetIndex * m_numAttribsPerAnimSet) + m_semanticLookup[semantic];
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE uint32_t SemanticLookupTable::getOptionalLookupIndex(
  AttribDataSemantic semantic,
  AnimSetIndex       animSetIndex) const
{
  NMP_ASSERT(semantic < m_numSemantics);
  if (m_semanticLookup[semantic] == INVALID_LOOKUP_INDEX)
    return INVALID_LOOKUP_INDEX;
  return ((uint32_t)animSetIndex * m_numAttribsPerAnimSet) + m_semanticLookup[semantic];
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE bool SemanticLookupTable::hasLookupIndex(AttribDataSemantic semantic) const
{
  return m_semanticLookup[semantic] != INVALID_LOOKUP_INDEX;
}

//----------------------------------------------------------------------------------------------------------------------
// SharedTaskFnTables
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const SharedTaskFnTables::SharedTaskFn* SharedTaskFnTables::getTaskFnTable(uint32_t indx) const
{
  NMP_ASSERT(indx < m_numTaskFnTables);
  return m_taskFnTables[indx];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE SharedTaskFnTables::SharedTaskFn* SharedTaskFnTables::getTaskFnTable(uint32_t indx)
{
  NMP_ASSERT(indx < m_numTaskFnTables);
  return m_taskFnTables[indx];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SharedTaskFnTables::setTaskFnTable(uint32_t indx, SharedTaskFnTables::SharedTaskFn* taskFnTbl)
{
  NMP_ASSERT(indx < m_numTaskFnTables);
  NMP_ASSERT(taskFnTbl);
  m_taskFnTables[indx] = taskFnTbl;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SharedTaskFnTables::getNumTaskFnTables() const
{
  return m_numTaskFnTables;
}

//----------------------------------------------------------------------------------------------------------------------
