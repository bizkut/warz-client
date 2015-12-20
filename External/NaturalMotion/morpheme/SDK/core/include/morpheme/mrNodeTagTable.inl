// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeTagTable::nodeHasTag(NodeID nodeID, const char* tag) const
{
  NMP_ASSERT(nodeID < m_numNodes);
  const uint16_t numTagEntries = m_tagListLengths[nodeID];
  for (uint16_t i = 0; i < numTagEntries; ++i)
  {
    const uint16_t tagIndex = m_tagEntries[nodeID][i];
    if (NMP_STRCMP(tag, m_tags + tagIndex) == 0)
    {
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool NodeTagTable::nodeHasTag(NodeID nodeID, uint32_t tagIndex) const
{
  NMP_ASSERT(nodeID < m_numNodes);
  NMP_ASSERT(tagIndex < m_tagCharCount);
  const uint16_t numTagEntries = m_tagListLengths[nodeID];
  for (uint16_t i = 0; i < numTagEntries; ++i)
  {
    const uint16_t tableTagIndex = m_tagEntries[nodeID][i];
    if (tableTagIndex == tagIndex)
    {
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
uint16_t NodeTagTable::findTag(const char* tag) const
{
  for (uint16_t i = 0; i < m_tagCharCount; ++i)
  {
    if ((i == 0 || m_tags[i - 1] == '\0') &&
        (NMP_STRCMP(tag, m_tags + i) == 0))
    {
      return i;
    }
  }

  return TAG_NOT_FOUND;
}

//----------------------------------------------------------------------------------------------------------------------
uint16_t NodeTagTable::getNumTagsOnNode(NodeID nodeID) const
{
  NMP_ASSERT(nodeID < m_numNodes);
  return m_tagListLengths[nodeID];
}

//----------------------------------------------------------------------------------------------------------------------
const char* NodeTagTable::getTagOnNode(NodeID nodeID, uint16_t index) const
{
  NMP_ASSERT(nodeID < m_numNodes);
  NMP_ASSERT(index < m_tagListLengths[nodeID]);
  const uint16_t tagIndex = m_tagEntries[nodeID][index];
  return m_tags + tagIndex;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace MR
