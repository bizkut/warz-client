//----------------------------------------------------------------------------------------------------------------------
/// \file   HermitePatch.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines a hermite patch type
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/ParametricSurface.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XHermitePatch
/// \brief Basically the same as a 4x4 bezier patch, however the curves 
///        are defined by tangents rather than control points.
class XMD_EXPORT XHermitePatch
  : public XParametricSurface
{
  XMD_CHUNK(XHermitePatch);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kLast = XParametricSurface::kLast;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Hermite Patch Node Params
  /// \brief  Sets/Gets the params for the XHermitePatch class
  //@{
  //@}
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::HermitePatch, XHermitePatch, "hermitePatch", "HERMITE_PATCH");
#endif
}
