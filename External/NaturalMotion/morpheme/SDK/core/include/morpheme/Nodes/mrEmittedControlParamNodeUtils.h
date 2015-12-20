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
#ifndef MR_EMITTED_CONTROL_PARAM_NODE_UTILS_H
#define MR_EMITTED_CONTROL_PARAM_NODE_UTILS_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Find which one of possibly several potential child paths to update from an emitted control param node.
// Update the found path if there is one, return the resulting AttribData pointer cast to the provided template type.
template <typename T> 
NM_INLINE T* updateEmittedControlParamInputCPConnection(
  NodeDef*  emittedCPNode,
  Network*  net) 
{
  NMP_ASSERT(emittedCPNode->getNumChildNodes() == emittedCPNode->getNumInputCPConnections());

  for (PinIndex i = 0; i < emittedCPNode->getNumChildNodes(); ++i)
  {
    // Only update a connected child path if the associated node is active.
    NMP_ASSERT(emittedCPNode->getChildNodeID(i) < net->getNetworkDef()->getNumNodeDefs());
    NodeBin* nodeBin = net->getNodeBin(emittedCPNode->getChildNodeID(i));
    if (nodeBin->getLastFrameUpdate() == net->getCurrentFrameNo())
    {
      const CPConnection* cpConnection = emittedCPNode->getInputCPConnection(i);

      NMP_ASSERT(cpConnection && (cpConnection->m_sourceNodeID != INVALID_NODE_ID));
      AttribData *result = net->updateOutputCPAttribute(cpConnection->m_sourceNodeID, cpConnection->m_sourcePinIndex, net->getActiveAnimSetIndex());
      NMP_ASSERT(!result || T::getDefaultType() == result->getType());
      return static_cast<T*>(result);
    }
  }

  return NULL;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_EMITTED_CONTROL_PARAM_NODE_UTILS_H
//----------------------------------------------------------------------------------------------------------------------
