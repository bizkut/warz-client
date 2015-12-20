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
inline IdentificationCmdPacket::IdentificationCmdPacket() :
  CmdPacketBase()
{
  PK_HEADER_CMD(IdentificationCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline IdentificationReplyPacket::IdentificationReplyPacket(uint32_t requestId) :
  ReplyPacket(requestId, pk_IdentificationCmd),
  m_protocolVersion(NM_PROTOCOL_VER)
{
  PK_HEADER_CMDREPLY(IdentificationReply)
}

//----------------------------------------------------------------------------------------------------------------------
inline void IdentificationReplyPacket::serialize()
{
  ReplyPacket::serialize();
  NMP::netEndianSwap(m_flags);
}

//----------------------------------------------------------------------------------------------------------------------
inline void IdentificationReplyPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
inline ReplyPacket::ReplyPacket(uint32_t requestId, uint16_t requestedCmd) :
  PacketBase(),
  m_requestId(requestId),
  m_result(kResultSuccess),
  m_requestedCmd(requestedCmd)
{
  PK_HEADER_CMDREPLY(Reply);
}

//----------------------------------------------------------------------------------------------------------------------
inline void ReplyPacket::serialize()
{
  NMP::netEndianSwap(m_requestId);
  NMP::netEndianSwap(m_result);
  NMP::netEndianSwap(m_requestedCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void ReplyPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
inline BeginFramePacket::BeginFramePacket(float timeStep) :
  PacketBase(),
  m_timeStep(timeStep)
{
  PK_HEADER_FD(BeginFrame)
}

//----------------------------------------------------------------------------------------------------------------------
inline void BeginFramePacket::serialize()
{
  NMP::netEndianSwap(m_timeStep);
  NMP::netEndianSwap(m_frameId);
  NMP::netEndianSwap(m_numNetworkDefs);
  NMP::netEndianSwap(m_numNetworkInstances);
}

//----------------------------------------------------------------------------------------------------------------------
inline void BeginFramePacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
inline EndFramePacket::EndFramePacket() :
  PacketBase()
{
  PK_HEADER_FD(EndFrame)
}

//----------------------------------------------------------------------------------------------------------------------
inline void EndFramePacket::serialize()
{
  NMP::netEndianSwap(m_stepRequestId);
}

//----------------------------------------------------------------------------------------------------------------------
inline void EndFramePacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
inline BeginFrameSegmentPacket::BeginFrameSegmentPacket(uint8_t segmentId) :
  PacketBase(),
  m_segmentId(segmentId),
  m_segmentPayload(0xFFFFFFFF)
{
  PK_HEADER_FD(BeginFrameSegment);
}

//----------------------------------------------------------------------------------------------------------------------
inline void BeginFrameSegmentPacket::serialize()
{
  NMP::netEndianSwap(m_segmentPayload);
}

//----------------------------------------------------------------------------------------------------------------------
inline void BeginFrameSegmentPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
inline EndFrameSegmentPacket::EndFrameSegmentPacket(uint8_t segmentId) :
  PacketBase(),
  m_segmentId(segmentId)
{
  PK_HEADER_FD(EndFrameSegment);
}

//----------------------------------------------------------------------------------------------------------------------
inline void EndFrameSegmentPacket::serialize()
{
}

//----------------------------------------------------------------------------------------------------------------------
inline void EndFrameSegmentPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
inline BeginInstanceDataPacket::BeginInstanceDataPacket(InstanceID instanceId) :
  PacketBase(),
  m_instanceId(instanceId),
  m_instanceDataPayload(0xFFFFFFFF)
{
  PK_HEADER_FD(BeginInstanceData);
}

//----------------------------------------------------------------------------------------------------------------------
inline void BeginInstanceDataPacket::serialize()
{
  NMP::netEndianSwap(m_instanceId);
  NMP::netEndianSwap(m_instanceDataPayload);
}

//----------------------------------------------------------------------------------------------------------------------
inline void BeginInstanceDataPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
inline EndInstanceDataPacket::EndInstanceDataPacket(InstanceID instanceId) :
  PacketBase(),
  m_instanceId(instanceId)
{
  PK_HEADER_FD(EndInstanceData);
}

//----------------------------------------------------------------------------------------------------------------------
inline void EndInstanceDataPacket::serialize()
{
  NMP::netEndianSwap(m_instanceId);
}

//----------------------------------------------------------------------------------------------------------------------
inline void EndInstanceDataPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
inline BeginInstanceSectionPacket::BeginInstanceSectionPacket(uint8_t sectionId) :
  PacketBase(),
  m_sectionId(sectionId),
  m_sectionPayload(0xFFFFFFFF)
{
  PK_HEADER_FD(BeginInstanceSection);
}

//----------------------------------------------------------------------------------------------------------------------
inline void BeginInstanceSectionPacket::serialize()
{
  NMP::netEndianSwap(m_sectionPayload);
}

//----------------------------------------------------------------------------------------------------------------------
inline void BeginInstanceSectionPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
inline EndInstanceSectionPacket::EndInstanceSectionPacket(uint8_t sectionId) :
  PacketBase(),
  m_sectionId(sectionId)
{
  PK_HEADER_FD(EndInstanceSection);
}

//----------------------------------------------------------------------------------------------------------------------
inline void EndInstanceSectionPacket::serialize()
{
}

//----------------------------------------------------------------------------------------------------------------------
inline void EndInstanceSectionPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
inline AddStringToCachePacket::AddStringToCachePacket(const char* string, StringToken token) :
  PacketBase()
{
  PK_HEADER_GBL(AddStringToCache)

  m_token = token;
  size_t stringLen = strlen(string);
  stringLen = NMP::minimum(stringLen, maxStrLen - 1);
  NMP_STRNCPY_S(m_string, stringLen + 1, string);
  m_string[stringLen] = '\0';

  // modify the length of the packet to fit the string
  hdr.m_length = (PacketLen)(sizeof(PacketBase) + sizeof(StringToken) + stringLen + 1);
}

//----------------------------------------------------------------------------------------------------------------------
inline void AddStringToCachePacket::serialize()
{
  NMP::netEndianSwap(m_token);
}

//----------------------------------------------------------------------------------------------------------------------
inline void AddStringToCachePacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline PingCmdPacket::PingCmdPacket(uint32_t id) :
  CmdPacketBase(),
  m_id(id)
{
  PK_HEADER_CMD(PingCmd)
}

//----------------------------------------------------------------------------------------------------------------------
inline void PingCmdPacket::serialize()
{
  CmdPacketBase::serialize();
  NMP::netEndianSwap(m_id);
}

//----------------------------------------------------------------------------------------------------------------------
inline void PingCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline PingDataPacket::PingDataPacket(uint32_t id) :
  PacketBase(),
  m_id(id)
{
  PK_HEADER_CTRL(PingData)
}

//----------------------------------------------------------------------------------------------------------------------
inline void PingDataPacket::serialize()
{
  NMP::netEndianSwap(m_id);
}

//----------------------------------------------------------------------------------------------------------------------
inline void PingDataPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline SetControlParameterCmdPacket::SetControlParameterCmdPacket() :
  CmdPacketBase()
{
  PK_HEADER_CMD(SetControlParameterCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SetControlParameterCmdPacket::serialize()
{
  m_controlParamValue.endianSwap();
  NMP::netEndianSwap(m_instanceId);
  NMP::netEndianSwap(m_controlParamId);
  NMP::netEndianSwap(m_type);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SetControlParameterCmdPacket::deserialize()
{
  NMP::netEndianSwap(m_instanceId);
  NMP::netEndianSwap(m_controlParamId);
  NMP::netEndianSwap(m_type);
  m_controlParamValue.endianSwap();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline SetRootTransformCmdPacket::SetRootTransformCmdPacket() :
  CmdPacketBase()
{
  PK_HEADER_CMD(SetRootTransformCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SetRootTransformCmdPacket::serialize()
{
  NMP::netEndianSwap(m_transform);
  NMP::netEndianSwap(m_instanceId);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SetRootTransformCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline SetAnimationSetCmdPacket::SetAnimationSetCmdPacket() :
CmdPacketBase()
{
  PK_HEADER_CMD(SetAnimationSetCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SetAnimationSetCmdPacket::serialize()
{
  NMP::netEndianSwap(m_instanceId);
  NMP::netEndianSwap(m_animSetIndex);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SetAnimationSetCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline SetAssetManagerAnimationSetCmdPacket::SetAssetManagerAnimationSetCmdPacket() :
CmdPacketBase()
{
  PK_HEADER_CMD(SetAssetManagerAnimationSetCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SetAssetManagerAnimationSetCmdPacket::serialize()
{
  NMP::netEndianSwap(m_animSetIndex);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SetAssetManagerAnimationSetCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline SendRequestCmdPacket::SendRequestCmdPacket() :
  CmdPacketBase()
{
  PK_HEADER_CMD(SendRequestCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SendRequestCmdPacket::serialize()
{
  NMP::netEndianSwap(m_instanceId);
  NMP::netEndianSwap(m_nodeId);
  NMP::netEndianSwap(m_networkRequestID);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SendRequestCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline BroadcastRequestCmdPacket::BroadcastRequestCmdPacket() :
  CmdPacketBase()
{
  PK_HEADER_CMD(BroadcastRequestCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void BroadcastRequestCmdPacket::serialize()
{
  NMP::netEndianSwap(m_instanceId);
  NMP::netEndianSwap(m_networkRequestID);
}

//----------------------------------------------------------------------------------------------------------------------
inline void BroadcastRequestCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline SetCurrentStateCmdPacket::SetCurrentStateCmdPacket() :
  CmdPacketBase()
{
  PK_HEADER_CMD(SetCurrentStateCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SetCurrentStateCmdPacket::serialize()
{
  NMP::netEndianSwap(m_instanceId);
  NMP::netEndianSwap(m_stateMachineID);
  NMP::netEndianSwap(m_rootStateID);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SetCurrentStateCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline ExecuteCommandCmdPacket::ExecuteCommandCmdPacket() :
  CmdPacketBase()
{
  PK_HEADER_CMD(ExecuteCommandCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void ExecuteCommandCmdPacket::serialize()
{
  NMP::netEndianSwap(m_instanceId);
}

//----------------------------------------------------------------------------------------------------------------------
inline void ExecuteCommandCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
inline char* ExecuteCommandCmdPacket::getCommand() const
{
  return (char*)(this + 1);
}

//----------------------------------------------------------------------------------------------------------------------
inline ExecuteCommandCmdPacket* ExecuteCommandCmdPacket::create(InstanceID instanceID, const char* command)
{
  size_t lenStr = strlen(command);
  size_t packetLen = sizeof(ExecuteCommandCmdPacket) + lenStr + 1;
  ExecuteCommandCmdPacket* packet =
    (ExecuteCommandCmdPacket*)NMPMemoryAllocAligned(packetLen, 4);
  NMP_ASSERT(packet);
  packet->m_instanceId = instanceID;
  packet->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_CMD, pk_ExecuteCommandCmd, (PacketLen)packetLen);

  NMP_STRNCPY_S(packet->getCommand(), lenStr + 1, command);
  packet->getCommand()[lenStr] = '\0';

  return packet;
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline EnableOutputDataCmdPacket::EnableOutputDataCmdPacket() :
  CmdPacketBase()
{
  PK_HEADER_CMD(EnableOutputDataCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void EnableOutputDataCmdPacket::serialize()
{
  NMP::netEndianSwap(m_instanceID);
  NMP::netEndianSwap(m_nodeID);
  NMP::netEndianSwap(m_outputDataType);
  NMP::netEndianSwap(m_enable);
}

//----------------------------------------------------------------------------------------------------------------------
inline void EnableOutputDataCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline SetDebugOutputFlagsCmdPacket::SetDebugOutputFlagsCmdPacket() :
  CmdPacketBase()
{
  PK_HEADER_CMD(SetDebugOutputFlagsCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SetDebugOutputFlagsCmdPacket::serialize()
{
  NMP::netEndianSwap(m_instanceID);
  NMP::netEndianSwap(m_debugOutputFlags);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SetDebugOutputFlagsCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline uint32_t SetDebugOutputOnNodesCmdPacket::getRequiredMemorySize(
  uint32_t numNodes,
  uint32_t numSemantics)
{
  uint32_t pktLen = sizeof(SetDebugOutputOnNodesCmdPacket) +
                    (sizeof(uint16_t) * numNodes) +
                    (sizeof(uint16_t) * numSemantics);
  return (uint32_t) NMP::Memory::align(pktLen, 16);
}

//----------------------------------------------------------------------------------------------------------------------
inline SetDebugOutputOnNodesCmdPacket* SetDebugOutputOnNodesCmdPacket::createAllOn(NetworkDataBuffer* buffer)
{
  uint32_t pktLen = getRequiredMemorySize(0, 0);
  SetDebugOutputOnNodesCmdPacket* pkt = buffer->reserveAlignedMemory<SetDebugOutputOnNodesCmdPacket*>(pktLen, 16);

  pkt->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_FD, pk_SetDebugOutputOnNodesCmd, pktLen);
  pkt->m_on = true;
  pkt->m_allNodes = true;
  pkt->m_allSemantics = true;
  pkt->m_numNodeIDs = 0;
  pkt->m_numSemantics = 0;
  return pkt;
}

//----------------------------------------------------------------------------------------------------------------------
inline SetDebugOutputOnNodesCmdPacket* SetDebugOutputOnNodesCmdPacket::createAllOff(NetworkDataBuffer* buffer)
{
  uint32_t pktLen = getRequiredMemorySize(0, 0);
  SetDebugOutputOnNodesCmdPacket* pkt = buffer->reserveAlignedMemory<SetDebugOutputOnNodesCmdPacket*>(pktLen, 16);

  pkt->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_FD, pk_SetDebugOutputOnNodesCmd, pktLen);
  pkt->m_on = false;
  pkt->m_allNodes = true;
  pkt->m_allSemantics = true;
  pkt->m_numNodeIDs = 0;
  pkt->m_numSemantics = 0;
  return pkt;
}

//----------------------------------------------------------------------------------------------------------------------
inline SetDebugOutputOnNodesCmdPacket* SetDebugOutputOnNodesCmdPacket::createSomeNodesAllSemantics(
  NetworkDataBuffer* buffer,
  bool               on,     // Or off.
  uint32_t           numNodes)
{
  uint32_t pktLen = getRequiredMemorySize(numNodes, 0);
  SetDebugOutputOnNodesCmdPacket* pkt = buffer->reserveAlignedMemory<SetDebugOutputOnNodesCmdPacket*>(pktLen, 16);

  pkt->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_FD, pk_SetDebugOutputOnNodesCmd, pktLen);
  pkt->m_on = on;
  pkt->m_allNodes = false;
  pkt->m_allSemantics = true;
  pkt->m_numNodeIDs = numNodes;
  pkt->m_numSemantics = 0;
  return pkt;
}

//----------------------------------------------------------------------------------------------------------------------
inline SetDebugOutputOnNodesCmdPacket* SetDebugOutputOnNodesCmdPacket::createAllNodesSomeSemantics(
  NetworkDataBuffer* buffer,
  bool               on,     // Or off.
  uint32_t           numSemantics)
{
  uint32_t pktLen = getRequiredMemorySize(0, numSemantics);
  SetDebugOutputOnNodesCmdPacket* pkt = buffer->reserveAlignedMemory<SetDebugOutputOnNodesCmdPacket*>(pktLen, 16);

  pkt->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_FD, pk_SetDebugOutputOnNodesCmd, pktLen);
  pkt->m_on = on;
  pkt->m_allNodes = true;
  pkt->m_allSemantics = false;
  pkt->m_numNodeIDs = 0;
  pkt->m_numSemantics = numSemantics;
  return pkt;
}

//----------------------------------------------------------------------------------------------------------------------
inline SetDebugOutputOnNodesCmdPacket* SetDebugOutputOnNodesCmdPacket::createSomeNodesSomeSemantics(
  NetworkDataBuffer* buffer,
  bool               on,     // Or off.
  uint32_t           numNodes,
  uint32_t           numSemantics)
{
  uint32_t pktLen = getRequiredMemorySize(numNodes, numSemantics);
  SetDebugOutputOnNodesCmdPacket* pkt = buffer->reserveAlignedMemory<SetDebugOutputOnNodesCmdPacket*>(pktLen, 16);

  pkt->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_FD, pk_SetDebugOutputOnNodesCmd, pktLen);
  pkt->m_on = on;
  pkt->m_allNodes = false;
  pkt->m_allSemantics = false;
  pkt->m_numNodeIDs = numNodes;
  pkt->m_numSemantics = numSemantics;
  return pkt;
}

//----------------------------------------------------------------------------------------------------------------------
inline void SetDebugOutputOnNodesCmdPacket::serialize()
{
  uint16_t* nodeIDs = getNodeIDs();
  NMP::netEndianSwapArray(nodeIDs, m_numNodeIDs);

  uint16_t* semantics = getSemantics();
  NMP::netEndianSwapArray(semantics, m_numSemantics);

  NMP::netEndianSwap(m_on);
  NMP::netEndianSwap(m_allNodes);
  NMP::netEndianSwap(m_numNodeIDs);
  NMP::netEndianSwap(m_allSemantics);
  NMP::netEndianSwap(m_numSemantics);
  NMP::netEndianSwap(m_instanceID);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SetDebugOutputOnNodesCmdPacket::deserialize()
{
  NMP::netEndianSwap(m_on);
  NMP::netEndianSwap(m_allNodes);
  NMP::netEndianSwap(m_numNodeIDs);
  NMP::netEndianSwap(m_allSemantics);
  NMP::netEndianSwap(m_numSemantics);
  NMP::netEndianSwap(m_instanceID);

  uint16_t* nodeIDs = getNodeIDs();
  NMP::netEndianSwapArray(nodeIDs, m_numNodeIDs);

  uint16_t* semantics = getSemantics();
  NMP::netEndianSwapArray(semantics, m_numSemantics);
}

//----------------------------------------------------------------------------------------------------------------------
inline uint16_t* SetDebugOutputOnNodesCmdPacket::getNodeIDs()
{
  return (uint16_t*)(this + 1);
}

//----------------------------------------------------------------------------------------------------------------------
inline uint16_t* SetDebugOutputOnNodesCmdPacket::getSemantics()
{
  return (((uint16_t*)(this + 1)) + m_numNodeIDs);
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline CreateNetworkInstanceCmdPacket* CreateNetworkInstanceCmdPacket::create(
  NetworkDataBuffer* buffer,
  const char*        instanceName)
{
  uint32_t requiredSize = getRequiredMemorySize(instanceName);
  CreateNetworkInstanceCmdPacket* pkt = buffer->reserveMemory<CreateNetworkInstanceCmdPacket*>(requiredSize);

  pkt->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_CMD, pk_CreateNetworkInstanceCmd, (PacketLen)(requiredSize));
  pkt->setNetworkInstanceName(instanceName);

  return pkt;
}

//----------------------------------------------------------------------------------------------------------------------
inline uint32_t CreateNetworkInstanceCmdPacket::getRequiredMemorySize(
  const char* instanceName)
{
  uint32_t size = sizeof(CreateNetworkInstanceCmdPacket);

  // make sure null instance names are handled gracefully
  if (instanceName != 0)
  {
    size_t length = strlen(instanceName);
    size += static_cast<uint32_t>(length);
  }

  // there will at the very least be an empty string at the end of the packet so reserve space for the
  // null terminator character
  size += 1;

  return size;
}

//----------------------------------------------------------------------------------------------------------------------
inline void CreateNetworkInstanceCmdPacket::serialize()
{
  CmdPacketBase::serialize();
  m_characterStartPosition.endianSwap();
  m_characterStartRotation.endianSwap();
  NMP::netEndianSwap(m_initialAnimSetIndex);
}

//----------------------------------------------------------------------------------------------------------------------
inline void CreateNetworkInstanceCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
inline const char* CreateNetworkInstanceCmdPacket::getNetworkInstanceName() const
{
  return reinterpret_cast<const char*>(this + 1);
}

//----------------------------------------------------------------------------------------------------------------------
inline CreateNetworkInstanceCmdPacket::CreateNetworkInstanceCmdPacket() :
  CmdPacketBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
inline void CreateNetworkInstanceCmdPacket::setNetworkInstanceName(const char* instanceName)
{
  size_t length = 0;
  char* packetInstanceName = getNetworkInstanceName();

  // make sure null instance names are handled gracefully
  if (instanceName != 0)
  {
    length = strlen(instanceName);
    NMP_STRNCPY_S(packetInstanceName, length + 1, instanceName);
  }

  packetInstanceName[length + 1] = '\0';
}

//----------------------------------------------------------------------------------------------------------------------
inline char* CreateNetworkInstanceCmdPacket::getNetworkInstanceName()
{
  return reinterpret_cast<char*>(this + 1);
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline ReferenceNetworkInstanceCmdPacket::ReferenceNetworkInstanceCmdPacket() :
  CmdPacketBase()
{
  PK_HEADER_CMD(ReferenceNetworkInstanceCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void ReferenceNetworkInstanceCmdPacket::serialize()
{
  CmdPacketBase::serialize();
  NMP::netEndianSwap(m_instanceId);
}

//----------------------------------------------------------------------------------------------------------------------
inline void ReferenceNetworkInstanceCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline ReferenceNetworkInstanceReplyPacket::ReferenceNetworkInstanceReplyPacket(uint32_t requestId) :
  ReplyPacket(requestId, pk_ReferenceNetworkInstanceCmd)
{
  PK_HEADER_CMDREPLY(ReferenceNetworkInstanceReply);
}

//----------------------------------------------------------------------------------------------------------------------
inline void ReferenceNetworkInstanceReplyPacket::serialize()
{
  ReplyPacket::serialize();
  NMP::netEndianSwap(m_instanceId);
  NMP::netEndianSwap(m_nameToken);
  NMP::netEndianSwap(m_initialAnimSetIndex);
}

//----------------------------------------------------------------------------------------------------------------------
inline void ReferenceNetworkInstanceReplyPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline UnreferenceNetworkInstanceCmdPacket::UnreferenceNetworkInstanceCmdPacket() :
  CmdPacketBase()
{
  PK_HEADER_CMD(UnreferenceNetworkInstanceCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void UnreferenceNetworkInstanceCmdPacket::serialize()
{
  CmdPacketBase::serialize();
  NMP::netEndianSwap(m_instanceId);
}

//----------------------------------------------------------------------------------------------------------------------
inline void UnreferenceNetworkInstanceCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline LoadNetworkCmdPacket::LoadNetworkCmdPacket() :
  CmdPacketBase()
{
  PK_HEADER_CMD(LoadNetworkCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void LoadNetworkCmdPacket::serialize()
{
  CmdPacketBase::serialize();
}

//----------------------------------------------------------------------------------------------------------------------
inline void LoadNetworkCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline NetworkDefinitionDestroyedCtrlPacket::NetworkDefinitionDestroyedCtrlPacket() :
  PacketBase()
{
  PK_HEADER_CTRL(NetworkDefinitionDestroyedCtrl);
}

//----------------------------------------------------------------------------------------------------------------------
inline void NetworkDefinitionDestroyedCtrlPacket::deserialize()
{

}

//----------------------------------------------------------------------------------------------------------------------
inline void NetworkDefinitionDestroyedCtrlPacket::serialize()
{

}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline DestroyNetworkDefinitionCmdPacket::DestroyNetworkDefinitionCmdPacket() :
  CmdPacketBase()
{
  PK_HEADER_CMD(DestroyNetworkDefinitionCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void DestroyNetworkDefinitionCmdPacket::serialize()
{
  CmdPacketBase::serialize();
}

//----------------------------------------------------------------------------------------------------------------------
inline void DestroyNetworkDefinitionCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline DestroyNetworkInstanceCmdPacket::DestroyNetworkInstanceCmdPacket() :
  CmdPacketBase()
{
  PK_HEADER_CMD(DestroyNetworkInstanceCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void DestroyNetworkInstanceCmdPacket::serialize()
{
  CmdPacketBase::serialize();
  NMP::netEndianSwap(m_instanceId);
}

//----------------------------------------------------------------------------------------------------------------------
inline void DestroyNetworkInstanceCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline StepCmdPacket::StepCmdPacket() :
  CmdPacketBase()
{
  PK_HEADER_CMD(StepCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void StepCmdPacket::serialize()
{
  CmdPacketBase::serialize();
  NMP::netEndianSwap(m_deltaTime);
  NMP::netEndianSwap(m_frameRate);
}

//----------------------------------------------------------------------------------------------------------------------
inline void StepCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline StepModeCmdPacket::StepModeCmdPacket() :
  CmdPacketBase()
{
  PK_HEADER_CMD(StepModeCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void StepModeCmdPacket::serialize()
{
  CmdPacketBase::serialize();
}

//----------------------------------------------------------------------------------------------------------------------
inline void StepModeCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline PreviewChannelsPacket* PreviewChannelsPacket::create(NetworkDataBuffer* buffer, uint16_t numChannels)
{
  PacketLen len = (PacketLen)(sizeof(PreviewChannelsPacket) + numChannels * sizeof(Channel));
  PreviewChannelsPacket* packet = buffer->reserveMemory<PreviewChannelsPacket*>(len);
  packet->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_FD, pk_PreviewChannels, len);
  packet->m_numChannels = numChannels;

  return packet;
}

//----------------------------------------------------------------------------------------------------------------------
inline PreviewChannelsPacket::PreviewChannelsPacket() :
  PacketBase()
{
  PK_HEADER_FD(PreviewChannels);
  m_numChannels = 0;
}

//----------------------------------------------------------------------------------------------------------------------
inline void PreviewChannelsPacket::deserialize()
{
  NMP::netEndianSwap(m_numChannels);
  NMP::netEndianSwap(m_instanceId);

  Channel* channels = getChannels();
  NMP::netEndianSwapArray(channels, m_numChannels);
}

//----------------------------------------------------------------------------------------------------------------------
inline void PreviewChannelsPacket::serialize()
{
  Channel* channels = getChannels();
  NMP::netEndianSwapArray(channels, m_numChannels);

  NMP::netEndianSwap(m_numChannels);
  NMP::netEndianSwap(m_instanceId);
}

//----------------------------------------------------------------------------------------------------------------------
inline Channel* PreviewChannelsPacket::getChannels()
{
  return (Channel*)(this + 1);
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline ClearCachedDataCmdPacket::ClearCachedDataCmdPacket() :
  CmdPacketBase()
{
  PK_HEADER_CMD(ClearCachedDataCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void ClearCachedDataCmdPacket::serialize()
{
  CmdPacketBase::serialize();
}

//----------------------------------------------------------------------------------------------------------------------
inline void ClearCachedDataCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
/// This command should be sent after creating a network instance that connect wants to start debugging.
inline StartSessionCmdPacket::StartSessionCmdPacket() :
  CmdPacketBase()
{
  PK_HEADER_CMD(StartSessionCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void StartSessionCmdPacket::serialize()
{
  CmdPacketBase::serialize();
  NMP::netEndianSwap(m_frameRate);
}

//----------------------------------------------------------------------------------------------------------------------
inline void StartSessionCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline StopSessionCmdPacket::StopSessionCmdPacket() :
  CmdPacketBase()
{
  PK_HEADER_CMD(StopSessionCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void StopSessionCmdPacket::serialize()
{
  CmdPacketBase::serialize();
}

//----------------------------------------------------------------------------------------------------------------------
inline void StopSessionCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
PauseSessionCmdPacket::PauseSessionCmdPacket() :
  CmdPacketBase()
{
  PK_HEADER_CMD(PauseSessionCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void PauseSessionCmdPacket::serialize()
{
  CmdPacketBase::serialize();
}

//----------------------------------------------------------------------------------------------------------------------
inline void PauseSessionCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline NetworkDefPacket::NetworkDefPacket() :
  PacketBase()
{
  PK_HEADER_DEF(NetworkDef);
}

//----------------------------------------------------------------------------------------------------------------------
inline void NetworkDefPacket::serialize()
{
  NMP::netEndianSwap(m_nameToken);
  NMP::netEndianSwap(m_numAnimSets);
}

//----------------------------------------------------------------------------------------------------------------------
inline void NetworkDefPacket::deserialize()
{
  serialize();
}
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
CreateConstraintCmdPacket::CreateConstraintCmdPacket()
{
  PK_HEADER_CMD(CreateConstraintCmd);
}
//----------------------------------------------------------------------------------------------------------------------
inline void CreateConstraintCmdPacket::serialize()
{
  NMP::netEndianSwap(m_constraintGUID);
  NMP::netEndianSwap(m_physicsEngineObjectID);
  NMP::netEndianSwap(m_constraintFlags);
  m_localSpaceConstraintPosition.endianSwap();
  m_worldSpaceConstraintPosition.endianSwap();
  m_sourceRayWSPositionHint.endianSwap();
  m_sourceRayWSDirectionHint.endianSwap();
}

//----------------------------------------------------------------------------------------------------------------------
inline void CreateConstraintCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
MoveConstraintCmdPacket::MoveConstraintCmdPacket()
{
  PK_HEADER_CMD(MoveConstraintCmd);
}
//----------------------------------------------------------------------------------------------------------------------
inline void MoveConstraintCmdPacket::serialize()
{
  NMP::netEndianSwap(m_constraintGUID);
  m_worldSpaceConstraintPosition.endianSwap();
}

//----------------------------------------------------------------------------------------------------------------------
inline void MoveConstraintCmdPacket::deserialize()
{
  serialize();
}
//----------------------------------------------------------------------------------------------------------------------
RemoveConstraintCmdPacket::RemoveConstraintCmdPacket()
{
  PK_HEADER_CMD(RemoveConstraintCmd);
}
//----------------------------------------------------------------------------------------------------------------------
inline void RemoveConstraintCmdPacket::serialize()
{
  NMP::netEndianSwap(m_constraintGUID);

}

//----------------------------------------------------------------------------------------------------------------------
inline void RemoveConstraintCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
ApplyForceCmdPacket::ApplyForceCmdPacket()
{
  PK_HEADER_CMD(ApplyForceCmd);
}
//----------------------------------------------------------------------------------------------------------------------
inline void ApplyForceCmdPacket::serialize()
{
  NMP::netEndianSwap(m_physicsEngineObjectID);
  NMP::netEndianSwap(m_flags);
  m_force.endianSwap();
  m_localSpacePosition.endianSwap();
  m_worldSpacePosition.endianSwap();
}

//----------------------------------------------------------------------------------------------------------------------
inline void ApplyForceCmdPacket::deserialize()
{
  serialize();
}


//----------------------------------------------------------------------------------------------------------------------
inline AnimRigDefPacket* AnimRigDefPacket::create(
  NetworkDataBuffer*          buffer,
  const GUID&                 networkGUID,
  uint32_t                    dataLength)
{
  uint32_t pktLen = sizeof(AnimRigDefPacket) + dataLength;

  AnimRigDefPacket* pkt = buffer->reserveAlignedMemory<AnimRigDefPacket*>(pktLen, 16);

  pkt->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_PD, pk_AnimRigDef, pktLen);
  pkt->m_dataLength = dataLength;
  pkt->m_networkGUID = networkGUID;

  return pkt;
}
//----------------------------------------------------------------------------------------------------------------------
inline void AnimRigDefPacket::serialize()
{
  // NOTE: The data block must be endian swapped correctly for comms transmission (big-endian)
  NMP::netEndianSwap(m_dataLength);
}

//----------------------------------------------------------------------------------------------------------------------
inline void AnimRigDefPacket::deserialize()
{
  NMP::netEndianSwap(m_dataLength);

  if (m_dataLength == 0)
  {
    return;
  }

  void *data = getData();
  MR::AnimRigDefPersistentData::deserialiseTx(data);
}

//----------------------------------------------------------------------------------------------------------------------
inline void *AnimRigDefPacket::getData()
{
  if (m_dataLength == 0)
  {
    return NULL;
  }

  return this + 1;
}

//----------------------------------------------------------------------------------------------------------------------
/// BeginDefPacket
//----------------------------------------------------------------------------------------------------------------------
inline BeginPersistentPacket::BeginPersistentPacket(uint8_t defType) :
  PacketBase(),
  m_dataType(defType)
{
  PK_HEADER_DEF(BeginPersistent);
}

//----------------------------------------------------------------------------------------------------------------------
inline void BeginPersistentPacket::serialize()
{
}

//----------------------------------------------------------------------------------------------------------------------
inline void BeginPersistentPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline EndPersistentPacket::EndPersistentPacket() :
  PacketBase()
{
  PK_HEADER_DEF(EndPersistent);
}

//----------------------------------------------------------------------------------------------------------------------
inline void EndPersistentPacket::serialize()
{
}

//----------------------------------------------------------------------------------------------------------------------
inline void EndPersistentPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
inline NodeDefPacket* NodeDefPacket::create(
  NetworkDataBuffer* buffer, 
  commsNodeID        nodeID,
  MR::NodeType       nodeTypeID,
  StringToken        nameToken,
  uint32_t           numTokens)
{
  NodeDefPacket* pkt = buffer->reserveMemory<NodeDefPacket*>(sizeof(NodeDefPacket) + numTokens * sizeof(StringToken));

  PacketLen len = (PacketLen)(sizeof(NodeDefPacket) + numTokens * sizeof(StringToken));
  pkt->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_PD, pk_NodeDef, len);

  pkt->m_nodeID = nodeID;
  pkt->m_nodeTypeID = nodeTypeID;
  pkt->m_nameToken = nameToken;
  pkt->m_numTagTokens = numTokens; 
  return pkt;
}

//----------------------------------------------------------------------------------------------------------------------
inline void NodeDefPacket::serialize()
{
  if (m_numTagTokens > 0)
  {
    StringToken* ids = getTagTokens();
    NMP::netEndianSwapArray(ids, m_numTagTokens);
  }
  NMP::netEndianSwap(m_numTagTokens);
  NMP::netEndianSwap(m_nodeID);
  NMP::netEndianSwap(m_nodeTypeID);
  NMP::netEndianSwap(m_nameToken);
}

//----------------------------------------------------------------------------------------------------------------------
inline void NodeDefPacket::deserialize()
{
  NMP::netEndianSwap(m_numTagTokens);

  if (m_numTagTokens > 0)
  {
    StringToken* ids = getTagTokens();
    NMP::netEndianSwapArray(ids, m_numTagTokens);
  }

  NMP::netEndianSwap(m_nodeID);
  NMP::netEndianSwap(m_nodeTypeID);
  NMP::netEndianSwap(m_nameToken);
}

//----------------------------------------------------------------------------------------------------------------------
inline StringToken* NodeDefPacket::getTagTokens() const
{
  return (StringToken*)(this + 1);
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline MessageDefPacket::MessageDefPacket() :
  PacketBase()
{
  PK_HEADER_DEF(MessageDef);
}

//----------------------------------------------------------------------------------------------------------------------
inline void MessageDefPacket::serialize()
{
  NMP::netEndianSwap(m_messageID);
  NMP::netEndianSwap(m_nameToken);
}

//----------------------------------------------------------------------------------------------------------------------
inline void MessageDefPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline NetworkInstancePacket::NetworkInstancePacket() :
  PacketBase()
{
  PK_HEADER_FD(NetworkInstance);
}

//----------------------------------------------------------------------------------------------------------------------
inline void NetworkInstancePacket::serialize()
{
  NMP::netEndianSwap(m_networkId);
  NMP::netEndianSwap(m_nodeOutputCount);
  NMP::netEndianSwap(m_frameIndex);
  NMP::netEndianSwap(m_animSetIndex);
  NMP::netEndianSwap(m_maxTransformCount);
  NMP::netEndianSwap(m_numActiveNodes);
}

//----------------------------------------------------------------------------------------------------------------------
inline void NetworkInstancePacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline ConnectActiveInstancePacket::ConnectActiveInstancePacket(InstanceID activeInstanceId) :
  PacketBase(),
  m_activeInstanceId(activeInstanceId)
{
  PK_HEADER_FD(ConnectActiveInstance);
}

//----------------------------------------------------------------------------------------------------------------------
inline void ConnectActiveInstancePacket::serialize()
{
  NMP::netEndianSwap(m_activeInstanceId);
}

//----------------------------------------------------------------------------------------------------------------------
inline void ConnectActiveInstancePacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline TransformBufferPacket::TransformBufferPacket() :
  PacketBase()
{
  PK_HEADER_FD(TransformBuffer);
  m_numTransforms = 0;
}

//----------------------------------------------------------------------------------------------------------------------
inline TransformBufferPacket* TransformBufferPacket::create(NetworkDataBuffer* buffer, uint32_t numTransforms)
{
  uint32_t sz = sizeof(TransformBufferPacket);
  uint32_t len = sz + sizeof(NMP::PosQuat) * numTransforms;
  TransformBufferPacket* pkt = buffer->reserveAlignedMemory<TransformBufferPacket*>(len, 16);

  pkt->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_FD, pk_TransformBuffer, len);

  pkt->m_numTransforms = numTransforms;
  return pkt;
}

//----------------------------------------------------------------------------------------------------------------------
inline void TransformBufferPacket::deserialize()
{
  NMP::netEndianSwap(m_numTransforms);
  NMP::netEndianSwap(m_poseChange);

  NMP::PosQuat* transforms = getTransforms();
  NMP::netEndianSwapArray(transforms, m_numTransforms);
}

//----------------------------------------------------------------------------------------------------------------------
inline void TransformBufferPacket::serialize()
{
  NMP::PosQuat* transforms = getTransforms();
  NMP::netEndianSwapArray(transforms, m_numTransforms);

  NMP::netEndianSwap(m_numTransforms);
  NMP::netEndianSwap(m_poseChange);
}

//----------------------------------------------------------------------------------------------------------------------
inline NMP::PosQuat* TransformBufferPacket::getTransforms()
{
  return (NMP::PosQuat*)(this + 1);
}

//----------------------------------------------------------------------------------------------------------------------
// MessageBufferPacket
//----------------------------------------------------------------------------------------------------------------------
inline MessageBufferCmdPacket::MessageBufferCmdPacket() :
CmdPacketBase()
{
  PK_HEADER_CMD(MessageBufferCmd);
  m_dataBufferSize = 0;
}

//----------------------------------------------------------------------------------------------------------------------
inline MessageBufferCmdPacket* MessageBufferCmdPacket::create(NetworkDataBuffer* buffer, 
                                                                                  InstanceID  instanceId,
                                                                                  bool sendToAssetManager,
                                                                                  NetworkMessageID  networkRequestID,
                                                                                  MR::MessageType requestTypeID, 
                                                                                  commsNodeID broadcastOrNodeID,
                                                                                  uint32_t dataBufferSize,
                                                                                  void * dataBuffer)
{
  uint32_t sz = sizeof(MessageBufferCmdPacket);
  uint32_t len = sz + dataBufferSize;
  MessageBufferCmdPacket* pkt = buffer->reserveAlignedMemory<MessageBufferCmdPacket*>(len, 16);

  pkt->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_FD, pk_MessageBufferCmd, len);

  pkt->m_dataBufferSize = dataBufferSize;
  pkt->m_instanceId = instanceId;
  pkt->m_sendToAssetManager = sendToAssetManager;
  pkt->m_networkMessageID = networkRequestID;
  pkt->m_broadcastOrNodeID = broadcastOrNodeID;
  pkt->m_messageTypeID = requestTypeID;
  memcpy(pkt->getDataBuffer(), dataBuffer, dataBufferSize);
  return pkt;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t MessageBufferCmdPacket::getRequiredMemorySize(uint32_t bufferSize)
{
  uint32_t sz = sizeof(MessageBufferCmdPacket);
  uint32_t len = sz + bufferSize;
  return len; 
}
//----------------------------------------------------------------------------------------------------------------------
inline void MessageBufferCmdPacket::deserialize()
{
  // No need to endian swap the data buffer as connect has already done this.
  NMP::netEndianSwap(m_instanceId); 
  NMP::netEndianSwap(m_networkMessageID);
  NMP::netEndianSwap(m_messageTypeID);
  NMP::netEndianSwap(m_broadcastOrNodeID);
  NMP::netEndianSwap(m_dataBufferSize);
}

//----------------------------------------------------------------------------------------------------------------------
inline void MessageBufferCmdPacket::serialize()
{
  // No need to endian swap the data buffer as connect has already done this.
  NMP::netEndianSwap(m_instanceId); 
  NMP::netEndianSwap(m_networkMessageID);
  NMP::netEndianSwap(m_messageTypeID);
  NMP::netEndianSwap(m_broadcastOrNodeID);
  NMP::netEndianSwap(m_dataBufferSize);
}

//----------------------------------------------------------------------------------------------------------------------
inline void* MessageBufferCmdPacket::getDataBuffer()
{
  return (void*)(this + 1);
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline ID32sPacket* ID32sPacket::create(NetworkDataBuffer* buffer, uint32_t numIDs)
{
  ID32sPacket* pkt = buffer->reserveMemory<ID32sPacket*>(sizeof(ID32sPacket) + numIDs * sizeof(CommsID32));

  PacketLen len = (PacketLen)(sizeof(ID32sPacket) + numIDs * sizeof(CommsID32));
  pkt->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_FD, pk_ID32s, len);
  pkt->m_numIDs = numIDs;

  return pkt;
}

//----------------------------------------------------------------------------------------------------------------------
inline void ID32sPacket::deserialize()
{
  NMP::netEndianSwap(m_numIDs);

  CommsID32* ids = getIDs();
  NMP::netEndianSwapArray(ids, m_numIDs);
}

//----------------------------------------------------------------------------------------------------------------------
inline void ID32sPacket::serialize()
{
  CommsID32* ids = getIDs();
  NMP::netEndianSwapArray(ids, m_numIDs);

  NMP::netEndianSwap(m_numIDs);
}

//----------------------------------------------------------------------------------------------------------------------
inline bool ID32sPacket::find(CommsID32 id)
{
  CommsID32* ids = getIDs();
  for (uint32_t i = 0; i < m_numIDs; ++i)
    if (ids[i] == id)
      return true;
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
inline CommsID32* ID32sPacket::getIDs() const
{
  return (CommsID32*)(this + 1);
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline ID16sPacket* ID16sPacket::create(NetworkDataBuffer* buffer, uint32_t numIDs)
{
  ID16sPacket* pkt = buffer->reserveMemory<ID16sPacket*>(sizeof(ID16sPacket) + numIDs * sizeof(CommsID16));

  PacketLen len = (PacketLen)(sizeof(ID16sPacket) + numIDs * sizeof(CommsID16));
  pkt->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_FD, pk_ID16s, len);
  pkt->m_numIDs = numIDs;

  return pkt;
}

//----------------------------------------------------------------------------------------------------------------------
inline void ID16sPacket::deserialize()
{
  NMP::netEndianSwap(m_numIDs);

  CommsID16* ids = getIDs();
  NMP::netEndianSwapArray(ids, m_numIDs);
}

//----------------------------------------------------------------------------------------------------------------------
inline void ID16sPacket::serialize()
{
  CommsID16* ids = getIDs();
  NMP::netEndianSwapArray(ids, m_numIDs);

  NMP::netEndianSwap(m_numIDs);
}

//----------------------------------------------------------------------------------------------------------------------
inline bool ID16sPacket::find(CommsID16 id)
{
  CommsID16* ids = getIDs();
  for (uint32_t i = 0; i < m_numIDs; ++i)
    if (ids[i] == id)
      return true;
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
inline CommsID16* ID16sPacket::getIDs()
{
  return (CommsID16*)(this + 1);
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline ActiveInstancesPacket* ActiveInstancesPacket::create(
  NetworkDataBuffer* buffer, uint32_t numNetInstances)
{
  uint32_t len =
    sizeof(ActiveInstancesPacket) +
    sizeof(InstanceID) * numNetInstances;

  ActiveInstancesPacket* packet = buffer->reserveMemory<ActiveInstancesPacket*>(len);

  packet->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_FD, pk_ActiveInstances, len);
  packet->m_numNetInstances = numNetInstances;
  return packet;
}

//----------------------------------------------------------------------------------------------------------------------
inline void ActiveInstancesPacket::serialize()
{
  InstanceID* activeInstances = getInstances();
  NMP::netEndianSwapArray(activeInstances, m_numNetInstances);

  NMP::netEndianSwap(m_numNetInstances);
}

//----------------------------------------------------------------------------------------------------------------------
inline void ActiveInstancesPacket::deserialize()
{
  NMP::netEndianSwap(m_numNetInstances);

  InstanceID* activeInstances = getInstances();
  NMP::netEndianSwapArray(activeInstances, m_numNetInstances);
}

//----------------------------------------------------------------------------------------------------------------------
inline InstanceID* ActiveInstancesPacket::getInstances() const
{
  return (InstanceID*)(this + 1);
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline ActiveNodesPacket* ActiveNodesPacket::create(NetworkDataBuffer* buffer, uint32_t numNodes)
{
  ActiveNodesPacket* activeNodes = (ActiveNodesPacket*)ID16sPacket::create(buffer, numNodes);
  activeNodes->hdr.m_id = pk_ActiveNodes;
  return activeNodes;
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline NodeEventMessagesPacket* NodeEventMessagesPacket::create(NetworkDataBuffer* buffer, uint32_t numNodes)
{
  NodeEventMessagesPacket* nodeEventMessages = (NodeEventMessagesPacket*)ID32sPacket::create(buffer, numNodes);
  nodeEventMessages->hdr.m_id = pk_NodeEventMessages;
  return nodeEventMessages;
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline RootStateMachineNodesPacket* RootStateMachineNodesPacket::create(NetworkDataBuffer* buffer, uint32_t numNodes)
{
  RootStateMachineNodesPacket* rootStates = (RootStateMachineNodesPacket*)ID16sPacket::create(buffer, numNodes);
  rootStates->hdr.m_id = pk_RootStateMachineNodes;
  return rootStates;
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline ActiveSceneObjectsPacket* ActiveSceneObjectsPacket::create(NetworkDataBuffer* buffer, uint32_t numSceneObjects)
{
  ActiveSceneObjectsPacket* activeSceneObjects = (ActiveSceneObjectsPacket*)ID32sPacket::create(buffer, numSceneObjects);
  activeSceneObjects->hdr.m_id = pk_ActiveSceneObjects;
  return activeSceneObjects;
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline FilenamePacket* FilenamePacket::create(NetworkDataBuffer* buffer, const char* str)
{
  size_t l = strlen(str);
  uint32_t len = (uint32_t)l;
  FilenamePacket* pkt = buffer->reserveMemory<FilenamePacket*>(sizeof(FilenamePacket) + len + 1);
  pkt->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_FSERV, pk_Filename, (PacketLen)(sizeof(FilenamePacket) + len + 1));
  NMP_STRNCPY_S(pkt->getStr(), len + 1, str);
  pkt->getStr()[len] = '\0';
  return pkt;
}

//----------------------------------------------------------------------------------------------------------------------
inline void FilenamePacket::serialize()
{
}

//----------------------------------------------------------------------------------------------------------------------
inline void FilenamePacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
inline char* FilenamePacket::getStr() const
{
  return (char*)(this + 1);
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline FileSizeRequestPacket* FileSizeRequestPacket::create(NetworkDataBuffer* buffer, const char* str)
{
  FileSizeRequestPacket* pkt = (FileSizeRequestPacket*)FilenamePacket::create(buffer, str);
  pkt->hdr.m_id = pk_FileSizeRequest;
  return pkt;
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline FileRequestPacket* FileRequestPacket::create(NetworkDataBuffer* buffer, const char* str)
{
  FileRequestPacket* pkt = (FileRequestPacket*)FilenamePacket::create(buffer, str);
  pkt->hdr.m_id = pk_FileRequest;
  return pkt;
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline FileSizePacket::FileSizePacket() :
  PacketBase()
{
  PK_HEADER_FSERV(FileSize);
}

//----------------------------------------------------------------------------------------------------------------------
inline void FileSizePacket::serialize()
{
  NMP::netEndianSwap(m_fileSize);
}

//----------------------------------------------------------------------------------------------------------------------
inline void FileSizePacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline FilePacket* FilePacket::create(uint32_t fileSize)
{
  FilePacket* pkt = (FilePacket*)NMPMemoryAllocAligned(sizeof(FilePacket) + fileSize, 4);
  NMP_ASSERT(pkt);
  pkt->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_FSERV, pk_File, (PacketLen)(sizeof(FilePacket) + fileSize));
  pkt->m_dataSize = fileSize;
  return pkt;
}

//----------------------------------------------------------------------------------------------------------------------
inline FilePacket::FilePacket() :
  PacketBase()
{
  // This constructor shouldn't be normally used, but it's useful
  // to partially read the packet, retrieve the actual size and then read the data.
  PK_HEADER_FSERV(File);
}

//----------------------------------------------------------------------------------------------------------------------
inline void FilePacket::serialize()
{
  NMP::netEndianSwap(m_dataSize);
}

//----------------------------------------------------------------------------------------------------------------------
inline void FilePacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
inline char* FilePacket::getData() const
{
  return (char*)(this + 1);
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline ConnectScriptCommandPacket* ConnectScriptCommandPacket::create(NetworkDataBuffer* buffer, const char* str)
{
  size_t l = strlen(str);
  uint32_t len = (uint32_t)l;
  ConnectScriptCommandPacket* pkt = buffer->reserveMemory<ConnectScriptCommandPacket*>(sizeof(ConnectScriptCommandPacket) + len + 1);
  pkt->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_CTRL, pk_ConnectScriptCommand, (PacketLen)(sizeof(ConnectScriptCommandPacket) + len + 1));
  NMP_STRNCPY_S(pkt->getStr(), len + 1, str);
  pkt->getStr()[len] = '\0';
  return pkt;
}

//----------------------------------------------------------------------------------------------------------------------
inline void ConnectScriptCommandPacket::serialize()
{
}

//----------------------------------------------------------------------------------------------------------------------
inline void ConnectScriptCommandPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
inline char* ConnectScriptCommandPacket::getStr() const
{
  return (char*)(this + 1);
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline NetworkCreatedReplyPacket::NetworkCreatedReplyPacket(
  uint32_t requestId, InstanceID instanceId, const GUID& guid, StringToken instanceName, uint32_t animSetIndex) :
  ReplyPacket(requestId, pk_CreateNetworkInstanceCmd),
  m_instanceId(instanceId),
  m_networkGUID(guid),
  m_initialAnimSetIndex(animSetIndex),
  m_instanceName(instanceName)
{
  PK_HEADER_CMDREPLY(NetworkCreatedReply);
}

//----------------------------------------------------------------------------------------------------------------------
inline void NetworkCreatedReplyPacket::serialize()
{
  ReplyPacket::serialize();
  NMP::netEndianSwap(m_instanceId);
  NMP::netEndianSwap(m_initialAnimSetIndex);
  NMP::netEndianSwap(m_instanceName);
}

//----------------------------------------------------------------------------------------------------------------------
inline void NetworkCreatedReplyPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline NetworkDestroyedReplyPacket::NetworkDestroyedReplyPacket(uint32_t requestId, InstanceID instanceId) :
  ReplyPacket(requestId, pk_DestroyNetworkInstanceCmd),
  m_instanceId(instanceId)
{
  PK_HEADER_CMDREPLY(NetworkDestroyedReply);
}

//----------------------------------------------------------------------------------------------------------------------
inline void NetworkDestroyedReplyPacket::serialize()
{
  ReplyPacket::serialize();
  NMP::netEndianSwap(m_instanceId);
}

//----------------------------------------------------------------------------------------------------------------------
inline void NetworkDestroyedReplyPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline NetworkDefLoadedReplyPacket::NetworkDefLoadedReplyPacket(uint32_t requestId) :
  ReplyPacket(requestId, pk_LoadNetworkCmd)
{
  PK_HEADER_CMDREPLY(NetworkDefLoadedReply);
}

//----------------------------------------------------------------------------------------------------------------------
inline void NetworkDefLoadedReplyPacket::serialize()
{
  ReplyPacket::serialize();
}

//----------------------------------------------------------------------------------------------------------------------
inline void NetworkDefLoadedReplyPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline NetworkDefDestroyedReplyPacket::NetworkDefDestroyedReplyPacket(uint32_t requestId, const GUID& guid) :
  ReplyPacket(requestId, pk_DestroyNetworkDefinitionCmd),
  m_networkGUID(guid)
{
  PK_HEADER_CMDREPLY(NetworkDefDestroyedReply);
}

//----------------------------------------------------------------------------------------------------------------------
inline void NetworkDefDestroyedReplyPacket::serialize()
{
  ReplyPacket::serialize();
}

//----------------------------------------------------------------------------------------------------------------------
inline void NetworkDefDestroyedReplyPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline ConnectFrameDataPacket::ConnectFrameDataPacket() :
  PacketBase()
{
  PK_HEADER_FD(ConnectFrameData);
}

//----------------------------------------------------------------------------------------------------------------------
inline void ConnectFrameDataPacket::serialize()
{
  NMP::netEndianSwap(m_rootTranslation);
  NMP::netEndianSwap(m_rootOrientation);
}

//----------------------------------------------------------------------------------------------------------------------
inline void ConnectFrameDataPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline SceneObjectDestroyedPacket::SceneObjectDestroyedPacket()
{
  PK_HEADER_CTRL(SceneObjectDestroyed);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SceneObjectDestroyedPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
inline void SceneObjectDestroyedPacket::serialize()
{
  NMP::netEndianSwap(m_sceneObjectId);
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline DestroySceneObjectCmdPacket::DestroySceneObjectCmdPacket() :
  CmdPacketBase()
{
  PK_HEADER_CMD(DestroySceneObjectCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void DestroySceneObjectCmdPacket::serialize()
{
  NMP::netEndianSwap(m_sceneObjectId);
}

//----------------------------------------------------------------------------------------------------------------------
inline void DestroySceneObjectCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline BeginSceneObjectCmdPacket::BeginSceneObjectCmdPacket() :
  CmdPacketBase()
{
  PK_HEADER_CMD(BeginSceneObjectCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void BeginSceneObjectCmdPacket::serialize()
{
  NMP::netEndianSwap(m_numAttributes);
}

//----------------------------------------------------------------------------------------------------------------------
inline void BeginSceneObjectCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline EndSceneObjectCmdPacket::EndSceneObjectCmdPacket() :
  CmdPacketBase()
{
  PK_HEADER_CMD(EndSceneObjectCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void EndSceneObjectCmdPacket::serialize()
{
}

//----------------------------------------------------------------------------------------------------------------------
inline void EndSceneObjectCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline AttributeCmdPacket* AttributeCmdPacket::create(const Attribute* attribute)
{
  PacketLen packetSize = (PacketLen)(sizeof(AttributeCmdPacket) + sizeof(Attribute) + attribute->getDataSize());
  AttributeCmdPacket* attrPacket = (AttributeCmdPacket*)NMPMemoryAllocAligned(packetSize, 4);
  NMP_ASSERT(attrPacket);
  attrPacket->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_CMD, pk_AttributeCmd, packetSize);

  // Construct similar Attribute in-place in the packet
  const Attribute::Descriptor* desc = attrPacket->getAttributeDescriptor();
  void* data = attrPacket->getAttributeData();

  // Copy the attribute descriptor.
  memcpy((void*)desc, attribute->getDescriptor(), sizeof(Attribute::Descriptor));

  // copy the attribute data.
  memcpy(data, attribute->getData(), attribute->getDataSize());

  return attrPacket;
}

//----------------------------------------------------------------------------------------------------------------------
inline void AttributeCmdPacket::serialize()
{
  CmdPacketBase::serialize();
  Attribute::endianSwapData(getAttributeDescriptor(), getAttributeData());
  Attribute::endianSwapDesc(getAttributeDescriptor());
}

//----------------------------------------------------------------------------------------------------------------------
inline void AttributeCmdPacket::deserialize()
{
  CmdPacketBase::deserialize();
  Attribute::endianSwapDesc(getAttributeDescriptor());
  Attribute::endianSwapData(getAttributeDescriptor(), getAttributeData());
}

//----------------------------------------------------------------------------------------------------------------------
inline Attribute::Descriptor* AttributeCmdPacket::getAttributeDescriptor()
{
  return (Attribute::Descriptor*)((char*)this + sizeof(AttributeCmdPacket));
}

//----------------------------------------------------------------------------------------------------------------------
inline void* AttributeCmdPacket::getAttributeData()
{
  return (void*)((char*)getAttributeDescriptor() + sizeof(Attribute::Descriptor));
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline SetAttributeCmdPacket* SetAttributeCmdPacket::create(uint32_t objectId, const Attribute* attribute)
{
  PacketLen packetSize = (PacketLen)(sizeof(SetAttributeCmdPacket) + sizeof(Attribute) + attribute->getDataSize());
  SetAttributeCmdPacket* attrPacket =
    (SetAttributeCmdPacket*)NMPMemoryAllocAligned(packetSize, 4);
  NMP_ASSERT(attrPacket);

  attrPacket->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_CMD, pk_SetAttributeCmd, packetSize);

  attrPacket->m_sceneObjectId = objectId;

  // Construct similar Attribute in-place in the packet
  const Attribute::Descriptor* desc = attrPacket->getAttributeDescriptor();
  void* data = attrPacket->getAttributeData();

  // Copy the attribute descriptor.
  memcpy((void*)desc, attribute->getDescriptor(), sizeof(Attribute::Descriptor));

  // copy the attribute data.
  memcpy(data, attribute->getData(), attribute->getDataSize());

  return attrPacket;
}

//----------------------------------------------------------------------------------------------------------------------
inline void SetAttributeCmdPacket::serialize()
{
  Attribute::endianSwapData(getAttributeDescriptor(), getAttributeData());
  Attribute::endianSwapDesc(getAttributeDescriptor());
  NMP::netEndianSwap(m_sceneObjectId);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SetAttributeCmdPacket::deserialize()
{
  NMP::netEndianSwap(m_sceneObjectId);
  Attribute::endianSwapDesc(getAttributeDescriptor());
  Attribute::endianSwapData(getAttributeDescriptor(), getAttributeData());
}

//----------------------------------------------------------------------------------------------------------------------
inline Attribute::Descriptor* SetAttributeCmdPacket::getAttributeDescriptor()
{
  return (Attribute::Descriptor*)((char*)this + sizeof(SetAttributeCmdPacket));
}

//----------------------------------------------------------------------------------------------------------------------
inline void* SetAttributeCmdPacket::getAttributeData()
{
  return (void*)((char*)getAttributeDescriptor() + sizeof(Attribute::Descriptor));
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline SetEnvironmentAttributeCmdPacket* SetEnvironmentAttributeCmdPacket::create(const Attribute* attribute)
{
  PacketLen packetSize = (PacketLen)(sizeof(SetEnvironmentAttributeCmdPacket) + sizeof(Attribute) + attribute->getDataSize());
  SetEnvironmentAttributeCmdPacket* attrPacket =
    (SetEnvironmentAttributeCmdPacket*)NMPMemoryAllocAligned(packetSize, 4);
  NMP_ASSERT(attrPacket);

  attrPacket->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_CMD, pk_SetEnvironmentAttributeCmd, packetSize);

  // Construct similar Attribute in-place in the packet
  const Attribute::Descriptor* desc = attrPacket->getAttributeDescriptor();
  void* data = attrPacket->getAttributeData();

  // Copy the attribute descriptor.
  memcpy((void*)desc, attribute->getDescriptor(), sizeof(Attribute::Descriptor));

  // copy the attribute data.
  memcpy(data, attribute->getData(), attribute->getDataSize());

  return attrPacket;
}

//----------------------------------------------------------------------------------------------------------------------
inline void SetEnvironmentAttributeCmdPacket::serialize()
{
  Attribute::endianSwapData(getAttributeDescriptor(), getAttributeData());
  Attribute::endianSwapDesc(getAttributeDescriptor());
}

//----------------------------------------------------------------------------------------------------------------------
inline void SetEnvironmentAttributeCmdPacket::deserialize()
{
  Attribute::endianSwapDesc(getAttributeDescriptor());
  Attribute::endianSwapData(getAttributeDescriptor(), getAttributeData());
}

//----------------------------------------------------------------------------------------------------------------------
inline Attribute::Descriptor* SetEnvironmentAttributeCmdPacket::getAttributeDescriptor()
{
  return (Attribute::Descriptor*)((char*)this + sizeof(SetEnvironmentAttributeCmdPacket));
}

//----------------------------------------------------------------------------------------------------------------------
inline void* SetEnvironmentAttributeCmdPacket::getAttributeData()
{
  return (void*)((char*)getAttributeDescriptor() + sizeof(Attribute::Descriptor));
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline SceneObjectDefPacket::SceneObjectDefPacket() :
  PacketBase()
{
  PK_HEADER_DEF(SceneObjectDef);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SceneObjectDefPacket::serialize()
{
  NMP::netEndianSwap(m_numAttributes);
  NMP::netEndianSwap(m_sceneObjectId);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SceneObjectDefPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline AttributeDefPacket* AttributeDefPacket::create(NetworkDataBuffer* buffer, const Attribute* attribute)
{
  PacketLen packetSize =
    (PacketLen)(sizeof(AttributeDefPacket) + sizeof(Attribute::Descriptor)  + attribute->getDataSize());

  AttributeDefPacket* attributePacket = buffer->reserveMemory<AttributeDefPacket*>(packetSize);

  attributePacket->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_PD, pk_AttributeDef, packetSize);

  Attribute::Descriptor* desc = attributePacket->getAttributeDescriptor();
  void* data = attributePacket->getAttributeData();

  // Manually copy the attribute data and descriptor.
  memcpy(data, attribute->getData(), attribute->getDataSize());
  memcpy(desc, attribute->getDescriptor(), sizeof(Attribute::Descriptor));

  return attributePacket;
}

//----------------------------------------------------------------------------------------------------------------------
inline void AttributeDefPacket::serialize()
{
  void* attrData = getAttributeData();
  Attribute::Descriptor* attrDesc = getAttributeDescriptor();

  Attribute::endianSwapData(attrDesc, attrData);
  Attribute::endianSwapDesc(attrDesc);
}

//----------------------------------------------------------------------------------------------------------------------
inline void AttributeDefPacket::deserialize()
{
  void* attrData = getAttributeData();
  Attribute::Descriptor* attrDesc = getAttributeDescriptor();

  Attribute::endianSwapDesc(attrDesc);
  Attribute::endianSwapData(attrDesc, attrData);
}

//----------------------------------------------------------------------------------------------------------------------
inline Attribute::Descriptor* AttributeDefPacket::getAttributeDescriptor()
{
  return (Attribute::Descriptor*)(this + 1);
}

//----------------------------------------------------------------------------------------------------------------------
inline void* AttributeDefPacket::getAttributeData()
{
  return (void*) (getAttributeDescriptor() + 1);
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline AttributeUpdatePacket* AttributeUpdatePacket::create(NetworkDataBuffer* buffer, const Attribute* attribute)
{
  PacketLen packetSize =
    (PacketLen)(sizeof(AttributeUpdatePacket) + sizeof(Attribute::Descriptor) + attribute->getDataSize());

  AttributeUpdatePacket* attributePacket = buffer->reserveMemory<AttributeUpdatePacket*>(packetSize);

  attributePacket->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_FD, pk_AttributeUpdate, packetSize);

  Attribute::Descriptor* desc = attributePacket->getAttributeDescriptor();
  void* data = attributePacket->getAttributeData();

  // Manually copy the attribute data and descriptor.
  memcpy(data, attribute->getData(), attribute->getDataSize());
  memcpy(desc, attribute->getDescriptor(), sizeof(Attribute::Descriptor));

  return attributePacket;
}

//----------------------------------------------------------------------------------------------------------------------
inline void AttributeUpdatePacket::serialize()
{
  void* attrData = getAttributeData();
  Attribute::Descriptor* attrDesc = getAttributeDescriptor();

  Attribute::endianSwapData(attrDesc, attrData);
  Attribute::endianSwapDesc(attrDesc);
}

//----------------------------------------------------------------------------------------------------------------------
inline void AttributeUpdatePacket::deserialize()
{
  void* attrData = getAttributeData();
  Attribute::Descriptor* attrDesc = getAttributeDescriptor();

  Attribute::endianSwapDesc(attrDesc);
  Attribute::endianSwapData(attrDesc, attrData);
}

//----------------------------------------------------------------------------------------------------------------------
inline Attribute::Descriptor* AttributeUpdatePacket::getAttributeDescriptor()
{
  return (Attribute::Descriptor*)(this + 1);
}

//----------------------------------------------------------------------------------------------------------------------
inline void* AttributeUpdatePacket::getAttributeData()
{
  return (void*) (getAttributeDescriptor() + 1);
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline SceneObjectUpdatePacket::SceneObjectUpdatePacket() :
  PacketBase()
{
  PK_HEADER_FD(SceneObjectUpdate);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SceneObjectUpdatePacket::serialize()
{
  NMP::netEndianSwap(m_numDynamicAttributes);
  NMP::netEndianSwap(m_sceneObjectId);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SceneObjectUpdatePacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline DownloadSceneObjectsCmdPacket::DownloadSceneObjectsCmdPacket()
{
  PK_HEADER_CMD(DownloadSceneObjectsCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void DownloadSceneObjectsCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
inline void DownloadSceneObjectsCmdPacket::serialize()
{
  CmdPacketBase::serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline DownloadGlobalDataCmdPacket::DownloadGlobalDataCmdPacket()
{
  PK_HEADER_CMD(DownloadGlobalDataCmd);

  m_globalDataFlags = (uint32_t)GBLDATAFLAGS_ALL;
}

//----------------------------------------------------------------------------------------------------------------------
inline void DownloadGlobalDataCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
inline void DownloadGlobalDataCmdPacket::serialize()
{
  CmdPacketBase::serialize();
  NMP::netEndianSwap(m_globalDataFlags);
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline DownloadFrameDataCmdPacket::DownloadFrameDataCmdPacket()
{
  PK_HEADER_CMD(DownloadFrameDataCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void DownloadFrameDataCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
inline void DownloadFrameDataCmdPacket::serialize()
{
  CmdPacketBase::serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline DownloadNetworkDefinitionCmdPacket::DownloadNetworkDefinitionCmdPacket()
{
  PK_HEADER_CMD(DownloadNetworkDefinitionCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void DownloadNetworkDefinitionCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
inline void DownloadNetworkDefinitionCmdPacket::serialize()
{
  CmdPacketBase::serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline BeginDispatcherTaskExecutePacket::BeginDispatcherTaskExecutePacket() :
  PacketBase()
{
  PK_HEADER_FD(BeginDispatcherTaskExecute);
}

//----------------------------------------------------------------------------------------------------------------------
inline void BeginDispatcherTaskExecutePacket::serialize()
{
  NMP::netEndianSwap(m_networkInstanceID);
  NMP::netEndianSwap(m_taskid);
  NMP::netEndianSwap(m_taskNameTag);
  NMP::netEndianSwap(m_owningNodeID);
  NMP::netEndianSwap(m_sourceFrame);
}

//----------------------------------------------------------------------------------------------------------------------
inline void BeginDispatcherTaskExecutePacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline AddDispatcherTaskExecuteParameterPacket::AddDispatcherTaskExecuteParameterPacket() :
  PacketBase()
{
  PK_HEADER_FD(AddDispatcherTaskExecuteParameter);
}

//----------------------------------------------------------------------------------------------------------------------
inline void AddDispatcherTaskExecuteParameterPacket::serialize()
{
  NMP::netEndianSwap(m_semanticName);
  NMP::netEndianSwap(m_owningNodeID);
  NMP::netEndianSwap(m_targetNodeID);
  NMP::netEndianSwap(m_validFrame);
  NMP::netEndianSwap(m_lifespan);
}

//----------------------------------------------------------------------------------------------------------------------
inline void AddDispatcherTaskExecuteParameterPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline EndDispatcherTaskExecutePacket::EndDispatcherTaskExecutePacket() :
  PacketBase()
{
  PK_HEADER_FD(EndDispatcherTaskExecute);
}

//----------------------------------------------------------------------------------------------------------------------
inline void EndDispatcherTaskExecutePacket::serialize()
{
}

//----------------------------------------------------------------------------------------------------------------------
inline void EndDispatcherTaskExecutePacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
inline StateMachineMessageEventMsgPacket::StateMachineMessageEventMsgPacket() :
  PacketBase()
{
  PK_HEADER_FD(StateMachineMessageEventMsg);
}

//----------------------------------------------------------------------------------------------------------------------
inline void StateMachineMessageEventMsgPacket::serialize()
{
  NMP::netEndianSwap(m_messageID);
  NMP::netEndianSwap(m_targetSMNodeID);
  NMP::netEndianSwap(m_networkInstanceID);
  NMP::netEndianSwap(m_set);
}

//----------------------------------------------------------------------------------------------------------------------
inline void StateMachineMessageEventMsgPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
// TargetStatusCtrlPacket
//----------------------------------------------------------------------------------------------------------------------
inline TargetStatusCtrlPacket* TargetStatusCtrlPacket::create(
  NetworkDataBuffer* buffer,
  uint32_t           numNetInstances,
  uint32_t           numNetDefinitions)
{
  uint32_t len =
    sizeof(TargetStatusCtrlPacket) +
    sizeof(TargetStatusCtrlPacket::ActiveInstance) * numNetInstances +
    sizeof(GUID) * numNetDefinitions;

  TargetStatusCtrlPacket* packet =
    buffer->reserveMemory<TargetStatusCtrlPacket*>(len);

  packet->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_CTRL, pk_TargetStatusCtrl, len);
  packet->m_numNetInstances = numNetInstances;
  packet->m_numNetDefinitions = numNetDefinitions;
  return packet;
}

//----------------------------------------------------------------------------------------------------------------------
inline void TargetStatusCtrlPacket::serialize()
{
  TargetStatusCtrlPacket::ActiveInstance* activeInstances = getInstances();
  for (uint32_t i = 0; i < m_numNetInstances; ++i)
  {
    NMP::netEndianSwap(activeInstances[i].m_id);
  }
  NMP::netEndianSwap(m_numNetInstances);
  NMP::netEndianSwap(m_numNetDefinitions);
}

//----------------------------------------------------------------------------------------------------------------------
inline void TargetStatusCtrlPacket::deserialize()
{
  TargetStatusCtrlPacket::ActiveInstance* activeInstances = getInstances();
  NMP::netEndianSwap(m_numNetInstances);
  NMP::netEndianSwap(m_numNetDefinitions);
  for (uint32_t i = 0; i < m_numNetInstances; ++i)
  {
    NMP::netEndianSwap(activeInstances[i].m_id);
  }
}

//----------------------------------------------------------------------------------------------------------------------
inline TargetStatusCtrlPacket::ActiveInstance* TargetStatusCtrlPacket::getInstances() const
{
  return (TargetStatusCtrlPacket::ActiveInstance*)(this + 1);
}

//----------------------------------------------------------------------------------------------------------------------
inline GUID* TargetStatusCtrlPacket::getGUIDs() const
{
  char* instancesPtr = (char*)getInstances();
  instancesPtr += getNumInstances() * sizeof(TargetStatusCtrlPacket::ActiveInstance);
  return (GUID*)instancesPtr;
}

//----------------------------------------------------------------------------------------------------------------------
// ScatterBlendWeightsPacket
//----------------------------------------------------------------------------------------------------------------------
inline ScatterBlendWeightsPacket* ScatterBlendWeightsPacket::create(
  NetworkDataBuffer* buffer,
  commsNodeID        scatterBlendNodeID,
  float              barycentricWeights[3],
  commsNodeID        childNodeIDs[3],
  float              desiredMotionParams[2],
  float              achievedMotionParams[2],
  bool               wasProjected)
{
  uint32_t len =
    sizeof(ScatterBlendWeightsPacket);

  ScatterBlendWeightsPacket* packet =
    buffer->reserveMemory<ScatterBlendWeightsPacket*>(len);

  packet->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_FD, pk_ScatterBlendWeights, len);
  packet->m_scatterBlendNodeID = scatterBlendNodeID;

  packet->m_barycentricWeights[0] = barycentricWeights[0];
  packet->m_barycentricWeights[1] = barycentricWeights[1];
  packet->m_barycentricWeights[2] = barycentricWeights[2];

  packet->m_childNodeIDs[0] = childNodeIDs[0];
  packet->m_childNodeIDs[1] = childNodeIDs[1];
  packet->m_childNodeIDs[2] = childNodeIDs[2];

  packet->m_desiredMotionParams[0] = desiredMotionParams[0];
  packet->m_desiredMotionParams[1] = desiredMotionParams[1];

  packet->m_achievedMotionParams[0] = achievedMotionParams[0];
  packet->m_achievedMotionParams[1] = achievedMotionParams[1];

  packet->m_wasProjected = wasProjected;

  return packet;
}

//----------------------------------------------------------------------------------------------------------------------
inline void ScatterBlendWeightsPacket::serialize()
{
  NMP::netEndianSwap(m_scatterBlendNodeID);
  NMP::netEndianSwapArray(m_childNodeIDs, 3);
  NMP::netEndianSwapArray(m_barycentricWeights, 3);
  NMP::netEndianSwapArray(m_desiredMotionParams, 2);
  NMP::netEndianSwapArray(m_achievedMotionParams, 2);
}

//----------------------------------------------------------------------------------------------------------------------
inline void ScatterBlendWeightsPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
