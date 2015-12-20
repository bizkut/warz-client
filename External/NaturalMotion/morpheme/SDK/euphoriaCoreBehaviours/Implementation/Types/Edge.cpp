/*
 * Copyright (c) 2011 NaturalMotion Ltd. All rights reserved.
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

#include "euphoria/erDebugDraw.h"
#include "Types/Edge.h"
#include "Types/EdgeBuffer.h"
#include "Helpers/Helpers.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

#define EDGEBUFFER_PRINT_LOG 0

//----------------------------------------------------------------------------------------------------------------------
Edge::Edge()
{
  corner.setToZero();
  edge.set(1, 0, 0);
  point.set(0, 0, 0);
  uprightNormal.set(0, 1, 0);
  otherNormal.set(0, 0, 1);
  quality = -1;
  shapeID = -1;
  gameEdgeID = -1;
}

//----------------------------------------------------------------------------------------------------------------------
// Same if they belong to same patch, and point in the same direction (a patch can't have parallel edges)
//----------------------------------------------------------------------------------------------------------------------
bool Edge::equals(const Edge& other) const
{
  const float precision = 0.01f;
  if (shapeID == -1)
  {
    // Edge is not attached to a dynamic object
    return corner.compare(other.corner, precision) && edge.compare(other.edge, precision);
  }
  else
  {
    // Edge is attached to a dynamic object
    return (shapeID == other.shapeID) && edge.compare(other.edge, precision);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Edge::debugDraw(MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst)) const
{
#if defined(MR_OUTPUT_DEBUGGING)
  // convert from Vector3 to Colour
  NMP::Colour colour = NMP::Colour(getRainbowColourMapped(quality));
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Force, corner, edge, colour);
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, point, uprightNormal * 0.15f, colour);
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, point, otherNormal * 0.1f, colour);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
EdgeBuffer::EdgeBuffer()
{
  m_pushIndex = 0;
  m_popIndex = 0;
  m_size = 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool EdgeBuffer::isEmpty() const
{
  return m_size == 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool EdgeBuffer::isFull() const
{
  return m_size == MAX_SIZE;
}

//----------------------------------------------------------------------------------------------------------------------
uint8_t EdgeBuffer::size() const
{
  return m_size;
}

//----------------------------------------------------------------------------------------------------------------------
Edge* EdgeBuffer::first()
{
  if (m_size > 0)
  {
    return &m_edges[m_pushIndex - 1];
  }
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
Edge* EdgeBuffer::last()
{
  if (m_size > 0)
  {
    return &m_edges[m_popIndex];
  }
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
bool EdgeBuffer::push(Edge& edge, bool avoidDuplicates)
{
  // if necessary, check whether edge is already in the buffer
  bool doPush = avoidDuplicates ? find(edge) == -1 : true;

  // add new item at the end
  if (doPush)
  {
    m_edges[m_pushIndex] = edge;
    m_pushIndex = (m_pushIndex + 1) % MAX_SIZE;

    // if not full yet, increment size
    if (m_size < MAX_SIZE)
    {
      m_size++;
    }
    // otherwise buffer was full already, in which case the head has to move as well
    else
    {
      m_popIndex = m_pushIndex;
    }
#if EDGEBUFFER_PRINT_LOG
    printf("Pushed Buffer: back %i | front %i | size %i [patch: %i]\n", m_popIndex, m_pushIndex, m_size, edge.shapeID);
#endif
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool EdgeBuffer::pop()
{
  if (m_size > 0)
  {
    m_popIndex = (m_popIndex + 1) % MAX_SIZE;
    m_size--;
#if EDGEBUFFER_PRINT_LOG
    printf("Popped Buffer: back %i | front %i | size %i \n", m_popIndex, m_pushIndex, m_size);
#endif
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
Edge* EdgeBuffer::get(uint8_t i)
{
  if (i < m_size)
  {
    int index = (m_popIndex + i) % MAX_SIZE;
    return &m_edges[index];
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
int8_t EdgeBuffer::find(const Edge& edge) const
{
  // loop over edges and see if it exists already
  for (int8_t i = 0; i < m_size; i++)
  {
    int8_t index = (m_popIndex + i) % MAX_SIZE;
    if (m_edges[index].equals(edge))
    {
      return index;
    }
  }

  // item not already in buffer
  return -1;
}

//----------------------------------------------------------------------------------------------------------------------
bool EdgeBuffer::priorityInsert(Edge& edge)
{
  // don't add if already in buffer (avoid duplicates)
  if (find(edge) >=  0)
    return false;

  if (!isFull())
  {
    return push(edge, true);
  }
  else
  {
    // find worst
    int worstIndex = m_popIndex;
    float worstQuality = 1.0f;
    for (int8_t i = 0; i < m_size; i++)
    {
      int8_t index = (m_popIndex + i) % MAX_SIZE;
      if (m_edges[index].quality < worstQuality)
      {
        worstIndex = index;
        worstQuality = m_edges[index].quality;
      }
    }

    // replace
    if (m_edges[worstIndex].quality < edge.quality)
    {
      m_edges[worstIndex] = edge;
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void EdgeBuffer::cullLowQuality(float minQuality)
{
  for (uint8_t i = 0; i < m_size; )
  {
    Edge* edge = get(i);
    if (edge->quality <= minQuality)
    {
      remove(i);  // Note that m_size is decremented.
    }
    else  // Edge out of interest, continue.
    {
      i++;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void EdgeBuffer::cullShapeID(int64_t edgeShapeID)
{
  for (uint8_t i = 0; i < m_size; )
  {
    Edge* edge = get(i);
    if (edge && (edge->shapeID == edgeShapeID))
    {
      remove(i);  // Note that m_size is decremented.
    }
    else  // Edge out of interest, continue.
    {
      i++;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void EdgeBuffer::remove(uint8_t i)
{
  NMP_ASSERT(i < m_size);
  if (i < m_size)
  {
    const uint8_t numToMove = (m_size - 1) - i;
    for (uint8_t k = 0; k < numToMove ; ++k)
    {
      Edge* e1 = get(i+k);
      Edge* e2 = get(i+k+1);
      *e1 = *e2;
    }
    m_pushIndex = (m_pushIndex + MAX_SIZE - 1) % MAX_SIZE;
    --m_size;
#if EDGEBUFFER_PRINT_LOG
    printf("Removed from Buffer: back %i | front %i | size %i \n", m_popIndex, m_pushIndex, m_size);
#endif
  }
}

//----------------------------------------------------------------------------------------------------------------------
void EdgeBuffer::debugDraw(MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst)) const
{
#if defined(MR_OUTPUT_DEBUGGING)
  for (int i = 0; i < m_size; i++)
  {
    int8_t index = (m_popIndex + i) % MAX_SIZE;
    m_edges[index].debugDraw(pDebugDrawInst);
  }
#endif
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE
