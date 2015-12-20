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
#include "morpheme/mrBlendOps.h"
#include "morpheme/mrRig.h"

#ifdef NM_HOST_CELL_SPU
  #include "morphemeSPU/mrDispatcherSPU.h"
  #include "NMPlatform/ps3/NMSPUDMAController.h"
#endif // NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------

#define MR_BLEND_TRANSFORM_TASKS_FORCEINLINE NM_FORCEINLINE

namespace MR
{
//----------------------------------------------------------------------------------------------------------------------
// Blend2 tasks
//----------------------------------------------------------------------------------------------------------------------
typedef void (*Blend2TransformBlendFunc)(
  NMP::DataBuffer*,
  const NMP::DataBuffer*,
  const NMP::DataBuffer*,
  const float);

typedef void (*Blend2TrajectoryBlendFunc)(
  NMP::Vector3* pos,
  NMP::Quat* att,
  bool* filteredOut,
  const NMP::Vector3* source0Pos,
  const NMP::Quat* source0Att,
  bool source0Used,
  const NMP::Vector3* source1Pos,
  const NMP::Quat* source1Att,
  bool source1Used,
  float blendWeight,
  const AttribDataFeatherBlend2ChannelAlphas* boneWeights);

//----------------------------------------------------------------------------------------------------------------------
template <Blend2TransformBlendFunc _TransformBlendFunc>
MR_BLEND_TRANSFORM_TASKS_FORCEINLINE
static void Blend2TransformBuffs(Dispatcher::TaskParameters* parameters)
{
  AttribDataTransformBuffer* source0Transforms = parameters->getInputAttrib<AttribDataTransformBuffer>(1, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  AttribDataTransformBuffer* source1Transforms = parameters->getInputAttrib<AttribDataTransformBuffer>(2, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(3, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  NMP_ASSERT(blendWeights->m_trajectoryAndTransformsNumWeights == 1);

  const uint32_t numRigJoints = source0Transforms->m_transformBuffer->getLength();
  AttribDataTransformBuffer* transforms = parameters->createOutputAttribTransformBuffer(0, numRigJoints, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);

#ifdef NMP_ENABLE_ASSERTS
  const uint32_t numRigJoints1 = source1Transforms->m_transformBuffer->getLength();
  NMP_ASSERT(numRigJoints == numRigJoints1);
#endif

  // Perform the actual blend.
  _TransformBlendFunc(
    transforms->m_transformBuffer,
    source0Transforms->m_transformBuffer,
    source1Transforms->m_transformBuffer,
    blendWeights->m_trajectoryAndTransformsWeights[0]);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TransformBuffsAddAttAddPos(Dispatcher::TaskParameters* parameters)
{
  Blend2TransformBuffs
  <BlendOps::addQuatAddPosPartial>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TransformBuffsSubtractAttSubtractPos(Dispatcher::TaskParameters* parameters)
{
  Blend2TransformBuffs
    <BlendOps::subtractQuatSubtractPosPartial>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TransformBuffsAddAttInterpPos(Dispatcher::TaskParameters* parameters)
{
  Blend2TransformBuffs
  <BlendOps::addQuatLeavePosPartial>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TransformBuffsInterpAttAddPos(Dispatcher::TaskParameters* parameters)
{
  Blend2TransformBuffs
  <BlendOps::interpQuatAddPosPartial>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TransformBuffsInterpAttInterpPos(Dispatcher::TaskParameters* parameters)
{
  Blend2TransformBuffs
  <BlendOps::interpQuatInterpPosPartial>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
// Blend 2 trajectory and transform functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NullBlend2TrajectoryFunc(
  NMP::Vector3* NMP_UNUSED(pos),
  NMP::Quat* NMP_UNUSED(attitude),
  bool* NMP_UNUSED(filteredOut),
  const NMP::Vector3* NMP_UNUSED(source0Pos),
  const NMP::Quat* NMP_UNUSED(source0Att),
  bool NMP_UNUSED(source0Used),
  const NMP::Vector3* NMP_UNUSED(source1Pos),
  const NMP::Quat* NMP_UNUSED(source1Att),
  bool NMP_UNUSED(source1Used),
  const float NMP_UNUSED(blendWeight),
  const AttribDataFeatherBlend2ChannelAlphas* NMP_UNUSED(boneWeights))
{
}

//----------------------------------------------------------------------------------------------------------------------
template <Blend2TransformBlendFunc TransformBlendFunc, Blend2TrajectoryBlendFunc TrajectoryBlendFunc>
MR_BLEND_TRANSFORM_TASKS_FORCEINLINE
static void Blend2TrajectoryDeltaAndTransforms(Dispatcher::TaskParameters* parameters)
{
  AttribDataTransformBuffer* source0Transforms = parameters->getInputAttrib<AttribDataTransformBuffer>(1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
  AttribDataTransformBuffer* source1Transforms = parameters->getInputAttrib<AttribDataTransformBuffer>(2, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
  AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(3, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  NMP_ASSERT(blendWeights->m_trajectoryAndTransformsNumWeights == 1);
  const uint32_t numRigJoints = source0Transforms->m_transformBuffer->getLength();
  NMP_ASSERT(numRigJoints == source1Transforms->m_transformBuffer->getLength());
  AttribDataTransformBuffer* transforms = parameters->createOutputAttribTransformBuffer(0, numRigJoints, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);

  // Perform the actual blend.
  TransformBlendFunc(
    transforms->m_transformBuffer,
    source0Transforms->m_transformBuffer,
    source1Transforms->m_transformBuffer,
    blendWeights->m_trajectoryAndTransformsWeights[0]);

  bool trajectoryFilteredOut = !transforms->m_transformBuffer->hasChannel(0);

  TrajectoryBlendFunc(
    transforms->m_transformBuffer->getPosQuatChannelPos(0), // OUT
    transforms->m_transformBuffer->getPosQuatChannelQuat(0), // OUT
    &trajectoryFilteredOut, // OUT
    source0Transforms->m_transformBuffer->getPosQuatChannelPos(0),
    source0Transforms->m_transformBuffer->getPosQuatChannelQuat(0),
    !source0Transforms->m_transformBuffer->hasChannel(0),
    source1Transforms->m_transformBuffer->getPosQuatChannelPos(0),
    source1Transforms->m_transformBuffer->getPosQuatChannelQuat(0),
    !source1Transforms->m_transformBuffer->hasChannel(0),
    blendWeights->m_trajectoryAndTransformsWeights[0],
    NULL);

  transforms->m_transformBuffer->setChannelUsedFlag(0, !trajectoryFilteredOut);
}

//----------------------------------------------------------------------------------------------------------------------
template <Blend2TransformBlendFunc TransformBlendFunc>
MR_BLEND_TRANSFORM_TASKS_FORCEINLINE
static void Blend2TransformsPassDestTraj(Dispatcher::TaskParameters* parameters)
{
  AttribDataTransformBuffer* source0Transforms = parameters->getInputAttrib<AttribDataTransformBuffer>(1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
  AttribDataTransformBuffer* source1Transforms = parameters->getInputAttrib<AttribDataTransformBuffer>(2, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
  AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(3, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  NMP_ASSERT(blendWeights->m_trajectoryAndTransformsNumWeights == 1);

  const uint32_t numRigJoints = source0Transforms->m_transformBuffer->getLength();
  NMP_ASSERT(numRigJoints == source1Transforms->m_transformBuffer->getLength());
  AttribDataTransformBuffer* transforms = parameters->createOutputAttribTransformBuffer(0, numRigJoints, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);

  // Perform the actual blend.
  TransformBlendFunc(
    transforms->m_transformBuffer,
    source0Transforms->m_transformBuffer,
    source1Transforms->m_transformBuffer,
    blendWeights->m_trajectoryAndTransformsWeights[0]);

  transforms->m_transformBuffer->setChannelPos(0, *source1Transforms->m_transformBuffer->getPosQuatChannelPos(0));
  transforms->m_transformBuffer->setChannelQuat(0, *source1Transforms->m_transformBuffer->getPosQuatChannelQuat(0));
  transforms->m_transformBuffer->setChannelUsedFlag(0, source1Transforms->m_transformBuffer->hasChannel(0));
}

//----------------------------------------------------------------------------------------------------------------------
template <Blend2TransformBlendFunc TransformBlendFunc>
MR_BLEND_TRANSFORM_TASKS_FORCEINLINE
static void Blend2TransformsPassSourceTraj(Dispatcher::TaskParameters* parameters)
{
  AttribDataTransformBuffer* source0Transforms = parameters->getInputAttrib<AttribDataTransformBuffer>(1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
  AttribDataTransformBuffer* source1Transforms = parameters->getInputAttrib<AttribDataTransformBuffer>(2, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
  AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(3, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  NMP_ASSERT(blendWeights->m_trajectoryAndTransformsNumWeights == 1);

  const uint32_t numRigJoints = source0Transforms->m_transformBuffer->getLength();
  NMP_ASSERT(numRigJoints == source1Transforms->m_transformBuffer->getLength());
  AttribDataTransformBuffer* transforms = parameters->createOutputAttribTransformBuffer(0, numRigJoints, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);

  // Perform the actual blend.
  TransformBlendFunc(
    transforms->m_transformBuffer,
    source0Transforms->m_transformBuffer,
    source1Transforms->m_transformBuffer,
    blendWeights->m_trajectoryAndTransformsWeights[0]);

  transforms->m_transformBuffer->setChannelPos(0, *source0Transforms->m_transformBuffer->getPosQuatChannelPos(0));
  transforms->m_transformBuffer->setChannelQuat(0, *source0Transforms->m_transformBuffer->getPosQuatChannelQuat(0));
  transforms->m_transformBuffer->setChannelUsedFlag(0, source0Transforms->m_transformBuffer->hasChannel(0));
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TrajectoryAndTransformsAddAttAddPosSlerpTraj(Dispatcher::TaskParameters* parameters)
{
  Blend2TrajectoryDeltaAndTransforms<
    BlendOps::addQuatAddPosPartial, 
    Blend2TrajectoryDeltaTransforms<BLEND2_WEIGHTING_PER_RIG, BlendOps::additiveBlendQuats, BLEND2_SLERP_POS>
    >(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TrajectoryAndTransformsAddAttInterpPosSlerpTraj(Dispatcher::TaskParameters* parameters)
{
  Blend2TrajectoryDeltaAndTransforms<
    BlendOps::addQuatLeavePosPartial, 
    Blend2TrajectoryDeltaTransforms<BLEND2_WEIGHTING_PER_RIG, BlendOps::additiveBlendQuats, BLEND2_SLERP_POS>
    >(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TrajectoryAndTransformsInterpAttAddPosSlerpTraj(Dispatcher::TaskParameters* parameters)
{
  Blend2TrajectoryDeltaAndTransforms<
    BlendOps::interpQuatAddPosPartial, 
    Blend2TrajectoryDeltaTransforms<BLEND2_WEIGHTING_PER_RIG, BlendOps::interpBlendQuats, BLEND2_SLERP_POS>
    >(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TrajectoryAndTransformsInterpAttInterpPosSlerpTraj(Dispatcher::TaskParameters* parameters)
{
  Blend2TrajectoryDeltaAndTransforms<
    BlendOps::interpQuatInterpPosPartial, 
    Blend2TrajectoryDeltaTransforms<BLEND2_WEIGHTING_PER_RIG, BlendOps::interpBlendQuats, BLEND2_SLERP_POS>
    >(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TrajectoryAndTransformsSubtractAttSubtractPosSlerpTraj(Dispatcher::TaskParameters* parameters)
{
  Blend2TrajectoryDeltaAndTransforms<
    BlendOps::subtractQuatSubtractPosPartial, 
    Blend2TrajectoryDeltaTransforms<BLEND2_WEIGHTING_PER_RIG, BlendOps::subtractiveBlendQuats, BLEND2_SLERP_POS>
    >(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TrajectoryAndTransformsSubtractAttInterpPosSlerpTraj(Dispatcher::TaskParameters* parameters)
{
  Blend2TrajectoryDeltaAndTransforms<
    BlendOps::subtractQuatInterpPosPartial, 
    Blend2TrajectoryDeltaTransforms<BLEND2_WEIGHTING_PER_RIG, BlendOps::subtractiveBlendQuats, BLEND2_SLERP_POS>
    >(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TrajectoryAndTransformsInterpAttSubtractPosSlerpTraj(Dispatcher::TaskParameters* parameters)
{
  Blend2TrajectoryDeltaAndTransforms<
    BlendOps::interpQuatSubtractPosPartial, 
    Blend2TrajectoryDeltaTransforms<BLEND2_WEIGHTING_PER_RIG, BlendOps::interpBlendQuats, BLEND2_SLERP_POS>
    >(parameters);
}

//---------------------------------------------------------------------------------------------------------------------
void TaskBlend2TrajectoryAndTransformsAddAttAddPosInterpTraj(Dispatcher::TaskParameters* parameters)
{
  // No need for a trajectory blending function because it will be correctly handled by the transform blending function.
  Blend2TrajectoryDeltaAndTransforms<
    BlendOps::addQuatAddPosPartial, 
    NullBlend2TrajectoryFunc
    >(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TrajectoryAndTransformsAddAttInterpPosInterpTraj(Dispatcher::TaskParameters* parameters)
{
  // No need for a trajectory blending function because it will be correctly handled by the transform blending function.
  Blend2TrajectoryDeltaAndTransforms<
    BlendOps::addQuatLeavePosPartial, 
    NullBlend2TrajectoryFunc
    >(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TrajectoryAndTransformsInterpAttAddPosInterpTraj(Dispatcher::TaskParameters* parameters)
{
  // No need for a trajectory blending function because it will be correctly handled by the transform blending function.
  Blend2TrajectoryDeltaAndTransforms<
    BlendOps::interpQuatAddPosPartial, 
    NullBlend2TrajectoryFunc
    >(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TrajectoryAndTransformsInterpAttInterpPosInterpTraj(Dispatcher::TaskParameters* parameters)
{
  // No need for a trajectory blending function because it will be correctly handled by the transform blending function.
  Blend2TrajectoryDeltaAndTransforms<
    BlendOps::interpQuatInterpPosPartial, 
    NullBlend2TrajectoryFunc
    >(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TrajectoryAndTransformsSubtractAttSubtractPosInterpTraj(Dispatcher::TaskParameters* parameters)
{
  // No need for a trajectory blending function because it will be correctly handled by the transform blending function.
  Blend2TrajectoryDeltaAndTransforms<
    BlendOps::subtractQuatSubtractPosPartial, 
    NullBlend2TrajectoryFunc
    >(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TransformsAddAttAddPosPassDestTraj(Dispatcher::TaskParameters* parameters)
{
  Blend2TransformsPassDestTraj<BlendOps::addQuatAddPosPartial>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TransformsAddAttInterpPosPassDestTraj(Dispatcher::TaskParameters* parameters)
{
  Blend2TransformsPassDestTraj<BlendOps::addQuatLeavePosPartial>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TransformsInterpAttAddPosPassDestTraj(Dispatcher::TaskParameters* parameters)
{
  Blend2TransformsPassDestTraj<BlendOps::interpQuatAddPosPartial>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TransformsInterpAttInterpPosPassDestTraj(Dispatcher::TaskParameters* parameters)
{
  Blend2TransformsPassDestTraj<BlendOps::interpQuatInterpPosPartial>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TransformsAddAttAddPosPassSourceTraj(Dispatcher::TaskParameters* parameters)
{
  Blend2TransformsPassSourceTraj<BlendOps::addQuatAddPosPartial>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TransformsAddAttInterpPosPassSourceTraj(Dispatcher::TaskParameters* parameters)
{
  Blend2TransformsPassSourceTraj<BlendOps::addQuatLeavePosPartial>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TransformsInterpAttAddPosPassSourceTraj(Dispatcher::TaskParameters* parameters)
{
  Blend2TransformsPassSourceTraj<BlendOps::interpQuatAddPosPartial>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2TransformsInterpAttInterpPosPassSourceTraj(Dispatcher::TaskParameters* parameters)
{
  Blend2TransformsPassSourceTraj<BlendOps::interpQuatInterpPosPartial>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
// Feather blend2 tasks
//----------------------------------------------------------------------------------------------------------------------
typedef void (*FeatherBlend2TransformBuffsFunc)(
  NMP::DataBuffer*       destBuffer,
  const NMP::DataBuffer* sourceBuffer0,
  const NMP::DataBuffer* sourceBuffer1,
  const float            alpha,
  const uint32_t         numAlphaValues,
  const float*           alphaValues);

//----------------------------------------------------------------------------------------------------------------------
template <FeatherBlend2TransformBuffsFunc _TransformBlendFunc>
MR_BLEND_TRANSFORM_TASKS_FORCEINLINE
void FeatherBlend2TransformBuffs(Dispatcher::TaskParameters* parameters)
{
  AttribDataTransformBuffer* source0Transforms = parameters->getInputAttrib<AttribDataTransformBuffer>(1, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  AttribDataTransformBuffer* source1Transforms = parameters->getInputAttrib<AttribDataTransformBuffer>(2, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(3, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  NMP_ASSERT(blendWeights->m_trajectoryAndTransformsNumWeights == 1);
  AttribDataFeatherBlend2ChannelAlphas* boneWeights = parameters->getInputAttrib<AttribDataFeatherBlend2ChannelAlphas>(4, ATTRIB_SEMANTIC_BONE_WEIGHTS);
  const uint32_t numRigJoints = source0Transforms->m_transformBuffer->getLength();
  AttribDataTransformBuffer* transforms = parameters->createOutputAttribTransformBuffer(0, numRigJoints, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);

#ifdef NMP_ENABLE_ASSERTS
  const uint32_t numRigJoints1 = source1Transforms->m_transformBuffer->getLength();
  NMP_ASSERT(numRigJoints == numRigJoints1);
#endif

  // Perform the actual blend.
  _TransformBlendFunc(
    transforms->m_transformBuffer,
    source0Transforms->m_transformBuffer,
    source1Transforms->m_transformBuffer,
    blendWeights->m_trajectoryAndTransformsWeights[0],
    boneWeights->m_numChannelAlphas,
    boneWeights->m_channelAlphas);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskFeatherBlend2TransformBuffsAddAttAddPos(Dispatcher::TaskParameters* parameters)
{
  FeatherBlend2TransformBuffs
    <BlendOps::addQuatAddPosPartialFeathered>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskFeatherBlend2TransformBuffsAddAttInterpPos(Dispatcher::TaskParameters* parameters)
{
  FeatherBlend2TransformBuffs
    <BlendOps::addQuatLeavePosPartialFeathered>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskFeatherBlend2TransformBuffsInterpAttAddPos(Dispatcher::TaskParameters* parameters)
{
  FeatherBlend2TransformBuffs
    <BlendOps::interpQuatAddPosPartialFeathered>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskFeatherBlend2TransformBuffsInterpAttInterpPos(Dispatcher::TaskParameters* parameters)
{
  FeatherBlend2TransformBuffs
    <BlendOps::interpQuatInterpPosPartialFeathered>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
MR_BLEND_TRANSFORM_TASKS_FORCEINLINE
void FeatherBlend2TrajectoryDeltaAndTransforms(
  Dispatcher::TaskParameters* parameters,
  FeatherBlend2TransformBuffsFunc transformBlendFunc,
  Blend2TrajectoryBlendFunc trajectoryBlendFunc)
{
  AttribDataTransformBuffer* source0TransformsAttr = parameters->getInputAttrib<AttribDataTransformBuffer>(1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
  AttribDataTransformBuffer* source1TransformsAttr = parameters->getInputAttrib<AttribDataTransformBuffer>(2, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
  AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(3, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  NMP_ASSERT(blendWeights->m_trajectoryAndTransformsNumWeights == 1);
  AttribDataFeatherBlend2ChannelAlphas* boneWeights = parameters->getInputAttrib<AttribDataFeatherBlend2ChannelAlphas>(4, ATTRIB_SEMANTIC_BONE_WEIGHTS);
  const uint32_t numRigJoints = source0TransformsAttr->m_transformBuffer->getLength();
  AttribDataTransformBuffer* transformsAttr = parameters->createOutputAttribTransformBuffer(0, numRigJoints, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);

#ifdef NMP_ENABLE_ASSERTS
  const uint32_t numRigJoints1 = source1TransformsAttr->m_transformBuffer->getLength();
  NMP_ASSERT(numRigJoints == numRigJoints1);
#endif

  NMP::DataBuffer* transforms = transformsAttr->m_transformBuffer;
  NMP::DataBuffer* source0Transforms = source0TransformsAttr->m_transformBuffer;
  NMP::DataBuffer* source1Transforms = source1TransformsAttr->m_transformBuffer;

  // Perform the actual transform blend.
  transformBlendFunc(
    transforms,
    source0Transforms,
    source1Transforms,
    blendWeights->m_trajectoryAndTransformsWeights[0],
    boneWeights->m_numChannelAlphas,
    boneWeights->m_channelAlphas);

  // Perform the actual trajectory blend.
  bool trajectoryFilteredOut = !transforms->hasChannel(0);

  trajectoryBlendFunc(
    transforms->getPosQuatChannelPos(0),
    transforms->getPosQuatChannelQuat(0),
    &trajectoryFilteredOut,
    source0Transforms->getPosQuatChannelPos(0),
    source0Transforms->getPosQuatChannelQuat(0),
    !source0Transforms->hasChannel(0),
    source1Transforms->getPosQuatChannelPos(0),
    source1Transforms->getPosQuatChannelQuat(0),
    !source1Transforms->hasChannel(0),
    blendWeights->m_trajectoryAndTransformsWeights[0],
    boneWeights);

  transforms->setChannelUsedFlag(0, !trajectoryFilteredOut);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskFeatherBlend2TrajectoryDeltaAndTransformsAddAttAddPosSlerpTraj(Dispatcher::TaskParameters* parameters)
{
  FeatherBlend2TrajectoryDeltaAndTransforms(
    parameters,
    BlendOps::addQuatAddPosPartialFeathered,
    Blend2TrajectoryDeltaTransforms<BLEND2_WEIGHTING_PER_BONE, BlendOps::interpBlendQuats, BLEND2_SLERP_POS>);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskFeatherBlend2TrajectoryDeltaAndTransformsAddAttInterpPosSlerpTraj(Dispatcher::TaskParameters* parameters)
{
  FeatherBlend2TrajectoryDeltaAndTransforms(
    parameters,
    BlendOps::addQuatLeavePosPartialFeathered,
    Blend2TrajectoryDeltaTransforms<BLEND2_WEIGHTING_PER_BONE, BlendOps::interpBlendQuats, BLEND2_SLERP_POS>);
}

//----------------------------------------------------------------------------------------------------------------------;
void TaskFeatherBlend2TrajectoryDeltaAndTransformsInterpAttAddPosSlerpTraj(Dispatcher::TaskParameters* parameters)
{
  FeatherBlend2TrajectoryDeltaAndTransforms(
    parameters,
    BlendOps::interpQuatAddPosPartialFeathered,
    Blend2TrajectoryDeltaTransforms<BLEND2_WEIGHTING_PER_BONE, BlendOps::interpBlendQuats, BLEND2_SLERP_POS>);
}

//----------------------------------------------------------------------------------------------------------------------;
void TaskFeatherBlend2TrajectoryDeltaAndTransformsInterpAttInterpPosSlerpTraj(Dispatcher::TaskParameters* parameters)
{
  FeatherBlend2TrajectoryDeltaAndTransforms(
    parameters,
    BlendOps::interpQuatInterpPosPartialFeathered,
    Blend2TrajectoryDeltaTransforms<BLEND2_WEIGHTING_PER_BONE, BlendOps::interpBlendQuats, BLEND2_SLERP_POS>);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskFeatherBlend2TrajectoryDeltaAndTransformsAddAttAddPosInterpTraj(Dispatcher::TaskParameters* parameters)
{
  FeatherBlend2TrajectoryDeltaAndTransforms(
    parameters,
    BlendOps::addQuatAddPosPartialFeathered,
    Blend2TrajectoryDeltaTransforms<BLEND2_WEIGHTING_PER_BONE, BlendOps::interpBlendQuats, BLEND2_SLERP_POS>);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskFeatherBlend2TrajectoryDeltaAndTransformsAddAttInterpPosInterpTraj(Dispatcher::TaskParameters* parameters)
{
  FeatherBlend2TrajectoryDeltaAndTransforms(
    parameters,
    BlendOps::addQuatLeavePosPartialFeathered,
    Blend2TrajectoryDeltaTransforms<BLEND2_WEIGHTING_PER_BONE, BlendOps::interpBlendQuats, BLEND2_SLERP_POS>);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskFeatherBlend2TrajectoryDeltaAndTransformsInterpAttAddPosInterpTraj(Dispatcher::TaskParameters* parameters)
{
  FeatherBlend2TrajectoryDeltaAndTransforms(
    parameters,
    BlendOps::interpQuatAddPosPartialFeathered,
    Blend2TrajectoryDeltaTransforms<BLEND2_WEIGHTING_PER_BONE, BlendOps::interpBlendQuats, BLEND2_SLERP_POS>);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskFeatherBlend2TrajectoryDeltaAndTransformsInterpAttInterpPosInterpTraj(Dispatcher::TaskParameters* parameters)
{
  FeatherBlend2TrajectoryDeltaAndTransforms(
    parameters,
    BlendOps::interpQuatInterpPosPartialFeathered,
    Blend2TrajectoryDeltaTransforms<BLEND2_WEIGHTING_PER_BONE, BlendOps::interpBlendQuats, BLEND2_SLERP_POS>);
}

//----------------------------------------------------------------------------------------------------------------------
// Blend2x2 tasks
//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2x2TransformBuffsInterpAttInterpPos(Dispatcher::TaskParameters* parameters)
{
  AttribDataTransformBuffer* source0Transforms = parameters->getInputAttrib<AttribDataTransformBuffer>(1, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  AttribDataTransformBuffer* source1Transforms = parameters->getInputAttrib<AttribDataTransformBuffer>(2, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  AttribDataTransformBuffer* source2Transforms = parameters->getInputAttrib<AttribDataTransformBuffer>(3, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  AttribDataTransformBuffer* source3Transforms = parameters->getInputAttrib<AttribDataTransformBuffer>(4, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(5, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  NMP_ASSERT(blendWeights->m_trajectoryAndTransformsNumWeights == 2);

  const uint32_t numRigJoints = source0Transforms->m_transformBuffer->getLength();
#ifdef NMP_ENABLE_ASSERTS
  const uint32_t numRigJoints1 = source1Transforms->m_transformBuffer->getLength();
  const uint32_t numRigJoints2 = source2Transforms->m_transformBuffer->getLength();
  const uint32_t numRigJoints3 = source3Transforms->m_transformBuffer->getLength();
  NMP_ASSERT(numRigJoints == numRigJoints1 && numRigJoints1 == numRigJoints2 && numRigJoints2 == numRigJoints3);
#endif

  // Get temp allocator
  NMP::TempMemoryAllocator* tempAllocator = parameters->m_dispatcher->getTempMemoryAllocator();
  NMP_ASSERT(tempAllocator);
  NMP::TempMemoryAllocator* childAllocator = tempAllocator->createChildAllocator();

  AttribDataTransformBuffer* transforms = parameters->createOutputAttribTransformBuffer(0, numRigJoints, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);

  // Allocate temporary buffers for storing intermediate interpolation results.
  NMP::Memory::Format buffMemReqs = NMP::DataBuffer::getPosQuatMemoryRequirements(numRigJoints);
  NMP::DataBuffer* interpTempBuf01 = NMP::DataBuffer::createPosQuat(childAllocator, buffMemReqs, numRigJoints);
  NMP::DataBuffer* interpTempBuf23 = NMP::DataBuffer::createPosQuat(childAllocator, buffMemReqs, numRigJoints);

  float blendWeightX = blendWeights->m_trajectoryAndTransformsWeights[0];
  float blendWeightY = blendWeights->m_trajectoryAndTransformsWeights[1];

  // Four way blend implemented as three two way blends
  BlendOps::interpQuatInterpPosPartial(
    interpTempBuf01,
    source0Transforms->m_transformBuffer,
    source1Transforms->m_transformBuffer,
    blendWeightX);
  BlendOps::interpQuatInterpPosPartial(
    interpTempBuf23,
    source2Transforms->m_transformBuffer,
    source3Transforms->m_transformBuffer,
    blendWeightX);
  BlendOps::interpQuatInterpPosPartial(
    transforms->m_transformBuffer,
    interpTempBuf01,
    interpTempBuf23,
    blendWeightY);

  // Free the temp buffers by rolling back the temp memory allocator.
  childAllocator->memFree(interpTempBuf01);
  childAllocator->memFree(interpTempBuf23);
  tempAllocator->destroyChildAllocator(childAllocator);
}

//----------------------------------------------------------------------------------------------------------------------
namespace 
{

//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
// Disable 'Conditional expression is constant' warning, template parameters are deliberately compile-time constants.
#pragma warning(push)
#pragma warning(disable: 4127)
#endif // _MSC_VER
template <bool SLERP>
MR_BLEND_TRANSFORM_TASKS_FORCEINLINE
static void subTaskBlend2x2TrajectoryDeltaAndTransformBuffs(Dispatcher::TaskParameters* parameters)
{
  AttribDataTransformBuffer* source0Transforms = parameters->getInputAttrib<AttribDataTransformBuffer>(1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
  AttribDataTransformBuffer* source1Transforms = parameters->getInputAttrib<AttribDataTransformBuffer>(2, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
  AttribDataTransformBuffer* source2Transforms = parameters->getInputAttrib<AttribDataTransformBuffer>(3, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
  AttribDataTransformBuffer* source3Transforms = parameters->getInputAttrib<AttribDataTransformBuffer>(4, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
  AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(5, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  NMP_ASSERT(blendWeights->m_trajectoryAndTransformsNumWeights == 2);

  const NMP::DataBuffer* sourceTransformBuffers[4];
  sourceTransformBuffers[0] = source0Transforms->m_transformBuffer;
  sourceTransformBuffers[1] = source1Transforms->m_transformBuffer;
  sourceTransformBuffers[2] = source2Transforms->m_transformBuffer;
  sourceTransformBuffers[3] = source3Transforms->m_transformBuffer;

  const uint32_t numRigJoints = sourceTransformBuffers[0]->getLength();
#ifdef NMP_ENABLE_ASSERTS
  const uint32_t numRigJoints1 = sourceTransformBuffers[1]->getLength();
  const uint32_t numRigJoints2 = sourceTransformBuffers[2]->getLength();
  const uint32_t numRigJoints3 = sourceTransformBuffers[3]->getLength();
  NMP_ASSERT(numRigJoints == numRigJoints1 && numRigJoints1 == numRigJoints2 && numRigJoints2 == numRigJoints3);
#endif

  // Create the output transform buffer (before any working memory)
  AttribDataTransformBuffer* transforms = parameters->createOutputAttribTransformBuffer(0, numRigJoints, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
  NMP::DataBuffer* outputTransformBuffer = transforms->m_transformBuffer;

  // Store a roll back point on the fast heap to restore back to after we have done our local working.
  NMP::TempMemoryAllocator* tempAllocator = parameters->m_dispatcher->getTempMemoryAllocator();
  NMP_ASSERT(tempAllocator);
  NMP::TempMemoryAllocator* childAllocator = tempAllocator->createChildAllocator();

  // Allocate temporary buffers for storing intermediate interpolation results.
  NMP::Memory::Format buffMemReqs = NMP::DataBuffer::getPosQuatMemoryRequirements(numRigJoints);
  NMP::DataBuffer* interpTempBuf01 = NMP::DataBuffer::createPosQuat(childAllocator, buffMemReqs, numRigJoints);
  NMP::DataBuffer* interpTempBuf23 = NMP::DataBuffer::createPosQuat(childAllocator, buffMemReqs, numRigJoints);

  //---------------------------
  // Four way blend implemented as three two way blends
  BlendOps::interpQuatInterpPosPartial(
    interpTempBuf01,
    sourceTransformBuffers[0],
    sourceTransformBuffers[1],
    blendWeights->m_trajectoryAndTransformsWeights[0]);
  BlendOps::interpQuatInterpPosPartial(
    interpTempBuf23,
    sourceTransformBuffers[2],
    sourceTransformBuffers[3],
    blendWeights->m_trajectoryAndTransformsWeights[0]);
  BlendOps::interpQuatInterpPosPartial(
    outputTransformBuffer,
    interpTempBuf01,
    interpTempBuf23,
    blendWeights->m_trajectoryAndTransformsWeights[1]);

  // Free temp buffers
  childAllocator->memFree(interpTempBuf01);
  childAllocator->memFree(interpTempBuf23);
  tempAllocator->destroyChildAllocator(childAllocator);

  //---------------------------
  // Spherical interpolation of the trajectory position channel
  if (SLERP)
  {
    // Four way blend implemented as three two way blends
    NMP::Vector3 interpVecTmp01, interpVecTmp23;
    bool filteredOut01, filteredOut23, filteredOut;

    BlendOps::blend2PartialSlerpPos(
      &interpVecTmp01,
      &filteredOut01,
      sourceTransformBuffers[0]->getPosQuatChannelPos(0),
      !sourceTransformBuffers[0]->hasChannel(0),
      sourceTransformBuffers[1]->getPosQuatChannelPos(0),
      !sourceTransformBuffers[1]->hasChannel(0),
      blendWeights->m_trajectoryAndTransformsWeights[0]);
    BlendOps::blend2PartialSlerpPos(
      &interpVecTmp23,
      &filteredOut23,
      sourceTransformBuffers[2]->getPosQuatChannelPos(0),
      !sourceTransformBuffers[2]->hasChannel(0),
      sourceTransformBuffers[3]->getPosQuatChannelPos(0),
      !sourceTransformBuffers[3]->hasChannel(0),
      blendWeights->m_trajectoryAndTransformsWeights[0]);
    BlendOps::blend2PartialSlerpPos(
      outputTransformBuffer->getPosQuatChannelPos(0),
      &filteredOut,
      &interpVecTmp01,
      filteredOut01,
      &interpVecTmp23,
      filteredOut23,
      blendWeights->m_trajectoryAndTransformsWeights[1]);
    NMP_ASSERT(filteredOut == !outputTransformBuffer->hasChannel(0)); // Should have already been set
  }
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

//----------------------------------------------------------------------------------------------------------------------
} // Anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2x2TrajectoryDeltaAndTransformsInterpTraj(Dispatcher::TaskParameters* parameters)
{
  subTaskBlend2x2TrajectoryDeltaAndTransformBuffs<false>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2x2TrajectoryDeltaAndTransformsSlerpTraj(Dispatcher::TaskParameters* parameters)
{
  subTaskBlend2x2TrajectoryDeltaAndTransformBuffs<true>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
// Triangle blend tasks
//----------------------------------------------------------------------------------------------------------------------
void TaskTriangleBlendTransformBuffsInterpAttInterpPos(Dispatcher::TaskParameters* parameters)
{
  AttribDataTransformBuffer* source0Transforms = parameters->getInputAttrib<AttribDataTransformBuffer>(1, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  AttribDataTransformBuffer* source1Transforms = parameters->getInputAttrib<AttribDataTransformBuffer>(2, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  AttribDataTransformBuffer* source2Transforms = parameters->getInputAttrib<AttribDataTransformBuffer>(3, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(4, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  NMP_ASSERT(blendWeights->m_trajectoryAndTransformsNumWeights == 2);

  const uint32_t numRigJoints = source0Transforms->m_transformBuffer->getLength();
#ifdef NMP_ENABLE_ASSERTS
  const uint32_t numRigJoints1 = source1Transforms->m_transformBuffer->getLength();
  const uint32_t numRigJoints2 = source2Transforms->m_transformBuffer->getLength();
  NMP_ASSERT(numRigJoints == numRigJoints1 && numRigJoints1 == numRigJoints2);
#endif

  // Create the output transform buffer (before any working memory)
  AttribDataTransformBuffer* transforms = parameters->createOutputAttribTransformBuffer(0, numRigJoints, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);

  // Store a roll back point on the fast heap to restore back to after we have done our local working.
  NMP::TempMemoryAllocator* tempAllocator = parameters->m_dispatcher->getTempMemoryAllocator();
  NMP_ASSERT(tempAllocator);

  NMP::TempMemoryAllocator* childAllocator = tempAllocator->createChildAllocator();

  // Allocate temporary buffers for storing intermediate interpolation results.
  NMP::Memory::Format buffMemReqs = NMP::DataBuffer::getPosQuatMemoryRequirements(numRigJoints);
  NMP::DataBuffer* interpTempBuf = NMP::DataBuffer::createPosQuat(childAllocator, buffMemReqs, numRigJoints);

  // Three way blend implemented as a pair of two way blends
  BlendOps::interpQuatInterpPosPartial(
    interpTempBuf,
    source0Transforms->m_transformBuffer,
    source1Transforms->m_transformBuffer,
    blendWeights->m_trajectoryAndTransformsWeights[0]);

  BlendOps::interpQuatInterpPosPartial(
    transforms->m_transformBuffer,
    interpTempBuf,
    source2Transforms->m_transformBuffer,
    blendWeights->m_trajectoryAndTransformsWeights[1]);

  // Free temp buffers
  childAllocator->memFree(interpTempBuf);
  tempAllocator->destroyChildAllocator(childAllocator);
}

//----------------------------------------------------------------------------------------------------------------------
namespace 
{

//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
// Disable 'Conditional expression is constant' warning, template parameters are deliberately compile-time constants.
#pragma warning(push)
#pragma warning(disable: 4127)
#endif // _MSC_VER
template <bool SLERP>
MR_BLEND_TRANSFORM_TASKS_FORCEINLINE
static void subTaskTriangleBlendTrajectoryDeltaAndTransformBuffs(Dispatcher::TaskParameters* parameters)
{
  AttribDataTransformBuffer* source0Transforms = parameters->getInputAttrib<AttribDataTransformBuffer>(1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
  AttribDataTransformBuffer* source1Transforms = parameters->getInputAttrib<AttribDataTransformBuffer>(2, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
  AttribDataTransformBuffer* source2Transforms = parameters->getInputAttrib<AttribDataTransformBuffer>(3, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
  AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(4, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  NMP_ASSERT(blendWeights->m_trajectoryAndTransformsNumWeights == 2);

  const NMP::DataBuffer* sourceTransformBuffers[3];
  sourceTransformBuffers[0] = source0Transforms->m_transformBuffer;
  sourceTransformBuffers[1] = source1Transforms->m_transformBuffer;
  sourceTransformBuffers[2] = source2Transforms->m_transformBuffer;

  const uint32_t numRigJoints = sourceTransformBuffers[0]->getLength();
#ifdef NMP_ENABLE_ASSERTS
  const uint32_t numRigJoints1 = sourceTransformBuffers[1]->getLength();
  const uint32_t numRigJoints2 = sourceTransformBuffers[2]->getLength();
  NMP_ASSERT(numRigJoints == numRigJoints1 && numRigJoints1 == numRigJoints2);
#endif

  // Create the output transform buffer (before any working memory)
  AttribDataTransformBuffer* transforms = parameters->createOutputAttribTransformBuffer(0, numRigJoints, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
  NMP::DataBuffer* outputTransformBuffer = transforms->m_transformBuffer;

  // Store a roll back point on the fast heap to restore back to after we have done our local working.
  NMP::TempMemoryAllocator* tempAllocator = parameters->m_dispatcher->getTempMemoryAllocator();
  NMP_ASSERT(tempAllocator);
  NMP::TempMemoryAllocator* childAllocator = tempAllocator->createChildAllocator();

  // Allocate temporary buffers for storing intermediate interpolation results.
  NMP::Memory::Format buffMemReqs = NMP::DataBuffer::getPosQuatMemoryRequirements(numRigJoints);
  NMP::DataBuffer* interpTempBuf = NMP::DataBuffer::createPosQuat(childAllocator, buffMemReqs, numRigJoints);

  // Three way blend implemented as a pair of two way blends
  BlendOps::interpQuatInterpPosPartial(
    interpTempBuf,
    source0Transforms->m_transformBuffer,
    source1Transforms->m_transformBuffer,
    blendWeights->m_trajectoryAndTransformsWeights[0]);

  BlendOps::interpQuatInterpPosPartial(
    transforms->m_transformBuffer,
    interpTempBuf,
    source2Transforms->m_transformBuffer,
    blendWeights->m_trajectoryAndTransformsWeights[1]);

  // Free temp buffers
  childAllocator->memFree(interpTempBuf);
  tempAllocator->destroyChildAllocator(childAllocator);

  //---------------------------
  // Spherical interpolation of the trajectory position channel
  if (SLERP)
  {
    NMP::Vector3 interpVecTmp;
    bool filteredOutTmp, filteredOut;

    BlendOps::blend2PartialSlerpPos(
      &interpVecTmp,
      &filteredOutTmp,
      sourceTransformBuffers[0]->getPosQuatChannelPos(0),
      !sourceTransformBuffers[0]->hasChannel(0),
      sourceTransformBuffers[1]->getPosQuatChannelPos(0),
      !sourceTransformBuffers[1]->hasChannel(0),
      blendWeights->m_trajectoryAndTransformsWeights[0]);
    BlendOps::blend2PartialSlerpPos(
      outputTransformBuffer->getPosQuatChannelPos(0),
      &filteredOut,
      &interpVecTmp,
      filteredOutTmp,
      sourceTransformBuffers[2]->getPosQuatChannelPos(0),
      !sourceTransformBuffers[2]->hasChannel(0),
      blendWeights->m_trajectoryAndTransformsWeights[1]);
    NMP_ASSERT(filteredOut == !outputTransformBuffer->hasChannel(0)); // Should have already been set
  }
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

//----------------------------------------------------------------------------------------------------------------------
} // Anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
void TaskTriangleBlendTrajectoryDeltaAndTransformsInterpTraj(Dispatcher::TaskParameters* parameters)
{
  subTaskTriangleBlendTrajectoryDeltaAndTransformBuffs<false>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskTriangleBlendTrajectoryDeltaAndTransformsSlerpTraj(Dispatcher::TaskParameters* parameters)
{
  subTaskTriangleBlendTrajectoryDeltaAndTransformBuffs<true>(parameters);
}

//----------------------------------------------------------------------------------------------------------------------
// Blend All tasks
//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void subTaskBlendAllTransformBuffsInterpAttInterpPos(
  Dispatcher::TaskParameters* parameters,
  AttribDataSemantic transformSemantic,
  bool slerpTrajectory)
{
  // Get number of connections
  uint32_t numConnections = parameters->m_numParameters - 2;

  MR::DispatcherSPU* dispatcher = (MR::DispatcherSPU*)parameters->m_dispatcher;
  NMP::TempMemoryAllocator* tempAllocator = dispatcher->getTempMemoryAllocator();

  // Load the first transform buffer from main memory
  uint32_t dmaTagFirstBuffer = FetchParameterHelper(&parameters->m_parameters[1], tempAllocator);
  // Get the normalised blend weights
  uint32_t dmaTagWeights = FetchParameterHelper(&parameters->m_parameters[numConnections + 1], tempAllocator);

  //-----------------------------
  // Wait on completion of DMA  from PPU memory.
  NMP::SPUDMAController::dmaWait(dmaTagFirstBuffer);
  // Now write the local store value into the param.
  AttribDataTransformBuffer* accumulatedTrans = parameters->getInputAttrib<AttribDataTransformBuffer>(1, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  AttribDataTransformBuffer::relocate(accumulatedTrans);
  NMP::DataBuffer* nextSourceBuf = accumulatedTrans->m_transformBuffer;

  //-----------------------------
  NMP::SPUDMAController::dmaWait(dmaTagWeights);
  AttribDataFloatArray* weights = parameters->getInputAttrib<AttribDataFloatArray>(numConnections+1, ATTRIB_SEMANTIC_CP_FLOAT);
  AttribDataFloatArray::relocate(weights);

  //-----------------------------
  // Output transform buffer
  AttribDataTransformBuffer* outputTransforms = parameters->createOutputAttribTransformBuffer(
    0, 
    accumulatedTrans->m_transformBuffer->getLength(), 
    ATTRIB_SEMANTIC_TRANSFORM_BUFFER);

  //-----------------------------
  // Store a roll back point on the fast heap to restore back to after we have done our local working.
  NMP_ASSERT(tempAllocator);
  NMP::TempMemoryAllocator* childAllocator = tempAllocator->createChildAllocator();

  //-----------------------------
  // Allocate temporary buffers for storing intermediate interpolation results (2 buffers to toggle as we accmulate).
  NMP::Memory::Format buffMemReqs = nextSourceBuf->getInstanceMemoryRequirements();
  const uint32_t numRigJoints = nextSourceBuf->getLength();
  NMP::DataBuffer* workingBufs[2];
  workingBufs[0] = NMP::DataBuffer::createPosQuat(childAllocator, buffMemReqs, numRigJoints);
  workingBufs[1] = NMP::DataBuffer::createPosQuat(childAllocator, buffMemReqs, numRigJoints);
  

  uint32_t paramIndex = 2;
  float weight = 0.0f;
  float accumulatedWeight = weights->m_values[0];
  
  void* lsPtrs = NULL;
  AttribDataTransformBuffer* nextTransforms;
  NMP::Memory::Resource attrResource = NMPAllocatorAllocateFromFormat(childAllocator, 
                                         parameters->m_parameters[paramIndex].m_attribDataHandle.m_format);
  lsPtrs = attrResource.ptr;
  uint32_t dmaTags = 0;
  NMP::DataBuffer* accumulatedBuf = nextSourceBuf;
  for (uint32_t i = 0; i < numConnections - 1; ++i)
  {
    //-----------------------------
    // Load next transform buffer, but not allocate new memory for it
    dmaTags = NMP::SPUDMAController::dmaGet(
      lsPtrs, 
      (uint32_t)(parameters->m_parameters[paramIndex].m_attribDataHandle.m_attribData), 
      parameters->m_parameters[paramIndex].m_attribDataHandle.m_format.size);
    NMP::SPUDMAController::dmaWait(dmaTags);
    parameters->m_parameters[paramIndex].m_attribDataHandle.m_attribData = (MR::AttribData*)(lsPtrs);
    nextTransforms = parameters->getInputAttrib<AttribDataTransformBuffer>(paramIndex, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
    AttribDataTransformBuffer::relocate(nextTransforms);
    //-----------------------------
    // normalised blend weight
    weight = accumulatedWeight / (accumulatedWeight + weights->m_values[i+1]);
    //-----------------------------
    // perform the actual blend
    BlendOps::interpQuatInterpPosPartial(
      workingBufs[0],
      accumulatedBuf,
      nextTransforms->m_transformBuffer,
      1.0f - weight);

    if (slerpTrajectory)
    {
      workingBufs[0]->getPosQuatChannelPos(0)->slerp(
        *accumulatedBuf->getPosQuatChannelPos(0),
        *nextTransforms->m_transformBuffer->getPosQuatChannelPos(0),
        1.0f - weight);
    }

    accumulatedBuf = workingBufs[0];

    // swap the working buffer pointers.
    workingBufs[0] = workingBufs[1];
    workingBufs[1] = accumulatedBuf;

    accumulatedWeight += weights->m_values[i+1];
    ++paramIndex;
  }

  // Copy the final working buffer to the output.
  accumulatedBuf->copyTo(outputTransforms->m_transformBuffer);

  // Free the temp buffers by rolling back the temp memory allocator.
  childAllocator->memFree(workingBufs[0]);
  childAllocator->memFree(workingBufs[1]);
  tempAllocator->destroyChildAllocator(childAllocator);
}

#else // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void subTaskBlendAllTransformBuffsInterpAttInterpPos(
  Dispatcher::TaskParameters* parameters,
  AttribDataSemantic transformSemantic,
  bool slerpTrajectory)
{
  // Get number of child input connections.
  uint32_t numConnections = parameters->m_numParameters - 2;
  NMP_ASSERT(numConnections > 0);

  // Find out how many transform channels our output transform should have by getting the count from the first input transform buffer.
  NMP::DataBuffer* nextSourceBuf = (parameters->getInputAttrib<AttribDataTransformBuffer>(1, transformSemantic))->m_transformBuffer;
  const uint32_t numRigJoints = nextSourceBuf->getLength();

  // Create the output transform buffer.
  AttribDataTransformBuffer* outputTransforms = parameters->createOutputAttribTransformBuffer(0, numRigJoints, transformSemantic);
  
  // Get the normalised blend weights
  AttribDataFloatArray* weights = parameters->getInputAttrib<AttribDataFloatArray>(parameters->m_numParameters - 1, ATTRIB_SEMANTIC_CP_FLOAT);


  // Store a roll back point on the fast heap to restore back to after we have done our local working.
  NMP::TempMemoryAllocator* tempAllocator = parameters->m_dispatcher->getTempMemoryAllocator();
  NMP_ASSERT(tempAllocator);
  NMP::TempMemoryAllocator* childAllocator = tempAllocator->createChildAllocator();

  // Allocate temporary buffers for storing intermediate interpolation results (2 buffers to toggle as we accumulate).
  NMP::Memory::Format buffMemReqs = nextSourceBuf->getInstanceMemoryRequirements();
  NMP::DataBuffer* workingBufs[2];
  workingBufs[0] = NMP::DataBuffer::createPosQuat(childAllocator, buffMemReqs, numRigJoints);
  workingBufs[1] = NMP::DataBuffer::createPosQuat(childAllocator, buffMemReqs, numRigJoints);

  // Output transform buffer
  float weight = 0.0f;
  float accumulatedWeight = weights->m_values[0];

  // Temporary pointer to current transform buffer
  NMP::DataBuffer* accumulatedBuf = nextSourceBuf;
  for (uint32_t i = 0; i < numConnections - 1; ++i)
  {
    // normalised blend weight
    weight = accumulatedWeight / (accumulatedWeight + weights->m_values[i+1]);

    // Get the next input transform buffer to blend in
    nextSourceBuf = (parameters->getInputAttrib<AttribDataTransformBuffer>(i + 2, transformSemantic))->m_transformBuffer;
    NMP_ASSERT(nextSourceBuf->getLength() == numRigJoints);

    // perform the actual blend
    BlendOps::interpQuatInterpPosPartial(
      workingBufs[0],
      accumulatedBuf,
      nextSourceBuf,
      1.0f - weight);

    if (slerpTrajectory)
    {
      workingBufs[0]->getPosQuatChannelPos(0)->slerp(
        *accumulatedBuf->getPosQuatChannelPos(0),
        *nextSourceBuf->getPosQuatChannelPos(0),
        1.0f - weight);
    }

    accumulatedBuf = workingBufs[0];
        
    // swap the working buffer pointers.
    workingBufs[0] = workingBufs[1];
    workingBufs[1] = accumulatedBuf;

    accumulatedWeight += weights->m_values[i+1];
  }

  // Copy the final working buffer to the output.
  accumulatedBuf->copyTo(outputTransforms->m_transformBuffer);

  // Free temp buffers.
  childAllocator->memFree(workingBufs[0]);
  childAllocator->memFree(workingBufs[1]);
  tempAllocator->destroyChildAllocator(childAllocator);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void TaskBlendAllTransformBuffsInterpAttInterpPos(Dispatcher::TaskParameters* parameters)
{
  subTaskBlendAllTransformBuffsInterpAttInterpPos(parameters, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, false);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlendAllTrajectoryDeltaAndTransformsSlerpTraj(Dispatcher::TaskParameters* parameters)
{
  subTaskBlendAllTransformBuffsInterpAttInterpPos(parameters, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, true);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlendAllTrajectoryDeltaAndTransformsInterpTraj(Dispatcher::TaskParameters* parameters)
{
  subTaskBlendAllTransformBuffsInterpAttInterpPos(parameters, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, false);
}

//----------------------------------------------------------------------------------------------------------------------
// Distributes a root offset rotation over time into the trajectory channel so that the trajectory of the
// closest animation will be rotated to match up with the current orientation of the character root joint.
// The root offset rotation is initially applied fully to the character root of the closest animation in
// order to align it with the network output transforms. Over time this offset rotation is distributed into
// the trajectory channel. Note: We blend the character space hips transforms from the predicted network
// output and the root rotation aligned closest animation rather than the root channel itself, since the
// hips are usually located at the center of the body so distributing the offset rotation here will minimize
// the likelihood of ground penetration from the limbs during the blending.
NM_INLINE void subTaskClosestAnimTransforms(
  AttribDataTransformBuffer* inputTransforms,
  AttribDataTransformBuffer* outputTransforms,
  AttribDataRig* rigAttribData,
  AttribDataSyncEventTrack* syncEventTrack,
  AttribDataPlaybackPos* playbackPos,
  AttribDataFloat* blendWeightAttrib,
  const NMP::Vector3& trajectoryDeltaPos,
  const NMP::Quat& trajectoryDeltaAtt,
  AttribDataClosestAnimState* outputStateData,
  AttribDataClosestAnimState* inputStateData,
  AttribDataUpdatePlaybackPos* deltaTime,
  AttribDataTransformBuffer* deadBlendRatesAttrib,
  AttribDataTransformBuffer* curDeadBlendTransformsState)
{
  // Input transforms of active child
  const NMP::DataBuffer* inputBuffer = inputTransforms->m_transformBuffer;
  const uint32_t numRigJoints = inputTransforms->m_transformBuffer->getLength();

  // Output transforms
  NMP::DataBuffer* outputBuffer = outputTransforms->m_transformBuffer;

  AnimRigDef* rig = rigAttribData->m_rig;
  NMP_ASSERT(rig->getNumBones() == numRigJoints);
  const NMP::Hierarchy* hierarchy = rig->getHierarchy();

  // Dead blend transform rates
  NMP::DataBuffer* deadBlendRates = deadBlendRatesAttrib->m_transformBuffer;
  NMP_ASSERT(deadBlendRates);

  // Dead blend output transform state
  NMP::DataBuffer* deadBlendBuffer = curDeadBlendTransformsState->m_transformBuffer;
  NMP_ASSERT(deadBlendBuffer);

  //---------------------------
  // Connected control param providing blend weight.
  float blendWeight = NMP::clampValue(blendWeightAttrib->m_value, 0.0f, 1.0f);
  float dt = blendWeight * deltaTime->m_value;

  // Update the dead blend transforms. Note that all channels contain local space
  // transform data except the hips channel which contains the character space transforms.
  // The velocities for the hips channel are set to zero so it will not be modified.
  if (dt > 0.0f)
  {
    for (uint32_t j = 0; j < numRigJoints; ++j)
    {
      if (deadBlendBuffer->hasChannel(j) && deadBlendRates->hasChannel(j))
      {
        NMP::Quat q = *deadBlendBuffer->getPosQuatChannelQuat(j);
        NMP::Vector3 p = *deadBlendBuffer->getPosQuatChannelPos(j);

        const NMP::Vector3& vel = *deadBlendRates->getPosVelAngVelChannelPosVel(j);
        const NMP::Vector3& angVel = *deadBlendRates->getPosVelAngVelChannelAngVel(j);

        p += vel * dt;
        q += (NMP::Quat(angVel.x, angVel.y, angVel.z, 0.0f) * q) * (0.5f * dt);
        q.fastNormalise();

        deadBlendBuffer->setPosQuatChannelPos(j, p);
        deadBlendBuffer->setPosQuatChannelQuat(j, q);
      }
    }
  }

  //---------------------------
  // Update the closest anim state data
  AttribDataClosestAnimState::copy(inputStateData, outputStateData);
  float weight = 1.0f;
  if (!outputStateData->m_blendCompleteFlag)
  {
    // Compute the interpolation weight factor. Check if the current playback position has looped.
    // This may happen if the fraction through the source is set to 1.
    float T = outputStateData->m_fractionThroughSource * syncEventTrack->m_syncEventTrack.getDuration();
    if (T > ERROR_LIMIT && playbackPos->m_currentPosReal >= playbackPos->m_previousPosReal)
    {
      // Get the fractional playback positions through the blending duration
      weight = playbackPos->m_previousPosReal / T;
    }

    // Update the blend complete flag
    if (weight >= 1.0f)
      outputStateData->m_blendCompleteFlag = true;
  }

  //---------------------------
  // Perform the blend
  if (!outputStateData->m_blendCompleteFlag)
  {
    // Information
    int32_t hipsIndex = rig->getCharacterRootBoneIndex();
    int32_t hipsParentIndex = hierarchy->getParentIndex(hipsIndex);

    // Remove the current frame's trajectory delta from the character space hips transform of the
    // predicted network output. i.e. We are distributing the offset rotation between the hips and
    // the trajectory over time, so that the character appears in the same position while blending
    // the controlled root to the current orientation of the closest animation. This just updates
    // the hips into the frame of the trajectory for this current time frame.
    NMP::Quat charSpaceHipsQuatDeadBlend = *deadBlendBuffer->getPosQuatChannelQuat(hipsIndex);
    NMP::Vector3 charSpaceHipsPosDeadBlend = *deadBlendBuffer->getPosQuatChannelPos(hipsIndex);
    charSpaceHipsQuatDeadBlend = ~trajectoryDeltaAtt * charSpaceHipsQuatDeadBlend;
    charSpaceHipsPosDeadBlend = trajectoryDeltaAtt.inverseRotateVector(charSpaceHipsPosDeadBlend - trajectoryDeltaPos);
    deadBlendBuffer->setPosQuatChannelQuat(hipsIndex, charSpaceHipsQuatDeadBlend);
    deadBlendBuffer->setPosQuatChannelPos(hipsIndex, charSpaceHipsPosDeadBlend);

    if (weight <= 0.0f)
    {
      deadBlendBuffer->copyTo(outputBuffer);
    }
    else
    {
      // Sin weight function
      weight -= 0.5f;
      weight *= NM_PI;
      NMP_ASSERT(weight <= (0.5f * NM_PI) && weight >= -(0.5f * NM_PI));
      weight = NMP::fastSin(weight);
      weight += 1.0f;
      weight *= 0.5f;
      NMP_ASSERT(weight <= 1.0f && weight >= 0.0f);

      // Find the representation of the identity in the same semi-arc as the offset quat
      NMP::Quat qIdentity(NMP::Quat::kIdentity);
      float fromDotTo = outputStateData->m_rootRotationOffset.dot(qIdentity);
      if (fromDotTo < 0)
      {
        fromDotTo = -fromDotTo;
        qIdentity = -qIdentity;
      }

      // Compute the offset quat for this frame that when applied to the character root
      // should align the closest animation with the original matched position of the
      // network output character pose.
      NMP::Quat qOffset;
      qOffset.fastSlerp(outputStateData->m_rootRotationOffset, qIdentity, weight, fromDotTo);

      // Accumulate the character space transform of the hips joint for the closest animation
      NMP::Quat charSpaceHipsQuat;
      NMP::Vector3 charSpaceHipsPos;
      BlendOps::accumulateTransform(
        hipsIndex,
        NMP::Vector3(NMP::Vector3::InitZero),
        qOffset,  // Apply the offset rotation
        inputBuffer,
        rig,
        charSpaceHipsPos,
        charSpaceHipsQuat);

      // Blend together all local channels of the predicted network output transforms
      // and the current closest animation. Note that the hips channel will be invalid
      // but we will blend that separately later, and recover the required local transforms.
      BlendOps::interpQuatInterpPosPartial(
        outputBuffer,
        deadBlendBuffer,
        inputBuffer,
        weight);

      // Blend together the character space hips transforms for the predicted network output
      // and the aligned closest animation.
      fromDotTo = charSpaceHipsQuatDeadBlend.dot(charSpaceHipsQuat);
      if (fromDotTo < 0)
      {
        fromDotTo = -fromDotTo;
        charSpaceHipsQuat = -charSpaceHipsQuat;
      }
      NMP::Quat charSpaceHipsQuatBlended;
      charSpaceHipsQuatBlended.fastSlerp(charSpaceHipsQuatDeadBlend, charSpaceHipsQuat, weight, fromDotTo);
      NMP::Vector3 charSpaceHipsPosBlended;
      charSpaceHipsPosBlended.lerp(charSpaceHipsPosDeadBlend, charSpaceHipsPos, weight);

      // Accumulate the character space transform of the hips bone parent for the output animation
      // that was blended from the predicted network output transforms and the closest animation.
      NMP::Quat charSpaceHipsParentQuat;
      NMP::Vector3 charSpaceHipsParentPos;
      BlendOps::accumulateTransform(
        hipsParentIndex,
        NMP::Vector3(NMP::Vector3::InitZero),
        NMP::Quat(NMP::Quat::kIdentity),
        outputBuffer,
        rig,
        charSpaceHipsParentPos,
        charSpaceHipsParentQuat);

      // Compute the local space transforms of the hips bone for the output animation
      NMP::Quat localHipsQuat;
      NMP::Vector3 localHipsPos;
      localHipsQuat = ~charSpaceHipsParentQuat * charSpaceHipsQuatBlended;
      localHipsPos = charSpaceHipsParentQuat.inverseRotateVector(charSpaceHipsPosBlended - charSpaceHipsParentPos);

      // Set the output hips transform
      outputBuffer->setPosQuatChannelQuat(hipsIndex, localHipsQuat);
      outputBuffer->setPosQuatChannelPos(hipsIndex, localHipsPos);
      outputBuffer->setChannelUsed(hipsIndex);
    }
  }
  else
  {
    // The blend has completed so just copy the closest animation transforms.
    inputBuffer->copyTo(outputBuffer);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Distributes a root offset rotation over time into the trajectory channel so that the trajectory of the
// closest animation will be rotated to match up with the current orientation of the character root joint.
// The root offset rotation is initially applied fully to the character root of the closest animation in
// order to align it with the network output transforms. Over time this offset rotation is distributed into
// the trajectory channel. Note: We blend the character space hips transforms from the predicted network
// output and the root rotation aligned closest animation rather than the root channel itself, since the
// hips are usually located at the center of the body so distributing the offset rotation here will minimize
// the likelihood of ground penetration from the limbs during the blending.
void TaskClosestAnimTransforms(Dispatcher::TaskParameters* parameters)
{
  // Input transforms of active child
  AttribDataTransformBuffer* inputTransforms = parameters->getInputAttrib<AttribDataTransformBuffer>(0, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  const uint32_t numRigJoints = inputTransforms->m_transformBuffer->getLength();
  
  // Output transforms
  AttribDataTransformBuffer* outputTransforms = parameters->createOutputAttribTransformBuffer(1, numRigJoints, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);

  // Animation rig
  AttribDataRig* rigAttribData = parameters->getInputAttrib<AttribDataRig>(2, ATTRIB_SEMANTIC_RIG);

  // Sync event tack
  AttribDataSyncEventTrack* syncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(3, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);

  // Current playback position in the active child
  AttribDataPlaybackPos* playbackPos = parameters->getInputAttrib<AttribDataPlaybackPos>(4, ATTRIB_SEMANTIC_TIME_POS);

  // Dead blend control weight
  AttribDataFloat* blendWeightAttrib = parameters->getInputAttrib<AttribDataFloat>(5, ATTRIB_SEMANTIC_CP_FLOAT);

  // Trajectory delta transform
  AttribDataTrajectoryDeltaTransform* inputTrajDeltaTransform = parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(6, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);

  // Output closest anim state, Index 7 is an InOut task parameter
  AttribDataClosestAnimState* outputStateData = parameters->createOutputAttribReplace<AttribDataClosestAnimState>(7, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE);
  AttribDataClosestAnimState* inputStateData = outputStateData;
  NMP_ASSERT(inputStateData);

  // Dead blend delta time update (network dt)
  AttribDataUpdatePlaybackPos* deltaTime = parameters->getInputAttrib<AttribDataUpdatePlaybackPos>(8, ATTRIB_SEMANTIC_UPDATE_TIME_POS);

  // Dead blend transform rates
  AttribDataTransformBuffer* deadBlendRatesAttrib = parameters->getInputAttrib<AttribDataTransformBuffer>(9, ATTRIB_SEMANTIC_TRANSFORM_RATES);

  // Dead blend output transform state
  AttribDataTransformBuffer* curDeadBlendTransformsState = parameters->createOutputAttribReplaceTransformBuffer(10, ATTRIB_SEMANTIC_DEAD_BLEND_TRANSFORMS_STATE, numRigJoints);

  subTaskClosestAnimTransforms(
    inputTransforms,
    outputTransforms,
    rigAttribData,
    syncEventTrack,
    playbackPos,
    blendWeightAttrib,
    inputTrajDeltaTransform->m_deltaPos,
    inputTrajDeltaTransform->m_deltaAtt,
    outputStateData,
    inputStateData,
    deltaTime,
    deadBlendRatesAttrib,
    curDeadBlendTransformsState);
}
    
//----------------------------------------------------------------------------------------------------------------------   
namespace
{

//----------------------------------------------------------------------------------------------------------------------
// Distributes a root offset rotation over time into the trajectory channel so that the trajectory of the
// closest animation will be rotated to match up with the current orientation of the character root joint.
void subTaskClosestAnimTrajectoryDelta(
  NMP::Vector3* deltaTrajectoryPos,
  NMP::Quat* deltaTrajectoryAtt,
  bool* trajectoryFilteredOut,
  const NMP::Vector3& inputDeltaTrajectoryPos,
  const NMP::Quat& inputDeltaTrajectoryAtt,
  bool inputTrajectoryFilteredOut,
  AttribDataSyncEventTrack* syncEventTrack,
  AttribDataPlaybackPos* playbackPos,
  AttribDataClosestAnimState* inputStateData)
{
  // Copy data
  *deltaTrajectoryAtt = inputDeltaTrajectoryAtt;
  *deltaTrajectoryPos = inputDeltaTrajectoryPos;
  *trajectoryFilteredOut = inputTrajectoryFilteredOut;

  // Check if we need to perform the blend
  if (!inputStateData->m_blendCompleteFlag)
  {
    // Compute the offset quat interpolation factor
    float T = inputStateData->m_fractionThroughSource * syncEventTrack->m_syncEventTrack.getDuration();
    float wa, wb;

    // Check if the current playback position has looped. This may happen if the fraction
    // through the source is set to 1
    if (T > ERROR_LIMIT && playbackPos->m_currentPosReal >= playbackPos->m_previousPosReal)
    {
      // Get the fractional playback positions through the blending duration
      float ta = NMP::clampValue(playbackPos->m_previousPosReal / T, 0.0f, 1.0f);
      float tb = NMP::clampValue(playbackPos->m_currentPosReal / T, 0.0f, 1.0f);

      // Sin weight function
      wa = ta - 0.5f;
      wa *= NM_PI;
      wa = NMP::fastSin(wa);
      wa += 1.0f;
      wa *= 0.5f;

      // Sin weight function
      wb = tb - 0.5f;
      wb *= NM_PI;
      wb = NMP::fastSin(wb);
      wb += 1.0f;
      wb *= 0.5f;
    }
    else
    {
      wa = 0.0f;
      wb = 1.0f;
    }
    NMP_ASSERT(wb >= wa);

    // Find the representation of the identity in the same semi-arc as the offset quat
    NMP::Quat qIdentity(NMP::Quat::kIdentity);
    float fromDotTo = inputStateData->m_rootRotationOffset.dot(qIdentity);
    if (fromDotTo < 0)
    {
      fromDotTo = -fromDotTo;
      qIdentity = -qIdentity;
    }

    // Apply the delta trajectory offset rotation adjustment for this frame
    NMP::Quat dq;
    dq.fastSlerp(qIdentity, inputStateData->m_rootRotationOffset, wb - wa, fromDotTo);
    *deltaTrajectoryAtt = *deltaTrajectoryAtt * dq;

    // Apply the delta trajectory translation adjustment for this frame. i.e. rotate the
    // translation delta by the offset rotation for the aligned character.
    NMP::Quat qc;
    qc.fastSlerp(inputStateData->m_rootRotationOffset, qIdentity, wb, fromDotTo);
    *deltaTrajectoryPos = qc.rotateVector(*deltaTrajectoryPos);
  }
}

//----------------------------------------------------------------------------------------------------------------------  
}

//----------------------------------------------------------------------------------------------------------------------
// Distributes a root offset rotation over time into the trajectory channel so that the trajectory of the
// closest animation will be rotated to match up with the current orientation of the character root joint.
void TaskClosestAnimTrajectoryDelta(Dispatcher::TaskParameters* parameters)
{
  // Input trajectory delta transform
  AttribDataTrajectoryDeltaTransform* inputTrajDeltaTransform = parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);
  // Output trajectory delta transform
  AttribDataTrajectoryDeltaTransform* outputTrajDeltaTransform = parameters->createOutputAttrib<AttribDataTrajectoryDeltaTransform>(1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);
  // Sync event track
  AttribDataSyncEventTrack* syncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(2, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  // Current playback position in the active child
  AttribDataPlaybackPos* playbackPos = parameters->getInputAttrib<AttribDataPlaybackPos>(3, ATTRIB_SEMANTIC_TIME_POS);
  // Input closest anim state
  AttribDataClosestAnimState* inputStateData = parameters->getInputAttrib<AttribDataClosestAnimState>(4, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE);

  subTaskClosestAnimTrajectoryDelta(
    &outputTrajDeltaTransform->m_deltaPos,
    &outputTrajDeltaTransform->m_deltaAtt,
    &outputTrajDeltaTransform->m_filteredOut,
    inputTrajDeltaTransform->m_deltaPos,
    inputTrajDeltaTransform->m_deltaAtt,
    inputTrajDeltaTransform->m_filteredOut,
    syncEventTrack,
    playbackPos,
    inputStateData);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskClosestAnimTrajectoryDeltaAndTransforms(Dispatcher::TaskParameters* parameters)
{
  // Input transforms of active child
  AttribDataTransformBuffer* inputTransforms = parameters->getInputAttrib<AttribDataTransformBuffer>(0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
  const uint32_t numRigJoints = inputTransforms->m_transformBuffer->getLength();

  // Output transforms
  AttribDataTransformBuffer* outputTransforms = parameters->createOutputAttribTransformBuffer(1, numRigJoints, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);

  // Animation rig
  AttribDataRig* rigAttribData = parameters->getInputAttrib<AttribDataRig>(2, ATTRIB_SEMANTIC_RIG);

  // Sync event tack
  AttribDataSyncEventTrack* syncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(3, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);

  // Current playback position in the active child
  AttribDataPlaybackPos* playbackPos = parameters->getInputAttrib<AttribDataPlaybackPos>(4, ATTRIB_SEMANTIC_TIME_POS);

  // Dead blend control weight
  AttribDataFloat* blendWeightAttrib = parameters->getInputAttrib<AttribDataFloat>(5, ATTRIB_SEMANTIC_CP_FLOAT); // Optional

  // Output closest anim state, Index 7 is an InOut task parameter
  AttribDataClosestAnimState* outputStateData = parameters->createOutputAttribReplace<AttribDataClosestAnimState>(6, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE);
  AttribDataClosestAnimState* inputStateData = outputStateData;
  NMP_ASSERT(inputStateData);

  // Dead blend delta time update (network dt)
  AttribDataUpdatePlaybackPos* deltaTime = parameters->getInputAttrib<AttribDataUpdatePlaybackPos>(7, ATTRIB_SEMANTIC_UPDATE_TIME_POS);

  // Dead blend transform rates
  AttribDataTransformBuffer* deadBlendRatesAttrib = parameters->getInputAttrib<AttribDataTransformBuffer>(8, ATTRIB_SEMANTIC_TRANSFORM_RATES);

  // Dead blend output transform state
  AttribDataTransformBuffer* curDeadBlendTransformsState = parameters->createOutputAttribReplaceTransformBuffer(9, ATTRIB_SEMANTIC_DEAD_BLEND_TRANSFORMS_STATE, numRigJoints);

  subTaskClosestAnimTransforms(
    inputTransforms,
    outputTransforms,
    rigAttribData,
    syncEventTrack,
    playbackPos,
    blendWeightAttrib,
    *inputTransforms->m_transformBuffer->getPosQuatChannelPos(0),
    *inputTransforms->m_transformBuffer->getPosQuatChannelQuat(0),
    outputStateData,
    inputStateData,
    deltaTime,
    deadBlendRatesAttrib,
    curDeadBlendTransformsState);

  bool trajectoryFilteredOut;

  subTaskClosestAnimTrajectoryDelta(
    outputTransforms->m_transformBuffer->getPosQuatChannelPos(0),
    outputTransforms->m_transformBuffer->getPosQuatChannelQuat(0),
    &trajectoryFilteredOut,
    *inputTransforms->m_transformBuffer->getPosQuatChannelPos(0),
    *inputTransforms->m_transformBuffer->getPosQuatChannelQuat(0),
    !inputTransforms->m_transformBuffer->hasChannel(0),
    syncEventTrack,
    playbackPos,
    inputStateData);

  outputTransforms->m_transformBuffer->setChannelUsedFlag(0, !trajectoryFilteredOut);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
