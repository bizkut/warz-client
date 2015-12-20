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
#ifndef NM_QUAT_SPLINE_FITTER_ANGLE_TOL_H
#define NM_QUAT_SPLINE_FITTER_ANGLE_TOL_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMNumerics/NMQuatSplineFitterTangents.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Quaternion spline curve fitting class. Fit a cubic quaternion spline to the
/// quaternion sample data. The fitting algorithm performs a local approximation to fit
/// a quaternion spline to segments interpolating the data by using the Levenberg-Marquardt
/// non-linear optimisation algorithm.
class QuatSplineFitterAngleTol : public QuatSplineFitterBase
{
public:
  //---------------------------------------------------------------------
  /// \name   Constructors
  /// \brief  Functions to fit a quaternion spline or preallocate memory
  //---------------------------------------------------------------------
  //@{
  QuatSplineFitterAngleTol(uint32_t numSamples, const float* ts, const Quat* quats, float qTol);
  QuatSplineFitterAngleTol(uint32_t numSamples, const Quat* quats, float qTol);
  QuatSplineFitterAngleTol(uint32_t maxNumSamples);

  // Destructor
  ~QuatSplineFitterAngleTol();
  //@}

  //---------------------------------------------------------------------
  /// \name   Fitting functions
  /// \brief  Functions to fit a quaternion spline
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief Fit a cubic quaternion spline to the quaternion sample data.
  /// The fitting algorithm performs a local approximation to fit a quaternion
  /// spline to segments interpolating the data to within the specified tolerance.
  ///
  /// \param numSamples - The number of samples within the data buffers
  /// \param ts - The times at which the quaternions are sampled
  /// \param quats - The quaternion sample data
  virtual bool fit(uint32_t numSamples, const float* ts, const Quat* quats);

  //---------------------------------------------------------------------
  /// \brief User defined callback function to provide an additional mechanism
  /// to terminate the fitting step. The function should return true if the
  /// fitting step should be terminated. This function is called whenever the
  /// currently fitted curve span is within the angular tolerance.
  ///
  /// \param ka - The start index for the time samples that are currently being fitted
  /// \param kb - The end index for the time samples that are currently being fitted
  virtual bool func(uint32_t ka, uint32_t kb);
  //@}

  //---------------------------------------------------------------------
  /// \name   Data functions
  /// \brief  Functions to get the fitted quaternion spline
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief Function to get the fitting tolerance angle in radians that
  /// corresponds to the maximum allowable error between any reconstructed
  /// and data sample quaternions
  float getQTol() const;
  void setQTol(float qTol);

  //---------------------------------------------------------------------
  /// \brief Function to get the resampled quaternion samples
  Quat* getResampledQuats();
  const Quat* getResampledQuats() const;
  //@}

protected:
  float m_qTol;

  QuatSpline m_qspan;
  QuatSpline m_qspan_;
  Quat*      m_resampledQuats;

  // Optimisation parameters for the C0 system: P = [wa, wb]
  Vector<double>                             m_P;
  QuatSplineFitterSingleSpanTangentsFuncJac* m_lsqFunc;
  NonLinearOptimiser<double>*                m_optimiser;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_QUAT_SPLINE_FITTER_ANGLE_TOL_H
