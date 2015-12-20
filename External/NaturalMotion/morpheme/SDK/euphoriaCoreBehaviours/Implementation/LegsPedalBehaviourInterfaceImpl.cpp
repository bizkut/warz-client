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
#include "LegsPedalBehaviourInterface.h"
#include "MyNetwork.h"
#include "Behaviours/LegsPedalBehaviour.h"
#include "NetworkDescriptor.h"
#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void LegsPedalBehaviourInterface::update(float NMP_UNUSED(timeStep))
{
  const LegsPedalBehaviourData& params =
    ((LegsPedalBehaviour*)(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_LegsPedal)))->getParams();

  NMP::Vector3 rotation = NMP::degreesToRadians(params.getTargetRotationDeltaCP());
  if (params.getSpinInLocalSpace())
  {
    owner->data->spineLimbSharedStates[0].m_rootTM.rotateVector(rotation); // convert from local space to world space
  }

  RotationRequest target(rotation, SCALING_SOURCE);
  target.stiffnessScale = 2.0f; // can go to high strengths
  const float epsilon = 0.001f;
  target.imminence = 1 / SCALE_TIME(epsilon + params.getRotationTimeCP());

  SpinParameters spinParams;
  spinParams.maxRadius = SCALE_DIST(params.getMaxRadius());
  spinParams.maxAngSpeed = SCALE_ANGVEL(params.getMaxAngSpeed() * 2.0f * NM_PI);
  spinParams.synchronised = params.getSynchronised();
  spinParams.spinOutwardsDistanceWhenBehind = SCALE_DIST(0.0f);

  spinParams.spinWeightLateral = params.getSpinWeightLateral();
  spinParams.spinWeightUp      = params.getSpinWeightUp();
  spinParams.spinWeightForward = params.getSpinWeightForward();

  spinParams.spinCentreLateral = spinParams.spinCentreUp = spinParams.spinCentreForward = 0.0f;
  spinParams.spinArmControlCompensationScale = 1.0f;

  out->setLegsSpinParams(spinParams);
  for (uint32_t i = 0 ; i < owner->data->numLegs ; ++i)
  {
    out->setSpinStiffnessScalesAt(i, params.getSpinAmounts(i));
    out->setSwingStiffnessScalesAt(i, params.getSwingAmounts(i));

    out->setTargetRequestsAt(
      i, target, NMP::clampValue(
      params.getImportanceForLegCP(i), 0.0f, 1.0f));
  }

  out->setMaxSwingTimeOnGround(SCALE_TIME(params.getFallenFor()));
}

//----------------------------------------------------------------------------------------------------------------------
void LegsPedalBehaviourInterface::entry()
{
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE
