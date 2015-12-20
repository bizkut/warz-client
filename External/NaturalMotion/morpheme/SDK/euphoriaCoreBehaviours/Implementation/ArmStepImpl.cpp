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
#include "ArmPackaging.h"
#include "ArmStepPackaging.h"
#include "ArmStep.h"
#include "Arm.h"
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
void ArmStepUpdatePackage::update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst)
{
  updateStep(owner, in, out, data, timeStep, stepTypeArm, pDebugDrawInst);
}

//----------------------------------------------------------------------------------------------------------------------
void ArmStepFeedbackPackage::feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst)
{
  const NMP::Vector3& avgHandPosition = owner->owner->data->bodyLimbSharedState.m_averageOfHandsTM.translation();
  feedbackStep(owner, in, feedOut, data, avgHandPosition, timeStep, stepTypeArm, pDebugDrawInst);
}

//----------------------------------------------------------------------------------------------------------------------
void ArmStep::entry()
{

  data->stepLength = SCALE_DIST(1.0f);
  data->isStepping = false;
  data->isDoingRecoveryStep = false;
  data->stepTargetPos = owner->owner->data->armLimbSharedStates[getChildIndex()].m_endTM.translation();
  data->recoveryStepTarget.identity();
  data->recoveryStepTarget.identity();
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

