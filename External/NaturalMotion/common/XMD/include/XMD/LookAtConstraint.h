//----------------------------------------------------------------------------------------------------------------------
/// \file   LookAtConstraint.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines a look at constraint
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/AimConstraint.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XLookAtConstraint
/// \brief This class holds the data for a look at constraint
//---------------------------------------------------------------------------------------------------------------------- 
class XMD_EXPORT XLookAtConstraint
  : public XAimConstraint
{
  XMD_CHUNK(XLookAtConstraint);
// node data get/set functions
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kDistanceBetween = XAimConstraint::kLast+1; // XReal
  static const XU32 kTwist           = XAimConstraint::kLast+2; // XReal
  static const XU32 kLast            = kTwist;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Look-At Constraint Params
  /// \brief  Parameters used by the look-at constraint
  //@{

  /// \brief  returns the output distance between the object and the target (object space)
  /// \return the output distance between the object and the target (in object space)
  XReal GetDistanceBetween() const;

  /// \brief  returns the angle to rotate around the aim vector
  /// \return the rotation angle around the aim vector
  XReal GetTwist() const;

  /// \brief  sets the output distance between the object and the target (in object space)
  /// \param  v - the new output distance
  void SetDistanceBetween(XReal v);

  /// \brief  sets the up vector for the aim constraint
  /// \param  v - the new up vector 
  void SetTwist(XReal v);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  XReal m_DistanceBetween;
  XReal m_Twist;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::LookAtConstraint, XLookAtConstraint, "lookAt", "LOOK_AT_CONSTRAINT");
#endif
}
