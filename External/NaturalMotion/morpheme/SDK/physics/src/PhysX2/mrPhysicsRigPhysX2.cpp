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
#include "physics/PhysX2/mrPhysX2.h"
#include "physics/PhysX2/mrPhysicsDriverDataPhysX2.h"
#include "physics/PhysX2/mrPhysicsRigPhysX2.h"
#include "physics/PhysX2/mrPhysicsScenePhysX2.h"
#include "sharedDefines/mPhysicsDebugInterface.h"

//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
//----------------------------------------------------------------------------------------------------------------------
bool locatePhysicsRigDefPhysX2(uint32_t NMP_USED_FOR_ASSERTS(assetType), void* assetMemory)
{
  NMP_ASSERT(assetType == Manager::kAsset_PhysicsRigDef);
  PhysicsRigDef* physicsRigDef = (PhysicsRigDef*)assetMemory;
  bool result = physicsRigDef->locate();
  if (result)
  {
    return locateDriverDataPhysX2(physicsRigDef);
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
// PhysicsRigPhysX2
//----------------------------------------------------------------------------------------------------------------------
PhysicsRigPhysX2::PhysicsRigPhysX2()
{
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format PhysicsRigPhysX2::getMemoryRequirements(PhysicsRigDef* physicsRigDef)
{
  uint32_t numBones = physicsRigDef->getNumParts();
  uint32_t numJoints = physicsRigDef->getNumJoints();
  uint32_t numMaterials = physicsRigDef->getNumMaterials();

  NMP::Memory::Format result(sizeof(PhysicsRigPhysX2), NMP_VECTOR_ALIGNMENT);

  // Space for the material pointers
  result += NMP::Memory::Format(sizeof(NxMaterial*) * numMaterials, NMP_NATURAL_TYPE_ALIGNMENT);

  // Space for the part pointers
  result += NMP::Memory::Format(sizeof(PhysicsRigPhysX2::PartPhysX*) * numBones, NMP_NATURAL_TYPE_ALIGNMENT);

  // Space for the joint pointers
  result += NMP::Memory::Format(sizeof(PhysicsRigPhysX2::JointPhysX*) * numJoints, NMP_NATURAL_TYPE_ALIGNMENT);

  // Space for the parts
  result += NMP::Memory::Format(NMP::Memory::align(sizeof(PhysicsRigPhysX2::PartPhysX), NMP_NATURAL_TYPE_ALIGNMENT) * numBones, NMP_NATURAL_TYPE_ALIGNMENT);

  // Space for the joints
  result += NMP::Memory::Format(NMP::Memory::align(sizeof(PhysicsRigPhysX2::JointPhysX), NMP_NATURAL_TYPE_ALIGNMENT) * numJoints, NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigPhysX2* PhysicsRigPhysX2::init(
  NMP::Memory::Resource& resource,
  PhysicsRigDef*         physicsRigDef,
  PhysicsScene*          physicsScene,
  AnimRigDef*            animRigDef,
  AnimToPhysicsMap*      animToPhysicsMap,
  int32_t                NMP_UNUSED(collisionTypeMask),
  int32_t                NMP_UNUSED(collisionIgnoreMask))
{
  PhysicsRigPhysX2* result = (PhysicsRigPhysX2*)resource.ptr;
  resource.increment(sizeof(PhysicsRigPhysX2));

  new (result) PhysicsRigPhysX2();
  result->m_physicsScene = physicsScene;
  result->m_characterControllerActor = 0;
  result->m_refCount = 0;

  result->m_kinematicPos.setToZero();

  uint32_t numBones = physicsRigDef->getNumParts();
  uint32_t numJoints = physicsRigDef->getNumJoints();
  uint32_t numMaterials = physicsRigDef->getNumMaterials();

  // Unique material pointers
  resource.align(NMP::Memory::Format(sizeof(NxMaterial*) * numMaterials, NMP_NATURAL_TYPE_ALIGNMENT));
  result->m_materials = (NxMaterial**)resource.ptr;
  resource.increment(NMP::Memory::Format(sizeof(NxMaterial*) * numMaterials, NMP_NATURAL_TYPE_ALIGNMENT));

  // Part pointers
  resource.align(NMP::Memory::Format(sizeof(PhysicsRig::Part*) * numBones, NMP_NATURAL_TYPE_ALIGNMENT));
  result->m_parts = (PhysicsRig::Part**)resource.ptr;
  resource.increment(NMP::Memory::Format(sizeof(PhysicsRig::Part*) * numBones, NMP_NATURAL_TYPE_ALIGNMENT));

  // Joint pointers
  resource.align(NMP::Memory::Format(sizeof(PhysicsRig::Joint*) * numJoints, NMP_NATURAL_TYPE_ALIGNMENT));
  result->m_joints = (PhysicsRig::Joint**)resource.ptr;
  resource.increment(NMP::Memory::Format(sizeof(PhysicsRig::Joint*) * numJoints, NMP_NATURAL_TYPE_ALIGNMENT));

  // Parts
  for (uint32_t i = 0; i < numBones; i++)
  {
    resource.align(NMP::Memory::Format(sizeof(PhysicsRigPhysX2::PartPhysX), NMP_NATURAL_TYPE_ALIGNMENT));
    result->m_parts[i] = (PhysicsRigPhysX2::PartPhysX*)resource.ptr;
    resource.increment(NMP::Memory::Format(sizeof(PhysicsRigPhysX2::PartPhysX), NMP_NATURAL_TYPE_ALIGNMENT));
  }

  // Joints
  for (uint32_t i = 0; i < numJoints; i++)
  {
    resource.align(NMP::Memory::Format(sizeof(PhysicsRigPhysX2::JointPhysX), NMP_NATURAL_TYPE_ALIGNMENT));
    result->m_joints[i] = (PhysicsRigPhysX2::JointPhysX*)resource.ptr;
    resource.increment(NMP::Memory::Format(sizeof(PhysicsRigPhysX2::JointPhysX), NMP_NATURAL_TYPE_ALIGNMENT));
  }

  result->m_animRigDef = animRigDef;
  result->m_animToPhysicsMap = animToPhysicsMap;
  result->m_physicsRigDef = physicsRigDef;
  result->m_enableJointProjection = false;
  result->m_desiredJointProjectionLinearTolerance = FLT_MAX;
  result->m_desiredJointProjectionAngularTolerance = NM_PI;

  // Fill m_materials in with the different materials in the physicsScene
  result->createMaterialsList(physicsRigDef, physicsScene);

  for (uint32_t i = 0; i < physicsRigDef->getNumParts(); ++i)
  {
    PhysicsRigDef::Part& part = physicsRigDef->m_parts[i];

    // Actor
    NxActorDesc actorDesc;
    actorDesc.name = part.name;
    actorDesc.globalPose.setColumnMajor44(&part.actor.globalPose.r[0].x);
    if (part.actor.hasCollision)
      actorDesc.flags &= ~NX_AF_DISABLE_COLLISION;
    else
      actorDesc.flags |= NX_AF_DISABLE_COLLISION;

    NxBodyDesc bodyDesc;
    if (part.actor.isFixed)
    {
      // null body descriptor indicates a static actor.
      actorDesc.body = 0;
    }
    else
    {
      actorDesc.body = &bodyDesc;
      bodyDesc.angularDamping = part.body.angularDamping;
      bodyDesc.linearDamping = part.body.linearDamping;

      const PhysicsBodyDriverDataPhysX2* driverData = (const PhysicsBodyDriverDataPhysX2*)part.body.driverData;
      bodyDesc.maxAngularVelocity = driverData->m_maxAngularVelocity;

      actorDesc.density = 1.0f;

      bodyDesc.solverIterationCount = driverData->m_solverIterationCount;

      if (driverData->m_usesSleepEnergyThreshold)
      {
        bodyDesc.flags |= NX_BF_ENERGY_SLEEP_TEST;
        bodyDesc.sleepEnergyThreshold = driverData->m_sleepEnergyThreshold;
      }
    }

    // Volumes
    PhysicsRigDef::Part::Volume& volume = part.volume;
    uint32_t numShapes = volume.numSpheres + volume.numBoxes + volume.numCapsules;

    PhysicsRigDef::Part::Volume::Shape* inOrderShapes[MAX_SHAPES_IN_VOLUME];
    NxShapeType inOrderShapeTypes[MAX_SHAPES_IN_VOLUME];
    NMP_ASSERT(numShapes <= MAX_SHAPES_IN_VOLUME);

    for (int32_t j = 0 ; j < volume.numBoxes ; ++j)
    {
      int32_t parentIndex = volume.boxes[j].parentIndex;
      inOrderShapes[parentIndex] = &(volume.boxes[j]);
      inOrderShapeTypes[parentIndex] = NX_SHAPE_BOX;
    }
    for (int32_t j = 0 ; j < volume.numCapsules ; ++j)
    {
      int32_t parentIndex = volume.capsules[j].parentIndex;
      inOrderShapes[parentIndex] = &(volume.capsules[j]);
      inOrderShapeTypes[parentIndex] = NX_SHAPE_CAPSULE;
    }
    for (int32_t j = 0 ; j < volume.numSpheres ; ++j)
    {
      int32_t parentIndex = volume.spheres[j].parentIndex;
      inOrderShapes[parentIndex] = &(volume.spheres[j]);
      inOrderShapeTypes[parentIndex] = NX_SHAPE_SPHERE;
    }

    NxShapeDesc** shapes = (NxShapeDesc**)NMPMemoryAlloc(sizeof(NxShapeDesc*) * numShapes);
    NMP_ASSERT(shapes);
    uint32_t iShape = 0;

    // Volumes
    for (uint32_t k = 0 ; k < numShapes ; ++k)
    {
      const PhysicsShapeDriverDataPhysX2* driverData = (const PhysicsShapeDriverDataPhysX2*)inOrderShapes[k]->driverData;

      switch (inOrderShapeTypes[k])
      {
      case NX_SHAPE_SPHERE:
        {
          PhysicsRigDef::Part::Volume::Sphere* shape = reinterpret_cast<PhysicsRigDef::Part::Volume::Sphere *>(inOrderShapes[k]);
          void* shapeMem = NMPMemoryAlloc(sizeof(NxSphereShapeDesc));
          NMP_ASSERT(shapeMem);
          NxSphereShapeDesc* sphereShapeDesc = new(shapeMem) NxSphereShapeDesc;
          shapes[iShape++] = sphereShapeDesc;

          // Get material from the list of different materials (m_materials)
          NMP_ASSERT(shape->materialID <= numMaterials);
          NxMaterial* material = result->m_materials[shape->materialID];

          actorDesc.shapes.pushBack(sphereShapeDesc);
          sphereShapeDesc->materialIndex = material->getMaterialIndex();
          sphereShapeDesc->radius = shape->radius;
          sphereShapeDesc->localPose = nmMatrix34ToNxMat34(shape->localPose);
          sphereShapeDesc->skinWidth = driverData->m_skinWidth;
          sphereShapeDesc->density = shape->density;
        }
        break;

      case NX_SHAPE_CAPSULE:
        {
          PhysicsRigDef::Part::Volume::Capsule* shape = reinterpret_cast<PhysicsRigDef::Part::Volume::Capsule *>(inOrderShapes[k]);
          void* shapeMem = NMPMemoryAlloc(sizeof(NxCapsuleShapeDesc));
          NMP_ASSERT(shapeMem);
          NxCapsuleShapeDesc* capsuleShapeDesc = new(shapeMem) NxCapsuleShapeDesc;
          shapes[iShape++] = capsuleShapeDesc;

          // Get material from the list of different materials (m_materials)
          NMP_ASSERT(shape->materialID <= numMaterials);
          NxMaterial* material = result->m_materials[shape->materialID];

          actorDesc.shapes.pushBack(capsuleShapeDesc);
          capsuleShapeDesc->materialIndex = material->getMaterialIndex();
          capsuleShapeDesc->radius = shape->radius;
          capsuleShapeDesc->height = shape->height;
          // convert to capsule orientated along z, not y
          NMP::Matrix34 m(NMP::Matrix34::kIdentity);
          m.fromEulerXYZ(NMP::Vector3(NM_PI_OVER_TWO, 0, 0));
          capsuleShapeDesc->localPose = nmMatrix34ToNxMat34(m * shape->localPose);
          capsuleShapeDesc->skinWidth = driverData->m_skinWidth;
          capsuleShapeDesc->density = shape->density;
        }
        break;

      case NX_SHAPE_BOX:
        {
          PhysicsRigDef::Part::Volume::Box* shape = reinterpret_cast<PhysicsRigDef::Part::Volume::Box *>(inOrderShapes[k]);
          void* shapeMem = NMPMemoryAlloc(sizeof(NxBoxShapeDesc));
          NMP_ASSERT(shapeMem);
          NxBoxShapeDesc* boxShapeDesc = new(shapeMem) NxBoxShapeDesc;
          shapes[iShape++] = boxShapeDesc;

          // Get material from the list of different materials (m_materials)
          NMP_ASSERT(shape->materialID <= numMaterials);
          NxMaterial* material = result->m_materials[shape->materialID];

          actorDesc.shapes.pushBack(boxShapeDesc);
          boxShapeDesc->materialIndex = material->getMaterialIndex();
          boxShapeDesc->dimensions = nmVector3ToNxVec3(shape->dimensions);
          boxShapeDesc->localPose = nmMatrix34ToNxMat34(shape->localPose);
          boxShapeDesc->skinWidth = driverData->m_skinWidth;
          boxShapeDesc->density = shape->density;
        }
        break;
      default:
        NMP_ASSERT_FAIL();
      }
    }

    NxActor* actor = ((PhysicsScenePhysX2*)physicsScene)->getPhysXScene()->createActor(actorDesc);
    NMP_ASSERT(actor);

    for (uint32_t j = 0 ; j < numShapes ; ++j)
    {
      NMP::Memory::memFree(shapes[j]);
    }
    NMP::Memory::memFree(shapes);

    // Allow very large angular velocities for soft keyframing to work
    actor->setContactReportFlags(NX_NOTIFY_ON_TOUCH);

    new(result->m_parts[i]) PhysicsRigPhysX2::PartPhysX();
    ((PhysicsRigPhysX2::PartPhysX*)result->m_parts[i])->m_actor = actor;
    ((PhysicsRigPhysX2::PartPhysX*)result->m_parts[i])->m_physicsRig = result;
  }

  for (uint32_t i = 0; i < physicsRigDef->getNumJoints(); ++i)
  {
    new (result->m_joints[i]) PhysicsRigPhysX2::JointPhysX();

    NMP_ASSERT(physicsRigDef->m_joints[i]->m_jointType == PhysicsJointDef::JOINT_TYPE_SIX_DOF);
    const PhysicsSixDOFJointDef* jointDef = (const PhysicsSixDOFJointDef*)physicsRigDef->m_joints[i];
    const PhysicsJointDriverDataPhysX2* driverData = (const PhysicsJointDriverDataPhysX2*)jointDef->m_driverData;

    NMP_ASSERT(jointDef->m_parentPartIndex >= 0);
    NMP_ASSERT(jointDef->m_childPartIndex >= 0);
    NMP_ASSERT(jointDef->m_parentPartIndex < (int32_t) result->getNumParts());
    NMP_ASSERT(jointDef->m_childPartIndex < (int32_t) result->getNumParts());
    NxD6JointDesc d6JointDesc;

    d6JointDesc.actor[0] = ((PhysicsRigPhysX2::PartPhysX*)result->m_parts[jointDef->m_parentPartIndex])->m_actor;
    d6JointDesc.actor[1] = ((PhysicsRigPhysX2::PartPhysX*)result->m_parts[jointDef->m_childPartIndex])->m_actor;

    d6JointDesc.localAnchor[0] = nmVector3ToNxVec3(jointDef->m_parentPartFrame.translation());
    d6JointDesc.localAnchor[1] = nmVector3ToNxVec3(jointDef->m_childPartFrame.translation());
    d6JointDesc.localAxis[0] = nmVector3ToNxVec3(jointDef->m_parentPartFrame.xAxis());
    d6JointDesc.localAxis[1] = nmVector3ToNxVec3(jointDef->m_childPartFrame.xAxis());
    d6JointDesc.localNormal[0] = nmVector3ToNxVec3(jointDef->m_parentPartFrame.yAxis());
    d6JointDesc.localNormal[1] = nmVector3ToNxVec3(jointDef->m_childPartFrame.yAxis());

    d6JointDesc.xMotion = NX_D6JOINT_MOTION_LOCKED;
    d6JointDesc.yMotion = NX_D6JOINT_MOTION_LOCKED;
    d6JointDesc.zMotion = NX_D6JOINT_MOTION_LOCKED;

    NxD6JointMotion motions[PhysicsSixDOFJointDef::NUM_MOTIONS];
    motions[PhysicsSixDOFJointDef::MOTION_LIMITED] = NX_D6JOINT_MOTION_LIMITED;
    motions[PhysicsSixDOFJointDef::MOTION_FREE] = NX_D6JOINT_MOTION_FREE;
    motions[PhysicsSixDOFJointDef::MOTION_LOCKED] = NX_D6JOINT_MOTION_LOCKED;

    d6JointDesc.swing1Motion = motions[jointDef->m_swing1Motion];
    d6JointDesc.swing2Motion = motions[jointDef->m_swing2Motion];
    d6JointDesc.twistMotion = motions[jointDef->m_twistMotion];

#define DISABLE_JOINTSx
#ifdef DISABLE_JOINTS
    d6JointDesc.xMotion = NX_D6JOINT_MOTION_FREE;
    d6JointDesc.yMotion = NX_D6JOINT_MOTION_FREE;
    d6JointDesc.zMotion = NX_D6JOINT_MOTION_FREE;
    d6JointDesc.swing1Motion = NX_D6JOINT_MOTION_FREE;
    d6JointDesc.swing2Motion = NX_D6JOINT_MOTION_FREE;
    d6JointDesc.twistMotion = NX_D6JOINT_MOTION_FREE;
#endif

    // limits
    d6JointDesc.swing1Limit.value = jointDef->m_hardLimits.getSwing1Limit();
    d6JointDesc.swing2Limit.value = jointDef->m_hardLimits.getSwing2Limit();
    d6JointDesc.twistLimit.low.value = NMP::clampValue(jointDef->m_hardLimits.getTwistLimitLow(), -NM_PI_OVER_TWO , NM_PI);
    d6JointDesc.twistLimit.high.value = NMP::clampValue(jointDef->m_hardLimits.getTwistLimitHigh(), -NM_PI_OVER_TWO , NM_PI);
#ifdef NMP_ENABLE_ASSERTS
    if ((d6JointDesc.twistLimit.low.value != jointDef->m_hardLimits.getTwistLimitLow()) ||
        (d6JointDesc.twistLimit.high.value != jointDef->m_hardLimits.getTwistLimitHigh()))
    {
      NMP_DEBUG_MSG("WARNING: The twist limit value in joint %s has been clamped.", jointDef->m_name);
    }
#endif

    // drives
    d6JointDesc.twistDrive.damping = driverData->m_twistDriveDamping;
    d6JointDesc.twistDrive.spring = driverData->m_twistDriveSpring;
    d6JointDesc.swingDrive.damping = driverData->m_swingDriveDamping;
    d6JointDesc.swingDrive.spring = driverData->m_swingDriveSpring;
    d6JointDesc.slerpDrive.damping = driverData->m_slerpDriveDamping;
    d6JointDesc.slerpDrive.spring = driverData->m_slerpDriveSpring;

#define NX_VERSION_WITH_ACCEL_SPRING ((2<<24)+(8<<16)+(1<<8) + 0)
#if NX_PHYSICS_SDK_VERSION >= NX_VERSION_WITH_ACCEL_SPRING
    d6JointDesc.useAccelerationSpring = driverData->m_useAccelerationSprings;
#endif

    d6JointDesc.twistDrive.driveType = NX_D6JOINT_DRIVE_POSITION;
    d6JointDesc.swingDrive.driveType = NX_D6JOINT_DRIVE_POSITION;
    d6JointDesc.slerpDrive.driveType = NX_D6JOINT_DRIVE_POSITION;

    if (driverData->m_useSlerpDrive)
      d6JointDesc.flags |= NX_D6JOINT_SLERP_DRIVE;

    // joint projection
    d6JointDesc.projectionAngle = 0.0f;
    d6JointDesc.projectionDistance = 0.0f;
    d6JointDesc.projectionMode = NX_JPM_NONE;

    // make each joint use the damping but not the strength by default - and copy the (max) values
    // so that they can be safely modified in the future
    PhysicsRigPhysX2::JointPhysX* newJoint = (PhysicsRigPhysX2::JointPhysX*)result->m_joints[i];
    new(newJoint) PhysicsRigPhysX2::JointPhysX();

    newJoint->m_def = jointDef;

    newJoint->m_maxTwistDamping = d6JointDesc.twistDrive.damping;
    newJoint->m_maxTwistStrength = d6JointDesc.twistDrive.spring;
    newJoint->m_maxSwingDamping = d6JointDesc.swingDrive.damping;
    newJoint->m_maxSwingStrength = d6JointDesc.swingDrive.spring;
    newJoint->m_maxSlerpDamping = d6JointDesc.slerpDrive.damping;
    newJoint->m_maxSlerpStrength = d6JointDesc.slerpDrive.spring;

    newJoint->m_limitsEnabled = true;

    // Copy joint limits from joint def.
    newJoint->getLimits().setSwingLimit(
      jointDef->m_hardLimits.getSwing1Limit(), 
      jointDef->m_hardLimits.getSwing2Limit());
    newJoint->getLimits().setTwistLimit(
      jointDef->m_hardLimits.getTwistLimitLow(), 
      jointDef->m_hardLimits.getTwistLimitHigh());

    NxJoint* nxJoint = ((PhysicsScenePhysX2*)physicsScene)->getPhysXScene()->createJoint(d6JointDesc);
    NMP_ASSERT(nxJoint);
    newJoint->m_joint = nxJoint;
  }

  for (int32_t i = 0; i < physicsRigDef->m_numCollisionGroups; ++i)
  {
    if (physicsRigDef->m_collisionGroups[i].enabled)
    {
      for (int32_t j = 0; j < physicsRigDef->m_collisionGroups[i].numIndices - 1; j++)
      {
        for (int32_t k = j + 1; k < physicsRigDef->m_collisionGroups[i].numIndices; k++)
        {
          NMP_ASSERT(physicsRigDef->m_collisionGroups[i].indices[j] < (int32_t) result->getNumParts());
          NMP_ASSERT(physicsRigDef->m_collisionGroups[i].indices[k] < (int32_t) result->getNumParts());
          ((PhysicsScenePhysX2*)physicsScene)->getPhysXScene()->setActorPairFlags(
            *((PhysicsRigPhysX2::PartPhysX*)result->m_parts[physicsRigDef->m_collisionGroups[i].indices[j]])->m_actor,
            *((PhysicsRigPhysX2::PartPhysX*)result->m_parts[physicsRigDef->m_collisionGroups[i].indices[k]])->m_actor,
            NX_IGNORE_PAIR);
        }
      }
    }
  }

  // The parent joint index of a part is always the part index - 1, since its a tree.
  Part* part = result->getPart(0);
  part->setParentPartIndex(-1);
  for (int32_t i = 1; i < physicsRigDef->m_numParts; i++)
  {
    part = result->getPart(i);
    part->setParentPartIndex(physicsRigDef->m_joints[i - 1]->m_parentPartIndex);
  }

  result->generateCachedValues();

  result->restoreAllJointDrivesToDefault();

  result->makeKinematic(true);

  // Make sure that the physx setup is properly sent to the sdk.
  // This is necessary because physx calls are cached.
  result->applyModifiedValues();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsRigPhysX2::term()
{
  NMP_ASSERT(getPhysicsScenePhysX());
  NxScene* physXScene = getPhysicsScenePhysX()->getPhysXScene();
  if (physXScene)
  {
    for (int32_t i = (int32_t) getNumJoints(); i-- != 0; )
      physXScene->releaseJoint(*((JointPhysX*)m_joints[i])->m_joint);
    for (int32_t i = (int32_t) getNumParts(); i-- != 0; )
      physXScene->releaseActor(*((PartPhysX*)m_parts[i])->m_actor);
    for (int32_t i = (int32_t) getNumMaterials(); i-- != 0; )
      physXScene->releaseMaterial(*m_materials[i]);
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigPhysX2::PartPhysX::PartPhysX()
{
  m_userData = NULL;
  m_parentPartIndex = -1;
  m_isBeingKeyframed = false;
  m_SKDeviation = 0.0f;
  m_SKDeviationAngle = 0.0f;
  m_dirtyFlags = 0;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigPhysX2::PartPhysX::~PartPhysX()
{
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigPhysX2::PartPhysX::PartPhysX(const PhysicsRigPhysX2::PartPhysX& other)
{
  *this = other;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigPhysX2::PartPhysX& PhysicsRigPhysX2::PartPhysX::operator=(const PhysicsRigPhysX2::PartPhysX& other)
{
  if (this == &other)
    return *this;

  m_actor = other.m_actor;
  m_parentPartIndex = other.m_parentPartIndex;
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Matrix34 PhysicsRigPhysX2::PartPhysX::getCOMOffsetLocal() const
{
  return m_cache.COMOffsetLocal;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 PhysicsRigPhysX2::PartPhysX::getCOMPosition() const
{
  return m_cache.COMPosition;
}

//----------------------------------------------------------------------------------------------------------------------
float PhysicsRigPhysX2::PartPhysX::getMass() const
{
  return m_actor->getMass();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 PhysicsRigPhysX2::PartPhysX::getPosition() const
{
  return getTransform().translation();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 PhysicsRigPhysX2::PartPhysX::getVel() const
{
  return nmNxVec3ToVector3(m_actor->getLinearVelocity());
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 PhysicsRigPhysX2::PartPhysX::getAngVel() const
{
  return nmNxVec3ToVector3(m_actor->getAngularVelocity());
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 PhysicsRigPhysX2::PartPhysX::getVelocityAtPoint(const NMP::Vector3& point) const
{
  return nmNxVec3ToVector3(m_actor->getPointVelocity(nmVector3ToNxVec3(point)));
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 PhysicsRigPhysX2::PartPhysX::getAngularMomentum() const
{
  return nmNxVec3ToVector3(m_actor->getAngularMomentum());
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 PhysicsRigPhysX2::PartPhysX::getLinearMomentum() const
{
  // this appears to be a bug in the physX
  //return nmNxVec3ToVector3(m_actor->getLinearMomentum());
  // for now this is what we'll do
  return getVel() * getMass();
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::PartPhysX::applyModifiedValues()
{
  if (m_dirtyFlags)
  {
    if (m_dirtyFlags & kDirty_BodyFlags)
    {
      m_actor->raiseBodyFlag((NxBodyFlag)(m_cache.bodyFlags & (NX_BF_KINEMATIC)));
      m_actor->clearBodyFlag((NxBodyFlag)(~m_cache.bodyFlags & (NX_BF_KINEMATIC)));
    }

    if (m_dirtyFlags & kDirty_ActorFlags)
    {
      m_actor->raiseActorFlag((NxActorFlag)(m_cache.actorFlags & (NX_AF_DISABLE_COLLISION)));
      m_actor->clearActorFlag((NxActorFlag)(~m_cache.actorFlags & (NX_AF_DISABLE_COLLISION)));
    }

    if (m_dirtyFlags & kDirty_GlobalPose)
    {
      m_actor->setGlobalPose(nmMatrix34ToNxMat34(m_cache.globalPose));
    }

    // Move the kinematic target if the body was made kinematic, set the velocities otherwise.
    if (m_cache.bodyFlags & (NX_BF_KINEMATIC))
    {
      if (m_dirtyFlags & kDirty_KinematicTarget)
      {
        m_actor->moveGlobalPose(nmMatrix34ToNxMat34(m_cache.kinematicTarget));
      }
    }
    else
    {
      if (m_dirtyFlags & kDirty_LinearVel)
      {
        m_actor->setLinearVelocity(NxVec3(m_cache.linearVel.x, m_cache.linearVel.y, m_cache.linearVel.z));
      }

      if (m_dirtyFlags & kDirty_AngularVel)
      {
        m_actor->setAngularVelocity(NxVec3(m_cache.angularVel.x, m_cache.angularVel.y, m_cache.angularVel.z));
      }
    }

    m_dirtyFlags = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::PartPhysX::generateCachedValues()
{
  m_cache.mass = m_actor->getMass();
  m_cache.COMPosition = nmNxVec3ToVector3(m_actor->getCMassGlobalPosition());
  m_cache.COMVelocity = nmNxVec3ToVector3(m_actor->getLocalPointVelocity(m_actor->getCMassLocalPosition()));
  m_cache.angularMomentum = nmNxVec3ToVector3(m_actor->getAngularMomentum());
  m_cache.COMOffsetLocal = nmNxMat34ToNmMatrix34(m_actor->getCMassLocalPose());
  m_cache.globalInertiaTensor = nmNxMat33ToNmMatrix34(m_actor->getGlobalInertiaTensor());
  m_cache.globalInertiaTensor.translation() = m_cache.COMPosition;
  m_cache.globalPose = nmNxMat34ToNmMatrix34(m_actor->getGlobalPose());
  m_cache.kinematicTarget = m_cache.globalPose;

  m_cache.bodyFlags = 0;
  m_cache.bodyFlags |= m_actor->readBodyFlag(NX_BF_KINEMATIC) ? NX_BF_KINEMATIC : 0;

  m_cache.actorFlags = 0;
  m_cache.actorFlags |= m_actor->readActorFlag(NX_AF_DISABLE_COLLISION) ? NX_AF_DISABLE_COLLISION : 0;
}

//----------------------------------------------------------------------------------------------------------------------
// inertia and summations thereof
//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3  PhysicsRigPhysX2::PartPhysX::getMassSpaceInertiaTensor() const
{
  NMP_ASSERT_FAIL_MSG("This function should never get called");
  return NMP::Vector3::InitZero;
}

//----------------------------------------------------------------------------------------------------------------------
// returns the mass moment of inertia in the top 3x3 components
// along with the com position in the translation component
NMP::Matrix34 PhysicsRigPhysX2::PartPhysX::getGlobalInertiaTensor() const
{
  NMP::Matrix34 result(nmNxMat33ToNmMatrix34(m_actor->getGlobalInertiaTensor()));
  result.translation() = nmNxVec3ToVector3(m_actor->getCMassGlobalPosition());
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// given the inertia tensor I for a given body or system of bodies, where I is the moment
// wrt the centre of mass of the body (or system), the moment about a point displaced
// from the centre is easily computed by adding a correction I(offset) say.
// so if J is the corrected inertia then, J = I + I(offset)
// and [by parallel axis theorem], "ij"th coordinate is given by:
//
// J_ij = I_ij + m(r_k * r_k * d_ij - r_i * r_j)
//
// where,
// m is the total mass of the body (or system of bodies)
// r = com - x, offset of the body's com from the given point x
// d_ij the kroenecker delta function (= 1 for i == j, = 0 otherwise)
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
NMP::Quat PhysicsRigPhysX2::PartPhysX::getQuaternion() const
{
  return getTransform().toQuat();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Matrix34 PhysicsRigPhysX2::PartPhysX::getTransform() const
{
  // The transform is treated in a special way. The value is taken from the cache, which means
  // that it would be either the value generated during the post physics update or the result of a 'setTransform'
  // or 'moveTo' command. It's been implemented this way because physx2 exposes the effects of setGlobalPose/moveGlobalPose
  // directly even if a physics step doesn't happen in between a set and a get call. Using the cache still allows us to call
  // setTransform from multiple threads, plus to call it several times before the values are sent to physx. 

  if ((m_cache.bodyFlags & NX_BF_KINEMATIC) != 0)
  {
    return m_cache.kinematicTarget;
  }

  return m_cache.globalPose;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::PartPhysX::moveTo(const NMP::Matrix34& tm)
{
  m_dirtyFlags |= kDirty_KinematicTarget;
  m_cache.kinematicTarget = tm;

  updateCOMPosition();
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::PartPhysX::setPosition(const NMP::Vector3& p)
{
  m_dirtyFlags |= kDirty_GlobalPose;
  m_cache.globalPose.translation() = p;

  updateCOMPosition();
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::PartPhysX::setQuaternion(const NMP::Quat& q)
{
  m_dirtyFlags |= kDirty_GlobalPose;
  m_cache.globalPose = NMP::Matrix34(q, m_cache.globalPose.translation());

  updateCOMPosition();
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::PartPhysX::setVel(const NMP::Vector3& v)
{
  m_dirtyFlags |= kDirty_LinearVel;
  m_cache.linearVel = v;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::PartPhysX::setAngVel(const NMP::Vector3& r)
{
  m_dirtyFlags |= kDirty_AngularVel;
  m_cache.angularVel = r;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::PartPhysX::updateCOMPosition()
{
  // Changing the transform affects the global com, too.
  m_cache.COMPosition = nmNxVec3ToVector3(
      nmMatrix34ToNxMat34(getTransform()) * nmVector3ToNxVec3(m_cache.COMOffsetLocal.translation()));

}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::PartPhysX::setTransform(const NMP::Matrix34& tm)
{
  m_dirtyFlags |= kDirty_GlobalPose;
  m_cache.globalPose = tm;

  updateCOMPosition();
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::PartPhysX::makeKinematic(bool kinematic, float NMP_UNUSED(massMultiplier), bool NMP_UNUSED(enableConstraint))
{
  if (!kinematic)
    m_isBeingKeyframed = false;

  if (kinematic == isKinematic())
    return;

  if (kinematic)
  {
    // Raising the flag if it's already raised might be bad for performance (not checked fully)
    m_dirtyFlags |= kDirty_BodyFlags;
    m_cache.bodyFlags |= NX_BF_KINEMATIC;
  }
  else
  {
    // Raising the flag if it's already raised might be bad for performance (not checked fully)
    m_dirtyFlags |= kDirty_BodyFlags;
    m_cache.bodyFlags &= ~NX_BF_KINEMATIC;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsRigPhysX2::PartPhysX::isKinematic() const
{
  return ((m_cache.bodyFlags & NX_BF_KINEMATIC) != 0);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::PartPhysX::enableCollision(bool enable)
{
  m_dirtyFlags |= kDirty_ActorFlags;
  if (enable)
    m_cache.actorFlags &= ~NX_AF_DISABLE_COLLISION;
  else
    m_cache.actorFlags |= NX_AF_DISABLE_COLLISION;
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsRigPhysX2::PartPhysX::getCollisionEnabled() const
{
  return (m_cache.actorFlags | NX_AF_DISABLE_COLLISION) == 0;
}

//----------------------------------------------------------------------------------------------------------------------
float PhysicsRigPhysX2::PartPhysX::getSKDeviation() const
{
  // Note that the deviation is only set when actually being soft keyframed
  if (!m_isBeingKeyframed || isKinematic())
    return 0.0;
  return m_SKDeviation;
}

//----------------------------------------------------------------------------------------------------------------------
float PhysicsRigPhysX2::PartPhysX::getSKDeviationAngle() const
{
  // Note that the deviation is only set when actually being soft keyframed
  if (!m_isBeingKeyframed || isKinematic())
    return 0.0;
  return m_SKDeviationAngle;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::PartPhysX::addVelocityChange(
  const NMP::Vector3& NMP_UNUSED(velChange), const NMP::Vector3& NMP_UNUSED(worldPos), float NMP_UNUSED(angularMultiplier))
{
  NMP_ASSERT_FAIL_MSG("This function should never get called");
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::PartPhysX::addImpulse(const NMP::Vector3 &NMP_UNUSED(impulse))
{
  NMP_ASSERT_FAIL_MSG("This function should never get called");
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::PartPhysX::addTorqueImpulse(const NMP::Vector3& NMP_UNUSED(torqueImpulse))
{
  NMP_ASSERT_FAIL_MSG("This function should never get called");
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::PartPhysX::addTorque(const NMP::Vector3& NMP_UNUSED(torque))
{
  NMP_ASSERT_FAIL_MSG("This function should never get called");
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::PartPhysX::addForce(const NMP::Vector3 &NMP_UNUSED(force))
{
  NMP_ASSERT_FAIL_MSG("This function should never get called");
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::PartPhysX::addLinearVelocityChange(const NMP::Vector3& NMP_UNUSED(velChange))
{
  NMP_ASSERT_FAIL_MSG("This function should never get called");
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::PartPhysX::addAngularAcceleration(const NMP::Vector3& NMP_UNUSED(angularAcceleration))
{
  NMP_ASSERT_FAIL_MSG("This function should never get called");
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsRigPhysX2::PartPhysX::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(getPosition());
  savedState.addValue(getQuaternion());
  savedState.addValue(getVel());
  savedState.addValue(getAngVel());
  savedState.addValue(getCachedData());
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsRigPhysX2::PartPhysX::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  setPosition(savedState.getValue<NMP::Vector3>());
  setQuaternion(savedState.getValue<NMP::Quat>());
  setVel(savedState.getValue<NMP::Vector3>());
  setAngVel(savedState.getValue<NMP::Vector3>());
  setCachedData(savedState.getValue<MR::PhysicsRigPhysX2::PartPhysX::CachedData>());
  return true;
}

#if defined(MR_OUTPUT_DEBUGGING)
//----------------------------------------------------------------------------------------------------------------------
uint32_t PhysicsRigPhysX2::PartPhysX::serializeTxPersistentData(uint16_t nameToken, uint32_t objectID, void* outputBuffer, uint32_t NMP_USED_FOR_ASSERTS(outputBufferSize)) const
{
  uint32_t dataSize = sizeof(PhysicsPartPersistentData);

  uint32_t numBoxes = 0;
  uint32_t numCapsules = 0;
  uint32_t numSpheres = 0;

  const NxU32 numShapes = m_actor->getNbShapes();
  const NxShape * const *shapes = m_actor->getShapes();
  for (NxU32 i = 0; i != numShapes; ++i)
  {
    const NxShape *shape = shapes[i];

    NxShapeType type = shape->getType();
    switch (type)
    {
    case NX_SHAPE_SPHERE:
      ++numSpheres;
      break;
    case NX_SHAPE_BOX:
      ++numBoxes;
      break;
    case NX_SHAPE_CAPSULE:
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
    yToZ.fromEulerXYZ(NMP::Vector3(-NM_PI_OVER_TWO, 0, 0));

    uint32_t indexBox = 0;
    uint32_t indexCapsule = 0;
    uint32_t indexSphere = 0;
    for (NxU32 i = 0; i != numShapes; ++i)
    {
      const NxShape *nxShape = shapes[i];

      NxShapeType type = nxShape->getType();
      switch (type)
      {
      case NX_SHAPE_SPHERE:
        {
          const NxSphereShape *nxSphere = nxShape->isSphere();
          NMP_ASSERT(nxSphere != 0);

          PhysicsSpherePersistentData* persistentData = partPersistentData->getSphere(indexSphere);

          NxMat34 localPose = nxSphere->getLocalPose();
          persistentData->m_localPose = nmNxMat34ToNmMatrix34(localPose);

          persistentData->m_parentIndex = i;
          persistentData->m_radius = nxSphere->getRadius();

          NMP::netEndianSwap(persistentData->m_parentIndex);
          NMP::netEndianSwap(persistentData->m_localPose);
          NMP::netEndianSwap(persistentData->m_radius);

          ++indexSphere;
          break;
        }
      case NX_SHAPE_BOX:
        {
          const NxBoxShape *nxBox = nxShape->isBox();
          NMP_ASSERT(nxBox != 0);

          PhysicsBoxPersistentData* persistentData = partPersistentData->getBox(indexBox);

          NxMat34 localPose = nxBox->getLocalPose();
          persistentData->m_localPose = nmNxMat34ToNmMatrix34(localPose);

          // mult by 2 to counter the halving in RigDefBuilderPhysX.cpp
          // physicsRigDefPhysX->m_parts[i].volume.boxes[j].dimensions = box->getDimensions() * 0.5f;
          NxVec3 dimensions = nxBox->getDimensions();
          persistentData->m_width = 2.0f * dimensions.x;
          persistentData->m_height = 2.0f * dimensions.y;
          persistentData->m_depth = 2.0f * dimensions.z;

          persistentData->m_parentIndex = i;
          NMP::netEndianSwap(persistentData->m_parentIndex);
          NMP::netEndianSwap(persistentData->m_localPose);
          NMP::netEndianSwap(persistentData->m_width);
          NMP::netEndianSwap(persistentData->m_height);
          NMP::netEndianSwap(persistentData->m_depth);

          ++indexBox;
          break;
        }
      case NX_SHAPE_CAPSULE:
        {
          const NxCapsuleShape *nxCapsule = nxShape->isCapsule();
          NMP_ASSERT(nxCapsule != 0);

          PhysicsCapsulePersistentData* persistentData = partPersistentData->getCapsule(indexCapsule);

          NxMat34 localPose = nxCapsule->getLocalPose();
          persistentData->m_localPose = yToZ * nmNxMat34ToNmMatrix34(localPose);

          persistentData->m_radius = nxCapsule->getRadius();

          persistentData->m_height = nxCapsule->getHeight();

          persistentData->m_parentIndex = i;
          NMP::netEndianSwap(persistentData->m_parentIndex);
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
uint32_t PhysicsRigPhysX2::PartPhysX::serializeTxFrameData(void* outputBuffer, uint32_t NMP_USED_FOR_ASSERTS(outputBufferSize)) const
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
#endif // MR_OUTPUT_DEBUGGING
//----------------------------------------------------------------------------------------------------------------------
PhysicsRigPhysX2::JointPhysX::JointPhysX()
{
  m_dirtyFlags = 0;

#if defined(MR_OUTPUT_DEBUGGING)
  // Initialise serialization data.
  updateSerializeTxFrameData();
#endif // MR_OUTPUT_DEBUGGING
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsRigPhysX2::JointPhysX::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  NxD6JointDesc jointDesc;
  NxD6Joint* d6 = (NxD6Joint*) m_joint;
  d6->saveToDesc(jointDesc);
  jointDesc.actor[0] = jointDesc.actor[1] = 0;
  savedState.addValue(jointDesc);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsRigPhysX2::JointPhysX::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  NxD6Joint* d6 = (NxD6Joint*) m_joint;
  NxD6JointDesc jointDesc = savedState.getValue<NxD6JointDesc>();
  d6->loadFromDesc(jointDesc);
  return true;
}

#if defined(MR_OUTPUT_DEBUGGING)
//----------------------------------------------------------------------------------------------------------------------
uint32_t PhysicsRigPhysX2::JointPhysX::serializeTxPersistentData(
  const PhysicsJointDef* jointDef,
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

    persistentData->m_swing1Limit = m_cache.jointDesc.swing1Limit.value;
    persistentData->m_swing2Limit = m_cache.jointDesc.swing2Limit.value;
    persistentData->m_twistLimitLow = m_cache.jointDesc.twistLimit.low.value;
    persistentData->m_twistLimitHigh = m_cache.jointDesc.twistLimit.high.value;
    persistentData->m_nameToken = stringToken;

    PhysicsSixDOFJointPersistentData::endianSwap(persistentData);
  }
  return dataSize;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t PhysicsRigPhysX2::JointPhysX::serializeTxFrameData(
  void*     outputBuffer,
  uint32_t  NMP_USED_FOR_ASSERTS(outputBufferSize)) const
{
  uint32_t dataSize = sizeof(PhysicsSixDOFJointFrameData);

  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);

    PhysicsSixDOFJointFrameData* frameData = reinterpret_cast<PhysicsSixDOFJointFrameData*>(outputBuffer);

    *frameData = m_serializeTxFrameData;

    PhysicsSixDOFJointFrameData::endianSwap(frameData);
  }

  return dataSize;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::JointPhysX::updateSerializeTxFrameData()
{
  m_serializeTxFrameData.m_jointType = PhysicsJointFrameData::JOINT_TYPE_SIX_DOF;

  m_serializeTxFrameData.m_swing1Limit = m_modifiableLimits.getSwing1Limit();
  m_serializeTxFrameData.m_swing2Limit = m_modifiableLimits.getSwing2Limit();
  m_serializeTxFrameData.m_twistLimitLow = m_modifiableLimits.getTwistLimitLow();
  m_serializeTxFrameData.m_twistLimitHigh = m_modifiableLimits.getTwistLimitHigh();
}
#endif

//----------------------------------------------------------------------------------------------------------------------
NMP::Quat PhysicsRigPhysX2::JointPhysX::getRotation(
  const PhysicsJointDef* jointDef,
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
void PhysicsRigPhysX2::JointPhysX::enableLimit(bool enable)
{
  if (enable == m_limitsEnabled)
    return;

  m_limitsEnabled = enable;

  m_cache.jointDesc.swing1Motion = enable ? NX_D6JOINT_MOTION_LIMITED : NX_D6JOINT_MOTION_FREE;
  m_cache.jointDesc.swing2Motion = enable ? NX_D6JOINT_MOTION_LIMITED : NX_D6JOINT_MOTION_FREE;
  m_cache.jointDesc.twistMotion = enable ? NX_D6JOINT_MOTION_LIMITED : NX_D6JOINT_MOTION_FREE;

  m_dirtyFlags |= kDirty_Desc;
}

void PhysicsRigPhysX2::JointPhysX::setDriveOrientation(const NMP::Quat &quat)
{
  // This is what allows unchanging drive to go to sleep, since setDriveOrientation wakes up regardless of whether the
  // drive has changed
  if (quat != m_cache.driveOrientation)
  {
    m_dirtyFlags |= kDirty_DriveOrientation;
    m_cache.driveOrientation = quat;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::JointPhysX::setDriveStrength(float twistStrength, float swingStrength, float slerpStrength)
{
  if (m_joint->getType() == NX_JOINT_D6)
  {
    bool descChanged = false;
    NxD6JointDesc& jointDesc = m_cache.jointDesc;
    descChanged |= NMRU::GeomUtils::changeValueIfDifferent(jointDesc.swingDrive.spring, swingStrength, 0.000001f);
    descChanged |= NMRU::GeomUtils::changeValueIfDifferent(jointDesc.twistDrive.spring, twistStrength, 0.000001f);
    descChanged |= NMRU::GeomUtils::changeValueIfDifferent(jointDesc.slerpDrive.spring, slerpStrength, 0.000001f);
    if (descChanged)
    {
      m_dirtyFlags |= kDirty_Desc;
    }
  }  
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::JointPhysX::getDriveParams(
  float& twistStrength, float& swingStrength, float& slerpStrength,
  float& twistDamping, float& swingDamping, float& slerpDamping)
{
  NMP_ASSERT(m_joint->getType() == NX_JOINT_D6);

  NxD6Joint* d6 = (NxD6Joint*) m_joint;
  NxD6JointDesc desc;
  d6->saveToDesc(desc);
  twistStrength = desc.twistDrive.spring;
  swingStrength = desc.swingDrive.spring;
  slerpStrength = desc.slerpDrive.spring;

  twistDamping = desc.twistDrive.damping;
  swingDamping = desc.swingDrive.damping;
  slerpDamping = desc.slerpDrive.damping;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::JointPhysX::setDriveDamping(float twistDamping, float swingDamping, float slerpDamping)
{
  if (m_joint->getType() == NX_JOINT_D6)
  {
    bool descChanged = false;
    NxD6JointDesc& jointDesc = m_cache.jointDesc;
    descChanged |= NMRU::GeomUtils::changeValueIfDifferent(jointDesc.swingDrive.damping, swingDamping, 0.000001f);
    descChanged |= NMRU::GeomUtils::changeValueIfDifferent(jointDesc.twistDrive.damping, twistDamping, 0.000001f);
    descChanged |= NMRU::GeomUtils::changeValueIfDifferent(jointDesc.slerpDrive.damping, slerpDamping, 0.000001f);
    if (descChanged)
    {
      m_dirtyFlags |= kDirty_Desc;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::JointPhysX::setSlerpDriveStrengthAndDamping(float slerpStrength, float slerpDamping)
{
  if (m_joint->getType() == NX_JOINT_D6)
  {
    bool descChanged = false;
    NxD6JointDesc& jointDesc = m_cache.jointDesc;
    descChanged |= NMRU::GeomUtils::changeValueIfDifferent(jointDesc.swingDrive.spring, 0.0f, 0.000001f);
    descChanged |= NMRU::GeomUtils::changeValueIfDifferent(jointDesc.twistDrive.spring, 0.0f, 0.000001f);
    descChanged |= NMRU::GeomUtils::changeValueIfDifferent(jointDesc.slerpDrive.spring, slerpStrength, 0.000001f);
    descChanged |= NMRU::GeomUtils::changeValueIfDifferent(jointDesc.swingDrive.damping, 0.0f, 0.000001f);
    descChanged |= NMRU::GeomUtils::changeValueIfDifferent(jointDesc.twistDrive.damping, 0.0f, 0.000001f);
    descChanged |= NMRU::GeomUtils::changeValueIfDifferent(jointDesc.slerpDrive.damping, slerpDamping, 0.000001f);
    if (descChanged)
    {
      m_dirtyFlags |= kDirty_Desc;
    }
  }

#ifdef NM_DEBUG
  if (m_joint->getType() == NX_JOINT_D6)
  {
    NxD6Joint* d6 = (NxD6Joint*) m_joint;
    NxD6JointDesc desc;
    d6->saveToDesc(desc);
    NxD6JointDesc& jointDesc = m_cache.jointDesc;
    NMP_ASSERT(fabsf(desc.slerpDrive.spring - jointDesc.slerpDrive.spring) < 0.001f);
    NMP_ASSERT(fabsf(desc.slerpDrive.damping - jointDesc.slerpDrive.damping) < 0.001f);
  }
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::JointPhysX::setStrength(float NMP_UNUSED(strength))
{
  NMP_ASSERT_FAIL_MSG("This function should never get called");
}

//----------------------------------------------------------------------------------------------------------------------
float PhysicsRigPhysX2::JointPhysX::getStrength() const
{
  NMP_ASSERT_FAIL_MSG("This function should never get called");
  return 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::JointPhysX::setDamping(float NMP_UNUSED(damping))
{
  NMP_ASSERT_FAIL_MSG("This function should never get called");
}

//----------------------------------------------------------------------------------------------------------------------
float PhysicsRigPhysX2::JointPhysX::getDamping() const
{
  NMP_ASSERT_FAIL_MSG("This function should never get called");
  return 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Quat PhysicsRigPhysX2::JointPhysX::getTargetOrientation()
{
  NMP_ASSERT_FAIL_MSG("This function should never get called");
  return NMP::Quat::kIdentity;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::JointPhysX::setTargetOrientation(const NMP::Quat &NMP_UNUSED(orientation))
{
  NMP_ASSERT_FAIL_MSG("This function should never get called");
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::JointPhysX::applyModifiedValues(bool enableProjection, float linearTolerance, float angularTolerance)
{
  if ((m_cache.jointDesc.projectionMode == NX_JPM_NONE) == enableProjection)
  {
    m_cache.jointDesc.projectionMode = enableProjection ? NX_JPM_POINT_MINDIST : NX_JPM_NONE;
    m_dirtyFlags |= kDirty_Desc;
  }
  if (m_cache.jointDesc.projectionDistance != linearTolerance)
  {
    m_cache.jointDesc.projectionDistance = linearTolerance;
    m_dirtyFlags |= kDirty_Desc;
  }
  if (m_cache.jointDesc.projectionAngle != angularTolerance)
  {
    m_cache.jointDesc.projectionAngle = angularTolerance;
    m_dirtyFlags |= kDirty_Desc;
  }

  // Optimise this with MORPH-16668 - In PhysX2 the only way to modify the limits is to update it
  // with a new descriptor, which is very slow. Now dirty flag for the limits yet, so force an
  // update.
  if (
    m_cache.jointDesc.swing1Limit.value != m_modifiableLimits.getSwing1Limit() ||
    m_cache.jointDesc.swing2Limit.value != m_modifiableLimits.getSwing2Limit() ||
    m_cache.jointDesc.twistLimit.low.value != m_modifiableLimits.getTwistLimitLow() ||
    m_cache.jointDesc.twistLimit.high.value != m_modifiableLimits.getTwistLimitHigh()
  )
  {
    m_dirtyFlags |= kDirty_Desc;

    m_cache.jointDesc.swing1Limit.value = m_modifiableLimits.getSwing1Limit();
    m_cache.jointDesc.swing2Limit.value = m_modifiableLimits.getSwing2Limit();
    m_cache.jointDesc.twistLimit.low.value = m_modifiableLimits.getTwistLimitLow();
    m_cache.jointDesc.twistLimit.high.value = m_modifiableLimits.getTwistLimitHigh();
  }

  if (m_joint && m_dirtyFlags && m_joint->getType() == NX_JOINT_D6)
  {
    NxD6Joint *d6 = (NxD6Joint*) m_joint;
    NxD6JointDesc& jointDesc = m_cache.jointDesc;
    if (m_dirtyFlags & kDirty_Desc)
    {
      d6->loadFromDesc(jointDesc);
    }
    if (m_dirtyFlags & kDirty_DriveOrientation)
    {
      d6->setDriveOrientation(MR::nmQuatToNxQuat(m_cache.driveOrientation));
    }

    m_dirtyFlags = 0;
  }

#if defined(MR_OUTPUT_DEBUGGING)
  updateSerializeTxFrameData();
#endif // MR_OUTPUT_DEBUGGING
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::JointPhysX::generateCachedValues()
{
  if (m_joint && m_joint->getType() == NX_JOINT_D6)
  {
    NxD6Joint *d6 = (NxD6Joint*) m_joint;
    d6->saveToDesc(m_cache.jointDesc);
    m_cache.driveOrientation = nmNxQuatToQuat(m_cache.jointDesc.driveOrientation);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::JointPhysX::clampToLimits(
  NMP::Quat& orientation,
  float limitFrac,
  const NMP::Quat* origQ) const
{
  if (origQ)
    m_modifiableLimits.clampToLimits(orientation, limitFrac, *m_def, *origQ);
  else
    m_modifiableLimits.clampToLimits(orientation, limitFrac, *m_def);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::JointPhysX::expandLimits(const NMP::Quat& orientation)
{
  m_modifiableLimits.expand(orientation, *m_def);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::JointPhysX::scaleLimits(float scaleFactor)
{
  m_modifiableLimits.scale(scaleFactor);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::JointPhysX::resetLimits()
{
#if defined(MR_OUTPUT_DEBUGGING)
  // Copy current limits used this frame into frame data cache before they are reset so that they can be made availible
  // to the debug render system later in the update.
  updateSerializeTxFrameData();
#endif // MR_OUTPUT_DEBUGGING

  m_modifiableLimits = m_def->m_hardLimits.getModifiableLimits();
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::JointPhysX::writeLimits()
{

}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::removeFromScene()
{
  // TODO: there is no add/delete from scene in physX2, just create/release which is presumably slower and allocates/deallocates more memory
  // For the moment this function is disabled until we find the correct solution
  makeKinematic(true);
}
//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::addToScene()
{
  // TODO: there is no add/delete from scene in physX2, just create/release which is presumably slower and allocates/deallocates more memory
  // For the moment this function is disabled until we find the correct solution
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::makeKinematic(bool moveToKinematicPos)
{
  NMP_ASSERT(m_refCount == 0);
  for (uint32_t i = 0; i < getNumParts(); ++i)
  {
    PartPhysX* part = (PartPhysX*)m_parts[i];
    part->makeKinematic(true, 1.0f, false);
    part->enableCollision(false);
    if (moveToKinematicPos)
    {
      NMP::Matrix34 tm = part->getTransform();
      tm.translation() = m_kinematicPos;
      part->moveTo(tm);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::makeDynamic()
{
  for (uint32_t i = 0; i < getNumParts(); ++i)
  {
    PartPhysX* part = (PartPhysX*)m_parts[i];
    part->makeKinematic(false, 1.0f, false);
    part->enableCollision(true);
  }

  for (uint32_t i = 0; i < getNumJoints(); ++i)
  {
    PhysicsRigPhysX2::JointPhysX* joint = (JointPhysX*)getJoint(i);
    // enable the joint limit
    joint->enableLimit(true);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::restoreAllJointDrivesToDefault()
{
  for (uint32_t i = 0; i < getNumJoints(); ++i)
  {
    JointPhysX* joint = (JointPhysX*)m_joints[i];
    joint->setDriveStrength(0.0f, 0.0f, 0.0f);
    joint->setDriveDamping(joint->getMaxTwistDamping(), joint->getMaxSwingDamping(), joint->getMaxSlerpDamping());

    bool descChanged = false;
    NxD6JointDesc& jointDesc = joint->m_cache.jointDesc;
    descChanged |= NMRU::GeomUtils::changeValueIfDifferent(jointDesc.swing1Motion , NX_D6JOINT_MOTION_LIMITED);
    descChanged |= NMRU::GeomUtils::changeValueIfDifferent(jointDesc.swing2Motion,  NX_D6JOINT_MOTION_LIMITED);
    descChanged |= NMRU::GeomUtils::changeValueIfDifferent(jointDesc.twistMotion,   NX_D6JOINT_MOTION_LIMITED);

    if (descChanged)
      joint->m_dirtyFlags |= JointPhysX::kDirty_Desc;
  }
  m_enableJointProjection = false;
  m_desiredJointProjectionLinearTolerance = FLT_MAX;
  m_desiredJointProjectionAngularTolerance = NM_PI;
}

//----------------------------------------------------------------------------------------------------------------------
float PhysicsRigPhysX2::getMaxSKDeviation() const
{
  float maxDeviation = 0.0f;
  for (uint32_t i = 0; i < getNumParts(); ++i)
  {
    PhysicsRigPhysX2::PartPhysX *part = (PartPhysX*)m_parts[i];
    float SKDeviation = part->getSKDeviation();

    if (SKDeviation > maxDeviation)
      maxDeviation = SKDeviation;
  }
  return maxDeviation;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::requestJointProjectionParameters(int iterations, float linearTolerance, float angularTolerance)
{
  if (iterations != 0)
  {
    m_enableJointProjection = true;
  }
  if (linearTolerance < m_desiredJointProjectionLinearTolerance)
  {
    m_desiredJointProjectionLinearTolerance = linearTolerance;
  }
  if (angularTolerance < m_desiredJointProjectionAngularTolerance)
  {
    m_desiredJointProjectionAngularTolerance = angularTolerance;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::applyHardKeyframing(
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
  NMP_ASSERT_MSG(m_physicsRigDef != NULL, "No RigDef exists (anymore)! Check AnimationSets to have physics rigs defined!");

  for (uint32_t i = 0; i < getNumParts(); ++i)
  {
    if (!partChooser.usePart(i))
      continue;

    PartPhysX* part = (PartPhysX*)m_parts[i];
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
void PhysicsRigPhysX2::applySoftKeyframing(
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
  NMP_ASSERT_MSG(m_physicsRigDef != NULL, "No RigDef existing (anymore)! Check AnimationSets to have physics rigs defined!");

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

    PartPhysX* part = (PartPhysX*)m_parts[partIndex];
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
          JointPhysX* joint = (JointPhysX*)getJoint(parentJointIndex);
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

    // calculate the motion of the target itself
    NMP::Matrix34 invTargetTMOld(targetTMOld); invTargetTMOld.invertFast();
    NMP::Matrix34 targetMotionTM = invOffsetTM * invTargetTMOld * targetTM * offsetTM;

    // Store the distance/angle error
    part->m_SKDeviation = motionTM.translation().magnitude();
    part->m_SKDeviationAngle = motionTM.toRotationVector().magnitude();

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
      PhysicsRigPhysX2::PartPhysX *part = (PartPhysX*)m_parts[i];
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
void PhysicsRigPhysX2::applyActiveAnimation(
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
  NMP_ASSERT_MSG(m_physicsRigDef != NULL, "No RigDef existing (anymore)! Check AnimationSets to have physics rigs defined!");

  for (uint32_t i = 0; i < getNumJoints(); ++i)
  {
    if (!jointChooser.useJoint(i))
      continue;

    JointPhysX* joint = (JointPhysX*)m_joints[i];
    const PhysicsJointDef* jointDef = m_physicsRigDef->m_joints[i];

    PartPhysX* childPart = (PartPhysX*)m_parts[jointDef->m_childPartIndex];
    childPart->makeKinematic(false, 1.0f, false);
    childPart->m_isBeingKeyframed = false;
    // don't force either of the parts to have collision - no way we could know which one _should_
    // have collision if it's disabled elsewhere.

    bool descChanged = false;
    float tol = 0.0001f;
    NxD6JointDesc& jointDesc = joint->m_cache.jointDesc;
    descChanged |= NMRU::GeomUtils::changeValueIfDifferent(jointDesc.twistDrive.spring, joint->getMaxTwistStrength() * strengthMultiplier, tol);
    descChanged |= NMRU::GeomUtils::changeValueIfDifferent(jointDesc.swingDrive.spring, joint->getMaxSwingStrength() * strengthMultiplier, tol);
    descChanged |= NMRU::GeomUtils::changeValueIfDifferent(jointDesc.slerpDrive.spring, joint->getMaxSlerpStrength() * strengthMultiplier, tol);
    descChanged |= NMRU::GeomUtils::changeValueIfDifferent(jointDesc.twistDrive.damping, joint->getMaxTwistDamping() * dampingMultiplier, tol);
    descChanged |= NMRU::GeomUtils::changeValueIfDifferent(jointDesc.swingDrive.damping, joint->getMaxSwingDamping() * dampingMultiplier, tol);
    descChanged |= NMRU::GeomUtils::changeValueIfDifferent(jointDesc.slerpDrive.damping, joint->getMaxSlerpDamping() * dampingMultiplier, tol);

    if (descChanged)
      joint->m_dirtyFlags |= JointPhysX::kDirty_Desc;

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
    NMP::Quat l1Inv = ~l1;

    // target orientations outside the limits cause oscillations when physical limits are enabled
    if (limitClampFraction >= 0.0f)
    {
      joint->clampToLimits(curQ, limitClampFraction, NULL);
    }

    NMP::Quat curFrameQ = l0Inv * curQ * l1;
    joint->setDriveOrientation(curFrameQ);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::applyActiveAnimation(uint32_t jointIndex, const NMP::Quat& targetQuat, bool makeChildDynamic)
{
  NMP_ASSERT(jointIndex < getNumJoints());
  JointPhysX* joint = (JointPhysX*)m_joints[jointIndex];
  const PhysicsJointDef* jointDef = m_physicsRigDef->m_joints[jointIndex];
  if (makeChildDynamic)
  {
    PartPhysX* childPart = (PartPhysX*) m_parts[jointDef->m_childPartIndex];
    childPart->makeKinematic(false, 1.0f, false);
    childPart->m_isBeingKeyframed = false;
  }
  // don't force either of the parts to have collision - no way we could know which one _should_
  // have collision if it's disabled elsewhere.
  joint->setDriveOrientation(targetQuat);
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigPhysX2::PartPhysX* PhysicsRigPhysX2::getPartPhysX(uint32_t index)
{
  return (PhysicsRigPhysX2::PartPhysX*) getPart(index);
}

//----------------------------------------------------------------------------------------------------------------------
const PhysicsRigPhysX2::PartPhysX* PhysicsRigPhysX2::getPartPhysX(uint32_t index) const
{
  return (const PhysicsRigPhysX2::PartPhysX*) getPart(index);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::setSkinWidthIncrease(uint32_t NMP_UNUSED(partIndex), float NMP_UNUSED(skinWidthIncrease))
{
  NMP_ASSERT_FAIL_MSG("This function should never get called");
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::scaleFrictionOnPart(const int32_t partIndex, const float frictionScale)
{
  const MR::PhysicsRigDef::Part& partDef = m_physicsRigDef->m_parts[partIndex];

  // Build a list of all the material id's that belong to this part.
  int32_t materialID[MAX_SHAPES_IN_VOLUME];
  int32_t materialIDCount = 0;

  for (int32_t i = 0; i < partDef.volume.numSpheres; ++i)
  {
    materialID[materialIDCount++] = partDef.volume.spheres[i].materialID;
  }

  for (int32_t i = 0; i < partDef.volume.numBoxes; ++i)
  {
    materialID[materialIDCount++] = partDef.volume.boxes[i].materialID;
  }

  for (int32_t i = 0; i < partDef.volume.numCapsules; ++i)
  {
    materialID[materialIDCount++] = partDef.volume.capsules[i].materialID;
  }

  // Scale the static and dynamic friction on all the materials in the list.
  for (int32_t i = 0; i < materialIDCount; ++i)
  {
    NMP_ASSERT(materialID[i] < static_cast<int32_t>(m_physicsRigDef->getNumMaterials()));
    const MR::PhysicsRigDef::Part::Material& materialDef = m_physicsRigDef->m_materials[materialID[i]];

    NxMaterial* const material = m_materials[materialID[i]];
    material->setStaticFriction(materialDef.friction * frictionScale);
    PhysicsMaterialDriverDataPhysX2* driverData = static_cast<PhysicsMaterialDriverDataPhysX2*>(materialDef.driverData);
    material->setDynamicFriction(driverData->m_dynamicFriction * frictionScale);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::receiveImpulse(int32_t inputPartIndex,
                                      const NMP::Vector3& inputPosition,
                                      const NMP::Vector3& inputDirection,
                                      float inputLocalMagnitude,
                                      float inputLocalAngularMultiplier,
                                      float inputLocalResponseRatio,
                                      float inputFullBodyMagnitude,
                                      float inputFullBodyAngularMultiplier,
                                      float inputFullBodyLinearMultiplier,
                                      float inputFullBodyResponseRatio,
                                      bool positionInWorldSpace,
                                      bool directionInWorldSpace,
                                      bool applyAsVelocityChange)
{
  bool invalidInputPartIndex = inputPartIndex < 0 || inputPartIndex >= (int32_t) getNumParts();
  if (invalidInputPartIndex)
  {
    inputPartIndex = 0;
    inputLocalMagnitude = 0.0f;
  }

  MR::PhysicsRigPhysX2::PartPhysX* hitPart = getPartPhysX(inputPartIndex);
  NxActor* rigidBody = hitPart->getActor();

  NMP::Matrix34 actorToWorld = hitPart->getTransform();

  // Get the world space position
  NMP::Vector3 positionWorld = inputPosition;
  if (!positionInWorldSpace)
  {
    // Local space positions are specified relative to the COM, not the actor, position.
    NMP::Matrix34 actorCOMToWorld = actorToWorld;
    actorCOMToWorld.translation() = hitPart->getCOMPosition();
    actorCOMToWorld.transformVector(positionWorld);
  }

  // Get world space direction
  NMP::Vector3 directionWorld = inputDirection;
  if (!directionInWorldSpace)
  {
    actorToWorld.rotateVector(directionWorld);
  }

  float totalRigMass = calculateMass();
  float averagePartMass = totalRigMass / getNumParts();

  // Here we apply the impulse to the part, scaled according to distribution
  if (inputLocalMagnitude != 0.0f)
  {
    // World space impulse or velocity change
    NMP::Vector3 impulseWorld = directionWorld * inputLocalMagnitude;

    float hitPartMass = hitPart->getMass();
    float multiplier = powf(hitPartMass / averagePartMass, inputLocalResponseRatio);

    if (applyAsVelocityChange == false)
    {
      // impulse
      MR::addImpulseToActor(
        *rigidBody, 
        impulseWorld * multiplier, 
        positionWorld, 
        inputLocalAngularMultiplier);
    }
    else if (applyAsVelocityChange == true)
    {
      // velocity change
      MR::addVelocityChangeToActor(
        *rigidBody, 
        impulseWorld * multiplier, 
        positionWorld);
    }
  }

  // Apply the impulse to the character as a whole, treating it as a rigid body with the
  // character's COM and inertia properties.
  if (inputFullBodyMagnitude != 0.0f)
  {
    // World space impulse or velocity change
    NMP::Vector3 impulseWorld = directionWorld * inputFullBodyMagnitude;

    NMP::Matrix34 invInertiaWorld = calculateGlobalInertiaTensor();
    invInertiaWorld.invert3x3();
    NMP::Vector3  COMPositionWorld = calculateCentreOfMass();

    NMP::Vector3 impulsePositionRelCOMPositionWorld = positionWorld - COMPositionWorld;

    if (applyAsVelocityChange == true)
    {
      // If applying velocity changes we need to convert impulseWorld into a real impulse (rather
      // than velocity change). This is done using the same equations as found in collision
      // response code - i.e. relating a required change in velocity in a certain direction to
      // an impulse.

      float velChangePerUnitImpulse =  
        (1.0f / totalRigMass) + 
        NMP::vDot(directionWorld, 
        NMP::vCross(invInertiaWorld.getRotatedVector(NMP::vCross(impulsePositionRelCOMPositionWorld, 
        directionWorld)), 
        impulsePositionRelCOMPositionWorld));

      impulseWorld = impulseWorld / velChangePerUnitImpulse;
    }

    // Apply the impulse by calculating the response of a rigid body with the mass/inertia
    // properties of the physics rig
    {
      NMP::Vector3 linearVelocityChange = impulseWorld/totalRigMass;
      NMP::Vector3 angularImpulse = NMP::vCross(impulsePositionRelCOMPositionWorld, impulseWorld);
      NMP::Vector3 angularVelocityChange = angularImpulse;
      angularVelocityChange.rotate(invInertiaWorld);

      // apply the multipliers, and also the distribution
      linearVelocityChange *= inputFullBodyLinearMultiplier;
      angularVelocityChange *= inputFullBodyAngularMultiplier;

      for (uint32_t iPart = 0; iPart < getNumParts(); ++iPart)
      {
        MR::PhysicsRigPhysX2::PartPhysX* part = getPartPhysX(iPart);

        float partMass = part->getMass();
        float multiplier = powf(partMass / averagePartMass, inputFullBodyResponseRatio);

        // add angular velocity due to torque
        MR::addAngularVelocityChangeToActor(*part->getActor(), angularVelocityChange * multiplier);

        // Add linear velocity due to linear push and torque around COM
        NMP::Vector3 partCOMOffset = part->getCOMPosition() - COMPositionWorld;
        NMP::Vector3 rotationalLinearVelocityChange = NMP::vCross(angularVelocityChange, partCOMOffset);
        MR::addLinearVelocityChangeToActor(
          *part->getActor(), 
          (linearVelocityChange + rotationalLinearVelocityChange) * multiplier);
      }
    }
  } // inputDistribution > 0
} 

//----------------------------------------------------------------------------------------------------------------------
// TODO move this into PhysicsRig
NMP::Quat PhysicsRigPhysX2::getJointQuat(uint32_t jointIndex)
{
  JointPhysX* joint = (JointPhysX*)getJoint(jointIndex);
  const PhysicsJointDef* jointDef = m_physicsRigDef->m_joints[jointIndex];
  uint32_t p1 = jointDef->m_parentPartIndex;
  uint32_t p2 = jointDef->m_childPartIndex;
  NMP::Matrix34 part1TM = getPart(p1)->getTransform();
  NMP::Matrix34 part2TM = getPart(p2)->getTransform();
  NMP::Quat result = joint->getRotation(jointDef, part1TM, part2TM);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::setKinematicPos(const NMP::Vector3& pos)
{
  m_kinematicPos = pos;
  if (!isReferenced())
    makeKinematic(true);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::applyModifiedValues()
{
  for (uint32_t i = 0; i < getNumParts(); ++i)
  {
    ((PartPhysX*)m_parts[i])->applyModifiedValues();   
  }

  for (uint32_t i = 0; i < getNumJoints(); ++i)
  {
    JointPhysX* joint = (JointPhysX*) m_joints[i];
    joint->applyModifiedValues(
      m_enableJointProjection, 
      m_desiredJointProjectionLinearTolerance, 
      m_desiredJointProjectionAngularTolerance);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::generateCachedValues()
{
  for (uint32_t i = 0; i < getNumParts(); ++i)
  {
    ((PartPhysX*)m_parts[i])->generateCachedValues();
  }

  for (uint32_t i = 0; i < getNumJoints(); ++i)
  {
    ((JointPhysX*)m_joints[i])->generateCachedValues();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::disableSleeping()
{
  getPartPhysX(0)->getActor()->wakeUp();
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::updatePrePhysics(float NMP_UNUSED(timeStep))
{
  applyModifiedValues();
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::updatePostPhysics(float NMP_UNUSED(timeStep))
{
  generateCachedValues();
  resetJointLimits();
  m_enableJointProjection = false;
  m_desiredJointProjectionLinearTolerance = FLT_MAX;
  m_desiredJointProjectionAngularTolerance = NM_PI;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX2::createMaterialsList(PhysicsRigDef* physicsRigDef, PhysicsScene* physicsScene)
{
  NxScene* physXScene = ((PhysicsScenePhysX2*)physicsScene)->getPhysXScene();
  for (uint32_t i = 0; i < (uint32_t)physicsRigDef->m_numMaterials; ++i)
  {
    const PhysicsMaterialDriverDataPhysX2* driverData =
      (const PhysicsMaterialDriverDataPhysX2*)physicsRigDef->m_materials[i].driverData;

    NxMaterialDesc materialDesc;
    materialDesc.dynamicFriction = driverData->m_dynamicFriction;
    materialDesc.staticFriction = physicsRigDef->m_materials[i].friction;
    materialDesc.restitution = physicsRigDef->m_materials[i].restitution;
    if (driverData->m_disableStrongFriction)
    {
      materialDesc.flags |= NX_MF_DISABLE_STRONG_FRICTION;
    }
    materialDesc.frictionCombineMode = driverData->m_frictionCombineMode;
    materialDesc.restitutionCombineMode = driverData->m_restitutionCombineMode;

    m_materials[i] = physXScene->createMaterial(materialDesc);
  }
}

} // namespace MR
//----------------------------------------------------------------------------------------------------------------------
