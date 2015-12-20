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
#ifndef MR_NODE_DEF_H
#define MR_NODE_DEF_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"
#include "sharedDefines/mSharedDefines.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/mrNode.h"

//----------------------------------------------------------------------------------------------------------------------
/// \defgroup NodeDefModule NodeDef Classes, Functions and Constants
/// \ingroup CoreModule
///
/// The classes, functions, data types and structures forming the basis of a node definition.
//----------------------------------------------------------------------------------------------------------------------
namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Forward declarations
class SemanticLookupTable;
class SharedTaskFnTables;
class NetworkDef;

struct DataPinInfo;
struct PinAttribDataInfo;

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::NodeDef
/// \brief Class to represent the definition of a node, its connections, task capabilities
///        and provide storage for it's static attribute data.
/// \ingroup NetworkModule
///
/// All data in a NodeDef is constant and the MR::Network s initialised from it will reference data within it.
//----------------------------------------------------------------------------------------------------------------------
class NodeDef
{
public:
  //---------------------------------------------------------------------
  /// \enum  MR::NodeDef::NodeFlag
  /// \brief Capability flags for the node definitions.
  /// \ingroup NetworkModule
  //---------------------------------------------------------------------
  typedef enum NodeFlag
  {
    NODE_FLAG_IS_NONE                = 0,             ///<
    NODE_FLAG_IS_ANIM_SOURCE         = 1 << 0,        ///< This is an animation source Node.
    NODE_FLAG_IS_TRANSITION_ZERO     = 1 << 1,        ///< This is a transition Node. There can be many types of transition Node.
    NODE_FLAG_IS_CONTROL_PARAM       = 1 << 2,        ///< This is a source control parameter Node.
    NODE_FLAG_IS_TRANSITION          = 1 << 3,        ///< This is a transition Node. There can be many types of transition Node.
    NODE_FLAG_IS_STATE_MACHINE       = 1 << 4,        ///< System for state
    NODE_FLAG_IS_PHYSICAL            = 1 << 5,        ///< Manages some amount of physical simulation.
    NODE_FLAG_IS_PHYSICS_GROUPER     = 1 << 6,        ///< Groups together physics rig body parts with different physical simulation settings.
    NODE_FLAG_OUTPUT_REFERENCED      = 1 << 7,        ///< Potentially more than 1 node may be referencing this node as a child.
                                                      ///<  This situation occurs when using the pass down system in a network
                                                      ///<  exported from Connect.
    NODE_FLAG_IS_FILTER              = 1 << 8,        ///< Has only one active child and passes through any attribute data that
                                                      ///<  it does not directly process.
    NODE_FLAG_IS_BREAKOUT_TRANSITION = 1 << 9,        ///< A transition that can be activated from the destination node of an
                                                      ///<  already active transition. This kind of transition forces a transition
                                                      ///<  from a transition.
    NODE_FLAG_IS_TRANSITION_CAN_DEAD_BLEND = 1 << 10, ///< A transition that can dead blend its source to permit a breakout
                                                      ///< to an already active state.
    NODE_FLAG_IS_BEHAVIOURAL         = 1 << 11,       ///<
    NODE_FLAG_IS_OPERATOR_NODE       = 1 << 12,       ///<
    NODE_FLAG_REQUIRES_SEPARATE_TRAJECTORY_AND_TRANSFORMS = 1 << 13, ///< The node depends on trajectory and transforms being queued separately
    NODE_FLAG_IS_BLEND_OPT_NODE      = 1 << 14,       ///< This is a blend node that supports blend optimisation.
    NODE_FLAG_NO_PHYSICAL_CHILDREN   = 1 << 15,       ///< This node and all it's children are not physical nodes (i.e. none of them have
                                                      ///< NODE_FLAG_IS_PHYSICAL or NODE_FLAG_IS_PHYSICS_GROUPER set).
    NODE_FLAG_FORCE_INT = 0xFFFFFFFF
  } _NodeFlag;

  typedef NMP::Flags16 NodeFlags;

  /// \brief Structure to store the details of a message which should be emitted on a Node Event
  ///  e.g. on Node activation.
  typedef struct NodeEventMessage
  {
    bool      m_external;
    MessageID m_msgID;
  } NodeEventMessage;


public:
  //---------------------------------------------------------------------
  /// \name   NodeDef Initialisation
  /// \brief  Functions to determine the memory requirements for the nodeDef
  ///         and initialise the memory.
  //---------------------------------------------------------------------
  //@{

  /// \brief Full memory requirements of a NodeDef.
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t               numChildren,
    uint32_t               numInputCPConnections, ///< Number of input control param connections. Each one is a single connection to another node.
    uint32_t               numAttribDatas,
    uint32_t               numOnEnterMessages,
    MR::DataPinInfo*       nodeDataPinInfo);

  /// \brief Memory requirements of a NodeDef, excluding the memory requirements for the base NodeDef class itself.
  static NMP::Memory::Format getMemoryRequirementsExcludeBaseMem(
    uint32_t               numChildren,
    uint32_t               numInputCPConnections, ///< Number of input control param connections. Each one is a single connection to another node.
    uint32_t               numAttribDatas,
    uint32_t               numOnEnterMessages,
    MR::DataPinInfo*       nodeDataPinInfo = NULL);

  /// \brief Full initialisation of a NodeDef.
  static NodeDef* init(
    NMP::Memory::Resource& resource,
    uint32_t               numChildren,
    uint32_t               numInputCPConnections,  ///< Number of input control param connections. Each one is a single connection to another node.
    uint16_t               numAttribDatas,         ///< Number of AttribDataHandle slots available on this node.
    uint16_t               maxNumActiveChildNodes, ///< The maximum number of child nodes that can be active on this node at a time.
    uint8_t                numOutputCPPins,        ///< Number of output control param pins. Multiple other nodes can read from our outputs.
    uint32_t               numOnEnterMessages,
    MR::DataPinInfo*       nodeDataPinInfo); 

  /// \brief Initialisation of a NodeDef. But assumes that incrementing the base NodeDef class memory requirements
  /// has been done externally.
  static void initExcludeBaseMem(
    NodeDef*               target,
    NMP::Memory::Resource& resource,
    uint32_t               numChildren,
    uint32_t               numInputCPConnections,  ///< Number of input control param connections. Each one is a single connection to another node.
    uint16_t               numAttribDatas,
    uint16_t               maxNumActiveChildNodes, ///< The maximum number of child nodes that can be active on this node at a time.
    uint8_t                numOutputCPPins,        ///< Number of output control param pins. Multiple other nodes can read from our outputs.
    uint32_t               numOnEnterMessages,
    MR::DataPinInfo*       nodeDataPinInfo); 

  /// \brief Initialisation of a NodeDef. But assumes that incrementing the base NodeDef class memory requirements
  /// has been done externally.
  static void relocateExcludeBaseMem(
    NodeDef*               target,
    NMP::Memory::Resource& resource,
    uint32_t               numChildren,
    uint32_t               numInputCPConnections, ///< Number of input control param connections. Each one is a single connection to another node.
    uint32_t               numAttribDatas,
    MR::DataPinInfo*       nodeDataPinInfo);

  void locate(NetworkDef* owningNetworkDef);
  void dislocate();
  //@}

  //---------------------------------------------------------------------
  /// \name   NodeDef management
  /// \brief  Functions to get and set information about the nodeDef
  //---------------------------------------------------------------------
  //@{
  NM_INLINE const char* getName() const;

  NM_INLINE NodeType getNodeTypeID() const { return m_nodeTypeID; }
  NM_INLINE void setNodeTypeID(NodeType nodeTypeID) { m_nodeTypeID = nodeTypeID; }

  NM_INLINE NodeFlags getNodeFlags() const { return m_nodeFlags; }
  NM_INLINE void setNodeFlags(NodeFlags flags) { m_nodeFlags.set(flags); }

  NM_INLINE uint16_t getPassThroughChildIndex() const { return m_passThroughChildIndex; }
  NM_INLINE void setPassThroughChildIndex(uint16_t index) { m_passThroughChildIndex = index; }

  NM_INLINE const NetworkDef* getOwningNetworkDef() const { return m_owningNetworkDef; }
  NM_INLINE NetworkDef* getOwningNetworkDef() { return m_owningNetworkDef; }
  NM_INLINE void setOwningNetworkDef(NetworkDef* owningNetworkDef) { m_owningNetworkDef = owningNetworkDef; }

  NM_INLINE NodeID getNodeID() const { return m_nodeID; }
  NM_INLINE void setNodeID(NodeID nodeID) { m_nodeID = nodeID; }

  NM_INLINE NodeID getParentNodeID() const { return m_parentNodeID; }
  NM_INLINE void setParentNodeID(NodeID nodeID) { m_parentNodeID = nodeID; }
  NM_INLINE const NodeDef* getParentNodeDef() const;
  NM_INLINE NodeDef* getParentNodeDef();

  NM_INLINE uint16_t getNumChildNodes() const { return m_numChildNodeIDs; }
  NM_INLINE uint16_t getMaxNumActiveChildNodes() const { return m_maxNumActiveChildNodes; }
  NM_INLINE NodeID getChildNodeID(uint32_t childNodeIndex) const;
  NM_INLINE bool hasChildNodeID(NodeID nodeID) const;
  NM_INLINE void setChildNodeID(uint32_t childNodeIndex, NodeID nodeID);
  NM_INLINE const NodeDef* getChildNodeDef(uint32_t childNodeIndex) const;
  NM_INLINE NodeDef* getChildNodeDef(uint32_t childNodeIndex);

  NM_INLINE uint8_t getNumInputCPConnections() const { return m_numInputCPConnections; }
  NM_INLINE NodeID getInputCPConnectionSourceNodeID(uint32_t inputPinIndex) const;
  NM_INLINE PinIndex getInputCPConnectionSourcePinIndex(uint32_t inputPinIndex) const;
  NM_INLINE const NodeDef* getInputCPConnectionSourceNodeDef(uint32_t inputPinIndex) const;
  NM_INLINE NodeDef* getInputCPConnectionSourceNodeDef(uint32_t inputPinIndex);
  NM_INLINE void setInputCPConnection(
    PinIndex inputPinIndex,         ///< My input pin index.
    NodeID   sourceNodeID,          ///< The node we are connected to.
    PinIndex sourceOutputPinIndex); ///< The output pin we are connected to on the source node
  NM_INLINE const CPConnection* getInputCPConnection(PinIndex inputPinIndex) const; ///< My input pin index.

  NM_INLINE uint8_t getNumOutputCPPins() const { return m_numOutputCPPins; }

  NM_INLINE uint16_t getNumAttribDataHandles() const { return m_numAttribDataHandles; }
  NM_INLINE const AttribDataHandle* getAttribDataHandles() const { return m_nodeAttribDataHandles; }
  NM_INLINE AttribDataHandle* getAttribDataHandles() { return m_nodeAttribDataHandles; }

  NM_INLINE uint16_t getNumPinAttribDataHandles() const { return m_numPinAttribDataHandles; }
  NM_INLINE const AttribDataHandle* getPinAttribDataHandles() const { return m_nodePinAttribDataHandles; }
  NM_INLINE AttribDataHandle* getPinAttribDataHandles() { return m_nodePinAttribDataHandles; }

  NM_INLINE uint16_t getNumReflexiveCPPins() const { return m_numReflexiveCPPins; }
  NM_INLINE void setNumReflexiveCPPins(uint16_t count) { m_numReflexiveCPPins = count; }
  NM_INLINE uint32_t getPinAttribIndex(uint32_t index, AnimSetIndex animSetIndex = 0) const;
  NM_INLINE PinAttribDataInfo* getPinAttribDataInfo(uint32_t index) const;
  NM_INLINE PinIndex newPinAttribDataInfo(bool perAnimSet, AttribDataSemantic semantic);
  NM_INLINE void addPinAttribDataAnimSetEntry(uint32_t NMP_USED_FOR_ASSERTS(pin), MR::AnimSetIndex NMP_USED_FOR_ASSERTS(animSetIndex));

  NM_INLINE AttribDataHandle*  getPinAttribDataHandle(uint32_t index, AnimSetIndex animSetIndex) const;

  /// AttribData must exist.
  NM_INLINE AttribDataHandle* getAttribDataHandle(
    AttribDataSemantic semantic,
    AnimSetIndex       animSetIndex) const;
  NM_INLINE AttribDataHandle* getAttribDataHandle(
    AttribDataSemantic semantic) const;

  /// AttribData must exist.
  NM_INLINE AttribData* getAttribData(
    AttribDataSemantic semantic,
    AnimSetIndex       animSetIndex) const;
  NM_INLINE AttribData* getAttribData(
    AttribDataSemantic semantic) const;

  /// AttribData must exist.
  template <class T>
  NM_INLINE T* getPinAttribData(
    uint32_t     index,
    AnimSetIndex animSetIndex) const;
  
  /// More secure templated versions that validate types and do a cast to the template type internally.
  /// AttribData must exist.
  template <class T> 
  NM_INLINE T* getAttribData(
    AttribDataSemantic semantic,
    AnimSetIndex       animSetIndex) const;
  template <class T> 
  NM_INLINE T* getAttribData(
    AttribDataSemantic semantic) const;

  /// AttribData can optionally not exist.
  NM_INLINE AttribDataHandle* getOptionalAttribDataHandle(
    AttribDataSemantic semantic,
    AnimSetIndex       animSetIndex) const;
  NM_INLINE AttribDataHandle* getOptionalAttribDataHandle(
    AttribDataSemantic semantic) const;

  //@}

  //---------------------------------------------------------------------
  /// \name   Task function and queuing management
  /// \brief  Functions to manage task queuing and node state
  //---------------------------------------------------------------------
  //@{
  NM_INLINE uint16_t getTaskQueuingFnsID() const { return m_taskQueuingFnsID; }
  NM_INLINE const QueueAttrTaskFn* getTaskQueueingFns() const { return m_taskQueuingFns; }
  NM_INLINE QueueAttrTaskFn getTaskQueueingFn(AttribDataSemantic nodeAttribSemantic);

  NM_INLINE uint16_t getOutputCPTasksID() const { return m_outputCPTasksID; }
  NM_INLINE const OutputCPTask* getOutputCPTasks() const { return m_outputCPTasks; }
  NM_INLINE OutputCPTask getOutputCPTask(PinIndex outputCPPinIndex);

  NM_INLINE FindGeneratingNodeForSemanticFn getFindGeneratingNodeForSemanticFn() const;

  /// Updates all data for this node that can only be computed immediately rather than as part of the task queue.
  /// Includes determining which children are active.
  NM_INLINE NodeID updateConnections(Network* net);

  /// Initialise node instance data at network instantiation (where any initialisation is necessary).
  NM_INLINE void initNodeInstance(Network* net);

  /// Delete node instances attribute data where necessary and recurse to created children.
  void deleteNodeInstance(Network* net);

  void registerEmptyTaskQueuingFns(
    SharedTaskFnTables* taskQueuingFnTables);

  void registerTaskQueuingFns(
    SharedTaskFnTables*    taskQueuingFnTables,
    const QueueAttrTaskFn* taskQueuingFns);

  void registerEmptyOutputCPTasks(
    SharedTaskFnTables* outputCPTaskFnTables);

  void registerOutputCPTasks(
    SharedTaskFnTables* outputCPTaskFnTables,
    const OutputCPTask* outputCPTaskFns);

  void setTaskQueuingFnId(
    QueueAttrTaskFn*  taskQueuingFns,
    uint32_t          semantic,
    const char*       fnName,
    QueueAttrTaskFn   queuingFn,
    NMP::BasicLogger* logger = 0);

  void setOutputCPTaskFnId(
    OutputCPTask*     outputCPTaskFns,
    PinIndex          outputCPPinIndex,
    uint32_t          semantic,
    const char*       fnName,
    OutputCPTask      outputCPTask,
    NMP::BasicLogger* logger = 0);

  void setDeleteNodeInstanceId(const char* fnName, DeleteNodeInstance deleteFn, NMP::BasicLogger* logger = 0);
  void setUpdateConnectionsFnId(const char* fnName, UpdateNodeConnections updateNodeConnectionsFn, NMP::BasicLogger* logger = 0);
  void setFindGeneratingNodeForSemanticFnId(const char* fnName, FindGeneratingNodeForSemanticFn generatesAttribSemanticTypeFn, NMP::BasicLogger* logger = 0);
  void setInitNodeInstanceFnId(const char* fnName, InitNodeInstance initNodeFn, NMP::BasicLogger* logger = 0);
  void setMessageHandlerFnId(const char* fnName, MessageHandlerFn messageHandlerFn, NMP::BasicLogger* logger = 0);

  /// Enqueue the update of an attribute belonging to a tree node.
  /// The task that updates this attribute may also update other attributes belonging to this node.
  NM_INLINE Task* putAttributeOnQueue(
    TaskQueue*     queue,
    Network*       net,
    TaskParameter* dependentParam ///< IN: The param from the above task that is dependent on this attribute.
    );

  /// Perform the immediate update of an attribute belonging to a control parameter node.
  /// The computed attribute is inserted in to the network and is also returned by this function.
  /// The task that updates this attribute may also update other attributes belonging to this node.
  /// These will also be inserted on to the network.
  NM_INLINE AttribData* updateOutputCPAttribute(
    Network* net,
    PinIndex outputCPPinIndex);
  //@}

  /// Perform the immediate update of an attribute belonging to a emitted request.
  NM_INLINE AttribData* updateEmittedMessages(
    Network* net,
    PinIndex emitMessagePinIndex);

  /// \brief Passes a message to the node's message handler if it has one.
  /// \return False if the node doesn't have a message handler or the handler choose to ignore the message
  NM_INLINE bool handleMessage(
    Network* net,
    const Message& message);

  /// \brief Access the the semantic lookup table for this NodeType.
  SemanticLookupTable* getSemanticLookupTable() const { return m_semanticLookupTable; }
  void setSemanticLookupTable(SemanticLookupTable* semanticLookupTable) {  m_semanticLookupTable = semanticLookupTable; }

  /// \brief Get the number of messages to be emitted when this node becomes active
  uint8_t getNumOnEnterMessages() const { return m_numOnEnterMessages; }
  /// \brief Get the array of OnEnter messages
  const NodeEventMessage* getOnEnterMessages() const  { return m_onEnterMessages; }
        NodeEventMessage* getOnEnterMessages()        { return m_onEnterMessages; }
  /// \brief Set the message to be emitted when this node is activated.  For use in the asset compiler.
  void setOnEnterMessage(uint32_t eventIndex, MessageID messageID, bool external);

protected:
  NodeType                        m_nodeTypeID;                   ///< The type of this node definition
  NodeFlags                       m_nodeFlags;                    ///< Specifies if the node belongs to any special groups
                                                                  ///<  (e.g. anim source nodes, control parameters).
  NodeID                          m_nodeID;                       ///< This node's network ID
  NodeID                          m_parentNodeID;                 ///<  The downstream parent node ID. A node can only ever have one parent
                                                                  ///<  (controlling) node. If the output of this node is multiply connected
                                                                  ///<  to inputs of other nodes via a pass down pin, then the downstream parent
                                                                  ///<  ID will be connected to the parent of the pass down state / blend tree.
                                                                  ///<  i.e. time updates will bypass the multiple connections and will come
                                                                  ///<  directly from the downstream parent node.
  uint16_t                        m_numChildNodeIDs;              ///< The number of connected child nodes
  uint16_t                        m_maxNumActiveChildNodes;       ///< The maximum number of child nodes that can be active on this node at a time.
  uint8_t                         m_numInputCPConnections;        ///< The number of input control param connections.
  uint8_t                         m_numOutputCPPins;              ///< The number of output control param pins.
                                                                  ///<  An AttribData can be stored on each of a nodes output pins at runtime.
                                                                  ///<  This can be accessed by any number of connected nodes.

  uint16_t                        m_numReflexiveCPPins;           ///< The number of reflexive CP pins in this node def.
  uint16_t                        m_numPinAttribDataHandles;      ///< The number of reflexive CP pin attrib data entries for this node def.
  uint16_t                        m_lastPinAttribDataOffset;
  uint16_t                        m_lastPinAttribDataIndex;
  uint16_t                        m_numAttribDataHandles;         ///< The number of attrib data entries for this node def.
  uint16_t                        m_passThroughChildIndex;        ///< In a Filter node, which active child index to pass through.

  NetworkDef*                     m_owningNetworkDef;             ///< The network def that this node belongs to.
  NodeID*                         m_childNodeIDs;                 ///< Connected child nodes.
  CPConnection*                   m_inputCPConnections;           ///< Input control param or operator node connections.
  PinAttribDataInfo*              m_nodePinAttribDataInfo;        ///< Array of all this NodeDefs reflexive CP pin AttribData info structures.
  AttribDataHandle*               m_nodePinAttribDataHandles;     ///< Array of all this NodeDefs reflexive CP pin AttribData.
  AttribDataHandle*               m_nodeAttribDataHandles;        ///< Array of all this NodeDefs static AttribData.
                                                                  ///<  Fast access is provided through m_semLookupTable.
                                                                  ///<  Note that this array contains data for all AnimSets 
                                                                  ///<  i.e. All AttribData AnimSet 0, followed by All AttribData AnimSet 1 etc

  /// Queuing function table index to retrieve the two shared task function pointer arrays from the network def.
  uint16_t                        m_taskQueuingFnsID;
  uint16_t                        m_outputCPTasksID;

  /// A list of pointers to task queuing functions. Used by tree nodes only.
  QueueAttrTaskFn*                m_taskQueuingFns;

  /// A list of function pointers to output control param tasks. One entry per output pin.
  OutputCPTask*                   m_outputCPTasks;

  /// Deletes all attrib data for an instance of this nodeDef existing in a Network instances AttribDatas array.
  /// Recurses deletion to any children that this node knows may have attrib data existing in the Network.
  DeleteNodeInstance              m_deleteNodeInstanceFn;

  /// Updates all data for this node that can only be computed immediately rather than as part of the task queue.
  /// This includes determining those child nodes that are active and hence require update this frame.
  /// Also includes update of connected control parameters.
  /// This function is called as the first operation on an active node in the network. This is done automatically via
  /// the queueTasksFor function on the Network.
  UpdateNodeConnections           m_updateNodeConnectionsFn;

  /// Indicates if this node generates attribute semantic data of the specified type.
  /// For use by node queuing functions to determine the capabilities of their connected nodes.
  FindGeneratingNodeForSemanticFn m_findGeneratingNodeForSemanticFn;

  /// For an per node initialisation that is required at network instancing.
  InitNodeInstance                m_initNodeInstanceFn;

  /// Handles all messages for this node.  It is valid for this function to be NULL at runtime.
  MessageHandlerFn                m_messageHandlerFn;

  ///< Reference to this NodeTypes semantic lookup table. Provides fast access into the NodeDefs packed AttribDataArray.
  SemanticLookupTable*            m_semanticLookupTable;

  uint8_t                         m_numOnEnterMessages;
  NodeEventMessage*               m_onEnterMessages;
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NodeDef::setOnEnterMessage(uint32_t eventIndex, MessageID messageID, bool external)
{
  NMP_ASSERT(eventIndex < m_numOnEnterMessages);
  m_onEnterMessages[eventIndex].m_msgID = messageID;
  m_onEnterMessages[eventIndex].m_external = external;
}

//----------------------------------------------------------------------------------------------------------------------
/// \struct MR::PinAttribDataInfo
/// \brief Specifies the nature of the attribute data (possibly per animset) attached to a Data Pin.
///        
/// \ingroup NodeDefModule
///
//----------------------------------------------------------------------------------------------------------------------
struct PinAttribDataInfo
{
  /// \brief Initialises the info structure to default values, for a given pin index. Used internally.
  NM_INLINE void init(PinIndex pin)
  { 
    m_offset = 0;
    m_semantic = ATTRIB_SEMANTIC_NA;
    m_pin = pin;
    m_perAnimSet = false;
  };

  /// \brief Initialise the info structure for the data pin. Used internally.
  NM_INLINE void set(uint32_t offset, bool perAnimSet, AttribDataSemantic semantic)
  { 
    m_offset = offset;
    m_semantic = semantic;
    m_perAnimSet = perAnimSet;
  };

  NM_INLINE void endianSwap()
  {
    NMP::endianSwap(m_offset);
    NMP::endianSwap(m_semantic);
    NMP::endianSwap(m_pin);
    NMP::endianSwap(m_perAnimSet);
  };

  uint32_t           m_offset;      ///< Simple a fast lookup offset of this data
  AttribDataSemantic m_semantic;    ///< Semantic type of this data
  PinIndex           m_pin;         ///< Pin index
  bool               m_perAnimSet;  ///< Does this data pin require per animset data?
};

//----------------------------------------------------------------------------------------------------------------------
/// \struct MR::DataPinInfo
/// \brief The Node's resource requirements for its control parameters that are data Pins with attached attrib data.
///        
/// \ingroup NodeDefModule
///
//----------------------------------------------------------------------------------------------------------------------
struct DataPinInfo
{
public:
  /// \brief Initialises the structure to default values. Used internally.
  void init()
  { 
    m_numPinAttribData = 0;
    m_pinAtttribDataMemory.set(0, MR_ATTRIB_DATA_ALIGNMENT);
    m_reflexiveCPCount = 0;
  };

  uint32_t m_numPinAttribData;                ///< Total number of attribDatas required by this Node's data pins.
  NMP::Memory::Format m_pinAtttribDataMemory; ///< Total amount of formated memory required by this Node's data pin.
  uint16_t m_reflexiveCPCount;                ///< Number of the controll parameters that are data pins with attrib
                                              ///< data attached (in which case they are attached to hidden ouputs
                                              ///< of the same node, hence 'reflexive').
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::NodeTypeSemanticLookupTable
/// \brief Provides a fast and simple mapping from a Semantic Index to an index
///        in to a specific NodeTypes AttribData storage array.
/// \ingroup NodeDefModule
///
/// Each NodeType has a limited set of AttribSemanticTypes that it stores (n semantic types).
/// Each instance of a NodeDef type stores an array of semantic AttribDatas of this size.
/// This structure provides fast constant time lookup in to this array via a AttribSemanticType.
/// There is one of these per node type.
//----------------------------------------------------------------------------------------------------------------------
class SemanticLookupTable
{
public:
  /// \brief Get the memory requirements for an array of semantic lookup tables
  static NMP::Memory::Format getSemanticLookupTablesMemoryRequirements(uint32_t numSemanticLookupTables);

  /// \brief Initialise the table array of pointers
  static SemanticLookupTable** initSemanticLookupTables(NMP::Memory::Resource& resource, uint32_t numSemanticLookupTables);

  static const uint8_t INVALID_LOOKUP_INDEX = 0xFF;

  /// \brief Get the memory requirements for a semantic lookup table
  static NMP::Memory::Format getMemoryRequirements();

  /// \brief Create an unpopulated table in the provided resource,
  static SemanticLookupTable* init(NMP::Memory::Resource& resource, NodeType nodeType);

  /// \brief Create a table in the provided resource, copying from a source table.
  static SemanticLookupTable* initFrom(NMP::Memory::Resource& resource, SemanticLookupTable* const sourceTable);

  void initFrom(const SemanticLookupTable* const initFrom);

  void locate();
  void dislocate();

  /// \brief Set the lookup index for a semantic.
  void setLookupIndex(AttribDataSemantic semantic, uint8_t lookupIndex); 

  /// \brief Set the lookup index for a semantic using the internal count of the number of lookups.
  /// The count of the number of lookups is incremented.
  /// returns the index lookup stored by this semantic.
  uint8_t addLookupIndex(AttribDataSemantic semantic);

  /// \brief Get the lookup index in to a nodes AttribData array, for a semantic and animSetIndex.
  ///        There must be an entry for it.
  NM_FORCEINLINE uint32_t getLookupIndex(
    AttribDataSemantic semantic,
    AnimSetIndex       animSetIndex) const;

  /// \brief Get the lookup index in to a nodes AttribData array, for a semantic and animSetIndex.
  ///        There can optionally not be an entry for it, in this case it returns INVALID_LOOKUP_INDEX.
  NM_FORCEINLINE uint32_t getOptionalLookupIndex(
    AttribDataSemantic semantic,
    AnimSetIndex       animSetIndex) const;

  /// \brief Determines if there is a lookup index in the nodes AttridData array, for a semantic and animSetIndex
  NM_FORCEINLINE bool hasLookupIndex(AttribDataSemantic semantic) const;

  /// \brief Get the NodeType that this is a lookup table for.
  NodeType getNodeType() const { return m_nodeType; }

  /// \brief Set the NodeType that this is a lookup table for.
  void setNodeType(NodeType nodeType) { m_nodeType = nodeType; }

  /// \brief Return how many AttribData slots that there are for each anim set on this node type.
  uint8_t getNumAttribsPerAnimSet() const { return m_numAttribsPerAnimSet; }

  SemanticLookupTable(NodeType nodeType) :
    m_nodeType(nodeType)
  {
  }

private:
  NodeType m_nodeType;             ///< The NodeType that this table is providing a mapping for.
  uint8_t  m_numAttribsPerAnimSet; ///< Num Def AttribData slots per anim set on this NodeType.
  uint16_t m_numSemantics;         ///< This is the number of AttribSemanticTypes registered with the Manager when the table was built in the Asset Compiler.
                                   ///<  It is also the size of the m_semanticLookup array.
  uint8_t* m_semanticLookup;       ///< An entry per semantic. These semantics that do not exist on this NodeType will have an entry of INVALID_LOOKUP_INDEX.
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
class SharedTaskFnTables
{
public:
  typedef void (*SharedTaskFn)();

public:
  //---------------------------------------------------------------------
  /// \name   Task function table registering
  /// \brief  Functions required to initialise and register shared task function tables.
  ///         These functions are used during asset compilation to build the tables.
  //---------------------------------------------------------------------
  //@{

  /// \brief Get the memory requirements for a tables container without any actual table data.
  static NMP::Memory::Format getMemoryRequirementsExcludeData(
    uint32_t numTaskFnTables);

  /// \brief Initialise the memory for a tables container without any actual table data.
  /// The number of task function tables is reset to zero after initialisation so that
  /// task function tables can be registered.
  static SharedTaskFnTables* initExcludeData(
    uint32_t numTaskFnTables);

  /// \brief Creates a shared task function table
  static SharedTaskFn* createSharedTaskFnTable();

  /// \brief Compares the input task function table with the currently registered set of
  /// task function tables and returns the index of the compatible table or -1 otherwise.
  uint32_t findCompatibleTaskFnTable(const SharedTaskFn* taskFnTbl) const;

  /// \brief Finds the index of the empty task function table
  uint32_t findEmptyTaskFnTable() const;

  /// \brief Compares the input task function table with the currently registered set of
  /// task function tables. If a similar table has already been registered then the index
  /// of the registered table is returned. If the table has not been registered then the
  /// memory for a new task function table is allocated, added to the list and the input
  /// table values are copied into it.
  uint32_t registerTaskFnTable(const SharedTaskFn* taskFnTbl);

  /// \brief Registers an empty task function table or returns the index of the existing one.
  uint32_t registerEmptyTaskFnTable();

  /// \brief Frees the memory corresponding to any registered task function tables.
  void freeRegisteredTaskFnTables();

  /// \brief Frees the last registered task function table
  void freeLastRegisteredTaskFnTable();
  //@}

  //---------------------------------------------------------------------
  /// \name   Tables management
  /// \brief  Functions required to initialise a set of shared task function tables
  ///         in a single continuous memory block.
  //---------------------------------------------------------------------
  //@{

  /// \brief Get the memory requirements for a tables container complete with table data
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numTaskFnTables);

  /// \brief Initialise the memory for a tables container complete with table data
  static SharedTaskFnTables* init(
    NMP::Memory::Resource& memRes,
    const SharedTaskFnTables* taskFnTables);

  void locateTaskQueuingFnTables();
  void dislocateTaskQueuingFnTables();

  void locateOutputCPTaskFnTables();
  void dislocateOutputCPTaskFnTables();

  NM_INLINE const SharedTaskFn* getTaskFnTable(uint32_t indx) const;
  NM_INLINE SharedTaskFn* getTaskFnTable(uint32_t indx);
  NM_INLINE void setTaskFnTable(uint32_t indx, SharedTaskFn* taskFnTbl);

  NM_INLINE uint32_t getNumTaskFnTables() const;
  //@}

protected:
  uint32_t            m_numTaskFnTables;      ///< The number of shared task function tables
  uint32_t            m_numEntriesPerTable;   ///< Should be the same as the number of semantics
  SharedTaskFn**      m_taskFnTables;         ///< A vector of shared task function tables
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_DEF_H
//----------------------------------------------------------------------------------------------------------------------
