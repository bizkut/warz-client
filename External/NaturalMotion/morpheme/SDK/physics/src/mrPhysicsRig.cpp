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
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMProfiler.h"
#include "morpheme/mrBlendOps.h"
#include "morpheme/mrDispatcher.h"
#include "physics/mrPhysicsRig.h"
#include "physics/mrPhysicsScene.h"
#include "sharedDefines/mPhysicsDebugInterface.h"

// We should not be using the trajectory in the conversion from animation to
// world space since the character transform comes from elsewhere. However, it is needed at the
// moment to handle the bind pose (which ends up using TRAJ_IN_CALC_WORLD_SPACE) - which can have a
// rotation on the trajectory bone - to extract the character orientation whilst in physics. There
// are other similar places (TRAJ_IN_FROM_TRANS_BUFFER) that should be doing the same calculation,
// but if they use the trajectory then the world space transform is wrong with a normal animation.
// However, if they don't use the calculation then initialisation from the bind pose is wrong.
// Chosen to the latter case for the moment until things are fixed properly.
// See MORPH-11296
//#define TRAJ_IN_FROM_TRANS_BUFFER
#define TRAJ_IN_CALC_WORLD_SPACE

//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
  
//----------------------------------------------------------------------------------------------------------------------
static const NMP::DataBuffer& selectBufferForInputBufferChannel(const NMP::DataBuffer& inputBuffer,
                                                                const NMP::DataBuffer& fallbackBuffer,
                                                                uint32_t               channelIndex)
{
  return (!inputBuffer.hasChannel(channelIndex)) ? fallbackBuffer : inputBuffer;
}
  
  
//----------------------------------------------------------------------------------------------------------------------
// PhysicsRig
//----------------------------------------------------------------------------------------------------------------------
PhysicsRig::PhysicsRig() 
  : m_refCount(0)
  , m_animRigDef(0)
  , m_animToPhysicsMap(0)
  , m_jointLimitsModified(false)
  , m_userData(0)
{
  m_rootOriginalOffsetTranslation.setToZero();
  m_rootOriginalOffsetRotation.identity();
}

//----------------------------------------------------------------------------------------------------------------------
// PhysicsRig::Part
//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 PhysicsRig::Part::getPredictedPosition(float predTime) const
{
  NMP::Vector3 pos = getPosition();
  if (predTime > 0.0f)
    pos += getVel() * predTime;
  return pos;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Quat PhysicsRig::Part::getPredictedQuaternion(float predTime) const
{
  NMP::Quat q = getQuaternion();
  if (predTime > 0.0f)
  {
    NMP::Vector3 r = getAngVel();
    q += (NMP::Quat(r.x, r.y, r.z, 0.0f) * q) * (0.5f * predTime);
    q.normalise();
  }
  return q;
}


//----------------------------------------------------------------------------------------------------------------------
// PhysicsRig::Joint
//----------------------------------------------------------------------------------------------------------------------
bool PhysicsRig::Joint::supportsDriveCompensation()
{
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRig::Joint::setDriveCompensation(float NMP_UNUSED(driveCompensation))
{
  NMP_ASSERT_FAIL_MSG("This function should never be called");
}

//----------------------------------------------------------------------------------------------------------------------
float PhysicsRig::Joint::getDriveCompensation() const
{
  NMP_ASSERT_FAIL_MSG("This function should never be called");
  return 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
float PhysicsRig::Joint::getExternalCompliance() const
{
  NMP_ASSERT_FAIL_MSG("This function should never be called");
  return 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRig::Joint::setExternalCompliance(float NMP_UNUSED(compliance))
{
  NMP_ASSERT_FAIL_MSG("This function should never be called");
}

//----------------------------------------------------------------------------------------------------------------------
// PhysicsRig
//----------------------------------------------------------------------------------------------------------------------
uint32_t PhysicsRig::getNumMaterials() const
{
  return getPhysicsRigDef()->getNumMaterials();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Matrix34 PhysicsRig::getRootPartTransform() const
{
  NMP::Matrix34 offset(m_physicsRigDef->m_rootPart.transform);
  const Part* part = getPart(m_physicsRigDef->m_rootPart.index);
  if (part)
  {
    return offset * part->getTransform();
  }
  else
  {
    return NMP::Matrix34::kIdentity;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 PhysicsRig::getMarkedUpMeanTrajectoryPosition() const
{
  if (m_physicsRigDef->m_numTrajectoryParts == 0)
  {
    NET_LOG_ERROR_MESSAGE("Trajectory parts haven't been tagged in physics rig - using hip/default part\n");
    return getRootPartTransform().translation();
  }
  NMP::Vector3 p(NMP::Vector3::InitZero);
  for (int32_t i = 0; i < m_physicsRigDef->m_numTrajectoryParts; ++i)
  {
    const Part* part = getPart(m_physicsRigDef->m_trajectoryParts[i]);
    p += part->getPosition();
  }
  p /= (float) m_physicsRigDef->m_numTrajectoryParts;
  return p;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRig::receiveWrenchImpulse(
  const NMP::Vector3& impulse,
  const NMP::Vector3& torqueImpulse,
  const bool* usePartFlags)
{
  // Calculate the required linear and angular velocity changes
  //
  float partsMass;
  NMP::Vector3 partsCOM;
  NMP::Matrix34 partsInertia;
  caclulateMassProperties(partsMass, partsCOM, partsInertia, usePartFlags);
  // Early out if for some reason there is no mass to act on.
  if (partsMass <= 0.0f)
  {
    return;
  }
  NMP::Matrix34 partsInertiaInv(partsInertia);
  partsInertiaInv.invert3x3();

  NMP::Vector3 linearVelocityChange = impulse / partsMass;
  NMP::Vector3 angularVelocityChange = partsInertiaInv.getRotatedVector(torqueImpulse);

  // Apply com impulses to each part.
  //
  const int numParts = getNumParts();
  for (int i = 0; i < numParts; ++i)
  {
    // Skip excluded parts.
    if (usePartFlags && !usePartFlags[i])
    {
      continue;
    }

    // Calculate linear and rotational components of the part COM impulse.
    // partCOMImpulse = partMass * (linearVelocityChange + cross (angularvelchange, partCOM - partsCOM ))
    //
    MR::PhysicsRig::Part* const rigPart(getPart(i));
    NMP::Vector3 partCOMImpulse = rigPart->getMass() 
      * (linearVelocityChange + NMP::vCross(angularVelocityChange, rigPart->getCOMPosition() - partsCOM));

    // Calculate the torque to apply to the part given the required angular vel change and the part inertia
    NMP::Matrix34 partInertia = rigPart->getGlobalInertiaTensor();
    NMP::Vector3 partTorque = partInertia.getRotatedVector(angularVelocityChange);

    // Apply "actor" impulse and torque-impulse.
    //
    MR::PhysicsRig::Part* part = getPart(i);
    part->addImpulse(partCOMImpulse);
    part->addTorqueImpulse(partTorque);
  }
}


//----------------------------------------------------------------------------------------------------------------------
NMP::Matrix34 PhysicsRig::getRoot(
  NMP::Matrix34*          originalRoot,
  float                   physicsRootFraction,
  CharacterControllerDef* characterControllerDef)
{
  NMP::Matrix34 result;
  // 1. Place the bind pose of the character in the world with the hips joint aligned with the hips
  //    physics body (as in the bind pose). Using this calculate what the world transform of the
  //    root transform would be. This gives us an approximation of what the attitude and position of
  //    the animation root transform should be with the character in this position.
  // 2. Get the trajectory vertical alignment axis. This describes how the character root should be
  //    aligned vertically in the world. So if for example the character is on a slope in the
  //    environment it may be that we want the character to crawl on it, in this case the trajectory
  //    vertical alignment axis would likely be normal with the slope. However if the character was
  //    to walk on the slope the trajectory vertical alignment axis would probably be vertical in
  //    world space so that the character was vertical. This axis becomes one of primary axis of our
  //    result. The other axis can be rotated about this axis in any direction, but the axis will
  //    not change. Which one of the primary axis it will become is dependent on whether x, y or z
  //    are defined as being UP in world space. Note: if we place a character at identity in world
  //    space its root axes will be aligned with the world space axes.
  // 3. Calculate the rotational difference between the trajectory vertical alignment axis and the
  //    up component vx, vy or vz of the world root transform. Turn this into a rotation matrix and
  //    apply it to the world root transform to find the remaining axes of the result.

  // Currently using hips transform as animation world space root transform. Not correct but will do
  // for now. See MORPH-11296
  NMP::Matrix34 worldRoot = getRootPartTransform();
  NMP_ASSERT(worldRoot.isValidTM(0.01f));

  // Account for the root bone being offset from the trajectory (need to confirm if the trajectory TM
  // is always identity). See MORPH-12977
  uint32_t animTrajectoryBoneIndex = m_animRigDef->getTrajectoryBoneIndex();
  uint32_t animRootBoneIndex = m_animRigDef->getCharacterRootBoneIndex();
  NMP::Matrix34 trajTM(*m_animRigDef->getBindPoseBoneQuat(animTrajectoryBoneIndex), NMP::Vector3::InitZero);
  NMP::Matrix34 rootTMInv(*m_animRigDef->getBindPoseBoneQuat(animRootBoneIndex), NMP::Vector3::InitZero);
  rootTMInv.invert3x3();
  NMP::Matrix34 rootToTraj = rootTMInv * trajTM; // need to check the order here - when trajTM is not identity!
  worldRoot = rootToTraj * worldRoot;

  // Currently using the world up direction as the trajectory vertical alignment axis.
  // This should eventually come from the character controller and be part of the trajectory root
  // transform. See MORPH-11296
  NMP::Vector3 trajectoryVerticalAxis = getPhysicsScene()->getWorldUpDirection();

  // Currently programatically determining which of the primary axes of the trajectory root
  // transform is formed by the trajectoryVerticalAxis. This should be determined in the asset
  // compiler and cached for use here. See MORPH-11296
  NMP_ASSERT(trajectoryVerticalAxis.x > 0.99f || trajectoryVerticalAxis.z > 0.99f || trajectoryVerticalAxis.y > 0.99f);
  NMP::Quat rotation;
  if (trajectoryVerticalAxis.x > 0.99f)
  {
    // vx forms the up component to the trajectory.
    result.xAxis().set(trajectoryVerticalAxis);

    // Calculate the rotation quat to get from worldRoot.xAxis() to the trajectoryVerticalAxis.
    rotation.forRotation(worldRoot.xAxis(), trajectoryVerticalAxis);

    // Rotate remaining axes in to new alignment.
    rotation.rotateVector(worldRoot.yAxis(), result.yAxis());
    rotation.rotateVector(worldRoot.zAxis(), result.zAxis());
  }
  else if (trajectoryVerticalAxis.z > 0.99f)
  {
    // vz forms the up component to the trajectory.
    result.zAxis().set(trajectoryVerticalAxis);

    // Calculate the rotation quat to get from worldRoot.zAxis() to the trajectoryVerticalAxis.
    rotation.forRotation(worldRoot.zAxis(), trajectoryVerticalAxis);

    // Rotate remaining axes in to new alignment.
    rotation.rotateVector(worldRoot.xAxis(), result.xAxis());
    rotation.rotateVector(worldRoot.yAxis(), result.yAxis());
  }
  else
  {
    // vy forms the up component to the trajectory.
    result.yAxis().set(trajectoryVerticalAxis);

    // Calculate the rotation quat to get from worldRoot.yAxis() to the trajectoryVerticalAxis.
    rotation.forRotation(worldRoot.yAxis(), trajectoryVerticalAxis);

    // Rotate remaining axes in to new alignment.
    rotation.rotateVector(worldRoot.xAxis(), result.xAxis());
    rotation.rotateVector(worldRoot.zAxis(), result.zAxis());
  }

  NMP_ASSERT(result.isValidTM3x3(0.001f));

  // adjust the mean trajectory position to place it on the floor
  NMP::Vector3 footPos = getMarkedUpMeanTrajectoryPosition();

  // If there is a character controller def available we use it to scale the raycast. Otherwise we assume metres.
  float distToCheck = (characterControllerDef ? (0.3f * characterControllerDef->getHeight()) : 0.3f);
  // Avoid problems when foot just below floor.
  float footUpOffset = (characterControllerDef ? (0.2f * characterControllerDef->getStepHeight()) : 0.05f);
  NMP::Vector3 floor = getPhysicsScene()->getFloorPositionBelow(
    footPos + trajectoryVerticalAxis * footUpOffset,
    this,         // Don't intersect this character.
    distToCheck + footUpOffset);
  result.translation() = floor;

  // Gradually convert from the original to the new root position.
  // Can't do this by just blending in the transition... fails when there's a blend to physics (not sure
  // quite why)
  if (originalRoot)
  {
    NMP_ASSERT(originalRoot->isValidTM(0.001f));
    m_rootOriginalOffsetTranslation = originalRoot->translation() - result.translation();
    m_rootOriginalOffsetRotation = originalRoot->toQuat() * ~result.toQuat();
  }

  if (physicsRootFraction >= 1.0f)
  {
    return result; // the normal case, so avoid slerps when we know the result
  }
  else
  {
    float frac = 1.0f - physicsRootFraction;

    NMP::Vector3 workingOffsetTranslation = m_rootOriginalOffsetTranslation * frac;
    NMP::Quat workingOffsetRotation(NMP::Quat::kIdentity);
    workingOffsetRotation.slerp(m_rootOriginalOffsetRotation, frac);

    NMP::Quat q = workingOffsetRotation * result.toQuat();
    q.fastNormalise();
    NMP::Matrix34 newResult(q, workingOffsetTranslation + result.translation());
    NMP_ASSERT(newResult.isValidTM(0.001f));
    return newResult;
  }
}

//----------------------------------------------------------------------------------------------------------------------
#if !defined(NMP_PLATFORM_SIMD) || defined(NM_HOST_IOS)
void PhysicsRig::calculateWorldSpacePartTMsCacheWithVelocity(  
  NMP::Matrix34*          TMcache,
  NMP::Matrix34*          TMcacheOld,
  const NMP::DataBuffer&  inputBuffer,
  const NMP::DataBuffer&  inputBufferOld,
  const NMP::DataBuffer&  fallbackBuffer,
  const NMP::Matrix34    &worldRoot,
  const NMP::Matrix34    &previousWorldRoot
  )
{
  // get world transform
  {
#ifdef TRAJ_IN_CALC_WORLD_SPACE
    int32_t j = (int32_t) m_animRigDef->getTrajectoryBoneIndex();
    NMP::Matrix34 trajTM;
    NMP::Matrix34 trajTMOld;
    if (inputBuffer.hasChannel(j) && inputBufferOld.hasChannel(j))
    {
      trajTM.initialise(*inputBuffer.getChannelQuat(j), *inputBuffer.getChannelPos(j));
      trajTMOld.initialise(*inputBufferOld.getChannelQuat(j), *inputBufferOld.getChannelPos(j));
    }
    else
    {
      trajTM.initialise(*fallbackBuffer.getChannelQuatFast(j), *fallbackBuffer.getChannelPosFast(j));
      trajTMOld.initialise(trajTM);
    }
    TMcache[0] = trajTM * worldRoot;
    TMcacheOld[0] = trajTMOld * previousWorldRoot;
#else
    TMcache[0] = worldRoot;
    TMcacheOld[0] = previousWorldRoot;
#endif
  }

  NMP::Matrix34 TM;  
  NMP::Matrix34 TMOld;

  int32_t numParts =  getNumParts();

  for (int32_t index = 0; index < numParts; ++index)
  {

    int32_t a = m_animToPhysicsMap->getAnimIndexFromPhysicsIndex(index);

    if (inputBuffer.hasChannel(a) && inputBufferOld.hasChannel(a))
    {
      TM.initialise(*inputBuffer.getChannelQuat(a), *inputBuffer.getChannelPos(a));
      TMOld.initialise(*inputBufferOld.getChannelQuat(a), *inputBufferOld.getChannelPos(a));
    }
    else
    {
      TM.initialise(*fallbackBuffer.getChannelQuatFast(a), *fallbackBuffer.getChannelPosFast(a));
      TMOld.initialise(TM);
    }

    int32_t p = (int32_t)m_animRigDef->getParentBoneIndex(a);
    int32_t k = m_animToPhysicsMap->getPhysicsIndexFromAnimIndex(p);

    // if p is the root joint then there is nothing to do
    if (p!=0)
    {
      // collect the animation transforms between physics parts
      while (k==-1)
      {
        NMP::Matrix34 TB(*fallbackBuffer.getChannelQuatFast(p), *fallbackBuffer.getChannelPosFast(p));
        TM = TM * TB;
        TMOld = TMOld * TB;

        p = (int32_t)m_animRigDef->getParentBoneIndex(p);
        if (p==0)
        {       
          break; // got root..
        }
        k = m_animToPhysicsMap->getPhysicsIndexFromAnimIndex(p);
      }
    }

    NMP_ASSERT_MSG((p!=0 || k==-1), "Root joint should not be a physics part.");

    // (zero pos is world transform)
    TMcache[index+1] = TM * TMcache[k+1];
    TMcache[index+1].orthonormalise();      
    TMcacheOld[index+1] = TMOld * TMcacheOld[k+1];
    TMcacheOld[index+1].orthonormalise();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRig::calculateWorldSpacePartTMsCache(  
  NMP::Matrix34*          TMcache,
  const NMP::DataBuffer&  inputBuffer,
  const NMP::DataBuffer&  fallbackBuffer,
  const NMP::Matrix34    &worldRoot
  )
{
  // get world transform
  {
#ifdef TRAJ_IN_CALC_WORLD_SPACE
    int32_t j = (int32_t) m_animRigDef->getTrajectoryBoneIndex();
    NMP::Matrix34 trajTM;
    const NMP::DataBuffer& trajTMBuffer = MR::selectBufferForInputBufferChannel(inputBuffer, fallbackBuffer, j);
    trajTM.initialise(*trajTMBuffer.getChannelQuat(j), *trajTMBuffer.getChannelPos(j));

    TMcache[0] = trajTM * worldRoot;    
#else
    TMcache[0] = worldRoot;
#endif
  }

  NMP::Matrix34 TM;  

  int32_t numParts =  getNumParts();

  for (int32_t index = 0; index < numParts; ++index)
  {

    int32_t a = m_animToPhysicsMap->getAnimIndexFromPhysicsIndex(index);

    // Select buffer.
    const NMP::DataBuffer& buffer = selectBufferForInputBufferChannel(inputBuffer, fallbackBuffer, a);
    TM.initialise(*buffer.getChannelQuat(a), *buffer.getChannelPos(a));

    int32_t p = (int32_t)m_animRigDef->getParentBoneIndex(a);
    int32_t k = m_animToPhysicsMap->getPhysicsIndexFromAnimIndex(p);

    // if p is the root joint then there is nothing to do
    if (p!=0)
    {
      // collect the animation transforms between physics parts
      while (k==-1)
      {
        NMP::Matrix34 TB(*fallbackBuffer.getChannelQuat(p), *fallbackBuffer.getChannelPos(p));
        TM.multiply(TB);

        p = (int32_t)m_animRigDef->getParentBoneIndex(p);
        if (p==0)
        {       
          break; // got root..
        }
        k = m_animToPhysicsMap->getPhysicsIndexFromAnimIndex(p);
      }
    }

    NMP_ASSERT_MSG((p!=0 || k==-1), "Root joint should not be a physics part.");

    // (zero pos is world transform)
    TMcache[index+1] = TM * TMcache[k+1];
    TMcache[index+1].orthonormalise();      
  }
}
#else
//----------------------------------------------------------------------------------------------------------------------
// NMP_PLATFORM_SIMD
void PhysicsRig::calculateWorldSpacePartTMsCacheWithVelocity(  
  NMP::vpu::Matrix*          TMcache,
  NMP::vpu::Matrix*          TMcacheOld,
  const NMP::DataBuffer&     inputBuffer,
  const NMP::DataBuffer&     inputBufferOld,
  const NMP::DataBuffer&     fallbackBuffer,
  const NMP::Matrix34       &worldRoot,
  const NMP::Matrix34       &previousWorldRoot
  )
{
  // get world transform
  {
#ifdef TRAJ_IN_CALC_WORLD_SPACE
    int32_t j = (int32_t) m_animRigDef->getTrajectoryBoneIndex();
    NMP::vpu::Matrix trajTM;
    NMP::vpu::Matrix trajTMOld;

    if (inputBuffer.hasChannel(j) && inputBufferOld.hasChannel(j))
    {
      trajTM = NMP::vpu::Matrix(*inputBuffer.getChannelQuatFast(j), *inputBuffer.getChannelPosFast(j));
      trajTMOld = NMP::vpu::Matrix(*inputBufferOld.getChannelQuatFast(j), *inputBufferOld.getChannelPosFast(j));
    }
    else
    {
      trajTM = NMP::vpu::Matrix(*fallbackBuffer.getChannelQuatFast(j), *fallbackBuffer.getChannelPosFast(j));
      trajTMOld = trajTM;
    }
    TMcache[0] = vpuTM(worldRoot) * trajTM;    
    TMcacheOld[0] = vpuTM(previousWorldRoot) * trajTMOld;
#else
    TMcache[0] = vpuTM(worldRoot);
    TMcacheOld[0] = vpuTM(*previousWorldRoot);
#endif
  }
  
  NMP::vpu::Matrix TM;  
  NMP::vpu::Matrix TMOld;

  int32_t numParts =  getNumParts();

  for (int32_t index = 0; index < numParts; ++index)
  {
    int32_t a = m_animToPhysicsMap->getAnimIndexFromPhysicsIndex(index);

    if (inputBuffer.hasChannel(a) && inputBufferOld.hasChannel(a))
    {
      TM = NMP::vpu::Matrix(*inputBuffer.getChannelQuatFast(a), *inputBuffer.getChannelPosFast(a));
      TMOld = NMP::vpu::Matrix(*inputBufferOld.getChannelQuatFast(a), *inputBufferOld.getChannelPosFast(a));
    }
    else
    {
      TM = NMP::vpu::Matrix(*fallbackBuffer.getChannelQuatFast(a), *fallbackBuffer.getChannelPosFast(a));
      TMOld = TM;
    }

    int32_t p = (int32_t)m_animRigDef->getParentBoneIndex(a);
    int32_t k = m_animToPhysicsMap->getPhysicsIndexFromAnimIndex(p);

    // if p is the root joint then there is nothing to do
    if (p != 0)
    {
      // collect the animation transforms between physics parts
      while (k == -1)
      {
        NMP::vpu::Matrix TB(*fallbackBuffer.getChannelQuatFast(p), *fallbackBuffer.getChannelPosFast(p));
        TM = TB * TM;
        TMOld = TB * TMOld;

        p = (int32_t)m_animRigDef->getParentBoneIndex(p);
        if (p == 0)
        {
          break; // got root..
        }
        k = m_animToPhysicsMap->getPhysicsIndexFromAnimIndex(p);
      }
    }

    NMP_ASSERT_MSG((p!=0 || k==-1), "Root joint should not be a physics part.");

    // (zero pos is world transform)
    TMcache[index+1] = TMcache[k+1] * TM;
    TMcache[index+1].tmReOrthonormalise();      
    TMcacheOld[index+1] = TMcacheOld[k+1] * TMOld;
    TMcacheOld[index+1].tmReOrthonormalise();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRig::calculateWorldSpacePartTMsCache(  
  NMP::vpu::Matrix*          TMcache,
  const NMP::DataBuffer&     inputBuffer,
  const NMP::DataBuffer&     fallbackBuffer,
  const NMP::Matrix34       &worldRoot
  )
{
  // get world transform
  {
#ifdef TRAJ_IN_CALC_WORLD_SPACE
    int32_t j = (int32_t) m_animRigDef->getTrajectoryBoneIndex();
    NMP::vpu::Matrix trajTM;
    const NMP::DataBuffer& trajTMBuffer = selectBufferForInputBufferChannel(inputBuffer, fallbackBuffer, j);
    trajTM = NMP::vpu::Matrix(*trajTMBuffer.getChannelQuatFast(j), *trajTMBuffer.getChannelPosFast(j));

    TMcache[0] = vpuTM(worldRoot) * trajTM;    
#else
    TMcache[0] = vpuTM(worldRoot);
#endif
  }

  NMP::vpu::Matrix TM;  

  int32_t numParts =  getNumParts();

  for (int32_t index = 0; index < numParts; ++index)
  {

    int32_t a = m_animToPhysicsMap->getAnimIndexFromPhysicsIndex(index);

    // Select buffer.
    const NMP::DataBuffer& buffer = selectBufferForInputBufferChannel(inputBuffer, fallbackBuffer, a);
    TM = NMP::vpu::Matrix(*buffer.getChannelQuatFast(a), *buffer.getChannelPosFast(a));

    int32_t p = (int32_t)m_animRigDef->getParentBoneIndex(a);
    int32_t k = m_animToPhysicsMap->getPhysicsIndexFromAnimIndex(p);

    // if p is the root joint then there is nothing to do
    if (p!=0)
    {
      // collect the animation transforms between physics parts
      while (k==-1)
      {
        NMP::vpu::Matrix TB(*fallbackBuffer.getChannelQuat(p), *fallbackBuffer.getChannelPos(p));
        TM = TB * TM;

        p = (int32_t)m_animRigDef->getParentBoneIndex(p);
        if (p==0)
        {       
          break; // got root..
        }
        k = m_animToPhysicsMap->getPhysicsIndexFromAnimIndex(p);
      }
    }

    NMP_ASSERT_MSG((p!=0 || k==-1), "Root joint should not be a physics part.");

    // (zero pos is world transform)
    TMcache[index+1] = TMcache[k+1] * TM;
    TMcache[index+1].tmReOrthonormalise();      
  }
}
#endif

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRig::calculateWorldSpacePartTM(
  NMP::Matrix34&          TMout,
  int32_t                 physicsPartIndex,
  const NMP::DataBuffer&  inputBuffer,
  const NMP::DataBuffer&  fallbackBuffer,
  const NMP::Matrix34&    worldRoot,
  bool                    enforceJointLimits) const
{
  NMP_ASSERT(m_animToPhysicsMap);

  int32_t j = m_animToPhysicsMap->getAnimIndexFromPhysicsIndex(physicsPartIndex);
  NMP_ASSERT(j >= 0);

#if !defined(NMP_PLATFORM_SIMD) || defined(NM_HOST_IOS)

  // Select buffer.
  const NMP::DataBuffer& buffer = selectBufferForInputBufferChannel(inputBuffer, fallbackBuffer, j);

  // walk the hierarchy and accumulate the transforms
  if (enforceJointLimits)
  {
    TMout.initialise(getLimitedJointQuat(j, *buffer.getChannelQuat(j), 1.0f), *buffer.getChannelPos(j));

    while ((j = (int32_t)m_animRigDef->getParentBoneIndex(j)) > 0)
    {
      // The reference transforms are kept between physics nodes. Just animate physics joints!
      int32_t k = m_animToPhysicsMap->getPhysicsIndexFromAnimIndex(j);
      if (k != -1)
      {
        // Select buffer.
        const NMP::DataBuffer& selectedBuffer = selectBufferForInputBufferChannel(inputBuffer, fallbackBuffer, j);
        NMP::Matrix34 T(getLimitedJointQuat(j, *selectedBuffer.getChannelQuat(j), 1.0f), *selectedBuffer.getChannelPos(j));
        TMout.multiply(T);
      }
      else
      {
        NMP::Matrix34 T(*fallbackBuffer.getChannelQuatFast(j), *fallbackBuffer.getChannelPosFast(j));
        TMout.multiply(T);
      }
    }
  }
  else
  {
    TMout.initialise(*buffer.getChannelQuat(j), *buffer.getChannelPos(j));

    while ((j = (int32_t)m_animRigDef->getParentBoneIndex(j)) > 0)
    {
      // The reference transforms are kept between physics nodes. Just animate physics joints!
      int32_t k = m_animToPhysicsMap->getPhysicsIndexFromAnimIndex(j);
      if (k != -1)
      {
        // Select buffer.
        const NMP::DataBuffer& selectedBuffer = selectBufferForInputBufferChannel(inputBuffer, fallbackBuffer, j);
        NMP::Matrix34 T(*selectedBuffer.getChannelQuat(j), *selectedBuffer.getChannelPos(j));
        TMout.multiply(T);
      }
      else
      {
        NMP::Matrix34 T(*fallbackBuffer.getChannelQuatFast(j), *fallbackBuffer.getChannelPosFast(j));
        TMout.multiply(T);
      }
    }
  }

#ifdef TRAJ_IN_CALC_WORLD_SPACE
  j = (int32_t) m_animRigDef->getTrajectoryBoneIndex();
  const NMP::DataBuffer& trajTMBuffer = selectBufferForInputBufferChannel(inputBuffer, fallbackBuffer, j);
  NMP::Matrix34 trajTM(*trajTMBuffer.getChannelQuat(j), *trajTMBuffer.getChannelPos(j));
  TMout = TMout * trajTM;
#endif

  TMout.multiply(worldRoot);

#else // NMP_PLATFORM_SIMD

  NMP::vpu::Matrix TM;

  // Select buffer.
  const NMP::DataBuffer& buffer = selectBufferForInputBufferChannel(inputBuffer, fallbackBuffer, j);

  // walk the hierarchy and accumulate the transforms
  if (enforceJointLimits)
  {
    TM = NMP::vpu::Matrix(getLimitedJointQuat(j, *buffer.getChannelQuatFast(j), 1.0f), *buffer.getChannelPosFast(j));    

    while((j = (int32_t)m_animRigDef->getParentBoneIndex(j)) > 0)
    {
      // The reference transforms are kept between physics nodes. Just animate physics joints!
      int32_t k = m_animToPhysicsMap->getPhysicsIndexFromAnimIndex(j);
      if (k != -1)
      {
        // Select buffer.
        const NMP::DataBuffer& selectedBuffer = selectBufferForInputBufferChannel(inputBuffer, fallbackBuffer, j);
        NMP::vpu::Matrix T(getLimitedJointQuat(j, *selectedBuffer.getChannelQuatFast(j), 1.0f), *selectedBuffer.getChannelPosFast(j));
        TM = T * TM;
      }
      else
      {
        NMP::vpu::Matrix T(*fallbackBuffer.getChannelQuatFast(j), *fallbackBuffer.getChannelPosFast(j));
        TM = T * TM;
      }

    }
  }
  else
  {
    TM = NMP::vpu::Matrix(*buffer.getChannelQuatFast(j), *buffer.getChannelPosFast(j));

    while((j = (int32_t)m_animRigDef->getParentBoneIndex(j)) > 0)
    {
      // The reference transforms are kept between physics nodes. Just animate physics joints!
      int32_t k = m_animToPhysicsMap->getPhysicsIndexFromAnimIndex(j);
      if (k != -1)
      {
        // Select buffer.
        const NMP::DataBuffer& selectedBuffer = selectBufferForInputBufferChannel(inputBuffer, fallbackBuffer, j);
        NMP::vpu::Matrix T(*selectedBuffer.getChannelQuatFast(j), *selectedBuffer.getChannelPosFast(j));
        TM = T * TM;
      }
      else
      {
        NMP::vpu::Matrix T(*fallbackBuffer.getChannelQuatFast(j), *fallbackBuffer.getChannelPosFast(j));
        TM = T * TM;
      }
    }
  }
#ifdef TRAJ_IN_CALC_WORLD_SPACE
  j = (int32_t) m_animRigDef->getTrajectoryBoneIndex();
  const NMP::DataBuffer& trajTMBuffer = selectBufferForInputBufferChannel(inputBuffer, fallbackBuffer, j);
  NMP::vpu::Matrix trajTM(*trajTMBuffer.getChannelQuatFast(j), *trajTMBuffer.getChannelPosFast(j));
  TM = trajTM * TM;
#endif // TRAJ_IN_CALC_WORLD_SPACE

  NMP::vpu::Matrix WR = vpuTM(worldRoot);
  TM = WR * TM;

  TMout = M34vpu(TM);

#endif // NMP_PLATFORM_SIMD

  TMout.orthonormalise();
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRig::calculatePartTMsRelativeToRoot(
  NMP::Matrix34          TMs[],
  const NMP::DataBuffer& inputBuffer,
  const NMP::DataBuffer& fallbackBuffer) const
{
  // This code path requires that parent physics parts always have a smaller index than the child
  const int32_t numParts = getNumParts();
  TMs[0] = NMP::Matrix34::kIdentity;
  for (int32_t iPhysicsPart = 1 ; iPhysicsPart < numParts ; ++iPhysicsPart)
  {
    int32_t iAnimPart = m_animToPhysicsMap->getAnimIndexFromPhysicsIndex(iPhysicsPart);
    int32_t iPhysicsParentPart = getPart(iPhysicsPart)->getParentPartIndex();
    NMP_ASSERT(iAnimPart >= 0);
    NMP_ASSERT(iPhysicsParentPart < iPhysicsPart);
    NMP_ASSERT(iPhysicsParentPart >= 0);

    // If there is no channel for a part in inputBuffer (channel was filtered out),
    // use default pose animation data buffer (fallbackBuffer)
    // instead for that part. It's required to have all channels set.
    const NMP::DataBuffer& buffer = selectBufferForInputBufferChannel(inputBuffer, fallbackBuffer, iAnimPart);

    NMP::Quat partQuat = *buffer.getChannelQuatFast(iAnimPart);
    NMP::Vector3 partPos = *buffer.getChannelPosFast(iAnimPart);

    // We need to potentially account for sparse physics to animation rig mappings.

    // If this joint's physical parent is not the same as its animation parent then some animation joints
    // have been skipped. This means that partQuat and partPos are the local transform of the animation joint
    // but not the physics joint. To calculate the local transform of the physics joint we must accumulate
    // all parent animation joint transforms until an animation joint with a physical equivalent is found.
    int32_t iAnimParent = m_animRigDef->getParentBoneIndex(iAnimPart);
    NMP_ASSERT(iAnimParent >= 0);

    while (m_animToPhysicsMap->getPhysicsIndexFromAnimIndex(iAnimParent) == -1)
    {
      const NMP::DataBuffer& selectedBuffer = selectBufferForInputBufferChannel(inputBuffer, fallbackBuffer, iAnimParent);
      const NMP::Quat* parentQuat = selectedBuffer.getChannelQuatFast(iAnimParent);
      const NMP::Vector3* parentPos = selectedBuffer.getChannelPosFast(iAnimParent);

      partQuat.multiply(*parentQuat, partQuat);
      partPos += *parentPos;

      iAnimParent = m_animRigDef->getParentBoneIndex(iAnimParent);
      NMP_ASSERT(iAnimParent >= 0);
    }
    TMs[iPhysicsPart].initialise(partQuat, partPos);
    TMs[iPhysicsPart].multiply(TMs[iPhysicsParentPart]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRig::addReference()
{
  if (m_refCount == 0)
  {
    addToScene();
  }
  makeDynamic();
  // We need to call this to make sure that compliance etc gets set on parts that were controlled by
  // behaviours.
  restoreAllJointDrivesToDefault();
  m_refCount++;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRig::removeReference()
{
  NMP_ASSERT(m_refCount > 0);
  m_refCount--;

  // We need to call this to make sure that compliance etc gets set on parts that were controlled by
  // behaviours.
  restoreAllJointDrivesToDefault();
  if (m_refCount == 0)
  {
    removeFromScene();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRig::toTransformBuffer(
  NMP::DataBuffer&     outputBuffer,
  const NMP::Matrix34& worldRoot,
  const PartChooser*   partChooser)
{
  NM_BEGIN_PROFILING("PhysicsRig::toTransformBuffer");

  // The following block of code finds animation joints that are sandwiched between physics joints,
  // and sets them to the bind pose. This is needed because the physics rig is allowed to have
  // "missing" joints.
  for (uint32_t animBoneIndex = 0; animBoneIndex < m_animRigDef->getNumBones(); ++animBoneIndex)
  {
    int32_t physicsBoneIndex = m_animToPhysicsMap->getPhysicsIndexFromAnimIndex(animBoneIndex);
    if (physicsBoneIndex != -1)
    { // It's a physics bone, so we must go up the animation hierarchy and set the output buffer for its parents
      // up to the next physical bone to the bind pose.
      int32_t parentAnimBoneIndex = m_animRigDef->getParentBoneIndex(animBoneIndex);
      while (parentAnimBoneIndex != -1 && m_animToPhysicsMap->getPhysicsIndexFromAnimIndex(parentAnimBoneIndex) == -1)
      {
        outputBuffer.setChannelQuat(parentAnimBoneIndex, *m_animRigDef->getBindPoseBoneQuat(parentAnimBoneIndex));
        outputBuffer.setChannelPos(parentAnimBoneIndex, *m_animRigDef->getBindPoseBonePos(parentAnimBoneIndex));
        outputBuffer.setChannelUsed(parentAnimBoneIndex);
        parentAnimBoneIndex = m_animRigDef->getParentBoneIndex(parentAnimBoneIndex);
      }
    }
  }

  for (uint32_t i = 0 ; i < getNumParts() ; ++i)
  {
    if (partChooser && !partChooser->usePart(i))
      continue;

    int32_t j = m_animToPhysicsMap->getAnimIndexFromPhysicsIndex(i);

    if (j != -1) // To generate a transform a physics bone must have a mapping to an animation channel.
    {
      // 1. get the new position / quaternion from physics (world space)
      PhysicsRig::Part* part = m_parts[i];
      NMP::Matrix34 worldTM = part->getTransform();
      worldTM.r[3] = part->getPosition();

      NMP::Matrix34 localTM(NMP::Matrix34::kIdentity);

      uint32_t parentJointId = m_animRigDef->getParentBoneIndex(j); 
      int32_t parentPhysicsId = m_animToPhysicsMap->getPhysicsIndexFromAnimIndex(parentJointId);
      if (parentJointId != (uint32_t)(-1))
      {
        // 2. transform everything in the parent-child relationship
        NMP::Vector3 parentPos;
        NMP::Quat parentQ;

        NMP::Vector3 parentVel, parentAngVel;

        if (parentPhysicsId >= 0)
        {
          PhysicsRig::Part* parentPart = m_parts[parentPhysicsId];

          NMP::Matrix34 parentTMInv = parentPart->getTransform();
          parentTMInv.r[3] = parentPart->getPosition();

          parentTMInv.invert();
          localTM.multiply(worldTM, parentTMInv);
        }
        else
        {
          NMP::Matrix34 parentTMInv;

          BlendOpsBase::accumulateTransformTM(
            parentJointId,
            worldRoot,
            &outputBuffer,
            m_animRigDef,
            parentTMInv);

          parentTMInv.invert();
          localTM.multiply(worldTM, parentTMInv);
        }
      }

      NMP::Quat endQ = localTM.toQuat();
      NMP::Vector3 endPos = localTM.translation();
      outputBuffer.setChannelQuat(j, endQ);
      outputBuffer.setChannelPos(j, endPos);
      outputBuffer.setChannelUsed(j);
    }
  }
  outputBuffer.calculateFullFlag();

  NM_END_PROFILING(); // "PhysicsRig::toTransformBuffer"
}

//----------------------------------------------------------------------------------------------------------------------
#if 1
#if !defined(NMP_PLATFORM_SIMD) || defined(NM_HOST_IOS)
void PhysicsRig::fromTransformBuffer(
  const NMP::DataBuffer&  inputBuffer,
  const NMP::DataBuffer&  previousInputBuffer,
  const NMP::Matrix34&    worldRoot,
  const NMP::Matrix34&    previousWorldRoot,
  float                   dt,
  Dispatcher*             dispatcher)
{
  NMP::MemoryAllocator* allocator = dispatcher->getTempMemoryAllocator();

  int32_t numParts = getNumParts();
  NMP::Matrix34* targetTMs = (NMP::Matrix34*)NMPAllocatorMemAlloc(
    allocator, sizeof(NMP::Matrix34) * (numParts+1), NMP_NATURAL_TYPE_ALIGNMENT);
  NMP_ASSERT(targetTMs);

  NMP::Matrix34* targetTMsOld = (NMP::Matrix34*)NMPAllocatorMemAlloc(
    allocator, sizeof(NMP::Matrix34) * (numParts+1), NMP_NATURAL_TYPE_ALIGNMENT);
  NMP_ASSERT(targetTMsOld);

  calculateWorldSpacePartTMsCacheWithVelocity(
    targetTMs,
    targetTMsOld,
    inputBuffer,
    previousInputBuffer,
    *m_animRigDef->getBindPose()->m_transformBuffer,
    worldRoot,
    previousWorldRoot);

  for (uint32_t i = 0; i < getNumParts(); ++i)
  {
    // setup the actors
    NMP::Matrix34* jointTM = (NMP::Matrix34*)&targetTMs[i+1];
    NMP::Matrix34* jointTMOld = (NMP::Matrix34*)&targetTMsOld[i+1];

    const NMP::Vector3 newPos = jointTM->translation();
    const NMP::Quat newQuat = jointTM->toQuat();

    const NMP::Vector3 oldPos = jointTMOld->translation();
    const NMP::Quat oldQuat = jointTMOld->toQuat();

    NMP::Vector3 vel = dt > 0.0f ? (newPos - oldPos) / dt : NMP::Vector3::InitZero;
    NMP::Quat oldQuatInv = ~oldQuat;
    NMP::Quat deltaQuat;
    deltaQuat.multiply(newQuat, oldQuatInv);
    NMP::Vector3 angVel = dt > 0.0f ? deltaQuat.toRotationVector(false) / dt : NMP::Vector3::InitZero;

    // use the fact that the position of the physics bone is offset so that it is at the same
    // location as the joint morpheme joint
    PhysicsRig::Part* part = m_parts[i];
    part->setTransform(NMP::Matrix34(newQuat, newPos));
    part->setVel(vel);
    part->setAngVel(angVel);
  }
}
#else
//----------------------------------------------------------------------------------------------------------------------
// NMP_PLATFORM_SIMD
void PhysicsRig::fromTransformBuffer(
  const NMP::DataBuffer&  inputBuffer,
  const NMP::DataBuffer&  previousInputBuffer,
  const NMP::Matrix34&    worldRoot,
  const NMP::Matrix34&    previousWorldRoot,
  float                   dt,
  Dispatcher*             dispatcher)
{
  NMP::MemoryAllocator* allocator = dispatcher->getTempMemoryAllocator();

  int32_t numParts = getNumParts();
  NMP::vpu::Matrix* targetTMs = (NMP::vpu::Matrix*)NMPAllocatorMemAlloc(
    allocator, sizeof(NMP::vpu::Matrix) * (numParts+1), NMP_VECTOR_ALIGNMENT);
  NMP_ASSERT(targetTMs);

  NMP::vpu::Matrix* targetTMsOld = (NMP::vpu::Matrix*)NMPAllocatorMemAlloc(
    allocator, sizeof(NMP::vpu::Matrix) * (numParts+1), NMP_VECTOR_ALIGNMENT);
  NMP_ASSERT(targetTMsOld);

  calculateWorldSpacePartTMsCacheWithVelocity(
    targetTMs,
    targetTMsOld,
    inputBuffer,
    previousInputBuffer,
    *m_animRigDef->getBindPose()->m_transformBuffer,
    worldRoot,
    previousWorldRoot);

  for (uint32_t i = 0; i < getNumParts(); ++i)
  {
    // setup the actors
    NMP::Matrix34* jointTM = (NMP::Matrix34*)&targetTMs[i+1];
    NMP::Matrix34* jointTMOld = (NMP::Matrix34*)&targetTMsOld[i+1];

    const NMP::Vector3 newPos = jointTM->translation();
    const NMP::Quat newQuat = jointTM->toQuat();

    const NMP::Vector3 oldPos = jointTMOld->translation();
    const NMP::Quat oldQuat = jointTMOld->toQuat();

    NMP::Vector3 vel = dt > 0.0f ? (newPos - oldPos) / dt : NMP::Vector3::InitZero;
    NMP::Quat oldQuatInv = ~oldQuat;
    NMP::Quat deltaQuat;
    deltaQuat.multiply(newQuat, oldQuatInv);
    NMP::Vector3 angVel = dt > 0.0f ? deltaQuat.toRotationVector(false) / dt : NMP::Vector3::InitZero;

    // use the fact that the position of the physics bone is offset so that it is at the same
    // location as the joint morpheme joint
    PhysicsRig::Part* part = m_parts[i];
    part->setTransform(NMP::Matrix34(newQuat, newPos));
    part->setVel(vel);
    part->setAngVel(angVel);
  }
}
#endif
//----------------------------------------------------------------------------------------------------------------------
#else
void PhysicsRig::fromTransformBuffer(
  const NMP::DataBuffer&  inputBuffer,
  const NMP::DataBuffer&  previousInputBuffer,
  const NMP::Matrix34&    worldRoot,
  const NMP::Matrix34&    previousWorldRoot,
  float                   dt,
  Dispatcher*             NMP_UNUSED(dispatcher))
{
  for (uint32_t i = 0; i < getNumParts(); ++i)
  {
    int32_t j = m_animToPhysicsMap->getAnimIndexFromPhysicsIndex(i);
    // Make sure that the physics root maps an animation bone.
    NMP_ASSERT(j >= 0);

    NMP::Matrix34 channelTM(*inputBuffer.getChannelQuat(j), *inputBuffer.getChannelPos(j));
    NMP::Matrix34 jointTM(NMP::Matrix34::kIdentity);
    NMP::Matrix34 rootToParentTM(NMP::Matrix34::kIdentity);

    NMP::Matrix34 channelTMOld(*previousInputBuffer.getChannelQuat(j), *previousInputBuffer.getChannelPos(j));
    NMP::Matrix34 jointTMOld(NMP::Matrix34::kIdentity);
    NMP::Matrix34 rootToParentTMOld(NMP::Matrix34::kIdentity);

    // Physics TODO: this is extremely inefficient since it doesn't re-use the
    // results for bones nearer the root that have already been calculated.
    if (m_animRigDef->getParentBoneIndex(j) != -1)
    {
      jointTM.set(channelTM);
      jointTMOld.set(channelTMOld);

      // walk the hierarchy and accumulate the transforms
      uint32_t jointId = j;
      while ((jointId = m_animRigDef->getParentBoneIndex(jointId)) != (uint32_t) 0)
      {
        NMP::Matrix34 T;
        NMP::Matrix34 TOld;
        if (inputBuffer.getUsedFlags()->isBitSet(jointId) && previousInputBuffer.getUsedFlags()->isBitSet(jointId))
        {
          T.initialise(*inputBuffer.getChannelQuat(jointId), *inputBuffer.getChannelPos(jointId));
          TOld.initialise(*previousInputBuffer.getChannelQuat(jointId), *previousInputBuffer.getChannelPos(jointId));
        }
        else
        {
          T.initialise(*m_animRigDef->getBindPoseBoneQuat(jointId), *m_animRigDef->getBindPoseBonePos(jointId));
          TOld.initialise(*m_animRigDef->getBindPoseBoneQuat(jointId), *m_animRigDef->getBindPoseBonePos(jointId));
        }
        rootToParentTM.multiply(T);
        rootToParentTMOld.multiply(TOld);
      }

#ifdef TRAJ_IN_FROM_TRANS_BUFFER
      jointId = m_animRigDef->getTrajectoryBoneIndex();
      NMP::Matrix34 trajTM(*inputBuffer.getChannelQuat(jointId), *inputBuffer.getChannelPos(jointId));
      rootToParentTM = trajTM * rootToParentTM;

      NMP::Matrix34 trajTMOld(*previousInputBuffer.getChannelQuat(jointId), *previousInputBuffer.getChannelPos(jointId));
      rootToParentTMOld = trajTMOld * rootToParentTMOld;
#endif

      // accumulate the global trajectory transform.
      rootToParentTM.multiply(worldRoot);
      jointTM.multiply(rootToParentTM); // this is now the transform for the physics bone, in world space

      rootToParentTMOld.multiply(previousWorldRoot);
      jointTMOld.multiply(rootToParentTMOld); // this is now the transform for the physics bone, in world space
    }

    // setup the actor.
    const NMP::Vector3 newPos = jointTM.translation();
    const NMP::Quat newQuat = jointTM.toQuat();

    const NMP::Vector3 oldPos = jointTMOld.translation();
    const NMP::Quat oldQuat = jointTMOld.toQuat();

    NMP::Vector3 vel = dt > 0.0f ? (newPos - oldPos) / dt : NMP::Vector3::InitZero;
    NMP::Quat oldQuatInv = ~oldQuat;
    NMP::Quat deltaQuat;
    deltaQuat.multiply(newQuat, oldQuatInv);
    NMP::Vector3 angVel = dt > 0.0f ? deltaQuat.toRotationVector(false) / dt : NMP::Vector3::InitZero;

    // use the fact that the position of the physics bone is offset so that it is at the same
    // location as the joint morpheme joint
    PhysicsRig::Part* part = m_parts[i];
    part->setTransform(NMP::Matrix34(newQuat, newPos));
    part->setVel(vel);
    part->setAngVel(angVel);
  }
}
#endif

//----------------------------------------------------------------------------------------------------------------------
NMP::Quat PhysicsRig::getLimitedJointQuat(int animBoneIndex, const NMP::Quat &inputQuat, float limitFrac) const
{
  // Result will be a (possibly clamped) copy of the input quat
  NMP::Quat qResult = inputQuat;

  // Get the index of the physics part that corresponds to the given anim bone index
  // If it exists this will be the part attached to the moving frame of the joint (the "child part")
  // 
  int32_t physicsChildPartIndex = m_animToPhysicsMap->getPhysicsIndexFromAnimIndex(animBoneIndex);
  if (physicsChildPartIndex != -1)
  {
    // Get the index of the parent animation bone
    uint32_t animParentBoneIndex = m_animRigDef->getParentBoneIndex(animBoneIndex);
    if (animParentBoneIndex != (uint32_t)(-1))
    {
      // If the animation parent bone exists and has a corresponding physics part, this will be the part that the joint 
      // moves with, (i.e. the joint's "parent part")
      int32_t physicsParentPartIndex = m_animToPhysicsMap->getPhysicsIndexFromAnimIndex(animParentBoneIndex);
      for (uint32_t physicsJointIndex = 0; physicsJointIndex < getNumJoints(); ++physicsJointIndex)
      {
        // We have found the joint if it's parent and child part indexes match the ones we have just found
        const Joint* const joint = getJoint(physicsJointIndex);
        const PhysicsJointDef* jointDef = m_physicsRigDef->m_joints[physicsJointIndex];

        if (jointDef->m_parentPartIndex == physicsParentPartIndex && jointDef->m_childPartIndex == physicsChildPartIndex)
        {
          joint->clampToLimits(qResult, limitFrac, NULL);
        }
      }
    }
  }
  return qResult;
}

//----------------------------------------------------------------------------------------------------------------------
float PhysicsRig::calculateMass() const
{
  float totalMass = 0.f;
  for (uint32_t i = 0; i < getNumParts(); ++i)
  {
    totalMass += getPart(i)->getMass();
  }
  return totalMass;
}

//----------------------------------------------------------------------------------------------------------------------
// This uses the parallel axis theorem (also known as Huygens-Steiner theorem) to provide an adjustment
// for an inertia tensor to convert it to being offset from a different position (point). You would use:
// NMP::Matrix34 originalInertia
// originalInertia.add3x3(inertiaOffsetCorrection(mass, pos - point)
static NMP::Matrix34 inertiaOffsetCorrection(const float &mass, const NMP::Vector3 &offset)
{
  float offsetSq = offset.magnitudeSquared();  

  // This is is a loopless form of the expression:
  // int KDelta(int i, int j) {return i == j ? 1 : 0;}
  // see http://en.wikipedia.org/wiki/Parallel_axis_theorem
  NMP::Matrix34 result;
  result.r[0] = NMP::Vector3(offsetSq - offset[0] * offset[0], -offset[0] * offset[1], -offset[0] * offset[2]);
  result.r[1] = NMP::Vector3(-offset[1] * offset[0], offsetSq - offset[1] * offset[1], -offset[1] * offset[2]);
  result.r[2] = NMP::Vector3(-offset[2] * offset[0], -offset[2] * offset[1], offsetSq - offset[2] * offset[2]);

  result.scale3x3(mass);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Matrix34 PhysicsRig::calculateGlobalInertiaTensor() const
{
  NMP::Vector3 COMPosition = calculateCentreOfMass();

  NMP::Matrix34 globalInertiaTensor(NMP::Matrix34::kZero);
  for (uint32_t i = 0; i < getNumParts(); ++i)
  {
    NMP::Matrix34 partInertiaTensor = getPart(i)->getGlobalInertiaTensor();
    // need to adjust this since it's offset from the rig COM
    NMP::Vector3 offset = getPart(i)->getCOMPosition() - COMPosition;
    partInertiaTensor.add3x3(inertiaOffsetCorrection(getPart(i)->getMass(), offset));
    globalInertiaTensor.add3x3(partInertiaTensor);
  }
  return globalInertiaTensor;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 PhysicsRig::calculateCentreOfMass() const
{
  NMP::Vector3 totalPos(0,0,0);
  float totalMass = 0.f;
  const uint32_t numParts = getNumParts();
  for (uint32_t i = 0; i < numParts; ++i)
  {
    const Part* part = getPart(i);
    float mass = part->getMass();
    totalMass += mass;
    totalPos += part->getCOMPosition() * mass;
  }
  NMP_ASSERT(totalMass > 0.f);
  return totalPos / totalMass;
}

//----------------------------------------------------------------------------------------------------------------------
// Calculates mass, com and inertia for the whole rig or subset thereof specified by entries in the usePart mask if 
// supplied.
//
void PhysicsRig::caclulateMassProperties(
  float& partsMass,
  NMP::Vector3& partsCOMPos,
  NMP::Matrix34& partsGlobalInertia,
  const bool* usePart)
{
  // Mass and centre of mass
  NMP::Vector3 totalPos(0,0,0);
  partsMass = 0.f;
  const uint32_t numParts = getNumParts();
  for (uint32_t i = 0; i < numParts; ++i)
  {
    // Skip parts not flagged for inclusion
    if (!usePart || usePart[i])
    {
      const Part* part = getPart(i);
      float mass = part->getMass();
      partsMass += mass;
      totalPos += part->getCOMPosition() * mass;
    }
  }

  // We only continue with the calculations if we have some mass to work with
  if (partsMass > 0.0f)
  {
    partsCOMPos = totalPos / partsMass;

    // Inertia
    partsGlobalInertia.zero();
    for (uint32_t i = 0; i < getNumParts(); ++i)
    {
      // Skip parts not flagged for inclusion (as above for mass calc).
      if (!usePart || usePart[i])
      {
        NMP::Matrix34 partInertiaTensor = getPart(i)->getGlobalInertiaTensor();
        // Adjust this since it's offset from the rig COM (i.e. transform to global space for summation)
        NMP::Vector3 offset = getPart(i)->getCOMPosition() - partsCOMPos;
        partInertiaTensor.add3x3(inertiaOffsetCorrection(getPart(i)->getMass(), offset));
        partsGlobalInertia.add3x3(partInertiaTensor);
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 PhysicsRig::calculateCentreOfMassVelocity() const
{
  NMP::Vector3 totalVel(0,0,0);
  float totalMass = 0.f;
  const uint32_t numParts = getNumParts();
  for (uint32_t i = 0; i < numParts; i++)
  {
    const Part* part = getPart(i);
    float mass = part->getMass();
    totalMass += mass;
    totalVel  += part->getVel() * mass;
  }
  NMP_ASSERT(totalMass > 0.f);
  return totalVel / totalMass;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Matrix34 PhysicsRig::getJointFrame1World(uint32_t jointIndex) const
{
  const PhysicsJointDef* jointDef = m_physicsRigDef->m_joints[jointIndex];
  int32_t parentPartIndex = jointDef->m_parentPartIndex;
  const Part* parentPart = getPart(parentPartIndex);
  NMP::Matrix34 parentPartTransform = parentPart->getTransform();

  NMP::Matrix34 parentFrame;
  parentFrame.multiply(m_physicsRigDef->m_joints[jointIndex]->m_parentPartFrame, parentPartTransform);
  return parentFrame;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Matrix34 PhysicsRig::getJointFrame2World(uint32_t jointIndex) const
{
  const PhysicsJointDef* jointDef = m_physicsRigDef->m_joints[jointIndex];
  uint32_t childPartIndex = jointDef->m_childPartIndex;
  const Part* childPart = getPart(childPartIndex);
  NMP::Matrix34 childPartTransform = childPart->getTransform();

  NMP::Matrix34 parentFrame;
  parentFrame.multiply(m_physicsRigDef->m_joints[jointIndex]->m_childPartFrame, childPartTransform);
  return parentFrame;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRig::setRigAndAnimToPhysicsMap(AnimRigDef* rigDef, AnimToPhysicsMap* animToPhysicsMap)
{
  m_animRigDef = rigDef;
  m_animToPhysicsMap = animToPhysicsMap;
}

//----------------------------------------------------------------------------------------------------------------------
int32_t PhysicsRig::getJointIndex(int32_t boneIndex1, int32_t boneIndex2) const
{
  uint32_t nJoints = getNumJoints();
  for (uint32_t i = 0 ; i < nJoints ; ++i)
  {
    const PhysicsJointDef* jointDef = m_physicsRigDef->m_joints[i];
    if ((jointDef->m_parentPartIndex == boneIndex1 && jointDef->m_childPartIndex == boneIndex2) ||
        (jointDef->m_parentPartIndex == boneIndex2 && jointDef->m_childPartIndex == boneIndex1))
      return i;
  }
  return -1;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Matrix34 PhysicsRig::getBindPoseTM(uint32_t partIndex)
{
  NMP_ASSERT(partIndex < getNumParts());
  const NMP::DataBuffer& bindPoseBuffer = *getAnimRigDef()->getBindPose()->m_transformBuffer;
  NMP::Matrix34 targetTM;
  calculateWorldSpacePartTM(targetTM, partIndex, bindPoseBuffer, bindPoseBuffer, NMP::Matrix34::kIdentity, false);
  return targetTM;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 PhysicsRig::getBindPoseDimensions() const
{
  const NMP::DataBuffer& bindPoseBuffer = *getAnimRigDef()->getBindPose()->m_transformBuffer;

  NMP::Vector3 minBound(FLT_MAX, FLT_MAX, FLT_MAX);
  NMP::Vector3 maxBound = -minBound;
  for (size_t i = 0; i < getNumParts(); ++i)
  {
    NMP::Matrix34 targetTM;
    calculateWorldSpacePartTM(targetTM, (int32_t) i, bindPoseBuffer, bindPoseBuffer, NMP::Matrix34::kIdentity, false);
    const NMP::Vector3 p = targetTM.translation();
    maxBound.x = NMP::maximum(maxBound.x, p.x);
    maxBound.y = NMP::maximum(maxBound.y, p.y);
    maxBound.z = NMP::maximum(maxBound.z, p.z);
    minBound.x = NMP::minimum(minBound.x, p.x);
    minBound.y = NMP::minimum(minBound.y, p.y);
    minBound.z = NMP::minimum(minBound.z, p.z);
  }
  maxBound -= minBound;
  return maxBound;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRig::integrateAnimation(
  NMP::DataBuffer &transforms,
  NMP::DataBuffer &transformRates,
  float            dt,
  float            rateDamping,
  bool             clampToLimits)
{
  uint32_t buffLength = transforms.getLength(); 
  NMP_ASSERT(buffLength == transformRates.getLength());
  for (uint32_t j = 0; j < buffLength; ++j)
  {
    if (!transforms.getUsedFlags()->isBitSet(j) || !transformRates.getUsedFlags()->isBitSet(j))
      continue;

    NMP::Quat q = *transforms.getChannelQuat(j);
    NMP::Vector3 p = *transforms.getChannelPos(j);

    const NMP::Vector3 &vel = *transformRates.getChannelVel(j);
    NMP::Vector3 &angVel = *transformRates.getChannelAngVel(j);

    angVel *= expf(-dt * rateDamping);

    p += vel * dt;
    q += (NMP::Quat(angVel.x, angVel.y, angVel.z, 0.0f) * q) * (0.5f * dt);
    q.normalise();

    if (clampToLimits)
    {
      // check against the limits. 
      int32_t i = m_animToPhysicsMap->getPhysicsIndexFromAnimIndex(j);
      if (i != -1)
      {
        uint32_t parentJointId = m_animRigDef->getParentBoneIndex(j); 
        int32_t parentPhysicsId = m_animToPhysicsMap->getPhysicsIndexFromAnimIndex(parentJointId);
        if (parentJointId != (uint32_t)(-1))
        {
          for (uint32_t jointIndex = 0 ; jointIndex < getNumJoints() ; ++jointIndex)
          {
            const PhysicsRig::Joint *joint = m_joints[jointIndex];
            const PhysicsJointDef* jointDef = m_physicsRigDef->m_joints[jointIndex];
            if (jointDef->m_parentPartIndex == parentPhysicsId && jointDef->m_childPartIndex == i)
            {
              joint->clampToLimits(q, 1.0f, transforms.getChannelQuat(j));
            } // case when we find the joint
          } // loop to find the joint
        } // parent bone
      } // physics bone corresponding to j
    }
    transforms.setChannelPos(j, p);
    transforms.setChannelQuat(j, q);
    transforms.setChannelUsed(j);
  }
  transforms.calculateFullFlag();
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRig::getQuatFromTransformBuffer(
  uint32_t                partIndex,
  const NMP::DataBuffer&  inputBuffer,
  const NMP::DataBuffer&  fallbackBuffer,
  NMP::Quat&              quat)
{
  const Part* part = getPart(partIndex);
  NMP_ASSERT(part);

  int32_t jointIndex = m_animToPhysicsMap->getAnimIndexFromPhysicsIndex(partIndex);

  // If there is no channel for a joint in inputBuffer (channel was filtered out),
  // use bind pose buffer (fallbackBuffer)
  // instead for that joint. It's required to have all channels set.
  const NMP::DataBuffer& buffer = selectBufferForInputBufferChannel(inputBuffer, fallbackBuffer, jointIndex);

  quat = *buffer.getChannelQuat(jointIndex);

  // We now need to account for potentially sparse physics to animation rig mappings.

  // This is the root joint so curQ and prevQ are correct as they are.
  if (part->getParentPartIndex() != -1)
  {
    // If this joint's physical parent is not the same as its animation parent then some animation joints
    // have been skipped. This means that curQ and prevQ are the local rotation of the animation joint
    // but not the physics joint. To calculate the local rotation of the physics joint we must accumulate
    // all parent animation joint rotations until an animation joint with a physical equivalent is found.
    int32_t parentIndex = m_animRigDef->getParentBoneIndex(jointIndex);
    NMP_ASSERT(parentIndex != -1);

    while (m_animToPhysicsMap->getPhysicsIndexFromAnimIndex(parentIndex) == -1)
    {
      const NMP::DataBuffer& selectedBuffer = selectBufferForInputBufferChannel(inputBuffer, fallbackBuffer, parentIndex);
      const NMP::Quat* parentQuat = selectedBuffer.getChannelQuat(parentIndex);

      quat.multiply(*parentQuat, quat);

      parentIndex = m_animRigDef->getParentBoneIndex(parentIndex);
      NMP_ASSERT(parentIndex != -1);
    }
  }
}

#if defined(MR_OUTPUT_DEBUGGING)
//----------------------------------------------------------------------------------------------------------------------
uint32_t PhysicsRig::serializeTxPersistentData(
  MR::AnimSetIndex animSetIndex,
  void* outputBuffer,
  uint32_t NMP_USED_FOR_ASSERTS(outputBufferSize)) const
{
  uint32_t dataSize = sizeof(PhysicsRigPersistentData);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    PhysicsRigPersistentData* outputData = (PhysicsRigPersistentData*)outputBuffer;
    outputData->m_numParts = getNumParts();
    outputData->m_numJoints = getNumJoints();
    outputData->m_animSetIndex = animSetIndex;

    NMP::netEndianSwap(outputData->m_numParts);
    NMP::netEndianSwap(outputData->m_numJoints);
    NMP::netEndianSwap(outputData->m_animSetIndex);
  }
  return dataSize;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t PhysicsRig::serializeTxFrameData(
  MR::AnimSetIndex animSetIndex,
  void* outputBuffer,
  uint32_t NMP_USED_FOR_ASSERTS(outputBufferSize)) const
{
  uint32_t dataSize = sizeof(PhysicsRigFrameData);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    PhysicsRigFrameData* outputData = (PhysicsRigFrameData*)outputBuffer;
    outputData->m_refCount = m_refCount;
    outputData->m_animSetIndex = animSetIndex;

    NMP::netEndianSwap(outputData->m_refCount);
    NMP::netEndianSwap(outputData->m_animSetIndex);
  }
  return dataSize;
}
#endif // MR_OUTPUT_DEBUGGING


//----------------------------------------------------------------------------------------------------------------------
void PhysicsRig::writeJointLimits()
{
  if (m_jointLimitsModified)
  {
    // Update joint limits on physics engine representation of rig.
    for (uint32_t i = 0, numJoints = getNumJoints(); i < numJoints; ++i)
    {
      getJoint(i)->writeLimits();
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRig::resetJointLimits()
{
  if (m_jointLimitsModified)
  {
    // Copy joint limits from joint def.
    for (uint32_t i = 0, numJoints = getNumJoints(); i < numJoints; ++i)
    {
      getJoint(i)->resetLimits();
    }

    m_jointLimitsModified = false;
  }
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
