// Copyright (c) 2013 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "Morpheme/Nodes/mrNodeModifyTrajectoryTransform.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/mrCoreTaskIDs.h"
#include "morpheme/mrCommonTasks.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Queuing functions.
//----------------------------------------------------------------------------------------------------------------------
Task* nodeModifyTrajectoryTransformQueueTrajectoryDelta(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  // Get the active state attribute data
  const CPConnection* cpConnection = node->getInputCPConnection(0);
  const NodeBin* nodeBin = net->getNodeBin(cpConnection->m_sourceNodeID);
  MR::OutputCPPin* outputCPPin = nodeBin->getOutputCPPin(cpConnection->m_sourcePinIndex);
  const AttribDataBool* activate = outputCPPin->getAttribData<AttribDataBool>();

  if (activate->m_value)
  {
    NodeID nodeID = node->getNodeID();
    Task* task = queue->createNewTaskOnQueue(
                            CoreTaskIDs::MR_TASKID_MODIFYTRAJECTORYTRANSFORM_TRAJECTORY_DELTA,
                            nodeID,
                            7,
                            dependentParameter);
    if (task)
    {
      FrameCount currFrameNo = net->getCurrentFrameNo();
      NMP_ASSERT(currFrameNo > 0);

      NodeID activeChildNodeID = net->getActiveChildNodeID(nodeID, 0);

      // Input delta trajectory
      net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, activeChildNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
      
      // Output delta trajectory
      net->TaskAddOutputParamZeroLifespan(task, 1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, INVALID_NODE_ID, currFrameNo);
      
      // Position control parameter
      net->TaskAddOptionalInputCP(task, 2, ATTRIB_SEMANTIC_CP_VECTOR3, node->getInputCPConnection(1));

      // Rotation control parameter
      net->TaskAddOptionalInputCP(task, 3, ATTRIB_SEMANTIC_CP_VECTOR4, node->getInputCPConnection(2));

      // Definition state.
      net->TaskAddDefInputParam(task, 4, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, nodeID);

      // The character controller, for determining the world-space location of the character.
      net->TaskAddNetInputParam(task, 5, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES, NETWORK_NODE_ID, INVALID_NODE_ID, currFrameNo);

      // The global network delta time value.
      net->TaskAddParamAndDependency(task, 6, ATTRIB_SEMANTIC_UPDATE_TIME_POS, ATTRIB_TYPE_UPDATE_PLAYBACK_POS, NETWORK_NODE_ID, INVALID_NODE_ID , TPARAM_FLAG_INPUT, currFrameNo);
    }

    return task;
  }
  else
  {
    // Just pass through if not active.
    return queuePassThroughChild0(node, queue, net, dependentParameter);
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeModifyTrajectoryTransformQueueTrajectoryDeltaAndTransforms(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  // Get the active state attribute data
  const CPConnection* cpConnection = node->getInputCPConnection(0);
  const NodeBin* nodeBin = net->getNodeBin(cpConnection->m_sourceNodeID);
  MR::OutputCPPin* outputCPPin = nodeBin->getOutputCPPin(cpConnection->m_sourcePinIndex);
  const AttribDataBool* activate = outputCPPin->getAttribData<AttribDataBool>();

  if (activate->m_value)
  {
    NodeID nodeID = node->getNodeID();
    Task* task = queue->createNewTaskOnQueue(
                            CoreTaskIDs::MR_TASKID_MODIFYTRAJECTORYTRANSFORM_TRAJECTORY_DELTA_AND_TRANSFORMS,
                            nodeID,
                            7,
                            dependentParameter);
    if (task)
    {
      FrameCount currFrameNo = net->getCurrentFrameNo();
      NMP_ASSERT(currFrameNo > 0);

      AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
      NodeID activeChildNodeID = net->getActiveChildNodeID(nodeID, 0);

      // Input transforms
      net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, activeChildNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
                  
      // Output transforms
      net->TaskAddOutputParamZeroLifespan(task, 1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
      
      // Position control parameter
      net->TaskAddOptionalInputCP(task, 2, ATTRIB_SEMANTIC_CP_VECTOR3, node->getInputCPConnection(1));

      // Rotation control parameter
      net->TaskAddOptionalInputCP(task, 3, ATTRIB_SEMANTIC_CP_VECTOR4, node->getInputCPConnection(2));
      
      // Definition state.
      net->TaskAddDefInputParam(task, 4, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, nodeID);

      // The character controller, for determining the world-space location of the character.
      net->TaskAddNetInputParam(task, 5, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES, NETWORK_NODE_ID, INVALID_NODE_ID, currFrameNo);

      // The global network delta time value.
      net->TaskAddParamAndDependency(task, 6, ATTRIB_SEMANTIC_UPDATE_TIME_POS, ATTRIB_TYPE_UPDATE_PLAYBACK_POS, NETWORK_NODE_ID, INVALID_NODE_ID , TPARAM_FLAG_INPUT, currFrameNo);
    }

    return task;
  }
  else
  {
    // Just pass through if not active.
    return queuePassThroughChild0(node, queue, net, dependentParameter);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Task functions.
//----------------------------------------------------------------------------------------------------------------------
static void modifyTrajectoryTransform(
  const NMP::Vector3&                  inputDeltaTrajectoryPos,
  const NMP::Quat&                     inputDeltaTrajectoryAtt,
  NMP::Vector3&                        outputDeltaTrajectoryPos,
  NMP::Quat&                           outputDeltaTrajectoryAtt,
  const AttribDataVector3*             modifyingTranslationAttrib,
  const AttribDataVector4*             modifyingRotationAttrib,
  const AttribDataModifyTrajectoryDef* nodeStateDefAttrib,
  const AttribDataCharacterProperties* characterControllerStateAttrib,
  const AttribDataUpdatePlaybackPos*   networkUpdateTimePos)
{
  NMP::Quat prevCharAttWorld = characterControllerStateAttrib->m_worldRootTransform.toQuat();
  NMP::Vector3 prevCharPosWorld = characterControllerStateAttrib->m_worldRootTransform.translation();


  //-----------------------------
  if (modifyingTranslationAttrib)
  {
    // Calculate an absolute translation if the input cp is expressed as a velocity.
    NMP::Vector3 modifyingTranslation = modifyingTranslationAttrib->m_value;
    if (nodeStateDefAttrib->m_linearModifyType == AttribDataModifyTrajectoryDef::MODIFY_TYPE_VELOCITY)
    {
      modifyingTranslation *= networkUpdateTimePos->m_value;
    }

    switch (nodeStateDefAttrib->m_linearOperation)
    {
      case AttribDataModifyTrajectoryDef::OPERATION_TYPE_ADD_TO_DELTA:
        // Add to the input delta translation.
        outputDeltaTrajectoryPos = inputDeltaTrajectoryPos + modifyingTranslation;
        break;
      case AttribDataModifyTrajectoryDef::OPERATION_TYPE_SET_DELTA:
        // Replace the input delta translation with the new value.
        outputDeltaTrajectoryPos = modifyingTranslation;
        break;
      case AttribDataModifyTrajectoryDef::OPERATION_TYPE_SET_TRANSFORM:
      default:
        // We are setting an absolute root transform, so we need to calculate the necessary delta from the last frames character transform.
        outputDeltaTrajectoryPos = modifyingTranslation - prevCharPosWorld;
        outputDeltaTrajectoryPos = prevCharAttWorld.inverseRotateVector(outputDeltaTrajectoryPos);
        break;
    }
  }


  //-----------------------------
  if (modifyingRotationAttrib)
  {
    // Get the input rotation expressed as a quaternion.
    NMP::Quat modifyingQuat = (NMP::Quat) modifyingRotationAttrib->m_value;
    if (nodeStateDefAttrib->m_rotationType == AttribDataModifyTrajectoryDef::ROTATION_TYPE_EULER)
    {
      NMP::Vector3 eularRotation(modifyingRotationAttrib->m_value.x, modifyingRotationAttrib->m_value.y, modifyingRotationAttrib->m_value.z);

      // Calculate an absolute rotation if the input cp is expressed as a velocity.
      if (nodeStateDefAttrib->m_angularModifyType == AttribDataModifyTrajectoryDef::MODIFY_TYPE_VELOCITY)
        eularRotation *= networkUpdateTimePos->m_value;
      eularRotation.x = fmodf(eularRotation.x, 360.0f);
      eularRotation.y = fmodf(eularRotation.y, 360.0f);
      eularRotation.z = fmodf(eularRotation.z, 360.0f);

      // Convert the Euler angle to a quaternion.
      modifyingQuat.fromEulerXYZ(NMP::degreesToRadians(eularRotation));
    }
    else if (nodeStateDefAttrib->m_angularModifyType == AttribDataModifyTrajectoryDef::MODIFY_TYPE_VELOCITY)
    {
      // Convert to an axis angle, scale the angle and convert back to a quaternion.
      NMP::Vector3 axis;
      float angle;
      modifyingQuat.toAxisAngle(axis, angle);
      angle = fmodf(angle * networkUpdateTimePos->m_value, (NM_PI *2.0f));
      modifyingQuat.fromAxisAngle(axis, angle);
    }
    modifyingQuat.normalise();

    //-----------------------------
    switch (nodeStateDefAttrib->m_angularOperation)
    {
      case AttribDataModifyTrajectoryDef::OPERATION_TYPE_ADD_TO_DELTA:
        // Add to the input delta rotation.
        outputDeltaTrajectoryAtt = inputDeltaTrajectoryAtt * modifyingQuat;
        break;
      case AttribDataModifyTrajectoryDef::OPERATION_TYPE_SET_DELTA:
        // Replace the input delta rotation with the new value.
        outputDeltaTrajectoryAtt = modifyingQuat;
        break;
      case AttribDataModifyTrajectoryDef::OPERATION_TYPE_SET_TRANSFORM:
      default:
        // We are setting an absolute root transform, so we need to calculate the necessary delta from the last frames character transform. 
        NMP::Quat invPrevCharAttWorld = prevCharAttWorld;
        invPrevCharAttWorld.conjugate();
        outputDeltaTrajectoryAtt = invPrevCharAttWorld * modifyingQuat;
        break;
    }
    outputDeltaTrajectoryAtt.normalise();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskModifyTrajectoryTransformTrajectoryDelta(Dispatcher::TaskParameters* parameters)
{
  const AttribDataTrajectoryDeltaTransform* inputTrajectoryDelta = parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);
  AttribDataTrajectoryDeltaTransform* outputTrajectoryDelta = parameters->createOutputAttrib<AttribDataTrajectoryDeltaTransform>(1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);
  const AttribDataVector3* translation = parameters->getOptionalInputAttrib<AttribDataVector3>(2, ATTRIB_SEMANTIC_CP_VECTOR3);
  const AttribDataVector4* rotation = parameters->getOptionalInputAttrib<AttribDataVector4>(3, ATTRIB_SEMANTIC_CP_VECTOR4);
  const AttribDataModifyTrajectoryDef* nodeStateDef = parameters->getInputAttrib<AttribDataModifyTrajectoryDef>(4, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  const AttribDataCharacterProperties* const characterControllerState = parameters->getInputAttrib<AttribDataCharacterProperties>(5, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES);
  const AttribDataUpdatePlaybackPos* networkUpdateTimePos = parameters->getInputAttrib<AttribDataUpdatePlaybackPos>(6, ATTRIB_SEMANTIC_UPDATE_TIME_POS);

  modifyTrajectoryTransform(
    inputTrajectoryDelta->m_deltaPos,
    inputTrajectoryDelta->m_deltaAtt,
    outputTrajectoryDelta->m_deltaPos,
    outputTrajectoryDelta->m_deltaAtt,
    translation,
    rotation,
    nodeStateDef,
    characterControllerState,
    networkUpdateTimePos);
 }

//----------------------------------------------------------------------------------------------------------------------
void TaskModifyTrajectoryTransformTrajectoryDeltaAndTransforms(Dispatcher::TaskParameters* parameters)
{
  const AttribDataTransformBuffer* inputTransforms = parameters->getInputAttrib<AttribDataTransformBuffer>(0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
  const uint32_t numRigJoints = inputTransforms->m_transformBuffer->getLength();
  AttribDataTransformBuffer* outputTransforms = parameters->createOutputAttribTransformBuffer(1, numRigJoints, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
  const AttribDataVector3* translation = parameters->getOptionalInputAttrib<AttribDataVector3>(2, ATTRIB_SEMANTIC_CP_VECTOR3);
  const AttribDataVector4* rotation = parameters->getOptionalInputAttrib<AttribDataVector4>(3, ATTRIB_SEMANTIC_CP_VECTOR4);
  const AttribDataModifyTrajectoryDef* nodeStateDef = parameters->getInputAttrib<AttribDataModifyTrajectoryDef>(4, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  const AttribDataCharacterProperties* const characterControllerState = parameters->getInputAttrib<AttribDataCharacterProperties>(5, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES);
  const AttribDataUpdatePlaybackPos* networkUpdateTimePos = parameters->getInputAttrib<AttribDataUpdatePlaybackPos>(6, ATTRIB_SEMANTIC_UPDATE_TIME_POS);

  // Copy input buffer to output buffer.
  inputTransforms->m_transformBuffer->copyTo(outputTransforms->m_transformBuffer);
  
  modifyTrajectoryTransform(
              *(inputTransforms->m_transformBuffer->getPosQuatChannelPos(0)),
              *(inputTransforms->m_transformBuffer->getPosQuatChannelQuat(0)),
              *(outputTransforms->m_transformBuffer->getPosQuatChannelPos(0)),
              *(outputTransforms->m_transformBuffer->getPosQuatChannelQuat(0)),
              translation,
              rotation,
              nodeStateDef,
              characterControllerState,
              networkUpdateTimePos);
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataModifyTrajectoryDef functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataModifyTrajectoryDef::getMemoryRequirements()
{
  // Header
  NMP::Memory::Format result(sizeof(AttribDataModifyTrajectoryDef), MR_ATTRIB_DATA_ALIGNMENT);
    
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataModifyTrajectoryDef* AttribDataModifyTrajectoryDef::init(
  NMP::Memory::Resource& resource,
  RotationType           rotationType,
  ModifyType             linearModifyType,
  OperationType          linearOperation,
  ModifyType             angularModifyType,
  OperationType          angularOperation,
  uint16_t               refCount)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataModifyTrajectoryDef), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataModifyTrajectoryDef* result = (AttribDataModifyTrajectoryDef*) resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_MODIFY_TRAJECTORY_DEF);
  result->setRefCount(refCount);
  
  result->m_rotationType      = rotationType;
  result->m_linearModifyType  = linearModifyType;
  result->m_linearOperation   = linearOperation;
  result->m_angularModifyType = angularModifyType;
  result->m_angularOperation  = angularOperation;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataModifyTrajectoryDef::create(
  NMP::MemoryAllocator* allocator,
  RotationType          rotationType,
  ModifyType            linearModifyType,
  OperationType         linearOperation,
  ModifyType            angularModifyType,
  OperationType         angularOperation,
  uint16_t              refCount)
{
  NMP::Memory::Format memReqs = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  NMP_ASSERT(resource.ptr);

  AttribDataHandle result;
  result.m_attribData = init(
                          resource,
                          rotationType,
                          linearModifyType,
                          linearOperation,
                          angularModifyType,
                          angularOperation,
                          refCount);
  result.m_format = memReqs;

  // Store the allocator so we know where to free this attribData from when we destroy it.
  result.m_attribData->m_allocator = allocator;
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataModifyTrajectoryDef::locate(AttribData* target)
{
  AttribDataModifyTrajectoryDef* result = (AttribDataModifyTrajectoryDef*) target;

  // Header
  AttribData::locate(target);
  
  NMP::endianSwap(result->m_rotationType);
  NMP::endianSwap(result->m_linearModifyType);
  NMP::endianSwap(result->m_linearOperation);
  NMP::endianSwap(result->m_angularModifyType);
  NMP::endianSwap(result->m_angularOperation);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataModifyTrajectoryDef::dislocate(AttribData* target)
{
  AttribDataModifyTrajectoryDef* result = (AttribDataModifyTrajectoryDef*) target;

  NMP::endianSwap(result->m_rotationType);
  NMP::endianSwap(result->m_linearModifyType);
  NMP::endianSwap(result->m_linearOperation);
  NMP::endianSwap(result->m_angularModifyType);
  NMP::endianSwap(result->m_angularOperation);
  
  AttribData::dislocate(target);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------

