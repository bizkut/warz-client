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

#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
// ramps down large accelerations. The reason is that very large accelerations are usually caused by impulses
// rather than continuous accelerations, so they won't continue, and so this function ramps the perceived (continued) acceleration
// down to 0
bool MyNetworkAPIBase::rampDownLargeAccelerations(NMP::Vector3& acc) const
{
  float maxAcc = data->dimensionalScaling.scaleAccel(20.0f); // consistent value used here for all modules that call this function
  float accMag2 = acc.magnitudeSquared();
  if (accMag2 > NMP::sqr(maxAcc))
  {
    float length = sqrtf(accMag2);
    float scale = (2.0f * maxAcc - length) / length;
    acc *= NMP::maximum(scale, 0.0f);
    return true; // we are ramping down
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 MyNetworkAPIBase::smoothAcceleration(const NMP::Vector3& currentAcc, const NMP::Vector3& newAcc, float timeStep) const
{
  // this is exponential smoothing towards the passed in, new acceleration, stable at timeSteps
  NMP::Vector3 smoothedAcc = currentAcc;
  NMP::smoothExponential(smoothedAcc, timeStep, newAcc, data->dimensionalScaling.scaleTime(0.5f));
  // value ramping 0 up to 1 if acceleration is downwards like gravity
  float accDown = newAcc.dot(data->down);

  float ratioOfAccelerationToGravity = accDown / (data->gravityMagnitude + 1e-10f);
  float downScale = NMP::clampValue(ratioOfAccelerationToGravity, 0.000001f, 1.0f);

  // people are very sensitive to the feeling of falling, since it is important we give it high weight here
  NMP::Vector3 newAccDown = data->down * accDown;
  float fallSensitivity = data->dimensionalScaling.scaleFrequency(5.0f) * downScale;

  // again, apply smoothing
  NMP::smoothExponential(smoothedAcc, timeStep, newAccDown, 1.0f / fallSensitivity);
  return smoothedAcc;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 MyNetworkAPIBase::smoothAngularVelocity(const NMP::Vector3& currentAngVel, const NMP::Vector3& newAngVel, float timeStep) const
{
  NMP::Vector3 smoothedAngVel = currentAngVel;
  NMP::smoothExponential(smoothedAngVel, timeStep, newAngVel, data->dimensionalScaling.scaleTime(1.0f));
  return smoothedAngVel;
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

