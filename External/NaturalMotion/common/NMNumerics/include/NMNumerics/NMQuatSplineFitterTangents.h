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
#ifndef NM_QUAT_SPLINE_FITTER_TANGENTS_H
#define NM_QUAT_SPLINE_FITTER_TANGENTS_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMNumerics/NMQuatSplineFitterBase.h"
#include "NMNumerics/NMNonLinearOptimiser.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Quaternion spline fitting function that computes the residuals and
/// Jacobian analytically
class QuatSplineFitterSingleSpanTangentsFuncJac : public NonLinearLeastSquaresFuncJac<double>
{
public:
  //---------------------------------------------------------------------
  /// \brief  Function to preallocate memory for the analytical Jacobian
  /// and residuals vector for optimisation
  QuatSplineFitterSingleSpanTangentsFuncJac(uint32_t maxNumSamples);

  ~QuatSplineFitterSingleSpanTangentsFuncJac();

  //---------------------------------------------------------------------
  /// \brief Function to compute the analytical Jacobian and residuals vector
  virtual bool func(
    const NMP::NonLinearOptimiserBase<double>& optimiser,
    const NMP::Vector<double>&                 P,
    NMP::Vector<double>&                       fRes,
    NMP::JacobianBase<double>*                 J);

  //---------------------------------------------------------------------
  /// \brief Function to set the span information data required for fitting
  void setSpanInfo(
    uint32_t     numSpanSamples,
    const float* spanTs,
    const Quat*  spanQuats,
    float        ta,
    float        tb,
    const Quat&  qa,
    const Quat&  qb);

protected:
  // Alligned structures
  QuatSplineJacobian m_JacBuilder;
  Quat               m_qa;
  Quat               m_qb;

  // Span information data: P = [wa, wb]
  uint32_t     m_numSpanSamples;
  const float* m_spanTs;
  const Quat*  m_spanQuats;
  float        m_ta;
  float        m_tb;

  // Workspace memory
  Matrix<double> m_mat4_3;
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Quaternion spline curve fitting class. Fit a cubic quaternion spline to the
/// quaternion sample data with a user specified knot vector. The fitting algorithm performs
/// a local approximation to fit a quaternion spline to segments interpolating the data by
/// using the Levenberg-Marquardt non-linear optimisation algorithm.
class QuatSplineFitterTangents : public QuatSplineFitterBase
{
public:
  //---------------------------------------------------------------------
  /// \name   Constructors
  /// \brief  Functions to fit a quaternion spline or preallocate memory
  //---------------------------------------------------------------------
  //@{
  QuatSplineFitterTangents(uint32_t numSamples, const float* ts, const Quat* quats, uint32_t numKnots, const float* knots);
  QuatSplineFitterTangents(uint32_t numSamples, const Quat* quats, uint32_t numKnots, const float* knots);
  QuatSplineFitterTangents(uint32_t maxNumSamples);

  // Destructor
  ~QuatSplineFitterTangents();
  //@}

  //---------------------------------------------------------------------
  /// \name   Fitting functions
  /// \brief  Functions to fit a quaternion spline
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief Fit a cubic quaternion spline to the quaternion sample data.
  /// You must set the knot vector before calling this function
  ///
  /// \param numSamples - The number of samples within the data buffers
  /// \param ts - The times at which the quaternions are sampled
  /// \param quats - The quaternion sample data
  virtual bool fit(uint32_t numSamples, const float* ts, const Quat* quats);
  //@}

protected:
  QuatSpline m_qspan;

  // Optimisation parameters for the C0 system: P = [wa, wb]
  Vector<double>                             m_P;
  QuatSplineFitterSingleSpanTangentsFuncJac* m_lsqFunc;
  NonLinearOptimiser<double>*                m_optimiser;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_QUAT_SPLINE_FITTER_TANGENTS_H
