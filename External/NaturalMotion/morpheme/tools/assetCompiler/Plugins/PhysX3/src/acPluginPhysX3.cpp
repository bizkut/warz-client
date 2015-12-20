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
#include "acPluginPhysX3.h"
#include "physics/PhysX3/mrPhysicsDriverDataPhysX3.h"
#include "physics/PhysX3/mrPhysicsRigPhysX3.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{
//----------------------------------------------------------------------------------------------------------------------
  inline physx::PxCombineMode::Enum convertExportCombineModeToPhysX3(const char* exportCombineMode)
{
  if (strcmp(exportCombineMode, "Average") == 0)
  {
    return physx::PxCombineMode::eAVERAGE;
  }
  else if (strcmp(exportCombineMode, "Min") == 0)
  {
    return physx::PxCombineMode::eMIN;
  }
  else if (strcmp(exportCombineMode, "Multiply") == 0)
  {
    return physx::PxCombineMode::eMULTIPLY;
  }
  else if (strcmp(exportCombineMode, "Max") == 0)
  {
    return physx::PxCombineMode::eMAX;
  }
  else
  {
    NMP_VERIFY_FAIL("encountered unknown combine mode when creating PhysicsRigDef: %s", exportCombineMode);
    return physx::PxCombineMode::ePAD_32;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// PhysX3DriverBuilder
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format PhysX3DriverBuilder::getMaterialMemoryRequirements(uint32_t numMaterials)
{
  NMP::Memory::Format requirements(sizeof(MR::PhysicsMaterialDriverDataPhysX3) * numMaterials, NMP_NATURAL_TYPE_ALIGNMENT);
  return requirements;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format PhysX3DriverBuilder::getShapeMemoryRequirements(
  ME::PhysicsShapeExport::ShapeType NMP_UNUSED(type),
  uint32_t numShapes)
{
  NMP::Memory::Format requirements(sizeof(MR::PhysicsShapeDriverDataPhysX3) * numShapes, NMP_NATURAL_TYPE_ALIGNMENT);
  return requirements;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format PhysX3DriverBuilder::getPartMemoryRequirements(const ME::PhysicsPartExport* NMP_UNUSED(part))
{
  NMP::Memory::Format requirements(sizeof(MR::PhysicsActorDriverDataPhysX3), NMP_NATURAL_TYPE_ALIGNMENT);
  requirements += NMP::Memory::Format(sizeof(MR::PhysicsBodyDriverDataPhysX3), NMP_NATURAL_TYPE_ALIGNMENT);
  return requirements;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format PhysX3DriverBuilder::getJointMemoryRequirements(const ME::PhysicsJointExport* NMP_UNUSED(joint))
{
  NMP::Memory::Format requirements(sizeof(MR::PhysicsJointDriverDataPhysX3), NMP_NATURAL_TYPE_ALIGNMENT);
  return requirements;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysX3DriverBuilder::initMaterialDriverData(
  const ME::PhysicsShapeExport*       physicsMaterialExport,
  MR::PhysicsRigDef::Part::Material&  material,
  NMP::Memory::Resource&              resource)
{
  NMP::Memory::Format format(sizeof(MR::PhysicsMaterialDriverDataPhysX3), NMP_NATURAL_TYPE_ALIGNMENT);
  MR::PhysicsMaterialDriverDataPhysX3* driverData = (MR::PhysicsMaterialDriverDataPhysX3*)resource.alignAndIncrement(format);

  const ME::AttributeBlockExport* attributeBlock = physicsMaterialExport->getAttributeBlock();

  double dynamicFriction = 0;
  bool result = attributeBlock->getDoubleAttribute("dynamicFriction", dynamicFriction);
  NMP_VERIFY_MSG(result, "could not find attribute 'dynamicFriction' in exported physics data");
  driverData->m_dynamicFriction = (float)dynamicFriction;

  const char* frictionCombineMode = 0;
  result = attributeBlock->getStringAttribute("frictionCombineMode", frictionCombineMode);
  NMP_VERIFY_MSG(result, "could not find attribute 'frictionCombineMode' in exported physics data");
  driverData->m_frictionCombineMode = convertExportCombineModeToPhysX3(frictionCombineMode);

  const char* restitutionCombineMode = 0;
  result = attributeBlock->getStringAttribute("restitutionCombineMode", restitutionCombineMode);
  NMP_VERIFY_MSG(result, "could not find attribute 'restitutionCombineMode' in exported physics data");
  driverData->m_restitutionCombineMode = convertExportCombineModeToPhysX3(restitutionCombineMode);

  bool disableStrongFriction = false;
  result = attributeBlock->getBoolAttribute("disableStrongFriction", disableStrongFriction);
  NMP_VERIFY_MSG(result, "could not find attribute 'disableStrongFriction' in exported physics data");
  driverData->m_disableStrongFriction = disableStrongFriction;

  material.driverData = driverData;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysX3DriverBuilder::initShapeDriverData(
  const ME::PhysicsShapeExport*           physicsShapeExport,
  MR::PhysicsRigDef::Part::Volume::Shape& shape,
  NMP::Memory::Resource&                  resource)
{
  NMP::Memory::Format format(sizeof(MR::PhysicsShapeDriverDataPhysX3), NMP_NATURAL_TYPE_ALIGNMENT);
  MR::PhysicsShapeDriverDataPhysX3* driverData = (MR::PhysicsShapeDriverDataPhysX3*)resource.alignAndIncrement(format);

  const ME::AttributeBlockExport* attributeBlock = physicsShapeExport->getAttributeBlock();
  double contactOffset = 0.0f;
  bool result = attributeBlock->getDoubleAttribute("contactOffset", contactOffset);
  NMP_VERIFY_MSG(result, "could not find attribute 'contactOffset' in exported physics data");
  driverData->m_contactOffset = (float)contactOffset;

  double restOffset = 0.0f;
  result = attributeBlock->getDoubleAttribute("restOffset", restOffset);
  NMP_VERIFY_MSG(result, "could not find attribute 'restOffset' in exported physics data");
  driverData->m_restOffset = (float)restOffset;

  shape.driverData = driverData;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysX3DriverBuilder::initPartDriverData(
  const ME::PhysicsPartExport*  physicsPartExport,
  MR::PhysicsRigDef::Part&      part,
  NMP::Memory::Resource&        resource)
{
  NMP::Memory::Format actorFormat(sizeof(MR::PhysicsActorDriverDataPhysX3), NMP_NATURAL_TYPE_ALIGNMENT);
  MR::PhysicsActorDriverDataPhysX3* actorDriverData = (MR::PhysicsActorDriverDataPhysX3*)resource.alignAndIncrement(actorFormat);

  const ME::AttributeBlockExport* attributeBlock = physicsPartExport->getAttributeBlock();

  double maxContactOffsetIncrease = 0.0f;
  bool result = attributeBlock->getDoubleAttribute("maxContactOffsetIncrease", maxContactOffsetIncrease);
  NMP_VERIFY_MSG(result, "could not find attribute 'maxContactOffsetIncrease' in exported physics data");
  actorDriverData->m_maxContactOffsetIncrease = (float)maxContactOffsetIncrease;

  part.actor.driverData = actorDriverData;

  NMP::Memory::Format bodyFormat(sizeof(MR::PhysicsBodyDriverDataPhysX3), NMP_NATURAL_TYPE_ALIGNMENT);
  MR::PhysicsBodyDriverDataPhysX3* bodyDriverData = (MR::PhysicsBodyDriverDataPhysX3*)resource.alignAndIncrement(bodyFormat);

  double maxAngularVelocity = -1.0f;
  result = attributeBlock->getDoubleAttribute("maxAngularVelocity", maxAngularVelocity);
  NMP_VERIFY_MSG(result, "could not find attribute 'maxAngularVelocity' in exported physics data");
  bodyDriverData->m_maxAngularVelocity = (float)maxAngularVelocity;

  double inertiaSphericalisation = -1.0f;
  result = attributeBlock->getDoubleAttribute("inertiaSphericalisation", inertiaSphericalisation);
  NMP_VERIFY_MSG(result, "could not find attribute 'inertiaSphericalisation' in exported physics data");
  bodyDriverData->m_inertiaSphericalisation = (float)inertiaSphericalisation;

  uint32_t positionSolverIterationCount = 4;
  result = attributeBlock->getUIntAttribute("positionSolverIterationCount", positionSolverIterationCount);
  NMP_VERIFY_MSG(result, "could not find attribute 'positionSolverIterationCount' in exported physics data");
  bodyDriverData->m_positionSolverIterationCount = positionSolverIterationCount;

  uint32_t velocitySolverIterationCount = 1;
  result = attributeBlock->getUIntAttribute("velocitySolverIterationCount", velocitySolverIterationCount);
  NMP_VERIFY_MSG(result, "could not find attribute 'velocitySolverIterationCount' in exported physics data");
  bodyDriverData->m_velocitySolverIterationCount = velocitySolverIterationCount;

  part.body.driverData = bodyDriverData;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysX3DriverBuilder::initJointDriverData(
  const ME::PhysicsJointExport* physicsJointExport,
  MR::PhysicsJointDef*          jointDef,
  NMP::Memory::Resource&        resource)
{
  NMP::Memory::Format format(sizeof(MR::PhysicsJointDriverDataPhysX3), NMP_NATURAL_TYPE_ALIGNMENT);
  MR::PhysicsJointDriverDataPhysX3* driverData = (MR::PhysicsJointDriverDataPhysX3*)resource.alignAndIncrement(format);

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

  double articulationDamping = 0.0;
  result = attributeBlock->getDoubleAttribute("articulationDamping", articulationDamping);
  NMP_VERIFY_MSG(result, "could not find attribute 'articulationDamping' in exported physics data");
  driverData->m_articulationDamping = (float)articulationDamping;

  double articulationSpring = 0.0;
  result = attributeBlock->getDoubleAttribute("articulationSpring", articulationSpring);
  NMP_VERIFY_MSG(result, "could not find attribute 'articulationSpring' in exported physics data");
  driverData->m_articulationSpring = (float)articulationSpring;

  bool useAccelerationSprings = true;
  result = attributeBlock->getBoolAttribute("useAccelerationSprings", useAccelerationSprings);
  NMP_VERIFY_MSG(result, "could not find attribute 'useAccelerationSprings' in exported physics data");
  driverData->m_useAccelerationSprings = (useAccelerationSprings ? 1 : 0);

  double driveStrengthScale = 1.0;
  result = attributeBlock->getDoubleAttribute("driveStrengthScale", driveStrengthScale);
  NMP_VERIFY_MSG(result, "could not find attribute 'driveStrengthScale' in exported physics data");
  driverData->m_driveStrengthScale = (float)driveStrengthScale;

  double driveDampingScale = 1.0;
  result = attributeBlock->getDoubleAttribute("driveDampingScale", driveDampingScale);
  NMP_VERIFY_MSG(result, "could not find attribute 'driveDampingScale' in exported physics data");
  driverData->m_driveDampingScale = (float)driveDampingScale;

  double driveMinDampingScale = 1.0;
  result = attributeBlock->getDoubleAttribute("driveMinDampingScale", driveMinDampingScale);
  NMP_VERIFY_MSG(result, "could not find attribute 'driveMinDampingScale' in exported physics data");
  driverData->m_driveMinDampingScale = (float)driveMinDampingScale;

  double driveCompensation = 1.0;
  result = attributeBlock->getDoubleAttribute("driveCompensation", driveCompensation);
  NMP_VERIFY_MSG(result, "could not find attribute 'driveCompensation' in exported physics data");
  driverData->m_driveCompensationScale = (float)driveCompensation;

  MR::PhysicsSixDOFJointDef* sixDOFJointDef = (MR::PhysicsSixDOFJointDef*)jointDef;
  sixDOFJointDef->m_driverData = driverData;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysX3DriverBuilder::dislocateDriverData(MR::PhysicsRigDef* physicsRigDef)
{
  MR::dislocateDriverDataPhysX3(physicsRigDef);
}

PhysX3DriverBuilder* g_driverBuilder = NULL;

#ifndef NMP_MEMORY_LOGGING
//----------------------------------------------------------------------------------------------------------------------
void loadPlugin(MR::Manager* manager, const NMP::Memory::Config* c)
{
  NMP_VERIFY_MSG(!g_driverBuilder, "PhysX3 asset compiler plugin has already been initialised.");

  manager->registerAsset(MR::Manager::kAsset_PhysicsRigDef, MR::locatePhysicsRigDefPhysX3);

  loadPluginPhysics(manager, c);

  g_driverBuilder = new PhysX3DriverBuilder();
  registerPhysicsDriverBuilder(g_driverBuilder);
}
#else
//----------------------------------------------------------------------------------------------------------------------
void loadPluginWithLogging(MR::Manager* manager, const NMP::Memory::Config* c NMP_MEMORY_LOGGING_DECL)
{
  NMP_VERIFY_MSG(!g_driverBuilder, "PhysX3 asset compiler plugin has already been initialised.");

  manager->registerAsset(MR::Manager::kAsset_PhysicsRigDef, MR::locatePhysicsRigDefPhysX3);

  loadPluginPhysics(manager, c NMP_MEMORY_LOGGING_PASS_THROUGH);

  g_driverBuilder = new PhysX3DriverBuilder();
  registerPhysicsDriverBuilder(g_driverBuilder);
}
#endif
//----------------------------------------------------------------------------------------------------------------------
void finalisePluginInitialisation()
{
  finalisePluginInitialisationPhysics();
}

//----------------------------------------------------------------------------------------------------------------------
void registerPluginWithProcessor(
  AssetProcessor*  assetProc,
  MR::Manager*     manager,
  uint32_t         numDispatchers,
  MR::Dispatcher** dispatchers)
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
