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
#include "morpheme/mrNetworkRestorePoint.h"
#include "morpheme/Nodes/mrNodeStateMachine.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// SerialisableNodeBin functions
//----------------------------------------------------------------------------------------------------------------------
void NetworkRestorePoint::SerialisableNodeBin::init(
  NMP::Memory::Resource*           resource,
  const NodeBinEntry*              networkBinEntry,
  const AttribDataToTableIndexMap& attribDataMap,
  uint16_t&                        attribTableIndex,
  const CompileInfo&               NMP_USED_FOR_ASSERTS(compileInfo))
{
  m_numBinEntries = 0;

  const NodeBinEntry* binEntry = networkBinEntry;
  while (binEntry)
  {
    ++m_numBinEntries;
    binEntry = binEntry->m_next;
  }

  NMP::Memory::Format fmt(sizeof(SerialisableNodeBinEntry) * m_numBinEntries);
  m_binEntries = (SerialisableNodeBinEntry*)resource->alignAndIncrement(fmt);

  uint32_t numBinEntries = 0;
  binEntry = networkBinEntry;
  while (binEntry)
  {
    m_binEntries[numBinEntries].m_address = binEntry->m_address;
    m_binEntries[numBinEntries].m_lifespan = binEntry->m_lifespan;
    m_binEntries[numBinEntries].m_attributeIndex = attribTableIndex;

    bool foundAttribData = attribDataMap.find(binEntry->m_attribDataHandle.m_attribData, &m_binEntries[numBinEntries].m_attributeIndex);
    if (!foundAttribData || m_binEntries[numBinEntries].m_attributeIndex == attribTableIndex)
    {
      ++attribTableIndex;
    }
    NMP_ASSERT(m_binEntries[numBinEntries].m_attributeIndex < compileInfo.m_numUniqueAttribDatas);
    ++numBinEntries;
    binEntry = binEntry->m_next;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataTable functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NetworkRestorePoint::AttribDataTable::getInternalMemoryRequirements(
  const Network*             network,
  AttribDataToTableIndexMap* attribDataToHandleMap,
  CompileInfo*               compileInfo)
{
  const NetworkDef* netDef = network->getNetworkDef();
  const uint32_t nodeDefCount = netDef->getNumNodeDefs();

  attribDataToHandleMap->clear();

  uint16_t attribDataIndex = 0;

  NMP::Memory::Format attribDataMemReqs;
  for (uint32_t i = 0; i < nodeDefCount; ++i)
  {
    MR::NodeBin* nodeBin = network->getNodeBin((NodeID)i);
    const MR::NodeBinEntry* binEntry = nodeBin->getEntries();
    while (binEntry)
    {
      // Multiply referenced attrib data need to be added to the map so they're not added twice.
      if (binEntry->m_attribDataHandle.m_attribData->getRefCount() > 1)
      {
        if (!attribDataToHandleMap->find(binEntry->m_attribDataHandle.m_attribData))
        {
          attribDataToHandleMap->insert(binEntry->m_attribDataHandle.m_attribData, attribDataIndex);
          attribDataMemReqs += binEntry->m_attribDataHandle.m_format;
          ++attribDataIndex;
        }
      }
      else
      {
        attribDataMemReqs += binEntry->m_attribDataHandle.m_format;
        ++attribDataIndex;
      }
      binEntry = binEntry->m_next;
    }
  }

  compileInfo->m_numUniqueAttribDatas = attribDataIndex;

  NMP::Memory::Format memReqs(sizeof(AttribDataHandle) * compileInfo->m_numUniqueAttribDatas, MR_ATTRIB_DATA_ALIGNMENT);
  memReqs += attribDataMemReqs;

  return memReqs;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkRestorePoint::AttribDataTable::init(
  NMP::Memory::Resource*           resource,
  const Network*                   net,
  const CompileInfo&               compileInfo,
  const AttribDataToTableIndexMap& attribDataToHandleMap)
{
  MR::Manager& manager = MR::Manager::getInstance();

  const NetworkDef* netDef = net->getNetworkDef();
  const uint32_t nodeDefCount = netDef->getNumNodeDefs();

  m_numAttribDatas = compileInfo.m_numUniqueAttribDatas;

  NMP::Memory::Format fmt(sizeof(AttribDataHandle) * compileInfo.m_numUniqueAttribDatas, MR_ATTRIB_DATA_ALIGNMENT);
  m_attribDataArray = (AttribDataHandle*)resource->alignAndIncrement(fmt);

  uint32_t attribDataIndex = 0;

  for (uint32_t nodeI = 0; nodeI < nodeDefCount; ++nodeI)
  {
    MR::NodeBin* nodeBin = net->getNodeBin((NodeID)nodeI);
    const MR::NodeBinEntry* binEntry = nodeBin->getEntries();
    while (binEntry)
    {
      uint16_t mappedAttribDataIndex = 0xFFFF;

      // Don't add attrib data if it has already been added. It has already been added if its in the map and the index
      // its mapped too is the current attribDataIndex.
      bool alreadyInsertedIntoBundle = 
                  binEntry->m_attribDataHandle.m_attribData->getRefCount() > 1 &&
                  attribDataToHandleMap.find(binEntry->m_attribDataHandle.m_attribData, &mappedAttribDataIndex) &&
                  mappedAttribDataIndex != attribDataIndex;

      NMP_ASSERT(binEntry->m_attribDataHandle.m_attribData->getRefCount() != IS_DEF_ATTRIB_DATA);

      if (!alreadyInsertedIntoBundle)
      {
        // Copy the attrib data into the bundle
        NMP::Memory::Format binEntryFmt = binEntry->m_attribDataHandle.m_format;
        m_attribDataArray[attribDataIndex].m_format = binEntryFmt;
        m_attribDataArray[attribDataIndex].m_attribData = (AttribData*)resource->alignAndIncrement(binEntryFmt);
        memcpy(m_attribDataArray[attribDataIndex].m_attribData, binEntry->m_attribDataHandle.m_attribData, binEntryFmt.size);

        AttribDataType dataType = m_attribDataArray[attribDataIndex].m_attribData->getType();
        AttribRelocateFn relocateFn = manager.getAttribRelocateFn(dataType);
        if (relocateFn)
        {
          relocateFn(m_attribDataArray[attribDataIndex].m_attribData, m_attribDataArray[attribDataIndex].m_attribData);
        }
        else if (dataType == ATTRIB_TYPE_STATE_MACHINE)
        {
          AttribDataStateMachine::relocate(m_attribDataArray[attribDataIndex].m_attribData, netDef);
        }

        ++attribDataIndex;
      }

      binEntry = binEntry->m_next;
    }
  }
  NMP_ASSERT_MSG(attribDataIndex == m_numAttribDatas, "Number of attrib data initialised differs "
    "from the number allocated for.");
}

//----------------------------------------------------------------------------------------------------------------------
// NetworkRestorePoint functions
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NetworkRestorePoint::getMemoryRequirements(
  const Network*             network,
  AttribDataToTableIndexMap* attribDataToHandleMap,
  CompileInfo*               compileInfo)
{
  CompileInfo localInfo;
  if (!compileInfo)
  {
    compileInfo = &localInfo;
  }

  NetworkDef* netDef = network->getNetworkDef();

  NMP::Memory::Format result(sizeof(NetworkRestorePoint));

  const uint32_t numNodeDefs =  netDef->getNumNodeDefs();

  // m_nodeBins
  // m_activeNodesConnections
  result += NMP::Memory::Format(sizeof(SerialisableNodeBin) * numNodeDefs);
  result += NMP::Memory::Format(sizeof(SerialisableNodeConnections) * numNodeDefs);
  for (uint32_t i = 0; i < numNodeDefs; ++i)
  {
    result += SerialisableNodeBin::getInternalMemoryRequirements(network, (NodeID)i);
    result += SerialisableNodeConnections::getInternalMemoryRequirements(*network->m_activeNodesConnections[i]);
  }

  // MORPH-21371: numOutputControlParams is constant per netDef. numPostProcessAttribs could be tracked on the network
  NMP::Memory::Format outputCPPinAttribDataReq;
  compileInfo->m_numOutputControlParams = 0;
  compileInfo->m_numPostProcessAttribNodes = 0;
  for (uint32_t i = 0; i < numNodeDefs; ++i)
  {
    NodeDef* nodeDef = netDef->getNodeDef((NodeID)i);
    compileInfo->m_numOutputControlParams += nodeDef->getNumOutputCPPins();

    if (network->m_postUpdateAccessAttribEntries[i])
    {
      ++compileInfo->m_numPostProcessAttribNodes;
    }

    for (uint32_t j = 0; j < nodeDef->getNumOutputCPPins(); ++j)
    {
      NodeBin* nodeBin = network->getNodeBin((NodeID)i);
      outputCPPinAttribDataReq += nodeBin->getOutputCPPin((PinIndex)j)->m_attribDataHandle.m_format;
    }
  }

  // m_outputControlParamLastUpdateFrames
  result += NMP::Memory::Format(sizeof(SerialisableOutputCPPin) * compileInfo->m_numOutputControlParams);
  result += outputCPPinAttribDataReq;

  // m_postUpdateAccessAttribEntries
  result += NMP::Memory::Format(sizeof(PostUpdateAccessAttribHead) * compileInfo->m_numPostProcessAttribNodes);
  for (uint32_t i = 0; i < numNodeDefs; ++i)
  {
    Network::PostUpdateAccessAttribEntry* networkEntry = network->m_postUpdateAccessAttribEntries[i];
    if (networkEntry)
    {
      result += PostUpdateAccessAttribHead::getInternalMemoryRequirements(networkEntry);
    }
  }

  // m_attribDataTable
  result += AttribDataTable::getInternalMemoryRequirements(network, attribDataToHandleMap, compileInfo);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NetworkRestorePoint* NetworkRestorePoint::createAndInit(
  const Network*             network,
  NMP::MemoryAllocator*      allocator,
  AttribDataToTableIndexMap* attribDataToHandleMap)
{
  AttribDataToTableIndexMap* attribDataMap = attribDataToHandleMap;
  if (!attribDataMap)
  {
    void* mapMemory = NMPMemoryAllocateFromFormat(AttribDataToTableIndexMap::getMemoryRequirements()).ptr;
    attribDataMap = new(mapMemory) AttribDataToTableIndexMap();
  }

  CompileInfo compileInfo;
  const NMP::Memory::Format memReqs = getMemoryRequirements(network, attribDataMap, &compileInfo);
  NMP::Memory::Resource resource;
  if (allocator)
  {
    resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  }
  else
  {
    resource = NMPMemoryAllocateFromFormat(memReqs);
  }

  NetworkDef* netDef = network->getNetworkDef();

  //---------------------
  // NetworkRestorePoint
  NMP::Memory::Format fmt(sizeof(NetworkRestorePoint));
  NetworkRestorePoint* result = (NetworkRestorePoint*)resource.alignAndIncrement(fmt);
  result->m_netDef = netDef;
  result->m_format = memReqs;
  result->m_currentFrameNo = network->m_currentFrameNo;
  result->m_activeAnimSetIndex = network->getActiveAnimSetIndex();
  result->m_rootControlMethod = network->m_rootControlMethod;
  result->m_lastUpdateTimeStep = network->m_lastUpdateTimeStep;

  //---------------------
  // Physics and grouper nodes
  result->m_numActivePhysicsNodes = network->m_numActivePhysicsNodes;
  result->m_numActiveGrouperNodes = network->m_numActiveGrouperNodes;
  NMP_ASSERT(network->m_numActivePhysicsNodes < MAX_ACTIVE_PHYSICS_NODE_IDS);
  NMP_ASSERT(network->m_numActiveGrouperNodes < MAX_ACTIVE_GROUPER_NODE_IDS);
  if(netDef->isPhysical())
  {
    memcpy(result->m_activePhysicsNodeIDs, network->m_activePhysicsNodeIDs, sizeof(NodeID) * network->m_numActivePhysicsNodes);
    memcpy(result->m_activeGrouperNodeIDs, network->m_activeGrouperNodeIDs, sizeof(NodeID) * network->m_numActiveGrouperNodes);
  }

  result->initNodeBinsAndConnections(&resource, network, compileInfo, *attribDataMap);

  result->initOutputControlParams(&resource, network, compileInfo);

  result->initPostUpdateAccessAttribEntries(&resource, network, compileInfo);

  result->m_attribDataTable.init(&resource, network, compileInfo, *attribDataMap);

  // Make sure we used all of the memory requested
  NMP_ASSERT(resource.format.size == 0);

  // Release local resources
  if (!attribDataToHandleMap)
  {
    attribDataMap->~AttribDataToTableIndexMap();
    NMP::Memory::memFree(attribDataMap);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
Network::StateMachineStateChangeCallback** NetworkRestorePoint::cacheStateMachineStateChangeCallbacks(
  Network*              network,
  NMP::MemoryAllocator* allocator) const
{
  Network::StateMachineStateChangeCallback** stateMachineCallbacks = NULL;
  uint32_t numStateMachines = m_netDef->getNumStateMachines();

  if (numStateMachines == 0)
    return NULL;

  if (allocator)
  {
    stateMachineCallbacks = (Network::StateMachineStateChangeCallback**) NMPAllocatorMemAlloc(
      allocator,
      sizeof(Network::StateMachineStateChangeCallback*) * numStateMachines,
      NMP_NATURAL_TYPE_ALIGNMENT);
  }
  else
  {
    stateMachineCallbacks = (Network::StateMachineStateChangeCallback**) NMPMemoryAlloc(
      sizeof(Network::StateMachineStateChangeCallback*) * numStateMachines);
  }

  NodeID smNodeID;
  for(uint32_t i = 0; i < numStateMachines; ++i)
  {
    smNodeID = m_netDef->getStateMachineNodeID(i);
    stateMachineCallbacks[i] = network->getStateMachineStateChangeCB(smNodeID);
  }

  return stateMachineCallbacks;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkRestorePoint::applyCachedStateMachineStateChangeCallbacks(
  Network::StateMachineStateChangeCallback** stateMachineCallbacks,
  Network*                                   network,
  NMP::MemoryAllocator*                      allocator) const
{
  uint32_t numStateMachines = m_netDef->getNumStateMachines();

  if (numStateMachines == 0)
  {
    NMP_ASSERT(stateMachineCallbacks == NULL);
    return;
  }

  NodeID smNodeID;
  for(uint32_t i = 0; i < numStateMachines; ++i)
  {
    smNodeID = m_netDef->getStateMachineNodeID(i);
    network->setStateMachineStateChangeCB(smNodeID, stateMachineCallbacks[i]);
  }

  if (allocator)
  {
    allocator->memFree(stateMachineCallbacks);
  }
  else
  {
    NMP::Memory::memFree(stateMachineCallbacks);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkRestorePoint::applyToNetwork(Network* network, NMP::MemoryAllocator* allocator) const
{
  NMP_ASSERT(network);
  NMP_ASSERT(m_netDef == network->getNetworkDef());

  //---------------------
  // StateMachineAttribDatas store a callback pointer that cannot be restored, so we just maintain those values that are set on the existing Network.
  Network::StateMachineStateChangeCallback** stateMachineCallbacks = cacheStateMachineStateChangeCallbacks(network, allocator);

  //---------------------
  // Delete all the existing attrib data from the network. Note that if the task
  // queue is not empty then the attrib data handles will point to invalid data
  // after calling this function
  network->release();

  network->m_currentFrameNo = m_currentFrameNo;
  network->m_lastUpdateTimeStep = m_lastUpdateTimeStep;

  //---------------------
  // Physics and grouper node ids
  network->m_numActivePhysicsNodes = m_numActivePhysicsNodes;
  network->m_numActiveGrouperNodes = m_numActiveGrouperNodes;
  NMP_ASSERT(m_numActivePhysicsNodes <= Network::MAX_ACTIVE_PHYSICS_NODE_IDS);
  NMP_ASSERT(m_numActiveGrouperNodes <= Network::MAX_ACTIVE_GROUPER_NODE_IDS);
  memcpy(network->m_activePhysicsNodeIDs, m_activePhysicsNodeIDs, sizeof(NodeID) * m_numActivePhysicsNodes);
  memcpy(network->m_activeGrouperNodeIDs, m_activeGrouperNodeIDs, sizeof(NodeID) * m_numActiveGrouperNodes);

  network->setRootControlMethod(m_rootControlMethod);

  NMP::MemoryAllocator* persistantAllocator = network->getPersistentMemoryAllocator();
  NMP_ASSERT(persistantAllocator);

  AttribData** networkAttribData;
  if (allocator)
  {
    networkAttribData = (AttribData**)NMPAllocatorMemAlloc(
      allocator, sizeof(AttribData*) * m_attribDataTable.m_numAttribDatas, NMP_NATURAL_TYPE_ALIGNMENT);
  }
  else
  {
    networkAttribData = (AttribData**)NMPMemoryAlloc(sizeof(AttribData*) * m_attribDataTable.m_numAttribDatas);
  }
  NMP_ASSERT(networkAttribData);

  // Create all the network attrib data
  for (uint32_t i = 0; i < m_attribDataTable.m_numAttribDatas; ++i)
  {
    AttribDataHandle& sourceAttribData = m_attribDataTable.m_attribDataArray[i];
    // Copy the attrib data into a block of memory and then refix it.
    AttribData* newAttribData = (MR::AttribData*)NMPAllocatorAllocateFromFormat(persistantAllocator, 
      sourceAttribData.m_format).ptr;
    NMP_ASSERT(newAttribData);
    NMP::Memory::memcpy(newAttribData, sourceAttribData.m_attribData, sourceAttribData.m_format.size);
    newAttribData->m_allocator = persistantAllocator;

    // Relocate the asset
    AttribDataType dataType = newAttribData->getType();
    MR::AttribRelocateFn relocateFn = MR::Manager::getInstance().getAttribRelocateFn(dataType);
    if (relocateFn)
    {
      relocateFn(newAttribData, newAttribData);
    }
    else if (dataType == ATTRIB_TYPE_STATE_MACHINE)
    {
      // Specialist relocation of AttribDataStateMachine because it needs to get a pointer to its 
      //  partner AttribDataStateMachineDef data before it can do its relocation.
      AttribDataStateMachine::relocate(newAttribData, m_netDef);
    }


    networkAttribData[i] = newAttribData;
  }

  uint32_t outputPinI = 0;
  uint32_t postUpdateI = 0;
  for (uint32_t nodeI = 0; nodeI < m_netDef->getNumNodeDefs(); ++nodeI)
  {
    // Set the active node connections
    SerialisableNodeConnections& sourceConnection = m_activeNodesConnections[nodeI];
    NodeConnections* networkNodeConns = network->m_activeNodesConnections[nodeI];
    networkNodeConns->m_activeParentNodeID = sourceConnection.m_activeParentNodeID;
    networkNodeConns->m_numActiveChildNodes = sourceConnection.m_numActiveChildNodes;
    networkNodeConns->m_justBecameActive = sourceConnection.m_justBecameActive;
    networkNodeConns->m_active = sourceConnection.m_active;
    networkNodeConns->m_activeGatherTag = false;

    NMP_ASSERT(sourceConnection.m_numActiveChildNodes <= networkNodeConns->m_maxNumActiveChildNodes);
    memcpy(networkNodeConns->m_activeChildNodeIDs, sourceConnection.m_activeChildNodeIDs, sizeof(NodeID) * sourceConnection.m_numActiveChildNodes);

    //---------------------
    // Node bin
    network->m_nodeBins[nodeI].m_lastFrameUpdate = m_nodeBins[nodeI].m_lastFrameUpdate;
    network->m_nodeBins[nodeI].m_frameLifespanUpdated = m_nodeBins[nodeI].m_frameLifespanUpdated;
    network->m_nodeBins[nodeI].m_outputAnimSet = m_nodeBins[nodeI].m_outputAnimSet;

    //---------------------
    // Node bin entries. Network::addAttribData adds data to the end of the list. Adding them in reverse means their
    // order is the same as the original network.
    SerialisableNodeBin& restoreNodeBin = m_nodeBins[nodeI];
    for (uint32_t binI = restoreNodeBin.m_numBinEntries; binI--;)
    {
      SerialisableNodeBinEntry& binEntry = restoreNodeBin.m_binEntries[binI];
      uint32_t attribTableIndex = binEntry.m_attributeIndex;
      const AttribDataHandle& restoreHandle = m_attribDataTable.m_attribDataArray[attribTableIndex];

      // The ref count is increased when the attrib data is added to the network so decrease it
      networkAttribData[attribTableIndex]->refCountDecrease();

      // Add the attrib data to the destination network
      AttribDataHandle handle = { networkAttribData[attribTableIndex], restoreHandle.m_format };
      network->addAttribData(binEntry.m_address, handle, binEntry.m_lifespan);     
    }

    //---------------------
    // Update the output control params
    const uint8_t outputPinCount = network->m_netDef->getNodeDef((NodeID)nodeI)->getNumOutputCPPins();
    NodeBin* nodeBin = network->getNodeBin((NodeID)nodeI);
    for (uint8_t outputPinIndex = 0; outputPinIndex < outputPinCount; ++outputPinIndex)
    {
      const SerialisableOutputCPPin& srcOutputPin = m_outputControlParamLastUpdateFrames[outputPinI++];
      OutputCPPin& dstOutputPin = *nodeBin->getOutputCPPin(outputPinIndex);

      dstOutputPin.m_lastUpdateFrame = srcOutputPin.m_lastUpdateFrame;
      dstOutputPin.m_attribDataHandle = srcOutputPin.m_attribDataHandle;

      if (srcOutputPin.m_attribDataHandle.m_attribData)
      {
        dstOutputPin.m_attribDataHandle.m_attribData =
          (AttribData*)NMPAllocatorAllocateFromFormat(persistantAllocator, srcOutputPin.m_attribDataHandle.m_format).ptr;

        NMP::Memory::memcpy(
          dstOutputPin.m_attribDataHandle.m_attribData,
          srcOutputPin.m_attribDataHandle.m_attribData,
          srcOutputPin.m_attribDataHandle.m_format.size);

        dstOutputPin.m_attribDataHandle.m_attribData->m_allocator = persistantAllocator;

        // Relocate the attrib data
        MR::AttribRelocateFn relocateFn = Manager::getInstance().getAttribRelocateFn(srcOutputPin.m_attribDataHandle.m_attribData->getType());
        if (relocateFn)
        {
          relocateFn(dstOutputPin.m_attribDataHandle.m_attribData, dstOutputPin.m_attribDataHandle.m_attribData);
        }
      }
      else
      {
        dstOutputPin.m_attribDataHandle.m_attribData = NULL;
      }
    }

    //--------
    // Restore all the post access attribs
    NMP::Memory::Format postAccessAttribFmt(sizeof(Network::PostUpdateAccessAttribEntry), NMP_NATURAL_TYPE_ALIGNMENT);
    network->m_postUpdateAccessAttribEntries[nodeI] = NULL;

    const PostUpdateAccessAttribHead& postUpdateAccessAttrib = m_postUpdateAccessAttribEntries[postUpdateI];

    if (postUpdateAccessAttrib.m_nodeID == nodeI)
    {
      Network::PostUpdateAccessAttribEntry* previousNetworkEntry = NULL;
      for (uint32_t entryI = 0; entryI < postUpdateAccessAttrib.m_numPostAccessAttribs; ++entryI)
      {
        SerialisablePostUpdateAccessAttribEntry& entry = postUpdateAccessAttrib.m_postAccessAttribs[entryI];

        Network::PostUpdateAccessAttribEntry* networkEntry = 
          (Network::PostUpdateAccessAttribEntry*)NMPAllocatorAllocateFromFormat(persistantAllocator, postAccessAttribFmt).ptr;

        networkEntry->m_minLifespan = entry.m_minLifespan;
        networkEntry->m_nodeID = entry.m_nodeID;
        networkEntry->m_semantic = entry.m_semantic;
        networkEntry->m_refCount = entry.m_refCount;
        networkEntry->m_prev = previousNetworkEntry;
        networkEntry->m_next = NULL;

        if (previousNetworkEntry)
        {
          previousNetworkEntry->m_next = networkEntry;
        }
        else
        {
          network->m_postUpdateAccessAttribEntries[nodeI] = networkEntry;
        }
        previousNetworkEntry = networkEntry;
      }
      ++postUpdateI;
    }

  }

  //---------------------
  // Restore the active animation set attrib pointer
  NodeBinEntry* entry = network->getAttribDataNodeBinEntry(ATTRIB_SEMANTIC_ACTIVE_ANIM_SET_INDEX, NETWORK_NODE_ID);
  NMP_ASSERT(entry);
  network->m_activeAnimSetIndex = entry->getAttribData<AttribDataUInt>();
  NMP_ASSERT(network->m_activeAnimSetIndex);

  //---------------------
  applyCachedStateMachineStateChangeCallbacks(stateMachineCallbacks, network, allocator);

  if (allocator)
  {
    allocator->memFree(networkAttribData);
  }
  else
  {
    NMP::Memory::memFree(networkAttribData);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkRestorePoint::dislocate(bool storeDefID)
{
  for (uint32_t i = 0; i < m_netDef->getNumNodeDefs(); ++i)
  {
    m_nodeBins[i].dislocate();
    m_activeNodesConnections[i].dislocate();
  }
  UNFIX_SWAP_PTR(SerialisableNodeBin, m_nodeBins);
  UNFIX_SWAP_PTR(SerialisableNodeConnections, m_activeNodesConnections);

  for (uint32_t i = 0; i < m_numOutputControlParams; ++i)
  {
    m_outputControlParamLastUpdateFrames[i].dislocate();
  }
  UNFIX_SWAP_PTR(SerialisableOutputCPPin, m_outputControlParamLastUpdateFrames);
  
  for (uint32_t i = 0; i < m_numPostUpdateAccessAttribs; ++i)
  {
    m_postUpdateAccessAttribEntries[i].dislocate();
  }
  UNFIX_SWAP_PTR(PostUpdateAccessAttribHead, m_postUpdateAccessAttribEntries);

  if (storeDefID)
  {
    ObjectID netID = MR::Manager::getInstance().getObjectIDFromObjectPtr(m_netDef);
    m_netDef = NULL;
    m_netID = netID;
  }
  else
  {
    m_netDef = NULL;
  }

  m_attribDataTable.dislocate();
}

//----------------------------------------------------------------------------------------------------------------------
bool NetworkRestorePoint::locate(MR::NetworkDef* netDef, bool validateNetworkDef)
{
  NMP_ASSERT(netDef);
  if (validateNetworkDef)
  {
    ObjectID netDefID = MR::Manager::getInstance().getObjectIDFromObjectPtr(netDef);
    if (netDefID == INVALID_OBJECT_ID || netDefID != m_netID)
    {
      // Network definition does not match the one used when the restore point was dislocated
      //
      // This check relies on the net def being registered with the manager. If network definitions are not registered
      // with the manager than validation can't be done and validateNetworkDef should be set to false
      return false;
    }
  }
  m_netDef = netDef;

  REFIX_SWAP_PTR(SerialisableNodeBin, m_nodeBins);
  REFIX_SWAP_PTR(SerialisableNodeConnections, m_activeNodesConnections);
  for (uint32_t i = 0; i < netDef->getNumNodeDefs(); ++i)
  {
    m_nodeBins[i].locate();
    m_activeNodesConnections[i].locate();
  }

  REFIX_SWAP_PTR(SerialisableOutputCPPin, m_outputControlParamLastUpdateFrames);
  for (uint32_t i = 0; i < m_numOutputControlParams; ++i)
  {
    m_outputControlParamLastUpdateFrames[i].locate();
  }

  REFIX_SWAP_PTR(PostUpdateAccessAttribHead, m_postUpdateAccessAttribEntries);
  for (uint32_t i = 0; i < m_numPostUpdateAccessAttribs; ++i)
  {
    m_postUpdateAccessAttribEntries[i].locate();
  }

  m_attribDataTable.locate(netDef);

  return true;
}

#ifdef MR_NETWORK_LOGGING
//----------------------------------------------------------------------------------------------------------------------
void NetworkRestorePoint::printMemoryUsage(uint32_t priority)
{
  const uint32_t numNodeDefs = m_netDef->getNumNodeDefs();

  NMP::Memory::Format nodeBinMemoryUsage(sizeof(SerialisableNodeBin) * numNodeDefs);
  NMP::Memory::Format nodeConnectionsMemoryUsage(sizeof(SerialisableNodeConnections) * numNodeDefs);
  NMP::Memory::Format nodeBinAndConnectionUsage = nodeBinMemoryUsage;
  nodeBinAndConnectionUsage += nodeConnectionsMemoryUsage;
  for (uint32_t nodeBinI = 0; nodeBinI < numNodeDefs; ++nodeBinI)
  {
    nodeBinMemoryUsage += NMP::Memory::Format(sizeof(SerialisableNodeBinEntry) * m_nodeBins[nodeBinI].m_numBinEntries);
    nodeBinAndConnectionUsage += NMP::Memory::Format(sizeof(SerialisableNodeBinEntry) * m_nodeBins[nodeBinI].m_numBinEntries);
    nodeConnectionsMemoryUsage += NMP::Memory::Format(sizeof(NodeID) * m_activeNodesConnections[nodeBinI].m_numActiveChildNodes);
    nodeBinAndConnectionUsage += NMP::Memory::Format(sizeof(NodeID) * m_activeNodesConnections[nodeBinI].m_numActiveChildNodes);
  }

  NMP::Memory::Format outputControlParamsUsage(sizeof(SerialisableOutputCPPin) * m_numOutputControlParams);
  NMP::Memory::Format outputControlParamAttribDataUsage;
  for (uint32_t conI = 0; conI < m_numOutputControlParams; ++conI)
  {
    outputControlParamAttribDataUsage += m_outputControlParamLastUpdateFrames[conI].m_attribDataHandle.m_format;
  }
  outputControlParamsUsage += outputControlParamAttribDataUsage;

  NMP::Memory::Format postUpdateAccessAttribsUsage(sizeof(PostUpdateAccessAttribHead) * m_numPostUpdateAccessAttribs);
  for (uint32_t attribI = 0; attribI < m_numPostUpdateAccessAttribs; ++attribI)
  {
    postUpdateAccessAttribsUsage += NMP::Memory::Format(sizeof(SerialisablePostUpdateAccessAttribEntry) * m_postUpdateAccessAttribEntries[attribI].m_numPostAccessAttribs);
  }

  NMP::Memory::Format attribTableMemoryUsage(sizeof(AttribDataHandle) * m_attribDataTable.m_numAttribDatas, MR_ATTRIB_DATA_ALIGNMENT);
  for (uint32_t attribI = 0; attribI < m_attribDataTable.m_numAttribDatas; ++attribI)
  {
    attribTableMemoryUsage += m_attribDataTable.m_attribDataArray[attribI].m_format;
  }

#ifdef NMP_ENABLE_ASSERTS
  // Make sure all data is accounted for
  NMP::Memory::Format total(sizeof(NetworkRestorePoint));
  total += nodeBinAndConnectionUsage;
  total += outputControlParamsUsage;
  total += postUpdateAccessAttribsUsage;
  total += attribTableMemoryUsage;
  NMP_ASSERT(total == m_format);
#endif// NMP_ENABLE_ASSERTS

  NET_LOG_MESSAGE(priority,
    "                                     bytes    percentage\n"
    "Total memory used                  %7i\t\t%f\n"
    "Structure used                     %7i\t\t%f\n"
    "Node bins used                     %7i\t\t%f\n"
    "Node connections used              %7i\t\t%f\n"
    "OutputControlParamaters used       %7i\t\t%f\n"
    "Post update access attributes used %7i\t\t%f\n"
    "Attribute table used               %7i\t\t%f\n",
    m_format.size,
    100.0f,
    sizeof(NetworkRestorePoint),
    (sizeof(NetworkRestorePoint) / (float)m_format.size) * 100.0f,
    nodeBinMemoryUsage.size,
    (nodeBinMemoryUsage.size / (float)m_format.size) * 100.0f,
    nodeConnectionsMemoryUsage.size,
    (nodeConnectionsMemoryUsage.size / (float)m_format.size) * 100.0f,
    outputControlParamsUsage.size,
    (outputControlParamsUsage.size / (float)m_format.size) * 100.0f,
    postUpdateAccessAttribsUsage.size,
    (postUpdateAccessAttribsUsage.size / (float)m_format.size) * 100.0f,
    attribTableMemoryUsage.size,
    (attribTableMemoryUsage.size / (float)m_format.size) * 100.0f
    );
}
#endif//MR_NETWORK_LOGGING

//----------------------------------------------------------------------------------------------------------------------
void NetworkRestorePoint::initNodeBinsAndConnections(
  NMP::Memory::Resource*           resource,
  const Network*                   network,
  const CompileInfo&               compileInfo,
  const AttribDataToTableIndexMap& attribDataMap)
{
  const uint32_t numNodeDefs = network->getNetworkDef()->getNumNodeDefs();

  NMP::Memory::Format fmt(sizeof(SerialisableNodeBin) * numNodeDefs, NMP_NATURAL_TYPE_ALIGNMENT);
  m_nodeBins = (SerialisableNodeBin*)resource->alignAndIncrement(fmt);

  fmt.set(sizeof(SerialisableNodeConnections) * numNodeDefs, NMP_NATURAL_TYPE_ALIGNMENT);
  m_activeNodesConnections = (SerialisableNodeConnections*)resource->alignAndIncrement(fmt);
  uint16_t attribDataIndex = 0;
  for (uint32_t i = 0; i < numNodeDefs; ++i)
  {
    MR::NodeBin* nodeBin = network->getNodeBin((NodeID)i);
    const MR::NodeBinEntry* networkBinEntry = nodeBin->getEntries();
    SerialisableNodeBin& restoreNodeBin = m_nodeBins[i];
    restoreNodeBin.init(resource, networkBinEntry, attribDataMap, attribDataIndex, compileInfo);

    NodeBin& networkNodeBin = network->m_nodeBins[i];
    restoreNodeBin.m_lastFrameUpdate = networkNodeBin.m_lastFrameUpdate;
    restoreNodeBin.m_frameLifespanUpdated = networkNodeBin.m_frameLifespanUpdated;
    restoreNodeBin.m_outputAnimSet = networkNodeBin.m_outputAnimSet;

    m_activeNodesConnections[i].init(resource, *network->m_activeNodesConnections[i]);
  }

}

//----------------------------------------------------------------------------------------------------------------------
void NetworkRestorePoint::initOutputControlParams(
  NMP::Memory::Resource* resource,
  const Network*         network,
  const CompileInfo&     compileInfo)
{
  NMP::Memory::Format fmt(sizeof(SerialisableOutputCPPin) * compileInfo.m_numOutputControlParams, NMP_NATURAL_TYPE_ALIGNMENT);
  m_outputControlParamLastUpdateFrames = (SerialisableOutputCPPin*)resource->alignAndIncrement(fmt);
  m_numOutputControlParams = compileInfo.m_numOutputControlParams;

  MR::Manager& manager = MR::Manager::getInstance();
  const NetworkDef* netDef = network->getNetworkDef();
  const uint32_t numNodeDefs = netDef->getNumNodeDefs();

  uint32_t outputCPIndex = 0;
  for (uint32_t nodeI = 0; nodeI < numNodeDefs; ++nodeI)
  {
    const uint32_t numOutputCPPins = netDef->getNodeDef((NodeID)nodeI)->getNumOutputCPPins();
    NodeBin* nodeBin = network->getNodeBin((NodeID)nodeI);
    for (uint32_t pinI = 0; pinI < numOutputCPPins; ++pinI)
    {
      const OutputCPPin* netOutputPin = nodeBin->getOutputCPPin((PinIndex)pinI);

      const NMP::Memory::Format attribDataFormat = netOutputPin->m_attribDataHandle.m_format;
      AttribData* networkPinAttribData = netOutputPin->m_attribDataHandle.m_attribData;
      SerialisableOutputCPPin& restorePin = m_outputControlParamLastUpdateFrames[outputCPIndex];
      restorePin.m_lastUpdateFrame = netOutputPin->m_lastUpdateFrame;
      restorePin.m_attribDataHandle.m_format = attribDataFormat;

      if (netOutputPin->m_attribDataHandle.m_attribData)
      {
        restorePin.m_attribDataHandle.m_attribData = (AttribData*)resource->alignAndIncrement(attribDataFormat);
        memcpy(restorePin.m_attribDataHandle.m_attribData, networkPinAttribData, attribDataFormat.size);

        AttribDataType dataType = restorePin.m_attribDataHandle.m_attribData->getType();
        AttribRelocateFn relocateFn = manager.getAttribRelocateFn(dataType);
        if (relocateFn)
        {
          relocateFn(restorePin.m_attribDataHandle.m_attribData, restorePin.m_attribDataHandle.m_attribData);
        }
      }
      else
      {
        restorePin.m_attribDataHandle.m_attribData = NULL;
      }

      ++outputCPIndex;
    }
  }
  NMP_ASSERT_MSG(outputCPIndex == compileInfo.m_numOutputControlParams, 
    "Number of output control param pins initialised differs from the number allocated for.");
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkRestorePoint::initPostUpdateAccessAttribEntries(
  NMP::Memory::Resource* resource,
  const Network*         network,
  const CompileInfo&     compileInfo)
{
  NMP::Memory::Format fmt(sizeof(PostUpdateAccessAttribHead) * compileInfo.m_numPostProcessAttribNodes, NMP_NATURAL_TYPE_ALIGNMENT);
  m_postUpdateAccessAttribEntries = (PostUpdateAccessAttribHead*)resource->alignAndIncrement(fmt);
  m_numPostUpdateAccessAttribs = compileInfo.m_numPostProcessAttribNodes;

  const uint32_t numNodeDefs = network->getNetworkDef()->getNumNodeDefs();

  uint32_t postUpdateAccessAttribI = 0;
  for (uint32_t nodeI = 0; nodeI < numNodeDefs; ++nodeI)
  {
    Network::PostUpdateAccessAttribEntry* networkEntry = network->m_postUpdateAccessAttribEntries[nodeI];

    if (networkEntry)
    {
      NMP_ASSERT(postUpdateAccessAttribI < compileInfo.m_numPostProcessAttribNodes);
      m_postUpdateAccessAttribEntries[postUpdateAccessAttribI++].init(resource, (NodeID)nodeI, networkEntry);
    }
  }
  NMP_ASSERT(postUpdateAccessAttribI == m_numPostUpdateAccessAttribs);
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace MR
