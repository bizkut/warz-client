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

#include "NMGeomUtils/NMJointLimits.h"
#include "NMIK/NMPointIK.h"
//----------------------------------------------------------------------------------------------------------------------

#define ISFINITE(x) ((x) * 0 == 0)
#define ISFINITE_VEC(q) (ISFINITE((q).x) && ISFINITE((q).y) && ISFINITE((q).z) && ISFINITE((q).w))

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
void TaskHeadLookSetup(Dispatcher::TaskParameters* parameters)
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
namespace
{
  
//----------------------------------------------------------------------------------------------------------------------
void subTaskHeadLookTransforms(
  Dispatcher::TaskParameters* parameters,
  AttribDataTransformBuffer* localBufferAttrib,
  const NMP::Vector3& deltaPos,
  const NMP::Quat& deltaQuat,
  AttribDataFloat* blendWeightAttrib,
  AttribDataVector3* targetPosAttrib,
  AttribDataHeadLookChain* headLookChainAttrib,
  AttribDataHeadLookSetup* headLookSetupAttrib,
  AttribDataRig* rigAttrib,
  AttribDataTransformBuffer* outputBufferAttrib,
  const NMP::Vector3& trajectoryPos,
  const NMP::Quat& trajectoryQuat)
{
  //--------------------------------------------
  // Grab some parameters into local variables and aliases
  const AnimRigDef* rig = rigAttrib->m_rig;
  NMP::DataBuffer* outputBuffer = outputBufferAttrib->m_transformBuffer;
  const NMP::DataBuffer* localBuffer = localBufferAttrib->m_transformBuffer;

  const uint32_t& endJointIndex = headLookChainAttrib->m_endJointIndex;
  const uint32_t& rootJointIndex = headLookChainAttrib->m_rootJointIndex;
  const uint32_t& numJoints = headLookChainAttrib->m_numJoints;
  const bool& worldSpaceTarget = headLookSetupAttrib->m_worldSpaceTarget;
  const bool& updateTargetByDeltas = headLookSetupAttrib->m_updateTargetByDeltas;
  const NMP::Vector3& endEffectorOffset = headLookChainAttrib->m_endEffectorOffset;
  NMP::Vector3 targetPos = targetPosAttrib->m_value;

  // Connected control param providing blend weight.
  float blendWeight = NMP::clampValue(blendWeightAttrib->m_value, 0.0f, 1.0f);

  //--------------------------------------------

  // Copy input buffer to output buffer.  I will then be modifying the output buffer at the
  // joints of the chain.
  localBuffer->copyTo(outputBuffer);

  // Add any missing channels.
  int32_t j = endJointIndex;
  do
  {
    // If failed there is no connected chain between root and end effector, or hierarchy is not valid
    NMP_ASSERT(j >= 0);
    if (!outputBuffer->hasChannel(j))
    {
      outputBuffer->setPosQuatChannelPos(j, *rig->getBindPoseBonePos(j));
      outputBuffer->setPosQuatChannelQuat(j, *rig->getBindPoseBoneQuat(j));
      outputBuffer->setChannelUsed(j);
    }
  }
  while ((j = rig->getParentBoneIndex(j)) >= (signed)rootJointIndex);
  // Reset the 'full' flag
  outputBuffer->calculateFullFlag();

  //--------------------------------------------
  // Memory allocation for working buffers

  // Get temp allocator.
  NMP::TempMemoryAllocator* tempAllocator = parameters->m_dispatcher->getTempMemoryAllocator();
  NMP_ASSERT(tempAllocator);
  NMP::TempMemoryAllocator* childAllocator = tempAllocator->createChildAllocator();

  // Allocate an array to hold the joint indices associated with each joint
  NMP::Memory::Format format(sizeof(uint32_t) * numJoints, NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::Resource res = NMPAllocatorAllocateFromFormat(childAllocator, format);
  uint32_t* jointIndexMap = (uint32_t*)res.ptr;

  // We have one more NMIK joint than real joints, because NMIK treats the end effector as a joint
  // (albeit fixed).
  uint32_t numIKJoints = numJoints + 1;

  // Allocate memory for the NMIK geometry
  format.set(sizeof(NMRU::GeomUtils::PosQuat) * numIKJoints, NMP_VECTOR_ALIGNMENT);
  res = NMPAllocatorAllocateFromFormat(childAllocator, format);
  NMRU::GeomUtils::PosQuat* ikGeometry = (NMRU::GeomUtils::PosQuat*)res.ptr;

  // Allocate memory for the NMIK index map (to conform with PointIK input data requirements)
  format.set(sizeof(uint32_t) * numIKJoints, NMP_NATURAL_TYPE_ALIGNMENT);
  res = NMPAllocatorAllocateFromFormat(childAllocator, format);
  uint32_t* ikJointMap = (uint32_t*)res.ptr;

  //--------------------------------------------
  // Set up geometry

  // Create index maps
  j = endJointIndex;
  ikJointMap[numJoints] = numJoints;
  for (int32_t i = (numJoints - 1); i >= 0; --i, j = rig->getParentBoneIndex(j))
  {
    NMP_ASSERT(outputBuffer->hasChannel(j));
    jointIndexMap[i] = j;
    ikJointMap[i] = i;
  }

  // For world-space targets, retrieve the current trajectory and move the target into character space.
  // We have to use the previous frame's trajectory because the current frame's value may contain some but
  // not necessarily all of the character motion for this frame, depending whether we are pre- or post-physics.
  if (worldSpaceTarget)
  {
    // Put the target into the space of the character at the last frame
    targetPos = trajectoryQuat.inverseRotateVector(targetPos - trajectoryPos);

    // Update for the trajectory deltas calculated by the network so far
    targetPos = deltaQuat.inverseRotateVector(targetPos - deltaPos);
  }
  // For character-space targets: if required, update the target transform by one trajectory delta.
  // This is useful when you are targeting world-space coordinates but, when converting
  // this to character space, were only able to retrieve the character root transform
  // at the last update, when obviously you really wanted the target transform relative
  // to the character after /this/ update.
  else if (updateTargetByDeltas)
  {
    targetPos = deltaQuat.inverseRotateVector(targetPos - deltaPos);
  }

  // FK the root joint to the world, to get the coordinate frame of the parent of the root
  NMRU::GeomUtils::PosQuat rootParentT = NMRU::GeomUtils::identityPosQuat();
  j = rootJointIndex;
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

  //--------------------------------------------
  // PointIK solver set up

  // Fill the IK geometry
  for (uint32_t i = 0; i < numJoints; ++i)
  {
    uint32_t jointIndex = jointIndexMap[i];
    ikGeometry[i].t = *outputBuffer->getPosQuatChannelPos(jointIndex);
    ikGeometry[i].q = *outputBuffer->getPosQuatChannelQuat(jointIndex);
  }
  // Set the end effector geometry
  ikGeometry[numIKJoints - 1].t = endEffectorOffset;
  ikGeometry[numIKJoints - 1].q.identity();

  //--------------------------------------------
  // SOLVE
  NMRU::GeomUtils::PosQuat target;
  target.t = targetPos;
  target.q.identity();
  NMRU::PointIK::solve(
    headLookChainAttrib->m_ikParams, ikGeometry, ikJointMap,
    numIKJoints, target, &rootParentT);

  //--------------------------------------------
  // Set results in output buffer as weighted blends
  for (uint32_t i = 0; i < numJoints; ++i)
  {
    uint32_t jointIndex = jointIndexMap[i];
    NMP_ASSERT(outputBuffer->hasChannel(jointIndex));
    NMP::Quat* outputQuat = outputBuffer->getPosQuatChannelQuat(jointIndex);
    NMRU::GeomUtils::shortestArcFastSlerp(*outputQuat, ikGeometry[i].q, blendWeight);
  }

  //--------------------------------------------
  // Finally release the temporary joint buffer.
  childAllocator->memFree(ikJointMap);
  childAllocator->memFree(ikGeometry);
  childAllocator->memFree(jointIndexMap);
  tempAllocator->destroyChildAllocator(childAllocator);
}
 
//----------------------------------------------------------------------------------------------------------------------
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
void TaskHeadLookTransforms(Dispatcher::TaskParameters* parameters)
{
  //--------------------------------------------
  // Task parameters

  // Input transforms
  AttribDataTransformBuffer* localBufferAttrib =
    parameters->getInputAttrib<AttribDataTransformBuffer>(0, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);

  // Delta transform for this update - for 'update target by deltas' functionality
  AttribDataTrajectoryDeltaTransform* deltaTransformAttrib =
    parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);

  // FK/IK blend weight control parameter
  AttribDataFloat* blendWeightAttrib = parameters->getInputAttrib<AttribDataFloat>(2, ATTRIB_SEMANTIC_CP_FLOAT);

  // Target position control parameter
  AttribDataVector3* targetPosAttrib = parameters->getInputAttrib<AttribDataVector3>(3, ATTRIB_SEMANTIC_CP_VECTOR3);

  // Node anim-set-dependent attributes
  AttribDataHeadLookChain* headLookChainAttrib =
    parameters->getInputAttrib<AttribDataHeadLookChain>(4, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);

  // Node anim-set-independent attributes
  AttribDataHeadLookSetup* headLookSetupAttrib =
    parameters->getInputAttrib<AttribDataHeadLookSetup>(5, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  // Rig definition
  AttribDataRig* rigAttrib = parameters->getInputAttrib<AttribDataRig>(6, ATTRIB_SEMANTIC_RIG);
  const uint32_t numRigJoints = rigAttrib->m_rig->getNumBones();

  // Output transforms
  AttribDataTransformBuffer* outputBufferAttrib =
    parameters->createOutputAttribTransformBuffer(7, numRigJoints, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);

  // The world space trajectory transform from the previous frame
  const AttribDataTransform* trajectoryTransform =
    parameters->getInputAttrib<AttribDataTransform>(8, ATTRIB_SEMANTIC_TRAJECTORY_TRANSFORM);

  subTaskHeadLookTransforms(
    parameters,
    localBufferAttrib,
    deltaTransformAttrib->m_deltaPos,
    deltaTransformAttrib->m_deltaAtt,
    blendWeightAttrib,
    targetPosAttrib,
    headLookChainAttrib,
    headLookSetupAttrib,
    rigAttrib,
    outputBufferAttrib,
    trajectoryTransform->m_pos,
    trajectoryTransform->m_att);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskHeadLookTrajectoryDeltaAndTransforms(Dispatcher::TaskParameters* parameters)
{
  //--------------------------------------------
  // Task parameters

  // Input transforms
  AttribDataTransformBuffer* localBufferAttrib =
    parameters->getInputAttrib<AttribDataTransformBuffer>(0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);

  // FK/IK blend weight control parameter
  AttribDataFloat* blendWeightAttrib = parameters->getInputAttrib<AttribDataFloat>(1, ATTRIB_SEMANTIC_CP_FLOAT);

  // Target position control parameter
  AttribDataVector3* targetPosAttrib = parameters->getInputAttrib<AttribDataVector3>(2, ATTRIB_SEMANTIC_CP_VECTOR3);

  // Node anim-set-dependent attributes
  AttribDataHeadLookChain* headLookChainAttrib =
    parameters->getInputAttrib<AttribDataHeadLookChain>(3, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);

  // Node anim-set-independent attributes
  AttribDataHeadLookSetup* headLookSetupAttrib =
    parameters->getInputAttrib<AttribDataHeadLookSetup>(4, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  // Rig definition
  AttribDataRig* rigAttrib = parameters->getInputAttrib<AttribDataRig>(5, ATTRIB_SEMANTIC_RIG);
  const uint32_t numRigJoints = rigAttrib->m_rig->getNumBones();

  // Output transforms
  AttribDataTransformBuffer* outputBufferAttrib =
    parameters->createOutputAttribTransformBuffer(6, numRigJoints, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);

  // The world space trajectory transform from the previous frame
  const AttribDataCharacterProperties* characterController =
    parameters->getInputAttrib<AttribDataCharacterProperties>(7, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES);

  const NMP::Vector3& previousFramePosition = characterController->m_prevWorldRootTransform.translation();
  NMP::Quat previousFrameOrientation = characterController->m_prevWorldRootTransform.toQuat();

  subTaskHeadLookTransforms(
    parameters,
    localBufferAttrib,
    *localBufferAttrib->m_transformBuffer->getPosQuatChannelPos(0),
    *localBufferAttrib->m_transformBuffer->getPosQuatChannelQuat(0),
    blendWeightAttrib,
    targetPosAttrib,
    headLookChainAttrib,
    headLookSetupAttrib,
    rigAttrib,
    outputBufferAttrib,
    previousFramePosition,
    previousFrameOrientation);
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataHeadLookSetup functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataHeadLookSetup::locate(AttribData* target)
{
  AttribDataHeadLookSetup* result = (AttribDataHeadLookSetup*)target;

  AttribData::locate(target);

  NMP::endianSwap(result->m_updateTargetByDeltas);
  NMP::endianSwap(result->m_worldSpaceTarget);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataHeadLookSetup::dislocate(AttribData* target)
{
  AttribDataHeadLookSetup* result = (AttribDataHeadLookSetup*)target;

  NMP::endianSwap(result->m_updateTargetByDeltas);
  NMP::endianSwap(result->m_worldSpaceTarget);

  AttribData::dislocate(target);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataHeadLookSetup::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataHeadLookSetup), MR_ATTRIB_DATA_ALIGNMENT);

  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHeadLookSetup* AttribDataHeadLookSetup::init(
  NMP::Memory::Resource& resource,
  uint16_t               refCount)
{
  NMP::Memory::Format format = getMemoryRequirements();
  resource.align(format);

  AttribDataHeadLookSetup* result = (AttribDataHeadLookSetup*) resource.ptr;
  resource.increment(format);

  result->setType(ATTRIB_TYPE_HEAD_LOOK_SETUP);
  result->setRefCount(refCount);

  result->m_updateTargetByDeltas = false;
  result->m_worldSpaceTarget = false;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataHeadLookChain functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataHeadLookChain::locate(AttribData* target)
{
  AttribDataHeadLookChain* result = (AttribDataHeadLookChain*)target;

  AttribData::locate(target);

  NMP::endianSwap(result->m_endEffectorOffset);
  NMP::endianSwap(result->m_endJointIndex);
  NMP::endianSwap(result->m_rootJointIndex);
  NMP::endianSwap(result->m_numJoints);
  NMP::endianSwap(result->m_ikParams);

  // Must locate PointIK params last because it needs the m_numJoints attribute
  REFIX_PTR_RELATIVE(NMRU::PointIK::Params, result->m_ikParams, result);
  NMRU::PointIK::Params::locate(result->m_ikParams, result->m_numJoints + 1);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataHeadLookChain::dislocate(AttribData* target)
{
  AttribDataHeadLookChain* result = (AttribDataHeadLookChain*)target;

  // Must dislocate PointIK params first because it needs the m_numJoints attribute
  NMRU::PointIK::Params::dislocate(result->m_ikParams, result->m_numJoints + 1);
  UNFIX_PTR_RELATIVE(NMRU::PointIK::Params, result->m_ikParams, result);

  NMP::endianSwap(result->m_endEffectorOffset);
  NMP::endianSwap(result->m_endJointIndex);
  NMP::endianSwap(result->m_rootJointIndex);
  NMP::endianSwap(result->m_numJoints);
  NMP::endianSwap(result->m_ikParams);

  AttribData::dislocate(target);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AttribDataHeadLookChain::relocate(AttribData* target, void* location)
{
  AttribDataHeadLookChain* result = (AttribDataHeadLookChain*)target;
  NMP_ASSERT(NMP_IS_ALIGNED(target, MR_ATTRIB_DATA_ALIGNMENT));
  NMP_ASSERT(NMP_IS_ALIGNED(location, MR_ATTRIB_DATA_ALIGNMENT));

  NMP::Memory::Format memReqs = getMemoryRequirements(result->m_numJoints);
  NMP::Memory::Resource resultRes = {result, memReqs};
  NMP::Memory::Resource locationRes = {location, memReqs};

  // AttribDataHeadLookChain class.
  resultRes.increment(sizeof(AttribDataHeadLookChain));
  locationRes.increment(sizeof(AttribDataHeadLookChain));

  // IK params
  uint32_t numIKJoints = result->m_numJoints + 1;
  memReqs = NMRU::PointIK::Params::getMemoryRequirements(numIKJoints);
  NMRU::PointIK::Params* ikParams = (NMRU::PointIK::Params*)resultRes.alignAndIncrement(memReqs);
  result->m_ikParams = (NMRU::PointIK::Params*)locationRes.alignAndIncrement(memReqs);
  NMRU::PointIK::Params::relocate(ikParams, result->m_ikParams, numIKJoints);

  // Check we used up all the memory we think we should have
  NMP_ASSERT(resultRes.format.size == 0);
  NMP_ASSERT(locationRes.format.size == 0);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataHeadLookChain::getMemoryRequirements(uint32_t numJoints)
{
  // Shell
  NMP::Memory::Format result(sizeof(AttribDataHeadLookChain), MR_ATTRIB_DATA_ALIGNMENT);

  // IK params
  result += NMRU::PointIK::Params::getMemoryRequirements(numJoints + 1);

  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHeadLookChain* AttribDataHeadLookChain::init(
  NMP::Memory::Resource& resource,
  uint32_t               numJoints,
  uint16_t               refCount)
{
  NMP::Memory::Format format = getMemoryRequirements(numJoints);
  resource.align(format);
  AttribDataHeadLookChain* result = (AttribDataHeadLookChain*) resource.ptr;
  NMP::Memory::Resource internalMemTrack = resource;
  internalMemTrack.format = format;
  resource.increment(format);

  // Shell
  format.set(sizeof(AttribDataHeadLookChain), MR_ATTRIB_DATA_ALIGNMENT);
  internalMemTrack.align(format);
  //
  result->setType(ATTRIB_TYPE_HEAD_LOOK_CHAIN);
  result->setRefCount(refCount);
  //
  result->m_endJointIndex = (uint32_t) - 1;
  result->m_rootJointIndex = (uint32_t) - 1;
  result->m_endEffectorOffset.setToZero();
  result->m_numJoints = numJoints;
  //
  internalMemTrack.increment(format);

  // IK params
  // Note end effector is also a 'joint' to the IK solver, hence numJoints + 1
  uint32_t numIKJoints = numJoints + 1;
  format = NMRU::PointIK::Params::getMemoryRequirements(numIKJoints);
  internalMemTrack.align(format);
  NMP::Memory::Resource res = internalMemTrack;
  res.format = format;
  result->m_ikParams = (NMRU::PointIK::Params*)NMRU::PointIK::Params::init(&res, numIKJoints);
  // Set defaults
  result->m_ikParams->init(numIKJoints);
  //
  internalMemTrack.increment(format);

  // Check we used up all the memory we think we should have
  NMP_ASSERT(internalMemTrack.format.size == 0);

  return result;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
