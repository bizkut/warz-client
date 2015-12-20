//----------------------------------------------------------------------------------------------------------------------
/// \file   NormalConstraint.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines a contraint that forces an object along a surface normal
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Constraint.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \brief  This class holds the data for constraint where the constrained
///         object is aligned to the surface normal at a point on a 
///         geometry object
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XNormalConstraint
  : public XConstraint
{
  XMD_CHUNK(XNormalConstraint);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kAimVector = XConstraint::kLast+1; // XVector3
  static const XU32 kUpVector  = XConstraint::kLast+2; // XVector3
  static const XU32 kLast      = kUpVector;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Normal Constraint Params
  /// \brief  Parameters used by the normal constraint
  //@{

  /// \brief  returns the aim vector for the normal constraint
  /// \return the aim vector
  const XVector3& GetAimVector() const;

  /// \brief  returns the up vector for the normal constraint
  /// \return the up vector
  const XVector3& GetUpVector() const;

  /// \brief  sets the aim vector for the normal constraint
  /// \return v - the aim vector
  void SetAimVector(const XVector3& v);

  /// \brief  sets the up vector for the normal constraint
  /// \return v - the up vector
  void SetUpVector(const XVector3& v);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  XVector3 m_Aim;
  XVector3 m_Up;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::NormalConstraint, XNormalConstraint, "normalConstraint", "NORMAL_CONSTRAINT");
#endif
}
