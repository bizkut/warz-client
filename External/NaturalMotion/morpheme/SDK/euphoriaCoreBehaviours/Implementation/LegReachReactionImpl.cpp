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
#include "LegReachReactionPackaging.h"
#include "LegReachReaction.h"
#include "euphoria/erDebugDraw.h"
#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{
//----------------------------------------------------------------------------------------------------------------------
void LegReachReactionUpdatePackage::update(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* NMP_UNUSED(pDebugDrawInst))
{
  float reachTargetImportance = in->getReachTargetImportance();
  float avgReachingHandPosImportance = in->getAvgReachingHandPosImportance();
  bool balancing = (in->getSupportAmount() > 1e-5f);

  if (!balancing && reachTargetImportance > 0 && avgReachingHandPosImportance > 0)
  {
    ReachTarget rt = in->getReachTarget();

    // check this is a reach for this leg and not the leg root
    if ((rt.limbRigIndex == owner->data->limbRigIndex && rt.partIndex != 0))
    {
      const NMP::Matrix34& end = owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_endTM;
      const NMP::Matrix34& root = owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_rootTM;
      NMP::Vector3 reachingHandPos(in->getAvgReachingHandPos());

      // figure out a compression of the leg given the target, current compression of the leg, and the default pose
      //
      // we first "correct" the whole assembly by rotating the whole shooting match (foot and target positions)
      // to consider the current foot position in line with the DefaultPose foot position
      // (this manoevre makes the compression independent from the current rotation and is to encourage a little more
      // leg bend than we would otherwise be seeing)

      // get foot DefaultPose, current foot pos and target pos relative to the pelvis
      NMP::Vector3 footDefaultPoseW;
      root.transformVector(
        owner->owner->data->defaultPoseEndsRelativeToRoot[owner->data->limbNetworkIndex].translation(),
        footDefaultPoseW);
      NMP::Vector3 footBPRelPelvisW = footDefaultPoseW - root.translation();
      NMP::Vector3 targetRelPelvisW = rt.point - root.translation();
      NMP::Vector3 footRelPelvisW = end.translation() - root.translation();
      //
      // calc rotational correction (about pelvis) required to align the current foot pos with DefaultPose foot pos
      NMP::Quat qFootToDefaultPoseCorrection;
      qFootToDefaultPoseCorrection.forRotation(footRelPelvisW, footBPRelPelvisW);
      //
      // and calc the corrected foot and target positions
      NMP::Vector3 correctedFootRelPelvis, correctedTargetRelPelvis;
      qFootToDefaultPoseCorrection.rotateVector(footRelPelvisW, correctedFootRelPelvis);
      qFootToDefaultPoseCorrection.rotateVector(targetRelPelvisW, correctedTargetRelPelvis);
      NMP::Vector3 correctedFootPos = root.translation() + correctedFootRelPelvis;
      NMP::Vector3 correctedTargetPos = root.translation() + correctedTargetRelPelvis;

      // calc leg compression (this is analagous to "crouch" on a leg involved in supporting upper body)
      // if we were in support under balancing, supportHeight would be the ground reference here it is just the current
      // foot position (after the correction we made above)
      // "Up" would come from network gravity here it is the vector difference between foot and pelvis
      //
      NMP::Vector3 up = root.translation() - correctedFootPos;
      up.normalise();
      float supportHeight = correctedFootPos.dot(up);
      float pelvisHeight = root.translation().dot(up);
      float targetHeight = correctedTargetPos.dot(up);
      float effectorHeight = reachingHandPos.dot(up);

      float a0 = targetHeight - pelvisHeight; // drop from pelvis to target
      float b0 = supportHeight - targetHeight; // drop from target to feet
      float x = effectorHeight - pelvisHeight; // drop pelvis to hand

      float d = 0;
      if ((a0 < -1e-2f) && (x < -1e-2f))
      {
        d = (a0 - x) * (1 + a0 / (a0 + b0));
      }
      NMP::Vector3 correctedLiftedFootPos = end.translation() - d * up;
      NMP::Vector3 liftedFootPos;
      qFootToDefaultPoseCorrection.inverseRotateVector(correctedLiftedFootPos, liftedFootPos);

      // calc "swing" to move target towards reaching hand
      //

      // get the direction vectors going between pelvis and hand and pelvis and hit point
      NMP::Vector3 rootToHandW(reachingHandPos - root.translation());
      NMP::Vector3 rootToReachPointW(rt.point - root.translation());

      // get the rotational correction required to align the direction attached to the (lifted) foot with the hand
      NMP::Quat q;
      q.forRotation(rootToReachPointW, rootToHandW);
      NMP::Vector3 liftedFootPosRelPelvis;
      q.rotateVector(liftedFootPos - root.translation(), liftedFootPosRelPelvis);
      NMP::Vector3 targetFootPos = root.translation() + liftedFootPosRelPelvis;

      // output some IK control
      LimbControl& control = out->startControlModification();
      control.reinit(
        owner->data->normalStiffness, owner->data->normalDampingRatio, owner->data->normalDriveCompensation);
      control.setTargetPos(targetFootPos, 2.0f);
      control.setGravityCompensation(1.0f);
      control.setControlAmount(ER::reachControl, 1.0f);
      out->stopControlModification(reachTargetImportance);
    }

  }
}

//----------------------------------------------------------------------------------------------------------------------
void LegReachReaction::entry()
{
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

