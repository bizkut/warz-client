// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
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
#ifndef ER_INTERACTIONPROXYDEF_H
#define ER_INTERACTIONPROXYDEF_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
//----------------------------------------------------------------------------------------------------------------------

namespace ER
{

//----------------------------------------------------------------------------------------------------------------------
/// \class ER::InteractionProxyDef
/// \brief Holds properties of the Sample Interaction Proxy.
/// \ingroup EuphoriaModule
//----------------------------------------------------------------------------------------------------------------------
class InteractionProxyDef
{
public:

  /// \enum  ER::InteractionProxyDef::ShapeType
  /// \brief
  /// \ingroup
  enum ShapeType
  {
    BoxShape      = 0,  // "Box"
    CapsuleShape  = 1,  // "Capsule"
    SphereShape   = 2,  // "Sphere"
    NumShapeTypes = 3
  };

  InteractionProxyDef() {}
  ~InteractionProxyDef() {}

  /// \brief Prepare a dislocated InteractionProxyDef for use.
  bool locate();

  /// \brief Dislocate a InteractionProxyDef ready to move to a new memory location.
  bool dislocate();

  /// \brief Return InteractionProxyDef shape type
  ShapeType    getShapeType() const;
  /// \brief Return the InteractionProxyDef height.  Note this will be valid if the shape type is a box or capsule
  float        getHeight() const;
  /// \brief Return the InteractionProxyDef width.  Note this will only be valid if the shape type is a box
  float        getWidth() const;
  /// \brief Return the InteractionProxyDef depth.  Note this will only be valid if the shape type is a box
  float        getDepth() const;
  /// \brief Return the InteractionProxyDef radius.  Note this will only be valid if the shape type is a capsule
  float        getRadius() const;
  /// \brief Return the InteractionProxyDef parent index.
  uint32_t     getParentIndex() const;
  /// \brief Return the InteractionProxyDef rotation.
  const NMP::Quat&   getRotation() const;
  /// \brief Return the InteractionProxyDef translation.
  const NMP::Vector3& getTranslation() const;
  /// \brief Returns the translation and rotation
  NMP::Matrix34 getTransform() const;
  /// \brief Return the InteractionProxyDef user data.
  uint32_t     getUserData() const;
  /// \brief Return the InteractionProxyDef visibility.
  bool         getVisible() const;

  void setHeight(float height)         { m_height = height; }
  void setWidth(float width)           { m_width = width; }
  void setDepth(float depth)           { m_depth = depth; }
  void setRadius(float radius)         { m_radius = radius; }
  void setParentIndex(uint32_t parent) { m_parentIndex = parent; }
  void setUserData(uint32_t data)      { m_userData = data; }

protected:
  ShapeType     m_shapeType;
  float         m_height;
  float         m_width;
  float         m_depth;
  float         m_radius;
  uint32_t      m_parentIndex;
  uint32_t      m_userData;
  uint32_t      m_visible;
  NMP::Quat     m_rotation;
  NMP::Vector3  m_translation;
};

//----------------------------------------------------------------------------------------------------------------------
bool locateInteractionProxyDef(uint32_t assetDesc, void* assetMemory);

} // namespace ER

//----------------------------------------------------------------------------------------------------------------------
#endif // ER_INTERACTIONPROXYDEF_H
//----------------------------------------------------------------------------------------------------------------------
