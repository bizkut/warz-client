// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrCommonTasks.h"
#include "morpheme/mrNetwork.h"
#include "physics/mrPhysicsRig.h"
#include "physics/mrPhysicsAttribData.h"
#include "morpheme/mrBlendOps.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
#define OUTPUT_DEAD_TRANSFORMS (0)

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Quat uncorrectedSlerp(const NMP::Quat& qFrom, const NMP::Quat& qTo, float t)
{
  float x, y, z, w;
  float angle, s, startWeight, endWeight;

  float fromDotTo = qFrom.dot(qTo);
  if ((1.0f - fabs(fromDotTo)) > QUATERNION_SLERP_LERP_ERROR_LIMIT)
  {
    // Source quaternions are different enough to perform a slerp.
    // Also fastArccos becomes inaccurate as c tends towards 1.
    angle = NMP::fastArccos(fromDotTo);
    s = NMP::fastSin(angle);
    startWeight = NMP::fastSin((1.0f - t) * angle) / s;
    endWeight = NMP::fastSin(t * angle) / s;
  }
  else
  {
    // Source quaternions are almost the same and slerping becomes inaccurate so do a lerp instead.
    startWeight = 1.0f - t;
    endWeight = t;
  }

  x = startWeight * qFrom.x + endWeight * qTo.x;
  y = startWeight * qFrom.y + endWeight * qTo.y;
  z = startWeight * qFrom.z + endWeight * qTo.z;
  w = startWeight * qFrom.w + endWeight * qTo.w;
  NMP::Quat result(x, y, z, w);

  // Normalising is necessary as fastArccos is not accurate enough and because the lerp path does not produce
  // normal results when sources are too different.
  result.fastNormalise();
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU

namespace 
{

//----------------------------------------------------------------------------------------------------------------------
void subTaskPhysicalDeadBlendTransformBuffs(
  AttribDataTransformBuffer* destTransforms,
  AttribDataDeadBlendState* deadBlendState,
  AttribDataDeadBlendDef* deadBlendDef,
  AttribDataUpdatePlaybackPos* deltaTime,
  AttribDataBlendWeights* blendWeights,
  AttribDataTransformBuffer* deadBlendRates,
  AttribDataTransformBuffer* deadBlendTransformsState,
  AttribDataTransformBuffer* transformsAttr,
  AttribDataPhysicsRig* physicsRigAttrib,
  AttribDataAnimToPhysicsMap* animToPhysics)
{
  NMP::DataBuffer* transforms = transformsAttr->m_transformBuffer;

  NMP::Quat origRootOrientation(NMP::Quat::kIdentity);
  float dt = deltaTime->m_value;
  int32_t rootPartIndex = -1;

  // TODO note that this is extremely inefficient when there's no blend to physics, since the
  // integration of the physics bones (with the expensive joint limit clamping) gets ignored.
  if (physicsRigAttrib)
  {
    // physically limited integration (physics dead blend)
    NMP_ASSERT(animToPhysics);
    MR::PhysicsRig* physicsRig = physicsRigAttrib->m_physicsRig;
    rootPartIndex = animToPhysics->m_animToPhysicsMap->getAnimIndexFromPhysicsIndex(physicsRig->getRootPartIndex());
    NMP_ASSERT(rootPartIndex >= 0);
    origRootOrientation = *deadBlendTransformsState->m_transformBuffer->getPosQuatChannelQuat(rootPartIndex);

    physicsRig->integrateAnimation(
      *(deadBlendTransformsState->m_transformBuffer),
      *(deadBlendRates->m_transformBuffer),
      dt,
      deadBlendDef->m_deadReckoningAngularVelocityDamping,
      true);
  }
  else
  {
    // integration (animation dead blend)
    IntegrateTransforms(
      *(deadBlendTransformsState->m_transformBuffer),
      *(deadBlendRates->m_transformBuffer),
      dt, 
      deadBlendDef->m_deadReckoningAngularVelocityDamping);
  }

#if OUTPUT_DEAD_TRANSFORMS
  // nothing to do
  (void) blendWeights;
  (void) transforms;
  (void) deadBlendTransformsState;
  deadBlendTransformsState->m_transformBuffer->copyTo(transforms);
#else // OUTPUT_DEAD_TRANSFORMS

  // If we are going from physics to animation, we need to blend the
  // result of the dead reckoning with the destination transforms buffer.
  // The same must happen if going to physics and a blend is required.
  // Note that if there is no anim to physics map, we also perform a straight blend.
  if (!deadBlendState->m_isBlendingToPhysics || deadBlendDef->m_blendToDestinationPhysicsBones || animToPhysics == NULL)
  {
    NMP::DataBuffer* sourceBuffer0 = deadBlendTransformsState->m_transformBuffer;
    NMP::DataBuffer* sourceBuffer1 = destTransforms->m_transformBuffer;

    // Real blend 2
    switch (deadBlendState->m_blendMode)
    {
    case BLEND_MODE_ADD_ATT_ADD_POS:
      MR::BlendOps::addQuatAddPosPartial(
        transforms,
        sourceBuffer0,
        sourceBuffer1,
        blendWeights->m_trajectoryAndTransformsWeights[0]);
      break;
    case BLEND_MODE_ADD_ATT_LEAVE_POS:
      MR::BlendOps::addQuatLeavePosPartial(
        transforms,
        sourceBuffer0,
        sourceBuffer1,
        blendWeights->m_trajectoryAndTransformsWeights[0]);
      break;
    case BLEND_MODE_INTERP_ATT_ADD_POS:
      MR::BlendOps::interpQuatAddPosPartial(
        transforms,
        sourceBuffer0,
        sourceBuffer1,
        blendWeights->m_trajectoryAndTransformsWeights[0]);
      break;
    default:
      MR::BlendOps::interpQuatInterpPosPartial(
        transforms,
        sourceBuffer0,
        sourceBuffer1,
        blendWeights->m_trajectoryAndTransformsWeights[0]);
      break;
    }

    // Recalculate the hip bone to solve the problem that since this isn't clamped to limits, interpolating
    // between the dead-reckoned orientation and the target can quite often "flip" direction.
    // This will be basically the dead reckoning as above, but in addition we add on a term to make it
    // end up at the destination. Do this rather than blending to prevent angle popping as the shortest path flips -
    // even though we'll get velocity popping.
    if (deadBlendState->m_duration > 0.0f && (rootPartIndex >= 0))
    {
      float alpha = 1.0f - blendWeights->m_trajectoryAndTransformsWeights[0];
      float timeLeft = alpha * deadBlendState->m_duration; // not accurate, but never mind
      if (timeLeft > 0.0f)
      {
        NMP::Quat qCurrent = origRootOrientation;

        // redo the dead reckoning on the hips - but reduce the angular velocity over the blend time.
        NMP::Vector3 angVel = *deadBlendRates->m_transformBuffer->getPosVelAngVelChannelAngVel(rootPartIndex);
        angVel *= alpha;
        qCurrent += (NMP::Quat(angVel.x, angVel.y, angVel.z, 0.0f) * qCurrent) * (0.5f * dt);
        qCurrent.normalise();

        // now add the perturbation to take us to the target, and increase as the blend goes on.
        NMP::Quat qTarget = *sourceBuffer1->getPosQuatChannelQuat(rootPartIndex);
        // q will convert current into the target
        NMP::Quat q = qTarget * ~qCurrent;
        float frac = dt / timeLeft;
        frac = NMP::clampValue(frac, 0.0f, 1.0f);
        NMP::Quat qCorrection;
        qCorrection.slerp(NMP::Quat(NMP::Quat::kIdentity), q, frac);
        NMP::Quat qNew = qCorrection * qCurrent;
        deadBlendTransformsState->m_transformBuffer->setPosQuatChannelQuat(rootPartIndex, qNew);
        transforms->setPosQuatChannelQuat(rootPartIndex, qNew);
      }
    }
  }
  else
  {
    // The bonesFilter has 0 for the physics bones and 1 for non-physics. We always want to take the
    // destination physics bones, so flip the direction of the feather blend.
    float alpha = 1.0f - blendWeights->m_trajectoryAndTransformsWeights[0];
    NMP::DataBuffer* sourceBuffer0 = destTransforms->m_transformBuffer;
    NMP::DataBuffer* sourceBuffer1 = deadBlendTransformsState->m_transformBuffer;
    AttribDataFloatArray* bonesFilter = animToPhysics->m_animToPhysicsMap->m_bonesFilter;

    // Feather blend on the non physical channels.
    switch (deadBlendState->m_blendMode)
    {
    case BLEND_MODE_ADD_ATT_ADD_POS:
      MR::BlendOps::addQuatAddPosPartialFeathered(
        transforms,
        sourceBuffer0,
        sourceBuffer1,
        alpha,
        bonesFilter->m_numValues,
        bonesFilter->m_values);
      break;
    case BLEND_MODE_ADD_ATT_LEAVE_POS:
      MR::BlendOps::addQuatLeavePosPartialFeathered(
        transforms,
        sourceBuffer0,
        sourceBuffer1,
        alpha,
        bonesFilter->m_numValues,
        bonesFilter->m_values);
      break;
    case BLEND_MODE_INTERP_ATT_ADD_POS:
      MR::BlendOps::interpQuatAddPosPartialFeathered(
        transforms,
        sourceBuffer0,
        sourceBuffer1,
        alpha,
        bonesFilter->m_numValues,
        bonesFilter->m_values);
      break;
    default:
      MR::BlendOps::interpQuatInterpPosPartialFeathered(
        transforms,
        sourceBuffer0,
        sourceBuffer1,
        alpha,
        bonesFilter->m_numValues,
        bonesFilter->m_values);
      break;
    }
  }
#endif // OUTPUT_DEAD_TRANSFORMS
}

//----------------------------------------------------------------------------------------------------------------------
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
void TaskPhysicalDeadBlendTransformBuffs(Dispatcher::TaskParameters* parameters)
{
  AttribDataTransformBuffer* destTransforms = parameters->getInputAttrib<AttribDataTransformBuffer>(0, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  AttribDataDeadBlendState* deadBlendState = parameters->getInputAttrib<AttribDataDeadBlendState>(1, ATTRIB_SEMANTIC_DEAD_BLEND_STATE);
  AttribDataDeadBlendDef* deadBlendDef = parameters->getInputAttrib<AttribDataDeadBlendDef>(2, ATTRIB_SEMANTIC_DEAD_BLEND_DEF);
  AttribDataUpdatePlaybackPos* deltaTime = parameters->getInputAttrib<AttribDataUpdatePlaybackPos>(3, ATTRIB_SEMANTIC_UPDATE_TIME_POS);
  AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(4, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  NMP_ASSERT(blendWeights->m_trajectoryAndTransformsNumWeights == 1);
  const uint32_t numAnimationJoints = destTransforms->m_transformBuffer->getLength();
  // Dead blend rates - can get modified.
  AttribDataTransformBuffer* deadBlendRates = parameters->createOutputAttribReplaceTransformBuffer(5, ATTRIB_SEMANTIC_TRANSFORM_RATES, numAnimationJoints);
  // Create this frame's output attribute that will replace the last frames attribute when we copy back attrib.
  AttribDataTransformBuffer* deadBlendTransformsState = parameters->createOutputAttribReplaceTransformBuffer(6, ATTRIB_SEMANTIC_DEAD_BLEND_TRANSFORMS_STATE, numAnimationJoints);
  AttribDataTransformBuffer* transforms = parameters->createOutputAttribTransformBuffer(7, numAnimationJoints, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  // Optional physics rig and channel map
  AttribDataPhysicsRig* physicsRigAttrib = parameters->getOptionalInputAttrib<AttribDataPhysicsRig>(8, ATTRIB_SEMANTIC_PHYSICS_RIG);
  AttribDataAnimToPhysicsMap* animToPhysics = parameters->getOptionalInputAttrib<AttribDataAnimToPhysicsMap>(9, ATTRIB_SEMANTIC_ANIM_TO_PHYSICS_MAP);

  subTaskPhysicalDeadBlendTransformBuffs(
    destTransforms,
    deadBlendState,
    deadBlendDef,
    deltaTime,
    blendWeights,
    deadBlendRates,
    deadBlendTransformsState,
    transforms,
    physicsRigAttrib,
    animToPhysics);
}

#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void TaskFeatherBlendToPhysicsTransformBuffs(Dispatcher::TaskParameters* parameters)
{
  AttribDataDeadBlendState* blendState = parameters->getInputAttrib<AttribDataDeadBlendState>(0, ATTRIB_SEMANTIC_DEAD_BLEND_STATE);
  AttribDataTransformBuffer* animationTransforms = parameters->getInputAttrib<AttribDataTransformBuffer>(1, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  AttribDataTransformBuffer* physicsTransforms = parameters->getInputAttrib<AttribDataTransformBuffer>(2, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(3, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  NMP_ASSERT(blendWeights->m_trajectoryAndTransformsNumWeights == 1);

  AttribDataAnimToPhysicsMap* animToPhysics = parameters->getInputAttrib<AttribDataAnimToPhysicsMap>(4, ATTRIB_SEMANTIC_ANIM_TO_PHYSICS_MAP);
  AttribDataFloatArray* bonesFilter = animToPhysics->m_animToPhysicsMap->m_bonesFilter;

  // Create this frame's output attribute that will replace the last frames attribute when we copy back attrib.
  const uint32_t numAnimationJoints = animationTransforms->m_transformBuffer->getLength();

  AttribDataTransformBuffer* transforms = parameters->createOutputAttribTransformBuffer(5, numAnimationJoints, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);

  NMP::DataBuffer* sourceBuffer0 = physicsTransforms->m_transformBuffer;
  NMP::DataBuffer* sourceBuffer1 = animationTransforms->m_transformBuffer;

  float alpha = 1.0f - blendWeights->m_trajectoryAndTransformsWeights[0];

  // Feather blend on the non physical channels.
  switch (blendState->m_blendMode)
  {
  case BLEND_MODE_ADD_ATT_ADD_POS:
    MR::BlendOps::addQuatAddPosPartialFeathered(
      transforms->m_transformBuffer,
      sourceBuffer0,
      sourceBuffer1,
      alpha,
      bonesFilter->m_numValues,
      bonesFilter->m_values);
    break;
  case BLEND_MODE_ADD_ATT_LEAVE_POS:
    MR::BlendOps::addQuatLeavePosPartialFeathered(
      transforms->m_transformBuffer,
      sourceBuffer0,
      sourceBuffer1,
      alpha,
      bonesFilter->m_numValues,
      bonesFilter->m_values);
    break;
  case BLEND_MODE_INTERP_ATT_ADD_POS:
    MR::BlendOps::interpQuatAddPosPartialFeathered(
      transforms->m_transformBuffer,
      sourceBuffer0,
      sourceBuffer1,
      alpha,
      bonesFilter->m_numValues,
      bonesFilter->m_values);
    break;
  default:
    MR::BlendOps::interpQuatInterpPosPartialFeathered(
      transforms->m_transformBuffer,
      sourceBuffer0,
      sourceBuffer1,
      alpha,
      bonesFilter->m_numValues,
      bonesFilter->m_values);
    break;
  }
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
