//----------------------------------------------------------------------------------------------------------------------
/// \file   PoleVectorConstraint.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines a constraint that forces an object to be contrained to 
///         a specific point in space.
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/PointConstraint.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \brief  This class holds the data for a pole vector constraint
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XPoleVectorConstraint
  : public XPointConstraint
{
  XMD_CHUNK(XPoleVectorConstraint);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kPivotSpace = XPointConstraint::kLast+1; // XMatrix
  static const XU32 kLast       = kPivotSpace;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Pole Vector Constraint Params
  /// \brief  Parameters used by the pole vector constraint
  //@{

  /// \brief  returns the offset for the point constraint
  /// \return the offset value as a vector
  const XMatrix& GetPivotSpace() const;

  /// \brief  sets the offset for the point constraint
  /// \param v - the offset value as a vector
  void SetPivotSpace(const XMatrix& v);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  XMatrix m_PivotSpace;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::PoleVectorConstraint, XPoleVectorConstraint, "poleVectorConstraint", "POLE_VECTOR_CONSTRAINT");
#endif
}
