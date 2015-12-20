// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
#include "mrNetworkRestorePoint.h"
#include "morpheme/Nodes/mrNodeStateMachine.h"
#include "morpheme/Nodes/mrNodeCharacterControllerOverride.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// NetworkRestorePoint
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Memory::Format NetworkRestorePoint::getMemoryUsage() const
{
  return m_format;
}

//----------------------------------------------------------------------------------------------------------------------
// NodeBinS
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NetworkRestorePoint::SerialisableNodeBin::getInternalMemoryRequirements(const Network* net, NodeID id)
{
  uint32_t numBinEntries = 0;
  MR::NodeBin* nodeBin = net->getNodeBin(id);
  const MR::NodeBinEntry* binEntry = nodeBin->getEntries();
  while (binEntry)
  {
    ++numBinEntries;
    binEntry = binEntry->m_next;
  }
  return NMP::Memory::Format(sizeof(SerialisableNodeBinEntry) * numBinEntries);
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkRestorePoint::SerialisableNodeBin::dislocate()
{
  UNFIX_SWAP_PTR(SerialisableNodeBinEntry, m_binEntries);
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkRestorePoint::SerialisableNodeBin::locate()
{
  REFIX_SWAP_PTR(SerialisableNodeBinEntry, m_binEntries);
}

//----------------------------------------------------------------------------------------------------------------------
// OutputCCPinS
//----------------------------------------------------------------------------------------------------------------------
void NetworkRestorePoint::SerialisableOutputCPPin::dislocate()
{
  if (m_attribDataHandle.m_attribData)
  {
    MR::Manager& manager = MR::Manager::getInstance();
    AttribDislocateFn dislocateFn = manager.getAttribDislocateFn(m_attribDataHandle.m_attribData->getType());
    if (dislocateFn)
    {
      dislocateFn(m_attribDataHandle.m_attribData);
    }
    UNFIX_SWAP_PTR(AttribData, m_attribDataHandle.m_attribData);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkRestorePoint::SerialisableOutputCPPin::locate()
{
  if (m_attribDataHandle.m_attribData)
  {
    REFIX_SWAP_PTR(AttribData, m_attribDataHandle.m_attribData);
    MR::Manager& manager = MR::Manager::getInstance();

    AttribDataType attribType = m_attribDataHandle.m_attribData->getType();

    AttribLocateFn locateFn = manager.getAttribLocateFn(attribType);
    if (locateFn)
    {
      locateFn(m_attribDataHandle.m_attribData);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataTable
//----------------------------------------------------------------------------------------------------------------------
void NetworkRestorePoint::AttribDataTable::dislocate()
{
  MR::Manager& manager = MR::Manager::getInstance();

  for (uint32_t i = 0; i < m_numAttribDatas; ++i)
  {
    // Dislocate the attrib data
    AttribDataType attribType = m_attribDataArray[i].m_attribData->getType();
    AttribDislocateFn dislocateFn = manager.getAttribDislocateFn(attribType);
    if (dislocateFn)
    {
      dislocateFn(m_attribDataArray[i].m_attribData);
    }
    
    // Dislocate the handle
    UNFIX_SWAP_PTR(AttribData, m_attribDataArray[i].m_attribData);
  }
  UNFIX_SWAP_PTR(AttribDataHandle, m_attribDataArray);
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkRestorePoint::AttribDataTable::locate(MR::NetworkDef* netDef)
{
  MR::Manager& manager = MR::Manager::getInstance();

  REFIX_SWAP_PTR(AttribDataHandle, m_attribDataArray);
  for (uint32_t i = 0; i < m_numAttribDatas; ++i)
  {
    // Locate the handle
    REFIX_SWAP_PTR(AttribData, m_attribDataArray[i].m_attribData);

    // Locate the attrib data
    AttribDataType attribType = m_attribDataArray[i].m_attribData->getType();

    AttribLocateFn locateFn = manager.getAttribLocateFn(attribType);
    if (locateFn)
    {
      locateFn(m_attribDataArray[i].m_attribData);
    }
    else if (attribType == ATTRIB_TYPE_STATE_MACHINE)
    {
      AttribDataStateMachine::locate(m_attribDataArray[i].m_attribData, netDef);
    }
    else if (attribType == ATTRIB_TYPE_C_C_OVERRIDE_CONDITIONS)
    {
      AttribDataCCOverrideConditions::locate(m_attribDataArray[i].m_attribData, netDef);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// NodeConnectionsS
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NetworkRestorePoint::SerialisableNodeConnections::getInternalMemoryRequirements(const NodeConnections& nodeConnection)
{
  return NMP::Memory::Format(sizeof(NodeID) * nodeConnection.m_numActiveChildNodes);
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkRestorePoint::SerialisableNodeConnections::init(NMP::Memory::Resource* resource, const NodeConnections& nodeConnection)
{
  NMP::Memory::Format fmt(sizeof(NodeID) * nodeConnection.m_numActiveChildNodes);
  m_activeChildNodeIDs = (NodeID*)resource->alignAndIncrement(fmt);
  memcpy(m_activeChildNodeIDs, nodeConnection.m_activeChildNodeIDs, sizeof(NodeID) * nodeConnection.m_numActiveChildNodes);
  m_numActiveChildNodes = nodeConnection.m_numActiveChildNodes;
  m_activeParentNodeID = nodeConnection.m_activeParentNodeID;
  m_justBecameActive = nodeConnection.m_justBecameActive;
  m_active = nodeConnection.m_active;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkRestorePoint::SerialisableNodeConnections::locate()
{
  REFIX_SWAP_PTR(NodeID, m_activeChildNodeIDs);
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkRestorePoint::SerialisableNodeConnections::dislocate()
{
  UNFIX_SWAP_PTR(NodeID, m_activeChildNodeIDs);
}

//----------------------------------------------------------------------------------------------------------------------
// PostUpdateAccessAttribHead
//----------------------------------------------------------------------------------------------------------------------
void NetworkRestorePoint::PostUpdateAccessAttribHead::dislocate()
{
  UNFIX_SWAP_PTR(SerialisablePostUpdateAccessAttribEntry, m_postAccessAttribs);
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkRestorePoint::PostUpdateAccessAttribHead::locate()
{
  REFIX_SWAP_PTR(SerialisablePostUpdateAccessAttribEntry, m_postAccessAttribs);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NetworkRestorePoint::PostUpdateAccessAttribHead::getInternalMemoryRequirements(
  const Network::PostUpdateAccessAttribEntry* entry)
{
  uint32_t numPostUpdateAccessAttribs = 0;
  while (entry)
  {
    ++numPostUpdateAccessAttribs;
    entry = entry->m_next;
  }
  return NMP::Memory::Format(sizeof(SerialisablePostUpdateAccessAttribEntry) * numPostUpdateAccessAttribs, NMP_NATURAL_TYPE_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkRestorePoint::PostUpdateAccessAttribHead::init(
  NMP::Memory::Resource* resource,
  NodeID nodeID,
  const Network::PostUpdateAccessAttribEntry* firstEntry)
{
  NMP_ASSERT(firstEntry);

  const Network::PostUpdateAccessAttribEntry* entry = firstEntry;
  m_numPostAccessAttribs = 0;
  m_nodeID = nodeID;
  while (entry)
  {
    ++m_numPostAccessAttribs;
    entry = entry->m_next;
  }

  NMP::Memory::Format fmt(sizeof(SerialisablePostUpdateAccessAttribEntry) * m_numPostAccessAttribs, NMP_NATURAL_TYPE_ALIGNMENT);
  m_postAccessAttribs = (SerialisablePostUpdateAccessAttribEntry*)resource->alignAndIncrement(fmt);

  uint32_t attribI = 0;
  entry = firstEntry;
  while (entry)
  {
    m_postAccessAttribs[attribI].m_nodeID = entry->m_nodeID;
    m_postAccessAttribs[attribI].m_minLifespan = entry->m_minLifespan;
    m_postAccessAttribs[attribI].m_refCount = entry->m_refCount;
    m_postAccessAttribs[attribI].m_semantic = entry->m_semantic;
    ++attribI;
    entry = entry->m_next;
  }
}

//----------------------------------------------------------------------------------------------------------------------
}// namespace MR
