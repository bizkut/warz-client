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
#ifndef NM_PPOLY_CURVE_H
#define NM_PPOLY_CURVE_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMNumerics/NMCurveBase.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Piecewise Polynomial curve
///
/// Represent the curve in shifted power form:
/// \f$ C_i(x) = p_{1,i} + p_{2,i} \cdot (x-t_i) + ... + p_{k,i} \cdot (x-t_i)^{k-1} \f$
///
/// Store piecewise coefficients in coefs as an order by (pieces*dims) matrix
/// \f$ C = [p_1 | p_2 | ... | p_d] \f$, where \f$p_i\f$ is the order by pieces
/// matrix plane corresponding to the data samples of a single dimension. pieces
/// corresponds to the number of curve spans that make up the polynomial curve.
/// Coefficients of the shifted power form are stored lowest to highest corresponding
/// to the polynomial power down the columns of the \f$p_i\f$.
///
template <typename _T>
class PPolyCurve : public CurveBase<_T>
{
public:
  //---------------------------------------------------------------------
  /// \name   Constructors
  /// \brief  Functions to preallocate memory for a piecewise polynomial curve
  //---------------------------------------------------------------------
  //@{
  PPolyCurve();
  PPolyCurve(uint32_t order, uint32_t dims);
  PPolyCurve(uint32_t order, uint32_t dims, uint32_t number); // number breaksites
  PPolyCurve(uint32_t order, uint32_t dims, const Vector<_T>& breaks, const Matrix<_T>& coefs);
  PPolyCurve(const PPolyCurve& curve);

  // Destructor
  ~PPolyCurve();
  //@}

  //---------------------------------------------------------------------
  /// \name   Curve coefficients
  /// \brief  Functions to get / set the curve coefficients
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief  Shifted power form coefficients \f$p_{j,i}\f$
  /// \param  j - j^th order coefficients
  /// \param  i - i^th span of the polynomial curve
  /// \param  pj - A vector of length dims for the coefficients
  void getCoef(uint32_t j, uint32_t i, Vector<_T>& pj) const;
  void setCoef(uint32_t j, uint32_t i, const Vector<_T>& pj);
  //@}

  //---------------------------------------------------------------------
  /// \name   Knot vector / Intervals
  /// \brief  Functions to determine which knot interval a parameter lies in,
  ///         unique breaksites and curve spans.
  //---------------------------------------------------------------------
  //@{
  virtual uint32_t spansNum() const;

  //---------------------------------------------------------------------
  /// \brief  Knot insertion
  virtual void insertKnot(_T x, uint32_t mult = 1);

  //---------------------------------------------------------------------
  /// \brief  Knot removal
  virtual void removeKnot(uint32_t intv, uint32_t mult = 1);
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
  /// \brief  Get the curve segment between x0 and x1
  virtual CurveBase* curveSegment(_T x0, _T x1) const;
  //@}
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_PPOLY_CURVE_H
