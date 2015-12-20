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
#ifndef MR_NODE_BLEND_NXM_H
#define MR_NODE_BLEND_NXM_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// BlendNxM: where S0 ... Sn are the flattened input source connections
//  ----------> X
//  | S0 S1 S2
//  | S3 S4 S5
//  | S6 S7 S8
//  V
//  Y
//----------------------------------------------------------------------------------------------------------------------
/// \brief Function that calculates the flat child node index address corresponding to the
/// the X, Y coordinate of the source.
NM_FORCEINLINE
uint16_t nodeBlendNxMGetChildNodeIndex(
  uint16_t activeChildIndexX,
  uint16_t activeChildIndexY,
  uint16_t numSourcesX,
  uint16_t NMP_USED_FOR_ASSERTS(numSourcesY))
{
  NMP_ASSERT(numSourcesX > 0);
  NMP_ASSERT(numSourcesY > 0);
  NMP_ASSERT(activeChildIndexX < numSourcesX);
  NMP_ASSERT(activeChildIndexY < numSourcesY);
  return activeChildIndexY * numSourcesX + activeChildIndexX;
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeBlendNxMUpdateConnections(
  NodeDef*           nodeDef,
  Network*           net);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_BLEND_NXM_H
//----------------------------------------------------------------------------------------------------------------------
