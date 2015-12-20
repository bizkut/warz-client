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

#include "Types/EndConstraintControl.h"
#include "Types/HoldTimer.h"
#include "Types/GrabDetectionParams.h"
#include "Types/GrabEnableConditions.h"
#include "Types/GrabFailureConditions.h"
#include "Types/GrabAliveParams.h"
#include "euphoria/erDimensionalScaling.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void EndConstraintControl::initialise(const ER::DimensionalScaling& scaling)
{
  // defaults
  targetShapeID = -1;       // if no actor has been selected (-1), then any actor close enough will be grabbed
  // first Bit is xAxis (normal), 2nd is yAxis (hand forward direction), and 3rd Bit the zAxis (hand left)
  lockedLinearDofs = (1 << 0) | (1 << 1) | (1 << 2); // all linear dofs are locked
  lockedAngularDofs = (1 << 0) | (1 << 1) | (1 << 2); // all angular dofs are locked
  createDistance = scaling.scaleDist(0.1f); // metres
  destroyDistance = scaling.scaleDist(0.2f); // metres
  startOrientationAngle = NMP::degreesToRadians(120.0f);
  stopOrientationAngle = NMP::degreesToRadians(150.0f);
  desiredTM = NMP::Matrix34Identity(); // not meaningful really until a grab is actually activated
  disableCollisions = true; // disable collisions between connected parts
  constrainOnContact = false;
  useCheatForces = false;
  disableAngularDofsUntilHanging = true;
}

//----------------------------------------------------------------------------------------------------------------------
void EndConstraintControl::setDesiredOrientation(NMP::Matrix34& desTM)
{
  desiredTM = desTM;
}

//----------------------------------------------------------------------------------------------------------------------
void EndConstraintControl::setTargetShapeID(int64_t actorID)
{
  targetShapeID = actorID;
}

//----------------------------------------------------------------------------------------------------------------------
void EndConstraintControl::setDisableCollisions(bool disable)
{
  disableCollisions = disable;
}

//----------------------------------------------------------------------------------------------------------------------
// HoldTimer
//----------------------------------------------------------------------------------------------------------------------
void HoldTimer::initialise(const ER::DimensionalScaling& scaling)
{
  minHoldPeriod = scaling.scaleTime(0.1f);
  maxHoldPeriod = FLT_MAX;
  noHoldPeriod = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
// GrabDetectionParams
//----------------------------------------------------------------------------------------------------------------------
void GrabDetectionParams::initialise(const ER::DimensionalScaling& scaling)
{
  maxReachDistance = scaling.scaleDist(2.0f);
  minEdgeLength = scaling.scaleDist(0.25f);
  minAngleBetweenNormals = NMP::degreesToRadians(10.0f);
  maxSlope = NMP::degreesToRadians(30.0f);
  minMass = scaling.scaleMass(30.0f);
  minVolume = scaling.scaleVolume(0.5f);
  minEdgeQuality = 0.1f;
}

//----------------------------------------------------------------------------------------------------------------------
// GrabEnableConditions
//----------------------------------------------------------------------------------------------------------------------
void GrabEnableConditions::initialise(const ER::DimensionalScaling& scaling)
{
  minSupportSlope  = NMP::degreesToRadians(25.0f); // ground normals less upright than this are considered non-supportive (angle between "up" and normal)
  verticalSpeedToStart = scaling.scaleVel(0.8f);   // vertical speed needs to be bigger to start a grab
  verticalSpeedToStop = scaling.scaleVel(0.06f);   // vertical speed needs to be smaller to stop the grab
  unbalancedAmount = 0.65f;     // balance amounts greater than this for a period of time are considered supported (no grab)
  minUnbalancedPeriod = scaling.scaleTime(0.5f);      // see minBalanceAmount
}

//----------------------------------------------------------------------------------------------------------------------
// GrabFailureConditions
//----------------------------------------------------------------------------------------------------------------------
void GrabFailureConditions::initialise(const ER::DimensionalScaling& scaling)
{
  maxHandsBehindBackPeriod = scaling.scaleTime(1.0f);  // if hands are constrained behind the back for this long, grab will be aborted
  maxReachAttemptPeriod = scaling.scaleTime(5.0f);          // if character has tried but failed to reach an edge for this long the attempt is aborted
  minReachRecoveryPeriod = scaling.scaleTime(3.0f);         // after failed attempt, a new attempt isn't started until this period has elapsed
}

//----------------------------------------------------------------------------------------------------------------------
// GrabAliveParams
//----------------------------------------------------------------------------------------------------------------------
void GrabAliveParams::initialise()
{
  pullUpAmount = 0.25f;   // How high the character tries to pull himself up. 1: to chest level, 0: no pulling.
  pullUpStrengthScale = 1.0f; // How strong the arms will try to pull up.
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE