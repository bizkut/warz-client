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
#include "NMPlatform/NMMemoryAllocator.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// MemoryAllocator functions
//----------------------------------------------------------------------------------------------------------------------
MemoryAllocator::MemoryAllocator()
{
  initStats();
}

//----------------------------------------------------------------------------------------------------------------------
MemoryAllocator::~MemoryAllocator()
{
}

//----------------------------------------------------------------------------------------------------------------------
// HeapAllocator functions
//----------------------------------------------------------------------------------------------------------------------
void* HeapAllocator::memAlloc(size_t size, uint32_t alignment NMP_MEMORY_TRACKING_DECL)
{
  void* result = NMP::Memory::memAllocAligned(size, alignment NMP_MEMORY_TRACKING_PASS_THROUGH);
  size_t allocSize = NMP::Memory::memSize(result);
  logInternalAlloc(allocSize);
  logExternalAlloc(allocSize);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void HeapAllocator::memFree(void* ptr)
{
  size_t size = NMP::Memory::memSize(ptr);
  logExternalFree(size);
  logInternalFree(size);
  NMP::Memory::memFree(ptr);
  return;
}

//----------------------------------------------------------------------------------------------------------------------
bool HeapAllocator::term()
{
  return true;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
