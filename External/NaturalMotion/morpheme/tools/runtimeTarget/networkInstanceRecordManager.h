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
#ifndef NM_RTT_NETWORKINSTANCERECORDMANAGER_H
#define NM_RTT_NETWORKINSTANCERECORDMANAGER_H
//----------------------------------------------------------------------------------------------------------------------
#include "comms/mcomms.h"
#include <map>
#include <vector>
#include "morpheme/mrInstanceDebugInterface.h"
#include <string>
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{
class FastHeapAllocator;
class LoggingMemoryAllocator;
class MemoryAllocator;
class DataBuffer;
struct PosQuat;
}

namespace MR
{
class Network;
}

namespace MCOMMS
{
class Connection;
class CoreDebugInterfaceComms;
}

class NetworkDefRecord;

//----------------------------------------------------------------------------------------------------------------------
/// \class NodeOutputRecord
/// \brief Simple value class to keep track of which output types are selected for a node.
/// \ingroup RuntimeTarget
//----------------------------------------------------------------------------------------------------------------------
class NodeOutputRecord
{
public:

  NodeOutputRecord(const NodeOutputRecord& from) :
    m_nodeID(from.m_nodeID),
    m_dataType(from.m_dataType),
    m_refCount(from.m_refCount)
  {
  }

  NodeOutputRecord& operator=(const NodeOutputRecord& from)
  {
    m_nodeID = from.m_nodeID;
    m_dataType = from.m_dataType;
    m_refCount = from.m_refCount;

    return (*this);
  }

  NodeOutputRecord(MCOMMS::commsNodeID nodeID, MR::NodeOutputDataType dataType) :
    m_nodeID(nodeID),
    m_dataType(dataType),
    m_refCount(1)
  {
  }

  MCOMMS::commsNodeID getNodeID() const { return m_nodeID; }
  MR::NodeOutputDataType getNodeOutputDataType() const { return m_dataType; }
  uint32_t decRefCount() { NMP_ASSERT(m_refCount > 0); return --m_refCount; }
  void incRefCount() { ++m_refCount; }

  bool operator == (const NodeOutputRecord& other)
  {
    return ((m_nodeID == other.m_nodeID) && (m_dataType == other.m_dataType));
  }

private:

  MCOMMS::commsNodeID       m_nodeID;
  MR::NodeOutputDataType    m_dataType;

  uint32_t                  m_refCount;
};

typedef std::vector<NodeOutputRecord> NodeOutputList;

//----------------------------------------------------------------------------------------------------------------------
/// \class NetworkInstanceRecord
/// \brief Decorates a network instance with additional information as required by the runtime target to operate.
/// \ingroup RuntimeTarget
///
/// In addition the record also loads creates/destroys morpheme network instances and provides additional utility
/// functions for example to compute the network output in world space (see computeWorldTransforms()). Each network
/// instance is owned by a single connection (see getOwner()), but can be monitored from many different connections.
//----------------------------------------------------------------------------------------------------------------------
class NetworkInstanceRecord
{
public:

  /// \brief Initializes the record with the given data but does not create an actual network instance yet (see init()).
  NetworkInstanceRecord(
    MCOMMS::InstanceID  id,
    NetworkDefRecord*   networkDefRecord,
    MCOMMS::Connection* owner,
    const char*         instanceName);
  virtual ~NetworkInstanceRecord();

  /// \brief This custom assignment operator is required as the compiler cannot generate the default assignment operator
  ///  due to const members.
  NetworkInstanceRecord & operator= (const NetworkInstanceRecord & other) {*this = other; return *this;}

  /// \brief Creates a morpheme network instance.
  /// If an instance has already been created or the construction of the morpheme network instance fails for other
  /// reasons false is returned. The initial animation set and character controller position are set as given.
  virtual bool init(
    uint32_t            startingAnimSetIndex,
    const NMP::Vector3& characterStartPosition = NMP::Vector3::InitZero,
    const NMP::Quat&    characterStartRotation = NMP::Quat::kIdentity,
    bool                stepNetwork = true);

  /// \brief Returns the instance ID used to identify this instance over the connection.
  MCOMMS::InstanceID getInstanceID() const { return m_id; }

  /// \brief Returns the network definition used by this instance.
  NetworkDefRecord* getNetworkDefRecord() const { return m_networkDefRecord; }

  /// \brief Returns the connection this instance is owned and controlled by.
  MCOMMS::Connection* getOwner() const { return m_owner; }

  /// \bried Returns the name given to this instance.
  const char* getName() const { return m_name.c_str(); }

public:

  /// \brief Returns the network instance associated with this record.
  MR::Network* getNetwork() { return m_network; }

  /// \brief Requests the given animation set index to be set on the network. The request is cached and applied before
  ///  the next update is executed (this is required to maintain a consistent view on each frame at all times).
  void requestAnimSetIndex(uint32_t animSetIndex) { m_requestedAnimSetIndex = animSetIndex; }

  /// \brief Returns the animation set index currently requested.
  uint32_t getRequestedAnimSetIndex() const { return m_requestedAnimSetIndex; }

  /// Set the root point transform
  void setRootTransform(const NMP::Matrix34& transform);

  /// \brief Compute the world space transforms, putting them in the given buffer.
  /// \return The number of transforms accumulated in to the result.
  uint32_t computeWorldTransforms(NMP::DataBuffer* outputBuffer) const;

  /// \brief Compute the world space transforms, putting them in the given buffer.
  /// \return The number of transforms accumulated in to the result.
  uint32_t computeWorldTransforms(
    uint32_t      numOutputBufferEntries,
    NMP::PosQuat* outputBuffer) const;

  /// \brief Compute the world space transforms, putting them in the given buffer.
  /// \return The number of transforms accumulated in to the result.
  uint32_t computeWorldTransforms(
    uint32_t       numOutputBufferEntries,
    NMP::Matrix34* outputBuffer) const;

  /// \brief Compute the world space transforms, putting them in the given buffer.
  /// \return The number of transforms accumulated in to the result.
  uint32_t computeLocalTransforms(
    uint32_t       numOutputBufferEntries,
    NMP::Matrix34* outputBuffer) const;

public:

  /// \brief Returns the number of node outputs registered with this network instance.
  uint32_t getNumNodeOutputs() const { return (uint32_t)m_nodeOutputs.size(); }

  /// \brief Returns node output record at the given index or NULL if the record does not exist.
  const NodeOutputRecord* getNodeOutputRecord(uint32_t index) const;

  /// \brief Adds the given node output record to this instance.
  void addNodeOutputRecord(MCOMMS::commsNodeID nodeID, MR::NodeOutputDataType dataType);

  /// \brief Removes all node output records matching the given record.
  void removeNodeOutputRecord(MCOMMS::commsNodeID nodeID, MR::NodeOutputDataType dataType);
protected:

  const MCOMMS::InstanceID          m_id;
  NetworkDefRecord*                 const m_networkDefRecord;
  MCOMMS::Connection*               const m_owner;
  std::string                       m_name;

  NMP::FastHeapAllocator*           m_tempAllocator;
  NMP::MemoryAllocator*             m_persistantAllocator;

  MR::Network*                      m_network;
  MR::InstanceDebugInterface        m_debugInterface;    // Per network debug output interface. Stores useful debugging info 
  uint32_t                          m_requestedAnimSetIndex;

  NodeOutputList                    m_nodeOutputs;
};

typedef std::map<MCOMMS::InstanceID, NetworkInstanceRecord*> NetworkInstanceMap;
typedef std::pair<MCOMMS::InstanceID, NetworkInstanceRecord*> NetworkInstanceMapEntry;

//----------------------------------------------------------------------------------------------------------------------
/// \class NetworkInstanceRecordManager
/// \brief Keeps track of the set of network instances created on the runtime target.
/// \ingroup RuntimeTarget
///
/// The actual loading/unloading of network definitions for the runtime target is handled by DefaultAssetMgr. This class
/// simply keeps track of the available set of network instances for use across the various runtime target components.
//----------------------------------------------------------------------------------------------------------------------
class NetworkInstanceRecordManager
{
public:

  NetworkInstanceRecordManager();
  ~NetworkInstanceRecordManager();

  /// \brief Returns the network instance record associated with the given ID or NULL if such a record does not exist.
  NetworkInstanceRecord* findInstanceRecord(MCOMMS::InstanceID id) const;

  /// \brief Adds the given network instance record to the map.
  void addNetworkInstanceRecord(NetworkInstanceRecord* record);

  /// \brief Removes the instance record associated with the given ID.
  void removeNetworkInstanceRecord(MCOMMS::InstanceID id);

  /// \brief Returns the actual network instance record map.
  NetworkInstanceMap& getNetworkInstanceMap() { return m_networkInstances; }

  /// \brief Returns the actual network instance record map.
  const NetworkInstanceMap& getNetworkInstanceMap() const { return m_networkInstances; }

public:

  /// \brief Applies the cached animation set changes.
  /// Animation set change requests during a frame (such as via COMMS) are cached in the network instances records and
  /// to be applied right before the next update. This ensures a consistent view on the frame data at all times.
  void applyAnimationSetChanges() const;

private:

  NetworkInstanceMap                m_networkInstances;
  mutable NetworkInstanceMapEntry   m_cachedNetworkInstance;

  void resetCache();
};

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_RTT_NETWORKINSTANCERECORDMANAGER_H
//----------------------------------------------------------------------------------------------------------------------
