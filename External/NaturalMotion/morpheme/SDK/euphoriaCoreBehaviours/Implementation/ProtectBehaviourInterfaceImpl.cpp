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
#include "ProtectBehaviourInterface.h"
#include "NetworkDescriptor.h"
#include "Behaviours/ProtectBehaviour.h"
#include "MyNetwork.h"
#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void ProtectBehaviourInterface::update(float NMP_UNUSED(timeStep))
{
  const ProtectBehaviourData& params =
    ((ProtectBehaviour*)(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_Protect)))->getParams();

  out->setSwivelAmount(params.getSwivelAmount());
  out->setMaxArmExtensionScale(params.getMaxArmExtensionScale());
  out->setCrouchDownAmount(params.getCrouchDownAmount());
  out->setCrouchPitchAmount(params.getCrouchPitchAmount());

  // we pass down the boolean flags which are then used by hazardResponse
  out->setHeadAvoidWeight(params.getHeadAvoidWeight());
  out->setHeadDodgeWeight(params.getHeadDodgeWeight());
  out->setBraceWeight(params.getBraceWeight());
  out->setArmsPlacementWeight(params.getArmsPlacementWeight());
  out->setArmsSwingWeight(params.getArmsSwingWeight());
  out->setLegsSwingWeight(params.getLegsSwingWeight());

  ProtectParameters toImpactPredictorParameters;
  toImpactPredictorParameters.headLookAmount = params.getHeadLookWeight();
  toImpactPredictorParameters.sensitivityToCloseMovements = params.getSensitivityToCloseMovements();
  toImpactPredictorParameters.objectTrackingRadius = SCALE_DIST(params.getObjectTrackingRadius());
  toImpactPredictorParameters.hazardLookTime = SCALE_TIME(params.getHazardLookTime());
  toImpactPredictorParameters.offset = SCALE_DIST(params.getOffsetFromChestCP());
  toImpactPredictorParameters.supportIgnoreRadius = SCALE_DIST(params.getSupportIgnoreRadius());
  out->setToImpactPredictorParameters(toImpactPredictorParameters);

  if (userIn->getUseFiltering())
  {
    ObjectFiltering toImpactPredictorHazardFiltering;
    toImpactPredictorHazardFiltering.word0 = userIn->getFilterBitmaskWord(0);
    toImpactPredictorHazardFiltering.word1 = userIn->getFilterBitmaskWord(1);
    toImpactPredictorHazardFiltering.word2 = userIn->getFilterBitmaskWord(2);
    toImpactPredictorHazardFiltering.word3 = userIn->getFilterBitmaskWord(3);
    out->setToImpactPredictorHazardFiltering(toImpactPredictorHazardFiltering);
  }

}

//----------------------------------------------------------------------------------------------------------------------
void ProtectBehaviourInterface::entry()
{
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

