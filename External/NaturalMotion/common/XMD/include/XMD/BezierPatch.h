//----------------------------------------------------------------------------------------------------------------------
/// \file   BezierPatch.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This defines an interface for a 4x4 bezier patch. For other surface types, use nurbs.
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/ParametricSurface.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XBezierPatch
/// \brief this class handles a 4x4 bezier patch. 
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XBezierPatch
  : public XParametricSurface
{
  XMD_CHUNK(XBezierPatch);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kLast = XParametricSurface::kLast;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Bezier Patch Node Params
  /// \brief  Sets/Gets the params for the XBezierPatch class
  //@{
  //@}
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::BezierPatch, XBezierPatch, "bezierPatch", "BEZIER_PATCH");
#endif
}
