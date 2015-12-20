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
#ifndef NM_QUAT_SPLINE_FITTER_SMOOTHED_C1_H
#define NM_QUAT_SPLINE_FITTER_SMOOTHED_C1_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMNumerics/NMQuatSplineFitterTangents.h"
#include "NMNumerics/NMNonLinearOptimiser.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
class SpanInfoSmoothed
{
public:
  SpanInfoSmoothed();
  ~SpanInfoSmoothed();

  void clear();

public:
  uint32_t     m_numSpanSamples;  ///< Number of samples in the curve span
  const float* m_spanTs;          ///< Time samples for the data in the span
  const Quat*  m_spanQuats;       ///< Quaternion samples for the data within the span
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Quaternion spline fitting function that computes the residuals and
/// Jacobian analytically
class QuatSplineFitterSmoothedC0FuncJac : public NonLinearLeastSquaresFuncJac<double>
{
public:
  //---------------------------------------------------------------------
  /// \brief  Function to preallocate memory for the analytical Jacobian
  /// and residuals vector for optimisation
  QuatSplineFitterSmoothedC0FuncJac(uint32_t maxNumSamples);
  ~QuatSplineFitterSmoothedC0FuncJac();

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
    const SpanInfoSmoothed* spanInfo1,
    const SpanInfoSmoothed* spanInfo2,
    float                   ta,
    float                   tb,
    float                   tc,
    const Quat&             qa,
    const Quat&             qc,
    float                   alpha_a,
    float                   alpha_b,
    const Vector3&          wa,
    const Vector3&          wc,
    float                   weight_wa,
    float                   weight_wc);

protected:
  // Alligned structures
  QuatSplineJacobian m_JacBuilder;

  Quat    m_qa;                             ///< Start quaternion key of the first curve span (fixed)
  Quat    m_qc;                             ///< End quaternion key of the second curve span (fixed)
  Vector3 m_wa;                             ///< Start tangent vector of the first curve span (original)
  Vector3 m_wc;                             ///< End tangent vector of the second curve span (original)

  // Span information: P = [wa; rb; v; wc]
  const SpanInfoSmoothed* m_spanInfo1;      ///< Span information for the first curve span
  const SpanInfoSmoothed* m_spanInfo2;      ///< Span information for the second curve span

  float m_ta;                                ///< Start knot position of the first curve span
  float m_tb;                                ///< Mid knot position between curve spans
  float m_tc;                                ///< End knot position of the second curve span

  float m_alpha_a;                           ///< C1 Continuity alpha for the first curve span
  float m_alpha_b;                           ///< C1 Continuity alpha for the second curve span
  float m_weight_wa;                         ///< Weight factor for the start angular tangent wa
  float m_weight_wc;                         ///< Weight factor for the end angular tangent wc

  // Workspace memory
  Matrix<double> m_dqb_by_drb;
  Matrix<double> m_dw2_by_drb;
  Matrix<double> m_mat3_3;
  Matrix<double> m_matA4_3;
  Matrix<double> m_matB4_3;
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Quaternion spline fitting function that computes the residuals and
/// Jacobian analytically
class QuatSplineFitterSmoothedC1FuncJac : public NonLinearLeastSquaresFuncJac<double>
{
public:
  //---------------------------------------------------------------------
  /// \brief  Function to preallocate memory for the analytical Jacobian
  /// and residuals vector for optimisation
  QuatSplineFitterSmoothedC1FuncJac(uint32_t maxNumSamples);
  ~QuatSplineFitterSmoothedC1FuncJac();

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
    const SpanInfoSmoothed* spanInfo1,
    const SpanInfoSmoothed* spanInfo2,
    float                   ta,
    float                   tb,
    float                   tc,
    const Quat&             qa,
    const Quat&             qc,
    float                   alpha_a,
    float                   alpha_b,
    const Vector3&          wa,
    const Vector3&          wc);

protected:
  // Alligned structures
  QuatSplineJacobian m_JacBuilder;

  Quat    m_qa;                             ///< Start quaternion key of the first curve span (fixed)
  Quat    m_qc;                             ///< End quaternion key of the second curve span (fixed)
  Vector3 m_wa;                             ///< Start tangent vector of the first curve span (fixed)
  Vector3 m_wc;                             ///< End tangent vector of the second curve span (fixed)

  // Span information: P = [rb; v]
  const SpanInfoSmoothed* m_spanInfo1;      ///< Span information for the first curve span
  const SpanInfoSmoothed* m_spanInfo2;      ///< Span information for the second curve span

  float m_ta;                                ///< Start knot position of the first curve span
  float m_tb;                                ///< Mid knot position between curve spans
  float m_tc;                                ///< End knot position of the second curve span

  float m_alpha_a;                           ///< C1 Continuity alpha for the first curve span
  float m_alpha_b;                           ///< C1 Continuity alpha for the second curve span

  // Workspace memory
  Matrix<double> m_dqb_by_drb;
  Matrix<double> m_dw2_by_drb;
  Matrix<double> m_mat3_3;
  Matrix<double> m_matA4_3;
  Matrix<double> m_matB4_3;
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Quaternion spline curve fitting class. Fit a cubic quaternion spline to the
/// quaternion sample data with a user specified knot vector. The fitting algorithm performs
/// a local approximation to fit a quaternion spline to segments interpolating the data by
/// using the Levenberg-Marquardt non-linear optimisation algorithm.
class QuatSplineFitterSmoothedC1 : public QuatSplineFitterBase
{
public:
  //---------------------------------------------------------------------
  /// \name   Constructors
  /// \brief  Functions to fit a quaternion spline or preallocate memory
  //---------------------------------------------------------------------
  //@{
  QuatSplineFitterSmoothedC1(uint32_t numSamples, const float* ts, const Quat* quats, uint32_t numKnots, const float* knots);
  QuatSplineFitterSmoothedC1(uint32_t numSamples, const Quat* quats, uint32_t numKnots, const float* knots);
  QuatSplineFitterSmoothedC1(uint32_t maxNumSamples, uint32_t numKnots);

  // Destructor
  ~QuatSplineFitterSmoothedC1();
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
  //---------------------------------------------------------------------
  /// \brief Main function for smoothing the angular velocities and optimising the
  /// quaternion key between two adjacent spans.
  void smoothC0(uint32_t indx, float weight_wa, float weight_wb);

  //---------------------------------------------------------------------
  /// \brief Main function for smoothing the angular velocities and optimising the
  /// quaternion key between two adjacent spans.
  void smoothC1(uint32_t indx);

protected:
  // Optimisation parameters for the C0 system: P = [wa; rb; v; wc]
  Vector<double>                     m_P0;
  QuatSplineFitterSmoothedC0FuncJac* m_lsqFuncC0;
  NonLinearOptimiser<double>*        m_optimiserC0;

  // Optimisation parameters for the C1 system: P = [rb; v]
  Vector<double>                     m_P1;
  QuatSplineFitterSmoothedC1FuncJac* m_lsqFuncC1;
  NonLinearOptimiser<double>*        m_optimiserC1;

  // Span information data
  SpanInfoSmoothed* m_spanInfoInc;  ///< Inclusive of span ta, exclusive of span tb
  SpanInfoSmoothed* m_spanInfoExc;  ///< Exclusive of both span ta and tb

  // Optimiser for computing the initial curve spans
  QuatSplineFitterTangents m_tangentFitter;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_QUAT_SPLINE_FITTER_SMOOTHED_C1_H
