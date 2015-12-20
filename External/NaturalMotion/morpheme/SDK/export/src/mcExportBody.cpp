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
#include <string.h>
#include <stdarg.h>
#include "export/mcExportBody.h"

#pragma warning(disable: 4127)

#define ARMTYPENAME "Arm"
#define HEADTYPENAME "Head"
#define SPINETYPENAME "Spine"
#define LEGTYPENAME "Leg"

//----------------------------------------------------------------------------------------------------------------------
namespace ME
{

//----------------------------------------------------------------------------------------------------------------------
/// HamstringExport
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
bool HamstringExport::getEnabled() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  bool value = false; 
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getBoolAttribute("enabled", value);
  }
  return value; 
}

//----------------------------------------------------------------------------------------------------------------------
float HamstringExport::getThreshold() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  double value = 0;
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getDoubleAttribute("threshold", value);
  }

  return (float)value;
}

//----------------------------------------------------------------------------------------------------------------------
float HamstringExport::getStiffness() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  double value = 0;
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getDoubleAttribute("stiffness", value);
  }

  return (float)value;
}

//----------------------------------------------------------------------------------------------------------------------
void HamstringExport::setThreshold(float threshold)
{
  AttributeBlockExport* blockExport = getAttributeBlock();

  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->setDoubleAttribute("threshold", threshold);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void HamstringExport::setStiffness(float strength)
{
  AttributeBlockExport* blockExport = getAttributeBlock();

  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->setDoubleAttribute("stiffness", strength);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void HamstringExport::setEnabled(bool enabled)
{
  AttributeBlockExport* blockExport = getAttributeBlock();

  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->setBoolAttribute("enabled", enabled);
  }
}


//----------------------------------------------------------------------------------------------------------------------

bool HamstringExport::getTwistBendScaleWeights(float* twistBendScales, uint32_t arraySize) const
{
  NMP_ASSERT(twistBendScales);
  if (twistBendScales)
  {
    const AttributeBlockExport* blockExport = getAttributeBlock();
    NMP_ASSERT(blockExport != 0);
    if (blockExport && blockExport->getFloatAttributeArray("twistBendScales", twistBendScales, arraySize))
    {
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------

void HamstringExport::setTwistBendScaleWeights(const float* twistBendScales, uint32_t arraySize)
{
  AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->setFloatAttributeArray("twistBendScales", twistBendScales, arraySize);
  }
}

//----------------------------------------------------------------------------------------------------------------------

bool HamstringExport::getSwing1BendScaleWeights(float* swing1BendScales, uint32_t arraySize) const
{
  NMP_ASSERT(swing1BendScales);
  if (swing1BendScales)
  {
    const AttributeBlockExport* blockExport = getAttributeBlock();
    NMP_ASSERT(blockExport != 0);
    if (blockExport && blockExport->getFloatAttributeArray("swing1BendScales", swing1BendScales, arraySize))
    {
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------

void HamstringExport::setSwing1BendScaleWeights(const float* swing1BendScales, uint32_t arraySize)
{
  AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->setFloatAttributeArray("swing1BendScales", swing1BendScales, arraySize);
  }
}

//----------------------------------------------------------------------------------------------------------------------

bool HamstringExport::getSwing2BendScaleWeights(float* swing2BendScales, uint32_t arraySize) const
{
  NMP_ASSERT(swing2BendScales);
  if (swing2BendScales)
  {
    const AttributeBlockExport* blockExport = getAttributeBlock();
    NMP_ASSERT(blockExport != 0);
    if (blockExport && blockExport->getFloatAttributeArray("swing2BendScales", swing2BendScales, arraySize))
    {
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------

void HamstringExport::setSwing2BendScaleWeights(const float* swing2BendScales, uint32_t arraySize)
{
  AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->setFloatAttributeArray("swing2BendScales", swing2BendScales, arraySize);
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// LimbDefExport
//----------------------------------------------------------------------------------------------------------------------
void LimbDefExport::setEndPartName(const char* partName)
{
  AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->setStringAttribute("endPartName", partName);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void LimbDefExport::setMidPartName(const char* partName)
{
  AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->setStringAttribute("midPartName", partName);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void LimbDefExport::setRootPartName(const char* partName)
{
  AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->setStringAttribute("rootPartName", partName);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void LimbDefExport::setBasePartName(const char* partName)
{
  AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->setStringAttribute("basePartName", partName);
  }
}

//----------------------------------------------------------------------------------------------------------------------
const char* LimbDefExport::getEndPartName() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  const char* value = "";
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getStringAttribute("endPartName", value);
  }

  return value;
}

//----------------------------------------------------------------------------------------------------------------------
const char* LimbDefExport::getRootPartName() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  const char* value = "";
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getStringAttribute("rootPartName", value);
  }

  return value;
}

//----------------------------------------------------------------------------------------------------------------------
const char*  LimbDefExport::getBasePartName() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  const char* value = "";
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getStringAttribute("basePartName", value);
  }

  return value;
}

//----------------------------------------------------------------------------------------------------------------------
const char*  LimbDefExport::getMidPartName() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  const char* value = "";
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getStringAttribute("midPartName", value);
  }

  return value;
}

//----------------------------------------------------------------------------------------------------------------------
const char* LimbDefExport::getType() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  const char* value = "";
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getStringAttribute("limbType", value);
  }

  return value;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbDefExport::setType(const char* lType)
{
  AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->setStringAttribute("limbType", lType);
  }
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t LimbDefExport::getNumExtraParts() const
{
  uint32_t numExtraParts = 0;
  const AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport != 0)
  {
    blockExport->getAttributeArraySize("extraParts", numExtraParts);
  }

  return numExtraParts;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t LimbDefExport::getExtraParts(const char* parts[], uint32_t numParts) const
{
  uint32_t numExtraParts = 0;
  const AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport != 0)
  {
    blockExport->getAttributeArraySize("extraParts", numExtraParts);
    NMP_ASSERT(numParts <= numExtraParts);

    blockExport->getStringAttributeArray("extraParts", parts, numParts);
  }

  return numExtraParts;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbDefExport::setReachConeAngle(const float reachAngle)
{
  AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->setDoubleAttribute("reachConeAngle", reachAngle);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void LimbDefExport::setReachDistance(const float reachDistance)
{
  AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->setDoubleAttribute("reachDistance", reachDistance);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Matrix34 LimbDefExport::getReachOffset() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  NMP::Matrix34 value(NMP::Matrix34::kIdentity);
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getMatrix34Attribute("reachOffset", value);
  }
  return value;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbDefExport::setReachOffset(const NMP::Matrix34& offset)
{
  AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    NMP_ASSERT(offset.isValidTM(0.001f));
    blockExport->setMatrix34Attribute("reachOffset", offset);
  }
}

//----------------------------------------------------------------------------------------------------------------------
float LimbDefExport::getReachConeAngle() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  double value = 0.0;
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getDoubleAttribute("reachConeAngle", value);
  }
  return (float) value;
}

//----------------------------------------------------------------------------------------------------------------------
float LimbDefExport::getReachDistance() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  double value = 0.0;
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getDoubleAttribute("reachDistance", value);
  }
  return (float) value;
}

//----------------------------------------------------------------------------------------------------------------------
const NMP::Matrix34 LimbDefExport::getEndOffset() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  NMP::Matrix34 value(NMP::Matrix34::kIdentity);
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getMatrix34Attribute("endOffset", value);
  }
  return value;
}

//----------------------------------------------------------------------------------------------------------------------
const NMP::Matrix34 LimbDefExport::getRootOffset() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  NMP::Matrix34 value(NMP::Matrix34::kIdentity);
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getMatrix34Attribute("rootOffset", value);
  }
  return value;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbDefExport::setEndOffset(const NMP::Matrix34& endOffset)
{
  AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    NMP_ASSERT(endOffset.isValidTM(0.001f));
    blockExport->setMatrix34Attribute("endOffset", endOffset);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void LimbDefExport::setRootOffset(const NMP::Matrix34& rootOffset)
{
  AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    NMP_ASSERT(rootOffset.isValidTM(0.001f));
    blockExport->setMatrix34Attribute("rootOffset", rootOffset);
  }
}

//----------------------------------------------------------------------------------------------------------------------
float LimbDefExport::getGuidePoseWeight() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  double value = 1.0f;
  if (blockExport)
  {
    blockExport->getDoubleAttribute("guidePoseWeight", value);
  }

  return (float)value;
}

//----------------------------------------------------------------------------------------------------------------------
float LimbDefExport::getNeutralPoseWeight() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  double value = 1.0f;
  if (blockExport)
  {
    blockExport->getDoubleAttribute("neutralPoseWeight", value);
  }

  return (float)value;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbDefExport::setGuidePoseWeight(float guidePoseWeight)
{
  AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->setDoubleAttribute("guidePoseWeight", guidePoseWeight);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void LimbDefExport::setNeutralPoseWeight(float neutralPoseWeight)
{
  AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->setDoubleAttribute("neutralPoseWeight", neutralPoseWeight);
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool LimbDefExport::getGuidePoseJoints(bool* joints, uint32_t arraySize) const
{
  NMP_ASSERT(joints);
  if (joints)
  {
    const AttributeBlockExport* blockExport = getAttributeBlock();
    NMP_ASSERT(blockExport != 0);
    if (blockExport && blockExport->getBoolAttributeArray("guidePoseJoints", joints, arraySize))
    {
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool LimbDefExport::getPositionWeights(float* positionWeights, uint32_t arraySize) const
{
  NMP_ASSERT(positionWeights);
  if (positionWeights)
  {
    const AttributeBlockExport* blockExport = getAttributeBlock();
    NMP_ASSERT(blockExport != 0);
    if (blockExport && blockExport->getFloatAttributeArray("positionWeights", positionWeights, arraySize))
    {
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool LimbDefExport::getOrientationWeights(float* orientationWeights, uint32_t arraySize) const
{
  NMP_ASSERT(orientationWeights);
  if (orientationWeights)
  {
    const AttributeBlockExport* blockExport = getAttributeBlock();
    NMP_ASSERT(blockExport != 0);
    if (blockExport && blockExport->getFloatAttributeArray("orientationWeights", orientationWeights, arraySize))
    {
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbDefExport::setGuidePoseJoints(const bool* joints, uint32_t count)
{
  AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->setBoolAttributeArray("guidePoseJoints", joints, count);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void LimbDefExport::setPositionWeights(const float* positionWeights, uint32_t weightCount)
{
  AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->setFloatAttributeArray("positionWeights", positionWeights, weightCount);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void LimbDefExport::setOrientationWeights(const float* orientationWeights, uint32_t weightCount)
{
  AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->setFloatAttributeArray("orientationWeights", orientationWeights, weightCount);
  }
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t getNumLimbsOfType(const char* typeName, const BodyDefExport* bodyDef)
{
  uint32_t limbCount = 0;
  for (size_t i = 0 ; i < bodyDef->getNumLimbs(); i++)
  {
    const LimbDefExport* limb = bodyDef->getLimb((uint32_t)i);
    const char* limbType = limb->getType();
    if (strcmp(limbType, typeName) == 0)
    {
      limbCount++;
    }
  }
  return limbCount;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t BodyDefExport::getNumArmLimbs() const
{
  return getNumLimbsOfType(ARMTYPENAME, this);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t BodyDefExport::getNumHeadLimbs() const
{
  return getNumLimbsOfType(HEADTYPENAME, this);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t BodyDefExport::getNumSpineLimbs() const
{
  return getNumLimbsOfType(SPINETYPENAME, this);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t BodyDefExport::getNumLegLimbs() const
{
  return getNumLimbsOfType(LEGTYPENAME, this);
}

//----------------------------------------------------------------------------------------------------------------------
const char* BodyDefExport::getRootLimbName() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  const char* value = "";
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getStringAttribute("rootLimbName", value);
  }

  return value;
}

//----------------------------------------------------------------------------------------------------------------------
void BodyDefExport::setRootLimbName(const char* rootLimbName)
{
  AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->setStringAttribute("rootLimbName", rootLimbName);
  }
}

//----------------------------------------------------------------------------------------------------------------------
GUID BodyDefExport::getPhysicsRigGUID() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  const char* value = "";
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getStringAttribute("physicsRig", value);
  }

  return value;
}

//----------------------------------------------------------------------------------------------------------------------
void BodyDefExport::setPhysicsRigGUID(GUID guid)
{
  AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->setStringAttribute("physicsRig", guid);
  }
}

//----------------------------------------------------------------------------------------------------------------------
GUID BodyDefExport::getRigGUID() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  const char* value = "";
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getStringAttribute("rig", value);
  }

  return value;
}

//----------------------------------------------------------------------------------------------------------------------
void BodyDefExport::setRigGUID(GUID guid)
{
  AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->setStringAttribute("rig", guid);
  }
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace ME
