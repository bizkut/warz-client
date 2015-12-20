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
#include "morpheme/mrManager.h"
#include "euphoria/erInteractionProxyDef.h"
//----------------------------------------------------------------------------------------------------------------------

namespace ER
{

//----------------------------------------------------------------------------------------------------------------------
bool InteractionProxyDef::locate()
{
  NMP::endianSwap(m_shapeType);
  NMP::endianSwap(m_height);
  NMP::endianSwap(m_width);
  NMP::endianSwap(m_radius);
  NMP::endianSwap(m_depth);
  NMP::endianSwap(m_parentIndex);
  NMP::endianSwap(m_rotation);
  NMP::endianSwap(m_translation);
  NMP::endianSwap(m_userData);
  NMP::endianSwap(m_visible);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool InteractionProxyDef::dislocate()
{
  NMP::endianSwap(m_shapeType);
  NMP::endianSwap(m_height);
  NMP::endianSwap(m_width);
  NMP::endianSwap(m_radius);
  NMP::endianSwap(m_depth);
  NMP::endianSwap(m_parentIndex);
  NMP::endianSwap(m_rotation);
  NMP::endianSwap(m_translation);
  NMP::endianSwap(m_userData);
  NMP::endianSwap(m_visible);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
InteractionProxyDef::ShapeType InteractionProxyDef::getShapeType() const
{
  return m_shapeType;
}

//----------------------------------------------------------------------------------------------------------------------
float InteractionProxyDef::getHeight() const
{
  return m_height;
}

//----------------------------------------------------------------------------------------------------------------------
float InteractionProxyDef::getRadius() const
{
  return m_radius;
}

//----------------------------------------------------------------------------------------------------------------------
float InteractionProxyDef::getWidth() const
{
  return m_width;
}

//----------------------------------------------------------------------------------------------------------------------
float InteractionProxyDef::getDepth() const
{
  return m_depth;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t InteractionProxyDef::getParentIndex() const
{
  return m_parentIndex;
}

//----------------------------------------------------------------------------------------------------------------------
const NMP::Quat& InteractionProxyDef::getRotation() const
{
  return m_rotation;
}

//----------------------------------------------------------------------------------------------------------------------
const NMP::Vector3& InteractionProxyDef::getTranslation() const
{
  return m_translation;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Matrix34 InteractionProxyDef::getTransform() const
{
  return NMP::Matrix34(m_rotation, m_translation);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t InteractionProxyDef::getUserData() const
{
  return m_userData;
}

//----------------------------------------------------------------------------------------------------------------------
bool InteractionProxyDef::getVisible() const
{
  return m_visible != 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool locateInteractionProxyDef(uint32_t NMP_USED_FOR_ASSERTS(assetType), void* assetMemory)
{
  NMP_ASSERT(assetType == MR::Manager::kAsset_InteractionProxyDef);
  ER::InteractionProxyDef* interactionProxyDef = (ER::InteractionProxyDef*)assetMemory;
  return interactionProxyDef->locate();
}

} // namespace ER

//----------------------------------------------------------------------------------------------------------------------
