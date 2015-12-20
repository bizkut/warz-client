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
#ifndef MR_NODE_DEF_BUILDER_H
#define MR_NODE_DEF_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"
#include "morpheme/mrNetworkDef.h"
#include "export/mcExport.h"
#include "AssetProcessor.h"
#include "assetProcessor/MessageBuilder.h"
#include "nmtl/hashmap_dense_dynamic.h"
#include "nmtl/vector.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::NodeDefDependency
// MORPH-21327: documentation required.
//----------------------------------------------------------------------------------------------------------------------
class NodeDefDependency
{
public:
  explicit NodeDefDependency() 
    : m_nodeID(MR::INVALID_NODE_ID)
    , m_nodeTypeID(MR::INVALID_NODE_TYPE_ID)
    , m_isControlParamType(false)
    , m_animationID(0xffffffff)
 {}

  void addChildConnection(NodeDefDependency* nodeDefDep);
  void removeChildConnection(NodeDefDependency* nodeDefDep);
  bool isAnimSetValid(MR::AnimSetIndex animSetIndex) const;

public:
  MR::NodeID                        m_nodeID;
  MR::NodeType                      m_nodeTypeID;
  bool                              m_isControlParamType;
  uint32_t                          m_animationID;        ///< The AnimationId in the network.xml. 
                                                          ///  The AnimationEntry index in the Library.xml. '-1' if not an anim node.
  std::vector<MR::AnimSetIndex>     m_animSets;           ///< The Anim Sets that this node is valid for - that is used by this node.
  std::vector<NodeDefDependency*>   m_parentNodeIDs;
  std::vector<NodeDefDependency*>   m_childNodeIDs;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::NodeDefCompilationInfo
/// \brief Utility container for holding information about node definitions
//----------------------------------------------------------------------------------------------------------------------

struct InputPinInfo
{
  char*                  m_nodeIDLabel;
  MR::AttribDataSemantic m_semantic;
  MR::PinIndex           m_pinIndex;
  uint32_t               m_numAnimSets;
  bool                   m_readPerAnimSetData;
  bool                   m_optional;

  InputPinInfo():m_nodeIDLabel(NULL) { };
  NM_INLINE InputPinInfo(MR::AttribDataSemantic semantic,
    MR::PinIndex pinIndex, uint32_t numAnimSets, bool readPerAnimSetData, bool optional):
  m_semantic(semantic), m_pinIndex(pinIndex), m_numAnimSets(numAnimSets), m_readPerAnimSetData(readPerAnimSetData), m_optional(optional) { };
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::AttribDataSemanticNameTable
/// \brief Utility table that contains a map of the strings exported by connect that are used by declareDataPin()
//----------------------------------------------------------------------------------------------------------------------

struct AttribDataSemanticNameTable
{
  MR::AttribDataSemantic m_semantic;
  const char *           m_typeName;
};

extern const AttribDataSemanticNameTable BUILT_IN_ATTRIB_DATA_SEMANTIC_NAMES[];
extern const uint32_t NUM_BUILT_IN_ATTRIB_DATA_SEMANTICS;

class NodeDefCompilationInfo
{
public:
  NMP::Memory::Format               m_nodeDefMemReqs; ///< Memory requirements for the node definition data
  MR::NodeDef*                      m_nodeDef;        ///< Pointer to the node definition data
  void*                             m_userData;       ///< Pointer to data maintained by the user.

  MR::DataPinInfo                   m_dataPinInfo;    ///< Info structure for attrib data attached to control parameters pins.

  typedef nmtl::vector<InputPinInfo> InputPinInfoArray;
  InputPinInfoArray m_inputPinInfo;
 
  NodeDefCompilationInfo();
  NM_INLINE void initPinInfo();
  NM_INLINE void clearPinInfo();
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::MessageDistributorInfo
/// \brief Utility container for holding information about which nodes are interested in a particular message.
//----------------------------------------------------------------------------------------------------------------------
struct MessageInterest
{
  MR::MessageID                 m_messageID;
  MR::MessageType               m_messageType;
  NMP::Memory::Format           m_messageDistributorMemReqs;
  MR::MessageDistributor*       m_messageDistributor;

  nmtl::pod_vector<MR::NodeID>  m_nodeIDs;

  NM_INLINE MessageInterest();
};

//----------------------------------------------------------------------------------------------------------------------
// MORPH-21327: documentation required.
//----------------------------------------------------------------------------------------------------------------------
class NetworkDefCompilationInfo
{
public:
  /// \brief Initialise the external memory pointers for the networkDef compilation info.
  NetworkDefCompilationInfo(
    const ME::NetworkDefExport*       netDefExport,
    const ME::AnimationLibraryExport* animLibraryExport,
    MR::SharedTaskFnTables*           taskQueuingFnTables,
    MR::SharedTaskFnTables*           outputCPTaskFnTables);

  ~NetworkDefCompilationInfo();

  // Information
  uint32_t getNumNodes() const { return (uint32_t)m_nodeDefCompilationInfo.size(); }
  uint32_t getNumAnimSets() const { return m_numAnimSets; }
  uint32_t getNumMessages() const { return (uint32_t)m_messageInterest.size(); }

  // NodeDef compilation information
  NM_INLINE void setNodeDef(
    uint32_t nodeID,
    NMP::Memory::Format& nodeDefMemReqs,
    MR::NodeDef* nodeDef);

  NM_INLINE void releaseNodeDef(uint32_t nodeID);

  NM_INLINE MR::DataPinInfo* getNodeDataPinInfo(uint32_t nodeID);
  NM_INLINE MR::NodeDef* getNodeDef(uint32_t nodeID);
  NM_INLINE const MR::NodeDef* getNodeDef(uint32_t nodeID) const;
  NM_INLINE NMP::Memory::Format getNodeDefMemReqs(uint32_t nodeID) const;
  NM_INLINE bool isNodeDefCompiled(uint32_t nodeID) const;

  NM_INLINE void setNodeDataPinInfo(uint32_t nodeID,
    const char* nodeIDLabel,
    MR::AttribDataSemantic semantic,
    MR::PinIndex pinIndex,
    uint32_t numAnimSets,
    bool readPerAnimSetData,
    bool optional);
  NM_INLINE MR::AttribDataSemantic getNodeDataPinSemantic(uint32_t nodeID, uint32_t pinIndex);

  NM_INLINE void getNodeDataPinInfoMemory(uint32_t nodeID, const ME::NodeExport* nodeDefExport);
  NM_INLINE void processNodeDataPinInfo(
    uint32_t               nodeID,
    NMP::Memory::Resource& resource,
    MR::NodeDef*           nodeDef,
    const ME::NodeExport*  nodeDefExport);

  // Attached user data 
  NM_INLINE void attachUserData(uint32_t nodeID, void* userData);
  NM_INLINE void* getUserData(uint32_t nodeID);
  NM_INLINE void* detachUserData(uint32_t nodeID);

  // NodeDef dependency graph
  NM_INLINE NodeDefDependency& getNodeDefDependency(uint32_t nodeID);
  NM_INLINE const NodeDefDependency& getNodeDefDependency(uint32_t nodeID) const;

  void addSubRig(const char* subRigName) {m_subRigs.push_back(subRigName);}
  const char* getSubRig(uint32_t i) { return m_subRigs[i].c_str();}

  // Shared task function tables
  MR::SharedTaskFnTables* getTaskQueuingFnTables() { return m_taskQueuingFnTables; }
  MR::SharedTaskFnTables* getOutputCPTaskFnTables() { return m_outputCPTaskFnTables; }

  // Message distributor functions
  void registerMessageInterest(MR::MessageID messageID, MR::NodeID nodeID);

  NM_INLINE void getInterestedNodesForMessage(
    MR::MessageID messageID,
    nmtl::pod_vector<MR::NodeID>* interestedNodes) const;

  NM_INLINE void setMessageDistributor(
    MR::MessageID messageID,
    NMP::Memory::Format& messageDistributorMemReqs,
    MR::MessageDistributor* messageDistributor);

  NM_INLINE MR::MessageDistributor* getMessageDistributor(MR::MessageID messageID);
  NM_INLINE const MR::MessageDistributor* getMessageDistributor(MR::MessageID messageID) const;
  NM_INLINE NMP::Memory::Format getMessageDistributorMemReqs(MR::MessageID messageID) const;
  NM_INLINE bool isMessageDistributorCompiled(MR::MessageID messageID) const;

  /// \brief Find the SemanticLookupTable for the specified NodeType.
  MR::SemanticLookupTable* findSemanticLookupTable(MR::NodeType nodeType);

  /// \brief Store the tables that have been generated so that node builders can access them when needed.
  void setSemanticLookupTables(
    uint32_t                  semanticLookupTablesCount,
    MR::SemanticLookupTable** semanticLookupTables,
    NMP::Memory::Format       semanticLookupTablesMemReqs);

  /// \brief Detaches the semantic lookup tables without freeing the memory. This is used
  /// for save / restore behaviour.
  void detachSemanticLookupTables(
    uint32_t&                  semanticLookupTablesCount,
    MR::SemanticLookupTable**& semanticLookupTables,
    NMP::Memory::Format&       semanticLookupTablesMemReqs);

  /// \brief Free allocated arrays of lookup tables
  void tidySemanicLookupTables();

  /// \brief Free allocated message interests
  void tidyMessageInterest();

  uint32_t getSemanticLookupTablesCount() const { return m_semanticLookupTablesCount; }
  MR::SemanticLookupTable** getSemanticLookupTables() { return m_semanticLookupTables; }
  NMP::Memory::Format getSemanticLookupTablesMemReqs() { return m_semanticLookupTablesMemReqs; }

protected:
  // vector typedefs
  typedef nmtl::vector<NodeDefCompilationInfo>  NodeDefCompilationInfoArray;
  typedef nmtl::vector<NodeDefDependency>       NodeDefDependencyArray;
  typedef nmtl::vector<MessageInterest>         MessageInterestArray;

  // Information
  uint32_t                          m_numAnimSets;
  
  /// Individually compiled node definitions
  NodeDefCompilationInfoArray       m_nodeDefCompilationInfo;

  /// Node def dependency graph
  NodeDefDependencyArray            m_nodeDefDependencyGraph;

  /// Shared task function tables
  MR::SharedTaskFnTables*           m_taskQueuingFnTables;
  MR::SharedTaskFnTables*           m_outputCPTaskFnTables;

  /// message distributor information
  MessageInterestArray              m_messageInterest;

  /// Semantic lookup tables for all node types.
  ///  They provide fast access into a NodeDefs packed AttribDataArray.
  uint32_t                          m_semanticLookupTablesCount;
  MR::SemanticLookupTable**         m_semanticLookupTables;
  NMP::Memory::Format               m_semanticLookupTablesMemReqs;

  nmtl::vector<std::string>   m_subRigs;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::NodeDefBuilder
/// \brief For construction of Nodes in the asset compiler - virtual base class.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class NodeDefBuilder
{
public:

  /// \brief Does this NodeType have a SemanticLookupTable i.e. it has some Def data.
  ///        Default returns false.
  virtual bool hasSemanticLookupTable();

  /// \brief Initialise the semantic lookup table for this NodeType in the memory resource provided.
  ///        Default returns NULL, this is used when the node has no Def data.
  virtual MR::SemanticLookupTable* initSemanticLookupTable(
    NMP::Memory::Resource&      memRes);

  /// \brief Compute the vector of input node connections for node def dependency processing.
  /// The vector of connected nodes should list all child node inputs and control parameter
  /// connections.
  virtual void getNodeDefInputConnections(
    std::vector<MR::NodeID>&    inputNodeIDs,
    const ME::NodeExport*       nodeDefExport,
    const ME::NetworkDefExport* netDefExport,
    AssetProcessor*             processor) = 0;

  /// \brief Pre-initialisation function that is called before initialising the node def. This
  /// is where any sub-network evaluation should be performed. Computed data can be attached to
  /// the user data of the node def compilation info. This data can then be deleted in the init
  /// function after it is no longer required.
  virtual void preInit(
    NetworkDefCompilationInfo*  netDefCompilationInfo,
    const ME::NodeExport*       nodeDefExport,
    const ME::NetworkDefExport* netDefExport,
    AssetProcessor*             processor) = 0;

  /// \brief Compute the memory requirements for this node
  virtual NMP::Memory::Format getNodeDefMemoryRequirements(
    NetworkDefCompilationInfo*  netDefCompilationInfo,
    const ME::NodeExport*       nodeDefExport,
    const ME::NetworkDefExport* netDefExport,
    AssetProcessor*             processor) = 0;

  /// \brief Initialise the NodeDef attribute data for this node and set up the queuing functions for this node.
  virtual MR::NodeDef* init(
    NMP::Memory::Resource&      memRes,
    NetworkDefCompilationInfo*  netDefCompilationInfo,
    const ME::NodeExport*       nodeDefExport,
    const ME::NetworkDefExport* netDefExport,
    AssetProcessor*             processor) = 0;

public:
  /// \brief Calculate the basic MR::NodeDef memory requirements. Includes AttribData slots, but does not include AttribData itself.
  static NMP::Memory::Format getCoreNodeDefMemoryRequirements(
    NetworkDefCompilationInfo*  netDefCompilationInfo, ///< Information about the network being built, including the data for each node as it is built.
    uint32_t                    numChildren,           ///< Total number of available child node connections.
                                                       ///<  It may be that only a sub-set of these are actually active at on time on execution.
    uint32_t                    numInputCPConnections, ///< Number of input control param connections. 
                                                       ///<  Each one is a single connection to another node.
    uint32_t                    numAnimSetEntries,     ///< 0 if no DefData data,
                                                       ///< 1 if all DefData is in AnimSet 0,
                                                       ///< Otherwise the same as the number of anim sets in the network.
    const ME::NodeExport*       nodeDefExport          ///< Data exported from connect from which to initialise this node.
    );

  /// \brief Initialise a MR::NodeDef in the memory resource provided. Set the  common node def header information.
  static MR::NodeDef* initCoreNodeDef(
    NMP::Memory::Resource&      resource,              ///< Mem resource where we will build this node def.
    NetworkDefCompilationInfo*  netDefCompilationInfo, ///< Information about the network being built, including the data for each node as it is built.
    uint32_t                    numChildren,           ///< Total number of available child node connections.
                                                       ///<  It may be that only a sub-set of these are actually active at a time.
    uint16_t                    maxNumActiveChildNodes, ///< The maximum number of child nodes that can be active on this node at a time.
    uint32_t                    numInputCPConnections, ///< Number of input control param connections. 
                                                       ///<  Each one is a single connection to another node.
    uint8_t                     numOutputCPPins,       ///< Number of output control parameter pins. Multiple connections can be reading from each pin.
    uint16_t                    numAnimSetEntries,     ///< 0 if no DefData data,
                                                       ///< 1 if all DefData is in AnimSet 0,
                                                       ///< Otherwise the same as the number of anim sets in the network.
    const ME::NodeExport*       nodeDefExport          ///< Data exported from connect from which to initialise this node.
    );

  /// \brief Initialise an AttribDataHandle entry on the supplied NodeDef.
  ///        Returns the entry index that of the AttribDataHandle that has been initialised.
  static uint32_t initAttribEntry(
    MR::NodeDef*               nodeDef,         ///< NodeDef being initialised.
    MR::AttribDataSemantic     semantic,        ///< The AttribDataHandle entry for this semantic is to be initialised.
    MR::AnimSetIndex           animSetIndex,    ///< Which anim set this AttribDataHandle is in.
    MR::AttribData*            attribData,      ///< Attrib data to store on the nodes AttribDataHandle.
    const NMP::Memory::Format& attribMemReqs    ///< Memory requirements to store on the nodes AttribDataHandle.
    );

  /// \brief Add a reflexive CP Pin AttribDataHandle entry on the supplied NodeDef.
  ///        Returns the outputCPPin pin index
  static MR::PinIndex newPinAttribEntry(
    MR::NodeDef*               nodeDef,
    MR::AttribDataSemantic     semantic,
    bool                       perAnimSet
    );
  
  /// \brief Add another animSet entry for the last reflexive CP Pin added to the NodeDef.
  static void addPinAttribAnimSetEntry(
    uint32_t                   pin,
    MR::NodeDef*               nodeDef,
    MR::AnimSetIndex           animSetIndex,
    MR::AttribData*            attribData,
    const NMP::Memory::Format& attribMemReqs
    );
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::NetworkDefBuilderBase
/// \brief For construction of a network definition in the asset compiler - pure virtual base class
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class NetworkDefBuilderBase
{
public:
  /// \brief Builds a complete network definition
  virtual NMP::Memory::Resource init(
    const ME::NetworkDefExport* netDefExport,
    AP::AssetProcessor*         processor) = 0;

  /// \brief Builds a partial network definition from the specified root node id. This
  /// function can be called during the network def building process to create a partially
  /// built network definition.
  virtual NMP::Memory::Resource buildSubNetworkDef(
    MR::NodeID rootNodeID,
    bool enableCombineTrajectoryTransformTasks) = 0;

  ///
  virtual void initCompileAnimations(
    const ME::AnimationLibraryExport* animLibraryExport,
    AssetProcessor* processor) = 0;

  virtual bool findAnimAndRigToAnimMapAssetIDPair(
    const AP::AssetCompilerPlugin::AnimationFileMetadata& animData,
    const char*         rigFilename,
    bool                generateDeltas,
    uint32_t&           runtimeAnimLocator,
    MR::RuntimeAssetID& rigToAnimMapAssetID) = 0;

  virtual void tidyCompileAnimations() = 0;
};

//----------------------------------------------------------------------------------------------------------------------
// NodeDefCompilationInfo inline functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NodeDefCompilationInfo::NodeDefCompilationInfo()
: m_nodeDef(0),
  m_userData(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NodeDefCompilationInfo::initPinInfo()
{
  m_dataPinInfo.init();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NodeDefCompilationInfo::clearPinInfo()
{
  m_dataPinInfo.init();
  for (uint32_t i = 0; i < m_inputPinInfo.size(); ++i)
  {
    if (m_inputPinInfo[i].m_nodeIDLabel)
    {
      NMP::Memory::memFree(m_inputPinInfo[i].m_nodeIDLabel);
      m_inputPinInfo[i].m_nodeIDLabel = NULL;
    }
  }
  m_inputPinInfo.resize(0);
}

//----------------------------------------------------------------------------------------------------------------------
// MessageInterest inline functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE MessageInterest::MessageInterest()
: m_messageID(0),
  m_messageType(MR::INVALID_MESSAGE_TYPE_ID),
  m_messageDistributor(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
// NetworkDefCompilationInfo inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NetworkDefCompilationInfo::setNodeDef(
  uint32_t nodeID,
  NMP::Memory::Format& nodeDefMemReqs,
  MR::NodeDef* nodeDef)
{
  NMP_VERIFY(nodeID < getNumNodes());
  NMP_VERIFY_MSG(
    m_nodeDefCompilationInfo[nodeID].m_nodeDef == NULL,
    "There is already a nodeDef attached to the nodeDef compilation info.");

  m_nodeDefCompilationInfo[nodeID].m_nodeDefMemReqs = nodeDefMemReqs;
  m_nodeDefCompilationInfo[nodeID].m_nodeDef = nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NetworkDefCompilationInfo::releaseNodeDef(uint32_t nodeID)
{
  NMP_VERIFY(nodeID < getNumNodes());
  if (m_nodeDefCompilationInfo[nodeID].m_nodeDef)
  {
    // Note that the user is responsible for managing the user data
    NMP::Memory::memFree(m_nodeDefCompilationInfo[nodeID].m_nodeDef);
    m_nodeDefCompilationInfo[nodeID].m_nodeDef = NULL;
    m_nodeDefCompilationInfo[nodeID].m_nodeDefMemReqs.set(0, NMP_NATURAL_TYPE_ALIGNMENT);
    m_nodeDefCompilationInfo[nodeID].clearPinInfo();
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE MR::DataPinInfo* NetworkDefCompilationInfo::getNodeDataPinInfo(uint32_t nodeID)
{
  NMP_VERIFY(nodeID < getNumNodes());
  return &(m_nodeDefCompilationInfo[nodeID].m_dataPinInfo);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE MR::AttribDataSemantic NetworkDefCompilationInfo::getNodeDataPinSemantic(uint32_t nodeID, uint32_t pinIndex)
{
  NMP_VERIFY(nodeID < getNumNodes());
  NMP_VERIFY(pinIndex < m_nodeDefCompilationInfo[nodeID].m_inputPinInfo.size());
  return m_nodeDefCompilationInfo[nodeID].m_inputPinInfo[pinIndex].m_semantic;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NetworkDefCompilationInfo::setNodeDataPinInfo(
  uint32_t               nodeID,
  const char*            nodeIDLabel,
  MR::AttribDataSemantic semantic,
  MR::PinIndex           pinIndex,
  uint32_t               numAnimSets,
  bool                   readPerAnimSetData,
  bool                   optional)
{
  NMP_VERIFY(nodeID < getNumNodes());
  NMP_VERIFY(pinIndex == m_nodeDefCompilationInfo[nodeID].m_inputPinInfo.size());

  InputPinInfo pinInfo(semantic, pinIndex, numAnimSets, readPerAnimSetData, optional);
  m_nodeDefCompilationInfo[nodeID].m_inputPinInfo.push_back(pinInfo);

  size_t stringLen = strlen(nodeIDLabel) + 1;
  m_nodeDefCompilationInfo[nodeID].m_inputPinInfo[pinIndex].m_nodeIDLabel = (char*) NMPMemoryAlloc(stringLen);
  strcpy_s(m_nodeDefCompilationInfo[nodeID].m_inputPinInfo[pinIndex].m_nodeIDLabel, stringLen, nodeIDLabel);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE MR::NodeDef* NetworkDefCompilationInfo::getNodeDef(uint32_t nodeID)
{
  NMP_VERIFY(nodeID < getNumNodes());
  return m_nodeDefCompilationInfo[nodeID].m_nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const MR::NodeDef* NetworkDefCompilationInfo::getNodeDef(uint32_t nodeID) const
{
  NMP_VERIFY(nodeID < getNumNodes());
  return m_nodeDefCompilationInfo[nodeID].m_nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Memory::Format NetworkDefCompilationInfo::getNodeDefMemReqs(uint32_t nodeID) const
{
  NMP_VERIFY(nodeID < getNumNodes());
  return m_nodeDefCompilationInfo[nodeID].m_nodeDefMemReqs;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool NetworkDefCompilationInfo::isNodeDefCompiled(uint32_t nodeID) const
{
  NMP_VERIFY(nodeID < getNumNodes());
  return m_nodeDefCompilationInfo[nodeID].m_nodeDef != NULL;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NetworkDefCompilationInfo::attachUserData(uint32_t nodeID, void* userData)
{
  NMP_VERIFY(nodeID < getNumNodes());
  NMP_VERIFY_MSG(
    m_nodeDefCompilationInfo[nodeID].m_userData == NULL,
    "There is already user data attached to the nodeDef compilation info.");
  
  m_nodeDefCompilationInfo[nodeID].m_userData = userData;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void* NetworkDefCompilationInfo::getUserData(uint32_t nodeID)
{
  NMP_VERIFY(nodeID < getNumNodes());
  return m_nodeDefCompilationInfo[nodeID].m_userData;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void* NetworkDefCompilationInfo::detachUserData(uint32_t nodeID)
{
  NMP_VERIFY(nodeID < getNumNodes());
  NMP_VERIFY_MSG(
    m_nodeDefCompilationInfo[nodeID].m_userData != NULL,
    "There is no user data currently attached to the nodeDef compilation info.");
    
  void* userData = m_nodeDefCompilationInfo[nodeID].m_userData;
  m_nodeDefCompilationInfo[nodeID].m_userData = NULL;
  return userData; 
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NodeDefDependency& NetworkDefCompilationInfo::getNodeDefDependency(uint32_t nodeID)
{
  NMP_VERIFY(nodeID < m_nodeDefDependencyGraph.size());
  return m_nodeDefDependencyGraph[nodeID];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const NodeDefDependency& NetworkDefCompilationInfo::getNodeDefDependency(uint32_t nodeID) const
{
  NMP_VERIFY(nodeID < m_nodeDefDependencyGraph.size());
  return m_nodeDefDependencyGraph[nodeID];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NetworkDefCompilationInfo::getInterestedNodesForMessage(
  MR::MessageID messageID,
  nmtl::pod_vector<MR::NodeID>* interestedNodes) const
{
  NMP_VERIFY(messageID < m_messageInterest.size());
  NMP_VERIFY(interestedNodes);

  *interestedNodes = m_messageInterest[messageID].m_nodeIDs;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NetworkDefCompilationInfo::setMessageDistributor(
  MR::MessageID messageID,
  NMP::Memory::Format& messageDistributorMemReqs,
  MR::MessageDistributor* messageDistributor)
{
  NMP_VERIFY(messageID < m_messageInterest.size());
  NMP_VERIFY_MSG(
    m_messageInterest[messageID].m_messageDistributor == NULL,
    "There is already a messageDistributor attached to the messageDistributor compilation info.");

  m_messageInterest[messageID].m_messageDistributorMemReqs = messageDistributorMemReqs;
  m_messageInterest[messageID].m_messageDistributor = messageDistributor;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE MR::MessageDistributor* NetworkDefCompilationInfo::getMessageDistributor(MR::MessageID messageID)
{
  NMP_VERIFY(messageID < (MR::MessageID)m_messageInterest.size());
  return m_messageInterest[messageID].m_messageDistributor;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const MR::MessageDistributor* NetworkDefCompilationInfo::getMessageDistributor(MR::MessageID messageID) const
{
  NMP_VERIFY(messageID < (MR::MessageID)m_messageInterest.size());
  return m_messageInterest[messageID].m_messageDistributor;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Memory::Format NetworkDefCompilationInfo::getMessageDistributorMemReqs(MR::MessageID messageID) const
{
  NMP_VERIFY(messageID < (MR::MessageID)m_messageInterest.size());
  return m_messageInterest[messageID].m_messageDistributorMemReqs;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool NetworkDefCompilationInfo::isMessageDistributorCompiled(MR::MessageID messageID) const
{
  NMP_VERIFY(messageID < (MR::MessageID)m_messageInterest.size());
  return (m_messageInterest[messageID].m_messageDistributor != 0);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_DEF_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
