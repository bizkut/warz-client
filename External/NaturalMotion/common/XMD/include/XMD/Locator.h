//----------------------------------------------------------------------------------------------------------------------
/// \file   Locator.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file defines the interface to a simple point locator 
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Shape.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XLocator
/// \brief Defines a simple locator node
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XLocator :
  public XShape
{
  XMD_CHUNK(XLocator);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kLast = XShape::kLast;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Locator Node Params
  /// \brief  Sets/Gets the params for the XLocator class
  //@{
  //@}
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::Locator, XLocator, "locator", "LOCATOR");
#endif
}
