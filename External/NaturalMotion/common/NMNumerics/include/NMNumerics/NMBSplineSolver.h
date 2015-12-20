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
#ifndef NM_BSPLINE_SOLVER_H
#define NM_BSPLINE_SOLVER_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMatrix.h"
#include "NMNumerics/NMBSplineCurve.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Used to return the status of the B-Spline solver
class BSplineSolverStatus
{
public:
  enum eType {
    Success, // Successfully computed B-Spline
    InvalidInterval, // Incompatible knot vector with sample data
    Singular, // Solution is not unique
    BadParams // Bad input parameters
  };
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Least squares B-Spline solver
///
/// Fits data samples and m^th derivatives using least squares approximation
/// to a B-Spline curve with given order and a user specified knot vector.
/// i.e. it minimizes the residual: sum_{i} w(i) || Y(i,:) - C(x(i)) ||^2,
/// where w is a weight vector, Y the set of data sample values and x
/// the set of data sites.
///
/// The m^th derivatives of a B-Spline C(x) can be written in terms of
/// the zero^th order control points P_i:
///
/// D^(m)C(x) = \sum_{i=1}^n D^(m)N_{i,k}(x) P_i
///
/// This allows us to compute the set of P_i given any of the m^th derivatives
/// as known input data samples. Typically we input a set of m^th derivatives
/// as a matrix DmY with a corresponding integer vector m specifying the
/// derivative order of each data sample within the matrix.
///
template <typename _T>
class BSplineSolver
{
public:
  //---------------------------------------------------------------------
  /// \name   Constructors
  /// \brief  Functions to compute a B-Spline curve via least squares fitting
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief  Constructor to compute the B-Spline curve of given order
  /// on the specified knot vector.
  ///
  /// \param  order - The desired curve polynomial order
  /// \param  knots - The monotonically increasing knot vector
  /// \param  DmY - The N by dims matrix of m^th derivative order data samples.
  /// \param  m - An integer vector of length N identifying the order of each data sample.
  /// \param  x - The vector of data sites of length N which is assumed to be
  ///             monotonically increasing. i.e. All data is input sorted on parameter x.
  /// \param  w - The vector of fitting weights of length N
  BSplineSolver(
    uint32_t order, const Vector<_T>& knots,
    const Matrix<_T>& DmY, const Vector<int32_t>& m,
    const Vector<_T>& x, const Vector<_T>& w);

  //---------------------------------------------------------------------
  /// \brief  Constructor to compute the B-Spline curve of given order
  /// on the specified knot vector. The input data is assumed to contain
  /// positional (zero^th order) samples only. The unit weight vector is
  /// also used within the main solve procedure.
  ///
  /// \param  order - The desired curve polynomial order
  /// \param  knots - The monotonically increasing knot vector
  /// \param  Y - The N by dims matrix of data samples. All samples are zero^th order.
  /// \param  x - The vector of data sites of length N which is assumed to be
  ///             monotonically increasing. i.e. All data is input sorted on parameter x.
  BSplineSolver(
    uint32_t order, const Vector<_T>& knots,
    const Matrix<_T>& Y, const Vector<_T>& x);

  //---------------------------------------------------------------------
  /// \brief  Constructor to compute the B-Spline curve of given order
  /// with the specified number of spans. A suitable knot vector is determined
  /// from the data sample vector x.
  ///
  /// \param  order - The desired curve polynomial order
  /// \param  spans - The number of spans to partition the curve into
  /// \param  DmY - The N by dims matrix of m^th derivative order data samples.
  /// \param  m - An integer vector of length N identifying the order of each data sample.
  /// \param  x - The vector of data sites of length N which is assumed to be
  ///             monotonically increasing. i.e. All data is input sorted on parameter x.
  /// \param  w - The vector of fitting weights of length N
  BSplineSolver(
    uint32_t order, uint32_t spans,
    const Matrix<_T>& DmY, const Vector<int32_t>& m,
    const Vector<_T>& x, const Vector<_T>& w);

  //---------------------------------------------------------------------
  /// \brief  Constructor to compute the B-Spline curve of given order
  /// with the specified number of spans. A suitable knot vector is determined
  /// from the data sample vector x. The input data is assumed to contain
  /// positional (zero^th order) samples only. The unit weight vector is
  /// also used within the main solve procedure.
  ///
  /// \param  order - The desired curve polynomial order
  /// \param  spans - The number of spans to partition the curve into
  /// \param  Y - The N by dims matrix of data samples. All samples are zero^th order.
  /// \param  x - The vector of data sites of length N which is assumed to be
  ///             monotonically increasing. i.e. All data is input sorted on parameter x.
  BSplineSolver(
    uint32_t order, uint32_t spans,
    const Matrix<_T>& Y, const Vector<_T>& x);

  //---------------------------------------------------------------------
  /// \brief  Constructor to specify the order, dimension and knots of the
  /// desired B-Spline curve.
  BSplineSolver(uint32_t order, uint32_t dims, const Vector<_T>& knots);

  //---------------------------------------------------------------------
  /// \brief  Constructor to specify the order, dimension and number of
  /// spans in the desired B-Spline curve. A suitable knot vector is
  /// determined from the data sample vector x.
  BSplineSolver(uint32_t order, uint32_t dims, uint32_t spans, const Vector<_T>& x);

  //---------------------------------------------------------------------
  /// Destructor
  ~BSplineSolver();
  //@}

  //---------------------------------------------------------------------
  /// \name   Repeat Solve functions
  /// \brief  Functions to compute a B-Spline curve via least squares fitting
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief  Function to compute the B-Spline curve of given order
  /// on the specified knot vector that approximates the input data.
  ///
  /// \param  DmY - The N by dims matrix of m^th derivative order data samples.
  /// \param  m - An integer vector of length N identifying the order of each data sample.
  /// \param  x - The vector of data sites of length N which is assumed to be
  ///             monotonically increasing. i.e. All data is input sorted on parameter x.
  /// \param  w - The vector of fitting weights of length N
  BSplineSolverStatus::eType solve(
    const Matrix<_T>& DmY, const Vector<int32_t>& m,
    const Vector<_T>& x, const Vector<_T>& w);

  //---------------------------------------------------------------------
  /// \brief  Function to compute the B-Spline curve of given order on the
  /// specified knot vector that approximates the input data. The input data
  /// is assumed to contain positional (zero^th order) samples only. The unit
  /// weight vector is also used within the main solve procedure.
  ///
  /// \param  Y - The N by dims matrix of data samples. All samples are zero^th order.
  /// \param  x - The vector of data sites of length N which is assumed to be
  ///             monotonically increasing. i.e. All data is input sorted on parameter x.
  BSplineSolverStatus::eType solve(
    const Matrix<_T>& Y, const Vector<_T>& x);
  //@}

  //---------------------------------------------------------------------
  /// \name   Solver Data
  /// \brief  Functions get the status of the solve and the computed curve
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief  Get the status of the solve
  BSplineSolverStatus::eType getStatus() const;

  //---------------------------------------------------------------------
  /// \brief  Test if the solver failed to compute a B-spline curve
  bool fail() const;

  //---------------------------------------------------------------------
  /// \brief  Function to get the computed B-Spline curve
  const BSplineCurve<_T>& getCurve() const;
  BSplineCurve<_T>& getCurve();
  //@}

private:
  /// Computation status
  BSplineSolverStatus::eType      m_status;

  /// B-Spline curve
  BSplineCurve<_T>                m_curve;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_BSPLINE_SOLVER_H
