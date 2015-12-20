//----------------------------------------------------------------------------------------------------------------------
/// \file   CollisionCapsule.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines a collision capsule type
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/CollisionBase.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XCollisionCapsule
/// \brief This class defines a collision capsule type
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XCollisionCapsule :
  public XCollisionObject
{
  XMD_CHUNK(XCollisionCapsule);
// node data get/set functions
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kLength = XCollisionObject::kLast+1; // XReal
  static const XU32 kRadius = XCollisionObject::kLast+2; // XReal
  static const XU32 kLast   = kRadius;
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Collision Capsule Params
  /// \brief  Parameters used by the capsule collision object
  //@{

  /// \brief  returns the radius of the collision capsule
  /// \return  the radius
  XReal GetRadius() const;

  /// \brief  returns the length of the collision capsule
  /// \return  the length
  XReal GetLength() const;

  /// \brief  sets the radius of the collision capsule
  /// \param  radius - the radius
  void SetRadius(XReal radius);

  /// \brief  sets the length of the collision capsule
  /// \param  length - the length
  void SetLength(XReal length);

  //@}

// internal node data
private:
#ifndef DOXYGEN_PROCESSING

  /// the spheres radius
  XReal m_Radius;

  /// the spheres radius
  XReal m_Length;

#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::CollisionCapsule, XCollisionCapsule, "collisionCapsule", "COLLISION_CAPSULE");
#endif
}
