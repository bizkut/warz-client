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
#include "Morpheme/Nodes/mrNodeModifyJointTransform.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/mrCoreTaskIDs.h"
#include "morpheme/mrBlendOps.h"
#include "morpheme/mrCommonTasks.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Queuing functions.
//----------------------------------------------------------------------------------------------------------------------
Task* nodeModifyJointTransformQueueTransforms(
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
                            CoreTaskIDs::MR_TASKID_MODIFYJOINTTRANSFORM_TRANSFORMS,
                            nodeID,
                            10,
                            dependentParameter);
    if (task)
    {
      FrameCount currFrameNo = net->getCurrentFrameNo();
      NMP_ASSERT(currFrameNo > 0);

      AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
      NodeID activeChildNodeID = net->getActiveChildNodeID(nodeID, 0);

      // Input transforms
      net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, activeChildNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);

      // Input delta trajectory
      net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, activeChildNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
            
      // Output transforms
      net->TaskAddOutputParamZeroLifespan(task, 2, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);

      // Output delta trajectory
      net->TaskAddOutputParamZeroLifespan(task, 3, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
      
      // Position control parameter
      net->TaskAddOptionalInputCP(task, 4, ATTRIB_SEMANTIC_CP_VECTOR3, node->getInputCPConnection(1));

      // Rotation control parameter
      net->TaskAddOptionalInputCP(task, 5, ATTRIB_SEMANTIC_CP_VECTOR4, node->getInputCPConnection(2));

      // Joint index
      net->TaskAddDefInputParam(task, 6, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, nodeID, activeAnimSetIndex);

      // Definition state.
      net->TaskAddDefInputParam(task, 7, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, nodeID);

      // The character controller, for determining the world-space location of the character.
      net->TaskAddNetInputParam(task, 8, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES, NETWORK_NODE_ID, INVALID_NODE_ID, currFrameNo);

      // Animation rig.
      net->TaskAddDefInputParam(task, 9, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);
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
Task* nodeModifyJointTransformQueueTrajectoryDeltaAndTransforms(
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
                            CoreTaskIDs::MR_TASKID_MODIFYJOINTTRANSFORM_TRAJECTORY_DELTA_AND_TRANSFORMS,
                            nodeID,
                            8,
                            dependentParameter);
    if (task)
    {
      FrameCount currFrameNo = net->getCurrentFrameNo();
      NMP_ASSERT(currFrameNo > 0);

      AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
      NodeID activeChildNodeID = net->getActiveChildNodeID(nodeID, 0);

      // Input transforms
      net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, activeChildNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
                  
      // Output transforms
      net->TaskAddOutputParamZeroLifespan(task, 1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
      
      // Position control parameter
      net->TaskAddOptionalInputCP(task, 2, ATTRIB_SEMANTIC_CP_VECTOR3, node->getInputCPConnection(1));

      // Rotation control parameter
      net->TaskAddOptionalInputCP(task, 3, ATTRIB_SEMANTIC_CP_VECTOR4, node->getInputCPConnection(2));

      // Joint index
      net->TaskAddDefInputParam(task, 4, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, nodeID, activeAnimSetIndex);

      // Definition state.
      net->TaskAddDefInputParam(task, 5, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, nodeID);

      // The character controller, for determining the world-space location of the character.
      net->TaskAddNetInputParam(task, 6, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES, NETWORK_NODE_ID, INVALID_NODE_ID, currFrameNo);

      // Animation rig.
      net->TaskAddDefInputParam(task, 7, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);
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
static void modifyJointTransform(
  const NMP::DataBuffer*               inputTransforms,
  NMP::Vector3&                        outputTransformPos,   // Initialised with values from the input.
  NMP::Quat&                           outputTransformAtt,   // Initialised with values from the input.
  const AttribDataVector3*             modifyingTranslationAttrib,
  const AttribDataVector4*             modifyingRotationAttrib,
  int32_t                              jointIndex,
  const AttribDataModifyJointDef*      nodeStateDefAttrib,
  const AttribDataCharacterProperties* characterControllerStateAttrib,
  const AnimRigDef*                    animRig,
  const NMP::Vector3&                  inputDeltaTrajectoryPos,
  const NMP::Quat&                     inputDeltaTrajectoryAtt)
{
  // Make sure the Rig and Transform Buffer joint counts match.
  NMP_ASSERT(inputTransforms->getLength() == animRig->getNumBones());

  // Make sure the joint index isn't out of range.
  NMP_ASSERT(jointIndex < ((int32_t)animRig->getNumBones()));

    
  //-----------------------------
  // Calculate the character space transform of the parent joint.
  NMP::Vector3 parentJointPosChar(0.0f, 0.0f, 0.0f);
  NMP::Quat parentJointAttChar = NMP::Quat::kIdentity; 
  int32_t parentJointIndex = animRig->getParentBoneIndex(jointIndex);
  if (parentJointIndex != -1)
  {
    NMP::Vector3 rootPos(0.0f, 0.0f, 0.0f);
    NMP::Quat rootAtt = NMP::Quat::kIdentity;
    MR::BlendOpsBase::accumulateTransform(
                                      parentJointIndex,
                                      rootPos,
                                      rootAtt,
                                      inputTransforms,
                                      animRig,
                                      parentJointPosChar,
                                      parentJointAttChar);
  }
  NMP::Quat invParentJointAttChar = parentJointAttChar;
  invParentJointAttChar.conjugate();

  //-----------------------------
  // Calculate the world space transform of the parent joint.

  // Calculate this frames character transform, based on the last frames character controller position + the delta trajectory from the child.
  NMP::Quat parentJointAttWorld = characterControllerStateAttrib->m_worldRootTransform.toQuat();
  NMP::Vector3 parentJointPosWorld = characterControllerStateAttrib->m_worldRootTransform.translation();

  if (parentJointIndex != -1)
  {
    // The networks suggested delta translation is in the characters local space (relative to the characters current world orientation).
    // So to get the suggested delta translation in to world space rotate it by the characters current world orientation.
    NMP::Vector3 workingDeltaTrajectoryPos = parentJointAttWorld.rotateVector(inputDeltaTrajectoryPos);

    // Calculate what the character root transform would be after applying the delta transform this frame.
    NMP::Quat nextCharAttWorld = parentJointAttWorld * inputDeltaTrajectoryAtt;
    NMP::Vector3 nextCharPosWorld = parentJointPosWorld + workingDeltaTrajectoryPos;

    // Calculate the world space transform of the parent joint.
    MR::BlendOpsBase::accumulateTransform(
                                      parentJointIndex,
                                      nextCharPosWorld,
                                      nextCharAttWorld,
                                      inputTransforms,
                                      animRig,
                                      parentJointPosWorld,
                                      parentJointAttWorld);
  }
  NMP::Quat invParentJointAttWorld = parentJointAttWorld;
  invParentJointAttWorld.conjugate();


  //-----------------------------
  if (modifyingTranslationAttrib)
  {
    if (nodeStateDefAttrib->m_linearOperation == AttribDataModifyJointDef::OPERATION_TYPE_SET) // Set operation.
    {
      switch (nodeStateDefAttrib->m_coordinateSpace)
      {
        case AttribDataModifyJointDef::COORD_SPACE_WORLD:
          // Set joint translation, world space.
          // Work out what the world space input translation is in local space (local to parent joint),
          // by multiplying it by the inverse of its parent world space transform.
          outputTransformPos = modifyingTranslationAttrib->m_value - parentJointPosWorld;
          outputTransformPos = invParentJointAttWorld.rotateVector(outputTransformPos);
          break;
        case AttribDataModifyJointDef::COORD_SPACE_CHARACTER:
          // Set joint translation, character space.
          // Work out what the character space input translation is in local space (local to parent joint),
          // by multiplying it by the inverse of its parent character space transform.
          outputTransformPos = modifyingTranslationAttrib->m_value - parentJointPosChar;
          outputTransformPos = invParentJointAttChar.rotateVector(outputTransformPos);
          break;
        case AttribDataModifyJointDef::COORD_SPACE_LOCAL:
        default:
          // Set joint translation, local space.
          outputTransformPos = modifyingTranslationAttrib->m_value;
          break;
      }
    }
    else  // Add operation.
    {
      if (!inputTransforms->hasChannel(jointIndex))
      {
        // This is an additive op, but the channel is not present so use the transform from the bind pose as the basis.
        outputTransformPos = *(animRig->getBindPoseBonePos(jointIndex));
      }

      NMP::Vector3 jointPosWorld;
      NMP::Vector3 jointPosChar;
      switch (nodeStateDefAttrib->m_coordinateSpace)
      {
        case AttribDataModifyJointDef::COORD_SPACE_WORLD:
          // Add to joint translation, world space.
          jointPosWorld = parentJointPosWorld + parentJointAttWorld.rotateVector(outputTransformPos) + modifyingTranslationAttrib->m_value;
          // Calculate the position relative to the parent joint.
          outputTransformPos = jointPosWorld - parentJointPosWorld;
          outputTransformPos = invParentJointAttWorld.rotateVector(outputTransformPos);
          break;
        case AttribDataModifyJointDef::COORD_SPACE_CHARACTER:
          // Add to joint translation, character space.
          jointPosChar = parentJointPosChar + parentJointAttChar.rotateVector(outputTransformPos) + modifyingTranslationAttrib->m_value;
          // Calculate the position relative to the parent joint.
          outputTransformPos = jointPosChar - parentJointPosChar;
          outputTransformPos = invParentJointAttChar.rotateVector(outputTransformPos);
          break;
        case AttribDataModifyJointDef::COORD_SPACE_LOCAL:
        default:
          // Add to joint translation, local space.
          outputTransformPos += modifyingTranslationAttrib->m_value; 
          break;
      }
    }
  }

  //-----------------------------
  if (modifyingRotationAttrib)
  {
    // Get the input rotation expressed as a quaternion.
    NMP::Quat quaternionRotation = (NMP::Quat) modifyingRotationAttrib->m_value;
    if (nodeStateDefAttrib->m_rotationType == AttribDataModifyJointDef::ROTATION_TYPE_EULER)
    {
      NMP::Vector3 eularRotation(
                      fmodf(modifyingRotationAttrib->m_value.x, 360),
                      fmodf(modifyingRotationAttrib->m_value.y, 360),
                      fmodf(modifyingRotationAttrib->m_value.z, 360));
      quaternionRotation.fromEulerXYZ(NMP::degreesToRadians(eularRotation));
    }
    quaternionRotation.normalise();


    if (nodeStateDefAttrib->m_angularOperation == AttribDataModifyJointDef::OPERATION_TYPE_SET) // Set operation.
    {
      switch (nodeStateDefAttrib->m_coordinateSpace)
      {
        case AttribDataModifyJointDef::COORD_SPACE_WORLD:
          // Set joint rotation, world space.
          outputTransformAtt = invParentJointAttWorld * quaternionRotation;
          break;
        case AttribDataModifyJointDef::COORD_SPACE_CHARACTER:
          // Set joint rotation, character space.
          outputTransformAtt = invParentJointAttChar * quaternionRotation;
          break;
        case AttribDataModifyJointDef::COORD_SPACE_LOCAL:
        default:
          // Set joint rotation, local space.
          outputTransformAtt = quaternionRotation;
          break;
      }
    }
    else  // Add operation.
    {
      if (!inputTransforms->hasChannel(jointIndex))
      {
        // This is an additive op, but the channel is not present so use the transform from the bind pose as the basis.
        outputTransformAtt = *(animRig->getBindPoseBoneQuat(jointIndex));
      }

      NMP::Quat jointAttWorld;
      NMP::Quat jointAttChar;
      switch (nodeStateDefAttrib->m_coordinateSpace)
      {
        case AttribDataModifyJointDef::COORD_SPACE_WORLD:
          // Add to joint rotation, world space.
          jointAttWorld = quaternionRotation * parentJointAttWorld * outputTransformAtt;
          // Calculate the rotation relative to the parent joint.
          outputTransformAtt = invParentJointAttWorld * jointAttWorld;
          break;
        case AttribDataModifyJointDef::COORD_SPACE_CHARACTER:
          // Add to joint rotation, character space.
          jointAttWorld = quaternionRotation * parentJointAttChar * outputTransformAtt;
          // Calculate the rotation relative to the parent joint.
          outputTransformAtt = invParentJointAttChar * jointAttWorld;
          break;
        case AttribDataModifyJointDef::COORD_SPACE_LOCAL:
        default:
          // Add to joint rotation, local space.
          outputTransformAtt *= quaternionRotation;
          break;
      }
    }

    outputTransformAtt.normalise();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskModifyJointTransformTrajectoryDeltaAndTransforms(Dispatcher::TaskParameters* parameters)
{
  const AttribDataTransformBuffer* inputTransforms = parameters->getInputAttrib<AttribDataTransformBuffer>(0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
  const uint32_t numRigJoints = inputTransforms->m_transformBuffer->getLength();
  AttribDataTransformBuffer* outputTransforms = parameters->createOutputAttribTransformBuffer(1, numRigJoints, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
  const AttribDataVector3* translation = parameters->getOptionalInputAttrib<AttribDataVector3>(2, ATTRIB_SEMANTIC_CP_VECTOR3);
  const AttribDataVector4* rotation = parameters->getOptionalInputAttrib<AttribDataVector4>(3, ATTRIB_SEMANTIC_CP_VECTOR4);
  const AttribDataInt* jointIndex = parameters->getInputAttrib<AttribDataInt>(4, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);
  const AttribDataModifyJointDef* nodeStateDef = parameters->getInputAttrib<AttribDataModifyJointDef>(5, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  const AttribDataCharacterProperties* const characterControllerState = parameters->getInputAttrib<AttribDataCharacterProperties>(6, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES);
  const AttribDataRig* animRig = parameters->getInputAttrib<AttribDataRig>(7, ATTRIB_SEMANTIC_RIG);
  

  // Copy input buffer to output buffer.
  inputTransforms->m_transformBuffer->copyTo(outputTransforms->m_transformBuffer);
  
  //-----------------------------
  // Make a copy of the trajectory delta, if it is not present use the identity.
  NMP::Vector3 inputTrajDeltaPos(0.0f, 0.0f, 0.0f);
  NMP::Quat inputTrajDeltaAtt = NMP::Quat::kIdentity; 
  if (!inputTransforms->m_transformBuffer->hasChannel(0))
  {
    inputTrajDeltaPos = *(inputTransforms->m_transformBuffer->getPosQuatChannelPos(0));
    inputTrajDeltaAtt = *(inputTransforms->m_transformBuffer->getPosQuatChannelQuat(0)); 
  }

  //-----------------------------
  // Make a reference to the joint local space transform, if it is not present copy in the transform from the bind pose.
  NMP::Vector3* outputJointPos = outputTransforms->m_transformBuffer->getPosQuatChannelPos(jointIndex->m_value);
  NMP::Quat* outputJointAtt = outputTransforms->m_transformBuffer->getPosQuatChannelQuat(jointIndex->m_value);
  if (!outputTransforms->m_transformBuffer->hasChannel(jointIndex->m_value))
  {
    // We are modifying a joint that has been filtered out so just set it to the bind pose.
    (*outputJointPos) = *(animRig->m_rig->getBindPoseBonePos(jointIndex->m_value));
    (*outputJointAtt) = *(animRig->m_rig->getBindPoseBoneQuat(jointIndex->m_value));
  }
  
  //-----------------------------
  // Apply the joint modification.
  modifyJointTransform(
            inputTransforms->m_transformBuffer,
            *outputJointPos,
            *outputJointAtt,
            translation,
            rotation,
            jointIndex->m_value,
            nodeStateDef,
            characterControllerState,
            animRig->m_rig,
            inputTrajDeltaPos,
            inputTrajDeltaAtt);

  //-----------------------------
  // We may have changed the used status of the joint, so we need to recalculate the Full flag.
  outputTransforms->m_transformBuffer->setChannelUsed(jointIndex->m_value);
  outputTransforms->m_transformBuffer->calculateFullFlag();
}

//----------------------------------------------------------------------------------------------------------------------
void TaskModifyJointTransformTransforms(Dispatcher::TaskParameters* parameters)
{
  const AttribDataTransformBuffer* inputTransforms = parameters->getInputAttrib<AttribDataTransformBuffer>(0, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  const AttribDataTrajectoryDeltaTransform* inputTrajDeltaTransform = parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);
  const uint32_t numRigJoints = inputTransforms->m_transformBuffer->getLength();
  AttribDataTransformBuffer* outputTransforms = parameters->createOutputAttribTransformBuffer(2, numRigJoints, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  AttribDataTrajectoryDeltaTransform* outputTrajDeltaTransform = parameters->createOutputAttrib<AttribDataTrajectoryDeltaTransform>(3, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);
  const AttribDataVector3* translation = parameters->getOptionalInputAttrib<AttribDataVector3>(4, ATTRIB_SEMANTIC_CP_VECTOR3);
  const AttribDataVector4* rotation = parameters->getOptionalInputAttrib<AttribDataVector4>(5, ATTRIB_SEMANTIC_CP_VECTOR4);
  const AttribDataInt* jointIndex = parameters->getInputAttrib<AttribDataInt>(6, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);
  const AttribDataModifyJointDef* nodeStateDef = parameters->getInputAttrib<AttribDataModifyJointDef>(7, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  const AttribDataCharacterProperties* const characterControllerState = parameters->getInputAttrib<AttribDataCharacterProperties>(8, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES);
  const AttribDataRig* animRig = parameters->getInputAttrib<AttribDataRig>(9, ATTRIB_SEMANTIC_RIG);

  //-----------------------------
  // Copy the input delta trajectory to the output.
  outputTrajDeltaTransform->m_deltaAtt = inputTrajDeltaTransform->m_deltaAtt;
  outputTrajDeltaTransform->m_deltaPos = inputTrajDeltaTransform->m_deltaPos;
  outputTrajDeltaTransform->m_filteredOut = inputTrajDeltaTransform->m_filteredOut;

  //-----------------------------
  // Make a copy of the trajectory delta, if it is not present use the identity.
  NMP::Vector3 inputTrajDeltaPos(0.0f, 0.0f, 0.0f);
  NMP::Quat inputTrajDeltaAtt = NMP::Quat::kIdentity; 
  if (!inputTrajDeltaTransform->m_filteredOut)
  {
    inputTrajDeltaPos = inputTrajDeltaTransform->m_deltaPos;
    inputTrajDeltaAtt = inputTrajDeltaTransform->m_deltaAtt; 
  }
  
  //-----------------------------
  // Make a reference to the joint local space transform, if it is not present copy in the transform from the bind pose.
  NMP::Vector3* outputJointPos = outputTransforms->m_transformBuffer->getPosQuatChannelPos(jointIndex->m_value);
  NMP::Quat* outputJointAtt = outputTransforms->m_transformBuffer->getPosQuatChannelQuat(jointIndex->m_value);
  if (jointIndex->m_value == 0)
  { 
    // We are modifying the trajectory joint.
    if (inputTrajDeltaTransform->m_filteredOut)
    {
      // The trajectory delta is filtered out so just set it to identity for now.
      outputJointPos->setToZero();
      outputJointAtt->identity();
    }
    else
    {
      // Initialise the trajectory transform in the buffer from the input trajectory delta transform.
      (*outputJointPos) = inputTrajDeltaTransform->m_deltaPos;
      (*outputJointAtt) = inputTrajDeltaTransform->m_deltaAtt;
    }
  }
  else if (!outputTransforms->m_transformBuffer->hasChannel(jointIndex->m_value))
  {
    // We are modifying a joint that has been filtered out so just set it to the bind pose.
    (*outputJointPos) = *(animRig->m_rig->getBindPoseBonePos(jointIndex->m_value));
    (*outputJointAtt) = *(animRig->m_rig->getBindPoseBoneQuat(jointIndex->m_value));
  }

  //-----------------------------
  // Apply the joint modification.
  modifyJointTransform(
            inputTransforms->m_transformBuffer,
            *outputJointPos,
            *outputJointAtt,
            translation,
            rotation,
            jointIndex->m_value,
            nodeStateDef,
            characterControllerState,
            animRig->m_rig,
            inputTrajDeltaPos,
            inputTrajDeltaAtt);

  //-----------------------------
  // If we are modifying the trajectory, we have done so in the transform buffer so we need to copy it in to the output trajectory attrib.
  if (jointIndex->m_value == 0)
  {
    outputTrajDeltaTransform->m_deltaPos = *outputJointPos;
    outputTrajDeltaTransform->m_deltaAtt = *outputJointAtt;
    outputTrajDeltaTransform->m_filteredOut = false;
  }

  // We may have changed the used status of the joint, so we need to recalculate the Full flag.
  outputTransforms->m_transformBuffer->setChannelUsed(jointIndex->m_value);
  outputTransforms->m_transformBuffer->calculateFullFlag();
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataModifyJointDef functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataModifyJointDef::getMemoryRequirements()
{
  // Header
  NMP::Memory::Format result(sizeof(AttribDataModifyJointDef), MR_ATTRIB_DATA_ALIGNMENT);
    
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataModifyJointDef* AttribDataModifyJointDef::init(
  NMP::Memory::Resource& resource,
  RotationType           rotationType,
  CoordinateSpace        coordinateSpace,
  OperationType          linearOperation,
  OperationType          angularOperation,
  uint16_t               refCount)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataModifyJointDef), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataModifyJointDef* result = (AttribDataModifyJointDef*) resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_MODIFY_JOINT_DEF);
  result->setRefCount(refCount);
  
  result->m_rotationType = rotationType;
  result->m_coordinateSpace = coordinateSpace;
  result->m_linearOperation = linearOperation;
  result->m_angularOperation = angularOperation;
      
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataModifyJointDef::create(
  NMP::MemoryAllocator* allocator,
  RotationType          rotationType,
  CoordinateSpace       coordinateSpace,
  OperationType         linearOperation,
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
                          coordinateSpace,
                          linearOperation,
                          angularOperation,
                          refCount);
  result.m_format = memReqs;

  // Store the allocator so we know where to free this attribData from when we destroy it.
  result.m_attribData->m_allocator = allocator;
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataModifyJointDef::locate(AttribData* target)
{
  AttribDataModifyJointDef* result = (AttribDataModifyJointDef*) target;

  // Header
  AttribData::locate(target);
  
  NMP::endianSwap(result->m_rotationType);
  NMP::endianSwap(result->m_coordinateSpace);
  NMP::endianSwap(result->m_linearOperation);
  NMP::endianSwap(result->m_angularOperation);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataModifyJointDef::dislocate(AttribData* target)
{
  AttribDataModifyJointDef* result = (AttribDataModifyJointDef*) target;

  NMP::endianSwap(result->m_rotationType);
  NMP::endianSwap(result->m_coordinateSpace);
  NMP::endianSwap(result->m_linearOperation);
  NMP::endianSwap(result->m_angularOperation);
  
  AttribData::dislocate(target);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------

