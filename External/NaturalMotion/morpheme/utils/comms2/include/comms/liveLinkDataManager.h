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
#ifndef MCOMMS_LIVE_LINK_DATA_MANAGER_H
#define MCOMMS_LIVE_LINK_DATA_MANAGER_H
//----------------------------------------------------------------------------------------------------------------------
#include "comms/simpleDataManager.h"
#include "comms/commsServer.h"
#include "comms/debugPackets.h"
#include "comms/networkManagementUtils.h"
#include "NMPlatform/NMMapContainer.h"
//----------------------------------------------------------------------------------------------------------------------
namespace MR
{
  struct NodeBinEntry;
  struct OutputCPPin;
}

//----------------------------------------------------------------------------------------------------------------------
namespace MCOMMS
{

//----------------------------------------------------------------------------------------------------------------------
// NodeOutput
// A helper class used by LiveLinkDataManager.
//
// This class stores a pair of node ID and data type representing a bit of debug information requested for that node.
// The data manager below maintains a list of per-instance node outputs.
//----------------------------------------------------------------------------------------------------------------------
class NodeOutput
{
public:

  bool operator==(const NodeOutput& other) const
  {
    return ((m_nodeID == other.m_nodeID) && (m_dataType == other.m_dataType));
  }

  MCOMMS::commsNodeID       m_nodeID;
  MR::NodeOutputDataType    m_dataType;
};

//----------------------------------------------------------------------------------------------------------------------
template <typename T> class FixedSizeVector
{
public:
  //-------------------------------
  //
  class iterator
  {
  public:
    friend class FixedSizeVector<T>;

    iterator(T* p) { m_ptr = p; }

    bool operator!=(const iterator& other) const
    {
      return other.m_ptr != m_ptr;
    }

    bool operator==(const iterator& other) const
    {
      return other.m_ptr == m_ptr;
    }

    const T& operator*() const
    {
      return *m_ptr;
    }

    T& operator*()
    {
      return *m_ptr;
    }

    iterator& operator++()
    {
      // Preincrement version
      m_ptr++;
      return (*this);
    }

    iterator operator++(int)
    {
      // Postincrement version
      iterator temp = m_ptr;
      m_ptr++;
      return temp;
    }

    T* m_ptr;
  };

  /// \brief
  static NM_INLINE NMP::Memory::Format getMemoryRequirements(uint32_t maxEntries);
  
  /// \brief
  static NM_INLINE FixedSizeVector* init(NMP::Memory::Resource& resource, uint32_t maxEntries);

  NM_INLINE void push_back(const T& data);

  NM_INLINE iterator begin();
  NM_INLINE iterator end();

  NM_INLINE bool empty() const;
  NM_INLINE uint32_t size() const;

  NM_INLINE void pop_back();

  NM_INLINE T& back();

  NM_INLINE iterator erase(const iterator& iterToErase);

  NM_INLINE T& operator[](uint32_t index) const;

  NM_INLINE iterator find(const T& val);

private:
  T& getData(uint32_t index) const;

  uint32_t m_numElements;
  uint32_t m_maxEntries;

  // The data for the vector is allocated at the end of the class data. No need for an additional pointer.
};

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
NMP::Memory::Format FixedSizeVector<T>::getMemoryRequirements(uint32_t maxEntries)
{
  // You can't create a vector of 0 entries.
  NMP_ASSERT_MSG(maxEntries, "0-length VectorContainer is not allowed");

  NMP::Memory::Format result(sizeof(FixedSizeVector<T>), NMP_VECTOR_ALIGNMENT);

  // Reserve space for the data
  result += NMP::Memory::Format(sizeof(void*) * maxEntries, NMP_NATURAL_TYPE_ALIGNMENT);

  // Align to 16 bytes to allow the structure to be DMAd
  result.size = NMP::Memory::align(result.size, NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
FixedSizeVector<T>* FixedSizeVector<T>::init(
  NMP::Memory::Resource& resource,
  uint32_t               maxEntries)
{
  NMP_ASSERT_MSG(maxEntries, "0-length FixedSizeVector is not allowed");
  NMP_ASSERT_MSG(resource.ptr, "NULL resource passed into FixedSizeVector::init");
  NMP_ASSERT_MSG(resource.format.size, "0-sized resource passed into FixedSizeVector::init");
  NMP_ASSERT_MSG(resource.format.alignment, "0-aligned resource passed into FixedSizeVector::init");

  resource.align(getMemoryRequirements(maxEntries));
  FixedSizeVector<T>* result = (FixedSizeVector<T>*)resource.ptr;
  resource.increment(NMP::Memory::Format(sizeof(FixedSizeVector<T>), NMP_VECTOR_ALIGNMENT));

  result->m_numElements = 0;
  result->m_maxEntries = maxEntries;
  
  uint8_t* currentPtr = (uint8_t*)resource.ptr;
  for (uint32_t i=0; i<maxEntries; ++i)
  {
    new (currentPtr) T;
    currentPtr += sizeof(T);
  }
  
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T> 
T& FixedSizeVector<T>::getData(uint32_t index) const
{
  NMP_ASSERT(index < m_maxEntries);
  T* data = (T*)(this + 1 + index);
  return *data;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void FixedSizeVector<T>::push_back(const T& data)
{
  NMP_ASSERT(m_numElements < m_maxEntries);

  T& dataSlot = getData(m_numElements);
  dataSlot = data;
  ++m_numElements;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
typename FixedSizeVector<T>::iterator FixedSizeVector<T>::begin()
{
  return iterator((T*)(this + 1));
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
typename FixedSizeVector<T>::iterator FixedSizeVector<T>::end()
{
  uint8_t* p = (uint8_t*)(this + 1);
  p += sizeof(T) * m_numElements;
  return iterator((T*)p);
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
typename FixedSizeVector<T>::iterator FixedSizeVector<T>::erase(
  const typename FixedSizeVector<T>::iterator& iterToErase)
{
  NMP_ASSERT_MSG(iterToErase != end(), "NMP::FixedSizeVector::erase called on an invalid iterator");

  // Copy the last element, then decrease the size of the vector.
  iterator result = iterToErase;
  (*result) = back();
  --m_numElements;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void FixedSizeVector<T>::pop_back()
{
  NMP_ASSERT_MSG(m_numElements > 0, "NMP::FixedSizeVector::pop_back called on an empty container");
  --m_numElements;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
T& FixedSizeVector<T>::back()
{
  NMP_ASSERT_MSG(m_numElements > 0, "NMP::FixedSizeVector::back called on an empty container");

  return getData(m_numElements - 1);
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
bool FixedSizeVector<T>::empty() const
{
  return m_numElements == 0;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
uint32_t FixedSizeVector<T>::size() const
{
  return m_numElements;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
T& FixedSizeVector<T>::operator[](uint32_t index) const
{
  NMP_ASSERT(index < size());
  uint8_t* p = (uint8_t*)(this + 1);
  p += sizeof(T) * index;
  return (*(T*)p);
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
typename FixedSizeVector<T>::iterator FixedSizeVector<T>::find(const T& val)
{
  for (iterator iter = begin(); iter != end(); ++iter)
  {
    if ((*iter) == val)
    {
      return iter;
    }
  }

  return end();
}

typedef FixedSizeVector<NodeOutput> NodeOutputList;
typedef NMP::MapContainer<MCOMMS::InstanceID, NodeOutputList*> InstanceNodeOutputs;

//----------------------------------------------------------------------------------------------------------------------
// LiveLinkDataManager
// Implements MCOMMS::SimpleDataManager, MCOMMS::DataManagementInterface.
//
// This implementation adds support for per-node debug information on top of the functionality already provided by
// SimpleDataManager. For this purpose a list of per-instance node output information is maintained and reported on
// to COMMS by this manager.
//
// We also provide a mechanism to disable network control on a per-instance basis. Those instances can still be
// monitored from morpheme:connect but state machine and control parameter changes will be ignored which, amongst other
// possible use cases, can serve to protect AI controlled instanced form accidental interference over COMMS.
//----------------------------------------------------------------------------------------------------------------------
class LiveLinkDataManager :
  public MCOMMS::SimpleDataManager
{
public:

  LiveLinkDataManager(uint32_t maxOutputListEntries, uint32_t maxNumInstances);
  virtual ~LiveLinkDataManager();

  static LiveLinkDataManager* getInstance() { return sm_instance; }

  //----------------------------
  // Returns the node output list associated with the given instance ID or NULL if that list does not exist.
  // This method uses a cached (MRU) to accelerate the repeated lookup of information on the same instance ID.
  const NodeOutputList* findNodeOutputList(MCOMMS::InstanceID id);

  //----------------------------
  // Adds all control parameters in the given network instance to the node outputs. This will make the control
  // parameter values available to morpheme:connect for display in the controls panel.
  bool enableControlParameterBuffering(MCOMMS::InstanceID id);

  //----------------------------
  // Adds the given instance to the control exclusion set. State machine and control parameter changes are
  // ignored for instances in that set.
  void disableNetworkControl(MCOMMS::InstanceID id);

public:

  //----------------------------
  // Uses GameManager to find the morpheme network instance associated with the given instance ID.
  virtual MR::Network* findNetworkByInstanceID(MCOMMS::InstanceID id) const NM_OVERRIDE;

  //----------------------------
  // Uses GameManager to find the morpheme network definitions associated with the given GUID.
  virtual MR::NetworkDef* findNetworkDefByGuid(const MCOMMS::GUID& guid) const NM_OVERRIDE;

public:

  virtual bool sendMessage(
    MCOMMS::InstanceID id,
    MCOMMS::commsNodeID stateMachineNodeID,
    const MR::Message& message) NM_OVERRIDE;

  virtual bool broadcastMessage(
    MCOMMS::InstanceID id,
    const MR::Message& message) NM_OVERRIDE;

  virtual bool setCurrentState(
    MCOMMS::InstanceID  id,
    MCOMMS::commsNodeID stateMachineNodeID,
    MCOMMS::commsNodeID newRootStateID) NM_OVERRIDE;

  virtual bool setControlParameter(
    MCOMMS::InstanceID     id,
    MCOMMS::commsNodeID    nodeID,
    MR::NodeOutputDataType type,
    const void*            cparamData) NM_OVERRIDE;

  virtual uint32_t getNodeOutputCount(MCOMMS::InstanceID id) NM_OVERRIDE;
  virtual MCOMMS::commsNodeID getNodeOutputNodeID(MCOMMS::InstanceID id, uint32_t outputIndex) NM_OVERRIDE;
  virtual MR::NodeOutputDataType getNodeOutputDataType(MCOMMS::InstanceID id, uint32_t outputIndex) NM_OVERRIDE;

  virtual uint32_t getNodeOutputDataLength(
    MCOMMS::InstanceID      id,
    uint32_t                outputIndex,
    MR::NodeID&             owningNodeID,
    MR::NodeID&             targetNodeID,
    MR::AttribDataType&     attribType,
    MR::AttribDataSemantic& attribSemantic,
    MR::AnimSetIndex&       animSetIndex,
    MR::FrameCount&         validFrame) NM_OVERRIDE;

  virtual bool getNodeOutputData(
    MCOMMS::InstanceID id,
    uint32_t           outputIndex,
    void*              destBuffer,
    uint32_t           bufferLength) NM_OVERRIDE;

  virtual bool enableOutputDataBuffering(
    MCOMMS::InstanceID     id,
    MCOMMS::commsNodeID    nodeID,
    MR::NodeOutputDataType dataType,
    bool                   enable) NM_OVERRIDE;

  virtual bool serializeTxTreeNodeAttribData(MCOMMS::InstanceID  instanceID, MCOMMS::Connection *connection) NM_OVERRIDE;

  virtual bool serializeTxNodeTimings(
    MCOMMS::InstanceID  instanceID,
    MCOMMS::Connection* connection) NM_OVERRIDE;

  virtual bool serializeTxControlParamAndOpNodeAttribData(
    MCOMMS::InstanceID  instanceID,
    MCOMMS::Connection* connection) NM_OVERRIDE;

  void serialiseAndBufferDebugAttribData(
    MR::Network*            net, 
    const MR::NodeBinEntry* entry, 
    MCOMMS::Connection*     connection);

  MR::AttribDataSemantic getAttribSemanticFromType(MR::AttribDataType attribType);

  void serialiseAndBufferDebugOutputCPAttribData(
    MR::Network*                 net,
    const MR::OutputCPPin*       outputCPPin,
    const MR::NodeID             owningNodeID,
    const MR::PinIndex           pinIndex,
    const MR::AttribDataSemantic semantic,
    MCOMMS::Connection*          connection);

private:
  LiveLinkDataManager(const LiveLinkDataManager& other);
  LiveLinkDataManager& operator=(const LiveLinkDataManager& other);

  typedef NMP::MapContainer<MCOMMS::InstanceID, bool> ControlExclusionSet;

  static LiveLinkDataManager*   sm_instance;

  InstanceNodeOutputs*          m_instanceNodeOutputs;
  ControlExclusionSet*          m_controlExclusionSet;

  MCOMMS::InstanceID            m_cachedInstanceId;
  const NodeOutputList*         m_cachedNodeOutputs;

  const uint32_t                m_maxOutputListEntries;
  const uint32_t                m_maxNumInstances;
};

} // namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_LIVE_LINK_DATA_MANAGER_H
//----------------------------------------------------------------------------------------------------------------------
