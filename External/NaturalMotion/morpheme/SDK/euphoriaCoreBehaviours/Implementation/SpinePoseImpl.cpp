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
#include "SpinePackaging.h"
#include "SpinePosePackaging.h"
#include "SpinePose.h"
#include "euphoria/erDebugDraw.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void SpinePoseUpdatePackage::update(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  float poseHIImp = in->getSpinePoseHighImpImportance() ;
  if (poseHIImp > 0.0f)
  {
    PoseData poseData = in->getSpinePoseHighImp();
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
    TM.multiply(owner->owner->data->spineLimbSharedStates[owner->data->childIndex].m_rootTM);
    NMP::Vector3 vel = owner->owner->data->spineLimbSharedStates[owner->data->childIndex].m_rootTM.getRotatedVector(
      poseData.poseEndRelativeToRootVelocity);
    NMP::Vector3 angVel = owner->owner->data->spineLimbSharedStates[owner->data->childIndex].m_rootTM.getRotatedVector(
      poseData.poseEndRelativeToRootAngularVelocity);
    control.setTargetPos(TM.translation(), 1.0f, vel);
    control.setTargetOrientation(TM, 1.0f, angVel);
    control.setGravityCompensation(poseData.gravityCompensation);
    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, TM, SCALE_DIST(0.5f));

    out->stopControlHighImpModification(poseHIImp);
  }
  float poseLIImp = in->getSpinePoseLowImpImportance() ;
  // No need to set the low importance if the high imp is 1.0 as it's a priority junction.
  if (poseHIImp < 1.0f && poseLIImp > 0.0f)
  {
    PoseData poseData = in->getSpinePoseLowImp();
    // Set control limb.
    LimbControl& control = out->startControlLowImpModification();

    control.reinit(
      owner->data->normalStiffness * poseData.stiffnessScale,
      owner->data->normalDampingRatio * poseData.dampingRatioScale,
      owner->data->normalDriveCompensation *  poseData.driveCompensationScale);

    control.setStrengthReductionTowardsEnd(poseData.strengthReductionTowardsEnd);
    control.setControlAmount(ER::animateControl, 1.0f);

    // Compute target translation and orientation in world space.
    NMP::Matrix34 TM(poseData.poseEndRelativeToRoot);
    TM.multiply(owner->owner->data->spineLimbSharedStates[owner->data->childIndex].m_rootTM);
    NMP::Vector3 vel = owner->owner->data->spineLimbSharedStates[owner->data->childIndex].m_rootTM.getRotatedVector(
      poseData.poseEndRelativeToRootVelocity);
    NMP::Vector3 angVel
      = owner->owner->data->spineLimbSharedStates[owner->data->childIndex].m_rootTM.getRotatedVector(
          poseData.poseEndRelativeToRootAngularVelocity);
    control.setTargetPos(TM.translation(), 1.0f, vel);
    control.setTargetOrientation(TM, 1.0f, angVel);
    control.setGravityCompensation(poseData.gravityCompensation);
    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, TM, SCALE_DIST(0.5f));

    out->stopControlLowImpModification(poseLIImp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SpinePose::entry()
{
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

