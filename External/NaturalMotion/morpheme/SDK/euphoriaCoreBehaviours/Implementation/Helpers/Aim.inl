#pragma once

/*
* Copyright (c) 2012 NaturalMotion Ltd. All rights reserved.
*
* Not to be copied, adapted, modified, used, distributed, sold,
* licensed or commercially exploited in any manner without the
* written consent of NaturalMotion.
*
* All non public elements of this software are the confidential
* information of NaturalMotion and may not be disclosed to any
* person nor used for any purpose not expressly approved by
* NaturalMotion in writing.
*
*/

#ifndef NM_AIM_INL
#define NM_AIM_INL

#include "NMGeomUtils/NMGeomUtils.h"
#include "euphoria/erDebugDraw.h"
#include "Types/AimInfo.h"
#include "Types/LimbControl.h"

#define SCALING_SOURCE pkg.owner->owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{
static const float ONE_OVER_2_PI = 1.0f / (2.0f * NM_PI);

//----------------------------------------------------------------------------------------------------------------------
// calculateAimTargetTM
// 
// Calculate end effector TM by applying difference in relative root TMs from pose to world to end rel root pose.
// 
inline NMP::Matrix34 calculateAimTargetTM(
  const NMP::Matrix34& rootTM,
  const NMP::Matrix34& poseEndRelativeToRoot,
  const NMP::Vector3& targetPositionInWorldSpace,
  const NMP::Vector3& poseAimDirRelRoot,
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  // Find direction to target rel current root.
  NMP::Vector3 targetDirRelRoot = targetPositionInWorldSpace;
  rootTM.inverseTransformVector(targetDirRelRoot);
  targetDirRelRoot -= poseEndRelativeToRoot.translation().getComponentOrthogonalToDir(poseAimDirRelRoot);
  targetDirRelRoot.normalise();

  // Find rotation from current to desired root.
  NMP::Quat rotationFromCurrentToDesiredRoot(NMP::Quat::kIdentity);
  rotationFromCurrentToDesiredRoot.forRotation(poseAimDirRelRoot, targetDirRelRoot);

  NMP::Matrix34 currentToDesiredRootTM(rotationFromCurrentToDesiredRoot);
  currentToDesiredRootTM.setTranslation(NMP::Vector3Zero());
  NMP::Matrix34 targetTM = poseEndRelativeToRoot * currentToDesiredRootTM * rootTM;

  MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, targetTM, 0.4f);

  return targetTM;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename PackageType, typename LimbStateType>
inline NMP::Matrix34 aimUpdate(
  PackageType& pkg,
  const LimbStateType& limbState,
  MR::InstanceDebugInterface* pDebugDrawInst)
{
  const float importance = pkg.in->getAimInfoImportance();
  NMP::Matrix34 endTargetTM(limbState.m_endTM);
  const NMP::Matrix34 currentRootTM = limbState.m_rootTM;

  if (importance > 0.0f)
  {
    float limbControlPositionWeight    = pkg.in->getAimInfo().limbControlPositionWeight;
    float limbControlOrientationWeight = pkg.in->getAimInfo().limbControlOrientationWeight;

    if ((pkg.in->getAimInfo().poseTwistWeight > 0.0f) || (pkg.in->getAimInfo().poseSwingWeight > 0.0f))
    {
      // Calculate the final world space target TM for the end effector from the pose and the target position.
      endTargetTM = calculateAimTargetTM(
        limbState.m_rootTM,
        pkg.in->getAimInfo().poseEndRelativeToRoot,
        pkg.in->getTargetPositionInWorldSpace(),
        pkg.in->getPoseAimDirRelRoot(),
        pDebugDrawInst);

      // Try to avoid the end effector overshooting its final world space target by modifying the target when the limb
      // root is rotating. When the limb root is rotating slowly the target will remain unchanged, when it is rotating
      // quickly the target will be fixed relative to the root and effectively lag behind the un-modified target.
      // 
      // The target is modified by transforming the target into local space relative to the root then transforming back
      // into world space using a new, effective root TM. The effective root TM is calculated by interpolating between
      // the target and current root TMs using the root's angular speed as a weight.

      NMP::Matrix34 targetRootTM(NMP::Matrix34::kIdentity);
      targetRootTM.interpolate(limbState.m_rootTM, pkg.in->getTargetRootTM(), pkg.in->getTargetRootTMImportance());

      // Transform the end effector target TM into the frame of the root target. 
      NMP::Matrix34 rootTargetInv = targetRootTM;
      rootTargetInv.invertFast();
      endTargetTM = endTargetTM * rootTargetInv;

      // Interpolate between current and target root TM with this weight.
      const float rootRotationsPerSecond =
        NMP::maximum(SCALE_FREQUENCY(limbState.m_rootData.angularVelocity.magnitude() - 0.1f) * ONE_OVER_2_PI, 0.0f);
      const float expectedRootInterpolationWeight =
        NMP::minimum(rootRotationsPerSecond * pkg.in->getAimInfo().rootRotationCompensation, 1.0f);
      // Calculate an effective root TM. 
      NMP::Matrix34 effectiveRootTM;
      effectiveRootTM.interpolate(targetRootTM, limbState.m_rootTM, expectedRootInterpolationWeight);
      // Find the desired world space target for the end effector.
      endTargetTM = endTargetTM * effectiveRootTM;

      // Decompose transform from current to target end TM into rotation vectors about and orthogonal to the limb
      // direction in the input pose. These rotation vectors are then scaled by the pose twist and swing weights, then
      // recombined to produce the final end target transform.
      NMP::Matrix34 poseEndTM =
        pkg.in->getAimInfo().poseEndRelativeToRoot * limbState.m_rootTM;
      NMP::Matrix34 poseEndInvTM = poseEndTM;
      poseEndInvTM.invertFast();
      NMP::Matrix34 endOffset = endTargetTM * poseEndInvTM;
      NMP::Vector3 endTargetOffsetTranslation = endOffset.translation();

      NMP::Vector3 endOffsetAsRotationVector = endOffset.toRotationVector();
      NMP::Vector3 endOffsetOrthogonalToLimbAxis =
        endOffsetAsRotationVector.getComponentOrthogonalToDir(pkg.in->getAimInfo().poseEndRelativeToRoot.translation());
      NMP::Vector3 endOffsetAboutLimbAxis = endOffsetAsRotationVector - endOffsetOrthogonalToLimbAxis;

      // Weight each rotation axis according to values in aim info.
      endOffsetAboutLimbAxis        *= pkg.in->getAimInfo().poseTwistWeight;
      endOffsetOrthogonalToLimbAxis *= pkg.in->getAimInfo().poseSwingWeight;
      endTargetOffsetTranslation    *= pkg.in->getAimInfo().poseSwingWeight;

      endOffsetAsRotationVector = endOffsetAboutLimbAxis + endOffsetOrthogonalToLimbAxis;
      endOffset.fromRotationVector(endOffsetAsRotationVector);
      endOffset.translation() = endTargetOffsetTranslation;

      endTargetTM = endOffset * poseEndTM;

      // Apply limb type specific end target corrections. This includes self avoidance, ensuring that the winding of 
      // limbs match across the whole rig and limiting target positions to within the limb's reach.
      const NMP::Vector3 preCorrectionTargetPosition = endTargetTM.translation();
      aimEndTargetCorrection<PackageType>(pkg, endTargetTM, pDebugDrawInst);

      // Reduce the IK orientation weight for large target adjustments. This avoids odd looking wrist
      // orientations when approaching the target position and reduces the risk that the IK will fail to reach the
      // target position because it is prioritising the target orientation.
      const float targetCorrectionDistance =
        (endTargetTM.translation() - preCorrectionTargetPosition).magnitude();
      if (targetCorrectionDistance > SCALE_DIST(0.2f))
      {
        limbControlPositionWeight    = 1.0f;
        limbControlOrientationWeight = 0.2f;
      }
    }
    else
    {
      endTargetTM = pkg.in->getAimInfo().poseEndRelativeToRoot * limbState.m_rootTM;
    }

    // Configure limb control.
    LimbControl& control = pkg.out->startControlModification();
    control.reinit(
      pkg.owner->data->normalStiffness * NMP::fastSqrt(pkg.in->getAimInfo().strengthScale),
      pkg.owner->data->normalDampingRatio * pkg.in->getAimInfo().dampingScale,
      pkg.owner->data->normalDriveCompensation);
    control.setTargetPos(endTargetTM.translation(), limbControlPositionWeight);
    control.setTargetOrientation(endTargetTM, limbControlOrientationWeight);
    control.setSupportForceMultiplier(1.0f);
    control.setImplicitStiffness(1.0f);
    control.setGravityCompensation(pkg.in->getAimInfo().gravityCompensation);
    control.setEndSupportAmount(0.0f);
    control.setUseIncrementalIK(pkg.in->getAimInfo().useIncrementalIK);
    control.setSwivelAmount(pkg.in->getAimInfo().swivelAmount);
    control.setTargetDeltaStep(pkg.in->getAimInfo().limbControlDeltaStep);

    const float angVelPredictability = 0.75f;
    control.setExpectedRootForTarget(
      currentRootTM,
      1.0f,
      NMP::Vector3Zero(),
      limbState.m_rootData.angularVelocity * angVelPredictability);

    control.setControlAmount(ER::aimControl, 1.0f);
    pkg.out->stopControlModification(importance);

#if defined(MR_OUTPUT_DEBUGGING)
    const NMP::Vector3 endPosition = limbState.m_endTM.translation();

    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, endTargetTM, SCALE_DIST(0.4f));

    if (static_cast<int32_t>(pkg.in->getAimInfo().aimingLimbIndex) == pkg.owner->data->limbRigIndex)
    {
      NMP::Vector3 aimingDirectionInWorldSpace(NMP::Vector3::InitZero);
      aimingDirectionInWorldSpace.x = pkg.in->getAimInfo().aimDirection[0];
      aimingDirectionInWorldSpace.y = pkg.in->getAimInfo().aimDirection[1];
      aimingDirectionInWorldSpace.z = pkg.in->getAimInfo().aimDirection[2];
      endTargetTM.rotateVector(aimingDirectionInWorldSpace);

      const float       lineLength = SCALE_DIST(32.0f);
      const uint32_t    lineTickCount = 16;
      const NMP::Colour lineColour = NMP::Colour::DARK_ORANGE;

      for (uint32_t i = 0; i < lineTickCount; ++i)
      {
        const NMP::Vector3 tickPosition = endPosition +
          aimingDirectionInWorldSpace * lineLength * static_cast<float>(i + 1) / lineTickCount;
        MR_DEBUG_DRAW_POINT(pDebugDrawInst, tickPosition, SCALE_DIST(0.1f), lineColour);
      }

      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta, endPosition,
        aimingDirectionInWorldSpace * lineLength, lineColour);
    }
#endif
  }

  return endTargetTM;
}

}

#undef SCALING_SOURCE

#endif // NM_AIM_INL
