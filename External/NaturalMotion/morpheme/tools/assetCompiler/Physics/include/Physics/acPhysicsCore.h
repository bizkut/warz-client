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
#ifndef AP_ACCOREPHYSICS_H
#define AP_ACCOREPHYSICS_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMBasicLogger.h"

#include "assetProcessor/AssetProcessor.h"

#include "physics/mrPhysicsRigDef.h"
#include "physics/mrPhysicsAttribData.h"
#include "morpheme/mrManager.h"
#include "morpheme/mrDefines.h"
#include "NetworkDefBuilder.h"

#ifdef AC_PHYSICS_CORE_EXPORTS
#define AC_PHYSICS_CORE_EXPORT __declspec(dllexport)
#else
#define AC_PHYSICS_CORE_EXPORT __declspec(dllimport)
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \namespace AP
/// \brief
/// \see AssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
class PhysicsNetworkBuilderPlugin : public NetworkDefBuilderPlugin
{
public:
  void populateNetDefSemanticLookupTable(
    const ME::NetworkDefExport*      netDefExport,                     ///< 
    MR::SemanticLookupTable*         netDefSemanticTable) NM_OVERRIDE; ///< 
    
  NMP::Memory::Format getPluginMemoryRequirements(
    const ME::NetworkDefExport*      netDefExport) NM_OVERRIDE;

  bool initPluginData(
    NMP::Memory::Resource&           memRes,                    ///< Where plugin data is to be initialised.
    MR::AttribDataHandle*            netDefAttribDataArray,     ///< The the array of AttribData of the NetworkDef itself.
    uint32_t                         netDefAttribDataArraySize,
    MR::SemanticLookupTable*         netDefSemanticLookupTable, ///< Lookup table for the NetworkDef itself.
    AP::AssetProcessor*              processor,
    const ME::NetworkDefExport*      netDefExport) NM_OVERRIDE;

  /// \brief Set up per animation set animation rig to physics rig mapping.
  static void buildAnimToPhysicsMapAttibData(
    NMP::Memory::Resource&           memRes,
    MR::AttribDataHandle*            netDefAttribDataArray,
    uint32_t                         NMP_USED_FOR_ASSERTS(netDefAttribDataArraySize),
    MR::SemanticLookupTable*         netDefSemanticLookupTable,
    const ME::AnimationSetExport*    animSetExport,
    MR::AnimRigDef*                  animRigDef,
    MR::AnimSetIndex                 animSetIndex);

  /// \brief Generate animation rig to physics rig mapping data and store in the provided mapping structure.
  static void populateAnimToPhysicsMap(
    MR::AnimToPhysicsMap*          animToPhysics,
    const ME::RigExport*           animRig,
    const ME::PhysicsRigDefExport* physicsRig);

  /// \brief Create AttribData to hold a pointer to the animation sets physics rig (may be a NULL pointer).
  static void buildPhysicsRigAttribData(
    NMP::Memory::Resource&           memRes,
    MR::AttribDataHandle*            netDefAttribDataArray,
    uint32_t                         NMP_USED_FOR_ASSERTS(netDefAttribDataArraySize),
    MR::SemanticLookupTable*         netDefSemanticLookupTable,
    AP::AssetProcessor*              processor,
    const ME::AnimationSetExport*    animSetExport,
    MR::AnimSetIndex                 animSetIndex);
};

//----------------------------------------------------------------------------------------------------------------------
class AC_PHYSICS_CORE_EXPORT PhysicsDriverDataBuilder
{
public:
  virtual ~PhysicsDriverDataBuilder();

  virtual NMP::Memory::Format getMaterialMemoryRequirements(uint32_t numMaterials) = 0;

  virtual NMP::Memory::Format getShapeMemoryRequirements(ME::PhysicsShapeExport::ShapeType type, uint32_t numShapes) = 0;
  virtual NMP::Memory::Format getPartMemoryRequirements(const ME::PhysicsPartExport* part) = 0;

  virtual NMP::Memory::Format getJointMemoryRequirements(const ME::PhysicsJointExport* joint) = 0;

  virtual void initMaterialDriverData(
    const ME::PhysicsShapeExport* physicsMaterialExport,
    MR::PhysicsRigDef::Part::Material& material,
    NMP::Memory::Resource& resource) = 0;

  virtual void initShapeDriverData(
    const ME::PhysicsShapeExport* physicsShapeExport,
    MR::PhysicsRigDef::Part::Volume::Shape& shape,
    NMP::Memory::Resource& resource) = 0;
  virtual void initPartDriverData(
    const ME::PhysicsPartExport* physicsPartExport,
    MR::PhysicsRigDef::Part& part,
    NMP::Memory::Resource& resource) = 0;

  virtual void initJointDriverData(
    const ME::PhysicsJointExport* physicsJointExport,
    MR::PhysicsJointDef* joint,
    NMP::Memory::Resource& resource) = 0;

  /// \brief This will be called before MR::PhysicsRigDef::dislocate.
  virtual void dislocateDriverData(MR::PhysicsRigDef* physicsRigDef) = 0;
};

//----------------------------------------------------------------------------------------------------------------------
class ACPluginPhysics : public AssetCompilerPlugin
{
public:
  ACPluginPhysics(AP::AssetProcessor* assetProc, MR::Manager* manager);
  ~ACPluginPhysics();

  const ProcessedAsset* processAsset(
    ME::AssetExport* const sourceAsset,  ///< Source connect asset structure.
    MR::Manager::AssetType assetTypeToGenerate = MR::Manager::kAsset_Invalid ///< Indicates to the processor what kind of asset it is trying to create
                                                                             ///<  from the source asset data. For example we create a NetworkDef and 
                                                                             ///<  a NetworkPredictionDef from the same source data.
    ) NM_OVERRIDE;

  bool dislocateAsset(ProcessedAsset* asset);
  
  bool prepareAnimationMetaData(
    const std::vector<ME::AssetExport*>& sourceAssets,
    AnimationMetadata** metadata) NM_OVERRIDE;

  /// \brief Create a runtime physics rig def from the source rig data exported from Connect.
  NMP::Memory::Resource createPhysicsRigDef(ME::PhysicsRigDefExport* sourceRig);

  static bool registerPhysicsDriverBuilder(PhysicsDriverDataBuilder* plugin);

private:
  static PhysicsDriverDataBuilder* sm_physicsDriverBuilder;
};

//----------------------------------------------------------------------------------------------------------------------
AC_PHYSICS_CORE_EXPORT bool registerPhysicsDriverBuilder(PhysicsDriverDataBuilder* plugin);

//----------------------------------------------------------------------------------------------------------------------
AC_PHYSICS_CORE_EXPORT void loadPluginPhysics(
  MR::Manager*               manager,
  const NMP::Memory::Config* c
  NMP_MEMORY_LOGGING_DECL);

//----------------------------------------------------------------------------------------------------------------------
AC_PHYSICS_CORE_EXPORT void finalisePluginInitialisationPhysics();

//----------------------------------------------------------------------------------------------------------------------
AC_PHYSICS_CORE_EXPORT void registerPluginWithProcessorPhysics(
  AssetProcessor*  assetProc,
  MR::Manager*     manager,
  uint32_t         numDispatchers,
  MR::Dispatcher** dispatchers);

//----------------------------------------------------------------------------------------------------------------------
AC_PHYSICS_CORE_EXPORT void deinitialisePluginPhysics();

//----------------------------------------------------------------------------------------------------------------------
AC_PHYSICS_CORE_EXPORT void unloadPluginPhysics();

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // AP_ACCOREPHYSICS_H
//----------------------------------------------------------------------------------------------------------------------
