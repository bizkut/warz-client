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
#ifndef NM_CSPLINE_INTERPOLATOR_H
#define NM_CSPLINE_INTERPOLATOR_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMatrix.h"
#include "NMNumerics/NMSolverTriDiag.h"
#include "NMNumerics/NMPPolyCurve.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Boundary condition specifiers
class CSplineBoundaryType
{
public:
  enum eType {
    FirstDerivative,
    SecondDerivative,
    NotAKnot
  };
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Cubic Spline interpolation with various end conditions
///
template <typename _T>
class CSplineInterpolator : protected SolverTriDiag<_T>
{
public:
  //---------------------------------------------------------------------
  /// \name   Constructors
  /// \brief  Constructors to compute or preallocate memory for a linear system
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief  Constructor to compute the interpolating cubic spline with
  /// the natural spline end conditions: C''(a) = C''(b) = 0
  CSplineInterpolator(const Matrix<_T>& X, const Vector<_T>& t);
  CSplineInterpolator(const Matrix<_T>& X); // t = [0:n-1]

  //---------------------------------------------------------------------
  /// \brief  Constructor to compute the interpolating cubic spline with
  /// the specified end conditions given by the supplied vectors
  CSplineInterpolator(
    const Matrix<_T>& X, const Vector<_T>& t,
    CSplineBoundaryType::eType startC, const Vector<_T>& startV,
    CSplineBoundaryType::eType endC, const Vector<_T>& endV);

  //---------------------------------------------------------------------
  /// \brief  Constructor to preallocate the memory for the cubic spline
  /// interpolation with the natural spline end conditions: C''(a) = C''(b) = 0
  CSplineInterpolator(uint32_t n, uint32_t dims);

  //---------------------------------------------------------------------
  // Destructor
  ~CSplineInterpolator();
  //@}

  //---------------------------------------------------------------------
  /// \name   Repeat Solve functions
  /// \brief  Functions to compute the interpolating cubic spline curve
  //---------------------------------------------------------------------
  //@{
  SolverTriDiagStatus::eType solve(const Matrix<_T>& X, const Vector<_T>& t);
  //@}

  //---------------------------------------------------------------------
  /// \name   Interpolator Data
  /// \brief  Functions get / set the end conditions, the status of the solve
  ///         and the computed curve.
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// Start boundary condition types
  CSplineBoundaryType::eType getStartC() const;
  void setStartC(CSplineBoundaryType::eType startC);

  //---------------------------------------------------------------------
  /// Start end-condition vectors
  const Vector<_T>& getStartV() const;
  Vector<_T>& getStartV();
  void setStartV(const Vector<_T>& v);

  //---------------------------------------------------------------------
  /// End boundary condition types
  CSplineBoundaryType::eType getEndC() const;
  void setEndC(CSplineBoundaryType::eType endC);

  //---------------------------------------------------------------------
  /// End end-condition vectors
  const Vector<_T>& getEndV() const;
  Vector<_T>& getEndV();
  void setEndV(const Vector<_T>& v);

  //---------------------------------------------------------------------
  /// \brief  Get the status of the solve
  SolverTriDiagStatus::eType getStatus() const;
  bool fail() const;

  //---------------------------------------------------------------------
  /// \brief  Function to get the computed interpolating polynomial
  const PPolyCurve<_T>& getCurve() const;
  PPolyCurve<_T>& getCurve();
  //@}

private:
  /// Define the start and end boundary constraint types
  CSplineBoundaryType::eType      m_startC;
  CSplineBoundaryType::eType      m_endC;
  Vector<_T>                      m_startV;
  Vector<_T>                      m_endV;

  /// Divided difference and delta sample data
  Vector<_T>                      m_dt;
  Matrix<_T>                      m_divdif;

  /// Piecewise polynomial interpolating curve
  PPolyCurve<_T>                  m_curve;

private:
  /// Temporary workspace memory
  Vector<_T>                      m_temp;
  Vector<_T>                      m_q;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_CSPLINE_INTERPOLATOR_H
