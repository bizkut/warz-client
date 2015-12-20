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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef NM_BITSTREAM_CODER_H
#define NM_BITSTREAM_CODER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{
// Maximum extra bytes required for the bit-stream buffer
#define BITSTREAM_BUFFER_MAX_EXTRA_BYTES 4

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::BitStreamEncoder
/// \brief A class to write integer values into a bit-stream buffer. This class
/// is not responsible for allocating or freeing the buffer memory.
//----------------------------------------------------------------------------------------------------------------------
class BitStreamEncoder
{
public:
  BitStreamEncoder() {}

  /// \brief Calculate the memory required to create a bit-stream buffer to hold a
  /// specified number of bytes.
  static NMP::Memory::Format getMemoryRequirementsForBuffer(size_t numBytes);

  /// \brief Initialise the encoder with the bit-stream buffer
  NM_INLINE void init(
    size_t   numBytes,            ///< The maximum number of bytes that can be written to the buffer
    uint8_t* buffer               ///< A pointer to the bit-stream buffer memory
  );

  /// \brief Function to reset the bit position back to the start of the buffer
  NM_INLINE void reset();

  /// \brief Function to write a specified number of bits from an 8-bit integer to the bit-stream
  NM_INLINE void write(
    size_t numBits,
    uint8_t  data);

  /// \brief Function to write a specified number of bits from a 16-bit integer to the bit-stream
  NM_INLINE void write(
    size_t numBits,
    uint16_t data);

  /// \brief Function to write a specified number of bits from a 32-bit integer to the bit-stream
  NM_INLINE void write(
    size_t numBits,
    uint32_t data);

  /// \brief Function to write a single bit from an integer to the bit-stream
  template <class T>
  NM_INLINE void writeBit(T data);

  /// \brief Function to check if the bit position is within the buffer
  NM_INLINE bool isValidPosition(size_t bitPos) const;

  /// \brief Function to check to see if the bit stream is aligned to a byte boundary
  NM_INLINE bool isAligned() const;

  /// \brief Function to check to see if the bit stream is aligned with the specified alignment
  NM_INLINE bool isAligned(size_t alignment) const;

  /// \brief Function to flush the bit stream position to the nearest byte boundary
  NM_INLINE void flush();

  /// \brief Function to align the bit stream position to the specified granularity
  NM_INLINE void align(size_t alignment);

  /// \brief Function to move the bit stream position to the specified position
  NM_INLINE void seek(size_t bitPos);

  /// \brief Function to move the bit stream position to the specified position
  NM_INLINE void seek(uint8_t* addr);

  /// \brief Function to move the bit stream position relative to the current position
  NM_INLINE void increment(size_t bits);

  /// \brief Function to move the bit stream position relative to the current position
  NM_INLINE void incrementBytes(size_t bytes);

  /// \brief Function to get the number of bits remaining in the buffer
  NM_INLINE size_t getBitsRemaining() const;

  /// \brief Function to get the number of bytes remaining in the buffer
  NM_INLINE size_t getBytesRemaining() const;

  /// \brief Function to get the number of bits written to the buffer
  NM_INLINE size_t getBitsWritten() const;

  /// \brief Function to get the number of bytes written to the buffer
  NM_INLINE size_t getBytesWritten() const;

  /// \brief Function to get the size of the buffer in bytes
  NM_INLINE size_t getBufferSize() const;

  /// \brief Function to get a pointer to the buffer data
  NM_INLINE uint8_t* getBuffer();

  /// \brief Function to get the current byte position
  NM_INLINE uint8_t* getCurrentBytePos() const;

protected:
  // Bit-stream buffer
  size_t              m_bitsBufferSize;
  uint8_t*            m_bitsBuffer;

  // Current position within the bit-stream buffer
  size_t              m_bitIndex;
  uint8_t*            m_bitsPos;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::BitStreamDecoder
/// \brief A class to read integer values from a bit-stream buffer. This class
/// is not responsible for allocating or freeing the buffer memory.
//----------------------------------------------------------------------------------------------------------------------
class BitStreamDecoder
{
public:
  BitStreamDecoder() {}

  /// \brief Calculate the memory required to create a bit-stream buffer to hold a
  /// specified number of bytes.
  static NMP::Memory::Format getMemoryRequirementsForBuffer(size_t numBytes);

  /// \brief Initialise the encoder with the bit-stream buffer
  NM_INLINE void init(
    size_t   numBytes,            ///< The maximum number of bytes that can be written to the buffer
    const uint8_t* buffer         ///< A pointer to the bit-stream buffer memory
  );

  /// \brief Initialise the encoder with the bit-stream buffer
  NM_INLINE void init(
    const BitStreamDecoder& decoder);

  /// \brief Function to reset the bit position back to the start of the buffer
  NM_INLINE void reset();

  /// \brief Function to read a specified number of bits from the bit-stream to an 8-bit integer
  NM_INLINE void read(
    size_t numBits,
    uint8_t& data);

  /// \brief Function to read a specified number of bits from the bit-stream to a 16-bit integer
  NM_INLINE void read(
    size_t numBits,
    uint16_t& data);

  /// \brief Function to read a specified number of bits from the bit-stream to a 32-bit integer
  NM_INLINE void read(
    size_t numBits,
    uint32_t& data);

  /// \brief Function to read a single bit from the bit-stream to an integer
  template <class T>
  NM_INLINE void readBit(T& data);

  /// \brief Function to read bits from the bit-stream to an integer array
  template <class T>
  NM_INLINE void readAlignedBitArray(
    size_t   numBits,
    T*      data);

  /// \brief Function to read bits from the bit-stream to an integer array
  template <class T>
  NM_INLINE void readBitArray(
    size_t   numBits,
    T*      data);

  /// \brief Function to read a half byte from the bit-stream to an integer.
  /// This function assumes that the half byte is aligned with a half byte boundary
  template <class T>
  NM_INLINE void readAlignedHalfByte(T& data);

  /// \brief Function to read an aligned byte from the bit-stream to an integer
  template <class T>
  NM_INLINE void readAlignedUInt8(T& data);

  /// \brief Function to read an aligned 16-bit integer from the bit-stream to an integer
  template <class T>
  NM_INLINE void readAlignedUInt16(T& data);

  /// \brief Function to read an aligned 32-bit integer from the bit-stream to an integer
  NM_INLINE void readAlignedUInt32(uint32_t& data);

  /// \brief Function to check if the bit position is within the buffer
  NM_INLINE bool isValidPosition(size_t bitPos) const;

  /// \brief Function to check to see if the bit stream is aligned to a byte boundary
  NM_INLINE bool isAligned() const;

  /// \brief Function to check to see if the bit stream is aligned with the specified alignment
  NM_INLINE bool isAligned(size_t alignment) const;

  /// \brief Function to flush the bit stream position to the nearest byte boundary
  NM_INLINE void flush();

  /// \brief Function to align the bit stream position to the specified granularity
  NM_INLINE void align(size_t alignment);

  /// \brief Function to move the bit stream position to the specified position
  NM_INLINE void seek(size_t bitPos);

  /// \brief Function to move the bit stream position to the specified position
  NM_INLINE void seek(uint8_t* addr);

  /// \brief Function to move the bit stream position relative to the current position
  NM_INLINE void increment(size_t bits);

  /// \brief Function to move the bit stream position relative to the current position
  NM_INLINE void incrementBytes(size_t bytes);

  /// \brief Function to get the number of bits remaining in the buffer
  NM_INLINE size_t getBitsRemaining() const;

  /// \brief Function to get the number of bytes remaining in the buffer
  NM_INLINE size_t getBytesRemaining() const;

  /// \brief Function to get the number of bits written to the buffer
  NM_INLINE size_t getBitsRead() const;

  /// \brief Function to get the number of bytes written to the buffer
  NM_INLINE size_t getBytesRead() const;

  /// \brief Function to get the size of the buffer in bytes
  NM_INLINE size_t getBufferSize() const;

  /// \brief Function to get a pointer to the buffer data
  NM_INLINE const uint8_t* getBuffer();

  /// \brief Function to get the current byte position
  NM_INLINE const uint8_t* getCurrentBytePos() const;

protected:
  // Bit-stream buffer
  size_t              m_bitsBufferSize;
  const uint8_t*      m_bitsBuffer;

  // Current position within the bit-stream buffer
  size_t              m_bitIndex;
  const uint8_t*      m_bitsPos;
};

//----------------------------------------------------------------------------------------------------------------------
// BitStreamEncoder Inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitStreamEncoder::init(
  size_t numBytes,
  uint8_t* buffer)
{
  m_bitsBufferSize = numBytes;
  m_bitsBuffer = buffer;
  reset();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitStreamEncoder::reset()
{
  m_bitIndex = 0;
  m_bitsPos = m_bitsBuffer;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitStreamEncoder::write(
  size_t numBits,
  uint8_t data)
{
  size_t flags;
  NMP_ASSERT(numBits <= 8);
  NMP_ASSERT(getBitsRemaining() >= numBits);

  // Clean out the current byte if required
  flags = (m_bitIndex + 0x07) >> 3;
  m_bitsPos[flags] = 0;

  // Shift the 8-bit value to the correct location
  flags = ((size_t)data << m_bitIndex);

  // Pack the value into the bit-stream (buffer needs required scratch space).
  m_bitsPos[0] |= (uint8_t)flags;
  m_bitsPos[1] = (uint8_t)(flags >> 8);

  // Update the bit index offset and byte pointer
  m_bitIndex += numBits;
  m_bitsPos += (m_bitIndex >> 3);
  m_bitIndex &= 0x07;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitStreamEncoder::write(
  size_t numBits,
  uint16_t data)
{
  size_t flags;
  NMP_ASSERT(numBits <= 16);
  NMP_ASSERT(getBitsRemaining() >= numBits);

  // Clean out the current byte if required
  flags = (m_bitIndex + 0x07) >> 3;
  m_bitsPos[flags] = 0;

  // Shift the 16-bit value to the correct location
  flags = ((size_t)data << m_bitIndex);

  // Pack the value into the bit-stream (buffer needs required scratch space).
  m_bitsPos[0] |= (uint8_t)flags;
  m_bitsPos[1] = (uint8_t)(flags >> 8);
  m_bitsPos[2] = (uint8_t)(flags >> 16);

  // Update the bit index offset and byte pointer
  m_bitIndex += numBits;
  m_bitsPos += (m_bitIndex >> 3);
  m_bitIndex &= 0x07;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitStreamEncoder::write(
  size_t numBits,
  uint32_t data)
{
  size_t flags;
  NMP_ASSERT(numBits <= 32);
  NMP_ASSERT(getBitsRemaining() >= numBits);

  // Clean out the current byte if required
  flags = (m_bitIndex + 0x07) >> 3;
  m_bitsPos[flags] = 0;

  // Lower bits: Shift the 32-bit value to the correct location
  flags = ((size_t)data << m_bitIndex);
  // Pack the value into the bit-stream (buffer needs required scratch space).
  m_bitsPos[0] |= (uint8_t)flags;
  m_bitsPos[1] = (uint8_t)(flags >> 8);
  m_bitsPos[2] = (uint8_t)(flags >> 16);
  m_bitsPos[3] = (uint8_t)(flags >> 24);

  // Upper bits: Shift the 32-bit value to the correct location: data >> (32 - m_bitIndex)
  // Note that m_bitIndex can be 0, in which case the register is not filled with
  // zeros, but will flag an exception and keep data in the register. Instead we
  // shift the upper 8-bits into the lower 8-bits (zeroing the upper 24-bits).
  flags = ((size_t)data >> 24); // Intermediate step to avoid (data >> 32)
  m_bitsPos[4] = (uint8_t)(flags >> (8 - m_bitIndex));

  // Update the bit index offset and byte pointer
  m_bitIndex += numBits;
  m_bitsPos += (m_bitIndex >> 3);
  m_bitIndex &= 0x07;
}

//----------------------------------------------------------------------------------------------------------------------
template <class T>
NM_INLINE void BitStreamEncoder::writeBit(T data)
{
  NMP_ASSERT(getBitsRemaining() >= 1);

  // Clear out the current bit
  m_bitsPos[0] &= ~(0xff << m_bitIndex);

  // Write the bit to the bit stream
  m_bitsPos[0] |= static_cast<uint8_t>((size_t)data << m_bitIndex);

  // Update the bit index offset and byte pointer
  m_bitIndex++;
  m_bitsPos += (m_bitIndex >> 3);
  m_bitIndex &= 0x07;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool BitStreamEncoder::isValidPosition(size_t bitPos) const
{
  return bitPos <= (m_bitsBufferSize << 3);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool BitStreamEncoder::isAligned() const
{
  return m_bitIndex == 0;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool BitStreamEncoder::isAligned(size_t alignment) const
{
  if (m_bitIndex != 0)
    return false;
  return NMP_IS_ALIGNED(m_bitsPos, alignment);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitStreamEncoder::flush()
{
  m_bitsPos += ((m_bitIndex + 0x07) >> 3);
  m_bitIndex = 0;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitStreamEncoder::align(size_t alignment)
{
  flush();
  m_bitsPos = (uint8_t*)NMP::Memory::align((void*)m_bitsPos, alignment);
  NMP_ASSERT(isValidPosition(getBitsWritten()));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitStreamEncoder::seek(size_t bitPos)
{
  m_bitsPos = m_bitsBuffer + (bitPos >> 3);
  m_bitIndex = (bitPos & 0x07);
  NMP_ASSERT(isValidPosition(getBitsWritten()));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitStreamEncoder::seek(uint8_t* addr)
{
  m_bitsPos = addr;
  m_bitIndex = 0;
  NMP_ASSERT(isValidPosition(getBitsWritten()));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitStreamEncoder::increment(size_t bits)
{
  m_bitIndex += bits;
  m_bitsPos += (m_bitIndex >> 3);
  m_bitIndex &= 0x07;
  NMP_ASSERT(isValidPosition(getBitsWritten()));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitStreamEncoder::incrementBytes(size_t bytes)
{
  m_bitsPos += bytes;
  NMP_ASSERT(isValidPosition(getBitsWritten()));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE size_t BitStreamEncoder::getBitsRemaining() const
{
  size_t numBytesWritten = (size_t)(m_bitsPos - m_bitsBuffer);
  size_t numBitsWrittten = (numBytesWritten << 3) + m_bitIndex;
  return (m_bitsBufferSize << 3) - numBitsWrittten;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE size_t BitStreamEncoder::getBytesRemaining() const
{
  size_t numBytesWritten = (size_t)(m_bitsPos - m_bitsBuffer); // Number of complete bytes written
  return m_bitsBufferSize - numBytesWritten;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE size_t BitStreamEncoder::getBitsWritten() const
{
  size_t numBytesWritten = (size_t)(m_bitsPos - m_bitsBuffer);
  size_t numBitsWrittten = (numBytesWritten << 3) + m_bitIndex;
  return numBitsWrittten;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE size_t BitStreamEncoder::getBytesWritten() const
{
  size_t numBytesWritten = (size_t)(m_bitsPos - m_bitsBuffer);
  numBytesWritten += ((m_bitIndex + 0x07) >> 3); // To nearest byte
  return numBytesWritten;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE size_t BitStreamEncoder::getBufferSize() const
{
  return m_bitsBufferSize;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint8_t* BitStreamEncoder::getBuffer()
{
  return m_bitsBuffer;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint8_t* BitStreamEncoder::getCurrentBytePos() const
{
  return m_bitsPos;
}

//----------------------------------------------------------------------------------------------------------------------
// BitStreamDecoder Inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitStreamDecoder::init(
  size_t       numBytes,
  const uint8_t* buffer)
{
  m_bitsBufferSize = numBytes;
  m_bitsBuffer = buffer;
  reset();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitStreamDecoder::init(
  const BitStreamDecoder& decoder)
{
  NMP_ASSERT(decoder.m_bitsBuffer); // Bytes can be zero but must have a buffer
  m_bitsBufferSize = decoder.m_bitsBufferSize;
  m_bitsBuffer = decoder.m_bitsBuffer;
  m_bitIndex = decoder.m_bitIndex;
  m_bitsPos = decoder.m_bitsPos;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitStreamDecoder::reset()
{
  m_bitIndex = 0;
  m_bitsPos = m_bitsBuffer;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitStreamDecoder::read(
  size_t numBits,
  uint8_t& data)
{
  size_t flags;
  NMP_ASSERT(numBits <= 8);
  NMP_ASSERT(getBitsRemaining() >= numBits);

  // Unpack the shifted value from the bit-stream
  flags = (size_t)m_bitsPos[0] | ((size_t)m_bitsPos[1] << 8);

  // Shift from the packed location and mask the valid bits
  data = (uint8_t)((flags >> m_bitIndex) & (~((size_t)(-1) << numBits)));

  // Update the bit index offset and byte pointer
  m_bitIndex += numBits;
  m_bitsPos += (m_bitIndex >> 3);
  m_bitIndex &= 0x07;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitStreamDecoder::read(
  size_t  numBits,
  uint16_t& data)
{
  size_t flags;
  NMP_ASSERT(numBits <= 16);
  NMP_ASSERT(getBitsRemaining() >= numBits);

  // Unpack the shifted value from the bit-stream
  flags = (size_t)m_bitsPos[0] | ((size_t)m_bitsPos[1] << 8) | ((size_t)m_bitsPos[2] << 16);

  // Shift from the packed location and mask the valid bits
  data = (uint16_t)((flags >> m_bitIndex) & (~((size_t)(-1) << numBits)));

  // Update the bit index offset and byte pointer
  m_bitIndex += numBits;
  m_bitsPos += (m_bitIndex >> 3);
  m_bitIndex &= 0x07;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitStreamDecoder::read(
  size_t  numBits,
  uint32_t& data)
{
  size_t flags, mask, val;
  NMP_ASSERT(numBits <= 32);
  NMP_ASSERT(getBitsRemaining() >= numBits);

  // Compute the bit mask: (0x01 << numBits) - 1
  // Note that numBits can be 32, but 0x01 << 32 will not fill the register
  // with zeros, but will flag an exception and keep 0x01 in the register.
  // Instead we must keep track of the bit mask in two parts.
  mask = 0 - ((numBits >> 5) & 0x01); // 0xffffffff if 32 bits, 0 otherwise
  mask |= ((0x01 << (numBits & 0x1f)) - 1); // First 31 bits of mask

  // Lower 16-bits: Unpack the shifted value from the bit-stream
  flags = (size_t)m_bitsPos[0] | ((size_t)m_bitsPos[1] << 8) | ((size_t)m_bitsPos[2] << 16);
  val = ((flags >> m_bitIndex) & 0xffff);

  // Upper 16-bits: Unpack the shifted value from the bit-stream
  flags = (size_t)m_bitsPos[2] | ((size_t)m_bitsPos[3] << 8) | ((size_t)m_bitsPos[4] << 16);
  val |= ((flags >> m_bitIndex) << 16);

  // Mask the valid bits
  data = (uint32_t)(val & mask);

  // Update the bit index offset and byte pointer
  m_bitIndex += numBits;
  m_bitsPos += (m_bitIndex >> 3);
  m_bitIndex &= 0x07;
}

//----------------------------------------------------------------------------------------------------------------------
template <class T>
NM_INLINE void BitStreamDecoder::readBit(T& data)
{
  NMP_ASSERT(getBitsRemaining() >= 1);

  // Read the bit from the stream
  data = static_cast<T>(((size_t)m_bitsPos[0] >> m_bitIndex) & 0x01);

  // Update the bit index offset and byte pointer
  m_bitIndex++;
  m_bitsPos += (m_bitIndex >> 3);
  m_bitIndex &= 0x07;
}

//----------------------------------------------------------------------------------------------------------------------
template <class T>
NM_INLINE void BitStreamDecoder::readAlignedBitArray(
  size_t numBits,
  T*      data)
{
  NMP_ASSERT(data);
  NMP_ASSERT(getBitsRemaining() >= numBits);
  NMP_ASSERT(m_bitIndex == 0);
  size_t flags, numBytes;

  // Aligned Read
  numBytes = numBits >> 3;
  for (size_t i = 0; i < numBytes; ++i)
  {
    flags = m_bitsPos[0];
    data[0] = static_cast<T>(flags & 0x01);
    data[1] = static_cast<T>((flags >> 1) & 0x01);
    data[2] = static_cast<T>((flags >> 2) & 0x01);
    data[3] = static_cast<T>((flags >> 3) & 0x01);
    data[4] = static_cast<T>((flags >> 4) & 0x01);
    data[5] = static_cast<T>((flags >> 5) & 0x01);
    data[6] = static_cast<T>((flags >> 6) & 0x01);
    data[7] = static_cast<T>((flags >> 7) & 0x01);

    // Update buffer position and data pointers
    m_bitsPos++;
    data += 8;
    numBits -= 8;
  }

  // Non Aligned
  for (size_t i = 0; i < numBits; ++i)
  {
    data[i] = static_cast<T>((m_bitsPos[0] >> m_bitIndex) & 0x01);
    m_bitIndex++;
    NMP_ASSERT(m_bitIndex < 8);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <class T>
NM_INLINE void BitStreamDecoder::readBitArray(
  size_t numBits,
  T*      data)
{
  NMP_ASSERT(data);
  NMP_ASSERT(getBitsRemaining() >= numBits);
  size_t flags, rem;

  // Non Aligned
  rem = (8 - m_bitIndex) & 0x07;
  flags = m_bitsPos[0] >> m_bitIndex;
  if (numBits < rem)
  {
    // Read the required number of bits
    for (size_t i = 0; i < numBits; ++i)
    {
      data[i] = static_cast<T>(flags & 0x01);
      flags >>= 1;
    }
    m_bitIndex += numBits;
    return;
  }
  else
  {
    // Read bits until alignment
    for (size_t i = 0; i < rem; ++i)
    {
      data[i] = static_cast<T>(flags & 0x01);
      flags >>= 1;
    }
    data += rem;
    numBits -= rem;

    // Update the bit index offset and byte pointer
    m_bitIndex += rem;
    m_bitsPos += (m_bitIndex >> 3);
    m_bitIndex &= 0x07;
  }

  // Aligned Read
  readAlignedBitArray(numBits, data);
}

//----------------------------------------------------------------------------------------------------------------------
template <class T>
NM_INLINE void BitStreamDecoder::readAlignedHalfByte(T& data)
{
  NMP_ASSERT(m_bitIndex == 0 || m_bitIndex == 4);
  size_t flags = (((size_t)m_bitsPos[0]) >> m_bitIndex) & 0x0f;
  data = static_cast<T>(flags);

  m_bitIndex += 4;
  m_bitsPos += (m_bitIndex >> 3);
  m_bitIndex &= 0x07;
}

//----------------------------------------------------------------------------------------------------------------------
template <class T>
NM_INLINE void BitStreamDecoder::readAlignedUInt8(T& data)
{
  NMP_ASSERT(isAligned());
  data = static_cast<T>(m_bitsPos[0]);
  m_bitsPos++;
}

//----------------------------------------------------------------------------------------------------------------------
template <class T>
NM_INLINE void BitStreamDecoder::readAlignedUInt16(T& data)
{
  NMP_ASSERT(isAligned());
  NMP_ASSERT(sizeof(T) >= 2);
  size_t flags = (size_t)m_bitsPos[0] | (((size_t)m_bitsPos[1]) << 8);
  data = static_cast<T>(flags);
  m_bitsPos += 2;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitStreamDecoder::readAlignedUInt32(uint32_t& data)
{
  NMP_ASSERT(isAligned());
  data = (uint32_t)m_bitsPos[0] |
         (((uint32_t)m_bitsPos[1]) << 8) |
         (((uint32_t)m_bitsPos[2]) << 16) |
         (((uint32_t)m_bitsPos[3]) << 24);
  m_bitsPos += 4;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool BitStreamDecoder::isValidPosition(size_t bitPos) const
{
  return bitPos <= (m_bitsBufferSize << 3);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool BitStreamDecoder::isAligned() const
{
  return m_bitIndex == 0;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool BitStreamDecoder::isAligned(size_t alignment) const
{
  if (m_bitIndex != 0)
    return false;
  return NMP_IS_ALIGNED(m_bitsPos, alignment);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitStreamDecoder::flush()
{
  m_bitsPos += ((m_bitIndex + 0x07) >> 3);
  m_bitIndex = 0;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitStreamDecoder::align(size_t alignment)
{
  flush();
  m_bitsPos = (const uint8_t*)NMP::Memory::align((void*)m_bitsPos, alignment);
  NMP_ASSERT(isValidPosition(getBitsRead()));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitStreamDecoder::seek(size_t bitPos)
{
  m_bitsPos = m_bitsBuffer + (bitPos >> 3);
  m_bitIndex = (bitPos & 0x07);
  NMP_ASSERT(isValidPosition(getBitsRead()));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitStreamDecoder::seek(uint8_t* addr)
{
  m_bitsPos = addr;
  m_bitIndex = 0;
  NMP_ASSERT(isValidPosition(getBitsRead()));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitStreamDecoder::increment(size_t bits)
{
  m_bitIndex += bits;
  m_bitsPos += (m_bitIndex >> 3);
  m_bitIndex &= 0x07;
  NMP_ASSERT(isValidPosition(getBitsRead()));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitStreamDecoder::incrementBytes(size_t bytes)
{
  m_bitsPos += bytes;
  NMP_ASSERT(isValidPosition(getBitsRead()));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE size_t BitStreamDecoder::getBitsRemaining() const
{
  size_t numBytesRead = (size_t)(m_bitsPos - m_bitsBuffer);
  size_t numBitsRead = (numBytesRead << 3) + m_bitIndex;
  return (m_bitsBufferSize << 3) - numBitsRead;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE size_t BitStreamDecoder::getBytesRemaining() const
{
  size_t numBytesRead = (size_t)(m_bitsPos - m_bitsBuffer); // Number of complete bytes read
  return m_bitsBufferSize - numBytesRead;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE size_t BitStreamDecoder::getBitsRead() const
{
  size_t numBytesRead = (size_t)(m_bitsPos - m_bitsBuffer);
  size_t numBitsRead = (numBytesRead << 3) + m_bitIndex;
  return numBitsRead;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE size_t BitStreamDecoder::getBytesRead() const
{
  size_t numBytesRead = (size_t)(m_bitsPos - m_bitsBuffer);
  numBytesRead += ((m_bitIndex + 0x07) >> 3); // To nearest byte
  return numBytesRead;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE size_t BitStreamDecoder::getBufferSize() const
{
  return m_bitsBufferSize;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint8_t* BitStreamDecoder::getBuffer()
{
  return m_bitsBuffer;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint8_t* BitStreamDecoder::getCurrentBytePos() const
{
  return m_bitsPos;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_BITSTREAM_CODER_H
//----------------------------------------------------------------------------------------------------------------------
