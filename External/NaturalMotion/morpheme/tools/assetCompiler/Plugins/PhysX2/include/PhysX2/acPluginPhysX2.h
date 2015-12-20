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
#ifndef AP_ACPLUGINPHYSX2_H
#define AP_ACPLUGINPHYSX2_H

#ifdef ACPLUGIN_PHYSX2_EXPORTS
  #define ACPLUGIN_PHYSX2_EXPORT __declspec(dllexport)
#else
  #define ACPLUGIN_PHYSX2_EXPORT __declspec(dllimport)
#endif

//----------------------------------------------------------------------------------------------------------------------
#include "acPhysicsCore.h"

//----------------------------------------------------------------------------------------------------------------------
/// \namespace AP
/// \brief
/// \see AssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
class PhysX2DriverBuilder : public PhysicsDriverDataBuilder
{
public:

  virtual NMP::Memory::Format getMaterialMemoryRequirements(uint32_t numMaterials) NM_OVERRIDE;

  virtual NMP::Memory::Format getShapeMemoryRequirements(
    ME::PhysicsShapeExport::ShapeType type,
    uint32_t numShapes) NM_OVERRIDE;
  virtual NMP::Memory::Format getPartMemoryRequirements(const ME::PhysicsPartExport* part) NM_OVERRIDE;

  virtual NMP::Memory::Format getJointMemoryRequirements(const ME::PhysicsJointExport* joint) NM_OVERRIDE;

  virtual void initMaterialDriverData(
    const ME::PhysicsShapeExport* physicsMaterialExport,
    MR::PhysicsRigDef::Part::Material& material,
    NMP::Memory::Resource& resource) NM_OVERRIDE;

  virtual void initShapeDriverData(
    const ME::PhysicsShapeExport* physicsShapeExport,
    MR::PhysicsRigDef::Part::Volume::Shape& shape,
    NMP::Memory::Resource& resource) NM_OVERRIDE;
  virtual void initPartDriverData(
    const ME::PhysicsPartExport* physicsPartExport,
    MR::PhysicsRigDef::Part& part,
    NMP::Memory::Resource& resource) NM_OVERRIDE;

  virtual void initJointDriverData(
    const ME::PhysicsJointExport* physicsJointExport,
    MR::PhysicsJointDef* joint,
    NMP::Memory::Resource& resource) NM_OVERRIDE;

  virtual void dislocateDriverData(MR::PhysicsRigDef* physicsRigDef) NM_OVERRIDE;
};

extern "C"
{

#ifndef NMP_MEMORY_LOGGING
//----------------------------------------------------------------------------------------------------------------------
NMP_DLL_EXPORT void loadPlugin(MR::Manager* manager, const NMP::Memory::Config* c);
#else
//----------------------------------------------------------------------------------------------------------------------
NMP_DLL_EXPORT void loadPluginWithLogging(MR::Manager* manager, const NMP::Memory::Config* c NMP_MEMORY_LOGGING_DECL);
#endif // NMP_MEMORY_LOGGING

//----------------------------------------------------------------------------------------------------------------------
NMP_DLL_EXPORT void finalisePluginInitialisation();

//----------------------------------------------------------------------------------------------------------------------
NMP_DLL_EXPORT void registerPluginWithProcessor(
  AssetProcessor*            assetProc,
  MR::Manager*               manager,
  uint32_t                   numDispatchers,
  MR::Dispatcher**           dispatchers);

//----------------------------------------------------------------------------------------------------------------------
NMP_DLL_EXPORT void destroyPluginData();

//----------------------------------------------------------------------------------------------------------------------
NMP_DLL_EXPORT void unloadPlugin();

} // extern "C"

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // AP_ACPLUGINPHYSX2_H
//----------------------------------------------------------------------------------------------------------------------
