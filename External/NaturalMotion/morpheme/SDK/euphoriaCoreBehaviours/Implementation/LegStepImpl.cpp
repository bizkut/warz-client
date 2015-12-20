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
#include "LegPackaging.h"
#include "LegStepPackaging.h"
#include "LegStep.h"
#include "Leg.h"
#include "MyNetworkPackaging.h"
#include "MyNetwork.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

#include "euphoria/erDebugDraw.h"
#include "euphoria/erEuphoriaLogger.h"
#include "Helpers/Step.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void LegStepUpdatePackage::update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst)
{
  updateStep(owner, in, out, data, timeStep, stepTypeLeg, pDebugDrawInst);
}

//----------------------------------------------------------------------------------------------------------------------
void LegStepFeedbackPackage::feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst)
{
  const NMP::Vector3& avgOfFootPositions = owner->owner->data->bodyLimbSharedState.m_averageOfFeetTM.translation();
  feedbackStep(owner, in, feedOut, data, avgOfFootPositions, timeStep, stepTypeLeg, pDebugDrawInst);
}

//----------------------------------------------------------------------------------------------------------------------
void LegStep::entry()
{
  data->stepLength = 1.0f; // avoid divide by zero if we step immediately
  data->isStepping = false;
  data->isDoingRecoveryStep = false;
  data->stepTargetPos = owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_endTM.translation();
  data->recoveryStepTarget.identity();
  data->recoveryStepTarget.identity();
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

