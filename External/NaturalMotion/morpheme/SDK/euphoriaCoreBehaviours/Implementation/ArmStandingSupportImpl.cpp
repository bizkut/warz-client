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
#include "ArmStandingSupportPackaging.h"
#include "ArmStandingSupport.h"
#include "MyNetworkPackaging.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

#include "euphoria/erDebugDraw.h"

#include "Helpers/StandingSupport.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void ArmStandingSupportUpdatePackage::update(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* pDebugDrawInst)
{
  const ArmAndLegLimbSharedState& armLimbState = owner->owner->data->armLimbSharedStates[owner->data->childIndex];
  updateStandingSupport(owner, in, out, data, armLimbState, pDebugDrawInst);
}

//----------------------------------------------------------------------------------------------------------------------
void ArmStandingSupportFeedbackPackage::feedback(
  float NMP_UNUSED(timeStep),
  MR::InstanceDebugInterface* NMP_UNUSED(pDebugDrawInst))
{
  float controlAmount = 0;
  const ArmAndLegLimbSharedState& armLimbState = owner->owner->data->armLimbSharedStates[owner->data->childIndex];
  feedbackStandingSupport(owner, in, data, armLimbState, feedOut, controlAmount);
  feedOut->setChestControlAmount(controlAmount); // done this way so we can have a clear name (e.g. chest / pelvis)
}

//----------------------------------------------------------------------------------------------------------------------
void ArmStandingSupport::entry()
{
  data->hasFeedbackRun = false;
  data->lostContactWithGround = false;
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

