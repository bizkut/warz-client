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
#include "physics/PhysX2/mrPhysicsDriverDataPhysX2.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
//----------------------------------------------------------------------------------------------------------------------
bool locateDriverDataPhysX2(PhysicsRigDef* physicsRigDef)
{
  for (uint32_t i = 0; i != physicsRigDef->m_numMaterials; ++i)
  {
    MR::PhysicsMaterialDriverDataPhysX2* driverData =
      (MR::PhysicsMaterialDriverDataPhysX2*)physicsRigDef->m_materials[i].driverData;
    driverData->locate();
  }

  for (int32_t i = 0; i != physicsRigDef->m_numParts; ++i)
  {
    MR::PhysicsBodyDriverDataPhysX2* bodyDriverData =
      (MR::PhysicsBodyDriverDataPhysX2*)physicsRigDef->m_parts[i].body.driverData;
    bodyDriverData->locate();

    for (int32_t j = 0; j != physicsRigDef->m_parts[i].volume.numSpheres; ++j)
    {
      MR::PhysicsShapeDriverDataPhysX2* driverData =
        (MR::PhysicsShapeDriverDataPhysX2*)physicsRigDef->m_parts[i].volume.spheres[j].driverData;
      driverData->locate();
    }

    for (int32_t j = 0; j != physicsRigDef->m_parts[i].volume.numBoxes; ++j)
    {
      MR::PhysicsShapeDriverDataPhysX2* driverData =
        (MR::PhysicsShapeDriverDataPhysX2*)physicsRigDef->m_parts[i].volume.boxes[j].driverData;
      driverData->locate();
    }

    for (int32_t j = 0; j != physicsRigDef->m_parts[i].volume.numCapsules; ++j)
    {
      MR::PhysicsShapeDriverDataPhysX2* driverData =
        (MR::PhysicsShapeDriverDataPhysX2*)physicsRigDef->m_parts[i].volume.capsules[j].driverData;
      driverData->locate();
    }
  }

  for (int32_t i = 0; i != physicsRigDef->m_numJoints; ++i)
  {
    NMP_ASSERT(physicsRigDef->m_joints[i]->m_jointType == MR::PhysicsJointDef::JOINT_TYPE_SIX_DOF);
    MR::PhysicsSixDOFJointDef* jointDef = (MR::PhysicsSixDOFJointDef*)physicsRigDef->m_joints[i];

    MR::PhysicsJointDriverDataPhysX2* driverData =
      (MR::PhysicsJointDriverDataPhysX2*)jointDef->m_driverData;
    driverData->locate();
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool dislocateDriverDataPhysX2(PhysicsRigDef* physicsRigDef)
{
  for (uint32_t i = 0; i != physicsRigDef->m_numMaterials; ++i)
  {
    MR::PhysicsMaterialDriverDataPhysX2* driverData =
      (MR::PhysicsMaterialDriverDataPhysX2*)physicsRigDef->m_materials[i].driverData;
    driverData->dislocate();
  }

  for (int32_t i = 0; i != physicsRigDef->m_numParts; ++i)
  {
    MR::PhysicsBodyDriverDataPhysX2* bodyDriverData =
      (MR::PhysicsBodyDriverDataPhysX2*)physicsRigDef->m_parts[i].body.driverData;
    bodyDriverData->dislocate();

    for (int32_t j = 0; j != physicsRigDef->m_parts[i].volume.numSpheres; ++j)
    {
      MR::PhysicsShapeDriverDataPhysX2* driverData =
        (MR::PhysicsShapeDriverDataPhysX2*)physicsRigDef->m_parts[i].volume.spheres[j].driverData;
      driverData->dislocate();
    }

    for (int32_t j = 0; j != physicsRigDef->m_parts[i].volume.numBoxes; ++j)
    {
      MR::PhysicsShapeDriverDataPhysX2* driverData =
        (MR::PhysicsShapeDriverDataPhysX2*)physicsRigDef->m_parts[i].volume.boxes[j].driverData;
      driverData->dislocate();
    }

    for (int32_t j = 0; j != physicsRigDef->m_parts[i].volume.numCapsules; ++j)
    {
      MR::PhysicsShapeDriverDataPhysX2* driverData =
        (MR::PhysicsShapeDriverDataPhysX2*)physicsRigDef->m_parts[i].volume.capsules[j].driverData;
      driverData->dislocate();
    }
  }

  for (int32_t i = 0; i != physicsRigDef->m_numJoints; ++i)
  {
    NMP_ASSERT(physicsRigDef->m_joints[i]->m_jointType == MR::PhysicsJointDef::JOINT_TYPE_SIX_DOF);
    MR::PhysicsSixDOFJointDef* jointDef = (MR::PhysicsSixDOFJointDef*)physicsRigDef->m_joints[i];

    MR::PhysicsJointDriverDataPhysX2* driverData =
      (MR::PhysicsJointDriverDataPhysX2*)jointDef->m_driverData;
    driverData->dislocate();
  }

  return true;
}

} // namespace MR
