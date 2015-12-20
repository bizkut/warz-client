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
#include "NMPlatform/NMBitStreamCoder.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format BitStreamEncoder::getMemoryRequirementsForBuffer(size_t numBytes)
{
  NMP::Memory::Format result(numBytes + BITSTREAM_BUFFER_MAX_EXTRA_BYTES, NMP_NATURAL_TYPE_ALIGNMENT);
  result.size = NMP::Memory::align(result.size, NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format BitStreamDecoder::getMemoryRequirementsForBuffer(size_t numBytes)
{
  NMP::Memory::Format result(numBytes + BITSTREAM_BUFFER_MAX_EXTRA_BYTES, NMP_NATURAL_TYPE_ALIGNMENT);
  result.size = NMP::Memory::align(result.size, NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
