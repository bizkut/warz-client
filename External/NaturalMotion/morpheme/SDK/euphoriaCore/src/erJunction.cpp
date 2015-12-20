// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "euphoria/erJunction.h"
//----------------------------------------------------------------------------------------------------------------------

namespace ER
{

const float g_minImportanceForJunction = 0.00001f;

// TODO This function has been de-inlined as there is a bug in the SPU compiler that causes compilation time to
// increase exponentially for complicated buildConnections() functions.
Junction* Junction::init(
  NMP::Memory::Resource& resource,
  uint32_t numEdges,
  ER::Junction::CombineType NMP_USED_FOR_ASSERTS(combineType))
{
  NMP_ASSERT(numEdges > 0);
  resource.align(NMP_NATURAL_TYPE_ALIGNMENT);
  Junction* result = (Junction*)resource.ptr;
  resource.increment(getMemoryRequirements(numEdges));
#ifdef NMP_ENABLE_ASSERTS
  result->m_combineType = combineType;
#endif
  result->m_numEdges = numEdges;
  return result;
}

Junction* Junction::relocate(NMP::Memory::Resource& resource)
{
  resource.align(NMP_NATURAL_TYPE_ALIGNMENT);
  Junction* result = (Junction*)resource.ptr;
  resource.increment(getMemoryRequirements(result->m_numEdges));
  return result;
}
} // namespace ER

//----------------------------------------------------------------------------------------------------------------------
