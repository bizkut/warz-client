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
#ifndef NM_POLYLINE_H
#define NM_POLYLINE_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMNumerics/NMBSplineCurve.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Poly-Line \f$C(x)\f$
///
/// \f$ C(x) = \sum_{i=1}^{n} N_{i,2}(x) P_{i} \f$
///
/// Is a B-Spline curve with order two that corresponds to a set of straight
/// line segments between the specified control points. The default knot vector
/// represents the arc-length parameterisation along the curve.
///
template <typename _T>
class PolyLine : public BSplineCurve<_T>
{
public:
  //---------------------------------------------------------------------
  /// \name   Constructors
  /// \brief  Functions to preallocate memory for a polyline curve
  //---------------------------------------------------------------------
  //@{
  PolyLine();
  PolyLine(uint32_t dims);
  PolyLine(uint32_t dims, uint32_t number); // number of ctrl pts
  PolyLine(const Matrix<_T>& X);
  PolyLine(const PolyLine& curve);

  // Destructor
  ~PolyLine();
  //@}
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_POLYLINE_H
