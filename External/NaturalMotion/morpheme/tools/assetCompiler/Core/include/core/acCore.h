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
#ifndef AP_ACCORE_H
#define AP_ACCORE_H
//----------------------------------------------------------------------------------------------------------------------

#ifdef AC_CORE_EXPORTS
  #define AC_CORE_EXPORT __declspec(dllexport)
#else
  #define AC_CORE_EXPORT __declspec(dllimport)
#endif

#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMBasicLogger.h"

#include "export/mcExport.h"
#include "export/mcExportXml.h"
#include "export/mcExportPhysics.h"
#include "export/mcExportBody.h"

#include "assetProcessor/AssetProcessor.h"

#include "morpheme/mrRig.h"
#include "morpheme/mrRigToAnimMap.h"
#include "morpheme/mrManager.h"

#include "morpheme/mrDefines.h"

#include "morpheme/TransitConditions/mrTransitCondition.h"
#include "morpheme/mrTransitDeadBlend.h"

#include <list>
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \namespace AP
/// \brief
/// \see AssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

class NodeDefBuilder;
class TransitConditionDefBuilder;
class TransitDeadBlendDefBuilder;

//----------------------------------------------------------------------------------------------------------------------
class ACPluginCore : public AssetCompilerPlugin
{
public:
  ACPluginCore(AP::AssetProcessor* assetProc, MR::Manager* manager);
  ~ACPluginCore();

  const ProcessedAsset* processAsset(
    ME::AssetExport* const sourceAsset,  ///< Source connect asset structure.
    MR::Manager::AssetType assetTypeToGenerate = MR::Manager::kAsset_Invalid ///< Indicates to the processor what kind of asset it is trying to create
                                                                             ///<  from the source asset data. For example we create a NetworkDef and 
                                                                             ///<  a NetworkPredictionDef from the same source data.
    ) NM_OVERRIDE;

  bool dislocateAsset(ProcessedAsset* asset) NM_OVERRIDE;

  /// \brief called to prepare all data needed for animation compilation given the provided sourceAssets.
  bool prepareAnimationMetaData(const std::vector<ME::AssetExport*>& sourceAssets, AnimationMetadata** metadata) NM_OVERRIDE;

  /// \brief Create a runtime NetworkDef from the source network def data exported from Connect.
  NMP::Memory::Resource createNetworkDef(ME::NetworkDefExport* netDefExport);

  /// \brief Create a runtime NetworkPredictionDef from the source network def data exported from Connect.
  NMP::Memory::Resource createNetworkPredictionDef(ME::NetworkDefExport* netDefExport);

  /// \brief Create a runtime Rig from the source rig data exported from Connect.
  NMP::Memory::Resource createRig(ME::RigExport* sourceRig);

  /// \brief Create a runtime Character Controller from the source controller data exported from Connect.
  NMP::Memory::Resource createCharacterController(ME::CharacterControllerExport* characterControllerExport);

  /// \brief Create a runtime DiscreteEventTrack from the source data exported from Connect.
  NMP::Memory::Resource createEventTrackDiscrete(ME::EventTrackExport* eventTrackExport);

  /// \brief Create a runtime DurationEventTrack from the source data exported from Connect.
  NMP::Memory::Resource createEventTrackDuration(ME::EventTrackExport* eventTrackExport);

  /// \brief Create a runtime CurveEventTrack from the source data exported from Connect.
  NMP::Memory::Resource createEventTrackCurve(ME::EventTrackExport* eventTrackExport);

  /// \brief Process everything that the networkDef depends on
  bool processNetworkDefDependencies(ME::NetworkDefExport* netDefExport);

  /// \brief
  bool processAnimLibrary(ME::AnimationLibraryExport* animLibraryExport);

  /// \brief Process the analysis library, outputting to a result library indicated in the Export asset
  bool processAnalysisLibrary(ME::AnalysisLibraryExport* analysisLibrary);
};

//----------------------------------------------------------------------------------------------------------------------
AC_CORE_EXPORT void loadPluginCore(const NMP::Memory::Config* config NMP_MEMORY_LOGGING_DECL);

//----------------------------------------------------------------------------------------------------------------------
AC_CORE_EXPORT AssetCompilerPlugin* initPluginCore(
  AssetProcessor*  assetProc,
  MR::Manager*     manager,
  uint32_t         numDispatchers,
  MR::Dispatcher** dispatchers);

//----------------------------------------------------------------------------------------------------------------------
AC_CORE_EXPORT void deinitialisePluginCore();

//----------------------------------------------------------------------------------------------------------------------
AC_CORE_EXPORT void unloadPluginCore();

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // AP_ACCORE_H
//----------------------------------------------------------------------------------------------------------------------

