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
#include <cstdio>
#include "NMPlatform/NMFastHeapAllocator.h"
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMProfiler.h"
#include "NMGeomUtils/NMGeomUtils.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/mrBlendOps.h"
#include "morpheme/mrRig.h"
#include "physics/mrPhysicsRigDef.h"
#include "physics/mrPhysicsSerialisationBuffer.h"
#include "physics/mrPhysicsAttribData.h"
#include "physics/PhysX3/mrPhysX3.h"
#include "physics/PhysX3/mrPhysicsDriverDataPhysX3.h"
#include "physics/PhysX3/mrPhysicsRigPhysX3Jointed.h"
#include "physics/PhysX3/mrPhysicsScenePhysX3.h"
#include "sharedDefines/mPhysicsDebugInterface.h"

//----------------------------------------------------------------------------------------------------------------------

#define DISABLE_JOINTSx

namespace MR
{

// this is like a limit "skin width" - small and rather arbitrary.
const float PhysicsRigPhysX3Jointed::s_limitContactAngle = NM_PI * 0.1f;

// This limit isn't nice, but PhysX can't handle 0 swing ranges. 
static const float s_minSwingLimit = NMP::degreesToRadians(0.01f);

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigPhysX3Jointed::PhysicsRigPhysX3Jointed(PhysicsScenePhysX3 *physicsScene)
{
  m_physicsScene = physicsScene;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format PhysicsRigPhysX3Jointed::getMemoryRequirements(PhysicsRigDef* physicsRigDef)
{
  uint32_t numBones = physicsRigDef->getNumParts();
  uint32_t numJoints = physicsRigDef->getNumJoints();
  uint32_t numMaterials = physicsRigDef->getNumMaterials();

  NMP::Memory::Format result(sizeof(PhysicsRigPhysX3Jointed), NMP_VECTOR_ALIGNMENT);

  // Space for the unique material pointers
  result += NMP::Memory::Format(sizeof(physx::PxMaterial *) * numMaterials, NMP_NATURAL_TYPE_ALIGNMENT);

  // Space for the part pointers
  result += NMP::Memory::Format(sizeof(PhysicsRigPhysX3Jointed::PartPhysX3Jointed*) * numBones, NMP_NATURAL_TYPE_ALIGNMENT);

  // Space for the joint pointers
  result += NMP::Memory::Format(sizeof(PhysicsRigPhysX3Jointed::JointPhysX3Jointed*) * numJoints, NMP_NATURAL_TYPE_ALIGNMENT);

  // Space for the parts
  result += NMP::Memory::Format(NMP::Memory::align(
    sizeof(PhysicsRigPhysX3Jointed::PartPhysX3Jointed), NMP_NATURAL_TYPE_ALIGNMENT) * numBones, NMP_NATURAL_TYPE_ALIGNMENT);

  // Space for the joints
  result += NMP::Memory::Format(NMP::Memory::align(
    sizeof(PhysicsRigPhysX3Jointed::JointPhysX3Jointed), NMP_NATURAL_TYPE_ALIGNMENT) * numJoints, NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigPhysX3Jointed* PhysicsRigPhysX3Jointed::init(
  NMP::Memory::Resource& resource,
  PhysicsRigDef*         physicsRigDef,
  PhysicsScene*          physicsScene,
  physx::PxClientID      ownerClientID,
  uint32_t               clientBehaviourBits,
  AnimRigDef*            animRigDef,
  AnimToPhysicsMap*      animToPhysicsMap,
  int32_t                collisionTypeMask,
  int32_t                collisionIgnoreMask)
{
  PhysicsRigPhysX3Jointed* result = (PhysicsRigPhysX3Jointed*)resource.ptr;
  resource.increment(sizeof(PhysicsRigPhysX3Jointed));

  new (result) PhysicsRigPhysX3Jointed((PhysicsScenePhysX3*) physicsScene);
  PhysicsRigPhysX3::init(result, PhysicsRigPhysX3::TYPE_JOINTED, ownerClientID, clientBehaviourBits);

  uint32_t numParts = physicsRigDef->getNumParts();
  uint32_t numJoints = physicsRigDef->getNumJoints();
  uint32_t numMaterials = physicsRigDef->getNumMaterials();

  // Unique material pointers
  resource.align(NMP::Memory::Format(sizeof(physx::PxMaterial*) * numMaterials, NMP_NATURAL_TYPE_ALIGNMENT));
  result->m_materials = (physx::PxMaterial**)resource.ptr;
  resource.increment(NMP::Memory::Format(sizeof(physx::PxMaterial*) * numMaterials, NMP_NATURAL_TYPE_ALIGNMENT));

  // Part pointers
  resource.align(NMP::Memory::Format(sizeof(PhysicsRig::Part*) * numParts, NMP_NATURAL_TYPE_ALIGNMENT));
  result->m_parts = (PhysicsRig::Part**)resource.ptr;
  resource.increment(NMP::Memory::Format(sizeof(PhysicsRig::Part*) * numParts, NMP_NATURAL_TYPE_ALIGNMENT));

  // Joint pointers
  resource.align(NMP::Memory::Format(sizeof(PhysicsRig::Joint*) * numJoints, NMP_NATURAL_TYPE_ALIGNMENT));
  result->m_joints = (PhysicsRig::Joint**)resource.ptr;
  resource.increment(NMP::Memory::Format(sizeof(PhysicsRig::Joint*) * numJoints, NMP_NATURAL_TYPE_ALIGNMENT));

  // Parts
  for (uint32_t i = 0; i < numParts; i++)
  {
    resource.align(NMP::Memory::Format(sizeof(PhysicsRigPhysX3Jointed::PartPhysX3Jointed), NMP_NATURAL_TYPE_ALIGNMENT));
    result->m_parts[i] = (PhysicsRigPhysX3Jointed::PartPhysX3Jointed*)resource.ptr;
    resource.increment(NMP::Memory::Format(sizeof(PhysicsRigPhysX3Jointed::PartPhysX3Jointed), NMP_NATURAL_TYPE_ALIGNMENT));
  }

  // Joints
  for (uint32_t i = 0; i < numJoints; i++)
  {
    resource.align(NMP::Memory::Format(sizeof(PhysicsRigPhysX3Jointed::JointPhysX3Jointed), NMP_NATURAL_TYPE_ALIGNMENT));
    result->m_joints[i] = (PhysicsRigPhysX3Jointed::JointPhysX3Jointed*)resource.ptr;
    resource.increment(NMP::Memory::Format(sizeof(PhysicsRigPhysX3Jointed::JointPhysX3Jointed), NMP_NATURAL_TYPE_ALIGNMENT));
  }

  result->m_animRigDef = animRigDef;
  result->m_animToPhysicsMap = animToPhysicsMap;
  result->m_physicsRigDef = physicsRigDef;

  result->m_collisionTypeMask = collisionTypeMask;
  result->m_collisionIgnoreMask = collisionIgnoreMask;

  // Fill m_materials in with the different materials in the physicsScene
  for (uint32_t i = 0; i < physicsRigDef->m_numMaterials; ++i)
  {
    result->m_materials[i] = createMaterial(physicsRigDef->m_materials[i]);
  }

  uint32_t totalNumShapes = 0;
  for (uint32_t iPart = 0 ; iPart < numParts ; ++iPart)
  {
    PhysicsRigDef::Part &part = physicsRigDef->m_parts[iPart];
    PhysicsRigDef::Part::Volume &volume = part.volume;
    uint32_t numShapes = volume.numSpheres + volume.numBoxes + volume.numCapsules;
    totalNumShapes += numShapes;
  }

  // Allocate memory for shape descriptors
  PxShapeDesc **shapeDescs = (PxShapeDesc **)NMPMemoryAlloc(
    sizeof(PxShapeDesc*) * totalNumShapes);
  NMP_ASSERT(shapeDescs);
  for (uint32_t iShape = 0 ; iShape < totalNumShapes ; ++iShape)
  {
    shapeDescs[iShape] = (PxShapeDesc *)NMPMemoryAlloc(
      sizeof(PxShapeDesc));
    NMP_ASSERT(shapeDescs[iShape]);
    (void) new (shapeDescs[iShape]) PxShapeDesc(PxGetPhysics().getTolerancesScale());
    shapeDescs[iShape]->name = "Jointed rig part";
  }

  // convert collision sets into a mask per part
  int *partGroupMasks = (int *)alloca(numParts * sizeof(int));
  int *allowedPartGroupMasks = (int *)alloca(numParts * sizeof(int));
  for (uint32_t i = 0; i<numParts; i++)
  {
    partGroupMasks[i] = 0;
    allowedPartGroupMasks[i] = 0;
  }
  for (int32_t i = 0; i<physicsRigDef->m_numCollisionGroups; i++)
  {
    if (physicsRigDef->m_collisionGroups[i].enabled)
    {
      for (int j = 0; j<physicsRigDef->m_collisionGroups[i].numIndices; j++)
      {
        // indices can't reference a part out of range
        NMP_ASSERT(physicsRigDef->m_collisionGroups[i].indices[j] < (int)numParts); 
        partGroupMasks[physicsRigDef->m_collisionGroups[i].indices[j]] |= 1<<i;
      }
    }

    for (int j = 0; j<physicsRigDef->m_collisionGroups[i].numIndices; j++)
    {
      // indices can't reference a part out of range
      NMP_ASSERT(physicsRigDef->m_collisionGroups[i].indices[j] < (int)numParts); 
      allowedPartGroupMasks[physicsRigDef->m_collisionGroups[i].indices[j]] |= 1<<i;
    }
  }

  // incremented as we work on each shape
  uint32_t iShape = 0;

  // Loop setting up each part
  for (uint32_t iPart = 0; iPart < numParts; ++iPart)
  {
    const PhysicsRigDef::Part& part = physicsRigDef->m_parts[iPart];
    const PhysicsRigDef::Part::Volume &volume = part.volume;

    const PhysicsBodyDriverDataPhysX3* bodyDriverData = (const PhysicsBodyDriverDataPhysX3*)part.body.driverData;

    // Actor
    PxRigidDynamicDesc rigidDynamicDesc(PxGetPhysics().getTolerancesScale());
    rigidDynamicDesc.ownerClient = result->m_ownerClientID;
    rigidDynamicDesc.name = part.name;
    rigidDynamicDesc.globalPose = nmMatrix34ToPxTransform(part.actor.globalPose);
    NMP_ASSERT(part.actor.hasCollision);
    NMP_ASSERT(!part.actor.isFixed);

    rigidDynamicDesc.angularDamping = part.body.angularDamping;
    rigidDynamicDesc.linearDamping = part.body.linearDamping;
    rigidDynamicDesc.maxAngularVelocity = bodyDriverData->m_maxAngularVelocity;
    if (rigidDynamicDesc.maxAngularVelocity < 0.0f)
      rigidDynamicDesc.maxAngularVelocity = 1e6;

    rigidDynamicDesc.mass = 1.0f; // gets overridden
    rigidDynamicDesc.massSpaceInertia = 
      physx::PxVec3(rigidDynamicDesc.mass,rigidDynamicDesc.mass,rigidDynamicDesc.mass);

    // This is the number iterations for resolving collisions
    rigidDynamicDesc.minPositionIterations = bodyDriverData->m_positionSolverIterationCount;
    // This is the number of iterations for resolving penetration
    rigidDynamicDesc.minVelocityIterations = bodyDriverData->m_velocitySolverIterationCount;

    uint32_t iFirstShapeForPart = iShape;
    uint32_t numShapes = volume.numBoxes + volume.numCapsules + volume.numSpheres;

    physx::PxFilterData filterData(
      result->m_collisionTypeMask, 
      result->m_collisionIgnoreMask, 
      result->getRigID(),
      partGroupMasks[iPart]);
    physx::PxShapeFlags flags = physx::PxShapeFlag::eSCENE_QUERY_SHAPE | physx::PxShapeFlag::eSIMULATION_SHAPE;
#ifdef USE_PHYSX_SWEEPS_FOR_CHARACTER
    flags |= physx::PxShapeFlag::eUSE_SWEPT_BOUNDS;
#endif
    // Volumes
    for (int32_t j = 0 ; j < volume.numBoxes ; ++j)
    {
      PhysicsShapeDriverDataPhysX3* shapeDriverData = (PhysicsShapeDriverDataPhysX3*)volume.boxes[j].driverData;

      // Get material from the list of different materials (m_materials)
      NMP_ASSERT(volume.boxes[j].materialID <= numMaterials);
      physx::PxMaterial* material = result->m_materials[volume.boxes[j].materialID];

      PxShapeDesc *shapeDesc = shapeDescs[iShape++];
      void *shapeMem = NMPMemoryAlloc(sizeof(physx::PxBoxGeometry));
      NMP_ASSERT(shapeMem);
      physx::PxBoxGeometry *box = new(shapeMem) physx::PxBoxGeometry(nmVector3ToPxVec3(volume.boxes[j].dimensions));
      shapeDesc->geometry = box; 
      shapeDesc->localPose = nmMatrix34ToPxTransform(volume.boxes[j].localPose);
      shapeDesc->contactOffset = shapeDriverData->m_contactOffset;
      shapeDesc->restOffset = shapeDriverData->m_restOffset;
      shapeDesc->setSingleMaterial(material);
      shapeDesc->userData = &volume.boxes[j].density;
    }

    for (int32_t j = 0 ; j < volume.numCapsules ; ++j)
    {
      PhysicsShapeDriverDataPhysX3* shapeDriverData = (PhysicsShapeDriverDataPhysX3*)volume.capsules[j].driverData;

      // Get material from the list of different materials (m_materials)
      NMP_ASSERT(volume.capsules[j].materialID <= numMaterials);
      physx::PxMaterial* material = result->m_materials[volume.capsules[j].materialID];

      PxShapeDesc *shapeDesc = shapeDescs[iShape++];
      void *shapeMem = NMPMemoryAlloc(sizeof(physx::PxCapsuleGeometry));
      NMP_ASSERT(shapeMem);
      physx::PxCapsuleGeometry *capsule = new(shapeMem) 
        physx::PxCapsuleGeometry(volume.capsules[j].radius, volume.capsules[j].height * 0.5f);
      shapeDesc->geometry = capsule; 
      // convert to capsule orientated along z, not y
      NMP::Matrix34 m(NMP::Matrix34::kIdentity);
      m.fromEulerXYZ(NMP::Vector3(0, NM_PI_OVER_TWO, 0));
      shapeDesc->localPose = nmMatrix34ToPxTransform(m * volume.capsules[j].localPose);
      shapeDesc->contactOffset = shapeDriverData->m_contactOffset;
      shapeDesc->restOffset = shapeDriverData->m_restOffset;
      shapeDesc->setSingleMaterial(material);
      shapeDesc->userData = &volume.capsules[j].density;
    }

    for (int32_t j = 0 ; j < volume.numSpheres ; ++j)
    {
      PhysicsShapeDriverDataPhysX3* shapeDriverData = (PhysicsShapeDriverDataPhysX3*)volume.spheres[j].driverData;

      // Get material from the list of different materials (m_materials)
      NMP_ASSERT(volume.spheres[j].materialID <= numMaterials);
      physx::PxMaterial* material = result->m_materials[volume.spheres[j].materialID];

      PxShapeDesc *shapeDesc = shapeDescs[iShape++];
      void *shapeMem = NMPMemoryAlloc(sizeof(physx::PxSphereGeometry));
      NMP_ASSERT(shapeMem);
      physx::PxSphereGeometry *sphere = new(shapeMem) physx::PxSphereGeometry(volume.spheres[j].radius);
      shapeDesc->geometry = sphere; 
      shapeDesc->localPose = nmMatrix34ToPxTransform(volume.spheres[j].localPose);
      shapeDesc->contactOffset = shapeDriverData->m_contactOffset;
      shapeDesc->restOffset = shapeDriverData->m_restOffset;
      shapeDesc->setSingleMaterial(material);
      shapeDesc->userData = &volume.spheres[j].density;
    }

    // Fix up common data
    for (uint32_t shapeIndex = 0 ; shapeIndex < numShapes ; ++shapeIndex)
    {
      PxShapeDesc *shapeDesc = shapeDescs[shapeIndex+iFirstShapeForPart];
      shapeDesc->simulationFilterData = filterData;
      shapeDesc->queryFilterData = filterData;
      shapeDesc->flags = flags;
    }

    rigidDynamicDesc.setShapes(&shapeDescs[iFirstShapeForPart], numShapes);
    
    physx::PxRigidDynamic* rigidDynamic = PxCreateRigidDynamic(rigidDynamicDesc);
    NMP_ASSERT(rigidDynamic);

    PhysicsRigPhysX3Jointed::PartPhysX3Jointed* jointedPart = new (result->m_parts[iPart]) PhysicsRigPhysX3Jointed::PartPhysX3Jointed();
    jointedPart->m_rigidBody = rigidDynamic;
    jointedPart->m_physicsRig = result;

    // Update the mass, which is always specified using density of the individual parts
    {
      float* densities = (float*)NMPMemoryAlloc(sizeof(float) * numShapes);
      NMP_ASSERT(densities);
      for (uint32_t shapeIndex = 0 ; shapeIndex < numShapes ; ++shapeIndex)
      {
        PxShapeDesc *shapeDesc = shapeDescs[shapeIndex+iFirstShapeForPart];
        densities[shapeIndex] = *((float*)shapeDesc->userData);
      }
      physx::PxRigidBodyExt::updateMassAndInertia(*rigidDynamic, densities, numShapes); // uses density
      NMP::Memory::memFree(densities);
    }
  }

  for (uint32_t i = 0; i < physicsRigDef->getNumJoints(); ++i)
  {
    const PhysicsSixDOFJointDef* jointDef = (const PhysicsSixDOFJointDef*)physicsRigDef->m_joints[i];

    new(result->m_joints[i]) PhysicsRigPhysX3Jointed::JointPhysX3Jointed(jointDef);

    NMP_ASSERT(physicsRigDef->m_joints[i]->m_jointType == PhysicsJointDef::JOINT_TYPE_SIX_DOF);
    const PhysicsJointDriverDataPhysX3* driverData = (const PhysicsJointDriverDataPhysX3*)jointDef->m_driverData;

    NMP_ASSERT(jointDef->m_parentPartIndex >= 0);
    NMP_ASSERT(jointDef->m_childPartIndex >= 0);
    NMP_ASSERT(jointDef->m_parentPartIndex < (int32_t) result->getNumParts());
    NMP_ASSERT(jointDef->m_childPartIndex < (int32_t) result->getNumParts());

    PhysicsRigPhysX3Jointed::JointPhysX3Jointed* newJoint = (PhysicsRigPhysX3Jointed::JointPhysX3Jointed*)result->m_joints[i];

    newJoint->m_joint = PxD6JointCreate(
      PxGetPhysics(), 
      ((PhysicsRigPhysX3Jointed::PartPhysX3Jointed*)result->m_parts[jointDef->m_parentPartIndex])->m_rigidBody,
      nmMatrix34ToPxTransform(jointDef->m_parentPartFrame),  
      ((PhysicsRigPhysX3Jointed::PartPhysX3Jointed*)result->m_parts[jointDef->m_childPartIndex])->m_rigidBody, 
      nmMatrix34ToPxTransform(jointDef->m_childPartFrame));
    NMP_ASSERT(newJoint->m_joint);

    physx::PxD6Motion::Enum motions[PhysicsSixDOFJointDef::NUM_MOTIONS];
    motions[PhysicsSixDOFJointDef::MOTION_LIMITED] = physx::PxD6Motion::eLIMITED;
    motions[PhysicsSixDOFJointDef::MOTION_FREE] = physx::PxD6Motion::eFREE;
    motions[PhysicsSixDOFJointDef::MOTION_LOCKED] = physx::PxD6Motion::eLOCKED;

#ifdef DISABLE_JOINTS
    motions[PhysicsSixDOFJointDef::MOTION_LIMITED] = physx::PxD6Motion::eFREE;
#endif

    newJoint->m_joint->setMotion(physx::PxD6Axis::eX, physx::PxD6Motion::eLOCKED);
    newJoint->m_joint->setMotion(physx::PxD6Axis::eY, physx::PxD6Motion::eLOCKED);
    newJoint->m_joint->setMotion(physx::PxD6Axis::eZ, physx::PxD6Motion::eLOCKED);

    newJoint->m_joint->setMotion(physx::PxD6Axis::eSWING1, motions[jointDef->m_swing1Motion]);
    newJoint->m_joint->setMotion(physx::PxD6Axis::eSWING2, motions[jointDef->m_swing2Motion]);
    newJoint->m_joint->setMotion(physx::PxD6Axis::eTWIST, motions[jointDef->m_twistMotion]);

    // limits
    newJoint->writeLimits();

    // drives
    if (driverData->m_useSlerpDrive)
    {
      physx::PxD6JointDrive slerpDrive(
        driverData->m_slerpDriveSpring,
        driverData->m_slerpDriveDamping,
        PX_MAX_F32,
        driverData->m_useAccelerationSprings != 0);
      newJoint->m_joint->setDrive(physx::PxD6Drive::eSLERP, slerpDrive);
    }
    else
    {
      physx::PxD6JointDrive swingDrive(
        driverData->m_swingDriveSpring,
        driverData->m_swingDriveDamping,
        PX_MAX_F32,
        driverData->m_useAccelerationSprings != 0);
      physx::PxD6JointDrive twistDrive(
        driverData->m_twistDriveSpring,
        driverData->m_twistDriveDamping,
        PX_MAX_F32,
        driverData->m_useAccelerationSprings != 0);
      newJoint->m_joint->setDrive(physx::PxD6Drive::eSWING, swingDrive);
      newJoint->m_joint->setDrive(physx::PxD6Drive::eTWIST, twistDrive);
    }

    // joint projection
    newJoint->m_joint->setProjectionLinearTolerance(0.0f);
    newJoint->m_joint->setProjectionAngularTolerance(NM_PI); // disable
    newJoint->m_joint->setConstraintFlag(physx::PxConstraintFlag::ePROJECTION, false);

    // make each joint use the damping but not the strength by default - and copy the (max) values
    // so that they can be safely modified in the future
    float jointScale = 1.0f;
#ifdef DISABLE_JOINTS
    jointScale = 0.0f;
#endif

    newJoint->m_maxTwistDamping = driverData->m_twistDriveDamping * jointScale;
    newJoint->m_maxTwistStrength = driverData->m_twistDriveSpring * jointScale;
    newJoint->m_maxSwingDamping = driverData->m_swingDriveDamping * jointScale;
    newJoint->m_maxSwingStrength = driverData->m_swingDriveSpring * jointScale;
    newJoint->m_maxSlerpDamping = driverData->m_slerpDriveDamping * jointScale;
    newJoint->m_maxSlerpStrength = driverData->m_slerpDriveSpring * jointScale;

    newJoint->m_driveMinDampingScale = driverData->m_driveMinDampingScale;
    newJoint->m_limitsEnabled = true;
  }

  // Free the working memory.
  for (uint32_t shapeIndex = 0 ; shapeIndex < totalNumShapes ; ++shapeIndex)
  {
    if (shapeDescs[shapeIndex]->geometry)
    {
      NMP::Memory::memFree(shapeDescs[shapeIndex]->geometry);
    }
  }

  for (uint32_t i = 0; i < totalNumShapes; ++i)
  {
    NMP::Memory::memFree(shapeDescs[i]);
  }
  NMP::Memory::memFree(shapeDescs);

  // The parent joint index of a part is always the part index - 1, since its a tree.
  for (int32_t i = 0; i < physicsRigDef->m_numParts; i++)
    result->getPart(i)->setParentPartIndex(i ? physicsRigDef->m_joints[i-1]->m_parentPartIndex : -1);
  
  // Need to generate the cached values.
  result->generateCachedValues(); // timestep is irrelevant at this point

  result->restoreAllJointDrivesToDefault();

  result->makeKinematic(true);

  result->applyModifiedValues();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsRigPhysX3Jointed::term()
{
  physx::PxScene *physXScene = getPhysicsScenePhysX()->getPhysXScene();
  if (physXScene)
  {
    for (int32_t i = (int32_t) getNumJoints(); i-- != 0; )
      ((JointPhysX3Jointed*)m_joints[i])->m_joint->release();
    for (int32_t i = (int32_t) getNumParts(); i-- != 0; )
      ((PartPhysX3Jointed*)m_parts[i])->m_rigidBody->release();
    for (int32_t i = (int32_t) getNumMaterials(); i-- != 0; )
      m_materials[i]->release();
  }
  m_refCount = 0;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigPhysX3Jointed::PartPhysX3Jointed::PartPhysX3Jointed()
{
  m_userData = NULL;
  m_parentPartIndex = -1;
  m_dirtyFlags = 0;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigPhysX3Jointed::PartPhysX3Jointed::~PartPhysX3Jointed()
{
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigPhysX3Jointed::PartPhysX3Jointed::PartPhysX3Jointed(
  const PhysicsRigPhysX3Jointed::PartPhysX3Jointed& other)
{
  *this = other;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigPhysX3Jointed::PartPhysX3Jointed& PhysicsRigPhysX3Jointed::PartPhysX3Jointed::operator=(
  const PhysicsRigPhysX3Jointed::PartPhysX3Jointed& other)
{
  if (this == &other)
    return *this;

  m_rigidBody = other.m_rigidBody;
  m_parentPartIndex = other.m_parentPartIndex;
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Matrix34 PhysicsRigPhysX3Jointed::PartPhysX3Jointed::getCOMOffsetLocal() const
{
  // We don't expose methods that modifies the local com offset, so just get the value from the cache.
  return m_cache.COMOffsetLocal;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 PhysicsRigPhysX3Jointed::PartPhysX3Jointed::getCOMPosition() const
{
  return m_cache.COMPosition;
}

//----------------------------------------------------------------------------------------------------------------------
float PhysicsRigPhysX3Jointed::PartPhysX3Jointed::getMass() const
{
  // We don't expose methods that modifies the mass, so just get the value from the cache.
  return m_cache.mass;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 PhysicsRigPhysX3Jointed::PartPhysX3Jointed::getPosition() const
{
  return getTransform().translation();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 PhysicsRigPhysX3Jointed::PartPhysX3Jointed::getVel() const
{
  return nmPxVec3ToVector3(m_rigidBody->getLinearVelocity());
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 PhysicsRigPhysX3Jointed::PartPhysX3Jointed::getAngVel() const
{
  return nmPxVec3ToVector3(m_rigidBody->getAngularVelocity());
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 PhysicsRigPhysX3Jointed::PartPhysX3Jointed::getVelocityAtPoint(const NMP::Vector3& point) const
{
  NMP::Vector3 rpoint = point - getCOMPosition();
  return getVel() + NMP::vCross(getAngVel(), rpoint);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 PhysicsRigPhysX3Jointed::PartPhysX3Jointed::getAngularMomentum() const
{
  NMP_ASSERT_FAIL();
  return NMP::Vector3(0,0,0);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 PhysicsRigPhysX3Jointed::PartPhysX3Jointed::getLinearMomentum() const
{
  // there appears to be a bug in the physX implementation so rather than call
  // return nmNxVec3ToVector3(m_actor->getLinearMomentum());
  // for now we'll do this
  return getVel() * getMass();
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::PartPhysX3Jointed::generateCachedValues()
{
  const physx::PxTransform currentGlobalPose = m_rigidBody->getGlobalPose();

  m_cache.bodyFlags = (uint16_t)getRigidDynamic()->getRigidDynamicFlags();
  m_cache.collisionOn = getCollisionEnabled();

  m_cache.mass = m_rigidBody->getMass();
  m_cache.COMOffsetLocal = nmPxTransformToNmMatrix34(m_rigidBody->getCMassLocalPose());
  m_cache.globalPose = nmPxTransformToNmMatrix34(currentGlobalPose);

  // The transforms from PhysX can be pretty bad, so orthonormalise the result.
  m_cache.globalPose.orthonormalise();
  m_cache.kinematicTarget = m_cache.globalPose;

  m_cache.angularVel = nmPxVec3ToVector3(m_rigidBody->getAngularVelocity());
  m_cache.linearVel = nmPxVec3ToVector3(m_rigidBody->getLinearVelocity());

  updateCOMPosition();

  m_dirtyFlags = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::PartPhysX3Jointed::applyModifiedValues()
{
  if (m_dirtyFlags)
  {
    if (m_dirtyFlags & kDirty_Collision)
    {
      physx::PxShape *shapes[MAX_SHAPES_IN_VOLUME];
      NMP_ASSERT(m_rigidBody->getNbShapes() <= MAX_SHAPES_IN_VOLUME);
      physx::PxU32 numShapes = m_rigidBody->getShapes(&shapes[0], MAX_SHAPES_IN_VOLUME);
      NMP_ASSERT(numShapes && shapes[0]);

      // Assume all have the same collision enabled/disabled status. However, note that they may have
      // different collision and query status.
      physx::PxShapeFlags flags = shapes[0]->getFlags(); 

      bool enabledSimulation = flags & physx::PxShapeFlag::eSIMULATION_SHAPE;
      if (enabledSimulation != m_cache.collisionOn)
      {
        for (physx::PxU32 i = 0 ; i < numShapes ; ++i)
        {
          shapes[i]->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, m_cache.collisionOn);
        }
      }

      bool enabledQuery = flags & physx::PxShapeFlag::eSCENE_QUERY_SHAPE;
      if (enabledQuery != m_cache.collisionOn)
      {
        for (physx::PxU32 i = 0 ; i < numShapes ; ++i)
        {
          shapes[i]->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, m_cache.collisionOn);
        }
      }
      // Shouldn't need to call resetFiltering here, as the docs don't indicate that it's necessary
      // after setting the flags since the filter function uses eSUPPRESS (i.e. the filter shader will
      // get called again if the flags change).
    }

    if (m_dirtyFlags & kDirty_BodyFlags)
    {
      getRigidDynamic()->setRigidDynamicFlags((physx::PxRigidDynamicFlags)m_cache.bodyFlags);
    }

    if (m_dirtyFlags & kDirty_GlobalPose)
    {
      getRigidDynamic()->setGlobalPose(nmMatrix34ToPxTransform(m_cache.globalPose));
    }

    if (m_cache.bodyFlags & (physx::PxRigidDynamicFlag::eKINEMATIC))
    {
      //Body is kinematic.
      if (m_dirtyFlags & kDirty_KinematicTarget)
      {
        getRigidDynamic()->setKinematicTarget(nmMatrix34ToPxTransform(m_cache.kinematicTarget));
      }
    }
    else
    {
      // Body is dynamic.
      if (m_dirtyFlags & kDirty_LinearVel)
      {
        getRigidDynamic()->setLinearVelocity(nmVector3ToPxVec3(m_cache.linearVel));
      }

      if (m_dirtyFlags & kDirty_AngularVel)
      {
        getRigidDynamic()->setAngularVelocity(nmVector3ToPxVec3(m_cache.angularVel));
      }
    }

    m_dirtyFlags = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// inertia and summations thereof
//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 PhysicsRigPhysX3Jointed::PartPhysX3Jointed::getMassSpaceInertiaTensor() const
{
  return nmPxVec3ToVector3(m_rigidBody->getMassSpaceInertiaTensor());
}

//----------------------------------------------------------------------------------------------------------------------
// returns the mass moment of inertia in the top 3x3 components
// along with the com position in the translation component
NMP::Matrix34 PhysicsRigPhysX3Jointed::PartPhysX3Jointed::getGlobalInertiaTensor() const
{
  NMP::Vector3 t = nmPxVec3ToVector3(m_rigidBody->getMassSpaceInertiaTensor());
  NMP::Matrix34 massSpaceInertiaTensor(NMP::Matrix34::kIdentity);
  massSpaceInertiaTensor.scale3x3(t);
  NMP::Matrix34 localOffset = getCOMOffsetLocal();
  NMP::Matrix34 result = massSpaceInertiaTensor * localOffset * getTransform();
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Quat PhysicsRigPhysX3Jointed::PartPhysX3Jointed::getQuaternion() const
{
  return getTransform().toQuat();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Matrix34 PhysicsRigPhysX3Jointed::PartPhysX3Jointed::getTransform() const
{
  if ((m_cache.bodyFlags & physx::PxRigidDynamicFlag::eKINEMATIC) != 0)
  {
    return m_cache.kinematicTarget;
  }

  return m_cache.globalPose;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::PartPhysX3Jointed::updateCOMPosition()
{
  m_cache.COMPosition = 
      nmPxVec3ToVector3(
          nmMatrix34ToPxTransform(getTransform()).transform(nmVector3ToPxVec3(m_cache.COMOffsetLocal.translation())));
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::PartPhysX3Jointed::moveTo(const NMP::Matrix34& tm)
{
  m_dirtyFlags |= kDirty_KinematicTarget;
  m_cache.kinematicTarget = tm;

  updateCOMPosition();
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::PartPhysX3Jointed::setPosition(const NMP::Vector3& p)
{
  m_dirtyFlags |= kDirty_GlobalPose;
  m_cache.globalPose.translation() = p;

  updateCOMPosition();
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::PartPhysX3Jointed::setQuaternion(const NMP::Quat& q)
{
  m_dirtyFlags |= kDirty_GlobalPose;
  m_cache.globalPose.fromQuat(q);

  updateCOMPosition();
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::PartPhysX3Jointed::setVel(const NMP::Vector3& v)
{
  m_dirtyFlags |= kDirty_LinearVel;
  m_cache.linearVel = v;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::PartPhysX3Jointed::setAngVel(const NMP::Vector3& angVel)
{
  m_dirtyFlags |= kDirty_AngularVel;
  m_cache.angularVel = angVel;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::PartPhysX3Jointed::setTransform(const NMP::Matrix34& tm)
{
  m_dirtyFlags |= kDirty_GlobalPose;
  m_cache.globalPose = tm;

  updateCOMPosition();
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::PartPhysX3Jointed::makeKinematic(bool kinematic, float NMP_UNUSED(massMultiplier), bool NMP_UNUSED(enableConstraint))
{
  if (!kinematic)
    m_isBeingKeyframed = false;

  if (kinematic == isKinematic())
    return;

  m_dirtyFlags |= kDirty_BodyFlags;
  if (kinematic)
  {
    m_cache.bodyFlags |= physx::PxRigidDynamicFlag::eKINEMATIC;
  }
  else
  {
    m_cache.bodyFlags &= ~(uint16_t)physx::PxRigidDynamicFlag::eKINEMATIC;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsRigPhysX3Jointed::PartPhysX3Jointed::isKinematic() const
{
  return (getRigidDynamic()->getRigidDynamicFlags() & physx::PxRigidDynamicFlag::eKINEMATIC);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::PartPhysX3Jointed::enableCollision(bool enable)
{
  m_dirtyFlags |= kDirty_Collision;
  m_cache.collisionOn = enable;
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsRigPhysX3Jointed::PartPhysX3Jointed::getCollisionEnabled() const
{
  static const physx::PxU32 maxShapes = 1;
  physx::PxShape *shapes[maxShapes];
  m_rigidBody->getShapes(&shapes[0], maxShapes);
  NMP_ASSERT(shapes[0]);
  physx::PxShapeFlags flags = shapes[0]->getFlags(); // assume all have the same collision enabled/disabled status
  bool enabledOnDynamic = flags & physx::PxShapeFlag::eSIMULATION_SHAPE;//getActorFlags() & NX_AF_DISABLE_COLLISION;
  return enabledOnDynamic;
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsRigPhysX3Jointed::PartPhysX3Jointed::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(getPosition());
  savedState.addValue(getQuaternion());
  savedState.addValue(getVel());
  savedState.addValue(getAngVel());
  savedState.addValue(getCachedData());
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsRigPhysX3Jointed::PartPhysX3Jointed::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  setPosition(savedState.getValue<NMP::Vector3>());
  setQuaternion(savedState.getValue<NMP::Quat>());
  setVel(savedState.getValue<NMP::Vector3>());
  setAngVel(savedState.getValue<NMP::Vector3>());
  setCachedData(savedState.getValue<MR::PhysicsRigPhysX3Jointed::PartPhysX3Jointed::CachedData>());
  return true;
}

#if defined(MR_OUTPUT_DEBUGGING)

//----------------------------------------------------------------------------------------------------------------------
uint32_t PhysicsRigPhysX3Jointed::PartPhysX3Jointed::serializeTxPersistentData(
  uint16_t nameToken, 
  uint32_t objectID, 
  void* outputBuffer, 
  uint32_t NMP_USED_FOR_ASSERTS(outputBufferSize)) const
{
  uint32_t dataSize = sizeof(PhysicsPartPersistentData);

  uint32_t numBoxes = 0;
  uint32_t numCapsules = 0;
  uint32_t numSpheres = 0;

  physx::PxU32 numShapes = m_rigidBody->getNbShapes();
  NMP_ASSERT(numShapes <= MAX_SHAPES_IN_VOLUME);

  physx::PxShape* shapes[MAX_SHAPES_IN_VOLUME];
  numShapes = m_rigidBody->getShapes(shapes, MAX_SHAPES_IN_VOLUME);

  for (physx::PxU32 i = 0; i != numShapes; ++i)
  {
    const physx::PxShape *shape = shapes[i];

    physx::PxGeometryType::Enum type = shape->getGeometryType();
    switch (type)
    {
    case physx::PxGeometryType::eSPHERE:
      ++numSpheres;
      break;
    case physx::PxGeometryType::eBOX:
      ++numBoxes;
      break;
    case physx::PxGeometryType::eCAPSULE:
      ++numCapsules;
      break;
    default:
      break;
    }
  }

  dataSize += numBoxes * sizeof(PhysicsBoxPersistentData);
  dataSize += numCapsules * sizeof(PhysicsCapsulePersistentData);
  dataSize += numSpheres * sizeof(PhysicsSpherePersistentData);

  if (outputBuffer != 0)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    PhysicsPartPersistentData *partPersistentData = (PhysicsPartPersistentData *)outputBuffer;

    partPersistentData->m_parentIndex = getParentPartIndex();
    partPersistentData->m_physicsObjectID = objectID;
    partPersistentData->m_numBoxes = numBoxes;
    partPersistentData->m_numCapsules = numCapsules;
    partPersistentData->m_numSpheres = numSpheres;
    partPersistentData->m_nameToken = nameToken;

    // convert to capsule orientated along y, not z, this code mirrors the creation code.
    NMP::Matrix34 yToZ(NMP::Matrix34::kIdentity);

    // convert to capsule orientated along z, not y
    NMP::Matrix34 capsuleConversionTx(NMP::Matrix34::kIdentity);
    capsuleConversionTx.fromEulerXYZ(NMP::Vector3(0, NM_PI_OVER_TWO, 0));

    uint32_t indexBox = 0;
    uint32_t indexCapsule = 0;
    uint32_t indexSphere = 0;
    for (physx::PxU32 i = 0; i != numShapes; ++i)
    {
      const physx::PxShape *pxShape = shapes[i];

      physx::PxGeometryType::Enum type = pxShape->getGeometryType();
      switch (type)
      {
      case physx::PxGeometryType::eSPHERE:
        {
          physx::PxSphereGeometry pxSphere;
          NMP_USED_FOR_ASSERTS(bool result =) pxShape->getSphereGeometry(pxSphere);
          NMP_ASSERT(result);

          PhysicsSpherePersistentData* persistentData = partPersistentData->getSphere(indexSphere);

          physx::PxTransform localPose = pxShape->getLocalPose();
          persistentData->m_localPose = nmPxTransformToNmMatrix34(localPose);

          persistentData->m_radius = pxSphere.radius;

          NMP::netEndianSwap(persistentData->m_localPose);
          NMP::netEndianSwap(persistentData->m_radius);

          ++indexSphere;
          break;
        }
      case physx::PxGeometryType::eBOX:
        {
          physx::PxBoxGeometry pxBox;
          NMP_USED_FOR_ASSERTS(bool result =) pxShape->getBoxGeometry(pxBox);
          NMP_ASSERT(result);

          PhysicsBoxPersistentData* persistentData = partPersistentData->getBox(indexBox);

          physx::PxTransform localPose = pxShape->getLocalPose();
          persistentData->m_localPose = nmPxTransformToNmMatrix34(localPose);

          persistentData->m_width = 2.0f * pxBox.halfExtents.x;
          persistentData->m_height = 2.0f * pxBox.halfExtents.y;
          persistentData->m_depth = 2.0f * pxBox.halfExtents.z;

          NMP::netEndianSwap(persistentData->m_localPose);
          NMP::netEndianSwap(persistentData->m_width);
          NMP::netEndianSwap(persistentData->m_height);
          NMP::netEndianSwap(persistentData->m_depth);

          ++indexBox;
          break;
        }
      case physx::PxGeometryType::eCAPSULE:
        {
          physx::PxCapsuleGeometry pxCapsule;
          NMP_USED_FOR_ASSERTS(bool result =) pxShape->getCapsuleGeometry(pxCapsule);
          NMP_ASSERT(result);

          PhysicsCapsulePersistentData* persistentData = partPersistentData->getCapsule(indexCapsule);

          physx::PxTransform localPose = pxShape->getLocalPose();
          persistentData->m_localPose = capsuleConversionTx * nmPxTransformToNmMatrix34(localPose);

          persistentData->m_radius = pxCapsule.radius;
          persistentData->m_height = 2.0f * pxCapsule.halfHeight;

          NMP::netEndianSwap(persistentData->m_localPose);
          NMP::netEndianSwap(persistentData->m_radius);
          NMP::netEndianSwap(persistentData->m_height);

          ++indexCapsule;
          break;
        }
      default:
        break;
      }
    }

    NMP_ASSERT(indexBox == numBoxes);
    NMP_ASSERT(indexCapsule == numCapsules);
    NMP_ASSERT(indexSphere == numSpheres);

    NMP::netEndianSwap(partPersistentData->m_numSpheres);
    NMP::netEndianSwap(partPersistentData->m_numCapsules);
    NMP::netEndianSwap(partPersistentData->m_numBoxes);
    NMP::netEndianSwap(partPersistentData->m_nameToken);
    NMP::netEndianSwap(partPersistentData->m_parentIndex);
    NMP::netEndianSwap(partPersistentData->m_physicsObjectID);
  }

  return dataSize;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t PhysicsRigPhysX3Jointed::PartPhysX3Jointed::serializeTxFrameData(
  void* outputBuffer, 
  uint32_t NMP_USED_FOR_ASSERTS(outputBufferSize)) const
{
  uint32_t dataSize = sizeof(PhysicsPartFrameData);

  if (outputBuffer != 0)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    PhysicsPartFrameData *partFrameData = (PhysicsPartFrameData *)outputBuffer;
    partFrameData->m_globalPose = getTransform();
    NMP::netEndianSwap(partFrameData->m_globalPose);
  }

  return dataSize;
}
#endif

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigPhysX3Jointed::JointPhysX3Jointed::JointPhysX3Jointed(const PhysicsSixDOFJointDef* const def)
: JointPhysX3(def)
{}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::JointPhysX3Jointed::generateCachedValues()
{
  for (uint32_t i=0; i<physx::PxD6Axis::eCOUNT; ++i)
  {
    m_cache.motions[i] = m_joint->getMotion((physx::PxD6Axis::Enum)i);
  }

  m_cache.swingDrive = m_joint->getDrive(physx::PxD6Drive::eSWING);
  m_cache.twistDrive = m_joint->getDrive(physx::PxD6Drive::eTWIST);
  m_cache.slerpDrive = m_joint->getDrive(physx::PxD6Drive::eSLERP);
  
  physx::PxTransform target = m_joint->getDrivePosition();
  NMP::Matrix34 tm = nmPxTransformToNmMatrix34(target);
  m_cache.driveOrientation = tm.toQuat();

  m_dirtyFlags = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::JointPhysX3Jointed::applyModifiedValues()
{
  if (m_dirtyFlags)
  {
    if (m_dirtyFlags & kDirty_DriveOrientation)
    {
      physx::PxTransform target(nmQuatToPxQuat(m_cache.driveOrientation));
      m_joint->setDrivePosition(target);
    }

    if (m_dirtyFlags & kDirty_Limits)
    {
      // Note that the current api only disable/enable the limits, modifying the motions, so those are the only
      // values that get cached. This could be extended with the swing and twist limits, if necessary.
      for (uint32_t i=0; i<physx::PxD6Axis::eCOUNT; ++i)
      {
        m_joint->setMotion((physx::PxD6Axis::Enum)i, m_cache.motions[i]);
      }
    }

    if (m_dirtyFlags & kDirty_SwingDrive)
    {
      m_joint->setDrive(physx::PxD6Drive::eSWING, m_cache.swingDrive); 
    }

    if (m_dirtyFlags & kDirty_TwistDrive)
    {
      m_joint->setDrive(physx::PxD6Drive::eTWIST, m_cache.twistDrive); 
    }
    
    if (m_dirtyFlags & kDirty_SlerpDrive)
    {
      m_joint->setDrive(physx::PxD6Drive::eSLERP, m_cache.slerpDrive); 
    }

    m_dirtyFlags = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsRigPhysX3Jointed::JointPhysX3Jointed::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  (void) savedState;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsRigPhysX3Jointed::JointPhysX3Jointed::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  (void) savedState;
  return true;
}

#if defined(MR_OUTPUT_DEBUGGING)
//----------------------------------------------------------------------------------------------------------------------
uint32_t PhysicsRigPhysX3Jointed::JointPhysX3Jointed::serializeTxPersistentData(
  const MR::PhysicsJointDef* jointDef,
  uint16_t  stringToken,
  void*     outputBuffer,
  uint32_t  NMP_USED_FOR_ASSERTS(outputBufferSize)) const
{
  uint32_t dataSize = sizeof(PhysicsSixDOFJointPersistentData);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);

    PhysicsSixDOFJointPersistentData* persistentData = (PhysicsSixDOFJointPersistentData*)outputBuffer;

    persistentData->m_parentLocalFrame = jointDef->m_parentPartFrame;
    persistentData->m_childLocalFrame = jointDef->m_childPartFrame;
    persistentData->m_parentPartIndex = jointDef->m_parentPartIndex;
    persistentData->m_childPartIndex = jointDef->m_childPartIndex;

    persistentData->m_jointType = PhysicsJointPersistentData::JOINT_TYPE_SIX_DOF;

    physx::PxJointLimitCone swingLimit = m_joint->getSwingLimit();

    persistentData->m_swing1Limit = swingLimit.yAngle;
    persistentData->m_swing2Limit = swingLimit.zAngle;

    physx::PxJointLimitPair twistLimit = m_joint->getTwistLimit();

    persistentData->m_twistLimitLow = twistLimit.lower;
    persistentData->m_twistLimitHigh = twistLimit.upper;
    persistentData->m_nameToken = stringToken;

    PhysicsSixDOFJointPersistentData::endianSwap(persistentData);
  }

  return dataSize;
}
#endif

//----------------------------------------------------------------------------------------------------------------------
NMP::Quat PhysicsRigPhysX3Jointed::JointPhysX3Jointed::getRotation(
  const MR::PhysicsJointDef* jointDef,
  const NMP::Matrix34& part1TM,
  const NMP::Matrix34& part2TM) const
{
  NMP::Matrix34 frame1 = jointDef->m_parentPartFrame;
  NMP::Matrix34 frame2 = jointDef->m_childPartFrame;

  NMP::Matrix34 joint1inverse;
  joint1inverse.multiply3x3(frame1, part1TM);
  joint1inverse.invertFast3x3();
  NMP::Matrix34 joint2;
  joint2.multiply3x3(frame2, part2TM);
  NMP::Matrix34 jointTM;
  jointTM.multiply3x3(joint2, joint1inverse);
  return jointTM.toQuat();
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::JointPhysX3Jointed::enableLimit(bool enable)
{
  if (enable == m_limitsEnabled)
    return;
#ifdef DISABLE_JOINTS
  return;
#else
  m_limitsEnabled = enable;

  physx::PxD6Motion::Enum motion = enable ? physx::PxD6Motion::eLIMITED : physx::PxD6Motion::eFREE;

  m_cache.motions[physx::PxD6Axis::eSWING1] = motion;
  m_cache.motions[physx::PxD6Axis::eSWING2] = motion;
  m_cache.motions[physx::PxD6Axis::eTWIST] = motion;

  m_dirtyFlags |= kDirty_Limits;
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::JointPhysX3Jointed::writeLimits()
{
  // PhysX crashes with zero swing range
  float swing1 = NMP::clampValue(m_modifiableLimits.getSwing1Limit(), s_minSwingLimit, NM_PI - 0.001f);
  float swing2 = NMP::clampValue(m_modifiableLimits.getSwing2Limit(), s_minSwingLimit, NM_PI - 0.001f);
  float twistLow = NMP::clampValue(m_modifiableLimits.getTwistLimitLow(), -NM_PI_OVER_TWO , NM_PI);
  float twistHigh = NMP::clampValue(m_modifiableLimits.getTwistLimitHigh(), -NM_PI_OVER_TWO , NM_PI);

  // Optimise this with MORPH-16668
  physx::PxJointLimitCone swingLimit(swing1, swing2, m_def->m_hardLimits.getSwingLimitContactDistance());
  physx::PxJointLimitPair twistLimit(twistLow, twistHigh, m_def->m_hardLimits.getTwistLimitContactDistance());

  m_joint->setSwingLimit(swingLimit);
  m_joint->setTwistLimit(twistLimit);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::JointPhysX3Jointed::setDriveOrientation(const NMP::Quat &quat)
{
  // This is what allows unchanging drive to go to sleep, since setDriveOrientation wakes up
  // regardless of whether the drive has changed
  if (quat != m_cache.driveOrientation)
  {
    m_dirtyFlags |= kDirty_DriveOrientation;
    m_cache.driveOrientation = quat;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::JointPhysX3Jointed::setDriveStrength(float twistStrength, float swingStrength, float slerpStrength)
{
  if (slerpStrength != m_cache.slerpDrive.spring)
  {
    m_dirtyFlags |= kDirty_SlerpDrive;
    m_cache.slerpDrive.spring = slerpStrength;
  }
  if (swingStrength != m_cache.swingDrive.spring)
  {
    m_dirtyFlags |= kDirty_SwingDrive;
    m_cache.swingDrive.spring = swingStrength;
  }
  if (twistStrength != m_cache.twistDrive.spring)
  {
    m_dirtyFlags |= kDirty_TwistDrive;
    m_cache.twistDrive.spring = twistStrength;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::JointPhysX3Jointed::setDriveDamping(float twistDamping, float swingDamping, float slerpDamping)
{
  if (slerpDamping != m_cache.slerpDrive.damping)
  {
    m_dirtyFlags |= kDirty_SlerpDrive;
    m_cache.slerpDrive.damping = slerpDamping;
  }
  if (swingDamping != m_cache.swingDrive.damping)
  {
    m_dirtyFlags |= kDirty_SwingDrive;
    m_cache.swingDrive.damping = swingDamping;
  }
  if (twistDamping != m_cache.twistDrive.damping)
  {
    m_dirtyFlags |= kDirty_TwistDrive;
    m_cache.twistDrive.damping = twistDamping;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::JointPhysX3Jointed::setStrength(float NMP_UNUSED(strength))
{
  NMP_ASSERT_FAIL_MSG("This function should never get called");
}

//----------------------------------------------------------------------------------------------------------------------
float PhysicsRigPhysX3Jointed::JointPhysX3Jointed::getStrength() const
{
  NMP_ASSERT_FAIL_MSG("This function should never get called");
  return 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::JointPhysX3Jointed::setDamping(float NMP_UNUSED(damping))
{
  NMP_ASSERT_FAIL_MSG("This function should never get called");
}

//----------------------------------------------------------------------------------------------------------------------
float PhysicsRigPhysX3Jointed::JointPhysX3Jointed::getDamping() const
{
  NMP_ASSERT_FAIL_MSG("This function should never get called");
  return 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Quat PhysicsRigPhysX3Jointed::JointPhysX3Jointed::getTargetOrientation()
{
  NMP_ASSERT_FAIL_MSG("This function should never get called");
  return NMP::Quat::kIdentity;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::JointPhysX3Jointed::setTargetOrientation(const NMP::Quat &NMP_UNUSED(orientation))
{
  NMP_ASSERT_FAIL_MSG("This function should never get called");
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::removeFromScene()
{
  NMP_ASSERT(m_refCount == 0);
  for (uint32_t i = 0; i < getNumParts(); ++i)
  {
    PartPhysX3Jointed *part = (PartPhysX3Jointed*)m_parts[i];
    getPhysicsScenePhysX()->getPhysXScene()->removeActor(*part->m_rigidBody);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::addToScene()
{
  NMP_ASSERT(m_refCount == 0);
  for (uint32_t i = 0; i < getNumParts(); ++i)
  {
    PartPhysX3Jointed *part = (PartPhysX3Jointed*)m_parts[i];
    getPhysicsScenePhysX()->getPhysXScene()->addActor(*part->m_rigidBody);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::makeKinematic(bool moveToKinematicPos)
{
  NMP_ASSERT(m_refCount == 0);
  for (uint32_t i = 0; i < getNumParts(); ++i)
  {
    PartPhysX3Jointed* part = (PartPhysX3Jointed*)m_parts[i];
    part->makeKinematic(true, 1.0f, false);
    part->enableCollision(false);
    if (moveToKinematicPos)
    {
      part->moveTo(m_kinematicPose);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::makeDynamic()
{
  for (uint32_t i = 0; i < getNumParts(); ++i)
  {
    PartPhysX3Jointed* part = (PartPhysX3Jointed*)m_parts[i];
    part->makeKinematic(false, 1.0f, false);
    part->enableCollision(true);
  }

  for (uint32_t i = 0; i < getNumJoints(); ++i)
  {
    PhysicsRigPhysX3Jointed::JointPhysX3Jointed* joint = (JointPhysX3Jointed*)getJoint(i);
    // enable the joint limit
    joint->enableLimit(true);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::restoreAllJointDrivesToDefault()
{
  for (uint32_t i = 0; i < getNumJoints(); ++i)
  {
    JointPhysX3Jointed* joint = (JointPhysX3Jointed*)m_joints[i];
    joint->setDriveStrength(0.0f, 0.0f, 0.0f);
    joint->setDriveDamping(joint->getMaxTwistDamping(), joint->getMaxSwingDamping(), joint->getMaxSlerpDamping());
    joint->enableLimit(true);
  }

  m_desiredJointProjectionIterations = 0;
  m_desiredJointProjectionLinearTolerance = FLT_MAX;
  m_desiredJointProjectionAngularTolerance = NM_PI;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::applyHardKeyframing(
  const NMP::DataBuffer& targetBuffer,
  const NMP::DataBuffer* NMP_UNUSED(previousTargetBuffer),
  const NMP::DataBuffer& fallbackBuffer,
  const NMP::Matrix34&   worldRoot,
  const NMP::Matrix34*   NMP_UNUSED(previousWorldRoot),
  bool                   enableCollision,
  float                  NMP_UNUSED(massMultiplier),
  bool                   NMP_UNUSED(enableConstraint),
  float                  NMP_UNUSED(dt),
  const PartChooser&     partChooser)
{
  NMP_ASSERT_MSG(m_physicsRigDef != NULL, "No RigDef exists. Check AnimationSets to have physics rigs defined");

  for (uint32_t i = 0; i < getNumParts(); ++i)
  {
    if (!partChooser.usePart(i))
      continue;

    PartPhysX3Jointed* part = (PartPhysX3Jointed*)m_parts[i];
    part->makeKinematic(true, 1.0f, false);
    part->m_isBeingKeyframed = true;

    part->enableCollision(enableCollision);

    NMP::Matrix34 targetTM;
    calculateWorldSpacePartTM(targetTM, i, targetBuffer, fallbackBuffer, worldRoot, false);

    // use the fact that the position of the PhysX part is offset so that it is at the same
    // location as the morpheme joint
    part->moveTo(targetTM);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::applySoftKeyframing(
  const NMP::DataBuffer& targetBuffer,
  const NMP::DataBuffer& previousTargetBuffer,
  const NMP::DataBuffer& fallbackBuffer,
  const NMP::Matrix34&   worldRoot,
  const NMP::Matrix34&   previousWorldRoot,
  bool                   enableCollision,
  bool                   enableJointLimits,
  bool                   preserveMomentum,
  float                  NMP_UNUSED(externalJointCompliance),
  float                  gravityCompensationFrac,
  float                  dt,
  float                  weight,
  float                  maxAccel,
  float                  maxAngAccel,
  const PartChooser&     partChooser)
{
  NMP_ASSERT_MSG(m_physicsRigDef != NULL, "No RigDef exists. Check AnimationSets to have physics rigs defined");

  if (dt == 0.0f)
    return;

  NMP::Vector3 gravityDeltaVel = getPhysicsScenePhysX()->getGravity() *
                                 (weight * dt * gravityCompensationFrac);

  maxAccel *= weight;
  maxAngAccel *= weight;

  NMP::Vector3 originalCOMVel(NMP::Vector3::InitZero);
  NMP::Vector3 newCOMVel(NMP::Vector3::InitZero);
  float totalPreservedMass = 0.0f;

  for (uint32_t partIndex = 0; partIndex < getNumParts(); ++partIndex)
  {
    // Skip this part if the part chooser tells us not to apply SK to it.
    if (!partChooser.usePart(partIndex))
      continue;

    PartPhysX3Jointed* part = (PartPhysX3Jointed*)m_parts[partIndex];
    part->makeKinematic(false, 1.0f, false);
    part->m_isBeingKeyframed = true;

    part->enableCollision(enableCollision);

    if (preserveMomentum)
    {
      originalCOMVel += part->getVel() * part->getMass();
      totalPreservedMass += part->getMass();
    }

    // enable/disable joint limits on the parent joint, but only if the parent part is also soft
    // keyframed by this node.
    int32_t parentPartIndex = part->getParentPartIndex();
    if (parentPartIndex >= 0)
    {
      if (partChooser.usePart(parentPartIndex))
      {
        int32_t parentJointIndex = partIndex - 1;
        if (parentJointIndex >= 0)
        {
          // This would be faster if the joint limit enabled state was cached
          JointPhysX3Jointed* joint = (JointPhysX3Jointed*)getJoint(parentJointIndex);
          joint->enableLimit(enableJointLimits);
        }
      }
    }

    NMP::Matrix34 targetTM, targetTMOld;
    calculateWorldSpacePartTM(targetTM, partIndex, targetBuffer, fallbackBuffer, worldRoot, false);
    calculateWorldSpacePartTM(targetTMOld, partIndex, previousTargetBuffer, fallbackBuffer, previousWorldRoot, false);

    // use the fact that the position of the PhysX part is offset so that it is at the same
    // location as the morpheme joint
    NMP::Matrix34 currentTM = part->getTransform();

    NMP::Vector3 offset = part->getCOMPosition();
    NMP::Matrix34 offsetTM(NMP::Matrix34::kIdentity), invOffsetTM(NMP::Matrix34::kIdentity);
    offsetTM.translation() = -offset;
    invOffsetTM.translation() = offset;

    // calculate the motion to go from current to new
    NMP::Matrix34 invCurrentTM(currentTM); invCurrentTM.invertFast();
    // The following pre- and post-multiplication converts diffTM into the actual motion TM
    // centered at the COM.
    NMP::Matrix34 motionTM = invOffsetTM * invCurrentTM * targetTM * offsetTM;

    // Store the distance/angle error
    part->m_SKDeviation = motionTM.translation().magnitude();
    part->m_SKDeviationAngle = motionTM.toRotationVector().magnitude();

    // calculate the motion of the target itself
    NMP::Matrix34 invTargetTMOld(targetTMOld); invTargetTMOld.invertFast();
    NMP::Matrix34 targetMotionTM = invOffsetTM * invTargetTMOld * targetTM * offsetTM;

    // This is where the velocity multiplier can be applied
    NMP::Vector3 translation = motionTM.translation();
    NMP::Vector3 rotation = motionTM.toQuat().toRotationVector(false);

    NMP::Vector3 newVel = translation / dt;
    NMP::Vector3 curVel = part->getVel();
    NMP::Vector3 deltaVel = newVel - curVel;
    if (maxAccel >= 0.0f)
    {
      // prevent overshoot by calculating the max speed we can have in the direction towards
      // the target given that we cannot decelerate faster than maxAccel
      NMP::Vector3 targetVel = targetMotionTM.translation() / dt;
      NMP::Vector3 translationDir = translation;
      float distToTarget = translationDir.normaliseGetLength();
      float curVelAlongTranslation = curVel.dot(translationDir);
      float targetVelAlongTranslation = targetVel.dot(translationDir);

      if (curVelAlongTranslation > targetVelAlongTranslation)
      {
        float timeToCatchUp = distToTarget / (curVelAlongTranslation - targetVelAlongTranslation);
        float maxCurVelAlongTranslation = targetVelAlongTranslation + timeToCatchUp * maxAccel;
        if (curVelAlongTranslation > maxCurVelAlongTranslation)
        {
          // replace the old component along the translation with the new max value
          newVel += translationDir * (maxCurVelAlongTranslation - newVel.dot(translationDir));
          deltaVel = newVel - curVel;
        }
      }

      // clamp the acceleration
      float deltaVelMag = deltaVel.magnitude();
      if (deltaVelMag > maxAccel * dt)
        deltaVel *= maxAccel * dt / deltaVelMag;
    }

    // apply gravity compensation
    deltaVel -= gravityDeltaVel;

    newVel = curVel + deltaVel;
    part->setVel(newVel);

    if (preserveMomentum)
    {
      newCOMVel += newVel * part->getMass();
    }

    NMP::Vector3 newAngVel = rotation / dt;
    NMP::Vector3 curAngVel = part->getAngVel();
    NMP::Vector3 deltaAngVel = newAngVel - curAngVel;
    if (maxAngAccel >= 0.0f)
    {
      // limit the max angular velocity target - this is just a straight conversion of the linear velocity code,
      // so I _think_ it's "correct"!
      NMP::Vector3 targetAngVel = targetMotionTM.toQuat().toRotationVector(false) / dt;
      NMP::Vector3 rotationDir = rotation;
      float distToTarget = rotationDir.normaliseGetLength();
      float curAngVelAlongRotation = curAngVel.dot(rotationDir);
      float targetAngVelAlongRotation = targetAngVel.dot(rotationDir);

      if (curAngVelAlongRotation > targetAngVelAlongRotation)
      {
        float timeToCatchUp = distToTarget / (curAngVelAlongRotation - targetAngVelAlongRotation);
        float maxCurAngVelAlongRotation = targetAngVelAlongRotation + timeToCatchUp * maxAngAccel;

        if (curAngVelAlongRotation > maxCurAngVelAlongRotation)
        {
          // replace the old component along the translation with the new max value
          newAngVel += rotationDir * (maxCurAngVelAlongRotation - newAngVel.dot(rotationDir));
          deltaAngVel = newAngVel - curAngVel;
        }
      }

      // clamp the acceleration
      float deltaAngVelMag = deltaAngVel.magnitude();
      if (deltaAngVelMag > maxAngAccel * dt)
        deltaAngVel *= maxAngAccel * dt / deltaAngVelMag;
    }
    newAngVel = curAngVel + deltaAngVel;
    part->setAngVel(newAngVel);
  }

  if (preserveMomentum)
  {
    originalCOMVel /= totalPreservedMass;
    newCOMVel /= totalPreservedMass;
    NMP::Vector3 correctionVel = originalCOMVel - newCOMVel;
    for (uint32_t i = 0; i < getNumParts(); ++i)
    {
      if (!partChooser.usePart(i))
        continue;
      PhysicsRigPhysX3Jointed::PartPhysX3Jointed *part = (PartPhysX3Jointed*)m_parts[i];
      NMP::Vector3 partVel = part->getVel();
      part->setVel(partVel + correctionVel);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Drives the joints to the targets given by the input animation buffer.   
// Using twist/swing drives seems to result in significant jittering - slerp drive works much
// better. However, currently the physx description defines the params using twist/swing, so we just
// assume the swing strengths can be applied to slerp.
void PhysicsRigPhysX3Jointed::applyActiveAnimation(
  const NMP::DataBuffer& targetBuffer,
  const NMP::DataBuffer& fallbackBuffer,
  float                  strengthMultiplier,
  float                  dampingMultiplier,
  float                  NMP_UNUSED(internalCompliance),
  float                  NMP_UNUSED(externalCompliance),
  bool                   enableJointLimits,
  const JointChooser&    jointChooser,
  float                  limitClampFraction)
{
  NMP_ASSERT_MSG(m_physicsRigDef != NULL, "No RigDef exists. Check AnimationSets to have physics rigs defined");

  for (uint32_t i = 0; i < getNumJoints(); ++i)
  {
    if (!jointChooser.useJoint(i))
      continue;

    JointPhysX3Jointed* joint = (JointPhysX3Jointed*)m_joints[i];
    const PhysicsSixDOFJointDef* jointDef = static_cast<const PhysicsSixDOFJointDef*>(m_physicsRigDef->m_joints[i]);

    PartPhysX3Jointed* childPart = (PartPhysX3Jointed*)m_parts[jointDef->m_childPartIndex];
    childPart->makeKinematic(false, 1.0f, false);
    childPart->m_isBeingKeyframed = false;

    // don't force either of the parts to have collision - no way we could know which one _should_
    // have collision if it's disabled elsewhere.

    joint->setDriveStrength(
      joint->getMaxTwistStrength() * strengthMultiplier,
      joint->getMaxSwingStrength() * strengthMultiplier,
      joint->getMaxSlerpStrength() * strengthMultiplier);
    joint->setDriveDamping(
      joint->getMaxTwistDamping() * dampingMultiplier,
      joint->getMaxSwingDamping() * dampingMultiplier,
      joint->getMaxSlerpDamping() * dampingMultiplier);
    joint->enableLimit(enableJointLimits);

    if (strengthMultiplier < 0.0000001f)
      continue;

    NMP::Quat curQ;
    getQuatFromTransformBuffer(jointDef->m_childPartIndex, targetBuffer, fallbackBuffer, curQ);

    // q is the rotation of the child relative to the parent (in parent space).
    // We need to account for the offset axes in the joint.

    // Get the local joint axes in each frame as l0, l1
    NMP::Quat l0 = m_physicsRigDef->m_joints[i]->m_parentPartFrame.toQuat();
    NMP::Quat l1 = m_physicsRigDef->m_joints[i]->m_childPartFrame.toQuat();

    // now "assuming" the parent is at the origin (since we already have the relative rotation q)
    // we want to calculate rot, the relative rotation of the child local frame from the parent local frame
    NMP::Quat l0Inv = ~l0;

    // target orientations outside the limits cause oscillations when physical limits are enabled
    if (limitClampFraction >= 0.0f)
    {
      NMP_ASSERT(m_physicsRigDef->m_joints[i]->m_jointType == PhysicsJointDef::JOINT_TYPE_SIX_DOF);
      joint->clampToLimits(curQ, limitClampFraction, NULL);
    }

    NMP::Quat curFrameQ = l0Inv * curQ * l1;
    joint->setDriveOrientation(curFrameQ);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::applyActiveAnimation(uint32_t jointIndex, const NMP::Quat& targetQuat, bool makeChildDynamic)
{
  NMP_ASSERT(jointIndex < getNumJoints());
  JointPhysX3Jointed* joint = (JointPhysX3Jointed*)m_joints[jointIndex];
  const PhysicsJointDef* jointDef = m_physicsRigDef->m_joints[jointIndex];
  if (makeChildDynamic)
  {
    PartPhysX3Jointed *childPart = (PartPhysX3Jointed*)m_parts[jointDef->m_childPartIndex];
    childPart->makeKinematic(false, 1.0f, false);
    childPart->m_isBeingKeyframed = false;
  }
  // don't force either of the parts to have collision - no way we could know which one _should_
  // have collision if it's disabled elsewhere.
  joint->setDriveOrientation(targetQuat);
}

//----------------------------------------------------------------------------------------------------------------------
// TODO move this into PhysicsRig
NMP::Quat PhysicsRigPhysX3Jointed::getJointQuat(uint32_t jointIndex)
{
  JointPhysX3Jointed* joint = (JointPhysX3Jointed*)getJoint(jointIndex);
  const PhysicsJointDef* jointDef = m_physicsRigDef->m_joints[jointIndex];
  uint32_t p1 = jointDef->m_parentPartIndex;
  uint32_t p2 = jointDef->m_childPartIndex;
  NMP::Matrix34 part1TM = getPart(p1)->getTransform();
  NMP::Matrix34 part2TM = getPart(p2)->getTransform();
  NMP::Quat result = joint->getRotation(jointDef, part1TM, part2TM);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::generateCachedValues()
{
  for (uint32_t i = 0; i < getNumParts(); ++i)
  {
    ((PartPhysX3Jointed*)m_parts[i])->generateCachedValues();
  }
  for (uint32_t i = 0; i < getNumJoints(); ++i)
  {
    ((JointPhysX3Jointed*)m_joints[i])->generateCachedValues();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::applyModifiedValues()
{
  for (uint32_t i = 0; i < getNumParts(); ++i)
  {
    ((PartPhysX3Jointed*)m_parts[i])->applyModifiedValues();
  }

  bool doProjection = m_desiredJointProjectionIterations != 0;
  for (uint32_t i = 0; i < getNumJoints(); ++i)
  {
    JointPhysX3Jointed* j = (JointPhysX3Jointed*) m_joints[i];
    j->applyModifiedValues();

    // Set the separation properties - these will get reset in updatePostPhysics
    j->m_joint->setProjectionLinearTolerance(m_desiredJointProjectionLinearTolerance);
    j->m_joint->setProjectionAngularTolerance(m_desiredJointProjectionAngularTolerance);
    j->m_joint->setConstraintFlag(physx::PxConstraintFlag::ePROJECTION, doProjection);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::disableSleeping()
{
  getPartPhysXJointed(0)->getRigidDynamic()->wakeUp();
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::updatePrePhysics(float NMP_UNUSED(timeStep))
{
  applyModifiedValues();
  updateRegisteredJoints();
  writeJointLimits();
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::updatePostPhysics(float NMP_UNUSED(timeStep))
{
  if (!isReferenced())
    return;
  reenableSleeping(); 
  PhysicsRigPhysX3Jointed* physicsRigPhysX = (PhysicsRigPhysX3Jointed*)this;
  physicsRigPhysX->generateCachedValues();

#if defined(MR_OUTPUT_DEBUGGING)
  // Copy joint limits from joint into serialisation data structure.
  for (uint32_t i = 0, numJoints = getNumJoints(); i < numJoints; ++i)
  {
    static_cast< JointPhysX3* >(getJoint(i))->updateSerializeTxFrameData();
  }
#endif // MR_OUTPUT_DEBUGGING

  resetJointLimits();
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Jointed::dumpToRepX(physx::repx::RepXCollection *collection, 
                                         physx::repx::RepXIdToRepXObjectMap *idMap) const
{
  for (uint32_t iMaterial = 0 ; iMaterial < m_physicsRigDef->m_numMaterials ; ++iMaterial)
  {
    physx::repx::RepXObject material = physx::repx::createRepXObject(m_materials[iMaterial]);
#ifdef NMP_ENABLE_ASSERTS
    physx::repx::RepXAddToCollectionResult result =
#endif
      collection->addRepXObjectToCollection(material, *idMap);
    NMP_ASSERT(result.isValid());
  }

  for (uint32_t iPart = 0 ; iPart < getNumParts() ; ++iPart)
  {
    physx::repx::RepXObject actor = physx::repx::createRepXObject(getPartPhysXJointed(iPart)->getRigidBody());
#ifdef NMP_ENABLE_ASSERTS
    physx::repx::RepXAddToCollectionResult result =
#endif
      collection->addRepXObjectToCollection(actor, *idMap);
    NMP_ASSERT(result.isValid());
  }

  for (uint32_t iJoint = 0 ; iJoint < getNumJoints() ; ++iJoint)
  {
    physx::repx::RepXObject actor = physx::repx::createRepXObject(getJointPhysXJointed(iJoint)->m_joint);
#ifdef NMP_ENABLE_ASSERTS
    physx::repx::RepXAddToCollectionResult result =
#endif
      collection->addRepXObjectToCollection(actor, *idMap);
    NMP_ASSERT(result.isValid());
  }
}

} // namespace MR
//----------------------------------------------------------------------------------------------------------------------
