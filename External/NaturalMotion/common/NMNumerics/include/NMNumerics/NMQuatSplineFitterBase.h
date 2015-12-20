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
#ifndef NM_QUAT_SPLINE_FITTER_BASE_H
#define NM_QUAT_SPLINE_FITTER_BASE_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMatrix.h"
#include "NMNumerics/NMSimpleKnotVector.h"
#include "NMNumerics/NMQuatSpline.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief This class builds the Jacobian for the Bezier and Hermite forms of the
/// Quaternion spline evaluation function
class QuatSplineJacobian
{
public:
  //---------------------------------------------------------------------
  QuatSplineJacobian();
  ~QuatSplineJacobian();

  //---------------------------------------------------------------------
  /// \brief Function for computing the Jacobian of the quaternion spline function
  /// that converts the Hermite control points into the corresponding Bezier control
  /// points. Specifically the middle control point w2:
  ///
  /// w2 = qlog( qqmul( qinv( qqmul(qa, qexp(wa)) ), qqmul(qb, qinv( qexp(wb) )) ) )
  ///
  void w2Jac(const Quat& qa, const Vector3& wa, const Vector3& wb, const Quat& qb);

  //---------------------------------------------------------------------
  // Jacobians for the Hermite to Bezier quaternion spline coefficient conversion
  const Matrix<double>& getDw2ByDqa() const { return m_dw2_by_dqa; }
  const Matrix<double>& getDw2ByDwa() const { return m_dw2_by_dwa; }
  const Matrix<double>& getDw2ByDwb() const { return m_dw2_by_dwb; }
  const Matrix<double>& getDw2ByDqb() const { return m_dw2_by_dqb; }
  const Vector3& getW2() const { return m_w2; }

  //---------------------------------------------------------------------
  /// \brief Function for computing the Jacobian of the quaternion spline evaluation.
  void valJac(const Quat& q0, const Vector3& w1, const Vector3& w2, const Vector3& w3, float t, float ta, float tb);

  //---------------------------------------------------------------------
  // Jacobians for evaluation of the quaternion spline in the Bezier form
  const Matrix<double>& getDqtByDq0() const { return m_dqt_by_dq0; }
  const Matrix<double>& getDqtByDw1() const { return m_dqt_by_dw1; }
  const Matrix<double>& getDqtByDw2() const { return m_dqt_by_dw2; }
  const Matrix<double>& getDqtByDw3() const { return m_dqt_by_dw3; }
  const Quat& getQt() const { return m_qt; }

protected:
  // Alligned structures
  Vector3 m_w2;
  Quat    m_qt;

  // Temporary workspace Jacobians
  Matrix<double> m_mat4_4;
  Matrix<double> m_mat4_3;
  Matrix<double> m_matA3_4;
  Matrix<double> m_matB3_4;

  // Temporary workspace Jacobians for Hermite to Bezier coefficient conversion
  Matrix<double> m_dea_by_dwa;
  Matrix<double> m_deb_by_dwb;
  Matrix<double> m_dw2_by_dq12;

  // Jacobians for Hermite to Bezier coefficient conversion
  Matrix<double> m_dw2_by_dqa;
  Matrix<double> m_dw2_by_dwa;
  Matrix<double> m_dw2_by_dwb;
  Matrix<double> m_dw2_by_dqb;

  // Temporary workspace Jacobians for quaternion spline evaluation
  Matrix<double> m_de1_by_dw1;
  Matrix<double> m_de2_by_dw2;
  Matrix<double> m_de3_by_dw3;

  // Jacobians for evaluation of the quaternion spline in the Bezier form
  Matrix<double> m_dqt_by_dq0;
  Matrix<double> m_dqt_by_dw1;
  Matrix<double> m_dqt_by_dw2;
  Matrix<double> m_dqt_by_dw3;
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Quaternion spline curve fitting base class. Fit a cubic quaternion spline to the
/// quaternion sample data.
class QuatSplineFitterBase
{
public:
  //---------------------------------------------------------------------
  /// \name   Constructors
  /// \brief  Functions to preallocate memory for quaternion spline fitting
  //---------------------------------------------------------------------
  //@{
  QuatSplineFitterBase(uint32_t maxNumSamples);
  virtual ~QuatSplineFitterBase();
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

  //---------------------------------------------------------------------
  /// \brief Fit a cubic quaternion spline to the regularly sampled quaternion data.
  /// The fitting algorithm performs a local approximation to fit a quaternion
  /// spline to segments interpolating the data to within the specified tolerance.
  ///
  /// \param numSamples - The number of samples within the data buffers
  /// \param quats - The quaternion sample data
  virtual bool fitRegular(uint32_t numSamples, const Quat* quats);
  //@}

  //---------------------------------------------------------------------
  /// \name   Data functions
  /// \brief  Functions to get the fitted quaternion spline
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
  /// \brief Function to get the quaternion samples
  Quat* getQuats();
  const Quat* getQuats() const;
  void setQuatsRaw(uint32_t numSamples, const Quat* quats);
  void setQuatsConditioned(uint32_t numSamples, const Quat* quats);
  bool getEnableConditioning() const;
  void setEnableConditioning(bool enable);

  //---------------------------------------------------------------------
  /// \brief Function to get the fitted quaternion curve
  const QuatSpline& getQuatSpline() const;
  void setQuatSpline(const QuatSpline& qsp);
  uint32_t getNumKnots() const;
  void setNumKnots(uint32_t numKnots);
  const float* getKnots() const;
  void setKnots(uint32_t numKnots, const float* knots);
  void setKnots(uint32_t numKnots, const uint32_t* knots);
  void setKnots(const SimpleKnotVector& knots);
  //@}

protected:
  uint32_t    m_maxNumSamples;        ///< Maximum reserved number of data samples
  uint32_t    m_numSamples;           ///< Actual number of samples used for fitting
  float*      m_ts;                   ///< The set of time samples

  Quat*       m_quats;                ///< The corresponding set of quaternion samples
  bool        m_conditionFlag;        ///< Flag that specifies if the input quat sames are conditioned
                                      ///< prior to fitting

  QuatSpline  m_qsp;                  ///< The computed quaternion spline
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_QUAT_SPLINE_FITTER_BASE_H
