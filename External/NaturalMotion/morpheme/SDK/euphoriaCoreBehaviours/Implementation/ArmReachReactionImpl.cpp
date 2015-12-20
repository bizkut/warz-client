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
#include "ArmReachReactionPackaging.h"
#include "ArmReachReaction.h"
#include "euphoria/erDebugDraw.h"
#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"
#include "Helpers/Reach.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
// ArmReachReactionUpdatePackage
//----------------------------------------------------------------------------------------------------------------------
void ArmReachReactionUpdatePackage::update(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  // determine the existence of a reach target on this arm
  const int32_t thisArmRigIndex = owner->data->limbRigIndex;
  //
  float maxReachTargetImportance = 0.0f;
  uint32_t maxReachTargetImportanceIndex = owner->owner->data->numArms + 1;
  for (uint32_t i = 0; i < owner->owner->data->numArms; ++i)
  {
    float imp = in->getCurrentReachTargetsImportance(i);
    if (imp > 0.0f)
    {
      const ReachTarget& rt = in->getCurrentReachTargets(i);

      // valid targets are parts on this arm that are not the root part
      if (rt.limbRigIndex == thisArmRigIndex && rt.partIndex != 0)
      {
        maxReachTargetImportance = NMP::maximum(maxReachTargetImportance, imp);
        maxReachTargetImportanceIndex = i;
      }
    }
  }

  // valid reach target index implies target was found above
  if (maxReachTargetImportanceIndex < owner->owner->data->numArms)
  {
    const ReachTarget& rt = in->getCurrentReachTargets(maxReachTargetImportanceIndex);

    // check this is a reach for this arm and not the arm root
    if ((rt.limbRigIndex == owner->data->limbRigIndex && rt.partIndex != 0))
    {
      // grab the animation input
      const ER::HandFootTransform& root = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootTM;
      const ER::HandFootTransform& handPoseRelRoot(in->getArmHitPoseEndRelRoot());
      const ER::HandFootTransform& handPoseW(handPoseRelRoot * root);

      NMP::Vector3 ikTgt = reachApproachAdjustedTarget(
        handPoseW.translation(),
        handPoseW.normalDirection(),
        owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_endTM.translation(),
        owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_reachLimit.getReachConeDir(),
        0,
        owner->data->length);

      // Ouput distance to target
      float distanceToTarget = (owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_endTM.translation() - handPoseW.translation()).magnitude();
      out->setDistanceToTarget(distanceToTarget);
      // Correlate orientation weight with proximity to target
      float thresholdD = 0.01f;
      float orientationWeight = 1.0f;
      if (distanceToTarget > thresholdD)
      {
        orientationWeight = thresholdD/distanceToTarget;
      }

      // output some IK control
      LimbControl& control = out->startControlModification();
      control.reinit(owner->data->normalStiffness, owner->data->normalDampingRatio, owner->data->normalDriveCompensation);

      control.setTargetPos(ikTgt, 1.0f);
      control.setTargetOrientation(handPoseW, orientationWeight);
      control.setGravityCompensation(1.0f);
      control.setControlAmount(ER::reachControl, 1.0f);
      out->stopControlModification(maxReachTargetImportance);

      MR_DEBUG_DRAW_POINT(pDebugDrawInst,
        owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_endTM.translation(),
        SCALE_DIST(0.1f), NMP::Colour::WHITE);
      MR_DEBUG_DRAW_POINT(pDebugDrawInst, handPoseW.translation(), SCALE_DIST(0.1f), NMP::Colour::RED);
      MR_DEBUG_DRAW_POINT(pDebugDrawInst, ikTgt, SCALE_DIST(0.1f), NMP::Colour::GREEN);
      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta, handPoseW.translation(),
        SCALE_DIST(owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_reachLimit.getReachConeDir()), NMP::Colour::GREEN);
      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta, handPoseW.translation(), SCALE_DIST(handPoseW.normalDirection()), NMP::Colour::RED);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ArmReachReaction::entry()
{
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

