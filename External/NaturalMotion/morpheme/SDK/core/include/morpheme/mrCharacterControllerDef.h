// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef MR_CHARACTERCONTROLLERDEF_H
#define MR_CHARACTERCONTROLLERDEF_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMColour.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::CharacterControllerDef
/// \brief Holds data that describes the properties of a character controller.
/// \ingroup CoreModule
//----------------------------------------------------------------------------------------------------------------------
class CharacterControllerDef
{
public:

  /// \enum  MR::CharacterControllerDef::ShapeType
  enum ShapeType
  {
    BoxShape      = 0,  // "Box"
    CapsuleShape  = 1,  // "Capsule"
    CylinderShape = 2,  // "Cylinder" NB: not currently supported
    SphereShape   = 3,  // "Sphere" NB: not currently supported
    NumShapeTypes = 4
  };

  CharacterControllerDef();
  ~CharacterControllerDef();

  /// \brief Prepare a dislocated CharacterControllerDef for use.
  bool locate();

  /// \brief Dislocate a CharacterControllerDef ready to move to a new memory location.
  bool dislocate();

  /// \brief Return CharacterControllerDef shape type
  ShapeType   getShape() const;
  /// \brief Return the CharacterControllerDef height.  Note this will be valid if the shape type is a box or capsule
  float       getHeight() const;
  /// \brief Return the CharacterControllerDef width.  Note this will only be valid if the shape type is a box
  float       getWidth() const;
  /// \brief Return the CharacterControllerDef depth.  Note this will only be valid if the shape type is a box
  float       getDepth() const;
  /// \brief Return the CharacterControllerDef radius.  Note this will only be valid if the shape type is a capsule
  float       getRadius() const;

  float       getSkinWidth() const;
  float       getStepHeight() const;
  float       getMaxPushForce() const;
  float       getMaxSlopeAngle() const;
  NMP::Colour getColour() const;
  bool        getVisible() const;

  void setHeight(float height)         { m_height = height; }
  void setWidth(float width)           { m_width = width; }
  void setDepth(float depth)           { m_depth = depth; }
  void setRadius(float radius)         { m_radius = radius; }
  void setSkinWidth(float skinWidth)   { m_skinWidth = skinWidth; }
  void setStepHeight(float stepHeight) { m_stepHeight = stepHeight; }

protected:
  ShapeType   m_shape;
  float       m_height;
  float       m_width;
  float       m_depth;
  float       m_radius;
  float       m_skinWidth;
  float       m_stepHeight;
  float       m_maxPushForce;
  float       m_maxSlopeAngle;
  NMP::Colour m_colour;
  uint32_t    m_visible;
};

//----------------------------------------------------------------------------------------------------------------------
bool locateCharacterControllerDef(uint32_t assetDesc, void* assetMemory);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_CHARACTERCONTROLLERDEF_H
//----------------------------------------------------------------------------------------------------------------------
