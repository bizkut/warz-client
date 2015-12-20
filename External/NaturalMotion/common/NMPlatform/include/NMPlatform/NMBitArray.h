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
#ifndef NM_BIT_ARRAY_H
#define NM_BIT_ARRAY_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::BitArray
/// \brief Holds an array of bits (clamped to 32bit boundaries)
/// \ingroup NaturalMotionPlatform
//----------------------------------------------------------------------------------------------------------------------
class BitArray
{
public:

  static const uint32_t INVALID_INDEX = 0xFFFFFFFF;

  /// \brief Calculate the memory requirements of an instance.
  NM_INLINE static NMP::Memory::Format getMemoryRequirements(const uint32_t numBits);

  /// \brief Initialise an instance into the provided memory region.
  NM_INLINE static BitArray* init(NMP::Memory::Resource& resource, const uint32_t numBits);

  /// \brief Relocate an instance in the memory position
  NM_INLINE static BitArray* relocate(void*& ptr);

  /// \brief Calculate the memory requirements of this instance.
  NM_INLINE NMP::Memory::Format getMemoryRequirements() const;

  /// \brief copy array from another array
  NM_INLINE void copy(const BitArray* source);

  /// \brief Set all bits in the array FALSE
  NM_INLINE void clearAll();

  /// \brief Set all bits in the array TRUE
  NM_INLINE void setAll();

  /// \brief Calculated if all bits in the array are set.
  /// \return true if all bits in the array are set.
  NM_INLINE bool calculateAreAllSet() const;

  /// \brief Get the number of bits in this array.
  NM_INLINE uint32_t getNumBits() const;

  /// \brief Set the specified bit TRUE
  NM_INLINE void setBit(uint32_t bitIdx);

  /// \brief Set the specified bit according to value
  NM_INLINE void setBit(uint32_t bitIdx, bool value);

  // \brief calculate set theoretic union
  NM_INLINE void setUnion(const BitArray* operand2, const BitArray* operand3);

  // \brief calculate set theoretic intersection
  NM_INLINE void setIntersect(const BitArray* operand2, const BitArray* operand3);

  // \brief calculate set theoretic exclusive or
  NM_INLINE void setNotEqual(const BitArray* operand2, const BitArray* operand3);

  // \brief calculate set theoretic inverse intersect
  NM_INLINE void setNotIntersect(const BitArray* operand2, const BitArray* operand3);

  // \brief return 4 contiguous bits
  NM_INLINE uint32_t getQuad(uint32_t startBit) const;

  /// \brief Set the specified bit FALSE
  NM_INLINE void clearBit(uint32_t bitIdx);

  /// \brief Not that this returns a non-zero value (not necessarily 1) if the bit is set.
  NM_INLINE bool isBitSet(uint32_t bitIdx) const;

  /// \brief Returns the index of the first clear bit
  uint32_t findFirstClearBit() const;

  /// \brief Prepare a dislocated BitArray for use.
  NM_INLINE bool locate();

  /// \brief Dislocate a BitArray ready to move to a new memory location.
  NM_INLINE bool dislocate();

private:
  BitArray() {};
  ~BitArray() {};

  static uint32_t numBitsToNumUInts(uint32_t numBits);
  static uint32_t bitIndexToUIntIndex(uint32_t numBits);
  static uint32_t bitIndexToUIntMask(uint32_t numBits);

  uint32_t m_numBits;
  uint32_t m_numUInts;
  uint32_t m_data[1];
};

//----------------------------------------------------------------------------------------------------------------------
// BitArray functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t BitArray::numBitsToNumUInts(uint32_t numBits)
{
  return (uint32_t)((NMP::Memory::align(numBits, 32)) >> 5);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t BitArray::bitIndexToUIntIndex(uint32_t bitIndex)
{
  return bitIndex >> 5;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t BitArray::bitIndexToUIntMask(uint32_t bitIndex)
{
  // High-order bits are considered as coming 'first' so that we can use the clz (_CountLeadingZeros) instruction on
  // PPC platforms.
  return 0x80000000 >> (bitIndex & 31L);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Memory::Format BitArray::getMemoryRequirements(const uint32_t numBits)
{
  // It is not a valid operation to get the memory requirements of a zero-length bitfield.
  NMP_ASSERT_MSG(numBits, "Zero-entry NMP::BitArrays are not allowed");

  NMP::Memory::Format result(sizeof(BitArray), NMP_NATURAL_TYPE_ALIGNMENT);
  size_t numUInts = numBitsToNumUInts(numBits);
  result.size += (numUInts - 1) * sizeof(uint32_t);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Memory::Format BitArray::getMemoryRequirements() const
{
  NMP::Memory::Format result(sizeof(BitArray), NMP_NATURAL_TYPE_ALIGNMENT);
  result.size += (m_numUInts - 1) * sizeof(uint32_t);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE BitArray* BitArray::init(NMP::Memory::Resource& resource, const uint32_t numBits)
{
  // It is not a valid operation to get the memory requirements of a zero-length bitfield.
  NMP_ASSERT_MSG(numBits, "Zero-entry NMP::BitArrays are not allowed");
  NMP::Memory::Format memReqs(sizeof(BitArray), NMP_NATURAL_TYPE_ALIGNMENT);
  uint32_t numUInts = numBitsToNumUInts(numBits);
  memReqs.size += (numUInts - 1) * sizeof(uint32_t);
  
  BitArray* result = (BitArray*) resource.alignAndIncrement(memReqs);
  result->m_numBits = numBits;
  result->m_numUInts = numUInts;
  result->clearAll();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE BitArray* BitArray::relocate(void*& ptr)
{
  NMP::Memory::Format memReqs(sizeof(BitArray), NMP_NATURAL_TYPE_ALIGNMENT);
  ptr = (void*) NMP::Memory::align(ptr, memReqs.alignment);
  BitArray* result = (BitArray*) ptr;

  NMP_ASSERT(result->m_numBits > 0);
  NMP_ASSERT(result->m_numUInts > 0);
  memReqs.size += (result->m_numUInts - 1) * sizeof(uint32_t);
  ptr = NMP::Memory::increment(ptr, memReqs.size);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitArray::copy(const BitArray* source)
{
  NMP_ASSERT_MSG(source->m_numBits == m_numBits, "Bit lengths of NMP::BitArray must match to copy");
  NMP_ASSERT_MSG(source->m_numUInts == m_numUInts, "Word lengths of NMP::BitArray must match to copy");

  for (uint32_t i = 0 ; i < m_numUInts ; ++i)
  {
    m_data[i] = source->m_data[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool BitArray::calculateAreAllSet() const
{
  // High bits correspond to low index values in the bit array.
  // eg. bit index 0 corresponds to bit 31 of the first int32.
  // First we check all the words that are fully utilised.
  // If m_numBits is a multiple of 32 that means all words.
  uint32_t isFull = 0xFFFFFFFF;
  for (uint32_t i = 31 ; i < m_numBits; i += 32)
  {
    isFull &= m_data[bitIndexToUIntIndex(i)];
  }

  // Deal with the last, possibly partially specified, int32.
  // If m_numBits is a multiple of 32 lastmask is 0xFFFFFFFF
  // and this doesn't really change the result.
  uint32_t lastmask = (0xFFFFFFFF >> (m_numBits & 31L));
  isFull &= lastmask | m_data[m_numUInts-1];

  return ~isFull == 0;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitArray::setUnion(const BitArray* operand2, const BitArray* operand3)
{
  NMP_ASSERT(operand2->m_numBits == m_numBits);
  NMP_ASSERT(operand2->m_numUInts == m_numUInts);
  NMP_ASSERT(operand3->m_numBits == operand2->m_numBits);
  NMP_ASSERT(operand3->m_numUInts == operand2->m_numUInts);

  for (uint32_t i = 0 ; i < m_numUInts ; ++i)
  {
    m_data[i] = operand2->m_data[i] | operand3->m_data[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitArray::setIntersect(const BitArray* operand2, const BitArray* operand3)
{
  NMP_ASSERT(operand2->m_numBits == m_numBits);
  NMP_ASSERT(operand2->m_numUInts == m_numUInts);
  NMP_ASSERT(operand3->m_numBits == operand2->m_numBits);
  NMP_ASSERT(operand3->m_numUInts == operand2->m_numUInts);

  for (uint32_t i = 0 ; i < m_numUInts ; ++i)
  {
    m_data[i] = operand2->m_data[i] & operand3->m_data[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitArray::setNotEqual(const BitArray* operand2, const BitArray* operand3)
{
  NMP_ASSERT(operand2->m_numBits == m_numBits);
  NMP_ASSERT(operand2->m_numUInts == m_numUInts);
  NMP_ASSERT(operand3->m_numBits == operand2->m_numBits);
  NMP_ASSERT(operand3->m_numUInts == operand2->m_numUInts);

  // XOR
  for (uint32_t i = 0 ; i < m_numUInts ; ++i)
  {
    m_data[i] = operand2->m_data[i] ^ operand3->m_data[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitArray::setNotIntersect(const BitArray* operand2, const BitArray* operand3)
{
  NMP_ASSERT(operand2->m_numBits == m_numBits);
  NMP_ASSERT(operand2->m_numUInts == m_numUInts);
  NMP_ASSERT(operand3->m_numBits == operand2->m_numBits);
  NMP_ASSERT(operand3->m_numUInts == operand2->m_numUInts);

  // NAND
  for (uint32_t i = 0 ; i < m_numUInts ; ++i)
  {
    m_data[i] = ~(operand2->m_data[i] & operand3->m_data[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t BitArray::getQuad(uint32_t startBit) const
{
  NMP_ASSERT(startBit < m_numBits);
  NMP_ASSERT((startBit & 3) == 0);

  return m_data[startBit >> 5] >> (28 - (startBit & 28L));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitArray::clearAll()
{
  for (uint32_t i = 0 ; i < m_numUInts ; ++i)
  {
    m_data[i] = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitArray::setAll()
{
  for (uint32_t i = 0 ; i < m_numUInts ; ++i)
  {
    m_data[i] = 0xFFFFFFFF;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t BitArray::getNumBits() const
{
  return m_numBits;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitArray::setBit(uint32_t bitIdx)
{
  NMP_ASSERT_MSG(bitIdx < m_numBits, "Valid bit index");
  uint32_t uintIndex = bitIndexToUIntIndex(bitIdx);
  uint32_t mask = bitIndexToUIntMask(bitIdx);
  m_data[uintIndex] |= mask;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitArray::clearBit(uint32_t bitIdx)
{
  NMP_ASSERT_MSG(bitIdx < m_numBits, "Valid bit index");
  uint32_t uintIndex = bitIndexToUIntIndex(bitIdx);
  uint32_t mask = bitIndexToUIntMask(bitIdx);
  m_data[uintIndex] &= ~mask;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BitArray::setBit(uint32_t bitIdx, bool value)
{
  NMP_ASSERT_MSG(bitIdx < m_numBits, "Valid bit index");
  uint32_t uintIndex = bitIndexToUIntIndex(bitIdx);
  uint32_t mask = bitIndexToUIntMask(bitIdx);
  m_data[uintIndex] = value ? m_data[uintIndex] | mask : m_data[uintIndex] & ~mask;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool BitArray::isBitSet(uint32_t bitIdx) const
{
  NMP_ASSERT_MSG(bitIdx < m_numBits, "Valid bit index");
  uint32_t uintIndex = bitIndexToUIntIndex(bitIdx);
  uint32_t mask = bitIndexToUIntMask(bitIdx);
  return ((m_data[uintIndex] & mask) != 0);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool BitArray::locate()
{
  NMP::endianSwap(m_numBits);
  NMP::endianSwap(m_numUInts);
  NMP::endianSwapArray(m_data, m_numUInts);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool BitArray::dislocate()
{
  NMP::endianSwapArray(m_data, m_numUInts);
  NMP::endianSwap(m_numUInts);
  NMP::endianSwap(m_numBits);
  return true;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_BIT_ARRAY_H
//----------------------------------------------------------------------------------------------------------------------
