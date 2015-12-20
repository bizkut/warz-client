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
#include "morpheme/mrManager.h"
#include "morpheme/mrCharacterControllerDef.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
CharacterControllerDef::CharacterControllerDef()
{

}

//----------------------------------------------------------------------------------------------------------------------
CharacterControllerDef::~CharacterControllerDef()
{

}

//----------------------------------------------------------------------------------------------------------------------
bool CharacterControllerDef::locate()
{
  NMP::endianSwap(m_shape);
  NMP::endianSwap(m_height);
  NMP::endianSwap(m_radius);
  NMP::endianSwap(m_skinWidth);
  NMP::endianSwap(m_stepHeight);
  NMP::endianSwap(m_maxPushForce);
  NMP::endianSwap(m_maxSlopeAngle);
  NMP::endianSwap(m_colour);
  NMP::endianSwap(m_visible);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool CharacterControllerDef::dislocate()
{
  NMP::endianSwap(m_shape);
  NMP::endianSwap(m_height);
  NMP::endianSwap(m_radius);
  NMP::endianSwap(m_skinWidth);
  NMP::endianSwap(m_stepHeight);
  NMP::endianSwap(m_maxPushForce);
  NMP::endianSwap(m_maxSlopeAngle);
  NMP::endianSwap(m_colour);
  NMP::endianSwap(m_visible);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
CharacterControllerDef::ShapeType CharacterControllerDef::getShape() const
{
  return m_shape;
}

//----------------------------------------------------------------------------------------------------------------------
float CharacterControllerDef::getHeight() const
{
  return m_height;
}

//----------------------------------------------------------------------------------------------------------------------
float CharacterControllerDef::getRadius() const
{
  return m_radius;
}

//----------------------------------------------------------------------------------------------------------------------
float CharacterControllerDef::getWidth() const
{
  return m_width;
}

//----------------------------------------------------------------------------------------------------------------------
float CharacterControllerDef::getDepth() const
{
  return m_depth;
}

//----------------------------------------------------------------------------------------------------------------------
float CharacterControllerDef::getSkinWidth() const
{
  return m_skinWidth;
}

//----------------------------------------------------------------------------------------------------------------------
float CharacterControllerDef::getStepHeight() const
{
  return m_stepHeight;
}

//----------------------------------------------------------------------------------------------------------------------
float CharacterControllerDef::getMaxPushForce() const
{
  return m_maxPushForce;
}

//----------------------------------------------------------------------------------------------------------------------
float CharacterControllerDef::getMaxSlopeAngle() const
{
  return m_maxSlopeAngle;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Colour CharacterControllerDef::getColour() const
{
  return m_colour;
}

//----------------------------------------------------------------------------------------------------------------------
bool CharacterControllerDef::getVisible() const
{
  return m_visible != 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool locateCharacterControllerDef(uint32_t NMP_USED_FOR_ASSERTS(assetType), void* assetMemory)
{
  NMP_ASSERT(assetType == MR::Manager::kAsset_CharacterControllerDef);
  MR::CharacterControllerDef* characterControllerDef = (MR::CharacterControllerDef*)assetMemory;
  return characterControllerDef->locate();
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
