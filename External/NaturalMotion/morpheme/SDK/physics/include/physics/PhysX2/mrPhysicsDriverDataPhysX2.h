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
#ifndef MR_PHYSICS_DRIVER_DATA_PHYSX2_H
#define MR_PHYSICS_DRIVER_DATA_PHYSX2_H
//----------------------------------------------------------------------------------------------------------------------
#include "physics/mrPhysicsRigDef.h"
#include "physics/PhysX2/mrPhysX2.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
//----------------------------------------------------------------------------------------------------------------------
// PhysicsShapeDriverDataPhysX2
//----------------------------------------------------------------------------------------------------------------------
struct PhysicsShapeDriverDataPhysX2 : public PhysicsDriverData
{
  void locate();
  void dislocate();

  float m_skinWidth;
};

//----------------------------------------------------------------------------------------------------------------------
// PhysicsMaterialDriverDataPhysX2
//----------------------------------------------------------------------------------------------------------------------
struct PhysicsMaterialDriverDataPhysX2 : public PhysicsDriverData
{
  void locate();
  void dislocate();

  float         m_dynamicFriction;
  NxCombineMode m_frictionCombineMode;
  NxCombineMode m_restitutionCombineMode;
  uint32_t      m_disableStrongFriction;
};


//----------------------------------------------------------------------------------------------------------------------
// PhysicsBodyDriverDataPhysX2
//----------------------------------------------------------------------------------------------------------------------
struct PhysicsBodyDriverDataPhysX2 : public PhysicsDriverData
{
  void locate();
  void dislocate();

  float   m_maxAngularVelocity;
  int32_t m_solverIterationCount;
  float   m_sleepEnergyThreshold;
  bool    m_usesSleepEnergyThreshold;
};

//----------------------------------------------------------------------------------------------------------------------
// PhysicsJointDriverDataPhysX2
//----------------------------------------------------------------------------------------------------------------------
struct PhysicsJointDriverDataPhysX2 : public PhysicsDriverData
{
  void locate();
  void dislocate();

  void endianSwap();

  float m_twistDriveDamping;
  float m_twistDriveSpring;

  float m_swingDriveDamping;
  float m_swingDriveSpring;

  float m_slerpDriveDamping;
  float m_slerpDriveSpring;

  uint32_t m_useSlerpDrive;
  uint32_t m_useAccelerationSprings;
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Locates all PhysX2 driver data for a PhysicsRigDef, this must be called after PhysicsRigDef::locate.
//----------------------------------------------------------------------------------------------------------------------
bool locateDriverDataPhysX2(PhysicsRigDef* physicsRigDef);

//----------------------------------------------------------------------------------------------------------------------
/// \brief Dislocates all PhysX2 driver data for a PhysicsRigDef, this must be called before PhysicsRigDef::dislocate.
//----------------------------------------------------------------------------------------------------------------------
bool dislocateDriverDataPhysX2(PhysicsRigDef* physicsRigDef);

//----------------------------------------------------------------------------------------------------------------------
// Inline implementations
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// PhysicsShapeDriverDataPhysX2
//----------------------------------------------------------------------------------------------------------------------
inline void PhysicsShapeDriverDataPhysX2::locate()
{
  NMP::endianSwap(m_skinWidth);
}

//----------------------------------------------------------------------------------------------------------------------
inline void PhysicsShapeDriverDataPhysX2::dislocate()
{
  NMP::endianSwap(m_skinWidth);
}

//----------------------------------------------------------------------------------------------------------------------
// PhysicsMaterialDriverDataPhysX2
//----------------------------------------------------------------------------------------------------------------------
inline void PhysicsMaterialDriverDataPhysX2::locate()
{
  NMP::endianSwap(m_dynamicFriction);
  NMP::endianSwap(m_frictionCombineMode);
  NMP::endianSwap(m_restitutionCombineMode);
  NMP::endianSwap(m_disableStrongFriction);
}

//----------------------------------------------------------------------------------------------------------------------
inline void PhysicsMaterialDriverDataPhysX2::dislocate()
{
  NMP::endianSwap(m_dynamicFriction);
  NMP::endianSwap(m_frictionCombineMode);
  NMP::endianSwap(m_restitutionCombineMode);
  NMP::endianSwap(m_disableStrongFriction);
}

//----------------------------------------------------------------------------------------------------------------------
// PhysicsBodyDriverDataPhysX2
//----------------------------------------------------------------------------------------------------------------------
inline void PhysicsBodyDriverDataPhysX2::locate()
{
  NMP::endianSwap(m_maxAngularVelocity);
  NMP::endianSwap(m_solverIterationCount);
  NMP::endianSwap(m_sleepEnergyThreshold);
  NMP::endianSwap(m_usesSleepEnergyThreshold);
}

//----------------------------------------------------------------------------------------------------------------------
inline void PhysicsBodyDriverDataPhysX2::dislocate()
{
  NMP::endianSwap(m_maxAngularVelocity);
  NMP::endianSwap(m_solverIterationCount);
  NMP::endianSwap(m_sleepEnergyThreshold);
  NMP::endianSwap(m_usesSleepEnergyThreshold);
}

//----------------------------------------------------------------------------------------------------------------------
// PhysicsJointDriverDataPhysX2
//----------------------------------------------------------------------------------------------------------------------
inline void PhysicsJointDriverDataPhysX2::locate()
{
  endianSwap();
}

//----------------------------------------------------------------------------------------------------------------------
inline void PhysicsJointDriverDataPhysX2::dislocate()
{
  endianSwap();
}

//----------------------------------------------------------------------------------------------------------------------
inline void PhysicsJointDriverDataPhysX2::endianSwap()
{
  NMP::endianSwap(m_twistDriveDamping);
  NMP::endianSwap(m_twistDriveSpring);

  NMP::endianSwap(m_swingDriveDamping);
  NMP::endianSwap(m_swingDriveSpring);

  NMP::endianSwap(m_slerpDriveDamping);
  NMP::endianSwap(m_slerpDriveSpring);

  NMP::endianSwap(m_useSlerpDrive);
  NMP::endianSwap(m_useAccelerationSprings);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_PHYSICS_DRIVER_DATA_PHYSX2_H
//----------------------------------------------------------------------------------------------------------------------
