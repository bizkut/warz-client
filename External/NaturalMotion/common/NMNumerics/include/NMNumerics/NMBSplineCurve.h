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
#ifndef NM_BSPLINE_CURVE_H
#define NM_BSPLINE_CURVE_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMNumerics/NMCurveBase.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  B-Spline curve \f$ C(x) = \sum_{i=1}^{n} N_{i,k}(x) P_{i} \f$
///
/// Define the B-Spline basis functions \f$N_{i,k}(x)\f$, where i is the i^th basis
/// function of order k, by the recurrence relation:
///
/// \f{eqnarray*}
/// N_{i,1}(x) & = & \left\{
///   \begin{array}{ll}
///     1 & \mbox{if $t_i \le x < t_{i+1}$ (inclusive on end knot)} \\
///     0 & \mbox{otherwise}
///   \end{array}
/// \right. \\[8pt]
///
/// N_{i,k}(x) & = &
///   \frac{x - t_i}{t_{i+k-1} - t_{i}} N_{i,k-1}(x) +
///   \frac{t_{i+k} - x}{t_{i+k} - t_{i+1}} N_{i+1,k-1}(x)
/// \f}
///
/// The knot vector that breaks the curve into parameterized segments is
/// strictly increasing with possible multiplicities.
/// knots: \f$ T = [t_1, t_2, .... , t_{n+k}] \f$, i.e. [0,0,0,0,1,1,1,1] gives
/// the knots sequence for an open uniform cubic B-spline with a single curve span.
///
/// Store B-Spline coefficients in C as a dims by numC matrix
//----------------------------------------------------------------------------------------------------------------------
template <typename _T>
class BSplineCurve : public CurveBase<_T>
{
public:
  //---------------------------------------------------------------------
  /// \name   Constructors
  /// \brief  Functions to preallocate memory for a B-spline curve
  //---------------------------------------------------------------------
  //@{
  BSplineCurve();
  BSplineCurve(uint32_t order, uint32_t dims);
  BSplineCurve(uint32_t order, uint32_t dims, uint32_t number); // number of ctrl pts
  BSplineCurve(uint32_t order, uint32_t dims, const Vector<_T>& knots); // zeros ctrl pts
  BSplineCurve(uint32_t order, uint32_t dims, const Vector<_T>& knots, const Matrix<_T>& coefs); // dims by numC matrix
  BSplineCurve(uint32_t order, uint32_t dims, const Vector<_T>& breaks, const Vector<int32_t>& mults);
  BSplineCurve(uint32_t order, uint32_t dims, uint32_t spans, const Vector<_T>& x); // make suitable knots
  BSplineCurve(const BSplineCurve& curve);

  // Destructor
  ~BSplineCurve();
  //@}

  //---------------------------------------------------------------------
  /// \name   Knot vector / Intervals
  /// \brief  Functions to manipulate or create a suitable knot vector
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief Determines the knot index of the last span. i.e. the index of the
  /// knot left of the possibly m-multiple end knot(s).
  virtual uint32_t endSpan() const;

  //---------------------------------------------------------------------
  /// \brief  Get the number of curve spans. This includes all zero
  /// length spans obtained from knot multiplicities
  virtual uint32_t spansNum() const;

  //---------------------------------------------------------------------
  /// \brief  Get the actual number of repeated knots at the start and
  /// end of the knot vector.
  uint32_t multiplicityStart() const;
  uint32_t multiplicityEnd() const;

  //---------------------------------------------------------------------
  /// \brief  Make the knot vector open uniform between the specified
  /// start and end values.
  void knotsOpenUniform(_T start, _T end);

  //---------------------------------------------------------------------
  /// \brief  Make an arc-length parameterisation of the knot vector using
  /// the specified control points. X is a dims by number matrix of control
  /// points.
  void knotsOpenArcLength(const Matrix<_T>& X);

  //---------------------------------------------------------------------
  /// \brief  Make a suitable open uniform knot vector using the parameter
  /// data vector x. This ensures that all knot spans contain at least one
  /// data sample.
  void knotsOpen(const Vector<_T>& x);

  //---------------------------------------------------------------------
  /// \brief  Calculate the number of knots for the current curve order
  /// given the desired number of curve spans.
  static uint32_t numKnotsFromSpans(uint32_t order, uint32_t n);
  uint32_t numKnotsFromSpans(uint32_t n) const;

  //---------------------------------------------------------------------
  /// \brief  Calculate the number of control points for the current curve
  /// order given the desired number of curve spans.
  static uint32_t numCtrlPtsFromKnots(uint32_t order, uint32_t numKnots);
  static uint32_t numCtrlPtsFromSpans(uint32_t order, uint32_t n);
  uint32_t numCtrlPtsFromSpans(uint32_t n) const;

  //---------------------------------------------------------------------
  /// \brief  Knot insertion
  virtual void insertKnot(_T x, uint32_t mult = 1);

  //---------------------------------------------------------------------
  /// \brief  Knot removal
  virtual void removeKnot(uint32_t intv, uint32_t mult = 1);
  //@}

  //---------------------------------------------------------------------
  /// \name   Control points
  /// \brief  Functions to get / set the curve control points
  //---------------------------------------------------------------------
  //@{
  uint32_t getNumControlPoints() const;
  uint32_t getControlPointIndexFromKnotInterval(uint32_t intv) const;
  uint32_t getKnotIntervalFromControlPointIndex(uint32_t i) const;
  void getControlPoint(uint32_t i, Vector<_T>& x) const;
  void getControlPoint(uint32_t i, _T* x) const;
  void setControlPoint(uint32_t i, const Vector<_T>& x);
  void setControlPoint(uint32_t i, const _T* x);
  void linearControlPoints(const Vector<_T>& x0, const Vector<_T>& x1);
  void transformControlPoints(const Vector<_T>& scales, const Vector<_T>& offsets); // linear transform
  //@}

  //---------------------------------------------------------------------
  /// \name   Basis functions
  /// \brief  Functions to compute the non-vanishing basis functions on
  ///         the specified knot span i with the curve parameter x
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief  Compute the k non-vanishing basis functions at x on knot span i:
  /// \f$ N_{i-k+1,k}(x), \cdots , N_{i,k}(x) \f$ where k is the order of the basis.
  ///
  /// \param  x - The value that lies within the knot span \f$ t_i \le x < t_{i+1} \f$
  /// to evaluate the basis function at.
  ///
  /// \param  Nik - The vector of length order for the evaluated set of
  /// basis functions \f$ N_{i-k+1,k}(x), \cdots , N_{i,k}(x) \f$
  void basis(uint32_t i, _T x, Vector<_T>& Nik) const;

  //---------------------------------------------------------------------
  ///  \brief  Compute the [0:m] non-vanishing derivatives of the basis
  ///  functions at x on knot span i:
  /// 
  ///  \f[
  ///  DN_{i,k}(x) = \left(
  ///  \begin{array}{ccc}
  ///    D^{(0)} N_{i-k+1,k}(x) & \cdots & D^{(m)} N_{i-k+1,k}(x) \\
  ///    \vdots & \ddots & \vdots \\
  ///    D^{(0)} N_{i,k}(x) & \cdots & D^{(m)} N_{i,k}(x)
  ///  \end{array}
  ///  \right)
  ///  \f]
  /// 
  ///  \param  x - The value that lies within the knot span \f$ t_i \le x < t_{i+1} \f$
  ///  to evaluate the basis function derivatives at.
  /// 
  ///  \param  DNik - The order by (m+1) matrix of basis function derivatives
  ///  evaluated at x. The number of columns in DNik determines the number of
  ///  basis function derivatives to compute. If all derivatives are required
  ///  then you must supply an order by order matrix to the function.
  void basisDerivatives(uint32_t i, _T x, Matrix<_T>& DNik) const;
  //@}

  //---------------------------------------------------------------------
  /// \name   Evaluation
  /// \brief  Functions to evaluate the position on the curve, compute the
  ///         curve derivative or integral.
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief  Evaluate the position on the curve \f$C(x)\f$ at x.
  ///
  /// \return  The span interval region as returned from interval(). The
  /// vector v is filled with the computed curve vector \f$C(x)\f$.
  virtual int32_t evaluate(_T x, uint32_t& intv, Vector<_T>& v, bool flag = false) const;

  //---------------------------------------------------------------------
  /// \brief  Evaluate the positions on the curve \f$C(x_i)\f$ at all \f$x_i\f$.
  ///
  /// \param  x - Vector of \f$x_i\f$. parameters of length N
  /// \param  v - N by dims matrix of computed curve values \f$C(x_i)\f$
  virtual void evaluate(const Vector<_T>& x, Matrix<_T>& v) const;

  //---------------------------------------------------------------------
  /// \brief  Evaluate the j^th derivative of the curve \f$D^{(j)}C(x)\f$ at x.
  ///
  /// \return  The span interval region as returned from interval(). The
  /// vector v is filled with the computed derivative vector \f$ D^{(j)}C(x)\f$
  virtual int32_t derivative(uint32_t j, _T x, uint32_t& intv, Vector<_T>& v, bool flag = false) const;

  //---------------------------------------------------------------------
  /// \brief  Evaluate the j^th derivative of the curve \f$D^{(j)}C(x_i)\f$ at \f$x_i\f$.
  ///
  /// \param  x - Vector of \f$x_i\f$. parameters of length N
  /// \param  v - N by dims matrix of computed curve derivative values \f$D^{(j)}C(x_i)\f$
  virtual void derivative(uint32_t j, const Vector<_T>& x, Matrix<_T>& v) const;

  //---------------------------------------------------------------------
  /// \brief  Compute the j^th derivatives of the curve \f$D^{(j)}C(x)\f$ over the entire curve,
  /// returning the result as a new curve of similar type. The computed derivative curve
  /// retains the same break sites as the original curve.
  virtual CurveBase* derivative(uint32_t j = 1) const;

  //---------------------------------------------------------------------
  /// \brief  Computes a curve representation of the indefinite integral that is
  /// zero (or specified value) at the left of the basic interval
  virtual CurveBase* integral(const Vector<_T>& va) const;
  virtual CurveBase* integral() const;
  //@}

  //---------------------------------------------------------------------
  /// \name   Curve management
  /// \brief  Functions to manipulate curve segments
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief  Convert the curve to Bezier form by ensuring all interior
  /// knots have a multiplicity of order-1
  void convertToBezierForm();

  //---------------------------------------------------------------------
  /// \brief  Get the curve segment between x0 and x1, returning the
  /// B-Spline curve with open knots between [x0, x1]
  virtual CurveBase* curveSegment(_T x0, _T x1) const;
  //@}
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_BSPLINE_CURVE_H
