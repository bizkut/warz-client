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
#ifndef NM_QUAT_SPLINE_FITTER_REMOVE_KNOT_H
#define NM_QUAT_SPLINE_FITTER_REMOVE_KNOT_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMNumerics/NMQuatSplineFitterTangents.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Quaternion spline curve fitting class. Refit a single span cubic quaternion
/// spline to the quaternion sample data to the new span created by removing a specified
/// knot from the original quaternion spline.
class QuatSplineFitterRemoveKnot : public QuatSplineFitterTangents
{
public:
  //---------------------------------------------------------------------
  /// \name   Constructors
  /// \brief  Functions to remove a knot from a quaternion spline or preallocate memory
  //---------------------------------------------------------------------
  //@{
  QuatSplineFitterRemoveKnot(
    uint32_t          numSamples,    ///< Number of data samples
    const float*      ts,            ///< The array of time samples
    const Quat*       quats,         ///< The array or quaternion keyframes sampled at the time positions
    const QuatSpline& qsp,           ///< The quaternion spline that is assumed to fit the data
    uint32_t          indx           ///< Index of the knot site to remove
  );

  QuatSplineFitterRemoveKnot(uint32_t maxNumSamples);

  // Destructor
  ~QuatSplineFitterRemoveKnot();
  //@}

  //---------------------------------------------------------------------
  /// \name   Fitting functions
  /// \brief  Functions to refit a quaternion spline by removing a specified
  /// knot and fitting the affected span to the provided data samples. The
  /// fitting algorithm fixes the quaternion keyframes at the start and end
  /// of the affected span and optimises the tangent vectors to compute the
  /// best fitting curve span that has continuity C0.
  ///
  /// Note that you must have set the quaternion spline and the data samples
  /// before calling any of these functions.
  //---------------------------------------------------------------------
  //@{
  bool removeKnotAtIndex(uint32_t indx);
  bool removeKnot(float t);
  //@}
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_QUAT_SPLINE_FITTER_REMOVE_KNOT_H
