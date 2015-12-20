#pragma once

/*
 * Copyright (c) 2013 NaturalMotion Ltd. All rights reserved.
 *
 * Not to be copied, adapted, modified, used, distributed, sold,
 * licensed or commercially exploited in any manner without the
 * written consent of NaturalMotion.
 *
 * All non public elements of this software are the confidential
 * information of NaturalMotion and may not be disclosed to any
 * person nor used for any purpose not expressly approved by
 * NaturalMotion in writing.
 *
 */

//----------------------------------------------------------------------------------------------------------------------
//                                  This file is auto-generated
//----------------------------------------------------------------------------------------------------------------------

#ifndef NM_MDF_TYPE_EDGE_H
#define NM_MDF_TYPE_EDGE_H

// include definition file to create project dependency
#include "./Definition/Types/Hold.types"

// external types
#include "NMPlatform/NMVector3.h"

// for combiners
#include "euphoria/erJunction.h"

// constants
#include "NetworkConstants.h"

// hook functions
#include "euphoria/erValueValidators.h"
#include "euphoria/erValuePostCombiners.h"

namespace MR { class InstanceDebugInterface; } 

#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

struct Edge; 


//----------------------------------------------------------------------------------------------------------------------
// Declaration from 'Hold.types'
// Data Payload: 96 Bytes
// Alignment: 16

//----------------------------------------------------------------------------------------------------------------------
// Describes an edge with information required by the hold behaviour.
//----------------------------------------------------------------------------------------------------------------------

struct Edge
{

  NMP::Vector3 corner;  ///< Start of the edge.  (Position)

  NMP::Vector3 edge;  ///< Direction of the edge (left/right hand positioning depends on this).  (PositionDelta)

  NMP::Vector3 uprightNormal;  ///< Normal of the more horizontal surface (the more supporting one).  (Direction)

  NMP::Vector3 otherNormal;  ///< Normal of the less supportive surface.  (Direction)

  NMP::Vector3 point;  ///< A point on the edge. Used as average grab point for the end-effector.  (Position)

  int64_t shapeID;  ///< Cast from pointer.

  float quality;  ///< "Grabbability" of this edge.

  int16_t gameEdgeID;  ///< Index into the list of game specified edges, or -1 if it's not from there

  bool isWall;  ///< True: belongs to wall/box geometry. False: it's a pole or ledge.  


  // functions

  Edge();
  /// Draw edge with "grabbability" colour coding.
  void debugDraw(MR::InstanceDebugInterface* pDebugDrawInst) const;
  /// Do these two instances describe the same edge?
  bool equals(const Edge& other) const;


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(corner), "corner");
    NM_VALIDATOR(Vector3Valid(edge), "edge");
    NM_VALIDATOR(Vector3Normalised(uprightNormal), "uprightNormal");
    NM_VALIDATOR(Vector3Normalised(otherNormal), "otherNormal");
    NM_VALIDATOR(Vector3Valid(point), "point");
    NM_VALIDATOR(FloatValid(quality), "quality");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postCombine()
  {
    PostCombiners::Vector3Normalise(uprightNormal);
    PostCombiners::Vector3Normalise(otherNormal);
    validate();
  }

}; // struct Edge


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_EDGE_H

