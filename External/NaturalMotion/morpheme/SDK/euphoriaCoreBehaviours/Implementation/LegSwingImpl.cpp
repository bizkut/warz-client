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
#include "LegSwingPackaging.h"
#include "LegSwing.h"
#include "MyNetworkPackaging.h"
#include "types/processRequest.h"

#include "euphoria/erDebugDraw.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{
//----------------------------------------------------------------------------------------------------------------------
void LegSwingUpdatePackage::update(float timeStep, MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
#if defined(TEST_LEG_SWING)
  // Will swing to keep pelvis upright, at imminence 2, should have approximately full importance but no more than normalStiffness
  RotationRequest& req = (RotationRequest&)in->getRotationRequest();
  NMP::Vector3 chestUp = owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_rootTM.upDirection();
  NMP::Quat quat;
  quat.forRotation(chestUp, owner->owner->data->up);
  req.rotation = quat.toRotationVector();
  req.imminence = 2.0f;
  req.stiffnessScale = 2.0f;
  float& imp = (float&)in->getRotationRequestImportanceRef();
  imp = 1.0f;
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_AngleDelta,
    owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_rootTM.translation(),
    req.rotation, NMP::Colour::ORANGE);
#endif

  if (in->getRotationRequestImportance() == 0.0f)
  {
    return;
  }

  // draw what is asked of the hips
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_AngleDelta,
    owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_rootTM.translation(),
    in->getRotationRequest().rotation, 
    NMP::Colour(ER::convertControlAmountsToDefaultColour(in->getRotationRequest().debugControlAmounts)));

  if (owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_endContact.inContact
      && !in->getSpineInContact())
  {
    return;
  }
  float stiffnessScale = 1.0f;
  float importanceScale = 1.0f;
  if (in->getSwingStiffnessScaleImportance())
  {
    stiffnessScale = in->getSwingStiffnessScale();
    if (stiffnessScale == 0.0f)
    {
      return; // no swing
    }
    importanceScale = NMP::clampValue(stiffnessScale, 0.0f, 1.0f);
  }

  // Extra measure to control how long character runs swing for after touching the ground.
  // Add spineInContact hysteresis to smooth out contact information and
  // eliminate re-enabling of the legs swing instantly after the contact was lost for a single frame.

  // Continuously register chest collision information (0 - no collision, 1 - collision) as uint8_t variable bits.
  // This provides continuous collision information history from last 8 frames.
  data->chestContactInfoHysteresis = data->chestContactInfoHysteresis << 1;

  // Register a collision on contact by setting the least significant bit from 0 (above bit shift) to 1,
  // otherwise leave 0.
  if (in->getSpineInContact())
  {
    data->chestContactInfoHysteresis |= 1;
    data->timeLegsOrSpineCollided += timeStep;
  }

  // Read collision history looking for continuous string of zeros
  // for a given number of frames.
  uint8_t historyLength = 8;
  uint8_t hysteresis = ~(data->chestContactInfoHysteresis);
  while ((hysteresis & 1) && historyLength)
  {
    hysteresis = hysteresis >> 1;
    --historyLength;
  }

  // Reset timer and re-enable legs swing when chest collision was not detected for historyLength number of consecutive frames.
  data->timeLegsOrSpineCollided = (historyLength == 0) ? (0.0f) : (data->timeLegsOrSpineCollided);

  // The idea of this module is to control the root orientation and angular velocity by waving the legs around (they
  // simply swing in the opposite direction to the spring-damper like torque)
  const float collidedAbortTime = in->getMaxSwingTimeOnGroundImportance() ? in->getMaxSwingTimeOnGround() : 1.0f;
  if (data->timeLegsOrSpineCollided < collidedAbortTime && owner->data->normalStiffness > 0.001f)
  {
    ProcessRequest process;
    const float stiffnessReduction = 0.8f; // this is an approximation of the leg inertia / the pelvis or lower body inertia
    // calculate importance with which to swing leg based on urgency and leg stiffness
    float importance = importanceScale * process.processRequest(
      in->getRotationRequestImportance(),
      in->getRotationRequest().imminence,
      owner->data->normalStiffness * stiffnessReduction,
      1.0f,
      owner->owner->data->minStiffnessForActing * stiffnessReduction,
      owner->owner->data->normalStiffness * in->getRotationRequest().stiffnessScale * stiffnessReduction);

    if (!importance)
    {
      return;
    }

    float stiffness = owner->data->normalStiffness * process.stiffnessScale;
    // 1. get target rotation acc / sqr(stiffness)
    float speedScale = 1.0f;
    NMP::Vector3 angleDelta = in->getRotationRequest().rotation;
    const float eps = 1e-10f;
    angleDelta -= owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_rootData.angularVelocity
      / (eps + in->getRotationRequest().imminence);

    NMP::Vector3 rotation = speedScale * angleDelta;
    NMP::Vector3 pivot = owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_rootTM.translation();
    NMP::Vector3 toFoot = owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_endTM.translation() - pivot;

    // 2. shift foot laterally to current direction... this spins it around and out
    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta,
      owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_endTM.translation(),
      vCross(toFoot, in->getRotationRequest().rotation), NMP::Colour::YELLOW);
    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Velocity,
      owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_endTM.translation(),
      vCross(toFoot, -owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_rootData.angularVelocity
      / (eps + in->getRotationRequest().imminence)), NMP::Colour::YELLOW);

    NMP::Vector3 handShift = vCross(toFoot, rotation);
    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal,
      owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_endTM.translation(), handShift, 
      NMP::Colour(ER::convertControlAmountsToDefaultColour(in->getRotationRequest().debugControlAmounts)));
    toFoot += handShift;

    // 3. pull foot in to centre of ellipse, this means squash, clamp to circle, unsquash
    float toFootDown = toFoot.dot(owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_rootTM.upDirection());
    float squash = 0.7f;
    toFoot += owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_rootTM.upDirection()
      * (squash * toFootDown - toFootDown);
    toFoot.normalise();
    toFoot *= owner->data->length * squash;
    toFootDown = toFoot.dot(owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_rootTM.upDirection());
    toFoot += owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_rootTM.upDirection()
      * (toFootDown / squash - toFootDown);
    NMP::Vector3 footTarget = pivot + toFoot;
    MR_DEBUG_DRAW_LINE(pDebugDrawInst, pivot, footTarget, NMP::Colour::TURQUOISE);

    LimbControl& control = out->startControlModification();

    control.reinit(stiffness * stiffnessScale, 0.2f, owner->data->normalDriveCompensation);
    control.setTargetPos(footTarget, 1);
    control.setControlAmount(ER::swingControl, 1.0f);
    out->stopControlModification(importance);
    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_rootTM,
      SCALE_DIST(2.5f));
  }
}

//----------------------------------------------------------------------------------------------------------------------
void LegSwing::entry()
{
  data->chestContactInfoHysteresis = 0;
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

