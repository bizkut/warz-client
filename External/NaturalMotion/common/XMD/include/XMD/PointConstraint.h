//----------------------------------------------------------------------------------------------------------------------
/// \file   PointConstraint.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines a constraint that forces an object to be contrained to 
///         a specific point in space.
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Constraint.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \brief  This class holds the data for a point constraint
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XPointConstraint
  : public XConstraint
{
  XMD_CHUNK(XPointConstraint);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kOffset = XConstraint::kLast+1; // XVector3
  static const XU32 kLast   = kOffset;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Point Constraint Params
  /// \brief  Parameters used by the point constraint
  //@{

  /// \brief  returns the offset for the point constraint
  /// \return the offset value as a vector
  const XVector3& GetOffset() const;

  /// \brief  sets the offset for the point constraint
  /// \param v - the offset value as a vector
  void SetOffset(const XVector3& v);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  XVector3 m_Offset;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::PointConstraint, XPointConstraint, "pointConstraint", "POINT_CONSTRAINT");
#endif
}
