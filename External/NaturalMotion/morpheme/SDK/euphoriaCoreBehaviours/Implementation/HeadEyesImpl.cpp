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
#include "HeadEyesPackaging.h"
#include "HeadEyes.h"
#include "HeadPackaging.h"
#include "MyNetworkPackaging.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void HeadEyesUpdatePackage::update(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  // Here we pass down the focal centre, it becomes what the character is
  // actually looking at, not what he wants to look at blend on a forwards gaze
  // if specific focus isn't specified
  float lookImportance = in->getTargetImportance();
  // weakly pull the eyes back to their centre of vision
  const float defaultGazeDistance = SCALE_DIST(4.0f);
  if (data->eyeVector.magnitudeSquared() == 0.0f)
  {
    data->eyeVector = owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_endTM.frontDirection()
      * defaultGazeDistance;
  }
  else
  {
    data->eyeVector +=
      (owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_endTM.frontDirection() * defaultGazeDistance - data->eyeVector) * 0.1f * (1.0f - lookImportance);
  }

  if (lookImportance != 0.0f)
  {
    const MyNetworkAPIBase base(owner->owner->data);
    const HeadAPIBase head(owner->data, owner->in, &base);

    float stiffness = owner->data->normalStiffness * 1.5f * in->getTarget().stiffnessScale;
    ProcessRequest process;
    NMP::Vector3 target = in->getTarget().target;
    // Rather than using the input importance we hard code importance of 1. This is because the
    // initial request does not have any accumulated urgency already... the eyes should take 100% of
    // this request. in order to adjust the strength of the look, you should change the target
    // imminence. Low will give a lesser response
    float importance = process.processRequest(1.0f, in->getTarget().imminence, stiffness);

    NMP::Vector3 dirToTarget = target -owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_endTM.translation();
    data->eyeVector += (dirToTarget - data->eyeVector) * importance;
    NMP::Vector3 lookDir = vNormalise(data->eyeVector);
    NMP::Vector3 lookConeDir =owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_reachLimit.getReachConeDir();
    float dot = lookDir.dot(lookConeDir);
    // the intended look target is out of range, so clamp the centre to this value
    if (dot < cosf(owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_reachLimit.m_angle))
    {
      NMP::Vector3 perp = vCross(lookConeDir, lookDir);
      float angle = atan2f(perp.normaliseGetLength(), dot); // should be bigger value than lookConeAngle
      perp *= angle -owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_reachLimit.m_angle; // this is the wrong angle, we need one for the eyes
      NMP::Quat rotate(-perp, false);
      data->eyeVector = rotate.rotateVector(data->eyeVector);
    }

    // based on lookOutOfRangeWeight, we decide whether or not to look at targets that are behind the character's head
#if defined(MR_OUTPUT_DEBUGGING)
    NMP::Vector3 col = ER::convertControlAmountsToDefaultColour(in->getTarget().debugControlAmounts);
#endif
    if (head.canReachPointTarget(target, data->doingPoint, in->getTarget().lookOutOfRangeWeight))
    {
      out->setLookTarget(in->getTarget(), process.importance);
      MR_DEBUG_DRAW_POINT(pDebugDrawInst, data->focalCentre, SCALE_DIST(0.2f), NMP::Colour(col, 255));
    }
    MR_DEBUG_DRAW_POINT(pDebugDrawInst, data->focalCentre, SCALE_DIST(0.4f), NMP::Colour(col, 255));
  }
  else
  {
    MR_DEBUG_DRAW_POINT(pDebugDrawInst, data->focalCentre, SCALE_DIST(0.4f), NMP::Colour::WHITE);
  }
  data->focalCentre = data->eyeVector 
    + owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_endTM.translation();

  data->focalRadius = data->eyeVector.magnitude() + SCALE_DIST(1.0f); // extra width as a buffer

  // if your behaviour doesn't need to see then just don't enable headEyes, but if you explicitly
  // want the eyes to be shut (and other behaviours are running), then set the shutEyes bool
  if (!in->getShutEyes())
  {
    out->setFocalCentre(data->focalCentre);
    out->setFocalRadius(data->focalRadius);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void HeadEyesFeedbackPackage::feedback(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  feedOut->setFocalCentre(data->focalCentre);
  feedOut->setFocalRadius(data->focalRadius);
  feedOut->setFocalDirection(NMP::vNormalise(data->eyeVector));

#if defined(MR_OUTPUT_DEBUGGING)
  // Note that these points are tuned for the NM reference character - it's useful to be able to see
  // a debug representation of the eyes.
  NMP::Vector3 eye1(0.1f, 0.0f, 0.035f);
  NMP::Vector3 eye2(0.1f, 0.0f, -0.035f);
  eye1 = SCALE_DIST(eye1);
  eye2 = SCALE_DIST(eye2);
 owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_endTM.transformVector(eye1);
 owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_endTM.transformVector(eye2);

  MR_DEBUG_DRAW_POINT(pDebugDrawInst, eye1, SCALE_DIST(0.02f), NMP::Colour::LIGHT_BLUE);
  MR_DEBUG_DRAW_POINT(pDebugDrawInst, eye2, SCALE_DIST(0.02f), NMP::Colour::LIGHT_BLUE);
  NMP::Vector3 target =owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_endTM.translation() + data->eyeVector;
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, eye1, NMP::vNormalise(target - eye1) * 0.1f, NMP::Colour::RED);
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, eye2, NMP::vNormalise(target - eye2) * 0.1f, NMP::Colour::RED);
#endif // defined(MR_OUTPUT_DEBUGGING)
}

//----------------------------------------------------------------------------------------------------------------------
void HeadEyes::entry()
{
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

