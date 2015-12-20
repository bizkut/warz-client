//----------------------------------------------------------------------------------------------------------------------
/// \file   VolumeObject.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines the base class for volume primitives. These primitives are 
///         frequently used within maya to determine volume fog regions, light 
///         cones etc. 
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Shape.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XVolumeObject
/// \brief the generic base class for all volume object shapes
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XVolumeObject :
  public XShape
{
  XMD_ABSTRACT_CHUNK(XVolumeObject);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kLast          = XShape::kLast;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  Common Volume Object Params
  //@{
  //@}
};
#ifndef DOXYGEN_PROCESSING
DECLARE_API_TYPE(XFn::VolumeObject, XVolumeObject, "volumeObject");
#endif
}
