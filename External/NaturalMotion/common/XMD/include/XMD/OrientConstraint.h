//----------------------------------------------------------------------------------------------------------------------
/// \file   OrientConstraint.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines a contraint to orient an object with another transform
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Constraint.h"  
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \brief  This class holds the data for an orient based constraint, 
///         where the constrained object matches its rotation to another
///         transform.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XOrientConstraint
  : public XConstraint
{
  XMD_CHUNK(XOrientConstraint);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kOffset = XConstraint::kLast+1; // XVector3
  static const XU32 kLast   = kOffset;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Orient Constraint Params
  /// \brief  Parameters used by the orient constraint
  //@{

  /// \brief  returns the offset for the orient constraint
  /// \return the offset value as a vector
  const XVector3& GetOffset() const;

  /// \brief  sets the offset for the orient constraint
  /// \param v - the offset value as a vector
  void SetOffset(const XVector3& v);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  XVector3 m_Offset;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::OrientConstraint, XOrientConstraint, "orientConstraint", "ORIENT_CONSTRAINT");
#endif
}
