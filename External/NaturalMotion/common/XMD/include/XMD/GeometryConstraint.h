//----------------------------------------------------------------------------------------------------------------------
/// \file   GeometryConstraint.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines a jiggle deformation
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Constraint.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XGeometryConstraint
/// \brief This class holds the data for a constraint where the 
///        constrained object follows the surface of a second piece of geometry
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XGeometryConstraint
  : public XConstraint
{
  XMD_CHUNK(XGeometryConstraint);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kLast      = XConstraint::kLast;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Geometry Constraint Params
  /// \brief  Parameters used by the geometry constraint
  //@{
  //@}
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::GeometryConstraint, XGeometryConstraint, "geometryConstraint", "GEOMETRY_CONSTRAINT");
#endif
}
