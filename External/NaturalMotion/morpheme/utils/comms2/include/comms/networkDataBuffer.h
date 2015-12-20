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
#ifndef MCOMMS_NETWORKDATABUFFER_H
#define MCOMMS_NETWORKDATABUFFER_H
//----------------------------------------------------------------------------------------------------------------------

#include "NMPlatform/NMPlatform.h"

namespace NMP
{
class SocketWrapper;
}

namespace MCOMMS
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief
class NetworkDataBuffer
{
public:
  NetworkDataBuffer();
  ~NetworkDataBuffer();

  class ScopedBuffer
  {
  public:
    ScopedBuffer(NetworkDataBuffer* buffer) :
      m_buffer(buffer)
    {
    }
    ~ScopedBuffer()
    {
      m_buffer->clear();
    }

    NetworkDataBuffer* m_buffer;
  };

  void initReceiveBuffer(uint32_t initialSize, uint32_t alignment, bool autoResize = true);
  void init(NMP::SocketWrapper* socketWrapper, uint32_t initialSize, uint32_t alignment, bool autoResize = true);
  void term();
  void flush();
  void clear();

  inline bool isEmpty() const { return m_dataBufferUsed == 0; }

  /// \brief Allocates memory from an internal data buffer.  All allocations are aligned to 4 byte boundaries
  uint8_t* alloc(uint32_t size);
  uint8_t* alignedAlloc(uint32_t size, uint32_t alignment);

  template <typename T> T reserveAlignedMemory(uint32_t size, uint32_t aligment)
  {
    return (T)alignedAlloc(size, aligment);
  }

  template <typename T> T reserveMemory(uint32_t size)
  {
    return (T)alloc(size);
  }

  /// \brief Buffer data directly.
  uint32_t bufferData(uint8_t* data, uint32_t size);
  uint32_t bufferAlignedData(uint8_t* data, uint32_t size, uint32_t alignment);

  inline uint32_t getAvailableMemory() const { return m_currentSize - m_dataBufferUsed; }

protected:
  void expand(uint32_t newSize);

  bool     m_autoResize;
  uint8_t* m_dataBuffer;
  uint32_t m_dataBufferUsed;
  uint32_t m_currentSize;
  uint32_t m_initialAlignment;

  NMP::SocketWrapper* m_socketWrapper;
};

}

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_NETWORKDATABUFFER_H
//----------------------------------------------------------------------------------------------------------------------
