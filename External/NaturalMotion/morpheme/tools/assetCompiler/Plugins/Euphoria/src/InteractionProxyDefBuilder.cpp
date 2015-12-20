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
#include "InteractionProxyDefBuilder.h"
#include "export/mcExport.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
const char* InteractionProxyDefBuilder::ShapeTypeTokens[NumShapeTypes] =
{
  "Box",
  "Capsule",
  "Sphere"
};

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format InteractionProxyDefBuilder::getMemoryRequirements(
  AP::AssetProcessor*               NMP_UNUSED(processor),
  const ME::InteractionProxyExport* NMP_UNUSED(proxyExport))
{
  NMP::Memory::Format format(NMP::Memory::align(sizeof(InteractionProxyDef), NMP_NATURAL_TYPE_ALIGNMENT), NMP_NATURAL_TYPE_ALIGNMENT);
  return format;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Resource InteractionProxyDefBuilder::init(
  AP::AssetProcessor*               processor,
  const ME::InteractionProxyExport* proxyExport)
{
  NMP::Memory::Format format = getMemoryRequirements(processor, proxyExport);
  NMP::Memory::Resource result = NMPMemoryAllocateFromFormat(format);
  InteractionProxyDefBuilder* interactionProxy = (InteractionProxyDefBuilder*)result.ptr;

  // Shape attribute
  interactionProxy->m_shapeType = CapsuleShape;
  const char* shapeName = proxyExport->getShape();
  if (shapeName != 0)
  {
    std::string requiredShapeName(shapeName);
    for (uint32_t shapeType = 0; shapeType < NumShapeTypes; ++shapeType)
    {
      std::string availableShapeName(ShapeTypeTokens[shapeType]);
      if (requiredShapeName == availableShapeName)
      {
        interactionProxy->m_shapeType = ShapeType(shapeType);
        break;
      }
    }
  }

  // Height attribute
  interactionProxy->m_height = proxyExport->getHeight();
  interactionProxy->m_radius = proxyExport->getRadius();
  interactionProxy->m_width = proxyExport->getWidth();
  interactionProxy->m_depth = proxyExport->getDepth();

  // Parent index attribute
  interactionProxy->m_parentIndex = proxyExport->getParentIndex();

  // User data attribute
  interactionProxy->m_userData = proxyExport->getUserData();

  // Visible attribute
  interactionProxy->m_visible = proxyExport->getVisibility();

  // Rotation attribute
  proxyExport->getRotation(
    interactionProxy->m_rotation.x,
    interactionProxy->m_rotation.y,
    interactionProxy->m_rotation.z,
    interactionProxy->m_rotation.w);

  // Translation attribute
  proxyExport->getTranslation(
    interactionProxy->m_translation.x,
    interactionProxy->m_translation.y,
    interactionProxy->m_translation.z);

  return result;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
