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
#include "ArmsBraceBehaviourInterface.h"
#include "NetworkDescriptor.h"
#include "Behaviours/ArmsBraceBehaviour.h"
#include "MyNetwork.h"
#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void ArmsBraceBehaviourInterface::update(float NMP_UNUSED(timeStep))
{
  const ArmsBraceBehaviourData& params =
    ((ArmsBraceBehaviour*)(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_ArmsBrace)))->getParams();

  // We create a brace behaviour by creating a patch from the parameters and inserting that into a BraceHazard structure
  // Note that the hazard position is the contact position (with normal) not the centre of the sphere
  Environment::Patch patch;
  NMP::Vector3 normal = params.getHazardNormalCP();
  normal.fastNormalise();

  patch.createAsSphere(
    params.getHazardPositionCP() - normal * params.getHazardRadiusCP(), params.getHazardRadiusCP(),
    params.getHazardVelocityCP(), params.getHazardMassCP(), 0);

  BraceHazard hazard;
  hazard.position = params.getHazardPositionCP();
  hazard.velocity = params.getHazardVelocityCP();
  hazard.normal = normal;
  hazard.patch = patch;
  out->setHazard(hazard, params.getWeightCP());

  NMP_ASSERT((params.getSwivelAmount() >= -1.0f) && (params.getSwivelAmount() <= 1.0f));
  out->setSwivelAmount(params.getSwivelAmount());
  out->setMaxArmExtensionScale(params.getMaxArmExtensionScale());
}

//----------------------------------------------------------------------------------------------------------------------
void ArmsBraceBehaviourInterface::entry()
{
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

