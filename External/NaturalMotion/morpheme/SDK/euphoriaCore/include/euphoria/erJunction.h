  #pragma once

// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#ifndef NM_JUNCTION_H
#define NM_JUNCTION_H

#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMMemory.h"

#include "euphoria/erValueValidators.h"

#include <stddef.h>
#include <new>

namespace ER
{

extern const float g_minImportanceForJunction;

// choose closest version of quat since there are two quats per attitude
template<typename T1, typename T2>
NM_INLINE void quatFlip(T1&, const T2&) {}
template<>
NM_INLINE void quatFlip(NMP::Quat& val, const NMP::Quat& accumulator)
{
  if (val.dot(accumulator) < 0.0f)
    val.negate();
}

//----------------------------------------------------------------------------------------------------------------------
/// This is a junction, it uses a particular combine method to combine its inputs
//----------------------------------------------------------------------------------------------------------------------
class Junction
{
public:

  enum CombineType
  {
    invalid = -1,
    directInput = 0,  ///< Uses the single input (if it exists) - asserts there is only one input
    average,          ///< average of each value weighted by its importance
    winnerTakesAll,   ///< value with highest importance is chosen, latter values win over earlier ones
    priority,         ///< latter values blend over previous, weighted by importance...
    sum,              ///< sum of each value, weighted by its importance
    maxFloat,         ///< maximum of each value (float only), weighted by its importance
    maxFloatValue,    ///< maximum of each value ignoring importances (importance output is always 1)
    isBoolSet,        ///< output is true (imp=1) if any input is true (ignoring importance)
  };

  //--------------------------------------------------------------------------------------------------------------------
  struct Edge
  {
    Edge(const char* source, const float* sourceImportance) : m_source(*source), m_sourceImportance(*sourceImportance) {}

    Edge(const Edge& other) : m_source(other.m_source), m_sourceImportance(other.m_sourceImportance) {}

    void reinit(const char* source, const float* sourceImportance)
    {
      new (this) Edge(source, sourceImportance);
    }

    // The source data - needs to be cast to the correct type
    const char& m_source;
    const float& m_sourceImportance;

  private:
    Edge& operator=(const Edge&);   // not needed/implemented
  };

  static Junction* init(NMP::Memory::Resource& resource, uint32_t numEdges, ER::Junction::CombineType combineType);
  static Junction* relocate(NMP::Memory::Resource& resource);

  //--------------------------------------------------------------------------------------------------------------------
  static NM_INLINE NMP::Memory::Format getMemoryRequirements(uint32_t numEdges)
  {
    NMP_ASSERT(numEdges > 0);

    NMP::Memory::Format result(sizeof(Junction), NMP_NATURAL_TYPE_ALIGNMENT);

    result += NMP::Memory::Format(NMP::Memory::align(sizeof(Edge), NMP_NATURAL_TYPE_ALIGNMENT) * numEdges, NMP_NATURAL_TYPE_ALIGNMENT);

    return result;
  }

  // The edges will always be placed immediately after this class in memory so
  // we don't need to explicitly store a pointer to them
  Edge* getEdges() const { return (Edge*)(this + 1); }
  uint32_t    m_numEdges;
#ifdef NMP_ENABLE_ASSERTS
  CombineType m_combineType;
#endif

  NM_INLINE void destroy()
  {
  }

  //--------------------------------------------------------------------------------------------------
  template <class T>
  float combineDirectInput(T* result) const
  {
    NMP_ASSERT(m_combineType == directInput);
    NMP_ASSERT(m_numEdges == 1); // DirectInput isn't going to work on multiple edges

    if (getEdges()->m_sourceImportance > 0.0f)  // Don't memcopy when 0 importance
    {
      *result = (T&)getEdges()->m_source;
    }

    return getEdges()->m_sourceImportance;
  }

  //--------------------------------------------------------------------------------------------------
  // Specialisation for float that avoids branching
  float combineDirectInput(float* result) const
  {
    NMP_ASSERT(m_combineType == directInput);
    NMP_ASSERT(m_numEdges == 1); // DirectInput isn't going to work on multiple edges
    // Note that this even copies things that are 0 importance
    *result = (float&)getEdges()->m_source;
    return getEdges()->m_sourceImportance;
  }

  //--------------------------------------------------------------------------------------------------
  // Specialisation for bool that avoids branching
  float combineDirectInput(bool* result) const
  {
    NMP_ASSERT(m_combineType == directInput);
    NMP_ASSERT(m_numEdges == 1); // DirectInput isn't going to work on multiple edges
    // Note that this even copies things that are 0 importance
    *result = (bool&)getEdges()->m_source;
    return getEdges()->m_sourceImportance;
  }

  //--------------------------------------------------------------------------------------------------
  // Specialisation for int32_t that avoids branching
  float combineDirectInput(int32_t* result) const
  {
    NMP_ASSERT(m_combineType == directInput);
    NMP_ASSERT(m_numEdges == 1); // DirectInput isn't going to work on multiple edges
    // Note that this even copies things that are 0 importance
    *result = (int32_t&)getEdges()->m_source;
    return getEdges()->m_sourceImportance;
  }

  //--------------------------------------------------------------------------------------------------
  // Specialisation for uint32_t that avoids branching
  float combineDirectInput(uint32_t* result) const
  {
    NMP_ASSERT(m_combineType == directInput);
    NMP_ASSERT(m_numEdges == 1); // DirectInput isn't going to work on multiple edges
    // Note that this even copies things that are 0 importance
    *result = (uint32_t&)getEdges()->m_source;
    return getEdges()->m_sourceImportance;
  }

  //--------------------------------------------------------------------------------------------------
  template <class T>
  float combineAverage(T* result) const
  {
    NMP_ASSERT(m_combineType == average);
    NMP_ASSERT(m_numEdges > 1); // If you hit this assert, you should be using directInput

    float totalImportance = getEdges()->m_sourceImportance;
    float averageImportance = totalImportance * totalImportance;

    const T& firstVal = (T&)getEdges()->m_source;
    *result = firstVal * getEdges()->m_sourceImportance;

    for (uint32_t i = 1; i < m_numEdges; i++)
    {
      const Edge& edge = getEdges()[i];
      float imp = edge.m_sourceImportance;

      if (imp > g_minImportanceForJunction)
      {
        const T& val = (T&)(edge.m_source);
        quatFlip(*result, val);
        *result += val * imp;
        averageImportance += imp * imp;
        totalImportance += imp;
      }
    }

    if (totalImportance > g_minImportanceForJunction)
    {
      *result *= 1.0f / totalImportance;
      averageImportance /= totalImportance;
      return averageImportance;
    }
    else
    {
      return 0.0f;
    }

  }

  //--------------------------------------------------------------------------------------------------
  template <class T>
  float combineSum(T* result) const
  {
    NMP_ASSERT(m_combineType == sum);
    NMP_ASSERT(m_numEdges > 1); // If you hit this assert, you should be using directInput

    float imp = getEdges()->m_sourceImportance;
    float maxImportance = imp;

    const T& firstVal = (T&)(getEdges()->m_source);
    *result = firstVal * imp;

    for (uint32_t i = 1; i < m_numEdges; i++)
    {
      const Edge& edge = getEdges()[i];
      imp = edge.m_sourceImportance;
      if (imp > g_minImportanceForJunction)
      {
        const T& val = (T&)(edge.m_source);
        quatFlip(*result, val);
        *result += val * imp;
        // this means a ramping down of one importance from 1 to 0 will be a linear interpolation of the value
        maxImportance = NMP::maximum(imp, maxImportance);
      }
    }

    if (maxImportance > g_minImportanceForJunction)
    {
      *result *= 1.0f / maxImportance;
      return maxImportance;
    }
    else
    {
      return 0.0f;
    }
  }

  //--------------------------------------------------------------------------------------------------
  float combineMaxFloat(float* result) const
  {
    NMP_ASSERT(m_combineType == maxFloat);
    NMP_ASSERT(m_numEdges > 1); // If you hit this assert, you should be using directInput

    float resultImportance = 0.0f;

    resultImportance = getEdges()->m_sourceImportance;
    float res = (float&)(getEdges()->m_source);
    float best = res * resultImportance;

    for (uint32_t i = 1; i < m_numEdges; i++)
    {
      const Edge& edge = getEdges()[i];
      const float imp = edge.m_sourceImportance;
      const float val = (float&)(edge.m_source);
      const float val_imp_best = val * imp - best;
      // avoid branches
      resultImportance = NMP::floatSelect(val_imp_best, imp, resultImportance);
      best = NMP::floatSelect(val_imp_best, val * imp, best);
      res = NMP::floatSelect(val_imp_best, val, res);
    }
    *result = res;
    return resultImportance;
  }

  //--------------------------------------------------------------------------------------------------
  float combineMaxFloatValue(float* result) const
  {
    NMP_ASSERT(m_combineType == maxFloatValue);
    NMP_ASSERT(m_numEdges > 1); // If you hit this assert, you should be using directInput

    float res = (float&)(getEdges()->m_source);
    for (uint32_t i = 1; i < m_numEdges; i++)
    {
      const float val = (float&)(getEdges()[i].m_source);
      res = NMP::maximum(res, val);
    }
    *result = res;
    return 1.0f;
  }

  //--------------------------------------------------------------------------------------------------
  float combineIsBoolSet(bool* result) const
  {
    NMP_ASSERT(m_combineType == isBoolSet);
    NMP_ASSERT(m_numEdges > 1); // If you hit this assert, you should be using directInput

    const Edge* edges = getEdges();
    for (uint32_t i = 0; i < m_numEdges; i++)
    {
      const Edge& edge = edges[i];
      // assume that if the importance is 0 the val will be false
      const bool val = (bool&)(edge.m_source);
      if (val)
      {
        *result = true;
        return 1.0f;
      }
    }
    *result = false;
    return 0.0f;
  }

  //--------------------------------------------------------------------------------------------------
  template <class T>
  float combineWinnerTakesAll(T* result) const
  {
    NMP_ASSERT(m_combineType == winnerTakesAll);
    NMP_ASSERT(m_numEdges > 1); // If you hit this assert, you should be using directInput

    const Edge* edges = getEdges();
    float imp, resultImportance = edges[0].m_sourceImportance;
    uint32_t resultIndex = 0;

    for (uint32_t i = 1; i < m_numEdges; i++)
    {
      imp = edges[i].m_sourceImportance;

      if (imp >= resultImportance)
      {
        resultIndex = i;
        resultImportance = imp;
      }
    }

    if (resultImportance > 0.0f)
      *result = (T&)(edges[resultIndex].m_source);

    return resultImportance;
  }

  //--------------------------------------------------------------------------------------------------
  template <class T>
  float combinePriority(T* result) const
  {
    NMP_ASSERT(m_combineType == priority);
    NMP_ASSERT(m_numEdges > 1); // If you hit this assert, you should be using directInput

    // Find the first edge that's actually going to contribute (it's the last edge with 1.0f importance)
    uint32_t startIndex;
    bool nonZeroWeights = false;
    const Edge* edges = getEdges();
    for (startIndex = m_numEdges - 1; startIndex > 0; startIndex--)
    {
      float edgeImp = edges[startIndex].m_sourceImportance;

      if (edgeImp > g_minImportanceForJunction)
      {
        // on presence of a 1.0f, bail out, as no point in blending anything earlier
        if (edgeImp >= 0.999f)
          break;

        nonZeroWeights = true;
      }
    }

    // Now startIndex points to the edge that we start at (it may be 0 of course).
    float imp = edges[startIndex].m_sourceImportance;

    // if we hit no non-zero importances (or, in a special case, we simply hit a single 1.0f imp)
    if (!nonZeroWeights)
    {
      if (imp > g_minImportanceForJunction)
      {
        // just return the single value which will either have the 1.0 imp, or will be the first input
        *result = (T&)(edges[startIndex].m_source);
        return imp;
      }
      else
      {
        // if importance is zero then the result should be ignored...
        return 0.0f;
      }
    }

    float importance = imp;

    *result = (T&)(edges[startIndex].m_source) * imp;

    for (uint32_t i = startIndex + 1; i < m_numEdges; i++)
    {
      const Edge& edge = edges[i];
      imp = edge.m_sourceImportance;
      if (imp > g_minImportanceForJunction)
      {
        const T& val = (T&)(edge.m_source);
        quatFlip(*result, val);
        // avoid a single copy of potentially large T (compared to *result = *result * (1.0f-imp) + val * imp; )
        *result *= 1.0f - imp;
        *result += val * imp;
        importance += imp * (1.0f - importance);
      }
    }
    if (importance > g_minImportanceForJunction)
    {
      *result *= 1.0f / importance; // importance cannot be 0 here
      return importance;
    }
    else
    {
      return 0.0f;
    }
  }
}; // class

} // namespace

#endif // NM_JUNCTION_H
