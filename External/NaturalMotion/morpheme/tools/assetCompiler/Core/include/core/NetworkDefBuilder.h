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
#ifndef MR_NETWORK_DEF_BUILDER_H
#define MR_NETWORK_DEF_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"
#include "assetProcessor/NodeBuilder.h"
#include "assetProcessor/AssetProcessor.h"
#include "assetProcessor/NodeBuilderUtils.h"
#include "export/mcExport.h"
#include "export/mcExportXml.h"
#include "morpheme/mrManager.h"
#include "morpheme/mrNetworkDef.h"
#include "acCore.h"
#include <vector>
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::RigInfo
/// \brief Utility container for holding information about a Rig.
/// \ingroup AnimationLibraryAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class RigInfo
{
public:
  const MR::AnimRigDef* m_rig;
  uint32_t              m_compatibleRigsGroupIndex; ///< Index into sm_compatibleRigGroupsVector vector
  float                 m_rigScaleFactor;
  float                 m_medianRigBoneLength;      ///< Median bone length information used later to determine
                                                    ///< any unchanging channels during animation compression.
  char                  m_rigFilename[1];           ///< A copy of the destination filename.

  static size_t getRequiredMemory(const char* filename)
  {
    return sizeof(RigInfo) + strlen(filename);
  }
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::TrajectoryComponentCompatibleRigsGroup
/// \brief  Utility container for holding a vector of rigs that all share the same Trajectory Components.
/// \ingroup AnimationLibraryAssetProcessorModule
///
/// Rigs within one of these groups can share animations at runtime.
//----------------------------------------------------------------------------------------------------------------------
class TrajectoryComponentCompatibleRigsGroup
{
public:
  std::vector<uint32_t> m_compatibleRigsVector; ///< Indexes into a RigInfo vector.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::AnimCompilationInfo
/// \brief Utility container for holding information about how an need to be compiled.
/// \ingroup AnimationLibraryAssetProcessorModule
///
/// e.g. What rigs it needs to be compiled against, rig to anim map references etc.
//----------------------------------------------------------------------------------------------------------------------
class AnimCompilationInfo
{
public: 
  /// \brief Find an entry in the m_rigInfoIndexesToCompileAgainst with the specified value.
  /// \return Index into m_rigInfoIndexesToCompileAgainst or NOT_FOUND if not found.
  uint32_t findRigInfoIndexToCompileAgainst(uint32_t rigInfoIndexToFind);

  /// \brief Add another entry to the vector rigs that this anim needs to be compiled against.
  void addRigInfoIndexToCompileAgainst(uint32_t rigInfoIndex);

public:
  AP::AssetCompilerPlugin::AnimationFileMetadata m_data;

  uint32_t                        m_compatibleRigsGroupIndex;       ///< Index it to AnimationLibraryBuilder's sm_compatibleRigGroupsVector (To compile this anim against).
  std::vector<uint32_t>           m_rigInfoIndexesToCompileAgainst; ///< Indexes within AnimationLibraryBuilder's sm_rigInfoVector (To compile this anim against).
  uint32_t                        m_conglomeratedRigBoneNamesIndex; ///< Index into AnimationLibraryBuilder's sm_conglomeratedRigBoneNamesVector (To compile this anim against).

  MR::RuntimeAnimAssetID          m_animAssetID;                    ///< Filled in by processAnim Function.
  std::vector<MR::RuntimeAssetID> m_rigToAnimMapAssetIDs;           ///< Filled in by processAnim Function.
                                                                    ///< Asset IDs of generated rig to anim maps from this anim to each rig.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::ConglomeratedRigBoneNames
/// \brief Utility container for holding a vector of conglomerated rig bone names.
/// \ingroup AnimationLibraryAssetProcessorModule
///
/// i.e. a bone names agains which animations are to be compiled.
//----------------------------------------------------------------------------------------------------------------------
class ConglomeratedRigBoneNames
{
public:
  /// \brief
  bool hasIdenticalRigSet(
    std::vector<uint32_t>& rigInfoIndexes   ///< Indexes in to AnimationLibraryBuilders m_animLibraryRigInfoVector.
  );

  /// \brief
  void addRigInfoIndexes(std::vector<uint32_t>& rigInfoIndexes);

  /// \brief
  uint32_t findBoneIndex(const char* boneName);

public:
  std::vector<uint32_t>              m_rigInfoIndexes; ///< Indexes in to AnimationLibraryBuilders
                                                       ///<  m_animLibraryRigInfoVector. Lexicographically sorted by rig name.
  AP::AssetCompilerPlugin::AnimationFileRigData m_data;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::NetworkDefBuilderPlugin
/// \brief Utility class used by the NetworkDefBuilder to create data it doesn't know about. The
/// attribute data is initialised and attached to the network node def's attribute data entries.
/// \ingroup NetworkDefAssetProcessorModule
/// \see AP::NetworkDefBuilder
//----------------------------------------------------------------------------------------------------------------------
class NetworkDefBuilderPlugin
{
public:
  /// \brief Allows a plugin to add semantics entries to the NetworkDefs lookup table that are unique to the plugin.
  ///        There is only one slot for each semantic type so any attempt at making a duplicate entry will case an assertion.
  virtual void populateNetDefSemanticLookupTable(
    const ME::NetworkDefExport*  netDefExport,              ///< 
    MR::SemanticLookupTable*     netDefSemanticTable) = 0;  ///< Returns the number of semantics that are unique to this plugin.

  virtual NMP::Memory::Format getPluginMemoryRequirements(const ME::NetworkDefExport* netDefExport) = 0;

  virtual bool initPluginData(
    NMP::Memory::Resource&           memRes,                    ///< Where plugin data is to be initialised.
    MR::AttribDataHandle*            netDefAttribDataArray,     ///< The the array of AttribData of the NetworkDef itself.
    uint32_t                         netDefAttribDataArraySize,
    MR::SemanticLookupTable*         netDefSemanticLookupTable, ///< Lookup table for the NetworkDef itself.
    AP::AssetProcessor*              processor,
    const ME::NetworkDefExport*      netDefExport) = 0;
};

class NetworkDefBuilderEx;

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::NetworkDefBuilder
/// \brief Utility class used by the asset compiler to construct a NetworkDef asset.
/// \ingroup NetworkDefAssetProcessorModule
/// \see MR::NetworkDef
//----------------------------------------------------------------------------------------------------------------------
class NetworkDefBuilder : public NetworkDefBuilderBase
{
  // ACPluginCore is currently filling the NetworkDefBuilders internal containers, thus it needs to get access to it.
  // normally these containers should not be static - it will never be possible to compile networks multi-threaded
  // as long as there are still static containers like these accessed.
  friend class ACPluginCore;

public:
  /// \brief Builds a complete network definition
  virtual NMP::Memory::Resource init(
    const ME::NetworkDefExport* netDefExport,
    AP::AssetProcessor*         processor) NM_OVERRIDE;

  /// \brief Builds a partial network definition from the specified root node id. This
  /// function can be called during the network def building process to create a partially
  /// built network definition.
  virtual NMP::Memory::Resource buildSubNetworkDef(
    MR::NodeID rootNodeID,
    bool enableCombineTrajectoryTransformTasks) NM_OVERRIDE;

  virtual void initCompileAnimations(
    const ME::AnimationLibraryExport* animLibraryExport,
    AssetProcessor* processor) NM_OVERRIDE;

  virtual bool findAnimAndRigToAnimMapAssetIDPair(
    const AP::AssetCompilerPlugin::AnimationFileMetadata& animData,
    const char*         rigFilename,
    bool                generateDeltas,
    uint32_t&           runtimeAnimLocator,
    MR::RuntimeAssetID& rigToAnimMapAssetID) NM_OVERRIDE;

  virtual void tidyCompileAnimations() NM_OVERRIDE;

  /// \brief Allows an asset compiler plug-in to register a NetworkDefBuilderPlugin with the network builder, which
  ///  provides a way to insert AttribData into the network.
  static bool registerBuilderPlugin(NetworkDefBuilderPlugin* plugin);

  /// \brief Returns true if this rig is made up of sub rigs
  static bool isMultiRig(const ME::RigExport* rigExport);
  /// \brief Searches for sub rigs in an anim def. Sub rigs are identified by the presence of a namespace. The namespace delimeter is ":"
  static void findSubRigs(const MR::AnimRigDef* animRigDef, std::list<std::string>& subRigs);
  /// \brief constricts the name of the mrarig to open for the specified subrig
  static std::string constructSubRigFilename(const char *rigFilename, const char *subRigName);
  /// \brief gets the sub rig filename if are dealing with a multi rig.
  static void getMultiRigSubRigFilename( 
    std::string & subRigFilenameOut,
    const char * rigFilename,
    const ME::AnimationEntryExport* animEntry,
    const MR::AnimRigDef * curRig,
    AssetProcessor* processor=NULL);// provide an AssetProcessor pointer for message logger output.

  /// \brief Check using the valid animation set indices written when building node dependency graph whether this animation needs to be compiled.
  static bool NetworkDefBuilder::isAnimEntryValidWithAnimSet( 
    const ME::AnimationEntryExport* animEntry, 
    const ME::AnimationSetExport* animSetExport);

  /// \brief Gets the NodeTypes of all the descendants (all nodes in a towards children direction) of this node 
  ///  (that is downstream using runtime terminology).
  static AC_CORE_EXPORT void getNodeTypesOfAllDescendants( const NodeDefDependency & nodeDep, std::vector<MR::NodeType> & nodeTypes );
  static void getNodeTypesRecursive( const NodeDefDependency & nodeDep, std::vector<MR::NodeType> & nodeTypes, std::vector<MR::NodeID> & nodeIDs );

protected:
  
  /// \brief The set of functions for calculating and writing the valid anim sets for each node in the network.
  void computeValidAnimSetsForAllNodes(const ME::NetworkDefExport* netDefExport);
  void computeValidAnimSetsForNode(const ME::NetworkDefExport* netDefExport, NodeDefDependency & nodeDep, std::vector<MR::AnimSetIndex> animSets);
  bool getNodeValidAnimSets(const ME::NetworkDefExport* netDefExport, const NodeDefDependency & nodeDep, std::vector<MR::AnimSetIndex> & animSets);
  bool setAnimSets(NodeDefDependency & nodeDep, const std::vector<MR::AnimSetIndex> & animSets);
  static void debugWriteAnimSetsPerNode(const ME::NetworkDefExport* netDefExport);

  //-------------------------
  void createNetworkDefCompilationInfo(
    const ME::NetworkDefExport* netDefExport,
    AP::AssetProcessor*         processor);

  void tidyNetworkDefCompilationInfo();

  void tidyNetworkNodeDefCompilationInfo();

  void buildNodeDefDependencyGraph(
    AP::AssetProcessor*         processor,
    const ME::NetworkDefExport* netDefExport);

  void computeNodeDefDependencies(
    NodeDefDependency*          nodeDefDep,
    bool*                       evalNodeDefDeps);

  void computeNodeDefDependencies(
    MR::NodeID rootNodeID,
    std::vector<MR::NodeID>& nodeDefDeps);

  void buildNetworkNodeDef(
    AP::AssetProcessor*         processor,
    const ME::NetworkDefExport* netDefExport);

  void buildControlParameterNodeDefs(
    AP::AssetProcessor*         processor,
    const ME::NetworkDefExport* netDefExport);

  void buildAnimSourceNodeDefs(
    AP::AssetProcessor*         processor,
    const ME::NetworkDefExport* netDefExport,
    MR::NodeID                  rootNodeID);

  void buildNetworkDefInfo(
    AP::AssetProcessor*         processor,
    const ME::NetworkDefExport* netDefExport);

  void buildNodeDef(
    uint32_t                    nodeID,
    const ME::NetworkDefExport* netDefExport,
    AssetProcessor*             processor);

  void buildAnimNodeDefs(
    NodeDefDependency*          nodeDefDep,
    bool*                       evalNodeDefDeps,
    AP::AssetProcessor*         processor,
    const ME::NetworkDefExport* netDefExport);

  void buildNodeDefs(
    NodeDefDependency*          nodeDefDep,
    bool*                       evalNodeDefDeps,
    AP::AssetProcessor*         processor,
    const ME::NetworkDefExport* netDefExport);

  void buildNodeDefs(
    AP::AssetProcessor*         processor,
    const ME::NetworkDefExport* netDefExport,
    MR::NodeID                  rootNodeID);

  void buildMessageDistributors(
    AP::AssetProcessor*         processor,
    const ME::NetworkDefExport* netDefExport);

  void setNodeOptimisationFlags(MR::NodeDef* nodeDef);

  NMP::Memory::Format getMemoryRequirementsNetworkNodeDef(
    const ME::NetworkDefExport* netDefExport);

  /// \brief Build an array of unique NodeTypes used in the Network being compiled and store in sm_networkDefCompilationInfo.
  uint32_t buildSemanticLookupTables(
    NetworkDefCompilationInfo*  networkDefCompilationInfo,
    AP::AssetProcessor*         processor,
    const ME::NetworkDefExport* netDefExport);

    
  //-------------------------
  /// \brief Function to compute the median bone length of the animation rig
  static float computeMedianRigBoneLength(const MR::AnimRigDef* rig);

  /// \brief Build up a vector of information about all the rigs to be used in this AnimationLibrary.
  static void buildRigInfoVector(
    AssetProcessor*                   processor,
    const ME::AnimationLibraryExport* animLibraryExport,
    std::vector<RigInfo*>&            rigInfoVector);

  /// \brief Free any memory used in the rig info vector and empty the vector.
  static void tidyRigInfoVector(std::vector<RigInfo*>& rigInfoVector);

  /// \brief add the specified rig to the sm_rigInfoVector
  /// \return true if added, false if already present.
  static bool addRigToRigInfoVector(
    const ME::RigExport* rigExport, 
    MR::AnimRigDef* currentRig, 
    std::vector<RigInfo*>& rigInfoVector);

  /// \brief Find the specified rig in the rig info vector.
  /// \return Index into sm_rigInfoVector or NOT_FOUND if not found.
  static uint32_t findRigInRigInfoVector(const char* rigFilename, const std::vector<RigInfo*>& rigInfoVector);

  /// \brief Find the specified rig in the rig info vector.
  /// \return Index into sm_rigInfoVector or NOT_FOUND if not found.
  static uint32_t findRigInRigInfoVector(const MR::AnimRigDef* rigToFind, const std::vector<RigInfo*>& rigInfoVector);

  /// \brief Build up a vector of trajectory compatible rigs.
  static void buildCompatibleRigGroupsVector(
    std::vector<RigInfo*>&                                rigInfoVector,
    std::vector<TrajectoryComponentCompatibleRigsGroup*>& compatibleRigGroupsVector);

  /// \brief Free any memory used in the trajectory compatible rigs vector and empty the vector.
  static void tidyCompatibleRigGroupsVector(std::vector<TrajectoryComponentCompatibleRigsGroup*>& compatibleRigGroupsVector);

  /// \brief Search through the vector of rig groups to find one that the supplied rig is compatible with.
  /// \return Index into sm_compatibleRigGroupsVector or NOT_FOUND if not found.
  static uint32_t findCompatibleRigGroup(
    const MR::AnimRigDef*                                       rigToTest,
    const std::vector<TrajectoryComponentCompatibleRigsGroup*>& compatibleRigsVector,
    const std::vector<RigInfo*>&                                rigInfoVector);

  /// \brief Build a vector of animations and the rig groups and the rigs from within the groups that they need to be compiled against.
  ///
  /// Construct this from all of the anim rig combinations in all the anim sets in this library.
  static void buildAnimCompilationInfoVector(
    AP::AssetProcessor*               processor,
    const ME::AnimationLibraryExport* animLibraryExport,
    const std::vector<RigInfo*>&      rigInfoVector,
    std::vector<AnimCompilationInfo*>&animCompilationInfoVector);

  /// \brief Called from buildAnimCompilationInfoVector get the rigInfoIndex and currentRigInfo.
  static void getCurrentRigInfoAndIndex(  AP::AssetProcessor*             processor,
                                          const ME::AnimationEntryExport* animEntry,
                                          const ME::AnimationSetExport*   animSetExport,
                                          const MR::AnimRigDef*           currentRig,
                                          const std::vector<RigInfo*> &   rigInfoVector,
                                          uint32_t &                      rigInfoIndex,
                                          RigInfo* &                      currentRigInfo );

  /// \brief Return whether the animCompilationInfoVector was added to or updated.
  static bool updateAnimCompilationInfoVector( 
    AP::AssetCompilerPlugin::AnimationFileMetadata &  animData,
    std::vector<AnimCompilationInfo*> &               animCompilationInfoVector,
    uint32_t                                          animCompilationIndex,
    uint32_t                                          rigInfoIndex,
    uint32_t                                          compatibleRigsGroupIndex );

  /// \brief Free any memory used in the anim compilation vector and empty the vector.
  static void tidyAnimCompilationInfoVector(std::vector<AnimCompilationInfo*>& animCompilationVector);

  /// \brief Get the processed Rig asset for the specified animation set export.
  static MR::AnimRigDef* getAnimSetExportsProcessedRig(
    AP::AssetProcessor*           processor,
    const ME::AnimationSetExport* animSetExport);

  /// \brief Find an entry in the animation compilation vector with the specified anim name, take name and rigGroupIndex.
  /// \return Index into sm_animCompilationVector or NOT_FOUND if not found.
  static uint32_t findAnimationAndRigGroupInAnimCompilationInfoVector(
    const AP::AssetCompilerPlugin::AnimationFileMetadata& animData,
    uint32_t                                              searchCompatibleRigsGroupIndex,
    const std::vector<AnimCompilationInfo*>&              sourceVector);

  /// \brief
  static int32_t findRigBoneIndexForSimpleHierarchy(
    const std::vector<const MR::AnimRigDef*>& rigs,
    const std::vector<const char*>& rigBoneNames,
    uint32_t rigIndex);

  /// \brief Builds a conglomerated vector of rig bone names in *simple hierarchy order*. This
  /// also orders the bones in the names vector by their weighting (number of rigs the
  /// bone appears in) so that animations compiled against the rig set only need to partially
  /// decompress the animation channels depending on the LOD.
  static void createRigBoneNamesForSimpleHierarchy(
    const std::vector<const MR::AnimRigDef*>& rigs,         ///< IN: The rigs that make up the super set.
    const std::vector<const char*>&           rigBoneNames, ///< IN: The unique list of all bone names contained within the rigs
    std::vector<const char*>&                 rigBoneNamesForSimpleHierarchy ///< OUT: The super set bone names in simple hierarchy (and weighted) order
    );

  /// \brief
  static void buildConglomeratedRigBoneNamesVector(
    const std::vector<AnimCompilationInfo*>& animCompilationVector,
    const std::vector<RigInfo*>&             rigInfoVector,
    std::vector<ConglomeratedRigBoneNames*>& conglomeratedRigBoneNamesVector);

  /// \brief
  static void tidyConglomeratedRigBoneNamesVector(std::vector<ConglomeratedRigBoneNames*>& conglomeratedRigBoneNamesVector);

  /// \brief
  static uint32_t findRigSetInConglomeratedRigBonesVector(
    std::vector<uint32_t>& rigInfoIndexes ///< Indexes in to AnimationLibraryBuilders m_animLibraryRigInfoVector.
  );

  /// \brief
  static void constructRigCompilationVectors(ConglomeratedRigBoneNames* rigBoneNamesEntry, const std::vector<RigInfo*>& rigInfoVector);

  /// \brief
  static void compileAnimationsAndRigToAnimMaps(
    AP::AssetProcessor*               processor,
    const ME::AnimationLibraryExport* animLibraryExport);

  /// \brief Build the event track name to runtime id map.
  static NMP::OrderedStringTable* buildEventTrackNameToRuntimeIDMap(
    AP::AssetProcessor* processor,
    const ME::NetworkDefExport* netDefExport);

  /// \brief Build a string table for each state machine state.
  ///        A state machine string table maps state names to state IDs.
  static NMP::IDMappedStringTable* buildStateMachineStateIDStringTable(
    const ME::NetworkDefExport* netDefExport);

  /// \brief Calculate memory requirements of NodeID to Node name mapping table.
  /// \return Number of strings and table size.
  static NMP::Memory::Format getNodeIDNameMappingTableMemReqs(
    const ME::NetworkDefExport* netDefExport,
    uint32_t&                   numStrings,
    uint32_t&                   tableDataSize);

  /// \brief Set up the NodeID to Node name mapping table.
  static NMP::IDMappedStringTable* buildNodeIDNameMappingTable(
    const ME::NetworkDefExport* netDefExport);

  /// \brief Calculate memory requirements of the Message to Node name mapping table.
  /// \return Number of strings and table size.
  static NMP::Memory::Format getMessageIDNameMappingTableMemReqs(
    const ME::NetworkDefExport* netDefExport,
    uint32_t&                   numStrings,
    uint32_t&                   tableDataSize);

  /// \brief Set up the MessageID to Node name mapping table.
  static NMP::OrderedStringTable* buildMessageIDNameMappingTable(
    const ME::NetworkDefExport* netDefExport);

  static NMP::Memory::Format getStateMachineNodeIDsArrayMemReqs(
    const ME::NetworkDefExport* netDefExport,
    uint32_t& numStateMachineNodes);

  /// \brief Set up the state machine node ids array.
  static MR::NodeIDsArray* buildStateMachineNodeIDsArray(
    const ME::NetworkDefExport* netDefExport);

  static NMP::Memory::Format getMultiplyConnectedNodeIDsArrayMemReqs(
    const ME::NetworkDefExport* netDefExport,
    uint32_t& numStateMachineNodes);

  /// \brief Set up the multiply connected node ids array.
  static void buildMultiplyConnectedNodeIDsArray(
    NodeDefDependency*          nodeDefDep,
    bool*                       evalNodeDefDeps,
    const ME::NetworkDefExport* netDefExport,
    uint32_t&                   numMultiplyConnectedNodes,
    MR::NodeIDsArray*           result);

  static MR::NodeIDsArray* buildMultiplyConnectedNodeIDsArray(
    const ME::NetworkDefExport* netDefExport);

  /// \brief Build the node tag table.
  static MR::NodeTagTable* buildNodeTagsTable(
    const ME::NetworkDefExport* netDefExport);

  /// \brief Calculate memory requirements of the request emitter NodeID array.
  static NMP::Memory::Format getMessageEmitterNodeIDsArrayMemReqs(
    const ME::NetworkDefExport* netDefExport,
    uint32_t& numMessageEmitterNodeIDs);

  /// \brief Set up the message emitter node ids array.
  static MR::NodeIDsArray* buildMessageEmitterNodeIDsArray(
    const ME::NetworkDefExport* netDefExport);

  /// \brief Calculate the memory requirements of the Node OnExit Message array for this net def export.
  static NMP::Memory::Format getNodeOnExitMessageArrayMemReqs(const ME::NetworkDefExport* netDefExport);

  /// \brief Construct the array of Node OnExit Messages.
  static MR::NetworkDef::NodeEventOnExitMessage* buildNodeOnExitMessageArray(
    const ME::NetworkDefExport* netDefExport, NMP::Memory::Format* format);

  /// \brief Calculate memory requirements of the semantic lookup tables of the network being processed.
  static NMP::Memory::Format getSemanticLookupTablesMemoryRequirements(
    uint32_t numUniqueNodeTypes); ///< Including the NetworkDef node itself.

  /// \brief Copy from the static store to a new instance in the supplied resource.
  void copySemanticLookupTables(
    NetworkDefBuilderEx*    networkDef,
    NMP::Memory::Resource&  memRes);

  /// \brief Initialise the semantic lookup tables of the network being processed.
  ///        One lookup table per NodeType in the network.
  void copySemanticLookupTables(
    AP::AssetProcessor*    processor,
    NMP::Memory::Resource& memRes);

  /// \brief Calculate memory requirements of per animation set mirror mapping attributes.
  static NMP::Memory::Format getMirrorMappingAttribDataMemReqs(
    const ME::AnimationSetExport*   animSetExport);

  /// \brief Calculate memory requirements of per animation set joint limits attribute.
  static NMP::Memory::Format getJointLimitsAttribDataMemReqs(
    const ME::RigExport* animRigExport);

  /// \brief Calculate memory requirements of per animation set retarget mapping attributes.
  static NMP::Memory::Format getRigRetargetMappingAttribDataMemReqs(
    const ME::AnimationSetExport *animSetExport
    );

  /// \brief Set up per animation set mirror mapping attributes.
  static void buildMirrorMappingAttribData(
    NMP::Memory::Resource&           memRes,
    MR::AttribDataHandle*            netDefAttribDataArray,
    uint32_t                         NMP_USED_FOR_ASSERTS(netDefAttribDataArraySize),
    MR::SemanticLookupTable*         netDefSemanticLookupTable,
    MR::AnimSetIndex                 animSetIndex,
    const ME::AnimationSetExport*    animSetExport,
    const ME::RigExport*             animRigExport);

  /// \brief Set up per animation set joint limits attribute.
  static void buildJointLimitsAttribData(
    NMP::Memory::Resource&           memRes,
    MR::AttribDataHandle*            netDefAttribDataArray,
    uint32_t                         NMP_USED_FOR_ASSERTS(netDefAttribDataArraySize),
    MR::SemanticLookupTable*         netDefSemanticLookupTable,
    MR::AnimSetIndex                 animSetIndex,
    const ME::RigExport*             animRigExport);

  /// \brief Set up retarget storage stats attribute.
  static void buildRetargetStorageStatsAttribData(
    NMP::Memory::Resource&           memRes,
    const ME::AnimationLibraryExport* animLibraryExport,
    MR::AttribDataHandle*            netDefAttribDataArray,
    uint32_t                         NMP_USED_FOR_ASSERTS(netDefAttribDataArraySize),
    MR::SemanticLookupTable*         netDefSemanticLookupTable);

  /// \brief Set up per animation set retarget mapping attributes.
  static void buildRigRetargetMappingAttribData(
    NMP::Memory::Resource&           memRes,
    const ME::AnimationSetExport*    animSetExport,
    MR::AttribDataHandle*            netDefAttribDataArray,
    uint32_t                         NMP_USED_FOR_ASSERTS(netDefAttribDataArraySize),
    MR::SemanticLookupTable*         netDefSemanticLookupTable,
    const ME::RigExport*             animRigExport,
    MR::AnimSetIndex                 animSetIndex,
    const std::map<std::string, int32_t>& retargetMap);

  /// \brief Find a unique index for each retarget mapping on all animation rigs
  static void buildRetargetingIndexMapping(
    const NetworkDefBuilderEx* networkNodeDef,
    const ME::AnimationLibraryExport* animLibraryExport,
    std::map<std::string, int32_t>& retargetGroupMap);

  /// \brief Set up per animation set default output mask.
  static void buildDefaultOutputMaskAttribData(
    NMP::Memory::Resource&           memRes,
    MR::AttribDataHandle*            netDefAttribDataArray,
    uint32_t                         NMP_USED_FOR_ASSERTS(netDefAttribDataArraySize),
    MR::SemanticLookupTable*         netDefSemanticLookupTable,
    MR::AnimSetIndex                 animSetIndex,
    const ME::RigExport*             animRigExport);

  /// \brief Create AttribData to hold a pointer to the animation sets character controller (may be a NULL pointer).
  static void buildCharacterControllerAttribData(
    NMP::Memory::Resource&           memRes,
    MR::AttribDataHandle*            netDefAttribDataArray,
    uint32_t                         NMP_USED_FOR_ASSERTS(netDefAttribDataArraySize),
    MR::SemanticLookupTable*         netDefSemanticLookupTable,
    MR::AnimSetIndex                 animSetIndex,
    const ME::AnimationSetExport*    animSetExport,
    AP::AssetProcessor*              processor);

  /// \brief Create AttribData to hold a pointer to the animation sets animation rig (there must be one).
  static void buildAnimRigAttribData(
    NMP::Memory::Resource&           memRes,
    MR::AttribDataHandle*            netDefAttribDataArray,
    uint32_t                         NMP_USED_FOR_ASSERTS(netDefAttribDataArraySize),
    MR::SemanticLookupTable*         netDefSemanticLookupTable,
    MR::AnimSetIndex                 animSetIndex,
    MR::AnimRigDef*                  animRigDef);

  /// \brief Searches Nodes in export for an instance with the specified typeID.
  static bool containsNodeOfType(const ME::NetworkDefExport* netDefExport, MR::NodeType nodeTypeID);

  /// \brief Fills in the semantic lookup table for the NetworkDef, including visiting all the plugins to find the semantics that they add.
  ///        Returns the number of AttribData slots required per anim set.
  static uint16_t initNetworkDefNodeSemanticLookupTable(
    const ME::NetworkDefExport*  netDefExport, 
    MR::SemanticLookupTable*     netDefSemanticLookupTable = NULL); ///< If supplied it is filled in otherwise a local working copy is used.

  /// \brief Returns the total number of AttribData slots required on the NetworkDef.
  static uint16_t calcNumNetworkDefAttribDatas(
    const ME::NetworkDefExport* netDefExportg);

  /// \brief Initialise as correct NodeDef type; fills in queuing ops table and outputCP ops table.
  static void initTaskQueuingFns(
    MR::NodeDef*                nodeDef,
    NetworkDefCompilationInfo*  netDefCompilationInfo,
    NMP::BasicLogger*           logger);

protected:
  //-------------------------
  /// A vector of plug-ins which have been registered with the network builder. Plug-ins allow attrib data to be
  /// generated from the source XML even if it's not understood by the core.
  static std::vector<NetworkDefBuilderPlugin*>                  sm_networkPlugins;

  //-------------------------
  // Animation source compiled data

  /// Information about each Rig used in this AnimLibrary.
  static std::vector<RigInfo*>                                  sm_rigInfoVector;

  /// Vector of Trajectory Component compatible rig groups. Rigs in a group can share animations at runtime.
  static std::vector<TrajectoryComponentCompatibleRigsGroup*>   sm_compatibleRigGroupsVector;

  /// Vector of animations and the rig groups and the rigs from within the groups that they need to be compiled against.
  /// Note: An animation can have more than one entry in this vector if it needs to be compiled against multiple rig groups.
  static std::vector<AnimCompilationInfo*>                      sm_animCompilationVector;

  /// Vector of conglomerated rig bone names
  static std::vector<ConglomeratedRigBoneNames*>                sm_conglomeratedRigBoneNamesVector;

  //-------------------------
  // NetworkDef compilation data
  static NetworkDefCompilationInfo*                             sm_networkDefCompilationInfo;

  /// Output control parameter Node IDs and semantics
  static MR::EmittedControlParamsInfo*                          sm_emittedControlParamsInfo;

  /// State machine Node IDs array
  static MR::NodeIDsArray*                                      sm_stateMachineNodeIDs;

  /// Message Emitter Nodes ID array
  static MR::NodeIDsArray*                                      sm_messageEmitterNodeIDs;

  /// Multiply connected Nodes ID array
  static MR::NodeIDsArray*                                      sm_multiplyConnectedNodeIDs;

  /// NodeEventOnExitMessage array
  static MR::NetworkDef::NodeEventOnExitMessage*                sm_onExitMessageArray;
  static NMP::Memory::Format                                    sm_onExitMessageArrayMemReqs;

  /// Map of state machine state name to state IDs
  static NMP::IDMappedStringTable*                              sm_stateMachineStateIDStringTable;

  /// NodeID to Node name mapping table
  static NMP::IDMappedStringTable*                              sm_nodeIDNamesTable;

  /// MessageID to Message name mapping table
  static NMP::OrderedStringTable*                               sm_messageIDNamesTable;

  /// Event track names table
  static NMP::OrderedStringTable*                               sm_eventTrackIDNamesTable;

  /// Node tag table
  static MR::NodeTagTable*                                      sm_nodeTagTable;

  static uint32_t**                                             sm_rigToUberrigMaps;
  static NMP::Memory::Format                                    sm_rigToUberrigMapsMemReqs;
  static uint32_t**                                             sm_uberrigToRigMaps;
  static NMP::Memory::Format                                    sm_uberrigToRigMapsMemReqs;
};

AC_CORE_EXPORT bool registerBuilderPlugin(NetworkDefBuilderPlugin* plugin);

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NETWORK_DEF_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
