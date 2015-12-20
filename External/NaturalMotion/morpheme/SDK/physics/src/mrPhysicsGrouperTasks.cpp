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
#include "morpheme/mrNetworkLogger.h"
#include "morpheme/mrAttribData.h"
#include "physics/mrPhysicsRig.h"
#include "physics/mrPhysicsScene.h"
#include "physics/mrPhysicsAttribData.h"
#include "morpheme/mrCharacterControllerAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
void TaskPhysicsGrouperUpdateTrajectory(Dispatcher::TaskParameters* parameters)
{
  NET_LOG_ENTER_FUNC();
  AttribDataTrajectoryDeltaTransform* inputDeltaTrasform =
    parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);
  AttribDataTrajectoryDeltaTransform* outputDeltaTransform =
    parameters->createOutputAttrib<AttribDataTrajectoryDeltaTransform>(1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);

  outputDeltaTransform->m_deltaPos = inputDeltaTrasform->m_deltaPos;
  outputDeltaTransform->m_deltaAtt = inputDeltaTrasform->m_deltaAtt;
  outputDeltaTransform->m_filteredOut = inputDeltaTrasform->m_filteredOut;
}

//----------------------------------------------------------------------------------------------------------------------
void TaskPhysicsGrouperUpdateTransforms(Dispatcher::TaskParameters* parameters)
{
  NET_LOG_ENTER_FUNC();
  AttribDataRig* rigAttribData = parameters->getInputAttrib<AttribDataRig>(0, ATTRIB_SEMANTIC_RIG);
  AttribDataTransformBuffer* outputTransforms = parameters->createOutputAttribTransformBuffer(1, rigAttribData->m_rig->getNumBones(), ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  AttribDataUIntArray* channelIndices = parameters->getInputAttrib<AttribDataUIntArray>(2, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);
  AttribDataTransformBuffer* child0Transforms = parameters->getInputAttrib<AttribDataTransformBuffer>(3, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  AttribDataTransformBuffer* child1Transforms = parameters->getInputAttrib<AttribDataTransformBuffer>(4, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);

  NMP_ASSERT(channelIndices->m_numValues == 0 || channelIndices->m_numValues == child0Transforms->m_transformBuffer->getLength());
  NMP_ASSERT(channelIndices->m_numValues == 0 || channelIndices->m_numValues == child1Transforms->m_transformBuffer->getLength());

  AttribDataTransformBuffer* childTransforms[] = { child0Transforms, child1Transforms };

  uint32_t numBones = child0Transforms->m_transformBuffer->getLength();

  if (channelIndices->m_numValues)
  {
    for (uint32_t i = 0 ; i < numBones ; ++i)
    {
      uint32_t childIndex = channelIndices->m_values[i];
      outputTransforms->m_transformBuffer->setPosQuatChannelPos(i, *childTransforms[childIndex]->m_transformBuffer->getPosQuatChannelPos(i));
      outputTransforms->m_transformBuffer->setPosQuatChannelQuat(i, *childTransforms[childIndex]->m_transformBuffer->getPosQuatChannelQuat(i));
      outputTransforms->m_transformBuffer->setChannelUsedFlag(i, childTransforms[childIndex]->m_transformBuffer->hasChannel(i));
    }

    // the full flag doesn't get updated during the setChannelUsed so calculated it here
    outputTransforms->m_transformBuffer->calculateFullFlag();
  }
  else
  {
    // If there are no channel indices then copy the child0 straight to the output
    childTransforms[0]->m_transformBuffer->copyTo(outputTransforms->m_transformBuffer);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskPhysicsGrouperUpdateTransformsPrePhysics(Dispatcher::TaskParameters* parameters)
{
  NET_LOG_ENTER_FUNC();

  AttribDataPhysicsRig* physicsRigAttrib = parameters->getInputAttrib<AttribDataPhysicsRig>(0, ATTRIB_SEMANTIC_PHYSICS_RIG);
  AttribDataCharacterProperties* characterController = parameters->getInputAttrib<AttribDataCharacterProperties>(1, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES);
  AttribDataBool* outputUpdated = parameters->createOutputAttrib<AttribDataBool>(2, ATTRIB_SEMANTIC_PRE_PHYSICS_TRANSFORMS);
  (void)outputUpdated;
  AttribDataPhysicsGrouperConfig* config = parameters->getInputAttrib<AttribDataPhysicsGrouperConfig>(3, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE);
  AttribDataAnimToPhysicsMap* animToPhysics = parameters->getInputAttrib<AttribDataAnimToPhysicsMap>(4, ATTRIB_SEMANTIC_ANIM_TO_PHYSICS_MAP);

  if (!config->m_convertToPhysics[0] && !config->m_convertToPhysics[1])
    return;

  NMP::Matrix34 worldRoot = characterController->m_worldRootTransform;

  PhysicsRig* physicsRig = physicsRigAttrib->m_physicsRig;
  NMP_ASSERT(physicsRig);
  if (!physicsRig || !physicsRig->getPhysicsRigDef())
    return;

  PhysicsScene* ps = physicsRig->getPhysicsScene();
  float timeStep = ps ? ps->getNextPhysicsTimeStep() : 1.0f;

  uint32_t paramIndex = 5;

  AttribDataTransformBuffer* mergedTransforms = 0;
  if (config->m_convertToPhysics[0] || config->m_convertToPhysics[1])
  {
    mergedTransforms = parameters->getInputAttrib<AttribDataTransformBuffer>(paramIndex++, ATTRIB_SEMANTIC_MERGED_PHYSICS_RIG_TRANSFORM_BUFFER);
    ++paramIndex; // skip past char controller updated
  }

  // Physics TODO add previous buffer to the calls below so that velocity info is available
  if (config->m_convertToPhysics[0])
  {
    AttribDataTransformBuffer* child0Transforms = mergedTransforms;
    AttribDataBoolArray* mask0 = parameters->getInputAttrib<AttribDataBoolArray>(paramIndex++, ATTRIB_SEMANTIC_OUTPUT_MASK);
    PhysicsRig::PartChooser chooser0(mask0, animToPhysics->m_animToPhysicsMap);

    physicsRig->applyHardKeyframing(
      *child0Transforms->m_transformBuffer,
      0,
      *physicsRig->getAnimRigDef()->getBindPose()->m_transformBuffer,
      worldRoot,
      0,
      false,
      10.0f,
      false,
      timeStep,
      chooser0);
  }
  if (config->m_convertToPhysics[1])
  {
    AttribDataTransformBuffer* child1Transforms = mergedTransforms;
    AttribDataBoolArray* mask1 = parameters->getInputAttrib<AttribDataBoolArray>(paramIndex++, ATTRIB_SEMANTIC_OUTPUT_MASK);
    PhysicsRig::PartChooser chooser1(mask1, animToPhysics->m_animToPhysicsMap);

    physicsRig->applyHardKeyframing(
      *child1Transforms->m_transformBuffer,
      0,
      *physicsRig->getAnimRigDef()->getBindPose()->m_transformBuffer,
      worldRoot,
      0,
      false,
      10.0f,
      false,
      timeStep,
      chooser1);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskPhysicsGrouperGetOutputMaskBase(Dispatcher::TaskParameters* parameters)
{
  NET_LOG_ENTER_FUNC();
  AttribDataRig* rigAttribData = parameters->getInputAttrib<AttribDataRig>(0, ATTRIB_SEMANTIC_RIG);
  AttribDataUIntArray* channelIndices = parameters->getInputAttrib<AttribDataUIntArray>(1, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);
  AttribDataBoolArray* parentMask = parameters->getInputAttrib<AttribDataBoolArray>(2, ATTRIB_SEMANTIC_OUTPUT_MASK);
  const uint32_t numRigJoints = rigAttribData->m_rig->getNumBones();
  AttribDataArrayCreateDesc desc(numRigJoints);  
  AttribDataBoolArray* outputMask = parameters->createOutputAttrib<AttribDataBoolArray>(3, ATTRIB_SEMANTIC_OUTPUT_MASK, &desc);
  NMP_ASSERT(channelIndices->m_numValues == numRigJoints || channelIndices->m_numValues == 0);
  NMP_ASSERT(parentMask->m_numValues == numRigJoints);

  if (channelIndices->m_numValues == 0)
  {
    for (uint32_t i = 0 ; i < numRigJoints ; ++i)
      outputMask->m_values[i] = true; // default to the base
  }
  else
  {
    for (uint32_t i = 0 ; i < numRigJoints ; ++i)
      outputMask->m_values[i] = (channelIndices->m_values[i] == 0) && (parentMask->m_values[i] == true);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskPhysicsGrouperGetOutputMaskOverride(Dispatcher::TaskParameters* parameters)
{
  NET_LOG_ENTER_FUNC();
  AttribDataRig* rigAttribData = parameters->getInputAttrib<AttribDataRig>(0, ATTRIB_SEMANTIC_RIG);
  AttribDataUIntArray* channelIndices = parameters->getInputAttrib<AttribDataUIntArray>(1, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);
  AttribDataBoolArray* parentMask = parameters->getInputAttrib<AttribDataBoolArray>(2, ATTRIB_SEMANTIC_OUTPUT_MASK);
  const uint32_t numRigJoints = rigAttribData->m_rig->getNumBones();
  AttribDataArrayCreateDesc desc(numRigJoints);  
  AttribDataBoolArray* outputMask = parameters->createOutputAttrib<AttribDataBoolArray>(3, ATTRIB_SEMANTIC_OUTPUT_MASK, &desc);

  NMP_ASSERT(channelIndices->m_numValues == numRigJoints || channelIndices->m_numValues == 0);
  NMP_ASSERT(parentMask->m_numValues == numRigJoints);

  if (channelIndices->m_numValues == 0)
  {
    for (uint32_t i = 0 ; i < numRigJoints ; ++i)
      outputMask->m_values[i] = false; // default to the base
  }
  else
  {
    for (uint32_t i = 0 ; i < numRigJoints ; ++i)
      outputMask->m_values[i] = (channelIndices->m_values[i] == 1) && (parentMask->m_values[i] == true);
  }
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
