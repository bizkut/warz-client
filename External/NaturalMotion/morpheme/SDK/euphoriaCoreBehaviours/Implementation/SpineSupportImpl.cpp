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
#include "SpinePackaging.h"
#include "SpineSupportPackaging.h"
#include "SpineSupport.h"
#include "euphoria/erDebugDraw.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
// SpineSupportUpdatePackage
//----------------------------------------------------------------------------------------------------------------------
void SpineSupportUpdatePackage::update(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  NMP::Matrix34 rootTM = owner->owner->data->spineLimbSharedStates[owner->data->childIndex].m_rootTM;

  // When balancing use the desired pelvis orientation as a basic on which to calculate the target
  // chest orientation, since the actual pelvis will wobble around a fair bit during stepping, and
  // we don't want the upper body to just follow that.
  if (feedIn->getTargetPelvisTMImportance() > 0.0f)
    rootTM.set3x3(feedIn->getTargetPelvisTM());

  NMP::Vector3 upperPositionWorld = in->getDesiredEndTranslationRelRoot();
  upperPositionWorld.transform(rootTM);
  NMP::Quat targetOrientation = rootTM.toQuat() * in->getDesiredEndRotationRelRoot();
  MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, NMP::Matrix34(targetOrientation, upperPositionWorld), SCALE_DIST(0.5f));

  // convert the world space orientation into a rotation delta from the current end pos
  NMP::Quat end = owner->owner->data->spineLimbSharedStates[owner->data->childIndex].m_endTM.toQuat();
  targetOrientation *= ~end;
  float rotImp = in->getDesiredEndRotationRelRootImportance();
  float transImp = in->getDesiredEndTranslationRelRootImportance();

  // Set the requests based on the input local space targets
  out->setUpperRotationRequestLocal(
    RotationRequest(targetOrientation.toRotationVector(), SCALING_SOURCE), rotImp);
  out->setLowerRotationRequestLocal(
    RotationRequest(NMP::Vector3(0, 0, 0), SCALING_SOURCE), rotImp);
  out->setUpperTranslationRequestLocal(
    TranslationRequest(upperPositionWorld
    - owner->owner->data->spineLimbSharedStates[owner->data->childIndex].m_endTM.translation(), SCALING_SOURCE), transImp);
  out->setLowerTranslationRequestLocal(
    TranslationRequest(NMP::Vector3(0, 0, 0), SCALING_SOURCE), transImp);

  // Also output the world space requests that have come to the spine, so that they can be combined in the
  // destination module, which is spineControl.
  out->setUpperRotationRequestWorld(owner->data->upperRotationRequest, owner->data->upperRotationRequestImp);
  out->setLowerRotationRequestWorld(owner->data->lowerRotationRequest, owner->data->lowerRotationRequestImp);
  out->setUpperTranslationRequestWorld(owner->data->upperTranslationRequest, owner->data->upperTranslationRequestImp);
  out->setLowerTranslationRequestWorld(owner->data->lowerTranslationRequest, owner->data->lowerTranslationRequestImp);
}
//----------------------------------------------------------------------------------------------------------------------
// SpineSupportFeedbackPackage
//----------------------------------------------------------------------------------------------------------------------
void SpineSupportFeedbackPackage::feedback(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* NMP_UNUSED(pDebugDrawInst))
{
  feedOut->setRootSupportPoint(owner->owner->data->spineLimbSharedStates[owner->data->childIndex].m_rootTM.translation());
}

//----------------------------------------------------------------------------------------------------------------------
void SpineSupport::entry()
{
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

