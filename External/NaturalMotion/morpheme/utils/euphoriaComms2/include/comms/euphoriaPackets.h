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
#ifndef MCOMMS_EUPHORIAPACKETS_H
#define MCOMMS_EUPHORIAPACKETS_H
//----------------------------------------------------------------------------------------------------------------------

#include "comms/mcomms.h"
#include "comms/physicsPackets.h"

#include "sharedDefines/mEuphoriaDebugInterface.h"

//----------------------------------------------------------------------------------------------------------------------
namespace MCOMMS
{
//----------------------------------------------------------------------------------------------------------------------
enum
{
  // euphoria core packets start here
  pk_euphoriacore  = pk_lastphysicscore,

#define PACKET(pkn)   pk_##pkn,
#include "comms/euphoriaPacketsMacros.inl"
#undef PACKET

  // the end of the euphoria packet IDs
  //
  pk_lasteuphoriacore,
};

#pragma pack(push, 4)

//----------------------------------------------------------------------------------------------------------------------
struct EuphoriaPersistentDataPacket :
  public PacketBase
{
  ER::EuphoriaObjectDataTypes m_euphoriaType;
  uint32_t                    m_dataLength;

  static inline EuphoriaPersistentDataPacket* create(
    NetworkDataBuffer*          buffer,
    ER::EuphoriaObjectDataTypes euphoriaType,
    uint32_t                    dataLength);

  inline void serialize();
  inline void deserialize();

  inline void* getData();
};

//----------------------------------------------------------------------------------------------------------------------
struct EuphoriaFrameDataPacket :
  public PacketBase
{
  ER::EuphoriaObjectDataTypes m_euphoriaType;
  uint32_t                    m_dataLength;

  static inline EuphoriaFrameDataPacket* create(
    NetworkDataBuffer*          buffer,
    ER::EuphoriaObjectDataTypes euphoriaType,
    uint32_t                    dataLength);

  /// \brief Serializes this packet for comms transmission, does not serialise the contained data.
  inline void serialize();
  /// \brief Deserializes this packet and its contained data after comms transmission.
  inline void deserialize();

  inline void* getData();
};

//----------------------------------------------------------------------------------------------------------------------
struct EnableModuleDebugCmdPacket :
  public CmdPacketBase
{
  InstanceID  m_instanceID;
  uint32_t    m_moduleIndex;
  bool        m_enableDebug;

  inline EnableModuleDebugCmdPacket();

  inline void deserialize();
  inline void serialize();

protected:
  inline void endianSwap();
};

//----------------------------------------------------------------------------------------------------------------------
struct EuphoriaDebugControlCmdPacket :
  public CmdPacketBase
{
  InstanceID                        m_instanceID;
  ER::EuphoriaDebugControlFrameData m_controlData;

  inline EuphoriaDebugControlCmdPacket();

  inline void deserialize();

  inline void serialize(
    InstanceID instanceID,
    uint8_t    controlIndex,
    uint8_t    limbIndex,
    bool       value);

  inline void serialize(
    InstanceID instanceID,
    uint8_t    controlIndex,
    uint8_t    limbIndex,
    int32_t    value);

  inline void serialize(
    InstanceID instanceID,
    uint8_t    controlIndex,
    uint8_t    limbIndex,
    uint32_t   value);

  inline void serialize(
    InstanceID instanceID,
    uint8_t    controlIndex,
    uint8_t    limbIndex,
    float      value);
};

#pragma pack(pop)

} // namespace MCOMMS

#include "comms/euphoriaPackets.inl"

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_EUPHORIAPACKETS_H
//----------------------------------------------------------------------------------------------------------------------
