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
#ifndef MCOMMS_PHYSICSPACKETS_H
#define MCOMMS_PHYSICSPACKETS_H
//----------------------------------------------------------------------------------------------------------------------

#include "comms/mcomms.h"
#include "comms/packet.h"

#include "sharedDefines/mPhysicsDebugInterface.h"

//----------------------------------------------------------------------------------------------------------------------
namespace MCOMMS
{
//----------------------------------------------------------------------------------------------------------------------
enum
{
  // core packets start here
  pk_physicscore  = pk_available,

#define PACKET(pkn)   pk_##pkn,
#include "comms/physicsPacketsMacros.inl"
#undef PACKET

  // the end of the physics packet IDs
  //
  pk_lastphysicscore,
};

#pragma pack(push, 4)

//----------------------------------------------------------------------------------------------------------------------
struct PhysicsPersistentDataPacket :
  public PacketBase
{
  MR::PhysicsObjectDataTypes  m_physicsType;
  uint32_t                    m_dataLength;

  static inline PhysicsPersistentDataPacket* create(
    NetworkDataBuffer*          buffer,
    MR::PhysicsObjectDataTypes  physicsType,
    uint32_t                    dataLength);

  inline void serialize();
  inline void deserialize();

  inline void *getData();
};

//----------------------------------------------------------------------------------------------------------------------
struct PhysicsFrameDataPacket :
  public PacketBase
{
  MR::PhysicsObjectDataTypes  m_physicsType;
  uint32_t                    m_dataLength;

  static inline PhysicsFrameDataPacket* create(
    NetworkDataBuffer*          buffer,
    MR::PhysicsObjectDataTypes  physicsType,
    uint32_t                    dataLength);

  inline void serialize();
  inline void deserialize();

  inline void *getData();
};

#pragma pack(pop)

} // namespace MCOMMS

#include "comms/physicsPackets.inl"

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_PHYSICSPACKETS_H
//----------------------------------------------------------------------------------------------------------------------
