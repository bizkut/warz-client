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
#include "export/mcExportPhysics.h"

#pragma warning(disable: 4127)

//----------------------------------------------------------------------------------------------------------------------
namespace ME
{

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsShapeExport
//----------------------------------------------------------------------------------------------------------------------
NMP::Matrix34 PhysicsShapeExport::getTransform() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  NMP::Matrix34 transform(NMP::Matrix34::kIdentity);
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getMatrix34Attribute("transform", transform);
  }
  return transform;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsShapeExport::setFriction(float friction)
{
  AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->setDoubleAttribute("friction", friction);
  }
}

//----------------------------------------------------------------------------------------------------------------------
float PhysicsShapeExport::getFriction() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  double value = 0.0;
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getDoubleAttribute("friction", value);
  }
  return static_cast<float>(value);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsShapeExport::setRestitution(float restitution)
{
  AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->setDoubleAttribute("restitution", restitution);
  }
}

//----------------------------------------------------------------------------------------------------------------------
float PhysicsShapeExport::getRestitution() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  double value = 0.0;
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getDoubleAttribute("restitution", value);
  }
  return static_cast<float>(value);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsShapeExport::setDensity(float density)
{
  AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->setDoubleAttribute("density", density);
    blockExport->setDoubleAttribute("mass", -1.0f);
  }
}

//----------------------------------------------------------------------------------------------------------------------
float PhysicsShapeExport::getDensity() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  double value = -1.0;
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getDoubleAttribute("density", value);
  }
  return static_cast<float>(value);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsShapeExport::setParentIndex(int index)
{
  AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->setIntAttribute("parentIndex", index);
  }
}

//----------------------------------------------------------------------------------------------------------------------
int PhysicsShapeExport::getParentIndex() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  int index = -1;
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getIntAttribute("parentIndex", index);
  }
  return index;
}

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsBoxExport
//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 PhysicsBoxExport::getDimensions() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  NMP::Vector3 dimensions;
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getVector3Attribute("dimensions", dimensions);
  }
  return dimensions;
}

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsCapsuleExport
//----------------------------------------------------------------------------------------------------------------------
float PhysicsCapsuleExport::getRadius() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  double radius = 0.0;
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getDoubleAttribute("radius", radius);
  }
  return static_cast<float>(radius);
}

//----------------------------------------------------------------------------------------------------------------------
float PhysicsCapsuleExport::getLength() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  double height = 0.0;
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getDoubleAttribute("length", height);
  }
  return static_cast<float>(height);
}

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsCylinderExport
//----------------------------------------------------------------------------------------------------------------------
float PhysicsCylinderExport::getRadius() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  double radius = 0.0;
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getDoubleAttribute("radius", radius);
  }
  return static_cast<float>(radius);
}

//----------------------------------------------------------------------------------------------------------------------
float PhysicsCylinderExport::getLength() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  double height = 0.0;
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getDoubleAttribute("length", height);
  }
  return static_cast<float>(height);
}

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsMeshExport
//----------------------------------------------------------------------------------------------------------------------
uint32_t PhysicsMeshExport::getNumVertices() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  unsigned int numVertices = 0;
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getAttributeArraySize("vertices", numVertices);
  }
  return static_cast<uint32_t>(numVertices);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsMeshExport::getVertices(NMP::Vector3 vertices[], uint32_t size) const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getVector3AttributeArray("vertices", vertices, size);
  }
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t PhysicsMeshExport::getNumIndices() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  unsigned int numIndices = 0;
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getAttributeArraySize("indices", numIndices);
  }
  return static_cast<uint32_t>(numIndices);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsMeshExport::getIndices(uint32_t indicies[], uint32_t size) const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getUIntAttributeArray("indices", indicies, size);
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsSphereExport
//----------------------------------------------------------------------------------------------------------------------
float PhysicsSphereExport::getRadius() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  double radius = 0.0;
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getDoubleAttribute("radius", radius);
  }
  return static_cast<float>(radius);
}

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsPartExport
//----------------------------------------------------------------------------------------------------------------------
void PhysicsPartExport::setTransform(const NMP::Matrix34& transform)
{
  AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    NMP_ASSERT(transform.isValidTM(0.001f));
    blockExport->setMatrix34Attribute("transform", transform);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsPartExport::setHasCollision(bool collision)
{
  AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->setBoolAttribute("hasCollision", collision);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsPartExport::setLinearDamping(float linearDamping)
{
  AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->setDoubleAttribute("linearDamping", linearDamping);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsPartExport::setAngularDamping(float angularDamping)
{
  AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->setDoubleAttribute("angularDamping", angularDamping);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsPartExport::setMaxContactOffsetIncrease(float maxContactOffsetIncrease)
{
  AttributeBlockExport* blockExport = getAttributeBlock();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->setDoubleAttribute("maxContactOffsetIncrease", maxContactOffsetIncrease);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Matrix34 PhysicsPartExport::getTransform() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  NMP::Matrix34 value;
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getMatrix34Attribute("transform", value);
  }
  return value;
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsPartExport::hasCollision() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  bool value = false;
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getBoolAttribute("hasCollision", value);
  }
  return value;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 PhysicsPartExport::getCenterOfMass() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  NMP::Vector3 value = NMP::Vector3Zero();
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getVector3Attribute("centerOfMass", value);
  }
  return value;
}

//----------------------------------------------------------------------------------------------------------------------
float PhysicsPartExport::getLinearDamping() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  double value = 0.0;
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getDoubleAttribute("linearDamping", value);
  }
  return static_cast<float>(value);
}

//----------------------------------------------------------------------------------------------------------------------
float PhysicsPartExport::getAngularDamping() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  double value = 0.0;
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getDoubleAttribute("angularDamping", value);
  }
  return static_cast<float>(value);
}

//----------------------------------------------------------------------------------------------------------------------
float PhysicsPartExport::getMaxContactOffsetIncrease() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  double value = 0.0;
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getDoubleAttribute("maxContactOffsetIncrease", value);
  }
  return static_cast<float>(value);
}

//----------------------------------------------------------------------------------------------------------------------
const char* PhysicsPartExport::getBodyPlanTag() const
{
  const AttributeBlockExport* blockExport = getAttributeBlock();
  const char* value = 0;
  NMP_ASSERT(blockExport != 0);
  if (blockExport)
  {
    blockExport->getStringAttribute("BodyPlanTag", value);
  }
  return value;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace ME
