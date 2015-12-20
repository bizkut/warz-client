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
#ifndef AP_ASSET_PROCESSOR_UTILS_H
#define AP_ASSET_PROCESSOR_UTILS_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include <sstream>
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// returns -1 if not found.
NM_INLINE int32_t findIndexOfStringInVector(const char* string, const std::vector<const char*>& stringVector)
{
  for (uint32_t k = 0; k < stringVector.size(); ++k)
  {
    if (strcmp(string, stringVector[k]) == 0)
    {
      return k;
    }
  }
  return -1;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // AP_ASSET_PROCESSOR_UTILS_H
//----------------------------------------------------------------------------------------------------------------------

