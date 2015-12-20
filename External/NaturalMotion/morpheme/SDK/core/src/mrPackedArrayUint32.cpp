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
#include "morpheme/mrPackedArrayUint32.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
void PackedArrayUInt32::dislocate(PackedArrayUInt32* target)
{
  NMP_ASSERT(target);
  uint32_t numBits = target->m_numEntries * target->m_numBitsPerEntry;
  uint32_t numUInts = ((numBits + 0x1F) >> 5);
  NMP::endianSwapArray(target->m_dataBuffer, numUInts + 1);

  NMP::endianSwap(target->m_numBitsPerEntry);
  NMP::endianSwap(target->m_numEntries);
}

//----------------------------------------------------------------------------------------------------------------------
void PackedArrayUInt32::locate(PackedArrayUInt32* target)
{
  NMP_ASSERT(target);
  NMP::endianSwap(target->m_numEntries);
  NMP::endianSwap(target->m_numBitsPerEntry);

  uint32_t numBits = target->m_numEntries * target->m_numBitsPerEntry;
  uint32_t numUInts = ((numBits + 0x1F) >> 5);
  NMP::endianSwapArray(target->m_dataBuffer, numUInts + 1);
}

//----------------------------------------------------------------------------------------------------------------------
PackedArrayUInt32* PackedArrayUInt32::relocate(void*& ptr)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(PackedArrayUInt32), NMP_NATURAL_TYPE_ALIGNMENT);
  PackedArrayUInt32* result = (PackedArrayUInt32*)NMP::Memory::alignAndIncrement(ptr, memReqsHdr);

  // Increment the pointer for the payload data
  uint32_t numBits = result->m_numEntries * result->m_numBitsPerEntry;
  uint32_t numUInts = ((numBits + 0x1F) >> 5);
  NMP::Memory::Format memReqsData(sizeof(uint32_t) * numUInts, NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::alignAndIncrement(ptr, memReqsData);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format PackedArrayUInt32::getMemoryRequirements(
  uint32_t numEntries,
  uint32_t numBitsPerEntry)
{
  NMP_ASSERT(numEntries > 0);
  NMP_ASSERT(numBitsPerEntry > 0);

  NMP::Memory::Format result(sizeof(PackedArrayUInt32), NMP_NATURAL_TYPE_ALIGNMENT);
  
  uint32_t numBits = numEntries * numBitsPerEntry;
  uint32_t numUInts = ((numBits + 0x1F) >> 5);
  NMP::Memory::Format memReqsData(sizeof(uint32_t) * numUInts, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsData;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format PackedArrayUInt32::getMemoryRequirements(
  const PackedArrayUInt32* src)
{
  NMP_ASSERT(src);
  NMP::Memory::Format result = getMemoryRequirements(
    src->m_numEntries,
    src->m_numBitsPerEntry);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
PackedArrayUInt32* PackedArrayUInt32::init(
  NMP::Memory::Resource& resource,
  uint32_t numEntries,
  uint32_t numBitsPerEntry)
{
  NMP_ASSERT(numEntries > 0);
  NMP_ASSERT(numBitsPerEntry > 0);

  // Header
  NMP::Memory::Format memReqsHdr(sizeof(PackedArrayUInt32), NMP_NATURAL_TYPE_ALIGNMENT);
  PackedArrayUInt32* result = (PackedArrayUInt32*)resource.alignAndIncrement(memReqsHdr);

  result->m_numEntries = numEntries;
  result->m_numBitsPerEntry = numBitsPerEntry;

  // Increment the resource for the payload data
  uint32_t numBits = numEntries * numBitsPerEntry;
  uint32_t numUInts = ((numBits + 0x1F) >> 5);
  NMP::Memory::Format memReqsData(sizeof(uint32_t) * numUInts, NMP_NATURAL_TYPE_ALIGNMENT);
  resource.alignAndIncrement(memReqsData);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
PackedArrayUInt32* PackedArrayUInt32::init(
  NMP::Memory::Resource& resource,
  const PackedArrayUInt32* src)
{
  NMP_ASSERT(src);

  // Header
  NMP::Memory::Format memReqsHdr(sizeof(PackedArrayUInt32), NMP_NATURAL_TYPE_ALIGNMENT);
  PackedArrayUInt32* result = (PackedArrayUInt32*)resource.alignAndIncrement(memReqsHdr);

  result->m_numEntries = src->m_numEntries;
  result->m_numBitsPerEntry = src->m_numBitsPerEntry;

  // Increment the resource for the payload data
  uint32_t numBits = src->m_numEntries * src->m_numBitsPerEntry;
  uint32_t numUInts = ((numBits + 0x1F) >> 5);
  NMP::Memory::Format memReqsData(sizeof(uint32_t) * numUInts, NMP_NATURAL_TYPE_ALIGNMENT);
  resource.alignAndIncrement(memReqsData);

  // Copy the data buffer
  NMP::Memory::memcpy(
    result->m_dataBuffer,
    src->m_dataBuffer,
    sizeof(uint32_t) * (numUInts + 1));

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t PackedArrayUInt32::calculateNumBitsForValue(uint32_t value)
{
  uint32_t numBits = 0;
  for (uint32_t i = 0; value != 0; ++i)
  {
    value >>= 1;
    numBits++;
  }

  return numBits;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
