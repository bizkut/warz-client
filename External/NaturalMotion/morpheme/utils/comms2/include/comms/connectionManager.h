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
#ifndef MCOMMS_CONNECTIONMANAGER_H
#define MCOMMS_CONNECTIONMANAGER_H
//----------------------------------------------------------------------------------------------------------------------

#include "NMPlatform/NMPlatform.h"
#include "comms/networkDataBuffer.h"

namespace NMP
{
class SocketWrapper;
}

namespace MCOMMS
{

struct PacketBase;
class Connection;
class RuntimeTargetInterface;

//----------------------------------------------------------------------------------------------------------------------
class ConnectionManager
{
public:
  enum Status
  {
    kNoError,
    kDisconnected,
    kSocketError,
    kAcceptFailed,
  };

  ConnectionManager();
  ~ConnectionManager();

  Status init(int port);
  void term();

  static const uint32_t MAX_NUMBER_CONNECTIONS = 8;

  inline uint32_t getNumConnections() const { return m_numConnections; }
  inline Connection* getConnection(uint32_t i) const { NMP_ASSERT(i < m_numConnections); return m_connections[i]; }
  Connection* getMasterConnection() const;

  inline NMP::SocketWrapper* getListenCommandsSocket() const { return m_listenCommandsSocket; }
  inline NMP::SocketWrapper* getListenDataSocket() const { return m_listenDataSocket; }
  inline NMP::SocketWrapper* getListenFileServerSocket() const { return m_listenFileServerSocket; }

  Connection* addConnection(
    NMP::SocketWrapper& commandsSocketWrapper,
    NMP::SocketWrapper& dataSocketWrapper,
    NMP::SocketWrapper& fileServerSocketWrapper);

  void removeConnection(Connection* connection);
  void removeConnection(uint32_t connIndex);

  /// < buffer the packet on all the connections. use a connection mask to filter packets.
  bool broadcastPacket(PacketBase& pkt);

  void flushDataBuffers();

private:
  void destroy();

  bool initListenSocket(NMP::SocketWrapper* listenSocket, int listenPort);

  Connection* m_connections[MAX_NUMBER_CONNECTIONS];
  uint32_t    m_numConnections;

  NMP::SocketWrapper* m_listenCommandsSocket; /// < Socket to listen for new connections.
  NMP::SocketWrapper* m_listenDataSocket; /// <
  NMP::SocketWrapper* m_listenFileServerSocket; /// <

  bool canStartNewConnection();

  NMP::SocketWrapper* m_tempCommandsSocket;
  NMP::SocketWrapper* m_tempDataSocket;
  NMP::SocketWrapper* m_tempFileServerSocket;

public:
  bool update(bool& gotStep);

private:
  bool closeSocket(NMP::SocketWrapper* socketWrapper);
  void updateClientConnections();
  bool handleCommandOnConnection(Connection* connection, bool& gotMasterStep);

  static const uint32_t   COMMANDS_PACKET_INITIAL_BUFFER_SIZE = 64 * 1024;
  NetworkDataBuffer       m_commandsBuffer;

  bool shouldStep() const;
private:
  bool m_initialized;
};

} // namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_CONNECTIONMANAGER_H
//----------------------------------------------------------------------------------------------------------------------
