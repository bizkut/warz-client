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
#include "acPluginPhysX2.h"
#include "physics/PhysX2/mrPhysicsDriverDataPhysX2.h"
#include "physics/PhysX2/mrPhysicsRigPhysX2.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{
//----------------------------------------------------------------------------------------------------------------------
inline NxCombineMode convertExportCombineModeToPhysX2(const char* exportCombineMode)
{
  if (strcmp(exportCombineMode, "Average") == 0)
  {
    return NX_CM_AVERAGE;
  }
  else if (strcmp(exportCombineMode, "Min") == 0)
  {
    return NX_CM_MIN;
  }
  else if (strcmp(exportCombineMode, "Multiply") == 0)
  {
    return NX_CM_MULTIPLY;
  }
  else if (strcmp(exportCombineMode, "Max") == 0)
  {
    return NX_CM_MAX;
  }
  else
  {
    NMP_VERIFY_FAIL("encountered unknown combine mode when creating PhysicsRigDef: %s", exportCombineMode);
    return NX_CM_PAD_32;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// PhysX2DriverBuilder
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format PhysX2DriverBuilder::getMaterialMemoryRequirements(uint32_t numMaterials)
{
  NMP::Memory::Format requirements(sizeof(MR::PhysicsMaterialDriverDataPhysX2) * numMaterials, NMP_NATURAL_TYPE_ALIGNMENT);
  return requirements;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format PhysX2DriverBuilder::getShapeMemoryRequirements(
  ME::PhysicsShapeExport::ShapeType NMP_UNUSED(type),
  uint32_t numShapes)
{
  NMP::Memory::Format requirements(sizeof(MR::PhysicsShapeDriverDataPhysX2) * numShapes, NMP_NATURAL_TYPE_ALIGNMENT);
  return requirements;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format PhysX2DriverBuilder::getPartMemoryRequirements(const ME::PhysicsPartExport* NMP_UNUSED(part))
{
  NMP::Memory::Format requirements(sizeof(MR::PhysicsBodyDriverDataPhysX2), NMP_NATURAL_TYPE_ALIGNMENT);
  return requirements;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format PhysX2DriverBuilder::getJointMemoryRequirements(const ME::PhysicsJointExport* NMP_UNUSED(joint))
{
  NMP::Memory::Format requirements(sizeof(MR::PhysicsJointDriverDataPhysX2), NMP_NATURAL_TYPE_ALIGNMENT);
  return requirements;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysX2DriverBuilder::initMaterialDriverData(
  const ME::PhysicsShapeExport* physicsMaterialExport,
  MR::PhysicsRigDef::Part::Material& material,
  NMP::Memory::Resource& resource)
{
  NMP::Memory::Format format(sizeof(MR::PhysicsMaterialDriverDataPhysX2), NMP_NATURAL_TYPE_ALIGNMENT);
  MR::PhysicsMaterialDriverDataPhysX2* driverData = (MR::PhysicsMaterialDriverDataPhysX2*)resource.alignAndIncrement(format);

  const ME::AttributeBlockExport* attributeBlock = physicsMaterialExport->getAttributeBlock();

  double dynamicFriction = 0;
  bool result = attributeBlock->getDoubleAttribute("dynamicFriction", dynamicFriction);
  NMP_VERIFY_MSG(result, "could not find attribute 'dynamicFriction' in exported physics data");
  driverData->m_dynamicFriction = (float)dynamicFriction;

  const char* frictionCombineMode = 0;
  result = attributeBlock->getStringAttribute("frictionCombineMode", frictionCombineMode);
  NMP_VERIFY_MSG(result, "could not find attribute 'frictionCombineMode' in exported physics data");
  driverData->m_frictionCombineMode = convertExportCombineModeToPhysX2(frictionCombineMode);

  const char* restitutionCombineMode = 0;
  result = attributeBlock->getStringAttribute("restitutionCombineMode", restitutionCombineMode);
  NMP_VERIFY_MSG(result, "could not find attribute 'restitutionCombineMode' in exported physics data");
  driverData->m_restitutionCombineMode = convertExportCombineModeToPhysX2(restitutionCombineMode);

  bool disableStrongFriction = false;
  result = attributeBlock->getBoolAttribute("disableStrongFriction", disableStrongFriction);
  NMP_VERIFY_MSG(result, "could not find attribute 'disableStrongFriction' in exported physics data");
  driverData->m_disableStrongFriction = disableStrongFriction;

  material.driverData = driverData;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysX2DriverBuilder::initShapeDriverData(
  const ME::PhysicsShapeExport* physicsShapeExport,
  MR::PhysicsRigDef::Part::Volume::Shape& shape,
  NMP::Memory::Resource& resource)
{
  NMP::Memory::Format format(sizeof(MR::PhysicsShapeDriverDataPhysX2), NMP_NATURAL_TYPE_ALIGNMENT);
  MR::PhysicsShapeDriverDataPhysX2* driverData = (MR::PhysicsShapeDriverDataPhysX2*)resource.alignAndIncrement(format);

  const ME::AttributeBlockExport* attributeBlock = physicsShapeExport->getAttributeBlock();
  double skinWidth = 0.0f;
  bool result = attributeBlock->getDoubleAttribute("skinwidth", skinWidth);
  NMP_VERIFY_MSG(result, "could not find attribute 'skinwidth' in exported physics data");
  driverData->m_skinWidth = (float)skinWidth;

  shape.driverData = driverData;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysX2DriverBuilder::initPartDriverData(
  const ME::PhysicsPartExport* physicsPartExport,
  MR::PhysicsRigDef::Part& part,
  NMP::Memory::Resource& resource)
{
  // no additional PhysX2 data for actor
  part.actor.driverData = NULL;

  NMP::Memory::Format bodyFormat(sizeof(MR::PhysicsBodyDriverDataPhysX2), NMP_NATURAL_TYPE_ALIGNMENT);
  MR::PhysicsBodyDriverDataPhysX2* bodyDriverData = (MR::PhysicsBodyDriverDataPhysX2*)resource.alignAndIncrement(bodyFormat);

  const ME::AttributeBlockExport* attributeBlock = physicsPartExport->getAttributeBlock();

  double maxAngularVelocity = -1.0;
  bool result = attributeBlock->getDoubleAttribute("maxAngularVelocity", maxAngularVelocity);
  NMP_VERIFY_MSG(result, "could not find attribute 'maxAngularVelocity' in exported physics data");
  bodyDriverData->m_maxAngularVelocity = (float)maxAngularVelocity;

  uint32_t solverIterationCount = 4;
  result = attributeBlock->getUIntAttribute("solverIterationCount", solverIterationCount);
  NMP_VERIFY_MSG(result, "could not find attribute 'solverIterationCount' in exported physics data");
  bodyDriverData->m_solverIterationCount = (uint32_t)solverIterationCount;

  double sleepEnergyThreshold = 0.0;
  result = attributeBlock->getDoubleAttribute("sleepEnergyThreshold", sleepEnergyThreshold);
  NMP_VERIFY_MSG(result, "could not find attribute 'sleepEnergyThreshold' in exported physics data");
  bodyDriverData->m_sleepEnergyThreshold = (float)sleepEnergyThreshold;

  bool usesSleepEnergyThreshold = false;
  result = attributeBlock->getBoolAttribute("usesSleepEnergyThreshold", usesSleepEnergyThreshold);
  NMP_VERIFY_MSG(result, "could not find attribute 'usesSleepEnergyThreshold' in exported physics data");
  bodyDriverData->m_usesSleepEnergyThreshold = usesSleepEnergyThreshold;

  part.body.driverData = bodyDriverData;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysX2DriverBuilder::initJointDriverData(
  const ME::PhysicsJointExport* physicsJointExport,
  MR::PhysicsJointDef*          jointDef,
  NMP::Memory::Resource&        resource)
{
  NMP::Memory::Format format(sizeof(MR::PhysicsJointDriverDataPhysX2), NMP_NATURAL_TYPE_ALIGNMENT);
  MR::PhysicsJointDriverDataPhysX2* driverData = (MR::PhysicsJointDriverDataPhysX2*)resource.alignAndIncrement(format);

  const ME::AttributeBlockExport* attributeBlock = physicsJointExport->getAttributeBlock();

  bool useSlerpDrive = true;
  bool result = attributeBlock->getBoolAttribute("useSlerpDrive", useSlerpDrive);
  NMP_VERIFY_MSG(result, "could not find attribute 'useSlerpDrive' in exported physics data");
  driverData->m_useSlerpDrive = (useSlerpDrive ? 1 : 0);

  double twistDriveDamping = 0.0;
  result = attributeBlock->getDoubleAttribute("twistDriveDamping", twistDriveDamping);
  NMP_VERIFY_MSG(result, "could not find attribute 'twistDriveDamping' in exported physics data");
  driverData->m_twistDriveDamping = (float)twistDriveDamping;

  double twistDriveSpring = 0.0;
  result = attributeBlock->getDoubleAttribute("twistDriveSpring", twistDriveSpring);
  NMP_VERIFY_MSG(result, "could not find attribute 'twistDriveSpring' in exported physics data");
  driverData->m_twistDriveSpring = (float)twistDriveSpring;

  double swingDriveDamping = 0.0;
  result = attributeBlock->getDoubleAttribute("swingDriveDamping", swingDriveDamping);
  NMP_VERIFY_MSG(result, "could not find attribute 'swingDriveDamping' in exported physics data");
  driverData->m_swingDriveDamping = (float)swingDriveDamping;

  double swingDriveSpring = 0.0;
  result = attributeBlock->getDoubleAttribute("swingDriveSpring", swingDriveSpring);
  NMP_VERIFY_MSG(result, "could not find attribute 'swingDriveSpring' in exported physics data");
  driverData->m_swingDriveSpring = (float)twistDriveSpring;

  double slerpDriveDamping = 0.0;
  result = attributeBlock->getDoubleAttribute("slerpDriveDamping", slerpDriveDamping);
  NMP_VERIFY_MSG(result, "could not find attribute 'slerpDriveDamping' in exported physics data");
  driverData->m_slerpDriveDamping = (float)slerpDriveDamping;

  double slerpDriveSpring = 0.0;
  result = attributeBlock->getDoubleAttribute("slerpDriveSpring", slerpDriveSpring);
  NMP_VERIFY_MSG(result, "could not find attribute 'slerpDriveSpring' in exported physics data");
  driverData->m_slerpDriveSpring = (float)slerpDriveSpring;

  bool useAccelerationSprings = true;
  result = attributeBlock->getBoolAttribute("useAccelerationSprings", useAccelerationSprings);
  NMP_VERIFY_MSG(result, "could not find attribute 'useAccelerationSprings' in exported physics data");
  driverData->m_useAccelerationSprings = (useAccelerationSprings ? 1 : 0);

  MR::PhysicsSixDOFJointDef* sixDOFJointDef = (MR::PhysicsSixDOFJointDef*)jointDef;
  sixDOFJointDef->m_driverData = driverData;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysX2DriverBuilder::dislocateDriverData(MR::PhysicsRigDef* physicsRigDef)
{
  MR::dislocateDriverDataPhysX2(physicsRigDef);
}

PhysX2DriverBuilder* g_driverBuilder = NULL;

#ifndef NMP_MEMORY_LOGGING
//----------------------------------------------------------------------------------------------------------------------
void loadPlugin(MR::Manager* manager, const NMP::Memory::Config* c)
{
  NMP_VERIFY_MSG(!g_driverBuilder, "PhysX2 asset compiler plugin has already been initialised.");

  manager->registerAsset(MR::Manager::kAsset_PhysicsRigDef, MR::locatePhysicsRigDefPhysX2);

  loadPluginPhysics(manager, c);

  g_driverBuilder = new PhysX2DriverBuilder();
  registerPhysicsDriverBuilder(g_driverBuilder);
}
#else
//----------------------------------------------------------------------------------------------------------------------
void loadPluginWithLogging(MR::Manager* manager, const NMP::Memory::Config* c NMP_MEMORY_LOGGING_DECL)
{
  NMP_VERIFY_MSG(!g_driverBuilder, "PhysX2 asset compiler plugin has already been initialised.");

  manager->registerAsset(MR::Manager::kAsset_PhysicsRigDef, MR::locatePhysicsRigDefPhysX2);

  loadPluginPhysics(manager, c NMP_MEMORY_LOGGING_PASS_THROUGH);

  g_driverBuilder = new PhysX2DriverBuilder();
  registerPhysicsDriverBuilder(g_driverBuilder);
}
#endif//NMP_MEMORY_LOGGING

//----------------------------------------------------------------------------------------------------------------------
void finalisePluginInitialisation()
{
  finalisePluginInitialisationPhysics();
}

//----------------------------------------------------------------------------------------------------------------------
void registerPluginWithProcessor(
  AssetProcessor*            assetProc,
  MR::Manager*               manager,
  uint32_t                   numDispatchers,
  MR::Dispatcher**           dispatchers)
{
  registerPluginWithProcessorPhysics(assetProc, manager, numDispatchers, dispatchers);
}

//----------------------------------------------------------------------------------------------------------------------
void destroyPluginData()
{
  if (g_driverBuilder)
  {
    delete g_driverBuilder;
    g_driverBuilder = 0;
  }

  deinitialisePluginPhysics();
}

//----------------------------------------------------------------------------------------------------------------------
void unloadPlugin()
{
  unloadPluginPhysics();
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
