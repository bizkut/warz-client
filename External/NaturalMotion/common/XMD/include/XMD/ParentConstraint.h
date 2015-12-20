//----------------------------------------------------------------------------------------------------------------------
/// \file   ParentConstraint.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines a constraint that makes an object behave as if it is parented
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Constraint.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \brief  This class holds the data for a parent constraint. ie, the 
///         constrained object acts as though it is parented to a second
///         transform
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XParentConstraint
  : public XConstraint
{
  XMD_CHUNK(XParentConstraint);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kLast   = XConstraint::kLast;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Parent Constraint Params
  /// \brief  Parameters used by the parent constraint
  //@{
  //@}
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::ParentConstraint, XParentConstraint, "parentConstraint", "PARENT_CONSTRAINT");
#endif
}
