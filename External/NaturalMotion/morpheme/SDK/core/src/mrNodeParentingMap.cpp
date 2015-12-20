// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNodeParentingMap.h"
#include "morpheme/mrManager.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeParentingMap::getMemoryRequirements(MR::NetworkDef* networkDef)
{
  // Reserve memory space for the class itself
  NMP::Memory::Format result(sizeof(NodeParentingMap), NMP_NATURAL_TYPE_ALIGNMENT);

  uint32_t numNodes = networkDef->getNumNodeDefs();

  // Reserve space for the array of pointers to parent node IDs.
  NMP::Memory::Format parentNodeArrayMemoryRequirements(sizeof(MR::NodeID*) * numNodes, NMP_NATURAL_TYPE_ALIGNMENT);
  result += parentNodeArrayMemoryRequirements;
  // Reserve space for the arracy of parent node ID counts.
  NMP::Memory::Format parentNodeCountArrayMemoryRequirements(sizeof(uint32_t) * numNodes, NMP_NATURAL_TYPE_ALIGNMENT);
  result += parentNodeCountArrayMemoryRequirements;


  // Count the total number of child references that we are going to store in this map
  uint32_t totalNodeReferences = 0;
  for(MR::NodeID i = 0; i < numNodes; i++)
  {
    MR::NodeDef* nodeDef = networkDef->getNodeDef(i);

    // We don't count references from transitions
    if(!nodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_TRANSITION))
    {
      for(uint32_t j = 0; j < nodeDef->getNumChildNodes(); j++)
      {
        totalNodeReferences++;
      }
    }
  }

  // Reserve space for the parent node references themselves
  NMP::Memory::Format parentNodeRefMemoryRequirements(sizeof(MR::NodeID) * totalNodeReferences,
    NMP_NATURAL_TYPE_ALIGNMENT);
  result += parentNodeRefMemoryRequirements;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NodeParentingMap* NodeParentingMap::init(NMP::Memory::Resource& res, MR::NetworkDef* networkDef)
{
  NodeParentingMap* result = (NodeParentingMap*)res.ptr;
  res.increment(NMP::Memory::Format(sizeof(NodeParentingMap), NMP_NATURAL_TYPE_ALIGNMENT));
  result->m_networkDef = networkDef;

  uint32_t numNodes = networkDef->getNumNodeDefs();

  NMP::Memory::Format parentNodeArrayMemoryRequirements(sizeof(MR::NodeID*) * numNodes, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_parentNodeIDs = (MR::NodeID**)res.alignAndIncrement(parentNodeArrayMemoryRequirements);
  NMP::Memory::Format parentNodeCountArrayMemoryRequirements(sizeof(uint32_t) * numNodes, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_numParentNodeIDs = (uint32_t*)res.alignAndIncrement(parentNodeCountArrayMemoryRequirements);

  // Wipe down ready to begin filling in the structure.
  memset(result->m_numParentNodeIDs, 0, sizeof(uint32_t) * numNodes);

  // Count the number of times each node is referred to as a child of another
  for(MR::NodeID i = 0; i < numNodes; i++)
  {
    MR::NodeDef* nodeDef = networkDef->getNodeDef(i);

    // We don't count references from transitions, control parameters or operators
    if(!nodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_TRANSITION) &&
       !nodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_CONTROL_PARAM) &&
       !nodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_OPERATOR_NODE))
    {
      for(uint32_t j = 0; j < nodeDef->getNumChildNodes(); j++)
      {
        MR::NodeID childNodeID = nodeDef->getChildNodeID(j);
        result->m_numParentNodeIDs[childNodeID]++;
      }
    }
  }

  // Now that we have the correct number of references to each node, we can set up the arrays of parent node IDs for
  //  each node.
  for (MR::NodeID i = 0; i < numNodes; i++)
  {
    result->m_parentNodeIDs[i] = (MR::NodeID*)res.ptr;
    res.increment(NMP::Memory::Format(result->m_numParentNodeIDs[i] * sizeof(MR::NodeID), sizeof(MR::NodeID)));
  }

  // Wipe down the parent node ID counts again, now we have set up the arrays.  They will be re-set when we walk the
  //  network filling the actual node IDs in.
  memset(result->m_numParentNodeIDs, 0, sizeof(uint32_t) * numNodes);
  
  // Iterate over the network again, filling in the direct parent node IDs for each node
  for(MR::NodeID i = 0; i < numNodes; i++)
  {
    MR::NodeDef* nodeDef = networkDef->getNodeDef(i);

    // We don't count references from transitions, control parameters or operators
    if(!nodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_TRANSITION) &&
      !nodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_CONTROL_PARAM) &&
      !nodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_OPERATOR_NODE))
    {
      for(uint32_t j = 0; j < nodeDef->getNumChildNodes(); j++)
      {
        MR::NodeID childNodeID = nodeDef->getChildNodeID(j);
        MR::NodeID* parentNodeIDs = result->m_parentNodeIDs[childNodeID];
        parentNodeIDs[result->m_numParentNodeIDs[childNodeID]++] = i;
      }
    }
  }

  // Make sure nothing went wrong in construction
  result->validate();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NodeParentingMap* NodeParentingMap::create(MR::NetworkDef *networkDef)
{
  NMP::Memory::Format fmt = getMemoryRequirements(networkDef);
  NMP::Memory::Resource res = NMPMemoryAllocateFromFormat(fmt);
  NodeParentingMap* result = init(res, networkDef);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool NodeParentingMap::locate()
{
  // Fixup the reference to the NetworkDef
  NMP::endianSwap(m_networkDef);
  m_networkDef = (MR::NetworkDef*)Manager::getInstance().getObjectPtrFromObjectID((ObjectID)m_networkDef);

  uint32_t numNodes = m_networkDef->getNumNodeDefs();

  // Fixup the array of parent node ID counts.
  REFIX_SWAP_PTR(uint32_t, m_numParentNodeIDs);
  NMP::endianSwapArray(m_numParentNodeIDs, numNodes, sizeof(uint32_t));

  // Refix the array of parent node ID arrays.
  REFIX_SWAP_PTR(NodeID*, m_parentNodeIDs);
  for(uint32_t i = 0; i < numNodes; i++)
  {
    REFIX_SWAP_PTR(NodeID, m_parentNodeIDs[i]);
    NMP::endianSwapArray(m_parentNodeIDs[i], m_numParentNodeIDs[i], sizeof(MR::NodeID));
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool NodeParentingMap::dislocate()
{
  uint32_t numNodes = m_networkDef->getNumNodeDefs();

  // Unfix the array of parent node ID arrays.
  for(uint32_t i = 0; i < numNodes; i++)
  {
    NMP::endianSwapArray(m_parentNodeIDs[i], m_numParentNodeIDs[i], sizeof(MR::NodeID));
    UNFIX_SWAP_PTR(NodeID, m_parentNodeIDs[i]);
  }
  UNFIX_SWAP_PTR(NodeID*, m_parentNodeIDs);

  // Unfix the array of parent node ID counts.
  NMP::endianSwapArray(m_numParentNodeIDs, numNodes, sizeof(uint32_t));
  UNFIX_SWAP_PTR(uint32_t, m_numParentNodeIDs);

  // Unfix the reference to the NetworkDef.
  m_networkDef = (MR::NetworkDef*)Manager::getInstance().getObjectIDFromObjectPtr(m_networkDef);
  NMP_ASSERT((ObjectID)m_networkDef != INVALID_OBJECT_ID);
  NMP::endianSwap(m_networkDef);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool NodeParentingMap::validate()
{
  uint32_t numNodes = m_networkDef->getNumNodeDefs();

  // Validation - we make sure that any nodes which don't have exactly one parent are of the expected type.
  for (MR::NodeID i = 0; i < numNodes; i++)
  {
    MR::NodeDef* nodeDef = m_networkDef->getNodeDef(i);

    if(m_numParentNodeIDs[i] > 1)
    {
      NMP_ASSERT_MSG(nodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_OUTPUT_REFERENCED),
        "Non-referenced node found with more than one parent!");

      if(!nodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_OUTPUT_REFERENCED))
      {
        return false;
      }
    }

    if(m_numParentNodeIDs[i] == 0)
    {
      bool shouldHaveParentRefs = true;
      if(i == 0)
      {
        // It's ok for the network node not to have a reference
        shouldHaveParentRefs = false;
      }
      else if(nodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_CONTROL_PARAM) ||
              nodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_OPERATOR_NODE))
      {
        shouldHaveParentRefs = false;
      }

      NMP_ASSERT_MSG(!shouldHaveParentRefs, "An unexpected node was found with zero parents\n");

      if(shouldHaveParentRefs)
      {
        return false;
      }
    }
  }

  // We made it through the whole object without finding any errors.
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace MR
//----------------------------------------------------------------------------------------------------------------------
