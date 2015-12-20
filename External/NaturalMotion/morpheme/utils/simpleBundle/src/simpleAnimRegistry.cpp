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
#include "simpleBundle/simpleAnimRegistry.h"
#include <list>
//----------------------------------------------------------------------------------------------------------------------

#undef NM_LOG_MESSAGE
#define NM_LOG_MESSAGE(x, y, z, a, b)

namespace MR
{

namespace UTILS
{

//----------------------------------------------------------------------------------------------------------------------
class EntryList : public std::list<SimpleAnimRegistry::Entry*>
{
};

//----------------------------------------------------------------------------------------------------------------------
SimpleAnimRegistry::SimpleAnimRegistry()
: m_entries(new EntryList())
{
}

//----------------------------------------------------------------------------------------------------------------------
SimpleAnimRegistry::~SimpleAnimRegistry()
{
  EntryList::const_iterator it = m_entries->begin();
  EntryList::const_iterator end = m_entries->end();
  while (it != end)
  {
    (*it)->~Entry();
    NMP::Memory::memFree(*it);
    ++it;
  }
  m_entries->clear();
  delete m_entries;
}

//----------------------------------------------------------------------------------------------------------------------
SimpleAnimRegistry::Entry::Entry(MR::AnimSourceBase* animSource, const char* filename) :
  m_refCount(1),
  m_anim(animSource)
{
  size_t len = NMP_STRLEN(filename) + 1;
  m_filename = (char*)NMPMemoryAlloc(len);
  NMP_ASSERT(m_filename);
  NMP_STRNCPY_S(m_filename, len, filename);
}

//----------------------------------------------------------------------------------------------------------------------
SimpleAnimRegistry::Entry::~Entry()
{
  NMP::Memory::memFree(m_filename);
}

//----------------------------------------------------------------------------------------------------------------------
const SimpleAnimRegistry::Entry* SimpleAnimRegistry::getEntry(MR::AnimSourceBase* anim)
{
  EntryList::const_iterator it = m_entries->begin();
  EntryList::const_iterator end = m_entries->end();
  while (it != end)
  {
    if ((*it)->getAnim() == anim)
      return *it;
    ++it;
  }
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
const SimpleAnimRegistry::Entry* SimpleAnimRegistry::getEntry(const char* filename)
{
  EntryList::const_iterator it = m_entries->begin();
  EntryList::const_iterator end = m_entries->end();
  while (it != end)
  {
    if (NMP_STRCMP((*it)->getAnimFilename(), filename) == 0)
      return *it;
    ++it;
  }
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
const SimpleAnimRegistry::Entry* SimpleAnimRegistry::addEntry(const char* filename, MR::AnimSourceBase* anim)
{
  NMP_ASSERT(!getEntry(filename));
  NMP_ASSERT(!getEntry(anim));
  Entry* e = (Entry*)NMPMemoryAlloc(sizeof(SimpleAnimRegistry::Entry));
  NMP_ASSERT(e);
  new(e) Entry(anim, filename);
  m_entries->push_back(e);
  return e;
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleAnimRegistry::removeEntry(SimpleAnimRegistry::Entry* e)
{
  NMP_ASSERT(e->getRefCount() == 0);
  m_entries->remove(e);
  e->~Entry();
  NMP::Memory::memFree(e);
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleAnimRegistry::addReferenceToEntry(SimpleAnimRegistry::Entry* e)
{
  e->incRefCount();
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleAnimRegistry::removeReferenceFromEntry(SimpleAnimRegistry::Entry* e)
{
  e->decRefCount();
  if (e->getRefCount() == 0)
  {
    // Erase from registry.
    removeEntry(e);
    // no longer needed so can be deleted.
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
size_t SimpleAnimRegistry::getNumEntries() const
{
  return m_entries->size();
}

} // namespace UTILS

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
