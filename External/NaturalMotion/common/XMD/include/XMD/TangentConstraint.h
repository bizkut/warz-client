//----------------------------------------------------------------------------------------------------------------------
/// \file   TangentConstraint.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines a tangent constraint.
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Constraint.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XTangentConstraint
/// \brief This class holds the data for a tangent constraint
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XTangentConstraint
  : public XConstraint
{
  XMD_CHUNK(XTangentConstraint);
public:

  
  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kAimVector = XConstraint::kLast+1; // XVector3
  static const XU32 kUpVector  = XConstraint::kLast+2; // XVector3
  static const XU32 kLast      = kUpVector;
  //--------------------------------------------------------------------------------------------------------------------


  /// \name   Tangent Constraint Params
  /// \brief  Parameters used by the tangent constraint
  //@{

  /// \brief  returns the aim vector for the tangent constraint
  /// \return the aim vector
  const XVector3& GetAimVector() const;

  /// \brief  returns the up vector for the tangent constraint
  /// \return the up vector
  const XVector3& GetUpVector() const;

  /// \brief  sets the aim vector for the tangent constraint
  /// \param  v - the new aim vector 
  void SetAimVector(const XVector3& v);

  /// \brief  sets the up vector for the tangent constraint
  /// \param  v - the new up vector 
  void SetUpVector(const XVector3& v);

  //@}

// internal node data
private:
#ifndef DOXYGEN_PROCESSING
  XVector3 m_Aim;
  XVector3 m_Up;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::TangentConstraint, XTangentConstraint, "tangentConstraint", "TANGENT_CONSTRAINT");
#endif
}
