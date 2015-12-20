
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
#ifndef NM_FLAGS_H
#define NM_FLAGS_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::Flags
/// \brief Class for holding and accessing a 32 bit flag field.
/// \ingroup NaturalMotionPlatform
//----------------------------------------------------------------------------------------------------------------------
class Flags
{
public:
  Flags() {}
  Flags(uint32_t flags) { m_Flags = flags; }

  NM_INLINE void clearAll() { m_Flags = 0; }
  NM_INLINE void setAll() { m_Flags = 0xFFFFFFFFU; }
  NM_INLINE void set(uint32_t flags) { m_Flags |= flags; }
  NM_INLINE void set(Flags flags) { set(flags.m_Flags); }
  NM_INLINE bool areSet(uint32_t flags) const;
  NM_INLINE bool areSet(Flags flags) const;
  NM_INLINE bool isSet(uint32_t flag) const { return ((m_Flags & flag) != 0); }
  NM_INLINE bool isSet(Flags flag) const { return isSet(flag.m_Flags); }
  NM_INLINE bool areAllUnset() const;
  NM_INLINE void clear(uint32_t flags);
  NM_INLINE void clear(Flags flags);
  NM_INLINE void clearThenSet(uint32_t flags) { m_Flags = flags; }
  NM_INLINE void clearThenSet(Flags flags) {  m_Flags = flags.m_Flags; }

  operator uint32_t () const { return m_Flags; }

protected:
  uint32_t m_Flags;
};

//----------------------------------------------------------------------------------------------------------------------
// Flags functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Flags::areSet(uint32_t flags) const
{
  uint32_t working;
  working = m_Flags & flags;
  return (flags == working); // MORPH-21296: LHS
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Flags::areSet(Flags flags) const
{
  return areSet(flags.m_Flags);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Flags::areAllUnset() const
{
  return (m_Flags == 0);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Flags::clear(uint32_t flags)
{
  m_Flags &= (0xFFFFFFFFU ^ flags);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Flags::clear(Flags flags)
{
  clear(flags.m_Flags);
}

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::Flags16
/// \brief Class for holding and accessing a 16 bit flag field.
/// \ingroup NaturalMotionPlatform
//----------------------------------------------------------------------------------------------------------------------
class Flags16
{
public:
  Flags16() {};
  Flags16(uint16_t flags) { m_Flags = flags; }

  NM_INLINE void set(uint16_t flags) { m_Flags |= flags; }
  NM_INLINE void set(Flags16 flags) { set(flags.m_Flags); }
  NM_INLINE bool areSet(uint16_t flags) const;
  NM_INLINE bool areSet(Flags16 flags) const;
  NM_INLINE bool isSet(uint16_t flag) const { return ((m_Flags & flag) != 0); }
  NM_INLINE bool isSet(Flags16 flag) const { return isSet(flag.m_Flags); }

  operator uint16_t () const { return m_Flags; }

protected:
  uint16_t m_Flags;
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Flags16::areSet(uint16_t flags) const
{
  uint16_t working;
  working = m_Flags & flags;
  return (flags == working);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Flags16::areSet(Flags16 flags) const
{
  return areSet(flags.m_Flags);
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_FLAGS_H
//----------------------------------------------------------------------------------------------------------------------
