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
#include "HeadAvoidBehaviourInterface.h"
#include "Behaviours/HeadAvoidBehaviour.h"
#include "euphoria/erCharacter.h"
#include "NetworkDescriptor.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"
//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void HeadAvoidBehaviourInterface::update(float NMP_UNUSED(timeStep))
{
  const HeadAvoidBehaviourData& params =
    ((HeadAvoidBehaviour*)(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_HeadAvoid)))->getParams();
  SpatialTarget target;
  target.position = params.getAvoidPlanePositionCP();
  target.normal = params.getAvoidPlaneNormalCP();
  target.normal.fastNormalise();
  out->setAvoidTarget(target, params.getWeightCP());
  out->setAvoidRadius(SCALE_DIST(params.getAvoidSafeDistanceCP()));
  out->setTwoSidedTarget(params.getTwoSidedPlane());
}

//----------------------------------------------------------------------------------------------------------------------
void HeadAvoidBehaviourInterface::entry()
{
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

