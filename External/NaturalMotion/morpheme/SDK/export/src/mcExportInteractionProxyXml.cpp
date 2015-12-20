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
#include <string.h>
#include <stdarg.h>
#include "export/mcExportInteractionProxyXml.h"

#pragma warning(disable: 4127)

//----------------------------------------------------------------------------------------------------------------------
namespace ME
{

//----------------------------------------------------------------------------------------------------------------------
/// InteractionProxyExportXML
//----------------------------------------------------------------------------------------------------------------------
InteractionProxyExportXML::~InteractionProxyExportXML()
{
  delete m_xmlElement;
}

//----------------------------------------------------------------------------------------------------------------------
const char* InteractionProxyExportXML::getName() const
{
  return m_xmlElement->getAttribute("name");
}

//----------------------------------------------------------------------------------------------------------------------
GUID InteractionProxyExportXML::getGUID() const
{
  return m_xmlElement->getGUID();
}

//----------------------------------------------------------------------------------------------------------------------
const char* InteractionProxyExportXML::getDestFilename() const
{
  return m_destFileName.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
bool InteractionProxyExportXML::write()
{
  return m_xmlElement->saveFile(m_destFileName.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
void InteractionProxyExportXML::setDestFilename(const char* filename)
{
  m_destFileName = filename;
}

//----------------------------------------------------------------------------------------------------------------------
InteractionProxyExportXML::InteractionProxyExportXML(XMLElement* element, const std::string& destFileName) :
  m_xmlElement(element),
  m_destFileName(destFileName)
{
}

//----------------------------------------------------------------------------------------------------------------------
InteractionProxyExportXML::InteractionProxyExportXML(
  const GUID     guid,
  const wchar_t* destFileName,
  const char*    proxyName) :
  m_xmlElement(new XMLElement("InteractionProxy")),
  m_destFileName(toUTF8(destFileName))
{
  m_xmlElement->setGUID(guid);
  m_xmlElement->setAttribute("name", proxyName);
}

//----------------------------------------------------------------------------------------------------------------------
void InteractionProxyExportXML::setParentIndex(unsigned int index)
{
  m_xmlElement->setUIntAttribute("parent", index);
}

//----------------------------------------------------------------------------------------------------------------------
void InteractionProxyExportXML::setRotation(float x, float y, float z, float w)
{
  static char buf[128];
  sprintf(buf, "%f, %f, %f, %f", x, y, z, w);
  m_xmlElement->setChildText("Rotation", buf);
}

//----------------------------------------------------------------------------------------------------------------------
void InteractionProxyExportXML::setTranslation(float x, float y, float z)
{
  static char buf[128];
  sprintf(buf, "%f, %f, %f", x, y, z);
  m_xmlElement->setChildText("Translation", buf);
}

//----------------------------------------------------------------------------------------------------------------------
void InteractionProxyExportXML::setVisibility(bool visible)
{
  m_xmlElement->setAttribute("visible", visible ? "true" : "false");
}
 
//----------------------------------------------------------------------------------------------------------------------
void InteractionProxyExportXML::setUserData(unsigned int hipIndex)
{
  m_xmlElement->setUIntAttribute("userData", hipIndex);
}

//----------------------------------------------------------------------------------------------------------------------
void InteractionProxyExportXML::setShape(const char* name)
{
  m_xmlElement->setAttribute("shape", name);
}

//----------------------------------------------------------------------------------------------------------------------
void InteractionProxyExportXML::setHeight(float x)
{
  m_xmlElement->setFloatAttribute("height", x);
}

//----------------------------------------------------------------------------------------------------------------------
void InteractionProxyExportXML::setRadius(float x)
{
  m_xmlElement->setFloatAttribute("radius", x);
}

//----------------------------------------------------------------------------------------------------------------------
void InteractionProxyExportXML::setWidth(float x)
{
  m_xmlElement->setFloatAttribute("width", x);
}

//----------------------------------------------------------------------------------------------------------------------
void InteractionProxyExportXML::setDepth(float x)
{
  m_xmlElement->setFloatAttribute("depth", x);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int InteractionProxyExportXML::getParentIndex() const
{
  unsigned int val = 0;
  m_xmlElement->getUIntAttribute("parent", val);
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
void InteractionProxyExportXML::getRotation(float& x, float& y, float& z, float& w) const
{
  sscanf(m_xmlElement->getChildText("Rotation"),
    "%f, %f, %f, %f",
    &x, &y, &z, &w);
}

//----------------------------------------------------------------------------------------------------------------------
void InteractionProxyExportXML::getTranslation(float& x, float& y, float& z) const
{
  sscanf(m_xmlElement->getChildText("Translation"),
    "%f, %f, %f",
    &x, &y, &z);
}

//----------------------------------------------------------------------------------------------------------------------
bool InteractionProxyExportXML::getVisibility() const
{
  std::string result = m_xmlElement->getAttribute("visible");
  return (result == "true");
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int InteractionProxyExportXML::getUserData() const
{
  unsigned int val = 0;
  m_xmlElement->getUIntAttribute("userData", val);
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
const char* InteractionProxyExportXML::getShape() const
{
  return m_xmlElement->getAttribute("shape");
}

//----------------------------------------------------------------------------------------------------------------------
float InteractionProxyExportXML::getHeight() const
{
  float x = 1.0f;
  m_xmlElement->getFloatAttribute("height", x);
  return x;
}

//----------------------------------------------------------------------------------------------------------------------
float InteractionProxyExportXML::getRadius() const
{
  float x = 1.0f;
  m_xmlElement->getFloatAttribute("radius", x);
  return x;
}

//----------------------------------------------------------------------------------------------------------------------
float InteractionProxyExportXML::getWidth() const
{
  float x = 1.0f;
  m_xmlElement->getFloatAttribute("width", x);
  return x;
}

//----------------------------------------------------------------------------------------------------------------------
float InteractionProxyExportXML::getDepth() const
{
  float x = 1.0f;
  m_xmlElement->getFloatAttribute("depth", x);
  return x;
}

}
