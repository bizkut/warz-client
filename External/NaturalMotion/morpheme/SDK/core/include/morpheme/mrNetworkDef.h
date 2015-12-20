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
#ifndef MR_NETWORK_DEF
#define MR_NETWORK_DEF
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "morpheme/mrAttribAddress.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/mrNode.h"
#include "morpheme/mrNetworkLogger.h"
#include "morpheme/mrRig.h"
#include "morpheme/TransitConditions/mrTransitCondition.h"
#include "morpheme/mrTransitDeadBlend.h"
#include "morpheme/mrCharacterControllerDef.h"
#include "morpheme/mrMessageDistributor.h"
#include "morpheme/mrNodeDef.h"
#include "morpheme/mrNodeTagTable.h"
//----------------------------------------------------------------------------------------------------------------------

/// Set the required MR_NODE_INIT_DATA_ALIGNMENT
#define MR_NODE_INIT_DATA_ALIGNMENT 8

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Forward declarations
class NodeDef;
class NetworkDef;
class EmittedControlParamsInfo;
class NodeIDsArray;

//----------------------------------------------------------------------------------------------------------------------
static const uint32_t INVALID_EVENT_TRACK_NAME = 0xFFFFFFFF;

//----------------------------------------------------------------------------------------------------------------------
/// \enum NodeInitDataType
/// \brief Describes the data type of a particular MR::NodeInitData item.
/// \ingroup NetworkModule
//----------------------------------------------------------------------------------------------------------------------
typedef uint16_t NodeInitDataType;
enum
{
  NODE_INIT_DATA_TYPE_STATE_MACHINE = 0,
  NODE_INIT_DATA_TYPE_MAX,
  NODE_INIT_DATA_TYPE_NA = 0xFF
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::NodeInitData
/// \brief Used for passing initialisation data from a transition node to a specific node in the destination of the
///        transition.
/// \ingroup NetworkModule
//----------------------------------------------------------------------------------------------------------------------
NMP_ALIGN_PREFIX(NMP_NATURAL_TYPE_ALIGNMENT) class NodeInitData
{
public:
  NM_INLINE NodeInitDataType getType() { return m_type; }
  NM_INLINE void setType(NodeInitDataType value) { m_type = value; }
  NM_INLINE NodeID getTargetNodeID() { return m_targetNodeID; }
  NM_INLINE void setTargetNodeID(NodeID value) { m_targetNodeID = value; }

  static void locate(NodeInitData* target);
  static void dislocate(NodeInitData* target);

protected:
  NodeInitDataType m_type;         ///<
  NodeID           m_targetNodeID; ///< Which node this init data is intended for.
                                   ///< No specific target can be set using INVALID_NODE_ID.
} NMP_ALIGN_SUFFIX(NMP_NATURAL_TYPE_ALIGNMENT);


//----------------------------------------------------------------------------------------------------------------------
/// \class MR::NodeInitDataArrayDef
/// \brief For passing initialisation data to the child of a node.
/// \ingroup NetworkModule
///
/// Commonly used by transition nodes for initialising their destination sub-states.
//----------------------------------------------------------------------------------------------------------------------
NMP_ALIGN_PREFIX(MR_NODE_INIT_DATA_ALIGNMENT) class NodeInitDataArrayDef
{
public:
  /// \brief
  static NMP::Memory::Format getMemoryRequirements(uint16_t nodeInitDataCount);

  /// \brief
  static NodeInitDataArrayDef* init(NMP::Memory::Resource& resource, uint16_t numNodeInitDatas);

  bool locate();
  bool dislocate();

public:
  NodeInitData** m_nodeInitDataArray;
  uint16_t       m_numNodeInitDatas;
} NMP_ALIGN_SUFFIX(MR_NODE_INIT_DATA_ALIGNMENT);


//----------------------------------------------------------------------------------------------------------------------
/// \class MR::NodeIDsArray
/// \brief A class that stores an array of NodeID values
/// \ingroup NetworkModule
//----------------------------------------------------------------------------------------------------------------------
class NodeIDsArray
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numEntries);

  NM_INLINE NMP::Memory::Format getInstanceMemoryRequirements() const;

  static NodeIDsArray* init(
    NMP::Memory::Resource& memRes,
    uint32_t numNodes);

  void locate();
  void dislocate();
  void relocate();

  NM_INLINE uint32_t getNumEntries() const;
  NM_INLINE NodeID getEntry(uint32_t indx) const;
  NM_INLINE void setEntry(uint32_t indx, NodeID nodeID);

protected:
  uint32_t        m_numEntries;
  NodeID*         m_nodeIDs;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::NetworkDef
/// \brief Generally a MR::NetworkDef defines the animation behaviour of a game character. It is an asset generated by
///  the asset compiler. Loaded at runtime many MR::Networks can be initialised from it.
/// \ingroup NetworkModule
///
/// All data in the MR:NetworkDef is constant and the MR::Network s initialised from it will reference data within it.
//----------------------------------------------------------------------------------------------------------------------
class NetworkDef : public NodeDef
{
public:
  //---------------------------------------------------------------------
  /// \name   NetworkDef Initialisation
  /// \brief  Functions to manage the memory for the NetworkDef.
  //---------------------------------------------------------------------
  //@{
  void locate();
  void dislocate();
  //@}

  //---------------------------------------------------------------------
  /// \name   NodeDef information
  /// \brief  Functions to retrieve information about NodeDefs within the network.
  //---------------------------------------------------------------------
  //@{
  NM_INLINE AnimSetIndex getNumAnimSets() const { return m_numAnimSets; }

  uint32_t getNumNodeDefs() const { return m_numNodes; }
  const NodeDef* getNodeDef(NodeID nodeID) const { NMP_ASSERT(nodeID < m_numNodes); return m_nodes[nodeID]; }
  NodeDef* getNodeDef(NodeID nodeID) { NMP_ASSERT(nodeID < m_numNodes); return m_nodes[nodeID]; }

  /// \brief Get the ID of the Networks root node.
  ///
  ///  This remains static even when the network is executing.
  NodeID getRootNodeID() const { return m_childNodeIDs[0]; }

  /// \brief Get the table that maps: state machine state names <--> state id.
  NM_INLINE const NMP::IDMappedStringTable* getStateMachineStateNameToStateIDTable() const;

  /// \brief Find the StateID for the given state name.
  /// \return INVALID_STATE_ID if the name is not recognized nor a state.
  ///
  /// Asserts if m_stateMachineStateIDStringTable does not exist, or if more than 1 matching entry.
  /// Names must fully match.
  StateID getStateIDFromStateName(const char* stateName) const;

  /// \brief Find the NodeID for the root node for what is contained in the state named.
  /// \return INVALID_NODE_ID if the name is not recognized nor a state.
  ///
  /// Asserts if m_stateMachineStateIDStringTable does not exist, or if more than 1 matching entry.
  /// Names must fully match.
  NodeID getNodeIDFromStateName(const char* stateName) const;

  /// \brief Get the table that maps: node names <--> node id.
  NM_INLINE const NMP::IDMappedStringTable* getNodeIDNamesTable() const;

  /// \brief Get the name of the given nodeID.
  /// m_nodeIDNamesTable must exist and the desired node must exist, asserts if not.
  const char* getNodeNameFromNodeID(NodeID nodeID) const;

  /// \brief Find the NodeID for the given node name.
  /// \return INVALID_NODE_ID if the name is not recognized.
  ///
  /// Asserts if m_nodeIDNamesTable does not exist, or if more than 1 matching entry.
  /// Names must fully match.
  NodeID getNodeIDFromNodeName(const char* nodeName) const; ///< Node name with full path specification e.g. "Locomote|Forward|Run".
                                                            ///< Note: State Machines at the root of a network have no name.
                                                            ///< Note: State Machine states are not stored in this table.
                                                            ///< Must be null terminated.

  /// \brief Searches whole Network for a Node with the specified flags set. Note other flags may also be set.
  bool containsNodeWithFlagsSet(NodeFlags flags) const;

  /// \brief retrieve the shared task function tables
  const SharedTaskFnTables* getTaskQueuingFnTables() const { return m_taskQueuingFnTables; }
  const SharedTaskFnTables* getOutputCPTaskFnTables() const { return m_outputCPTaskFnTables; }

  /// \brief Find the attrib data handle with this address. It must exist
  NM_INLINE AttribDataHandle* getAttribDataHandle(
    const AttribAddress& attributeAddress) const;
  NM_INLINE AttribDataHandle* getAttribDataHandle(
    AttribDataSemantic semantic,
    NodeID             owningNodeID,
    AnimSetIndex       animSetIndex) const;

  /// \brief The same as getAttribDataHandle with animSetIndex = ANIMATION_SET_ANY.
  NM_INLINE AttribDataHandle* getAttribDataHandle(
    AttribDataSemantic semantic,
    NodeID             owningNodeID) const;

  /// \brief Find the attrib data with this address. AttribData must exist.
  NM_INLINE AttribData* getAttribData(
    const AttribAddress& attributeAddress) const;
  NM_INLINE AttribData* getAttribData(
    AttribDataSemantic semantic,
    NodeID             owningNodeID,
    AnimSetIndex       animSetIndex = 0) const;

  /// \brief Find the attrib data with this address. AttribData must exist.
  ///  More secure templated versions that validate type and does a cast to the template type internally.
  template <class T>
  NM_INLINE T* getAttribData(
    const AttribAddress& attributeAddress) const;
  template <class T>
  NM_INLINE T* getAttribData(
    AttribDataSemantic semantic,
    NodeID             owningNodeID,
    AnimSetIndex       animSetIndex = 0) const;

  /// \brief Find the attrib data with this address.
  ///  It is legal that the AttribData we are looking for does not exist.
  NM_INLINE AttribData* getOptionalAttribData(
    AttribDataSemantic semantic,
    NodeID             owningNodeID,
    AnimSetIndex       animSetIndex=0) const;


  /// \brief Find the attrib data handle with this address. It can optionally not exist.
  NM_INLINE AttribDataHandle* getOptionalAttribDataHandle(
    const AttribAddress& attributeAddress) const;
  NM_INLINE AttribDataHandle* getOptionalAttribDataHandle(
    AttribDataSemantic semantic,
    NodeID             owningNodeID,
    AnimSetIndex       animSetIndex) const;

    /// \brief The same as getOptionalAttribDataHandle with animSetIndex = ANIMATION_SET_ANY.
    NM_INLINE AttribDataHandle* getOptionalAttribDataHandle(
    AttribDataSemantic semantic,
    NodeID             owningNodeID) const;


  //@}

  //---------------------------------------------------------------------
  /// \name   Control parameters
  /// \brief  Functions to retrieve information about control parameters
  //---------------------------------------------------------------------
  //@{

  uint32_t getNumControlParameterNodes() const;

  /// \brief Listing control parameter nodes.
  ///
  /// Note that these functions search through the NetworkDefs whole nodes list.
  uint32_t getControlParameterNodeIDs(
    NodeID*   nodeIDs,        ///< Out.
    uint32_t  maxNumNodeIDs   ///< Size of node IDs array.
  ) const;

  /// Emitted control parameters.
  NM_INLINE const EmittedControlParamsInfo* getEmittedControlParamsInfo() const { return m_emittedControlParamsInfo; }
  //@}

  //---------------------------------------------------------------------
  /// \name   State machines
  /// \brief  Functions to retrieve information about state machines
  //---------------------------------------------------------------------
  //@{
  NM_INLINE const NodeIDsArray* getStateMachineNodeIDs() const { return m_stateMachineNodeIDs; }

  /// \brief Access state machine NodeIDs array.
  ///
  /// Fast access to a stored array.
  NM_INLINE uint32_t getNumStateMachines() const;
  NM_INLINE NodeID getStateMachineNodeID(uint32_t index) const;
  //@}

  //---------------------------------------------------------------------
  /// \name   Multiply connected nodes
  /// \brief  Functions to retrieve information about multiply connected nodes
  //---------------------------------------------------------------------
  //@{
  NM_INLINE const NodeIDsArray* getMultiplyConnectedNodeIDs() const { return m_multiplyConnectedNodeIDs; }

  /// \brief Access state machine NodeIDs array.
  ///
  /// Fast access to a stored array.
  NM_INLINE uint32_t getNumMultiplyConnectedNodes() const;
  NM_INLINE NodeID getMultiplyConnectedNodeID(uint32_t index) const;
  //@}

  //---------------------------------------------------------------------
  /// \name   Messages
  /// \brief  Functions to retrieve information about messages
  //---------------------------------------------------------------------
  //@{

  /// \brief get the number of messages.
  uint32_t getNumMessages() const;

  NM_INLINE const NMP::OrderedStringTable* getMessageIDNamesTable() const;

  /// \brief get the message id for the given name.
  /// \return  "Unknown" if the name is not recognized or the debug string table is not present.
  const char* getMessageNameFromMessageID(MessageID messageID) const;

  /// \brief get the message id for the given name.
  /// \return  INVALID_MESSAGE_ID if the name is not recognized or the debug string table is not present.
  MessageID getMessageIDFromMessageName(const char* messageName) const;

  /// \brief Message emitter node IDs
  NM_INLINE const NodeIDsArray* getMessageEmitterNodeIDs() const { return m_messageEmitterNodeIDs; }

  /// \brief Retrieves the node tag table. This table can be used to check if a node has a tag.
  NM_INLINE const NodeTagTable* getNodeTagTable() const;

  //@}

  //---------------------------------------------------------------------
  /// \name   Animation Source
  /// \brief  Functions to retrieve information about animation source and event track data
  //---------------------------------------------------------------------
  //@{

  /// \brief LoadAnimations will use the application-registered animation locater function to fix up the specified animation set
  /// to the animations that it uses.
  ///
  /// This function must be called before the animation set can be used. It is a basic implementation that simply loads
  /// and prepares the animation data using the loader provided by the manager. It presumes that all animations for all
  /// animation sets must be loaded before being able to run the network, which is not completely true. It is possible
  /// to this using streaming mechanisms and only preload the animations here that are needed for the active animSetIdx
  bool loadAnimations(AnimSetIndex animSetIndex, void* userdata = NULL);

  /// \brief unload all of the animations that are currently used by the specified animation set.
  ///
  /// You must call this before freeing the memory that an animation set takes up, or leaks may occur.
  bool unloadAnimations(AnimSetIndex animSetIndex, void* userdata = NULL);

  /// \brief return the rig used by the specified animation set.
  AnimRigDef* getRig(AnimSetIndex animSetIndex);

  /// \brief Get the maximum number of bones used by any rig in this network definition.
  /// \return 0 if the network is not initialised properly
  uint32_t getMaxBoneCount();

  /// \brief return the character controller def used by the specified animation set.
  CharacterControllerDef* getCharacterControllerDef(AnimSetIndex animSetIndex);

  NM_INLINE const NMP::OrderedStringTable* getEventTrackIDNamesTable() const;

  /// \brief Finds the event track runtime id corresponding to the given name.
  ///
  /// NOTE: It is currently possible for more than one event track to have the same name and hence will be
  /// assigned the same runtime ID.
  /// \return INVALID_EVENT_TRACK_NAME if the name is not recognized or the string table is not present.
  NM_INLINE uint32_t getEventTrackRuntimeIDFromName(const char* name) const;

  /// \brief get the track name for the given id.
  /// \return 0 if the id doesn't exist or the string table is not present.
  NM_INLINE const char* getEventTrackNameFromRuntimeID(uint32_t id) const;
  //@}

  /// \brief returns current memory usage for 
  /// m_stateMachineStateIDStringTable, m_nodeIDNamesTable, 
  /// m_eventTrackIDNamesTable & m_requestIDNamesTable
  size_t getStringTableMemoryFootprint() const;

  /// \brief returns a message distributor for a given messageID.
  /// \note The array of distributors can be sparse and contain gaps so this function can return null.
  NM_INLINE const MessageDistributor* getMessageDistributor(MR::MessageID messageID) const;

  /// \brief  To allow preset messages to be called from the runtime. 
  ///         This allows them to be called from UE4 amongst uses.
  const MessageDistributor* getMessageDistributorAndPresetMessage(
    MR::MessageType       messageType, 
    const char *          messageName, 
    const MR::Message **  presetMessageOut ) const;

  /// \brief Find the SemanticLookupTable for the specified NodeType.
  SemanticLookupTable* findSemanticLookupTable(NodeType nodeType) const;

  /// \brief Get the maximum number of bones required by any animation set.
  NM_INLINE uint32_t getMaximumAnimSetBoneCount() const;

  /// \brief  True if any NodeDefs are physics or physics groupers
  bool isPhysical() const;

  /// \brief A network may not be able to combine trajectory and transform semantics if a node has the flag
  ///  NODE_FLAG_REQUIRES_SEPARATE_TRAJECTORY_AND_TRANSFORMS.  If the semantics can't be combined or trajectories or 
  ///  transforms were not queued then the semantics used to retrieve the network output differs
  NM_INLINE bool canCombineTrajectoryAndTransformSemantics() const;
  NM_INLINE void setCanCombineTrajectoryAndTransformSemantics(bool value);

  // \brief Array element for storing a set of NodeEventMessage structures.
  typedef struct NodeEventOnExitMessage
  {
    NodeEventMessage      m_nodeEventMessage;
    NodeID                m_nodeID;
  } NodeEventOnExitMessage;

  /// \brief Get the total number of messages that can be emitted when Nodes in this Network deactivate
  NM_INLINE uint32_t getNumNodeEventOnExitMessages() { return m_numNodeEventOnExitMessages; };
  /// \brief Get the array of messages that can be emitted when Nodes in this Network deactivate.  The array length
  ///  can be retrieved using getNumNodeEventOnExitMessages().
  NM_INLINE NodeEventOnExitMessage* getNodeEventOnExitMessages() { return m_nodeEventOnExitMessages; };

  void mapCopyTransformBuffers(NMP::DataBuffer* sourceBuffer, AnimSetIndex sourceAnimSetIndex, NMP::DataBuffer* targetBuffer, AnimSetIndex targetAnimSetIndex);
  void mapCopyVelocityBuffers(NMP::DataBuffer* sourceBuffer, AnimSetIndex sourceAnimSetIndex, NMP::DataBuffer* targetBuffer, AnimSetIndex targetAnimSetIndex);

protected:
  uint32_t                  m_numNodes;
  NodeDef**                 m_nodes;                    ///< Array of all nodes defined in this NetworkDef, listed in NodeID order.

  AnimSetIndex              m_numAnimSets;

  EmittedControlParamsInfo* m_emittedControlParamsInfo; ///< Array of NodeIDs that emit control param values.

  NodeIDsArray*             m_stateMachineNodeIDs;      ///< Array of all state machine node IDs

  NodeIDsArray*             m_messageEmitterNodeIDs;    ///< Array of all request emitter node IDs, listed in emission processing order.
                                                        ///<  Every node that has "NodeEmitsRequests" in its manifest file 
                                                        ///<  (is able to emit requests) is entered in this list.

  NodeIDsArray*             m_multiplyConnectedNodeIDs; ///< Array of all state machine node IDs

  NMP::IDMappedStringTable* m_stateMachineStateIDStringTable; ///< A string table for each state machine state to it's state id.

  NMP::IDMappedStringTable* m_nodeIDNamesTable;         ///< Table to allow the lookup of NodeIDs or names.

  NMP::OrderedStringTable*  m_messageIDNamesTable;      ///< Table for lookup of a RequestID via the request name.

  NMP::OrderedStringTable*  m_eventTrackIDNamesTable;   ///< User specified names of event tracks used in this network.
                                                        ///< The index of each name in the table is used as a runtime
                                                        ///< identifier for that event track. Each string in the table is unique.
                                                        ///< Note that separate event track assets may share the same name and
                                                        ///< hence will share IDs. This is deliberate to support features such as
                                                        ///< event track blending etc. 
                                                        ///< Event tracks themselves can be uniquely identified through their 
                                                        ///< ObjectID assigned in the Manager.
  NodeTagTable*             m_tagTable;
  SharedTaskFnTables*       m_taskQueuingFnTables;      ///< Table of shared task functions between node definitions
  SharedTaskFnTables*       m_outputCPTaskFnTables;     ///< Table of shared task functions between node definitions

  uint32_t                  m_numMessageDistributors;
  MessageDistributor**      m_messageDistributors;      ///< Array of message distributors so the messages can be
                                                        ///< efficiently sent to the relevant nodes.

  uint32_t                  m_numSemanticLookupTables;  ///< There is a semantic lookup table for each node type.
  SemanticLookupTable**     m_semanticLookupTables;     ///< They provide fast access into a NodeDefs packed AttribDataArray.

  uint32_t                  m_maxBonesInAnimSets;       ///< Stores the maximum number of bones required by all animation sets.
  uint32_t                  m_numNetworkInputControlParameters;   ///< Stores the number of input control parameters

  bool                      m_isPhysical;               ///< True if any nodes are NODE_FLAG_IS_PHYSICAL or NODE_FLAG_IS_PHYSICS_GROUPER
  bool                      m_canCombineTrajectoryTransformTasks; ///< The semantic ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER
                                                                  ///< can be used on this network when this flag is set.
                                                                  ///< This will be unset if any node in the network has the flag
                                                                  ///< NODE_FLAG_REQUIRES_SEPARATE_TRAJECTORY_AND_TRANSFORMS
  uint32_t                  m_numNodeEventOnExitMessages;         ///< The number of events in this network which can be triggered
                                                                  ///<  when a Node becomes inactive.
  NodeEventOnExitMessage*   m_nodeEventOnExitMessages;            ///< Array of messages which will be sent on given nodes becoming inactive

  uint32_t**                m_rigToUberrigMaps;
  uint32_t**                m_uberrigToRigMaps;
};

bool locateNetworkDef(uint32_t assetType, void* assetMemory);

//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::EmittedControlParamsInfo
/// \brief A class that stores information about Emitted control parameters.
/// \ingroup NetworkModule
//----------------------------------------------------------------------------------------------------------------------
class EmittedControlParamsInfo
{
public:
  //---------------------------------------------------------------------
  /// \struct EmittedControlParamData
  /// \brief Access emitted control parameter NodeIDs array.
  //---------------------------------------------------------------------
  struct EmittedControlParamData
  {
    NodeID             m_nodeID;    ///< NodeID to update.
    // Note: If we ever want user input control parameters to have more than 1 output pin
    //       we will have to include a PinIndex in this structure.
  };
public:
  static NMP::Memory::Format getMemoryRequirements(uint32_t numNodes);

  NM_INLINE NMP::Memory::Format getInstanceMemoryRequirements() const;

  static EmittedControlParamsInfo* init(
    NMP::Memory::Resource& memRes,
    uint32_t               numNodes);

  void locate();
  void dislocate();
  void relocate();

  NM_INLINE uint32_t getNumEmittedControlParams() const;

  NM_INLINE const EmittedControlParamData& getEmittedControlParamData(uint32_t index) const;

  NM_INLINE void setEmittedControlParamData(
    uint32_t index,
    NodeID   nodeID);

protected:
  /// Array of output control parameter Node IDs.
  /// The value of these nodes is updated at the end of every frame update and is available for use as input
  uint32_t                  m_numEmittedControlParamNodes;
  EmittedControlParamData*  m_emittedControlParamsData;
};

//----------------------------------------------------------------------------------------------------------------------

#include "mrNodeDef.inl"    // included here to avoid circular deps
#include "mrNetworkDef.inl"

//----------------------------------------------------------------------------------------------------------------------

bool locateNetworkDef(uint32_t assetType, void* assetMemory);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NETWORK_DEF
//----------------------------------------------------------------------------------------------------------------------
