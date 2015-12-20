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
#include "MyNetworkPackaging.h"
#include "LegPackaging.h"
#include "LegBrace.h"
#include "LegBracePackaging.h"
#include "MyNetworkPackaging.h"
#include "Types/SpatialTarget.h"
#include "Helpers/Brace.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

#include "euphoria/erDebugDraw.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void LegBraceUpdatePackage::update(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* pDebugDrawInst)
{
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta, 
    NMP::Vector3(0, 0, 0), NMP::Vector3(0, 1, 0)*in->getBraceHazardImportance(), 
    NMP::Colour(ER::getDefaultColourForControl(ER::bufferControl)));
  const ER::LimbTransform& rootTM = owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_rootTM;
  // this is an interpolation amount to decide how much it tries to protect the chest from impacts (when 1)
  // compared to just protecting the pelvis (when 0)
  float protectChestAmount = 0.3f;
  data->protectPoint = owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_basePosition +
    (in->getProtectState().position - owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_basePosition)
      * protectChestAmount;
  NMP::Vector3 toProtectPoint = data->protectPoint - in->getProtectState().position;
  // flatten toProtectPoint along normal direction
  toProtectPoint -= in->getBraceHazard().normal * in->getBraceHazard().normal.dot(toProtectPoint);
  // only problem here is that we do a nearest point even if the legBrace isn't active
  // the reason is that the feedback needs this position.
  // of course, it won't call this if the leg brace hasn't been enabled.
  const Environment::Patch& patch = in->getBraceHazard().patch;
  patch.nearestPoint(in->getBraceHazard().position + toProtectPoint, data->placePos, true);
  MR_DEBUG_DRAW_LINE(pDebugDrawInst, in->getProtectState().position, in->getBraceHazard().position, NMP::Colour::WHITE);
  MR_DEBUG_DRAW_LINE(pDebugDrawInst, data->protectPoint, data->placePos, NMP::Colour::WHITE);

  float maxStiffness = owner->data->normalStiffness * 2.0f * in->getBraceHazardImportance();
  if (maxStiffness == 0.0f) // if it isn't very important then don't bother
  {
    return;
  }

  if(feedIn->getRootInContactWithPatchShapeID())
  {
    return;
  }

  data->patchShapeID = patch.state.shapeID;
  bool inContactWithHazard = feedIn->getLimbInContactWithPatchShapeID();

  if(!inContactWithHazard)
  {
    // set as inContactWithHazard if limb is considered to be vaguely supporting.
    inContactWithHazard
      = owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_endData.isCollidingWithWorld &&
      vNormalise(owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_endTM.translation()
      - rootTM.translation()).dot(owner->owner->data->down) > 0.5f;
  }

  // We set up the approximate 'masses' the body mass is about half the real mass (due to the softness of the body)
  // you should think of these masses as the force needed to get a unit acceleration from the pelvis, foot and the point
  // on the hazard respectively as such we use half the arm mass for the hand, and the incoming hazard mass.
  BraceDynamic dynamic;
  dynamic.init(owner->owner->data->totalMass * 0.5f, 
    owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_dynamicState.mass
    * 0.5f, patch.state.mass);
  dynamic.maxReachDistance = owner->data->length * 0.95f;
  dynamic.minReachDistance = owner->data->length * 0.2f;

  applyBraceDynamic(dynamic, owner, in, data, owner->owner->data->legLimbSharedStates[owner->data->childIndex], out, data->protectPoint, data->placePos, false, inContactWithHazard,
    dynamic.maxReachDistance, pDebugDrawInst);
}

//----------------------------------------------------------------------------------------------------------------------
void LegBraceFeedbackPackage::feedback(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* NMP_UNUSED(pDebugDrawInst))
{
  feedOut->setEndCushionPoint(owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_endTM.translation());

  // calculates data necessary to choose the brace leg
  BraceState braceState;
  // angle to target, closer feet will be chosen
  NMP::Quat toFoot;
  toFoot.forRotation(data->placePos - data->protectPoint,
    owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_endTM.translation() - data->protectPoint);
  braceState.error = toFoot.toRotationVector();
  // we could use more a moment of inertia here, or a predicted mass
  braceState.mass = owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_dynamicState.mass;
  feedOut->setBraceState(braceState);
}

//----------------------------------------------------------------------------------------------------------------------
void LegBrace::entry()
{
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

