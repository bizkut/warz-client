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
#include "ArmsPlacementBehaviourInterface.h"
#include "Behaviours/ArmsPlacementBehaviour.h"
#include "euphoria/erCharacter.h"
#include "NetworkDescriptor.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void ArmsPlacementBehaviourInterface::update(float NMP_UNUSED(timeStep))
{
  const ArmsPlacementBehaviourData& params = ((ArmsPlacementBehaviour*)
    (owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_ArmsPlacement)))->getParams();

  SpatialTarget target;
  target.position = params.getTargetPositionCP();
  target.normal = params.getTargetNormalCP();
  target.normal.fastNormalise();
  target.velocity = params.getTargetVelocityCP();
  out->setPlacementTarget(target, params.getWeightCP());

  NMP_ASSERT((params.getSwivelAmount() >= -1.0f) && (params.getSwivelAmount() <= 1.0f));
  out->setSwivelAmount(params.getSwivelAmount());
  out->setMaxArmExtensionScale(params.getMaxArmExtensionScale());
  out->setHandOffsetMultiplier(params.getHandOffsetMultiplier());
}

//----------------------------------------------------------------------------------------------------------------------
void ArmsPlacementBehaviourInterface::entry()
{
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

