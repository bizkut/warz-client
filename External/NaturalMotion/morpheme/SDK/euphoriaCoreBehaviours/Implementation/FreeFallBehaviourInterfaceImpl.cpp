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
#include "MyNetworkPackaging.h"
#include "FreeFallBehaviourInterface.h"
#include "FreeFallBehaviourInterfacePackaging.h"
#include "NetworkDescriptor.h"
#include "Behaviours/FreeFallBehaviour.h"
#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void FreeFallBehaviourInterface::update(float NMP_UNUSED(timeStep))
{
  const FreeFallBehaviourData& params =
    ((FreeFallBehaviour*)(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_FreeFall)))->getParams();

  FreefallParameters freefallParams;
  freefallParams.assistanceAmount   = params.getAssistanceAmount();
  freefallParams.dampingAmount      = params.getDampingAmount();
  freefallParams.angleLandingAmount = params.getAngleLandingAmount();
  freefallParams.rotationTime       = SCALE_TIME(params.getRotationTime());
  freefallParams.armsAmount         = params.getArmsAmount();
  freefallParams.legsAmount         = params.getLegsAmount();

  freefallParams.startOrientationTime = SCALE_TIME(params.getStartOrientationTime());
  freefallParams.stopOrientationTime  = SCALE_TIME(params.getStopOrientationTime());
  freefallParams.startOrientationAtTimeBeforeImpact = params.getStartOrientationAtTimeBeforeImpact();
  freefallParams.stopOrientationAtTimeBeforeImpact  = params.getStopOrientationAtTimeBeforeImpact();

  // Convert input orientation from degrees to radians.
  NMP::Vector3 inputOrientation(params.getOrientationCP());
  inputOrientation = degreesToRadians(inputOrientation);

  freefallParams.orientation        = NMP::Quat(inputOrientation, false);
  freefallParams.weight             = params.getWeightCP();
  freefallParams.secondaryDirectionThreshold = params.getSecondaryDirectionThreshold();

  freefallParams.characterAxis[0] = params.getCharacterAxis0();
  freefallParams.characterAxis[1] = params.getCharacterAxis1();

  freefallParams.targetAxis[0] = params.getTargetAxis0();
  freefallParams.targetAxis[1] = params.getTargetAxis1();

  freefallParams.disableWhenInContact = params.getDisableWhenInContactCP();

  out->setFreefallParams(freefallParams);

  // Only output the following "modifiers" if freefall is actually active. This allows other
  // behaviours (e.g. balance) to specify aspects of things like arms windmill.
  if (feedIn->getIsInFreefall())
  {
    out->setArmsSpinInPhase(params.getArmsInPhase());
    for (uint32_t i = 0 ; i < owner->data->numArms ; ++i)
    {
      if (params.getArmsSpinAmount(i) > 0.0f)
      {
        out->setArmsSpinStiffnessScalesAt(i, SCALE_DAMPING(params.getArmsSpinAmount(i)));
      }
    }

    for (uint32_t i = 0 ; i < owner->data->numLegs ; ++i)
    {
      if (params.getLegsSpinAmount(i) > 0.0f)
      {
        out->setLegsSpinStiffnessScalesAt(i, SCALE_DAMPING(params.getLegsSpinAmount(i)));
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void FreeFallBehaviourInterface::feedback(float NMP_UNUSED(timeStep))
{
  feedOut->setIsInFreefall(feedIn->getIsInFreefall(), feedIn->getIsInFreefallImportance());

  feedOut->setOrientationError(NMP::radiansToDegrees(feedIn->getOrientationError()),
    feedIn->getOrientationErrorImportance());

  // Update output pins
  FreeFallBehaviour* const behaviour =
    static_cast<FreeFallBehaviour*>(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_FreeFall));

  FreeFallBehaviourData& params = behaviour->getParams();

  params.setOrientationErrorOCP(NMP::radiansToDegrees(feedIn->getOrientationError()));
}

//----------------------------------------------------------------------------------------------------------------------
void FreeFallBehaviourInterface::entry()
{
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE
