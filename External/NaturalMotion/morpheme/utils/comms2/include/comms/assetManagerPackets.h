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
#ifndef MCOMMS_ASSETMANAGERPACKETS_H
#define MCOMMS_ASSETMANAGERPACKETS_H
//----------------------------------------------------------------------------------------------------------------------
#include "comms/mcomms.h"
#include "comms/networkDataBuffer.h"
#include "comms/packet.h"

#include "NMPlatform/NMMathUtils.h"
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMBuffer.h"


#include "sharedDefines/mSharedDefines.h"
#include "sharedDefines/mCoreDebugInterface.h"
#include "../../../SDK/sharedDefines/mPhysicsDebugInterface.h"
//----------------------------------------------------------------------------------------------------------------------


namespace MCOMMS
{

#pragma pack(push, 4)

//----------------------------------------------------------------------------------------------------------------------
/// \brief Packet sent to set the currently previewed animation file.
//----------------------------------------------------------------------------------------------------------------------
struct SetAnimBrowserAnimCmdPacket : public CmdPacketBase
{
  inline SetAnimBrowserAnimCmdPacket();

  inline void deserialize();
  inline void serialize();

  char m_animBrowserAnim[128];
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Packet sent to set the currently previewed network file.
//----------------------------------------------------------------------------------------------------------------------
struct SetAnimBrowserNetworkCmdPacket : public CmdPacketBase
{
  inline SetAnimBrowserNetworkCmdPacket();

  inline void deserialize();
  inline void serialize();

  char m_animBrowserNetwork[128];
  char m_animationSourceNodeName[128];
  char m_retargetingSourceNodeName[128];
  char m_rescalingSourceNodeName[128];
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Packet sent to load the currently set animation and network files.
//----------------------------------------------------------------------------------------------------------------------
struct LoadAnimBrowserDataCmdPacket : public CmdPacketBase
{
  inline LoadAnimBrowserDataCmdPacket();

  inline void deserialize();
  inline void serialize();

  char m_compressionType[32];
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Packet sent in response to LoadAnimBrowserDataCmdPacket indicating the duration of the loaded
/// animation data.
//----------------------------------------------------------------------------------------------------------------------
struct AnimBrowserDataLoadedPacket : public PacketBase
{
  inline AnimBrowserDataLoadedPacket();

  inline void deserialize();
  inline void serialize();

  float m_duration;
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Packet to set the currently previewed time.
//----------------------------------------------------------------------------------------------------------------------
struct SetAnimBrowserTimeCmdPacket : public CmdPacketBase
{
  inline SetAnimBrowserTimeCmdPacket();

  inline void deserialize();
  inline void serialize();

  float m_time;
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief
//----------------------------------------------------------------------------------------------------------------------
struct BeginAnimBrowserPacket : public PacketBase
{
  inline BeginAnimBrowserPacket(float time);

  inline void deserialize();
  inline void serialize();

  float m_time;
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief
//----------------------------------------------------------------------------------------------------------------------
struct AnimBrowserSourceDataPacket : public PacketBase
{
  enum SourceTypes
  {
    kAnimationSourceType = 0,
    kRetargetSourceType,
    kRescaleSourceType,
    kNumSourceTypes
  };

  inline static AnimBrowserSourceDataPacket *create(
    NetworkDataBuffer*  dataBuffer,
    uint32_t            dataLength,
    SourceTypes         sourceType);

  inline void deserialize();
  inline void serialize();

  MR::AttribTransformBufferOutputData* getData();

  uint32_t m_sourceType;
  uint32_t m_dataLength;     ///< Size of the transform buffer data this packet holds in bytes.
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief
//----------------------------------------------------------------------------------------------------------------------
struct EndAnimBrowserPacket : public PacketBase
{
  inline EndAnimBrowserPacket();

  inline void deserialize();
  inline void serialize();
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Download the animation take data required for detecting events.
//----------------------------------------------------------------------------------------------------------------------
struct DownloadAnimBrowserEventDetectionDataCmdPacket : CmdPacketBase
{
  inline DownloadAnimBrowserEventDetectionDataCmdPacket();

  inline void deserialize();
  inline void serialize();

  float     m_minTime;
  uint32_t  m_numFrames;
  float     m_dt;
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Contains all the animation take data required for detecting events in morpheme connect.
//----------------------------------------------------------------------------------------------------------------------
struct AnimBrowserEventDetectionDataPacket : public PacketBase
{
  static inline AnimBrowserEventDetectionDataPacket* create(
    NetworkDataBuffer* dataBuffer,
    uint32_t numFrames,
    uint32_t transformDataSize);

  inline void deserialize();
  inline void serialize();

  inline MR::AttribTransformBufferOutputData* getFrameTransformData(uint32_t frameIndex);

  uint32_t m_numFrames;
  uint32_t m_transformDataSize;
};

#pragma pack(pop)

} // namespace MCOMMS

#include "comms/assetManagerPackets.inl"

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_ASSETMANAGERPACKETS_H
//----------------------------------------------------------------------------------------------------------------------
