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
#ifndef MCOMMS_PACKET_H
#define MCOMMS_PACKET_H
//----------------------------------------------------------------------------------------------------------------------

#include "NMPlatform/NMPlatform.h"
#include "comms/MCOMMS.h"
#include "sharedDefines/mCoreDebugInterface.h"

//----------------------------------------------------------------------------------------------------------------------
namespace MCOMMS
{

class GUID;

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup morphemecomms
/// \defgroup
///
///.

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup packettypes
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
typedef MR::StringToken StringToken;
typedef uint32_t  PacketLen;
#define MCOMMS_INVALID_STRINGTOKEN   (0xFFFF)

//----------------------------------------------------------------------------------------------------------------------
enum
{
  // core packets start here
  pk_core        = 16,

#define PACKET(pkn)   pk_##pkn,
#include "comms/corePacketsMacros.inl"
#include "comms/assetManagerPacketsMacros.inl"
#include "comms/debugPacketsMacros.inl"
#undef PACKET

  // extension packets IDs start here
  //
  pk_available           = 1024,

  // maximum packet ID
  pk_top             = 65520
};


//----------------------------------------------------------------------------------------------------------------------
enum PersistentDataTypes
{
  kSceneObjectPersistentData = MR::kFirstReservedPersistentDataType
};

//----------------------------------------------------------------------------------------------------------------------
enum FrameSegmentType
{
  FRAMESEGTYPE_NETWORKINSTANCES,
  FRAMESEGTYPE_SCENEOBJECTS,
  FRAMESEGTYPE_DEBUGDRAW,
  FRAMESEGTYPE_CONNECT,   /// < Data section added to the frame on connect side when buffering the data.

  MAX_FRAMESEGTYPE = 0xFF
};

//----------------------------------------------------------------------------------------------------------------------
enum InstanceDataSectionType
{
  kCoreDataSectionType,
  kPhysicsDataSectionType,
  kEuphoriaDataSectionType,
  kControlParamsDataSection,
  kTreeNodesDataSection,

  maxInstanceDataSection = 0xFF
};

//----------------------------------------------------------------------------------------------------------------------

#pragma pack(push, 4)

//----------------------------------------------------------------------------------------------------------------------
/// variant type for float/bool/int/string token/vec3 storage
struct ValueUnion
{
  enum Type
  {
    kFloat,
    kBool,
    kInt,
    kUint,
    kBitfield32,
    kString,
    kVector3,
    kVector4,
    kAddress,
    kInvalid,
    maxValueUnionType = 0xFF
  };

  ValueUnion() : m_type((uint8_t)kInvalid), m_name(MCOMMS_INVALID_STRINGTOKEN) { m_value.i = 0; }

  explicit ValueUnion(float f) : m_type((uint8_t)kFloat), m_name(MCOMMS_INVALID_STRINGTOKEN) { m_value.f = f; }
  explicit ValueUnion(int32_t i) : m_type((uint8_t)kInt), m_name(MCOMMS_INVALID_STRINGTOKEN) { m_value.i = i; }
  explicit ValueUnion(uint32_t ui) : m_type((uint8_t)kUint), m_name(MCOMMS_INVALID_STRINGTOKEN) { m_value.u = ui; }
  explicit ValueUnion(bool b) : m_type((uint8_t)kBool), m_name(MCOMMS_INVALID_STRINGTOKEN) { m_value.b = b; }
  explicit ValueUnion(MCOMMS::StringToken s) : m_type((uint8_t)kString), m_name(MCOMMS_INVALID_STRINGTOKEN) { m_value.s = s; }
  explicit ValueUnion(const Vec3& v) : m_type((uint8_t)kVector3), m_name(MCOMMS_INVALID_STRINGTOKEN) { m_value.v = v; }
  explicit ValueUnion(float x, float y, float z) : m_type((uint8_t)kVector3), m_name(MCOMMS_INVALID_STRINGTOKEN) { m_value.v.v[0] = x; m_value.v.v[1] = y; m_value.v.v[2] = z; }
  explicit ValueUnion(float x, float y, float z, float w) : m_type((uint8_t)kVector4), m_name(MCOMMS_INVALID_STRINGTOKEN) { m_value.v.v[0] = x; m_value.v.v[1] = y; m_value.v.v[2] = z; m_value.w.v[3] = w; }

  inline void set(float f) { m_type = (uint8_t)kFloat; m_value.f = f; }
  inline void set(int32_t i) { m_type = (uint8_t)kInt; m_value.i = i; }
  inline void set(uint32_t ui) { m_type = (uint8_t)kUint; m_value.u = ui; }
  inline void set(bool b) { m_type = (uint8_t)kBool; m_value.b = b; }
  inline void set(MCOMMS::StringToken s) { m_type = (uint8_t)kString; m_value.s = s; }
  inline void set(const Vec3& v) { m_type = (uint8_t)kVector3; m_value.v = v; }
  inline void set(float x, float y, float z) { m_type = (uint8_t)kVector3; m_value.v.v[0] = x; m_value.v.v[1] = y; m_value.v.v[2] = z; }
  inline void set(float x, float y, float z, float w) { m_type = (uint8_t)kVector4; m_value.w.v[0] = x; m_value.w.v[1] = y; m_value.w.v[2] = z; m_value.w.v[3] = w; }

  inline void endianSwap()
  {
    NMP::netEndianSwap(m_name);

    if (m_type == static_cast<uint8_t>(kFloat) ||
        m_type == static_cast<uint8_t>(kInt)   ||
        m_type == static_cast<uint8_t>(kUint)   ||
        m_type == static_cast<uint8_t>(kBitfield32))
    {
      NMP::netEndianSwap(m_value.i); // 32 bits
    }
    else if (m_type == static_cast<uint8_t>(kString))
    {
      NMP::netEndianSwap(m_value.s); // 16 bits
    }
    else if (m_type == static_cast<uint8_t>(kVector3))
    {
      m_value.v.endianSwap();
    }
    else if (m_type == static_cast<uint8_t>(kVector4))
    {
      m_value.w.endianSwap();
    }
    else if (m_type == static_cast<uint8_t>(kAddress))
    {
      NMP::netEndianSwap(m_value.a); // 64 bits
    }
  }

  uint8_t                m_type;
  MCOMMS::StringToken    m_name;

  union
  {
    bool                  b;
    int32_t               i;
    uint32_t              u;
    float                 f;
    MCOMMS::StringToken   s;
    int64_t               a;
    Vec3                  v;
    Vec4                  w;
  } m_value;
};

//----------------------------------------------------------------------------------------------------------------------
/// container for standard primitive shapes
struct ShapePrimitive
{
  inline ShapePrimitive() :
    m_type((uint8_t)kInvalid)
  {
  }

  inline void endianSwap()
  {
    // swap the largest in the union
    NMP::netEndianSwap(m_data.box.m_dims[0]);
    NMP::netEndianSwap(m_data.box.m_dims[1]);
    NMP::netEndianSwap(m_data.box.m_dims[2]);
  }

  enum
  {
    kInvalid = 0,
    kShapeSphere = 1,
    kShapeBox,
    kShapeCapsule,
    kShapeCapsuleVertical,
    kShapeMeshRef
  };

  uint8_t          m_type;

  union
  {
    struct SphereShapeData
    {
      float         m_radius;
    } sphere;

    struct BoxShapeData
    {
      float         m_dims[3];
    } box;

    struct CapsuleShapeData
    {
      float         m_length;
      float         m_radius;
    } capsule;

    struct MeshRefData
    {
      uint32_t     m_polyCount;
    } mesh;

  } m_data;
};

/// The packet header must prefix all custom data packets sent;
/// m_id - set to the unique identifier for the packet being sent
/// m_length - set to the total packet size, header included
struct PacketHeader
{
  inline PacketHeader() :
    m_magicA(NM_PKT_MAGIC_A),
    m_magicB(NM_PKT_MAGIC_FD),
    m_id(0),
    m_length(0)
  {
  }

  uint8_t     m_magicA;
  uint8_t     m_magicB;
  uint16_t    m_id;
  PacketLen   m_length;

  inline void endianSwap()
  {
    NMP::netEndianSwap(m_id);
    NMP::netEndianSwap(m_length);
  }

  inline bool validMagicA() const
  {
    return (m_magicA == NM_PKT_MAGIC_A);
  }

  inline bool validMagicB() const
  {
    if (m_magicB == NM_PKT_MAGIC_FD ||
        m_magicB == NM_PKT_MAGIC_PD ||
        m_magicB == NM_PKT_MAGIC_ABROW  ||
        m_magicB == NM_PKT_MAGIC_CMDREPLY ||
        m_magicB == NM_PKT_MAGIC_GBL ||
        m_magicB == NM_PKT_MAGIC_CMD ||
        m_magicB == NM_PKT_MAGIC_CTRL)
    {
      return true;
    }
    return false;
  }

  inline bool validMagic() const
  {
    if (!validMagicA())
      return false;
    if (!validMagicB())
      return false;
    return true;
  }

  // Check if this is a frame data packet.
  inline bool isFD() const
  {
    return (m_magicB == NM_PKT_MAGIC_FD);
  }

  // Check if this is a persistent data packet.
  inline bool isPD() const
  {
    return (m_magicB == NM_PKT_MAGIC_PD);
  }

  // Check if this is a global data packet.
  inline bool isGlobal() const
  {
    return (m_magicB == NM_PKT_MAGIC_GBL);
  }

  // Check if this is an animation browser specific packet.
  inline bool isAB() const
  {
    return (m_magicB == NM_PKT_MAGIC_ABROW);
  }

  // Check if this is command reply packet.
  inline bool isCmdReply() const
  {
    return (m_magicB == NM_PKT_MAGIC_CMDREPLY);
  }

  // Check if this is a command packet.
  inline bool isCmd() const
  {
    return (m_magicB == NM_PKT_MAGIC_CMD);
  }

  // Check if this is a control packet.
  inline bool isCtrl() const
  {
    return (m_magicB == NM_PKT_MAGIC_CTRL);
  }

  // Check if this is a file server packet.
  inline bool isFServ() const
  {
    return (m_magicB == NM_PKT_MAGIC_FSERV);
  }
};

//----------------------------------------------------------------------------------------------------------------------
struct PacketBase
{
  PacketHeader    hdr;

  inline void setupHeader(uint8_t magicA, uint8_t magicB, uint16_t id, PacketLen len)
  {
    hdr.m_magicA = magicA;
    hdr.m_magicB = magicB;
    hdr.m_id = id;
    hdr.m_length = len;
  }
};

//----------------------------------------------------------------------------------------------------------------------
struct CmdPacketBase : public PacketBase
{
  inline CmdPacketBase(uint32_t requestId = 0xFFFFFFFF) :
    PacketBase(),
    m_requestId(requestId)
  {
  }

  inline void serialize()
  {
    NMP::netEndianSwap(m_requestId);
  }

  inline void deserialize()
  {
    serialize();
  }

  uint32_t m_requestId;
};

// simple macros function to fill in a packet header
// standard frame data packets
#define PK_HEADER_FD(pkn) \
  hdr.m_id = pk_##pkn; \
  hdr.m_length = sizeof(pkn##Packet);

#define PK_HEADER_CMD(pkn) \
  hdr.m_magicB = NM_PKT_MAGIC_CMD; \
  hdr.m_id = pk_##pkn; \
  hdr.m_length = sizeof(pkn##Packet);

// definition data packets
#define PK_HEADER_DEF(pkn) \
  hdr.m_magicB = NM_PKT_MAGIC_PD; \
  hdr.m_id = pk_##pkn; \
  hdr.m_length = sizeof(pkn##Packet);

// animation browser packets
#define PK_HEADER_ABROW(pkn) \
  hdr.m_magicB = NM_PKT_MAGIC_ABROW; \
  hdr.m_id = pk_##pkn; \
  hdr.m_length = sizeof(pkn##Packet);

// command reply packets
#define PK_HEADER_CMDREPLY(pkn) \
  hdr.m_magicB = NM_PKT_MAGIC_CMDREPLY; \
  hdr.m_id = pk_##pkn; \
  hdr.m_length = sizeof(pkn##Packet);

// global data packets
#define PK_HEADER_GBL(pkn) \
  hdr.m_magicB = NM_PKT_MAGIC_GBL; \
  hdr.m_id = pk_##pkn; \
  hdr.m_length = sizeof(pkn##Packet);

// control packets
#define PK_HEADER_CTRL(pkn) \
  hdr.m_magicB = NM_PKT_MAGIC_CTRL; \
  hdr.m_id = pk_##pkn; \
  hdr.m_length = sizeof(pkn##Packet);

// file server packets
#define PK_HEADER_FSERV(pkn) \
  hdr.m_magicB = NM_PKT_MAGIC_FSERV; \
  hdr.m_id = pk_##pkn; \
  hdr.m_length = sizeof(pkn##Packet);

#define mcommsBroadcastPacket(pk)              (pk).serialize(); MCOMMS::CommsServer::getInstance()->broadcastPacket(pk);
#define mcommsBufferDataPacket(pk, connection)   (pk).serialize(); connection->bufferDataPacket(pk);
#define mcommsSendDataPacket(pk, connection)   (pk).serialize(); connection->sendDataPacket(pk);
#define mcommsSerializeDataPacket(pk)   (pk).hdr.endianSwap(); (pk).serialize();
#define mcommsSerializeAndSendDataPacket(pk, connection)   (pk).hdr.endianSwap(); (pk).serialize(); connection->flushDataBuffer();

#pragma pack(pop)
} // namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_PACKET_H
//----------------------------------------------------------------------------------------------------------------------
