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
#ifndef MR_PHYSICS_DRIVER_DATA_PHYSX3_H
#define MR_PHYSICS_DRIVER_DATA_PHYSX3_H
//----------------------------------------------------------------------------------------------------------------------
#include "physics/mrPhysicsRigDef.h"
#include "physics/PhysX3/mrPhysX3.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
//----------------------------------------------------------------------------------------------------------------------
// PhysicsShapeDriverDataPhysX3
//----------------------------------------------------------------------------------------------------------------------
struct PhysicsShapeDriverDataPhysX3 : public PhysicsDriverData
{
  void locate();
  void dislocate();

  float m_restOffset;
  float m_contactOffset;
};

//----------------------------------------------------------------------------------------------------------------------
// PhysicsMaterialDriverDataPhysX3
//----------------------------------------------------------------------------------------------------------------------
struct PhysicsMaterialDriverDataPhysX3 : public PhysicsDriverData
{
  void locate();
  void dislocate();

  float                       m_dynamicFriction;
  physx::PxCombineMode::Enum  m_frictionCombineMode;
  physx::PxCombineMode::Enum  m_restitutionCombineMode;
  uint32_t                    m_disableStrongFriction;
};

//----------------------------------------------------------------------------------------------------------------------
// PhysicsActorDriverDataPhysX3
//----------------------------------------------------------------------------------------------------------------------
struct PhysicsActorDriverDataPhysX3 : public PhysicsDriverData
{
  void locate();
  void dislocate();

  float m_maxContactOffsetIncrease;
};

//----------------------------------------------------------------------------------------------------------------------
// PhysicsBodyDriverDataPhysX3
//----------------------------------------------------------------------------------------------------------------------
struct PhysicsBodyDriverDataPhysX3 : public PhysicsDriverData
{
  void locate();
  void dislocate();

  float     m_maxAngularVelocity;
  float     m_inertiaSphericalisation;
  uint32_t  m_positionSolverIterationCount;
  uint32_t  m_velocitySolverIterationCount;
};

//----------------------------------------------------------------------------------------------------------------------
// PhysicsJointDriverDataPhysX3
//----------------------------------------------------------------------------------------------------------------------
struct PhysicsJointDriverDataPhysX3 : public PhysicsDriverData
{
  void locate();
  void dislocate();

  void endianSwap();

  // For the twist/swing drive when jointed
  float m_twistDriveDamping;
  float m_twistDriveSpring;
  float m_swingDriveDamping;
  float m_swingDriveSpring;

  // For the slerp drive when jointed
  float m_slerpDriveDamping;
  float m_slerpDriveSpring;

  // For the drive when articulated
  float m_articulationSpring;
  float m_articulationDamping;

  float m_driveStrengthScale;
  float m_driveDampingScale;
  float m_driveMinDampingScale;
  float m_driveCompensationScale;

  uint32_t m_useSlerpDrive;
  uint32_t m_useAccelerationSprings;
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Locates all PhysX3 driver data for a PhysicsRigDef, this must be called after PhysicsRigDef::locate.
//----------------------------------------------------------------------------------------------------------------------
bool locateDriverDataPhysX3(PhysicsRigDef* physicsRigDef);

//----------------------------------------------------------------------------------------------------------------------
/// \brief Dislocates all PhysX3 driver data for a PhysicsRigDef, this must be called before PhysicsRigDef::dislocate.
//----------------------------------------------------------------------------------------------------------------------
bool dislocateDriverDataPhysX3(PhysicsRigDef* physicsRigDef);

//----------------------------------------------------------------------------------------------------------------------
// Inline implementations
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// PhysicsShapeDriverDataPhysX3
//----------------------------------------------------------------------------------------------------------------------
inline void PhysicsShapeDriverDataPhysX3::locate()
{
  NMP::endianSwap(m_restOffset);
  NMP::endianSwap(m_contactOffset);
}

//----------------------------------------------------------------------------------------------------------------------
inline void PhysicsShapeDriverDataPhysX3::dislocate()
{
  NMP::endianSwap(m_restOffset);
  NMP::endianSwap(m_contactOffset);
}

//----------------------------------------------------------------------------------------------------------------------
// PhysicsMaterialDriverDataPhysX3
//----------------------------------------------------------------------------------------------------------------------
inline void PhysicsMaterialDriverDataPhysX3::locate()
{
  NMP::endianSwap(m_dynamicFriction);
  NMP::endianSwap(m_frictionCombineMode);
  NMP::endianSwap(m_restitutionCombineMode);
  NMP::endianSwap(m_disableStrongFriction);
}

//----------------------------------------------------------------------------------------------------------------------
inline void PhysicsMaterialDriverDataPhysX3::dislocate()
{
  NMP::endianSwap(m_dynamicFriction);
  NMP::endianSwap(m_frictionCombineMode);
  NMP::endianSwap(m_restitutionCombineMode);
  NMP::endianSwap(m_disableStrongFriction);
}

//----------------------------------------------------------------------------------------------------------------------
// PhysicsActorDriverDataPhysX3
//----------------------------------------------------------------------------------------------------------------------
inline void PhysicsActorDriverDataPhysX3::locate()
{
  NMP::endianSwap(m_maxContactOffsetIncrease);
}

//----------------------------------------------------------------------------------------------------------------------
inline void PhysicsActorDriverDataPhysX3::dislocate()
{
  NMP::endianSwap(m_maxContactOffsetIncrease);
}

//----------------------------------------------------------------------------------------------------------------------
// PhysicsBodyDriverDataPhysX3
//----------------------------------------------------------------------------------------------------------------------
inline void PhysicsBodyDriverDataPhysX3::locate()
{
  NMP::endianSwap(m_maxAngularVelocity);
  NMP::endianSwap(m_inertiaSphericalisation);
  NMP::endianSwap(m_positionSolverIterationCount);
  NMP::endianSwap(m_velocitySolverIterationCount);
}

//----------------------------------------------------------------------------------------------------------------------
inline void PhysicsBodyDriverDataPhysX3::dislocate()
{
  NMP::endianSwap(m_maxAngularVelocity);
  NMP::endianSwap(m_inertiaSphericalisation);
  NMP::endianSwap(m_positionSolverIterationCount);
  NMP::endianSwap(m_velocitySolverIterationCount);
}

//----------------------------------------------------------------------------------------------------------------------
// PhysicsJointDriverDataPhysX3
//----------------------------------------------------------------------------------------------------------------------
inline void PhysicsJointDriverDataPhysX3::locate()
{
  endianSwap();
}

//----------------------------------------------------------------------------------------------------------------------
inline void PhysicsJointDriverDataPhysX3::dislocate()
{
  endianSwap();
}

//----------------------------------------------------------------------------------------------------------------------
inline void PhysicsJointDriverDataPhysX3::endianSwap()
{
  NMP::endianSwap(m_twistDriveDamping);
  NMP::endianSwap(m_twistDriveSpring);

  NMP::endianSwap(m_swingDriveDamping);
  NMP::endianSwap(m_swingDriveSpring);

  NMP::endianSwap(m_slerpDriveDamping);
  NMP::endianSwap(m_slerpDriveSpring);

  NMP::endianSwap(m_articulationSpring);
  NMP::endianSwap(m_articulationDamping);

  NMP::endianSwap(m_driveStrengthScale);
  NMP::endianSwap(m_driveDampingScale);
  NMP::endianSwap(m_driveMinDampingScale);
  NMP::endianSwap(m_driveCompensationScale);

  NMP::endianSwap(m_useSlerpDrive);
  NMP::endianSwap(m_useAccelerationSprings);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_PHYSICS_DRIVER_DATA_PHYSX3_H
//----------------------------------------------------------------------------------------------------------------------
