//----------------------------------------------------------------------------------------------------------------------
/// \file   ParametricBase.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines the base class of all parametric surfaces
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Geometry.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \brief  This class forms the base class of all parametrically curve
///         and surface types.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XParametricBase
  : public XGeometry
{
  XMD_ABSTRACT_CHUNK(XParametricBase);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kLast = XGeometry::kLast;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Parametric Base Class Params
  /// \brief  Sets/Gets the params for the XParametricBase class
  //@{
  //@}
};
#ifndef DOXYGEN_PROCESSING
DECLARE_API_TYPE(XFn::ParametricBase, XParametricBase, "parametricBase");
#endif
}
