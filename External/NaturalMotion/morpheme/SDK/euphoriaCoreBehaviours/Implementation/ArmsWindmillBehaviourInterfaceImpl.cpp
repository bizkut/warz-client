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
#include "ArmsWindmillBehaviourInterface.h"
#include "MyNetwork.h"
#include "Behaviours/ArmsWindmillBehaviour.h"
#include "NetworkDescriptor.h"
#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void ArmsWindmillBehaviourInterface::update(float NMP_UNUSED(timeStep))
{
  const ArmsWindmillBehaviourData& params =
    ((ArmsWindmillBehaviour*)(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_ArmsWindmill)))->getParams();

  NMP::Vector3 rotation = NMP::degreesToRadians(params.getTargetRotationDeltaCP());
  if (params.getSpinInLocalSpace())
  {
    owner->data->spineLimbSharedStates[0].m_endTM.rotateVector(rotation); // convert from local space to world space
  }

  RotationRequest target(rotation, SCALING_SOURCE);
  target.stiffnessScale = 2.0f; // can go to high strengths
  const float epsilon = 0.001f;
  target.imminence = 1 / SCALE_TIME(epsilon + params.getRotationTimeCP());

  // Attributes of how the spin is performed
  SpinParameters spinParams;
  spinParams.maxRadius = SCALE_DIST(params.getMaxRadius());
  spinParams.maxAngSpeed = SCALE_ANGVEL(params.getMaxAngSpeed() * 2.0f * NM_PI);
  spinParams.synchronised = params.getSynchronised();
  spinParams.spinCentreLateral = SCALE_DIST(params.getSpinCentreLateral());
  spinParams.spinCentreUp      = SCALE_DIST(params.getSpinCentreUp());
  spinParams.spinCentreForward = SCALE_DIST(params.getSpinCentreForward());
  spinParams.spinOutwardsDistanceWhenBehind = SCALE_DIST(params.getSpinOutwardsDistanceWhenBehind());
  spinParams.spinArmControlCompensationScale = params.getSpinArmControlCompensationScale();
  spinParams.strengthReductionTowardsEnd = params.getStrengthReductionTowardsHand();

  spinParams.spinWeightLateral = params.getSpinWeightLateral();
  spinParams.spinWeightUp      = params.getSpinWeightUp();
  spinParams.spinWeightForward = params.getSpinWeightForward();

  out->setArmsSpinInPhase(params.getArmsInPhase());

  out->setArmsSpinParams(spinParams);
  for (uint32_t i = 0 ; i < owner->data->numArms ; ++i)
  {
    out->setSpinStiffnessScalesAt(i, SCALE_DAMPING(params.getSpinAmounts(i)));
    out->setSwingStiffnessScalesAt(i, params.getSwingAmounts(i));

    out->setTargetRequestsAt(i, target, params.getImportanceForArmCP(i));
  }

  out->setArmsOutwardsOnly(params.getSwingOutwardsOnly());
}

//----------------------------------------------------------------------------------------------------------------------
void ArmsWindmillBehaviourInterface::entry()
{
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE
