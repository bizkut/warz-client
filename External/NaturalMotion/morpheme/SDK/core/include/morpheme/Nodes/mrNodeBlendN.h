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
#ifndef MR_NODE_BLEND_N_H
#define MR_NODE_BLEND_N_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeBlendNFindGeneratingNodeForSemantic(
  NodeID              callingNodeID,
  bool                fromParent,   // Was this query from a parent or child node.
  AttribDataSemantic  semantic,
  NodeDef*            node,
  Network*            network);

//----------------------------------------------------------------------------------------------------------------------
void nodeBlendNCalculateBlendWeightClamped(
  float                 interpolant,            ///< IN: Across any range.
  uint16_t              numSourceNodes,         ///< IN: The number of active source Nodes
  const float*          sourceWeights,          ///< IN: The array of weights (including the wrapping element)
  float&                blendWeight,            ///< OUT: Between the 2 active source Nodes.
  uint16_t&             newActiveIndex0,        ///< OUT: Index into array of source Nodes.
  uint16_t&             newActiveIndex1         ///< OUT: Index into array of source Nodes.
);

//----------------------------------------------------------------------------------------------------------------------
void nodeBlendNCalculateBlendWeightWrapped(
  float                 interpolant,            ///< IN: Across any range.
  uint16_t              numSourceNodes,         ///< IN: The number of active source Nodes
  const float*          sourceWeights,          ///< IN: The array of weights (including the wrapping element)
  float&                blendWeight,            ///< OUT: Between the 2 active source Nodes.
  uint16_t&             newActiveIndex0,        ///< OUT: Index into array of source Nodes.
  uint16_t&             newActiveIndex1         ///< OUT: Index into array of source Nodes.
);

//----------------------------------------------------------------------------------------------------------------------
// Update connections functions.
NodeID nodeBlendNUpdateConnections(
  NodeDef*           node,
  Network*           net);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_BLEND_N_H
//----------------------------------------------------------------------------------------------------------------------
