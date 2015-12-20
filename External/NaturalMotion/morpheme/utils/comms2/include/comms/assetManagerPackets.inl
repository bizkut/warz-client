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
#ifndef MCOMMS_COREPACKETS_INL
#define MCOMMS_COREPACKETS_INL
//----------------------------------------------------------------------------------------------------------------------

namespace MCOMMS
{
//----------------------------------------------------------------------------------------------------------------------
/// SetAnimBrowserAnimCmdPacket
//----------------------------------------------------------------------------------------------------------------------
inline SetAnimBrowserAnimCmdPacket::SetAnimBrowserAnimCmdPacket()
: CmdPacketBase()
{
  PK_HEADER_CMD(SetAnimBrowserAnimCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SetAnimBrowserAnimCmdPacket::serialize()
{
  CmdPacketBase::serialize();
}

//----------------------------------------------------------------------------------------------------------------------
inline void SetAnimBrowserAnimCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
/// SetAnimBrowserNetworkCmdPacket
//----------------------------------------------------------------------------------------------------------------------
inline SetAnimBrowserNetworkCmdPacket::SetAnimBrowserNetworkCmdPacket()
: CmdPacketBase()
{
  PK_HEADER_CMD(SetAnimBrowserNetworkCmd);

  m_animBrowserNetwork[0] = '\0';
  m_animationSourceNodeName[0] = '\0';
  m_retargetingSourceNodeName[0] = '\0';
  m_rescalingSourceNodeName[0] = '\0';
}

//----------------------------------------------------------------------------------------------------------------------
inline void SetAnimBrowserNetworkCmdPacket::serialize()
{
  CmdPacketBase::serialize();
}

//----------------------------------------------------------------------------------------------------------------------
inline void SetAnimBrowserNetworkCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
/// LoadAnimBrowserDataCmdPacket
//----------------------------------------------------------------------------------------------------------------------
inline LoadAnimBrowserDataCmdPacket::LoadAnimBrowserDataCmdPacket()
: CmdPacketBase()
{
  PK_HEADER_CMD(LoadAnimBrowserDataCmd);

  m_compressionType[0] = '\0';
}

//----------------------------------------------------------------------------------------------------------------------
inline void LoadAnimBrowserDataCmdPacket::serialize()
{
  CmdPacketBase::serialize();
}

//----------------------------------------------------------------------------------------------------------------------
inline void LoadAnimBrowserDataCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
/// AnimBrowserDataLoadedPacket
//----------------------------------------------------------------------------------------------------------------------
inline AnimBrowserDataLoadedPacket::AnimBrowserDataLoadedPacket()
: PacketBase()
{
  PK_HEADER_ABROW(AnimBrowserDataLoaded);
}

//----------------------------------------------------------------------------------------------------------------------
inline void AnimBrowserDataLoadedPacket::serialize()
{
  NMP::netEndianSwap(m_duration);
}

//----------------------------------------------------------------------------------------------------------------------
inline void AnimBrowserDataLoadedPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
/// SetAnimBrowserTimeCmdPacket
//----------------------------------------------------------------------------------------------------------------------
inline SetAnimBrowserTimeCmdPacket::SetAnimBrowserTimeCmdPacket()
: CmdPacketBase()
{
  PK_HEADER_CMD(SetAnimBrowserTimeCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SetAnimBrowserTimeCmdPacket::serialize()
{
  CmdPacketBase::serialize();

  NMP::netEndianSwap(m_time);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SetAnimBrowserTimeCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
/// BeginAnimBrowserPacket
//----------------------------------------------------------------------------------------------------------------------
inline BeginAnimBrowserPacket::BeginAnimBrowserPacket(float time)
: m_time(time)
{
  PK_HEADER_ABROW(BeginAnimBrowser);
}

//----------------------------------------------------------------------------------------------------------------------
inline void BeginAnimBrowserPacket::deserialize()
{
  NMP::netEndianSwap(m_time);
}

//----------------------------------------------------------------------------------------------------------------------
inline void BeginAnimBrowserPacket::serialize()
{
  NMP::netEndianSwap(m_time);
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief
//----------------------------------------------------------------------------------------------------------------------
inline AnimBrowserSourceDataPacket* AnimBrowserSourceDataPacket::create(
  NetworkDataBuffer* dataBuffer,
  uint32_t dataLength,
  AnimBrowserSourceDataPacket::SourceTypes sourceType)
{
  uint32_t packetLength = sizeof(AnimBrowserSourceDataPacket) + dataLength;

  AnimBrowserSourceDataPacket* packet = dataBuffer->reserveAlignedMemory<AnimBrowserSourceDataPacket*>(packetLength, 16);

  packet->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_ABROW, pk_AnimBrowserSourceData, packetLength);

  packet->m_sourceType = sourceType;
  packet->m_dataLength = dataLength;

  return packet;
}

//----------------------------------------------------------------------------------------------------------------------
inline void AnimBrowserSourceDataPacket::deserialize()
{
  NMP::netEndianSwap(m_sourceType);
  NMP::netEndianSwap(m_dataLength);
  MR::AttribTransformBufferOutputData* data = getData();
  MR::AttribTransformBufferOutputData::deserialiseTx(data);
}

//----------------------------------------------------------------------------------------------------------------------
inline void AnimBrowserSourceDataPacket::serialize()
{
  NMP::netEndianSwap(m_sourceType);
  NMP::netEndianSwap(m_dataLength);

  // this doesn't serialize the m_attribTransformBuffer as that is done by the attrib data it represents.
}

//----------------------------------------------------------------------------------------------------------------------
inline MR::AttribTransformBufferOutputData* AnimBrowserSourceDataPacket::getData()
{
  MR::AttribTransformBufferOutputData* buffer = 0;
  if (m_dataLength > 0)
  {
    void * memLengthAddress = &(this->m_dataLength);
    AnimBrowserSourceDataPacket * nextPacket = (this + 1); 
    if(memLengthAddress < (void *) nextPacket)
    {
      size_t x = sizeof(AnimBrowserSourceDataPacket);
      char* expectedAddress = ((char*)this) + x;
      expectedAddress++;
    }
    buffer = reinterpret_cast<MR::AttribTransformBufferOutputData*>(nextPacket);
  }
  return buffer;
}

//----------------------------------------------------------------------------------------------------------------------
// EndAnimBrowserPacket
//----------------------------------------------------------------------------------------------------------------------
inline EndAnimBrowserPacket::EndAnimBrowserPacket()
{
  PK_HEADER_ABROW(EndAnimBrowser);
}

//----------------------------------------------------------------------------------------------------------------------
inline void EndAnimBrowserPacket::deserialize()
{
}

//----------------------------------------------------------------------------------------------------------------------
inline void EndAnimBrowserPacket::serialize()
{
}

//----------------------------------------------------------------------------------------------------------------------
// DownloadAnimBrowserEventDetectionDataCmdPacket
//----------------------------------------------------------------------------------------------------------------------
inline DownloadAnimBrowserEventDetectionDataCmdPacket::DownloadAnimBrowserEventDetectionDataCmdPacket()
: CmdPacketBase()
{
  PK_HEADER_CMD(DownloadAnimBrowserEventDetectionDataCmd);
}

//----------------------------------------------------------------------------------------------------------------------
inline void DownloadAnimBrowserEventDetectionDataCmdPacket::serialize()
{
  CmdPacketBase::serialize();

  NMP::netEndianSwap(m_minTime);
  NMP::netEndianSwap(m_numFrames);
  NMP::netEndianSwap(m_dt);
}

//----------------------------------------------------------------------------------------------------------------------
inline void DownloadAnimBrowserEventDetectionDataCmdPacket::deserialize()
{
  serialize();
}

//----------------------------------------------------------------------------------------------------------------------
// AnimBrowserEventDetectionDataPacket
//----------------------------------------------------------------------------------------------------------------------
inline AnimBrowserEventDetectionDataPacket* AnimBrowserEventDetectionDataPacket::create(
  NetworkDataBuffer* dataBuffer,
  uint32_t numFrames,
  uint32_t transformDataSize)
{
  PacketLen packetLength = sizeof(AnimBrowserEventDetectionDataPacket) + (numFrames * transformDataSize);

  AnimBrowserEventDetectionDataPacket* packet =
    dataBuffer->reserveAlignedMemory<AnimBrowserEventDetectionDataPacket*>(packetLength, NMP_VECTOR_ALIGNMENT);

  packet->setupHeader(NM_PKT_MAGIC_A, NM_PKT_MAGIC_ABROW, pk_AnimBrowserEventDetectionData, packetLength);

  packet->m_numFrames = numFrames;
  packet->m_transformDataSize = transformDataSize;

  return packet;
}

//----------------------------------------------------------------------------------------------------------------------
inline void AnimBrowserEventDetectionDataPacket::deserialize()
{
  NMP::netEndianSwap(m_numFrames);
  NMP::netEndianSwap(m_transformDataSize);

  for (uint32_t i = 0; i != m_numFrames; ++i)
  {
    MR::AttribTransformBufferOutputData* transformData = getFrameTransformData(i);
    MR::AttribTransformBufferOutputData::deserialiseTx(transformData);
  }
}

//----------------------------------------------------------------------------------------------------------------------
inline void AnimBrowserEventDetectionDataPacket::serialize()
{
  NMP::netEndianSwap(m_numFrames);
  NMP::netEndianSwap(m_transformDataSize);
}

//----------------------------------------------------------------------------------------------------------------------
inline MR::AttribTransformBufferOutputData* AnimBrowserEventDetectionDataPacket::getFrameTransformData(
  uint32_t frameIndex)
{
  NMP_ASSERT(frameIndex < m_numFrames);
  if (m_numFrames > 0 && m_transformDataSize > 0)
  {
    // find the start of the transform data array just after this packet
    //
    char* transformDataBegin = (char*)(this + 1);

    // now find the specific array element from that array
    //
    MR::AttribTransformBufferOutputData* transformData =
      (MR::AttribTransformBufferOutputData*)(transformDataBegin + (frameIndex * m_transformDataSize));

    return transformData;
  }

  return 0;
}

} // namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_COREPACKETS_INL
//----------------------------------------------------------------------------------------------------------------------
