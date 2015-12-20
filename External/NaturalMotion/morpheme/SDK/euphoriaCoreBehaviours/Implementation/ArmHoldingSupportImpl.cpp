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
#include "ArmPackaging.h"
#include "ArmHoldingSupportPackaging.h"
#include "ArmHoldingSupport.h"
#include "MyNetworkPackaging.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

#include "euphoria/erDebugDraw.h"

#include "Helpers/HoldingSupport.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void ArmHoldingSupportUpdatePackage::update(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* NMP_UNUSED(pDebugDrawInst))
{
  updateHoldingSupport(owner, owner->owner->data->armLimbSharedStates[owner->data->childIndex], in, out);
}

//----------------------------------------------------------------------------------------------------------------------
void ArmHoldingSupportFeedbackPackage::feedback(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* NMP_UNUSED(pDebugDrawInst))
{
  float controlAmount = 0;
  feedbackHoldingSupport(owner, data, owner->owner->data->armLimbSharedStates[owner->data->childIndex], feedOut, controlAmount);
  feedOut->setChestControlAmount(controlAmount); // done this way so we can have a clear name (e.g. chest / pelvis)
}

//----------------------------------------------------------------------------------------------------------------------
void ArmHoldingSupport::entry()
{
  data->hasFeedbackRun = false;
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

