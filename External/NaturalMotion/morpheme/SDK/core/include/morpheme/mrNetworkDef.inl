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
// inline NetworkDef, EmittedControlParamsInfo & NodeIDsArray functions, included by mrNetworkDef.h
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NetworkDef::getEventTrackRuntimeIDFromName(const char* name) const
{
  if (m_eventTrackIDNamesTable)
    return m_eventTrackIDNamesTable->getIDForString(name);
  return INVALID_EVENT_TRACK_NAME;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const char* NetworkDef::getEventTrackNameFromRuntimeID(uint32_t id) const
{
  if (m_eventTrackIDNamesTable)
    return m_eventTrackIDNamesTable->getStringForID(id);
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribDataHandle* NetworkDef::getAttribDataHandle(const MR::AttribAddress& attributeAddress) const
{
  NMP_ASSERT(attributeAddress.m_owningNodeID < m_numNodes);
  NMP_ASSERT(attributeAddress.m_animSetIndex < m_numAnimSets || attributeAddress.m_animSetIndex == ANIMATION_SET_ANY);
  return m_nodes[attributeAddress.m_owningNodeID]->getAttribDataHandle(
    attributeAddress.m_semantic,
    attributeAddress.m_animSetIndex);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribDataHandle* NetworkDef::getAttribDataHandle(
  AttribDataSemantic semantic,
  NodeID             owningNodeID,
  AnimSetIndex       animSetIndex) const
{
  NMP_ASSERT(owningNodeID < m_numNodes);
  NMP_ASSERT(animSetIndex < m_numAnimSets || animSetIndex == ANIMATION_SET_ANY);
  return m_nodes[owningNodeID]->getAttribDataHandle(semantic, animSetIndex);
}

//----------------------------------------------------------------------------------------------------------------------
// The same as getAttribDataHandle with animSetIndex = ANIMATION_SET_ANY.
NM_INLINE AttribDataHandle* NetworkDef::getAttribDataHandle(
  AttribDataSemantic semantic,
  NodeID             owningNodeID) const
{
  NMP_ASSERT(owningNodeID < m_numNodes);
  return m_nodes[owningNodeID]->getAttribDataHandle(semantic);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribDataHandle* NetworkDef::getOptionalAttribDataHandle(
  const AttribAddress& attributeAddress) const
{
  NMP_ASSERT(attributeAddress.m_owningNodeID < m_numNodes);
  NMP_ASSERT(attributeAddress.m_animSetIndex < m_numAnimSets && attributeAddress.m_animSetIndex != ANIMATION_SET_ANY);
  return m_nodes[attributeAddress.m_owningNodeID]->getOptionalAttribDataHandle(
    attributeAddress.m_semantic,
    attributeAddress.m_animSetIndex);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribDataHandle* NetworkDef::getOptionalAttribDataHandle(
  AttribDataSemantic semantic,
  NodeID             owningNodeID,
  AnimSetIndex       animSetIndex) const
{
  NMP_ASSERT(owningNodeID < m_numNodes);
  NMP_ASSERT(animSetIndex < m_numAnimSets && animSetIndex != ANIMATION_SET_ANY);
  return m_nodes[owningNodeID]->getOptionalAttribDataHandle(semantic, animSetIndex);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribDataHandle* NetworkDef::getOptionalAttribDataHandle(
  AttribDataSemantic semantic,
  NodeID             owningNodeID) const
{
  NMP_ASSERT(owningNodeID < m_numNodes);
  return m_nodes[owningNodeID]->getOptionalAttribDataHandle(semantic, 0);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribData* NetworkDef::getAttribData(const AttribAddress& attributeAddress) const
{
  NMP_ASSERT(attributeAddress.m_owningNodeID < m_numNodes);
  NMP_ASSERT(attributeAddress.m_animSetIndex < m_numAnimSets && attributeAddress.m_animSetIndex != ANIMATION_SET_ANY);
  NMP_ASSERT(attributeAddress.m_targetNodeID == INVALID_NODE_ID); // NOTE:: Def AttribData data does not and cannot use the target node ID as an address specifier.
  AttribDataHandle* attribDataHandle = m_nodes[attributeAddress.m_owningNodeID]->getAttribDataHandle(
    attributeAddress.m_semantic,
    attributeAddress.m_animSetIndex);
  NMP_ASSERT(attribDataHandle);
  NMP_ASSERT_MSG(attribDataHandle->m_attribData,
    "The AttribDataHandle is valid but the data is NULL for %s, owningNodeID %d, animSetIndex %d", 
    attributeAddress.getAttribSemanticName(), 
    attributeAddress.m_owningNodeID,
    attributeAddress.m_animSetIndex);
  return attribDataHandle->m_attribData;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribData* NetworkDef::getOptionalAttribData(
  AttribDataSemantic semantic,
  NodeID             owningNodeID,
  AnimSetIndex       animSetIndex/*=0*/) const
{
  NMP_ASSERT(owningNodeID < m_numNodes);
  NMP_ASSERT(animSetIndex < m_numAnimSets && animSetIndex != ANIMATION_SET_ANY);
  AttribDataHandle* attribDataHandle = m_nodes[owningNodeID]->getOptionalAttribDataHandle(
    semantic,
    animSetIndex);
  if( ! (attribDataHandle && attribDataHandle->m_attribData))
    return NULL;
  return attribDataHandle->m_attribData;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribData* NetworkDef::getAttribData(
  AttribDataSemantic semantic,
  NodeID             owningNodeID,
  AnimSetIndex       animSetIndex/*=0*/) const
{
  NMP_ASSERT(owningNodeID < m_numNodes);
  NMP_ASSERT(animSetIndex < m_numAnimSets && animSetIndex != ANIMATION_SET_ANY);
  AttribDataHandle* attribDataHandle = m_nodes[owningNodeID]->getAttribDataHandle(
    semantic,
    animSetIndex);
  NMP_ASSERT(attribDataHandle);
  NMP_ASSERT_MSG(attribDataHandle->m_attribData,
    "The AttribDataHandle is valid but the data is NULL for %s, owningNodeID %d, animSetIndex %d", 
    AttribAddress::getAttribSemanticName(semantic), 
    owningNodeID,
    animSetIndex);
  return attribDataHandle->m_attribData;
}

//----------------------------------------------------------------------------------------------------------------------
template <class T>
NM_INLINE T* NetworkDef::getAttribData(const AttribAddress& attributeAddress) const
{
  NMP_ASSERT(attributeAddress.m_owningNodeID < m_numNodes);
  NMP_ASSERT(attributeAddress.m_animSetIndex < m_numAnimSets && attributeAddress.m_animSetIndex != ANIMATION_SET_ANY);
  NMP_ASSERT(attributeAddress.m_targetNodeID == INVALID_NODE_ID); // NOTE:: Def AttribData data does not and cannot use the target node ID as an address specifier.
  AttribDataHandle* attribDataHandle = m_nodes[attributeAddress.m_owningNodeID]->getAttribDataHandle(
    attributeAddress.m_semantic,
    attributeAddress.m_animSetIndex);
  NMP_ASSERT(attribDataHandle);
  NMP_ASSERT_MSG(attribDataHandle->m_attribData,
    "The AttribDataHandle is valid but the data is NULL for %s, owningNodeID %d, animSetIndex %d", 
    attributeAddress.getAttribSemanticName(), 
    attributeAddress.m_owningNodeID,
    attributeAddress.m_animSetIndex);
  NMP_ASSERT(T::getDefaultType() == attribDataHandle->m_attribData->getType());
  return static_cast<T*>(attribDataHandle->m_attribData);
}

//----------------------------------------------------------------------------------------------------------------------
template <class T>
NM_INLINE T* NetworkDef::getAttribData(AttribDataSemantic semantic,
                                       NodeID             owningNodeID,
                                       AnimSetIndex       animSetIndex) const
{
  NMP_ASSERT(owningNodeID < m_numNodes);
  NMP_ASSERT(animSetIndex < m_numAnimSets && animSetIndex != ANIMATION_SET_ANY);
  AttribDataHandle* attribDataHandle = m_nodes[owningNodeID]->getAttribDataHandle(
    semantic,
    animSetIndex);
  NMP_ASSERT(attribDataHandle);
  NMP_ASSERT_MSG(attribDataHandle->m_attribData,
    "The AttribDataHandle is valid but the data is NULL for %s, owningNodeID %d, animSetIndex %d", 
    AttribAddress::getAttribSemanticName(semantic), 
    owningNodeID,
    animSetIndex);
  NMP_ASSERT(T::getDefaultType() == attribDataHandle->m_attribData->getType());
  return static_cast<T*>(attribDataHandle->m_attribData);
}
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const NMP::IDMappedStringTable* NetworkDef::getStateMachineStateNameToStateIDTable() const
{
  return m_stateMachineStateIDStringTable;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const NMP::IDMappedStringTable* NetworkDef::getNodeIDNamesTable() const
{
  return m_nodeIDNamesTable;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const NMP::OrderedStringTable* NetworkDef::getEventTrackIDNamesTable() const
{
  return m_eventTrackIDNamesTable;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NetworkDef::getNumStateMachines() const
{
  return m_stateMachineNodeIDs->getNumEntries();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NodeID NetworkDef::getStateMachineNodeID(uint32_t index) const
{
  return m_stateMachineNodeIDs->getEntry(index);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NetworkDef::getNumMultiplyConnectedNodes() const
{
  return m_multiplyConnectedNodeIDs->getNumEntries();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NodeID NetworkDef::getMultiplyConnectedNodeID(uint32_t index) const
{
  return m_multiplyConnectedNodeIDs->getEntry(index);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const NMP::OrderedStringTable* NetworkDef::getMessageIDNamesTable() const
{
  return m_messageIDNamesTable;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const NodeTagTable* NetworkDef::getNodeTagTable() const
{
  return m_tagTable;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const MessageDistributor* NetworkDef::getMessageDistributor(MR::MessageID messageID) const
{
  NMP_ASSERT(messageID < m_numMessageDistributors);
  return m_messageDistributors[messageID];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NetworkDef::getMaximumAnimSetBoneCount() const
{
  return m_maxBonesInAnimSets;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool NetworkDef::canCombineTrajectoryAndTransformSemantics() const
{
  return m_canCombineTrajectoryTransformTasks;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NetworkDef::setCanCombineTrajectoryAndTransformSemantics(bool value)
{
  m_canCombineTrajectoryTransformTasks = value;
}

//----------------------------------------------------------------------------------------------------------------------
// EmittedControlParamsInfo
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Memory::Format EmittedControlParamsInfo::getInstanceMemoryRequirements() const
{
  return getMemoryRequirements(m_numEmittedControlParamNodes);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t EmittedControlParamsInfo::getNumEmittedControlParams() const
{
  return m_numEmittedControlParamNodes;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const EmittedControlParamsInfo::EmittedControlParamData&
EmittedControlParamsInfo::getEmittedControlParamData(uint32_t index) const
{
  NMP_ASSERT(index < m_numEmittedControlParamNodes);
  return m_emittedControlParamsData[index];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void EmittedControlParamsInfo::setEmittedControlParamData(
  uint32_t index,
  NodeID   nodeID)
{
  NMP_ASSERT(index < m_numEmittedControlParamNodes);
  m_emittedControlParamsData[index].m_nodeID = nodeID;
}

//----------------------------------------------------------------------------------------------------------------------
// NodeIDsArray
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Memory::Format NodeIDsArray::getInstanceMemoryRequirements() const
{
  return getMemoryRequirements(m_numEntries);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NodeIDsArray::getNumEntries() const
{
  return m_numEntries;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NodeID NodeIDsArray::getEntry(uint32_t indx) const
{
  NMP_ASSERT(indx < m_numEntries);
  return m_nodeIDs[indx];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NodeIDsArray::setEntry(uint32_t indx, NodeID nodeID)
{
  NMP_ASSERT(indx < m_numEntries);
  m_nodeIDs[indx] = nodeID;
}

//----------------------------------------------------------------------------------------------------------------------
