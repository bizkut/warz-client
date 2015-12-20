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
#ifndef ER_NODE_BEHAVIOUR_GROUPER_H
#define ER_NODE_BEHAVIOUR_GROUPER_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
#include "morpheme/mrNetworkDef.h"
//----------------------------------------------------------------------------------------------------------------------

namespace ER
{

//----------------------------------------------------------------------------------------------------------------------
MR::NodeID nodeBehaviourGrouperFindGeneratingNodeForSemantic(
  MR::NodeID              callingNodeID,
  bool                    fromParent,   // Was this query from a parent or child node.
  MR::AttribDataSemantic  semantic,
  MR::NodeDef*            node,
  MR::Network*            network);
} // namespace ER

//----------------------------------------------------------------------------------------------------------------------
#endif // ER_NODE_BEHAVIOUR_GROUPER_H
//----------------------------------------------------------------------------------------------------------------------
