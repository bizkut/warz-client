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
#include "export/mcExportControllerXml.h"

#pragma warning(disable: 4127)

//----------------------------------------------------------------------------------------------------------------------
namespace ME
{

//----------------------------------------------------------------------------------------------------------------------
/// CharacterControllerExportXML
//----------------------------------------------------------------------------------------------------------------------
CharacterControllerExportXML::~CharacterControllerExportXML()
{
  delete m_xmlElement;
  delete m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
const char* CharacterControllerExportXML::getName() const
{
  return m_xmlElement->getAttribute("name");
}

//----------------------------------------------------------------------------------------------------------------------
GUID CharacterControllerExportXML::getGUID() const
{
  return m_xmlElement->getGUID();
}

//----------------------------------------------------------------------------------------------------------------------
const char* CharacterControllerExportXML::getDestFilename() const
{
  return m_destFileName.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
bool CharacterControllerExportXML::write()
{
  return m_xmlElement->saveFile(m_destFileName.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerExportXML::setDestFilename(const char* filename)
{
  m_destFileName = filename;
}

//----------------------------------------------------------------------------------------------------------------------
const AttributeBlockExport* CharacterControllerExportXML::getAttributeBlock() const
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeBlockExport* CharacterControllerExportXML::getAttributeBlock()
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
CharacterControllerExportXML::CharacterControllerExportXML(XMLElement* element, const std::string& destFileName) :
  m_xmlElement(element),
  m_destFileName(destFileName),
  m_xmlAttributeBlock(0)
{
  for (NM::TiXmlNode* curChild = m_xmlElement->FirstChild(); curChild != 0; curChild = curChild->NextSibling())
  {
    XMLElement* element = static_cast<XMLElement*>(curChild->ToElement());
    if (element)
    {
      if (strcmp(element->Value(), "Attributes") == 0)
      {
        NMP_ASSERT(m_xmlAttributeBlock == 0);
        m_xmlAttributeBlock = new AttributeBlockXML(element);
      }
      else
      {
        NMP_ASSERT_MSG(false, "Unknown child element type of CharacterController");
      }
    }
  }
  NMP_ASSERT_MSG(m_xmlAttributeBlock != 0, "Attribute block element not found in CharacterController");
}

//----------------------------------------------------------------------------------------------------------------------
CharacterControllerExportXML::CharacterControllerExportXML(
  const GUID     guid,
  const wchar_t* destFileName,
  const char*    controllerName) :
  m_xmlElement(new XMLElement("CharacterController")),
  m_destFileName(toUTF8(destFileName))
{
  m_xmlElement->setGUID(guid);
  m_xmlElement->setAttribute("name", controllerName);
  m_xmlAttributeBlock = new AttributeBlockXML();
  m_xmlElement->LinkEndChild(m_xmlAttributeBlock->getXMLElement());
}

}
