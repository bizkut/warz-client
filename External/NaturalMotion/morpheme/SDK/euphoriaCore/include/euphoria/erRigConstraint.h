// Copyright (c) 2013 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#ifndef NM_RIGCONSTRAINT_H
#define NM_RIGCONSTRAINT_H

#include "erDebugDraw.h"
#include "mrPhysX3.h"
#include "mrPhysicsRigPhysX3Articulation.h"
#include "mrPhysX3Includes.h"
#include "erLimbTransforms.h"
#include "erEuphoriaUserData.h"
#include "mrPhysicsScenePhysX3.h"
#include "NMPlatform/NMVectorContainer.h"

namespace ER
{

/* 
  * Rig Constraint
  * 
  * Manages a physical constraint between two rig parts. Client code must call the keep() fn every frame or the 
  * constraint will be automatically destroyed. This is done to ensure constraints don't continue to exist after the 
  * creating behaviour has been deactivated.
  */
class RigConstraint
{
  friend class RigConstraintManager;

public:
  RigConstraint(
    const uint32_t partIndexA,
    const NMP::Matrix34& partJointFrameA,
    const uint32_t partIndexB,
    const NMP::Matrix34& partJointFrameB,
    MR::PhysicsRigPhysX3Articulation* const physicsRig);

  ~RigConstraint();

  void setLinearDof(const uint32_t axis, const bool locked);
  void setAngularDof(const uint32_t axis, const bool locked);
  void setLocalPoseForPartA(const NMP::Matrix34& pose);
  void setLocalPoseForPartB(const NMP::Matrix34& pose);
  void setDrive(const float spring, const float damping, const float forceLimit, const bool isAcceleration);
  void setDrive(const physx::PxD6JointDrive& drive);
  void setDrive(const physx::PxD6Drive::Enum axis, const physx::PxD6JointDrive& drive);

  /// Returns true if the constraint involves the rig part with the supplied index.
  bool referencesPart(const uint32_t partIndex) const;

  /// The constraint will automatically be destroyed on the first frame that this fn is not called.
  void keep() { m_hasExpired = false; }

private:
  // Private member functions
  physx::PxD6Joint* createJoint(
    physx::PxRigidActor* actor1,
    physx::PxRigidActor* actor2,
    const NMP::Matrix34& pose1,
    const NMP::Matrix34& pose2,
    uint16_t lockedLinearDofs,
    uint16_t lockedAngularDofs);

  // Private member data
  physx::PxD6Joint* m_constraint; // The actual underlying PhysX constraint.

  uint32_t m_partIndexA;
  uint32_t m_partIndexB;

  bool m_hasExpired;
};

  /*
  * Rig Constraint Collection
  * 
  * Store a set of rig constraints and allow lookup by either of the constrained part indexes.
  */
class RigConstraintManager
{
public:
  static NMP::Memory::Format getMemoryRequirements(uint32_t maxEntries);
  static RigConstraintManager* init(NMP::Memory::Resource& resource, const uint32_t maxEntries);

  RigConstraint* create(
    const uint32_t partIndexA,
    const NMP::Matrix34& partJointFrameA,
    const uint32_t partIndexB,
    const NMP::Matrix34& partJointFrameB,
    MR::PhysicsRigPhysX3Articulation* const physicsRig);

  void destroy(const uint32_t partIndexA, const uint32_t partIndexB);

  void update();

  /// Return first constraint involving the supplied physics part, NULL if no constraint exists.
  RigConstraint* find(const uint32_t partIndex);
  RigConstraint* find(const uint32_t partIndexA, const uint32_t partIndexB);

  void clear();

private:

  RigConstraintManager();

  void destroy(RigConstraint* const constraint);

  struct LookUpNode
  {
    LookUpNode()
      : m_partIndex(0)
      , m_constraint(NULL)
    {}

    LookUpNode(const uint32_t partIndex, RigConstraint* const constraint)
      : m_partIndex(partIndex)
      , m_constraint(constraint)
    {}

    uint32_t       m_partIndex;
    RigConstraint* m_constraint;
  };

  // Returns iterator to first map element with supplied part index appearing after 'start' in the map.
  NMP::VectorContainer<LookUpNode>::iterator findNext(
    const uint32_t partIndex,
    NMP::VectorContainer<LookUpNode>::iterator start);

  void removeConstraintFromMap(const RigConstraint* const constraint);

  NMP::VectorContainer<LookUpNode>*     m_partToConstraintLookup;
  NMP::VectorContainer<RigConstraint*>* m_constraints;
};

}  // namespace ER

#endif // NM_RIGCONSTRAINT_H