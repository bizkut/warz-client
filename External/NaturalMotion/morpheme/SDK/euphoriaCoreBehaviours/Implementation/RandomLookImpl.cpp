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
#include "RandomLookPackaging.h"
#include "RandomLook.h"
#include "MyNetwork.h"
#include "euphoria/erDebugDraw.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
// RandomLookUpdatePackage
//----------------------------------------------------------------------------------------------------------------------
void RandomLookUpdatePackage::update(float timeStep, MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  if (in->getRandomLookParametersImportance() > 0.0f)
  {
    const RandomLookParameters& rlp = in->getRandomLookParameters();

    ER::LimbTransform uprightTM;
    uprightTM.createFromUpAlignedByForward(
      owner->data->up, owner->data->spineLimbSharedStates[0].m_rootTM.frontDirection(), feedIn->getHeadPosition());

    // calculate look target
    data->lookTargetTimer -= timeStep;
    if (data->lookTargetTimer < 0.0f)
    {
      // Vary the times so multiple characters don't all synchronise!
      float timeScale = 0.5f + data->rng.genFloat();
      data->lookTargetTimer = SCALE_TIME(3.0f) * rlp.lookTimescale * timeScale;
      float ry = data->rng.genFloat();
      float rz = data->rng.genFloat();
      ry = ry * 2.0f - 1.0f;
      rz = rz * 2.0f - 1.0f;

      // The head limb locator is always such that x is forwards, so construct a quaternion based on
      // random rotations within the authored ranges and rotate a forwards vector. 
      NMP::Quat q;
      q.fromEulerXYZ(NMP::Vector3(0.0f, ry * rlp.lookRangeSideways, rz * rlp.lookRangeUpDown));
      q = uprightTM.toQuat() * q;
      data->lookTarget = feedIn->getHeadPosition() +
        q.rotateVector(NMP::Vector3(rlp.lookFocusDistance, 0.0f, 0.0f)) +
        owner->data->up * rlp.lookVerticalOffset;

      if (data->needToInitialise)
      {
        data->needToInitialise = false;
        data->lastLookTarget = data->lookTarget;
      }
    }

    // Smooth the transition between look targets.
    NMP::SmoothCD(data->lastLookTarget, data->lastLookTargetRate, timeStep, data->lookTarget, rlp.lookTransitionTime);
    TargetRequest lookRequest(data->lastLookTarget, SCALING_SOURCE);
    lookRequest.passOnAmount = rlp.lookWithWholeBody;
    out->setLookTarget(lookRequest, rlp.lookAmount);
    MR_DEBUG_DRAW_POINT(pDebugDrawInst, data->lastLookTarget, SCALE_DIST(0.5f), NMP::Colour::RED);
  }
  else
  {
    data->needToInitialise = true;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void RandomLook::entry()
{
  data->rng.setSeed(owner->getAndBumpNetworkRandomSeed());
  data->lookTargetTimer = SCALE_TIME(-1.0f);
  data->lookTarget.setToZero();
  data->lastLookTarget.setToZero();
  data->lastLookTargetRate.setToZero();
  data->needToInitialise = true;
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

