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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef MR_NODE_PARENTING_INFO_H
#define MR_NODE_PARENTING_INFO_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"
#include "morpheme/mrNetworkDef.h"

//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
//----------------------------------------------------------------------------------------------------------------------
/// \class MR::NodeParentingMap
/// \brief Provides a fast way to retrieve the set of nodes which are referencing a node as a direct child.
///
/// Nodes in the network store a reference to their "true parent", which may not always be the next node up 
///  the chain towards the root.  For example, when a node is used as an input to a multiply-connected passdown pin, 
///  the node's parent node will be the one directly above the state machine using that MCPDP.  This causes a problem 
///  when node parent/child relationships need to be calculated.  The NodeParentingMap allows rapid access to the set
///  of nodes which are directly connected to any node from above.  It can be used to efficiently iterate from any node
///  in the network towards the root.  When used in conjunction with the NetworkDef, it is possible to navigate around
///  connected nodes in the network.
//----------------------------------------------------------------------------------------------------------------------
class NodeParentingMap
{
public:
  /// \brief Calculate required memory to generate a NodeParentingMap for the specified NetworkDef.
  static NMP::Memory::Format getMemoryRequirements(MR::NetworkDef* networkDef);
  /// \brief Create a NodeParentingMap from the supplied NetworkDef using the a pre-allocated resource.
  static NodeParentingMap* init(NMP::Memory::Resource& res, MR::NetworkDef* networkDef);
  /// \brief Convenience function to allocate memory for and then create a NodeParentingMap.
  static NodeParentingMap* create(MR::NetworkDef *networkDef);

  /// \brief Check that this map is valid.
  bool validate();

  /// \brief Fix the NodeParentingMap in it's current memory address.
  bool locate();
  /// \brief Unfix the NodeParentingMap from it's current memory address.
  bool dislocate();

  /// \brief Retrieve the list of parent node IDs for the specified node.
  NM_INLINE MR::NodeID* getParentNodeIDsForNode(MR::NodeID nodeID);
  /// \brief Retrieve the number of parent node IDs for the specified node.
  NM_INLINE uint32_t getNumParentNodeIDsForNode(MR::NodeID nodeID);
  /// \brief Retrieve the NetworkDef which this NodeParentingMap was created from.
  NM_INLINE MR::NetworkDef* getOwningNetworkDef() {return m_networkDef;};

protected:

  MR::NetworkDef* m_networkDef;
  MR::NodeID** m_parentNodeIDs;
  uint32_t* m_numParentNodeIDs;
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE MR::NodeID* NodeParentingMap::getParentNodeIDsForNode(MR::NodeID nodeID)
{
  NMP_ASSERT_MSG(nodeID < m_networkDef->getNumNodeDefs(), "Invalid node ID passed to getParentNodeIDsForNode");
  return m_parentNodeIDs[nodeID];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NodeParentingMap::getNumParentNodeIDsForNode(MR::NodeID nodeID)
{
  NMP_ASSERT_MSG(nodeID < m_networkDef->getNumNodeDefs(), "Invalid node ID passed to getNumParentNodeIDsForNode");
  return m_numParentNodeIDs[nodeID];
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace MR
//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_PARENTING_INFO_H
//----------------------------------------------------------------------------------------------------------------------
