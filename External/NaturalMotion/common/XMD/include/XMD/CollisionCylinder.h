//----------------------------------------------------------------------------------------------------------------------
/// \file   CollisionCylinder.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines a collision cylinder primitive
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/CollisionBase.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XCollisionCylinder
/// \brief This class holds the data for a collision cylinder primitive
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XCollisionCylinder :
  public XCollisionObject
{
  XMD_CHUNK(XCollisionCylinder);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kLength = XCollisionObject::kLast+1; // XReal
  static const XU32 kRadius = XCollisionObject::kLast+2; // XReal
  static const XU32 kLast   = kRadius;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Collision Cylinder Params
  /// \brief  Parameters used by the cylinder collision object
  //@{

  /// \brief  returns the radius of the collision cylinder
  /// \return the radius
  XReal GetRadius() const;

  /// \brief  returns the length of the collision cylinder
  /// \return the length
  XReal GetLength() const;

  /// \brief  sets the radius of the collision cylinder
  /// \param  radius - the radius
  void SetRadius(XReal radius);

  /// \brief  sets the length of the collision cylinder
  /// \param  length - the length
  void SetLength(XReal length);

  //@}

// internal node data
private:
#ifndef DOXYGEN_PROCESSING

  /// the cylinder radius
  XReal m_Radius;

  /// the cylinder length
  XReal m_Length;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::CollisionCylinder, XCollisionCylinder, "collisionCylinder", "COLLISION_CYLINDER");
#endif 
}
