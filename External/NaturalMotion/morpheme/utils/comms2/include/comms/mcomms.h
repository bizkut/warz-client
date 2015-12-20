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
#ifndef MCOMMS_MCOMMS_H
#define MCOMMS_MCOMMS_H
//----------------------------------------------------------------------------------------------------------------------

#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMVector.h"

#include "../../../SDK/sharedDefines/mSharedDefines.h"

#define NM_STRINGCACHE_CHUNK     (1024 * 32)

// Define this to make the string cache heap manager spew out the number of allocs, footprint, heap stats, etc on dtor
#define NM_CHUNKMEM_LOG_DTORx

// Define this to improve the level of verbosity of the comms
#define MCOMMS_VERBOSEx

#define NM_PROTOCOL_VER      (6)

#define NM_PKT_MAGIC_A        (0xFE) ///< indicates a packet is a morpheme comms packet, Packet::m_magicA should always be set to this.
#define NM_PKT_MAGIC_FD       (0xB0) ///< indicates a packet contains frame data, Packet::m_magicB can be set to this
#define NM_PKT_MAGIC_PD       (0xB1) ///< indicates a packet contains persistent data, Packet::m_magicB can be set to this
#define NM_PKT_MAGIC_ABROW    (0xB2) ///< indicates a packet contains asset manager data, Packet::m_magicB can be set to this
#define NM_PKT_MAGIC_CMDREPLY (0xB3) ///< indicates a packet is a command reply, Packet::m_magicB can be set to this
#define NM_PKT_MAGIC_GBL      (0xB4) ///< indicates a packet contains global data (ie strings), Packet::m_magicB can be set to this
#define NM_PKT_MAGIC_CMD      (0xB5) ///< indicates a packet is a command, Packet::m_magicB can be set to this
#define NM_PKT_MAGIC_CTRL     (0xB6)
#define NM_PKT_MAGIC_FSERV    (0xB7) ///< indicates a packet is a file server request, Packet::m_magicB can be set to this

#define NM_PKT_MAGIC_GAP      (0xB9) // Indicate gap to keep memory aligned.

namespace NMP
{
class SocketWrapper;
}

namespace MCOMMS
{

class RuntimeTargetInterface;

typedef uint32_t CommsID32;   // Generic 4 bytes ID, usable for InstanceIDs, etc...
typedef uint16_t CommsID16;   // Generic 2 bytes ID, NodeIDs, etc...
typedef uint32_t InstanceID;
typedef uint32_t NetworkMessageID;
typedef uint32_t CmdRequestID;

typedef MR::NodeID commsNodeID;
typedef uint32_t commsNodeOutputDataType;

static const InstanceID INVALID_INSTANCE_ID = 0xFFFFFFFF;

//----------------------------------------------------------------------------------------------------------------------
struct DebugLine
{
  NMP::Vector3 m_begin;
  NMP::Vector3 m_end;
  NMP::Vector3 m_colour;
};

//----------------------------------------------------------------------------------------------------------------------
struct DebugTriangle
{
  NMP::Vector3 m_vertices[3];
  NMP::Vector3 m_colour;
};

//----------------------------------------------------------------------------------------------------------------------
struct DebugShape
{
  enum ShapeType
  {
    kPolygonShape,
    kSphereShape,
    kCapsuleShape,
    kBoxShape,
  };

  NMP::Vector3 m_translation;
  NMP::Quat    m_rotation;
  uint32_t     m_type;
};

//----------------------------------------------------------------------------------------------------------------------
class GUID
{
public:
  GUID(const uint8_t val[16])
  {
    for (uint32_t i = 0; i < 16; ++i)
    {
      value[i] = val[i];
    }
  }

  GUID()
  {
    for (uint32_t i = 0; i < 16; ++i)
    {
      value[i] = 0;
    }
  }

  uint8_t value[16];

  bool operator==(const GUID& other) const;
  bool operator<(const GUID& other) const;

  void toStringGUID(char* stringGUID) const;
  uint32_t hashStringGuid(const char* strGuid) const;
};

struct PacketBase;

//----------------------------------------------------------------------------------------------------------------------
uint32_t sendBuffer(NMP::SocketWrapper* socket, void* buffer, uint32_t bufferSize);

//----------------------------------------------------------------------------------------------------------------------
uint32_t recvBuffer(NMP::SocketWrapper* socket, void* buffer, uint32_t bufferSize);

//----------------------------------------------------------------------------------------------------------------------
void logPacket(PacketBase* pkt);

/// \brief For debugging.  NOT re-entrant.
/// \return Pointer to a string representing the given guid.
const char* guidToString(const GUID* guid);

//----------------------------------------------------------------------------------------------------------------------
RuntimeTargetInterface* getRuntimeTarget();

//----------------------------------------------------------------------------------------------------------------------
namespace utils
{
/// \ingroup morphemecomms
///
/// hash a string into a u32
/// also returns length of the string, seeing as it iterates over it anyway
///
inline int32_t hashString(const char* const str, uint32_t& strLength)
{
  const char* strP = str;
  uint32_t hash = 0;
  int32_t c = *strP;
  strLength = 0;

  while (c)
  {
    hash = c + (hash << 6) + (hash << 16) - hash;

    ++ strP;
    ++ strLength;
    c = *strP;
  }

  return hash;
}

/// \ingroup morphemecomms
///
/// hash a wide-char string into a u32
/// also returns length of the string, seeing as it iterates over it anyway
///
inline int32_t hashWString(const wchar_t* const str, uint32_t& strLength)
{
  const wchar_t* strP = str;
  uint32_t hash = 0;
  int32_t c = *strP;
  strLength = 0;

  while (c)
  {
    hash = c + (hash << 6) + (hash << 16) - hash;

    ++ strP;
    ++ strLength;
    c = *strP;
  }

  return hash;
}

} // namespace utils

//----------------------------------------------------------------------------------------------------------------------
/// generic 3-float vector class
struct Vec3
{
  float   v[3];

  inline void endianSwap()
  {
    NMP::netEndianSwap(v[0]);
    NMP::netEndianSwap(v[1]);
    NMP::netEndianSwap(v[2]);
  }
};

//----------------------------------------------------------------------------------------------------------------------
/// generic 4-float vector class
struct Vec4
{
  float   v[4];

  inline void endianSwap()
  {
    NMP::netEndianSwap(v[0]);
    NMP::netEndianSwap(v[1]);
    NMP::netEndianSwap(v[2]);
    NMP::netEndianSwap(v[3]);
  }
};

//----------------------------------------------------------------------------------------------------------------------
/// full 3x4 matrix representation
struct Mat34
{
  Vec3    m_row[4];

  inline void endianSwap()
  {
    m_row[0].endianSwap();
    m_row[1].endianSwap();
    m_row[2].endianSwap();
    m_row[3].endianSwap();
  }
};

//----------------------------------------------------------------------------------------------------------------------
Vec3 vec3fromVector3(const NMP::Vector3& vec);

//----------------------------------------------------------------------------------------------------------------------
Vec4 vec4fromQuat(const NMP::Quat& quat);

//----------------------------------------------------------------------------------------------------------------------
Mat34 mat34fromMatrix34(const NMP::Matrix34& tm);

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 vector3fromVec3(const Vec3& vec);

//----------------------------------------------------------------------------------------------------------------------
NMP::Quat quatfromVec4(const Vec4& quat);

//----------------------------------------------------------------------------------------------------------------------
NMP::Matrix34 matrix34fromMat34(const Mat34& tm);

//----------------------------------------------------------------------------------------------------------------------
enum SocketStatus
{
  SOCKET_ERROR_OCCURRED,
  SOCKET_NO_MESSAGES,
  SOCKET_HAS_MESSAGES,
  SOCKET_NOT_CONNECTED,
};

//----------------------------------------------------------------------------------------------------------------------
enum NetworkError
{
  NETWORK_NO_ERROR,
  NETWORK_TIMEOUT_ELAPSED,
  NETWORK_SOCKET_ACCEPT_FAILED,
  NETWORK_SOCKET_CREATION_FAILED,
  NETWORK_SOCKET_BIND_FAILED,
  NETWORK_SOCKET_SEND_FAILED,
  NETWORK_SOCKET_RECEIVE_FAILED,
};

//----------------------------------------------------------------------------------------------------------------------
SocketStatus checkSocketActivity(NMP::SocketWrapper* socketWrapper);

//----------------------------------------------------------------------------------------------------------------------
struct Channel
{
  uint16_t m_channelID;
  uint16_t m_runtimeID;
};

} // namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_MCOMMS_H
//----------------------------------------------------------------------------------------------------------------------
