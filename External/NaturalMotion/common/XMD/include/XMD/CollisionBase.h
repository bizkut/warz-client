//----------------------------------------------------------------------------------------------------------------------
/// \file   CollisionBase.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines the base class of collision primitives
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Shape.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XCollisionObject
/// \brief This class form the base class of collision primitives. 
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XCollisionObject :
  public XShape
{
  XMD_ABSTRACT_CHUNK(XCollisionObject);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kGravityScale = XShape::kLast+1; // XReal
  static const XU32 kMass         = XShape::kLast+2; // XReal
  static const XU32 kLast         = kMass;
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Common Collision Object Params
  //@{

  /// \brief  returns the gravity scale for the collision object
  /// \return  the gravity scale 
  XReal GetGravityScale() const;

  /// \brief  returns the mass of the object
  /// \return  the mass
  XReal GetMass() const;

  /// \brief  sets the gravity scale of the body
  /// \param  v - the new value
  void SetGravityScale(XReal v);

  /// \brief  sets the mass of the object
  /// \param  v - the new value
  void SetMass(XReal v);

  //@}

// internal node data
private:
#ifndef DOXYGEN_PROCESSING
  /// the mass
  XReal m_Mass;
  /// the gravity scale for the joint
  XReal m_GravityScale;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_API_TYPE(XFn::CollisionObject, XCollisionObject, "collisionObject");
#endif
}
