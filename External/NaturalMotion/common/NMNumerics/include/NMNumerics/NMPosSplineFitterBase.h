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
#ifndef NM_POS_SPLINE_FITTER_BASE_H
#define NM_POS_SPLINE_FITTER_BASE_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMNumerics/NMSimpleKnotVector.h"
#include "NMNumerics/NMPosSpline.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Position spline curve fitting base class. Fit a cubic position spline to the
/// position sample data.
class PosSplineFitterBase
{
public:
  //---------------------------------------------------------------------
  /// \name   Constructors
  /// \brief  Functions to preallocate memory for position spline fitting
  //---------------------------------------------------------------------
  //@{
  PosSplineFitterBase(uint32_t maxNumSamples);
  virtual ~PosSplineFitterBase();
  //@}

  //---------------------------------------------------------------------
  /// \name   Fitting functions
  /// \brief  Functions to fit a position spline
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief Fit a cubic position spline to the position sample data.
  ///
  /// \param numSamples - The number of samples within the data buffers
  /// \param ts - The times at which the positions are sampled
  /// \param data - The position sample data
  virtual bool fit(uint32_t numSamples, const float* ts, const Vector3* data);

  //---------------------------------------------------------------------
  /// \brief Fit a cubic position spline to the regularly sampled position data.
  ///
  /// \param numSamples - The number of samples within the data buffers
  /// \param data - The position sample data
  virtual bool fitRegular(uint32_t numSamples, const Vector3* data);
  //@}

  //---------------------------------------------------------------------
  /// \name   Data functions
  /// \brief  Functions to get the fitted position spline
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief Function to get the number of data samples
  uint32_t getNumSamples() const;
  uint32_t getMaxNumSamples() const;

  //---------------------------------------------------------------------
  /// \brief Function to get the time samples
  float* getTs();
  const float* getTs() const;
  void setTs(uint32_t numSamples, const float* ts);

  //---------------------------------------------------------------------
  /// \brief Function to get the position samples
  Vector3* getPosData();
  const Vector3* getPosData() const;
  void setPosData(uint32_t numSamples, const Vector3* data);

  //---------------------------------------------------------------------
  /// \brief Function to get the fitted position curve
  const PosSpline& getPosSpline() const;
  void setPosSpline(const PosSpline& psp);
  uint32_t getNumKnots() const;
  void setNumKnots(uint32_t numKnots);
  const float* getKnots() const;
  void setKnots(uint32_t numKnots, const float* knots);
  void setKnots(uint32_t numKnots, const uint32_t* knots);
  void setKnots(const SimpleKnotVector& knots);
  //@}

protected:
  uint32_t m_maxNumSamples;     ///< Maximum reserved number of data samples
  uint32_t m_numSamples;        ///< Actual number of samples used for fitting
  float*   m_ts;                ///< The set of time samples
  Vector3* m_posData;           ///< The corresponding set of position data samples

  PosSpline m_psp;              ///< The computed Bezier position spline
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_POS_SPLINE_FITTER_BASE_H
