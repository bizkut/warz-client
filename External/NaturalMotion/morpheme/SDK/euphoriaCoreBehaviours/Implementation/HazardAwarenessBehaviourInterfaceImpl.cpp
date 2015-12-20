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
#include "HazardAwarenessBehaviourInterface.h"
#include "NetworkDescriptor.h"
#include "Behaviours/HazardAwarenessBehaviour.h"
#include "MyNetwork.h"
#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void HazardAwarenessBehaviourInterface::update(float NMP_UNUSED(timeStep))
{
  const HazardAwarenessBehaviourData& params = ((HazardAwarenessBehaviour*)
    (owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_HazardAwareness)))->getParams();

  out->setVelocityIsControlled(params.getUseControlledVelocity());
  out->setIgnoreVerticalPredictionAmount(params.getIgnoreVerticalPredictionAmount());
  out->setProbeRadius(SCALE_DIST(params.getProbeRadius()));
}

//----------------------------------------------------------------------------------------------------------------------
void HazardAwarenessBehaviourInterface::feedback(float timeStep)
{
  HazardAwarenessBehaviourData& params =
    ((HazardAwarenessBehaviour*)(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_HazardAwareness)))->getParams();

  const float hazardImportanceCutoff = params.getHazardLevelThreshold();

  // HazardLevel is just a unitless imminence, with the addition that it is 0 if the character is below a small speed,
  // to prevent high levels when lying on the ground.
  float hazardLevel = 0.0f;
  if (feedIn->getHazardImportance() && feedIn->getHazard().impactSpeed > SCALE_VEL(0.1f))
  {
    hazardLevel = SCALE_TIME(1.0f) / (SCALE_TIME(1.0f) + feedIn->getHazard().impactTime);
  }
  if (hazardLevel > hazardImportanceCutoff)
  {
    data->hazardFreeTime = 0.0f;
  }
  else
  {
    data->hazardFreeTime += timeStep;
  }
  feedOut->setHazardFreeTime(data->hazardFreeTime);
  feedOut->setHazardousness(hazardLevel);

  // Set ouput CPs
  params.setHazardFreeTimeOCP(data->hazardFreeTime);
  params.setHazardLevelOCP(hazardLevel);

  // We add epsilon here, so we distinguish between an immediate hazard and no hazard
  const float epsilon = 0.0001f;
  params.setTimeToImpactOCP(feedIn->getHazardImportance() ? feedIn->getHazard().impactTime + epsilon : 0.0f);
  params.setImpactSpeedOCP(feedIn->getHazardImportance() ? feedIn->getHazard().impactSpeed : 0.0f);
}

//----------------------------------------------------------------------------------------------------------------------
void HazardAwarenessBehaviourInterface::entry()
{
  data->hazardFreeTime = 0.0f;
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

