/*
* Copyright (c) 2011 NaturalMotion Ltd. All rights reserved.
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

//----------------------------------------------------------------------------------------------------------------------
#include "MyNetworkPackaging.h"
#include "ArmPosePackaging.h"
#include "ArmPose.h"
#include "ArmPackaging.h"
#include "Helpers/Helpers.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
// ArmPoseUpdatePackage
//----------------------------------------------------------------------------------------------------------------------
void ArmPoseUpdatePackage::update(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  float poseHIImp = in->getArmPoseHighImpImportance() ;
  if (poseHIImp > 0.0f)
  {
    PoseData poseData = in->getArmPoseHighImp();
    // Set control limb.
    LimbControl& control = out->startControlHighImpModification();

    control.reinit(
      owner->data->normalStiffness * poseData.stiffnessScale,
      owner->data->normalDampingRatio * poseData.dampingRatioScale,
      owner->data->normalDriveCompensation *  poseData.driveCompensationScale);

    control.setStrengthReductionTowardsEnd(poseData.strengthReductionTowardsEnd);
    control.setControlAmount(ER::poseControl, 1.0f);

    // Compute target translation and orientation in world space.
    NMP::Matrix34 TM(poseData.poseEndRelativeToRoot);
    TM.multiply(owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootTM);
    NMP::Vector3 vel 
      = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootTM.getRotatedVector(poseData.poseEndRelativeToRootVelocity);
    NMP::Vector3 angVel
      = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootTM.getRotatedVector(
      poseData.poseEndRelativeToRootAngularVelocity);
    control.setTargetPos(TM.translation(), 1.0f, vel);
    control.setTargetOrientation(TM, 1.0f, angVel);
    control.setGravityCompensation(poseData.gravityCompensation);
    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, TM, SCALE_DIST(0.5f));
    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Velocity, TM.translation(), vel, NMP::Colour::WHITE);

    out->stopControlHighImpModification(poseHIImp);
  }
  float poseLIImp = in->getArmPoseLowImpImportance() ;
  if (poseHIImp < 1.0f && poseLIImp > 0.0f)
  {
    PoseData poseData = in->getArmPoseLowImp();
    // Compute target translation and orientation in world space.
    NMP::Matrix34 TM(poseData.poseEndRelativeToRoot);
    TM.multiply(owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootTM);

    // Set control limb.
    LimbControl& control = out->startControlLowImpModification();

// The control here gives the arms higher damping when they are far from their target
// this means that loose arms will still be controlled as they transition from a previous position
// they won't simply swing down limply, the will move down slowly, but stay loose when they are close to their target.
// TODO: make this more data driven, e.g. on a flag, if it works.
#define STABILISED_POSE
#if defined(STABILISED_POSE)
    float distanceToTarget = NMP::vDistanceBetween(TM.translation(),
      owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_endTM.translation());
    float error = distanceToTarget / owner->data->length;
    // scaled so that dampingRatio is less affected if poseData.stiffnessScale is close to 1
    // TODO: rather than this line below, we should add a flag and parameters in order to control this dampingRatio scale.
    error *= NMP::maximum(0.0f, 1.0f - poseData.stiffnessScale) / 0.7f;
    error = NMP::clampValue(error, 0.0f, 1.0f);
    // blend damping ratio
    const float maxDampingRatio = 8.0f; // for when arm is far from target
    float dampingRatio = maxDampingRatio * error + (owner->data->normalDampingRatio * poseData.dampingRatioScale) * (1.0f - error);
    control.reinit(
      owner->data->normalStiffness * poseData.stiffnessScale,
      dampingRatio,
      owner->data->normalDriveCompensation *  poseData.driveCompensationScale);
#else
    control.reinit(
      owner->data->normalStiffness * poseData.stiffnessScale,
      owner->data->normalDampingRatio * poseData.dampingRatioScale,
      owner->data->normalDriveCompensation *  poseData.driveCompensationScale);
#endif
    control.setStrengthReductionTowardsEnd(poseData.strengthReductionTowardsEnd);
    control.setControlAmount(ER::animateControl, 1.0f);

    NMP::Vector3 vel
      = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootTM.getRotatedVector(
      poseData.poseEndRelativeToRootVelocity);
    NMP::Vector3 angVel = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootTM.getRotatedVector(
      poseData.poseEndRelativeToRootAngularVelocity);
    control.setTargetPos(TM.translation(), 1.0f, vel);
    control.setTargetOrientation(TM, 1.0f, angVel);
    control.setGravityCompensation(poseData.gravityCompensation);
    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, TM, SCALE_DIST(0.5f));
    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Velocity, TM.translation(), vel, NMP::Colour::WHITE);

    out->stopControlLowImpModification(poseLIImp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ArmPose::entry()
{
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE
