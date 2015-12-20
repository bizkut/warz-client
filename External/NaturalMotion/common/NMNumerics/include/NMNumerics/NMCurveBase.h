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
#ifndef NM_CURVE_BASE_H
#define NM_CURVE_BASE_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMatrix.h"
#include "NMPlatform/NMVector.h"

#include <iostream>
#include <fstream>
#include <map>

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Knot properties class to hold information about the knot vector
///
class KnotProperties
{
public:
  //---------------------------------------------------------------------
  KnotProperties(int32_t rgn, uint32_t intv, uint32_t mult);
  KnotProperties();
  ~KnotProperties();

  //---------------------------------------------------------------------
  int32_t getRegion() const { return m_region; }
  void setRegion(int32_t rgn) { m_region = rgn; }

  //---------------------------------------------------------------------
  uint32_t getInterval() const { return m_interval; }
  void setInterval(uint32_t intv) { m_interval = intv; }

  //---------------------------------------------------------------------
  uint32_t getMultiplicity() const { return m_multiplicity; }
  void setMultiplicity(uint32_t mult) { m_multiplicity = mult; }

private:
  int32_t  m_region;       ///< Region that x lies in: -1 outside left, 0 inside, 1 outside right
  uint32_t m_interval;     ///< Interval for knot span at x
  uint32_t m_multiplicity; ///< Multiplicity of the knot at x
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Curve base class that stores the curve coefficients and
/// manages searching knot spans and break sites
///
template <typename _T>
class CurveBase
{
public:
  //---------------------------------------------------------------------
  /// \name   Constructors
  /// \brief  Functions to preallocate memory for a curve
  //---------------------------------------------------------------------
  //@{
  CurveBase();
  CurveBase(uint32_t order, uint32_t dims);
  CurveBase(uint32_t order, uint32_t dims, uint32_t knots, uint32_t Cr, uint32_t Cc);
  CurveBase(uint32_t order, uint32_t dims, const Vector<_T>& knots, const Matrix<_T>& coefs);

  // Destructor
  virtual ~CurveBase();
  //@}

  //---------------------------------------------------------------------
  /// \name   Curve Information
  /// \brief  Functions to get the dimension and order of the curve
  //---------------------------------------------------------------------
  //@{
  uint32_t order() const;
  uint32_t degree() const;
  uint32_t dims() const;
  //@}

  //---------------------------------------------------------------------
  /// \name   Curve Data
  /// \brief  Functions to manipulate the internal curve data
  //---------------------------------------------------------------------
  //@{
  const Matrix<_T>& coefs() const;
  Matrix<_T>& coefs();

  const Vector<_T>& knots() const;
  Vector<_T>& knots();
  _T knots(uint32_t i) const;

  // Swaps contents with curve
  void swap(CurveBase& curve);
  //@}

  //---------------------------------------------------------------------
  /// \name   Knot vector / Intervals
  /// \brief  Functions to determine which knot interval a parameter lies in,
  ///         unique breaksites and curve spans.
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief  Checks if the value x corresponds to the start knot
  bool isStartKnot(_T x) const;

  //---------------------------------------------------------------------
  /// \brief  Checks if the value x corresponds to the end knot
  bool isEndKnot(_T x) const;

  //---------------------------------------------------------------------
  /// \brief  Determines the span interval that the value x lies within.
  /// The function uses a binary search to determine the span interval i such
  /// that \f$t_i \le x < t_{i+1}\f$ (and inclusive on end knot). If flag is set
  /// true then the function uses the input value within intv to perform an
  /// initial local search for the span. This is handy if we are iterating over
  /// consecutive span intervals.
  ///
  /// \return  -1 if left of basic interval, 0 if within the interval and 1
  /// if right of interval. On output intv contains the index i of the span \f$t_i\f$.
  int32_t interval(_T x, uint32_t& intv, bool flag = false) const;

  //---------------------------------------------------------------------
  /// \brief  Determines the set of span intervals that the values in
  /// vector x lie within.
  ///
  /// \return  true if all values in x lie within the valid knot spans
  /// and false otherwise.
  bool interval(const Vector<_T>& x, Vector<int32_t>& intvs) const;

  //---------------------------------------------------------------------
  /// \brief  Get the knot vector corresponding to the breaksites with
  /// the requires multiplicities
  static void knots(Vector<_T>& knots, const Vector<_T>& breaks, const Vector<int32_t>& mults);
  void knots(const Vector<_T>& breaks, const Vector<int32_t>& mults) { CurveBase<_T>::knots(m_knots, breaks, mults); }

  //---------------------------------------------------------------------
  /// \brief  Get the vector of unique break sites (no repeated knots)
  Vector<_T> breaks() const;

  //---------------------------------------------------------------------
  /// \brief  Get the vector of unique break sites (no repeated knots)
  /// and the corresponding vector of multiplicities
  static void breaks(const Vector<_T>& knots, Vector<_T>& breaks, Vector<int32_t>& mults);
  void breaks(Vector<_T>& breaks, Vector<int32_t>& mults) const { CurveBase<_T>::breaks(m_knots, breaks, mults); }

  //---------------------------------------------------------------------
  /// \brief  Get the number of unique break sites (no repeated knots)
  uint32_t numBreaks() const;

  //---------------------------------------------------------------------
  /// \brief  Get the number of spans the curve is split into
  virtual uint32_t spansNum() const = 0;

  //---------------------------------------------------------------------
  /// \brief  Determines the knot index of the last span. i.e. the index of the
  /// knot left of the possibly m-multiple end knot(s).
  virtual uint32_t endSpan() const;

  //---------------------------------------------------------------------
  /// \brief  Get the knot properties at x
  void knotProperties(_T x, KnotProperties& properties) const;

  //---------------------------------------------------------------------
  /// \brief  Normalise the knots within the range [x0, x1]
  void normaliseKnots(_T x0 = 0, _T x1 = 1);

  //---------------------------------------------------------------------
  /// \brief  Apply a linear transform to the knots
  void transformKnots(_T scale, _T offset);

  //---------------------------------------------------------------------
  /// \brief  Knot insertion
  virtual void insertKnot(_T x, uint32_t mult = 1) = 0;

  //---------------------------------------------------------------------
  /// \brief  Knot removal
  virtual void removeKnot(_T x, uint32_t mult = 1);
  virtual void removeKnot(uint32_t intv, uint32_t mult = 1) = 0;
  //@}

  //---------------------------------------------------------------------
  /// \name   Arc length parameterization
  /// \brief  Functions aimed at manipulating the curve in an arc-length
  ///         parameterized way.
  ///
  /// NOTE : 1) these functions use std::map which is NOT TO BE USED for
  ///           real-time applications on consoles. So please do not use
  ///           these functions in this case.
  ///        2) also note we decided that the mapping' samples should be
  ///           kept outside the curve object. This is because in some
  ///           situations CurveBase objects are used as transient helper
  ///           objects. In most situations CurveBase objects are thus
  ///           instantiated and deleted many times without changing at
  ///           all. Without keeping the mapping' samples outside of the
  ///           CurveBase objects, the map would need to be recomputed
  ///           each time the CurveObjects are instantiated.
  //---------------------------------------------------------------------
  //@{
  virtual void  computeArcLengthMap(std::map<_T, _T>& mapSamples, _T threshold = 0.0001) const;
  static  _T    arcLengthMap(      _T parameter, std::map<_T, _T>& mapSamples);
  static  _T    inverseArcLengthMap(_T length,    std::map<_T, _T>& mapSamples);
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
  virtual int32_t evaluate(_T x, uint32_t& intv, Vector<_T>& v, bool flag = false) const = 0;

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
  virtual int32_t derivative(uint32_t j, _T x, uint32_t& intv, Vector<_T>& v, bool flag = false) const = 0;

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
  virtual CurveBase* derivative(uint32_t j = 1) const = 0;

  //---------------------------------------------------------------------
  /// \brief  Computes a curve representation of the indefinite integral that is
  /// zero (or specified value) at the left of the basic interval
  virtual CurveBase* integral(const Vector<_T>& va) const = 0;
  virtual CurveBase* integral() const = 0;
  //@}

  //---------------------------------------------------------------------
  /// \name   Curve management
  /// \brief  Functions to manipulate curve segments
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief  Get the curve segment between x0 and x1
  virtual CurveBase* curveSegment(_T x0, _T x1) const = 0;
  //@}

  //---------------------------------------------------------------------
  /// \name   Projection
  /// \brief  Functions to compute the curve parameter that corresponds to the
  ///         position of a projected point onto the curve.
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief  Compute the curve parameter x and corresponding point C(x) by projecting
  /// the specified space point P onto the curve. Newton iteration is used to minimize
  /// the distance between P and C(x). The initial parameter for optimization is chosen by
  /// sampling the curve at n equally spaced parameter values and selecting the closest.
  /// For curves with order \le 2 there is an analytical solution for the closest point and
  /// in this case n has no meaning.
  ///
  /// \param  P - The space point to project onto the curve.
  /// \param  Cx - The projected point on the curve C(x).
  /// \param  n - The number of evenly spaced samples to compute between the knots.
  /// \return  The curve parameter x
  _T projectPoint(const Vector<_T>& P, Vector<_T>& Cx, uint32_t n, _T tolerance = (_T) - 1) const;

  //---------------------------------------------------------------------
  /// \brief  Compute the curve parameter x and corresponding point C(x) by projecting
  /// the specified space point P onto the curve. Newton iteration is used to minimize
  /// the distance between P and C(x). On entry x is an initial estimate for the curve
  /// parameter and on exit contains the optimized estimate. The function returns true
  /// if the parameter converged within the maximum number of iterations.
  bool projectPoint(const Vector<_T>& P, Vector<_T>& Cx, _T& x) const;
  //@}

protected:
  uint32_t        m_order; ///< Polynomial order of curve
  uint32_t        m_dims;  ///< Number of dimensions in data
  Vector<_T>      m_knots; ///< Knot or break site vector
  Matrix<_T>      m_coefs; ///< Coefficient matrix

  _T projectPoint_newton(const Vector<_T>& P, Vector<_T>& Cx, uint32_t n) const;
  _T projectPoint_bisection(const Vector<_T>& P, Vector<_T>& Cx, uint32_t n, _T tolerance) const;
};

template <typename _T> std::ostream& operator<<(std::ostream& os, const CurveBase<_T>& curve);

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_CURVE_BASE_H
