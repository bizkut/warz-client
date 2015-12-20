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
#include "networkInstanceRecordManager.h"

#ifdef NMP_MEMORY_LOGGING
  #include "NMPlatform/NMLoggingMemoryAllocator.h"
#endif // NMP_MEMORY_LOGGING

#include "NMPlatform/NMProfiler.h"
#include "NMPlatform/NMFreelistMemoryAllocator.h"

#include "morpheme/mrBlendOps.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrRuntimeNodeInspector.h"
#include "morpheme/mrUtils.h"
#include "morpheme/mrDebugManager.h"
#include "comms/commsServer.h"
#include "comms/networkManagementUtils.h"

#include "iControllerMgr.h"
#include "iPhysicsMgr.h"

#include "defaultDataManager.h"
#include "networkDefRecordManager.h"
//----------------------------------------------------------------------------------------------------------------------

#ifdef NM_HOST_CELL_PPU
  #include "morpheme/mrDispatcherPS3.h"

  ///< When building for PS3 the SPU optimized dispatcher is used by default.
  ///< To use the basic dispatcher (PPU only) comment out this define.
  ///< *** ONLY DO THIS TEMPORARILY WHILST DEBUGGING ***
  ///< Otherwise it is easy to forget it and continue to add functionality that you will not be testing for execution on SPU.
  #define NM_USE_SPU_DISPATCHER_ON_PS3
#endif

//----------------------------------------------------------------------------------------------------------------------
// NetworkInstanceRecord
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
NetworkInstanceRecord::NetworkInstanceRecord(
  MCOMMS::InstanceID  id,
  NetworkDefRecord*   networkDefRecord,
  MCOMMS::Connection* owner,
  const char*         instanceName) :
  m_id(id),
  m_networkDefRecord(networkDefRecord),
  m_owner(owner),
  m_name(instanceName),
  m_tempAllocator(NULL),
  m_persistantAllocator(NULL),
  m_network(NULL),
  m_requestedAnimSetIndex(0),
  m_nodeOutputs()
{
}

//----------------------------------------------------------------------------------------------------------------------
NetworkInstanceRecord::~NetworkInstanceRecord()
{
  if (m_network)
  {
    m_network->getDispatcher()->releaseAndDestroy();
    m_network->releaseAndDestroy();
  }

  // Deallocate any memory from the per character debug interface.
  m_debugInterface.term();

  MCOMMS::termFastHeapAllocator(&m_tempAllocator);
  MCOMMS::termMemoryAllocator(&m_persistantAllocator);
}

//----------------------------------------------------------------------------------------------------------------------
bool NetworkInstanceRecord::init(
  uint32_t            startingAnimSetIndex,
  const NMP::Vector3& characterStartPosition,
  const NMP::Quat&    characterStartRotation,
  bool                stepNetwork)
{
  // Already created ?
  if (m_network)
  {
    NMP_ASSERT_FAIL();
    return false;
  }

  //----------------------
  // Initialise the global physical environment from the scene settings (including gravity and up direction).
  // NOTE: Connect must have sent environment information to us already for this function to work.
  DefaultDataManager::getPhysicsMgr()->updateEnvironment();

  //----------------------
  // Create allocators
  //
  // Set the temporary memory size to 1024KB. This should be large enough to cope with most networks for now but to
  // accurately determine the temp memory size used, see FastHeapAllocator::PrintAllocations().
  static const size_t tempDataSize = 1024 * 1024;
  m_tempAllocator = MCOMMS::initFastHeapAllocator(tempDataSize, NMP_VECTOR_ALIGNMENT);
  m_persistantAllocator = MCOMMS::initMemoryAllocator();

  //----------------------
  // Create dispatcher
#if defined(NM_HOST_CELL_PPU) && defined(NM_USE_SPU_DISPATCHER_ON_PS3)
  MR::DispatcherPS3* const dispatcher = MR::DispatcherPS3::createAndInit();
#else
  MR::DispatcherBasic* const dispatcher = MR::DispatcherBasic::createAndInit();
#endif // NM_HOST_CELL_PPU
  if (!dispatcher)
  {
    return false;
  }

  //----------------------
  // Create network instance
  MR::NetworkDef* const netDef = m_networkDefRecord->getNetDef();
  m_network = MR::Network::createAndInit(netDef, dispatcher, m_tempAllocator, m_persistantAllocator, 0);
  if (!m_network)
  {
    return false;
  }

  //----------------------
  // Initialise debugging interface.
  m_debugInterface.init(m_id, m_network, MR::DebugManager::getInstance());
  dispatcher->setDebugInterface(&m_debugInterface);

  //----------------------
  // Tell the network we want to watch the trajectory delta and transform output from the root node.
  m_network->addPostUpdateAccessAttrib(netDef->getRootNodeID(), MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, 1);

  // Tell the network we want to watch the trajectory delta transform output from the root node.
  m_network->addPostUpdateAccessAttrib(netDef->getRootNodeID(), MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, 1);

  // Tell the network we want to watch the transform output from the root node.
  m_network->addPostUpdateAccessAttrib(netDef->getRootNodeID(), MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, 1);

  // Tell the network we want to watch the sampled events buffer output from the root node.
  m_network->addPostUpdateAccessAttrib(netDef->getRootNodeID(), MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, 1);

  //----------------------
  // Set the proper initial animation set.
  // This allows the creation of the correct LoD for the physics rig and body.
  // connect, make sure this value matches the anim set in connect.
  m_network->setActiveAnimSetIndex((MR::AnimSetIndex)startingAnimSetIndex);
  requestAnimSetIndex(startingAnimSetIndex);

  //----------------------
  // Create a character controller record.
  IControllerMgr* const controllerManager = DefaultDataManager::getControllerMgr();
  controllerManager->createControllerRecord(m_id, m_network, characterStartPosition, characterStartRotation);
  
  //----------------------
  // Add all control parameters to the output list
  for (MR::NodeID nodeID = 0; nodeID < netDef->getNumNodeDefs(); ++nodeID)
  {
    // If this node is marked as a control parameter we want to get it.
    if (netDef->getNodeDef(nodeID)->getNodeFlags() & MR::NodeDef::NODE_FLAG_IS_CONTROL_PARAM)
    {
      // Control parameter node, has single output pin
      MR::NodeBin* nodeBin = m_network->getNodeBin(nodeID);
      MR::OutputCPPin *outputPin = nodeBin->getOutputCPPin(0);
      const MR::AttribData *attribData = outputPin->getAttribData();
      if(attribData)
      {
        MR::AttribDataType attribType = attribData->getType();
        MR::NodeOutputDataType dataType = NODE_OUTPUT_DATA_TYPE_RUNTIME_TYPE;

        // Identify corresponding node output data type
        switch(attribType)
        {
          case MR::ATTRIB_TYPE_BOOL:                   dataType = NODE_OUTPUT_DATA_TYPE_BOOL;                   break;
          case MR::ATTRIB_TYPE_INT:                    dataType = NODE_OUTPUT_DATA_TYPE_INT;                    break;
          case MR::ATTRIB_TYPE_UINT:                   dataType = NODE_OUTPUT_DATA_TYPE_UINT;                   break;
          case MR::ATTRIB_TYPE_PHYSICS_OBJECT_POINTER: dataType = NODE_OUTPUT_DATA_TYPE_PHYSICS_OBJECT_POINTER; break;
          case MR::ATTRIB_TYPE_FLOAT:                  dataType = NODE_OUTPUT_DATA_TYPE_FLOAT;                  break;
          case MR::ATTRIB_TYPE_VECTOR3:                dataType = NODE_OUTPUT_DATA_TYPE_VECTOR3;                break;
          case MR::ATTRIB_TYPE_VECTOR4:                dataType = NODE_OUTPUT_DATA_TYPE_VECTOR4;                break;
          default:
            NMP_ASSERT_FAIL();
        }

        addNodeOutputRecord((MCOMMS::commsNodeID)nodeID, dataType);
      }
    }
  }

  if(stepNetwork)
  {
    //----------------------
    // We don't want to use a delta time for the first update
    MR::Task* task;
    MR::ExecuteResult execResult;

    m_network->startUpdate(0.0f, true);

    while ((execResult = m_network->update(task)) != MR::EXECUTE_RESULT_COMPLETE)
    {
      if (execResult != MR::EXECUTE_RESULT_IN_PROGRESS)
      {
        // should just be getting the internal physics tasks
        NMP_ASSERT(
          task->m_taskid == MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATECHARACTERCONTROLLER ||
          task->m_taskid == MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATEPHYSICS ||
          task->m_taskid == MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATEROOT);

        (void)task;
      }
    }

    // As NetworkInstanceRecord owns the temp allocator instead of the Network, we are responsible for resetting it.
    m_tempAllocator->reset();
  }

  // ...
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NetworkInstanceRecord::computeLocalTransforms(
  uint32_t NMP_USED_FOR_ASSERTS(numOutputBufferEntries),
  NMP::Matrix34* outputBuffer) const
{
  NMP_ASSERT(outputBuffer);
  const uint32_t resultTransformCount = m_network->getCurrentTransformCount();
  NMP::DataBuffer* sourceBuffer = m_network->getTransforms();

  NMP_ASSERT(numOutputBufferEntries >= resultTransformCount);

  if (resultTransformCount > 0)
  {
    outputBuffer[0].initialise(
      DefaultDataManager::getControllerMgr()->getOrientation(m_id),
      DefaultDataManager::getControllerMgr()->getPosition(m_id));

    // Generate output local transform matrices from the network pos quat buffer.
    MR::AnimRigDef* rig = m_network->getActiveRig();
    NMP::DataBuffer* bindPose = rig->getBindPose()->m_transformBuffer;
    NMP_ASSERT(!sourceBuffer || bindPose->getLength() == sourceBuffer->getLength());
    for (uint32_t i = 1; i < bindPose->getLength(); ++i)
    {
      if (sourceBuffer && sourceBuffer->hasChannel(i))
      {
        outputBuffer[i].initialise(*sourceBuffer->getPosQuatChannelQuat(i), *sourceBuffer->getPosQuatChannelPos(i));
      }
      else
      {
        outputBuffer[i].initialise(*bindPose->getPosQuatChannelQuat(i), *bindPose->getPosQuatChannelPos(i));
      }
    }

    return resultTransformCount;
  }

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NetworkInstanceRecord::computeWorldTransforms(NMP::DataBuffer* outputBuffer) const
{
  return computeNetworkWorldTransforms(
          m_network,
          DefaultDataManager::getControllerMgr()->getPosition(m_id),
          DefaultDataManager::getControllerMgr()->getOrientation(m_id),
          outputBuffer);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NetworkInstanceRecord::computeWorldTransforms(
  uint32_t      numOutputBufferEntries,
  NMP::PosQuat* outputBuffer) const
{
  return computeNetworkWorldTransforms(
          m_network,
          DefaultDataManager::getControllerMgr()->getPosition(m_id),
          DefaultDataManager::getControllerMgr()->getOrientation(m_id),
          numOutputBufferEntries,
          outputBuffer);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NetworkInstanceRecord::computeWorldTransforms(
  uint32_t       numOutputBufferEntries,
  NMP::Matrix34* outputBuffer) const
{
  return computeNetworkWorldTransforms(
          m_network,
          DefaultDataManager::getControllerMgr()->getPosition(m_id),
          DefaultDataManager::getControllerMgr()->getOrientation(m_id),
          numOutputBufferEntries,
          outputBuffer);
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkInstanceRecord::setRootTransform(const NMP::Matrix34& transform)
{
  const NMP::Quat orientation = transform.toQuat();
  const NMP::Vector3& position = transform.translation();

  DefaultDataManager::getControllerMgr()->setPosition(m_id, position);
  DefaultDataManager::getControllerMgr()->setOrientation(m_id, orientation);
}

//----------------------------------------------------------------------------------------------------------------------
const NodeOutputRecord* NetworkInstanceRecord::getNodeOutputRecord(uint32_t index) const
{
  return (index < getNumNodeOutputs() ? &m_nodeOutputs[index] : NULL);
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkInstanceRecord::addNodeOutputRecord(MCOMMS::commsNodeID nodeID, MR::NodeOutputDataType dataType)
{
  const NodeOutputRecord newRecord(nodeID, dataType);

  for (NodeOutputList::iterator it = m_nodeOutputs.begin(); it != m_nodeOutputs.end(); ++it)
  {
    NodeOutputRecord& record = (*it);
    if (record == newRecord)
    {
      // The node output record is already in the list.
      record.incRefCount();
      return;
    }
  }

  m_nodeOutputs.push_back(newRecord);
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkInstanceRecord::removeNodeOutputRecord(MCOMMS::commsNodeID nodeID, MR::NodeOutputDataType dataType)
{
  const NodeOutputRecord record(nodeID, dataType);

  for (NodeOutputList::iterator it = m_nodeOutputs.begin(); it != m_nodeOutputs.end(); )
  {
    if ((*it) == record)
    {
      // Remove if the reference count goes to zero.
      if (!(*it).decRefCount())
      {
        it = m_nodeOutputs.erase(it);
      }
      break; // stop looping, as we keep the node output records unique.
    }
    else
    {
      ++it;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// NetworkInstanceRecordManager
//----------------------------------------------------------------------------------------------------------------------
NetworkInstanceRecordManager::NetworkInstanceRecordManager() :
  m_networkInstances(),
  m_cachedNetworkInstance()
{
  resetCache();
}

//----------------------------------------------------------------------------------------------------------------------
NetworkInstanceRecordManager::~NetworkInstanceRecordManager()
{
  NMP_ASSERT(m_networkInstances.empty());
}

//----------------------------------------------------------------------------------------------------------------------
NetworkInstanceRecord* NetworkInstanceRecordManager::findInstanceRecord(MCOMMS::InstanceID id) const
{
  // Check cached (MRU) value first ...
  if (m_cachedNetworkInstance.first == id)
  {
    return m_cachedNetworkInstance.second;
  }

  // ... then search whole map
  NetworkInstanceMap::const_iterator it = m_networkInstances.find(id);

  if (it == m_networkInstances.end())
  {
    return 0;
  }
  else
  {
    m_cachedNetworkInstance.first = id;
    m_cachedNetworkInstance.second = it->second;

    return it->second;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkInstanceRecordManager::addNetworkInstanceRecord(NetworkInstanceRecord* record)
{
  if (!findInstanceRecord(record->getInstanceID()))
  {
    m_networkInstances.insert(NetworkInstanceMapEntry(record->getInstanceID(), record));
  }
  else
  {
    NMP_MSG("Warning: Network instance record already added!");
    NMP_ASSERT_MSG((findInstanceRecord(record->getInstanceID()) == record), "Error: Network instance ID conflict!");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkInstanceRecordManager::removeNetworkInstanceRecord(MCOMMS::InstanceID id)
{
  if (m_cachedNetworkInstance.first == id)
  {
    resetCache();
  }

  NetworkInstanceMap::iterator it = m_networkInstances.find(id);

  if (it == m_networkInstances.end())
  {
    NMP_ASSERT_FAIL();
  }
  else
  {
    m_networkInstances.erase(it);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkInstanceRecordManager::applyAnimationSetChanges() const
{
  for (NetworkInstanceMap::const_iterator it = m_networkInstances.begin(); it != m_networkInstances.end(); ++it)
  {
    const MR::AnimSetIndex animSetIndex = (MR::AnimSetIndex)it->second->getRequestedAnimSetIndex();
    MR::Network* const network = it->second->getNetwork();

    if ((animSetIndex != network->getActiveAnimSetIndex()) && (animSetIndex < network->getNetworkDef()->getNumAnimSets()))
    {
      network->setActiveAnimSetIndex(animSetIndex);
      IControllerMgr* const controllerManager = DefaultDataManager::getControllerMgr();
      controllerManager->updateControllerRecord(it->second->getInstanceID());
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkInstanceRecordManager::resetCache()
{
  m_cachedNetworkInstance.first = 0XFFFFFFFE;
  m_cachedNetworkInstance.second = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
