/*
 * Copyright (c) 2012 NaturalMotion Ltd. All rights reserved.
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
#include "LegSittingSupportPackaging.h"
#include "LegSittingSupport.h"
#include "MyNetworkPackaging.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

#include "Helpers/SittingSupport.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
// LegSupportUpdatePackage
//----------------------------------------------------------------------------------------------------------------------
void LegSittingSupportUpdatePackage::update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst)
{
  const ArmAndLegLimbSharedState& legLimbState = owner->owner->data->legLimbSharedStates[owner->data->childIndex];
  updateSittingSupport(owner, in, out, data, legLimbState, timeStep, pDebugDrawInst);
}

//----------------------------------------------------------------------------------------------------------------------
// LegSittingSupportFeedbackPackage
//----------------------------------------------------------------------------------------------------------------------
void LegSittingSupportFeedbackPackage::feedback(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* NMP_UNUSED(pDebugDrawInst))
{
  const ArmAndLegLimbSharedState& legLimbState = owner->owner->data->legLimbSharedStates[owner->data->childIndex];
  feedbackSittingSupport(owner, in, data, legLimbState, feedOut);
}

//----------------------------------------------------------------------------------------------------------------------
void LegSittingSupport::entry()
{
  data->hasFeedbackRun = false;
  data->endOffGroundAmount = 0.0f;
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

