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
#include "ObserveBehaviourInterface.h"
#include "NetworkDescriptor.h"
#include "Behaviours/ObserveBehaviour.h"
#include "MyNetwork.h"
#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void ObserveBehaviourInterface::update(float NMP_UNUSED(timeStep))
{
#if defined(MR_OUTPUT_DEBUGGING)
  const ER::RootModule* rootModule = getRootModule();
#endif
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());

  const ObserveBehaviourData& params =
    ((ObserveBehaviour*)(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_Observe)))->getParams();

  // There are two looks here, the first is simply to look at objects that are moving quite fast,
  // the second is to focus on the part of an object that is on a collision course with you
  // how do we pick which one to do? Well the 2nd always overwrites the first, so we just output them in order
  data->findObject.speedSquaredCoefficient = 1.0f / NMP::sqr(SCALE_VEL(params.getMinSpeed()));
  data->findObject.accelerationSquaredCoefficient = 1.0f / NMP::sqr(SCALE_ACCEL(params.getMinAcceleration()));
  data->findObject.distanceSquaredCoefficient = 1.0f / NMP::sqr(SCALE_DIST(params.getMaxDistance()));
  data->findObject.maxAverageSize = SCALE_DIST(params.getMaxSize());
  data->findObject.minMass = SCALE_MASS(params.getMinMass());
  data->findObject.charactersPosition = owner->data->bodyLimbSharedState.m_dynamicState.getPosition();
  data->findObject.charactersVelocity = owner->data->bodyLimbSharedState.m_dynamicState.getVelocity();
  data->findObject.shapeAdvantage = params.getWinnersAdvantage();
  data->findObject.interestReductionRate = SCALE_FREQUENCY(params.getInterestReductionRate());

  // now set look target for seen object
  const Environment::State& foundObject = feedIn->getFoundObject();
  float metric = feedIn->getFoundObjectMetric();
  if (metric > 1.0f)
  {
    data->findObject.focusShapeID = foundObject.shapeID;
    TargetRequest lookRequest(foundObject.position, SCALING_SOURCE);
    lookRequest.passOnAmount = 0.25;
    MR_DEBUG_DRAW_POINT(rootModule->getDebugInterface(), foundObject.position, SCALE_DIST(0.5f), NMP::Colour::RED);
    out->setFocalCentre(lookRequest, params.getObserveWeightCP());
  }
  else
  {
    data->findObject.focusShapeID = -1;
  }
  out->setFindObject(data->findObject);
}

//----------------------------------------------------------------------------------------------------------------------
void ObserveBehaviourInterface::feedback(float NMP_UNUSED(timeStep))
{
  ObserveBehaviourData& params =
    ((ObserveBehaviour*)(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_Observe)))->getParams();
  params.setObservedWeightOCP(feedIn->getFoundObjectMetric());
}

//----------------------------------------------------------------------------------------------------------------------
void ObserveBehaviourInterface::entry()
{
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

