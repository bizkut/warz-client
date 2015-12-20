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
#include "ArmHoldPackaging.h"
#include "ArmPackaging.h"
#include "ArmHold.h"
#include "euphoria/erDebugDraw.h"
#include "euphoria/erEuphoriaLogger.h"
#include "Helpers/Helpers.h"
#include "Arm.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
// ArmHoldUpdatePackage
//----------------------------------------------------------------------------------------------------------------------
void ArmHoldUpdatePackage::update(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* NMP_UNUSED(pDebugDrawInst))
{
  // These are parameters passed in from the behaviour interface
  if (in->getTimerImportance() > 0)
  {
    data->timer = in->getTimer();
  }

  // When no input is received, defaults to 0 (i.e. disabled in the case of bool doConstrain)
  const EndConstraintControl& endConstraint = in->getEndConstraint();
  const float constraintImp = in->getEndConstraintImportance();

  // Implement timed behaviour: minimum and maximum grab periods as well as an intermittent recovery period during
  // which no constraint can be created (to avoid repeatedly turning on/off the constraint in rapid succession).
  if (constraintImp == 0.0f)
  {
    // If asked to turn off grab, but minimum hold time is not reached, use the cached constraint information.
    const bool inMinimumPeriod = (data->timeHeld > 0) && (data->timeHeld < data->timer.minHoldPeriod);

    // Fail safe: it's possible that this module becomes activated with a constraint already existing on the hand.
    // In that case, given that a feedback pass might get executed before the update, we would start inside the minimum
    // period. However, we don't necessarily have a valid cached constraint structure, as that could have been wiped at
    // activation of the hold behaviour. Wiping of data is implemented as memset to 0, so the following assures that we
    // have either cached a constraint previously, or it's been initialised to something sensible.
    const bool cachedConstraintIsValid = data->lastRequestedConstraint.createDistance > 0.0f;

    if (inMinimumPeriod && cachedConstraintIsValid)
    {
      out->setEndConstraint(data->lastRequestedConstraint, 1.0f);
      out->setEndDesiredTM(data->lastRequestedConstraint.desiredTM);
    }
    // Otherwise, if minimum hold period has been reached, or we don't have a valid cached constraint for some reason,
    // turn off constraint by not setting it (importance defaults to 0).
  }
  else
  {
    // Cache the constraint parameters for use when it gets disabled before having reached the minimum hold period
    data->lastRequestedConstraint = endConstraint;

    // Check timers to see whether a constraint is allowed right now
    bool inNoHoldPeriod = (data->timeNotHeld > -1); // && (data->timeNotHeld < data->timer.noHoldPeriod);
    bool isNewEdge = (endConstraint.targetShapeID != data->lastRequestedConstraint.targetShapeID);
    bool inAllowedPeriod = (data->timeHeld < data->timer.maxHoldPeriod) && (!inNoHoldPeriod || isNewEdge);
    //|| (in->getNumConstrainedArms() == 0);
    if (inAllowedPeriod)
    {
      out->setEndConstraint(endConstraint, 1.0f);
      out->setEndDesiredTM(data->lastRequestedConstraint.desiredTM);
    }
    // Otherwise, turn off constraint by not setting it (importance defaults to 0).
  }
}

//----------------------------------------------------------------------------------------------------------------------
// ArmHoldFeedbackPackage
// Updates timers measuring how long the character has or hasn't grabbed
//----------------------------------------------------------------------------------------------------------------------
void ArmHoldFeedbackPackage::feedback(float timeStep, MR::InstanceDebugInterface* NMP_UNUSED(pDebugDrawInst))
{
  const ArmAndLegLimbSharedState& limbState = owner->owner->data->armLimbSharedStates[owner->data->childIndex];
  const bool isConstrained = limbState.m_isConstrained;

  if (isConstrained)
  {
    data->timeHeld += timeStep;
    data->timeNotHeld = -1;
    feedOut->setTimeHeldProportion(data->timeHeld / data->timer.maxHoldPeriod);
  }
  else
  {
    bool wasConstrained = data->timeHeld > 0;
    if (wasConstrained)
    {
      // We've just turned off the constraint: start NotHeld timer.
#ifdef USING_EUPHORIA_LOGGING
      if (data->timeHeld > data->timer.maxHoldPeriod)
        EUPHORIA_LOG_MESSAGE("Grab %i: aborted. Was holding for too long: %2.2f of %2.2f sec.\n", owner->data->childIndex,
        data->timeHeld, data->timer.maxHoldPeriod);
#endif
      data->timeNotHeld = 0;
      data->timeHeld = 0;
    }
    else if (data->timeNotHeld > -1)
    {
      // The constraint was already off: advance NotHeld timer
      data->timeNotHeld += timeStep;
      if (data->timeNotHeld > data->timer.noHoldPeriod)
      {
        EUPHORIA_LOG_MESSAGE("Grab %i: allowed again. Exited no hold period: %2.2f of %2.2f sec.\n", owner->data->childIndex,
                             data->timeNotHeld, data->timer.noHoldPeriod);
        data->timeNotHeld = -1;
      }
      // Feedback 1 when not constrained to indicate we're in noHold period,
      // so not only do we not constrain, but also stop reaching.
      else
      {
        feedOut->setTimeHeldProportion(data->timeNotHeld);
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ArmHold::entry()
{
  data->timeNotHeld = -1;
  data->timer.initialise(SCALING_SOURCE);
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

