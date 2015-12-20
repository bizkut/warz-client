//----------------------------------------------------------------------------------------------------------------------
/// \file   AreaLight.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file defines the class XAreaLight.
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/ExtendedLight.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XAreaLight
/// \brief An area light type.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XAreaLight
  : public XExtendedLight
{
  XMD_CHUNK(XAreaLight);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kLast = XExtendedLight::kLast;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Area Light Params
  /// \brief  Sets/Gets the params for the XAreaLight
  //@{
  //@}
};

#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::AreaLight, XAreaLight, "areaLight", "AREA_LIGHT");
#endif
}
