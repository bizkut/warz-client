//----------------------------------------------------------------------------------------------------------------------
/// \file   HermiteCurve.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines a hermite curve geometry primitive
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/ParametricCurve.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XHermiteCurve
/// \brief A pretty cacky simple class to hold a cubic hermite curve  
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XHermiteCurve
  : public XParametricCurve
{
  XMD_CHUNK(XHermiteCurve);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kLast = XParametricCurve::kLast;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Hermite Curve Node Params
  /// \brief  Sets/Gets the params for the XHermiteCurve class
  //@{
  //@}
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::HermiteCurve, XHermiteCurve, "hermiteCurve", "HERMITE_CURVE");
#endif
}
