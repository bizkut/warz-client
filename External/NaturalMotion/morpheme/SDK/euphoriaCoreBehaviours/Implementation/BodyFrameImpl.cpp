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
#include "BodyFrame.h"
#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

#include "euphoria/erDebugDraw.h"
#include "euphoria/erEuphoriaLogger.h"

//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
// BodyFrameFeedbackPackage
//----------------------------------------------------------------------------------------------------------------------
void BodyFrameFeedbackPackage::feedback(float timeStep, MR::InstanceDebugInterface* NMP_UNUSED(pDebugDrawInst))
{
  // read com data
  NMP::Vector3 lastVel = data->chestState.velocity;
  data->chestState.velocity = feedIn->getChestState().velocity;
  data->chestState.position = feedIn->getAverageShoulders();
  // calculate a smooth acceleration for the chest
  NMP::Vector3 newAcc = (data->chestState.velocity - lastVel) / timeStep;
  owner->rampDownLargeAccelerations(newAcc);
  data->chestState.acceleration = owner->smoothAcceleration(data->chestState.acceleration, newAcc, timeStep);
  // calculate a smoothed acceleration squared (this cannot be retrieved from the smoothed acceleration)
  float newAccSqr = newAcc.magnitudeSquared();
  NMP::smoothExponential(data->chestState.accSqr, timeStep, newAccSqr, SCALE_TIME(0.5f));

  // we want to use the mass of the upper body here, which is the mass of the whole body minus the legs
  data->chestState.mass = owner->data->bodyLimbSharedState.m_dynamicState.mass;
  for (unsigned i = 0 ; i < owner->data->numLegs ; ++i)
  {
    data->chestState.mass -= owner->data->legLimbSharedStates[i].m_dynamicState.mass;
  }

  feedOut->setChestState(data->chestState);

  // just change a couple of the attributes for the centre of mass bodyState
  BodyState COMstate;
  COMstate.position = owner->data->bodyLimbSharedState.m_dynamicState.getPosition();
  COMstate.velocity = owner->data->bodyLimbSharedState.m_dynamicState.getVelocity();
  COMstate.mass = owner->data->bodyLimbSharedState.m_dynamicState.mass;
  // we currently don't need to calculate an acceleration for the COM so leave this as 0
  COMstate.acceleration.setToZero();
  COMstate.accSqr = 0.0f;
  feedOut->setCentreOfMassBodyState(COMstate);
}

//----------------------------------------------------------------------------------------------------------------------
void BodyFrame::entry()
{
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

