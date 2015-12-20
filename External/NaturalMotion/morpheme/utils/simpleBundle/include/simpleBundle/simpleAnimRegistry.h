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
#ifndef MR_UTILS_SIMPLEANIMREGISTRY_H
#define MR_UTILS_SIMPLEANIMREGISTRY_H

#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"

//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

class AnimSourceBase;

namespace UTILS
{

class EntryList;

class SimpleAnimRegistry
{
public:

  class Entry
  {
  public:
    Entry(MR::AnimSourceBase* animSource, const char* filename);
    ~Entry();

    void incRefCount() { ++m_refCount; }
    void decRefCount() { NMP_ASSERT(m_refCount > 0); --m_refCount; }
    uint32_t getRefCount() const { return m_refCount; }
    MR::AnimSourceBase* getAnim() const { return m_anim; }
    const char* getAnimFilename() const { return m_filename; }

  protected:
    uint32_t             m_refCount;
    MR::AnimSourceBase*  m_anim;
    char*                m_filename;
  };

  SimpleAnimRegistry();
  ~SimpleAnimRegistry();

  /// \brief
  const Entry* getEntry(MR::AnimSourceBase* anim);

  /// \brief
  const Entry* getEntry(const char* filename);

  /// \brief
  const Entry* addEntry(const char* filename, MR::AnimSourceBase* anim);

  /// \brief
  void removeEntry(Entry* e);

  /// \brief
  void addReferenceToEntry(SimpleAnimRegistry::Entry* e);

  /// \brief
  bool removeReferenceFromEntry(SimpleAnimRegistry::Entry* e);

  /// \brief
  size_t getNumEntries() const;

protected:

  EntryList*  m_entries;
};

} // namespace UTILS

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_UTILS_SIMPLEANIMREGISTRY_H
//----------------------------------------------------------------------------------------------------------------------
