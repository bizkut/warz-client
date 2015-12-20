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
#ifndef MR_NODE_OPERATOR_RATE_OF_CHANGE_H
#define MR_NODE_OPERATOR_RATE_OF_CHANGE_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// OutputCP op declarations.

// Critically damp the incoming float value. (User specified node time value controls rate of damping).
AttribData* nodeOperatorRateOfChangeFloat(
  NodeDef* node,
  PinIndex outputCPPinIndex, ///< The output pin we have been asked to update.
  Network* net);

// Critically damp the incoming vector value. (User specified node time value controls rate of damping).
AttribData* nodeOperatorRateOfChangeVector(
  NodeDef* node,
  PinIndex outputCPPinIndex, ///< The output pin we have been asked to update.
  Network* net);

//----------------------------------------------------------------------------------------------------------------------
void nodeOperatorRateOfChangeFloatInitInstance(
  NodeDef* node,
  Network* net);

//----------------------------------------------------------------------------------------------------------------------
void nodeOperatorRateOfChangeVectorInitInstance(
  NodeDef* node,
  Network* net);

//----------------------------------------------------------------------------------------------------------------------
// Enumerating the input and output control parameter pins.
enum NodeOperatorRateOfChangeInCPPinIDs
{
  NODE_OPERATOR_RATE_OF_CHANGE_IN_CP_PINID_FLOAT = 0,
  NODE_OPERATOR_RATE_OF_CHANGE_IN_CP_PINID_VECTOR,
  NODE_OPERATOR_RATE_OF_CHANGE_IN_CP_PINID_COUNT
};

enum NodeOperatorRateOfChangeOutCPPinIDs
{
  NODE_OPERATOR_RATE_OF_CHANGE_OUT_CP_PINID_FLOAT = 0,
  NODE_OPERATOR_RATE_OF_CHANGE_OUT_CP_PINID_VECTOR,
  NODE_OPERATOR_RATE_OF_CHANGE_OUT_CP_PINID_COUNT
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_OPERATOR_RATE_OF_CHANGE_H
//----------------------------------------------------------------------------------------------------------------------
