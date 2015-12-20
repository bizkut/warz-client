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
#ifndef MCOMMS_SCOPEDMEMORY_H
#define MCOMMS_SCOPEDMEMORY_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
namespace MCOMMS
{

//----------------------------------------------------------------------------------------------------------------------
/// \class ScopedMemory
/// \brief Helper class that frees a bit of memory as it goes out of scope.
//----------------------------------------------------------------------------------------------------------------------
class ScopedMemory
{
public:

  explicit ScopedMemory(void* const address);

  /// \brief Frees the block of memory using NMP::Memory::memFree().
  ~ScopedMemory();

  /// \brief Returns the address of the memory block held.
  void* getAddress() const;

  /// \brief Returns the address of the memory block held for use with pointer arithmetic.
  uint8_t* asByteArray() const;

  /// \brief Cast operator to allow substituting for a regular pointer.
  operator void* () const;

private:

  void* const m_address;

  ScopedMemory(const ScopedMemory&);
  ScopedMemory& operator=(const ScopedMemory&);
};

//----------------------------------------------------------------------------------------------------------------------
inline ScopedMemory::ScopedMemory(void* const address)
: m_address(address)
{
}

//----------------------------------------------------------------------------------------------------------------------
inline ScopedMemory::~ScopedMemory()
{
  NMP::Memory::memFree(m_address);
}

//----------------------------------------------------------------------------------------------------------------------
inline void* ScopedMemory::getAddress() const
{
  return m_address;
}

//----------------------------------------------------------------------------------------------------------------------
inline uint8_t* ScopedMemory::asByteArray() const
{
  return static_cast<uint8_t*>(m_address);
}

//----------------------------------------------------------------------------------------------------------------------
inline ScopedMemory::operator void *() const
{
  return getAddress();
}

} // namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_SCOPEDMEMORY_H
//----------------------------------------------------------------------------------------------------------------------
