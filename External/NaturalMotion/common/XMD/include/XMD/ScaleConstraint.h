//----------------------------------------------------------------------------------------------------------------------
/// \file   ScaleConstraint.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines a scale constraint
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Constraint.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XScaleConstraint
/// \brief This class holds the data for a constraint which only constrains scale. 
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XScaleConstraint
  : public XConstraint
{
  XMD_CHUNK(XScaleConstraint);
// node data get/set functions
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kOffset = XConstraint::kLast+1; // XVector3
  static const XU32 kLast   = kOffset;
  //--------------------------------------------------------------------------------------------------------------------


  /// \name   Scale Constraint Params
  /// \brief  Parameters used by the scale constraint
  //@{

  /// \brief  returns the offset for the scale constraint
  /// \return the offset value as a vector 
  const XVector3& GetOffset() const;

  /// \brief  sets the offset for the scale constraint
  /// \param v - the offset value as a vector
  void SetOffset(const XVector3& v);

  //@}

// internal io mechanisms
protected:
#ifndef DOXYGEN_PROCESSING
  XVector3 m_Offset;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::ScaleConstraint, XScaleConstraint, "scaleConstraint", "SCALE_CONSTRAINT");
#endif
}
