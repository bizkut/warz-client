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
#include "RotateCorePackaging.h"
#include "RotateCore.h"
#include "BodySectionPackaging.h"
#include "euphoria/erDebugDraw.h"

#define SCALING_SOURCE owner->owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
// RotateCoreUpdatePackage
//----------------------------------------------------------------------------------------------------------------------
void RotateCoreUpdatePackage::update(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* NMP_UNUSED(pDebugDrawInst))
{
  out->setRotationRequest(feedIn->getOneWayRotationRequest(), feedIn->getOneWayRotationRequestImportance());

  float useInternal =
    NMP::maximum(owner->feedIn->getControlledAmount(), 1.0f - owner->feedIn->getExternallyControlledAmount());
  float useExternal =
    NMP::maximum(owner->feedIn->getExternallyControlledAmount(), 1.0f - owner->feedIn->getControlledAmount());
  out->setRotationUnsupportedRequest(in->getRotationRequest(), in->getRotationRequestImportance()*useInternal);
  out->setOneWayRotationRequest(in->getRotationRequest(), in->getRotationRequestImportance()*useExternal);
}

//----------------------------------------------------------------------------------------------------------------------
// RotateCoreFeedbackPackage
//----------------------------------------------------------------------------------------------------------------------
void RotateCoreFeedbackPackage::feedback(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* NMP_UNUSED(pDebugDrawInst))
{
  feedOut->setRotationRequest(feedIn->getOneWayRotationRequest(), feedIn->getOneWayRotationRequestImportance());
}

//----------------------------------------------------------------------------------------------------------------------
void RotateCore::entry()
{
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

