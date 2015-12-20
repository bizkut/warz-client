//----------------------------------------------------------------------------------------------------------------------
/// \file   Uniform.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines a uniform dynamics field
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Field.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XUniform 
/// \brief A uniform force field  
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XUniform :
  public XField
{
  XMD_CHUNK(XUniform);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kDirection  = XField::kLast+1; // XVector3
  static const XU32 kLast       = kDirection;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Uniform Dynamics Field Attributes
  /// \brief  Sets/Gets the params for the XUniform dynamics field
  //@{

  /// \brief  returns the direction of the uniform force
  /// \return  The direction vector
  const XVector3& GetDirection() const;

  /// \brief  sets the direction of the uniform force
  /// \return  The direction vector
  void SetDirection(const XVector3& v);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  /// direction of the force
  XVector3 m_Direction;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::Uniform, XUniform, "uniformField", "UNIFORM_FIELD");
#endif
}
