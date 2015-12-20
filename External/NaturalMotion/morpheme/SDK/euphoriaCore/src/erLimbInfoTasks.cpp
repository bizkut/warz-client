// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
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
#include "euphoria/Nodes/erNodes.h"
#include "morpheme/mrTaskUtilities.h"

using namespace MR;

namespace ER
{
//----------------------------------------------------------------------------------------------------------------------
static void subTaskLimbInfoUpdate(MR::Dispatcher::TaskParameters* parameters,
                                  const MR::AttribDataSemantic transformSemantic)
{
  // Read attributes from supplied parameters.
  AttribDataTransformBuffer* inputTransforms =
    parameters->getInputAttrib<AttribDataTransformBuffer>(0, transformSemantic);

  AttribDataUInt* jointIndex
    = parameters->getInputAttrib<AttribDataUInt>(1, ATTRIB_SEMANTIC_CP_UINT);

  const uint32_t numRigJoints =
    inputTransforms->m_transformBuffer->getLength();

  AttribDataInt* angleType =
    parameters->getInputAttrib<AttribDataInt>(2, ATTRIB_SEMANTIC_CP_INT);

  AttribDataBool* measureUnits =
    parameters->getInputAttrib<AttribDataBool>(3, ATTRIB_SEMANTIC_CP_BOOL);

  AttribDataRig* rigAttribData =
    parameters->getInputAttrib<AttribDataRig>(4, ATTRIB_SEMANTIC_RIG);

  AttribDataTransformBuffer* outputTransforms =
    parameters->createOutputAttribTransformBuffer(5, numRigJoints, transformSemantic);

  AttribDataVector3* outputJointPosCP =
    parameters->createOutputCPAttrib<AttribDataVector3>(6, ATTRIB_SEMANTIC_CP_VECTOR3);

  AttribDataVector3* outputJointOriCP =
    parameters->createOutputCPAttrib<AttribDataVector3>(7, ATTRIB_SEMANTIC_CP_VECTOR3);

  const AnimRigDef*      animRig(rigAttribData->m_rig);
  const NMP::DataBuffer* inputBuffer(inputTransforms->m_transformBuffer);

  NMP::Matrix34 characterSpaceTM(
    *(inputBuffer->getChannelQuatFast(jointIndex->m_value)), *(inputBuffer->getChannelPosFast(jointIndex->m_value)));

  for (int32_t i(jointIndex->m_value); i != AnimRigDef::NO_PARENT_BONE; i = animRig->getParentBoneIndex(i))
  {
    NMP::Matrix34 partTM(*(animRig->getBindPoseBoneQuat(i)), *(animRig->getBindPoseBonePos(i)));
    characterSpaceTM = partTM * characterSpaceTM;
  }

  const uint32_t trajectoryIndex(animRig->getTrajectoryBoneIndex());
  const NMP::Matrix34 trajectoryTM(
    *(inputBuffer->getChannelQuatFast(trajectoryIndex)), *(inputBuffer->getChannelPosFast(trajectoryIndex)));
  characterSpaceTM = trajectoryTM * characterSpaceTM;

  // Construct character space transforms.
  calculateCharSpaceTransforms(
    (AnimRigDef*)animRig,
    inputTransforms->m_transformBuffer,
    outputTransforms->m_transformBuffer);

  // Extract the info from the joint.
  NMP::Quat jointQuat(NMP::Quat::kIdentity);

  if (jointIndex->m_value < outputTransforms->m_transformBuffer->getLength())
  {
    int32_t parentJointIndex = animRig->getParentBoneIndex(jointIndex->m_value);
    if (parentJointIndex != AnimRigDef::NO_PARENT_BONE)
    {
      // Find the angle between this bone and its parent.
      jointQuat = *(outputTransforms->m_transformBuffer->getChannelQuatFast(jointIndex->m_value));
    }

    // Find the position of this bone.
    outputJointPosCP->m_value = *(outputTransforms->m_transformBuffer->getChannelPosFast(jointIndex->m_value));
  }

  // Select the correct angle to output
  if (angleType->m_value == 1)
  {
    // Euler angles.
    outputJointOriCP->m_value = jointQuat.toEulerXYZ();
  }
  else
  {
    // Rotation vector.
    outputJointOriCP->m_value = jointQuat.toRotationVector();
  }

  // Convert to degrees if requested. [MeasureUnit=true] radians, [MeasureUnit=false] degrees
  if (measureUnits->m_value == false)
  {
    outputJointOriCP->m_value *= 180.0f * NM_ONE_OVER_PI;
  }

  // Transform orientation into limb space.
  const AttribDataVector4* const rigToLimbTransformAttrib =
    parameters->getInputAttrib<AttribDataVector4>(8, ATTRIB_SEMANTIC_CP_VECTOR4);

  NMP_ASSERT(rigToLimbTransformAttrib); // Null check.

  NMP::Quat rigToLimbTransform(NMP::Quat::kIdentity);

  rigToLimbTransform.x = rigToLimbTransformAttrib->m_value.x;
  rigToLimbTransform.y = rigToLimbTransformAttrib->m_value.y;
  rigToLimbTransform.z = rigToLimbTransformAttrib->m_value.z;
  rigToLimbTransform.w = rigToLimbTransformAttrib->m_value.w;

  outputJointOriCP->m_value = rigToLimbTransform.inverseRotateVector(outputJointOriCP->m_value);

  // Overwrite the output transforms buffer (where we have been working temporarily) with the input buffer.
  inputTransforms->m_transformBuffer->copyTo(outputTransforms->m_transformBuffer);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskLimbInfoUpdateTrajectoryDeltaAndTransforms(MR::Dispatcher::TaskParameters* parameters)
{
  subTaskLimbInfoUpdate(parameters, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskLimbInfoUpdateTransforms(MR::Dispatcher::TaskParameters* parameters)
{
  subTaskLimbInfoUpdate(parameters, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
}

} // namespace ER

//----------------------------------------------------------------------------------------------------------------------