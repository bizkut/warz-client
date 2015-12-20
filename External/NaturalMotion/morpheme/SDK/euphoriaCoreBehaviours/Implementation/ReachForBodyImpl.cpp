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
#include "BodyFramePackaging.h"
#include "ArmReachForBodyPartPackaging.h"

#include "Types/ReachTarget.h"
#include "Types/BodyHitInfo.h"
#include "ReachForBodyPackaging.h"
#include "ReachForBody.h"
#define SCALING_SOURCE owner->owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"
#include "euphoria/erDebugDraw.h"
#include "euphoria/erSharedEnums.h"
#include "NetworkDescriptor.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{
// update() aux
//
static uint32_t updateRefreshActiveEffectors(
  const BodyFrameAPIBase* const owner,
  const ReachForBodyInputs* in,
  ReachForBodyData* data,
  bool activeEffectors[NetworkConstants::networkMaxNumArms]);

// feedback() aux
//
static bool feedbackRefreshReachTarget(
  const BodyFrameAPIBase* const owner,
  const BodyHitInfo& hitInfo,
  const float timeStep,
  ReachTarget& rt);

//----------------------------------------------------------------------------------------------------------------------
// update() decides which effectors should be active (an effector may have been disabled via flag, or due to being
// required for bracing, which effector is yielded for bracing is determined on basis of the importance of a brace input
// and/or proximity of the given shoulder to the location of the brace hazard).
// Sets a reach target for each active effector plus a general one for full body supporting motion, a look target and 
// the reach action params.
//
void ReachForBodyUpdatePackage::update(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* NMP_UNUSED(pDebugDrawInst))
{
  if (data->haveReachTarget)
  {
    bool effectorFlagsForThisFrame[NetworkConstants::networkMaxNumArms];
    uint32_t numActiveEffectors = updateRefreshActiveEffectors(owner, in, data, effectorFlagsForThisFrame);

    if (numActiveEffectors > 0)
    {
      for (uint32_t i = 0; i < NetworkConstants::networkMaxNumArms; ++i)
      {
        if (data->reachTargetImportances[i])
        {
          out->setReachTargetsAt(i, data->rt[i], data->reachTargetImportances[i]);
          // Forces arm to "ready-position" even when not reaching
          out->setReachActionParamsAt(i, in->getReachActionParams(i));
        }
      }

      //
      // Output: "enabled/disabled" status for general access by the network
      // (in particular to the ArmReachForBodyPart module).
      //
      for (uint32_t i = 0; i < out->getMaxArmReachForBodyPartFlags(); ++i)
      {
        out->setArmReachForBodyPartFlagsAt(i, effectorFlagsForThisFrame[i]);
      }

      //
      // Output: look target, general reach target and average effector pos.
      //
      if (data->numArmsReaching > 0)
      {
        out->setErrorWeightedAverageEffectorPosition(data->weightedAverageEffectorPos, data->averageReachStrength);
      }
    } //~if (numActiveEffectors > 0)
  } // ~!(data->reachTargetImportance > 0)
}

//----------------------------------------------------------------------------------------------------------------------
// feedback() checks for new target input and reach module status.
// If no arms are reaching it feeds back "complete" otherwise caches info about any arms that are reaching.
//
void ReachForBodyFeedbackPackage::feedback(float timeStep, MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  data->haveReachTarget = false;
  bool newTarget = false;

  // Determines whether we have anything at all to do
  for (int32_t i = 0; i < NetworkConstants::networkMaxNumArms; ++ i)
  {
    data->reachTargetImportances[i] = feedIn->getHitInfoImportance(i);

    if (data->reachTargetImportances[i] > 0.0f)
    {
      data->haveReachTarget = true;
      if (feedbackRefreshReachTarget(owner, feedIn->getHitInfo(i), timeStep, data->rt[i]))
        newTarget = true;
    }
  }

  if (data->haveReachTarget)
  {
    // No new reach target then check that at least one arm is still reaching
    if (!newTarget)
    {
      uint32_t i = 0;
      for (; i < NetworkConstants::networkMaxNumArms; ++i)
      {
        // Break from loop if a reaching arm is found
        if (feedIn->getArmReachStatesImportance(i) != 0.0f &&
            feedIn->getArmReachStates(i) != ArmReachForBodyPartData::State_Done)
        {
          break;
        }
      }
      // Early out if all arms are done reaching
      if (i == NetworkConstants::networkMaxNumArms)
      {
        feedOut->setCompleted(true);
        return;
      }
    }

    // Either starting a reach or continuing to reach..

    // Cache feedIn's (for yield to brace, and for effector status)
    //
    data->averageReachStrength = 0.0f;
    float sumDistSqFromTarget = 0;
    data->numArmsReaching = 0;
    data->weightedAverageEffectorPos.setToZero();
    for (uint32_t i = 0; i < owner->owner->data->numArms; ++i)
    {
      // Read the competing control importances, and shoulder position info
      // update will use these to determine whether and if so which arm should stop reaching
      // to allow for bracing
      //
      data->competingControlImportances[i] = feedIn->getCompetingArmControlsImportance(i);
      data->shoulderPositions[i] = feedIn->getShoulderPositions(i);

      // Update the effector data for any arms that are actually reaching
      //
      data->isReaching[i] = feedIn->getEndEffectorDistanceSquaredToTargetsImportance(i) > 0.0f;
      if (data->isReaching[i])
      {
        // Positions, distance errors and strength reduction for each arm
        data->endEffectorTMs[i] = owner->owner->data->armLimbSharedStates[i].m_endTM;
        data->endEffectorDistanceSquaredToTargets[i] = feedIn->getEndEffectorDistanceSquaredToTargets(i);
        data->armStrengthReduction[i] = feedIn->getArmStrengthReduction(i);

        // Rolling sums for averages calcd below
        float reachStrength = (1.0f - data->armStrengthReduction[i]);
        data->averageReachStrength += reachStrength;
        ++data->numArmsReaching;
        sumDistSqFromTarget += data->endEffectorDistanceSquaredToTargets[i] * reachStrength;
        data->weightedAverageEffectorPos +=
          data->endEffectorTMs[i].translation() * data->endEffectorDistanceSquaredToTargets[i] * reachStrength;
      }
    }
    // Cache averages, reach strength and average effector position
    if (data->numArmsReaching > 0)
    {
      data->averageReachStrength /= (float)(data->numArmsReaching);
      data->weightedAverageEffectorPos /= sumDistSqFromTarget;
      MR_DEBUG_DRAW_POINT(pDebugDrawInst, data->weightedAverageEffectorPos, SCALE_DIST(1.0f), NMP::Colour::RED);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t updateRefreshActiveEffectors(
  const BodyFrameAPIBase* const owner, const ReachForBodyInputs* in,
  ReachForBodyData* data, bool activeEffectors[NetworkConstants::networkMaxNumArms])
{

  for (int32_t iRT = 0; iRT < NetworkConstants::networkMaxNumArms; ++iRT)
  {
    // Compile the list of active effectors.
    // An effector is deemed to be active if the game says so AND reachForBody hasn't seen fit to disable it.
    // ReachForBody disables an effector by maintaining it's own array of flags, at the start of a new reach, 
    // reachForBody enables the relevant effector.
    //
    if (data->rt[iRT].elapsedTime == 0.0f && data->reachTargetImportances[iRT] > 0)
    {
      data->armReachForBodyPartFlags[iRT] = true;

      // if this is an arm hit remove the arm that got hit from the active effector list
      if (data->rt[iRT].type == ER::LimbTypeEnum::L_arm)
      {
        int32_t hitArmIndex = data->rt[iRT].limbRigIndex - owner->owner->data->firstArmRigIndex;
        data->armReachForBodyPartFlags[hitArmIndex] = false;
      }
    }
  }
  // AND gameside with reachForBody's own flags to determine enabled/disabled status for each effector
  uint32_t numActiveEffectors = 0; // count the number of reaching effectors as we go
  for (uint32_t i = 0; i < in->getMaxArmReachForBodyPartFlags(); ++i)
  {
    activeEffectors[i] = in->getArmReachForBodyPartFlags(i) && data->armReachForBodyPartFlags[i];
    if (activeEffectors[i])
    {
      ++numActiveEffectors;
    }
  }

  // "yield to brace"
  //
  // If more than one arm is currently reaching, check brace LimbControl output importances and if
  // any is sufficiently large pick an effector to donate for use by the brace
  // we choose the arm with shoulder closest to the current brace target
  //

  // Determine if the character wants to be bracing (i.e. if any arm has sufficient brace importance)
  //
  bool wantsToBrace = false;
  const float braceHazardImp = in->getBraceHazardImportance();
  if (numActiveEffectors > 1 && braceHazardImp > 0.0f)
  {
    const float yieldImportance = 0.5f; //0.7f;             // yield threshold (found experimentally)
    for (uint32_t i = 0; i < in->getMaxArmReachForBodyPartFlags(); ++i)
    {
      if (activeEffectors[i] && data->competingControlImportances[i] > yieldImportance)
      {
        wantsToBrace = true;
        break;
      }
    }

    // If we do want to brace search active effectors for the one with shoulder closest to the brace hazard and exclude
    // that from the hit reaction
    //
    // note: this "closest effector" may not be the one with the highest brace importance or indeed have any brace
    // importance at all at this point, so we count on the assumption that if one arm is wanting to brace with high
    // importance then the closest will too within the next few frames and is the one most natural to excuse from
    // reaching duties. in practice this strategy seems to work nicely.
    //
    if (wantsToBrace)
    {
      // Search active effectors and store the index of the one with shoulder closest to the brace hazard
      //
      int32_t yieldArmIndex = -1;                             // index of effector to yield
      BraceHazard bh = in->getBraceHazard();                  // brace hazard
      NMP::Vector3 braceHazardPos = bh.position;   // brace hazard position
      float leastShoulderToHazardDistanceSq = 10e6f;          // least shoulder to bh distance
      for (uint32_t i = 0; i < in->getMaxArmReachForBodyPartFlags(); ++i)
      {
        if (activeEffectors[i])
        {
          float shoulderToHazardDistanceSq = (braceHazardPos - data->shoulderPositions[i]).magnitudeSquared();
          if (shoulderToHazardDistanceSq < leastShoulderToHazardDistanceSq)
          {
            leastShoulderToHazardDistanceSq = shoulderToHazardDistanceSq;
            yieldArmIndex = i;
          }
        }
      }
      // Remove the effector from hit reaction's own (persistent) flag set, (arms dedicated to bracing are never
      // reactivated - this stops arms cycling between brace/reach/brace/reach states which is usually inappropriate)
      // and from (the current frame's set) activeEffectors, which is the output of this function.
      //
      if (yieldArmIndex != -1)
      {
        activeEffectors[yieldArmIndex] = false;
        data->armReachForBodyPartFlags[yieldArmIndex] = false;
        --numActiveEffectors;
      }
    }
  }
  return numActiveEffectors;
}

//----------------------------------------------------------------------------------------------------------------------
bool feedbackRefreshReachTarget(
  const BodyFrameAPIBase* const owner,
  const BodyHitInfo& hitInfo,
  const float timeStep,
  ReachTarget& rt)
{
  // Always need to update reach target's geometry and framecount
  //
  rt.point.set(hitInfo.point);
  rt.normal.set(hitInfo.normal);
  rt.elapsedTime += timeStep;

  // Only update rt's limb/rig indices and type when we have a new hitInfo
  //
  bool newTarget = (hitInfo.elapsedTime == 0);
  if (newTarget)
  {
    // Zero the timer
    rt.elapsedTime = 0.0f;
    
    // Determine type of reach by limb/part index
    //
    rt.limbRigIndex = hitInfo.limbRigIndex;
    rt.partIndex = hitInfo.partIndex;
    bool isReachForSpine = (owner->owner->data->firstSpineRigIndex == (uint32_t)rt.limbRigIndex);
    bool isReachForLeg = false;
    bool isReachForHead = false;
    bool isReachForArm = false;
    if (!isReachForSpine)
    {
      // Is reach for leg?
      uint32_t iLegBegin = owner->owner->data->firstLegRigIndex;
      uint32_t iLegEnd = owner->owner->data->firstLegRigIndex + owner->owner->data->numLegs;
      for (uint32_t i = iLegBegin; (isReachForLeg == false) && (i < iLegEnd); ++i)
      {
        isReachForLeg = (i == (uint32_t)rt.limbRigIndex);
      }
      // If reach for root of leg then count this as a reach for spine
      if (isReachForLeg && (rt.partIndex == 0))
      {
        isReachForLeg = false;
        isReachForSpine = true;
      }
      // Not reach for leg (or spine) then is reach for head?
      if (!isReachForLeg && !isReachForSpine)
      {
        uint32_t iHeadBegin = owner->owner->data->firstHeadRigIndex;
        uint32_t iHeadEnd = owner->owner->data->firstHeadRigIndex + owner->owner->data->numHeads;
        for (uint32_t i = iHeadBegin; (isReachForHead == false) && (i < iHeadEnd); ++i)
        {
          isReachForHead = (i == (uint32_t)rt.limbRigIndex);
        }
        // Not reach for head (or leg or spine) then is reach for arm?
        if (!isReachForHead)
        {
          uint32_t iArmBegin = owner->owner->data->firstArmRigIndex;
          uint32_t iArmEnd = owner->owner->data->firstArmRigIndex + owner->owner->data->numArms;
          for (uint32_t i = iArmBegin; (isReachForArm == false) && (i < iArmEnd); ++i)
          {
            isReachForArm = (i == (uint32_t)rt.limbRigIndex);
          }
          // If reach for root of arm then count this as a reach for spine
          if (isReachForArm && (rt.partIndex == 0))
          {
            isReachForArm = false;
            isReachForSpine = true;
          }
        }
      }
    }
    if (isReachForSpine)     rt.type = ER::LimbTypeEnum::L_spine;
    else if (isReachForHead) rt.type = ER::LimbTypeEnum::L_head;
    else if (isReachForLeg)  rt.type = ER::LimbTypeEnum::L_leg;
    else if (isReachForArm)  rt.type = ER::LimbTypeEnum::L_arm;
    else                     rt.type = ER::LimbTypeEnum::L_unknown;
  }
  return newTarget;
}

//----------------------------------------------------------------------------------------------------------------------
void ReachForBody::entry()
{
  for (uint32_t i = 0 ; i < sizeof(data->endEffectorTMs) / sizeof(data->endEffectorTMs[0]) ; ++i)
  {
    data->endEffectorTMs[i].identity();
  }
  for (uint32_t i = 0 ; i < sizeof(data->isReaching) / sizeof(data->isReaching[0]) ; ++i)
  {
    data->isReaching[i] = false;
  }
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

