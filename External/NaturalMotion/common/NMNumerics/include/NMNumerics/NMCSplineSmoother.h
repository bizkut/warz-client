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
#ifndef NM_CSPLINE_SMOOTHER_H
#define NM_CSPLINE_SMOOTHER_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMatrix.h"
#include "NMNumerics/NMBSplineCurve.h"
//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Used to return the status of the smoothing spline solver
class CSplineSmootherStatus
{
public:
  enum eType {
    Success, // Successfully computed smoothing spline
    Divergent, // Roundoff error in vicinity of target tolerance
    MaxIterations, // Max iterations reached
    NotPosDef, // Symmetric system L L^T is not pos-def due to round-off error
    Singular, // Poor system
    Multiplicity, // Some data sites are repeated
    BadParams // Spectacularly poor system
  };
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Cubic Smoothing B-Spline solver
///
/// This is an implementation of the paper:
///
/// @article{ Boor98smoothingspline,
///   author = "Carl De Boor",
///   title = "Calculation of the smoothing spline with weighted roughness measure",
///   journal = "Math. Models Methods Appl. Sci",
///   year = "1998",
///   volume = "11",
///   pages = "33--41" }
///
/// DESCRIPTION:
///
/// The smoothing spline C(t) minimizes the roughness measure:
///
/// F( D^(2)C(t) ) = \integral || (D^(2)C)(t) ||^2 dt 
/// on the interval min(x) < t < max(x),
/// where D^(2)f is the second derivative of the B-spline f.
///
/// over all B-spline functions C(t) for which the weighted fitting
/// measurement error:
///
/// E( C(t) ) = sum_j w(j) * || (Y(:,j) - C(x(j))) ||^2
/// j = [0:length(x)-1]
///
/// is equal (within a supplied +/- precision) to the given
/// variance tolerance: tol. The specified precision eps is determined
/// as a scalar factor of the variance tolerance eps = fac * tol.
///
/// This is achieved by minimizing a single smoothing parameter p
/// of the functional:
///
/// p * E( C(t) ) + F( (D^(2)C)(t) )
///
/// so that E( C(t) ) equals the residual data variance tolerance: tol
///
/// When tol = 0 then the solver computes the interpolating cubic
/// B-spline of the input data Y at sites x.
///
/// As tol --> \infty then p --> 0 and the minimization is on the second
/// derivative (D^(2)f) of the B-spline C(t) i.e. the acceleration of the
/// curvature. This shapes the B-spline toward a straight line.
///
/// If the measurement weights are not given explicitly then they are
/// chosen so that E( C(t) ) is the composite Trapezoid rule approximation
/// for the integral F( Y - C(t) ).
///
/// The form of the linear system ensures that the smoothing parameter
/// converges very quickly. The minimization is achieved by first performing
/// a Newton iteration of the linear system at p=0, thereafter knowing the
/// the current and last error estimates it is faster to use the secant
/// method.
///
template <typename _T>
class CSplineSmoother
{
public:
  //---------------------------------------------------------------------
  /// \name   Constructors
  /// \brief  Functions to compute a cubic B-Spline smoothing curve
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief  Constructor to initialize a system for solving
  /// \param  dims the number of dimensions of the data.
  /// \param  tol is the target residual data variance tolerance
  CSplineSmoother(uint32_t dims, _T tol);

  //---------------------------------------------------------------------
  /// \brief  Constructor to compute the smoothing spline
  /// \param  x is a 1D data vector of sample sites (n - vector)
  /// \param  Y is a (n by dims) matrix of data samples at sites x
  /// \param  w is a 1D fitting weight vector for the data samples (n - vector)
  /// \param  tol is the target residual data variance tolerance
  CSplineSmoother(const Vector<_T>& x, const Matrix<_T>& Y, const Vector<_T>& w, _T tol);

  //---------------------------------------------------------------------
  /// \brief  Constructor to compute the smoothing spline using the
  /// composite trapezoid rule weighting strategy.
  ///
  /// \param  x is a 1D data vector of sample sites (n - vector)
  /// \param  Y is a (n by dims) matrix of data samples at sites x
  /// \param  tol is the target residual data variance tolerance
  CSplineSmoother(const Vector<_T>& x, const Matrix<_T>& Y, _T tol);

  //---------------------------------------------------------------------
  /// \brief  Constructor to compute the smoothing spline with uniformly spaced
  /// samples x = [0:n-1] and the composite trapezoid rule weights.
  ///
  /// \param  Y is a (n by dims) matrix of data samples at sites x
  /// \param  tol is the target residual data variance tolerance
  CSplineSmoother(const Matrix<_T>& Y, _T tol);

  //---------------------------------------------------------------------
  /// Destructor
  ~CSplineSmoother();
  //@}

  //---------------------------------------------------------------------
  /// \name   Solve functions
  /// \brief  Functions to fit a smoothing B-spline to the data
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief  Function to fit a smoothing B-spline to the data
  /// \param  x is a 1D data vector of sample sites (n - vector)
  /// \param  Y is a (n by dims) matrix of data samples at sites x
  /// \param  w is a 1D fitting weight vector for the data samples (n - vector)
  CSplineSmootherStatus::eType
  solve(const Vector<_T>& x, const Matrix<_T>& Y, const Vector<_T>& w);

  //---------------------------------------------------------------------
  /// \brief  Function to fit a smoothing B-spline to the data
  /// \param  x is a 1D data vector of sample sites (n - vector)
  /// \param  Y is a (n by dims) matrix of data samples at sites x
  CSplineSmootherStatus::eType
  solve(const Vector<_T>& x, const Matrix<_T>& Y);

  //---------------------------------------------------------------------
  /// \brief  Function to fit a smoothing B-spline to the data
  /// \param  Y is a (n by dims) matrix of data samples at sites x = [0:n-1]
  CSplineSmootherStatus::eType
  solve(const Matrix<_T>& Y);
  //@}

  //---------------------------------------------------------------------
  /// \name   Solver Data
  /// \brief  Functions get the status of the solve and the computed curve
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief  Get the computed curve
  const BSplineCurve<_T>& getCurve() const;
  BSplineCurve<_T>& getCurve();

  //---------------------------------------------------------------------
  /// \brief  Get the computed smoothing parameter
  _T getSmoothingParam() const;

  //---------------------------------------------------------------------
  /// \brief  Error functional E( C(t) ), where
  /// E( C(t) ) = sum_j w(j) * || (Y(:,j) - C(x(j))) ||^2
  ///
  /// \return  The actual residual data variance between the sample data
  /// and the computed smoothing spline. If the fit is good this should
  /// be very close to the fitting tolerance tol
  _T getError() const;

  //---------------------------------------------------------------------
  /// \brief  Fitting tolerance parameter
  _T getTol() const;
  void setTol(_T tol);

  //---------------------------------------------------------------------
  /// \brief  Termination condition factor
  _T getEpsFac() const;
  void setEpsFac(_T fac);

  //---------------------------------------------------------------------
  /// \brief  Maximum number of iterations
  uint32_t getMaxIterations() const;
  void setMaxIterations(uint32_t n);

  //---------------------------------------------------------------------
  /// \brief  Get the status of the solve
  CSplineSmootherStatus::eType getStatus() const;

  //---------------------------------------------------------------------
  /// \brief  Tests if the solver completely failed to compute a smoothing spline
  bool fail() const;
  //@}

protected:
  //---------------------------------------------------------------------
  /// \name   Utility solve functions
  /// \brief  Special case functions that perform the actual computation
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief  Main solve routine to compute the cubic smoothing spline
  CSplineSmootherStatus::eType
  doSolve(const Vector<_T>& x, const Matrix<_T>& Y, const Vector<_T>& w);

  //---------------------------------------------------------------------
  /// \brief  Fit single data point to a B-Spline of order 1
  CSplineSmootherStatus::eType
  doCurve1(const Vector<_T>& x, const Matrix<_T>& Y);

  //---------------------------------------------------------------------
  /// \brief  Fit two data points to a B-Spline of order 2
  CSplineSmootherStatus::eType
  doCurve2(const Vector<_T>& x, const Matrix<_T>& Y);

  //---------------------------------------------------------------------
  /// \brief  Fit three data points to a B-Spline of order 3
  CSplineSmootherStatus::eType
  doCurve3(const Vector<_T>& x, const Matrix<_T>& Y);

  //---------------------------------------------------------------------
  /// \brief  When tol=0 then the curve we compute is the interpolating B-spline
  CSplineSmootherStatus::eType
  doInterpolate(const Vector<_T>& x, const Matrix<_T>& Y);

  //---------------------------------------------------------------------
  /// \brief  Initialize default values for the solver
  void init();

  //---------------------------------------------------------------------
  /// \brief  Compute the composite Trapezoid rule weights
  void weights(const Vector<_T>& x, Vector<_T>& w) const;
  //@}

private:
  /// Target residual data variance tolerance
  _T                              m_tol;

  /// Termination condition tolerance multiplier. i.e. scaled factor of m_tol
  _T                              m_epsFac;

  /// Maximum number of iterations to perform
  uint32_t                        m_maxIts;

  /// Computed smoothing parameter
  _T                              m_rho;

  /// Computed error functional E( C(t) )
  _T                              m_Ep;

  /// Computation status
  CSplineSmootherStatus::eType    m_status;

  /// Fitted B-Spline curve
  BSplineCurve<_T>                m_curve;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_CSPLINE_SMOOTHER_H
