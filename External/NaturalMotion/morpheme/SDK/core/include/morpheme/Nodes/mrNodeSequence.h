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
#ifndef MR_NODE_SEQUENCE_H
#define MR_NODE_SEQUENCE_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
void nodeSequenceAndSwitchDeleteInstance(
  const NodeDef* node,
  Network*       net
);

//----------------------------------------------------------------------------------------------------------------------
// Overloaded update connections functions.
NodeID nodeSequenceUpdateConnections(
  NodeDef*           node,
  Network*           net);

//----------------------------------------------------------------------------------------------------------------------
// Helper functions
//----------------------------------------------------------------------------------------------------------------------
void ChangeActiveSourceIndex(
  uint32_t currentSourceNode, // IN: Across any range.
  uint32_t numSourceNodes,   // IN: Number of source nodes to choose from
  uint32_t& newActiveIndex0  // OUT: Index into array of source Nodes.
);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_SEQUENCE_H
//----------------------------------------------------------------------------------------------------------------------
