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
#include "PropertiesBehaviourInterface.h"
#include "MyNetwork.h"
#include "Behaviours/PropertiesBehaviour.h"
#include "NetworkDescriptor.h"
#include "euphoria/erDebugDraw.h"
#include "euphoria/erBody.h"
#include "euphoria/erCharacter.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void PropertiesBehaviourInterface::update(float NMP_UNUSED(timeStep))
{
  const PropertiesBehaviourData& params = ((PropertiesBehaviour*)
    (owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_Properties)))->getParams();

  owner->data->collidingSupportTime = SCALE_TIME(params.getCollidingSupportTimeCP());
  owner->data->cosMaxSlopeForGround = cosf(NMP::degreesToRadians(params.getMaxSlopeForGroundCP()));
  owner->data->awarenessPredictionTime = SCALE_TIME(params.getAwarenessPredictionTimeCP());

  // For the physics properties, set them directly
  if (params.getEnableJointProjectionCP())
  {
    ER::Character* character = owner->getCharacter();
    ER::Body& body = character->getBody();
    MR::PhysicsRig* physicsRig = body.getPhysicsRig();

    physicsRig->requestJointProjectionParameters(
      params.getJointProjectionIterationsCP(),
      params.getJointProjectionLinearToleranceCP(),
      params.getJointProjectionAngularToleranceCP());
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PropertiesBehaviourInterface::entry()
{
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

