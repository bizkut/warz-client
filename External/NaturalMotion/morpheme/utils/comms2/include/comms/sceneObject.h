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
#ifndef MC_SCENE_OBJECT_H
#define MC_SCENE_OBJECT_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMMatrix34.h"
#include "NMPlatform/NMColour.h"
#include "attribute.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MCOMMS
{

typedef uint32_t SceneObjectID;

//----------------------------------------------------------------------------------------------------------------------
/// An object is nothing more than a collection of attributes.
/// No assumption is made about the attribute types/semantics
/// that a given object must have.
/// IDs are assigned by the class when the SceneObjects are created.
//----------------------------------------------------------------------------------------------------------------------
class NMP_ALIGN_PREFIX(16) SceneObject
{
public:
  /// \brief Calculate the number of bytes required to create this object
  static uint32_t getMemorySize(uint32_t numAttributes);

  /// \brief Create a SceneObject in the memory provided.  Attributes in the list
  /// replicated (although the data they point to is not).
  static SceneObject* init(void* mem, SceneObjectID objectID, uint32_t numAttributes);

  /// \brief Destroy all internal objects
  void destroy();

  SceneObjectID getSceneObjectID() const;

  uint32_t getNumAttributes() const;

  const Attribute* getAttribute(uint32_t index) const;
  Attribute* getAttribute(uint32_t index);

  /// \brief Set an attribute on this object, from another attribute.
  /// The data referenced by the attribute is not copied.
  void setAttribute(uint32_t attrIdx, const Attribute* srcAttribute);

  const Attribute* getAttribute(Attribute::Semantic sematic) const;
  Attribute* getAttribute(Attribute::Semantic sematic);

  /// \brief Helper function that allocates and creates an object and copies
  /// all attributes in.  Naturally, the data referenced by the attributes is
  /// not touched.
  static SceneObject* allocAndCreate(SceneObjectID objectID, uint32_t numAttributes);

  /// \brief De-allocate objects created via allocAndCreate().
  void destroyAndFree();

  bool isFirstUpdate() const { return m_firstUpdate != 0; }
  void setUpdated() { m_firstUpdate = 0; }

protected:
  SceneObjectID   m_objectID;
  uint32_t        m_numAttributes;
  uint32_t        m_firstUpdate;
  uint32_t        m_pad1;

  friend class RuntimeTarget;
} NMP_ALIGN_SUFFIX(16);

//----------------------------------------------------------------------------------------------------------------------
// SceneObject inline functions.
//----------------------------------------------------------------------------------------------------------------------
inline SceneObjectID SceneObject::getSceneObjectID() const
{
  return m_objectID;
}

//----------------------------------------------------------------------------------------------------------------------
inline uint32_t SceneObject::getNumAttributes() const
{
  return m_numAttributes;
}

//----------------------------------------------------------------------------------------------------------------------
inline const Attribute* SceneObject::getAttribute(uint32_t index) const
{
  NMP_ASSERT_MSG(index < m_numAttributes, "Invalid attribute index.");
  const Attribute* attributes = (const Attribute*)(this + 1);
  return &attributes[index];
}

//----------------------------------------------------------------------------------------------------------------------
inline Attribute* SceneObject::getAttribute(uint32_t index)
{
  NMP_ASSERT_MSG(index < m_numAttributes, "Invalid attribute index.");
  Attribute* attributes = (Attribute*)(this + 1);
  return &attributes[index];
}

//----------------------------------------------------------------------------------------------------------------------
inline void SceneObject::setAttribute(uint32_t attrIdx, const Attribute* attribute)
{
  Attribute* destAttr = getAttribute(attrIdx);
  *destAttr = *attribute;
}

//----------------------------------------------------------------------------------------------------------------------
inline Attribute* SceneObject::getAttribute(Attribute::Semantic sematic)
{
  for (uint32_t i = 0 ; i < m_numAttributes ; ++i)
  {
    Attribute* attr = getAttribute(i);
    if (attr->getSemantic() == sematic)
    {
      return attr;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
inline const Attribute* SceneObject::getAttribute(Attribute::Semantic sematic) const
{
  return const_cast<SceneObject*>(this)->getAttribute(sematic);
}

} // MCCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MC_SCENE_OBJECT_H
//----------------------------------------------------------------------------------------------------------------------
