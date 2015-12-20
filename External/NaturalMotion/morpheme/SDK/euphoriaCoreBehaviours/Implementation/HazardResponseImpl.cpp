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
#include "HazardResponsePackaging.h"
#include "HazardResponse.h"
#include "HazardManagementPackaging.h"
#include "MyNetworkPackaging.h"
#include "euphoria/erDebugDraw.h"
#define SCALING_SOURCE pkg.owner->owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
static float shieldHazardResponse(const HazardResponseUpdatePackage& pkg, NMP::Vector3& shieldDirToHazard)
{
  if (!pkg.in->getShieldParametersImportance())
    return 0.0f;

  // data we use.
  const Hazard& hazard = pkg.feedIn->getHazard();
  const Environment::State& state = hazard.object.state;
  const DynamicState& dsBody = pkg.owner->owner->data->bodyLimbSharedState.m_dynamicState;
  const float timeToImpact = hazard.impactTime;
  const NMP::Vector3 objectVel = state.getAveragedPointVel(hazard.impactPosition, timeToImpact);

  // Non rig independent parameters which can have an impact on the efficiency of the shield.
  // Maximum time period before impact character will start a shield.
  const float maxPrepTimeForShield = SCALE_TIME(1.0f);
  // Maximum time to impact to do a shield in urgency
  const float maxTimeToImpactShieldUrgency = SCALE_TIME(0.15f);
  // Minimum difference of speed due to impact to do a shield in urgency.
  const float minSpeedChangeDueToImpactForShieldSq = NMP::sqr(SCALE_VEL(0.5f));
  // Maximum speed of the hazard to consider it 's static.
  const float maxSpeedHazardIsStatic = SCALE_VEL(0.5f);
  // Maximum difference of speed between the current hazard and the last one to do a shield in urgency.
  const float maxSpeedDifHazards = SCALE_VEL(0.5f);
  // Minimum difference of impact position to consider hazards from a different direction for a shield in urgency.
  const float minDifImpactPosition = SCALE_DIST(1.0f);

  // Defaults values for shield if there is no hazard.
  float shieldImportance = 0.0f;
  shieldDirToHazard.set(0, 0, 0);

// We work out if character will do a shield because his high angular speed
// is too high and if there is any danger.
  float maxAngSpeed = pkg.in->getShieldParameters().angularSpeedForShield;
  bool tooHighAngularSpeed = (maxAngSpeed * maxAngSpeed) < dsBody.getAngularVelocity().magnitudeSquared();
  if (tooHighAngularSpeed)
  {
    shieldImportance = 1.0f;
    //as you can't see anything, assume hazard from above so character will take a foetal pose
    shieldDirToHazard = pkg.owner->owner->data->spineLimbSharedStates[0].m_endTM.upDirection();
  }
  else
  {
    // We work out if character will do a shield because he will be hit with a big momentum or
    // because he didn't have the time to do other strategies, or if there are different hazards
    // coming at the same time.

    // If the character won't already do a shield because of his high angular velocity (we keep the  hazardDir as he
    // can't see anything and if the next impact is soon, we check  if character needs to do a shield because
    // other dangers. Can do a shield only if the predicted impact is soon, otherwise will choose
    // another strategy.
    if (timeToImpact < maxPrepTimeForShield)
    {
      bool hazardIsStatic = state.velocity.magnitudeSquared() < (maxSpeedHazardIsStatic * maxSpeedHazardIsStatic);
      if (hazardIsStatic)
      {
        // For static object, use predicted impactPosition as it's sufficiently accurate.
        shieldDirToHazard = hazard.impactPosition - pkg.owner->owner->data->spineLimbSharedStates[0].m_endTM.translation();
      }
      else
      {
        // If there is any danger from a dynamic object, set direction of the hazard based on its centre.
        shieldDirToHazard = hazard.object.state.aabb.getCentre()
          - pkg.owner->owner->data->spineLimbSharedStates[0].m_endTM.translation();
      }
      shieldDirToHazard.normalise();

      // Impact due to the impact
      NMP::Vector3 deltaVelDueToImpact = (objectVel - hazard.protectPath.velocity) * state.mass /
        (pkg.owner->owner->data->totalMass + state.mass);
      float deltaVelDueToImpactMagnitudeSq = deltaVelDueToImpact.magnitudeSquared();
      float impactResponseSpeedForShieldSq = NMP::sqr(pkg.in->getShieldParameters().impactResponseSpeedForShield);

      if (deltaVelDueToImpactMagnitudeSq > impactResponseSpeedForShieldSq)
      {
        shieldImportance = 1.0f;
      }
      else
      {
        // Check if next impact worth to do a shield.
        bool impactIsNoticeable = deltaVelDueToImpactMagnitudeSq > minSpeedChangeDueToImpactForShieldSq;

        // If multiple hazards, will do a shield as he can't brace all of them.
        if (pkg.feedIn->getHazardsNotBraceable() && impactIsNoticeable)
        {
          shieldImportance = 1.0f;
        }
        else
        {
          // Work out if we have to do a shield in urgency
          // Will continue to do a shield in urgency until he is hit by the object. These 3 conditions ensure that the
          // main hazard is still the same one than during the last frame.
          bool newHazardID = pkg.data->lastHazardID != state.shapeID; // It means only the ID changed and not there is a new hazard.
          bool continueShieldInUrgency = !newHazardID && pkg.data->wasDoingShieldInUrgency
            && (timeToImpact < pkg.data->lastTimeToImpact);

          if (continueShieldInUrgency)
          {
            shieldImportance = 1.0f;
          }
          else
          {
            // Check if next impact comes from a new direction.
            bool wasNotInDanger = pkg.data->lastHazardDanger == 0.0f;
            NMP::Vector3 distWithLastImpactPos = hazard.impactPosition - pkg.data->lastImpactPos;
            bool impactInNewDirection =
              wasNotInDanger || distWithLastImpactPos.magnitudeSquared() > minDifImpactPosition * minDifImpactPosition;

            //  Check if next impact comes with a different velocity.
            NMP::Vector3 difVelLastHazards = pkg.data->lastHazardVel - state.velocity;
            bool hazardWithDifferentVel =
              difVelLastHazards.magnitudeSquared() > maxSpeedDifHazards * maxSpeedDifHazards;

            //  Check if next impact is really urgent.
            bool impactIsUrgent = timeToImpact < maxTimeToImpactShieldUrgency;

            // Finally we used all these conditions to decide if we need to do a shield in urgency
            bool doShieldInUrgency =
              hazardWithDifferentVel && ((newHazardID && impactIsUrgent && impactIsNoticeable && impactInNewDirection));
            pkg.data->wasDoingShieldInUrgency = doShieldInUrgency;

            if (doShieldInUrgency)
            {
              shieldImportance = 1.0f;
            }
          }
        }
      }
    }
  }

  // If the hazard is significant (heavier than the character, then work out if character will do a
  // shield because his tangential speed along a bigger object than him is too high.
  if (state.mass > dsBody.mass)
  {
    float speedAlongObject = dsBody.getVelocity().getComponentOrthogonalToDir(hazard.contactNormal).magnitude();
    bool isTangentialSpeedTooHigh = pkg.in->getShieldParameters().tangentialSpeedForShield < speedAlongObject;
    if (isTangentialSpeedTooHigh)
    {
      shieldImportance = 1.0f;
      shieldDirToHazard = hazard.impactPosition
        - pkg.owner->owner->data->spineLimbSharedStates[0].m_endTM.translation();
      shieldDirToHazard.normalise();
    }
  }

  pkg.data->lastTimeToImpact = timeToImpact;

  return shieldImportance;
}

#undef SCALING_SOURCE
#define SCALING_SOURCE owner->owner->data->dimensionalScaling
//----------------------------------------------------------------------------------------------------------------------
void HazardResponseUpdatePackage::update(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  // receive impact prediction and optionally (if it is dangerous enough) brace and rotate away from the impact
  // can do a shield, if it's really dangerous caused by a high angular velocity of the COM, high momentum hazards
  // or high tangential velocity along a big object.
  const Hazard& hazard = feedIn->getHazard();
  const float hazardDanger = feedIn->getHazardImportance();
  const Environment::State& state = hazard.object.state;

  float shieldImportance = 0.0f;
  // respond to active hazards (ones that are approaching the character)
  if (hazardDanger > 0.0f)
  {
    NMP::Vector3 shieldDirToHazard;
    shieldImportance = shieldHazardResponse(*this, shieldDirToHazard);
    if (shieldImportance)
    {
      out->setShieldDirectionToHazard(shieldDirToHazard, shieldImportance);
    }

    // If there is a high danger, he will do a shield, we don't want to set brace, cushion
    // or avoid hazard in order to avoid some blendings in rotation requests for spine.
    bool notOnlyShield = shieldImportance < 1.0f;
    if (notOnlyShield)
    {
      const float timeToImpact = hazard.impactTime;
      const NMP::Vector3 objectVel = state.getAveragedPointVel(hazard.impactPosition, timeToImpact);

      if (in->getBraceWeight())
      {
        BraceHazard braceHazard; // pseudo local space, need to change
        braceHazard.normal = hazard.contactNormal;
        MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Velocity,
          hazard.impactPosition, state.velocity, NMP::Colour::DARK_PURPLE);
        braceHazard.position = hazard.impactPosition;
        braceHazard.velocity = objectVel;
        braceHazard.patch = hazard.object;
        MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Velocity,
          hazard.impactPosition, braceHazard.velocity, NMP::Colour::TURQUOISE);

        MR_DEBUG_DRAW_POINT(pDebugDrawInst, hazard.impactPosition, SCALE_DIST(1.0f) * hazardDanger, NMP::Colour::RED);
        out->setBraceHazard(braceHazard, in->getBraceWeight());
      }

      // this is what makes the balance crouch, it is a strategy for preparing for an impact
      // you lower your stance prior to impact
      NMP::Vector3 toHazard = NMP::vNormalise(hazard.impactPosition - hazard.protectPath.position);
      // to work out how much to change the stance, you base it on how fast the hazard is approaching
      NMP::Vector3 relVelocity =  hazard.protectPath.velocity - state.getVelocityAtPoint(hazard.impactPosition);
      float relVel = relVelocity.dot(toHazard);
      const float incomingVelocity = NMP::maximum(0.0f, relVel);

      // and the importance is based on how urgent the motion is
      float crouchPrepareTime = SCALE_TIME(0.4f);
      const float hazardImportance = crouchPrepareTime / (crouchPrepareTime + timeToImpact);

      out->setInstabilityPreparation(incomingVelocity, hazardImportance);

      if (in->getHeadDodgeWeight())
      {
        // next we implement an avoiding strategy for objects which can be avoided
        // this information controls the headDodge module
        DodgeHazard dodgeHazard;
        dodgeHazard.position = hazard.impactPosition;
        dodgeHazard.velocity = objectVel;
        // appropriate dodge radius, scaled by the amount of requested headDodge
        dodgeHazard.radius = feedIn->getHazard().protectPath.radius * 0.5f * in->getHeadDodgeWeight();
        dodgeHazard.dodgeableAmount = feedIn->getHazard().isDodgeable ? 1.0f : 0.0f;
        dodgeHazard.rampDownDurationScale = SCALE_TIME(0.5f);
        out->setDodgeHazard(dodgeHazard);
      }
    }
  }
  // if not shielding, then respond to passive hazards (things close by which could pose a hazard in future)
  if (shieldImportance < 1.0f)
  {
    // avoid with the head
    out->setAvoidTarget(feedIn->getPossibleFutureHazard(),
      feedIn->getPossibleFutureHazardImportance()*in->getHeadAvoidWeight());

    // also place arms near to such hazards
    data->placementImportance = feedIn->getPossibleFutureHazardImportance() * in->getArmsPlacementWeight();
    out->setPlacementTarget(feedIn->getPossibleFutureHazard(), data->placementImportance);
  }

  // Store data.
  if (data->lastHazardID != state.shapeID) // It means only the ID changed and not there is a new hazard.
  {
    data->lastImpactPos = hazard.impactPosition;
  }
  data->lastHazardID = state.shapeID; // 0 can be no hazard or any static object.
  data->lastHazardDanger = hazardDanger;
}

//----------------------------------------------------------------------------------------------------------------------
void HazardResponse::entry()
{
  data->lastHazardVel = NMP::Vector3::InitZero;
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

