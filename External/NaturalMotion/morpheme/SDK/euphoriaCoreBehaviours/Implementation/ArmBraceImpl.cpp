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
#include "ArmPackaging.h"
#include "ArmBrace.h"
#include "ArmBracePackaging.h"
#include "euphoria/erDebugDraw.h"
#include "Types/braceDynamic.h"
#include "Helpers/Brace.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
// ArmBraceUpdatePackage
//----------------------------------------------------------------------------------------------------------------------
void ArmBraceUpdatePackage::update(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* pDebugDrawInst)
{
  
  // don't try and brace if hand is constrained
  if (owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_isConstrained)
  {
    return;
  }
  // This routine responds to the brace hazard structure which includes the geometry patch, this structure can
  // be constructed artificially for bracing against non-physical objects
  const ER::LimbTransform& rootTM = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootTM;
  // Calculate placement position
  const NMP::Vector3 shoulderOffset = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_basePosition
    - owner->in->getCentreOfShoulders();
  data->placePos = in->getBraceHazard().position + shoulderOffset;
  owner->lowerTargetWhenBehindBack(in->getBraceHazard().position, data->placePos, shoulderOffset, pDebugDrawInst);

  // Since it is used in feedback for requests, we should set it to 0 before the early out.
  data->importance = 0.0f;
  // This is a simple early out if hazard is not important
  if (in->getBraceHazardImportance() == 0.0f)
  {
    return;
  }

  if(feedIn->getRootInContactWithPatchShapeID())
  {
    return;
  }

  // Here I adjust the protect position to consider the jarring effect of the legs/pelvis hitting a
  // target first the algorithm here is simply to move the position along the body's length in
  // proportion to how much the pelvis is likely to hit first. Possibly could be done in hazard
  // management instead, or in impact predictor.
  {
    NMP::Vector3 comPos = owner->owner->data->bodyLimbSharedState.m_dynamicState.getPosition();
    MR_DEBUG_DRAW_POINT(pDebugDrawInst, comPos, SCALE_DIST(0.05f), NMP::Colour::RED);
    NMP::Vector3 comToChestDir = owner->in->getCentreOfShoulders() - comPos;
    float toChestLength = comToChestDir.normaliseGetLength();
    NMP::Vector3 forward = NMP::vCross(comToChestDir, rootTM.rightDirection());

    const float forwardAdjust = 0.5f; // 30 degrees
    NMP::Vector3 pelvisToChest = comToChestDir + forward * forwardAdjust;

    float pelvisToChestDotNormal = NMP::vDot(pelvisToChest, in->getBraceHazard().normal);
    float modifiedPelvisToChestDotNormal = pelvisToChestDotNormal + forwardAdjust * forward.magnitude();
    float liftScale = NMP::clampValue(modifiedPelvisToChestDotNormal, 0.0f, SCALE_DIST(0.4f));
    float lift = toChestLength * liftScale;
    NMP::Vector3 posShift = NMP::vCross(in->getBraceHazard().normal, rootTM.rightDirection());
    data->placePos += posShift * lift;

    MR_DEBUG_DRAW_POINT(pDebugDrawInst, toChestLength * pelvisToChest + comPos, 0.2f * lift, NMP::Colour::RED);
    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta, 
      owner->in->getCentreOfShoulders(), posShift * lift, NMP::Colour::TURQUOISE);

    NMP::Vector3 braceDir = NMP::vNormalise(data->placePos - in->getProtectState().position);
    float approachSinAngle = NMP::vCross(braceDir, comToChestDir).magnitude();
    // dimensionless hysteresis adjustments
    approachSinAngle *= data->doingBrace ? 1.3f : 0.7f;
    // don't brace because feet will be able to do that job
    if (fabsf(approachSinAngle) < 0.45f && pelvisToChest.dot(braceDir) < 0.0f)
    {
      data->doingBrace = false; // doingBrace used for hysteresis as in above
      return;
    }
    data->doingBrace = true;
  }

  // Rather than passing a position to each arm, a patch (which is a piece of geometry) is passed down to both arms
  // each arm now finds the closest point on this patch to aim for.
  NMP::Vector3 targetPos;
  const Environment::Patch& patch = in->getBraceHazard().patch;
  data->patchShapeID = patch.state.shapeID;

  // Check if character is already inside the shape.
  const bool isCentreOfShouldersInsideShape =
    (patch.getNearestPointInside(owner->in->getCentreOfShoulders(), targetPos, true) == 0);

  // Find brace target position.
  patch.nearestPoint(data->placePos, targetPos, !isCentreOfShouldersInsideShape);

  MR_DEBUG_DRAW_POINT(pDebugDrawInst, data->placePos, SCALE_DIST(0.1f), NMP::Colour::LIGHT_RED);
  MR_DEBUG_DRAW_POINT(pDebugDrawInst, targetPos, SCALE_DIST(0.05f), NMP::Colour::LIGHT_TURQUOISE);

  // Now initialise the helper class braceDynamic: we set up the approximate 'masses' the body mass
  // is about half the real mass (due to the softness of the body) you should think of these masses
  // as the force needed to get a unit acceleration from the chest, hand and the point on the hazard
  // respectively as such we use half the arm mass for the hand, and the incoming hazard mass.
  const float armLengthMaxScale = in->getMaxArmExtensionScaleImportance() ? in->getMaxArmExtensionScale() : 1.0f;
  const float armLengthMinScale = 0.2f;
  BraceDynamic dynamic;
  dynamic.init(owner->owner->data->totalMass * 0.5f,
    owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_dynamicState.mass * 0.5f, patch.state.mass);
  dynamic.maxReachDistance = owner->data->length * armLengthMaxScale;
  dynamic.minReachDistance = owner->data->length * armLengthMinScale;

  float armClampLength = owner->data->length * 100.0f; // No clamping
  // We do this only when scale<1 so that target can go outside of arm length when scale is 1
  // (considered as no clamping) so that the tuned brace behaviour isn't adversely effected
  if (armLengthMaxScale < 1.0f)
  {
    armClampLength = dynamic.maxReachDistance;
  }

  // This is the main body of work, using this helper class allows us to share the code
  // between arms and legs.
  // As a function it fills in the LimbControl structure
  applyBraceDynamic(dynamic, owner, in, data, owner->owner->data->armLimbSharedStates[owner->data->childIndex],
    out, in->getProtectState().position, targetPos, true,
    feedIn->getLimbInContactWithPatchShapeID(), armClampLength, pDebugDrawInst);
}

//----------------------------------------------------------------------------------------------------------------------
void ArmBraceFeedbackPackage::feedback(
  float NMP_UNUSED(timeStep), 
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  // this endCushionPoint is passed up, in order to provide an average cushion point, which feeds down as cushionPoint
  feedOut->setEndCushionPoint(owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_endTM.translation());

  // don't try and brace if hand is constrained
  if (owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_isConstrained)
  {
    return;
  }

  // if we are bracing or cushioning then we want a spine twist to help with the goal
  if (data->importance)
  {
    // we try to twist the spine
    NMP::Vector3 braceDirection = data->reachDirection;
    const ER::LimbTransform& rootTM = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootTM;

    // braceDirection is computed to have little vertical component, so the brace twists the spine, but
    // doesn't try and bend it too much
    const float removeVerticalAmount = 0.75f; // 1 would be remove all vertical component
    NMP::Vector3 mostlyFlattenedDirection =
      braceDirection - removeVerticalAmount * rootTM.upDirection() * rootTM.upDirection().dot(braceDirection);
    braceDirection = NMP::vNormalise(mostlyFlattenedDirection);

    // convert this braceDirection vector relative to the comfy reach direction, into a quat
    NMP::Quat rotateToBraceDir;
    rotateToBraceDir.forRotation(
      owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_reachLimit.getReachConeDir(), braceDirection);
    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta, 
      owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_reachLimit.m_transform.translation(), 
      owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_reachLimit.getReachConeDir(),
      NMP::Colour::YELLOW);
    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta, 
      owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_reachLimit.m_transform.translation(), 
      braceDirection, NMP::Colour::RED);

    // convert the rotation quat into a rotation request in order to achieve a spine twist
    RotationRequest request(rotateToBraceDir.toRotationVector(), SCALING_SOURCE);
#if defined(MR_OUTPUT_DEBUGGING)
    request.debugControlAmounts.setControlAmount(ER::interceptControl, 1.0f);
#endif
    request.imminence = data->imminence;
    if (in->getBraceScaleImportance())
      request.stiffnessScale = in->getBraceScale();

    feedOut->setRootRotationRequest(request);

    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_AngleDelta, rootTM.translation(), request.rotation, 
      NMP::Colour(ER::getDefaultColourForControl(ER::interceptControl)));
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ArmBrace::entry()
{
  data->doingBrace = false;
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

