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
#include "HeadPackaging.h"
#include "HeadPointPackaging.h"
#include "HeadPoint.h"
#include "euphoria/erDebugDraw.h"
#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
// HeadPointUpdatePackage
//----------------------------------------------------------------------------------------------------------------------
void HeadPointUpdatePackage::update(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  data->process.importance = 0.0f;
  if (in->getTargetImportance() == 0.0f)
  {
    data->headTM.setTranslation(NMP::Vector3(0, 0, 0));
    return; // nothing to do
  }

  // by default use the target that came in
  // this input is connected directly to head's "point target" input
  // which is in turn fed by environment awareness
  NMP::Vector3 pointTarget = in->getTarget().target;
  // we want the chest to rotate so the comfy head pose wi ll face the target
  // the false at the end means point doesn't give up control
  float pointTargetImportance = data->process.processRequest(
    in->getTargetImportance(),
    in->getTarget().imminence,
    owner->data->normalStiffness,
    in->getTarget().passOnAmount, 0.0f, owner->owner->data->normalStiffness * in->getTarget().stiffnessScale);

  if (pointTargetImportance)
  {
    // What to do with the roll of the head? well if its near vertical then try and keep it so, otherwise blend towards
    // matching the roll of the chest.
    const ER::LimbTransform& chestTM = owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_rootTM;
    NMP::Vector3 chestUp = chestTM.upDirection();
    float toChestUpBlend = NMP::minimum(1.0f - chestUp.dot(owner->owner->data->up), 1.0f);

    const ER::LimbTransform& end = owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_endTM;
    MR_DEBUG_DRAW_POINT(pDebugDrawInst, pointTarget, SCALE_DIST(0.2f), NMP::Colour::WHITE);
    NMP::Matrix34 comfyHeadWorldSpace;
    comfyHeadWorldSpace.multiply(owner->owner->data->defaultPoseEndsRelativeToRoot[owner->data->limbNetworkIndex],
      owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_rootTM);
    float stiffness = data->process.stiffnessScale * owner->data->normalStiffness;

    // using this (rather than currentHeadPos) reduces feedback effects when target near to head
    NMP::Vector3 pointDir = pointTarget - comfyHeadWorldSpace.translation();
    MR_DEBUG_DRAW_LINE(pDebugDrawInst, comfyHeadWorldSpace.translation(), pointTarget,  NMP::Colour::LIGHT_YELLOW);
    float distanceToSingularity = pointDir.normaliseGetLength();
    float dontPointRadius = SCALE_DIST(0.1f);
    if (distanceToSingularity > dontPointRadius) // only move the head if the target is outside this radius
    {
      NMP::Quat quat;
      quat.forRotation(data->headTM.frontDirection(), pointDir);
      data->headTM.multiply3x3(NMP::Matrix34(quat));
      MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, data->headTM, SCALE_DIST(0.5f));
      NMP::Vector3 mildSingularity = chestUp * toChestUpBlend + owner->owner->data->up * (1.0f - toChestUpBlend);
      mildSingularity += chestTM.frontDirection() * 0.2f;
      mildSingularity.normalise();
      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, end.translation(), mildSingularity, NMP::Colour::WHITE);

      // now we decide how much to use the quaternion version, based on how close we are to the singularity where mildSingularity and pointDir are parallel
      float blend = 1.0f - fabsf(mildSingularity.dot(pointDir)); // i.e. 0 to 1
      float headAlertnessScale = 2.0f; // higher values will move faster when target is up/down but looks weird if too fast as can cause feedback cycle
      blend = NMP::minimum(blend * headAlertnessScale, 1.0f);
      NMP::Vector3 headUp = data->headTM.upDirection() * (1.0f - blend) + mildSingularity * blend;
      headUp.fastNormalise();

      data->headTM.createFromForwardAlignedByUp(pointDir, headUp, end.translation());
    }

    NMP_ASSERT(data->headTM.isValidTM3x3(0.01f));
    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, data->headTM, SCALE_DIST(1.5f));

    LimbControl& control = out->startControlModification();
    control.reinit(
      stiffness,
      owner->data->normalDampingRatio,
      owner->data->normalDriveCompensation);

    // Don't set a target position because it tends to conflict with the target orientation, and if it's not high enough
    // it pulls the head town and makes it tilt. Also don't set it as other modules want to set the position at the same
    // time, e.g. the cushion/avoid modules
    control.setTargetOrientation(data->headTM, 2.0f);
    control.setGravityCompensation(1.0f);
    control.setControlAmount(ER::pointControl, 1.0f);
    out->stopControlModification(pointTargetImportance);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// HeadPointFeedbackPackage
//----------------------------------------------------------------------------------------------------------------------
void HeadPointFeedbackPackage::feedback(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  if (data->headTM.translation() == NMP::Vector3(0, 0, 0))
  {
    data->headTM = owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_endTM;
  }

  if (data->process.importance)
  {
    // get comfortable head transform
    NMP::Matrix34 comfyHeadWorldSpace;
    comfyHeadWorldSpace.multiply3x3(
      owner->owner->data->defaultPoseEndsRelativeToRoot[owner->data->limbNetworkIndex],
      owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_rootTM);

    // create relative rotation from comfy pose to desired pose
    NMP::Quat toTarget = data->headTM.toQuat() * ~comfyHeadWorldSpace.toQuat();

    // request this rotation
    RotationRequest request(toTarget.toRotationVector(), SCALING_SOURCE);
    request.imminence = in->getTarget().imminence;
    request.stiffnessScale = in->getTarget().stiffnessScale;
    request.passOnAmount = in->getTarget().passOnAmount; // this makes the pelvis move less
#if defined(MR_OUTPUT_DEBUGGING)
    request.debugControlAmounts.setControlAmount(ER::pointControl, 1.0f);
#endif
    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_AngleDelta,
      owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_rootTM.translation(),
      request.rotation, NMP::Colour(ER::getDefaultColourForControl(ER::pointControl)));
    feedOut->setRootRotationRequest(request, data->process.importance);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void HeadPoint::entry()
{
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

