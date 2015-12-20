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
#ifndef MR_NODE_OPERATOR_SMOOTH_FLOAT_H
#define MR_NODE_OPERATOR_SMOOTH_FLOAT_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// OutputCP op declarations.

// Critically damp the incoming float value. (User specified node time value controls rate of damping).
AttribData* nodeOperatorSmoothFloatCriticallyDampFloat(
  NodeDef* node,
  PinIndex outputCPPinIndex, ///< The output pin we have been asked to update.
  Network* net);

// Critically damp the incoming vector value. (User specified node time value controls rate of damping).
AttribData* nodeOperatorSmoothFloatCriticallyDampVector(
  NodeDef* node,
  PinIndex outputCPPinIndex, ///< The output pin we have been asked to update.
  Network* net);

//----------------------------------------------------------------------------------------------------------------------
void nodeOperatorSmoothDampFloatInitInstance(
  NodeDef*               node,
  Network*               net);

//----------------------------------------------------------------------------------------------------------------------
void nodeOperatorSmoothDampVectorInitInstance(
  NodeDef*               node,
  Network*               net);

//----------------------------------------------------------------------------------------------------------------------
// Enumerating the input and output control parameter pins.
enum NodeOperatorSmoothFloatInCPPinIDs
{
  NODE_OPERATOR_SMOOTH_FLOAT_IN_CP_PINID_FLOAT = 0,
  NODE_OPERATOR_SMOOTH_FLOAT_IN_CP_PINID_VECTOR,
  NODE_OPERATOR_SMOOTH_FLOAT_IN_CP_PINID_COUNT
};

enum NodeOperatorSmoothFloatOutCPPinIDs
{
  NODE_OPERATOR_SMOOTH_FLOAT_OUT_CP_PINID_FLOAT = 0,
  NODE_OPERATOR_SMOOTH_FLOAT_OUT_CP_PINID_VECTOR,
  NODE_OPERATOR_SMOOTH_FLOAT_OUT_CP_PINID_COUNT
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_OPERATOR_SMOOTH_FLOAT_H
//----------------------------------------------------------------------------------------------------------------------
