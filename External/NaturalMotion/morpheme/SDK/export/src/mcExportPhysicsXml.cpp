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
#include "export/mcExportPhysicsXml.h"

#pragma warning(disable: 4127)

//----------------------------------------------------------------------------------------------------------------------
namespace ME
{

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsBoxExportXML
//----------------------------------------------------------------------------------------------------------------------
PhysicsBoxExportXML::~PhysicsBoxExportXML()
{
  delete m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
const AttributeBlockExport* PhysicsBoxExportXML::getAttributeBlock() const
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeBlockExport* PhysicsBoxExportXML::getAttributeBlock()
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsBoxExportXML::PhysicsBoxExportXML(XMLElement* element) : m_xmlElement(element), m_xmlAttributeBlock(0)
{
  XMLElement* attributeBlockElement = m_xmlElement->findChild("Attributes");
  NMP_ASSERT(attributeBlockElement != 0);
  if (attributeBlockElement != 0)
  {
    m_xmlAttributeBlock = new AttributeBlockXML(attributeBlockElement);
  }
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsBoxExportXML::PhysicsBoxExportXML(const NMP::Matrix34& transform, const NMP::Vector3& dimensions) :
  m_xmlElement(new XMLElement("Box"))
{
  m_xmlAttributeBlock = new AttributeBlockXML();
  m_xmlElement->LinkEndChild(m_xmlAttributeBlock->getXMLElement());

  m_xmlAttributeBlock->createMatrix34Attribute("transform", transform);
  m_xmlAttributeBlock->createVector3Attribute("dimensions", dimensions);
}

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsCapsuleExportXML
//----------------------------------------------------------------------------------------------------------------------
PhysicsCapsuleExportXML::~PhysicsCapsuleExportXML()
{
  delete m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
const AttributeBlockExport* PhysicsCapsuleExportXML::getAttributeBlock() const
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeBlockExport* PhysicsCapsuleExportXML::getAttributeBlock()
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsCapsuleExportXML::PhysicsCapsuleExportXML(XMLElement* element) : m_xmlElement(element), m_xmlAttributeBlock(0)
{
  XMLElement* attributeBlockElement = m_xmlElement->findChild("Attributes");
  NMP_ASSERT(attributeBlockElement != 0);
  if (attributeBlockElement != 0)
  {
    m_xmlAttributeBlock = new AttributeBlockXML(attributeBlockElement);
  }
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsCapsuleExportXML::PhysicsCapsuleExportXML(const NMP::Matrix34& transform, float radius, float height) :
  m_xmlElement(new XMLElement("Capsule"))
{
  m_xmlAttributeBlock = new AttributeBlockXML();
  m_xmlElement->LinkEndChild(m_xmlAttributeBlock->getXMLElement());

  m_xmlAttributeBlock->createMatrix34Attribute("transform", transform);
  m_xmlAttributeBlock->createDoubleAttribute("radius", radius);
  m_xmlAttributeBlock->createDoubleAttribute("length", height);
}

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsCylinderExportXML
//----------------------------------------------------------------------------------------------------------------------
PhysicsCylinderExportXML::~PhysicsCylinderExportXML()
{
  delete m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
const AttributeBlockExport* PhysicsCylinderExportXML::getAttributeBlock() const
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeBlockExport* PhysicsCylinderExportXML::getAttributeBlock()
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsCylinderExportXML::PhysicsCylinderExportXML(XMLElement* element) : m_xmlElement(element), m_xmlAttributeBlock(0)
{
  XMLElement* attributeBlockElement = m_xmlElement->findChild("Attributes");
  NMP_ASSERT(attributeBlockElement != 0);
  if (attributeBlockElement != 0)
  {
    m_xmlAttributeBlock = new AttributeBlockXML(attributeBlockElement);
  }
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsCylinderExportXML::PhysicsCylinderExportXML(const NMP::Matrix34& transform, float radius, float height) :
  m_xmlElement(new XMLElement("Cylinder"))
{
  m_xmlAttributeBlock = new AttributeBlockXML();
  m_xmlElement->LinkEndChild(m_xmlAttributeBlock->getXMLElement());

  m_xmlAttributeBlock->createMatrix34Attribute("transform", transform);
  m_xmlAttributeBlock->createDoubleAttribute("radius", radius);
  m_xmlAttributeBlock->createDoubleAttribute("length", height);
}

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsPlaneExportXML
//----------------------------------------------------------------------------------------------------------------------
PhysicsPlaneExportXML::~PhysicsPlaneExportXML()
{
  delete m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
const AttributeBlockExport* PhysicsPlaneExportXML::getAttributeBlock() const
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeBlockExport* PhysicsPlaneExportXML::getAttributeBlock()
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsPlaneExportXML::PhysicsPlaneExportXML(XMLElement* element) : m_xmlElement(element), m_xmlAttributeBlock(0)
{
  XMLElement* attributeBlockElement = m_xmlElement->findChild("Attributes");
  NMP_ASSERT(attributeBlockElement != 0);
  if (attributeBlockElement != 0)
  {
    m_xmlAttributeBlock = new AttributeBlockXML(attributeBlockElement);
  }
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsPlaneExportXML::PhysicsPlaneExportXML(const NMP::Matrix34& transform) :
  m_xmlElement(new XMLElement("Plane"))
{
  m_xmlAttributeBlock = new AttributeBlockXML();
  m_xmlElement->LinkEndChild(m_xmlAttributeBlock->getXMLElement());

  m_xmlAttributeBlock->createMatrix34Attribute("transform", transform);
}

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsSphereExportXML
//----------------------------------------------------------------------------------------------------------------------
PhysicsSphereExportXML::~PhysicsSphereExportXML()
{
  delete m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
const AttributeBlockExport* PhysicsSphereExportXML::getAttributeBlock() const
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeBlockExport* PhysicsSphereExportXML::getAttributeBlock()
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsSphereExportXML::PhysicsSphereExportXML(XMLElement* element) : m_xmlElement(element), m_xmlAttributeBlock(0)
{
  XMLElement* attributeBlockElement = m_xmlElement->findChild("Attributes");
  NMP_ASSERT(attributeBlockElement != 0);
  if (attributeBlockElement != 0)
  {
    m_xmlAttributeBlock = new AttributeBlockXML(attributeBlockElement);
  }
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsSphereExportXML::PhysicsSphereExportXML(const NMP::Matrix34& transform, float radius) :
  m_xmlElement(new XMLElement("Sphere"))
{
  m_xmlAttributeBlock = new AttributeBlockXML();
  m_xmlElement->LinkEndChild(m_xmlAttributeBlock->getXMLElement());

  m_xmlAttributeBlock->createMatrix34Attribute("transform", transform);
  m_xmlAttributeBlock->createDoubleAttribute("radius", radius);
}

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsMeshExportXML
//----------------------------------------------------------------------------------------------------------------------
PhysicsMeshExportXML::~PhysicsMeshExportXML()
{
  delete m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
const AttributeBlockExport* PhysicsMeshExportXML::getAttributeBlock() const
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeBlockExport* PhysicsMeshExportXML::getAttributeBlock()
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsMeshExportXML::PhysicsMeshExportXML(XMLElement* element) : m_xmlElement(element), m_xmlAttributeBlock(0)
{
  XMLElement* attributeBlockElement = m_xmlElement->findChild("Attributes");
  NMP_ASSERT(attributeBlockElement != 0);
  if (attributeBlockElement != 0)
  {
    m_xmlAttributeBlock = new AttributeBlockXML(attributeBlockElement);
  }
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsMeshExportXML::PhysicsMeshExportXML(
  const NMP::Matrix34& transform,
  uint32_t             numVertices,
  const NMP::Vector3   vertices[],
  uint32_t             numIndices,
  const uint32_t       indices[]) :
  m_xmlElement(new XMLElement("Mesh"))
{
  m_xmlAttributeBlock = new AttributeBlockXML();
  m_xmlElement->LinkEndChild(m_xmlAttributeBlock->getXMLElement());

  m_xmlAttributeBlock->createMatrix34Attribute("transform", transform);
  m_xmlAttributeBlock->createVector3AttributeArray("vertices", vertices, numVertices);
  m_xmlAttributeBlock->createUIntAttributeArray("indices", indices, numIndices);
}

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsVolumeExportXML
//----------------------------------------------------------------------------------------------------------------------
PhysicsVolumeExportXML::~PhysicsVolumeExportXML()
{
  size_t count = m_shapes.size();
  for (size_t i = 0; i != count; ++i)
  {
    delete m_shapes[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
const char* PhysicsVolumeExportXML::getName() const
{
  return m_xmlElement->getAttribute("name");
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsBoxExport* PhysicsVolumeExportXML::createBox(
  const NMP::Matrix34& transform,
  const NMP::Vector3&  dimensions)
{
  PhysicsBoxExportXML* shape = new PhysicsBoxExportXML(transform, dimensions);
  m_xmlElement->LinkEndChild(shape->m_xmlElement);
  shape->setParentIndex((int)m_shapes.size());
  m_shapes.push_back(shape);
  return shape;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsCapsuleExport* PhysicsVolumeExportXML::createCapsule(
  const NMP::Matrix34& transform,
  float                radius,
  float                height)
{
  PhysicsCapsuleExportXML* shape = new PhysicsCapsuleExportXML(transform, radius, height);
  m_xmlElement->LinkEndChild(shape->m_xmlElement);
  shape->setParentIndex((int)m_shapes.size());
  m_shapes.push_back(shape);
  return shape;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsCylinderExport* PhysicsVolumeExportXML::createCylinder(
  const NMP::Matrix34& transform,
  float                radius,
  float                height)
{
  PhysicsCylinderExportXML* shape = new PhysicsCylinderExportXML(transform, radius, height);
  m_xmlElement->LinkEndChild(shape->m_xmlElement);
  shape->setParentIndex((int)m_shapes.size());
  m_shapes.push_back(shape);
  return shape;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsPlaneExport* PhysicsVolumeExportXML::createPlane(const NMP::Matrix34& transform)
{
  PhysicsPlaneExportXML* shape = new PhysicsPlaneExportXML(transform);
  m_xmlElement->LinkEndChild(shape->m_xmlElement);
  shape->setParentIndex((int)m_shapes.size());
  m_shapes.push_back(shape);
  return shape;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsSphereExport* PhysicsVolumeExportXML::createSphere(const NMP::Matrix34& transform, float radius)
{
  PhysicsSphereExportXML* shape = new PhysicsSphereExportXML(transform, radius);
  m_xmlElement->LinkEndChild(shape->m_xmlElement);
  shape->setParentIndex((int)m_shapes.size());
  m_shapes.push_back(shape);
  return shape;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsMeshExport* PhysicsVolumeExportXML::createMesh(
  const NMP::Matrix34& transform,
  uint32_t             numVertices,
  const NMP::Vector3*  vertices,
  uint32_t             numIndices,
  const uint32_t*      indices)
{
  PhysicsMeshExportXML* shape = new PhysicsMeshExportXML(transform, numVertices, vertices, numIndices, indices);
  m_xmlElement->LinkEndChild(shape->m_xmlElement);
  shape->setParentIndex((int)m_shapes.size());
  m_shapes.push_back(shape);
  return shape;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t PhysicsVolumeExportXML::getNumShapes() const
{
  return (uint32_t)m_shapes.size();
}

//----------------------------------------------------------------------------------------------------------------------
const PhysicsShapeExport* PhysicsVolumeExportXML::getShape(uint32_t index) const
{
  return m_shapes[index];
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t PhysicsVolumeExportXML::getNumShapes(PhysicsShapeExport::ShapeType type) const
{
  size_t size = 0;
  size_t count = m_shapes.size();
  for (size_t i = 0; i != count; ++i)
  {
    if (m_shapes[i]->getShapeType() == type)
    {
      ++size;
    }
  }
  return (uint32_t)size;
}

//----------------------------------------------------------------------------------------------------------------------
const PhysicsShapeExport* PhysicsVolumeExportXML::getShape(PhysicsShapeExport::ShapeType type, uint32_t index) const
{
  size_t current = 0;
  size_t count = m_shapes.size();
  for (size_t i = 0; i != count; ++i)
  {
    if (m_shapes[i]->getShapeType() == type)
    {
      if (index == current)
      {
        return m_shapes[i];
      }
      ++current;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsVolumeExportXML::PhysicsVolumeExportXML(XMLElement* element) : m_xmlElement(element)
{
  for (NM::TiXmlNode* curChild = m_xmlElement->FirstChild(); curChild != 0; curChild = curChild->NextSibling())
  {
    XMLElement* element = static_cast<XMLElement*>(curChild->ToElement());
    if (element)
    {
      if (strcmp(element->Value(), "Box") == 0)
      {
        PhysicsBoxExportXML* shape = new PhysicsBoxExportXML(element);
        m_shapes.push_back(shape);
      }
      else if (strcmp(element->Value(), "Capsule") == 0)
      {
        PhysicsCapsuleExportXML* shape = new PhysicsCapsuleExportXML(element);
        m_shapes.push_back(shape);
      }
      else if (strcmp(element->Value(), "Cylinder") == 0)
      {
        PhysicsCylinderExportXML* shape = new PhysicsCylinderExportXML(element);
        m_shapes.push_back(shape);
      }
      else if (strcmp(element->Value(), "Plane") == 0)
      {
        PhysicsPlaneExportXML* shape = new PhysicsPlaneExportXML(element);
        m_shapes.push_back(shape);
      }
      else if (strcmp(element->Value(), "Sphere") == 0)
      {
        PhysicsSphereExportXML* shape = new PhysicsSphereExportXML(element);
        m_shapes.push_back(shape);
      }
      else if (strcmp(element->Value(), "Mesh") == 0)
      {
        PhysicsMeshExportXML* shape = new PhysicsMeshExportXML(element);
        m_shapes.push_back(shape);
      }
      else
      {
        NMP_ASSERT_MSG(false, "Unknown child element type of Volume");
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsVolumeExportXML::PhysicsVolumeExportXML(const char* name) : m_xmlElement(new XMLElement("Volume"))
{
  m_xmlElement->setAttribute("name", name);
}

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsPartExportXML
//----------------------------------------------------------------------------------------------------------------------
PhysicsPartExportXML::~PhysicsPartExportXML()
{
  delete m_volume;
  delete m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
const char* PhysicsPartExportXML::getName() const
{
  return m_xmlElement->getAttribute("name");
}

//----------------------------------------------------------------------------------------------------------------------
const PhysicsVolumeExport* PhysicsPartExportXML::getVolume() const
{
  return m_volume;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsVolumeExport* PhysicsPartExportXML::getVolume()
{
  return m_volume;
}

//----------------------------------------------------------------------------------------------------------------------
const AttributeBlockExport* PhysicsPartExportXML::getAttributeBlock() const
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeBlockExport* PhysicsPartExportXML::getAttributeBlock()
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsPartExportXML::PhysicsPartExportXML(XMLElement* element) : m_xmlElement(element)
{
  XMLElement* volumeElement = m_xmlElement->findChild("Volume");
  NMP_ASSERT(volumeElement != 0);
  if (volumeElement != 0)
  {
    m_volume = new PhysicsVolumeExportXML(volumeElement);
  }

  XMLElement* attributeBlockElement = m_xmlElement->findChild("Attributes");
  NMP_ASSERT(attributeBlockElement != 0);
  if (attributeBlockElement != 0)
  {
    m_xmlAttributeBlock = new AttributeBlockXML(attributeBlockElement);
  }
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsPartExportXML::PhysicsPartExportXML(const char* name) : m_xmlElement(new XMLElement("Part"))
{
  m_xmlElement->setAttribute("name", name);
  m_volume = new PhysicsVolumeExportXML("volume");
  m_xmlElement->LinkEndChild(m_volume->m_xmlElement);
  m_xmlAttributeBlock = new AttributeBlockXML();
  m_xmlElement->LinkEndChild(m_xmlAttributeBlock->getXMLElement());

  m_xmlAttributeBlock->createMatrix34Attribute("transform", NMP::Matrix34Identity());
  m_xmlAttributeBlock->createBoolAttribute("hasCollision", true);
}

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsJointExportXML
//----------------------------------------------------------------------------------------------------------------------
const char* PhysicsJointExportXML::getName() const
{
  return m_xmlElement->getAttribute("name");
}

//----------------------------------------------------------------------------------------------------------------------
const char* PhysicsJointExportXML::getJointType() const
{
  return m_xmlElement->getAttribute("type");
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t PhysicsJointExportXML::getParentIndex() const
{
  XMLElement* element = m_xmlElement->findChild("Parent");
  NMP_ASSERT(element != 0);
  if (element)
  {
    element = element->findChild("PartRef");

    unsigned int index = 0;
    element->getUIntAttribute("index", index);

    return (uint32_t)index;
  }
  return (uint32_t) - 1;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t PhysicsJointExportXML::getChildIndex() const
{
  XMLElement* element = m_xmlElement->findChild("Child");
  NMP_ASSERT(element != 0);
  if (element)
  {
    element = element->findChild("PartRef");

    unsigned int index = 0;
    element->getUIntAttribute("index", index);

    return (uint32_t)index;
  }
  return (uint32_t) - 1;
}

//----------------------------------------------------------------------------------------------------------------------
const AttributeBlockExport* PhysicsJointExportXML::getAttributeBlock() const
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeBlockExport* PhysicsJointExportXML::getAttributeBlock()
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsJointExportXML::PhysicsJointExportXML(XMLElement* element, PhysicsRigDefExportXML* rig) :
  m_xmlAttributeBlock(0),
  m_xmlElement(element),
  m_rig(rig)
{
  XMLElement* attributeBlockElement = m_xmlElement->findChild("Attributes");
  NMP_ASSERT(attributeBlockElement != 0);
  if (attributeBlockElement != 0)
  {
    m_xmlAttributeBlock = new AttributeBlockXML(attributeBlockElement);
  }
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsJointExportXML::PhysicsJointExportXML(
  const char*             name,
  const char*             type,
  uint32_t                parentIndex,
  uint32_t                childIndex,
  PhysicsRigDefExportXML* rig) :
  m_xmlElement(new XMLElement("Joint")),
  m_rig(rig)
{
  m_xmlElement->setAttribute("name", name);
  m_xmlElement->setAttribute("type", type);

  const PhysicsPartExport* parent = m_rig->getPart(parentIndex);
  XMLElement* element = m_xmlElement->addChild("Parent");
  element = element->addChild("PartRef");
  if (parent)
  {
    element->setAttribute("name", parent->getName());
    element->setUIntAttribute("index", parentIndex);
  }
  else
  {
    element->setAttribute("name", "null");
    element->setUIntAttribute("index", (unsigned int) - 1);
  }

  const PhysicsPartExport* child = m_rig->getPart(childIndex);
  element = m_xmlElement->addChild("Child");
  element = element->addChild("PartRef");
  if (child)
  {
    element->setAttribute("name", child->getName());
    element->setUIntAttribute("index", childIndex);
  }
  else
  {
    element->setAttribute("name", "null");
    element->setUIntAttribute("index", (unsigned int) - 1);
  }

  m_xmlAttributeBlock = new AttributeBlockXML();
  m_xmlElement->LinkEndChild(m_xmlAttributeBlock->getXMLElement());
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsJointExportXML::~PhysicsJointExportXML()
{
  delete m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsDisabledCollisionPairExportXML
//----------------------------------------------------------------------------------------------------------------------
uint32_t PhysicsDisabledCollisionPairExportXML::getPartAIndex() const
{
  XMLElement* element = m_xmlElement->findChild("PartA");
  NMP_ASSERT(element != 0);
  if (element)
  {
    element = element->findChild("PartRef");

    unsigned int index = 0;
    element->getUIntAttribute("index", index);
    return (uint32_t)index;
  }
  return (uint32_t) - 1;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t PhysicsDisabledCollisionPairExportXML::getPartBIndex() const
{
  XMLElement* element = m_xmlElement->findChild("PartB");
  NMP_ASSERT(element != 0);
  if (element)
  {
    element = element->findChild("PartRef");

    unsigned int index = 0;
    element->getUIntAttribute("index", index);
    return (uint32_t)index;
  }
  return (uint32_t) - 1;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsDisabledCollisionPairExportXML::PhysicsDisabledCollisionPairExportXML(
  XMLElement*             element,
  PhysicsRigDefExportXML* rig) :
  m_xmlElement(element),
  m_rig(rig)
{

}

//----------------------------------------------------------------------------------------------------------------------
PhysicsDisabledCollisionPairExportXML::PhysicsDisabledCollisionPairExportXML(
  uint32_t                partAIndex,
  uint32_t                partBIndex,
  PhysicsRigDefExportXML* rig) :
  m_xmlElement(new XMLElement("DisabledCollisionPair")),
  m_rig(rig)
{
  XMLElement* element = m_xmlElement->addChild("PartA");
  element = element->addChild("PartRef");

  const PhysicsPartExport* partA = m_rig->getPart(partAIndex);
  if (partA)
  {
    element->setAttribute("name", partA->getName());
    element->setUIntAttribute("index", partAIndex);
  }
  else
  {
    element->setAttribute("name", "null");
    element->setUIntAttribute("index", (unsigned int) - 1);
  }

  element = m_xmlElement->addChild("PartB");
  element = element->addChild("PartRef");

  const PhysicsPartExport* partB = m_rig->getPart(partBIndex);
  if (partB)
  {
    element->setAttribute("name", partB->getName());
    element->setUIntAttribute("index", partBIndex);
  }
  else
  {
    element->setAttribute("name", "null");
    element->setUIntAttribute("index", (unsigned int) - 1);
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsDisabledCollisionSetExportXML
//----------------------------------------------------------------------------------------------------------------------
void PhysicsDisabledCollisionSetExportXML::addPart(uint32_t partIndex)
{
  unsigned int size = 0;
  XMLElement* set = m_xmlElement->findChild("Set");
  NMP_ASSERT(set != 0);
  if (set)
  {
    set->getUIntAttribute("size", size);
    size += 1;
    set->setUIntAttribute("size", size);

    XMLElement* element = set->addChild("PartRef");
    const PhysicsPartExport* part = m_rig->getPart(partIndex);
    if (part)
    {
      element->setAttribute("name", part->getName());
      element->setUIntAttribute("index", partIndex);
    }
    else
    {
      element->setAttribute("name", "null");
      element->setUIntAttribute("index", (unsigned int) - 1);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsDisabledCollisionSetExportXML::setEnabled(bool enabled)
{
  m_xmlElement->setUIntAttribute("Enabled", enabled ? 1 : 0);
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsDisabledCollisionSetExportXML::getEnabled() const
{
  unsigned int enabled = 1;
  m_xmlElement->getUIntAttribute("Enabled", enabled);
  return enabled != 0;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t PhysicsDisabledCollisionSetExportXML::getNumParts() const
{
  unsigned int size = 0;

  XMLElement* set = m_xmlElement->findChild("Set");
  NMP_ASSERT(set != 0);
  if (set)
  {
    set->getUIntAttribute("size", size);
  }
  return (uint32_t)size;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t PhysicsDisabledCollisionSetExportXML::getPartIndex(uint32_t index) const
{
  XMLElement* set = m_xmlElement->findChild("Set");
  NMP_ASSERT(set != 0);
  if (set)
  {
    XMLElement* part = set->findChild("PartRef", index);
    if (part)
    {
      unsigned int index = 0;
      part->getUIntAttribute("index", index);
      return (uint32_t)index;
    }
  }
  return (uint32_t) - 1;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsDisabledCollisionSetExportXML::PhysicsDisabledCollisionSetExportXML(
  XMLElement*             element,
  PhysicsRigDefExportXML* rig) :
  m_xmlElement(element),
  m_rig(rig)
{

}

//----------------------------------------------------------------------------------------------------------------------
PhysicsDisabledCollisionSetExportXML::PhysicsDisabledCollisionSetExportXML(PhysicsRigDefExportXML* rig) :
  m_xmlElement(new XMLElement("DisabledCollisionSet")),
  m_rig(rig)
{
  m_xmlElement->addChild("Set");
}

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsRigDefExportXML
//----------------------------------------------------------------------------------------------------------------------
PhysicsRigDefExportXML::~PhysicsRigDefExportXML()
{
  delete m_xmlElement;
  delete m_xmlAttributeBlock;

  size_t count = m_parts.size();
  for (size_t i = 0; i != count; ++i)
  {
    delete m_parts[i];
  }
  count = m_joints.size();
  for (size_t i = 0; i != count; ++i)
  {
    delete m_joints[i];
  }
  count = m_disabledCollisions.size();
  for (size_t i = 0; i != count; ++i)
  {
    delete m_disabledCollisions[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
const char* PhysicsRigDefExportXML::getName() const
{
  return m_xmlElement->getAttribute("name");
}

//----------------------------------------------------------------------------------------------------------------------
GUID PhysicsRigDefExportXML::getGUID() const
{
  return m_xmlElement->getGUID();
}

//----------------------------------------------------------------------------------------------------------------------
const char* PhysicsRigDefExportXML::getDestFilename() const
{
  return m_destFileName.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsRigDefExportXML::write()
{
  return m_xmlElement->saveFile(m_destFileName.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigDefExportXML::setDestFilename(const char* filename)
{
  m_destFileName = filename;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsPartExport* PhysicsRigDefExportXML::createPart(const char* name)
{
  PhysicsPartExportXML* part = new PhysicsPartExportXML(name);
  m_xmlElement->LinkEndChild(part->m_xmlElement);
  m_parts.push_back(part);
  return part;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsJointExport* PhysicsRigDefExportXML::createJoint(
  const char* name,
  const char* type,
  uint32_t    parentIndex,
  uint32_t    childIndex)
{
  PhysicsJointExportXML* joint = new PhysicsJointExportXML(name, type, parentIndex, childIndex, this);
  m_xmlElement->LinkEndChild(joint->m_xmlElement);
  m_joints.push_back(joint);
  return joint;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsDisabledCollisionPairExport* PhysicsRigDefExportXML::createDisabledCollisionPair(
  uint32_t partAIndex,
  uint32_t partBIndex)
{
  PhysicsDisabledCollisionPairExportXML* pair = new PhysicsDisabledCollisionPairExportXML(partAIndex, partBIndex, this);
  m_xmlElement->LinkEndChild(pair->m_xmlElement);
  m_disabledCollisions.push_back(pair);
  return pair;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsDisabledCollisionSetExport* PhysicsRigDefExportXML::createDisabledCollisionSet()
{
  PhysicsDisabledCollisionSetExportXML* set = new PhysicsDisabledCollisionSetExportXML(this);
  m_xmlElement->LinkEndChild(set->m_xmlElement);
  m_disabledCollisions.push_back(set);
  return set;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t PhysicsRigDefExportXML::getNumParts() const
{
  return (uint32_t)m_parts.size();
}

//----------------------------------------------------------------------------------------------------------------------
const PhysicsPartExport* PhysicsRigDefExportXML::getPart(uint32_t index) const
{
  if (index < (uint32_t)m_parts.size())
  {
    return m_parts[index];
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const uint32_t PhysicsRigDefExportXML::getPartIndex(const PhysicsPartExport* part) const
{
  size_t count = m_parts.size();
  for (size_t i = 0; i != count; ++i)
  {
    if (m_parts[i] == part)
    {
      return (uint32_t)i;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t PhysicsRigDefExportXML::getNumJoints() const
{
  return (uint32_t)m_joints.size();
}

//----------------------------------------------------------------------------------------------------------------------
const PhysicsJointExport* PhysicsRigDefExportXML::getJoint(uint32_t index) const
{
  return m_joints[index];
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t PhysicsRigDefExportXML::getNumDisabledCollisions() const
{
  return (uint32_t)m_disabledCollisions.size();
}

//----------------------------------------------------------------------------------------------------------------------
const PhysicsDisabledCollisionExport* PhysicsRigDefExportXML::getDisabledCollision(uint32_t index) const
{
  return m_disabledCollisions[index];
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsRigDefExportXML::setPhysicsRootPart(
  uint32_t            index,
  const NMP::Vector3& translation,
  const NMP::Quat&    rotation)
{
  NMP_ASSERT(m_xmlAttributeBlock != 0);
  if (m_xmlAttributeBlock != 0)
  {
    uint32_t numParts = getNumParts();
    NMP_ASSERT(index < numParts);
    if (index < numParts)
    {
      m_xmlAttributeBlock->createUIntAttribute("RootPartIndex", index);
      m_xmlAttributeBlock->createVector3Attribute("RootPartTranslationOffset", translation);
      m_xmlAttributeBlock->createQuatAttribute("RootPartRotationOffset", rotation);
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsRigDefExportXML::getPhysicsRootPart(
  uint32_t&     index,
  NMP::Vector3& translation,
  NMP::Quat&    rotation) const
{
  NMP_ASSERT(m_xmlAttributeBlock != 0);
  if (m_xmlAttributeBlock != 0)
  {
    bool result = true;
    result &= m_xmlAttributeBlock->getUIntAttribute("RootPartIndex", index);
    result &= m_xmlAttributeBlock->getVector3Attribute("RootPartTranslationOffset", translation);
    result &= m_xmlAttributeBlock->getQuatAttribute("RootPartRotationOffset", rotation);
    return result;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsRigDefExportXML::addTrajectoryCalcMarkupPart(uint32_t index)
{
  if (m_trajectoryCalcMarkupArray != 0)
  {
    if (index < getNumParts())
    {
      return m_trajectoryCalcMarkupArray->appendToUIntArray(index);
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t PhysicsRigDefExportXML::getNumTrajectoryCalcMarkupParts() const
{
  if (m_trajectoryCalcMarkupArray != 0)
  {
    return (uint32_t)m_trajectoryCalcMarkupArray->getSize();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsRigDefExportXML::getTrajectoryCalcMarkupPartArray(uint32_t values[], uint32_t size) const
{
  if (m_trajectoryCalcMarkupArray != 0)
  {
    return m_trajectoryCalcMarkupArray->getAsUIntArray(values, size);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
const AttributeBlockExport* PhysicsRigDefExportXML::getAttributeBlock() const
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeBlockExport* PhysicsRigDefExportXML::getAttributeBlock()
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigDefExportXML::PhysicsRigDefExportXML(XMLElement* element, const char* destFileName) :
  m_xmlElement(element),
  m_destFileName(destFileName),
  m_xmlAttributeBlock(0),
  m_trajectoryCalcMarkupArray(0)
{
  for (NM::TiXmlNode* curChild = m_xmlElement->FirstChild(); curChild != 0; curChild = curChild->NextSibling())
  {
    XMLElement* element = static_cast<XMLElement*>(curChild->ToElement());
    if (element)
    {
      if (strcmp(element->Value(), "Part") == 0)
      {
        PhysicsPartExportXML* part = new PhysicsPartExportXML(element);
        m_parts.push_back(part);
      }
      else if (strcmp(element->Value(), "Joint") == 0)
      {
        PhysicsJointExportXML* joint = new PhysicsJointExportXML(element, this);
        m_joints.push_back(joint);
      }
      else if (strcmp(element->Value(), "DisabledCollisionPair") == 0)
      {
        PhysicsDisabledCollisionPairExportXML* pair = new PhysicsDisabledCollisionPairExportXML(element, this);
        m_disabledCollisions.push_back(pair);
      }
      else if (strcmp(element->Value(), "DisabledCollisionSet") == 0)
      {
        PhysicsDisabledCollisionSetExportXML* pair = new PhysicsDisabledCollisionSetExportXML(element, this);
        m_disabledCollisions.push_back(pair);
      }
      else if (strcmp(element->Value(), "Attributes") == 0)
      {
        NMP_ASSERT(m_xmlAttributeBlock == 0);
        m_xmlAttributeBlock = new AttributeBlockXML(element);
        m_trajectoryCalcMarkupArray = m_xmlAttributeBlock->getAttributeByName("TrajectoryCalcMarkup");
      }
      else
      {
        NMP_ASSERT_MSG(false, "Unknown child element type of PhysicsRigDef");
      }
    }
  }
  NMP_ASSERT_MSG(m_xmlAttributeBlock != 0, "Attribute block element not found in PhysicsRigDef");
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigDefExportXML::PhysicsRigDefExportXML(
  const GUID     guid,
  const wchar_t* destFileName,
  const char*    rigName) :
  m_xmlElement(new XMLElement("PhysicsRigDef")),
  m_destFileName(toUTF8(destFileName)),
  m_trajectoryCalcMarkupArray(0)
{
  m_xmlElement->setGUID(guid);
  m_xmlElement->setAttribute("name", rigName);
  m_xmlAttributeBlock = new AttributeBlockXML();
  m_xmlElement->LinkEndChild(m_xmlAttributeBlock->getXMLElement());

  unsigned int emptyArray[1]  = { 0 };
  m_trajectoryCalcMarkupArray = m_xmlAttributeBlock->createUIntAttributeArray("TrajectoryCalcMarkup", emptyArray, 0);
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace ME
