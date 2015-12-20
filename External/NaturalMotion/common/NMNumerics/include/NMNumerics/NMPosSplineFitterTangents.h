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
#ifndef NM_POS_SPLINE_FITTER_TANGENTS_H
#define NM_POS_SPLINE_FITTER_TANGENTS_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMNumerics/NMPosSplineFitterBase.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Cubic Bezier spline curve fitting class. Fit a cubic position spline to the
/// position sample data with a user specified knot vector.
class PosSplineFitterTangents : public PosSplineFitterBase
{
public:
  //---------------------------------------------------------------------
  /// \name   Constructors
  /// \brief  Functions to fit a position spline or preallocate memory
  //---------------------------------------------------------------------
  //@{
  PosSplineFitterTangents(uint32_t numSamples, const float* ts, const Vector3* data, uint32_t numKnots, const float* knots);
  PosSplineFitterTangents(uint32_t numSamples, const Vector3* data, uint32_t numKnots, const float* knots);
  PosSplineFitterTangents(uint32_t maxNumSamples);

  // Destructor
  ~PosSplineFitterTangents();
  //@}

  //---------------------------------------------------------------------
  /// \name   Fitting functions
  /// \brief  Functions to fit a position spline
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief Fit a cubic position spline to the position sample data.
  /// You must set the knot vector before calling this function
  ///
  /// \param numSamples - The number of samples within the data buffers
  /// \param ts - The times at which the positions are sampled
  /// \param data - The position sample data
  virtual bool fit(uint32_t numSamples, const float* ts, const Vector3* data);
  //@}

protected:
  PosSpline m_pspan;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_POS_SPLINE_FITTER_TANGENTS_H
