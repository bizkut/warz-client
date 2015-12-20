// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
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
#include "morpheme/mrAttribData.h"
#include "morpheme/mrNetwork.h"
#include "sharedDefines/mCoreDebugInterface.h"
#include "morpheme/mrCharacterControllerAttribData.h"
//----------------------------------------------------------------------------------------------------------------------
#include "NMGeomUtils/NMGeomUtils.h"
#include "NMIK/NMTwoBoneIK.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
namespace
{

//----------------------------------------------------------------------------------------------------------------------
// Note internally we refer to the Hips as the Pelvis to avoid confusion with the left or right hip joints at the root
// of each leg.
void doHipsIK(
  const NMP::DataBuffer* inputBuffer,
  NMP::DataBuffer* outputBuffer,
  const AnimRigDef* rig,
  const AttribDataHipsIKDef* hipsIKDefAttrib,
  const AttribDataHipsIKAnimSetDef* hipsIKAnimSetDefAttrib,
  const NMP::Vector3& positionDelta,
  const NMP::Quat& rotationDelta,
  const float& footTurnWeight,
  const float& ikFkBlendWeight)
{
  // Copy input buffer to output buffer.  I will then be modifying the output buffer at the
  // joints of the chain.
  inputBuffer->copyTo(outputBuffer);

  // Get the character-space transform of the pelvis joint, which we need to know to get the world up direction
  // in that frame (we're assuming that the world up in the trajectory frame is unchanged as the character moves).
  // Also to get the local delta for the hips if the input transform is specified in character space
  NMRU::GeomUtils::PosQuat pelvisT = NMRU::GeomUtils::identityPosQuat();
  { // Open scope to ensure j is a local variable - otherwise causes confusion with jointIndexI used later.
    int32_t j = hipsIKAnimSetDefAttrib->m_hipsIndex;
    do
    {
      const NMP::Vector3* p = outputBuffer->getPosQuatChannelPos(j);
      const NMP::Quat* q = outputBuffer->getPosQuatChannelQuat(j);
      if (!outputBuffer->hasChannel(j))
      {
        p = rig->getBindPoseBonePos(j);
        q = rig->getBindPoseBoneQuat(j);
      }
      NMRU::GeomUtils::PosQuat jointT = {*p, *q};
      NMRU::GeomUtils::multiplyPosQuat(pelvisT, jointT);
    }
    while ((j = rig->getParentBoneIndex(j)) > 0);
  }

  // Get the up direction in the pelvis frame
  NMP::Vector3 upDirectionInPelvisFrame = pelvisT.q.inverseRotateVector(hipsIKDefAttrib->m_upAxis);

  // Create a transform for the hips delta and store.
  // Note we are calling it the pelvis delta and referring to the hips throughout as the pelvis so as to
  // avoid confusion with the left/right hips joints at the root of each leg.
  NMRU::GeomUtils::PosQuat pelvisDeltaT;
  pelvisDeltaT.t = positionDelta;
  pelvisDeltaT.q = rotationDelta;
  if (!hipsIKDefAttrib->m_localReferenceFrame)
  {
    NMP::Quat worldRotation = rotationDelta * pelvisT.q;
    pelvisDeltaT.q = ~pelvisT.q * worldRotation;
    pelvisDeltaT.t = pelvisT.q.inverseRotateVector(positionDelta);
  }
  else
  {
    pelvisDeltaT.t = positionDelta;
    pelvisDeltaT.q = rotationDelta;
  }
  //
  // Adjust the delta for the blend weight, by blending back towards the identity
  pelvisDeltaT.t *= ikFkBlendWeight;
  float signOfRotationW = NMP::floatSelect(pelvisDeltaT.q.w, 1.0f, -1.0f);
  //   This is a slightly faster way of blending towards the identity, avoiding conditionals
  pelvisDeltaT.q.fastSlerp(
    NMP::Quat(0.0f, 0.0f, 0.0f, signOfRotationW), 1.0f - ikFkBlendWeight, NMP::nmfabs(pelvisDeltaT.q.w));

  //--------------------------------------------
  // First, adjust the pelvis transform, activating that channel if necessary
  //  Current transform
  const int32_t& pelvisIndex = hipsIKAnimSetDefAttrib->m_hipsIndex;
  NMP::Vector3* pelvisPos = outputBuffer->getPosQuatChannelPos(pelvisIndex);
  NMP::Quat* pelvisQuat = outputBuffer->getPosQuatChannelQuat(pelvisIndex);
  if (!outputBuffer->hasChannel(pelvisIndex))
  {
    outputBuffer->setChannelUsed(pelvisIndex);
    *pelvisPos = *rig->getBindPoseBonePos(pelvisIndex);
    *pelvisQuat = *rig->getBindPoseBoneQuat(pelvisIndex);
  }
  //  Adding the delta
  *pelvisPos += pelvisQuat->rotateVector(pelvisDeltaT.t);
  pelvisQuat->multiply(pelvisDeltaT.q);
  //  Also need to adjust the up direction.
  upDirectionInPelvisFrame = pelvisDeltaT.q.inverseRotateVector(upDirectionInPelvisFrame);

  //--------------------------------------------
  // Set up the TwoBoneIK parameters for the left leg in advance of the IK loop
  NMRU::TwoBoneIK::Params ikParams;
  ikParams.hingeAxis = hipsIKAnimSetDefAttrib->m_leftKneeRotationAxis;
  ikParams.endJointOrientationWeight = 1.0f;
  ikParams.swivelOrientationWeight = hipsIKDefAttrib->m_swivelOrientationWeight;
  ikParams.preventBendingBackwards = true;

  //--------------------------------------------
  // Leg IK, as a loop to save on code duplication
  const int32_t* jointIndex = hipsIKAnimSetDefAttrib->m_leftLegIndex;
  for (uint32_t whichLeg = 0; whichLeg < 2; ++whichLeg)
  {

    // Get the transform of the parent of the left/right hip in the frame of the pelvis joint, covering any
    // joints in between (if there are none, this will be the identity)
    NMRU::GeomUtils::PosQuat hipParentT = NMRU::GeomUtils::identityPosQuat();

    int32_t jointIndexI = jointIndex[0];
    while ((jointIndexI = rig->getParentBoneIndex(jointIndexI)) > hipsIKAnimSetDefAttrib->m_hipsIndex)
    {
      const NMP::Vector3* p = outputBuffer->getPosQuatChannelPos(jointIndexI);
      const NMP::Quat* q = outputBuffer->getPosQuatChannelQuat(jointIndexI);
      if (!outputBuffer->hasChannel(jointIndexI))
      {
        p = rig->getBindPoseBonePos(jointIndexI);
        q = rig->getBindPoseBoneQuat(jointIndexI);
      }
      NMRU::GeomUtils::PosQuat jointT = {*p, *q};
      NMRU::GeomUtils::multiplyPosQuat(hipParentT, jointT);
    }

    // Set up the IK geometry, adding missing channels from the bind pose and activating them in the
    // transform buffer, since we are modifying them.
    NMRU::GeomUtils::PosQuat ikGeometry[4];
    uint32_t numIKJoints = hipsIKAnimSetDefAttrib->m_useBallJoints ? 4 : 3;
    for (uint32_t i = 0; i < numIKJoints; ++i)
    {
      jointIndexI = jointIndex[i];
      if (!outputBuffer->hasChannel(jointIndexI))
      {
        ikGeometry[i].t = *rig->getBindPoseBonePos(jointIndexI);
        ikGeometry[i].q = *rig->getBindPoseBoneQuat(jointIndexI);
        outputBuffer->setChannelUsed(jointIndexI);
      }
      else
      {
        ikGeometry[i].t = *outputBuffer->getPosQuatChannelPos(jointIndexI);
        ikGeometry[i].q = *outputBuffer->getPosQuatChannelQuat(jointIndexI);
      }
    }
    // The fourth joint is the end effector, which is either the ball joint or the ankle again.  If it's
    // the ball we already got its geometry above, if it's the ankle it's just an identity transform.
    if (!hipsIKAnimSetDefAttrib->m_useBallJoints)
    {
      ikGeometry[3] = NMRU::GeomUtils::identityPosQuat();
    }

    // Get the end effector transform relative to the hip parent
    NMRU::GeomUtils::PosQuat endEffectorT = ikGeometry[3];
    NMRU::GeomUtils::multiplyPosQuat(endEffectorT, ikGeometry[2]);
    NMRU::GeomUtils::multiplyPosQuat(endEffectorT, ikGeometry[1]);
    NMRU::GeomUtils::multiplyPosQuat(endEffectorT, ikGeometry[0]);

    // Get the desired end effector target in the left/right hip parent frame by applying the inverse transform
    // of the pelvis.
    NMRU::GeomUtils::PosQuat target = endEffectorT;
    NMRU::GeomUtils::multiplyPosQuat(target, hipParentT);
    NMRU::GeomUtils::multiplyInversePosQuat(target, pelvisDeltaT);
    NMRU::GeomUtils::multiplyInversePosQuat(target, hipParentT);

    // Pivot the target orientation around the up axis to match the azimuthal motion of the hips
    NMP::Quat azimuthalRotationDeltaPelvisFrame =
      NMRU::GeomUtils::closestRotationAroundAxis(pelvisDeltaT.q, upDirectionInPelvisFrame);
    NMP::Quat rotatedFootOrientation = azimuthalRotationDeltaPelvisFrame * hipParentT.q * target.q;
    NMRU::GeomUtils::shortestArcFastSlerp(
      target.q, ~hipParentT.q * rotatedFootOrientation, footTurnWeight);

    //---------------------------------
    // SOLVE
    if (hipsIKDefAttrib->m_keepWorldFootOrientation)
    {
      NMRU::TwoBoneIK::solveWithEndJoint(&ikParams, ikGeometry, target);
    }
    else
    {
      NMRU::TwoBoneIK::solveWithFixedEndJoint(&ikParams, ikGeometry, target);
    }

    // *************************************************************************************

    // Set results in output buffer - note there is no need to blend because we already blended the
    // original target TM.
    for (uint32_t i = 0; i < 3; ++i)
    {
      uint32_t outputJointIndexI = jointIndex[i];
      NMP_ASSERT(outputBuffer->hasChannel(outputJointIndexI));
      outputBuffer->setPosQuatChannelQuat(outputJointIndexI, ikGeometry[i].q);
    }

    // Now set things up for the right leg
    jointIndex = hipsIKAnimSetDefAttrib->m_rightLegIndex;
    ikParams.hingeAxis = hipsIKAnimSetDefAttrib->m_rightKneeRotationAxis;
  }

  // *************************************************************************************

  // This node may have added channels, so we need to recalculate the Full flag
  outputBuffer->calculateFullFlag();
}

//----------------------------------------------------------------------------------------------------------------------
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
void TaskHipsIKTransforms(Dispatcher::TaskParameters* parameters)
{
  //--------------------------------------------
  // Gather task parameters

  // Input transform buffer
  const AttribDataTransformBuffer* inputBufferAttrib =
    parameters->getInputAttrib<AttribDataTransformBuffer>(0, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  const NMP::DataBuffer* inputBuffer = inputBufferAttrib->m_transformBuffer;
  uint32_t numJoints = inputBuffer->getLength();

  // Output buffer
  AttribDataTransformBuffer* outputBufferAttrib =
    parameters->createOutputAttribTransformBuffer(1, numJoints, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  NMP::DataBuffer* outputBuffer = outputBufferAttrib->m_transformBuffer;

  // The rig, with hierarchy and bind pose
  const AttribDataRig* rigAttrib = parameters->getInputAttrib<AttribDataRig>(2, ATTRIB_SEMANTIC_RIG);
  const AnimRigDef* rig = rigAttrib->m_rig;
  NMP_ASSERT(numJoints == rig->getNumBones());

  // The def data for this node
  const AttribDataHipsIKDef* hipsIKDefAttrib =
    parameters->getInputAttrib<AttribDataHipsIKDef>(3, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  // The def data for this animation set
  const AttribDataHipsIKAnimSetDef* hipsIKAnimSetDefAttrib =
    parameters->getInputAttrib<AttribDataHipsIKAnimSetDef>(4, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);

  // Position Delta control parameter
  const AttribDataVector3* positionDeltaAttrib =
    parameters->getInputAttrib<AttribDataVector3>(5, ATTRIB_SEMANTIC_CP_VECTOR3);
  NMP::Vector3 positionDelta = positionDeltaAttrib->m_value;

  // Rotation Delta control parameter - as quat or euler angles depending on which is connected
  NMP::Quat rotationDelta(NMP::Quat::kIdentity);
  const AttribDataVector4* rotationDeltaAttrib =
    parameters->getOptionalInputAttrib<AttribDataVector4>(6, ATTRIB_SEMANTIC_CP_VECTOR4);
  if (rotationDeltaAttrib)
  {
    rotationDelta = rotationDeltaAttrib->m_value;
    rotationDelta.fastNormalise();
  }
  // If the quat input is not connected then take the euler input.  If neither, leave at the identity.
  else
  {
    const AttribDataVector3* rotationDeltaEulerAttrib =
      parameters->getOptionalInputAttrib<AttribDataVector3>(7, ATTRIB_SEMANTIC_CP_VECTOR3);
    if (rotationDeltaEulerAttrib)
    {
      rotationDelta.fromEulerXYZ(NMP::degreesToRadians(rotationDeltaEulerAttrib->m_value));
    }
  }

  // Weight for the amount of foot pivot around the vertical
  const AttribDataFloat* footTurnWeightAttrib =
    parameters->getInputAttrib<AttribDataFloat>(8, ATTRIB_SEMANTIC_CP_FLOAT);
  float footTurnWeight = NMP::clampValue(footTurnWeightAttrib->m_value, 0.0f, 1.0f);

  // Weight for blending between the input and the IK solution
  const AttribDataFloat* ikFkBlendWeightAttrib =
    parameters->getInputAttrib<AttribDataFloat>(9, ATTRIB_SEMANTIC_CP_FLOAT);
  float ikFkBlendWeight = NMP::clampValue(ikFkBlendWeightAttrib->m_value, 0.0f, 1.0f);

  //--------------------------------------------
  // Pass to transforms sub task
  doHipsIK(
    inputBuffer,
    outputBuffer,
    rig,
    hipsIKDefAttrib,
    hipsIKAnimSetDefAttrib,
    positionDelta,
    rotationDelta,
    footTurnWeight,
    ikFkBlendWeight);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskHipsIKTrajectoryDeltaAndTransforms(Dispatcher::TaskParameters* parameters)
{
  //--------------------------------------------
  // Gather task parameters

  // Input trajectory and transform buffer
  const AttribDataTransformBuffer* inputBufferAttrib =
    parameters->getInputAttrib<AttribDataTransformBuffer>(0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
  const NMP::DataBuffer* inputBuffer = inputBufferAttrib->m_transformBuffer;
  uint32_t numJoints = inputBuffer->getLength();

  // Output buffer
  AttribDataTransformBuffer* outputBufferAttrib =
    parameters->createOutputAttribTransformBuffer(1, numJoints, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
  NMP::DataBuffer* outputBuffer = outputBufferAttrib->m_transformBuffer;

  // The rig, with hierarchy and bind pose
  const AttribDataRig* rigAttrib = parameters->getInputAttrib<AttribDataRig>(2, ATTRIB_SEMANTIC_RIG);
  const AnimRigDef* rig = rigAttrib->m_rig;
  NMP_ASSERT(numJoints == rig->getNumBones());

  // The def data for this node
  const AttribDataHipsIKDef* hipsIKDefAttrib =
    parameters->getInputAttrib<AttribDataHipsIKDef>(3, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  // The def data for this animation set
  const AttribDataHipsIKAnimSetDef* hipsIKAnimSetDefAttrib =
    parameters->getInputAttrib<AttribDataHipsIKAnimSetDef>(4, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);

  // Position Delta control parameter
  const AttribDataVector3* positionDeltaAttrib =
    parameters->getInputAttrib<AttribDataVector3>(5, ATTRIB_SEMANTIC_CP_VECTOR3);
  NMP::Vector3 positionDelta = positionDeltaAttrib->m_value;

  // Rotation Delta control parameter - as quat or euler angles depending on which is connected
  NMP::Quat rotationDelta(NMP::Quat::kIdentity);
  const AttribDataVector4* rotationDeltaAttrib =
    parameters->getOptionalInputAttrib<AttribDataVector4>(6, ATTRIB_SEMANTIC_CP_VECTOR4);
  if (rotationDeltaAttrib)
  {
    rotationDelta = rotationDeltaAttrib->m_value;
    rotationDelta.fastNormalise();
  }
  // If the quat input is not connected then take the euler input.  If neither, leave at the identity.
  else
  {
    const AttribDataVector3* rotationDeltaEulerAttrib =
      parameters->getOptionalInputAttrib<AttribDataVector3>(7, ATTRIB_SEMANTIC_CP_VECTOR3);
    if (rotationDeltaEulerAttrib)
    {
      rotationDelta.fromEulerXYZ(NMP::degreesToRadians(rotationDeltaEulerAttrib->m_value));
    }
  }

  // Weight for the amount of foot pivot around the vertical
  const AttribDataFloat* footTurnWeightAttrib = parameters->getInputAttrib<AttribDataFloat>(8, ATTRIB_SEMANTIC_CP_FLOAT);
  float footTurnWeight = NMP::clampValue(footTurnWeightAttrib->m_value, 0.0f, 1.0f);

  // Weight for blending between the input and the IK solution
  const AttribDataFloat* ikFkBlendWeightAttrib =
    parameters->getInputAttrib<AttribDataFloat>(9, ATTRIB_SEMANTIC_CP_FLOAT);
  float ikFkBlendWeight = NMP::clampValue(ikFkBlendWeightAttrib->m_value, 0.0f, 1.0f);

  //--------------------------------------------
  // Pass to transforms sub task
  doHipsIK(
    inputBuffer,
    outputBuffer,
    rig,
    hipsIKDefAttrib,
    hipsIKAnimSetDefAttrib,
    positionDelta,
    rotationDelta,
    footTurnWeight,
    ikFkBlendWeight);
}


//----------------------------------------------------------------------------------------------------------------------
// AttribDataHipsIKDefAnimSet functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataHipsIKDef::locate(AttribData* target)
{
  AttribDataHipsIKDef* result = (AttribDataHipsIKDef*)target;

  AttribData::locate(target);

  NMP::endianSwap(result->m_upAxis);
  NMP::endianSwap(result->m_swivelOrientationWeight);
  NMP::endianSwap(result->m_localReferenceFrame);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataHipsIKDef::dislocate(AttribData* target)
{
  AttribDataHipsIKDef* result = (AttribDataHipsIKDef*)target;

  NMP::endianSwap(result->m_localReferenceFrame);
  NMP::endianSwap(result->m_swivelOrientationWeight);
  NMP::endianSwap(result->m_upAxis);

  AttribData::dislocate(target);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataHipsIKDef::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataHipsIKDef), MR_ATTRIB_DATA_ALIGNMENT);

  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHipsIKDef* AttribDataHipsIKDef::init(
  NMP::Memory::Resource& resource,
  uint16_t               refCount)
{
  NMP::Memory::Format format = getMemoryRequirements();
  resource.align(format);

  AttribDataHipsIKDef* result = (AttribDataHipsIKDef*) resource.ptr;
  resource.increment(format);

  result->setType(ATTRIB_TYPE_HIPS_IK_DEF);
  result->setRefCount(refCount);

  result->m_upAxis.set(0.0f, 1.0f, 0.0f);
  result->m_swivelOrientationWeight = 0.5f;
  result->m_localReferenceFrame = false;
  result->m_keepWorldFootOrientation = true;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataHipsIKDefAnimSet functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataHipsIKAnimSetDef::locate(AttribData* target)
{
  AttribDataHipsIKAnimSetDef* result = (AttribDataHipsIKAnimSetDef*)target;

  AttribData::locate(target);

  NMP::endianSwap(result->m_leftKneeRotationAxis);
  NMP::endianSwap(result->m_rightKneeRotationAxis);
  NMP::endianSwap(result->m_hipsIndex);
  for (uint32_t j = 0; j < 4; ++j)
  {
    NMP::endianSwap(result->m_leftLegIndex[j]);
    NMP::endianSwap(result->m_rightLegIndex[j]);
  }
  NMP::endianSwap(result->m_useBallJoints);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataHipsIKAnimSetDef::dislocate(AttribData* target)
{
  AttribDataHipsIKAnimSetDef* result = (AttribDataHipsIKAnimSetDef*)target;

  NMP::endianSwap(result->m_useBallJoints);
  for (uint32_t j = 0; j < 4; ++j)
  {
    NMP::endianSwap(result->m_rightLegIndex[j]);
    NMP::endianSwap(result->m_leftLegIndex[j]);
  }
  NMP::endianSwap(result->m_hipsIndex);
  NMP::endianSwap(result->m_rightKneeRotationAxis);
  NMP::endianSwap(result->m_leftKneeRotationAxis);

  AttribData::dislocate(target);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataHipsIKAnimSetDef::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataHipsIKAnimSetDef), MR_ATTRIB_DATA_ALIGNMENT);

  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHipsIKAnimSetDef* AttribDataHipsIKAnimSetDef::init(
  NMP::Memory::Resource& resource,
  uint16_t               refCount)
{
  NMP::Memory::Format format = getMemoryRequirements();
  resource.align(format);

  AttribDataHipsIKAnimSetDef* result = (AttribDataHipsIKAnimSetDef*) resource.ptr;
  resource.increment(format);

  result->setType(ATTRIB_TYPE_HIPS_IK_ANIM_SET_DEF);
  result->setRefCount(refCount);

  result->m_leftKneeRotationAxis.setToZero();
  result->m_rightKneeRotationAxis.setToZero();
  for (uint32_t j = 0; j < 4; ++j)
  {
    result->m_leftLegIndex[j] = 0xFFFFFFFF;
    result->m_rightLegIndex[j] = 0xFFFFFFFF;
  }
  result->m_useBallJoints = true;

  return result;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
