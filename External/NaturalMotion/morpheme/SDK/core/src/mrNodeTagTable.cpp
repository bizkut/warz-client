// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNodeTagTable.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeTagTable::getMemoryRequirements(
  uint16_t tagCharCount,
  const uint16_t* tagEntryLengths,
  uint16_t numNodes)
{
  NMP::Memory::Format result(sizeof(NodeTagTable));
  result += NMP::Memory::Format(tagCharCount);
  result += NMP::Memory::Format(sizeof(uint16_t) * numNodes); // m_tagEntryLengths
  if (tagCharCount > 0)
  {
    result += NMP::Memory::Format(sizeof(uint16_t*) * numNodes); // m_tagEntries
    for (uint16_t i = 0; i < numNodes; ++i)
    {
      result += NMP::Memory::Format(sizeof(uint16_t) * tagEntryLengths[i]);
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeTagTable::dislocate()
{
  UNFIX_SWAP_PTR(char, m_tags);

  for (uint16_t i = 0; i < m_numNodes; ++i)
  {
    if (m_tagListLengths[i] > 0)
    {
      NMP::endianSwapArray(m_tagEntries[i], m_tagListLengths[i], sizeof(uint16_t));
      UNFIX_SWAP_PTR(uint16_t, m_tagEntries[i]);
    }
  }
  NMP::endianSwapArray(m_tagListLengths, m_numNodes, sizeof(uint16_t));
  UNFIX_SWAP_PTR(uint16_t, m_tagListLengths);

  if (m_tagEntries)
  {
    UNFIX_SWAP_PTR(uint16_t*, m_tagEntries);
  }

  NMP::endianSwap(m_tagCharCount);
  NMP::endianSwap(m_numNodes);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeTagTable::locate()
{
  NMP::endianSwap(m_tagCharCount);
  NMP::endianSwap(m_numNodes);

  if (m_tagEntries)
  {
    REFIX_SWAP_PTR(uint16_t*, m_tagEntries);
  }

  REFIX_SWAP_PTR(uint16_t, m_tagListLengths);

  NMP::endianSwapArray(m_tagListLengths, m_numNodes, sizeof(uint16_t));
  for (uint16_t i = 0; i < m_numNodes; ++i)
  {
    if (m_tagListLengths[i] > 0)
    {
      REFIX_SWAP_PTR(uint16_t, m_tagEntries[i]);
      NMP::endianSwapArray(m_tagEntries[i], m_tagListLengths[i], sizeof(uint16_t));
    }
  }

  REFIX_SWAP_PTR(char, m_tags);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeTagTable::relocate()
{
  NMP::Memory::Resource resource = {this, NMP::Memory::Format(0xFFFFFFFF)};
  resource.alignAndIncrement(NMP::Memory::Format(sizeof(NodeTagTable)));
  m_tags = (char*)resource.alignAndIncrement(NMP::Memory::Format(m_tagCharCount));
  m_tagListLengths = (uint16_t*)resource.alignAndIncrement(NMP::Memory::Format(sizeof(uint16_t) * m_numNodes));
  if (m_tagCharCount > 0)
  {
    m_tagEntries = (uint16_t**)resource.alignAndIncrement(NMP::Memory::Format(sizeof(uint16_t*) * m_numNodes));
    for (uint16_t i = 0; i < m_numNodes; ++i)
    {
      m_tagEntries[i] = (uint16_t*)resource.alignAndIncrement(NMP::Memory::Format(sizeof(uint16_t) * m_tagListLengths[i]));
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeTagTable::getInstanceMemoryRequirements() const
{
  NMP::Memory::Format result(sizeof(NodeTagTable));

  result += NMP::Memory::Format(m_tagCharCount); // m_tags
  result += NMP::Memory::Format(sizeof(uint16_t) * m_numNodes); // m_tagEntryLengths
  if (m_tagCharCount > 0)
  {
    result += NMP::Memory::Format(sizeof(uint16_t*) * m_numNodes); // m_tagEntries
    for (uint16_t i = 0; i < m_numNodes; ++i)
    {
      result += NMP::Memory::Format(sizeof(uint16_t) * m_tagListLengths[i]);
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NodeTagTable* NodeTagTable::init(
  NMP::Memory::Resource& resource,
  const char* tags,
  uint16_t tagCharCount,
  const uint16_t* const* nodesTagsIndices,
  const uint16_t* nodesTagsCount,
  uint16_t numNodes)
{
  NMP::Memory::Format format(sizeof(NodeTagTable));
  NodeTagTable* result = (NodeTagTable*)resource.alignAndIncrement(format);

  // Allocate and initialise the tags
  result->m_tagCharCount = tagCharCount;
  if (tagCharCount > 0)
  {
    format.size = tagCharCount;
    result->m_tags = (char*)resource.alignAndIncrement(format);
    memcpy(result->m_tags, tags, tagCharCount);
  }
  else
  {
    result->m_tags = NULL;
  }

  // Allocate and initialise the tag entries
  result->m_numNodes = numNodes;
  format.size = sizeof(uint16_t) * numNodes;
  result->m_tagListLengths = (uint16_t*)resource.alignAndIncrement(format);
  if (tagCharCount > 0)
  {
    format.size = sizeof(uint16_t*) * numNodes;
    result->m_tagEntries = (uint16_t**)resource.alignAndIncrement(format);
    for (NodeID tagIndex = 0; tagIndex < numNodes; ++tagIndex)
    {
      const uint16_t numTagEntries = nodesTagsCount[tagIndex];
      result->m_tagListLengths[tagIndex] = numTagEntries;

      if (numTagEntries > 0)
      {
        format.size = sizeof(uint16_t) * numTagEntries;
        result->m_tagEntries[tagIndex] = (uint16_t*)resource.alignAndIncrement(format);
        for (NodeID tagEntryIndex = 0; tagEntryIndex < numTagEntries; ++tagEntryIndex)
        {
          result->m_tagEntries[tagIndex][tagEntryIndex] = nodesTagsIndices[tagIndex][tagEntryIndex];
        }
      }
      else
      {
        result->m_tagEntries[tagIndex] = NULL;
      }
    }
  }
  else
  {
    for (NodeID tagIndex = 0; tagIndex < numNodes; ++tagIndex)
    {
      result->m_tagListLengths[tagIndex] = 0;
    }
    result->m_tagEntries = NULL;
  }

  return result;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
