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

#ifndef NMP_HASH_H
#define NMP_HASH_H

#include "NMPlatform/NMPlatform.h"

namespace NMP
{
NM_INLINE uint32_t hashCRC32(void* data, size_t count)
{
  NMP_ASSERT(data);

  const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data);
  const size_t bitsInChecksum = 8 * sizeof(uint32_t);
  const size_t byteShiftAmt = 8 * (sizeof(uint32_t) - 1);
  const size_t msb = 1u << (bitsInChecksum - 1);
  const unsigned int crcGenerator = 0x04C11DB7; // precomputed truncated CRC polynomial

  unsigned int remainder =  0xFFFFFFFF;

  // perform modulo-2 division byte by byte
  for (size_t currByte = 0; currByte < count; ++currByte)
  {
    // advance the next byte into the remainder
    remainder ^= bytes[currByte] << byteShiftAmt;

    // perform mod-2 division bit by bit
    for (uint8_t bit = 8; bit > 0; --bit)
    {
      // try to divide current bit
      if (remainder & msb)
      {
        remainder = (remainder << 1) ^ crcGenerator;
      }
      else
      {
        remainder <<= 1;
      }
    }
  }

  // CRC32 XORs remainder with 0xFFFFFFF to prevent data loss
  // this is the same as one's complement
  return ~remainder;
}
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t hashStringCRC32(const char* assetName)
{
  NMP_ASSERT(assetName);
  return hashCRC32((void*)assetName, strlen(assetName));
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NMP_HASH_H
//----------------------------------------------------------------------------------------------------------------------
