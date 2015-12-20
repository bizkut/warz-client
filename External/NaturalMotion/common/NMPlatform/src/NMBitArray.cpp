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
#include "NMPlatform/NMBitArray.h"

#if defined(NM_HOST_X360)
  #include <ppcintrinsics.h>
#endif

//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
uint32_t BitArray::findFirstClearBit() const
{
  uint32_t clearBit = INVALID_INDEX;

  // Note : Allocates bits from the high-order end first.
  uint32_t i = 0;
  while (clearBit == INVALID_INDEX && i < m_numUInts)
    //for(uint32_t i = 0 ; i < m_numUInts ; ++i)
  {
    uint32_t j;
    uint32_t data = m_data[i];
#if defined(NM_HOST_X360) // MORPH-21301: Equivalent code required for other PPC platforms.
    j = _CountLeadingZeros(~(data));

    if (j != 32)
    {
      clearBit = (i << 5) | j;
    }
#elif defined(NM_HOST_CELL_SPU)
    j = spu_extract(spu_cntlz(spu_promote(~(data), 0)), 0);
    if (j != 32)
    {
      clearBit = (i << 5) | j;
    }
#else
    if (data != 0xFFFFFFFF)
    {
      j = 0;
      while ((data & 0x80000000) != 0)
      {
        data <<= 1;
        ++j;
      }
      clearBit = (i << 5) | j;
    }
#endif
    ++i;
  }

  // We may have found an invalid bit in the last uint32_t.  Check for this.
  if (clearBit >= m_numBits)
  {
    clearBit = INVALID_INDEX;
  }

  return clearBit;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
