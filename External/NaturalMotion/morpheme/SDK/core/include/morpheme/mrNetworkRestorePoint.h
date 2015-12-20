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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef MR_NETWORK_RESTORE_POINT_H
#define MR_NETWORK_RESTORE_POINT_H
//----------------------------------------------------------------------------------------------------------------------
#include "mrNetwork.h"
#include "NMPlatform/NMHashMap.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::NetworkRestorePoint
/// \brief A network restore point takes a snapshot of a network in its current state which can then be applied to any
///  network that shares the same definition. Restore points are packed into a single memory bundle which allows them 
///  to be copied to a different memory location (or machine) and still function correctly. A restore point must be
///  dislocated before it can be moved and located before it can be used.
/// \ingroup NetworkModule
//----------------------------------------------------------------------------------------------------------------------
class NetworkRestorePoint
{
public:
  /// \brief Internal structure used to avoid iterating over the network unnecessarily
  struct CompileInfo
  {
    uint32_t m_numUniqueAttribDatas;
    uint32_t m_numOutputControlParams;
    uint32_t m_numPostProcessAttribNodes;
  };

  typedef NMP::hash_map<AttribData*, uint16_t> AttribDataToTableIndexMap;

  /// \brief Calculate the memory requirements of a restore point based on the network instance. If the network changes
  ///  state in any way then the memory requirements to create a restore point may change.
  static NMP::Memory::Format getMemoryRequirements(
    const Network* network,                                  ///< The state of this network will be captured
    AttribDataToTableIndexMap* attribDataToHandleMap = NULL, ///< Optional AttribDataToTableIndexMap used to map multiply
                                                             ///< referenced attrib data. An internal map will be allocated
                                                             ///< if this is left null.
    CompileInfo* compileInfo = NULL                          ///< Optional compile info filled internally to prevent
                                                             ///< future iterations over data when creating the restore
                                                             ///< point.
    );

  /// \brief Create a network restore point for the supplied network instance.
  static NetworkRestorePoint* createAndInit(
    const Network* network,
    NMP::MemoryAllocator* allocator = NULL,                 ///< Optional memory allocator used to create the restore point
    AttribDataToTableIndexMap* attribDataToHandleMap = NULL ///< Optional map required to create a restore point. Will 
                                                            ///< be created with the NMPlatform allocator if not provided.
    );

  /// \brief Use this restore point to set the state of the supplied network back to the condition
  ///  it was in when the restore point was taken.  Note that this function will not destroy any data in
  ///  the restore point.  The position and orientation of the character controller in  the applied network
  ///  will not be affected by this function.
  void applyToNetwork(
    Network* network,                       ///< This network will be restored to the state the restore point was created at.
    NMP::MemoryAllocator* allocator = NULL  ///< Optional allocator used to create memory for the lifespan of the function.
                                            ///< The NMPlatform allocator will be used if this argument is not provided.
    ) const;

  /// \brief Dislocates the restore point allowing it to be relocated to a different memory location.
  void dislocate(
    bool storeDefID ///< If true the network definition object id stored in the manager will be set in the bundle. When
                    ///< locate is called the object id can be checked against the one associated with the network def
                    ///< provided.
    );

  /// \brief Locates a dislocated restore point. Must be called after a dislocate before a restore point can be used on
  ///  a network. Will fail if the network def differs from the one it was created with and validateNetworkDef is set to
  ///  true
  bool locate(
    NetworkDef* netDef,       ///< The network definition that the restore point was created with
    bool validateNetworkDef   ///< If true the network definition object id will be validated against the one stored in
                              ///< the restore point. A mismatch will cause locate to fail. If there is no object id in
                              ///< the restore point validation won't be done.
    );

  /// \brief The memory used to store the entire restore point.
  NM_INLINE NMP::Memory::Format getMemoryUsage() const;

#ifdef MR_NETWORK_LOGGING
  /// \brief prints a breakdown of memory usage in the restore point. The usage is broken into the structure, node bins,
  /// node connections, output control parameters, post update access attributes and the attribute table.
  void printMemoryUsage(uint32_t priority);
#endif

protected:

  /// \brief StateMachineAttribDatas store a callback pointer that cannot be restored, 
  ///  so we just maintain those values that are set on the existing Network.
  Network::StateMachineStateChangeCallback** cacheStateMachineStateChangeCallbacks(
    Network*              network,
    NMP::MemoryAllocator* allocator) const;

  /// \brief Apply cached StateMachineAttribData callback pointers.
  void applyCachedStateMachineStateChangeCallbacks(
    Network::StateMachineStateChangeCallback** stateMachineCallbacks,
    Network*                                   network,
    NMP::MemoryAllocator*                      allocator) const;

  /// \class SerialisableNodeBinEntry
  /// \brief Serialisable version of NodeBinEntry.
  struct SerialisableNodeBinEntry
  {
    AttribAddress       m_address;
    uint16_t            m_attributeIndex; ///< Index into the attrib table in the restore point
    uint16_t            m_lifespan;
  };

  /// \class SerialisableNodeBin
  /// \brief Serialisable version of NodeBin that stores an array of NodeBinEntryS
  struct SerialisableNodeBin
  {
    FrameCount                m_lastFrameUpdate;      ///< The last frame that connections was updated on this node.
    uint32_t                  m_frameLifespanUpdated; ///< The frame at which the lifespan of this nodes attributes was last updated.
    AnimSetIndex              m_outputAnimSet;
    uint16_t                  m_numBinEntries;
    SerialisableNodeBinEntry* m_binEntries;

    NM_INLINE static NMP::Memory::Format getInternalMemoryRequirements(const Network* net, NodeID id);

    void init(
      NMP::Memory::Resource* resource,
      const NodeBinEntry* networkBinEntry,
      const AttribDataToTableIndexMap& attribDataMap,
      uint16_t& attribTableIndex,
      const CompileInfo& NMP_USED_FOR_ASSERTS(compileInfo));

    NM_INLINE void dislocate();
    NM_INLINE void locate();
  };

  /// \class SerialisableOutputCPPin
  /// \brief Serialisable version of OutputCPPin
  struct SerialisableOutputCPPin
  {
    FrameCount m_lastUpdateFrame;
    AttribDataHandle m_attribDataHandle;

    NM_INLINE void dislocate();
    NM_INLINE void locate();
  };

  /// \class AttribDataTable
  /// \brief Stores all attrib data from the networks node bins. Attrib data may be referenced multiple times by different
  ///  bin entries.
  struct AttribDataTable
  {
    /// \brief Calculates the memory required for the attrib data table excluding the structure itself.
    static NMP::Memory::Format getInternalMemoryRequirements(
      const Network* network,
      AttribDataToTableIndexMap* attribDataToHandleMap,
      CompileInfo* compileInfo);

    void init(
      NMP::Memory::Resource* resource,
      const Network*         network,
      const CompileInfo&     compileInfo,
      const AttribDataToTableIndexMap& attribDataToHandleMap);

    NM_INLINE void dislocate();
    NM_INLINE void locate(MR::NetworkDef* netDef);

    AttribDataHandle* m_attribDataArray;
    uint32_t m_numAttribDatas;
  };

  /// \class SerialisableNodeConnections
  /// \brief Serialisable version of NodeConnections
  struct SerialisableNodeConnections
  {
    NodeID   m_activeParentNodeID;  ///< The active up stream dependent node.
                                    ///<  Other nodes may reference us as a child (through the pass down system).
                                    ///<  But we can only have upstream dependency on this nodes data.
    NodeID*  m_activeChildNodeIDs;  ///< The active down stream dependent nodes.
                                    ///<  We can have dependency on data from any or all of these nodes.
    uint16_t m_numActiveChildNodes;
    bool     m_justBecameActive;    ///< Is this the first update for this node?
    bool     m_active;              ///<

    NM_INLINE static NMP::Memory::Format getInternalMemoryRequirements(const NodeConnections& nodeConnection);

    NM_INLINE void init(NMP::Memory::Resource* resource, const NodeConnections& nodeConnection);
    NM_INLINE void locate();
    NM_INLINE void dislocate();
  };

  /// \class SerialisablePostUpdateAccessAttribEntry
  /// \brief Serialisable version of PostUpdateAccessAttribEntry
  struct SerialisablePostUpdateAccessAttribEntry
  {
    NM_INLINE void dislocate();

    NM_INLINE void locate();

    NodeID             m_nodeID;
    AttribDataSemantic m_semantic;
    uint16_t           m_minLifespan;
    uint16_t           m_refCount;
  };

  /// \class PostUpdateAccessAttribHead
  /// \brief Stores a list of post update access attributes
  struct PostUpdateAccessAttribHead
  {
    NM_INLINE void dislocate();

    NM_INLINE void locate();

    NM_INLINE static NMP::Memory::Format getInternalMemoryRequirements(const Network::PostUpdateAccessAttribEntry* entry);

    NM_INLINE void init(NMP::Memory::Resource* resource, NodeID nodeID, const Network::PostUpdateAccessAttribEntry* firstEntry);

    uint32_t m_numPostAccessAttribs;
    SerialisablePostUpdateAccessAttribEntry* m_postAccessAttribs;
    NodeID m_nodeID;
  };

  void initNodeBinsAndConnections(
    NMP::Memory::Resource* resource,
    const Network*         network,
    const CompileInfo&     compileInfo,
    const AttribDataToTableIndexMap& attribDataMap);

  void initOutputControlParams(
    NMP::Memory::Resource* resource,
    const Network*         network,
    const CompileInfo&     compileInfo);

  void initPostUpdateAccessAttribEntries(
    NMP::Memory::Resource* resource,
    const Network*         network,
    const CompileInfo&     compileInfo);

  uint32_t                    m_currentFrameNo;
  AnimSetIndex                m_activeAnimSetIndex;
  Network::RootControlMethod  m_rootControlMethod;

  union
  {
    NetworkDef* m_netDef; ///< In use when the restore point is located
    ObjectID    m_netID;  ///< In use when the restore point is dislocated
  };

  NMP::Memory::Format m_format;
  float               m_lastUpdateTimeStep;

  SerialisableNodeBin*         m_nodeBins;
  SerialisableNodeConnections* m_activeNodesConnections;
  SerialisableOutputCPPin*     m_outputControlParamLastUpdateFrames;
  PostUpdateAccessAttribHead*  m_postUpdateAccessAttribEntries;

  uint32_t m_numPostUpdateAccessAttribs;
  uint32_t m_numOutputControlParams;


  static const uint32_t MAX_ACTIVE_PHYSICS_NODE_IDS = 32;
  NodeID                m_activePhysicsNodeIDs[MAX_ACTIVE_PHYSICS_NODE_IDS];
  uint32_t              m_numActivePhysicsNodes;

  static const uint32_t MAX_ACTIVE_GROUPER_NODE_IDS = 32;
  NodeID                m_activeGrouperNodeIDs[MAX_ACTIVE_GROUPER_NODE_IDS];
  uint32_t              m_numActiveGrouperNodes;

  AttribDataTable m_attribDataTable;

  // If this assert fails then the NodeBinEntry has changed sized. SerialisableNodeBin needs to be updated to reflect these changes
  #ifdef NM_HOST_64_BIT
    NM_ASSERT_COMPILE_TIME(sizeof(NodeBinEntry) == 64);
  #else
    NM_ASSERT_COMPILE_TIME(sizeof(NodeBinEntry) == 36);
  #endif

  // If this assert fails then the NodeBin has changed sized. SerialisableNodeBin needs to be updated to reflect these changes
  #ifdef MR_ATTRIB_DEBUG_BUFFERING
    #ifdef NM_HOST_64_BIT
      NM_ASSERT_COMPILE_TIME(sizeof(NodeBin) == 56);
    #else
      NM_ASSERT_COMPILE_TIME(sizeof(NodeBin) == 28);
    #endif
  #else
    #ifdef NM_HOST_64_BIT
      NM_ASSERT_COMPILE_TIME(sizeof(NodeBin) == 48);
    #else
      NM_ASSERT_COMPILE_TIME(sizeof(NodeBin) == 24);
    #endif
  #endif//MR_ATTRIB_DEBUG_BUFFERING

  // If this assert fails then the OutputCCPin has changed sized. SerialisableOutputCCPin needs to be updated to reflect these changes
  #ifdef NM_HOST_64_BIT
    NM_ASSERT_COMPILE_TIME(sizeof(OutputCPPin) == 32);
  #else
    NM_ASSERT_COMPILE_TIME(sizeof(OutputCPPin) == 16);
  #endif

  // If this assert fails then the NodeConnections has changed sized. SerialisableNodeConnections needs to be updated
  // to reflect these changes
  #ifdef NM_HOST_64_BIT
    NM_ASSERT_COMPILE_TIME(sizeof(NodeConnections) == 24);
  #else
    NM_ASSERT_COMPILE_TIME(sizeof(NodeConnections) == 12);
  #endif

  // If this assert fails then the PostUpdateAccessAttribEntry has changed sized. SerialisablePostUpdateAccessAttribEntry
  // needs to be updated to reflect these changes
  #ifdef NM_HOST_64_BIT
    NM_ASSERT_COMPILE_TIME(sizeof(Network::PostUpdateAccessAttribEntry) == 24);
  #else
    NM_ASSERT_COMPILE_TIME(sizeof(Network::PostUpdateAccessAttribEntry) == 16);
  #endif
};

}// namespace MR

#include "mrNetworkRestorePoint.inl"

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NETWORK_RESTORE_POINT_H
//----------------------------------------------------------------------------------------------------------------------
