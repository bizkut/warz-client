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
#include "CharacterControllerDefBuilder.h"
#include "export/mcExport.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
const char* CharacterControllerDefBuilder::ShapeTypeTokens[NumShapeTypes] =
{
  "Box",
  "Capsule",
  "Cylinder",
  "Sphere"
};

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format CharacterControllerDefBuilder::getMemoryRequirements(
  AP::AssetProcessor*                  NMP_UNUSED(processor),
  const ME::CharacterControllerExport* NMP_UNUSED(characterControllerExport))
{
  NMP::Memory::Format format(NMP::Memory::align(sizeof(CharacterControllerDef), NMP_NATURAL_TYPE_ALIGNMENT), NMP_NATURAL_TYPE_ALIGNMENT);
  return format;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Resource CharacterControllerDefBuilder::init(
  AP::AssetProcessor*                  processor,
  const ME::CharacterControllerExport* characterControllerExport)
{
  NMP::Memory::Format format = getMemoryRequirements(processor, characterControllerExport);
  NMP::Memory::Resource result = NMPMemoryAllocateFromFormat(format);
  CharacterControllerDefBuilder* characterController = (CharacterControllerDefBuilder*)result.ptr;

  const ME::AttributeBlockExport* attributes = characterControllerExport->getAttributeBlock();
  if (attributes)
  {
    // Shape attribute
    characterController->m_shape = CapsuleShape;
    const char* shapeName = 0;
    if (attributes->getStringAttribute("Shape", shapeName) && shapeName != 0)
    {
      std::string requiredShapeName(shapeName);
      for (uint32_t shapeType = 0; shapeType < NumShapeTypes; ++shapeType)
      {
        std::string availableShapeName(ShapeTypeTokens[shapeType]);
        if (requiredShapeName == availableShapeName)
        {
          characterController->m_shape = ShapeType(shapeType);
          break;
        }
      }
    }

    // Height attribute
    double height = 0.0;
    attributes->getDoubleAttribute("Height", height);
    characterController->m_height = float(height);

    // Radius attribute (only applicable for capsules)
    if (characterController->m_shape == CapsuleShape)
    {
      double radius = 0.0;
      attributes->getDoubleAttribute("Radius", radius);
      characterController->m_radius = float(radius);
    }
    else
    {
      characterController->m_radius = 0.0f;
    }

    // Width and depth attributes (only applicable for boxes)
    if (characterController->m_shape == BoxShape)
    {
      double width = 0.0;
      attributes->getDoubleAttribute("Width", width);
      characterController->m_width = float(width);

      double depth = 0.0;
      attributes->getDoubleAttribute("Depth", depth);
      characterController->m_depth = float(depth);
    }
    else
    {
      characterController->m_width = 0.0f;
      characterController->m_depth = 0.0f;
    }

    // SkinWidth attribute
    double skinWidth = 0.0;
    attributes->getDoubleAttribute("SkinWidth", skinWidth);
    characterController->m_skinWidth = float(skinWidth);

    // StepHeight attribute
    double stepHeight = 0.0;
    attributes->getDoubleAttribute("StepHeight", stepHeight);
    characterController->m_stepHeight = float(stepHeight);

    // maxPushForce attribute
    double maxPushForce = 0.0;
    attributes->getDoubleAttribute("MaxPushForce", maxPushForce);
    characterController->m_maxPushForce = float(maxPushForce);

    // maxSlopeAngle attribute
    double maxSlopeAngle = 0.0;
    attributes->getDoubleAttribute("MaxSlopeAngle", maxSlopeAngle);
    characterController->m_maxSlopeAngle = NMP::radiansToDegrees(float(maxSlopeAngle));

    // Colour attribute
    NMP::Vector3 colour = NMP::Vector3Zero();
    attributes->getVector3Attribute("Colour", colour);
    uint8_t r = uint8_t(colour.x * 255.0f);
    uint8_t g = uint8_t(colour.y * 255.0f);
    uint8_t b = uint8_t(colour.z * 255.0f);
    uint8_t a = 128;
    characterController->m_colour = NMP::Colour(r, g, b, a);

    // Visible attribute
    bool visible = false;
    attributes->getBoolAttribute("Visible", visible);
    characterController->m_visible = visible ? uint32_t(1) : uint32_t(0);
  }

  return result;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
