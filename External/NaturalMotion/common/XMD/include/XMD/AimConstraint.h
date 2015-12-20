//----------------------------------------------------------------------------------------------------------------------
/// \file   AimConstraint.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines an aim constraint
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Constraint.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XAimConstraint
/// \brief This class holds the data for a constraint where the 
///        constrained object is always aimed at the second transform.
//---------------------------------------------------------------------------------------------------------------------- 
class XMD_EXPORT XAimConstraint
  : public XConstraint
{
  XMD_CHUNK(XAimConstraint);
// node data get/set functions
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kOffset    = XConstraint::kLast+1; // XVector3
  static const XU32 kAimVector = XConstraint::kLast+2; // XVector3
  static const XU32 kUpVector  = XConstraint::kLast+3; // XVector3
  static const XU32 kLast      = kUpVector;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Aim Constraint Params
  /// \brief  Parameters used by the aim constraint
  //@{

  /// \brief  returns the aim vector for the aim constraint
  /// \return the aim vector
  const XVector3& GetAimVector() const;

  /// \brief  returns the up vector for the aim constraint
  /// \return the up vector
  const XVector3& GetUpVector() const;

  /// \brief  returns the offset for the aim constraint
  /// \return the offset as a vector
  const XVector3& GetOffset() const;

  /// \brief  sets the aim vector for the aim constraint
  /// \param v - the new aim vector 
  void SetAimVector(const XVector3& v);

  /// \brief  sets the up vector for the aim constraint
  /// \param v - the new up vector 
  void SetUpVector(const XVector3& v);

  /// \brief  sets the offset vector for the aim constraint
  /// \param v - the new offset vector 
  void SetOffset(const XVector3& v);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  XVector3 m_Aim;
  XVector3 m_Up;
  XVector3 m_Offset;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::AimConstraint,XAimConstraint,"aimConstraint","AIM_CONSTRAINT");
#endif
}
