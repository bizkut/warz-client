// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
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
#ifndef NMP_PACKED_ARRAY_UINT32_H
#define NMP_PACKED_ARRAY_UINT32_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::PackedArrayUInt32
/// \brief Packed array with specified number of bits per entry
/// \ingroup CoreModule
//----------------------------------------------------------------------------------------------------------------------
class PackedArrayUInt32
{
public:
  /// \brief Prepare data for another platform
  static void dislocate(PackedArrayUInt32* target);

  /// \brief Prepare data for the local platform
  static void locate(PackedArrayUInt32* target);

  /// \brief Prepare data after being moved on the local platform
  static PackedArrayUInt32* relocate(void*& ptr);

  /// \brief Calculate the memory requirements of an instance, given the parameters.
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numEntries,
    uint32_t numBitsPerEntry);

  static NMP::Memory::Format getMemoryRequirements(
    const PackedArrayUInt32* src);

  /// \brief Initialise an instance into the provided memory region.
  static PackedArrayUInt32* init(
    NMP::Memory::Resource& resource,
    uint32_t numEntries,
    uint32_t numBitsPerEntry);

  static PackedArrayUInt32* init(
    NMP::Memory::Resource& resource,
    const PackedArrayUInt32* src);

  /// \brief Calculates the number of bits required to represent the value
  static uint32_t calculateNumBitsForValue(uint32_t value);

  NM_INLINE void setEntry(uint32_t entryIndex, uint32_t value);

  NM_INLINE uint32_t getEntry(uint32_t entryIndex) const;

protected:
  uint32_t    m_numEntries;
  uint32_t    m_numBitsPerEntry;
  uint32_t    m_dataBuffer[1];
};


//----------------------------------------------------------------------------------------------------------------------
// PackedArrayUInt32 inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t PackedArrayUInt32::getEntry(uint32_t entryIndex) const
{
  NMP_ASSERT(entryIndex < m_numEntries);
  uint32_t bitAddr = entryIndex * m_numBitsPerEntry;
  uint32_t uintOffset = bitAddr >> 5;
  uint32_t bitOffset = bitAddr & 0x1F;

  // This implements a shift right by bitOffset bits, then masks the resulting least significant
  // bitCount bits for an assumed 64-bit word: valueB << 32 | valueA. This is done purely through
  // 32-bit integers without the use of conditionals.
  uint32_t shiftB = (32 - bitOffset) & 0x1F; // zero if bitOffset is 0
  uint32_t maskB = (uint32_t)(((int32_t)bitOffset | -(int32_t)bitOffset) >> 31); // 0xffffffff if bitOffset is non-zero
  uint32_t shiftedAB = (m_dataBuffer[uintOffset] >> bitOffset) | ((m_dataBuffer[uintOffset + 1] << shiftB) & maskB); // Combine shifted words A and B
  uint32_t maskAB = 0xFFFFFFFF >> (32 - m_numBitsPerEntry);
  uint32_t result = shiftedAB & maskAB;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void PackedArrayUInt32::setEntry(uint32_t entryIndex, uint32_t value)
{
  NMP_ASSERT(entryIndex < m_numEntries);
  NMP_ASSERT((value & (0xFFFFFFFF << m_numBitsPerEntry)) == 0);
  uint32_t bitAddr = entryIndex * m_numBitsPerEntry;
  uint32_t uintOffset = bitAddr >> 5;
  uint32_t bitOffset = bitAddr & 0x1F;
  uint32_t* data = &m_dataBuffer[uintOffset];

  // Word A
  uint32_t maskAB = 0xFFFFFFFF >> (32 - m_numBitsPerEntry);
  data[0] &= ~(maskAB << bitOffset); // Zero out the bits
  data[0] |= (value << bitOffset); // Set the bits

  // Word B
  uint32_t shiftB = (32 - bitOffset) & 0x1F; // zero if bitOffset is 0
  uint32_t maskB = (uint32_t)(((int32_t)bitOffset | -(int32_t)bitOffset) >> 31); // 0xffffffff if bitOffset is non-zero
  data[1] &= ~((0xFFFFFFFF >> shiftB) & maskB); // Zero out the bits
  data[1] |= ((value >> shiftB) & maskB); // Set the bits
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // NMP_PACKED_ARRAY_UINT32_H
//----------------------------------------------------------------------------------------------------------------------
