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
#include "morpheme/mrNodeBin.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Reduce the ref count to an AttribData and if zero delete the AttribData.
void NodeBinEntry::removeReference()
{
  // We are deleting a reference to an AttribData so reduce the refCount.
  MR::AttribData* attribData = getAttribData();

  NMP_ASSERT(attribData);

  if (attribData->refCountDecrease() == 0)
  {
    // Only delete the AttribData itself if its RefCount has gone to zero.
    NMP::MemoryAllocator* allocator = attribData->m_allocator;
    if (allocator)
    {
      allocator->memFree(attribData);
    }
    m_attribDataHandle.m_attribData = NULL;
  }
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
