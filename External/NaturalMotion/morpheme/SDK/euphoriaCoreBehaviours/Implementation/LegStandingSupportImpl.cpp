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
#include "LegStandingSupportPackaging.h"
#include "LegStandingSupport.h"
#include "MyNetworkPackaging.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

#include "Helpers/StandingSupport.h"

//----------------------------------------------------------------------------------------------------------------------


// some globals that may get accessed during testing/tuning
float g_supportForceMultipler = 1.0f; 
float g_supportImplicitStiffness = 0.75f;

//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
// LegStandingSupportUpdatePackage
//----------------------------------------------------------------------------------------------------------------------
void LegStandingSupportUpdatePackage::update(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* pDebugDrawInst)
{
  const ArmAndLegLimbSharedState& legLimbState = owner->owner->data->legLimbSharedStates[owner->data->childIndex];
  updateStandingSupport(owner, in, out, data, legLimbState, pDebugDrawInst);
}

//----------------------------------------------------------------------------------------------------------------------
// LegStandingSupportFeedbackPackage
//----------------------------------------------------------------------------------------------------------------------
void LegStandingSupportFeedbackPackage::feedback(
  float NMP_UNUSED(timeStep),
  MR::InstanceDebugInterface* NMP_UNUSED(pDebugDrawInst))
{
  const ArmAndLegLimbSharedState& legLimbState = owner->owner->data->legLimbSharedStates[owner->data->childIndex];
  float controlAmount = 0;
  feedbackStandingSupport(owner, in, data, legLimbState, feedOut, controlAmount);
  feedOut->setPelvisControlAmount(controlAmount); // done this way so we can have a clear name (e.g. chest / pelvis)
}

//----------------------------------------------------------------------------------------------------------------------
void LegStandingSupport::entry()
{
  data->hasFeedbackRun = false;
  data->lostContactWithGround = false;
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

