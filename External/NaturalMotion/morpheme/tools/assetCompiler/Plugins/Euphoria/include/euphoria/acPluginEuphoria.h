// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
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
#ifndef AP_ACPLUGINEUPHORIA_H
#define AP_ACPLUGINEUPHORIA_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMBasicLogger.h"
#include "assetProcessor/AssetProcessor.h"
#include "morpheme/mrRig.h"
#include "physics/mrPhysicsRigDef.h"
#include "morpheme/mrManager.h"
#include "morpheme/mrDefines.h"
#include "NetworkDefBuilder.h"

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
class EuphoriaNetworkBuilderPlugin : public NetworkDefBuilderPlugin
{
public:
  void populateNetDefSemanticLookupTable(
    const ME::NetworkDefExport*      netDefExport,                     ///<
    MR::SemanticLookupTable*         netDefSemanticTable) NM_OVERRIDE; ///<

  NMP::Memory::Format getPluginMemoryRequirements(const ME::NetworkDefExport* netDefExport) NM_OVERRIDE;

  const ME::BodyDefExport* buildBodyDefAttribData(
    NMP::Memory::Resource&           memRes,
    MR::AttribDataHandle*            netDefAttribDataArray,
    uint32_t                         NMP_USED_FOR_ASSERTS(netDefAttribDataArraySize),
    MR::SemanticLookupTable*         netDefSemanticLookupTable,
    const ME::AnimationSetExport*    animSetExport,
    AP::AssetProcessor*              processor,
    MR::AnimSetIndex                 animSetIndex);

  const ME::InteractionProxyExport* buildInteractionProxyDefAttribData(
    NMP::Memory::Resource&           memRes,
    MR::AttribDataHandle*            netDefAttribDataArray,
    uint32_t                         NMP_USED_FOR_ASSERTS(netDefAttribDataArraySize),
    MR::SemanticLookupTable*         netDefSemanticLookupTable,
    const ME::AnimationSetExport*    animSetExport,
    AP::AssetProcessor*              processor,
    MR::AnimSetIndex                 animSetIndex);

  /// \brief Create AttribData to hold animation poses that this BodyDef may use.
  void buildAnimPoseAttribData(
    NMP::Memory::Resource&           memRes,
    MR::AttribDataHandle*            netDefAttribDataArray,
    uint32_t                         NMP_USED_FOR_ASSERTS(netDefAttribDataArraySize),
    MR::SemanticLookupTable*         netDefSemanticLookupTable,
    const ME::AnimationSetExport*    animSetExport,
    const ME::BodyDefExport*         bodyDefExport,
    AP::AssetProcessor*              processor,
    MR::AnimSetIndex                 animSetIndex);

  bool initPluginData(
    NMP::Memory::Resource&           memRes,                    ///< Where plugin data is to be initialised.
    MR::AttribDataHandle*            netDefAttribDataArray,     ///< The the array of AttribData of the NetworkDef itself.
    uint32_t                         netDefAttribDataArraySize,
    MR::SemanticLookupTable*         netDefSemanticLookupTable, ///< Lookup table for the NetworkDef itself.
    AP::AssetProcessor*              processor,
    const ME::NetworkDefExport*      netDefExport) NM_OVERRIDE;
};

//----------------------------------------------------------------------------------------------------------------------
class ACPluginEuphoria : public AssetCompilerPlugin
{
public:
  ACPluginEuphoria(AP::AssetProcessor* assetProc, MR::Manager* manager);
  ~ACPluginEuphoria();

  const ProcessedAsset* processAsset(
    ME::AssetExport* const sourceAsset,  ///< Source connect asset structure.
    MR::Manager::AssetType assetTypeToGenerate = MR::Manager::kAsset_Invalid ///< Indicates to the processor what kind of asset it is trying to create
                                                                             ///<  from the source asset data. For example we create a NetworkDef and 
                                                                             ///<  a NetworkPredictionDef from the same source data.
    ) NM_OVERRIDE;
  
  bool dislocateAsset(ProcessedAsset* asset) NM_OVERRIDE;

  bool prepareAnimationMetaData(const std::vector<ME::AssetExport*>& sourceAssets, AnimationMetadata** metadata) NM_OVERRIDE;

  /// \brief Create a runtime body def from the source body data exported from Connect.
  NMP::Memory::Resource createBodyDef(ME::BodyDefExport* sourceBody, uint32_t numBones);

  /// \brief Create a runtime Interaction Proxy from the source proxy data exported from Connect.
  NMP::Memory::Resource createInteractionProxy(ME::InteractionProxyExport* interactionProxyExport);
};

//----------------------------------------------------------------------------------------------------------------------
extern "C"
{
#ifndef NMP_MEMORY_LOGGING
//----------------------------------------------------------------------------------------------------------------------
NMP_DLL_EXPORT void loadPlugin(MR::Manager* manager, const NMP::Memory::Config* c);
#else
NMP_DLL_EXPORT void loadPluginWithLogging(MR::Manager* manager, const NMP::Memory::Config* c NMP_MEMORY_LOGGING_DECL);
#endif // NMP_MEMORY_LOGGING

//----------------------------------------------------------------------------------------------------------------------
NMP_DLL_EXPORT void registerPluginWithProcessor(
  AssetProcessor*  assetProc,
  MR::Manager*     manager,
  uint32_t         numDispatchers,
  MR::Dispatcher** dispatchers);

//----------------------------------------------------------------------------------------------------------------------
NMP_DLL_EXPORT void finalisePluginInitialisation();

//----------------------------------------------------------------------------------------------------------------------
NMP_DLL_EXPORT void destroyPluginData();

//----------------------------------------------------------------------------------------------------------------------
NMP_DLL_EXPORT void unloadPlugin();

} // extern "C"

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // AP_ACPLUGINEUPHORIA_H
//----------------------------------------------------------------------------------------------------------------------
