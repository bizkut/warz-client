//----------------------------------------------------------------------------------------------------------------------
/// \file   CollisionCone.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines a collision cone primitive
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/CollisionBase.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XCollisionCone
/// \brief This class holds the data required for a cone shaped collision primitive.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XCollisionCone :
  public XCollisionObject
{
  XMD_CHUNK(XCollisionCone);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kAngle = XCollisionObject::kLast+1; // XReal
  static const XU32 kCap   = XCollisionObject::kLast+2; // XReal
  static const XU32 kLast  = kCap;
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Collision Cone Params
  /// \brief  Parameters used by the cone collision object
  //@{

  /// \brief  Returns the cone angle
  /// \return the angle of the cone
  XReal GetAngle() const;

  /// \brief  returns the cap length of the cone
  /// \return the cap distance
  XReal GetCap() const;

  /// \brief  sets the cone angle
  /// \param  angle - the angle of the cone
  void SetAngle(XReal angle);

  /// \brief  sets the cap length of the cone
  /// \param  cap - the cap distance
  void SetCap(XReal cap);

  //@}

// internal node data
private:
#ifndef DOXYGEN_PROCESSING

  /// the angle of the cone
  XReal m_Angle;

  /// the distance to the cone caps
  XReal m_Cap;

#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::CollisionCone, XCollisionCone, "collisionCone", "COLLISION_CONE");
#endif
}
