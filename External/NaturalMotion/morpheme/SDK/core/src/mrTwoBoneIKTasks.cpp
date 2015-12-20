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
#include "morpheme/mrCharacterControllerAttribData.h"
#include "morpheme/mrTaskUtilities.h"
#include "morpheme/mrBlendOps.h"

#include "NMIK/NMTwoBoneIK.h"

//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
void TaskTwoBoneIKSetup(Dispatcher::TaskParameters* parameters)
{
  //--------------------------------------------
  // Task parameters

  // The character controller, for determining the world-space location of the character
  AttribDataCharacterProperties* characterControllerAttrib =
    parameters->getInputAttrib<AttribDataCharacterProperties>(0, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES);

  // The output trajectory transform, extracted from the character controller
  AttribDataTransform* trajectoryTransform =
    parameters->createOutputAttrib<AttribDataTransform>(1, ATTRIB_SEMANTIC_TRAJECTORY_TRANSFORM);

  // Copy across
  trajectoryTransform->m_pos = characterControllerAttrib->m_prevWorldRootTransform.translation();
  trajectoryTransform->m_att = characterControllerAttrib->m_prevWorldRootTransform.toQuat();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void subTaskTwoBoneIKTransforms(
  const AttribDataTransformBuffer* localBufferAttrib,
  const NMP::Vector3& deltaPos,
  const NMP::Quat& deltaQuat,
  const AttribDataVector3* effectorTargetAttrib,
  const AttribDataVector4* targetOrientationAttrib,
  const AttribDataFloat* swivelAngleAttrib,
  const AttribDataFloat* ikFkBlendWeightAttrib,
  const AttribDataFloat* swivelContributionToOrientationAttrib,
  const AttribDataTwoBoneIKSetup* twoBoneIKSetupAttrib,
  const AttribDataTwoBoneIKChain* twoBoneIKChainAttrib,
  const AttribDataRig* rigAttrib,
  AttribDataTransformBuffer* outputBufferAttrib,
  const AttribDataTransform* trajectoryTransform)
{
  AnimRigDef* rig = rigAttrib->m_rig;

  // Aliases for task parameters and their contents
  NMP::DataBuffer* outputBuffer = outputBufferAttrib->m_transformBuffer;
  const NMP::DataBuffer* localBuffer = localBufferAttrib->m_transformBuffer;
  //
  const bool& updateTargetByDeltas = twoBoneIKSetupAttrib->m_updateTargetByDeltas;
  const bool& worldSpaceTarget = twoBoneIKSetupAttrib->m_worldSpaceTarget;
  const bool& globalReferenceAxis = twoBoneIKChainAttrib->m_globalReferenceAxis;
  const bool& keepEndEffOrientation = twoBoneIKSetupAttrib->m_keepEndEffOrientation;
  const bool& userControlledOrientation = twoBoneIKSetupAttrib->m_userControlledOrientation;
  // The hinge axis and reference axes are copies because they may be modified
  NMP::Vector3 midJointRotationAxis = twoBoneIKChainAttrib->m_midJointRotAxis;
  NMP::Vector3 midJointReferenceAxis = twoBoneIKChainAttrib->m_midJointReferenceAxis;

  // Initialise connected control param values or use the defaults.
  NMP::Quat targetOrientation = twoBoneIKSetupAttrib->m_defaultTargetOrientation;
  if (targetOrientationAttrib)
  {
    targetOrientation = targetOrientationAttrib->m_value;
    targetOrientation.fastNormalise();
  }

  // Same for the target position since it might be adjusted for the delta trajectory
  NMP::Vector3 targetPos;

  // Adjust the target position if necessary
  if (twoBoneIKSetupAttrib->m_useSpecifiedJointAsTarget)
  {
    NMP::Vector3 csPos;
    NMP::Quat csQuat;
    BlendOpsBase::accumulateTransform(twoBoneIKChainAttrib->m_targetJointIndex,
      NMP::Vector3Zero(),
      NMP::QuatIdentity(),
      localBuffer,
      rig,
      csPos,
      csQuat);

    targetPos = csPos + csQuat.rotateVector(effectorTargetAttrib->m_value);

    // Adjust the target orientation if necessary
    if (twoBoneIKSetupAttrib->m_useSpecifiedJointOrientation)
    {
      targetOrientation = csQuat;
      targetOrientation.fastNormalise();
    }
  }
  else
  {
    targetPos = effectorTargetAttrib->m_value;
  }

  float swivelAngle = swivelAngleAttrib->m_value;
  float ikFkBlendWeight = ikFkBlendWeightAttrib->m_value;
  float swivelContributionToOrientation = swivelContributionToOrientationAttrib->m_value;
  
  // For world-space targets, retrieve the current trajectory and move the target into character space.
  // We have to use the previous frame's trajectory because the current frame's value may contain some but
  // not necessarily all of the character motion for this frame, depending whether we are pre- or post-physics.
  if (worldSpaceTarget)
  {
    // Put the target into the space of the character at the last frame
    targetPos = trajectoryTransform->m_att.inverseRotateVector(targetPos - trajectoryTransform->m_pos);
    targetOrientation = ~trajectoryTransform->m_att * targetOrientation;

    // Update for the trajectory deltas calculated by the network so far
    targetPos = deltaQuat.inverseRotateVector(targetPos - deltaPos);
    targetOrientation = ~deltaQuat * targetOrientation;
  }
  // For character-space targets: if required, update the target transform by one trajectory delta.
  // This is useful when you are targeting world-space coordinates but, when converting
  // this to character space, were only able to retrieve the character root transform
  // at the last update, when obviously you really wanted the target transform relative
  // to the character after /this/ update.
  else if (updateTargetByDeltas)
  {
    targetPos = deltaQuat.inverseRotateVector(targetPos - deltaPos);
    targetOrientation = ~deltaQuat * targetOrientation;
  }

  //--------------------------------------------
  // Set up IK chain geometry

  // Copy input buffer to output buffer.  I will then be modifying the output buffer at the
  // joints of the chain.
  localBuffer->copyTo(outputBuffer);

  // Do rig-specific initialisations.
  int32_t jointIndex[4];
  jointIndex[2] = twoBoneIKChainAttrib->m_endJointIndex;
  // The present solution does not deal with non-sequential IK chains (i.e. with intermediate joints
  // that the IK does not try to rotate).
  NMP_ASSERT(twoBoneIKSetupAttrib->m_assumeSimpleHierarchy);
  jointIndex[1] = rig->getParentBoneIndex(jointIndex[2]);
  jointIndex[0] = rig->getParentBoneIndex(jointIndex[1]);

  // Get the world transform of the parent of the root joint, to relate the IK chain coordinate frame
  // to the target coordinate frame.
  NMRU::GeomUtils::PosQuat rootParentT = NMRU::GeomUtils::identityPosQuat();
  int32_t j = jointIndex[0];
  while ((j = rig->getParentBoneIndex(j)) > 0)
  {
    const NMP::Vector3* p = outputBuffer->getPosQuatChannelPos(j);
    const NMP::Quat* q = outputBuffer->getPosQuatChannelQuat(j);
    if (!outputBuffer->hasChannel(j))
    {
      p = rig->getBindPoseBonePos(j);
      q = rig->getBindPoseBoneQuat(j);
    }
    NMRU::GeomUtils::PosQuat jointT = {*p, *q};
    NMRU::GeomUtils::multiplyPosQuat(rootParentT, jointT);
  }

  // Set up the IK geometry, adding missing channels from the bind pose and activating them in the
  // transform buffer, since we are modifying them.
  NMRU::GeomUtils::PosQuat ikGeometry[4];
  for (uint32_t i = 0; i < 3; ++i)
  {
    int32_t jointI = jointIndex[i];
    if (!outputBuffer->hasChannel(jointI))
    {
      ikGeometry[i].t = *rig->getBindPoseBonePos(jointI);
      ikGeometry[i].q = *rig->getBindPoseBoneQuat(jointI);
      outputBuffer->setChannelUsed(jointI);
    }
    else
    {
      ikGeometry[i].t = *outputBuffer->getPosQuatChannelPos(jointI);
      ikGeometry[i].q = *outputBuffer->getPosQuatChannelQuat(jointI);
    }
  }
  // The fourth 'joint' is the end effector, which is co-located with the end joint in this case.
  ikGeometry[3] = NMRU::GeomUtils::identityPosQuat();

  //--------------------------------------------
  // Get the TwoBoneIK parameters
  NMRU::TwoBoneIK::Params ikParams;
  ikParams.hingeAxis = midJointRotationAxis;
  ikParams.midJointRedundancyParams.controlPoleVector = midJointRotationAxis;
  ikParams.midJointRedundancyParams.referencePoleVector = midJointReferenceAxis;
  if (globalReferenceAxis)
  {
    ikParams.midJointRedundancyParams.referencePoleVector = rootParentT.q.inverseRotateVector(midJointReferenceAxis);
  }
  ikParams.midJointRedundancyParams.active = true;
  ikParams.endJointOrientationWeight = userControlledOrientation || keepEndEffOrientation ? 1.0f : 0;
  ikParams.swivelOrientationWeight = swivelContributionToOrientation;
  ikParams.swivel = swivelAngle;
  ikParams.preventBendingBackwards = true;

  // Get the wrist transform in the coordinate frame of the root parent
  NMRU::GeomUtils::PosQuat target = {targetPos, targetOrientation};
  NMRU::GeomUtils::multiplyInversePosQuat(target, rootParentT);

  // Replace the target orientation with the current end effector orientation, if it is to be retained.
  if (keepEndEffOrientation)
  {
    target.q = ikGeometry[0].q * ikGeometry[1].q * ikGeometry[2].q;
  }

  //---------------------------------
  // SOLVE
  NMRU::TwoBoneIK::solveWithEndJoint(&ikParams, ikGeometry, target);

  // *************************************************************************************

  // Set results in output buffer as weighted blends
  for (uint32_t i = 0; i < 3; ++i)
  {
    uint32_t jointI = jointIndex[i];
    NMP_ASSERT(outputBuffer->hasChannel(jointI));
    NMP::Quat* outputQuat = outputBuffer->getPosQuatChannelQuat(jointI);
    NMRU::GeomUtils::shortestArcFastSlerp(*outputQuat, ikGeometry[i].q, ikFkBlendWeight);
  }

  // *************************************************************************************

  // This node may have added channels, so we need to recalculate the Full flag
  outputBuffer->calculateFullFlag();
}

//----------------------------------------------------------------------------------------------------------------------
void TaskTwoBoneIKTransforms(Dispatcher::TaskParameters* parameters)
{
  //--------------------------------------------
  // Task parameters

  // Local buffer - the input transforms
  const AttribDataTransformBuffer* localBufferAttrib =
    parameters->getInputAttrib<AttribDataTransformBuffer>(0, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);

  // Trajectory delta for this frame
  const AttribDataTrajectoryDeltaTransform* deltaTransformAttrib =
    parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);

  // IK target position
  const AttribDataVector3* effectorTargetAttrib =
    parameters->getInputAttrib<AttribDataVector3>(2, ATTRIB_SEMANTIC_CP_VECTOR3);

  // IK target orientation (Optional - runtime defined)
  const AttribDataVector4* targetOrientationAttrib =
    parameters->getOptionalInputAttrib<AttribDataVector4>(3, ATTRIB_SEMANTIC_CP_VECTOR4);

  // Angle of swivel for controlling redundant rotation at the root of the IK system
  const AttribDataFloat* swivelAngleAttrib =
    parameters->getInputAttrib<AttribDataFloat>(4, ATTRIB_SEMANTIC_CP_FLOAT);

  // Weight for blending between the input and the IK solution
  const AttribDataFloat* ikFkBlendWeightAttrib =
    parameters->getInputAttrib<AttribDataFloat>(5, ATTRIB_SEMANTIC_CP_FLOAT);

  // Weight to control how much swivel is used to help achieve the desired orientation, as opposed to
  // doing it all with rotation at the end joint
  const AttribDataFloat* swivelContributionToOrientationAttrib =
    parameters->getInputAttrib<AttribDataFloat>(6, ATTRIB_SEMANTIC_CP_FLOAT);

  // The non-anim-set dependent IK attributes
  const AttribDataTwoBoneIKSetup* twoBoneIKSetupAttrib =
    parameters->getInputAttrib<AttribDataTwoBoneIKSetup>(7, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  // The anim-set dependent IK attributes
  const AttribDataTwoBoneIKChain* twoBoneIKChainAttrib =
    parameters->getInputAttrib<AttribDataTwoBoneIKChain>(8, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);

  // The rig, with hierarchy and bind pose
  const AttribDataRig* rigAttrib = parameters->getInputAttrib<AttribDataRig>(9, ATTRIB_SEMANTIC_RIG);

  // The output geometry
  AttribDataTransformBuffer* outputBufferAttrib =
    parameters->createOutputAttribTransformBuffer(10, rigAttrib->m_rig->getNumBones(), ATTRIB_SEMANTIC_TRANSFORM_BUFFER);

  // The world space trajectory transform from the previous frame
  const AttribDataTransform* trajectoryTransform =
    parameters->getInputAttrib<AttribDataTransform>(11, ATTRIB_SEMANTIC_TRAJECTORY_TRANSFORM);

  subTaskTwoBoneIKTransforms(
    localBufferAttrib,
    deltaTransformAttrib->m_deltaPos,
    deltaTransformAttrib->m_deltaAtt,
    effectorTargetAttrib,
    targetOrientationAttrib,
    swivelAngleAttrib,
    ikFkBlendWeightAttrib,
    swivelContributionToOrientationAttrib,
    twoBoneIKSetupAttrib,
    twoBoneIKChainAttrib,
    rigAttrib,
    outputBufferAttrib,
    trajectoryTransform);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskTwoBoneIKTrajectoryDeltaAndTransforms(Dispatcher::TaskParameters* parameters)
{
  //--------------------------------------------
  // Task parameters

  // Local buffer - the input transforms
  const AttribDataTransformBuffer* localBufferAttrib =
    parameters->getInputAttrib<AttribDataTransformBuffer>(0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);

  // IK target position
  const AttribDataVector3* effectorTargetAttrib =
    parameters->getInputAttrib<AttribDataVector3>(1, ATTRIB_SEMANTIC_CP_VECTOR3);

  // IK target orientation (Optional - runtime defined)
  const AttribDataVector4* targetOrientationAttrib =
    parameters->getOptionalInputAttrib<AttribDataVector4>(2, ATTRIB_SEMANTIC_CP_VECTOR4);

  // Angle of swivel for controlling redundant rotation at the root of the IK system
  const AttribDataFloat* swivelAngleAttrib =
    parameters->getInputAttrib<AttribDataFloat>(3, ATTRIB_SEMANTIC_CP_FLOAT);

  // Weight for blending between the input and the IK solution
  const AttribDataFloat* ikFkBlendWeightAttrib =
    parameters->getInputAttrib<AttribDataFloat>(4, ATTRIB_SEMANTIC_CP_FLOAT);

  // Weight to control how much swivel is used to help achieve the desired orientation, as opposed to
  // doing it all with rotation at the end joint
  const AttribDataFloat* swivelContributionToOrientationAttrib =
    parameters->getInputAttrib<AttribDataFloat>(5, ATTRIB_SEMANTIC_CP_FLOAT);

  // The non-anim-set dependent IK attributes
  const AttribDataTwoBoneIKSetup* twoBoneIKSetupAttrib =
    parameters->getInputAttrib<AttribDataTwoBoneIKSetup>(6, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  // The anim-set dependent IK attributes
  const AttribDataTwoBoneIKChain* twoBoneIKChainAttrib =
    parameters->getInputAttrib<AttribDataTwoBoneIKChain>(7, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);

  // The rig, with hierarchy and bind pose
  const AttribDataRig* rigAttrib = parameters->getInputAttrib<AttribDataRig>(8, ATTRIB_SEMANTIC_RIG);

  // The output geometry
  AttribDataTransformBuffer* outputBufferAttrib =
    parameters->createOutputAttribTransformBuffer(9, rigAttrib->m_rig->getNumBones(), ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);

  // The world space trajectory transform from the previous frame
  const AttribDataTransform* trajectoryTransform =
    parameters->getInputAttrib<AttribDataTransform>(10, ATTRIB_SEMANTIC_TRAJECTORY_TRANSFORM);

  subTaskTwoBoneIKTransforms(
    localBufferAttrib,
    *localBufferAttrib->m_transformBuffer->getPosQuatChannelPos(0),
    *localBufferAttrib->m_transformBuffer->getPosQuatChannelQuat(0),
    effectorTargetAttrib,
    targetOrientationAttrib,
    swivelAngleAttrib,
    ikFkBlendWeightAttrib,
    swivelContributionToOrientationAttrib,
    twoBoneIKSetupAttrib,
    twoBoneIKChainAttrib,
    rigAttrib,
    outputBufferAttrib,
    trajectoryTransform);
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataTwoBoneIKSetup functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataTwoBoneIKSetup::locate(AttribData* target)
{
  AttribDataTwoBoneIKSetup* result = (AttribDataTwoBoneIKSetup*)target;

  AttribData::locate(target);

  // swap everything that needs to be swapped...
  NMP::endianSwap(result->m_defaultTargetOrientation);
  NMP::endianSwap(result->m_assumeSimpleHierarchy);
  NMP::endianSwap(result->m_keepEndEffOrientation);
  NMP::endianSwap(result->m_userControlledOrientation);
  NMP::endianSwap(result->m_updateTargetByDeltas);
  NMP::endianSwap(result->m_worldSpaceTarget);
  NMP::endianSwap(result->m_useSpecifiedJointAsTarget);
  NMP::endianSwap(result->m_useSpecifiedJointOrientation);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataTwoBoneIKSetup::dislocate(AttribData* target)
{
  AttribDataTwoBoneIKSetup* result = (AttribDataTwoBoneIKSetup*)target;

  NMP::endianSwap(result->m_defaultTargetOrientation);
  NMP::endianSwap(result->m_assumeSimpleHierarchy);
  NMP::endianSwap(result->m_keepEndEffOrientation);
  NMP::endianSwap(result->m_userControlledOrientation);
  NMP::endianSwap(result->m_updateTargetByDeltas);
  NMP::endianSwap(result->m_worldSpaceTarget);
  NMP::endianSwap(result->m_useSpecifiedJointAsTarget);
  NMP::endianSwap(result->m_useSpecifiedJointOrientation);

  AttribData::dislocate(target);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataTwoBoneIKSetup::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataTwoBoneIKSetup), MR_ATTRIB_DATA_ALIGNMENT);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataTwoBoneIKSetup* AttribDataTwoBoneIKSetup ::init(
  NMP::Memory::Resource& resource,
  uint16_t               refCount)
{
  NMP::Memory::Format format = getMemoryRequirements();
  resource.align(format);

  AttribDataTwoBoneIKSetup* result = (AttribDataTwoBoneIKSetup*) resource.ptr;
  resource.increment(format);

  result->setType(ATTRIB_TYPE_TWO_BONE_IK_SETUP);
  result->setRefCount(refCount);

  result->m_userControlledOrientation = false;
  result->m_keepEndEffOrientation = true;
  result->m_assumeSimpleHierarchy = true;
  result->m_updateTargetByDeltas = false;
  result->m_worldSpaceTarget = false;
  result->m_useSpecifiedJointAsTarget = false;
  result->m_useSpecifiedJointOrientation = false;

  result->m_defaultTargetOrientation.identity();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataTwoBoneIKChain functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataTwoBoneIKChain::locate(AttribData* target)
{
  AttribDataTwoBoneIKChain* result = (AttribDataTwoBoneIKChain*)target;

  AttribData::locate(target);

  NMP::endianSwap(result->m_midJointRotAxis);
  NMP::endianSwap(result->m_midJointReferenceAxis);
  NMP::endianSwap(result->m_endJointIndex);
  NMP::endianSwap(result->m_midJointIndex);
  NMP::endianSwap(result->m_rootJointIndex);
  NMP::endianSwap(result->m_targetJointIndex);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataTwoBoneIKChain::dislocate(AttribData* target)
{
  AttribDataTwoBoneIKChain* result = (AttribDataTwoBoneIKChain*)target;

  NMP::endianSwap(result->m_midJointRotAxis);
  NMP::endianSwap(result->m_midJointReferenceAxis);
  NMP::endianSwap(result->m_endJointIndex);
  NMP::endianSwap(result->m_midJointIndex);
  NMP::endianSwap(result->m_rootJointIndex);
  NMP::endianSwap(result->m_targetJointIndex);

  AttribData::dislocate(target);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataTwoBoneIKChain::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataTwoBoneIKChain), MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataTwoBoneIKChain* AttribDataTwoBoneIKChain::init(
  NMP::Memory::Resource& resource,
  uint16_t               refCount)
{
  NMP::Memory::Format format = getMemoryRequirements();
  resource.align(format);

  AttribDataTwoBoneIKChain* result = (AttribDataTwoBoneIKChain*) resource.ptr;
  resource.increment(format);

  result->setType(ATTRIB_TYPE_TWO_BONE_IK_CHAIN);
  result->setRefCount(refCount);

  result->m_midJointReferenceAxis.setToZero();
  result->m_midJointRotAxis.setToZero();

  result->m_endJointIndex = 0;
  result->m_midJointIndex = 0;
  result->m_rootJointIndex = 0;
  result->m_targetJointIndex = 0;
  result->m_globalReferenceAxis = false;

  return result;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
