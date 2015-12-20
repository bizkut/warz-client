//----------------------------------------------------------------------------------------------------------------------
/// \file   Radial.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines a radial dynamics field
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Field.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XRadial
/// \brief A radial field effect
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XRadial :
  public XField
{
  XMD_CHUNK(XRadial);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kRadialType = XField::kLast+1; // XReal
  static const XU32 kLast       = kRadialType;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Radial Dynamics Field Attributes
  /// \brief  Sets/Gets the params for the XRadial dynamics field
  //@{

  /// \brief  a function to return the radial type of the field. The 
  ///         value varies between 0 and 1, where zero indicates that
  ///         the magnitude smooths up to the max distance, whereas
  ///         1 gives an immediate drop off when you hit the max distance.
  /// \return a value between 0 and 1
  XReal GetRadialType() const;

  /// \brief  a function to set the radial type of the field. The 
  ///         value varies between 0 and 1, where zero indicates that
  ///         the magnitude smooths up to the max distance, whereas
  ///         1 gives an immediate drop off when you hit the max distance.
  /// \param  type - the radial type value
  void SetRadialType(XReal type);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  XReal m_RadialType;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::Radial, XRadial, "radial", "RADIAL_FIELD");
#endif
}
