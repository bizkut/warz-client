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
#include "simpleBundle/simpleAnimRuntimeIDtoFilenameLookup.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

namespace UTILS
{

//----------------------------------------------------------------------------------------------------------------------
SimpleAnimRuntimeIDtoFilenameLookup* SimpleAnimRuntimeIDtoFilenameLookup::init(
  NMP::Memory::Resource& resource,
  uint32_t               numAnims,
  uint32_t*              animCRCs,
  char**                 fileNames,
  char**                 animFormats,
  char**                 sourceFilenames,
  char**                 sourceTakenames)
{
  resource.align(NMP::Memory::Format(sizeof(SimpleAnimRuntimeIDtoFilenameLookup), NMP_NATURAL_TYPE_ALIGNMENT));
  SimpleAnimRuntimeIDtoFilenameLookup* result = (SimpleAnimRuntimeIDtoFilenameLookup*)resource.ptr;
  resource.increment(sizeof(SimpleAnimRuntimeIDtoFilenameLookup));

  result->m_animIDFilenamesTable = NMP::OrderedStringTable::init(resource, numAnims, fileNames);
  result->m_animIDAnimFormatTable = NMP::OrderedStringTable::init(resource, numAnims, animFormats);
  result->m_animIDSourceFilenamesTable = NMP::OrderedStringTable::init(resource, numAnims, sourceFilenames);
  result->m_animIDSourceTakenamesTable = NMP::OrderedStringTable::init(resource, numAnims, sourceTakenames);
  result->m_animIDFileCRCTable = (uint32_t*)resource.ptr;
  NMP::Memory::memcpy(result->m_animIDFileCRCTable, animCRCs, numAnims * sizeof(uint32_t));

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format SimpleAnimRuntimeIDtoFilenameLookup::getMemoryRequirements(
  uint32_t numAnims,
  char** fileNames,
  char** animFormats,
  char** sourceFilenames,
  char** sourceTakenames)
{
  NMP::Memory::Format result(0, NMP_NATURAL_TYPE_ALIGNMENT);

  // Reserve space for the NetworkDef class itself
  result.size += NMP::Memory::align(sizeof(SimpleAnimRuntimeIDtoFilenameLookup), NMP_NATURAL_TYPE_ALIGNMENT);

  // reserve space for each of the string tables
  result += NMP::OrderedStringTable::getMemoryRequirements(numAnims, fileNames);
  result += NMP::OrderedStringTable::getMemoryRequirements(numAnims, animFormats);
  result += NMP::OrderedStringTable::getMemoryRequirements(numAnims, sourceFilenames);
  result += NMP::OrderedStringTable::getMemoryRequirements(numAnims, sourceTakenames);
  // CRC list
  result.size += sizeof(uint32_t) * numAnims;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format SimpleAnimRuntimeIDtoFilenameLookup::getInstanceMemoryRequirements()
{
  NMP::Memory::Format result(NMP::Memory::align(sizeof(SimpleAnimRuntimeIDtoFilenameLookup), NMP_NATURAL_TYPE_ALIGNMENT), NMP_NATURAL_TYPE_ALIGNMENT);

  result += m_animIDFilenamesTable->getInstanceMemoryRequirements();
  result += m_animIDAnimFormatTable->getInstanceMemoryRequirements();
  result += m_animIDSourceFilenamesTable->getInstanceMemoryRequirements();
  result += m_animIDSourceTakenamesTable->getInstanceMemoryRequirements();
  result.size += sizeof(uint32_t) * m_animIDFilenamesTable->getNumEntries();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleAnimRuntimeIDtoFilenameLookup::locate()
{
  NMP::endianSwap(m_animIDFilenamesTable);
  REFIX_PTR(NMP::OrderedStringTable, m_animIDFilenamesTable);
  m_animIDFilenamesTable->locate();

  NMP::endianSwap(m_animIDAnimFormatTable);
  REFIX_PTR(NMP::OrderedStringTable, m_animIDAnimFormatTable);
  m_animIDAnimFormatTable->locate();

  NMP::endianSwap(m_animIDSourceFilenamesTable);
  REFIX_PTR(NMP::OrderedStringTable, m_animIDSourceFilenamesTable);
  m_animIDSourceFilenamesTable->locate();

  NMP::endianSwap(m_animIDSourceTakenamesTable);
  REFIX_PTR(NMP::OrderedStringTable, m_animIDSourceTakenamesTable);
  m_animIDSourceTakenamesTable->locate();

  NMP::endianSwap(m_animIDFileCRCTable);
  REFIX_PTR(uint32_t, m_animIDFileCRCTable);
  NMP::endianSwapArray(m_animIDFileCRCTable, m_animIDFilenamesTable->getNumEntries());

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleAnimRuntimeIDtoFilenameLookup::dislocate()
{
  NMP::endianSwapArray(m_animIDFileCRCTable, m_animIDFilenamesTable->getNumEntries());
  UNFIX_PTR(uint32_t, m_animIDFileCRCTable);
  NMP::endianSwap(m_animIDFileCRCTable);

  m_animIDFilenamesTable->dislocate();
  UNFIX_PTR(NMP::OrderedStringTable, m_animIDFilenamesTable);
  NMP::endianSwap(m_animIDFilenamesTable);

  m_animIDAnimFormatTable->dislocate();
  UNFIX_PTR(NMP::OrderedStringTable, m_animIDAnimFormatTable);
  NMP::endianSwap(m_animIDAnimFormatTable);

  m_animIDSourceFilenamesTable->dislocate();
  UNFIX_PTR(NMP::OrderedStringTable, m_animIDSourceFilenamesTable);
  NMP::endianSwap(m_animIDSourceFilenamesTable);

  m_animIDSourceTakenamesTable->dislocate();
  UNFIX_PTR(NMP::OrderedStringTable, m_animIDSourceTakenamesTable);
  NMP::endianSwap(m_animIDSourceTakenamesTable);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* SimpleAnimRuntimeIDtoFilenameLookup::getFilename(uint32_t id) const
{
  return m_animIDFilenamesTable->getStringForID(id);
}

//----------------------------------------------------------------------------------------------------------------------
const char* SimpleAnimRuntimeIDtoFilenameLookup::getFormatType(uint32_t id) const
{
  return m_animIDAnimFormatTable->getStringForID(id);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleAnimRuntimeIDtoFilenameLookup::getCRC(uint32_t id) const
{
  NMP_ASSERT_MSG(id < m_animIDFilenamesTable->getNumEntries(), "Invalid usage: index %u out of bounds!", id);
  return m_animIDFileCRCTable[id];
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleAnimRuntimeIDtoFilenameLookup::getNumAnims() const
{
  return m_animIDFilenamesTable->getNumEntries();
}

} // namespace UTILS
} // namespace MR

//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
