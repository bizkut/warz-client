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
#include "EuphoriaRagdollBehaviourInterface.h"
#include "NetworkDescriptor.h"
#include "Behaviours/EuphoriaRagdollBehaviour.h"
#include "MyNetwork.h"
#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void EuphoriaRagdollBehaviourInterface::update(float NMP_UNUSED(timeStep))
{
  EuphoriaRagdollBehaviourData& params = ((EuphoriaRagdollBehaviour*)
    (owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_EuphoriaRagdoll)))->getParams();
  out->setSleepingDisabled(params.getDisableSleeping());
}

//----------------------------------------------------------------------------------------------------------------------
void EuphoriaRagdollBehaviourInterface::entry()
{
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

