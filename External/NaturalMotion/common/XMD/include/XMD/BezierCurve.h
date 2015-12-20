//----------------------------------------------------------------------------------------------------------------------
/// \file   BezierCurve.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file defines a simplifed bezier curve interface. This only 
///         has four points. For other curves, use nurbs. 
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/ParametricCurve.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XBezierCurve
/// \brief A simple class to hold a cubic bezier curve  
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XBezierCurve
  : public XParametricCurve
{
  XMD_CHUNK(XBezierCurve);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kLast       = XParametricCurve::kLast;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Bezier Curve Node Params
  /// \brief  Sets/Gets the params for the XBezierCurve class
  //@{
  //@}
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::BezierCurve, XBezierCurve, "bezierCurve", "BEZIER_CURVE");
#endif
}
