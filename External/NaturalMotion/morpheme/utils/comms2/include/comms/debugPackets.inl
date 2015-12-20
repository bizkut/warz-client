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
#ifndef MCOMMS_DEBUGPACKETS_INL
#define MCOMMS_DEBUGPACKETS_INL
//----------------------------------------------------------------------------------------------------------------------

namespace MCOMMS
{
//----------------------------------------------------------------------------------------------------------------------
/// PersistentDataPacket
//----------------------------------------------------------------------------------------------------------------------
inline PersistentDataPacket::PersistentDataPacket() :
PacketBase()
{
  PK_HEADER_DEF(PersistentData);
}

//----------------------------------------------------------------------------------------------------------------------
inline PersistentDataPacket* PersistentDataPacket::create(
  NetworkDataBuffer*     buffer,
  int32_t                outputDataType,
  uint32_t               dataLen)
{
  uint32_t pktLen = sizeof(PersistentDataPacket) + dataLen;

  PersistentDataPacket* pkt = buffer->reserveAlignedMemory<PersistentDataPacket*>(pktLen, 16);

  pkt->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_PD, pk_PersistentData, pktLen);

  pkt->m_persistentDataType = kGlobalData;
  pkt->m_outputDataType = outputDataType;
  pkt->m_dataLength = dataLen;

  return pkt;
}

//----------------------------------------------------------------------------------------------------------------------
inline PersistentDataPacket* PersistentDataPacket::create(
  NetworkDataBuffer*     buffer,
  const GUID&            networkGUID,
  int32_t                outputDataType,
  uint32_t               dataLen)
{
  uint32_t pktLen = sizeof(PersistentDataPacket) + dataLen;

  PersistentDataPacket* pkt = buffer->reserveAlignedMemory<PersistentDataPacket*>(pktLen, 16);

  pkt->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_PD, pk_PersistentData, pktLen);

  pkt->m_persistentDataType = kDefData;
  pkt->m_networkGUID = networkGUID;
  pkt->m_outputDataType = outputDataType;
  pkt->m_dataLength = dataLen;

  return pkt;
}

//----------------------------------------------------------------------------------------------------------------------
inline PersistentDataPacket* PersistentDataPacket::create(
  NetworkDataBuffer*     buffer,
  const InstanceID&      instanceID,
  int32_t                outputDataType,
  uint32_t               dataLen)
{
  uint32_t pktLen = sizeof(PersistentDataPacket) + dataLen;

  PersistentDataPacket* pkt = buffer->reserveAlignedMemory<PersistentDataPacket*>(pktLen, 16);

  pkt->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_PD, pk_PersistentData, pktLen);

  pkt->m_persistentDataType = kInstanceData;
  pkt->m_instanceID = instanceID;
  pkt->m_outputDataType = outputDataType;
  pkt->m_dataLength = dataLen;

  return pkt;
}

//----------------------------------------------------------------------------------------------------------------------
inline void PersistentDataPacket::deserialize()
{
  NMP::netEndianSwap(m_persistentDataType);
  switch (m_persistentDataType)
  {
  case kGlobalData:
    break;
  case kDefData:
    // guids are not endian swapped
    break;
  case kInstanceData:
    NMP::netEndianSwap(m_instanceID);
    break;
  default:
    break;
  }
  NMP::netEndianSwap(m_outputDataType);
  NMP::netEndianSwap(m_dataLength);

  // don't forget to swap the data part of this packet
}

//----------------------------------------------------------------------------------------------------------------------
inline void PersistentDataPacket::serialize()
{
  // NOTE: The data block must be endian swapped correctly for comms transmission (big-endian)
  // dont endian swap the persistant type before switching on it...
  switch (m_persistentDataType)
  {
  case kGlobalData:
    break;
  case kDefData:
    // guids are not endian swapped
    break;
  case kInstanceData:
    NMP::netEndianSwap(m_instanceID);
    break;
  default:
    break;
  }
  NMP::netEndianSwap(m_persistentDataType);
  NMP::netEndianSwap(m_outputDataType);
  NMP::netEndianSwap(m_dataLength);
}

//----------------------------------------------------------------------------------------------------------------------
inline void *PersistentDataPacket::getData()
{
  if (m_dataLength == 0)
  {
    return NULL;
  }

  void *data = this + 1;
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
/// FrameDataPacket
//----------------------------------------------------------------------------------------------------------------------
inline FrameDataPacket::FrameDataPacket() :
PacketBase()
{
  PK_HEADER_FD(FrameData);
}

//----------------------------------------------------------------------------------------------------------------------
inline FrameDataPacket* FrameDataPacket::create(
  NetworkDataBuffer*     buffer,
  InstanceID             instanceID,
  int32_t                outputDataType,
  uint32_t               dataLen)
{
  uint32_t pktLen = sizeof(FrameDataPacket) + dataLen;

  FrameDataPacket* pkt = buffer->reserveAlignedMemory<FrameDataPacket*>(pktLen, 16);

  pkt->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_FD, pk_FrameData, pktLen);

  pkt->m_frameDataType = kInstanceData;
  pkt->m_instanceID = instanceID;
  pkt->m_dataType = outputDataType;
  pkt->m_dataLength = dataLen;

  return pkt;
}

//----------------------------------------------------------------------------------------------------------------------
inline void FrameDataPacket::deserialize()
{
  NMP::netEndianSwap(m_frameDataType);
  switch (m_frameDataType)
  {
  case kGlobalData:
    break;
  case kInstanceData:
    NMP::netEndianSwap(m_instanceID);
    break;
  default:
    break;
  }
  NMP::netEndianSwap(m_dataType);
  NMP::netEndianSwap(m_dataLength);
}

//----------------------------------------------------------------------------------------------------------------------
inline void FrameDataPacket::serialize()
{
  // NOTE: The data block must be endian swapped correctly for comms transmission (big-endian)
  switch (m_frameDataType)
  {
  case kGlobalData:
    break;
  case kInstanceData:
    NMP::netEndianSwap(m_instanceID);
    break;
  default:
    break;
  }
  NMP::netEndianSwap(m_frameDataType);
  NMP::netEndianSwap(m_dataType);
  NMP::netEndianSwap(m_dataLength);
}

//----------------------------------------------------------------------------------------------------------------------
inline void *FrameDataPacket::getData()
{
  if (m_dataLength == 0)
  {
    return NULL;
  }

  void *data = this + 1;
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
/// NodeOutputDataPacket
//----------------------------------------------------------------------------------------------------------------------
inline NodeOutputDataPacket::NodeOutputDataPacket() :
PacketBase()
{
  PK_HEADER_FD(NodeOutputData);
}

//----------------------------------------------------------------------------------------------------------------------
inline NodeOutputDataPacket* NodeOutputDataPacket::create(
  NetworkDataBuffer*     buffer,
  uint32_t               dataLen,
  commsNodeID            owningNodeID,
  commsNodeID            targetNodeID,
  MR::AttribDataType     type,
  MR::AttribDataSemantic semantic,
  StringToken            semanticName,
  MR::AnimSetIndex       animSetIndex,
  MR::FrameCount         validFrame,
  MR::PinIndex           pinIndex)
{
  uint32_t pktLen = sizeof(NodeOutputDataPacket) + dataLen;

  NodeOutputDataPacket* pkt = buffer->reserveAlignedMemory<NodeOutputDataPacket*>(pktLen, 16);

  pkt->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_FD, pk_NodeOutputData, pktLen);

  pkt->m_nodeOutputType = NODE_OUTPUT_DATA_TYPE_RUNTIME_TYPE;
  pkt->m_dataLength = dataLen;

  pkt->m_owningNodeID = owningNodeID;
  pkt->m_targetNodeID = targetNodeID;
  pkt->m_type = type;
  pkt->m_semantic = semantic;
  pkt->m_semanticNameToken = semanticName;
  pkt->m_animSetIndex = animSetIndex;
  pkt->m_validFrame = validFrame;
  pkt->m_pinIndex = pinIndex;

  return pkt;
}

//----------------------------------------------------------------------------------------------------------------------
inline void NodeOutputDataPacket::serialize()
{
  // NOTE: The data AttribData block must be endian swapped correctly for comms transmission (big-endian)
  // by each AttribTypes registered serializeTx function.

  NMP::netEndianSwap(m_nodeOutputType);
  NMP::netEndianSwap(m_owningNodeID);
  NMP::netEndianSwap(m_targetNodeID);
  NMP::netEndianSwap(m_type);
  NMP::netEndianSwap(m_semantic);
  NMP::netEndianSwap(m_semanticNameToken);
  NMP::netEndianSwap(m_animSetIndex);
  NMP::netEndianSwap(m_validFrame);
  NMP::netEndianSwap(m_dataLength);
  NMP::netEndianSwap(m_pinIndex);
}

//----------------------------------------------------------------------------------------------------------------------
void* NodeOutputDataPacket::getData()
{
  if (m_dataLength == 0)
    return NULL;
  return (this + 1);
}

//----------------------------------------------------------------------------------------------------------------------
/// FrameNodeTimingsPacket
//----------------------------------------------------------------------------------------------------------------------
inline FrameNodeTimingsPacket* FrameNodeTimingsPacket::create(
  NetworkDataBuffer*     buffer,
  InstanceID             networkInstanceID,
  uint32_t               numNodeTimings,
  const MR::NodeTimingElement* nodeTimingElements)
{
  uint32_t pktLen = (uint32_t)NMP::Memory::align(sizeof(FrameNodeTimingsPacket) + (sizeof(MR::NodeTimingElement) * numNodeTimings), 16);
  FrameNodeTimingsPacket* pkt = buffer->reserveAlignedMemory<FrameNodeTimingsPacket*>(pktLen, 16);

  pkt->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_FD, pk_FrameNodeTimings, pktLen);
  pkt->m_networkInstanceID = networkInstanceID;
  pkt->m_numNodeTimings = numNodeTimings;

  MR::NodeTimingElement* timings = pkt->getNodeTimingElements();
  for (uint32_t i = 0; i < numNodeTimings; ++i)
  {
    timings[i] = nodeTimingElements[i];
  }

  return pkt;
}

//----------------------------------------------------------------------------------------------------------------------
inline void FrameNodeTimingsPacket::serialize()
{
  MR::NodeTimingElement* timings = getNodeTimingElements();
  for (uint32_t i = 0; i < m_numNodeTimings; ++i)
  {
    NMP::netEndianSwap(timings[i].m_nodeID);
    NMP::netEndianSwap(timings[i].m_time);
    NMP::netEndianSwap(timings[i].m_type);
  }

  NMP::netEndianSwap(m_numNodeTimings);
  NMP::netEndianSwap(m_networkInstanceID);
}

//----------------------------------------------------------------------------------------------------------------------
inline void FrameNodeTimingsPacket::deserialize()
{
  NMP::netEndianSwap(m_networkInstanceID);
  NMP::netEndianSwap(m_numNodeTimings);

  MR::NodeTimingElement* timings = getNodeTimingElements();
  for (uint32_t i = 0; i < m_numNodeTimings; ++i)
  {
    NMP::netEndianSwap(timings[i].m_nodeID);
    NMP::netEndianSwap(timings[i].m_time);
    NMP::netEndianSwap(timings[i].m_type);
  }
}

//----------------------------------------------------------------------------------------------------------------------
inline MR::NodeTimingElement* FrameNodeTimingsPacket::getNodeTimingElements()
{
  return (MR::NodeTimingElement*)(this + 1);
}

//----------------------------------------------------------------------------------------------------------------------
/// ProfilePointTimingPacket
//----------------------------------------------------------------------------------------------------------------------
inline ProfilePointTimingPacket::ProfilePointTimingPacket()
: PacketBase()
{
  PK_HEADER_FD(ProfilePointTiming);
}

//----------------------------------------------------------------------------------------------------------------------
inline void ProfilePointTimingPacket::serialize()
{
  NMP::netEndianSwap(m_name);
  NMP::netEndianSwap(m_timing);
  NMP::netEndianSwap(m_networkInstanceID);
}

//----------------------------------------------------------------------------------------------------------------------
inline void ProfilePointTimingPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
// MorphemeScratchpadPacket
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE MorphemeScratchpadPacket::MorphemeScratchpadPacket(
  InstanceID networkId,
  ValueUnion::Type paramType)
: PacketBase(),
  m_sourceLineNumber(0),
  m_sourceNetworkID(networkId)
{
  PK_HEADER_FD(MorphemeScratchpad);
  m_data.m_type = (uint8_t)paramType;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void MorphemeScratchpadPacket::serialize()
{
  m_data.endianSwap();
  NMP::netEndianSwap(m_tag);
  NMP::netEndianSwap(m_sourceLineNumber);
  NMP::netEndianSwap(m_sourceNetworkID);
  NMP::netEndianSwap(m_sourceNodeID);
  NMP::netEndianSwap(m_sourceModuleName);
  NMP::netEndianSwap(m_sourceFrame);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void MorphemeScratchpadPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
// DebugDrawPacket
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawPacket* DebugDrawPacket::create(
  uint8_t*           pktData,           // Where the packet is to be created.
  DebugDrawTypes     debugDrawType,     // Line, circle, torque arc etc.
  InstanceID         sourceNetworkID,   // ID of the source network instance.
  commsNodeID        sourceNodeID,      // Which node has initiated the draw command. INVALID_NODE_ID if its not from a specific node.
  StringToken        sourceTagName,     // NULL if not from a specific module.
  MR::FrameCount     frame,             // What frame this data is from. Useful when attempting to buffer data in frame sequence.
  MR::LimbIndex      sourceLimbIndex,   // The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
  uint32_t           dataLength)        // Size of packet data following the base DebugDrawPacket structure.
{
  NMP_ASSERT(pktData != 0);

  DebugDrawPacket* pkt = (DebugDrawPacket*)pktData;

  uint32_t pktLen = sizeof(DebugDrawPacket) + dataLength;
  pkt->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_FD, pk_DebugDraw, pktLen);
  pkt->m_debugDrawType = debugDrawType;
  pkt->m_sourceNetworkID = sourceNetworkID;
  pkt->m_sourceNodeID = sourceNodeID;
  pkt->m_sourceModuleName = sourceTagName;
  pkt->m_dataLength = dataLength;
  pkt->m_sourceFrame = frame;
  pkt->m_sourceLimbIndex = sourceLimbIndex;

  return pkt;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawPacket* DebugDrawPacket::create(
  NetworkDataBuffer* buffer,            // Where the packet is to be created.
  DebugDrawTypes     debugDrawType,     // Line, circle, torque arc etc.
  InstanceID         sourceNetworkID,   // ID of the source network instance.
  commsNodeID        sourceNodeID,      // Which node has initiated the draw command. INVALID_NODE_ID if its not from a specific node.
  StringToken        sourceTagName,     // NULL if not from a specific module.
  MR::FrameCount     frame,             // What frame this data is from. Useful when attempting to buffer data in frame sequence.
  MR::LimbIndex      sourceLimbIndex,   // The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
  uint32_t           dataLength)        // Size of packet data following the base DebugDrawPacket structure.
{
  uint32_t pktLen = sizeof(DebugDrawPacket) + dataLength;

  uint8_t* pktData = buffer->alignedAlloc(pktLen, 16);
  DebugDrawPacket* pkt = create(pktData, debugDrawType, sourceNetworkID, sourceNodeID, sourceTagName,    frame, sourceLimbIndex, dataLength);

  return pkt;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DebugDrawPacket::serialize()
{
  // NOTE: The data block must be endian swapped correctly for comms transmission (big-endian)
  NMP::netEndianSwap(m_debugDrawType);
  NMP::netEndianSwap(m_sourceNetworkID);
  NMP::netEndianSwap(m_sourceNodeID);
  NMP::netEndianSwap(m_sourceModuleName);
  NMP::netEndianSwap(m_dataLength);
  NMP::netEndianSwap(m_sourceFrame);
  NMP::netEndianSwap(m_sourceLimbIndex);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void* DebugDrawPacket::getData()
{
  if (m_dataLength == 0)
  {
    return NULL;
  }
  return this + 1;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const void* DebugDrawPacket::getData() const
{
  if (m_dataLength == 0)
  {
    return NULL;
  }
  return this + 1;
}

}

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_DEBUGPACKETS_INL
//----------------------------------------------------------------------------------------------------------------------
