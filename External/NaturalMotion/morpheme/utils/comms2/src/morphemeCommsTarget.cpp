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
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMSocket.h"
#include "NMPlatform/NMSocketWrapper.h"

#include "comms/morphemeCommsTarget.h"

#include <stdio.h>

//----------------------------------------------------------------------------------------------------------------------

// Disable warnings about deprecated functions (sprintf, fopen)
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning (disable : 4996)
#endif

namespace MCOMMS
{

//----------------------------------------------------------------------------------------------------------------------
template <typename T> bool doSendCommandPacket(RuntimeTarget* target, T& packet)
{
  packet.serialize();
  return target->sendCommandPacket(packet);
}

//----------------------------------------------------------------------------------------------------------------------
// RuntimeTarget
//----------------------------------------------------------------------------------------------------------------------

CmdRequestID RuntimeTarget::sm_requestID = 1;
uint32_t RuntimeTarget::sm_lastPingID = 0;

//----------------------------------------------------------------------------------------------------------------------
RuntimeTarget::RuntimeTarget(NMP::SocketWrapper commandSocket) :
  m_statusCode(kValid)
{
  m_commandsSocket = new NMP::SocketWrapper(commandSocket);
}

//----------------------------------------------------------------------------------------------------------------------
RuntimeTarget::~RuntimeTarget()
{
  delete m_commandsSocket;
}

//----------------------------------------------------------------------------------------------------------------------
void RuntimeTarget::setNetworkError()
{
  m_statusCode = kNetworkError;
}

//----------------------------------------------------------------------------------------------------------------------
RuntimeTarget::StatusCode RuntimeTarget::getStatusCode() const
{
  return m_statusCode;
}

//----------------------------------------------------------------------------------------------------------------------
void RuntimeTarget::getCommandsConnectionInfo(RuntimeTarget::CommandsConnectionInfo& info)
{
  info = m_commandsConnectionInfo;
}

//----------------------------------------------------------------------------------------------------------------------
bool RuntimeTarget::pingConnection()
{
  MCOMMS::PingCmdPacket pingPacket(sm_lastPingID++);
  return doSendCommandPacket(this, pingPacket);
}

//----------------------------------------------------------------------------------------------------------------------
CmdRequestID RuntimeTarget::loadNetworkDefinition(
  const GUID& guid,
  const char* name,
  Connection* NMP_UNUSED(connection))
{
  LoadNetworkCmdPacket packet;
  ++sm_requestID;
  packet.m_requestId = sm_requestID;
  packet.m_networkGUID = guid;

  NMP_ASSERT(strlen(name) < 128);
  NMP_STRNCPY_S(packet.m_networkName, 128, name);

  if (doSendCommandPacket(this, packet))
  {
    return sm_requestID;
  }
  return INVALID_CMD_REQUEST_ID;
}

//----------------------------------------------------------------------------------------------------------------------
CmdRequestID RuntimeTarget::referenceNetworkInstance(InstanceID instanceID)
{
  ReferenceNetworkInstanceCmdPacket packet;
  ++sm_requestID;
  packet.m_requestId = sm_requestID;
  packet.m_instanceId = instanceID;

  if (doSendCommandPacket(this, packet))
  {
    return sm_requestID;
  }
  return INVALID_CMD_REQUEST_ID;
}

//----------------------------------------------------------------------------------------------------------------------
CmdRequestID RuntimeTarget::unreferenceNetworkInstance(InstanceID instanceID)
{
  UnreferenceNetworkInstanceCmdPacket packet;
  ++sm_requestID;
  packet.m_requestId = sm_requestID;
  packet.m_instanceId = instanceID;

  if (doSendCommandPacket(this, packet))
  {
    return sm_requestID;
  }
  return INVALID_CMD_REQUEST_ID;
}

//----------------------------------------------------------------------------------------------------------------------
CmdRequestID RuntimeTarget::pauseSession(bool pause)
{
  PauseSessionCmdPacket packet;
  ++sm_requestID;
  packet.m_requestId = sm_requestID;
  packet.m_pause = pause;
  if (doSendCommandPacket(this, packet))
  {
    return sm_requestID;
  }
  return INVALID_CMD_REQUEST_ID;
}

//----------------------------------------------------------------------------------------------------------------------
CmdRequestID RuntimeTarget::startSession(float frameRate)
{
  ++sm_requestID;
  StartSessionCmdPacket packet;
  packet.m_requestId = sm_requestID;
  packet.m_frameRate = frameRate;
  if (doSendCommandPacket(this, packet))
  {
    return sm_requestID;
  }
  return INVALID_CMD_REQUEST_ID;
}

//----------------------------------------------------------------------------------------------------------------------
CmdRequestID RuntimeTarget::stopSession()
{
  ++sm_requestID;
  StopSessionCmdPacket packet;
  packet.m_requestId = sm_requestID;
  if (doSendCommandPacket(this, packet))
  {
    return sm_requestID;
  }
  return INVALID_CMD_REQUEST_ID;
}

//----------------------------------------------------------------------------------------------------------------------
CmdRequestID RuntimeTarget::createNetworkInstance(
  const GUID&         guid,
  const char*         instanceName,
  uint32_t            animSetIndex,
  const NMP::Vector3& startPosition,
  const NMP::Quat&    startRotation)
{
  ++sm_requestID;

  uint32_t requiredSize = CreateNetworkInstanceCmdPacket::getRequiredMemorySize(instanceName);

  NetworkDataBuffer buffer;
  buffer.initReceiveBuffer(requiredSize, NMP_NATURAL_TYPE_ALIGNMENT);

  CreateNetworkInstanceCmdPacket* packet = CreateNetworkInstanceCmdPacket::create(&buffer, instanceName);
  packet->m_requestId = sm_requestID;
  packet->m_initialAnimSetIndex = animSetIndex;
  packet->m_characterStartPosition = vec3fromVector3(startPosition);
  packet->m_characterStartRotation = vec4fromQuat(startRotation);
  packet->m_networkGUID = guid;

  if (doSendCommandPacket(this, *packet))
  {
    buffer.term();
    return sm_requestID;
  }

  buffer.term();
  return INVALID_CMD_REQUEST_ID;
}

//----------------------------------------------------------------------------------------------------------------------
CmdRequestID RuntimeTarget::destroyNetworkDefinition(const GUID& guid)
{
  ++sm_requestID;
  DestroyNetworkDefinitionCmdPacket packet;
  packet.m_networkGUID = guid;
  packet.m_requestId = sm_requestID;
  if (doSendCommandPacket(this, packet))
  {
    return sm_requestID;
  }
  return INVALID_CMD_REQUEST_ID;
}

//----------------------------------------------------------------------------------------------------------------------
CmdRequestID RuntimeTarget::destroyNetworkInstance(InstanceID id)
{
  ++sm_requestID;
  DestroyNetworkInstanceCmdPacket packet;
  packet.m_instanceId = id;
  packet.m_requestId = sm_requestID;
  if (doSendCommandPacket(this, packet))
  {
    return sm_requestID;
  }
  return INVALID_CMD_REQUEST_ID;
}

//----------------------------------------------------------------------------------------------------------------------
CmdRequestID RuntimeTarget::clearCachedData()
{
  ++sm_requestID;
  ClearCachedDataCmdPacket packet;
  packet.m_requestId = sm_requestID;
  if (doSendCommandPacket(this, packet))
  {
    return sm_requestID;
  }
  return INVALID_CMD_REQUEST_ID;
}

//----------------------------------------------------------------------------------------------------------------------
bool RuntimeTarget::enableOutputDataBuffering(
  InstanceID             id,
  commsNodeID            nodeID,
  MR::NodeOutputDataType dataType,
  bool                   enable)
{
  ++sm_requestID;
  EnableOutputDataCmdPacket packet;
  packet.m_instanceID = id;
  packet.m_nodeID = nodeID;
  packet.m_outputDataType = dataType;
  packet.m_enable = enable;
  return doSendCommandPacket(this, packet);
}

//----------------------------------------------------------------------------------------------------------------------
bool RuntimeTarget::setDebugOutputFlags(InstanceID id, uint32_t flags)
{
  ++sm_requestID;
  SetDebugOutputFlagsCmdPacket packet;
  packet.m_instanceID = id;
  packet.m_debugOutputFlags = flags;
  return doSendCommandPacket(this, packet);
}

//----------------------------------------------------------------------------------------------------------------------
bool RuntimeTarget::setNodeDebugOutputAllOn(InstanceID instanceID)
{
  NetworkDataBuffer buffer;
  uint32_t pktSize = SetDebugOutputOnNodesCmdPacket::getRequiredMemorySize();
  buffer.initReceiveBuffer(pktSize, 16);
  SetDebugOutputOnNodesCmdPacket* packet = SetDebugOutputOnNodesCmdPacket::createAllOn(&buffer);
  packet->m_instanceID = instanceID;
  bool result = doSendCommandPacket(this, *packet);
  buffer.term();
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool RuntimeTarget::setNodeDebugOutputAllOff(InstanceID instanceID)
{
  NetworkDataBuffer buffer;
  uint32_t pktSize = SetDebugOutputOnNodesCmdPacket::getRequiredMemorySize();
  buffer.initReceiveBuffer(pktSize, 16);
  SetDebugOutputOnNodesCmdPacket* packet = SetDebugOutputOnNodesCmdPacket::createAllOff(&buffer);
  packet->m_instanceID = instanceID;
  bool result = doSendCommandPacket(this, *packet);
  buffer.term();
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool RuntimeTarget::setNodeDebugOutputSomeNodesAllSemantics(
  InstanceID instanceID,
  uint32_t   numNodes,
  uint16_t*  nodesIDs,
  bool       on)
{
  NetworkDataBuffer buffer;
  uint32_t pktSize = SetDebugOutputOnNodesCmdPacket::getRequiredMemorySize(numNodes);
  buffer.initReceiveBuffer(pktSize, 16);
  SetDebugOutputOnNodesCmdPacket* packet = SetDebugOutputOnNodesCmdPacket::createSomeNodesAllSemantics(&buffer, on, numNodes);
  packet->m_instanceID = instanceID;
  uint16_t* outputNodeIDs = packet->getNodeIDs();
  for (uint32_t i = 0; i < numNodes; ++i)
  {
    outputNodeIDs[i] = nodesIDs[i];
  }

  bool result = doSendCommandPacket(this, *packet);
  buffer.term();
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool RuntimeTarget::setNodeDebugOutputAllNodesSomeSemantics(
  InstanceID instanceID,
  uint32_t   numSemantics,
  uint16_t*  semantics,
  bool       on)
{
  NetworkDataBuffer buffer;
  uint32_t pktSize = SetDebugOutputOnNodesCmdPacket::getRequiredMemorySize(0, numSemantics);
  buffer.initReceiveBuffer(pktSize, 16);
  SetDebugOutputOnNodesCmdPacket* packet = SetDebugOutputOnNodesCmdPacket::createAllNodesSomeSemantics(&buffer, on, numSemantics);
  packet->m_instanceID = instanceID;
  uint16_t* outputSemantics = packet->getSemantics();
  for (uint32_t i = 0; i < numSemantics; ++i)
  {
    outputSemantics[i] = semantics[i];
  }

  bool result = doSendCommandPacket(this, *packet);
  buffer.term();
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool RuntimeTarget::setNodeDebugOutputSomeNodesSomeSemantics(
  InstanceID instanceID,
  uint32_t   numNodes,
  uint16_t*  nodeIDs,
  uint32_t   numSemantics,
  uint16_t*  semantics,
  bool       on)
{
  NetworkDataBuffer buffer;
  uint32_t pktSize = SetDebugOutputOnNodesCmdPacket::getRequiredMemorySize(numNodes, numSemantics);
  buffer.initReceiveBuffer(pktSize, 16);
  SetDebugOutputOnNodesCmdPacket* packet = SetDebugOutputOnNodesCmdPacket::createSomeNodesSomeSemantics(&buffer, on, numNodes, numSemantics);
  packet->m_instanceID = instanceID;
  uint16_t* outputNodeIDs = packet->getNodeIDs();
  for (uint32_t i = 0; i < numNodes; ++i)
  {
    outputNodeIDs[i] = nodeIDs[i];
  }
  uint16_t* outputSemantics = packet->getSemantics();
  for (uint32_t i = 0; i < numSemantics; ++i)
  {
    outputSemantics[i] = semantics[i];
  }

  bool result = doSendCommandPacket(this, *packet);
  buffer.term();
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool RuntimeTarget::enableAsynchStepping(bool enable)
{
  ++sm_requestID;
  StepModeCmdPacket packet;
  packet.m_requestId = sm_requestID;
  packet.m_stepMode = (uint8_t)(enable ? StepModeCmdPacket::kAsynchronous : StepModeCmdPacket::kSynchronous);
  return doSendCommandPacket(this, packet);
}

//----------------------------------------------------------------------------------------------------------------------
CmdRequestID RuntimeTarget::stepAllInstances(float dt, float fps)
{
  ++sm_requestID;
  StepCmdPacket packet;
  packet.m_requestId = sm_requestID;
  packet.m_deltaTime = dt;
  packet.m_frameRate = fps;
  if (doSendCommandPacket(this, packet))
  {
    return sm_requestID;
  }
  return INVALID_CMD_REQUEST_ID;
}

//----------------------------------------------------------------------------------------------------------------------
bool RuntimeTarget::setRootTransform(InstanceID instanceID, const NMP::PosQuat& rootTransform)
{
  ++sm_requestID;
  SetRootTransformCmdPacket packet;
  packet.m_instanceId = instanceID;
  packet.m_transform.m_pos = rootTransform.m_pos;
  packet.m_transform.m_quat = rootTransform.m_quat;
  return doSendCommandPacket(this, packet);
}

//----------------------------------------------------------------------------------------------------------------------
bool RuntimeTarget::setControlParameter(
  InstanceID               instanceID,
  commsNodeID              nodeID,
  MR::NodeOutputDataType   type,
  const void*              cparamData)
{
  ++sm_requestID;
  SetControlParameterCmdPacket packet;
  packet.m_controlParamId = nodeID;
  packet.m_instanceId = instanceID;
  packet.m_type = (uint16_t) type;

  const float* floatVals = (const float*)cparamData;
  switch (type)
  {
  case NODE_OUTPUT_DATA_TYPE_FLOAT:
    packet.m_controlParamValue.m_value.f = floatVals[0];
    packet.m_controlParamValue.m_type = MCOMMS::ValueUnion::kFloat;
    break;

  case NODE_OUTPUT_DATA_TYPE_BOOL:
    {
      const bool* bVal = (const bool*)cparamData;
      packet.m_controlParamValue.m_value.b = *bVal;
      packet.m_controlParamValue.m_type = MCOMMS::ValueUnion::kBool;
    }
    break;

  case NODE_OUTPUT_DATA_TYPE_VECTOR3:
    packet.m_controlParamValue.m_value.v.v[0] = floatVals[0];
    packet.m_controlParamValue.m_value.v.v[1] = floatVals[1];
    packet.m_controlParamValue.m_value.v.v[2] = floatVals[2];
    packet.m_controlParamValue.m_type = MCOMMS::ValueUnion::kVector3;
    break;

  case NODE_OUTPUT_DATA_TYPE_VECTOR4:
    packet.m_controlParamValue.m_value.w.v[0] = floatVals[0];
    packet.m_controlParamValue.m_value.w.v[1] = floatVals[1];
    packet.m_controlParamValue.m_value.w.v[2] = floatVals[2];
    packet.m_controlParamValue.m_value.w.v[3] = floatVals[3];
    packet.m_controlParamValue.m_type = MCOMMS::ValueUnion::kVector4;
    break;

  case NODE_OUTPUT_DATA_TYPE_INT:
    {
      const int32_t* iVal = (const int32_t*) cparamData;
      packet.m_controlParamValue.m_value.i = *iVal;
      packet.m_controlParamValue.m_type = MCOMMS::ValueUnion::kInt;
    }
    break;

  case NODE_OUTPUT_DATA_TYPE_UINT:
    {
      const uint32_t* iVal = (const uint32_t*) cparamData;
      packet.m_controlParamValue.m_value.u = *iVal;
      packet.m_controlParamValue.m_type = MCOMMS::ValueUnion::kUint;
    }
    break;

  case NODE_OUTPUT_DATA_TYPE_PHYSICS_OBJECT_POINTER:
    {
      const uint32_t* iVal = (const uint32_t*) cparamData;
      packet.m_controlParamValue.m_value.u = *iVal;
      packet.m_controlParamValue.m_type = MCOMMS::ValueUnion::kUint;
    }
    break;

  default:
    break;
  }

  return doSendCommandPacket(this, packet);
}

//----------------------------------------------------------------------------------------------------------------------
bool RuntimeTarget::setAnimSet(InstanceID instanceID, uint32_t animSetIndex)
{
  ++sm_requestID;
  SetAnimationSetCmdPacket packet;
  packet.m_instanceId = instanceID;
  packet.m_animSetIndex = animSetIndex;

  return doSendCommandPacket(this, packet);
}

//----------------------------------------------------------------------------------------------------------------------
bool RuntimeTarget::setAssetManagerAnimSet(uint32_t animSetIndex)
{
  ++sm_requestID;
  SetAssetManagerAnimationSetCmdPacket packet;
  packet.m_animSetIndex = animSetIndex;

  return doSendCommandPacket(this, packet);
}

//----------------------------------------------------------------------------------------------------------------------
bool RuntimeTarget::sendRequest(InstanceID instanceID, commsNodeID stateMachineNodeID, NetworkMessageID requestID)
{
  ++sm_requestID;

  NetworkDataBuffer buffer;
  uint32_t pktSize = MessageBufferCmdPacket::getRequiredMemorySize((uint32_t)0);
  buffer.initReceiveBuffer(pktSize, 16);
  MessageBufferCmdPacket* packet = MessageBufferCmdPacket::create(&buffer, instanceID, false, requestID, MESSAGE_TYPE_REQUEST, 
    stateMachineNodeID,(uint32_t) 0, 0);
  bool result = doSendCommandPacket(this, *packet);
  buffer.term();
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool RuntimeTarget::broadcastRequest(InstanceID instanceID, NetworkMessageID requestID)
{
  ++sm_requestID;

  NetworkDataBuffer buffer;
  uint32_t pktSize = MessageBufferCmdPacket::getRequiredMemorySize((uint32_t)0);
  buffer.initReceiveBuffer(pktSize, 16);
  MessageBufferCmdPacket* packet = MessageBufferCmdPacket::create(&buffer, instanceID,false, requestID, MESSAGE_TYPE_REQUEST, 
    MR::INVALID_NODE_ID,(uint32_t) 0, 0);
  bool result = doSendCommandPacket(this, *packet);
  buffer.term();
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool RuntimeTarget::sendMessage(InstanceID instanceID,bool sendToAssetManager, commsNodeID nodeID, NetworkMessageID requestID,
                              MR::MessageType typeID, void * dataBuffer, size_t bufferSize)
{
  ++sm_requestID;
  NetworkDataBuffer buffer;
  uint32_t pktSize = MessageBufferCmdPacket::getRequiredMemorySize((uint32_t)bufferSize);
  buffer.initReceiveBuffer(pktSize, 16);
  MessageBufferCmdPacket* packet = MessageBufferCmdPacket::create(&buffer, instanceID, sendToAssetManager,requestID, typeID, 
                                             nodeID,(uint32_t) bufferSize, dataBuffer);
  bool result = doSendCommandPacket(this, *packet);
  buffer.term();
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool RuntimeTarget::broadcastMessage(InstanceID instanceID,bool sendToAssetManager, NetworkMessageID requestID,
                                   MR::MessageType typeID, void * dataBuffer, size_t bufferSize)
{
  ++sm_requestID;
  NetworkDataBuffer buffer;
  uint32_t pktSize = MessageBufferCmdPacket::getRequiredMemorySize((uint32_t)bufferSize);
  buffer.initReceiveBuffer(pktSize, 16);
  MessageBufferCmdPacket* packet = MessageBufferCmdPacket::create(&buffer, instanceID, sendToAssetManager, requestID, typeID,
    MR::INVALID_NODE_ID, (uint32_t)bufferSize, dataBuffer);
  bool result = doSendCommandPacket(this, *packet); 
  buffer.term();
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool RuntimeTarget::setCurrentState(InstanceID instanceID, commsNodeID stateMachineNodeID, commsNodeID newRootStateID)
{
  ++sm_requestID;
  SetCurrentStateCmdPacket packet;
  packet.m_instanceId = instanceID;
  packet.m_rootStateID = newRootStateID;
  packet.m_stateMachineID = stateMachineNodeID;
  return doSendCommandPacket(this, packet);
}

//----------------------------------------------------------------------------------------------------------------------
bool RuntimeTarget::executeCommand(InstanceID instanceID, const char* utf8Command)
{
  ++sm_requestID;
  ExecuteCommandCmdPacket* packet = ExecuteCommandCmdPacket::create(instanceID, utf8Command);
  packet->m_requestId = sm_requestID;
  bool res = doSendCommandPacket(this, *packet);
  NMP::Memory::memFree(packet);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
CmdRequestID RuntimeTarget::setAnimationBrowserAnim(const char* animBrowserAnim)
{
  ++sm_requestID;
  SetAnimBrowserAnimCmdPacket packet;
  packet.m_requestId = sm_requestID;
  NMP_STRNCPY_S(packet.m_animBrowserAnim, 128, animBrowserAnim);
  if (doSendCommandPacket(this, packet))
  {
    return sm_requestID;
  }
  return INVALID_CMD_REQUEST_ID;
}

//----------------------------------------------------------------------------------------------------------------------
CmdRequestID RuntimeTarget::setAnimationBrowserNetwork(
  const char* animBrowserNetwork,
  const char* animationSourceNodeName,
  const char* retargetingSourceNodeName,
  const char* rescalingSourceNodeName)
{
  ++sm_requestID;
  SetAnimBrowserNetworkCmdPacket packet;
  packet.m_requestId = sm_requestID;
  NMP_STRNCPY_S(packet.m_animBrowserNetwork, 128, animBrowserNetwork);
  NMP_STRNCPY_S(packet.m_animationSourceNodeName, 128, animationSourceNodeName);
  NMP_STRNCPY_S(packet.m_retargetingSourceNodeName, 128, retargetingSourceNodeName);
  NMP_STRNCPY_S(packet.m_rescalingSourceNodeName, 128, rescalingSourceNodeName);
  if (doSendCommandPacket(this, packet))
  {
    return sm_requestID;
  }
  return INVALID_CMD_REQUEST_ID;
}

//----------------------------------------------------------------------------------------------------------------------
CmdRequestID RuntimeTarget::loadAnimationBrowserData(const char* compressionType)
{
  ++sm_requestID;
  LoadAnimBrowserDataCmdPacket packet;
  packet.m_requestId = sm_requestID;
  NMP_STRNCPY_S(packet.m_compressionType, sizeof(packet.m_compressionType), compressionType);
  if (doSendCommandPacket(this, packet))
  {
    return sm_requestID;
  }
  return INVALID_CMD_REQUEST_ID;
}

//----------------------------------------------------------------------------------------------------------------------
CmdRequestID RuntimeTarget::setAnimationBrowserTime(float time)
{
  ++sm_requestID;
  SetAnimBrowserTimeCmdPacket packet;
  packet.m_requestId = sm_requestID;
  packet.m_time = time;
  if (doSendCommandPacket(this, packet))
  {
    return sm_requestID;
  }
  return INVALID_CMD_REQUEST_ID;
}

//----------------------------------------------------------------------------------------------------------------------
CmdRequestID RuntimeTarget::downloadAnimationBrowserEventDetectionData(float minTime, uint32_t numFrames, float dt)
{
  ++sm_requestID;
  DownloadAnimBrowserEventDetectionDataCmdPacket packet;
  packet.m_requestId = sm_requestID;
  packet.m_minTime = minTime;
  packet.m_numFrames = numFrames;
  packet.m_dt = dt;
  if (doSendCommandPacket(this, packet))
  {
    return sm_requestID;
  }
  return INVALID_CMD_REQUEST_ID;
}

//----------------------------------------------------------------------------------------------------------------------
CmdRequestID RuntimeTarget::createConstraint(uint64_t constraintGUID, uint32_t physicsEngineObjectID,
                              const NMP::Vector3& localSpaceConstraintPosition, const NMP::Vector3& worldSpaceConstraintPosition,
                              bool constrainOrientation, bool constrainAtCentreOfMass,
                              const NMP::Vector3& sourceRayWSPositionHint, const NMP::Vector3& sourceRayWSDirectionHint
                              )
{
  ++sm_requestID;
  CreateConstraintCmdPacket packet;
  packet.m_requestId = sm_requestID;
  packet.m_constraintGUID = constraintGUID; 
  packet.m_physicsEngineObjectID = physicsEngineObjectID; 
  packet.m_localSpaceConstraintPosition = vec3fromVector3(localSpaceConstraintPosition); 
  packet.m_worldSpaceConstraintPosition = vec3fromVector3(worldSpaceConstraintPosition);
  packet.m_sourceRayWSDirectionHint = vec3fromVector3(sourceRayWSDirectionHint);
  packet.m_sourceRayWSPositionHint = vec3fromVector3(sourceRayWSPositionHint);
  packet.m_constraintFlags = (constrainOrientation ?  CreateConstraintCmdPacket::CONSTRAIN_ORIENTATION : 0) |
                             (constrainAtCentreOfMass ? CreateConstraintCmdPacket::CONSTRAINT_AT_COM : 0); 
  if (doSendCommandPacket(this, packet))
  {
    return sm_requestID;
  }
  return INVALID_CMD_REQUEST_ID;
}

//----------------------------------------------------------------------------------------------------------------------
CmdRequestID RuntimeTarget::moveConstraint(uint64_t constraintGUID, const NMP::Vector3& worldSpaceConstraintPosition)
{
  ++sm_requestID;
  MoveConstraintCmdPacket packet;
  packet.m_requestId = sm_requestID;
  packet.m_constraintGUID = constraintGUID;
  packet.m_worldSpaceConstraintPosition = vec3fromVector3(worldSpaceConstraintPosition); 

  if (doSendCommandPacket(this, packet))
  {
    return sm_requestID;
  }
  return INVALID_CMD_REQUEST_ID;
}

//----------------------------------------------------------------------------------------------------------------------
CmdRequestID RuntimeTarget::removeConstraint(uint64_t constraintGUID)
{
  ++sm_requestID;
  RemoveConstraintCmdPacket packet;
  packet.m_constraintGUID = constraintGUID; 
  packet.m_requestId = sm_requestID;
  if (doSendCommandPacket(this, packet))
  {
    return sm_requestID;
  }
  return INVALID_CMD_REQUEST_ID;
}

CmdRequestID RuntimeTarget::applyForce(
    uint32_t physicsEngineObjectID,
    bool isForce, 
    bool isImpulse,
    bool isVelocityChange,
    const NMP::Vector3& force,
    bool applyAtCOM,
    const NMP::Vector3& localSpacePosition,
    const NMP::Vector3& worldSpacePosition
    )
{
  ++sm_requestID;
  ApplyForceCmdPacket packet;
  packet.m_requestId = sm_requestID;
  packet.m_physicsEngineObjectID = physicsEngineObjectID;
  packet.m_force = vec3fromVector3(force); 
  packet.m_localSpacePosition = vec3fromVector3(localSpacePosition); 
  packet.m_worldSpacePosition = vec3fromVector3(worldSpacePosition);

  packet.m_flags = (isForce   ?  ApplyForceCmdPacket::MODE_FORCE : 0) |
                   (isImpulse ?  ApplyForceCmdPacket::MODE_IMPULSE : 0) |
                   (isVelocityChange ?  ApplyForceCmdPacket::MODE_VELOCITY_CHANGE : 0) |
                   (applyAtCOM ? ApplyForceCmdPacket::APPLY_AT_COM : 0);
  if (doSendCommandPacket(this, packet))
  {
    return sm_requestID;
  }
  return INVALID_CMD_REQUEST_ID;
}

//----------------------------------------------------------------------------------------------------------------------
bool RuntimeTarget::createSceneObject(unsigned int numAttr, const Attribute* const* attributes)
{
  // PROTOCOL:
  // connect :
  //  BeginSceneObjectPacket (num attributes)
  //  AttributePacket * n
  //  EndSceneObjectPacket

  bool res = true;
  ++sm_requestID;
  BeginSceneObjectCmdPacket beginSceneObject;
  beginSceneObject.m_requestId = sm_requestID;
  beginSceneObject.m_numAttributes = numAttr;

  res = doSendCommandPacket(this, beginSceneObject);
  if (!res)
  {
    return false;
  }

  // Send each attribute
  for (uint32_t attrIdx = 0 ; attrIdx < numAttr ; ++attrIdx)
  {
    // Send full attribute
    ++sm_requestID;
    AttributeCmdPacket* attrPacket = AttributeCmdPacket::create(attributes[attrIdx]);
    attrPacket->m_requestId = sm_requestID;
    res = doSendCommandPacket(this, *attrPacket);
    NMP::Memory::memFree(attrPacket);
    if (!res)
    {
      return false;
    }
  }

  ++sm_requestID;
  EndSceneObjectCmdPacket endSceneObjectPacket;
  endSceneObjectPacket.m_requestId = sm_requestID;
  res = doSendCommandPacket(this, endSceneObjectPacket);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
bool RuntimeTarget::destroySceneObjectOnTarget(SceneObjectID objectID)
{
  ++sm_requestID;
  DestroySceneObjectCmdPacket destroyObjectPacket;
  destroyObjectPacket.m_requestId = sm_requestID;
  destroyObjectPacket.m_sceneObjectId = objectID;
  return doSendCommandPacket(this, destroyObjectPacket);
}

//----------------------------------------------------------------------------------------------------------------------
bool RuntimeTarget::setSceneObjectAttribute(SceneObjectID objectID, const Attribute* attribute)
{
  ++sm_requestID;
  bool result = true;
  SetAttributeCmdPacket* setAttribute = SetAttributeCmdPacket::create(objectID, attribute);
  result = doSendCommandPacket(this, *setAttribute);
  NMP::Memory::memFree(setAttribute);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool RuntimeTarget::setEnvironmentAttribute(const Attribute* attribute)
{
  ++sm_requestID;
  bool result = true;
  SetEnvironmentAttributeCmdPacket* setAttribute = SetEnvironmentAttributeCmdPacket::create(attribute);
  result = doSendCommandPacket(this, *setAttribute);
  NMP::Memory::memFree(setAttribute);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
CmdRequestID RuntimeTarget::downloadSceneObjects()
{
  ++sm_requestID;
  DownloadSceneObjectsCmdPacket packet;
  packet.m_requestId = sm_requestID;
  if (doSendCommandPacket(this, packet))
  {
    return sm_requestID;
  }
  return INVALID_CMD_REQUEST_ID;
}

//----------------------------------------------------------------------------------------------------------------------
CmdRequestID RuntimeTarget::downloadNetworkDefinition(const GUID& guid)
{
  ++sm_requestID;
  DownloadNetworkDefinitionCmdPacket packet;
  packet.m_requestId = sm_requestID;
  packet.m_guid = guid;
  if (doSendCommandPacket(this, packet))
  {
    return sm_requestID;
  }
  return INVALID_CMD_REQUEST_ID;
}

//----------------------------------------------------------------------------------------------------------------------
CmdRequestID RuntimeTarget::downloadGlobalData()
{
  ++sm_requestID;
  DownloadGlobalDataCmdPacket packet;
  packet.m_requestId = sm_requestID;
  if (doSendCommandPacket(this, packet))
  {
    return sm_requestID;
  }
  return INVALID_CMD_REQUEST_ID;
}

//----------------------------------------------------------------------------------------------------------------------
CmdRequestID RuntimeTarget::downloadFrameData()
{
  ++sm_requestID;
  DownloadFrameDataCmdPacket packet;
  packet.m_requestId = sm_requestID;
  if (doSendCommandPacket(this, packet))
  {
    return sm_requestID;
  }
  return INVALID_CMD_REQUEST_ID;
}

//----------------------------------------------------------------------------------------------------------------------
CmdRequestID RuntimeTarget::identifyRuntimeTarget()
{
  ++sm_requestID;
  IdentificationCmdPacket packet;
  packet.m_requestId = sm_requestID;
  packet.m_connectProtocolVersion = NM_PROTOCOL_VER;
  if (doSendCommandPacket(this, packet))
  {
    return sm_requestID;
  }
  return INVALID_CMD_REQUEST_ID;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t RuntimeTarget::sendBuffer(NMP::SocketWrapper socket, const void* buffer, uint32_t bufferSize)
{
  int bytes;
  int size = bufferSize;
  const char* buf = (const char*)buffer;

  while (size > 0)
  {
    bytes = NMP::send(socket.getSocket(), buf, size, 0);
    if (bytes <= 0)
      break;

    size -= bytes;
    buf += bytes;
  }

  return bufferSize - size;
}

//----------------------------------------------------------------------------------------------------------------------
bool RuntimeTarget::sendCommandPacket(CmdPacketBase& cmdPacket)
{
  uint32_t pktLen = cmdPacket.hdr.m_length;
  cmdPacket.hdr.endianSwap();
  uint32_t bytesSent = sendBuffer(*m_commandsSocket, &cmdPacket, pktLen);
  if (bytesSent != pktLen)
  {
    setNetworkError();
  }
  return bytesSent == pktLen;
}

} // namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif
//----------------------------------------------------------------------------------------------------------------------

