// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "euphoria/erHitReaction.h"
#include "euphoria/erHitUtils.h"
#include "euphoria/erBody.h"
#include "euphoria/erLimb.h"
#include "euphoria/erCharacter.h"
#include "NMGeomUtils/NMGeomUtils.h"

//----------------------------------------------------------------------------------------------------------------------
namespace ER
{

//----------------------------------------------------------------------------------------------------------------------
void HitReaction::init()
{
  m_hsiLog.clear();

  m_standingStillTimeout = 0.02f;
  m_recoveredTimeout = 0.05f;
  m_fallenAliveTimeout = 0.05f;
  m_fallenDeadTimeout = 0.05f;
  m_preFallTargetStepVelocity = 1.0f;
  m_fallTargetStepVelocity = 3.0f;
  m_fallBodyImpulseMagnitude = 1.0f;
  m_prioritiseReaches = true;

  reset();
}

//----------------------------------------------------------------------------------------------------------------------
void HitReaction::reset()
{
  // Clear the log
  if (m_hsiLog.size > 0)
  {
    m_hsiLog.clear();
  }
  // Clear reach records
  m_reachRecords[0] = m_reachRecords[1] = -1;

  m_haveNewImpulse = false;
  m_haveNewReach[0] = m_haveNewReach[1] = false;
  m_statusCode = HitReaction::RUNNING;

  m_dying = false;
  m_deathTrigger = false;
  m_inDeathRelax = false;
  m_deathTimer = 0.0f;

  m_forceFall = false;
  m_forceFallTriggered = false;
  m_fallStepTimeLimit = FLT_MAX;
  m_fallStepCountLimit = 0xFFFF;

  m_recoveredTimer = 0.0f;

  // Reset values on these exit condition timers are < 0 so we can tell if they are being driven from inputs
  // (and switch logic appropriately).
  //
  m_standingStillTime = -1.0f;
  m_fallenTime = -1.0f;
  m_steppingTime = -1.0f;
  m_stepCount = -1;

  m_lookAtWoundOrHitSource = true;
  m_latestHSI.invalidate();
}

//----------------------------------------------------------------------------------------------------------------------
static void defaultBalanceInfo(ER::BalanceInfo& bi)
{
  bi.m_targetStepVelocity.setToZero();
  bi.m_bodyStrength = 1.0f;
  bi.m_assistanceOrientation = bi.m_assistancePosition = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
static void defaultLookInfo(ER::LookInfo& info)
{
  info.zero();
}

//----------------------------------------------------------------------------------------------------------------------
// Returns an approximate (see note below) set of "limb tm" coordinates for the posWorld input.
// The "limb tm" frame is a blend of the root and end frames biased according to where the projection of posWorld lies
// on the line joining them.
//
// Note: this approximation is used to determine the location of a hit relative to the spine and head limbs. Implicitly
// it relies on the assumption that the line joining the root and end of the limb lie wholely within the collision
// volumes of which it is comprised in other words, that the curvature of the limb is not excessive as compared with
// the parts it is made up of. In case of problems (particularly with front/back determination) this would be an 
// assumption to check.
//
static NMP::Vector3 getLimbLocalPosition(
  const NMP::Vector3& posWorld,
  const Body* body, 
  int32_t limbIndex,
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugInterface))
{
  // Calculate the projection of posWorld onto the line joining root and end tm's.
  //
  const ER::Limb& limb = body->getLimb(limbIndex);
  NMP::Matrix34 limbEndTM(limb.getEndTransform());
  NMP::Matrix34 limbRootTM(limb.getRootTransform());
  float positionOnLine;
  NMRU::GeomUtils::getPointProjectedOntoLine(
    limbRootTM.translation(), limbEndTM.translation(), posWorld, &positionOnLine);
  // Restrict s to [0,1] for a resulting limbTM in the "interval" [rootTM, endTM].
  positionOnLine = NMP::clampValue(positionOnLine, 0.0f, 1.0f);

  // Calculate the "limb tm" (and associated inverse).
  //
  NMP::Matrix34 limbTM;
  limbTM.interpolate(limbRootTM, limbEndTM, positionOnLine);
  NMP::Matrix34 limbTMInv(limbTM);
  limbTMInv.invertFast();

#if defined(MR_OUTPUT_DEBUGGING)
  if(pDebugInterface)
  {
    ER::DimensionalScaling scaling = body->getDimensionalScaling();
    float matrixScale = scaling.scaleDist(1.0f);
    MR_DEBUG_DRAW_MATRIX(pDebugInterface, limbRootTM, matrixScale);
    MR_DEBUG_DRAW_MATRIX(pDebugInterface, limbEndTM, matrixScale);
    MR_DEBUG_DRAW_MATRIX(pDebugInterface, limbTM, matrixScale);
  }
#endif

  // Transform posWorld to "limb tm" coordinates.
  //
  return limbTMInv.getTransformedVector(posWorld);
}

//----------------------------------------------------------------------------------------------------------------------
static ER::LimbTypeEnum::Type getHSILimbType(
  const ER::Body* body,
  const ER::HitSelectionInfo& hsi)
{
  if (0 <= hsi.limbIndex && hsi.limbIndex < body->getNumLimbs())
  {
    return body->getLimb(hsi.limbIndex).getType();
  }
  return ER::LimbTypeEnum::L_unknown;
}

//----------------------------------------------------------------------------------------------------------------------
// sets the field of the hsi to indicate "anatomical type" of the part that got hit
//
ER::BodyPartTypeEnum::Type HitReaction::determineHSIPartType(
  const ER::HitSelectionInfo& hsi,
  MR::InstanceDebugInterface* pDebugDrawInst)
{
  // do nothing if we don't have a character or if the limb or part indexes are not valid
  if ((m_character == NULL) || (hsi.limbIndex == -1 || hsi.partIndex == -1))
  {
    return ER::BodyPartTypeEnum::NumPartTypes;
  }

  const int limbRigIndex = hsi.limbIndex;
  const int partIndex = hsi.partIndex;
  NMP_ASSERT(0 <= hsi.limbIndex && hsi.limbIndex < m_character->getBody().getNumLimbs());
  const ER::LimbTypeEnum::Type limbType = m_character->getBody().getLimb(hsi.limbIndex).getType();

  // Translate this limb/part pair into an "anatomical" type eg. spine, head, leg
  //
  if (limbType != ER::LimbTypeEnum::L_unknown)
  {
    const ER::Body& body = m_character->getBody();
    // now depending on what limb we have get the rest of the details
    switch (limbType)
    {
    case ER::LimbTypeEnum::L_spine:
    {
      NMP::Vector3 xWorld;
      const physx::PxActor* selectedActor = (const physx::PxActor*)hsi.selectedActor;
      MR::actorToWorldPoint((physx::PxActor*)selectedActor, hsi.pointLocal, xWorld);
      NMP::Vector3 hitPosSpine = getLimbLocalPosition(
        xWorld,
        &body,
        body.m_definition->getFirstSpineLimbIndex(), pDebugDrawInst);

      // Determines front back on the basis of the result from getLimbLocalPosition(). In case of problems see note 
      // above regarding assumptions about curvature on the spine limb.
      bool hitBack = (hitPosSpine.x <= 0.0f);

      if (partIndex == 0)
      {
        return hitBack ? ER::BodyPartTypeEnum::PelvisBack :
          ER::BodyPartTypeEnum::PelvisFront;
      }
      else if (partIndex == (int)body.getLimb(limbRigIndex).getTotalNumParts() - 1)
      {
        return hitBack ? ER::BodyPartTypeEnum::ChestBack :
          ER::BodyPartTypeEnum::ChestFront;
      }
      else
      {
        return hitBack ? ER::BodyPartTypeEnum::TorsoBack :
          ER::BodyPartTypeEnum::TorsoFront;
      }
    }
    case ER::LimbTypeEnum::L_head:
      if (partIndex == (int)body.getLimb(limbRigIndex).getTotalNumParts() - 1)
      {
        return ER::BodyPartTypeEnum::Head;
      }
      else
      {
        return ER::BodyPartTypeEnum::Neck;
      }
    case ER::LimbTypeEnum::L_arm:
      if (partIndex == (int)body.getLimb(limbRigIndex).getBaseIndex())
      {
        return ER::BodyPartTypeEnum::Shoulder;
      }
      else if (partIndex == (int)body.getLimb(limbRigIndex).getTotalNumParts() - 1)
      {
        return ER::BodyPartTypeEnum::Hand;
      }
      else
      {
        return ER::BodyPartTypeEnum::Arm;
      }
    case ER::LimbTypeEnum::L_leg:
      if (partIndex == (int)body.getLimb(limbRigIndex).getTotalNumParts() - 1)
      {
        return ER::BodyPartTypeEnum::Foot;
      }
      else
      {
        return ER::BodyPartTypeEnum::Leg;
      }
    default:
      return ER::BodyPartTypeEnum::NumPartTypes;
    }
  }
  return ER::BodyPartTypeEnum::NumPartTypes;
}

//----------------------------------------------------------------------------------------------------------------------
void HitReaction::updateStatus(float dt, float& bodyStrength)
{
  // Check for death trigger by head hit (to remove)
  //

  if (m_deathTriggerOnHeadhit && m_hsiLog.size > 0)
  {
    ER::HitSelectionInfo& hsi2 = m_hsiLog.HSIs[0];
    if (hsi2.elapsedTime > 0.0f) // we'll only have a valid parttype when elapsed time is non-zero
    {
      switch (hsi2.partType)
      {
      case ER::BodyPartTypeEnum::Head:
      case ER::BodyPartTypeEnum::Neck:
        m_deathTrigger = true;
        break;
      default:
        break;
      }
    }
  }

  // Normal running if we are still processsing hsi inputs
  if(m_hsiLog.size > 0)
  {
    m_statusCode = HitReaction::RUNNING;
  }

  // Death
  //
  // Death delay is what was specified provided the character has not fallen over
  const float deathDelay = (m_fallenTime < m_fallenAliveTimeout) ? m_attribs.m_deathTriggerDelay : 0.0f;
  const float deathRelaxDuration = m_attribs.m_deathRelaxDuration;

  if (m_hsiLog.size > 0 && (m_dying || m_deathTrigger))
  {
    // Not dying yet so set dying flag and zero death timer
    if (!m_dying)
    {
      m_deathTimer = 0.0f;
      m_dying = true;
    }
    // Dying
    else
    {
      m_deathTimer += dt;

      // Delay before death ramp is still in force, so ensure death relax flag is not set
      if (m_deathTimer < deathDelay)
      {
        m_inDeathRelax = false;
      }
      // Death ramp: set death relax flag and manage rampdown
      else
      {
        m_inDeathRelax = true;
        const float epsilon = 0.001f; // numerical fuzz threshold
        float deathRelaxStrength = 1.0f -
          NMP::clampValue((m_deathTimer - deathDelay) / (deathRelaxDuration + epsilon), 0.0f, 1.0f);

        // apply relax strength to balance output, network author can decide what to do with this
        // when we hit the node interface the output should be named more generally eg. "bodyStrength"
        bodyStrength = deathRelaxStrength;

        // If rampdown is complete a flag status (for transition) and return since nothing further to do.
        if (deathRelaxStrength < epsilon)
        {
          // Exit if the character has died i.e. that the death timer is greater than the sum of the corresponding timeouts.
          if (m_deathTimer > (deathDelay + deathRelaxDuration + m_fallenDeadTimeout))
          {
            m_statusCode = HitReaction::DEAD;
            return;
          }
        }
      }
    }
  }
  // Fallen (not dead): set status but do not return, continue eg. reaching/looking and processing new hits.
  //
  else if (m_fallenTime > m_fallenAliveTimeout)
  {
    m_statusCode = HitReaction::FALLEN;
  }
  // Full recovery (last hit expired but character is not dead and not fallen)
  //
  else if (m_hsiLog.size == 0 && !m_dying && !(m_fallenTime > 0.0f))
  {
    // If there is standing still time input, delay recovery, otherwise, or if standing still time is not increasing 
    // and there is no fallen time input, do it now.
    //
    if ((m_standingStillTime > m_standingStillTimeout) || (m_standingStillTime < 0.0f) 
      || ( m_standingStillTime == 0.0f && m_fallenTime < 0.0f )
      )
    {
      m_recoveredTimer += dt;
    }
    else
    {
      m_recoveredTimer = 0.0f;
    }

    // Recovered: set status
    if (m_recoveredTimer > m_recoveredTimeout)
    {
      m_statusCode = HitReaction::RECOVERED;
      m_latestHSI.invalidate();
    }
  }
}


//----------------------------------------------------------------------------------------------------------------------

void HitReaction::updateNewImpulse()
{
  // Store the impulse hsi (the latest that came in - not necessarily the highest priority).
  m_impulseHitSelectionInfo = m_latestHSI;

  // Collate impulse modifiers from the associated (latest hit) attribs.
  m_impulseInfo.m_partImpulseMagnitude = m_latestHSIAttribs.m_partImpactMagnitude;
  m_impulseInfo.m_partResponseRatio = m_latestHSIAttribs.m_partResponseRatio;
  m_impulseInfo.m_bodyImpulseMagnitude = m_latestHSIAttribs.m_bodyImpactMagnitude;
  m_impulseInfo.m_bodyResponseRatio = m_latestHSIAttribs.m_bodyResponseRatio;
  m_impulseInfo.m_torqueMultiplier = m_latestHSIAttribs.m_torqueMultiplier;
  m_impulseInfo.m_liftResponseRatio = m_latestHSIAttribs.m_liftResponseRatio;

  // Calculate lift boost required to meet the desired minimum lift
  //
  NMP::Vector3 bodyImpulse = m_latestHSI.hitDirWorld * m_impulseInfo.m_bodyImpulseMagnitude;
  NMP::Vector3 up (-m_gravity);
  up.fastNormalise();
  float liftToAdd = m_latestHSIAttribs.m_desiredMinimumLift - bodyImpulse.dot(up);

  // Fill out remaining impulse info.
  m_impulseInfo.m_liftBoost = NMP::maximum(liftToAdd, 0.0f); // Only add +ve lift to attain minimum desired.
  m_impulseInfo.m_bodyImpulseDirection = bodyImpulse;
  m_impulseInfo.m_bodyImpulseMagnitude = m_impulseInfo.m_bodyImpulseDirection.normaliseGetLength();
  m_haveNewImpulse = true;
}

//----------------------------------------------------------------------------------------------------------------------
// returns true if a new hit was logged
bool HitReaction::updateHitRecords(
  bool newHitInput,
  float dt, 
  HitSelectionInfo& hsi, ER::HitAttribs& attribs,
  MR::InstanceDebugInterface* pDebugDrawInst)
{
  // History: (handling hits from previous frames).
  if (m_hsiLog.size > 0)
  {
    // Step timers on all records.
    m_hsiLog.updateTimers(dt);
    m_latestHSI.elapsedTime += dt;

    // If we have an impulse in the pipeline increment timer - the impulse is applied (and the flag reset) only once the
    // timer is non-zero. This avoids problems with part determination code running on impulse-deformed euphoria
    // character
    //
    if (m_haveNewImpulse)
    {
      m_impulseHitSelectionInfo.elapsedTime += dt;
    }

    // Determine part type on the highest priority hsi if it's more than a frame old and hasn't been determined yet.
    //
    ER::HitSelectionInfo& hsi2 = m_hsiLog.HSIs[0];
    if (hsi2.elapsedTime > 0.0f && hsi2.partType == ER::BodyPartTypeEnum::NumPartTypes)
    {
      hsi2.partType = determineHSIPartType(hsi2, pDebugDrawInst);
    }
  }

  // Current: (handling a new hit)
  int32_t hsiLogIndex = -1;
  if (newHitInput)
  {
    // Identify and register the new hit - or drop it if unidentifiable
    //
    hsi.limbType = getHSILimbType(&m_character->getBody(), hsi);
    if (hsi.limbType != ER::LimbTypeEnum::L_unknown)
    {
      // Reset timer prior to logging
      hsi.elapsedTime = 0.0f;
      // Temporarily mark the part type as unknown (to be determined later as part of reach reflex logic)
      // (note this is done a frame later to ensure that the euphoria character parts are in place eg. when transition
      // to this state is from a non-euphoria state)
      hsi.partType = ER::BodyPartTypeEnum::NumPartTypes;
      hsiLogIndex = m_hsiLog.logHSI(hsi);
      // Record latest and it's attribs
      m_latestHSI = m_hsiLog.HSIs[hsiLogIndex];
      m_latestHSIAttribs = attribs;
      m_haveNewHit = true;
    }
    else
    {
      m_haveNewHit = false;
    }
  }
  else
  {
    m_haveNewHit = false;
  }

  // Set attribs for highest priority hit
  //
  // HSI's are logged in priority order so a log index of zero means that the new hit is the current highest priority.
  bool highestPriorityHit = m_haveNewHit && (hsiLogIndex == 0);
  if (highestPriorityHit)
  {
    m_attribs = attribs;
  }

  return highestPriorityHit;
}

//----------------------------------------------------------------------------------------------------------------------
void HitReaction::updateSpineAndLegStrengths(float& spineDamping, float *legStrengths)
{
  // Accumulate current leg hit strength reductions over all current leg hits
  //
  // Reset temporary accumulators
  float legStrengthReductions[s_numLegs];
  for (int32_t i = 0; i < s_numLegs; ++i)
  {
    legStrengthReductions[i] = 0.0f;
  }
  // Sum as appropriate
  for (int32_t i = 0; i < m_hsiLog.size; ++i)
  {
    //  if it was a leg hit and the hit elapsed time < leg hurt duration
    bool leftLegHit = m_hsiLog.HSIs[i].limbIndex == m_leftLegIndex;
    bool rightLegHit = m_hsiLog.HSIs[i].limbIndex == m_rightLegIndex;

    // only applicable for leg hits and only if within duration
    if (leftLegHit || rightLegHit)
    {
      float hitLegStrengthReduction =
        (m_hsiLog.HSIs[i].elapsedTime < m_hsiLog.HSIs[i].legStrengthReductionDuration) ?
        m_hsiLog.HSIs[i].legStrengthReduction : 0.0f;

      legStrengthReductions[leftLegHit ? 0:1] += hitLegStrengthReduction;
    }
  }
  // Apply sums (within working range [0, 1])
  for (int32_t i = 0; i < s_numLegs; ++i)
  {
    legStrengths[i] = NMP::clampValue(1.0f - legStrengthReductions[i], 0.0f, 1.0f);
  }

  // Spine yield to impulse
  //

  // Assume default damping (and modify spine below as required)
  //
  float minSpineDamping = 1.0;
  for (int32_t j = 0; j < m_hsiLog.size; ++j)
  {
    ER::HitSelectionInfo& hsi2 = m_hsiLog.HSIs[j];
    if (hsi2.elapsedTime < m_attribs.m_impulseYieldDuration)
    {
      float sd = (1.0f - m_attribs.m_impulseYield);
      if (sd < minSpineDamping)
        minSpineDamping = sd;
    }
  }
  spineDamping = minSpineDamping;
}

//----------------------------------------------------------------------------------------------------------------------
void HitReaction::updateReaching( bool newReach, const ER::DimensionalScaling& scalingSource)
{
  // We assume no new reaches to process and no current reaching in progress and "wipe down" relevant state.
  // These conditions are verified and corresponding state built up below.
  //
  for (int32_t arm = 0; arm < s_numArms; ++arm)
  {
    // Reset "new reach" flags.
    m_haveNewReach[arm] = false;

    // Reset "reach info" state used to produce reach strength control outputs.
    m_reachInfo[arm].m_strength = 0.0f;
    m_reachInfo[arm].m_speedLimit = 0.0f;
  }

  // If reaching is enabled..
  if (m_attribs.m_reachSwitch == true)
  {
    // Terminate active reach if we have a new highest priority input
    // or the associated hsi has been dropped from the log
    //
    for (int32_t i = 0; i < s_numArms; ++i)
    {
      if (m_reachRecords[i] > 0)
      {
        if (newReach || (m_hsiLog.find(m_reachRecords[i]) == -1))
        {
          m_reachInfo[i].m_strength = 0.0f;
          m_reachRecords[i] = -1;
        }
      }
    }

    // Maintain any current reaching
    //
    // determine which arms are currently reaching
    bool reaching = false;
    int32_t reachingHSILogIndex[s_numArms];
    for (int32_t i = 0; i < s_numArms; ++i)
    {
      reachingHSILogIndex[i] = m_hsiLog.find(m_reachRecords[i]);
      if (reachingHSILogIndex[i] != -1)
      {
        reaching = true;
      }
    }
    if (reaching)
    {
      for (int32_t arm = 0; arm < s_numArms; ++arm)
      {
        if (reachingHSILogIndex[arm] != -1)
        {
          HitSelectionInfo& hsiCurrentReach = m_hsiLog.HSIs[reachingHSILogIndex[arm]];
          float reachTime = hsiCurrentReach.elapsedTime;
          float duration = m_attribs.m_reachDuration;
          float rampDuration = 0.0f; // instant ramp to full
           // the lesser of 1/2 second or half the specified duration
          float rampDownDuration = NMP::minimum(duration * 0.5f, scalingSource.scaleTime(0.5f));
          float fullStrengthDuration = duration - rampDownDuration;
          float maxReachStrength = 1.0f;

          float reachStrength = ER::calcRampedWeight(reachTime,
            m_attribs.m_reachDelay,
            maxReachStrength, // saturation strength
            rampDuration, // ramp duration
            fullStrengthDuration, // time at full strength
            rampDownDuration  // deramp duration
            );

          m_reachInfo[arm].m_strength = reachStrength;
          m_reachInfo[arm].m_speedLimit = m_attribs.m_maxReachSpeed;
        }
      }
    }
    else
    {
      // Decide whether the current highest priority hit is one we'd want to reach for
      // (some body reaches are either unattainable or just look awkward, the switch
      //  statement below lists the low risk ones)
      // Also note, part type determination is not attempted on a new hsi in case
      // the euphoria character parts have not been placed yet.
      //
      bool hitIsReachable;
      ER::HitSelectionInfo& hsiHighestPriority = m_hsiLog.HSIs[0];
      switch (hsiHighestPriority.partType)
      {
      case ER::BodyPartTypeEnum::ChestFront:
      case ER::BodyPartTypeEnum::TorsoFront:
      case ER::BodyPartTypeEnum::TorsoBack:
      case ER::BodyPartTypeEnum::PelvisFront:
      case ER::BodyPartTypeEnum::PelvisBack:
      case ER::BodyPartTypeEnum::Head:
      case ER::BodyPartTypeEnum::Neck:
      case ER::BodyPartTypeEnum::Shoulder:
      case ER::BodyPartTypeEnum::Arm:
      case ER::BodyPartTypeEnum::Hand:
      case ER::BodyPartTypeEnum::Leg:
        hitIsReachable = true;
        break;
      default:
        hitIsReachable = false;
        break;
      }
      if (hitIsReachable)
      {
        // Initiate a new reach if (we have a reach target, it's old enough and we're not dying)
        //
        float reachDelay = m_attribs.m_reachDelay;
        bool newReachTarget =
          (m_hsiLog.size > 0)                             // have a potential reach target
          && (m_hsiLog.HSIs[0].elapsedTime > reachDelay)  // old enough to reach for
          && (m_inDeathRelax == false);                   // not dying
        if (newReachTarget)
        {
          // Read message attrib to determine whether to reach with:
          // - a particular arm (left or right)
          // - both arms (left and right)
          // - one arm (left or right heuristically determined)
          //
          NMP_ASSERT(ER::BODY_REACH_TYPE_REACH_WITH_LEFT <= m_attribs.m_reachSku && m_attribs.m_reachSku < ER::BODY_REACH_TYPE_NUM_TYPES)
          int32_t reachtype = m_attribs.m_reachSku;

          // Special case for arm reaches: force the operator to treat this as a two armed reach so that strength
          // outputs are both updated this is useful status feedback back in the morpheme network.
          if (hsiHighestPriority.partType == BodyPartTypeEnum::Arm)
          {
            reachtype = ER::BODY_REACH_TYPE_REACH_FOR_ARM;
          }
          
          // Reach with the specified arm
          //
          if (reachtype == ER::BODY_REACH_TYPE_REACH_WITH_LEFT || reachtype == ER::BODY_REACH_TYPE_REACH_WITH_RIGHT)
          {
            int32_t armToReachWith = ((reachtype == ER::BODY_REACH_TYPE_REACH_WITH_LEFT) 
              ? m_leftArmIndex : m_rightArmIndex) - m_firstArmIndex;
            m_haveNewReach[armToReachWith] = true;
            m_reachHitSelectionInfo[armToReachWith] = hsiHighestPriority;
            m_reachInfo[armToReachWith].m_strength = 1.0f;
            m_reachInfo[armToReachWith].m_speedLimit = m_attribs.m_maxReachSpeed;
            m_reachRecords[armToReachWith] = hsiHighestPriority.id;
          }
          // Both arms are involved.
          //
          else if ((reachtype == ER::BODY_REACH_TYPE_REACH_WITH_RIGHT_AND_LEFT ) || (reachtype == ER::BODY_REACH_TYPE_REACH_FOR_ARM))
          {
            for (int32_t armToReachWith = 0; armToReachWith < 2; ++armToReachWith)
            {
              m_haveNewReach[armToReachWith] = true;
              m_reachHitSelectionInfo[armToReachWith] = hsiHighestPriority;
              m_reachInfo[armToReachWith].m_strength = 1.0f;
              m_reachInfo[armToReachWith].m_speedLimit = m_attribs.m_maxReachSpeed;
              m_reachRecords[armToReachWith] = hsiHighestPriority.id;
            }
          }
          // Reach with one arm, heuristically determine which depending on what/where the character was hit.
          // Note: implementation assumes one left and one right arm and leg.
          //
          else
          {
            NMP_ASSERT(reachtype == ER::BODY_REACH_TYPE_HEURISTIC);

            int32_t reachArmIndex = -1;
            switch (hsiHighestPriority.partType)
            {
              // Cases for opposite side arm reaches.
              //
            case ER::BodyPartTypeEnum::ChestFront:
              {
                const physx::PxActor* selectedActor = (const physx::PxActor*)hsiHighestPriority.selectedActor;
                NMP::Vector3 xWorld;
                MR::actorToWorldPoint((physx::PxActor*)selectedActor, hsiHighestPriority.pointLocal, xWorld);
                NMP::Vector3 posLimb =
                  getLimbLocalPosition(xWorld, &m_character->getBody(), hsiHighestPriority.limbIndex, 0);
                reachArmIndex = posLimb.z > 0.0f ? m_leftArmIndex : m_rightArmIndex;
              }
              break;

            case ER::BodyPartTypeEnum::Arm:
            case ER::BodyPartTypeEnum::Shoulder:
            case ER::BodyPartTypeEnum::Hand:
              reachArmIndex =
                m_character->getBody().getLimb(hsiHighestPriority.limbIndex).isLeftLimb() ?
                  m_rightArmIndex : m_leftArmIndex;
              break;

              // Cases for same side arm reaches.
              //
            case ER::BodyPartTypeEnum::TorsoFront:
            case ER::BodyPartTypeEnum::TorsoBack:
            case ER::BodyPartTypeEnum::PelvisFront:
            case ER::BodyPartTypeEnum::PelvisBack:
            case ER::BodyPartTypeEnum::Head:
            case ER::BodyPartTypeEnum::Neck:
              {
                const physx::PxActor* selectedActor = (const physx::PxActor*)hsiHighestPriority.selectedActor;
                NMP::Vector3 xWorld;
                MR::actorToWorldPoint((physx::PxActor*)selectedActor, hsiHighestPriority.pointLocal, xWorld);
                NMP::Vector3 posLimb = getLimbLocalPosition(xWorld, &m_character->getBody(), hsiHighestPriority.limbIndex, 0);
                reachArmIndex = posLimb.z > 0.0f ? m_rightArmIndex : m_leftArmIndex;
              }
              break;
            case ER::BodyPartTypeEnum::Leg:
              reachArmIndex =
                m_character->getBody().getLimb(hsiHighestPriority.limbIndex).isLeftLimb() ?
                  m_leftArmIndex : m_rightArmIndex;
              break;

              // Not handled.
            default:
              break;
            }

            // Having decided which arm should reach, setup the reaching.
            //
            if (reachArmIndex != -1)
            {
              int32_t armToReachWith = reachArmIndex % 2;
              m_haveNewReach[armToReachWith] = true;
              m_reachHitSelectionInfo[armToReachWith] = hsiHighestPriority;
              m_reachInfo[armToReachWith].m_strength = 1.0f;
              m_reachInfo[armToReachWith].m_speedLimit = m_attribs.m_maxReachSpeed;
              m_reachRecords[armToReachWith] = hsiHighestPriority.id;
            }
          }
        }
      }
    }
  }
  else // Reaching is disabled - make sure of it
  {
    // Terminate any current reaching
    for (int32_t i = 0; i < s_numArms; ++i)
    {
      if (m_reachRecords[i] > 0)
      {
        m_reachInfo[i].m_strength = 0.0f;
        m_reachRecords[i] = -1;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Set demands for balance assistance and target step velocity.
//
void HitReaction::updateBalance(ER::BalanceInfo& balanceInfo)
{

  // Select hsi and associated attribs to refer to for step request:
  //
  ER::HitSelectionInfo* hsi = 0;
  HitAttribs* attribs = 0;
  if (m_dying)                    //  - from highest priority hit
  {
    hsi = &m_hsiLog.HSIs[0];
    attribs = &m_attribs;
  }
  else if (                       //  - or from latest hit.
    m_latestHSI.isBodyHit() && m_latestHSI.elapsedTime < m_latestHSIAttribs.m_impulseTargetStepDuration )
  {
    hsi = &m_latestHSI;
    attribs = &m_latestHSIAttribs;
  }
  
  // Target step direction and speed.
  //
  if (hsi)
  {
    NMP::Vector3 targetStepDirection(hsi->hitDirWorld);
    if (!attribs->m_impulseDirWorldOrLocal)
    {
      MR::actorToWorldNormal((const physx::PxActor*)hsi->selectedActor, hsi->hitDirLocal, targetStepDirection);
    }
    if (m_dying)
    {
      // Take target step speed as a combination of the impulse and death target speeds
      float w = (m_deathTimer >= attribs->m_deathTriggerDelay) ? 0.0f: (1.0f - m_deathTimer/attribs->m_deathTriggerDelay);
      float targetStepSpeed = attribs->m_impulseTargetStepSpeed * w + attribs->m_deathTargetStepSpeed * (1.0f - w);
      balanceInfo.m_targetStepVelocity = targetStepDirection * targetStepSpeed;
      // apply specified balancer strength if not in death relax
      if (!m_inDeathRelax)
      {
        balanceInfo.m_bodyStrength = attribs->m_deathBalanceStrength;
      }
    }
    else
    {
      // Target velocity ramp (full on for half the specified duration, ramping to zero over the remaining time).
      //
      float fullStrengthDuration = 0.5f * m_latestHSIAttribs.m_impulseTargetStepDuration;
      float derampDuration = 0.5f * m_latestHSIAttribs.m_impulseTargetStepDuration;
      float targetStepSpeedMultiplier = ER::calcRampedWeight(
        hsi->elapsedTime,
        0.0f,                 // delay
        1.0f,                 // saturation strength
        0.0f,                 // ramp duration
        fullStrengthDuration, // time at full strength
        derampDuration        // deramp duration
        );
      float targetStepSpeed = attribs->m_impulseTargetStepSpeed * targetStepSpeedMultiplier;

      // Result ready for output
      balanceInfo.m_targetStepVelocity = targetStepDirection * targetStepSpeed;
      balanceInfo.m_bodyStrength = 1.0f;
    }
  }
  else
  {
    defaultBalanceInfo(balanceInfo);
  }

  // Balance Assistance
  //

  // We potentially need balance assistance to outlive the processing of any given hit.
  // This is to cover the time between hit processing ending and full recovery which is
  // deemed to have happened, not once the hit has expired but once the balancer is suitably
  // settled. Hence the following logic:

  // If we're not dying..
  // and we have a previous known hit in memory and running (i.e. not fallen, recovered, dead
  // or just waiting for a first hit to come in).
  // Then take the balance assistance value from the last known hit.
  if ( !m_dying && !m_latestHSI.isInvalid() && m_statusCode == HitReaction::RUNNING)
  {
    balanceInfo.m_assistanceOrientation = m_attribs.m_balanceAssistance;
    balanceInfo.m_assistancePosition = m_attribs.m_balanceAssistance;
  }
  // Otherwise, (i.e. fallen, dead, recovered or just waiting) set no balance assistance.
  else
  {
    balanceInfo.m_assistanceOrientation = 0.0f;
    balanceInfo.m_assistancePosition = 0.0f;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void HitReaction::updateLook(ER::LookInfo& lookInfo, const ER::DimensionalScaling& dimensionalScaling)
{
  // Default to no look settings
  defaultLookInfo(lookInfo);
  
  // but if "look reflex" is enabled, look at either wound or hit source
  //
  if (m_attribs.m_reflexLookSwitch == true)
  {
    // Very simple look "reflex" - if we have a highest priority target then look at it.
    //
    if (m_hsiLog.size > 0)
    {
      ER::HitSelectionInfo& hsi2 = m_hsiLog.HSIs[0];

      bool canLookAtWound = false;
      if (hsi2.elapsedTime > 0.0f) // have valid part-type at this point
      {
        // We can only look at the following part-types
        // (trying for eg. to look at a wound on the back may
        // cause oscillations for the head-look behaviour)
        //
        switch (hsi2.partType)
        {
        case ER::BodyPartTypeEnum::ChestFront:
        case ER::BodyPartTypeEnum::TorsoFront:
        case ER::BodyPartTypeEnum::PelvisFront:
        case ER::BodyPartTypeEnum::Shoulder:
        case ER::BodyPartTypeEnum::Leg:
        case ER::BodyPartTypeEnum::Arm:
        case ER::BodyPartTypeEnum::Hand:
          canLookAtWound = true;
          break;
        default:
          canLookAtWound = false;
          break;
        }
      }

      NMP::Vector3 lookTarget;
      if (m_lookAtWoundOrHitSource && canLookAtWound)
      {
        MR::actorToWorldPoint((const physx::PxActor*)hsi2.selectedActor, hsi2.pointLocal, lookTarget);
      }
      else
      {
        lookTarget = hsi2.sourcePointWorld;
      }

      // Set the look strength as a function of elapsed time and total duration
      //
      float lookDelay = 0.0f;
      float lookMaxStrength = 1.0f;
      float lookRampDuration = 0.0f;
      float lookTotalDuration = m_attribs.m_reflexLookDuration;
      float lookDerampDuration = NMP::minimum(
        lookTotalDuration * 0.5f,
        dimensionalScaling.scaleTime(0.5f));
      float lookFullDuration = lookTotalDuration - lookDerampDuration;
      float lookStrength = ER::calcRampedWeight(
        hsi2.elapsedTime,
        lookDelay,          // delay
        lookMaxStrength,    // saturation strength
        lookRampDuration,   // ramp duration
        lookFullDuration,   // time at full strength
        lookDerampDuration);// deramp duration
      lookInfo = ER::LookInfo(lookTarget, lookStrength);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void HitReaction::updateAnim(ER::AnimSelectionInfo& animSelectionInfo)
{
  // If "animated reflex" is enabled
  //
  if (m_attribs.m_reflexAnimSwitch == true && m_hsiLog.size > 0)
  {
    // Strike a pose with intensity according to ramp value inputs
    //
    float w = ER::calcRampedWeight(m_hsiLog.HSIs[0].elapsedTime,
      m_attribs.m_reflexAnimStart,
      m_attribs.m_reflexAnimMaxWeight,
      m_attribs.m_reflexAnimRampDuration,
      m_attribs.m_reflexAnimFullDuration,
      m_attribs.m_reflexAnimDerampDuration);
    animSelectionInfo = ER::AnimSelectionInfo( m_attribs.m_reflexAnim, w );
  }
  // Not enabled, so do nothing except set a "null" reflex anim info.
  else
  {
    animSelectionInfo = ER::AnimSelectionInfo();
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Calculate and apply joint limit expansion.
//
void HitReaction::updateExpandLimits(MR::PhysicsRig* physicsRig)
{
  NMP_ASSERT(physicsRig);

  // Early out if:
  // - have no physics rig (assert condition)
  // - have no hit in progress
  // - are not running expandLimits functionality
  // - expand limits control is done with
  //
  if( !physicsRig
    || (m_hsiLog.size == 0) 
    || (m_attribs.m_expandLimitsSwitch == false)
    || (m_hsiLog.HSIs[0].elapsedTime > m_attribs.m_expandLimitsFullDuration + m_attribs.m_expandLimitsDerampDuration)
    )
  {
    return;
  }

  // There is a hit being processed and limit scaling underway.
  // Calculate and apply the required joint limit expansion.
  //

  // Compile inputs for ramped weight calculator
  float limitExpansionStart = 0.0f,
    limitExpansionMax = m_attribs.m_expandLimitsMaxExpansion,
    limitExpansionRampDuration = 0.0f,
    limitExpansionFullDuration = m_attribs.m_expandLimitsFullDuration,
    limitExpansionDerampDuration = m_attribs.m_expandLimitsDerampDuration;

  // Calculate limit expansion for the current elapsed hit time given the requested profile.
  const float limitExpansion = ER::calcRampedWeight(m_hsiLog.HSIs[0].elapsedTime,
    limitExpansionStart,
    limitExpansionMax,
    limitExpansionRampDuration,
    limitExpansionFullDuration,
    limitExpansionDerampDuration);

  // Apply any resulting +ve non-zero expansion.
  //
  if (limitExpansion > 0.0f)
  {
    // Index the limbs that get the treatment (head0 and spine0 currently).
    ER::Body& body = m_character->getBody();
    int32_t firstSpineIndex = body.m_definition->getFirstSpineLimbIndex();
    int32_t firstHeadIndex = body.m_definition->getFirstHeadLimbIndex();
    int32_t limbIndexes[] = {firstHeadIndex, firstSpineIndex};
    uint32_t numLimbs = sizeof(limbIndexes) / sizeof(int32_t);

    // Apply the limit scaling for each joint in each limb.
    const float limitScale = 1.0f + limitExpansion;
    for(uint32_t i = 0; i < numLimbs; ++i)
    {
      ER::Limb& limb = body.getLimb(limbIndexes[i]);
      const uint32_t numPhysJoints = limb.getNumJointsInChain();
      for (uint32_t j = 0; j < numPhysJoints; ++j)
      {
        physicsRig->scaleJointLimits(limb.getPhysicsRigJointIndex(j), limitScale);
      }
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
void HitReaction::updateForcedFall(
  ER::BalanceInfo& balanceInfo,
  float* legStrengths,
  float dt,
  const ER::DimensionalScaling& dimensionalScaling)
{
  // Forced falls require either step time or step count input
  if (m_steppingTime < 0 && m_stepCount < 0)
  {
    return;
  }

  // Take stock of what's required viz forced fall when a new hit comes in.
  // The following code will not cancel or postpone a forced fall once requested
  // but allows for the fall to happen sooner (if step time or count is sooner)
  //
  if (m_haveNewHit)
  {
    // If forceFall has been triggered by this hit
    if (m_latestHSIAttribs.m_forcedFalldownSwitch)
    {
      // Set flag if not already been set (i.e. by a previous hit).
      if (!m_forceFall)
      {
        // set flag
        m_forceFall = true;
        m_forceFallTriggered = false;
        m_forceFallLegStrengthDecayTimer = 0.0f;
        // set time and step limits as given taking account of current step time and count
        m_fallStepTimeLimit = m_steppingTime + m_latestHSIAttribs.m_targetTimeBeforeFalldown;
        m_fallStepCountLimit = m_stepCount + m_latestHSIAttribs.m_targetNumStepsBeforeFalldown;
      }
      // Update time/step count limits if flag was already set but the fall should happen sooner
      else
      {
        // Latest time or count limits override previous if more urgent
        float stepTimeLimitLatest = m_steppingTime + m_latestHSIAttribs.m_targetTimeBeforeFalldown;
        if (stepTimeLimitLatest < m_fallStepTimeLimit)
          m_fallStepTimeLimit = stepTimeLimitLatest;

        int32_t stepCountLimitLatest =
          m_stepCount + m_latestHSIAttribs.m_targetNumStepsBeforeFalldown;

        if (stepCountLimitLatest < m_fallStepCountLimit)
          m_fallStepCountLimit = stepCountLimitLatest;
      }

      // Store the fall direction as it may outlive the hsi record
      // but update it to reflect the latest hit
      m_forceFallDirection = m_latestHSI.hitDirWorld;
      if (m_latestHSIAttribs.m_impulseDirWorldOrLocal)
      {
        MR::actorToWorldNormal(
          (const physx::PxActor*)m_latestHSI.selectedActor,
          m_latestHSI.hitDirLocal,
          m_forceFallDirection);
      }
    }
  }
  if (m_forceFall)
  {
    // Trigger the fall when either of the (step count or time) limits has been reached.
    if ((m_steppingTime >= m_fallStepTimeLimit) || (m_stepCount >= m_fallStepCountLimit))
    {
      // Cut all balance assistance.
      balanceInfo.m_assistanceOrientation = 0.0f;
      balanceInfo.m_assistancePosition = 0.0f;

      // Increase the balance target step velocity in the fall direction.
      float signOfStagger = m_attribs.m_impulseTargetStepSpeed > 0.0f ? 1.0f:-1.0f;
      balanceInfo.m_targetStepVelocity = m_forceFallDirection * m_fallTargetStepVelocity * signOfStagger;

      // Apply a one-time body impulse or just update the strength decay timer
      //
      if (!m_forceFallTriggered)
      {
        // If there is no impulse pending (from a hit) then this is the only impulse to setup.
        if (!m_haveNewImpulse)
        {
          // Make a simple no frills body impulse
          m_impulseInfo.m_partImpulseMagnitude = 0.0f;
          m_impulseInfo.m_partResponseRatio = 0.0f;
          m_impulseInfo.m_torqueMultiplier = 0.0f;
          m_impulseInfo.m_liftBoost = 0.0f;
          m_impulseInfo.m_bodyImpulseMagnitude = m_fallBodyImpulseMagnitude;
          m_impulseInfo.m_bodyResponseRatio = 1.0f;
          m_impulseInfo.m_bodyImpulseDirection = m_forceFallDirection;
          m_haveNewImpulse = true;
        }
        // Otherwise add the "fall impulse" to the current body hit impulse.
        else
        {
          NMP::Vector3 totalBodyImpulse = 
            m_fallBodyImpulseMagnitude * m_forceFallDirection
            + m_impulseInfo.m_bodyImpulseMagnitude * m_impulseInfo.m_bodyImpulseDirection;
          m_impulseInfo.m_bodyImpulseDirection = totalBodyImpulse;
          m_impulseInfo.m_bodyImpulseMagnitude = m_impulseInfo.m_bodyImpulseDirection.normaliseGetLength();
        }
        m_forceFallTriggered = true;
      }
      else
      {
        m_forceFallLegStrengthDecayTimer += dt;
      }

      // Use the strength decay timer to exponentially degrade leg strength, we aim to go from full to zero in 
      // (time scaled) 0.5 seconds on the standard character
      //
      const float timeToZeroStrength = dimensionalScaling.scaleTime(0.5f);
      float forceFallStrength = 1.0f;
      if (m_forceFallLegStrengthDecayTimer < timeToZeroStrength)
      {
        NMP::smoothExponential(forceFallStrength, m_forceFallLegStrengthDecayTimer, 0.0f, timeToZeroStrength);
      }
      else
      {
        forceFallStrength = 0.0f;
      }
      for (int32_t i = 0; i < s_numLegs; ++i)
      {
        legStrengths[i] = forceFallStrength;
      }
    }
    else if (!m_forceFallTriggered) // this applies only before the fall has been triggered
    {
      float signOfStagger = m_attribs.m_impulseTargetStepSpeed > 0.0f ? 1.0f:-1.0f;
      balanceInfo.m_targetStepVelocity = m_forceFallDirection * m_preFallTargetStepVelocity * signOfStagger;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void HitReaction::update(MR::PhysicsRig* physicsRig, 
                          float dt, 
                          bool newHit, 
                          ER::HitSelectionInfo& hsi,
                          ER::HitAttribs& attribs,
                          OutputSourceData& osd,
                          MR::InstanceDebugInterface* pDebugDrawInst)
{
  NMP_ASSERT(physicsRig);

  // grab the dimensional scaling for this character
  const ER::DimensionalScaling& dimensionalScaling = Body::getFromPhysicsRig(physicsRig)->getDimensionalScaling();
  
  // Hit records update (previous and current if applicable)
  bool haveNewHighestPriorityHit = updateHitRecords(newHit, dt, hsi, attribs, pDebugDrawInst);

  // Hit impulse
  // New impulse source data is registered when the hit comes in (so it goes through the system regardless of exit state
  // though it's application is deferred by a frame).
  //
  if (m_haveNewHit)
  {
    updateNewImpulse();
  }

  // Termination
  //
  updateStatus(dt, osd.m_balanceInfo.m_bodyStrength);

  // Leg strengths and spine yield (characteristics).
  //
  updateSpineAndLegStrengths(osd.m_spineDamping, osd.m_legStrengths);

  // Reach for wound
  //
  updateReaching(haveNewHighestPriorityHit, dimensionalScaling);

  // Balance outputs
  //
  updateBalance(osd.m_balanceInfo);

  // Look reflex
  //
  updateLook(osd.m_lookInfo, dimensionalScaling);

  // Animated reflex
  //
  updateAnim(osd.m_animSelectionInfo);

  // Forced fall
  //
  updateForcedFall(osd.m_balanceInfo, osd.m_legStrengths, dt, dimensionalScaling);

  // Purge the hit record provided not entering a death relax in which case there's no need.
  if (!(m_dying || m_deathTrigger))
  {
    m_hsiLog.removeExpiredItems();
  }

  updateExpandLimits(physicsRig);
}

} // namespace ER
