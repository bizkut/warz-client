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
#ifndef NM_QUAT_SPLINE_FITTER_CONTINUITY_C1_H
#define NM_QUAT_SPLINE_FITTER_CONTINUITY_C1_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMNumerics/NMQuatSplineFitterSmoothedC1.h"
#include "NMNumerics/NMNonLinearOptimiserBandDiag.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Quaternion spline fitting function that computes the residuals and
/// Jacobian analytically
class QuatSplineFitterContinuityC1FuncJac : public NonLinearLeastSquaresFuncJac<double>
{
public:
  //---------------------------------------------------------------------
  // Span information data
  class SpanInfoC1
  {
  public:
    SpanInfoC1();
    ~SpanInfoC1();

    void clear();

  public:
    uint32_t     m_numSpanSamples;
    const float* m_spanTs;
    const Quat*  m_spanQuats;
    float        m_ta;
    float        m_tb;
    float        m_alpha_a;
    float        m_alpha_b;
    uint32_t     m_jRow;
    uint32_t     m_jSize;
  };

public:
  //---------------------------------------------------------------------
  /// \brief  Function to preallocate memory for the analytical Jacobian
  /// and residuals vector for optimisation
  QuatSplineFitterContinuityC1FuncJac(uint32_t maxNumSamples, uint32_t numKnots);
  ~QuatSplineFitterContinuityC1FuncJac();

  //---------------------------------------------------------------------
  /// \brief Function to compute the sparse analytical Jacobian and residuals vector
  virtual bool func(
    const NMP::NonLinearOptimiserBase<double>& optimiser,
    const NMP::Vector<double>&                 P,
    NMP::Vector<double>&                       fRes,
    NMP::JacobianBase<double>*                 J);

  //---------------------------------------------------------------------
  /// \brief Function to set the span information data required for fitting
  void setSpanInfo(
    uint32_t     spanIndex,
    uint32_t     numSpanSamples,
    const float* spanTs,
    const Quat*  spanQuats,
    float        ta,
    float        tb,
    float        alpha_a,
    float        alpha_b,
    uint32_t     jRow,
    uint32_t     jSize);

  //---------------------------------------------------------------------
  const SpanInfoC1& getSpanInfo(uint32_t spanIndex) const;

protected:
  // Span information: P = [r1, v1, r2, v2, ... , rn, vn]
  uint32_t    m_numKnots;
  SpanInfoC1* m_spanInfo;

  // Workspace memory
  QuatSplineJacobian m_JacBuilder;

  Matrix<double> m_dqa_by_dra;
  Matrix<double> m_dqb_by_drb;
  Matrix<double> m_dw2_by_dra;
  Matrix<double> m_dw2_by_drb;
  Matrix<double> m_matA4_3;
  Matrix<double> m_matB4_3;
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Quaternion spline curve fitting class. Fit a cubic quaternion spline to the
/// quaternion sample data with a user specified knot vector. The fitting algorithm performs
/// a local approximation to fit a quaternion spline to segments interpolating the data by
/// using the Levenberg-Marquardt non-linear optimisation algorithm.
class QuatSplineFitterContinuityC1 : public QuatSplineFitterBase
{
public:
  //---------------------------------------------------------------------
  /// \name   Constructors
  /// \brief  Functions to fit a quaternion spline or preallocate memory
  //---------------------------------------------------------------------
  //@{
  QuatSplineFitterContinuityC1(uint32_t numSamples, const float* ts, const Quat* quats, uint32_t numKnots, const float* knots);
  QuatSplineFitterContinuityC1(uint32_t numSamples, const Quat* quats, uint32_t numKnots, const float* knots);
  QuatSplineFitterContinuityC1(uint32_t maxNumSamples, uint32_t numKnots);

  // Destructor
  ~QuatSplineFitterContinuityC1();
  //@}

  //---------------------------------------------------------------------
  /// \name   Fitting functions
  /// \brief  Functions to fit a quaternion spline
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief Fit a cubic quaternion spline to the quaternion sample data.
  ///
  /// \param numSamples - The number of samples within the data buffers
  /// \param ts - The times at which the quaternions are sampled
  /// \param quats - The quaternion sample data
  virtual bool fit(uint32_t numSamples, const float* ts, const Quat* quats);
  //@}

protected:
  // Optimisation parameters for the full sparse C1 system:
  // P = [r1, v1, r2, v2, ... , rn, vn]
  Vector<double>                       m_P;
  QuatSplineFitterContinuityC1FuncJac* m_lsqFunc;
  NonLinearOptimiserBandDiag<double>*  m_optimiser;

  // Optimiser for computing the initial curve spans
  QuatSplineFitterSmoothedC1 m_smoothFitter;

  // Knot continuity alphas
  Vector<float> m_alphas;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_QUAT_SPLINE_FITTER_CONTINUITY_C1_H
