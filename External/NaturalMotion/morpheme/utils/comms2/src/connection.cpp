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
#include "comms/MCOMMS.h"
#include "comms/connection.h"
#include "comms/packet.h"
#include "comms/corePackets.h"
#include "comms/commsServer.h"
#include "comms/runtimeTargetInterface.h"

#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMSocketWrapper.h"
#include "NMPlatform/NMFastHeapAllocator.h"

//----------------------------------------------------------------------------------------------------------------------
namespace MCOMMS
{

//----------------------------------------------------------------------------------------------------------------------
Connection::Connection() :
  m_isMaster(false),
  m_sessionRunning(false),
  m_sessionPaused(false),
  m_asynchSteppingEnabled(false)
{
  m_commandsSocket = new NMP::SocketWrapper();
  m_commandsSocket->setInvalid();

  m_dataSocket = new NMP::SocketWrapper();
  m_dataSocket->setInvalid();

  m_fileServerSocket = new NMP::SocketWrapper();
  m_fileServerSocket->setInvalid();
}

//----------------------------------------------------------------------------------------------------------------------
Connection::~Connection()
{
  delete m_commandsSocket;
  delete m_dataSocket;
  delete m_fileServerSocket;
}

//----------------------------------------------------------------------------------------------------------------------
PacketBase* Connection::receivePacket(NMP::SocketWrapper* socketWrapper, char* buffer, uint32_t bufferSize)
{
  // Try to receive a packet header. Directly in the buffer.
  uint32_t readBytes = MCOMMS::recvBuffer(socketWrapper, buffer, sizeof(PacketHeader));

  if (readBytes != sizeof(PacketHeader))
  {
    // Not a packet.
    NMP_MSG("MorphemeComms: read %d bytes only while expecting %d.", readBytes, sizeof(PacketHeader));
    return 0;
  }

  // is this a valid header packet?
  PacketHeader* pktHdr = reinterpret_cast<PacketHeader*>(buffer);
  if (pktHdr->m_magicA == NM_PKT_MAGIC_A)
  {
    pktHdr->endianSwap();

    uint32_t bytesToRead = pktHdr->m_length - readBytes;
    if (bytesToRead > bufferSize - readBytes)
    {
      // The buffer is not large enough.
      NMP_MSG("MorphemeComms: trying to receive %d bytes on a buffer of %d length.", pktHdr->m_length, bufferSize);
      return 0;
    }

    // Try to read the rest of the packet.
    readBytes = MCOMMS::recvBuffer(socketWrapper, &buffer[readBytes], bytesToRead);

    if (readBytes != bytesToRead)
    {
      // not enough data to complete the packet.
      // The buffer is not large enough.
      NMP_MSG("MorphemeComms: read %d bytes only while expecting %d.", readBytes, bytesToRead);
      return 0;
    }

    // We have a proper complete packet now.
    PacketBase* pkt = reinterpret_cast<PacketBase*>(recvBuffer);
    return pkt;
  }

  // Getting here means that no packet was found.
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool Connection::isAsynchronousSteppingEnabled() const
{
  if (m_asynchSteppingEnabled)
  {
    return true;
  }

  RuntimeTargetInterface* target = getRuntimeTarget();
  NMP_ASSERT(target);
  PlaybackManagementInterface* playbackControl = target->getPlaybackManager();
  if (playbackControl && !playbackControl->canStepInSynch())
  {
    // Force it to be asynchronous when the rtt claims that it can step in synch.
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Connection::findDebuggedNetwork(InstanceID instanceID) const
{
  for (uint32_t i = 0, s = (uint32_t)m_debuggedNetworks.size(); i < s; ++i)
  {
    if (m_debuggedNetworks[i] == instanceID)
    {
      return i;
    }
  }

  return (uint32_t) - 1;
}

//----------------------------------------------------------------------------------------------------------------------
bool Connection::isDebuggingNetwork(InstanceID instanceID) const
{
  return findDebuggedNetwork(instanceID) != (uint32_t) - 1;
}

//----------------------------------------------------------------------------------------------------------------------
bool Connection::startDebuggingNetwork(InstanceID instanceID)
{
  if (!isDebuggingNetwork(instanceID))
  {
    m_debuggedNetworks.push_back(instanceID);
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Connection::stopDebuggingNetwork(InstanceID instanceID)
{
  uint32_t pos = findDebuggedNetwork(instanceID);
  if (pos != (uint32_t) - 1)
  {
    m_debuggedNetworks.erase(m_debuggedNetworks.begin() + pos);
    return true;
  }

  NMP_MSG("MorphemeComms: %d network is not currently being debugged.\n", instanceID);
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void Connection::clearDebuggedNetworks()
{
  m_debuggedNetworks.clear();
}

//----------------------------------------------------------------------------------------------------------------------
// Try to fill the buffer with a packet
CmdPacketBase* Connection::receiveCommandPacket(NetworkDataBuffer* recvBuffer)
{
  PacketHeader pktHeader;
  uint32_t readBytes = MCOMMS::recvBuffer(m_commandsSocket, &pktHeader, sizeof(PacketHeader));

  if (readBytes != sizeof(PacketHeader))
  {
    // Not a packet.
    NMP_MSG("MorphemeComms: read %d bytes only while expecting %d.", readBytes, sizeof(PacketHeader));
    return 0;
  }

  // is this a valid header packet?
  PacketHeader pktHdrCopy = pktHeader;
  pktHdrCopy.endianSwap();

  if (pktHdrCopy.m_magicA == NM_PKT_MAGIC_A)
  {
    // Reserve memory for the whole packet.
    // Note that this should be done in one shot for the whole packet, because the buffer can re-allocate so
    // it can be tricky to take care of this when storing pointers to part of the packet.
    uint8_t* packetBuffer = recvBuffer->reserveMemory<uint8_t*>(pktHdrCopy.m_length);
    NMP_ASSERT(packetBuffer);

    // Copy the header in the reserved buffer.
    memcpy(packetBuffer, &pktHeader, sizeof(PacketHeader));

    // Try to read the rest of the packet.
    uint32_t bytesToRead = pktHdrCopy.m_length - readBytes;
    readBytes = MCOMMS::recvBuffer(m_commandsSocket, packetBuffer + sizeof(PacketHeader), bytesToRead);

    if (readBytes != bytesToRead)
    {
      // not enough data to complete the packet.
      NMP_MSG("MorphemeComms: read %d bytes only while expecting %d.", readBytes, bytesToRead);
      return 0;
    }

    // We have a proper packet in the buffer.
    CmdPacketBase* pkt = reinterpret_cast<CmdPacketBase*>(packetBuffer);
    return pkt;
  }

  // Getting here means that no packet was found.
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool Connection::initialize(
  NMP::SocketWrapper& commandsSocketWrapper,
  NMP::SocketWrapper& dataSocketWrapper,
  NMP::SocketWrapper& fileServerSocketWrapper)
{
  NMP_ASSERT(commandsSocketWrapper.isValid() == true);
  NMP_ASSERT(dataSocketWrapper.isValid() == true);
  NMP_ASSERT(fileServerSocketWrapper.isValid() == true);

  m_commandsSocket->setSocket(commandsSocketWrapper.getSocket());
  m_dataSocket->setSocket(dataSocketWrapper.getSocket());
  m_fileServerSocket->setSocket(fileServerSocketWrapper.getSocket());

  setMaster(false);
  m_sessionPaused = false;
  m_sessionRunning = false;
  m_asynchSteppingEnabled = false;

  m_dataBuffer.init(m_dataSocket, DATA_PACKET_INITIAL_BUFFER_SIZE, NMP_NATURAL_TYPE_ALIGNMENT);
  m_fileServerBuffer.init(m_fileServerSocket, FILESERVER_PACKET_INITIAL_BUFFER_SIZE, NMP_NATURAL_TYPE_ALIGNMENT);

  static const size_t tempAllocatorSize = 64 * 1024;
  NMP::Memory::Format allocatorFormat = NMP::FastHeapAllocator::getMemoryRequirements(tempAllocatorSize, NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Resource resource = NMPMemoryAllocateFromFormat(allocatorFormat);
  NMP_ASSERT(resource.ptr);
  m_tempAllocator = NMP::FastHeapAllocator::init(resource, tempAllocatorSize, NMP_VECTOR_ALIGNMENT);

  clearDebuggedNetworks();
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void Connection::terminate()
{
  if (m_commandsSocket->isValid())
  {
    NMP::shutdown(m_commandsSocket->getSocket(), 0x02);
    NMP::closesocket(m_commandsSocket->getSocket());
    m_commandsSocket->setInvalid();
  }

  if (m_dataSocket->isValid())
  {
    NMP::shutdown(m_dataSocket->getSocket(), 0x02);
    NMP::closesocket(m_dataSocket->getSocket());
    m_dataSocket->setInvalid();
  }

  if (m_fileServerSocket->isValid())
  {
    NMP::shutdown(m_fileServerSocket->getSocket(), 0x02);
    NMP::closesocket(m_fileServerSocket->getSocket());
    m_fileServerSocket->setInvalid();
  }

  NMP::Memory::memFree(m_tempAllocator);
  m_dataBuffer.term();
  m_fileServerBuffer.term();

  m_sessionRunning = false;
  m_sessionPaused = false;
  m_asynchSteppingEnabled = false;
  setMaster(false);

  clearDebuggedNetworks();
}

//----------------------------------------------------------------------------------------------------------------------
bool Connection::isAlive() const
{
  if (!m_commandsSocket->isValid())
    return false;

  if (!m_dataSocket->isValid())
    return false;

  if (!m_fileServerSocket->isValid())
    return false;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Connection::sendPingPacket(uint32_t pingId)
{
  if (!m_dataSocket->isValid())
    return false;

  PingDataPacket ping(pingId);
  ping.serialize();

  return sendDataPacket(ping);
}

//----------------------------------------------------------------------------------------------------------------------
bool Connection::sendPacket(NMP::SocketWrapper* socketWrapper, PacketBase& pkt)
{
  if (!socketWrapper->isValid())
    return false;

  PacketLen pktLen = pkt.hdr.m_length;
  pkt.hdr.endianSwap();

  // Send the packet immediately, without caching.
  int res = NMP::send(socketWrapper->getSocket(), (const char*)&pkt, pktLen, 0);

  if (res < 0)
  {
    NMP::shutdown(socketWrapper->getSocket(), 0x02);
    NMP::closesocket(socketWrapper->getSocket());
    socketWrapper->setInvalid();
    return false;
  }

  if (res < (int)pktLen)
  {
    NMP::shutdown(socketWrapper->getSocket(), 0x02);
    NMP::closesocket(socketWrapper->getSocket());
    socketWrapper->setInvalid();
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Connection::sendDataPacket(PacketBase& pkt)
{
  if (!bufferDataPacket(pkt))
  {
    return false;
  }

  flushDataBuffer();
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Connection::bufferDataPacket(PacketBase& pkt)
{
  // Check we have space in the buffer for this packet
  if (m_dataBuffer.getAvailableMemory() < pkt.hdr.m_length)
  {
    // Send the current buffer and reset
    m_dataBuffer.flush();
  }

  // validate packet contents
  unsigned int pktLen = (unsigned int)pkt.hdr.m_length;
  uint8_t* dPt = (uint8_t*)&pkt;
  NMP_ASSERT(pkt.hdr.m_id > pk_core && pkt.hdr.m_id < pk_top);

  // swap the header if required (rest of packet is swapped by owner)
  PacketHeader hdr = pkt.hdr;
  hdr.endianSwap();

  // Copy the header.
  m_dataBuffer.bufferData((uint8_t*)&hdr, sizeof(PacketHeader));

  pktLen -= sizeof(PacketHeader);
  dPt += sizeof(PacketHeader);

  // Copy the rest of the packet.
  while (pktLen > 0)
  {
    uint32_t dataToSend = NMP::minimum(pktLen, m_dataBuffer.getAvailableMemory());

    m_dataBuffer.bufferData(dPt, dataToSend);
    pktLen -= dataToSend;
    dPt += dataToSend;

    if (m_dataBuffer.getAvailableMemory() == 0)
    {
      m_dataBuffer.flush();
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void Connection::flushDataBuffer()
{
  if (!isAlive() || m_dataBuffer.isEmpty())
    return;

  m_dataBuffer.flush();
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Connection::getFileSize(const char* filename)
{
  NetworkDataBuffer::ScopedBuffer scopedBuffer(&m_fileServerBuffer);

  FileSizeRequestPacket* fsrp = FileSizeRequestPacket::create(&m_fileServerBuffer, filename);
  fsrp->serialize();

  if (!sendPacket(m_fileServerSocket, *fsrp))
  {
    return 0;
  }

  // Block waiting for the reply.
  FileSizePacket fsp;
  PacketBase* pkt = receivePacket(m_fileServerSocket, (char*)&fsp, sizeof(FileSizePacket));
  if (!pkt)
  {
    NMP_MSG("MorphemeComms: an error occurred while trying to retrieve the size of the file %s.", filename);
    return 0;
  }
  fsp.deserialize();

  return fsp.m_fileSize;
}

//----------------------------------------------------------------------------------------------------------------------
bool Connection::getFile(void* buffer, uint32_t bufferSize, const char* filename)
{
  NetworkDataBuffer::ScopedBuffer scopedBuffer(&m_fileServerBuffer);

  FileRequestPacket* fr = FileRequestPacket::create(&m_fileServerBuffer, filename);
  fr->serialize();

  if (!sendPacket(m_fileServerSocket, *fr))
  {
    return false;
  }

  // do the receive manually (no "receivePacket") to use input buffer directly.
  FilePacket filePacket;
  uint32_t readBytes = MCOMMS::recvBuffer(m_fileServerSocket, (char*)&filePacket, sizeof(FilePacket));

  if (readBytes != sizeof(FilePacket))
  {
    // Not enough data.
    return false;
  }

  // Is this a valid header packet? Is this a file server specific packet?
  NMP_ASSERT(filePacket.hdr.validMagicA());
  NMP_ASSERT(filePacket.hdr.isFServ());
  filePacket.hdr.endianSwap();
  NMP::netEndianSwap(filePacket.m_dataSize);

  NMP_ASSERT(filePacket.hdr.m_id == pk_File);
  // Read the file data directly into the buffer.
  uint32_t fileSize = filePacket.m_dataSize;
  char* destBuffer = (char*)buffer;
  if (bufferSize > fileSize)
  {
    bufferSize = fileSize;
  }

  while (fileSize > 0)
  {
    if (bufferSize > 0)
    {
      int bytesReceived = recvBuffer(m_fileServerSocket, destBuffer, bufferSize);
      if (bytesReceived <= 0)
      {
        return false;
      }

      destBuffer += bytesReceived;
      bufferSize -= bytesReceived;
      fileSize -= bytesReceived;
    }
    else
    {
      static const uint32_t BUFFERSIZE = 256;
      static char s_buffer[BUFFERSIZE];

      // No space left in buffer - discard the remainder, reading in a temp buff.
      int toRead = (fileSize > BUFFERSIZE) ? (BUFFERSIZE) : (fileSize);
      int read = recvBuffer(m_fileServerSocket, s_buffer, toRead);

      if (read <= 0)
      {
        return false;
      }

      fileSize -= read;
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace MCOMMS
