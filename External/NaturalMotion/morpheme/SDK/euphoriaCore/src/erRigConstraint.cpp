// Copyright (c) 2013 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "euphoria/erRigConstraint.h"

namespace ER
{
//----------------------------------------------------------------------------------------------------------------------
RigConstraint::RigConstraint(
  const uint32_t partIndexA,
  const NMP::Matrix34& partJointFrameA,
  const uint32_t partIndexB,
  const NMP::Matrix34& partJointFrameB,
  MR::PhysicsRigPhysX3Articulation* const physicsRig)
:m_constraint(NULL)
,m_partIndexA(partIndexA)
,m_partIndexB(partIndexB)
,m_hasExpired(false)
{
  NMP_ASSERT(physicsRig);

  uint16_t lockedLinearDofs  = 1 | 1<<1 | 1<<2; // All dofs locked.
  uint16_t lockedAngularDofs = 1 | 1<<1 | 1<<2; // All dofs locked.

  if (physicsRig)
  {
    physx::PxRigidActor* const partActorA = physicsRig->getPartPhysX3Articulation(m_partIndexA)->getArticulationLink();
    physx::PxRigidActor* const partActorB = physicsRig->getPartPhysX3Articulation(m_partIndexB)->getArticulationLink();

    NMP_ASSERT(partActorA && partActorB);

    if (partActorA && partActorB)
    {
      m_constraint =
        createJoint(partActorA, partActorB, partJointFrameA, partJointFrameB, lockedLinearDofs, lockedAngularDofs);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
RigConstraint::~RigConstraint()
{
  if (m_constraint)
  {
    m_constraint->release();
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool RigConstraint::referencesPart(const uint32_t partIndex) const
{
  return (partIndex == m_partIndexA) || (partIndex == m_partIndexB);
}

//----------------------------------------------------------------------------------------------------------------------
void RigConstraint::setLinearDof(const uint32_t axis, const bool locked)
{
  const physx::PxD6Axis::Enum pxAxis = static_cast<physx::PxD6Axis::Enum>(physx::PxD6Axis::eX + axis);
  const physx::PxD6Motion::Enum pxMotionType = locked ? physx::PxD6Motion::eLOCKED : physx::PxD6Motion::eFREE;

  m_constraint->setMotion(pxAxis, pxMotionType);
}

//----------------------------------------------------------------------------------------------------------------------
void RigConstraint::setAngularDof(const uint32_t axis, const bool locked)
{
  const physx::PxD6Axis::Enum pxAxis = static_cast<physx::PxD6Axis::Enum>(physx::PxD6Axis::eTWIST + axis);
  const physx::PxD6Motion::Enum pxMotionType = locked ? physx::PxD6Motion::eLOCKED : physx::PxD6Motion::eFREE;

  m_constraint->setMotion(pxAxis, pxMotionType);
}

//----------------------------------------------------------------------------------------------------------------------
void RigConstraint::setLocalPoseForPartA(const NMP::Matrix34& pose)
{
  m_constraint->setLocalPose(physx::PxJointActorIndex::eACTOR0, MR::nmMatrix34ToPxTransform(pose));
}

//----------------------------------------------------------------------------------------------------------------------
void RigConstraint::setLocalPoseForPartB(const NMP::Matrix34& pose)
{
  m_constraint->setLocalPose(physx::PxJointActorIndex::eACTOR1, MR::nmMatrix34ToPxTransform(pose));
}

//----------------------------------------------------------------------------------------------------------------------
void RigConstraint::setDrive(
  const float spring, 
  const float damping, 
  const float forceLimit, 
  const bool isAcceleration)
{
  const physx::PxD6JointDrive drive(spring, damping, forceLimit, isAcceleration);

  setDrive(drive);
}

//----------------------------------------------------------------------------------------------------------------------
void RigConstraint::setDrive(const physx::PxD6JointDrive& drive)
{
  m_constraint->setDrive(physx::PxD6Drive::eX, drive);
  m_constraint->setDrive(physx::PxD6Drive::eY, drive);
  m_constraint->setDrive(physx::PxD6Drive::eZ, drive);
  m_constraint->setDrive(physx::PxD6Drive::eSLERP, drive);
}

//----------------------------------------------------------------------------------------------------------------------
void RigConstraint::setDrive(const physx::PxD6Drive::Enum axis, const physx::PxD6JointDrive& drive)
{
  m_constraint->setDrive(axis, drive);
}

//----------------------------------------------------------------------------------------------------------------------
physx::PxD6Joint* RigConstraint::createJoint(
  physx::PxRigidActor* actor1,
  physx::PxRigidActor* actor2,
  const NMP::Matrix34& pose1,
  const NMP::Matrix34& pose2,
  uint16_t lockedLinearDofs,
  uint16_t lockedAngularDofs)
{
  physx::PxD6Joint* joint = PxD6JointCreate(
    PxGetPhysics(),
    actor1,
    MR::nmMatrix34ToPxTransform(pose1),
    actor2,
    MR::nmMatrix34ToPxTransform(pose2));
  NMP_ASSERT(joint);

  if (joint)
  {
    for (int i = 0; i < 3; i++)
    {
      // Translational and rotational dofs.
      joint->setMotion((physx::PxD6Axis::Enum) (physx::PxD6Axis::eX + i), lockedLinearDofs & (1 << i) ? physx::PxD6Motion::eLOCKED : physx::PxD6Motion::eFREE);
      joint->setMotion((physx::PxD6Axis::Enum) (physx::PxD6Axis::eTWIST + i), lockedAngularDofs & (1 << i) ? physx::PxD6Motion::eLOCKED : physx::PxD6Motion::eFREE);
    }
  }

  return joint;
}

//----------------------------------------------------------------------------------------------------------------------
// class RigConstraintCollection
RigConstraintManager* RigConstraintManager::init(NMP::Memory::Resource& resource, const uint32_t maxEntries)
{
  NMP_ASSERT_MSG(maxEntries, "0-length RigConstraintCollection is not allowed");
  NMP_ASSERT_MSG(resource.ptr, "NULL resource passed into RigConstraintCollection::init");
  NMP_ASSERT_MSG(resource.format.size, "0-sized resource passed into RigConstraintCollection::init");
  NMP_ASSERT_MSG(resource.format.alignment, "0-aligned resource passed into RigConstraintCollection::init");

  resource.align(getMemoryRequirements(maxEntries));
  RigConstraintManager* result = reinterpret_cast<RigConstraintManager*>(resource.ptr);
  resource.increment(NMP::Memory::Format(sizeof(RigConstraintManager), NMP_VECTOR_ALIGNMENT));

  result->m_constraints         = NMP::VectorContainer<RigConstraint*>::init(resource, maxEntries);
  result->m_partToConstraintLookup = NMP::VectorContainer<LookUpNode>::init(resource, maxEntries * 2);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void RigConstraintManager::clear()
{
  m_partToConstraintLookup->clear();
  m_constraints->clear();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format RigConstraintManager::getMemoryRequirements(uint32_t maxEntries)
{
  NMP_ASSERT_MSG(maxEntries, "0-length RigConstraintCollection is not allowed");

  NMP::Memory::Format result(sizeof(RigConstraintManager), NMP_VECTOR_ALIGNMENT);

  // Reserve space for the freelist that will be used to store entries
  result += NMP::VectorContainer<RigConstraint*>::getMemoryRequirements(maxEntries);
  result += NMP::VectorContainer<LookUpNode>::getMemoryRequirements(maxEntries * 2);

  // Align to 16 bytes to allow the structure to be DMAd
  result.size = NMP::Memory::align(result.size, NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
RigConstraintManager::RigConstraintManager()
  : m_partToConstraintLookup(NULL)
  , m_constraints(NULL)
{}

//----------------------------------------------------------------------------------------------------------------------
RigConstraint* RigConstraintManager::create(
  const uint32_t partIndexA,
  const NMP::Matrix34& partJointFrameA,
  const uint32_t partIndexB,
  const NMP::Matrix34& partJointFrameB,
  MR::PhysicsRigPhysX3Articulation* const physicsRig)
{
  NMP_ASSERT(partIndexA != partIndexB);

  RigConstraint* const constraint =
    new RigConstraint(partIndexA, partJointFrameA, partIndexB, partJointFrameB, physicsRig);

  m_constraints->push_back(constraint);
  m_partToConstraintLookup->push_back(LookUpNode(partIndexA, constraint));
  m_partToConstraintLookup->push_back(LookUpNode(partIndexB, constraint));

  return constraint;
}

//----------------------------------------------------------------------------------------------------------------------
void RigConstraintManager::destroy(const uint32_t partIndexA, const uint32_t partIndexB)
{
  RigConstraint* const constraint = find(partIndexA, partIndexB);

  if (constraint)
  {
    delete constraint;

    removeConstraintFromMap(constraint);

    // erase constraint
    NMP::VectorContainer<RigConstraint*>::iterator constraintItr = m_constraints->begin();
    NMP::VectorContainer<RigConstraint*>::iterator constraintEnd = m_constraints->end();

    for (; constraintItr != constraintEnd; ++constraintItr)
    {
      if (*constraintItr == constraint)
      {
        constraintItr = m_constraints->erase(constraintItr);
        break;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void RigConstraintManager::update()
{
  // erase constraints that have expired
  NMP::VectorContainer<RigConstraint*>::iterator constraintItr = m_constraints->begin();
  NMP::VectorContainer<RigConstraint*>::iterator constraintEnd = m_constraints->end();

  while (constraintItr != constraintEnd)
  {
    if ((*constraintItr)->m_hasExpired)
    {
      delete *constraintItr;
      removeConstraintFromMap(*constraintItr);
      constraintItr = m_constraints->erase(constraintItr);
    }
    else
    {
      (*constraintItr)->m_hasExpired = true;
      ++constraintItr;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Return first constraint involving the supplied physics part, NULL if no constraint exists.
RigConstraint* RigConstraintManager::find(const uint32_t partIndex)
{
  NMP::VectorContainer<LookUpNode>::iterator result = findNext(partIndex, m_partToConstraintLookup->begin());

  return (result != m_partToConstraintLookup->end()) ? (*result).m_constraint : NULL;
}

RigConstraint* RigConstraintManager::find(const uint32_t partIndexA, const uint32_t partIndexB)
{
  NMP::VectorContainer<LookUpNode>::iterator mapItr = m_partToConstraintLookup->begin();
  NMP::VectorContainer<LookUpNode>::iterator mapEnd = m_partToConstraintLookup->end();

  do
  {
      mapItr = findNext(partIndexA, mapItr);
  }
  while((mapItr != mapEnd) && !((*mapItr).m_constraint->referencesPart(partIndexB)));

  return (mapItr != mapEnd) ? (*mapItr).m_constraint : NULL;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::VectorContainer<RigConstraintManager::LookUpNode>::iterator RigConstraintManager::findNext(
  const uint32_t partIndex,
  NMP::VectorContainer<LookUpNode>::iterator start)
{
  NMP::VectorContainer<LookUpNode>::iterator itr = start;
  NMP::VectorContainer<LookUpNode>::iterator end = m_partToConstraintLookup->end();

  while((itr != end) && ((*itr).m_partIndex != partIndex))
  {
    ++itr;
  }

  return itr;
}

//----------------------------------------------------------------------------------------------------------------------
void RigConstraintManager::removeConstraintFromMap(const RigConstraint* const constraint)
{
  // erase index to constraint mappings
  NMP::VectorContainer<LookUpNode>::iterator mapItr = m_partToConstraintLookup->begin();
  NMP::VectorContainer<LookUpNode>::iterator mapEnd = m_partToConstraintLookup->end();

  while (mapItr != mapEnd)
  {
    if ((*mapItr).m_constraint == constraint)
    {
      mapItr = m_partToConstraintLookup->erase(mapItr);
    }
    else
    {
      ++mapItr;
    }
  }
}
}
