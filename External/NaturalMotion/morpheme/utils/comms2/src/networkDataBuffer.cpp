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
#include "comms/networkDataBuffer.h"
#include "comms/mcomms.h"
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMathUtils.h"
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMSocketWrapper.h"

namespace MCOMMS
{
//----------------------------------------------------------------------------------------------------------------------
NetworkDataBuffer::NetworkDataBuffer() :
  m_dataBuffer(0),
  m_dataBufferUsed(0)
{

}

//----------------------------------------------------------------------------------------------------------------------
NetworkDataBuffer::~NetworkDataBuffer()
{
  // Check that the buffer has been terminated properly.
  NMP_ASSERT(m_dataBuffer == 0);
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDataBuffer::init(
  NMP::SocketWrapper* socketWrapper, uint32_t initialSize, uint32_t alignment, bool autoResize)
{
  NMP_ASSERT(m_dataBuffer == 0);

  m_currentSize = initialSize;
  m_dataBufferUsed = 0;
  m_dataBuffer = (uint8_t*)NMPMemoryAllocAligned(m_currentSize, alignment);
  NMP_ASSERT(m_dataBuffer);
  m_socketWrapper = socketWrapper;
  m_autoResize = autoResize;
  m_initialAlignment = alignment;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDataBuffer::initReceiveBuffer(
  uint32_t initialSize, uint32_t alignment, bool autoResize)
{
  NMP_ASSERT(m_dataBuffer == 0);

  m_currentSize = initialSize;
  m_dataBufferUsed = 0;
  m_dataBuffer = (uint8_t*)NMPMemoryAllocAligned(m_currentSize, alignment);
  NMP_ASSERT(m_dataBuffer);
  m_socketWrapper = 0;
  m_autoResize = autoResize;
  m_initialAlignment = alignment;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDataBuffer::term()
{
  NMP_ASSERT(m_dataBuffer != 0); // Shouldn't be terminated if it wasn't initialized.
  NMP::Memory::memFree(m_dataBuffer);
  m_dataBuffer = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDataBuffer::flush()
{
  NMP_ASSERT(m_socketWrapper != 0); // Can't call flush on a buffer not properly linked to a socket.

  // Send the buffer
  uint32_t toSend = m_dataBufferUsed;
  uint8_t* bufPtr = m_dataBuffer;

  const uint32_t maxSendSize = 8192;
  do
  {
    uint32_t chunkSize = NMP::minimum(toSend, maxSendSize);
    int sent = NMP::send(m_socketWrapper->getSocket(), (char*)bufPtr, chunkSize, 0);

    if (sent < 0)
    {
      NMP::shutdown(m_socketWrapper->getSocket(), 0x02);
      NMP::closesocket(m_socketWrapper->getSocket());
      m_socketWrapper->setInvalid();
      NMP_MSG("NetworkDataBuffer: sendBuffer (%i bytes) - failed, closing connection", m_dataBufferUsed);
      return;
    }

    toSend -= sent;
    bufPtr += sent;

  } while (toSend > 0);

  clear();
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDataBuffer::clear()
{
  m_dataBufferUsed = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDataBuffer::expand(uint32_t newSize)
{
  // Realloc the buffer geometrically (or to a minimum size to accomodate the new data.
  uint8_t* newBuffer = (uint8_t*)NMPMemoryAllocAligned(newSize, m_initialAlignment);
  NMP_ASSERT(newBuffer);
  memcpy(newBuffer, m_dataBuffer, m_dataBufferUsed);
  NMP::Memory::memFree(m_dataBuffer);
  m_dataBuffer = newBuffer;
  m_currentSize = newSize;
}

//----------------------------------------------------------------------------------------------------------------------
uint8_t* NetworkDataBuffer::alignedAlloc(uint32_t size, uint32_t alignment)
{
  uint8_t* alignedAddr = (uint8_t*)NMP::Memory::align(m_dataBuffer + m_dataBufferUsed, alignment);
  uint32_t gap = static_cast<uint32_t>(alignedAddr - (m_dataBuffer + m_dataBufferUsed));
  size += gap;

  if (m_autoResize && (alignedAddr + size > m_dataBuffer + m_currentSize))
  {
    uint32_t newSize = NMP::maximum(m_currentSize * 2, m_dataBufferUsed + size);
    expand(newSize);
    alignedAddr = (uint8_t*)NMP::Memory::align(m_dataBuffer + m_dataBufferUsed, alignment);
  }

  NMP_ASSERT(size <= m_currentSize);

  // Fill the gap with a special character
  memset(m_dataBuffer + m_dataBufferUsed, NM_PKT_MAGIC_GAP, gap);

  m_dataBufferUsed += size;
  return alignedAddr;
}

//----------------------------------------------------------------------------------------------------------------------
uint8_t* NetworkDataBuffer::alloc(uint32_t size)
{
  return alignedAlloc(size, NMP_NATURAL_TYPE_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NetworkDataBuffer::bufferData(uint8_t* data, uint32_t size)
{
  uint8_t* dataCopy = alloc(size);
  if (dataCopy)
  {
    memcpy(dataCopy, data, size);
    return size;
  }

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NetworkDataBuffer::bufferAlignedData(uint8_t* data, uint32_t size, uint32_t alignment)
{
  uint8_t* dataCopy = alignedAlloc(size, alignment);
  if (dataCopy)
  {
    memcpy(dataCopy, data, size);
    return size;
  }

  return 0;
}

}
