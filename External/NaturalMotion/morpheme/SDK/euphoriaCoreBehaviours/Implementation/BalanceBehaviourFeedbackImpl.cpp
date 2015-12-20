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
#include "Behaviours/BalanceBehaviour.h"
#include "BalanceBehaviourFeedback.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{
//----------------------------------------------------------------------------------------------------------------------
void BalanceBehaviourFeedback::feedback(float NMP_UNUSED(timeStep))
{
  BalanceBehaviour* behaviour = ((BalanceBehaviour*)owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_Balance));
  BalanceBehaviourData& params = behaviour->getParams();

  feedOut->setSupportWithLegs(params.getEnableStand());
  feedOut->setSupportWithArms(params.getSupportWithArms() && params.getEnableStand());
}

//----------------------------------------------------------------------------------------------------------------------
void BalanceBehaviourFeedback::entry()
{
}
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

