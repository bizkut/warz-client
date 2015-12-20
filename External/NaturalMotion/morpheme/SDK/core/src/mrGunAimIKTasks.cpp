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
#include "morpheme/mrCommonTasks.h"
#include "morpheme/mrDebugMacros.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrCharacterControllerAttribData.h"

#include "NMGeomUtils/NMGeomUtils.h"
#include "NMIK/NMPointIK.h"
#include "NMIK/NMTwoBoneIK.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
namespace
{

//----------------------------------------------------------------------------------------------------------------------
void moveArm(
  const AnimRigDef* rig,
  int32_t wristIndex,
  const NMP::Vector3& elbowAxis,
  NMP::DataBuffer* transformBuffer,
  NMRU::GeomUtils::PosQuat& wristTarget,
  bool updateWristTarget,
  bool applyJointLimits,
  const AttribDataJointLimits* jointLimitsAttrib)
{
  // Retrieve the joint indices of the other joints in the arm.
  // Currently we are assuming the arm is a singly-linked sequence going wrist-elbow-shoulder.
  int32_t jointIndex[3];
  jointIndex[2] = wristIndex;
  jointIndex[1] = rig->getParentBoneIndex(jointIndex[2]);
  NMP_ASSERT(jointIndex[1] > 0);
  jointIndex[0] = rig->getParentBoneIndex(jointIndex[1]);
  NMP_ASSERT(jointIndex[0] > 0);

  // Get the world transform of the parent of the root joint, to relate the IK chain coordinate frame
  // to the target coordinate frame.
  NMRU::GeomUtils::PosQuat rootParentT = NMRU::GeomUtils::identityPosQuat();
  int32_t parentBoneIndex = jointIndex[0];
  while ((parentBoneIndex = rig->getParentBoneIndex(parentBoneIndex)) > 0)
  {
    const NMP::Vector3* p = transformBuffer->getPosQuatChannelPos(parentBoneIndex);
    const NMP::Quat* q = transformBuffer->getPosQuatChannelQuat(parentBoneIndex);
    if (!transformBuffer->hasChannel(parentBoneIndex))
    {
      p = rig->getBindPoseBonePos(parentBoneIndex);
      q = rig->getBindPoseBoneQuat(parentBoneIndex);
    }
    NMRU::GeomUtils::PosQuat jointT = {*p, *q};
    NMRU::GeomUtils::multiplyPosQuat(rootParentT, jointT);
  }

  // Set up the IK geometry, adding missing channels from the bind pose and activating them in the
  // transform buffer, since we are modifying them.
  NMRU::GeomUtils::PosQuat ikGeometry[4];
  for (uint32_t i = 0; i < 3; ++i)
  {
    int32_t j = jointIndex[i];
    if (!transformBuffer->hasChannel(j))
    {
      ikGeometry[i].t = *rig->getBindPoseBonePos(j);
      ikGeometry[i].q = *rig->getBindPoseBoneQuat(j);
      transformBuffer->setChannelUsed(j);
    }
    else
    {
      ikGeometry[i].t = *transformBuffer->getPosQuatChannelPos(j);
      ikGeometry[i].q = *transformBuffer->getPosQuatChannelQuat(j);
    }
  }
  // The fourth 'joint' is the end effector, which is co-located with the wrist in this case.
  ikGeometry[3] = NMRU::GeomUtils::identityPosQuat();

  // Get the TwoBoneIK parameters
  NMRU::TwoBoneIK::Params ikParams;
  ikParams.hingeAxis = elbowAxis;
  ikParams.endJointOrientationWeight = 1.0f;

  // Get the wrist transform in the coordinate frame of the root parent
  NMRU::GeomUtils::PosQuat target = wristTarget;
  NMRU::GeomUtils::multiplyInversePosQuat(target, rootParentT);

  //---------------------------------
  // SOLVE
  NMRU::TwoBoneIK::solveWithEndJoint(&ikParams, ikGeometry, target);

  // Apply joint limits
  if (applyJointLimits)
  {
    NMP_ASSERT(jointLimitsAttrib);
    for (uint32_t i = 0; i < 3; ++i)
    {
      int32_t j = jointIndex[i];
      const NMRU::JointLimits::Params* jointLimit = jointLimitsAttrib->getLimitForJoint(j);
      if (jointLimit)
      {
        NMRU::JointLimits::clamp(*jointLimit, ikGeometry[i].q);
      }
    }
  }

  // Get result
  for (uint32_t i = 0; i < 3; ++i)
  {
    int32_t j = jointIndex[i];
    transformBuffer->setPosQuatChannelPos(j, ikGeometry[i].t);
    transformBuffer->setPosQuatChannelQuat(j, ikGeometry[i].q);
  }

  // Given the limits of reach and joint limits, we may not have hit the target - recalculate where
  // we actually got the wrist to.
  if (updateWristTarget)
  {
    NMRU::GeomUtils::multiplyPosQuat(wristTarget, ikGeometry[2], ikGeometry[1]);
    NMRU::GeomUtils::multiplyPosQuat(wristTarget, ikGeometry[0]);
    NMRU::GeomUtils::multiplyPosQuat(wristTarget, rootParentT);
  }
}

//----------------------------------------------------------------------------------------------------------------------
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
void TaskGunAimSetup(Dispatcher::TaskParameters* parameters)
{
  //--------------------------------------------
  // Task parameters

  // The character controller, for determining the world-space location of the character
  AttribDataCharacterProperties* characterControllerAttrib =
    parameters->getInputAttrib<AttribDataCharacterProperties>(0, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES);

  // The output trajectory transform, extracted from the character controller
  AttribDataTransform* trajectoryTransform =
    parameters->createOutputAttrib<AttribDataTransform>(1, ATTRIB_SEMANTIC_TRAJECTORY_TRANSFORM);

  // Copy across
  trajectoryTransform->m_pos = characterControllerAttrib->m_prevWorldRootTransform.translation();
  trajectoryTransform->m_att = characterControllerAttrib->m_prevWorldRootTransform.toQuat();
}

//----------------------------------------------------------------------------------------------------------------------
namespace
{

//----------------------------------------------------------------------------------------------------------------------
void subTaskGunAimTransforms(
  Dispatcher::TaskParameters* parameters,
  const AttribDataRig* rigAttrib,
  const AttribDataTransformBuffer* localBufferAttrib,
  const NMP::Vector3& deltaPos,
  const NMP::Quat& deltaQuat,
  const AttribDataVector3* targetPosAttrib,
  const AttribDataFloat* blendWeightAttrib,
  const AttribDataGunAimSetup* gunAimSetupAttrib,
  const AttribDataGunAimIKChain* gunAimChainAttrib,
  AttribDataTransformBuffer* outputBufferAttrib,
  const NMP::Vector3& trajectoryPos,
  const NMP::Quat& trajectoryQuat,
  const AttribDataJointLimits* jointLimitsAttrib)
{
  // Aliases for task parameters and their contents
  const AnimRigDef* rig = rigAttrib->m_rig;
  NMP::DataBuffer* outputBuffer = outputBufferAttrib->m_transformBuffer;
  const NMP::DataBuffer* localBuffer = localBufferAttrib->m_transformBuffer;
  const int32_t& gunBindJointIndex = gunAimChainAttrib->m_gunBindJointIndex;
  const int32_t& rootJointIndex = gunAimChainAttrib->m_spineRootJointIndex;
  const bool& updateTargetByDeltas = gunAimChainAttrib->m_updateTargetByDeltas;
  const bool& useSecondaryArm = gunAimChainAttrib->m_useSecondaryArm;
  const NMP::Vector3& gunPivotOffset = gunAimChainAttrib->m_gunPivotOffset;
  const NMP::Vector3& gunBarrelOffset = gunAimChainAttrib->m_gunBarrelOffset;
  const uint32_t& numAimJoints = gunAimChainAttrib->m_numAimJoints;
  const int32_t& gunJointIndex = gunAimChainAttrib->m_gunJointIndex;
  // targetPos is a copy because we modify it - adjust it for trajectory deltas
  NMP::Vector3 targetPos = targetPosAttrib->m_value;

  // Connected control param providing blend weight.
  float blendWeight = NMP::clampValue(blendWeightAttrib->m_value, 0.0f, 1.0f);

  // Early declaration of some variables that we need in the outer scope for arm IK.
  NMRU::GeomUtils::PosQuat primaryHandLocalT = NMRU::GeomUtils::identityPosQuat();
  NMRU::GeomUtils::PosQuat secondaryHandLocalT = primaryHandLocalT;
  NMRU::GeomUtils::PosQuat newEndEffectorT = NMRU::GeomUtils::identityPosQuat();

  // Placing these block braces around the code helps to reduce the number of
  // parameters on the stack because they go out of scope when the close brace is encountered
  // and before the two arm IK is called.
  // This was done in particular to aid memory usage on PS3/SPU version
  {
    // Copy input buffer to output buffer.  I will then be modifying the output buffer at the
    // joints of the chain.
    localBuffer->copyTo(outputBuffer);

    // For world-space targets, retrieve the current trajectory and move the target into character space.
    // We have to use the previous frame's trajectory because the current frame's value may contain some but
    // not necessarily all of the character motion for this frame, depending whether we are pre- or post-physics.
    if (gunAimSetupAttrib->m_worldSpaceTarget)
    {
      // Put the target into the space of the character at the last frame
      targetPos = trajectoryQuat.inverseRotateVector(targetPos - trajectoryPos);

      // Update for the trajectory deltas calculated by the network so far
      targetPos = deltaQuat.inverseRotateVector(targetPos - deltaPos);
    }
    // For character-space targets: if required, update the target transform by one trajectory delta.
    // This is useful when you are targeting world-space coordinates but, when converting
    // this to character space, were only able to retrieve the character root transform
    // at the last update, when obviously you really wanted the target transform relative
    // to the character after /this/ update.
    else if (updateTargetByDeltas)
    {
      targetPos = deltaQuat.inverseRotateVector(targetPos - deltaPos);
    }

    MR_DEBUG_DRAW_POINT(parameters->m_dispatcher->getDebugInterface(), targetPos, 1.0f, NMP::Colour::RED);

    //--------------------------------------------
    // OBTAIN GEOMETRY VIA FK

    // We have four different objects that do not have to have a shared root in the hierarchy
    // for the node to work correctly - the gun, the bind joint, and the two wrists.  Consequently
    // they will each need to be FK'd out to the world to get their relative geometry.
    // An optimisation would be to find a shared root, which would allow us to do reduce the
    // amount of FKing.  Alternatively store a worldspace geometry buffer and only FK joints that
    // do not already have their worldspace transform calculated.

    // FK to get world gun pivot transform
    NMRU::GeomUtils::PosQuat gunPivotTransform;
    gunPivotTransform.q.identity();
    gunPivotTransform.t = gunPivotOffset;
    int32_t j = gunJointIndex;
    do
    {
      const NMP::Vector3* p = outputBuffer->getPosQuatChannelPos(j);
      const NMP::Quat* q = outputBuffer->getPosQuatChannelQuat(j);
      if (!outputBuffer->hasChannel(j))
      {
        p = rig->getBindPoseBonePos(j);
        q = rig->getBindPoseBoneQuat(j);
      }
      NMRU::GeomUtils::PosQuat jointT = {*p, *q};
      NMRU::GeomUtils::multiplyPosQuat(gunPivotTransform, jointT);
    }
    while((j = rig->getParentBoneIndex(j)) > 0);
    //
    // Make sure the gun joint channel is used in case it is moved
    outputBuffer->setChannelUsed(gunJointIndex);

    // FK to get gun bind joint in root parent frame.
    // These joints will be IKed, so we must activate them if they are unused channels.
    NMRU::GeomUtils::PosQuat bindJointT = NMRU::GeomUtils::identityPosQuat();
    j = gunBindJointIndex;
    NMP_ASSERT(outputBuffer->hasChannel(j));
    do
    {
      const NMP::Vector3* p = outputBuffer->getPosQuatChannelPos(j);
      const NMP::Quat* q = outputBuffer->getPosQuatChannelQuat(j);
      if (!outputBuffer->hasChannel(j))
      {
        p = rig->getBindPoseBonePos(j);
        q = rig->getBindPoseBoneQuat(j);
        outputBuffer->setChannelUsed(j);
      }
      NMRU::GeomUtils::PosQuat jointT = {*p, *q};
      NMRU::GeomUtils::multiplyPosQuat(bindJointT, jointT);
    }
    while ((j = rig->getParentBoneIndex(j)) >= (signed)rootJointIndex);

    // FK the root joint to the world, to get the coordinate frame of the parent of the root
    NMRU::GeomUtils::PosQuat rootParentT = NMRU::GeomUtils::identityPosQuat();
    j = rootJointIndex;
    while ((j = rig->getParentBoneIndex(j)) > 0)
    {
      const NMP::Vector3* p = outputBuffer->getPosQuatChannelPos(j);
      const NMP::Quat* q = outputBuffer->getPosQuatChannelQuat(j);
      if (!outputBuffer->hasChannel(j))
      {
        p = rig->getBindPoseBonePos(j);
        q = rig->getBindPoseBoneQuat(j);
      }
      NMRU::GeomUtils::PosQuat jointT = {*p, *q};
      NMRU::GeomUtils::multiplyPosQuat(rootParentT, jointT);
    }

    // Get the end effector (the gun barrel) in the frame of the pivot (its parent).  This is fixed.
    NMRU::GeomUtils::PosQuat gunBarrelLocalT;
    gunBarrelLocalT.q.identity();
    gunBarrelLocalT.t = gunBarrelOffset - gunPivotOffset;

    // Get the gun pivot in the frame of the bind joint (its parent).  This will rotate.
    NMRU::GeomUtils::PosQuat gunPivotLocalT;
    NMRU::GeomUtils::PosQuat bindJointWorldT;
    NMRU::GeomUtils::multiplyPosQuat(bindJointWorldT, bindJointT, rootParentT);
    NMRU::GeomUtils::multiplyInversePosQuat(gunPivotLocalT, gunPivotTransform, bindJointWorldT);

    //--------------------------------------------
    // Geometry needed for arm IK, must be calculated before we move the spine

    // Get the end effector in the world frame
    NMRU::GeomUtils::PosQuat endEffectorWorldT;
    NMRU::GeomUtils::multiplyPosQuat(endEffectorWorldT, gunBarrelLocalT, gunPivotLocalT);
    NMRU::GeomUtils::multiplyPosQuat(endEffectorWorldT, bindJointWorldT);

    // FK from the primary wrist to get its transform in the frame of the end effector.
    // Activate any unused channels in the primary arm IK chain.
    j = gunAimChainAttrib->m_primaryWristJointIndex;
    uint32_t jointCount = 0;
    do
    {
      const NMP::Vector3* p = outputBuffer->getPosQuatChannelPos(j);
      const NMP::Quat* q = outputBuffer->getPosQuatChannelQuat(j);
      if (!outputBuffer->hasChannel(j))
      {
        p = rig->getBindPoseBonePos(j);
        q = rig->getBindPoseBoneQuat(j);
        // If we are still in the two-bone IK chain, activate unused joints because we'll be
        // moving them.
        if (jointCount < 3)
        {
          outputBuffer->setChannelUsed(j);
        }
      }
      NMRU::GeomUtils::PosQuat jointT = {*p, *q};
      NMRU::GeomUtils::multiplyPosQuat(primaryHandLocalT, jointT);
      ++jointCount;
    }
    while ((j = rig->getParentBoneIndex(j)) > 0);
    NMRU::GeomUtils::multiplyInversePosQuat(primaryHandLocalT, endEffectorWorldT);

    if (useSecondaryArm)
    {
      // FK from the secondary wrist to get its transform in the frame of the end effector
      // Activate any unused channels in the secondary arm IK chain.
      j = gunAimChainAttrib->m_secondaryWristJointIndex;
      jointCount = 0;
      do
      {
        const NMP::Vector3* p = outputBuffer->getPosQuatChannelPos(j);
        const NMP::Quat* q = outputBuffer->getPosQuatChannelQuat(j);
        if (!outputBuffer->hasChannel(j))
        {
          p = rig->getBindPoseBonePos(j);
          q = rig->getBindPoseBoneQuat(j);
          // If we are still in the two-bone IK chain, activate unused joints because we'll be
          // moving them.
          if (jointCount < 3)
          {
            outputBuffer->setChannelUsed(j);
          }
        }
        NMRU::GeomUtils::PosQuat jointT = {*p, *q};
        NMRU::GeomUtils::multiplyPosQuat(secondaryHandLocalT, jointT);
        ++jointCount;
      }
      while ((j = rig->getParentBoneIndex(j)) > 0);
      NMRU::GeomUtils::multiplyInversePosQuat(secondaryHandLocalT, endEffectorWorldT);
    }

    //--------------------------------------------
    // Memory allocation for working buffers

    // Get temp allocator.
    NMP::TempMemoryAllocator* tempAllocator = parameters->m_dispatcher->getTempMemoryAllocator();
    NMP_ASSERT(tempAllocator);
    NMP::TempMemoryAllocator *childAllocator = tempAllocator->createChildAllocator();

    // Allocate an array to hold the joint indices associated with each joint
    NMP::Memory::Format format(sizeof(uint32_t) * numAimJoints, NMP_NATURAL_TYPE_ALIGNMENT);
    NMP::Memory::Resource res = NMPAllocatorAllocateFromFormat(childAllocator, format);
    uint32_t* jointIndexMap = (uint32_t*)res.ptr;

    // We have two more NMIK joints than real joints, because NMIK treats the end effector as a joint
    // (albeit fixed), and the pivot point on the gun stock counts as a joint.
    uint32_t numIKJoints = numAimJoints + 2;

    // Allocate memory for the NMIK geometry
    format.set(sizeof(NMRU::GeomUtils::PosQuat) * numIKJoints, NMP_VECTOR_ALIGNMENT);
    res = NMPAllocatorAllocateFromFormat(childAllocator, format);
    NMRU::GeomUtils::PosQuat* ikGeometry = (NMRU::GeomUtils::PosQuat*)res.ptr;

    // Allocate memory for the NMIK index map (to conform with PointIK input data requirements)
    format.set(sizeof(uint32_t) * numIKJoints, NMP_NATURAL_TYPE_ALIGNMENT);
    res = NMPAllocatorAllocateFromFormat(childAllocator, format);
    uint32_t* ikJointMap = (uint32_t*)res.ptr;

    //--------------------------------------------
    // PointIK solver set up

    // Create index maps
    j = gunBindJointIndex;
    ikJointMap[numAimJoints] = numAimJoints;
    ikJointMap[numAimJoints + 1] = numAimJoints + 1;
    for (int32_t i = (numAimJoints - 1); i >= 0; --i, j = rig->getParentBoneIndex(j))
    {
      NMP_ASSERT(outputBuffer->hasChannel(j));
      jointIndexMap[i] = j;
      ikJointMap[i] = i;
    }

    // Fill the IK geometry
    for (uint32_t i = 0; i < numAimJoints; ++i)
    {
      uint32_t jointIndex = jointIndexMap[i];
      ikGeometry[i].t = *outputBuffer->getPosQuatChannelPos(jointIndex);
      ikGeometry[i].q = *outputBuffer->getPosQuatChannelQuat(jointIndex);
    }
    // Set the bind joint geometry
    ikGeometry[numAimJoints] = gunPivotLocalT;
    // Set the end effector geometry
    ikGeometry[numIKJoints - 1] = gunBarrelLocalT;

    //--------------------------------------------
    // SOLVE

    NMRU::GeomUtils::PosQuat target;
    target.t = targetPos;
    target.q.identity();
    NMRU::PointIK::solve(
      gunAimChainAttrib->m_ikParams, ikGeometry, ikJointMap,
      numIKJoints, target, &rootParentT);

    //--------------------------------------------
    // Set results in output buffer as weighted blends, and retrieve the new end effector (barrel)
    // location in the world.
    for (uint32_t i = 0; i < numAimJoints; ++i)
    {
      uint32_t jointIndex = jointIndexMap[i];
      NMP_ASSERT(outputBuffer->hasChannel(jointIndex));
      NMP::Vector3* outputPos = outputBuffer->getPosQuatChannelPos(jointIndex);
      NMP::Quat* outputQuat = outputBuffer->getPosQuatChannelQuat(jointIndex);
      NMRU::GeomUtils::shortestArcFastSlerp(*outputQuat, ikGeometry[i].q, blendWeight);

      NMRU::GeomUtils::PosQuat jointT = {*outputPos, *outputQuat};
      NMRU::GeomUtils::premultiplyPosQuat(jointT, newEndEffectorT);
    }
    // Blend the pivot rotation appropriately when FKing the end effector
    NMRU::GeomUtils::shortestArcFastSlerp(gunPivotLocalT.q, ikGeometry[numAimJoints].q, blendWeight);
    NMRU::GeomUtils::premultiplyPosQuat(gunPivotLocalT, newEndEffectorT);
    NMRU::GeomUtils::premultiplyPosQuat(gunBarrelLocalT, newEndEffectorT);
    NMRU::GeomUtils::multiplyPosQuat(newEndEffectorT, rootParentT);

    //--------------------------------------------
    // Release working memory
    childAllocator->memFree(jointIndexMap);
    childAllocator->memFree(ikGeometry);
    childAllocator->memFree(ikJointMap);
    tempAllocator->destroyChildAllocator(childAllocator);
  }

  // Test whether the gun is parented to the wrist
  bool gunIsParentedToPrimaryWrist = gunJointIndex > 0 &&
    rig->getParentBoneIndex(gunJointIndex) != (signed)gunAimChainAttrib->m_primaryWristJointIndex;

  //--------------------------------------------
  // Primary arm IK

  // Calculate desired primary hand transform after spine movement.
  NMRU::GeomUtils::PosQuat armWorldTargetT = primaryHandLocalT;
  NMRU::GeomUtils::multiplyPosQuat(armWorldTargetT, newEndEffectorT);

  // Do IK for primary arm
  moveArm(
    rig, gunAimChainAttrib->m_primaryWristJointIndex,
    gunAimChainAttrib->m_primaryArmHingeAxis,
    outputBuffer, armWorldTargetT, gunIsParentedToPrimaryWrist,
    gunAimSetupAttrib->m_applyJointLimits && jointLimitsAttrib, jointLimitsAttrib);

  //--------------------------------------------
  // Set the new gun joint transform

  // Don't do this in the common circumstance that the gun joint is parented to the primary
  // wrist, because that is a waste of time - the gun will automatically be in the right
  // place in this case.
  // OPTIMISE  Further improvements could be made depending on typical rig setups.  For instance,
  // where the gun and IK systems share a root we need only FK the parent of that root out to world
  // space once, because that transform will not change.
  if (gunIsParentedToPrimaryWrist)
  {
    // Recalculate gun world transform
    NMRU::GeomUtils::PosQuat gunFinalT = armWorldTargetT;
    NMRU::GeomUtils::premultiplyInversePosQuat(primaryHandLocalT, gunFinalT);
    gunFinalT.t -= gunFinalT.q.rotateVector(gunBarrelOffset);

    // Now apply this to the gun joint to ensure the gun is in the correct position.
    //
    // Find gun parent world transform
    NMRU::GeomUtils::PosQuat gunParentWorldT = NMRU::GeomUtils::identityPosQuat();
    int32_t j = gunJointIndex;
    while ((j = rig->getParentBoneIndex(j)) > 0)
    {
      const NMP::Vector3* p = outputBuffer->getPosQuatChannelPos(j);
      const NMP::Quat* q = outputBuffer->getPosQuatChannelQuat(j);
      if (!outputBuffer->hasChannel(j))
      {
        p = rig->getBindPoseBonePos(j);
        q = rig->getBindPoseBoneQuat(j);
      }
      NMRU::GeomUtils::PosQuat jointT = {*p, *q};
      NMRU::GeomUtils::multiplyPosQuat(gunParentWorldT, jointT);
    }
    //
    // Apply the inverse of the parent to the gun joint and set its local transform in the output
    NMP_ASSERT(outputBuffer->hasChannel(gunJointIndex));
    NMRU::GeomUtils::multiplyInversePosQuat(gunFinalT, gunParentWorldT);
    outputBuffer->setChannelPos(gunJointIndex, gunFinalT.t);
    outputBuffer->setChannelQuat(gunJointIndex, gunFinalT.q);
  }

  //--------------------------------------------
  // Secondary arm IK

  if (useSecondaryArm)
  {
    // Calculate desired secondary hand transform after spine movement.
    NMRU::GeomUtils::multiplyPosQuat(armWorldTargetT, secondaryHandLocalT, newEndEffectorT);

    // Do IK for primary arm
    moveArm(
      rig, gunAimChainAttrib->m_secondaryWristJointIndex,
      gunAimChainAttrib->m_secondaryArmHingeAxis,
      outputBuffer, armWorldTargetT, false,
      gunAimSetupAttrib->m_applyJointLimits && jointLimitsAttrib, jointLimitsAttrib);
  }

  // This node may have added channels, so we need to recalculate the Full flag
  outputBuffer->setFullFlag(outputBuffer->getUsedFlags()->calculateAreAllSet());
}

  //---------------------------------------------------------------------------------------------------------------------- 
}

//----------------------------------------------------------------------------------------------------------------------
void TaskGunAimTransforms(Dispatcher::TaskParameters* parameters)
{
  //--------------------------------------------
  // Task parameters

  // Local buffer - the input transforms
  const AttribDataTransformBuffer* localBufferAttrib =
    parameters->getInputAttrib<AttribDataTransformBuffer>(0, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);

  // Trajectory delta for this frame
  const AttribDataTrajectoryDeltaTransform* deltaTransformAttrib =
    parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);

  // Target to aim at
  const AttribDataVector3* targetPosAttrib =
    parameters->getInputAttrib<AttribDataVector3>(2, ATTRIB_SEMANTIC_CP_VECTOR3);

  // Weight for blending between input and aiming pose
  const AttribDataFloat* blendWeightAttrib =
    parameters->getInputAttrib<AttribDataFloat>(3, ATTRIB_SEMANTIC_CP_FLOAT);

  // Contains the non-anim-set dependent node attributes
  const AttribDataGunAimSetup* gunAimSetupAttrib = parameters->getInputAttrib<AttribDataGunAimSetup>(4, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  // Contains the anim-set dependent node attributes
  const AttribDataGunAimIKChain* gunAimChainAttrib = parameters->getInputAttrib<AttribDataGunAimIKChain>(5, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);

  // The rig - hierarchy and bind pose
  const AttribDataRig* rigAttrib = parameters->getInputAttrib<AttribDataRig>(6, ATTRIB_SEMANTIC_RIG);
  const uint32_t numRigJoints = rigAttrib->m_rig->getNumBones();

  // The output transforms
  AttribDataTransformBuffer* outputBufferAttrib = parameters->createOutputAttribTransformBuffer(7, numRigJoints, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);

  // The world space trajectory transform from the previous frame
  const AttribDataTransform* trajectoryTransform =
    parameters->getInputAttrib<AttribDataTransform>(8, ATTRIB_SEMANTIC_TRAJECTORY_TRANSFORM);

  // The kinematic joint limits for this rig, which may or may not exist
  const AttribDataJointLimits* jointLimitsAttrib =
    parameters->getInputAttrib<AttribDataJointLimits>(9, ATTRIB_SEMANTIC_JOINT_LIMITS);

  subTaskGunAimTransforms(
    parameters,
    rigAttrib,
    localBufferAttrib,
    deltaTransformAttrib->m_deltaPos,
    deltaTransformAttrib->m_deltaAtt,
    targetPosAttrib,
    blendWeightAttrib,
    gunAimSetupAttrib,
    gunAimChainAttrib,
    outputBufferAttrib,
    trajectoryTransform->m_pos,
    trajectoryTransform->m_att,
    jointLimitsAttrib);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskGunAimTrajectoryDeltaAndTransforms(Dispatcher::TaskParameters* parameters)
{
  //--------------------------------------------
  // Task parameters

  // Local buffer - the input transforms
  const AttribDataTransformBuffer* localBufferAttrib =
    parameters->getInputAttrib<AttribDataTransformBuffer>(0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);

  // Target to aim at
  const AttribDataVector3* targetPosAttrib =
    parameters->getInputAttrib<AttribDataVector3>(1, ATTRIB_SEMANTIC_CP_VECTOR3);

  // Weight for blending between input and aiming pose
  const AttribDataFloat* blendWeightAttrib =
    parameters->getInputAttrib<AttribDataFloat>(2, ATTRIB_SEMANTIC_CP_FLOAT);

  // Contains the non-anim-set dependent node attributes
  const AttribDataGunAimSetup* gunAimSetupAttrib = parameters->getInputAttrib<AttribDataGunAimSetup>(3, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  // Contains the anim-set dependent node attributes
  const AttribDataGunAimIKChain* gunAimChainAttrib = parameters->getInputAttrib<AttribDataGunAimIKChain>(4, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);

  // The rig - hierarchy and bind pose
  const AttribDataRig* rigAttrib = parameters->getInputAttrib<AttribDataRig>(5, ATTRIB_SEMANTIC_RIG);
  const uint32_t numRigJoints = rigAttrib->m_rig->getNumBones();

  // The output transforms
  AttribDataTransformBuffer* outputBufferAttrib = parameters->createOutputAttribTransformBuffer(6, numRigJoints, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);

  // The world space trajectory transform from the previous frame
  const AttribDataCharacterProperties* characterController =
    parameters->getInputAttrib<AttribDataCharacterProperties>(7, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES);
  const NMP::Vector3& previousWorldPos = characterController->m_prevWorldRootTransform.translation();
  const NMP::Quat previousWorldOrientation = characterController->m_prevWorldRootTransform.toQuat();

  // The kinematic joint limits for this rig, which may or may not exist
  const AttribDataJointLimits* jointLimitsAttrib =
    parameters->getInputAttrib<AttribDataJointLimits>(8, ATTRIB_SEMANTIC_JOINT_LIMITS);

  subTaskGunAimTransforms(
    parameters,
    rigAttrib,
    localBufferAttrib,
    *localBufferAttrib->m_transformBuffer->getPosQuatChannelPos(0),
    *localBufferAttrib->m_transformBuffer->getPosQuatChannelQuat(0),
    targetPosAttrib,
    blendWeightAttrib,
    gunAimSetupAttrib,
    gunAimChainAttrib,
    outputBufferAttrib,
    previousWorldPos,
    previousWorldOrientation,
    jointLimitsAttrib);
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataGunAimSetup functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataGunAimSetup::locate(AttribData* target)
{
  AttribDataGunAimSetup* result = (AttribDataGunAimSetup*)target;

  AttribData::locate(target);

  NMP::endianSwap(result->m_worldUpAxis);
  NMP::endianSwap(result->m_keepUpright);
  NMP::endianSwap(result->m_worldSpaceTarget);
  NMP::endianSwap(result->m_applyJointLimits);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataGunAimSetup::dislocate(AttribData* target)
{
  AttribDataGunAimSetup* result = (AttribDataGunAimSetup*)target;

  NMP::endianSwap(result->m_worldUpAxis);
  NMP::endianSwap(result->m_keepUpright);
  NMP::endianSwap(result->m_worldSpaceTarget);
  NMP::endianSwap(result->m_applyJointLimits);

  AttribData::dislocate(target);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataGunAimSetup::getMemoryRequirements()
{
  return NMP::Memory::Format(NMP::Memory::align(sizeof(AttribDataGunAimSetup), MR_ATTRIB_DATA_ALIGNMENT), MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataGunAimSetup* AttribDataGunAimSetup::init(
  NMP::Memory::Resource& resource,
  uint16_t               refCount)
{
  NMP::Memory::Format format = getMemoryRequirements();
  resource.align(format);

  AttribDataGunAimSetup* result = (AttribDataGunAimSetup*) resource.ptr;
  resource.increment(format);

  result->setType(ATTRIB_TYPE_GUN_AIM_SETUP);
  result->setRefCount(refCount);

  result->m_worldUpAxis.set(NMP::Vector3YAxis());
  result->m_keepUpright = false;
  result->m_worldSpaceTarget = false;
  result->m_applyJointLimits = false;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataGunAimIKChain functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU 
void AttribDataGunAimIKChain::locate(AttribData* target)
{
  AttribDataGunAimIKChain* result = (AttribDataGunAimIKChain*)target;

  AttribData::locate(target);

  NMP::endianSwap(result->m_gunPivotOffset);
  NMP::endianSwap(result->m_gunBarrelOffset);
  NMP::endianSwap(result->m_gunPointingVector);
  NMP::endianSwap(result->m_secondaryArmHingeAxis);
  NMP::endianSwap(result->m_primaryArmHingeAxis);
  NMP::endianSwap(result->m_spineBias);
  NMP::endianSwap(result->m_updateTargetByDeltas);
  NMP::endianSwap(result->m_useSecondaryArm);

  NMP::endianSwap(result->m_gunJointIndex);
  NMP::endianSwap(result->m_gunBindJointIndex);
  NMP::endianSwap(result->m_spineRootJointIndex);
  NMP::endianSwap(result->m_secondaryWristJointIndex);
  NMP::endianSwap(result->m_primaryWristJointIndex);

  // Must locate PointIK params last because it needs the m_numAimJoints attribute
  NMP::endianSwap(result->m_numAimJoints);
  NMP::endianSwap(result->m_ikParams);
  REFIX_PTR_RELATIVE(NMRU::PointIK::Params, result->m_ikParams, result);
  NMRU::PointIK::Params::locate(result->m_ikParams, result->m_numAimJoints + 1);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataGunAimIKChain::dislocate(AttribData* target)
{
  AttribDataGunAimIKChain* result = (AttribDataGunAimIKChain*)target;

  // Must dislocate PointIK params first because it needs the m_numAimJoints attribute
  NMRU::PointIK::Params::dislocate(result->m_ikParams, result->m_numAimJoints + 2);
  UNFIX_PTR_RELATIVE(NMRU::PointIK::Params, result->m_ikParams, result);
  NMP::endianSwap(result->m_ikParams);
  NMP::endianSwap(result->m_numAimJoints);

  NMP::endianSwap(result->m_gunPivotOffset);
  NMP::endianSwap(result->m_gunBarrelOffset);
  NMP::endianSwap(result->m_gunPointingVector);
  NMP::endianSwap(result->m_secondaryArmHingeAxis);
  NMP::endianSwap(result->m_primaryArmHingeAxis);
  NMP::endianSwap(result->m_spineBias);
  NMP::endianSwap(result->m_updateTargetByDeltas);
  NMP::endianSwap(result->m_useSecondaryArm);

  NMP::endianSwap(result->m_gunJointIndex);
  NMP::endianSwap(result->m_gunBindJointIndex);
  NMP::endianSwap(result->m_spineRootJointIndex);
  NMP::endianSwap(result->m_secondaryWristJointIndex);
  NMP::endianSwap(result->m_primaryWristJointIndex);

  AttribData::dislocate(target);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AttribDataGunAimIKChain::relocate(AttribData* target, void* location)
{
  AttribDataGunAimIKChain* result = (AttribDataGunAimIKChain*)target;
  NMP_ASSERT(NMP_IS_ALIGNED(target, MR_ATTRIB_DATA_ALIGNMENT));
  NMP_ASSERT(NMP_IS_ALIGNED(location, MR_ATTRIB_DATA_ALIGNMENT));

  NMP::Memory::Format memReqs = getMemoryRequirements(result->m_numAimJoints);
  NMP::Memory::Resource resultRes = {result, memReqs};
  NMP::Memory::Resource locationRes = {location, memReqs};

  // AttribDataGunAimIKChain class.
  resultRes.increment(sizeof(AttribDataGunAimIKChain));
  locationRes.increment(sizeof(AttribDataGunAimIKChain));

  // IK params
  uint32_t numIKJoints = result->m_numAimJoints + 2;
  memReqs = NMRU::PointIK::Params::getMemoryRequirements(numIKJoints);
  NMRU::PointIK::Params* ikParams = (NMRU::PointIK::Params*)resultRes.alignAndIncrement(memReqs);
  result->m_ikParams = (NMRU::PointIK::Params*)locationRes.alignAndIncrement(memReqs);
  NMRU::PointIK::Params::relocate(ikParams, result->m_ikParams, numIKJoints);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataGunAimIKChain::getMemoryRequirements(uint32_t numAimJoints)
{
  // Shell
  NMP::Memory::Format result(sizeof(AttribDataGunAimIKChain), MR_ATTRIB_DATA_ALIGNMENT);

  // IK params
  result += NMRU::PointIK::Params::getMemoryRequirements(numAimJoints + 2);

  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataGunAimIKChain* AttribDataGunAimIKChain::init(
  NMP::Memory::Resource& resource,
  uint32_t               numAimJoints,
  uint16_t               refCount)
{
  NMP::Memory::Format format = getMemoryRequirements(numAimJoints);
  resource.align(format);
  AttribDataGunAimIKChain* result = (AttribDataGunAimIKChain*) resource.ptr;
  NMP::Memory::Resource internalMemTrack = resource;
  internalMemTrack.format = format;
  resource.increment(format);

  // Shell
  format.set(sizeof(AttribDataGunAimIKChain), MR_ATTRIB_DATA_ALIGNMENT);
  internalMemTrack.align(format);
  //
  result->setType(ATTRIB_TYPE_GUN_AIM_IK_CHAIN);
  result->setRefCount(refCount);
  //
  result->m_numAimJoints = numAimJoints;
  //
  result->m_gunPivotOffset.setToZero();
  result->m_gunBarrelOffset.setToZero();
  result->m_gunPointingVector.setToZero();
  result->m_spineBias = 0.0f;
  result->m_updateTargetByDeltas = false;
  //
  result->m_gunJointIndex = (uint32_t) - 1;
  result->m_gunBindJointIndex = (uint32_t) - 1;
  result->m_spineRootJointIndex = (uint32_t) - 1;
  result->m_secondaryWristJointIndex = (uint32_t) - 1;
  result->m_primaryWristJointIndex = (uint32_t) - 1;
  //
  internalMemTrack.increment(format);

  // IK params
  // Note end effector and pivot are also 'joints' to the IK solver, hence numAimJoints + 2
  uint32_t numIKJoints = numAimJoints + 2;
  format = NMRU::PointIK::Params::getMemoryRequirements(numIKJoints);
  internalMemTrack.align(format);
  NMP::Memory::Resource res = internalMemTrack;
  res.format = format;
  result->m_ikParams = (NMRU::PointIK::Params*)NMRU::PointIK::Params::init(&res, numIKJoints);
  // Set defaults
  result->m_ikParams->init(numIKJoints);
  //
  internalMemTrack.increment(format);

  // Check we used up all the memory we think we should have
  NMP_ASSERT(internalMemTrack.format.size == 0);

  return result;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
