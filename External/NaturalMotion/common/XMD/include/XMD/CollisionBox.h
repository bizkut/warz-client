//----------------------------------------------------------------------------------------------------------------------
/// \file   CollisionBox.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines a collision box primitive
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/CollisionBase.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XCollisionBox
/// \brief This class is used to hold a box collision primitive.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XCollisionBox :
  public XCollisionObject
{
  XMD_CHUNK(XCollisionBox);
// node data get/set functions
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kSizeX = XCollisionObject::kLast+1; // XReal
  static const XU32 kSizeY = XCollisionObject::kLast+2; // XReal
  static const XU32 kSizeZ = XCollisionObject::kLast+3; // XReal
  static const XU32 kLast  = kSizeZ;
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Collision Box Params
  /// \brief  Parameters used by the box collision object
  //@{

  /// \brief  returns the size of the x size of the box
  /// \return the x size
  XReal GetX() const;

  /// \brief  returns the size of the y size of the box
  /// \return  the y size
  XReal GetY() const;

  /// \brief  returns the size of the z size of the box
  /// \return the z size
  XReal GetZ() const;

  /// \brief  sets the x size of the box
  /// \param  v - the new value
  void SetX(XReal v);

  /// \brief  sets the y size of the box
  /// \param  v - the new value
  void SetY(XReal v);

  /// \brief  sets the z size of the box
  /// \param  v - the new value
  void SetZ(XReal v);

  //@}

// internal node data
private:
#ifndef DOXYGEN_PROCESSING
  XReal x;
  XReal y;
  XReal z;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::CollisionBox, XCollisionBox, "collisionBox", "COLLISION_BOX");
#endif
}
