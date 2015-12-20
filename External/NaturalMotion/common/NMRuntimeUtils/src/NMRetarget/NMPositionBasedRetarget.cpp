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
#include "NMPlatform/NMProfiler.h"
#include "NMIK/NMIKCommon.h"
#include "NMRetarget/NMPositionBasedRetarget.h"
//----------------------------------------------------------------------------------------------------------------------

#define ISFINITE_QUAT(q) ((q).x * 0 == 0 && (q).y * 0 == 0 && (q).z * 0 == 0 && (q).w * 0 == 0)
#define ISFINITE_VECTOR3(v) ((v).x * 0 == 0 && (v).y * 0 == 0 && (v).z * 0 == 0)
#define ISNORMAL_QUAT(q) (NMP::nmfabs((q).magnitudeSquared() - 1.0f) < 1e-2f)

#define MINIMUM_ALLOWED_RETARGET_SCALE_FACTOR 1e-6f

#define DO_FACE_RIGHT_WAYx
#define DO_MATRIX_METHODx
#define ONLY_DO_PREROTATION_FOR_ROOT
#define ALLOW_JOINT_WEIGHT_EXTRAPOLATIONx
#define INTERMEDIATE_JOINT_MOTION
#define PREVENT_HINGE_REVERSE_ROTATIONx

namespace NMRU
{

//----------------------------------------------------------------------------------------------------------------------
void PositionBasedRetarget::Solver::copy(Solver* dest)
{
  NMP_ASSERT(dest->m_memoryRequirements == m_memoryRequirements);

  NMP::Memory::memcpy(dest, this, m_memoryRequirements.size);
  dest->relocate();
}

//----------------------------------------------------------------------------------------------------------------------
void PositionBasedRetarget::Solver::retarget(
  const SourceParams* sourceParams,
  const NMP::DataBuffer* sourceTransforms,
  const TargetParams* targetParams,
  NMP::DataBuffer* targetTransforms,
  const int32_t* targetHierarchy,
  const NMRU::GeomUtils::PosQuat& rootTransform,
  float scaleMultiplier /* = 1.0f */,
  bool ignoreInputJoints /* = false */,
  bool calculateOffsets /* = false */)
{
  NMIK_PROFILE_BEGIN("PositionBasedRetarget::Solver::retarget");

  // IMPLEMENTATION NOTE
  // 'j' will refer to the target joint index and 's' will refer to the source index.
  // In context, the word 'target' will usually be omitted and assumed - e.g. 'jointPosition' means
  //  'targetJointPosition'.

  NMP_ASSERT(sourceParams);
  NMP_ASSERT(sourceTransforms);
  NMP_ASSERT(targetParams);
  NMP_ASSERT(targetTransforms);
  NMP_ASSERT(targetHierarchy);
  NMP_ASSERT(sourceTransforms->getLength() == m_numSourceJoints);

  // Initialise
  uint32_t jointCount = targetTransforms->getLength();
  float retargetScale = targetParams->rigScale * scaleMultiplier;
  //
  // Cache the input transforms
  targetTransforms->copyTo(m_inputTransforms);

  NMIK_PROFILE_BEGIN("Pre-amble");

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // FLAG RETARGETED JOINTS
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  // Mark leaf joints in the worldspace buffer as unused so they are not filled, avoiding unnecessary calculations.
  // Do this by marking everything as unused and then marking anything that isn't a leaf joint.
  // Leaf joints are unmapped joints which have no mapped Kinect joint in their descendants.
  // From here on, worldTransforms' used flags is how we know which joints are determined by the retarget map.
  m_worldTransforms->getUsedFlags()->clearAll();
  for (uint32_t s = 0; s < m_numSourceJoints; ++s)
  {
    int32_t j = m_sourceToTargetMap[s];
    NMP_ASSERT(j < (signed)jointCount);
    if (j >= 0)
    {
      int32_t jj = j;
      do
      {
        if (m_worldTransforms->hasChannel(jj))
          break;
        m_worldTransforms->setChannelUsed(jj);
      }
      while ((jj = targetHierarchy[jj]) >= 0);
    }
  }

  NMIK_PROFILE_BEGIN("Apply Offsets and Accumulate");

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // APPLY OFFSET TRANSFORMS
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  // Apply offsets to the input target transforms, and accumulate to give the offset worldspace input
  // transforms - a modified rig that should fit the source rig.

  // Apply offsets, and accumulate transforms
  for (uint32_t j = 0; j < jointCount; ++j)
  {
    // Don't waste time filling in leaf joints, which aren't affected by the source data
    if (!m_worldTransforms->hasChannel(j))
      continue;

    const TargetJointParams& jointParams = targetParams->jointParams[j];

    // Because of the mixed use of GeomUtils::PosQuats and PosQuat DataBuffers, we pull everything
    // out into quats and vectors
    const NMP::Quat& offsetQuat = jointParams.offsetTransform.q;
    const NMP::Vector3& offsetPos = jointParams.offsetTransform.t;

    // Get the world transform of this joint's parent
    NMRU::GeomUtils::PosQuat parentWorldInputPoseTransform = rootTransform;
    int32_t parentJointIndex = targetHierarchy[j];
    NMP_ASSERT(parentJointIndex < (signed)jointCount);
    if (parentJointIndex >= 0)
    {
      parentWorldInputPoseTransform.t = *m_worldTransforms->getPosQuatChannelPos((unsigned)parentJointIndex);
      parentWorldInputPoseTransform.q = *m_worldTransforms->getPosQuatChannelQuat((unsigned)parentJointIndex);
    }

    // Get the offset local space transform for this joint
    NMRU::GeomUtils::PosQuat jointInputPose;
    // Independent post-rotate and translate
    jointInputPose.q = offsetQuat * *m_inputTransforms->getPosQuatChannelQuat(j);
    jointInputPose.t = *m_inputTransforms->getPosQuatChannelPos(j) + offsetPos;
    // MORPH-21312: Undefined behaviour if you don't provide valid channel data for channels affected by the mapping

    // Now put into worldspace
    NMRU::GeomUtils::multiplyPosQuat(jointInputPose, parentWorldInputPoseTransform);

    // And put into the worldspace buffer
    m_worldTransforms->setPosQuatChannelPos(j, jointInputPose.t);
    m_worldTransforms->setPosQuatChannelQuat(j, jointInputPose.q);
    NMP_ASSERT(ISFINITE_VECTOR3(jointInputPose.t));
    NMP_ASSERT(ISFINITE_QUAT(jointInputPose.q));
    NMP_ASSERT(ISNORMAL_QUAT(jointInputPose.q));
  }

  // Remove any joints unaffected by Kinect below the root-most mapped joints - we only marked them
  // off so they would be involved in the accumulation above
  for (uint32_t s = 0; s < m_numSourceJoints; ++s)
  {
    int32_t j = m_sourceToTargetMap[s];
    if (j >= 0 && m_mappedHierarchy[s] < 0) // If mapped but has no mapped parent
    {
      int32_t jj = j;
      while ((jj = targetHierarchy[jj]) >= 0)
      {
        m_worldTransforms->setChannelUnused(jj);
      }
    }
  }

  NMIK_PROFILE_END();

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // SCALE CALCULATION
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  if (retargetScale < MINIMUM_ALLOWED_RETARGET_SCALE_FACTOR)
  {
    // Average the scale change for each joint
    m_estimatedTargetRigScale = 0;
    float sumSquareSourceBoneLengths = 0;
    bool atLeastOneMappedJoint = false;
    for (uint32_t s = 0; s != m_numSourceJoints; ++s)
    {
      // Get the target joint mapped to this source joint
      int32_t j = m_sourceToTargetMap[s];

      if (j >= 0)
      {
        atLeastOneMappedJoint = true;

        // This joint is mapped, so get the parent joint world position
        const NMP::Vector3* parentSourcePosition = &rootTransform.t;
        const NMP::Vector3* parentTargetPosition = &rootTransform.t;
        //
        //  Get the parent joints in each rig if mapped, root if not mapped
        int32_t sourceParentJoint = m_mappedHierarchy[s];
        if (sourceParentJoint >= 0)
        {
          int32_t targetParentJoint = m_sourceToTargetMap[sourceParentJoint];
          NMP_ASSERT(targetParentJoint >= 0);

          parentSourcePosition = sourceTransforms->getChannelPos(sourceParentJoint);
          parentTargetPosition = m_worldTransforms->getPosQuatChannelPos(targetParentJoint);
        }

        // Get this joint's world position in each rig
        const NMP::Vector3* sourcePosition = sourceTransforms->getChannelPos(s);
        const NMP::Vector3* targetPosition = m_worldTransforms->getPosQuatChannelPos(j);

        // Add the influence of this bone length to the overall estimate
        float sourceBoneLengthSquared = (*sourcePosition - *parentSourcePosition).magnitudeSquared();
        float targetBoneLengthSquared = (*targetPosition - *parentTargetPosition).magnitudeSquared();
        sumSquareSourceBoneLengths += sourceBoneLengthSquared;
        m_estimatedTargetRigScale += targetBoneLengthSquared;
      }
    }

    // Average over all per-bone scale calculations
    if (atLeastOneMappedJoint)
    {
      m_estimatedTargetRigScale = NMP::fastSqrt(m_estimatedTargetRigScale / sumSquareSourceBoneLengths);
    }
    else
    {
      m_estimatedTargetRigScale = retargetScale;
    }

    // Set for future calculations
    retargetScale = NMP::maximum(MINIMUM_ALLOWED_RETARGET_SCALE_FACTOR, m_estimatedTargetRigScale);
  }
  NMP_ASSERT(retargetScale > 0);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // Clear all the target channel 'used' flags - we will use these to tell when we've filled the output
  // with valid data
  targetTransforms->getUsedFlags()->clearAll();

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // IMPLEMENTATION NOTE
  // Our target buffer will, after the first stage of retargeting, contain worldspace joint positions
  // and orientations for all joints affected by the mapping.  Then those joints will be converted back
  // to local space, and then the remaining joints (such as fingers etc) will be copied back from the
  // input buffer we cached earlier.
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  NMIK_PROFILE_END();
  NMIK_PROFILE_BEGIN("Retrieve Positions");

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // POSITIONS
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  // Initialise output transforms to zero, and joint confidences to 1
  for (uint32_t j = 0; j < jointCount; ++j)
  {
    // Ignore joints unaffected by Kinect
    if (!m_worldTransforms->hasChannel(j))
      continue;

    targetTransforms->getPosQuatChannelQuat(j)->setXYZW(0, 0, 0, 0);
    targetTransforms->getPosQuatChannelPos(j)->setToZero();
    m_targetRotationConfidence[j] = 1.0f;
    m_targetPositionConfidence[j] = 1.0f;
  }

  // Copy source positions to the target and rescale
  for (uint32_t s = 0; s != m_numSourceJoints; ++s)
  {
    // Get the target joint mapped to this source joint
    int32_t j = m_sourceToTargetMap[s];

    if (j >= 0)
    {
      // This joint is mapped, so...

      // Set the target position, and rescale with respect to the root
      const NMP::Vector3& sourcePos = *sourceTransforms->getChannelPos(s);
      NMP::Vector3* pos = targetTransforms->getPosQuatChannelPos(j);
      //   In 'show offsets' debug mode, we leave the world joint positions unchanged
      if (targetParams->debugMode == TargetParams::kRetargetDebuggingShowOffsets)
      {
        pos->set(*m_worldTransforms->getPosQuatChannelPos(j));
      }
      else
      {
        *pos = rootTransform.t + (sourcePos - rootTransform.t) * retargetScale;
      }
      // Set the w value to zero, which we use for helping to blend the influence of different child
      // bones on the translation (even though we're not usually expecting to be using that)
      pos->w = 0;

      NMP_ASSERT(ISFINITE_VECTOR3(*pos));
    }
  }

  NMIK_PROFILE_END();
  NMIK_PROFILE_BEGIN("Recover Rotations");

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // ROTATIONS
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  // Calculate retargeted world space transforms for the mapped joints, based on rotation of child bones
  // from the input pose

  // IMPLEMENTATION NOTES:
  // The input pose is always retrieved from the world buffer rather than the input buffer.  This is
  //  because the world buffer is in worldspace and has had offset transforms applied, and so this is
  //  generally the most efficient way of getting the required information.  Were the offsets the
  //  identity then the two would be the same in local space.
  // Rig hierarchies are assumed to be in valid root-first order, namely, all joints have a lower
  //  index than their descendants and higher than their ancestors.

  for (uint32_t s = 0; s < m_numSourceJoints; ++s)
  {
    // Get some indices, and references to transform data
    int32_t j = m_sourceToTargetMap[s];
    if (j < 0)
      continue;  // Not mapped
    //
    const TargetJointParams& jointParams = targetParams->jointParams[j];
    NMP::Vector3* jointPosition = targetTransforms->getPosQuatChannelPos(j);
    NMP::Quat* jointQuat = targetTransforms->getPosQuatChannelQuat(j);

    NMP::Vector3* jointInputPosePosition = m_worldTransforms->getPosQuatChannelPos(j);
    NMP::Quat* jointInputPoseQuat = m_worldTransforms->getPosQuatChannelQuat(j);

    // While we're here, include the confidence in this source joint's data in the calculated
    // confidences in the target joint's retargeted transform
    m_targetRotationConfidence[j] *= sourceParams->jointParams[s].sourceJointConfidence;
    m_targetPositionConfidence[j] *= sourceParams->jointParams[s].sourceJointConfidence;

    // Calculate or retrieve the mapped parent joint transform.   'Parent' always
    // refers to the mapped parent, i.e. the next joint up in the target hierarchy that
    // is mapped, not necessarily the direct parent.
    NMP::Quat newJointInputPoseQuat = *jointInputPoseQuat;
    NMP::Quat parentJointQuat(NMP::Quat::kIdentity);
    NMP::Quat parentJointInputPoseQuat(NMP::Quat::kIdentity);
    NMP::Vector3 parentJointInputPosePosition(0, 0, 0);
    //
    int32_t parentSourceJointIndex = m_mappedHierarchy[s];
    int32_t parentTargetJointIndex = -1;
    if (parentSourceJointIndex >= 0)
    {
      NMIK_PROFILE_BEGIN("Intermediate joints fixup chunk");

      parentTargetJointIndex = m_sourceToTargetMap[parentSourceJointIndex];
      NMP_ASSERT(parentTargetJointIndex >= 0); // Must be mapped if we're here
      NMP_ASSERT(parentTargetJointIndex < (signed)jointCount);

#ifdef NMP_ENABLE_ASSERTS
      NMP::Vector3* parentJointPosition = targetTransforms->getPosQuatChannelPos(parentTargetJointIndex);
      // Test will fail if source skeleton does not have a strict root-first hierarchy (see
      // implementation note above) - we use 'w' as one way of tracking when we've filled data
      // in for this joint
      NMP_ASSERT(parentJointPosition->w != 0);
#endif

      parentJointQuat = *targetTransforms->getPosQuatChannelQuat(parentTargetJointIndex);
      parentJointInputPoseQuat = *m_worldTransforms->getPosQuatChannelQuat(parentTargetJointIndex);
      parentJointInputPosePosition = *m_worldTransforms->getPosQuatChannelPos(parentTargetJointIndex);
      float parentSourceJointConfidence = sourceParams->jointParams[parentSourceJointIndex].sourceJointConfidence;

      NMP_ASSERT(ISFINITE_QUAT(parentJointQuat));

      // Average the influence of this joint on intermediates between it and its mapped parent, and convert
      // them back to worldspace
      int32_t intJointIndex = j;
      while ((intJointIndex = targetHierarchy[intJointIndex]) != parentTargetJointIndex)
      {
        // Include this mapped child's confidence in the confidence for this intermediate joint.
        // Mapped joint's position confidence is entirely based on the confidence in the source data,
        // but for an intermediate joint it's based on the confidence in the spanning mapped joints, just
        // as for the rotation confidence.
        m_targetRotationConfidence[intJointIndex] *= sourceParams->jointParams[s].sourceJointConfidence;
        m_targetPositionConfidence[intJointIndex] *= sourceParams->jointParams[s].sourceJointConfidence;

        // Don't bother if this joint has already been visited.  This will happen because we may
        // have visited it while filling in a sibling joint.  We can then be sure we will have
        // dealt with this joint and all others up to the mapped parent.
        if (targetTransforms->hasChannel(intJointIndex))
          continue;

        // Include this mapped parent's confidence in the confidence for this intermediate joint.  We
        // only want to include this once, so we put it after the 'continue' above.
        m_targetRotationConfidence[intJointIndex] *= parentSourceJointConfidence;
        m_targetPositionConfidence[intJointIndex] *= parentSourceJointConfidence;

        NMP::Vector3* intJointPosition = targetTransforms->getPosQuatChannelPos((uint32_t)intJointIndex);
        NMP::Quat* intJointQuat = targetTransforms->getPosQuatChannelQuat((uint32_t)intJointIndex);

        // Sanity check - if child joint priorities were all zero, which will be indicated by a zero
        // quat value, make sure orientation is set to the input pose
        int32_t intJointParentIndex = targetHierarchy[intJointIndex];
        NMP_ASSERT(intJointParentIndex >= 0);
        NMP::Quat jointInputPoseLocalQuat =
          ~*m_worldTransforms->getPosQuatChannelQuat(intJointParentIndex) * *m_worldTransforms->getPosQuatChannelQuat(intJointIndex);
        if (intJointQuat->magnitudeSquared() < NMRU::GeomUtils::gTolSq)
        {
          *intJointQuat = jointInputPoseLocalQuat;
          intJointPosition->w = 1.0f;
        }

        // Position divides through by the value in its w element, which was keeping track of the
        // sum of bone lengths.  Position weights will be dealt with later as a post-process.
        NMP_ASSERT(intJointPosition->w != 0);
        *intJointPosition /= intJointPosition->w;
        intJointPosition->w = 1.0f;
        // Quats just normalise because they're homogeneous
        intJointQuat->normalise();
        NMP_ASSERT(ISFINITE_VECTOR3(*intJointPosition));
        NMP_ASSERT(ISFINITE_QUAT(*intJointQuat));

        // In pass-through mode, collapse all intermediate joints to zero so they don't interfere visually in the output
        if (targetParams->debugMode == TargetParams::kRetargetDebuggingPassThrough)
        {
          intJointPosition->setToZero();
          intJointPosition->w = 1.0f;
          intJointQuat->identity();
        }

#ifdef APPLY_WEIGHTS_DURING_CONVERSION_TO_LOCAL_SPACE
        // Apply joint weight to orientation
        float weight = 1.0f - targetParams->jointParams[intJointIndex].rotationWeight;
  #ifndef ALLOW_JOINT_WEIGHT_EXTRAPOLATION
        NMRU::GeomUtils::shortestArcFastSlerp(*intJointQuat, jointInputPoseLocalQuat, weight);
  #else
        NMRU::GeomUtils::interpOrExtrapSlerp(*intJointQuat, jointInputPoseLocalQuat, weight);
  #endif
        intJointQuat->normalise();
#endif

        // We don't set this channel as used, to flag that it is still in local space
      }

      // Convert to worldspace.  First find first intermediate
      intJointIndex = targetHierarchy[j];
      if (!targetTransforms->hasChannel(intJointIndex))
      {
        // Find the worldspace ancestor, which may well be the mapped parent
        int32_t referenceJointIndex = intJointIndex;
        do
        {
          referenceJointIndex = targetHierarchy[referenceJointIndex];
          NMP_ASSERT(referenceJointIndex >= 0);
        }
        while (!targetTransforms->hasChannel(referenceJointIndex));

        // Now convert this chain to worldspace
        fillTransforms(targetHierarchy, targetTransforms, intJointIndex, referenceJointIndex,
                       NMRU::GeomUtils::identityPosQuat());

        // Now set this chain as used, to indicate it's in worldspace
        do
        {
          targetTransforms->setChannelUsed(intJointIndex);
        }
        while ((intJointIndex = targetHierarchy[intJointIndex]) != referenceJointIndex);
      }

      NMIK_PROFILE_END();
    }

    // Get the transform of this joint's immediate parent
    NMP::Quat immediateParentJointQuat = parentJointQuat;
    NMP::Quat immediateParentJointInputPoseQuat = parentJointInputPoseQuat;
    if (parentSourceJointIndex >= 0)
    {
      int32_t immediateParentIndex = targetHierarchy[j];
      if (immediateParentIndex != parentTargetJointIndex)
      {
        immediateParentJointQuat = *targetTransforms->getPosQuatChannelQuat(immediateParentIndex);
        immediateParentJointInputPoseQuat = *m_worldTransforms->getPosQuatChannelQuat(immediateParentIndex);
      }
    }

    // Update input pose orientation of joint now that its parent has moved
    NMP::Quat inputPoseOffsetQ = immediateParentJointQuat * ~immediateParentJointInputPoseQuat;
    newJointInputPoseQuat = inputPoseOffsetQ * *jointInputPoseQuat;

    // If this joint is a hinge then get the hinge axis
    bool isHinge = jointParams.isHinge && jointParams.isLimited &&
                   targetParams->debugMode == TargetParams::kRetargetDebuggingOff;
    NMP::Vector3 hingeAxis;
    if (isHinge)
    {
      // This joint is a hinge
      // Find the hinge axis in world space
      NMP::Quat worldLimitQuat = newJointInputPoseQuat * jointParams.limits.offset;
      hingeAxis = worldLimitQuat.getXAxis();
      isHinge = true;

      // Check for uninitialised limits
      NMP_ASSERT(ISNORMAL_QUAT(jointParams.limits.offset));
      NMP_ASSERT(ISNORMAL_QUAT(jointParams.limits.frame));
    }

    // Find how many child joints this joint has
    int32_t numChildJoints = m_numMappedChildren[s];
    NMP_ASSERT(numChildJoints >= 0); // Can't be here if joint is unmapped

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // PRE-ROTATION
    NMP::Quat preRotate(NMP::Quat::kIdentity);

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // Face right way

#ifdef DO_FACE_RIGHT_WAY
    NMIK_PROFILE_BEGIN("Face right way");

  #ifdef ONLY_DO_PREROTATION_FOR_ROOT
    if (parentSourceJointIndex < 0 && numChildJoints == 3 && !isHinge)
  #else
    if (numChildJoints == 3 && !isHinge)
  #endif
    {
      NMP::Vector3 childPositions[3];
      NMP::Vector3 childDesiredPositions[3];
      for (uint32_t childCount = 0; (signed)childCount < numChildJoints; ++childCount)
      {
        int32_t childSourceJointIndex = m_reverseHierarchy[s][childCount];
        NMP_ASSERT(childSourceJointIndex >= 0);
        int32_t childJointIndex = m_sourceToTargetMap[childSourceJointIndex];
        NMP_ASSERT(childJointIndex >= 0 && childJointIndex < (signed)jointCount);

        NMP::Vector3* childJointPosition = targetTransforms->getPosQuatChannelPos(childJointIndex);
        NMP::Vector3* childJointInputPosePosition = m_worldTransforms->getPosQuatChannelPos(childJointIndex);

        // Get the source and target bone vectors.  We have to move the target pose based on the motion
        // of the parent joints we have visited so far.  This also takes the offset transforms into account.
        NMP::Vector3 childInputPoseOffset =
          jointInputPoseQuat->inverseRotateVector(*childJointInputPosePosition - *jointInputPosePosition);
        childPositions[childCount] = newJointInputPoseQuat.rotateVector(childInputPoseOffset);
        childDesiredPositions[childCount] = *childJointPosition - *jointPosition;
      }
      NMP::Vector3 facingDirection;
      facingDirection.cross(childPositions[1] - childPositions[0], childPositions[2] - childPositions[0]);
      NMP::Vector3 desiredFacingDirection;
      desiredFacingDirection.cross(
        childDesiredPositions[1] - childDesiredPositions[0],
        childDesiredPositions[2] - childDesiredPositions[0]);

      if (desiredFacingDirection.magnitudeSquared() > NMRU::GeomUtils::nTolSq &&
          facingDirection.magnitudeSquared() > NMRU::GeomUtils::nTolSq)
      {
        preRotate.forRotation(facingDirection, desiredFacingDirection);
      }
    }

    NMIK_PROFILE_END();
#endif

    // End of face-right-way method
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // Matrix method

#ifdef DO_MATRIX_METHOD
    NMIK_PROFILE_BEGIN("Matrix method");

    // Any joints with three children can be calculated via a matrix inverse.  This is much
    // better than bone rotation which gets confused.
    // In practice this just means the hips and the spine.
    // MORPH-21312: This needs to take priority into account and should be able to do this for joints
    // with two child bones as well; ought to do a least-squares fit for joints with any number of child bones.
  #ifdef ONLY_DO_PREROTATION_FOR_ROOT
    if (parentSourceJointIndex < 0 && numChildJoints == 3 && !isHinge)
  #else
    if (numChildJoints == 3 && !isHinge)
  #endif
    {
      // X will contain the three bones in the target's current (input) pose, and XR those bones
      // in the source pose; we will find the rotation that takes X to XR.
      NMP::Matrix34 X(NMP::Matrix34::kIdentity);
      NMP::Matrix34 XR(NMP::Matrix34::kIdentity);
      uint32_t i = 0;
      NMP_ASSERT(numChildJoints > 0);
      for (uint32_t childCount = 0; (signed)childCount < numChildJoints; ++childCount)
      {
        int32_t childSourceJointIndex = m_reverseHierarchy[s][childCount];
        NMP_ASSERT(childSourceJointIndex >= 0);
        int32_t childJointIndex = m_sourceToTargetMap[childSourceJointIndex];
        NMP_ASSERT(childJointIndex >= 0 && childJointIndex < (signed)jointCount);

        NMP::Vector3* childJointPosition = targetTransforms->getPosQuatChannelPos(childJointIndex);
        NMP::Vector3* childJointInputPosePosition = m_worldTransforms->getPosQuatChannelPos(childJointIndex);

        // Get the source and target bone vectors.  We have to move the target pose based on the motion
        // of the parent joints we have visited so far.  This also takes the offset transforms into account.
        NMP::Vector3 childInputPoseOffset =
          jointInputPoseQuat->inverseRotateVector(*childJointInputPosePosition - *jointInputPosePosition);
        NMP::Vector3 inputPoseDirection = newJointInputPoseQuat.rotateVector(childInputPoseOffset);
        NMP::Vector3 desiredDirection = *childJointPosition - *jointPosition;
        inputPoseDirection.normalise();

        // Avoid divide-by-zero (or normalise on zero-length vector)
        static const float xVectorLengthTol = 1e-6f;
        static const float xVectorLengthTolSq = xVectorLengthTol * xVectorLengthTol;
        if (desiredDirection.magnitudeSquared() > xVectorLengthTolSq)
          desiredDirection.normalise();
        else
          desiredDirection = inputPoseDirection;
        X.r[i] = inputPoseDirection;
        XR.r[i] = desiredDirection;
        ++i;
      }

      // Solution by construction of coordinate frames, cycling through different pairs of child bones so
      // no bones have more influence over the solution
      NMP::Matrix34 R1 = X;
      NMP::Matrix34 R2 = XR;
      R1.orthonormalise();
      R2.orthonormalise();
      R1.transpose3x3();
      NMP::Matrix34 R;
      R.multiply3x3(R1, R2);
      NMP::Quat newQ = R.toQuat();

      // Cycle
      R1.r[0] = X.r[1];
      R1.r[1] = X.r[2];
      R1.r[2] = X.r[0];
      R2.r[0] = XR.r[1];
      R2.r[1] = XR.r[2];
      R2.r[2] = XR.r[0];
      R1.orthonormalise();
      R2.orthonormalise();
      R1.transpose3x3();
      R.multiply3x3(R1, R2);
      NMP::Quat q = R.toQuat();
      if (q.dot(newQ) < 0)
        q = -q;
      newQ += q;

      // Cycle
      R1.r[0] = X.r[2];
      R1.r[1] = X.r[0];
      R1.r[2] = X.r[1];
      R2.r[0] = XR.r[2];
      R2.r[1] = XR.r[0];
      R2.r[2] = XR.r[1];
      R1.orthonormalise();
      R2.orthonormalise();
      R1.transpose3x3();
      R.multiply3x3(R1, R2);
      q = R.toQuat();
      if (q.dot(newQ) < 0)
        q = -q;
      newQ += q;

      // Normalising averages the influence of all the approximations of the rotation
      newQ.normalise();

      preRotate = newQ; // Record for use later

    } // if ([parentSourceJointIndex < 0] && numChildJoints == 3 && !isHinge)

    NMIK_PROFILE_END();
#endif // DO_MATRIX_METHOD

    // End of matrix method
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    // Add pre-rotation to starting orientation of joint
    newJointInputPoseQuat = preRotate * newJointInputPoseQuat;
    NMP_ASSERT(ISFINITE_QUAT(newJointInputPoseQuat));

    // End of pre-rotation
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // Bone rotation method

    NMIK_PROFILE_BEGIN("Bone rotation");

    // Go through all the children of this joint
    for (uint32_t childCount = 0; (signed)childCount < numChildJoints; ++childCount)
    {
      NMP_ASSERT(numChildJoints > 0);
      int32_t childSourceJointIndex = m_reverseHierarchy[s][childCount];
      NMP_ASSERT(childSourceJointIndex >= 0);
      int32_t childJointIndex = m_sourceToTargetMap[childSourceJointIndex];
      NMP_ASSERT(childJointIndex >= 0 && childJointIndex < (signed)jointCount);

      NMP::Vector3* childJointPosition = targetTransforms->getPosQuatChannelPos(childJointIndex);

      NMP::Vector3* childJointInputPosePosition = m_worldTransforms->getPosQuatChannelPos(childJointIndex);

      // While we're here, include the confidence in this child joint in the calculated confidence factor
      // for the target joint.  This is rotation confidence only, because a joint's position confidence
      // is derived directly from the confidence of the corresponding source data.
      m_targetRotationConfidence[j] *= sourceParams->jointParams[childSourceJointIndex].sourceJointConfidence;

      /////////////////////////////////////////////////////////////////////////////////////////////////////
      // Intermediate joints, 'rotation spreading'
      // Distribute the rotation between intermediate joints.  Each intermediate is rotated to provide
      // part of the desired rotation of the 'virtual bone' going between the two mapped joints.
      // Intermediate joint transforms are stored in local space for valid blending.

      // Count intermediates, get index array
      uint32_t numIntermediateJoints = 1;
      int32_t intJointIndex = childJointIndex;
      while ((intJointIndex = targetHierarchy[intJointIndex]) != (signed)j)
      {
        NMP_ASSERT(intJointIndex >= 0);
        ++numIntermediateJoints;
        NMP_ASSERT(numIntermediateJoints < 1000000); // Checks for crazy hierarchy or mapping (looping)
      }

      // Setup for intermediates
      //  Local offset from the target input pose caused by parent rotation and the pre-rotation from the matrix calc
      NMP::Quat offsetQ = newJointInputPoseQuat * ~*jointInputPoseQuat;
      //  Get the current bone vector
      NMP::Vector3 currentDirection = offsetQ.rotateVector(*childJointInputPosePosition - *jointInputPosePosition);
      //  Get the bone vector we are trying to get to
      NMP::Vector3 desiredDirection = *childJointPosition - *jointPosition;
      //  Track the world position of the mapped child joint, adjusted for motion of the chain so far
      NMP::Vector3 childJointInputPosePosTracked = *jointPosition + currentDirection;
      //  Get priority up front
      const float& sourceJointPriority = sourceParams->jointParams[childSourceJointIndex].sourceJointPriority;

#ifdef INTERMEDIATE_JOINT_MOTION

      NMIK_PROFILE_BEGIN("Intermediate joint motion");

      //  Get bias up front
      const float& sourceJointBias = sourceParams->jointParams[childSourceJointIndex].sourceJointBias;
      //  Rescale translations according to the way the limb has lengthened or shortened overall
      float scale = NMP::fastSqrt(
        GeomUtils::safelyDivide(desiredDirection.magnitudeSquared(), currentDirection.magnitudeSquared()));
      //  Keep track of proportion of rotation handed out so far
      float weightRemaining = 1.0f;
      //  Pre-inversions for efficiency
      float invNminus1 = 1.0f / numIntermediateJoints;
      float invN = 1.0f / (numIntermediateJoints + 1.0f);

      // Give each intermediate a portion of this rotation and a scaled position
      int32_t intParentJointIndex = targetHierarchy[childJointIndex];
      uint32_t numIntermediateJointsLeft = numIntermediateJoints;
      while (intParentJointIndex != j)
      {
        int32_t intJointIndex = intParentJointIndex;
        intParentJointIndex = targetHierarchy[intJointIndex];
        NMP_ASSERT(intJointIndex >= 0);
        // All unmapped intermediates have a parent, that's part of the definition
        NMP_ASSERT(intParentJointIndex >= 0);

        NMP::Vector3* intJointPosition = targetTransforms->getPosQuatChannelPos((uint32_t)intJointIndex);
        NMP::Quat* intJointQuat = targetTransforms->getPosQuatChannelQuat((uint32_t)intJointIndex);
        NMP::Vector3* intJointInputPosePosition = m_worldTransforms->getPosQuatChannelPos((uint32_t)intJointIndex);
        NMP::Quat* intJointInputPoseQuat = m_worldTransforms->getPosQuatChannelQuat((uint32_t)intJointIndex);
        NMP::Quat& intParentInputPoseQuat = *m_worldTransforms->getPosQuatChannelQuat((uint32_t)intParentJointIndex);
        NMP::Vector3& intParentInputPosePos = *m_worldTransforms->getPosQuatChannelPos((uint32_t)intParentJointIndex);

        // Get a local input pose for the joint including the offset transform and scaling
        NMP::Vector3 intJointLocalInputPosePos =
          intParentInputPoseQuat.inverseRotateVector(*intJointInputPosePosition - intParentInputPosePos);

        // Calculate new input pose of this intermediate following root motion
        NMP::Vector3 newIntJointPositionOffset = offsetQ.rotateVector(*intJointInputPosePosition - *jointInputPosePosition);
        NMP::Vector3 newIntJointInputPosePos = *jointPosition + newIntJointPositionOffset;

        // Calculate new scaled position of this intermediate and set in the output
        // Intermediate joint transforms are stored in local space for valid blending
        NMP::Vector3 p = intJointLocalInputPosePos * scale;
        p.w = 1.0f;
        float currentWVal = intJointPosition->w;
        *intJointPosition += p * sourceJointPriority;
        intJointPosition->w = currentWVal + sourceJointPriority;
        NMP_ASSERT(ISFINITE_VECTOR3(*intJointPosition));

        // Vector from the root joint to this intermediate joint
        NMP::Vector3 jointToIntJointVector = newIntJointInputPosePos - *jointPosition;
        // Vector from this intermediate joint to the mapped child following motion so far
        NMP::Vector3 intJointToChildVector = childJointInputPosePosTracked - newIntJointInputPosePos;
        // And the vector between the mapped joints
        NMP::Vector3 intDirection = intJointToChildVector + jointToIntJointVector;

        // Get the weight for this joint
        // x is the floating point relative position of this joint on the joint chain.
        float x = (numIntermediateJointsLeft - 1) * invNminus1;
        // adjustmentForNaturalBias approximately corrects for how rotation of a joint increases as you get further
        // up the chain because it has a weaker effect on the orientation of the virtual bone.
        float adjustmentForNaturalBias = 2.0f - (2.0f * x);
        // The bias weight is a linearly varying value with a slope dependent on the user bias parameter.  The
        // equation is designed to ensure the overall weights sum to 1 (after division by N).
        float bias = (3.0f * sourceJointBias * x) + 1.0f - sourceJointBias;
        float weight = adjustmentForNaturalBias * bias;
        // The previous weight values sum to N, so divide by N (where N is the number of joints in the chain
        // including the mapped joints)
        weight *= invN;
        weightRemaining -= weight;
        // Because we are moving the target bone vector incrementally, what we really want to know is how
        // far to move it from its last position, hence division by the remaining weight.
        float w = weight / weightRemaining;

        // Find the desired bone vector as a weighted blend of the start and final.  This is the means by
        // which we control how much work each joint does.
        NMP::Vector3 intDesiredDirection = vectorLerpDirection(intDirection, desiredDirection, w);
        intDesiredDirection.w = 0; // Just in case, this must not interfere with the dot product later

        // The length of the virtual bone from the intermediate joint to the child
        float intJointToChildDistanceSqr = intJointToChildVector.magnitudeSquared();

        // Solve for the rotation of this joint that would rotate the equivalent virtual bone between
        // the mapped parent and child to the desired orientation.
        //
        //  q is the result, a worldspace rotation of this joint.
        NMP::Quat q(NMP::Quat::kIdentity);
        //
        //  Avoid weird collapsing-in-on-itself situation where intermediate and child are coincident
        if (intJointToChildDistanceSqr > NMRU::GeomUtils::nTol)
        {
          float intJointToChildDistance = NMP::fastSqrt(intJointToChildDistanceSqr);
          //
          //  Quadratic solve for the new location of the mapped child after rotation of the intermediate
          float A = intDesiredDirection.magnitudeSquared();
          float B = -2.0f * jointToIntJointVector.dot(intDesiredDirection);
          float C = jointToIntJointVector.magnitudeSquared() - intJointToChildDistanceSqr;
          //  Solve quadratic, clamping to furthest reach if unsolvable
          //   This is a copy of GeomUtils::solveQuadratic() except it doesn't assert if there are no roots
          float coeff1sqr = B * B - 4.0f * A * C;
          coeff1sqr = NMP::maximum(coeff1sqr, 0.0f); // Clamp
          float coeff1 = NMP::fastSqrt(coeff1sqr);
          float signOfB = NMP::floatSelect(B, 1.0f, -1.0f);
          float t = -0.5f * (B + signOfB * coeff1);
          float tOverA = t / A;
          float cOverT = C / t;
          float lambda1 = NMP::floatSelect(NMP::nmfabs(A) - NMRU::GeomUtils::nTol, tOverA, cOverT);
          float lambda2 = NMP::floatSelect(NMP::nmfabs(t) - NMRU::GeomUtils::nTol, cOverT, tOverA);
          //
          //  Two possible new positions of the child
          NMP::Vector3 position1 = *jointPosition + intDesiredDirection * lambda1;
          NMP::Vector3 position2 = *jointPosition + intDesiredDirection * lambda2;
          NMP::Vector3 newIntJointToChildVector1 = position1 - newIntJointInputPosePos;
          NMP::Vector3 newIntJointToChildVector2 = position2 - newIntJointInputPosePos;
          //
          //  Reset the new vector's length in case we clamped
          //   A zero-length result is theoretically impossible; so if it happens, I'd like to know how!
          NMP_ASSERT(newIntJointToChildVector1.magnitudeSquared() > NMRU::GeomUtils::nTolSq);
          NMP_ASSERT(newIntJointToChildVector2.magnitudeSquared() > NMRU::GeomUtils::nTolSq);
          newIntJointToChildVector1.normalise();
          newIntJointToChildVector1 *= intJointToChildDistance;
          newIntJointToChildVector2.normalise();
          newIntJointToChildVector2 *= intJointToChildDistance;
          //
          //  Get the rotation that takes us to the calculated position, choosing the one which results in the
          //  least motion (where the new and old vectors are closest).
          //   Update the child position, but based on its actual location not the position1/position2
          //   calculation above, because we may have clamped (i.e. the desired orientation may have been
          //   unreachable).  That way, the remaining joints will successfully account for the failure of
          //   this joint.
          //  MORPH-21312: Account for hinges
          if ((intJointToChildVector - newIntJointToChildVector1).magnitudeSquared() <
              (intJointToChildVector - newIntJointToChildVector2).magnitudeSquared())
          {
            q.forRotation(intJointToChildVector, newIntJointToChildVector1);
            childJointInputPosePosTracked = newIntJointInputPosePos + newIntJointToChildVector1;
          }
          else
          {
            q.forRotation(intJointToChildVector, newIntJointToChildVector2);
            childJointInputPosePosTracked = newIntJointInputPosePos + newIntJointToChildVector2;
          }
        }

        // THIS IS WHERE THE UNMAPPED INTERMEDIATE JOINT TRANSFORMS ARE SET
        // Set in the output, adjusting for priority weighting
        // Intermediate joint transforms are stored in local space for valid blending
        NMP::Quat worldRotation = q * offsetQ * *intJointInputPoseQuat;
        NMP::Quat localRotation = ~(offsetQ * intParentInputPoseQuat) * worldRotation;
        NMP_ASSERT(ISFINITE_QUAT(localRotation));
        if (localRotation.dot(*intJointQuat) < 0)
          localRotation = -localRotation;
        *intJointQuat += localRotation * sourceJointPriority;

        --numIntermediateJointsLeft;
      }

      NMIK_PROFILE_END();

#endif // INTERMEDIATE_JOINT_MOTION

      // End of 'rotation spreading'
      /////////////////////////////////////////////////////////////////////////////////////////////////////

      /////////////////////////////////////////////////////////////////////////////////////////////////////
      // Rotation for the joint itself
      // The equation for the intermediates collapses to a simpler one for the joint itself, so we
      // do it here rather than inside the loop.  Plus we can ensure that any remaining offset between
      // the current and desired bone orientation is cleared up explicitly.  And it makes it possible
      // to skip any intermediate joint motion entirely if that is what is wanted.

      NMIK_PROFILE_BEGIN("Rotate to direction");

      // Get the rotation that takes the target bone to the source orientation
      //
      //  Get the bone vector after rotation of the parent, taking offset transforms into account
      currentDirection = childJointInputPosePosTracked - *jointPosition;
      NMP::Quat rotateToDesiredDirection(NMP::Quat::kIdentity);
      //
      //  Note: If this joint is a hinge then I'm only allowed to rotate around that axis.  The calculation
      //  for the parent, which I should have already done, is supposed to have taken account of this and
      //  ensured that the hinge axis is perpendicular to the plane of motion of the child bone
      if (desiredDirection.magnitudeSquared() > NMRU::GeomUtils::nTolSq &&
          currentDirection.magnitudeSquared() > NMRU::GeomUtils::nTolSq)
      {
        if (!isHinge)
        {
          rotateToDesiredDirection.forRotation(currentDirection, desiredDirection);
        }
        else
        {
          rotateToDesiredDirection =
            NMRU::GeomUtils::forRotationAroundAxis(currentDirection, desiredDirection, hingeAxis);
        }
      }

      // Get new joint orientation
      NMP::Quat newJointQuat = rotateToDesiredDirection * newJointInputPoseQuat;

      NMIK_PROFILE_END();

      /////////////////////////////////////////////////////////////////////////////////////////////////////
      // Deal with hinge-jointed children by applying additional twist around the bone

      NMIK_PROFILE_BEGIN("Twist for hinge children");

      // If this child joint is a hinge then I need to calculate a rotation that not only achieves
      // the direction to the child, but gets the child's hinge axis in the right orientation too,
      // to the extent that it is possible.  So in that case we calculate a twist rotation around
      // the child bone to achieve that.
      //   If there is no length in the child bone, there is no axis to twist around, so don't
      //   do anything about child hinges.
      //   If this joint is a hinge, again we can't do anything for child hinges
      NMP::Vector3 childBoneDirection = desiredDirection;
      const TargetJointParams& childJointParams = targetParams->jointParams[childJointIndex];
      if (!isHinge && childBoneDirection.magnitudeSquared() > 1e-6f &&
          targetParams->debugMode == TargetParams::kRetargetDebuggingOff &&
          childJointParams.isLimited && childJointParams.isHinge)
      {
        childBoneDirection.normalise();

        // Child joint is a hinge, get its hinge axis (in the world), again working on the assumption
        // that everything is in the input pose
        NMP_ASSERT(ISNORMAL_QUAT(childJointParams.limits.frame));
        NMP::Quat worldLimitQuat = newJointQuat * childJointParams.limits.frame;
        NMP::Vector3 childHingeAxis = worldLimitQuat.getXAxis();

        // Now visit each grandchild and retrieve a suitable rotation axis
        NMP::Quat twistRotation(0, 0, 0, 0);
        int32_t numGrandchildJoints = m_numMappedChildren[childSourceJointIndex];
        for (uint32_t grandchildCount = 0; (signed)grandchildCount < numGrandchildJoints; ++grandchildCount)
        {
          int32_t grandchildSourceIndex = m_reverseHierarchy[childSourceJointIndex][grandchildCount];
          NMP_ASSERT(grandchildSourceIndex >= 0);
          int32_t grandchildJointIndex = m_sourceToTargetMap[grandchildSourceIndex]; ;
          NMP_ASSERT(grandchildJointIndex >= 0 && grandchildJointIndex < (signed)jointCount);

          NMP::Vector3* grandchildJointPosition = targetTransforms->getPosQuatChannelPos(grandchildJointIndex);
          NMP::Vector3* grandchildJointInputPosePosition = m_worldTransforms->getPosQuatChannelPos(grandchildJointIndex);

          NMP::Vector3 grandchildInputPoseOffset =
            jointInputPoseQuat->inverseRotateVector(*grandchildJointInputPosePosition - *childJointInputPosePosition);
          NMP::Vector3 grandchildInputPoseDirection = newJointQuat.rotateVector(grandchildInputPoseOffset);
          NMP::Vector3 grandchildDirection = *grandchildJointPosition - *childJointPosition;

          // This gives us the desired world axis of rotation to get this grandchild bone pointed
          // in the right direction
          // MORPH-21312: This isn't rig-independent, it assumes the axis is perpendicular to the two bones
          NMP::Vector3 requiredAxis;
          requiredAxis.cross(grandchildDirection, childBoneDirection);
          // Our first effort will try to twist the parent bone by as little as possible
          if (requiredAxis.dot(childHingeAxis) < 0)
            requiredAxis = -requiredAxis;

          // Calculate the necessary twist to get the axis pointing this way
          NMP::Quat thisChildTwistRotation =
            NMRU::GeomUtils::forRotationAroundAxis(childHingeAxis, requiredAxis, childBoneDirection);

          // Calculate what the new world joint orientation will be
          NMP::Quat expectedNewJointQuat = thisChildTwistRotation * newJointQuat;

#ifdef PREVENT_HINGE_REVERSE_ROTATION
          // But we know that hinge joints operate in one semi-arc only, so perhaps we want
          // the axis pointing the other way to ensure the rotation will stay within limits.
          // To do this we assume that the centre of the limits marks the middle of the
          // desired semi-arc of motion.  We find where the grandchild bone would be if this
          // child joint were at the centre of its limits, and reverse the axis if this is in
          // the opposite direction to the 'desired direction'.
          //   Get input pose from world buffer, which accounts for offsets.
          NMP::Quat* childJointInputPoseQuat = m_worldTransforms->getPosQuatChannelQuat(childJointIndex);
          NMP::Vector3 grandchildInputPoseBoneLocal =
            childJointInputPoseQuat->inverseRotateVector(*grandchildJointInputPosePosition - *childJointInputPosePosition);
          NMP::Quat newChildLimitCentreQuat =
            expectedNewJointQuat * childJointParams.limits.frame * ~childJointParams.limits.offset;
          NMP::Vector3 grandchildBoneAtLimitCentreWorld =
            newChildLimitCentreQuat.rotateVector(grandchildInputPoseBoneLocal);
          NMP::Vector3 centreOfAllowedSemiArc =
            grandchildBoneAtLimitCentreWorld - childBoneDirection * (childBoneDirection.dot(grandchildBoneAtLimitCentreWorld));
          // Set a tolerance to allow some backwards bending, to prevent repeated flipping around
          // a straight limb
          // At the moment the tolerance is zero because we are dealing with flipping entirely using conditioning
          static const float xHingeBackwardsBendTol = 0.0f;
          grandchildDirection.normalise();
          centreOfAllowedSemiArc.normalise();
          if (grandchildDirection.dot(centreOfAllowedSemiArc) < -xHingeBackwardsBendTol)
          {
            // OPTIMISE  Rather than doing this again in its entirety, we could retain internal values and
            // just do the final operation inside forRotationAroundAxis() again
            thisChildTwistRotation = NMRU::GeomUtils::forRotationAroundAxis(childHingeAxis, -requiredAxis, childBoneDirection);
          }
#endif // PREVENT_HINGE_REVERSE_ROTATION

          // Conditioning - reduce the amount of twist to prevent large amounts of flip
          //
          // Two conditioning terms - one to reduce rotation when limb is straight, second to
          // reject rotation that is too large on the assumption that it is bad input.
          NMP::Vector3 jointToGrandChild = *grandchildJointPosition - *jointPosition;
          jointToGrandChild.normalise();
          float condition = NMP::nmfabs(1.0f - jointToGrandChild.dot(childBoneDirection));
          // Threshold and rescale
          //  This threshold is effectively the sine of the angle at which straightness is considered irrelevant
          static const float xStraightnessConditionThreshold = 0.02f;
          condition = NMP::minimum(condition / xStraightnessConditionThreshold, 1.0f);
          //
          // Ramp all rotation between two magnitudes back down to zero.
          // NOTE That this second conditioning term is suitable for most legs and arms but will possibly
          // interfere on very flexible limbs (ones which can twist all the way round).  Could make this
          // term optional in the future.
          //  These thresholds are cosine (half-angle) terms, hence the lower is higher than the upper
          static const float xRejectRotationMagnitudeLower = 0.8f;
          static const float xRejectRotationMagnitudeUpper = 0.7f;
          float amountOfRotationRescaledBetweenThresholds =
            (NMP::nmfabs(thisChildTwistRotation.w) - xRejectRotationMagnitudeUpper) /
            (xRejectRotationMagnitudeLower - xRejectRotationMagnitudeUpper);
          condition *= NMP::clampValue(amountOfRotationRescaledBetweenThresholds, 0.0f, 1.0f);
          //
          // Apply conditioning
          NMRU::GeomUtils::shortestArcFastSlerp(thisChildTwistRotation, NMP::Quat(NMP::Quat::kIdentity), 1.0f - condition);

          if (twistRotation.dot(thisChildTwistRotation) >= 0)
            twistRotation += thisChildTwistRotation;
          else
            twistRotation -= thisChildTwistRotation;
        }
        if (numGrandchildJoints > 0)
        {
          twistRotation.normalise();
          rotateToDesiredDirection = twistRotation * rotateToDesiredDirection;
        }
      }

      NMIK_PROFILE_END();

      // Finished dealing with hinge-jointed children
      /////////////////////////////////////////////////////////////////////////////////////////////////////

      // THIS IS WHERE THE MAPPED JOINT TRANSFORMS ARE SET
      // Set the result in the output, blending the effect of each mapped child joint
      newJointQuat = rotateToDesiredDirection * newJointInputPoseQuat;
      if (newJointQuat.dot(*jointQuat) < 0)
        newJointQuat = -newJointQuat;
      *jointQuat += newJointQuat * sourceJointPriority;
      jointPosition->w += sourceJointPriority;

    } // End of loop through mapped child joints

    NMIK_PROFILE_END();

    // End of bone rotation method
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    // What remains is to clear up special conditions and average the influence of all the child joints
    // on this joint

    if (numChildJoints == 0)
    {
      // Give joints with no children the input pose
      *jointQuat = newJointInputPoseQuat;
      jointPosition->w += 1.0f;
    }

    // Sanity check - if child joint priorities were all zero, result should be the input pose orientation.
    // Zero priorities will show itself as a zero quat.
    newJointInputPoseQuat = inputPoseOffsetQ * *jointInputPoseQuat;
    if (jointQuat->magnitudeSquared() < NMRU::GeomUtils::gTolSq)
      *jointQuat = newJointInputPoseQuat;

    // Average the influence of all children on this joint
    jointQuat->normalise();
    jointPosition->w = 1.0f;
    NMP_ASSERT(ISFINITE_QUAT(*jointQuat));
    NMP_ASSERT(ISNORMAL_QUAT(*jointQuat));

    // Apply joint weight
#ifndef APPLY_WEIGHTS_DURING_CONVERSION_TO_LOCAL_SPACE
    float rotationWeight = 1.0f - jointParams.rotationWeight;
  #ifndef ALLOW_JOINT_WEIGHT_EXTRAPOLATION
    NMRU::GeomUtils::shortestArcFastSlerp(*jointQuat, newJointInputPoseQuat, rotationWeight);
  #else
    NMRU::GeomUtils::interpOrExtrapSlerp(*jointQuat, newJointInputPoseQuat, rotationWeight);
  #endif
    jointQuat->normalise();
#endif

    // Mark this channel as filled with a finished-product worldspace transform
    targetTransforms->setChannelUsed(j);
  }

  NMIK_PROFILE_END();

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // CONVERT TO LOCAL SPACE and UNDO OFFSET TRANSFORMS and APPLY WEIGHTS
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  NMIK_PROFILE_BEGIN("Undo offsets and localise");

  // When applying weights during the un-accumulation, we need to work from the root upwards because
  // we need the weights to have been applied to the parent before un-accumulating the child.  To do
  // this we store the weighted worldspace geometry in m_worldBuffer, which is now available for use
  // for that purpose.
  for (uint32_t j = 0; j < jointCount; ++j)
  {
    if (targetTransforms->hasChannel(j))
    {
      int32_t parentTargetJointIndex = targetHierarchy[j];

      // get the world space position and rotation of the current joint
      NMP::Vector3* position = targetTransforms->getPosQuatChannelPos(j);
      NMP::Quat* rotation = targetTransforms->getPosQuatChannelQuat(j);
      NMP_ASSERT(rotation->magnitudeSquared() > 0);

      // Get this joint's offset transform
      const NMP::Quat& offsetQuat = targetParams->jointParams[j].offsetTransform.q;
      const NMP::Vector3& offsetPos = targetParams->jointParams[j].offsetTransform.t;

      // Get the parent worldspace offset TM
      NMP::Quat parentRotation = rootTransform.q;
      NMP::Vector3 parentPosition = rootTransform.t;
      //
      // If it has a valid parent transform it, otherwise use the root transform
      if (parentTargetJointIndex >= 0)
      {
        // All channels involved must either be used, or be ancestors of the root-most mapped
        // joints which must contain valid transforms at the input, even if they are unused.
        // There is no guaranteed way to check for valid transforms - this is the job of the caller.
        NMP_ASSERT(targetTransforms->hasChannel(parentTargetJointIndex) ||
                   // Checks if this channel is a root-most mapped joint, which is allowed to have an unused parent
                   (targetParams->targetToSourceMap[j] >= 0 && m_mappedHierarchy[targetParams->targetToSourceMap[j]] < 0));

        // Get the parent world space position and rotation - from the world buffer which we are
        // using to retain the accumulated transforms.
        // Note: if this joint is a root joint then its parent has not been visited by this loop,
        // but the world buffer still contains the correct transform because we filled it with the
        // offset input pose at the top of this function.
        parentRotation = *m_worldTransforms->getPosQuatChannelQuat(parentTargetJointIndex);
        parentPosition = *m_worldTransforms->getPosQuatChannelPos(parentTargetJointIndex);
        NMP_ASSERT(parentRotation.magnitudeSquared() > 0);
      }

      // Apply joint weights
      float positionWeight = 1.0f - (targetParams->jointParams[j].positionWeight * m_targetPositionConfidence[j]);
      float rotationWeight = 1.0f - (targetParams->jointParams[j].rotationWeight * m_targetRotationConfidence[j]);
      bool doWeighting =
        (NMP::nmfabs(positionWeight) > NMRU::GeomUtils::nTol ||
         NMP::nmfabs(rotationWeight) > NMRU::GeomUtils::nTol) &&
        targetParams->debugMode == TargetParams::kRetargetDebuggingOff;
      if (doWeighting)
      {
        // Get the start pose for weighting, which is the offset input pose
        NMP::Vector3 jointStartPosePosition = *m_inputTransforms->getPosQuatChannelPos(j);
        NMP::Quat jointStartPoseQuat = *m_inputTransforms->getPosQuatChannelQuat(j);
        // Offset
        jointStartPosePosition += offsetPos;
        jointStartPoseQuat = offsetQuat * jointStartPoseQuat;
        // Put start pose into worldspace
        jointStartPoseQuat.multiply(parentRotation, jointStartPoseQuat);
        jointStartPosePosition = parentRotation.rotateVector(jointStartPosePosition) + parentPosition;

        // Interpolate position and orientation
        position->lerp(jointStartPosePosition, positionWeight);
#ifndef ALLOW_JOINT_WEIGHT_EXTRAPOLATION
        NMRU::GeomUtils::shortestArcFastSlerp(*rotation, jointStartPoseQuat, rotationWeight);
#else
        NMRU::GeomUtils::interpOrExtrapSlerp(*rotation, jointStartPoseQuat, rotationWeight);
#endif
      }

      // Store the new worldspace transform for this joint in the world buffer (which is free to be used
      // for that purpose now), for use in conversion of child joints.
      m_worldTransforms->setPosQuatChannelPos(j, *position);
      m_worldTransforms->setPosQuatChannelQuat(j, *rotation);

      // Multiply by inverse parent transform to put into local space
      rotation->multiply(~parentRotation, *rotation);
      position->set(parentRotation.inverseRotateVector(*position - parentPosition));

      // Undo offset transform
      if (targetParams->debugMode == TargetParams::kRetargetDebuggingOff && !calculateOffsets)
      {
        *rotation = ~offsetQuat * *rotation;
        *position -= offsetPos;
      }

      // Normalise for safe output
      rotation->normalise();
      NMP_ASSERT(ISNORMAL_QUAT(*rotation));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // Used joints not affected by Kinect.  They already have the input pose because they've been left
    // unchanged.  Mark them as used, and in the debug modes do additional manipulation.
    else if (!ignoreInputJoints && m_inputTransforms->hasChannel(j))
    {
      targetTransforms->setChannelUsed(j);

      if (targetParams->debugMode != TargetParams::kRetargetDebuggingOff || calculateOffsets)
      {
        NMP::Vector3* position = targetTransforms->getPosQuatChannelPos(j);
        NMP::Quat* rotation = targetTransforms->getPosQuatChannelQuat(j);
        NMP::Vector3* jointInputPosePosition = m_inputTransforms->getPosQuatChannelPos(j);
        NMP::Quat* jointInputPoseQuat = m_inputTransforms->getPosQuatChannelQuat(j);

        position->set(*jointInputPosePosition);
        rotation->set(*jointInputPoseQuat);

        // When calculating offsets we actually need to /apply/ the offsets to these
        // joints, since you can legitimately have offsets on these other joints that aren't
        // affected by Kinect (such as the trajectory joint)
        if (targetParams->debugMode == TargetParams::kRetargetDebuggingShowOffsets || calculateOffsets)
        {
          const NMP::Quat& offsetQuat = targetParams->jointParams[j].offsetTransform.q;
          const NMP::Vector3& offsetPos = targetParams->jointParams[j].offsetTransform.t;
          *position += offsetPos;
          *rotation = offsetQuat * *rotation;
        }

        // When pass-through debugging, collapse all leaf joints down to nothing so they do not
        // visually appear to exist
        if (targetParams->debugMode == TargetParams::kRetargetDebuggingPassThrough)
        {
          // To be sure this is a leaf joint, we check it has a mapped parent.  If not then it is
          // a joint below the root-most mapped joints in the hierarchy, and we should leave it
          // alone.
          // We can do this slow code because this only happens in a debug mode.
          int32_t jj = j;
          while ((jj = targetHierarchy[jj]) >= 0)
          {
            if (targetParams->targetToSourceMap[jj] >= 0)
            {
              // Has a mapped parent
              position->setToZero();
              rotation->identity();
              break;
            }
          }
        }

        NMP_ASSERT(ISNORMAL_QUAT(*rotation));
      } // if (targetParams->debugMode != TargetParams::kRetargetDebuggingOff || calculateOffsets)
    }  // if (targetTransforms->hasChannel(j))
  }  // for loop through all joints

  // Make sure we've set all the channels to used that we should have.  That is all joints that are
  // affected by mapped source joints (marked as used in the worldspace buffer) and all joints that
  // are passed in from the input.
#ifdef NM_DEBUG
  for (uint32_t j = 0; j < jointCount; ++j)
  {
    if (m_worldTransforms->hasChannel(j) || (m_inputTransforms->hasChannel(j) && !ignoreInputJoints))
    {
      NMP_ASSERT(targetTransforms->hasChannel(j));
    }
    else if (!m_worldTransforms->hasChannel(j) && (!m_inputTransforms->hasChannel(j) || ignoreInputJoints))
    {
      NMP_ASSERT(!targetTransforms->hasChannel(j));
    }
  }
#endif

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // APPLY TRANSLATION WEIGHTS / REPLACE BIND POSE TRANSLATIONS
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  for (uint32_t j = 0; j < jointCount && targetParams->debugMode == TargetParams::kRetargetDebuggingOff; ++j)
  {
    if (m_worldTransforms->hasChannel(j)) // Only do this for joints which have been retargeted
    {
      NMP::Vector3& jointPos = *targetTransforms->getPosQuatChannelPos(j);
      const TargetJointParams& jointParams = targetParams->jointParams[j];

      // Set to input/bind pose translation
      if (jointParams.isRigid)
      {
        // MORPH-21312: Input transforms not having a valid channel for these joints may be considered an error...
        jointPos = *m_inputTransforms->getPosQuatChannelPos(j);
      }

      NMP_ASSERT(ISNORMAL_QUAT(*targetTransforms->getPosQuatChannelQuat(j)));
    }
  }

  NMIK_PROFILE_END();
  NMIK_PROFILE_END();

} // end of PositionBasedRetarget::Solver::retarget()

//----------------------------------------------------------------------------------------------------------------------
bool PositionBasedRetarget::Solver::prepare(
  const int32_t* targetToSourceMap,
  uint32_t numTargetJoints,
  const int32_t* targetHierarchy)
{
  // Clear the mappings
  for (uint32_t s = 0; s < m_numSourceJoints; ++s)
  {
    m_sourceToTargetMap[s] = -1;
    m_mappedHierarchy[s] = -1;
    m_reverseHierarchy[s] = NULL;
    m_numMappedChildren[s] = 0;
  }

  // Re-generate source-to-target map
  // Also Verify mapping
  // The only way the mapping can be broken is if the target rig is not in strict root-first order
  // (parent joints always have lower channel indices than their children), or if the mapping results
  // in a source rig not in strict root-first order (e.g. if target joint 1 is parented to joint 0,
  // and source joint 1 is parent to source joint 0, but you map target 0 to source 1 and target 1
  // to source 0).  Such mappings will be ignored by the system but as a result there will be
  // an incompatibility between your targetToSourceMap and the generated m_sourceToTargetMap.
  bool validMapping = true;
  for (uint32_t j = 0; j < numTargetJoints; ++j)
  {
    int32_t s = targetToSourceMap[j];
    if (s >= 0)
    {
      NMP_ASSERT(s < (signed)m_numSourceJoints);

      // Store the inverse mapping
      m_sourceToTargetMap[s] = j;

      // Find the mapped parent
      int32_t p = j;
      int32_t ps = s;
      while ((p = targetHierarchy[p]) >= 0)
      {
        ps = targetToSourceMap[p];
        if (ps >= 0)
        {
          // Set in the mapped hierarchy
          if (ps < s)
          {
            m_mappedHierarchy[s] = ps;
          }
          else
          {
            validMapping = false;
          }
          break;
        }
      }
    }
  }

  // Re-generate reverse hierarchy
  int32_t* reverseHierarchyDataPointer = m_reverseHierarchyData;
  for (uint32_t s = 0; s < m_numSourceJoints; ++s)
  {
    int32_t* startOfThisBlock = reverseHierarchyDataPointer;
    for (uint32_t childSourceIndex = 0; childSourceIndex < m_numSourceJoints; ++childSourceIndex)
    {
      if (m_mappedHierarchy[childSourceIndex] == (signed)s)
      {
        NMP_ASSERT((reverseHierarchyDataPointer - m_reverseHierarchyData) < ((signed)m_numSourceJoints - 1));
        ++(m_numMappedChildren[s]);
        *reverseHierarchyDataPointer = childSourceIndex;
        ++reverseHierarchyDataPointer;
      }
    }
    if (m_numMappedChildren[s] > 0)
      m_reverseHierarchy[s] = startOfThisBlock;
  }

  // For neatness, unmapped joints should show an invalid number of children rather than zero
  for (uint32_t s = 0; s < m_numSourceJoints; ++s)
  {
    if (m_sourceToTargetMap[s] < 0)
    {
      m_numMappedChildren[s] = -1;
    }
  }

  return validMapping;

}  // end of PositionBasedRetarget::Solver::prepare()

//----------------------------------------------------------------------------------------------------------------------
bool PositionBasedRetarget::Solver::validateMapping(
  const int32_t* targetToSourceMap,
  uint32_t numTargetJoints,
  const int32_t* targetHierarchy)
{
  for (uint32_t j = 0; j < numTargetJoints; ++j)
  {
    // A valid parent must have a lower index than its children
    if (targetHierarchy[j] >= (signed)j)
    {
      return false;
    }

    int32_t s = targetToSourceMap[j];
    if (s >= 0)
    {
      // Find the mapped parent
      int32_t p = j;
      int32_t ps = s;
      while ((p = targetHierarchy[p]) >= 0)
      {
        ps = targetToSourceMap[p];
        if (ps >= 0)
        {
          // A valid parent must have a lower index than its children
          if (ps >= s)
          {
            return false;
          }
          break;
        }
      }
    }
  }

  return true;

}  // end of PositionBasedRetarget::Solver::validateMapping()

//----------------------------------------------------------------------------------------------------------------------
void PositionBasedRetarget::Solver::fillTransforms(
  const int32_t* hierarchy,
  const NMP::DataBuffer* transformBuffer,
  uint32_t jointIndex,
  int32_t referenceJointIndex,
  const NMRU::GeomUtils::PosQuat& baseTransform)
{
  NMP::Vector3 parentPos = baseTransform.t;
  NMP::Quat parentQuat = baseTransform.q;

  // Get the parent transform, possibly by recursion
  int32_t parentJointIndex = hierarchy[jointIndex];
  if (parentJointIndex >= 0)
  {
    if (parentJointIndex != (signed) referenceJointIndex)
    {
      fillTransforms(hierarchy, transformBuffer, parentJointIndex, referenceJointIndex, baseTransform);
      parentPos = *transformBuffer->getPosQuatChannelPos(parentJointIndex);
      parentQuat = *transformBuffer->getPosQuatChannelQuat(parentJointIndex);
    }
    else
    {
      parentPos = baseTransform.q.rotateVector(*transformBuffer->getPosQuatChannelPos(parentJointIndex)) + baseTransform.t;
      parentPos.w = 1.0f;
      parentQuat = baseTransform.q * *transformBuffer->getPosQuatChannelQuat(parentJointIndex);
    }
  }

  // Compound with this joint to give this joint's accumulated transform
  NMP::Vector3* jointPos = transformBuffer->getPosQuatChannelPos(jointIndex);
  NMP::Quat* jointQuat = transformBuffer->getPosQuatChannelQuat(jointIndex);
  //
  // Lots of checks for dodgy data, can catch lots of bugs
  NMP_ASSERT(jointPos->w == 1.0f);
  NMP_ASSERT(parentPos.w == 1.0f);
  NMP_ASSERT(ISFINITE_QUAT(*jointQuat));
  NMP_ASSERT(ISNORMAL_QUAT(*jointQuat));
  NMP_ASSERT(ISNORMAL_QUAT(parentQuat));
  //
  *jointPos = parentQuat.rotateVector(*jointPos) + parentPos;
  jointPos->w = 1.0f; // Indicates filled, for our purposes
  *jointQuat = parentQuat * *jointQuat;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 PositionBasedRetarget::Solver::vectorLerpDirection(
  const NMP::Vector3& sourceVec0,
  const NMP::Vector3& sourceVec1,
  float alpha)
{
  float vec0Length;
  float vec1Length;
  float rescale;
  float interpolatedLength;
  NMP::Vector3 result;

  vec0Length = sourceVec0.magnitude();
  vec1Length = sourceVec1.magnitude();

  // Find required length of result.
  interpolatedLength = vec0Length + ((vec1Length - vec0Length) * alpha);

  // Find interpolated direction
  result.lerp(sourceVec0, sourceVec1, alpha);

  vec0Length = result.magnitude();
  if (vec0Length > NMRU::GeomUtils::nTol)
  {
    // Alter length of vector to match the required length
    rescale = interpolatedLength / vec0Length;
    result *= rescale;
  }

  return result;
}

} // end of namespace NMRU
