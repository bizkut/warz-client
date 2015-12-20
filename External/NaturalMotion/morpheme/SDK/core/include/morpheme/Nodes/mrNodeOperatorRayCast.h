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
#ifndef MR_NODE_OPERATOR_RAY_CAST_H
#define MR_NODE_OPERATOR_RAY_CAST_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// OutputCP op declarations.

// This function updates all output pins, but returns the value of the requested pin.
AttribData* nodeOperatorRayCastOutputCPUpdate(
  NodeDef* node,
  PinIndex outputCPPinIndex, ///< The output pin we have been asked to update.
  Network* net);

//----------------------------------------------------------------------------------------------------------------------
void nodeOperatorRayCastInitInstance(
  NodeDef*               node,
  Network*               net);

//----------------------------------------------------------------------------------------------------------------------
// Enumerating the output control parameter pins.
enum NodeOperatorRayCastOutCPPinIDs
{
  NODE_OPERATOR_RAY_CAST_OUT_CP_PINID_DISTANCE = 0,
  NODE_OPERATOR_RAY_CAST_OUT_CP_PINID_POSITION,
  NODE_OPERATOR_RAY_CAST_OUT_CP_PINID_HIT_NORMAL,
  NODE_OPERATOR_RAY_CAST_OUT_CP_PINID_HIT_PITCH_DOWN_ANGLE,
  NODE_OPERATOR_RAY_CAST_OUT_CP_PINID_HIT_ROLL_RIGHT_ANGLE,
  NODE_OPERATOR_RAY_CAST_OUT_CP_PINID_COUNT
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_OPERATOR_RAY_CAST_H
//----------------------------------------------------------------------------------------------------------------------
