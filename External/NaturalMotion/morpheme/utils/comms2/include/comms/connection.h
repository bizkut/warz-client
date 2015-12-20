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
#ifndef MCOMMS_CONNECTION_H
#define MCOMMS_CONNECTION_H
//----------------------------------------------------------------------------------------------------------------------

#include "NMPlatform/NMPlatform.h"
#include "comms/mcomms.h"
#include "comms/networkDataBuffer.h"
#include <vector>

namespace NMP
{
class SocketWrapper;
class TempMemoryAllocator;
}

namespace MCOMMS
{

struct PacketBase;
struct CmdPacketBase;
struct IdentityPacket;
struct CapabilitiesPacket;
struct Channel;

//----------------------------------------------------------------------------------------------------------------------
class Connection
{
public:
  Connection();
  ~Connection();

  bool initialize(
    NMP::SocketWrapper& commandsSocketWrapper,
    NMP::SocketWrapper& dataSocketWrapper,
    NMP::SocketWrapper& fileServerSocketWrapper);

  void terminate();

  CmdPacketBase* receiveCommandPacket(NetworkDataBuffer* recvBuffer);

  inline NMP::SocketWrapper* getCommandsSocket() const { return m_commandsSocket; }
  inline NMP::SocketWrapper* getDataSocket() const { return m_dataSocket; }
  inline NMP::SocketWrapper* getFileServerSocket() const { return m_fileServerSocket; }

  inline bool isMaster() const { return m_isMaster; }
  inline void setMaster(bool master) { m_isMaster = master; }

  /// < The connection is alive only if all the three sockets are valid.
  bool isAlive() const;

  /// < Used to check if the client is actively listening for frame data during broadcasting.
  inline bool isSessionRunning() const { return m_sessionRunning; }
  inline void startSession() { m_sessionRunning = true; }
  inline void stopSession() { m_sessionRunning = false; pauseSession(false); }

  inline bool isSessionPaused() const { return m_sessionPaused; }
  inline void pauseSession(bool pause) { m_sessionPaused = pause; }

  /// < Send a ping packet on the commands socket.
  bool sendPingPacket(uint32_t pingId);

  bool sendPacket(NMP::SocketWrapper* socketWrapper, PacketBase& pkt);
  PacketBase* receivePacket(NMP::SocketWrapper* socketWrapper, char* buffer, uint32_t bufferSize);

  bool sendDataPacket(PacketBase& pkt);
  bool bufferDataPacket(PacketBase& pkt);

  /// < Send all packets currently cached on the data buffer.
  void flushDataBuffer();

  inline bool isBufferEmpty() const { return m_dataBuffer.isEmpty(); }

  uint32_t getFileSize(const char* filename);
  bool getFile(void* buffer, uint32_t bufferSize, const char* filename);

  inline NetworkDataBuffer* getDataBuffer() { return &m_dataBuffer; }
  inline NMP::TempMemoryAllocator* getTempAllocator() { return m_tempAllocator; }
  inline void setTempAllocator(NMP::TempMemoryAllocator* allocator) { m_tempAllocator = allocator; }
  inline void enableAsynchronousStepping(bool enable) { m_asynchSteppingEnabled = enable; }
  bool isAsynchronousSteppingEnabled() const;
  bool isDebuggingNetwork(InstanceID instanceID) const;
  bool startDebuggingNetwork(InstanceID instanceID);
  bool stopDebuggingNetwork(InstanceID instanceID);

private:
  uint32_t findDebuggedNetwork(InstanceID instanceID) const;
  void clearDebuggedNetworks();

  std::vector<InstanceID> m_debuggedNetworks;

  bool m_isMaster;

  NMP::SocketWrapper* m_commandsSocket;   /// < Used to receive commands from connect.
  NMP::SocketWrapper* m_fileServerSocket; /// < Used to deal with file server communications.
  NMP::SocketWrapper* m_dataSocket;       /// < Used to send data to connect.

  /// < Data packets buffer.
  static const uint32_t   DATA_PACKET_INITIAL_BUFFER_SIZE = 256 * 1024;
  NetworkDataBuffer       m_dataBuffer;
  NMP::TempMemoryAllocator* m_tempAllocator;

  /// < Fileserver packets buffer.
  static const uint32_t  FILESERVER_PACKET_INITIAL_BUFFER_SIZE = 1024;
  NetworkDataBuffer      m_fileServerBuffer;

  bool                  m_sessionRunning;
  bool                  m_sessionPaused;
  bool                  m_asynchSteppingEnabled;
};

}

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_CONNECTION_H
//----------------------------------------------------------------------------------------------------------------------
