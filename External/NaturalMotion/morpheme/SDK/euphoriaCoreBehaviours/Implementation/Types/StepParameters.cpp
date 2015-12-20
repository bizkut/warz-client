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

#include "Types/StepParameters.h"
#include "Types/StepRecoveryParameters.h"
#include "Types/SteppingBalanceParameters.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

StepParameters::StepParameters()
{
  exclusionPlane.setToZero();
  collisionGroupIndex = -1;
  stepPredictionTimeForward = 0.0f;
  stepPredictionTimeBackward = 0.0f;
  stepUpDistance = 0.0f;
  stepDownDistance = 0.0f;
  maxStepLength = 0.0f;
  stepDownSpeed = 0.0f;
  footSeparationFraction = 0.0f;
  alignFootToFloorWeight = 0.0f;
  orientationWeight = 1.0f;
  lowerPelvisAmount = 0.0f;
  IKSubstep = 1.0f;
  gravityCompensation = 1.0f;
  strengthScale = 0.0f;
  steppingDirectionThreshold = 0.0f;
  steppingImplicitStiffness = 0.0f;
  limbLengthToAbort = 0.0f;
  rootDownSpeedToAbort = 0.0f;
  suppressFootCrossing = false;
  footCrossingOffset = 0.0f;

  memset(&stepWeaknessPerArm[0], 0, sizeof(stepWeaknessPerArm));
  memset(&stepWeaknessPerLeg[0], 0, sizeof(stepWeaknessPerLeg));
}

StepRecoveryParameters::StepRecoveryParameters()
{
  stepToRecoverPose = false;
  fwdDistanceToTriggerStep = 0.0f;
  sidewaysDistanceToTriggerStep = 0.0f;
  timeBeforeShiftingWeight = 0.0f;
  weightShiftingTime = 0.0f;
}

SteppingBalanceParameters::SteppingBalanceParameters()
{
  lookInStepDirection = true;
  wholeBodyLook = 0.2f;
  lookDownAngle = 0.0f;
  stopLookingTime = 1.0f;
  lookWeight = 1.0f;
  stepCountResetTime = 1.0f;
  minSpeedForLooking = 0.0f;
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

