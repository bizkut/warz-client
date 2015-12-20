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
#include "export/mcExportXml.h"
#include "export/mcExportBodyXml.h"
#include "export/mcExportPhysicsXml.h"

#pragma warning(disable: 4127)

//----------------------------------------------------------------------------------------------------------------------
namespace ME
{

//----------------------------------------------------------------------------------------------------------------------
/// HamstringExportXML
//----------------------------------------------------------------------------------------------------------------------
HamstringExportXML::HamstringExportXML(XMLElement* element) :
  m_xmlElement(element), m_xmlAttributeBlock(0)
{
  XMLElement* attributeBlockElement = m_xmlElement->findChild("Attributes");
  NMP_ASSERT(attributeBlockElement != 0);
  if (attributeBlockElement != 0)
  {
    m_xmlAttributeBlock = new AttributeBlockXML(attributeBlockElement);
  }
}

//----------------------------------------------------------------------------------------------------------------------
HamstringExportXML::HamstringExportXML() :
  m_xmlElement(new XMLElement("Hamstring"))
{
  m_xmlAttributeBlock = new AttributeBlockXML();
  m_xmlElement->LinkEndChild(m_xmlAttributeBlock->getXMLElement());
}

//----------------------------------------------------------------------------------------------------------------------
HamstringExportXML::~HamstringExportXML()
{
  delete m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
const AttributeBlockExport* HamstringExportXML::getAttributeBlock() const
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeBlockExport* HamstringExportXML::getAttributeBlock()
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
/// LimbDefExportXML
//----------------------------------------------------------------------------------------------------------------------
LimbDefExportXML::~LimbDefExportXML()
{
  delete m_hamstring;
  delete m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
HamstringExport* LimbDefExportXML::getHamstring()
{
  return m_hamstring;
}

//----------------------------------------------------------------------------------------------------------------------
const char* LimbDefExportXML::getName() const
{
  return m_xmlElement->getAttribute("name");
}

//----------------------------------------------------------------------------------------------------------------------
const AttributeBlockExport* LimbDefExportXML::getAttributeBlock() const
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeBlockExport* LimbDefExportXML::getAttributeBlock()
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
LimbDefExportXML::LimbDefExportXML(XMLElement* element) : m_xmlElement(element)
{
  XMLElement* hamstringElement = m_xmlElement->findChild("Hamstring");
  NMP_ASSERT(hamstringElement != 0);
  if (hamstringElement != 0)
  {
    m_hamstring = new HamstringExportXML(hamstringElement);
  }

  XMLElement* attributeBlockElement = m_xmlElement->findChild("Attributes");
  NMP_ASSERT(attributeBlockElement != 0);
  if (attributeBlockElement != 0)
  {
    m_xmlAttributeBlock = new AttributeBlockXML(attributeBlockElement);
  }
}

//----------------------------------------------------------------------------------------------------------------------
LimbDefExportXML::LimbDefExportXML(const char* name) : m_xmlElement(new XMLElement("Limb"))
{
  m_xmlElement->setAttribute("name", name);

  m_hamstring = new HamstringExportXML();
  m_xmlElement->LinkEndChild(m_hamstring->m_xmlElement);

  m_xmlAttributeBlock = new AttributeBlockXML();
  m_xmlElement->LinkEndChild(m_xmlAttributeBlock->getXMLElement());

  m_xmlAttributeBlock->createStringAttribute("endPartName", "");
  m_xmlAttributeBlock->createStringAttribute("midPartName", "");
  m_xmlAttributeBlock->createStringAttribute("rootPartName", "");
  m_xmlAttributeBlock->createStringAttribute("limbType", "");
  m_xmlAttributeBlock->createMatrix34Attribute("endOffset", NMP::Matrix34Identity());
  m_xmlAttributeBlock->createMatrix34Attribute("rootOffset", NMP::Matrix34Identity());
  m_xmlAttributeBlock->createStringAttributeArray("extraParts", 0, 0);
}

//----------------------------------------------------------------------------------------------------------------------
const HamstringExport* LimbDefExportXML::getHamstring() const
{
  return m_hamstring;
}

//----------------------------------------------------------------------------------------------------------------------
/// SelfAvoidanceExport
//----------------------------------------------------------------------------------------------------------------------
void SelfAvoidanceExportXML::setRadius(float r)
{
  m_xmlElement->setFloatAttribute("radius", r);
}

//----------------------------------------------------------------------------------------------------------------------
float SelfAvoidanceExportXML::getRadius() const
{
  float r = 0.0f;
  m_xmlElement->getFloatAttribute("radius", r);
  return r;
}

//----------------------------------------------------------------------------------------------------------------------
SelfAvoidanceExportXML::SelfAvoidanceExportXML(
  XMLElement* element) : m_xmlElement(element)
{
}

//----------------------------------------------------------------------------------------------------------------------
SelfAvoidanceExportXML::SelfAvoidanceExportXML() :
  m_xmlElement(new XMLElement("SelfAvoidance"))
{
}

//----------------------------------------------------------------------------------------------------------------------
/// BodyDefExportXML
//----------------------------------------------------------------------------------------------------------------------
BodyDefExportXML::~BodyDefExportXML()
{
  delete m_xmlElement;
  delete m_xmlAttributeBlock;
  delete m_selfAvoidance;

  size_t count = m_limbs.size();
  for (size_t i = 0; i != count; ++i)
  {
    delete m_limbs[i];
  }
  
  count = m_animationPoses.size();
  for (size_t i = 0; i != count; ++i)
  {
    delete m_animationPoses[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
const char* BodyDefExportXML::getName() const
{
  return m_xmlElement->getAttribute("name");
}

//----------------------------------------------------------------------------------------------------------------------
GUID BodyDefExportXML::getGUID() const
{
  return m_xmlElement->getGUID();
}

//----------------------------------------------------------------------------------------------------------------------
const char* BodyDefExportXML::getDestFilename() const
{
  return m_destFileName.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
bool BodyDefExportXML::write()
{
  return m_xmlElement->saveFile(m_destFileName.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
void BodyDefExportXML::setDestFilename(const char* filename)
{
  m_destFileName = filename;
}

//----------------------------------------------------------------------------------------------------------------------
LimbDefExport* BodyDefExportXML::createLimb(const char* name)
{
  LimbDefExportXML* limb = new LimbDefExportXML(name);
  m_xmlElement->LinkEndChild(limb->m_xmlElement);
  m_limbs.push_back(limb);
  return limb;
}

//----------------------------------------------------------------------------------------------------------------------
const LimbDefExport* BodyDefExportXML::getLimb(uint32_t index) const
{
  if (index < (uint32_t)m_limbs.size())
  {
    return m_limbs[index];
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t BodyDefExportXML::getNumLimbs() const
{
  return (uint32_t)m_limbs.size();
}

//----------------------------------------------------------------------------------------------------------------------
const AttributeBlockExport* BodyDefExportXML::getAttributeBlock() const
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeBlockExport* BodyDefExportXML::getAttributeBlock()
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
SelfAvoidanceExport* BodyDefExportXML::createSelfAvoidanceExport()
{
  SelfAvoidanceExportXML* set = new SelfAvoidanceExportXML();
  m_xmlElement->LinkEndChild(set->m_xmlElement);
  return set;
}

//----------------------------------------------------------------------------------------------------------------------
void BodyDefExportXML::getSelfAvoidance(float& radius) const
{
  radius = m_selfAvoidance ? m_selfAvoidance->getRadius() : 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
AnimationPoseDefExport* BodyDefExportXML::createAnimationPose(const char* name)
{
  AnimationPoseDefExportXML* thePose = new AnimationPoseDefExportXML(m_xmlElement, name);
  m_animationPoses.push_back(thePose);
  return thePose;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t BodyDefExportXML::getAnimationPoseCount() const
{
  return (uint32_t)m_animationPoses.size();
}

//----------------------------------------------------------------------------------------------------------------------
const AnimationPoseDefExport* BodyDefExportXML::getAnimationPose(uint32_t index) const
{
  return m_animationPoses[index];
}

//----------------------------------------------------------------------------------------------------------------------
const AnimationPoseDefExport* BodyDefExportXML::getAnimationPose(const char* name) const
{
  for (std::vector<AnimationPoseDefExport*>::const_iterator it = m_animationPoses.begin(), 
    end = m_animationPoses.end(); it!=end; ++it)
  {
    AnimationPoseDefExport* pose = *it;
    const char* currentPoseDefName = pose->getName();
    if (strcmp(currentPoseDefName, name) == 0)
    {
      return pose;
    }
  }

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
BodyDefExportXML::BodyDefExportXML(
   XMLElement*         element, 
   ExportFactoryXML*   factory,
   const char*         destFileName) :
  m_xmlElement(element),
  m_exportFactory(factory),
  m_destFileName(destFileName),
  m_xmlAttributeBlock(0),
  m_selfAvoidance(0)
{
  for (NM::TiXmlNode* curChild = m_xmlElement->FirstChild(); curChild != 0; curChild = curChild->NextSibling())
  {
    XMLElement* element = static_cast<XMLElement*>(curChild->ToElement());
    if (element)
    {
      const char *value = element->Value();
      if (strcmp(value, "Limb") == 0)
      {
        LimbDefExportXML* limb = new LimbDefExportXML(element);
        m_limbs.push_back(limb);
      }
      else if (strcmp(value, "Attributes") == 0)
      {
        NMP_ASSERT(m_xmlAttributeBlock == 0);
        m_xmlAttributeBlock = new AttributeBlockXML(element);
      }
      else if (strcmp(value, "SelfAvoidance") == 0)
      {
        NMP_ASSERT(m_selfAvoidance == 0);
        m_selfAvoidance = new SelfAvoidanceExportXML(element);
      }
      else if (strcmp(value, "AnimationPose") == 0)
      {
        AnimationPoseDefExportXML* animPose = new AnimationPoseDefExportXML(element);
        m_animationPoses.push_back(animPose);
      }
      else
      {
        NMP_ASSERT_MSG(false, "Unknown child element type of PhysicsRigDef");
      }
    }
  }
  NMP_ASSERT_MSG(m_xmlAttributeBlock != 0, "Attribute block element not found in BodyDef");
}

//----------------------------------------------------------------------------------------------------------------------
BodyDefExportXML::BodyDefExportXML(
  const GUID     guid,
  const wchar_t* destFileName,
  const char*    bodyName) :
  m_xmlElement(new XMLElement("BodyDef")),
  m_exportFactory(0),
  m_destFileName(toUTF8(destFileName)),
  m_selfAvoidance(0)
{
  m_xmlElement->setGUID(guid);
  m_xmlElement->setAttribute("name", bodyName);
  m_xmlAttributeBlock = new AttributeBlockXML();
  m_xmlElement->LinkEndChild(m_xmlAttributeBlock->getXMLElement());

  m_xmlAttributeBlock->createStringAttribute("physicsRig", "");
  m_xmlAttributeBlock->createStringAttribute("rig", "");
  m_xmlAttributeBlock->createStringAttribute("rootLimbName", "");
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace ME
