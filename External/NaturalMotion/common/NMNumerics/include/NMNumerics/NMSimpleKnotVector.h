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
#ifndef NM_SIMPLE_KNOT_VECTOR_H
#define NM_SIMPLE_KNOT_VECTOR_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include <iostream>
#include <fstream>

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Class implements a simple knot vector without knot multiplicities
class SimpleKnotVector
{
public:
  //---------------------------------------------------------------------
  /// \name   Constructors
  /// \brief  Functions to set a knot vector or preallocate memory
  //---------------------------------------------------------------------
  //@{

  /// \brief Constructor to set the quaternion spline from the specified
  /// Hermite coefficients.
  SimpleKnotVector(uint32_t numKnots, const float* knots);

  /// \brief Constructor to preallocate the memory for a knot vector
  SimpleKnotVector(uint32_t maxNumKnots);

  /// \brief Constructor to set the knot vector by copy
  SimpleKnotVector(const SimpleKnotVector& knots);

  // Destructor
  virtual ~SimpleKnotVector();
  //@}

  //---------------------------------------------------------------------
  /// \name   Knot vector functions
  /// \brief  Functions to get and set elements of the knot vector
  //---------------------------------------------------------------------
  //@{
  const float* getKnots() const { return m_knots; }
  uint32_t getNumSpans() const;
  uint32_t getNumKnots() const;
  void setNumKnots(uint32_t n);
  float getKnot(uint32_t i) const;
  void setKnot(uint32_t i, float t);
  void setKnots(uint32_t n, const float* knots);
  void setKnots(uint32_t n, const uint32_t* knots);
  float getStartKnot() const;
  float getEndKnot() const;

  /// \brief Function to set the knot vector that is regularly sampled between [ta:tb]
  void setKnots(uint32_t numKnots, float ta, float tb);

  /// \brief Function to set the knot vector that is regularly sampled between [ta:tb]
  void setKnotsRegular(uint32_t ta, uint32_t tb);

  /// \brief Function to compute the required number of knots for a discretely
  /// sampled knot vector between [ta:tb]
  static uint32_t getNumKnotsDiscrete(uint32_t samplesPerKnot, uint32_t ta, uint32_t tb);

  /// \brief Function to set the discrete knot vector that is regularly sampled between [ta:tb]
  void setKnotsDiscrete(uint32_t samplesPerKnot, uint32_t ta, uint32_t tb);

  /// \brief Function to append a value to the knot vector
  void appendKnot(float t);

  /// \brief Function to copy the knot vector. The required amount of
  /// memory must have already been preallocated to store the copy.
  SimpleKnotVector& operator=(const SimpleKnotVector& knots);

  /// \brief Function swaps the contents of the two knot vectors.
  void swap(SimpleKnotVector& knots);

  /// \brief Function to get the start and end knot indices for the curve spans
  /// that inclusively contain the specified [start : end] parameter range. The
  /// function returns false if the parameter range is not completely within the
  /// knot range of the spline curve.
  bool getKnotsForSamples(
    float     start,
    float     end,
    uint32_t& startKnotIndex,
    uint32_t& endKnotIndex) const;

  /// \brief Function computes the knot vector interval that t lies within.
  /// The corresponding curve span is assumed to lie between the knot interval
  /// m_knots[intv] and m_knots[intv+1].
  int32_t interval(float t, uint32_t& intv) const;

  /// \brief Function computes the index of the closest knot to t.
  uint32_t closestKnot(float t) const;
  //@}

protected:
  uint32_t m_maxNumKnots;       ///< Maximum number of knots preallocated for the knot vector
  uint32_t m_numKnots;          ///< The actual number of knots for this knot vector

  float* m_knots;            ///< The quaternion spline knot vector
};

/// \brief Function to write the knot vector data to the output stream
std::ostream& operator<<(std::ostream& os, const SimpleKnotVector& knots);

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_SIMPLE_KNOT_VECTOR_H
