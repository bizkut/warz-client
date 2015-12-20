//----------------------------------------------------------------------------------------------------------------------
/// \file   Vortex.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines a vortex field
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Field.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XVortex
/// \brief This class defines a vortex field  
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XVortex :
  public XField
{
  XMD_CHUNK(XVortex);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kAxis  = XField::kLast+1; // XVector3
  static const XU32 kLast  = kAxis;
  //--------------------------------------------------------------------------------------------------------------------


  /// \name   Vortex Dynamics Field Attributes
  /// \brief  Sets/Gets the params for the XVortex dynamics field
  //@{

  ///  \brief  This function gets the axis of the vortex field
  ///  \return the axis
  const XVector3& GetAxis() const;

  ///  \brief  This function sets the axis of the vortex field
  ///  \param  val  -  the new axis
  ///  \return true if OK
  bool SetAxis(const XVector3& val);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  /// The axis around which i guess the vortex spins...
  XVector3 m_Axis;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::Vortex, XVortex, "vortex", "VORTEX_FIELD");
#endif
}
