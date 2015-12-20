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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef MR_NODE_TAG_TABLE
#define MR_NODE_TAG_TABLE
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
#include "sharedDefines/mSharedDefines.h"

//----------------------------------------------------------------------------------------------------------------------
namespace MR
{

#define TAG_NOT_FOUND 0xFFFF

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::NodeTagTable
/// \brief A node tag table stores a list of tags for each node. Each tag string is stored in a flat array to minimise
/// the memory footprint. Each node has an array of tag indices. Tags can be searched for by string or index. Searching
/// by index is faster but requires the index to be looked up as a pre-process.
/// \ingroup NetworkModule
//----------------------------------------------------------------------------------------------------------------------
class NodeTagTable
{
public:
  /// \brief Calculates the memory requirements of a node tag table.
  static NMP::Memory::Format getMemoryRequirements(
    uint16_t tagCharCount, ///< The number of chars in the tag list
    const uint16_t* nodesTagsCount, ///< A list of array lengths. Each array length represents the number of tags per node.
    uint16_t numNodes ///< The number of nodes that tag lists are stored for.
  );

  /// \brief Dislocates the resource so that it can be relocated in memory.
  void dislocate();

  /// \brief A dislocated resource must be located before it can be used.
  void locate();

  /// \brief Fixes up internal pointers if the resource has been moved.
  void relocate();

  NMP::Memory::Format getInstanceMemoryRequirements() const;

  static NodeTagTable* init(
    NMP::Memory::Resource& resource, ///< The resource containing the memory to be filled by the table.
    const char* tags, ///< A list of tags
    uint16_t numTags, ///< The number of tags in the tag list
    const uint16_t* const* nodesTagsIndices, ///< A jagged array of tag indices. There is an array for each node.
    const uint16_t* nodesTagsCount, ///< A list of array lengths. Each array length represents the number of tags per node.
    uint16_t numNodes ///< The number of nodes that tag lists are stored for.
  );

  /// \brief Checks if a node has a tag using the tag name.
  NM_INLINE bool nodeHasTag(NodeID nodeID, const char* tag) const;

  /// \brief Checks if a node has a tag using a tag index. Tag indices are obtained using findTag.
  NM_INLINE bool nodeHasTag(NodeID nodeID, uint32_t tagIndex) const;

  /// \brief Searches the table for a tag, returning an index. This index can then be used in nodeHasTag to determine if
  /// a tag is present on a node. Searching for tags by index is faster than by string. Returns TAG_NOT_FOUND if the
  /// tag is not used on any node. This function will search through the entire tag list so should be restricted to a
  /// pre-process stage.
  NM_INLINE uint16_t findTag(const char* tag) const;

  /// \brief Find the number of tags that are used by the given node.
  NM_INLINE uint16_t getNumTagsOnNode(NodeID nodeID) const;

  /// \brief Returns the tag for the given node at the given index.  The index is between 0 and the value returned by 
  /// getNumTagsOnNode - it is not the same as the tagIndex return by findTag.  
  NM_INLINE const char* getTagOnNode(NodeID nodeID, uint16_t index) const;

private:
  char* m_tags; ///< List of tags stored in the table.
  uint16_t** m_tagEntries; ///< A jagged array of tag indices, one array per node.
  uint16_t* m_tagListLengths; ///< The length of each tag index array.
  uint16_t m_tagCharCount; ///< The number of chars stored in m_tags.
  uint16_t m_numNodes; ///< The number of nodes in the network def.
};


} // namespace MR

#include "mrNodeTagTable.inl"

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_TAG_TABLE
//----------------------------------------------------------------------------------------------------------------------
