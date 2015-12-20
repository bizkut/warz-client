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
#include "euphoria/Nodes/erNodeBehaviourGrouper.h"
//----------------------------------------------------------------------------------------------------------------------
#ifdef WIN32
  #pragma warning (disable : 4100)
#endif

namespace ER
{

//----------------------------------------------------------------------------------------------------------------------
MR::NodeID nodeBehaviourGrouperFindGeneratingNodeForSemantic(
  MR::NodeID              NMP_UNUSED(callingNodeID),
  bool                    fromParent,   // Was this query from a parent or child node.
  MR::AttribDataSemantic  semantic,
  MR::NodeDef*            node,
  MR::Network*            net)
{
  MR::NodeID result = MR::INVALID_NODE_ID;

  // Pass on to parent or child appropriately.
  if (fromParent)
  {
    if (node->getNumChildNodes() != 0)
    {
      MR::NodeID activeChildNodeID0 = net->getActiveChildNodeID(node->getNodeID(), 0);
      result = net->nodeFindGeneratingNodeForSemantic(activeChildNodeID0, semantic, fromParent, node->getNodeID());
    }
  }
  else
  {
    MR::NodeID activeParentNodeID = net->getActiveParentNodeID(node->getNodeID());
    result = net->nodeFindGeneratingNodeForSemantic(activeParentNodeID, semantic, fromParent, node->getNodeID());
  }

  return result;
}

} // namespace ER

//----------------------------------------------------------------------------------------------------------------------
