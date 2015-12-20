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
#include "BalancePoserBehaviourInterface.h"
#include "Behaviours/BalancePoserBehaviour.h"
#include "NetworkDescriptor.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void BalancePoserBehaviourInterface::update(float NMP_UNUSED(timeStep))
{
  const BalancePoserBehaviourData& params =
    ((BalancePoserBehaviour*)(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_BalancePoser)))->getParams();

  BalancePoseParameters poseParams;
  poseParams.yaw = NMP::degreesToRadians(params.getYawCP());
  poseParams.pitch = NMP::degreesToRadians(params.getPitchCP() +
    params.getCrouchAmountCP() * params.getPitchPerCrouchAmount());
  poseParams.roll = NMP::degreesToRadians(params.getRollCP());

  poseParams.crouchAmount = params.getCrouchAmountCP() * (1.0f - params.getCrouchHeightFraction());
  if (params.getChestTranslationInCharacterSpace() && feedIn->getSupportTMImportance() > 0.0f)
  {
    const ER::LimbTransform& supportTM = feedIn->getSupportTM();
    supportTM.rotateVector(params.getChestTranslationCP(), poseParams.chestTranslation);
  }
  else
  {
    poseParams.chestTranslation = params.getChestTranslationCP();
  }

  // Don't normalise the weights as that makes it impossible to identify what's really being
  // requested.
  poseParams.pelvisMultiplier = params.getPelvisWeight();
  poseParams.spineMultiplier = params.getSpineWeight();
  poseParams.headMultiplier = params.getHeadWeight();
  out->setPoseParameters(poseParams, 1.0f);
}

//----------------------------------------------------------------------------------------------------------------------
void BalancePoserBehaviourInterface::entry()
{

}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

