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
#include "SpineControlPackaging.h"
#include "SpineControl.h"
#include "euphoria/erDebugDraw.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
// SpineControlUpdatePackage
//----------------------------------------------------------------------------------------------------------------------
void SpineControlUpdatePackage::update(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  // we have to take the maximum importance in applying end and root control
  float maxRotStiffnessScale =
    NMP::maximum(in->getEndRotationRequest().stiffnessScale, in->getRootRotationRequest().stiffnessScale);
  float maxTransStiffnessScale =
    NMP::maximum(in->getEndTranslationRequest().stiffnessScale, in->getRootTranslationRequest().stiffnessScale);
  float maxStiffnessScale = NMP::maximum(maxRotStiffnessScale, maxTransStiffnessScale);

  float maxOrientImp = NMP::maximum(in->getEndRotationRequestImportance(), in->getRootRotationRequestImportance());
  float maxPosImp   = NMP::maximum(in->getEndTranslationRequestImportance(), in->getRootTranslationRequestImportance());
  float maxImp = NMP::maximum(maxOrientImp, maxPosImp);
  if (maxImp == 0.0f)
    return; // early out
  float posWeight = maxPosImp / maxImp;
  float orientWeight = maxOrientImp / maxImp;

  // calculate vector from root to end of spine
  const NMP::Matrix34& end = owner->owner->data->spineLimbSharedStates[owner->data->childIndex].m_endTM;
  const NMP::Matrix34& root = owner->owner->data->spineLimbSharedStates[owner->data->childIndex].m_rootTM;
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_AngleDelta, end.translation(), in->getEndRotationRequest().rotation, 
    NMP::Colour(ER::convertControlAmountsToDefaultColour(in->getEndRotationRequest().debugControlAmounts)));
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_AngleDelta, root.translation(), in->getRootRotationRequest().rotation, 
    NMP::Colour(ER::convertControlAmountsToDefaultColour(in->getRootRotationRequest().debugControlAmounts)));
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta, end.translation(), in->getEndTranslationRequest().translation, 
    NMP::Colour(ER::convertControlAmountsToDefaultColour(in->getEndTranslationRequest().debugControlAmounts)));
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta, root.translation(), in->getRootTranslationRequest().translation, 
    NMP::Colour(ER::convertControlAmountsToDefaultColour(in->getRootTranslationRequest().debugControlAmounts)));
  NMP::Vector3 toEnd = vNormalise(end.translation() - root.translation());

  // calculate target orientation and derivative
  NMP::Quat orient = NMP::Quat(in->getEndRotationRequest().rotation, false) * end.toQuat();

  // calculate target translation and derivative
  NMP::Vector3 endTranslation = in->getEndTranslationRequest().translation;
  // remove component that it can't do
  NMP::Vector3 endOffset = endTranslation - toEnd * toEnd.dot(endTranslation);

  // initialise limb control
  LimbControl& control = out->startControlModification();
  float dampingRatio = owner->data->normalDampingRatio + in->getDampingIncreaseImportance() * in->getDampingIncrease();
  control.reinit(owner->data->normalStiffness * maxStiffnessScale, dampingRatio, owner->data->normalDriveCompensation);

  // apply gravity compensation if spine is supported
  float rootWeight = 1.0f;
  if (owner->in->getPelvisControlledAmount() > 0.0f)
  {
    control.setGravityCompensation(1.0f);
    rootWeight = 0.0f; // don't try and rotate pelvis if in support (balancing)
  }
  // Use implicit stiffness if we're supported
  if (owner->in->getPelvisControlledAmount() > 0.0f || owner->in->getChestControlledAmount() > 0.0f)
  {
    control.setImplicitStiffness(1.0f);
  }

  // calculate root (pelvis) orientation and derivative
  NMP::Quat rootOrient = NMP::Quat(in->getRootRotationRequest().rotation, false) * root.toQuat();
  MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, root, SCALE_DIST(1.0f));
  MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, 
    NMP::Matrix34(rootOrient, root.translation() + NMP::Vector3(0, 0.01f, 0)), SCALE_DIST(1.0f));
  // calculate root translation and derivative
  NMP::Vector3 rootTranslation = in->getRootTranslationRequest().translation;
  NMP::Vector3 rootOffset = rootTranslation - toEnd * toEnd.dot(rootTranslation); // remove component that it can't do

  // now apply the limb control
  control.setTargetOrientation(orient, orientWeight);
  control.setTargetPos(end.translation() + endOffset, posWeight);
  control.setExpectedRootOrientationForTarget(rootOrient, rootWeight * orientWeight);
  control.setExpectedRootPosForTarget(root.translation() + rootOffset, rootWeight * posWeight);
  MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, NMP::Matrix34(orient, end.r[3] + endOffset), SCALE_DIST(1.0f));
  control.setControlAmount(ER::supportControl, 1.0f);
  out->stopControlModification(maxImp);
}

//----------------------------------------------------------------------------------------------------------------------
void SpineControl::entry()
{
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

