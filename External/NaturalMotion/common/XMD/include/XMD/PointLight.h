//----------------------------------------------------------------------------------------------------------------------
/// \file   PointLight.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file defines the class XPointLight.
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/ExtendedLight.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XPointLight
/// \brief A positional light
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XPointLight
  : public XExtendedLight
{
  XMD_CHUNK(XPointLight);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kLast        = XExtendedLight::kLast;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Point Light Params
  /// \brief  Sets/Gets the params for the XDirectional
  //@{
  //@}
};

#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::PointLight, XPointLight, "pointLight", "POINT_LIGHT");
#endif
}
