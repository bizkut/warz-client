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
#include "PositionCorePackaging.h"
#include "PositionCore.h"
#include "BodySectionPackaging.h"
#include "euphoria/erDebugDraw.h"

#define SCALING_SOURCE owner->owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void PositionCoreUpdatePackage::update(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* NMP_UNUSED(pDebugDrawInst))
{
  out->setTranslationRequest(feedIn->getOneWayTranslationRequest(), feedIn->getOneWayTranslationRequestImportance());

  // choosing how to stream requests down based on which is supporting, e.g. if legs are supporting
  // then upper body needn't use the arms to fulfil a request
  float useInternal =
    NMP::maximum(owner->feedIn->getControlledAmount(), 1.0f - owner->feedIn->getExternallyControlledAmount());
  float useExternal =
    NMP::maximum(owner->feedIn->getExternallyControlledAmount(), 1.0f - owner->feedIn->getControlledAmount());
  out->setTranslationUnsupportedRequest(in->getTranslationRequest(), in->getTranslationRequestImportance()*useInternal);
  out->setOneWayTranslationRequest(in->getTranslationRequest(), in->getTranslationRequestImportance()*useExternal);
}

//----------------------------------------------------------------------------------------------------------------------
void PositionCoreFeedbackPackage::feedback(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* NMP_UNUSED(pDebugDrawInst))
{
  feedOut->setTranslationRequest(feedIn->getOneWayTranslationRequest(), feedIn->getOneWayTranslationRequestImportance());
}

//----------------------------------------------------------------------------------------------------------------------
void PositionCore::entry()
{
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

