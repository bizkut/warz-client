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
#include "MyNetwork.h"
#include "HeadDodgeBehaviourInterface.h"
#include "Behaviours/HeadDodgeBehaviour.h"
#include "euphoria/erCharacter.h"
#include "NetworkDescriptor.h"
#include "Types/DodgeHazard.h"
#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void HeadDodgeBehaviourInterface::update(float NMP_UNUSED(timeStep))
{
  const HeadDodgeBehaviourData& params =
    ((HeadDodgeBehaviour*)(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_HeadDodge)))->getParams();
  DodgeHazard dodgeHazard;
  dodgeHazard.position = params.getHazardPositionCP();
  dodgeHazard.velocity = params.getHazardVelocityCP();
  dodgeHazard.radius = params.getHazardRadiusCP();
  dodgeHazard.dodgeableAmount = 1.0f; // I suppose don't call the behaviour if it isn't true
  dodgeHazard.rampDownDurationScale = SCALE_TIME(0.5f);
  out->setDodgeHazard(dodgeHazard, params.getWeightCP());
}

//----------------------------------------------------------------------------------------------------------------------
void HeadDodgeBehaviourInterface::entry()
{
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

