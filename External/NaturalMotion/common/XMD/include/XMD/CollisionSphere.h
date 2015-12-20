//----------------------------------------------------------------------------------------------------------------------
/// \file   CollisionSphere.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines a collision sphere primitive
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/CollisionBase.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XCollisionSphere
/// \brief This class holds the data for a collision sphere. 
//---------------------------------------------------------------------------------------------------------------------- 
class XMD_EXPORT XCollisionSphere :
  public XCollisionObject
{
  XMD_CHUNK(XCollisionSphere);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kRadius = XCollisionObject::kLast+1; // XReal
  static const XU32 kLast   = kRadius;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Collision Sphere Params
  /// \brief  Parameters used by the sphere collision object
  //@{

  /// \brief  returns the radius of the collision sphere
  /// \return the radius
  XReal GetRadius() const;

  /// \brief  sets the radius of the collision sphere
  /// \param  radius - the radius 
  void SetRadius(XReal radius);

  //@}

// internal node data
private:
#ifndef DOXYGEN_PROCESSING

  /// the spheres radius
  XReal m_Radius;

#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::CollisionSphere, XCollisionSphere, "collisionSphere", "COLLISION_SPHERE");
#endif
}
