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
#include "SpineReachReactionPackaging.h"
#include "SpineReachReaction.h"
#include "euphoria/erDebugDraw.h"
#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"
#include "euphoria/erSharedEnums.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{
//----------------------------------------------------------------------------------------------------------------------
void SpineReachReactionUpdatePackage::update(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  float reachTargetImportance = in->getReachTargetImportance();
  float avgReachingHandPosImportance = in->getAvgReachingHandPosImportance();

  bool balancing = (in->getSupportAmount() > 1e-5f);

  if (!balancing && reachTargetImportance > 0 && avgReachingHandPosImportance > 0)
  {
    ReachTarget rt = in->getReachTarget();

    // check this is a reach for spine
    if ((rt.type == ER::LimbTypeEnum::L_spine) || (rt.type == ER::LimbTypeEnum::L_leg))
    {
      const NMP::Matrix34& end = owner->owner->data->spineLimbSharedStates[owner->data->childIndex].m_endTM;
      const NMP::Matrix34& root = owner->owner->data->spineLimbSharedStates[owner->data->childIndex].m_rootTM;
      NMP::Vector3 handPos(in->getAvgReachingHandPos());

      // get the hand position on the DefaultPose chest
      // (we are assuming that the hands are rigidly attached to the chest)
      NMP::Vector3 handPosChestLocal;
      end.inverseTransformVector(handPos, handPosChestLocal);

      // Get the chest tm in the default pose.
      NMP::Matrix34 chestDefaultPoseW =
        owner->owner->data->defaultPoseEndsRelativeToRoot[owner->data->limbNetworkIndex] * root;
      NMP::Vector3 handPosDefaultPoseWorld;
      chestDefaultPoseW.transformVector(handPosChestLocal, handPosDefaultPoseWorld);

      // get the direction vectors going between chest and hand and chest and hit point (on the DefaultPose'd chest)
      NMP::Vector3 chestToDefaultPoseHandPosW(handPosDefaultPoseWorld - end.translation());
      NMP::Vector3 chestToReachPointW(rt.point - end.translation());
      // get the direction vectors going between root and hand and root and hit point (on the DefaultPose'd chest)
      NMP::Vector3 rootToDefaultPoseHandPosW(handPosDefaultPoseWorld - root.translation());
      NMP::Vector3 rootToReachPointW(rt.point - root.translation());

      // remove any component in the direction of the spine (to give only a twist)
      if (rt.type == ER::LimbTypeEnum::L_spine)
      {
        NMP::Vector3 spineDir(vNormalise(end.translation() - root.translation()));
        chestToDefaultPoseHandPosW -= spineDir * spineDir.dot(chestToDefaultPoseHandPosW);
        chestToReachPointW -= spineDir * spineDir.dot(chestToReachPointW);
      }

      // get the rotational correction required for chest orientation
      NMP::Quat qChestReOri;
      qChestReOri.forRotation(chestToDefaultPoseHandPosW, chestToReachPointW);
      // get the rotational correction required for chest position
      NMP::Quat qChestRePos;
      qChestRePos.forRotation(rootToDefaultPoseHandPosW, rootToReachPointW);

      // apply the corrections to the DefaultPose to get the target orientation
      NMP::Quat qTarget;
      qTarget.multiply(qChestReOri, chestDefaultPoseW.toQuat());

      NMP::Vector3 targetPos;
      NMP_ASSERT(qTarget.isNormal());
      NMP::Vector3 chestDefaultPoseRelRootW = targetPos - root.translation();
      if (rt.type == ER::LimbTypeEnum::L_leg)
      {
        qChestRePos.rotateVector(chestDefaultPoseRelRootW, targetPos);
        targetPos += root.translation();
      }
      else
      {
        targetPos = chestDefaultPoseRelRootW + root.translation();
      }
      MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, chestDefaultPoseW, SCALE_DIST(2.0f));

      // output some IK control
      LimbControl& control = out->startControlModification();
      control.reinit(owner->data->normalStiffness, owner->data->normalDampingRatio, owner->data->normalDriveCompensation);
      control.setTargetOrientation(qTarget, 1.0f);
      control.setTargetPos(targetPos, 1.0f);
      control.setGravityCompensation(1.0f);
      control.setControlAmount(ER::reachControl, 1.0f);
      out->stopControlModification(reachTargetImportance);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SpineReachReaction::entry()
{
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

