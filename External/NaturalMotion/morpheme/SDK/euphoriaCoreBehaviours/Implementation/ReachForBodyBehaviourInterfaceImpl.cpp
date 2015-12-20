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
#include "MyNetwork.h"

#include "euphoria/erBody.h"
#include "euphoria/erLimb.h"
#include "euphoria/erBodyDef.h"
#include "euphoria/erDebugDraw.h"

#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMMatrix34.h"
#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"
#include "Types/BodyHitInfo.h"
#include "Types/ActorImpulse.h"


//----------------------------------------------------------------------------------------------------------------------
#include "ReachForBodyBehaviourInterface.h"
#include "Behaviours/ReachForBodyBehaviour.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{
//----------------------------------------------------------------------------------------------------------------------
// aux

static bool updateBodyHitInfo(float timeStep, const ReachForBodyBehaviourData& params, BodyHitInfo* bhi);

//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------

void ReachForBodyBehaviourInterface::update(float timeStep)
{
  // grab the morpheme params
  const ReachForBodyBehaviourData& params =
    ((ReachForBodyBehaviour*)(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_ReachForBody)))->getParams();

  // update body hit info from morpheme cp's
  //
  bool haveNoHitInfo = updateBodyHitInfo(timeStep, params, data->lastBodyHitInfo);

  // from body hit set is reacting flag as appropriate
  // and early out if no longer reacting

  // no valid hit info: clear flag
  if (haveNoHitInfo)
  {
    data->isReacting = false;
  }
  else
  {
    data->isReacting = true;
  }

  // if not reacting then nothing further to do
  if (!data->isReacting)
    return;

  // write outputs:
  //  arm hit pose
  //  body hit infos
  //  reach action params
  //  effector enabled/disabled flags
  //
  const BodyPoseData& armHitPose = params.getArmReachForBodyPose();
  for (uint32_t i = 0 ; i < owner->data->numArms ; ++i)
  {
    // outputs
    out->setArmHitPoseEndRelRootAt(i, armHitPose.m_poseEndRelativeToRoots[i], armHitPose.m_poseWeights[i]);

    // make body hit info available to the network
    //
    out->setBodyHitInfoAt(i, data->lastBodyHitInfo[i]);

    if (data->lastBodyHitInfo[i].limbRigIndex != -1 && data->lastBodyHitInfo[i].partIndex != -1)
    {
      // make reach action parameters available to the network (use default for now)
      //
      ReachActionParams raps;
      raps.setToDefaults(SCALING_SOURCE);
      raps.withinReachTimeout = SCALE_TIME(params.getWithinReachTimeout(i)); // hold for this many seconds
      raps.outOfReachTimeout = SCALE_TIME(params.getOutOfReachTimeout(i));
      raps.rampDownCompletedDuration = SCALE_TIME(params.getRampDownCompletedDuration(i));
      raps.rampDownFailedDuration = SCALE_TIME(params.getRampDownFailedDuration(i));
      raps.outOfReachDistance = SCALE_DIST(params.getOutOfReachDistance(i));
      raps.strength = params.getStrengthCP(i);
      raps.effectorSpeedLimit = SCALE_VEL(params.getEffectorSpeedLimitCP(i));
      raps.swivelMode = params.getSwivelMode(i);
      raps.swivelAmount = params.getSwivelAmount(i);
      raps.maxLimbExtensionScale = NMP::maximum(0.1f, params.getMaxArmExtensionScale(i));
      raps.torsoRadiusMultiplier = params.getTorsoAvoidanceRadiusMultiplier(i);
      raps.penetrationAdjustment = SCALE_DIST(params.getSurfacePenetration(i));
      raps.handsCollisionGroupIndex = params.getCollisionGroupIndex();
      out->setReachActionParamsAt(i, raps);
    }

    // inform network which arms are clear for hit reacting
    out->setArmReachForBodyPartFlagsAt(i, data->armReachForBodyPartFlags[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ReachForBodyBehaviourInterface::feedback(float NMP_UNUSED(timeStep))
{
  if (feedIn->getIsCompleteImportance() > 0)
  {
    data->isReacting = !feedIn->getIsComplete();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ReachForBodyBehaviourInterface::entry()
{
  data->isReacting = false;
  // default to having all arms available
  for (uint32_t i = 0; i < NetworkConstants::networkMaxNumArms; ++i)
  {
    data->armReachForBodyPartFlags[i] = true;
    data->lastBodyHitInfo[i] = BodyHitInfo();
  }
}

//----------------------------------------------------------------------------------------------------------------------
// aux

bool updateBodyHitInfo(float timeStep, const ReachForBodyBehaviourData& params, BodyHitInfo* bhi)
{
  int32_t numValidHitInfo = 0;
  const int32_t numArms = NetworkConstants::networkMaxNumArms;
  for (int32_t i = 0; i < numArms; ++i)
  {
    // 1: no news from outside so just update the elapsedTime and exit
    if (!params.getNewHitCP(i))
    {
      bhi[i].elapsedTime += timeStep;
    }
    // 2: news from outside so update bhi
    else
    {
      bhi[i].limbRigIndex = (int32_t)params.getLimbIndexCP(i);
      bhi[i].partIndex = (int32_t)params.getPartIndexCP(i);
      bhi[i].point.set(params.getPositionCP(i));
      bhi[i].normal.set(params.getNormalCP(i));
      bhi[i].normal.fastNormalise();
      bhi[i].elapsedTime = 0.0f;
    }
    if (bhi[i].limbRigIndex != -1 && bhi[i].partIndex != -1)
      ++numValidHitInfo;
  }

  // returns true if no valid hit infos were encountered
  return numValidHitInfo == 0;

}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

