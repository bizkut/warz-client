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
#ifndef NM_QUAT_SPLINE_FITTER_INSERT_KNOT_H
#define NM_QUAT_SPLINE_FITTER_INSERT_KNOT_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMNumerics/NMQuatSplineFitterSmoothedC1.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Quaternion spline curve fitting class. Refit a cubic quaternion
/// spline to the quaternion sample data corresponding to the two new span
/// created by inserting a specified knot into the original quaternion spline.
class QuatSplineFitterInsertKnot : public QuatSplineFitterBase
{
public:
  //---------------------------------------------------------------------
  /// \name   Constructors
  /// \brief  Functions to insert a knot into a quaternion spline or preallocate memory
  //---------------------------------------------------------------------
  //@{
  QuatSplineFitterInsertKnot(
    uint32_t          numSamples, ///< Number of data samples
    const float*      ts,         ///< The array of time samples
    const Quat*       quats,      ///< The array or quaternion keyframes sampled at the time positions
    const QuatSpline& qsp,        ///< The quaternion spline that is assumed to fit the data
    float             t           ///< Location of the knot site to add
  );

  QuatSplineFitterInsertKnot(uint32_t maxNumSamples);

  // Destructor
  ~QuatSplineFitterInsertKnot();
  //@}

  //---------------------------------------------------------------------
  /// \name   Fitting functions
  /// \brief  Functions to refit a quaternion spline by inserting a specified
  /// knot and fitting the affected spans to the provided data samples. The
  /// fitting algorithm fixes the quaternion keyframes at the start and end
  /// of the affected spans and optimises the mid knot quat key and tangent
  /// vectors to compute the best fitting curve spans that have continuity C0.
  /// The tangent vectors at the start and end of the affected spans are also
  /// fitted in a weighted fashion.
  ///
  /// Note that you must have set the quaternion spline and the data samples
  /// before calling any of these functions.
  //---------------------------------------------------------------------
  //@{
  bool insertKnot(float t);
  //@}

  //---------------------------------------------------------------------
  /// \name   Data functions
  /// \brief  Functions to modify the fitting weights
  //@{
  float getWeightWa() const { return m_weight_wa; }
  void setWeightWa(float weight_wa) { m_weight_wa = weight_wa; }
  float getWeightWc() const { return m_weight_wc; }
  void setWeightWc(float weight_wc) { m_weight_wc = weight_wc; }
  //@}

protected:
  // Optimisation parameters for the C0 system: P = [wa; rb; v; wc]
  Vector<double>                     m_P;
  QuatSplineFitterSmoothedC0FuncJac* m_lsqFunc;
  NonLinearOptimiser<double>*        m_optimiser;

  // Span information data
  SpanInfoSmoothed m_spanInfo1;
  SpanInfoSmoothed m_spanInfo2;
  float            m_weight_wa;
  float            m_weight_wc;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_QUAT_SPLINE_FITTER_INSERT_KNOT_H
