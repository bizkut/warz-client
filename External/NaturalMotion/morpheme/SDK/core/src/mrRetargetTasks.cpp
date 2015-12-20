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
#include "morpheme/mrNetwork.h"
#include "morpheme/mrCommonTasks.h"
#include "morpheme/mrRigRetargetMapping.h"
#include "morpheme/Nodes/mrNodeRetarget.h"
#include "NMRetarget/NMFKRetarget.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
namespace
{

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void subTaskRetargetTransforms(
  Dispatcher::TaskParameters* parameters,
  const NMP::DataBuffer* inputTransforms,
  const AttribDataRig* inputRigAttrib,
  NMP::DataBuffer* outputTransforms,
  const AttribDataRetargetState* retargetState)
{
  //--------------------------------------------------------------------------------------------------------------------
  // Create a buffer to contain the source transforms.  This allows us to merge in the bind pose for any unused
  // channels, since we might actually need transforms for those channels.

  const uint32_t numSourceRigJoints = inputTransforms->getLength();

  // Get temp allocator.
  NMP::TempMemoryAllocator* tempAllocator = parameters->m_dispatcher->getTempMemoryAllocator();
  NMP_ASSERT(tempAllocator);
  NMP::TempMemoryAllocator* childAllocator = tempAllocator->createChildAllocator();

  // Create a transform buffer and copy in the source transforms
  NMP::Memory::Format format = NMP::DataBuffer::getPosQuatMemoryRequirements(numSourceRigJoints);
  NMP::DataBuffer* inputTMsPlusBindPose = NMP::DataBuffer::createPosQuat(childAllocator, format, numSourceRigJoints);
  inputTransforms->copyTo(inputTMsPlusBindPose);

  // Merge in the bind pose for any unused channels
  // OPTIMISE  We can remove this and just use the inputTransforms array.  However, the result is that
  // we will need to modify the solver to set all channels in a mapped group to unused if any channels
  // in that group are unused.  That's okay (the user can manually set the bind pose on unused channels
  // if they want) but is a bit unintuitive in that you don't see that channels are unused in the viewport.
  const NMP::DataBuffer* inputBindPose = inputRigAttrib->m_rig->getBindPose()->m_transformBuffer;
  for (uint32_t j = 0; j < numSourceRigJoints; ++j)
  {
    if (!inputTMsPlusBindPose->hasChannel(j))
    {
      const NMP::Vector3& jointPos = *inputBindPose->getPosQuatChannelPos(j);
      const NMP::Quat& jointQuat = *inputBindPose->getPosQuatChannelQuat(j);
      inputTMsPlusBindPose->setPosQuatChannelPos(j, jointPos);
      inputTMsPlusBindPose->setPosQuatChannelQuat(j, jointQuat);
    }
  }

  //----------------------------------------------------------------------------------------------------------------------
  // Call the solver
  retargetState->m_retargetSolver->retarget(
    inputTMsPlusBindPose,
    outputTransforms,
    childAllocator);

  // Force un-use channel 0 (the retargeter can add it)
  outputTransforms->setChannelUnused(0); // MORPH-21373: Remove this when possible

  // Recalculate full flag because channels may have been added
  outputTransforms->setFullFlag(
    outputTransforms->getUsedFlags()->calculateAreAllSet());

  //----------------------------------------------------------------------------------------------------------------------
  // Finally release the temporary buffer.
  childAllocator->memFree(inputTMsPlusBindPose);
  tempAllocator->destroyChildAllocator(childAllocator);
}

//----------------------------------------------------------------------------------------------------------------------
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
void TaskRetargetTransforms(Dispatcher::TaskParameters* parameters)
{
  //----------------------------------------------------------------------------------------------------------------------
  // Retrieve task parameters

  // Input transform buffer
  AttribDataTransformBuffer *inputTransforms =
    parameters->getInputAttrib<AttribDataTransformBuffer>(0, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);

  // Input rig
  const AttribDataRig *inputRigAttrib = parameters->getInputAttrib<AttribDataRig>(2, ATTRIB_SEMANTIC_RIG);

  // Output rig
  AttribDataRig *outputRigAttrib = parameters->getInputAttrib<AttribDataRig>(3, ATTRIB_SEMANTIC_RIG);
  const AnimRigDef *outputRig = outputRigAttrib->m_rig;
  const uint32_t numTargetRigJoints = outputRig->getNumBones();

  // Output transform buffer
  AttribDataTransformBuffer *outputTransforms =
    parameters->createOutputAttribTransformBuffer(1, numTargetRigJoints, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  // Fill the output transforms from the bind pose to act as the input to the retarget algorithm
  outputRig->getBindPose()->m_transformBuffer->copyTo(outputTransforms->m_transformBuffer);

  // The retarget state containing the solver
  AttribDataRetargetState *retargetState =
    parameters->getInputAttrib<AttribDataRetargetState>(4, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE);

  // Do the retargeting
  subTaskRetargetTransforms(
    parameters,
    inputTransforms->m_transformBuffer,
    inputRigAttrib,
    outputTransforms->m_transformBuffer,
    retargetState);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void subTaskRetargetTrajectoryDeltaTransform(
  const NMP::Vector3& inputTrajectoryDeltaPos,
  const NMP::Quat& inputTrajectoryDeltaAtt,
  NMP::Vector3* outputTrajectoryDeltaPos,
  NMP::Quat* outputTrajectoryDeltaAtt,
  bool* outputTrajectoryFilteredOut,
  const AnimRigDef* inputRig,
  const AnimRigDef* outputRig,
  const AttribDataRigRetargetMapping* inputRigRetargetMapping,
  const AttribDataRigRetargetMapping* outputRigRetargetMapping,
  float targetRigScale)
{
  //----------------------------------------------------------------------------------------------------------------------
  // Retarget the trajectory deltas

  // Get input transform as a PosQuat
  NMRU::GeomUtils::PosQuat deltaTransform = {inputTrajectoryDeltaPos, inputTrajectoryDeltaAtt};

  // Convert to reference rig
  uint32_t sourceTrajectoryIndex = inputRig->getTrajectoryBoneIndex();
  //  Apply scale
  deltaTransform.t /= inputRigRetargetMapping->m_retargetParams->rigScale;
  //  Re-reference to parent reference frame (parent is just the same joint in the previous frame,
  //  so has the same reference transform)
  NMRU::GeomUtils::multiplyInversePosQuat(
    deltaTransform, inputRigRetargetMapping->m_retargetParams->perJointParams[sourceTrajectoryIndex].referenceTransform);
  //  Now do a local transform into the reference frame
  NMRU::GeomUtils::premultiplyPosQuat(
    inputRigRetargetMapping->m_retargetParams->perJointParams[sourceTrajectoryIndex].referenceTransform, deltaTransform);

  // Do a local transform from the target reference frame into the target frame
  uint32_t targetTrajectoryIndex = outputRig->getTrajectoryBoneIndex();
  NMRU::GeomUtils::premultiplyInversePosQuat(
    outputRigRetargetMapping->m_retargetParams->perJointParams[targetTrajectoryIndex].referenceTransform, deltaTransform);
  // And move the joint back out of the parent reference frame into the parent frame (again, the parent
  // is just this joint in the previous frame, so it's the same reference transform)
  NMRU::GeomUtils::multiplyPosQuat(
    deltaTransform, outputRigRetargetMapping->m_retargetParams->perJointParams[targetTrajectoryIndex].referenceTransform);
  // Apply scale
  deltaTransform.t *= targetRigScale;

  // Set the output
  *outputTrajectoryDeltaPos = deltaTransform.t;
  *outputTrajectoryDeltaAtt = deltaTransform.q;
  *outputTrajectoryFilteredOut = false;
}

//----------------------------------------------------------------------------------------------------------------------
void TaskRetargetTrajectoryDeltaTransform(Dispatcher::TaskParameters *parameters)
{
  //----------------------------------------------------------------------------------------------------------------------
  // Retrieve task parameters

  // Input trajectory delta
  const AttribDataTrajectoryDeltaTransform* inputTrajectory =
    parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(
    0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);

  // Output trajectory delta
  AttribDataTrajectoryDeltaTransform* outputTrajectory = 
    parameters->createOutputAttrib<AttribDataTrajectoryDeltaTransform>(
    1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);

  // Early out if the trajectory is filtered out already
  if (inputTrajectory->m_filteredOut)
  {
    // Either we have filtered the trajectory channel or our child has.
    outputTrajectory->m_deltaAtt.identity();
    outputTrajectory->m_deltaPos.setToZero();
    outputTrajectory->m_filteredOut = true;

    return;
  }

  // Input rig
  const AttribDataRig* inputRigAttrib = parameters->getInputAttrib<AttribDataRig>(2, ATTRIB_SEMANTIC_RIG);
  const AnimRigDef* inputRig = inputRigAttrib->m_rig;

  // Output rig
  AttribDataRig* outputRigAttrib = parameters->getInputAttrib<AttribDataRig>(3, ATTRIB_SEMANTIC_RIG);
  const AnimRigDef* outputRig = outputRigAttrib->m_rig;

  // Input rig's retarget mappings
  const AttribDataRigRetargetMapping* inputRigRetargetMapping =
    parameters->getInputAttrib<AttribDataRigRetargetMapping>(4, ATTRIB_SEMANTIC_RIG_RETARGET_MAPPING);

  // Output rig's retarget mappings
  const AttribDataRigRetargetMapping* outputRigRetargetMapping =
    parameters->getInputAttrib<AttribDataRigRetargetMapping>(5, ATTRIB_SEMANTIC_RIG_RETARGET_MAPPING);

  // The scale factor for the target rig
  const AttribDataFloat* targetRigScaleAttrib =
    parameters->getOptionalInputAttrib<AttribDataFloat>(6, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE_2);
  float targetRigScale =
    targetRigScaleAttrib ? targetRigScaleAttrib->m_value : outputRigRetargetMapping->m_retargetParams->rigScale;

  subTaskRetargetTrajectoryDeltaTransform(
    inputTrajectory->m_deltaPos,
    inputTrajectory->m_deltaAtt,
    &outputTrajectory->m_deltaPos,
    &outputTrajectory->m_deltaAtt,
    &outputTrajectory->m_filteredOut,
    inputRig,
    outputRig,
    inputRigRetargetMapping,
    outputRigRetargetMapping,
    targetRigScale);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskRetargetTrajectoryDeltaAndTransforms(Dispatcher::TaskParameters* parameters)
{
  //----------------------------------------------------------------------------------------------------------------------
  // Retrieve task parameters

  // Input transform buffer
  AttribDataTransformBuffer* inputTransforms =
    parameters->getInputAttrib<AttribDataTransformBuffer>(0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);

  // Input rig
  const AttribDataRig* inputRigAttrib = parameters->getInputAttrib<AttribDataRig>(2, ATTRIB_SEMANTIC_RIG);

  // Output rig
  AttribDataRig* outputRigAttrib = parameters->getInputAttrib<AttribDataRig>(3, ATTRIB_SEMANTIC_RIG);
  const AnimRigDef* outputRig = outputRigAttrib->m_rig;
  const uint32_t numTargetRigJoints = outputRig->getNumBones();

  // Output transform buffer
  AttribDataTransformBuffer* outputTransforms =
    parameters->createOutputAttribTransformBuffer(1, numTargetRigJoints, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
  // Fill the output transforms from the bind pose to act as the input to the retarget algorithm
  outputRig->getBindPose()->m_transformBuffer->copyTo(outputTransforms->m_transformBuffer);

  // The retarget state containing the solver
  AttribDataRetargetState* retargetState =
    parameters->getInputAttrib<AttribDataRetargetState>(4, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE);

  // Input rig's retarget mappings
  const AttribDataRigRetargetMapping* inputRigRetargetMapping =
    parameters->getInputAttrib<AttribDataRigRetargetMapping>(5, ATTRIB_SEMANTIC_RIG_RETARGET_MAPPING);

  // Output rig's retarget mappings
  const AttribDataRigRetargetMapping* outputRigRetargetMapping =
    parameters->getInputAttrib<AttribDataRigRetargetMapping>(6, ATTRIB_SEMANTIC_RIG_RETARGET_MAPPING);

  // The scale factor for the target rig
  const AttribDataFloat* targetRigScaleAttrib =
    parameters->getOptionalInputAttrib<AttribDataFloat>(7, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE_2);
  float targetRigScale =
    targetRigScaleAttrib ? targetRigScaleAttrib->m_value : outputRigRetargetMapping->m_retargetParams->rigScale;


  if (!inputTransforms->m_transformBuffer->hasChannel(0))
  {
    // Either we have filtered the trajectory channel or our child has.
    outputTransforms->m_transformBuffer->getPosQuatChannelPos(0)->setToZero();
    outputTransforms->m_transformBuffer->getPosQuatChannelQuat(0)->identity();
    outputTransforms->m_transformBuffer->setChannelUnused(0);
  }
  else
  {
    bool outputChannelFilteredOut = false;
    subTaskRetargetTrajectoryDeltaTransform(
      *inputTransforms->m_transformBuffer->getPosQuatChannelPos(0),
      *inputTransforms->m_transformBuffer->getPosQuatChannelQuat(0),
      outputTransforms->m_transformBuffer->getPosQuatChannelPos(0),
      outputTransforms->m_transformBuffer->getPosQuatChannelQuat(0),
      &outputChannelFilteredOut,
      inputRigAttrib->m_rig,
      outputRig,
      inputRigRetargetMapping,
      outputRigRetargetMapping,
      targetRigScale);

    outputTransforms->m_transformBuffer->setChannelUsedFlag(0, !outputChannelFilteredOut);
  }
  
  // Retarget the transforms
  subTaskRetargetTransforms(
    parameters,
    inputTransforms->m_transformBuffer,
    inputRigAttrib,
    outputTransforms->m_transformBuffer,
    retargetState);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void subTaskScaleCharacterTransforms(
  const NMP::DataBuffer& inputTransforms,
  const AttribDataScaleCharacterState& state,
  NMP::DataBuffer* outputTransforms)
{
  //----------------------------------------------------------------------------------------------------------------------
  // Prepare the output
  inputTransforms.copyTo(outputTransforms);

  // Scale the transforms
  const float* jointScales = state.m_jointScale;
  const uint32_t numRigJoints = inputTransforms.getLength();
  for(uint32_t j = 0; j < numRigJoints; ++j)
  {
    NMP::Vector3& outputPos = *(outputTransforms->getPosQuatChannelPos(j));
    float scale = jointScales[j];
    outputTransforms->setPosQuatChannelPos(j, scale * outputPos);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskScaleCharacterTransforms(Dispatcher::TaskParameters* parameters)
{
  //----------------------------------------------------------------------------------------------------------------------
  // Retrieve task parameters

  // Input transform buffer
  AttribDataTransformBuffer* inputTransforms =
    parameters->getInputAttrib<AttribDataTransformBuffer>(0, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  const uint32_t numRigJoints = inputTransforms->m_transformBuffer->getLength();

  // Output transform buffer
  AttribDataTransformBuffer* outputTransforms =
    parameters->createOutputAttribTransformBuffer(1, numRigJoints, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);

  // The state attrib with the rig and joint scales
  AttribDataScaleCharacterState* state =
    parameters->getInputAttrib<AttribDataScaleCharacterState>(2, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE);

  subTaskScaleCharacterTransforms(*inputTransforms->m_transformBuffer, *state, outputTransforms->m_transformBuffer);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskScaleCharacterDeltaTransform(Dispatcher::TaskParameters* parameters)
{
  //----------------------------------------------------------------------------------------------------------------------
  // Retrieve task parameters

  // Input trajectory delta
  const AttribDataTrajectoryDeltaTransform* inputTrajectory =
    parameters->getOptionalInputAttrib<AttribDataTrajectoryDeltaTransform>(
    0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);

  // Output trajectory delta
  AttribDataTrajectoryDeltaTransform* outputTrajectory = 
    parameters->createOutputAttrib<AttribDataTrajectoryDeltaTransform>(
    1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);

  // The rig - from where we retrieve the trajectory bone index
  const AttribDataRig* rigAttrib =
    parameters->getInputAttrib<AttribDataRig>(2, ATTRIB_SEMANTIC_RIG);
  const AnimRigDef* rigDef = rigAttrib->m_rig;

  // The state attrib with the rig and joint scales
  AttribDataScaleCharacterState *state =
    parameters->getInputAttrib<AttribDataScaleCharacterState>(3, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE);

  //----------------------------------------------------------------------------------------------------------------------
  // Prepare the output

  // Copy the deltas from the input
  outputTrajectory->m_deltaAtt = inputTrajectory->m_deltaAtt;

  // Retrieve the required scale for the trajectory joint
  float scale = state->m_jointScale[rigDef->getTrajectoryBoneIndex()];

  // Do the scaling
  outputTrajectory->m_deltaPos = inputTrajectory->m_deltaPos * scale;
}

//----------------------------------------------------------------------------------------------------------------------
void TaskScaleCharacterDeltaAndTransforms(Dispatcher::TaskParameters* parameters)
{
  //----------------------------------------------------------------------------------------------------------------------
  // Retrieve task parameters

  // Input trajectory delta
  const AttribDataTransformBuffer* inputTransformsAttrib =
    parameters->getOptionalInputAttrib<AttribDataTransformBuffer>(
    0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
  const uint32_t numRigJoints = inputTransformsAttrib->m_transformBuffer->getLength();

  // Output trajectory delta
  AttribDataTransformBuffer* outputTransformsAttrib = 
    parameters->createOutputAttribTransformBuffer(
    1, numRigJoints, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);

  // The rig - from where we retrieve the trajectory bone index
  const AttribDataRig* rigAttrib =
    parameters->getInputAttrib<AttribDataRig>(2, ATTRIB_SEMANTIC_RIG);
  const AnimRigDef* rigDef = rigAttrib->m_rig;

  // The state attrib with the rig and joint scales
  AttribDataScaleCharacterState* state =
    parameters->getInputAttrib<AttribDataScaleCharacterState>(3, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE);

  NMP::DataBuffer* inputTransforms = inputTransformsAttrib->m_transformBuffer;
  NMP::DataBuffer* outputTransforms = outputTransformsAttrib->m_transformBuffer;

  // Scale the transforms
  subTaskScaleCharacterTransforms(*inputTransforms, *state, outputTransforms);

  // Scale the trajectory
  float* jointScales = state->m_jointScale;
  float trajectoryScale = jointScales[rigDef->getTrajectoryBoneIndex()];
  const NMP::Vector3& inputTrajectory = *inputTransforms->getPosQuatChannelPos(0);
  outputTransforms->setPosQuatChannelPos(0, inputTrajectory * trajectoryScale);
  outputTransforms->setPosQuatChannelQuat(0, *inputTransforms->getPosQuatChannelQuat(0));
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace MR
