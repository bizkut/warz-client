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
#ifndef MR_NETWORK_PREDICTION_DEF_BUILDER_H
#define MR_NETWORK_PREDICTION_DEF_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"
#include "assetProcessor/AssetProcessor.h"
#include "Analysis/AnalysisPropertyUtils.h"
#include "morpheme/Prediction/mrNetworkPredictionDef.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::AnimSetGroupPredictionModels
/// \brief A table of resources representing the compiled prediction models for the
/// used anim sets within an anim set group.
//----------------------------------------------------------------------------------------------------------------------
class AnimSetGroupPredictionModels
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numUsedAnimSets);

  static AnimSetGroupPredictionModels* init(
    NMP::Memory::Resource& resource,
    uint32_t numUsedAnimSets);

  static AnimSetGroupPredictionModels* create(
    uint32_t numUsedAnimSets);

  void releaseAndDestroy();

public:
  uint32_t                m_numUsedAnimSets;
  NMP::Memory::Resource*  m_usedAnimSetModels;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::PredictionModelAnimSetGroupInfo
/// \brief
//----------------------------------------------------------------------------------------------------------------------
class PredictionModelAnimSetGroupInfo
{
public:
  // Compilation of individual prediction model components
  static NMP::Memory::Format getMemoryRequirements(
    const char* modelName,
    AnalysisAnimSetDependencyMap* animSetDependencyMap,
    AnimSetGroupPredictionModels* animSetPredictionModels);

  static PredictionModelAnimSetGroupInfo* init(
    NMP::Memory::Resource& resource,
    const char* modelName,
    AnalysisAnimSetDependencyMap* animSetDependencyMap,
    AnimSetGroupPredictionModels* animSetPredictionModels);

  static PredictionModelAnimSetGroupInfo* create(
    const char* modelName,
    AnalysisAnimSetDependencyMap* animSetDependencyMap,
    AnimSetGroupPredictionModels* animSetPredictionModels);

  void releaseAndDestroy();

  // Compilation of the final prediction model
  NMP::Memory::Format getAnimSetPredictionModelsMemoryRequirements() const;

  void initAnimSetPredictionModels(
    NMP::Memory::Resource& resource,
    MR::PredictionModelAnimSetGroup* animSetGroup,
    AP::AssetProcessor* processor) const;

public:
  // General properties
  char*                           m_modelName;
  int32_t                         m_hash;

  // Prediction model defs
  AnalysisAnimSetDependencyMap*   m_animSetDependencyMap;     ///< Anim set dependency information. This contains a dependency
                                                              ///< map between anim sets and compiled prediction models.
  AnimSetGroupPredictionModels*   m_animSetPredictionModels;  ///< Compiled prediction models for the used anim sets.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::NodeModelsCompilationInfo
/// \brief
//----------------------------------------------------------------------------------------------------------------------
class NodeModelsCompilationInfo
{
public:
  PredictionModelAnimSetGroupInfo* findAnimSetGroupEntry(const char* modelName);

  PredictionModelAnimSetGroupInfo* addAnimSetGroupEntry(
    const char* modelName,
    AnalysisAnimSetDependencyMap* animSetDependencyMap,
    AnimSetGroupPredictionModels* animSetPredictionModels);

  /// \brief Free the compiled prediction model anim set group data
  void release();

  /// \brief Calculates the memory required for storing the compiled contents of the anim set groups
  /// stored on this nodeModels entry.
  NMP::Memory::Format getAnimSetGroupsMemoryRequirements() const;

  /// \brief Compile the contents of the anim set groups stored on this nodeModels entry.
  void initAnimSetGroups(
    NMP::Memory::Resource& resource,
    MR::PredictionNodeModels* nodeModelsEntry,
    AP::AssetProcessor* processor) const;

public:
  MR::NodeID                                  m_nodeID;             ///< The node that this is a list of prediction models for.
  std::list<PredictionModelAnimSetGroupInfo*> m_modelAnimSetGroups; ///< Grouped anim set specific incarnations of
                                                                    ///<  a prediction model, indexed by name.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::NetworkPredictionDefBuilder
/// \brief Utility class used by the asset compiler to construct a NetworkPredictionDef asset.
/// \see MR::NetworkPredictionDef
//----------------------------------------------------------------------------------------------------------------------
class NetworkPredictionDefBuilder
{
public:
  /// \brief 
  NMP::Memory::Resource init(
    AP::AssetProcessor*         processor,    ///< 
    const ME::NetworkDefExport* netDefExport, ///< Source DetworkDef asset.
    MR::NetworkDef*             netDef);      ///< Processed NetworkDef associated with this NetworkPredictionDef.

protected:
  /// \brief Builds all prediction models from analyses marked up on a node (does not dislocate them).
  void buildNodePredictionModelDefs(
    MR::NodeID                  nodeID,
    const ME::NetworkDefExport* netDefExport,
    AssetProcessor*             processor,
    MR::NetworkDef*             netDef);

  /// \brief
  static void extractModelNameFromAnalysisNodeName(
    const ME::AnalysisNodeExport* analysisNode,
    std::string&                  predictModelFullPathName,
    std::string&                  predictModelName);

  /// \brief Try and find an entry for the specified NodeID.
  /// \return NULL if not found.
  NodeModelsCompilationInfo* findNodeModelsEntry(MR::NodeID nodeID);

  NodeModelsCompilationInfo* addNodeModelsEntry(MR::NodeID nodeID);

  void release();

  /// \brief Calculates the memory requirements for a fully compiled NetworkPredictionDef using the contents of the
  ///  vector tables sm_animSetGroups and sm_modelNodeIDTable.
  ///
  /// Should only be called after buildPredictionModels.
  NMP::Memory::Format getPredictionDefMemoryRequirements() const;

  /// \brief Compile the contents of the final asset NodeModels.
  MR::NetworkPredictionDef* initPredictionDef(
    NMP::Memory::Resource& resource,
    AP::AssetProcessor* processor) const;

protected:
  std::list<NodeModelsCompilationInfo> m_nodeModels;
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NETWORK_PREDICTION_DEF_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
