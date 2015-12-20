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
#include "physics/PhysX3/mrPhysicsRigPhysX3.h"
#include "physics/PhysX3/mrPhysicsScenePhysX3.h"
#include "physics/PhysX3/mrPhysicsDriverDataPhysX3.h"

#if defined(NM_HOST_CELL_PPU)
  #include <sys/process.h>
  SYS_PROCESS_PARAM (1001, 98304) // increase the primary ppu thread stack size from 64k to 96k 
                                  // to avoid physx3 simulate() running out of stack space.
#endif
//----------------------------------------------------------------------------------------------------------------------

// This rig ID must start at 1, because the default value that dynamic objects ignore is 0. If the
// game deletes and inits rigs more than 2^32 times this will be a problem. See MORPH-11270
static int g_rigID = 1; 

namespace MR
{
//----------------------------------------------------------------------------------------------------------------------
bool locatePhysicsRigDefPhysX3(uint32_t NMP_USED_FOR_ASSERTS(assetType), void* assetMemory)
{
  NMP_ASSERT(assetType == Manager::kAsset_PhysicsRigDef);
  PhysicsRigDef* physicsRigDef = (PhysicsRigDef*)assetMemory;
  bool result = physicsRigDef->locate();
  if (result)
  {
    return locateDriverDataPhysX3(physicsRigDef);
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3::init(PhysicsRigPhysX3* physicsRigPhysX3, Type type, physx::PxClientID ownerClientID, uint32_t clientBehaviourBits)
{
  physicsRigPhysX3->m_type = type;
  physicsRigPhysX3->m_rigID = g_rigID++;
  physicsRigPhysX3->m_characterControllerActor = 0;
  physicsRigPhysX3->m_ownerClientID = ownerClientID;
  physicsRigPhysX3->m_clientBehaviourBits = clientBehaviourBits;
  physicsRigPhysX3->m_kinematicPose.identity();
  physicsRigPhysX3->m_desiredJointProjectionLinearTolerance = FLT_MAX;
  physicsRigPhysX3->m_desiredJointProjectionAngularTolerance = NM_PI;
  physicsRigPhysX3->m_desiredJointProjectionIterations = 0;

  NMP_ASSERT(physicsRigPhysX3->m_registeredJoints.getNumUsedSlots() == 0);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3::setKinematicPos(const NMP::Vector3& pos)
{
  m_kinematicPose.translation() = pos;
  if (!isReferenced())
  {
    makeKinematic(true);
  }
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigPhysX3::PartPhysX3::PartPhysX3()
{
  m_rigidBody = 0;
  m_currentSkinWidthIncrease = 0.0f;
  m_isBeingKeyframed = false;
  m_SKDeviation = 0.0f;
  m_SKDeviationAngle = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3::PartPhysX3::addVelocityChange(const NMP::Vector3& velChange, const NMP::Vector3& worldPos, float angularMultiplier)
{
  MR::addVelocityChangeToActor(*m_rigidBody, velChange, worldPos, angularMultiplier);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3::PartPhysX3::addImpulse(const NMP::Vector3 &impulse)
{
  MR::addImpulseToActor(*m_rigidBody, impulse);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3::PartPhysX3::addTorqueImpulse(const NMP::Vector3& torqueImpulse)
{
  MR::addTorqueImpulseToActor(*m_rigidBody, torqueImpulse);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3::PartPhysX3::addTorque(const NMP::Vector3& torque)
{
  MR::addTorqueToActor(*m_rigidBody, torque);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3::PartPhysX3::addForce(const NMP::Vector3 &force)
{
  MR::addForceToActor(*m_rigidBody, force);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3::PartPhysX3::addLinearVelocityChange(const NMP::Vector3& velChange)
{
  MR::addLinearVelocityChangeToActor(*m_rigidBody, velChange);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3::PartPhysX3::addAngularAcceleration(const NMP::Vector3& angularAcceleration)
{
  MR::addAngularAccelerationToActor(*m_rigidBody, angularAcceleration);
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigPhysX3Articulation* PhysicsRigPhysX3::getPhysicsRigPhysX3Articulation()
{
  return m_type == TYPE_ARTICULATED ? (PhysicsRigPhysX3Articulation*) this : 0;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigPhysX3Jointed* PhysicsRigPhysX3::getPhysicsRigPhysX3Jointed()
{
  return m_type == TYPE_JOINTED ? (PhysicsRigPhysX3Jointed*) this : 0;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigPhysX3::PartPhysX3* PhysicsRigPhysX3::getPartPhysX3(uint32_t index)
{
  return (PhysicsRigPhysX3::PartPhysX3*) getPart(index);
}

//----------------------------------------------------------------------------------------------------------------------
const PhysicsRigPhysX3::PartPhysX3* PhysicsRigPhysX3::getPartPhysX3(uint32_t index) const
{
  return (const PhysicsRigPhysX3::PartPhysX3*) getPart(index);
}

//----------------------------------------------------------------------------------------------------------------------
float PhysicsRigPhysX3::PartPhysX3::getSKDeviation() const
{
  // Note that the deviation is only set when actually being soft keyframed
  if (!m_isBeingKeyframed || isKinematic())
    return 0.0;
  return m_SKDeviation;
}

//----------------------------------------------------------------------------------------------------------------------
float PhysicsRigPhysX3::PartPhysX3::getSKDeviationAngle() const
{
  // Note that the deviation is only set when actually being soft keyframed
  if (!m_isBeingKeyframed || isKinematic())
    return 0.0;
  return m_SKDeviationAngle;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigPhysX3::JointPhysX3::JointPhysX3(const PhysicsSixDOFJointDef* const def)
: m_def(def)
{
  NMP_ASSERT(def); // Null check.

  if (def)
  {
    m_modifiableLimits = def->m_hardLimits.getModifiableLimits();
  }

#if defined(MR_OUTPUT_DEBUGGING)
  // Initialise serialization data.
  updateSerializeTxFrameData();
#endif // MR_OUTPUT_DEBUGGING
}


//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3::JointPhysX3::clampToLimits(
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
void PhysicsRigPhysX3::JointPhysX3::expandLimits(const NMP::Quat& orientation)
{
  m_modifiableLimits.expand(orientation, *m_def);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3::JointPhysX3::scaleLimits(float scaleFactor)
{
  m_modifiableLimits.scale(scaleFactor);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3::JointPhysX3::resetLimits()
{
#if defined(MR_OUTPUT_DEBUGGING)
  // Copy current limits used this frame into frame data cache before they are reset so that they can be made availible
  // to the debug render system later in the update.
  updateSerializeTxFrameData();
#endif // MR_OUTPUT_DEBUGGING

  m_modifiableLimits.setSwingLimit(
    m_def->m_hardLimits.getSwing1Limit(), 
    m_def->m_hardLimits.getSwing2Limit());
  m_modifiableLimits.setTwistLimit(
    m_def->m_hardLimits.getTwistLimitLow(), 
    m_def->m_hardLimits.getTwistLimitHigh());
}

#if defined(MR_OUTPUT_DEBUGGING)
//----------------------------------------------------------------------------------------------------------------------
uint32_t PhysicsRigPhysX3::JointPhysX3::serializeTxFrameData(
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
void PhysicsRigPhysX3::JointPhysX3::updateSerializeTxFrameData()
{
  m_serializeTxFrameData.m_jointType = PhysicsJointFrameData::JOINT_TYPE_SIX_DOF;

  m_serializeTxFrameData.m_swing1Limit = m_modifiableLimits.getSwing1Limit();
  m_serializeTxFrameData.m_swing2Limit = m_modifiableLimits.getSwing2Limit();
  m_serializeTxFrameData.m_twistLimitLow = m_modifiableLimits.getTwistLimitLow();
  m_serializeTxFrameData.m_twistLimitHigh = m_modifiableLimits.getTwistLimitHigh();
}
#endif // MR_OUTPUT_DEBUGGING

//----------------------------------------------------------------------------------------------------------------------
float PhysicsRigPhysX3::getMaxSKDeviation() const
{
  float maxDeviation = 0.0f;
  for (uint32_t i = 0; i < getNumParts(); ++i)
  {
    const PhysicsRigPhysX3::PartPhysX3 *part = getPartPhysX3(i);
    float SKDeviation = part->getSKDeviation();

    if (SKDeviation > maxDeviation)
      maxDeviation = SKDeviation;
  }
  return maxDeviation;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3::receiveImpulse(int32_t inputPartIndex,
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

  MR::PhysicsRigPhysX3::PartPhysX3* hitPart = getPartPhysX3(inputPartIndex);
  physx::PxRigidBody* rigidBody = hitPart->getRigidBody();

  NMP::Matrix34 actorToWorld;
  MR::getActorGlobalPoseTM(*rigidBody, actorToWorld);

  // Get the world space position
  NMP::Vector3 positionWorld = inputPosition;
  if (!positionInWorldSpace)
  {
    // Local space positions are specified relative to the COM, not the actor, position.
    NMP::Matrix34 actorCOMToWorld = actorToWorld;
    actorCOMToWorld.translation() = MR::getActorCOMPos(rigidBody);
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

    NMP::Vector3 impulsePositionRelCOMPositionWorld = 
      invalidInputPartIndex ? NMP::Vector3::InitZero : positionWorld - COMPositionWorld;

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
        MR::PhysicsRigPhysX3::PartPhysX3* part = getPartPhysX3(iPart);

        float partMass = part->getMass();
        float multiplier = powf(partMass / averagePartMass, inputFullBodyResponseRatio);

        // add angular velocity due to torque
        MR::addAngularVelocityChangeToActor(*part->getRigidBody(), angularVelocityChange * multiplier);

        // Add linear velocity due to linear push and torque around COM
        NMP::Vector3 partCOMOffset = part->getCOMPosition() - COMPositionWorld;
        NMP::Vector3 rotationalLinearVelocityChange = NMP::vCross(angularVelocityChange, partCOMOffset);
        MR::addLinearVelocityChangeToActor(
          *part->getRigidBody(), 
          (linearVelocityChange + rotationalLinearVelocityChange) * multiplier);
      }
    }
  } // inputDistribution > 0
} 

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3::receiveTorqueImpulse(
  int32_t inputPartIndex,
  const NMP::Vector3& inputDirection,
  float inputLocalMagnitude,
  float inputLocalResponseRatio,
  float inputFullBodyMagnitude,
  float inputFullBodyResponseRatio,
  bool directionInWorldSpace,
  bool applyAsVelocityChange)
{
  bool invalidInputPartIndex = inputPartIndex < 0 || inputPartIndex >= (int32_t) getNumParts();
  if (invalidInputPartIndex)
  {
    inputPartIndex = 0;
    inputLocalMagnitude = 0.0f;
  }

  MR::PhysicsRigPhysX3::PartPhysX3* hitPart = getPartPhysX3(inputPartIndex);
  physx::PxRigidBody* rigidBody = hitPart->getRigidBody();

  NMP::Matrix34 actorToWorld;
  MR::getActorGlobalPoseTM(*rigidBody, actorToWorld);

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
    NMP::Vector3 torqueImpulseWorld = directionWorld * inputLocalMagnitude;

    float hitPartMass = hitPart->getMass();
    float multiplier = powf(hitPartMass / averagePartMass, inputLocalResponseRatio);

    if (applyAsVelocityChange == false)
    {
      // impulse
      MR::addTorqueImpulseToActor(
        *rigidBody, 
        torqueImpulseWorld * multiplier);
    }
    else if (applyAsVelocityChange == true)
    {
      // velocity change
      MR::addAngularVelocityChangeToActor(
        *rigidBody, 
        torqueImpulseWorld * multiplier);
    }
  }

  // Apply the impulse to the character as a whole, treating it as a rigid body with the
  // character's COM and inertia properties.
  if (inputFullBodyMagnitude != 0.0f)
  {
    // World space impulse or velocity change
    NMP::Vector3 torqueImpulseWorld = directionWorld * inputFullBodyMagnitude;

    NMP::Matrix34 invInertiaWorld = calculateGlobalInertiaTensor();
    invInertiaWorld.invert3x3();
    NMP::Vector3  COMPositionWorld = calculateCentreOfMass();

    // Apply the impulse by calculating the response of a rigid body with the mass/inertia
    // properties of the physics rig
    {
      NMP::Vector3 angularVelocityChange = 
        applyAsVelocityChange ? torqueImpulseWorld : invInertiaWorld.getRotatedVector(torqueImpulseWorld);

      for (uint32_t iPart = 0; iPart < getNumParts(); ++iPart)
      {
        MR::PhysicsRigPhysX3::PartPhysX3* part = getPartPhysX3(iPart);

        float partMass = part->getMass();
        float multiplier = powf(partMass / averagePartMass, inputFullBodyResponseRatio);

        // add angular velocity due to torque
        MR::addAngularVelocityChangeToActor(*part->getRigidBody(), angularVelocityChange * multiplier);

        // Add linear velocity due to linear push and torque around COM
        NMP::Vector3 partCOMOffset = part->getCOMPosition() - COMPositionWorld;
        NMP::Vector3 rotationalLinearVelocityChange = NMP::vCross(angularVelocityChange, partCOMOffset);
        MR::addLinearVelocityChangeToActor(
          *part->getRigidBody(), 
          rotationalLinearVelocityChange * multiplier);
      }
    }
  } // inputDistribution > 0
} 

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3::setSkinWidthIncrease(uint32_t partIndex, float skinWidthIncrease)
{
  PartPhysX3 *partPhysX = getPartPhysX3(partIndex);
  // Note that if a skin width increase is in use, then it will end up getting set and then returned
  // to zero each update. However, this will prevent excessive calls in the normal cases.
  if (partPhysX->m_currentSkinWidthIncrease == skinWidthIncrease)
  {
    return;
  }

  // Only allow increases, unless this is supposed to reset to the authored value
  if (partPhysX->m_currentSkinWidthIncrease > skinWidthIncrease && skinWidthIncrease != 0.0f)
  {
    return;
  }

  physx::PxShape *shapes[MAX_SHAPES_IN_VOLUME];
  partPhysX->getRigidBody()->getShapes(&shapes[0], MAX_SHAPES_IN_VOLUME);

  partPhysX->m_currentSkinWidthIncrease = skinWidthIncrease;
  const PhysicsRigDef *physicsRigDef = getPhysicsRigDef();
  const PhysicsRigDef::Part &partDef = physicsRigDef->m_parts[partIndex];
  // shapes we created in the order boxes, capsules, spheres
  physx::PxU32 iShape = 0;
  for (int32_t iBox = 0 ; iBox < partDef.volume.numBoxes ; ++iBox)
  {
    PhysicsShapeDriverDataPhysX3* driverData = (PhysicsShapeDriverDataPhysX3*)partDef.volume.boxes[iBox].driverData;
    shapes[iShape++]->setContactOffset(driverData->m_contactOffset + skinWidthIncrease);
  }
  for (int32_t iCapsule = 0 ; iCapsule < partDef.volume.numCapsules ; ++iCapsule)
  {
    PhysicsShapeDriverDataPhysX3* driverData = (PhysicsShapeDriverDataPhysX3*)partDef.volume.capsules[iCapsule].driverData;
    shapes[iShape++]->setContactOffset(driverData->m_contactOffset + skinWidthIncrease);
  }
  for (int32_t iSphere = 0 ; iSphere < partDef.volume.numSpheres ; ++iSphere)
  {
    PhysicsShapeDriverDataPhysX3* driverData = (PhysicsShapeDriverDataPhysX3*)partDef.volume.spheres[iSphere].driverData;
    shapes[iShape++]->setContactOffset(driverData->m_contactOffset + skinWidthIncrease);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3::scaleFrictionOnPart(const int32_t partIndex, const float frictionScale)
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

    physx::PxMaterial* const material = m_materials[materialID[i]];
    material->setStaticFriction(materialDef.friction * frictionScale);
    PhysicsMaterialDriverDataPhysX3* driverData = static_cast<PhysicsMaterialDriverDataPhysX3*>(materialDef.driverData);
    material->setDynamicFriction(driverData->m_dynamicFriction * frictionScale);
  }
}

//----------------------------------------------------------------------------------------------------------------------
physx::PxMaterial* PhysicsRigPhysX3::createMaterial(const PhysicsRigDef::Part::Material& material)
{
  PhysicsMaterialDriverDataPhysX3* driverData = (PhysicsMaterialDriverDataPhysX3*)material.driverData;

  physx::PxPhysics& physics = PxGetPhysics();
  physx::PxMaterial* result = physics.createMaterial(material.friction, driverData->m_dynamicFriction, material.restitution);

  result->setFrictionCombineMode(driverData->m_frictionCombineMode);
  result->setRestitutionCombineMode(driverData->m_restitutionCombineMode);

  if (driverData->m_disableStrongFriction)
  {
    result->setFlag(physx::PxMaterialFlag::eDISABLE_STRONG_FRICTION, true);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3::registerJointOnRig(physx::PxD6Joint* joint)
{
  if (!m_registeredJoints.replace(joint, true))
  {
    m_registeredJoints.insert(joint, true);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3::deRegisterJointOnRig(physx::PxD6Joint* joint)
{
  if (!m_registeredJoints.erase(joint))
  {
    NMP_ASSERT_FAIL_MSG("Tried to deregister an unregistered joint");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3::updateRegisteredJoints()
{
  RegisteredJoints::value_walker it = m_registeredJoints.walker();
  while (it.next())
  {
    bool registered = it();
    if (!registered)
    {
      // The joint hasn't been deregistered, and it wasn't updated, so it needs to be released.
      physx::PxD6Joint* joint = it.key();
      joint->release();
      m_registeredJoints.erase(joint);
      // Erasing invalidates the walker so just start again
      it.reset();
    }
  }

  // Now clear the entries ready for the next update
  it.reset();
  while (it.next())
  {
    it() = false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3::requestJointProjectionParameters(int iterations, float linearTolerance, float angularTolerance)
{
  if (iterations > 0)
  {
    if (iterations > m_desiredJointProjectionIterations)
    {
      m_desiredJointProjectionIterations = iterations;
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
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3::addQueryFilterFlagToParts(uint32_t word0, uint32_t word1, uint32_t word2, uint32_t word3)
{
  for (uint32_t iPart = 0; iPart < getNumParts(); ++iPart)
  {
    MR::PhysicsRigPhysX3::PartPhysX3* part = getPartPhysX3(iPart);
    physx::PxRigidBody* rigidBody = part->getRigidBody();

    physx::PxU32 numShapes = rigidBody->getNbShapes();
    for (physx::PxU32 iShape = 0 ; iShape != numShapes ; ++iShape)
    {
      physx::PxShape* shape = 0;
      rigidBody->getShapes(&shape, 1, iShape);
      physx::PxFilterData filterData = shape->getQueryFilterData();
      filterData.word0 |= word0;
      filterData.word1 |= word1;
      filterData.word2 |= word2;
      filterData.word3 |= word3;
      shape->setQueryFilterData(filterData);
    }
  }
}



PhysicsRigPhysX3ActorData::ActorToPhysicsRigPhysX3ActorData* PhysicsRigPhysX3ActorData::m_actorToMorphemeMap = 0;
uint32_t PhysicsRigPhysX3ActorData::m_actorMapRefCount = 0;

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3ActorData::init()
{
  ++m_actorMapRefCount;

  if (m_actorMapRefCount == 1)
  {
    NMP_ASSERT(!m_actorToMorphemeMap);
    if (!m_actorToMorphemeMap)
    {
      m_actorToMorphemeMap = (ActorToPhysicsRigPhysX3ActorData*) 
        NMPMemoryAlloc(sizeof(ActorToPhysicsRigPhysX3ActorData));
      NMP_ASSERT(m_actorToMorphemeMap);
      new(m_actorToMorphemeMap) ActorToPhysicsRigPhysX3ActorData(32);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3ActorData::term()
{
  --m_actorMapRefCount;

  if (m_actorMapRefCount == 0)
  {
    if (m_actorToMorphemeMap)
    {
      m_actorToMorphemeMap->~ActorToPhysicsRigPhysX3ActorData();
      NMP::Memory::memFree(m_actorToMorphemeMap);
      m_actorToMorphemeMap = 0;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigPhysX3ActorData *PhysicsRigPhysX3ActorData::create(physx::PxActor *actor, 
                                                             PhysicsRig::Part *owningRigPart,
                                                             PhysicsRig *owningRig)
{
  PhysicsRigPhysX3ActorData *data = 
    (PhysicsRigPhysX3ActorData*) NMPMemoryAlloc(sizeof(PhysicsRigPhysX3ActorData));
  NMP_ASSERT(data);
  new(data) PhysicsRigPhysX3ActorData(actor, owningRigPart, owningRig);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3ActorData::destroy(PhysicsRigPhysX3ActorData *data, physx::PxActor *actor)
{
  if (!data)
  {
    return;
  }
  if (actor)
  {
    PhysicsRigPhysX3ActorData* element = 0;
    m_actorToMorphemeMap->find(actor, &element);
    NMP_ASSERT(m_actorToMorphemeMap->getNumUsedSlots()>0);
    NMP_ASSERT(element == data);
    m_actorToMorphemeMap->erase(actor);
  }
  NMP::Memory::memFree(data);
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigPhysX3ActorData::PhysicsRigPhysX3ActorData(
  physx::PxActor *actor, 
  PhysicsRig::Part *owningRigPart,
  PhysicsRig *owningRig) 
  : m_owningRigPart(owningRigPart), m_owningRig(owningRig), m_userData(0) 
{
  NMP_ASSERT(getFromActor(actor) == 0);
  m_actorToMorphemeMap->insert(actor, this); // ensures it definitely goes in, even if the old value exists!
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigPhysX3ActorData *PhysicsRigPhysX3ActorData::getFromActor(physx::PxActor *actor)
{
  PhysicsRigPhysX3ActorData* data = NULL;
  m_actorToMorphemeMap->find(actor, &data);
  return data;
}

} // namespace MR
//----------------------------------------------------------------------------------------------------------------------
