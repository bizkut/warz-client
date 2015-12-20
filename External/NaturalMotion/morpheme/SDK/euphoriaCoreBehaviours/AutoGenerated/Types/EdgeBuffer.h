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

#ifndef NM_MDF_TYPE_EDGEBUFFER_H
#define NM_MDF_TYPE_EDGEBUFFER_H

// include definition file to create project dependency
#include "./Definition/Types/Hold.types"

// known types
#include "Types/Edge.h"

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


//----------------------------------------------------------------------------------------------------------------------
// Declaration from 'Hold.types'
// Data Payload: 400 Bytes
// Alignment: 16

//----------------------------------------------------------------------------------------------------------------------
// Ring buffer for edges. Optionally used by the GrabDetection module to "memorise" edges that are otherwise
// forgotten by the EA system, even though they might still be valid as targets for the Hold behaviour.
// Pushes new edges to the top, and pops old edges from the bottom.
//----------------------------------------------------------------------------------------------------------------------

struct EdgeBuffer
{


  // functions

  EdgeBuffer();
  /// Adds a new edge.
  bool push(Edge& edge, bool avoidDuplicates);
  /// Removes oldest edge.
  bool pop();
  /// Replaces edge with lowest quality ("grabbability") with new one.
  bool priorityInsert(Edge& edge);
  bool isEmpty() const;
  bool isFull() const;
  uint8_t size() const;
  /// Returns newest edge.
  Edge* first();
  /// Returns oldest edge.
  Edge* last();
  /// Random access to edges.
  Edge* get(uint8_t i);
  /// Returns index of edge if it already exists in the buffer, otherwise -1.
  int8_t find(const Edge& edge) const;
  /// Removes all the edges with quality ("grabbability") less than minQuality.
  void cullLowQuality(float minQuality);
  /// Removes all the edges that belong to the shape with specified ID.
  void cullShapeID(int64_t edgeShapeID);
  void debugDraw(MR::InstanceDebugInterface* pDebugDrawInst) const;

private:


  enum Constants
  {
    /*  4 */ MAX_SIZE = 4,  
  };

  Edge m_edges[EdgeBuffer::MAX_SIZE];
 // 384 Bytes 
  uint8_t m_pushIndex;

  uint8_t m_popIndex;

  uint8_t m_size;


  // functions

  /// Remove edge by identifier.
  void remove(uint8_t i);

public:


}; // struct EdgeBuffer


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_EDGEBUFFER_H

