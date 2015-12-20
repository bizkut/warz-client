//----------------------------------------------------------------------------------------------------------------------
/// \file   Gravity.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines a gravity field to affect particle systems
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Field.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XGravity
/// \brief A gravity field  
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XGravity :
  public XField
{
  XMD_CHUNK(XGravity);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kDirection = XField::kLast+1; // XVector3
  static const XU32 kLast      = kDirection;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Gravity Dynamics Field Attributes
  /// \brief  Sets/Gets the params for the XGravity dynamics field
  //@{

  /// \brief  Returns the direction of the gravity field effect
  /// \return The direction vector
  const XVector3& GetDirection() const;

  /// \brief  sets the direction of the gravity field effect
  /// \param  direction  - The direction vector
  void SetDirection(const XVector3& direction);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  /// vector for the direction of the gravity effect
  XVector3 m_Direction;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::Gravity, XGravity, "gravity", "GRAVITY_FIELD");
#endif
}
