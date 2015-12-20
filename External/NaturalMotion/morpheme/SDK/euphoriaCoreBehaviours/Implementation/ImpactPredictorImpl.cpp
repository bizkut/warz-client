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
#include "HazardManagementPackaging.h"
#include "ImpactPredictorPackaging.h"
#include "ImpactPredictor.h"
#include "MyNetwork.h"
#include "mrPhysX3.h"
#include "mrPhysicsScenePhysX3.h"
#include "euphoria/erBody.h"
#include "euphoria/erDebugDraw.h"
#include "Types/Environment_CollideResult.h"

#define SCALING_SOURCE owner->owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
// ImpactPredictorUpdatePackage
//----------------------------------------------------------------------------------------------------------------------
void ImpactPredictorUpdatePackage::update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst)
{
  // provide a default look time
  float lookTime = fabsf(in->getProtectParameters().hazardLookTime);
  // we want this to be projected (into the future to detect impacts)
  out->setSphereProjection(data->protectSphere);
  if (pDebugDrawInst)
  {
    data->protectSphere.debugDraw(NMP::Vector3(1, 0, 0), NMP::Vector3(1, 1, 0), 2.5f, pDebugDrawInst);
  }
  if (!in->getProtectParameters().headLookAmount) // early-out if told not to look
    return;
  // if we're not seeing a hazard then ramp down the look target over a time specified in the input
  // based on a flag to decide to look in the direction the hazard went, or the direction it came from
  if (data->hazardUrgency == 0.0f)
  {
    if (in->getProtectParameters().hazardLookTime >= 0.0f)
      data->hazard.impactPosition += data->hazard.object.state.velocity * timeStep;
    else
      data->hazard.impactPosition = data->firstSeenPos;
    data->lookTime += timeStep;
    if (data->lookTime >= lookTime)
      return; // don't look if the timer has run out
  }
  else
  {
    // reset
    data->lookTime = 0.0f;
  }
  // this is a safer way to get the imminence
  float imminence = 4.0f * in->getProtectParameters().headLookAmount * data->hazard.impactSpeed /
    (data->hazard.impactSpeed * data->hazard.impactTime + data->protectSphere.radius);
  if (imminence > 1.0f)
  {
    // a fudge to not look down so much, it would be complicated to try to get the head to look to a
    // different position in a more general way
    NMP::Vector3 offset = NMP::vNormalise(feedIn->getLeanVector()) * SCALE_DIST(0.15f);
    TargetRequest lookRequest(data->hazard.impactPosition + offset, SCALING_SOURCE);
    lookRequest.imminence = imminence;
    lookRequest.stiffnessScale = 2.0f;
    lookRequest.passOnAmount = 0.25f;
    MR_DEBUG_DRAW_POINT(pDebugDrawInst,
      data->hazard.impactPosition + offset, SCALE_DIST(1.0f) * imminence, NMP::Colour::PURPLE);

    // don't look out of range if he's falling
    lookRequest.lookOutOfRangeWeight = owner->feedIn->getSupportAmount() > 0.0f ? 1.0f : 0.5f;

    // if he's falling then don't look when object too close... it is a distraction.
    float distance = (data->hazard.impactPosition - data->protectSphere.position).magnitude();
    if (owner->feedIn->getSupportAmount() > 0.0f || distance > data->hazard.protectPath.radius)
    {
      out->setFocalCentre(lookRequest);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// ImpactPredictorFeedbackPackage
//----------------------------------------------------------------------------------------------------------------------
void ImpactPredictorFeedbackPackage::feedback(float timeStep, MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  const float minTimeStep = SCALE_TIME(1e-6f);
  if (timeStep < minTimeStep)
  {
    timeStep = minTimeStep;
  }

  const ProtectParameters& protectParameters = in->getProtectParameters();
  // generally, this module should pass up the single most important imminent impact (it is,
  // incidentally, a tough question what counts as most important, closest? soonest? largest
  // momentum? requires most deceleration?)
  // we use the passed in protect position and velocity here, it isn't ImpactPredictor's place to define these
  const BodyState& chest = owner->feedIn->getChestState();
  data->protectSphere.position = chest.position;
  data->protectSphere.velocity = chest.velocity;
  data->protectSphere.acceleration = chest.acceleration;
  
  data->protectSphere.position += protectParameters.offset;

  if (in->getIgnoreVerticalPredictionAmount())
  {
    data->protectSphere.velocity -= owner->owner->data->up *
      owner->owner->data->up.dot(data->protectSphere.velocity) * in->getIgnoreVerticalPredictionAmount();
    data->protectSphere.acceleration -= owner->owner->data->up *
      owner->owner->data->up.dot(data->protectSphere.acceleration) * in->getIgnoreVerticalPredictionAmount();
  }
  data->protectSphere.extraSearchRadius = protectParameters.objectTrackingRadius;
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Velocity, data->protectSphere.position, chest.velocity, NMP::Colour::GREEN);

  const NMP::Vector3 leanDir = NMP::vNormalise(feedIn->getLeanVector());

  // Ignore any components of the chest acceleration in opposite to lean direction when calculating protect acceleration.
  NMP::Vector3 chestAcc = chest.acceleration;
  float chestAccDotLean = chestAcc.dot(leanDir);
  chestAccDotLean = NMP::minimum(chestAccDotLean, 0.0f);
  chestAcc -= (chestAccDotLean * leanDir);

  if (in->getVelocityIsControlled())
  {
    // it is controlled so, since we aren't and can't look ahead in frames of the animation, we
    // assume no acceleration
    data->protectSphere.acceleration.setToZero();
    chestAcc.setToZero();
  }

  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal,
    chest.position, -NMP::vNormalise(feedIn->getLeanVector()), NMP::Colour::DARK_GREY);

  // OK, here we can change our prediction of the acceleration, by assuming the body will support
  // this gravity based acceleration.
  if (feedIn->getLeanVectorImportance()) // no lean vector if no supporting points
  {
    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta, chest.position - NMP::Vector3(0.02f, 0, 0),
      -feedIn->getLeanVector(), NMP::Colour::WHITE);
    // prevents from 'being scared' of the ground on occasion
    data->protectSphere.acceleration =
      data->protectSphere.acceleration.getComponentOrthogonalToDir(feedIn->getLeanVector());
  }
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Acceleration,
    data->protectSphere.position, data->protectSphere.acceleration, NMP::Colour::PURPLE);
  float sphereRadius = in->getProbeRadiusImportance() ? in->getProbeRadius() : SCALE_DIST(0.5f);
  float smallRadiusScale = 0.5f;
  float smallSphereRadius = smallRadiusScale * sphereRadius;

  // initialise metrics before examining the list of potential hazards, where the metrics will be updated progressively
  data->protectSphere.radius = smallSphereRadius;
  data->protectSphere.maxTime = owner->owner->data->awarenessPredictionTime;

  // This loop should find the most hazardous of the impacts and send this up
  const Environment::Patch* potentialHazardPatches[PatchSet::MAX_NUM_PATCHES * 2];
  int32_t numPotentialHazards = 0;

  // Accumulate potential environment hazards.
  if ((in->getIgnoreNonUserHazardsImportance() == 0.0f) || (!in->getIgnoreNonUserHazards()))
  {
    const PatchSet& envHazardPatchSet = feedIn->getPotentialEnvironmentHazards();
    for (int32_t i = 0; i < envHazardPatchSet.numPatches; ++i)
    {
      const Environment::Patch& patch = envHazardPatchSet.patches[i];

      if (in->getProtectHazardFilteringImportance() != 0.0f)
      {
        const physx::PxShape* const shape = ER::getPxShapeFromShapeID(patch.state.shapeID);
        if (shape)
        {
          // Filter-out non-hazardous patches i.e. where their corresponding shapes have been masked as ignorable hazard.
          const ObjectFiltering userFilter = in->getProtectHazardFiltering();
          const physx::PxFilterData userFilterData(userFilter.word0, userFilter.word1, userFilter.word2, userFilter.word3);

          // Only add hazardous patches i.e. if their corresponding shapes have not been masked as ignorable hazard.
          const bool isHazardous = MR::applyFilterShader(shape, &userFilterData);

          if (isHazardous)
          {
            potentialHazardPatches[numPotentialHazards++] = &patch;
          }
        }
        else // No shape is associated with this patch, add as hazardous patch in this case.
        {
          potentialHazardPatches[numPotentialHazards++] = &patch;
        }
      }
      else // Filtering is disabled, add as hazardous patch.
      {
        potentialHazardPatches[numPotentialHazards++] = &patch;
      }
    }
  }

  // Accumulate potential user defined hazards.
  if (in->getPotentialUserHazardsImportance() > 0.0f)
  {
    const PatchSet& userHazardPatchSet = in->getPotentialUserHazards();
    for (int32_t i = 0; i < userHazardPatchSet.numPatches; ++i)
    {
      potentialHazardPatches[numPotentialHazards++] = &userHazardPatchSet.patches[i];
    }
  }

  Environment::CollideResult bestResult;
  bestResult.impactSpeed = 0.0f;
  bestResult.time = 0.0f;
  bestResult.distance = 0.0f;
  // this signals this hazard structure is not set
  data->hazardUrgency = 0.0f;
  float bestScaledUrgency = 0.0f;
  int32_t bestI = -1;
  // use bigger sphere if we get a normal that is too close to right angle
  float preferredMinNormalCosAngle = 0.2f;

  // Initialize data used to work out if more than one significant hazard will hit
  // the character soon and are not braceable (feedOut.hazardsNotBraceable).
  int32_t significantHazardsCounter = 0;
  float nextImpactsTime[2];
  nextImpactsTime[0] = data->protectSphere.maxTime;
  nextImpactsTime[1] = data->protectSphere.maxTime;
  NMP::Vector3 nextImpactsVel[2];
  nextImpactsVel[0].setToZero();
  nextImpactsVel[1].setToZero();
  NMP::Vector3 nextImpactsPos[2];
  nextImpactsPos[0].setToZero();
  nextImpactsPos[1].setToZero();
  float velInDirectionNormal = 0.0f;

  // ie make larger to demand a better normal from the search below.
  // This kind of gives us the 2 sphere conical shape using the minNormal and smallSphereScale values
  for (int32_t size = 0; size < 2; size++)
  {
    for (int32_t i = 0; i < numPotentialHazards; i++)
    {
      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_AngularVelocity,
        potentialHazardPatches[i]->state.position, potentialHazardPatches[i]->state.angularVelocity, NMP::Colour::YELLOW);
      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Velocity,
        potentialHazardPatches[i]->state.position, potentialHazardPatches[i]->state.velocity, NMP::Colour::TURQUOISE);
      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Acceleration,
        potentialHazardPatches[i]->state.position, potentialHazardPatches[i]->state.acceleration, NMP::Colour::PURPLE);

      // This purposely is a copy of the patch as its acceleration gets modified temporarily
      Environment::Patch patch = *(potentialHazardPatches[i]);
      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Velocity,
        patch.state.position, patch.state.velocity, NMP::Colour::WHITE);

      // sweep sphere to collide with patch. If collided, then check to see if it is the most urgent
      Environment::CollideResult result;
      if (patch.sphereTrajectoryCollides(data->protectSphere, NMP::Vector3(0, 0, 0), result, SCALING_SOURCE))
      {
        float timeMetric = result.time * patch.getReliabilityOfPosition(
          result.impactPosition, data->protectSphere.position, result.clippedToBoundingBox);

        MR_DEBUG_DRAW_POINT(pDebugDrawInst, result.impactPosition, data->protectSphere.radius, NMP::Colour::WHITE);
        const float epsilon = SCALE_TIME(1e-10f);
        // urgency is unitless 0 to 1 value
        float urgency = data->protectSphere.maxTime / (epsilon + result.time);

        float urgencyScale = 1.0f;
        if (in->getVelocityIsControlled())
        {
          urgencyScale = (result.impactVel - data->protectSphere.velocity).magnitude() /
            (result.impactVel.magnitude() + 1e-10f);
        }

        // When balancing, reduce the urgency for hazards that are very close to, or below, the
        // supportTM, as these are the ones counting as the "floor".
        if (feedIn->getSupportTMImportance())
        {
          const ER::LimbTransform& supportTM = feedIn->getSupportTM();
          NMP::Vector3 supportToHazardDelta = (result.impactPosition - supportTM.translation());
          float dot = supportToHazardDelta.dot(supportTM.upDirection());
          if (dot < 0.0f)
          {
            // treat the support like an infinite cylinder extending below
            supportToHazardDelta -= dot * supportTM.upDirection();
          }
          float supportToHazardDist = supportToHazardDelta.magnitude();
          const float ignoreDistance = protectParameters.supportIgnoreRadius;
          float leanUrgencyScale = NMP::clampValue((supportToHazardDist - ignoreDistance) / ignoreDistance, 0.0f, 1.0f);

          urgencyScale *= leanUrgencyScale;
        }

        // effectiveMass is how much force you need to push the hazard for it to accelerate relative to you by 1m/s^2
        float effectiveMass = patch.state.mass / (owner->owner->data->bodyLimbSharedState.m_dynamicState.mass + patch.state.mass);
        // timeMetric increases time with distance from known contact point
        float scaledUrgency = urgencyScale * effectiveMass / (epsilon + timeMetric);
        if (scaledUrgency >= bestScaledUrgency)
        {
          bestResult = result;
          data->hazardUrgency = NMP::clampValue(urgency, 0.0f, 1.0f) * urgencyScale;
          bestScaledUrgency = scaledUrgency;
          bestI = i;
        }

        // We work out how many significant hazards there are and store data for the first two of them.
        if (numPotentialHazards > 1 && size == 0)
        {
          // Minimum difference of velocity due to impact to consider an hazard as significant.
          const float minDifSpeedDueToImpact = SCALE_VEL(0.5f);

          float difSpeedDueToImpact =
            result.impactSpeed * patch.state.mass / (owner->owner->data->totalMass + patch.state.mass);
          bool isSignificant = minDifSpeedDueToImpact < difSpeedDueToImpact;

          if (isSignificant)
          {
            significantHazardsCounter++;
            if (result.time < nextImpactsTime[0])
            {
              nextImpactsTime[1]  = nextImpactsTime[0];
              nextImpactsVel[1]   = nextImpactsVel[0];
              nextImpactsPos[1]   = nextImpactsPos[0];
              nextImpactsTime[0]  = result.time;
              nextImpactsVel[0]   = result.impactVel;
              nextImpactsPos[0]   = result.impactPosition;
            }
            else if (result.time < nextImpactsTime[1])
            {
              nextImpactsTime[1]  = result.time;
              nextImpactsVel[1]   = result.impactVel;
              nextImpactsPos[1]   = result.impactPosition;
            }
          }
        }
      }
    }

    // if we haven't found anything using a small radius sweep, then repeat again using a larger radius sweep
    if (size == 0)
    {
      velInDirectionNormal = bestResult.normal.dot(vNormalise(bestResult.impactVel));
      // if hazard found and its not a glancing angle, then we are satisfied with this result and can break out of the
      // loop
      if (data->hazardUrgency && velInDirectionNormal < -preferredMinNormalCosAngle)
        break;
      // try with the larger sphere size
      data->protectSphere.radius = sphereRadius;
    }
  }

  // if a hazard has been found then fill in a hazard structure to pass up to protect modules
  if (data->hazardUrgency)
  {
    // If there are various significant hazards, we check if we can reasonably brace them by considering their
    // velocities, positions, and impact times.
    if (significantHazardsCounter > 1)
    {
      // Maximum time period before impact character will consider multiple hazards as non braceable.
      const float maxTimeForUrgentsHazards = SCALE_TIME(0.5f);
      // Maximum time to impact between two impacts to consider multiple hazards as non braceable
      const float maxTimeBetweenUrgentsHazards = SCALE_TIME(0.15f);
      // Minimum angle to consider two hazards comes from different directions.
      const float diffDirHazardsAngle = SCALE_ANG(NM_PI / 4.0f);
      // Minimum difference of impact position to consider the hazard is from a different direction for a shield in
      // urgency.
      const float minDifImpactPosition = SCALE_DIST(1.0f);

      // Work out if the hazards come from various directions to be considered as not braceable.
      NMP::Quat impactsVelAngle;
      impactsVelAngle.forRotation(nextImpactsVel[0], nextImpactsVel[1]);
      bool impactsFromDiffDir = impactsVelAngle.angle() > diffDirHazardsAngle;

      // Work out if the hazards are far enough to be considered as not braceable.
      float distSqrBetweenHazards = (nextImpactsPos[0] - nextImpactsPos[1]).magnitudeSquared();
      bool impactsFromDiffpos = distSqrBetweenHazards > minDifImpactPosition * minDifImpactPosition;

      // Work out if the hazards are urgent, otherwise no need to really care as after the first impact,
      // character will move so prediction can't be trusted if the time before impact is great..
      bool isUrgent = nextImpactsTime[0] < maxTimeForUrgentsHazards;
      bool impactsClosedInTime = (nextImpactsTime[1] - nextImpactsTime[0]) < maxTimeBetweenUrgentsHazards;

      if (isUrgent && impactsClosedInTime && impactsFromDiffDir && impactsFromDiffpos)
      {
        feedOut->setHazardsNotBraceable(true);
      }
    }
    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Velocity,
      data->protectSphere.position, bestResult.impactVel, NMP::Colour::TURQUOISE);

    // fill in hazard structure based on collision result of most urgent patch
    data->hazard.impactSpeed = bestResult.impactSpeed;
    data->hazard.objectDistance = bestResult.distance;
    data->hazard.contactNormal = bestResult.normal;
    data->hazard.impactPosition = bestResult.impactPosition;
    data->hazard.impactTime = bestResult.time;
    data->hazard.object = *potentialHazardPatches[bestI];
    if (data->lookTime > 0.0f)
    {
      data->firstSeenPos = data->hazard.object.state.position;
    }
    data->hazard.protectPath = data->protectSphere;

    const float cosDodgeGlanceAngle = 0.6f; // glancing angles > 45 degrees can be dodged
    // if protectSphere.radius==sphereRadius then it means that the thin probe has already
    // been tried and did not connect, only the fatter probe connected. This means there is a possible free path
    // which means the hazard is dodgeable.
    data->hazard.isDodgeable =
      (data->protectSphere.radius == sphereRadius) || velInDirectionNormal > -cosDodgeGlanceAngle;
    data->hazard.protectPath.radius = sphereRadius;
    data->hazard.protectMass = chest.mass;

    // now pass this hazard up
    feedOut->setHazard(data->hazard, data->hazardUrgency);

#if defined(MR_OUTPUT_DEBUGGING)
    data->hazard.debugDraw(data->protectSphere.radius, pDebugDrawInst);
    Environment::State debugState = data->hazard.object.state;
    debugState.position = data->protectSphere.position;
    debugState.velocity = data->protectSphere.velocity - data->hazard.object.state.velocity;
    debugState.acceleration = data->protectSphere.acceleration - data->hazard.object.state.acceleration;
    debugState.debugDraw(NMP::Vector3(1, 1, 1, 1), NMP::Vector3(0.5f, 0.5f, 0.5f, 1.0f), pDebugDrawInst);
#endif
  }

  // for use in update
  data->protectSphere.radius = sphereRadius;
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Velocity,
    data->protectSphere.position + NMP::Vector3(0, 0.3f, 0), data->protectSphere.velocity, NMP::Colour::TURQUOISE);
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Acceleration,
    data->protectSphere.position + NMP::Vector3(0, 0.4f, 0), data->protectSphere.acceleration, NMP::Colour::YELLOW);

  // Now completely separately we must signal patches that are potential future hazards
  // the character will reach to keep these in check, so we want objects that have had recent movement
  // (detected by their smoother acc) and are nearby (within arms reach)
  float maxAccSqr = 0.0f;
  float maxAccScale = 1.0f;
  NMP::Vector3 minPos;
  int maxI = -1;
  const float keepTrackRadiusSqr = NMP::sqr(protectParameters.objectTrackingRadius);
  for (int32_t i = 0; i < numPotentialHazards; i++)
  {
    const Environment::Patch* const patch = potentialHazardPatches[i];
    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Velocity,
      patch->state.position + NMP::Vector3(0, 0.3f, 0), patch->state.velocity, NMP::Colour::TURQUOISE);
    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Acceleration,
      patch->state.position + NMP::Vector3(0, 0.4f, 0), patch->state.acceleration, NMP::Colour::YELLOW);
    NMP::Vector3 nearestPoint;
    int result = patch->nearestPoint(data->protectSphere.position, nearestPoint);
    float distSqr = 1e-10f + (nearestPoint - data->protectSphere.position).magnitudeSquared();
    if (distSqr < keepTrackRadiusSqr)
    {
      // larger distance for less reliable situations (contact plane is low quality)
      float errorScale = (result != 1 || patch->type == Environment::Patch::EO_ContactPlane) ? 3.0f : 1.0f;
      float accScale = errorScale * (nearestPoint - data->protectSphere.position).magnitudeSquared();

      // Ignore any components of the patch acceleration in lean direction (coming from the support direction)
      // when calculating protect acceleration.
      NMP::Vector3 patchAcc = patch->state.acceleration;
      float patchAccDotLean = patchAcc.dot(leanDir);
      patchAccDotLean = NMP::maximum(patchAccDotLean, 0.0f);
      patchAcc -= (patchAccDotLean * leanDir);
      const float minProtectAccSqr = NMP::sqr(SCALE_ACCEL(0.5f));
      const float protectAccSqr
        = patchAcc.magnitudeSquared() + NMP::maximum(chestAcc.magnitudeSquared() - minProtectAccSqr, 0.0f);

      if (protectAccSqr / accScale > maxAccSqr / maxAccScale)
      {
        maxAccSqr = protectAccSqr;
        maxAccScale = accScale;
        maxI = i;
        minPos = nearestPoint;
      }
    }
  }


  // if we've found a nearby object that has a large acceleration, then send up potential future hazard
  if (maxI != -1)
  {
    SpatialTarget vec(minPos, potentialHazardPatches[maxI]->state.getVelocityAtPoint(minPos));
    vec.normal = NMP::vNormalise(data->protectSphere.position - minPos);
    // Here we convert the acc from world space *back* to standard character units (i.e. as a
    // fraction of a reference value), so it can be used as a 0 to 1 value, equal importance for all
    // character sizes
    float accScale = sqrtf(maxAccSqr) *
      NMP::sqr(SCALE_TIME(protectParameters.sensitivityToCloseMovements)) / SCALE_DIST(1.0f);

    MR_DEBUG_DRAW_POINT(pDebugDrawInst, 0.5f * (data->protectSphere.position + minPos), 
      accScale * SCALE_DIST(1.0f), NMP::Colour::RED);
    feedOut->setPossibleFutureHazard(vec, NMP::minimum(accScale, 1.0f));
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ImpactPredictor::entry()
{
  // needs to initialise above 0 so will set firstSeenVel correctly, so doesn't need to be dimensionally scaled
  data->lookTime = 1.0f;
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

