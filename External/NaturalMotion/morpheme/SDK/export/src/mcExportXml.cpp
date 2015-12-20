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
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMBasicLogger.h"

#include "export/mcExportXml.h"
#include "export/mcExportPhysicsXml.h"
#include "export/mcExportBodyXml.h"
#include "export/mcExportControllerXml.h"
#include "export/mcExportInteractionProxyXml.h"
#include "export/mcExportMessagePresetsXml.h"

#if defined(_MSC_VER)
  #pragma warning(disable:4996)
  #pragma warning(disable:4616)
  #pragma warning(disable:4250)
  #pragma warning(disable:4100)
#endif

// Instate this define in order to output messages tracking the progress of the export.
//#define OUTPUT_mcExport_MSGS 1
#if defined(OUTPUT_mcExport_MSGS)
static NMP::BasicLogger exportLogger;
  #define mcExport_MSG(fmt, ...) { exportLogger.output(#fmt, ##__VA_ARGS__); }
#else
  #define mcExport_MSG(fmt, ...)
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace ME
{

//----------------------------------------------------------------------------------------------------------------------
std::string FixPath(const char* inPath)
{
  std::string path(inPath);

  // Remove backslashes
  unsigned int dest = 0;
  unsigned int src = 0;
  for (; src < path.size() ; ++src)
  {
    if (path[src] == '\\')
    {
      path[dest] = '/';

      // Remove the second backslash from any "\\"'s
      if ((src < path.size() - 1) &&
          (path[src+1] == '\\'))
      {
        ++src;
      }
    }
    else
    {
      path[dest] = path[src];
    }

    ++dest;
  }

  if (src != dest)
  {
    path.resize(dest);
  }

  return path;
}

//----------------------------------------------------------------------------------------------------------------------
// Generate a animation file name with its full path specification.
std::string demacroizeString(const char* macroString, const char* rootDir)
{
  // test to see if it string start with "$(RootDir)"
  const char* locationOfRootDir = strstr(macroString, "$(RootDir)");
  std::string result;
  if (locationOfRootDir == macroString)
  {
    // The fileName starts with '$(RootDir)' replace it with stored root directory for asset loading (kCLA_BASE_DIR).
    result = rootDir;
    result = result + (macroString + 10); // append string after "$(RootDir)"
  }
  else
  {
    result = macroString;
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
std::string ConvertToRelativePath(const char* inPath)
{
  std::string path = FixPath(inPath);
  std::string::size_type lastSlashPos = path.rfind('/');

  if (lastSlashPos != std::string::npos)
  {
    return std::string(path, lastSlashPos + 1, std::string::npos);
  }

  return path;
}

//----------------------------------------------------------------------------------------------------------------------
std::string GetDirectory(const char* inPath)
{
  std::string path = FixPath(inPath);
  std::string::size_type lastSlashPos = path.rfind('/');

  if (lastSlashPos != std::string::npos)
  {
    return std::string(path, 0, lastSlashPos + 1);
  }
  return std::string("./");
}

//----------------------------------------------------------------------------------------------------------------------
// XMLReference
//----------------------------------------------------------------------------------------------------------------------
XMLReference::XMLReference(
  const char* referenceType,
  GUID        guid,
  const char* filename,
  const char* name) :
  XMLElement(referenceType)
{
  setAttribute("refGUID", guid);
  setAttribute("filename", filename);
  if (name != 0)
  {
    setAttribute("refName", name);
  }
}

//----------------------------------------------------------------------------------------------------------------------
GUID XMLReference::getGUID() const
{
  return getAttribute("refGUID");
}

//----------------------------------------------------------------------------------------------------------------------
const char* XMLReference::getFilename() const
{
  return getAttribute("filename");
}

//----------------------------------------------------------------------------------------------------------------------
const char* XMLReference::getName() const
{
  return getAttribute("refName");
}

//----------------------------------------------------------------------------------------------------------------------
// XMLReference
//----------------------------------------------------------------------------------------------------------------------

unsigned int MessageNameInfoXML::getMessageID() const
{
  unsigned int val;
  getUIntAttribute("messageID", val);
  return val;
}

const char* MessageNameInfoXML::getMessageName() const
{
  return getAttribute("messageName");
}

//----------------------------------------------------------------------------------------------------------------------
// InstanceInfoXML
//----------------------------------------------------------------------------------------------------------------------
unsigned int InstanceInfoXML::getTypeID() const
{
  unsigned int val;
  getUIntAttribute("typeID", val);
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int InstanceInfoXML::getMaxInstances() const
{
  unsigned int val;
  getUIntAttribute("maxInstances", val);
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int InstanceInfoXML::getTotalInstances() const
{
  unsigned int val;
  getUIntAttribute("totalInstances", val);
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
// DataBlockExportXML::Element
//----------------------------------------------------------------------------------------------------------------------
#define BUFFER_LENGTH (1024)

DataBlockExportXML::Element::Element(
  const void*  data,
  size_t       length,
  const char*  type,
  const char*  desc, 
  bool includeLengthXmlAttribute) :
  XMLElement("DataElement")
{
  setAttribute("type", type);
  if(includeLengthXmlAttribute)
  {
    setUIntAttribute("bytes", (unsigned int)length);
  }
  if (desc)
  {
    setAttribute("description", desc);
  }
  setBytes(data, length);
}

bool DataBlockExportXML::Element::getBytes(char* data_out, size_t bufferSize, size_t& length_out) const
{
  static char buffer[BUFFER_LENGTH];
  const size_t bufferLengthExcludingNULL = BUFFER_LENGTH - 1;
  const char* childText = getText();
  if (!childText)
    return false;

  // The stored attribute data may be longer than the buffer length so we must read
  // it in chunks.
  size_t bufferPos = 0;
  size_t length = strlen(childText);
  while (length > 0)
  {
    // Find the delimiter within the allowable length of the buffer
    if (length > bufferLengthExcludingNULL)
    {
      size_t chunkSize;
      for (chunkSize = bufferLengthExcludingNULL - 1; chunkSize > 0; --chunkSize)
      {
        if (childText[chunkSize] == ',')
          break;
      }
      // The buffer is not big enough to parse the string
      if (chunkSize == 0)
        return false;

      // Copy the string chunk (including the delimiter) and update
      chunkSize++;
      strncpy_s(buffer, BUFFER_LENGTH, childText, chunkSize);
      childText += chunkSize;
      length -= chunkSize;
    }
    else
    {
      // Copy the string into the buffer
      strcpy(buffer, childText);
      length = 0;
    }

    // Parse the buffer chunk
    char* token = strtok(buffer, ",");
    while (token != 0)
    {
      // Check we do not overflow the output buffer
      if (bufferPos >= bufferSize)
        return false;

      // Read the value into the output buffer
      unsigned int readInt;
      sscanf(token, "%02x", &readInt);
      NMP_ASSERT(readInt <= 255);
      data_out[bufferPos] = (char)readInt;
      bufferPos++;

      // Get the next token
      token = strtok(0, ",");
    }
  }

  length_out = bufferPos;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
size_t DataBlockExportXML::Element::getByteCount() const
{
  static char buffer[BUFFER_LENGTH];
  const size_t bufferLengthExcludingNULL = BUFFER_LENGTH - 1;
  const char* childText = getText();
  if (!childText)
    return 0;

  size_t count = 0;

  // The stored attribute data may be longer than the buffer length so we must read
  // it in chunks.
  size_t length = strlen(childText);
  while (length > 0)
  {
    // Find the delimiter within the allowable length of the buffer
    if (length > bufferLengthExcludingNULL)
    {
      size_t chunkSize;
      for (chunkSize = bufferLengthExcludingNULL - 1; chunkSize > 0; --chunkSize)
      {
        if (childText[chunkSize] == ',')
          break;
      }
      // The buffer is not big enough to parse the string
      if (chunkSize == 0)
        return false;

      // Copy the string chunk (including the delimiter) and update
      chunkSize++;
      strncpy_s(buffer, BUFFER_LENGTH, childText, chunkSize);
      childText += chunkSize;
      length -= chunkSize;
    }
    else
    {
      // Copy the string into the buffer
      strcpy(buffer, childText);
      length = 0;
    }

    // Parse the buffer chunk
    char* token = strtok(buffer, ",");
    while (token != 0)
    {
      ++count;

      // Get the next token
      token = strtok(0, ",");
    }
  }

  return count;
}

//----------------------------------------------------------------------------------------------------------------------
// AttributeExportXML
//----------------------------------------------------------------------------------------------------------------------
AttributeExportXML::AttributeExportXML(XMLElement* element)
{
  m_attributeElement = element;
  m_type = AttributeExport::ATTR_TYPE_STRING; // if there is an error just interpret the data as a string
  std::string typeStr = element->getAttribute("type");
  if (typeStr == "int")
  {
    m_type = AttributeExport::ATTR_TYPE_INT;
  }
  else if (typeStr == "uint")
  {
    m_type = AttributeExport::ATTR_TYPE_UINT;
  }
  else if (typeStr == "double")
  {
    m_type = AttributeExport::ATTR_TYPE_DOUBLE;
  }
  else if (typeStr == "string")
  {
    m_type = AttributeExport::ATTR_TYPE_STRING;
  }
  else if (typeStr == "bool")
  {
    m_type = AttributeExport::ATTR_TYPE_BOOL;
  }
  else if (typeStr == "vector3")
  {
    m_type = AttributeExport::ATTR_TYPE_VECTOR3;
  }
  else if (typeStr == "matrix34")
  {
    m_type = AttributeExport::ATTR_TYPE_MATRIX34;
  }
  else if (typeStr == "boolArray")
  {
    m_type = AttributeExport::ATTR_TYPE_BOOL_ARRAY;
  }
  else if (typeStr == "uintArray")
  {
    m_type = AttributeExport::ATTR_TYPE_UINT_ARRAY;
  }
  else if (typeStr == "floatArray")
  {
    m_type = AttributeExport::ATTR_TYPE_FLOAT_ARRAY;
  }
  else if (typeStr == "vector3Array")
  {
    m_type = AttributeExport::ATTR_TYPE_VECTOR3_ARRAY;
  }
  else if (typeStr == "stringArray")
  {
    m_type = AttributeExport::ATTR_TYPE_STRING_ARRAY;
  }
}

//----------------------------------------------------------------------------------------------------------------------
AttributeExportXML::AttributeExportXML(const char* name) : m_attributeElement(new XMLElement(name))
{
  m_attributeElement->setAttribute("type", "string");
}

//----------------------------------------------------------------------------------------------------------------------
AttributeExportXML::~AttributeExportXML()
{

}
//----------------------------------------------------------------------------------------------------------------------
const char* AttributeExportXML::getName() const
{
  return m_attributeElement->Value();
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExportXML::getAsInt(int& value) const
{
  if (getType() == AttributeExport::ATTR_TYPE_INT)
  {
    value = m_attributeElement->getInt();
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExportXML::getAsUInt(unsigned int& value) const
{
  if (getType() == AttributeExport::ATTR_TYPE_UINT)
  {
    value = m_attributeElement->getUInt();
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExportXML::getAsDouble(double& value) const
{
  if (getType() == AttributeExport::ATTR_TYPE_DOUBLE)
  {
    value = m_attributeElement->getDouble();
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExportXML::getAsString(const char *& value) const
{
  if (getType() == AttributeExport::ATTR_TYPE_STRING)
  {
    value = m_attributeElement->getText();
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExportXML::getAsStringArray(const char* values[], unsigned int size) const
{
  if (getType() == AttributeExport::ATTR_TYPE_STRING_ARRAY)
  {
    unsigned int count = 0;
    for (NM::TiXmlNode* curChild = m_attributeElement->FirstChild();
         count < size && curChild != 0;
         curChild = curChild->NextSibling(), ++count)
    {
      XMLElement* element = static_cast<XMLElement*>(curChild->ToElement());
      if (element)
      {
        values[count] = element->getText();
      }
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExportXML::getAsBool(bool& value) const
{
  if (getType() == AttributeExport::ATTR_TYPE_BOOL)
  {
    std::string textValue = m_attributeElement->getText();
    if (textValue == "true")
    {
      value = true;
      return true;
    }
    else if (textValue == "false")
    {
      value = false;
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExportXML::getAsVector3(NMP::Vector3& value) const
{
  if (getType() == AttributeExport::ATTR_TYPE_VECTOR3)
  {
    value = m_attributeElement->getVector3();
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExportXML::getAsQuat(NMP::Quat& value) const
{
  if (getType() == AttributeExport::ATTR_TYPE_QUAT)
  {
    value = m_attributeElement->getQuat();
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExportXML::getAsMatrix34(NMP::Matrix34& value) const
{
  if (getType() == AttributeExport::ATTR_TYPE_MATRIX34)
  {
    for (unsigned int i = 0; i != 4; ++i)
    {
      char buffer[32];
      sprintf(buffer, "r%u", i);
      if (!m_attributeElement->getChildVector3(buffer, value.r[i]))
      {
        return false;
      }
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExportXML::getAsBoolArray(bool values[], unsigned int size) const
{
  if (getType() == AttributeExport::ATTR_TYPE_BOOL_ARRAY)
  {
    unsigned int count = 0;
    for (NM::TiXmlNode* curChild = m_attributeElement->FirstChild();
      count < size && curChild != 0;
      curChild = curChild->NextSibling(), ++count)
    {
      XMLElement* element = static_cast<XMLElement*>(curChild->ToElement());
      if (element)
      {
        values[count] = element->getBool();
      }
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExportXML::getAsUIntArray(unsigned int values[], unsigned int size) const
{
  if (getType() == AttributeExport::ATTR_TYPE_UINT_ARRAY)
  {
    unsigned int count = 0;
    for (NM::TiXmlNode* curChild = m_attributeElement->FirstChild();
         count < size && curChild != 0;
         curChild = curChild->NextSibling(), ++count)
    {
      XMLElement* element = static_cast<XMLElement*>(curChild->ToElement());
      if (element)
      {
        values[count] = element->getUInt();
      }
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExportXML::getAsFloatArray(float values[], unsigned int size) const
{
  if (getType() == AttributeExport::ATTR_TYPE_FLOAT_ARRAY)
  {
    unsigned int count = 0;
    for (NM::TiXmlNode* curChild = m_attributeElement->FirstChild();
      count < size && curChild != 0;
      curChild = curChild->NextSibling(), ++count)
    {
      XMLElement* element = static_cast<XMLElement*>(curChild->ToElement());
      if (element)
      {
        values[count] = element->getFloat();
      }
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExportXML::getAsVector3Array(NMP::Vector3 values[], unsigned int size) const
{
  if (getType() == AttributeExport::ATTR_TYPE_VECTOR3_ARRAY)
  {
    unsigned int count = 0;
    for (NM::TiXmlNode* curChild = m_attributeElement->FirstChild();
         count < size && curChild != 0;
         curChild = curChild->NextSibling(), ++count)
    {
      XMLElement* element = static_cast<XMLElement*>(curChild->ToElement());
      if (element)
      {
        values[count] = element->getVector3();
      }
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExportXML::setAsInt(int value)
{
  m_type = AttributeExport::ATTR_TYPE_INT;
  m_attributeElement->setAttribute("type", "int");
  m_attributeElement->setInt(value);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExportXML::setAsUInt(unsigned int value)
{
  m_type = AttributeExport::ATTR_TYPE_UINT;
  m_attributeElement->setAttribute("type", "uint");
  m_attributeElement->setUInt(value);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExportXML::setAsDouble(double value)
{
  m_type = AttributeExport::ATTR_TYPE_DOUBLE;
  m_attributeElement->setAttribute("type", "double");
  m_attributeElement->setDouble(value);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExportXML::setAsString(const char* value)
{
  m_type = AttributeExport::ATTR_TYPE_STRING;
  m_attributeElement->setAttribute("type", "string");
  m_attributeElement->setText(value);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExportXML::setAsStringArray(const char* values[], unsigned int size)
{
  m_type = AttributeExport::ATTR_TYPE_STRING_ARRAY;
  m_attributeElement->setAttribute("type", "stringArray");
  m_attributeElement->setUIntAttribute("size", size);
  for (unsigned int i = 0; i != size; ++i)
  {
    m_attributeElement->addChildText("elem", values[i]);
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExportXML::setAsBool(bool value)
{
  m_type = AttributeExport::ATTR_TYPE_BOOL;
  m_attributeElement->setAttribute("type", "bool");
  if (value)
  {
    m_attributeElement->setText("true");
  }
  else
  {
    m_attributeElement->setText("false");
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExportXML::setAsVector3(const NMP::Vector3& value)
{
  m_type = AttributeExport::ATTR_TYPE_VECTOR3;
  m_attributeElement->setAttribute("type", "vector3");
  m_attributeElement->setVector3(value);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExportXML::setAsQuat(const NMP::Quat& value)
{
  m_type = AttributeExport::ATTR_TYPE_QUAT;
  m_attributeElement->setAttribute("type", "quat");
  m_attributeElement->setQuat(value);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExportXML::setAsMatrix34(const NMP::Matrix34& value)
{
  m_type = AttributeExport::ATTR_TYPE_MATRIX34;
  m_attributeElement->setAttribute("type", "matrix34");

  for (int i = 0; i != 4; ++i)
  {
    char buffer[32];
    sprintf(buffer, "r%d", i);
    XMLElement* row = m_attributeElement->findChild(buffer);
    if (!row)
    {
      row = m_attributeElement->addChild(buffer);
    }
    row->setVector3(value.r[i]);
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExportXML::setAsBoolArray(const bool values[], unsigned int size)
{
  m_type = AttributeExport::ATTR_TYPE_BOOL_ARRAY;
  m_attributeElement->setAttribute("type", "boolArray");
  m_attributeElement->setUIntAttribute("size", size);
  for (unsigned int i = 0; i != size; ++i)
  {
    const char* textValue = "0";
    if (values[i])
    {
      textValue = "1";
    }

     m_attributeElement->addChildText("elem", textValue);
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExportXML::setAsUIntArray(const unsigned int values[], unsigned int size)
{
  m_type = AttributeExport::ATTR_TYPE_UINT_ARRAY;
  m_attributeElement->setAttribute("type", "uintArray");
  m_attributeElement->setUIntAttribute("size", size);
  for (unsigned int i = 0; i != size; ++i)
  {
    m_attributeElement->addChildUInt("elem", values[i]);
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExportXML::setAsFloatArray(const float values[], unsigned int size)
{
  m_type = AttributeExport::ATTR_TYPE_FLOAT_ARRAY;
  m_attributeElement->setAttribute("type", "floatArray");
  m_attributeElement->setUIntAttribute("size", size);
  for (unsigned int i = 0; i != size; ++i)
  {
    m_attributeElement->addChildFloat("elem", values[i]);
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExportXML::setAsVector3Array(const NMP::Vector3 values[], unsigned int size)
{
  m_type = AttributeExport::ATTR_TYPE_VECTOR3_ARRAY;
  m_attributeElement->setAttribute("type", "vector3Array");
  m_attributeElement->setUIntAttribute("size", size);
  for (unsigned int i = 0; i != size; ++i)
  {
    m_attributeElement->addChildVector3("elem", values[i]);
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExportXML::appendToUIntArray(unsigned int value)
{
  if (m_type == AttributeExport::ATTR_TYPE_UINT_ARRAY)
  {
    unsigned int size = 0;
    m_attributeElement->getUIntAttribute("size", size);
    m_attributeElement->setUIntAttribute("size", size + 1);
    m_attributeElement->addChildUInt("elem", value);
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExportXML::appendToFloatArray(float value)
{
  if (m_type == AttributeExport::ATTR_TYPE_FLOAT_ARRAY)
  {
    unsigned int size = 0;
    m_attributeElement->getUIntAttribute("size", size);
    m_attributeElement->setUIntAttribute("size", size + 1);
    m_attributeElement->addChildFloat("elem", value);
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExportXML::appendToVector3Array(const NMP::Vector3& value)
{
  if (m_type == AttributeExport::ATTR_TYPE_VECTOR3_ARRAY)
  {
    unsigned int size = 0;
    m_attributeElement->getUIntAttribute("size", size);
    m_attributeElement->setUIntAttribute("size", size + 1);
    m_attributeElement->addChildVector3("elem", value);
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExportXML::appendToStringArray(const char* value)
{
  if (m_type == AttributeExport::ATTR_TYPE_STRING_ARRAY)
  {
    unsigned int size = 0;
    m_attributeElement->getUIntAttribute("size", size);
    m_attributeElement->setUIntAttribute("size", size + 1);
    m_attributeElement->addChildText("elem", value);
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeExport::AttributeType AttributeExportXML::getType() const
{
  return m_type;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int AttributeExportXML::getSize() const
{
  unsigned int size = 0;
  if (isArray())
  {
    m_attributeElement->getUIntAttribute("size", size);
  }
  return size;
}

//----------------------------------------------------------------------------------------------------------------------
// AttributeBlockXML
//----------------------------------------------------------------------------------------------------------------------
AttributeBlockXML::AttributeBlockXML(XMLElement* element)
{
  m_xmlElement = element;
  // loop over all children and create Attributes for them
  // name doesn't matter
  for (NM::TiXmlNode* childNode = m_xmlElement->FirstChild(); childNode != 0; childNode = childNode->NextSibling())
  {
    XMLElement* element = (XMLElement*)childNode->ToElement();
    if (element)
    {
      AttributeExportXML* newAttr = new AttributeExportXML(element);
      m_attributes.push_back(newAttr);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
AttributeBlockXML::AttributeBlockXML(): m_xmlElement(new XMLElement("Attributes"))
{
}

//----------------------------------------------------------------------------------------------------------------------
AttributeBlockXML::~AttributeBlockXML()
{
  for (unsigned int i = 0; i < m_attributes.size(); ++i)
  {
    delete m_attributes[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int AttributeBlockXML::getNumAttributes() const
{
  return (unsigned int) m_attributes.size();
}

//----------------------------------------------------------------------------------------------------------------------
const AttributeExport* AttributeBlockXML::getAttributeByIndex(unsigned int attrIdx) const
{
  if (attrIdx < m_attributes.size())
  {
    return m_attributes[attrIdx];
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeExport* AttributeBlockXML::getAttributeByIndex(unsigned int attrIdx)
{
  if (attrIdx < m_attributes.size())
  {
    return m_attributes[attrIdx];
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeExport* AttributeBlockXML::createDoubleAttribute(const char* name, double value)
{
  if (!hasAttribute(name))
  {
    AttributeExportXML* newAttr = new AttributeExportXML(name);
    newAttr->setAsDouble(value);
    m_xmlElement->LinkEndChild(newAttr->m_attributeElement);
    m_attributes.push_back(newAttr);
    return newAttr;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeExport* AttributeBlockXML::createIntAttribute(const char* name, int value)
{
  if (!hasAttribute(name))
  {
    AttributeExportXML* newAttr = new AttributeExportXML(name);
    newAttr->setAsInt(value);
    m_xmlElement->LinkEndChild(newAttr->m_attributeElement);
    m_attributes.push_back(newAttr);
    return newAttr;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeExport* AttributeBlockXML::createUIntAttribute(const char* name, unsigned int value)
{
  if (!hasAttribute(name))
  {
    AttributeExportXML* newAttr = new AttributeExportXML(name);
    newAttr->setAsUInt(value);
    m_xmlElement->LinkEndChild(newAttr->m_attributeElement);
    m_attributes.push_back(newAttr);
    return newAttr;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeExport* AttributeBlockXML::createStringAttribute(const char* name, const char* value)
{
  if (!hasAttribute(name))
  {
    AttributeExportXML* newAttr = new AttributeExportXML(name);
    newAttr->setAsString(value);
    m_xmlElement->LinkEndChild(newAttr->m_attributeElement);
    m_attributes.push_back(newAttr);
    return newAttr;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeExport* AttributeBlockXML::createBoolAttribute(const char* name, bool value)
{
  if (!hasAttribute(name))
  {
    AttributeExportXML* newAttr = new AttributeExportXML(name);
    newAttr->setAsBool(value);
    m_xmlElement->LinkEndChild(newAttr->m_attributeElement);
    m_attributes.push_back(newAttr);
    return newAttr;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeExport* AttributeBlockXML::createVector3Attribute(const char* name, const NMP::Vector3& value)
{
  if (!hasAttribute(name))
  {
    AttributeExportXML* newAttr = new AttributeExportXML(name);
    newAttr->setAsVector3(value);
    m_xmlElement->LinkEndChild(newAttr->m_attributeElement);
    m_attributes.push_back(newAttr);
    return newAttr;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeExport* AttributeBlockXML::createQuatAttribute(const char* name, const NMP::Quat& value)
{
  if (!hasAttribute(name))
  {
    AttributeExportXML* newAttr = new AttributeExportXML(name);
    newAttr->setAsQuat(value);
    m_xmlElement->LinkEndChild(newAttr->m_attributeElement);
    m_attributes.push_back(newAttr);
    return newAttr;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeExport* AttributeBlockXML::createMatrix34Attribute(const char* name, const NMP::Matrix34& value)
{
  if (!hasAttribute(name))
  {
    AttributeExportXML* newAttr = new AttributeExportXML(name);
    newAttr->setAsMatrix34(value);
    m_xmlElement->LinkEndChild(newAttr->m_attributeElement);
    m_attributes.push_back(newAttr);
    return newAttr;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeExport* AttributeBlockXML::createBoolAttributeArray(
  const char*        name,
  const bool         values[],
  unsigned int       size)
{
  if (!hasAttribute(name))
  {
    AttributeExportXML* newAttr = new AttributeExportXML(name);
    newAttr->setAsBoolArray(values, size);
    m_xmlElement->LinkEndChild(newAttr->m_attributeElement);
    m_attributes.push_back(newAttr);
    return newAttr;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeExport* AttributeBlockXML::createUIntAttributeArray(
  const char*        name,
  const unsigned int values[],
  unsigned int       size)
{
  if (!hasAttribute(name))
  {
    AttributeExportXML* newAttr = new AttributeExportXML(name);
    newAttr->setAsUIntArray(values, size);
    m_xmlElement->LinkEndChild(newAttr->m_attributeElement);
    m_attributes.push_back(newAttr);
    return newAttr;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeExport* AttributeBlockXML::createFloatAttributeArray(
  const char*        name,
  const float        values[],
  unsigned int       size)
{
  if (!hasAttribute(name))
  {
    AttributeExportXML* newAttr = new AttributeExportXML(name);
    newAttr->setAsFloatArray(values, size);
    m_xmlElement->LinkEndChild(newAttr->m_attributeElement);
    m_attributes.push_back(newAttr);
    return newAttr;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeExport* AttributeBlockXML::createVector3AttributeArray(
  const char*        name,
  const NMP::Vector3 values[],
  unsigned int       size)
{
  if (!hasAttribute(name))
  {
    AttributeExportXML* newAttr = new AttributeExportXML(name);
    newAttr->setAsVector3Array(values, size);
    m_xmlElement->LinkEndChild(newAttr->m_attributeElement);
    m_attributes.push_back(newAttr);
    return newAttr;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeExport* AttributeBlockXML::createStringAttributeArray(
  const char*  name,
  const char*  values[],
  unsigned int size)
{
  if (!hasAttribute(name))
  {
    AttributeExportXML* newAttr = new AttributeExportXML(name);
    newAttr->setAsStringArray(values, size);
    m_xmlElement->LinkEndChild(newAttr->m_attributeElement);
    m_attributes.push_back(newAttr);
    return newAttr;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
// WrappedAttributeBlockXML
//----------------------------------------------------------------------------------------------------------------------
WrappedAttributeBlockXML::WrappedAttributeBlockXML(XMLElement* xmlElement) :
  m_xmlElement(xmlElement), m_xmlAttributeBlock(0)
{
  XMLElement* attributesBlockElement = xmlElement->findChild("Attributes");
  if (attributesBlockElement != 0)
  {
    m_xmlAttributeBlock = new AttributeBlockXML(attributesBlockElement);
  }
}

//----------------------------------------------------------------------------------------------------------------------
WrappedAttributeBlockXML::WrappedAttributeBlockXML(const char* elementName)
{
  m_xmlElement = new XMLElement(elementName);
  m_xmlAttributeBlock = new AttributeBlockXML();
  m_xmlElement->LinkEndChild(m_xmlAttributeBlock->getXMLElement());
}

//----------------------------------------------------------------------------------------------------------------------
WrappedAttributeBlockXML::~WrappedAttributeBlockXML()
{
  delete m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
const AttributeBlockExport* WrappedAttributeBlockXML::getAttributeBlock() const
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeBlockExport* WrappedAttributeBlockXML::getAttributeBlock()
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
// DataBlockExportXML
//----------------------------------------------------------------------------------------------------------------------
unsigned int DataBlockExportXML::Element::getIndex() const
{
  unsigned int val = 0;
  getUIntAttribute("index", val);
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
const char* DataBlockExportXML::Element::getType() const
{
  return getAttribute("type");
}

//----------------------------------------------------------------------------------------------------------------------
const char* DataBlockExportXML::Element::getDesc() const
{
  return getAttribute("description");
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int DataBlockExportXML::Element::getLength() const
{
  unsigned int val = 0; 
  if(!getUIntAttribute("bytes", val))
  {
    val = sizeof(unsigned int);
    const char * typeName = getType(); 
    if(strcmp(typeName, "bool") == 0)
    {
      val  = sizeof(bool); 
    }
    else if(strcmp(typeName, "char") == 0)
    {
      val = sizeof(char);
    }
    else if(strcmp(typeName, "wchar") == 0)
    {
      val = sizeof(wchar_t);
    }
    else if(strcmp(typeName, "double") == 0)
    {
      val = sizeof(double);
    }
  }
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
void DataBlockExportXML::Element::writeData(void* destBuffer) const
{
  char buffer[4];
  unsigned char* destBufferC = (unsigned char*)destBuffer;
  const char* data = getText();
  unsigned int bytes = getLength();

  for (unsigned int i = 0 ; i < bytes ; ++i)
  {
    char* dest = buffer;
    while (*data != ',')
    {
      *dest++ = *data++;
    }
    *dest = '\0';

    unsigned int val;
    sscanf(buffer, "%x", &val);

    ++data;
    *destBufferC++ = (unsigned char)val;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// DataBlockExportXML
//----------------------------------------------------------------------------------------------------------------------
DataBlockExportXML::DataBlockExportXML(XMLElement* element) :
  m_xmlElement(element)
{
}

//----------------------------------------------------------------------------------------------------------------------
DataBlockExportXML::DataBlockExportXML(const DataBlockExportXML& other)
{
  if (other.m_xmlElement)
  {
    m_xmlElement = new XMLElement(*other.m_xmlElement);
  }
  else
  {
    m_xmlElement = 0;
  }
}
//----------------------------------------------------------------------------------------------------------------------
DataBlockExportXML::DataBlockExportXML() :
  m_xmlElement(new XMLElement("DataBlock"))
{
}

//----------------------------------------------------------------------------------------------------------------------
bool DataBlockExportXML::readElement(
  char*       data_out,
  size_t      bufferSize,
  size_t&     length_out,
  const char* desc) const
{
  NMP_ASSERT(desc);
  const Element* child = static_cast<const Element*>(m_xmlElement->findChild("DataElement"));

  while (child != 0)
  {
    if (strcmp(child->getDesc(), desc) == 0)
    {
      return child->getBytes(data_out, bufferSize, length_out);
    }

    child = static_cast<const Element*>(m_xmlElement->nextChild("DataElement", child));
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool DataBlockExportXML::readElementVerifyType(
  char*       data_out,
  size_t      bufferSize,
  size_t&     length_out,
  const char* desc,
  const char* requiredType) const
{
  NMP_ASSERT(desc);
  const Element* child = static_cast<const Element*>(m_xmlElement->findChild("DataElement"));

  while (child != 0)
  {
    if (strcmp(child->getDesc(), desc) == 0)
    {
      if(strcmp(child->getType(), requiredType) == 0)
      {
        return child->getBytes(data_out, bufferSize, length_out);
      }
      return false;
    }
    child = static_cast<const Element*>(m_xmlElement->nextChild("DataElement", child));
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool DataBlockExportXML::hasElement(const char* desc) const
{
  NMP_ASSERT(desc);
  const Element* child = static_cast<const Element*>(m_xmlElement->findChild("DataElement"));

  while (child != 0)
  {
    if (strcmp(child->getDesc(), desc) == 0)
    {
      return true;
    }

    child = static_cast<const Element*>(m_xmlElement->nextChild("DataElement", child));
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------

bool DataBlockExportXML::hasElementVerifyType(const char* desc, const char* requiredType) const
{
  NMP_ASSERT(desc);
  const Element* child = static_cast<const Element*>(m_xmlElement->findChild("DataElement"));

  while (child != 0)
  {
    if (strcmp(child->getDesc(), desc) == 0)
    {
      if(strcmp(child->getType(), requiredType) == 0)
      {
        return true;
      }
      return false;
    }
    child = static_cast<const Element*>(m_xmlElement->nextChild("DataElement", child));
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool DataBlockExportXML::readElementIntAttrib(
  int*        data_out,
  const char* elementDesc,
  const char* attribDesc) const
{
  NMP_ASSERT(elementDesc);
  const Element* child = static_cast<const Element*>(m_xmlElement->findChild("DataElement"));

  while (child != 0)
  {
    if (strcmp(child->getDesc(), elementDesc) == 0)
    {
      int32_t result = child->QueryIntAttribute(attribDesc, data_out);
      if (result == NM::TIXML_SUCCESS)
        return true;
      return false;
    }

    child = static_cast<const Element*>(m_xmlElement->nextChild("DataElement", child));
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool DataBlockExportXML::readString(
  std::string& data_out,
  const char*  desc) const
{
  data_out = "";
  Element* child = (Element*)m_xmlElement->findChild("DataElement");
  while (child != 0)
  {
    if (strcmp(child->getDesc(), desc) == 0)
    {
      unsigned int iByteCount = child->getLength();
      char* buffer = new char[iByteCount+1];
      size_t dummy;
      child->getBytes(buffer, iByteCount, dummy);
      buffer[iByteCount] = '\0';
      data_out = buffer;
      delete[] buffer;
      break;
    }

    child = (Element*)m_xmlElement->nextChild("DataElement", child);
  }

  return data_out.size() > 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool DataBlockExportXML::readUnicodeString(
  std::wstring& data_out,
  const char*    desc) const
{
  data_out = L"";

  Element* child = (Element*)m_xmlElement->findChild("DataElement");
  while (child != 0)
  {
    if (strcmp(child->getDesc(), desc) == 0)
    {
      unsigned int iByteCount = child->getLength();

      size_t stringLength = iByteCount/sizeof(wchar_t);
      data_out.resize(stringLength);

      if(stringLength)
      {
        size_t dummy;
        child->getBytes((char*)&data_out.at(0), iByteCount, dummy);
}

      break;
    }

    child = (Element*)m_xmlElement->nextChild("DataElement", child);
  }

  return data_out.size() > 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool DataBlockExportXML::readChar(
  char&       data_out,
  const char* desc) const
{
  size_t numChars = sizeof(data_out);
  if (!readElement((char*)&data_out, numChars, numChars, desc))
    return false;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
size_t DataBlockExportXML::getByteCount(const char* desc) const
{
  NMP_ASSERT(desc);
  const Element* child = static_cast<const Element*>(m_xmlElement->findChild("DataElement"));

  while (child != 0)
  {
    if (strcmp(child->getDesc(), desc) == 0)
    {
      return child->getByteCount();
    }

    child = static_cast<const Element*>(m_xmlElement->nextChild("DataElement", child));
  }

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool DataBlockExportXML::readCharArray(
  char* data_out,
  size_t      buffer_size,
  size_t&     length_out,
  const char* desc) const
{
  size_t numChars = buffer_size * sizeof(*data_out);
  if (!readElement((char*)data_out, numChars, numChars, desc))
    return false;
  length_out = numChars / sizeof(*data_out);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DataBlockExportXML::readBool(
  bool&       data_out,
  const char* desc) const
{
  size_t numChars = sizeof(data_out);
  if (!readElement((char*)&data_out, numChars, numChars, desc))
    return false;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DataBlockExportXML::readBoolArray(
  bool*       data_out,
  size_t      buffer_size,
  size_t&     length_out,
  const char* desc) const
{
  size_t numChars = buffer_size * sizeof(*data_out);
  if (!readElement((char*)data_out, numChars, numChars, desc))
    return false;
  length_out = numChars / sizeof(*data_out);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DataBlockExportXML::readInt(
  int&        data_out,
  const char* desc) const
{
  size_t numChars = sizeof(data_out);
  if (!readElement((char*)&data_out, numChars, numChars, desc))
    return false;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DataBlockExportXML::readIntArray(
  int*        data_out,
  size_t      buffer_size,
  size_t&     length_out,
  const char* desc) const
{
  size_t numChars = buffer_size * sizeof(*data_out);
  if (!readElement((char*)data_out, numChars, numChars, desc))
    return false;
  length_out = numChars / sizeof(*data_out);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DataBlockExportXML::readUInt(
  unsigned int& data_out,
  const char*   desc) const
{
  size_t numChars = sizeof(data_out);
  if (!readElement((char*)&data_out, numChars, numChars, desc))
    return false;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DataBlockExportXML::readUIntArray(
  unsigned int* data_out,
  size_t        buffer_size,
  size_t&       length_out,
  const char*   desc) const
{
  size_t numChars = buffer_size * sizeof(*data_out);
  if (!readElement((char*)data_out, numChars, numChars, desc))
    return false;
  length_out = numChars / sizeof(*data_out);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DataBlockExportXML::readUInt64(
  uint64_t&     data_out,
  const char*   desc) const
{
  size_t numChars = sizeof(data_out);
  if (!readElement((char*)&data_out, numChars, numChars, desc))
    return false;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DataBlockExportXML::readUInt64Array(
  uint64_t*     data_out,
  size_t        buffer_size,
  size_t&       length_out,
  const char*   desc) const
{
  size_t numChars = buffer_size * sizeof(*data_out);
  if (!readElement((char*)data_out, numChars, numChars, desc))
    return false;
  length_out = numChars / sizeof(*data_out);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DataBlockExportXML::readFloat(
  float&      data_out,
  const char* desc) const
{
  size_t numChars = sizeof(data_out);
  if (!readElement((char*)&data_out, numChars, numChars, desc))
    return false;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DataBlockExportXML::readFloatArray(
  float*      data_out,
  size_t      buffer_size,
  size_t&     length_out,
  const char* desc) const
{
  size_t numChars = buffer_size * sizeof(*data_out);
  if (!readElement((char*)data_out, numChars, numChars, desc))
    return false;
  length_out = numChars / sizeof(*data_out);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DataBlockExportXML::readDouble(
  double&     data_out,
  const char* desc) const
{
  size_t numChars = sizeof(data_out);
  if (!readElement((char*)&data_out, numChars, numChars, desc))
    return false;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DataBlockExportXML::readDoubleArray(
  double*     data_out,
  size_t      buffer_size,
  size_t&     length_out,
  const char* desc) const
{
  size_t numChars = buffer_size * sizeof(*data_out);
  if (!readElement((char*)data_out, numChars, numChars, desc))
    return false;
  length_out = numChars / sizeof(*data_out);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DataBlockExportXML::readIntAttribute(
  int&        data_out,
  const char* elementDesc,
  const char* attribDesc) const
{
  if (!readElementIntAttrib(&data_out, elementDesc, attribDesc))
    return false;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DataBlockExportXML::readUndefined(const char* desc) const
{
  return hasElementVerifyType(desc, "Undefined");
}

//----------------------------------------------------------------------------------------------------------------------
bool DataBlockExportXML::readNetworkNodeId(int& data_out, const char* desc) const
{
  size_t numChars = sizeof(data_out);
  if (!readElementVerifyType((char*)&data_out, numChars, numChars, desc, "NetworkNodeId"))
    return false;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DataBlockExportXML::readVector3(const NMP::Vector3& data, const char* desc) const
{
  size_t bufferSize = 3 * sizeof(float);
  size_t length_out = 0;

  if (!readElement((char*)data.f, bufferSize, length_out, desc))
    return false;

  NMP_VERIFY(length_out == bufferSize);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DataBlockExportXML::readVector4(const NMP::Quat& data, const char* desc) const
{
  size_t bufferSize = 4 * sizeof(float);
  size_t length_out = 0;
  if (!readElement((char*)data.f, bufferSize, length_out, desc))
    return false;

  NMP_VERIFY(length_out == bufferSize);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
DataBlockExportXML::Element* DataBlockExportXML::addElement(
  const void* data,
  size_t      length,
  const char* type,
  const char* desc, 
  bool includeLengthAttribute)
{
  Element* el = new Element(data, length,type, desc, includeLengthAttribute);
  m_xmlElement->LinkEndChild(el);
  return el;
}

//----------------------------------------------------------------------------------------------------------------------
void DataBlockExportXML::writeString(const std::string& data, const char* desc)
{
  size_t length = data.size();
  addElement(data.c_str(), length, "char", desc, true);
}

//----------------------------------------------------------------------------------------------------------------------
void DataBlockExportXML::writeUnicodeString(const wchar_t* data, const char* desc)
{
  size_t length = wcslen(data);
  addElement(data, sizeof(wchar_t)*length, "wchar", desc, true);
}

//----------------------------------------------------------------------------------------------------------------------
void DataBlockExportXML::writeChar(char data, const char* desc)
{
  addElement(&data, 1, "char", desc, false);
}

//----------------------------------------------------------------------------------------------------------------------
void DataBlockExportXML::writeCharArray(const char* data, size_t length, const char* desc)
{
  addElement(data, length, "char", desc, true);
}

//----------------------------------------------------------------------------------------------------------------------
void DataBlockExportXML::writeBool(bool data, const char* desc)
{
  unsigned char value = (data) ? (0x1) : (0x0);
  addElement(&value, 1, "bool", desc, false);
}

//----------------------------------------------------------------------------------------------------------------------
void DataBlockExportXML::writeBoolArray(const bool* data, size_t length, const char* desc)
{
  addElement(data, length * sizeof(bool), "bool", desc, true);
}

//----------------------------------------------------------------------------------------------------------------------
void DataBlockExportXML::writeInt(int data, const char* desc)
{
  addElement(&data, sizeof(int), "int", desc, false);
}

//----------------------------------------------------------------------------------------------------------------------
void DataBlockExportXML::writeNetworkNodeId(int nodeId, const char* desc)
{
  addElement(&nodeId, sizeof(int), "NetworkNodeId", desc, false);
}

//----------------------------------------------------------------------------------------------------------------------
void DataBlockExportXML::writeMessageId(int messageId, const char* desc)
{
  addElement(&messageId, sizeof(int), "MessageId", desc, false);
}
//----------------------------------------------------------------------------------------------------------------------
void DataBlockExportXML::writeNetworkNodeIdWithPinIndex(int nodeId, int pinIndex, const char * desc)
{
  Element *element = addElement(&nodeId, sizeof(int), "NetworkNodeId", desc, false);
  element->SetAttribute("pinIndex", pinIndex);
}

//----------------------------------------------------------------------------------------------------------------------
void DataBlockExportXML::writeAnimationId(int animId, const char* desc)
{
  addElement(&animId, sizeof(int), "AnimationId", desc, false);
}

//----------------------------------------------------------------------------------------------------------------------
void DataBlockExportXML::writeIntArray(const int* data, size_t length, const char* desc)
{
  addElement(data, length * sizeof(int), "int", desc, true);
}

//----------------------------------------------------------------------------------------------------------------------
void DataBlockExportXML::writeUInt(unsigned int data, const char* desc)
{
  addElement(&data, sizeof(unsigned int), "uint", desc, false);
}

//----------------------------------------------------------------------------------------------------------------------
void DataBlockExportXML::writeUIntArray(const unsigned int* data, size_t length, const char* desc)
{
  addElement(data, length * sizeof(unsigned int), "uint", desc, true);
}

//----------------------------------------------------------------------------------------------------------------------
void DataBlockExportXML::writeFloat(float data, const char* desc)
{
  addElement(&data, sizeof(float), "float", desc, false);
}

//----------------------------------------------------------------------------------------------------------------------
void DataBlockExportXML::writeFloatArray(const float* data, size_t length, const char* desc)
{
  addElement(data, length * sizeof(float), "float", desc, true);
}

//----------------------------------------------------------------------------------------------------------------------
void DataBlockExportXML::writeDouble(double data, const char* desc)
{
  addElement(&data, sizeof(double), "double", desc, false);
}

//----------------------------------------------------------------------------------------------------------------------
void DataBlockExportXML::writeDoubleArray(const double* data, size_t length, const char* desc)
{
  addElement(data, length * sizeof(double), "double", desc, true);
}

//----------------------------------------------------------------------------------------------------------------------
void DataBlockExportXML::writeUndefined(const char* desc)
{
  addElement(0, 0, "Undefined", desc, false);
}

//----------------------------------------------------------------------------------------------------------------------
void DataBlockExportXML::writeVector3(const NMP::Vector3& data, const char* desc)
{
  addElement(data.f, 3 * sizeof(float), "vector3", desc, true);
}

//----------------------------------------------------------------------------------------------------------------------
void DataBlockExportXML::writeVector4(const NMP::Quat& data, const char* desc)
{
  addElement(data.f, 4 * sizeof(float), "vector4", desc, true);
}

//----------------------------------------------------------------------------------------------------------------------
void DataBlockExportXML::padToAlignment(unsigned int alignmentValue)
{
  unsigned int currentLength = getDataBlockSize();

  unsigned int bytesToAdd = ((currentLength + (alignmentValue - 1)) & (~(alignmentValue - 1)))
    - currentLength;
  unsigned char pad = 0;

  while (bytesToAdd)
  {
    addElement(&pad, 1u, "char", "padding", false);
    --bytesToAdd;
  }
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int DataBlockExportXML::getDataBlockSize() const
{
  unsigned int blockSize = 0;
  unsigned int numElements = m_xmlElement->getChildCount("DataElement");

  // This is slow, but can be sped up
  for (unsigned int i = 0 ; i < numElements ; ++i)
  {
    const Element* dataEl = (const Element*)m_xmlElement->findChild("DataElement", i);
    blockSize += dataEl->getLength();
  }

  return blockSize;
}

//----------------------------------------------------------------------------------------------------------------------
void DataBlockExportXML::writeDataBlock(void* dest) const
{
  char* destBuffer = (char*)dest;
  unsigned int numElements = m_xmlElement->getChildCount("DataElement");

  // This is slow, but can be sped up
  for (unsigned int i = 0 ; i < numElements ; ++i)
  {
    const Element* dataEl = (const Element*)m_xmlElement->findChildWithUIntAttribute("DataElement", "index", i);
    unsigned int elLength = dataEl->getLength();

    dataEl->writeData(destBuffer);
    destBuffer += elLength;
  }
}

//----------------------------------------------------------------------------------------------------------------------
DataBlockExportXML::~DataBlockExportXML()
{
}

//----------------------------------------------------------------------------------------------------------------------
// AnalysisNodeExportXML
//----------------------------------------------------------------------------------------------------------------------
AnalysisNodeExportXML::AnalysisNodeExportXML(XMLElement* element) :
  m_xmlElement(element)
{
  m_dataBlock = new DataBlockExportXML(m_xmlElement->findChild("DataBlock"));
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisNodeExportXML::AnalysisNodeExportXML(const AnalysisNodeExportXML& other)
{
  if (other.m_dataBlock)
  {
    m_dataBlock = new DataBlockExportXML(*other.m_dataBlock);
  }
  else
  {
    m_dataBlock = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisNodeExportXML::AnalysisNodeExportXML(const char*  name, const char*  typeName) :
m_xmlElement(new XMLElement("AnalysisNode"))
{
  m_dataBlock = new DataBlockExportXML();
  m_xmlElement->LinkEndChild(m_dataBlock->m_xmlElement);
  m_xmlElement->setAttribute("name", name);
  m_xmlElement->setAttribute("typeName", typeName);
}
//----------------------------------------------------------------------------------------------------------------------
// should only every be called by derived classes
AnalysisNodeExportXML::AnalysisNodeExportXML()
{
}

//----------------------------------------------------------------------------------------------------------------------
const char* AnalysisNodeExportXML::getName() const
{
  return m_xmlElement->getAttribute("name");
}

//----------------------------------------------------------------------------------------------------------------------
const char* AnalysisNodeExportXML::getTypeName() const
{
  return m_xmlElement->getAttribute("typeName");
}

//----------------------------------------------------------------------------------------------------------------------
DataBlockExport* AnalysisNodeExportXML::getDataBlock()
{
  return m_dataBlock;
}

//----------------------------------------------------------------------------------------------------------------------
XMLElement* AnalysisNodeExportXML::getDataBlockXMLElement() const
{
  return m_dataBlock->m_xmlElement;
}

//----------------------------------------------------------------------------------------------------------------------
const DataBlockExport* AnalysisNodeExportXML::getDataBlock() const
{
  return m_dataBlock;
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisNodeExportXML::~AnalysisNodeExportXML()
{
  delete m_dataBlock;
}


//----------------------------------------------------------------------------------------------------------------------
// AnalysisTaskExportXML
//----------------------------------------------------------------------------------------------------------------------
AnalysisTaskExportXML::AnalysisTaskExportXML(const char* name, const char* analysisType) :
m_xmlElement(new XMLElement("AnalysisTask"))
{
  m_dataBlock = new DataBlockExportXML();
  m_xmlElement->LinkEndChild(m_dataBlock->m_xmlElement);
  m_xmlElement->setAttribute("name", name);
  m_xmlElement->setAttribute("analysisType", analysisType);
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisTaskExportXML::AnalysisTaskExportXML(XMLElement* element) :
m_xmlElement(element)
{
  m_dataBlock = new DataBlockExportXML(m_xmlElement->findChild("DataBlock"));
  BuildNodeList(AnalysisNodeExportXML, m_analysisNodes, m_xmlElement, "AnalysisNode");
}

//----------------------------------------------------------------------------------------------------------------------
const char* AnalysisTaskExportXML::getName() const
{
  return m_xmlElement->getAttribute("name");
}

//----------------------------------------------------------------------------------------------------------------------
const char* AnalysisTaskExportXML::getAnalysisType() const
{
  return m_xmlElement->getAttribute("analysisType");
}

//----------------------------------------------------------------------------------------------------------------------
DataBlockExport* AnalysisTaskExportXML::getDataBlock()
{
  return m_dataBlock;
}

//----------------------------------------------------------------------------------------------------------------------
XMLElement* AnalysisTaskExportXML::getDataBlockXMLElement() const
{
  return m_dataBlock->m_xmlElement;
}

//----------------------------------------------------------------------------------------------------------------------
const DataBlockExport* AnalysisTaskExportXML::getDataBlock() const
{
  return m_dataBlock;
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisNodeExport* AnalysisTaskExportXML::createAnalysisNode(const char* name, const char* typeName)
{
  AnalysisNodeExportXML* stat = new AnalysisNodeExportXML(name, typeName);
  m_xmlElement->LinkEndChild(stat->m_xmlElement);

  m_analysisNodes.push_back(stat);

  return stat;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int AnalysisTaskExportXML::getNumAnalysisNodes() const
{
  return (unsigned int)m_analysisNodes.size();
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisNodeExport* AnalysisTaskExportXML::getAnalysisNode(unsigned int index)
{
  return m_analysisNodes[index];
}

//----------------------------------------------------------------------------------------------------------------------
const AnalysisNodeExport* AnalysisTaskExportXML::getAnalysisNode(unsigned int index) const
{
  return m_analysisNodes[index];
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisTaskExportXML::addUsedAnimSetsToTask(const std::vector<int32_t>& usedAnimSetIndices)
{
  int32_t usedAnimSetCount = (int32_t)usedAnimSetIndices.size();
  const int32_t* ptr = NULL;

  if (usedAnimSetIndices.size() > 0)
  {
    ptr = &usedAnimSetIndices[0];
  }

  m_dataBlock->writeIntArray(ptr, usedAnimSetCount, "UsedAnimSets");
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisTaskExportXML::~AnalysisTaskExportXML()
{
  delete m_dataBlock;

  for (size_t i = 0, s = m_analysisNodes.size(); i < s; ++i)
  {
    if (m_analysisNodes[i])
    {
      delete m_analysisNodes[i];
      m_analysisNodes[i] = 0;
    }
  }
}


//----------------------------------------------------------------------------------------------------------------------
// AnalysisLibraryExportXML
//----------------------------------------------------------------------------------------------------------------------
AnalysisLibraryExportXML::AnalysisLibraryExportXML(GUID guid, const wchar_t* destFilename, const wchar_t* resultFilename) :
  m_xmlElement(new XMLElement("AnalysisLibrary")),
  m_destinationFilename(toUTF8(destFilename))
{
  m_xmlElement->setAttribute("resultFilename", resultFilename);
  m_xmlElement->setGUID(guid);
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisLibraryExportXML::AnalysisLibraryExportXML(XMLElement* element, const std::string& destFilename) :
  m_xmlElement(element),
  m_destinationFilename(destFilename)
{
  BuildNodeList(AnalysisTaskExportXML, m_tasks, m_xmlElement, "AnalysisTask");
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisLibraryExportXML::~AnalysisLibraryExportXML()
{
  for (size_t i = 0, s = m_tasks.size(); i < s; ++i)
  {
    if (m_tasks[i])
    {
      delete m_tasks[i];
      m_tasks[i] = 0;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool AnalysisLibraryExportXML::write()
{
  return m_xmlElement->saveFile(m_destinationFilename.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
const char* AnalysisLibraryExportXML::getName() const
{
  return "AnalysisLibrary";
}

//----------------------------------------------------------------------------------------------------------------------
ME::GUID AnalysisLibraryExportXML::getGUID() const
{
  return m_xmlElement->getGUID();
}

//----------------------------------------------------------------------------------------------------------------------
const char* AnalysisLibraryExportXML::getDestFilename() const
{
  return m_destinationFilename.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
const char* AnalysisLibraryExportXML::getResultFilename() const
{
  return m_xmlElement->getAttribute("resultFilename");
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisResultLibraryExport* AnalysisLibraryExportXML::createResultLibrary()
{
  return new AnalysisResultLibraryExportXML(getGUID(), getResultFilename());
}

void AnalysisLibraryExportXML::destroyResultLibrary(AnalysisResultLibraryExport* library)
{
  delete library;
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisTaskExport* AnalysisLibraryExportXML::createAnalysisTask(const char* name, const char* analysisType)
{
  AnalysisTaskExportXML* task = new AnalysisTaskExportXML(name, analysisType);
  m_xmlElement->LinkEndChild(task->m_xmlElement);

  m_tasks.push_back(task);

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int AnalysisLibraryExportXML::getNumAnalysisTasks() const
{
  return (unsigned int)m_tasks.size();
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisTaskExport* AnalysisLibraryExportXML::getAnalysisTask(unsigned int index)
{
  return m_tasks[index];
}

//----------------------------------------------------------------------------------------------------------------------
const AnalysisTaskExport* AnalysisLibraryExportXML::getAnalysisTask(unsigned int index) const
{
  return m_tasks[index];
}


//----------------------------------------------------------------------------------------------------------------------
// AnalysisResultLibraryExportXML
//----------------------------------------------------------------------------------------------------------------------
AnalysisResultLibraryExportXML::AnalysisResultLibraryExportXML(GUID guid, const char* destFilename) :
m_xmlElement(new XMLElement("AnalysisResultLibrary")),
  m_destinationFilename(destFilename)
{
  m_xmlElement->setGUID(guid);
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisResultLibraryExportXML::AnalysisResultLibraryExportXML(XMLElement* element, const std::string& destFilename) :
  m_xmlElement(element),
  m_destinationFilename(destFilename)
{
  BuildNodeList(AnalysisNodeExportXML, m_results, m_xmlElement, "AnalysisNode");
}

//----------------------------------------------------------------------------------------------------------------------
const char* AnalysisResultLibraryExportXML::getName() const
{
  return "AnalysisResultLibrary";
}

//----------------------------------------------------------------------------------------------------------------------
ME::GUID AnalysisResultLibraryExportXML::getGUID() const
{
  return m_xmlElement->getGUID();
}

//----------------------------------------------------------------------------------------------------------------------
const char* AnalysisResultLibraryExportXML::getDestFilename() const
{
  return m_destinationFilename.c_str();
}


//----------------------------------------------------------------------------------------------------------------------
AnalysisResultLibraryExportXML::~AnalysisResultLibraryExportXML()
{
  delete m_xmlElement;
  size_t size = m_results.size();
  for (size_t i = 0 ; i < size ; i++)
  {
    delete m_results[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool AnalysisResultLibraryExportXML::write()
{
  return m_xmlElement->saveFile(m_destinationFilename.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisNodeExport* AnalysisResultLibraryExportXML::createTaskResult(const char* name)
{
  AnalysisNodeExportXML* stat = new AnalysisNodeExportXML(name, "");
  m_xmlElement->LinkEndChild(stat->m_xmlElement);

  m_results.push_back(stat);

  return stat;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int AnalysisResultLibraryExportXML::getNumTaskResult() const
{
  return (unsigned int)m_results.size();
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisNodeExport* AnalysisResultLibraryExportXML::getTaskResult(unsigned int index)
{
  return m_results[index];
}

//----------------------------------------------------------------------------------------------------------------------
const AnalysisNodeExport* AnalysisResultLibraryExportXML::getTaskResult(unsigned int index) const
{
  return m_results[index];
}

//----------------------------------------------------------------------------------------------------------------------
// ConditionExportXML
//----------------------------------------------------------------------------------------------------------------------
ConditionExportXML::ConditionExportXML(XMLElement* element) :
  m_xmlElement(element)
{
  m_dataBlock = new DataBlockExportXML(m_xmlElement->findChild("DataBlock"));
}

//----------------------------------------------------------------------------------------------------------------------

ConditionExportXML::ConditionExportXML(const ConditionExportXML& other)
{
  if (other.m_dataBlock)
  {
    m_dataBlock = new DataBlockExportXML(*other.m_dataBlock);
  }
  else
  {
    m_dataBlock = 0;
  }
}
//----------------------------------------------------------------------------------------------------------------------
ConditionExportXML::ConditionExportXML(unsigned int index, unsigned int typeID) :
  m_xmlElement(new XMLElement("Condition"))
{
  m_dataBlock = new DataBlockExportXML();
  m_xmlElement->LinkEndChild(m_dataBlock->m_xmlElement);

  m_xmlElement->setUIntAttribute("index", index);
  m_xmlElement->setUIntAttribute("typeID", typeID);
}
//----------------------------------------------------------------------------------------------------------------------
// should only every be called by derived classes
ConditionExportXML::ConditionExportXML()
{
}

//----------------------------------------------------------------------------------------------------------------------
const char* ConditionExportXML::getName() const
{
  return m_xmlElement->getAttribute("name");
}

//----------------------------------------------------------------------------------------------------------------------
DataBlockExport* ConditionExportXML::getDataBlock()
{
  return m_dataBlock;
}

//----------------------------------------------------------------------------------------------------------------------
XMLElement* ConditionExportXML::getDataBlockXMLElement() const
{
  return m_dataBlock->m_xmlElement;
}

//----------------------------------------------------------------------------------------------------------------------
const DataBlockExport* ConditionExportXML::getDataBlock() const
{
  return m_dataBlock;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int ConditionExportXML::getIndex() const
{
  unsigned int val;

  if (!m_xmlElement->getUIntAttribute("index", val))
  {
    NMP_THROW_ERROR("No index on ConditionExport");
  }
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int ConditionExportXML::getTypeID() const
{
  unsigned int val;
  m_xmlElement->getUIntAttribute("typeID", val);
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
ConditionExportXML::~ConditionExportXML()
{
  delete m_dataBlock;
}

//----------------------------------------------------------------------------------------------------------------------
// ConditionSetExportXML
//----------------------------------------------------------------------------------------------------------------------
ConditionSetExportXML::ConditionSetExportXML(XMLElement* element) :
  m_xmlElement(element)
{
}

//----------------------------------------------------------------------------------------------------------------------
ConditionSetExportXML::ConditionSetExportXML(
  unsigned int index,
  unsigned int targetNodeID,
  const std::vector<unsigned int>& indices) :
  m_xmlElement(new XMLElement("ConditionSet"))
{
  m_xmlElement->setUIntAttribute("index", index);
  m_xmlElement->setUIntAttribute("targetNodeID", targetNodeID);

  setConditionIndices(indices);
}

//----------------------------------------------------------------------------------------------------------------------
void ConditionSetExportXML::setConditionIndices(const std::vector<unsigned int>& indices)
{
  std::string str;
  char buf[16];
  for (unsigned int i = 0 ; i < indices.size() ; ++i)
  {
    sprintf(buf, "%d, ", indices[i]);
    str.append(buf);
  }
  m_xmlElement->setChildText("conditionIndices", str.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int ConditionSetExportXML::getIndex() const
{
  unsigned int val = 0xffffffff;
  m_xmlElement->getUIntAttribute("index", val);
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int ConditionSetExportXML::getTargetNodeID() const
{
  unsigned int val = 0xffffffff;
  m_xmlElement->getUIntAttribute("targetNodeID", val);
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
std::vector<unsigned int> ConditionSetExportXML::getConditionIndices() const
{
  std::vector<unsigned int> vec;
  char buffer[16];

  const char* data = m_xmlElement->getChildText("conditionIndices");

  for (;;)
  {
    char* dest = buffer;
    while (*data == ' ')   // Skip white space.
    {
      data++;
    }
    while (*data != ',')   // Extract number string into buffer.
    {
      *dest++ = *data++;
    }
    *dest = '\0';

    unsigned int val;
    sscanf(buffer, "%d", &val); // Convert string to value.

    vec.push_back(val);         // Add to out put vector.

    ++data;
    if (data[0] == 0 || data[1] == 0)   // End if reached a NULL char in xml data.
    {
      break;
    }
  }

  return vec;
}

//----------------------------------------------------------------------------------------------------------------------
ConditionSetExportXML::~ConditionSetExportXML()
{
}

//----------------------------------------------------------------------------------------------------------------------
// NodeExportXML
//----------------------------------------------------------------------------------------------------------------------
NodeExportXML::NodeExportXML(XMLElement* element) :
  m_xmlElement(element)
{
  mcExport_MSG("Node '%s'\n", getName());
  m_dataBlock = new DataBlockExportXML(element->findChild("DataBlock"));
  mcExport_MSG("CONd\n");
  BuildIndexedNodeList(ConditionExportXML, m_conditions, m_xmlElement, "Condition");
  mcExport_MSG("Stats\n");
  BuildNodeList(AnalysisNodeExportXML, m_statistics, m_xmlElement, "AnalysisNode");
  mcExport_MSG("Condset.\n");
  BuildIndexedNodeList(ConditionSetExportXML, m_conditionSets, m_xmlElement, "ConditionSet");
  mcExport_MSG("CommonCondition.\n");
  BuildIndexedNodeList(ConditionExportXML, m_commonConditions, m_xmlElement, "CommonCondition");
  mcExport_MSG("CommonConditionSet.\n");
  BuildIndexedNodeList(ConditionSetExportXML, m_commonConditionSets, m_xmlElement, "CommonConditionSet");
}

//----------------------------------------------------------------------------------------------------------------------
NodeExportXML::NodeExportXML(
  unsigned int   networkID,
  unsigned int   typeID,
  unsigned int   downstreamParentID,
  bool           downstreamParentMultiplyConnected,
  const wchar_t* name,
  bool           persistent) :
  m_xmlElement(new XMLElement("Node"))
{
  m_dataBlock = new DataBlockExportXML();
  m_xmlElement->LinkEndChild(m_dataBlock->m_xmlElement);

  m_xmlElement->setAttribute("name", name);
  m_xmlElement->setUIntAttribute("networkID", networkID);
  m_xmlElement->setUIntAttribute("downstreamParentID", downstreamParentID);
  m_xmlElement->setUIntAttribute("typeID", typeID);
  if (persistent)
  {
    m_xmlElement->setAttribute("persistent", "true");
  }
  if (downstreamParentMultiplyConnected)
  {
    m_xmlElement->setAttribute("downstreamMultiplyConnected", "true");
  }
}

//----------------------------------------------------------------------------------------------------------------------
NodeExportXML::NodeExportXML(
  unsigned int networkID,
  unsigned int typeID,
  unsigned int downstreamParentID,
  bool         downstreamParentMultiplyConnected,
  const char*  name,
  bool         persistent) :
  m_xmlElement(new XMLElement("Node"))
{
  m_dataBlock = new DataBlockExportXML();
  m_xmlElement->LinkEndChild(m_dataBlock->m_xmlElement);

  m_xmlElement->setAttribute("name", name);
  m_xmlElement->setUIntAttribute("networkID", networkID);
  m_xmlElement->setUIntAttribute("downstreamParentID", downstreamParentID);
  m_xmlElement->setUIntAttribute("typeID", typeID);
  if (persistent)
  {
    m_xmlElement->setAttribute("persistent", "true");
  }
  if (downstreamParentMultiplyConnected)
  {
    m_xmlElement->setAttribute("downstreamMultiplyConnected", "true");
  }
}

//----------------------------------------------------------------------------------------------------------------------
NodeExportXML::NodeExportXML()
{

}

//----------------------------------------------------------------------------------------------------------------------
ConditionExport* NodeExportXML::createCondition(unsigned int index, unsigned int typeID)
{
  ConditionExportXML* condition = new ConditionExportXML(index, typeID);

  m_xmlElement->LinkEndChild(condition->m_xmlElement);
  InsertIntoIndexedList(m_conditions, index, condition);

  return condition;
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisNodeExport* NodeExportXML::createAnalysisNode(const char* name, const char * typeName)
{
  AnalysisNodeExportXML* newStat = new AnalysisNodeExportXML(name, typeName);

  m_xmlElement->LinkEndChild(newStat->m_xmlElement);

  m_statistics.push_back(newStat);
  return newStat;
}

//----------------------------------------------------------------------------------------------------------------------
ConditionSetExport* NodeExportXML::createConditionSet(
  unsigned int index,
  unsigned int targetNodeID,
  const std::vector<unsigned int>& indices)
{
  ConditionSetExportXML* condSet = new ConditionSetExportXML(index,
      targetNodeID,
      indices);
  m_xmlElement->LinkEndChild(condSet->m_xmlElement);
  InsertIntoIndexedList(m_conditionSets, index, condSet);

  return condSet;
}

//----------------------------------------------------------------------------------------------------------------------
ConditionExport* NodeExportXML::createCommonCondition(unsigned int index, unsigned int typeID)
{
  ConditionExportXML* condition = new ConditionExportXML(index, typeID);
  condition->m_xmlElement->SetValue("CommonCondition");

  m_xmlElement->LinkEndChild(condition->m_xmlElement);
  InsertIntoIndexedList(m_commonConditions, index, condition);

  return condition;
}

//----------------------------------------------------------------------------------------------------------------------
ConditionSetExport* NodeExportXML::createCommonConditionSet(
  unsigned int index,
  unsigned int targetNodeID,
  const std::vector<unsigned int>& indices)
{
  ConditionSetExportXML* conditionSet = new ConditionSetExportXML(index, targetNodeID, indices);
  conditionSet->m_xmlElement->SetValue("CommonConditionSet");

  m_xmlElement->LinkEndChild(conditionSet->m_xmlElement);
  InsertIntoIndexedList(m_commonConditionSets, index, conditionSet);

  return conditionSet;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int NodeExportXML::getIndex() const
{
  unsigned int val;
  m_xmlElement->getUIntAttribute("networkID", val);
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int NodeExportXML::getNodeID() const
{
  unsigned int val;
  m_xmlElement->getUIntAttribute("networkID", val);
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int NodeExportXML::getDownstreamParentID() const
{
  unsigned int val;
  m_xmlElement->getUIntAttribute("downstreamParentID", val);
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int NodeExportXML::getTypeID() const
{
  unsigned int val;
  m_xmlElement->getUIntAttribute("typeID", val);
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
const char* NodeExportXML::getName() const
{
  return m_xmlElement->getAttribute("name");
}

//----------------------------------------------------------------------------------------------------------------------
bool NodeExportXML::isPersistent() const
{
  const char* persistentChar = m_xmlElement->getAttribute("persistent");

  if (persistentChar == 0) return false;

  std::string persistentStr(persistentChar);

  if (persistentStr == std::string("true"))
  {
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool NodeExportXML::isDownstreamParentMultiplyConnected() const
{
  const char* dstreamMultiplyConnectedChar = m_xmlElement->getAttribute("downstreamMultiplyConnected");

  if (dstreamMultiplyConnectedChar == 0) return false;

  std::string dstreamMultiplyConnectedStr(dstreamMultiplyConnectedChar);

  if (dstreamMultiplyConnectedStr == std::string("true"))
  {
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
DataBlockExport* NodeExportXML::getDataBlock()
{
  return m_dataBlock;
}

//----------------------------------------------------------------------------------------------------------------------
const DataBlockExport* NodeExportXML::getDataBlock() const
{
  return m_dataBlock;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int NodeExportXML::getNumConditions() const
{
  return (unsigned int)m_conditions.size();
}

//----------------------------------------------------------------------------------------------------------------------
ConditionExport* NodeExportXML::getCondition(unsigned int index)
{
  return m_conditions[index];
}

//----------------------------------------------------------------------------------------------------------------------
const ConditionExport* NodeExportXML::getCondition(unsigned int index) const
{
  return m_conditions[index];
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int NodeExportXML::getNumAnalysisNodes() const
{
  return (unsigned int)m_statistics.size();
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisNodeExport* NodeExportXML::getAnalysisNode(unsigned int index)
{
  return m_statistics[index];
}

//----------------------------------------------------------------------------------------------------------------------
const AnalysisNodeExport* NodeExportXML::getAnalysisNode(unsigned int index) const
{
  return m_statistics[index];
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int NodeExportXML::getNumConditionSets() const
{
  return (unsigned int)m_conditionSets.size();
}

//----------------------------------------------------------------------------------------------------------------------
ConditionSetExport* NodeExportXML::getConditionSet(unsigned int index)
{
  return m_conditionSets[index];
}

//----------------------------------------------------------------------------------------------------------------------
const ConditionSetExport* NodeExportXML::getConditionSet(unsigned int index) const
{
  return m_conditionSets[index];
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int NodeExportXML::getNumCommonConditions() const
{
  return (unsigned int)m_commonConditions.size();
}

//----------------------------------------------------------------------------------------------------------------------
ConditionExport* NodeExportXML::getCommonCondition(unsigned int index)
{
  return m_commonConditions[index];
}

//----------------------------------------------------------------------------------------------------------------------
const ConditionExport* NodeExportXML::getCommonCondition(unsigned int index) const
{
  return m_commonConditions[index];
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int NodeExportXML::getNumCommonConditionSets() const
{
  return (unsigned int)m_commonConditionSets.size();
}

//----------------------------------------------------------------------------------------------------------------------
ConditionSetExport* NodeExportXML::getCommonConditionSet(unsigned int index)
{
  return m_commonConditionSets[index];
}

//----------------------------------------------------------------------------------------------------------------------
const ConditionSetExport* NodeExportXML::getCommonConditionSet(unsigned int index) const
{
  return m_commonConditionSets[index];
}

//----------------------------------------------------------------------------------------------------------------------
XMLElement* NodeExportXML::getDataBlockXMLElement() const
{
  return m_dataBlock->m_xmlElement;
}

//----------------------------------------------------------------------------------------------------------------------
NodeExportXML::~NodeExportXML()
{
  // m_conditions
  uint32_t numConditions = (uint32_t)m_conditions.size();
  for (uint32_t i = 0 ; i < numConditions ; ++i)
  {
    delete m_conditions[i];
  }

  // m_statistics
  uint32_t numStats = (uint32_t)m_statistics.size();
  for (uint32_t i = 0 ; i < numStats ; ++i)
  {
    delete m_statistics[i];
  }

  // m_conditionSets
  uint32_t numConditionSets = (uint32_t)m_conditionSets.size();
  for (uint32_t i = 0 ; i < numConditionSets ; ++i)
  {
    delete m_conditionSets[i];
  }

  // m_commonConditions
  uint32_t numCommonConditions = (uint32_t)m_commonConditions.size();
  for (uint32_t i = 0 ; i < numCommonConditions ; ++i)
  {
    delete m_commonConditions[i];
  }

  // m_commonConditions
  uint32_t numCommonConditionSets = (uint32_t)m_commonConditionSets.size();
  for (uint32_t i = 0 ; i < numCommonConditionSets ; ++i)
  {
    delete m_commonConditionSets[i];
  }

//   delete m_xmlElement;
  delete m_dataBlock;
}

//----------------------------------------------------------------------------------------------------------------------
// MessageExportXML
//----------------------------------------------------------------------------------------------------------------------
const char* MessageExportXML::getName() const
{
  const char* name = m_xmlElement->getAttribute("name");
  return name;
}

//----------------------------------------------------------------------------------------------------------------------
MR::MessageType MessageExportXML::getMessageType() const
{
  unsigned int messageTypeID;
  m_xmlElement->getUIntAttribute("messageTypeID", messageTypeID);
  return static_cast<MR::MessageType>(messageTypeID);
}

//----------------------------------------------------------------------------------------------------------------------
MR::MessageID MessageExportXML::getMessageID() const
{
  unsigned int messageID;
  m_xmlElement->getUIntAttribute("messageID", messageID);
  return static_cast<MR::MessageID>(messageID);
}

//----------------------------------------------------------------------------------------------------------------------
MessageExportXML::MessageExportXML(XMLElement* element)
: m_xmlElement(element)
{
}

//----------------------------------------------------------------------------------------------------------------------
MessageExportXML::MessageExportXML(
  const wchar_t*  messageName,
  MR::MessageType messageTypeID,
  MR::MessageID   messageID)
: m_xmlElement(new XMLElement("Message"))
{
  m_xmlElement->setAttribute("name", messageName);
  m_xmlElement->setUIntAttribute("messageTypeID", static_cast<unsigned int>(messageTypeID));
  m_xmlElement->setUIntAttribute("messageID", static_cast<unsigned int>(messageID));
}

//----------------------------------------------------------------------------------------------------------------------
// EventExportXML
//----------------------------------------------------------------------------------------------------------------------
EventBaseExportXML::EventBaseExportXML(XMLElement* xmlElement) :
  m_xmlElement(xmlElement), m_xmlAttributeBlock(0)
{
  XMLElement* attributesBlockElement = xmlElement->findChild("Attributes");
  if (attributesBlockElement != 0)
  {
    m_xmlAttributeBlock = new AttributeBlockXML(attributesBlockElement);
  }
}

//----------------------------------------------------------------------------------------------------------------------
EventBaseExportXML::EventBaseExportXML(const char* elementName, unsigned int index, unsigned int userData) :
  m_xmlElement(0)
{
  m_xmlElement = new XMLElement(elementName);
  m_xmlElement->setUIntAttribute("index", index);
  m_xmlAttributeBlock = new AttributeBlockXML();
  m_xmlElement->LinkEndChild(m_xmlAttributeBlock->getXMLElement());
  m_xmlAttributeBlock->createUIntAttribute("userData", userData);
}

//----------------------------------------------------------------------------------------------------------------------
EventBaseExportXML::~EventBaseExportXML()
{
  delete m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
const AttributeBlockExport* EventBaseExportXML::getAttributeBlock() const
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeBlockExport* EventBaseExportXML::getAttributeBlock()
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int EventBaseExportXML::getIndex() const
{
  unsigned int result = m_xmlElement->getUIntAttribute("index", result);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// DiscreteEventExportXML
//----------------------------------------------------------------------------------------------------------------------
DiscreteEventExportXML::DiscreteEventExportXML(XMLElement* xmlElement) : EventBaseExportXML(xmlElement)
{
}

//----------------------------------------------------------------------------------------------------------------------
DiscreteEventExportXML::DiscreteEventExportXML(unsigned int index, unsigned int userData, float normalisedTime) :
  EventBaseExportXML("DiscreteEvent", index, userData)
{
  if (getAttributeBlock() != 0)
  {
    getAttributeBlock()->createDoubleAttribute("startTime", normalisedTime);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// DurationEventExportXML
//----------------------------------------------------------------------------------------------------------------------
DurationEventExportXML::DurationEventExportXML(XMLElement* xmlElement): EventBaseExportXML(xmlElement)
{

}

//----------------------------------------------------------------------------------------------------------------------
DurationEventExportXML::DurationEventExportXML(unsigned int index, unsigned int userData, float normalisedTime,
    float normalisedDuration) :
  EventBaseExportXML("DurationEvent", index, userData)
{
  if (getAttributeBlock() != 0)
  {
    getAttributeBlock()->createDoubleAttribute("startTime", normalisedTime);
    getAttributeBlock()->createDoubleAttribute("duration", normalisedDuration);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// CurveEventExportXML
//----------------------------------------------------------------------------------------------------------------------
CurveEventExportXML::CurveEventExportXML(XMLElement* xmlElement) : EventBaseExportXML(xmlElement)
{
}

//----------------------------------------------------------------------------------------------------------------------
CurveEventExportXML::CurveEventExportXML(unsigned int index, unsigned int userData, float normalisedTime, float floatValue) :
  EventBaseExportXML("CurveEvent", index, userData)
{
  if (getAttributeBlock() != 0)
  {
    getAttributeBlock()->createDoubleAttribute("startTime", normalisedTime);
    getAttributeBlock()->createDoubleAttribute("floatVal", floatValue);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// DiscreteEventTrackExportXML
//----------------------------------------------------------------------------------------------------------------------

DiscreteEventTrackExportXML::DiscreteEventTrackExportXML(XMLElement* xmlElement, const TakeExportXML* eventTrackList) :
  m_xmlElement(xmlElement), m_eventTrackList(eventTrackList)
{
  BuildNodeList(DiscreteEventExportXML, m_events, m_xmlElement, "DiscreteEvent");
  XMLElement* attributesBlockElement = m_xmlElement->findChild("Attributes");
  if (attributesBlockElement != 0)
  {
    m_xmlAttributeBlock = new AttributeBlockXML(attributesBlockElement);
  }
}

//----------------------------------------------------------------------------------------------------------------------
DiscreteEventTrackExportXML::DiscreteEventTrackExportXML(
  GUID           guid,
  const wchar_t* name,
  unsigned int   eventTrackChannelID,
  unsigned int   userData,
  const TakeExportXML* eventTrackList) :
  m_eventTrackList(eventTrackList)
{
  m_xmlElement = new XMLElement("DiscreteEventTrack");
  m_xmlElement->setAttribute("name", name);
  m_xmlElement->setGUID(guid);
  m_xmlElement->setUIntAttribute("channelID", eventTrackChannelID);

  m_xmlAttributeBlock = new AttributeBlockXML();
  m_xmlElement->LinkEndChild(m_xmlAttributeBlock->getXMLElement());
  m_xmlAttributeBlock->createUIntAttribute("userData", userData);
}

//----------------------------------------------------------------------------------------------------------------------
DiscreteEventTrackExportXML::~DiscreteEventTrackExportXML()
{
  for (DiscreteEventExportXMLList::iterator it = m_events.begin(); it != m_events.end(); ++it)
  {
    DiscreteEventExportXML* eventExportXml = *it;
    delete eventExportXml;
  }
  delete m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
GUID DiscreteEventTrackExportXML::getGUID() const
{
  GUID result = m_xmlElement->getGUID();
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const char* DiscreteEventTrackExportXML::getName() const
{
  const char* result = m_xmlElement->getAttribute("name");
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int DiscreteEventTrackExportXML::getEventTrackChannelID() const
{
  unsigned int result = 0xffffffff;
  m_xmlElement->getUIntAttribute("channelID", result);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const TakeExport* DiscreteEventTrackExportXML::getTake() const
{
  return m_eventTrackList;
}

//----------------------------------------------------------------------------------------------------------------------
const AttributeBlockExport* DiscreteEventTrackExportXML::getAttributeBlock() const
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeBlockExport* DiscreteEventTrackExportXML::getAttributeBlock()
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int DiscreteEventTrackExportXML::getNumEvents() const
{
  return (unsigned int) m_events.size();
}

//----------------------------------------------------------------------------------------------------------------------
const DiscreteEventExport* DiscreteEventTrackExportXML::getEvent(unsigned int eventIdx) const
{
  if (eventIdx < m_events.size())
  {
    return m_events[eventIdx];
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
DiscreteEventExport* DiscreteEventTrackExportXML::getEvent(unsigned int eventIdx)
{
  if (eventIdx < m_events.size())
  {
    return m_events[eventIdx];
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
DiscreteEventExport* DiscreteEventTrackExportXML::createEvent(
  unsigned int index,
  float        normalisedTime,
  unsigned int userData)
{
  DiscreteEventExportXML* eventExport = new DiscreteEventExportXML(index, userData, normalisedTime);
  m_events.push_back(eventExport);
  m_xmlElement->LinkEndChild(eventExport->getXMLElement());
  return eventExport;
}

//----------------------------------------------------------------------------------------------------------------------
// DurationEventTrackExportXML
//----------------------------------------------------------------------------------------------------------------------
DurationEventTrackExportXML::DurationEventTrackExportXML(XMLElement* xmlElement, const TakeExportXML* eventTrackList) :
  m_xmlElement(xmlElement),
  m_eventTrackList(eventTrackList)
{
  BuildNodeList(DurationEventExportXML, m_events, m_xmlElement, "DurationEvent");
  XMLElement* attributesBlockElement = m_xmlElement->findChild("Attributes");
  if (attributesBlockElement != 0)
  {
    m_xmlAttributeBlock = new AttributeBlockXML(attributesBlockElement);
  }
}

//----------------------------------------------------------------------------------------------------------------------
DurationEventTrackExportXML::DurationEventTrackExportXML(
  GUID           guid,
  const wchar_t* name,
  unsigned int   eventTrackChannelID,
  unsigned int   userData,
  const TakeExportXML* eventTrackList) :
  m_eventTrackList(eventTrackList)
{
  m_xmlElement = new XMLElement("DurationEventTrack");
  m_xmlElement->setAttribute("name", name);
  m_xmlElement->setGUID(guid);
  m_xmlElement->setUIntAttribute("channelID", eventTrackChannelID);

  m_xmlAttributeBlock = new AttributeBlockXML();
  m_xmlElement->LinkEndChild(m_xmlAttributeBlock->getXMLElement());
  m_xmlAttributeBlock->createUIntAttribute("userData", userData);
}

//----------------------------------------------------------------------------------------------------------------------
DurationEventTrackExportXML::~DurationEventTrackExportXML()
{
  for (DurationEventExportXMLList::iterator it = m_events.begin(); it != m_events.end(); ++it)
  {
    DurationEventExportXML* eventExportXml = *it;
    delete eventExportXml;
  }
  delete m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
GUID DurationEventTrackExportXML::getGUID() const
{
  GUID result = m_xmlElement->getGUID();
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const char* DurationEventTrackExportXML::getName() const
{
  const char* result = m_xmlElement->getAttribute("name");
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int DurationEventTrackExportXML::getEventTrackChannelID() const
{
  unsigned int result = 0xffffffff;
  m_xmlElement->getUIntAttribute("channelID", result);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const TakeExport* DurationEventTrackExportXML::getTake() const
{
  return m_eventTrackList;
}

//----------------------------------------------------------------------------------------------------------------------
const AttributeBlockExport* DurationEventTrackExportXML::getAttributeBlock() const
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeBlockExport* DurationEventTrackExportXML::getAttributeBlock()
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int DurationEventTrackExportXML::getNumEvents() const
{
  return (unsigned int) m_events.size();
}

//----------------------------------------------------------------------------------------------------------------------
const DurationEventExport* DurationEventTrackExportXML::getEvent(unsigned int eventIdx) const
{
  if (eventIdx < m_events.size())
  {
    return m_events[eventIdx];
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
DurationEventExport* DurationEventTrackExportXML::getEvent(unsigned int eventIdx)
{
  if (eventIdx < m_events.size())
  {
    return m_events[eventIdx];
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
DurationEventExport* DurationEventTrackExportXML::createEvent(
  unsigned int index,
  float        normalisedTime,
  float        duration,
  unsigned int userData)
{
  DurationEventExportXML* eventExport = new DurationEventExportXML(index, userData, normalisedTime, duration);
  m_events.push_back(eventExport);
  m_xmlElement->LinkEndChild(eventExport->getXMLElement());
  return eventExport;
}

//----------------------------------------------------------------------------------------------------------------------
// CurveEventTrackExportXML
//----------------------------------------------------------------------------------------------------------------------
CurveEventTrackExportXML::CurveEventTrackExportXML(XMLElement* xmlElement, const TakeExportXML* eventTrackList) :
  m_xmlElement(xmlElement),
  m_eventTrackList(eventTrackList)
{
  BuildNodeList(CurveEventExportXML, m_events, m_xmlElement, "CurveEvent");
  XMLElement* attributesBlockElement = m_xmlElement->findChild("Attributes");
  if (attributesBlockElement != 0)
  {
    m_xmlAttributeBlock = new AttributeBlockXML(attributesBlockElement);
  }
}

//----------------------------------------------------------------------------------------------------------------------
CurveEventTrackExportXML::CurveEventTrackExportXML(
  GUID           guid,
  const wchar_t* name,
  unsigned int   eventTrackChannelID,
  unsigned int   userData,
  const TakeExportXML* eventTrackList) :
  m_eventTrackList(eventTrackList)
{
  m_xmlElement = new XMLElement("CurveEventTrack");
  m_xmlElement->setAttribute("name", name);
  m_xmlElement->setGUID(guid);
  m_xmlElement->setUIntAttribute("channelID", eventTrackChannelID);

  m_xmlAttributeBlock = new AttributeBlockXML();
  m_xmlElement->LinkEndChild(m_xmlAttributeBlock->getXMLElement());
  m_xmlAttributeBlock->createUIntAttribute("userData", userData);
}

//----------------------------------------------------------------------------------------------------------------------
CurveEventTrackExportXML::~CurveEventTrackExportXML()
{
  for (CurveEventTrackExportXMLList::iterator it = m_events.begin(); it != m_events.end(); ++it)
  {
    CurveEventExportXML* eventExportXml = *it;
    delete eventExportXml;
  }
  delete m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
GUID CurveEventTrackExportXML::getGUID() const
{
  GUID result = m_xmlElement->getGUID();
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const char* CurveEventTrackExportXML::getName() const
{
  const char* result = m_xmlElement->getAttribute("name");
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int CurveEventTrackExportXML::getEventTrackChannelID() const
{
  unsigned int result = 0xffffffff;
  m_xmlElement->getUIntAttribute("channelID", result);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const TakeExport* CurveEventTrackExportXML::getTake() const
{
  return m_eventTrackList;
}

//----------------------------------------------------------------------------------------------------------------------
const AttributeBlockExport* CurveEventTrackExportXML::getAttributeBlock() const
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeBlockExport* CurveEventTrackExportXML::getAttributeBlock()
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int CurveEventTrackExportXML::getNumEvents() const
{
  return (unsigned int) m_events.size();
}

//----------------------------------------------------------------------------------------------------------------------
const CurveEventExport* CurveEventTrackExportXML::getEvent(unsigned int eventIdx) const
{
  if (eventIdx < m_events.size())
  {
    return m_events[eventIdx];
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
CurveEventExport* CurveEventTrackExportXML::getEvent(unsigned int eventIdx)
{
  if (eventIdx < m_events.size())
  {
    return m_events[eventIdx];
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
CurveEventExport* CurveEventTrackExportXML::createEvent(
  unsigned int index,
  float        normalisedTime,
  float        floatValue,
  unsigned int userData)
{
  CurveEventExportXML* eventExport = new CurveEventExportXML(index, userData, normalisedTime, floatValue);
  m_events.push_back(eventExport);
  m_xmlElement->LinkEndChild(eventExport->getXMLElement());
  return eventExport;
}

//----------------------------------------------------------------------------------------------------------------------
// TemplateInfoXML
//----------------------------------------------------------------------------------------------------------------------
TemplateInfoXML::TemplateInfoXML(XMLElement* xmlElement): WrappedAttributeBlockXML(xmlElement)
{

}

//----------------------------------------------------------------------------------------------------------------------
TemplateInfoXML::TemplateInfoXML(
  const wchar_t* templateName,
  const char* type,
  double AbsEndFraction) :
  WrappedAttributeBlockXML("Template")
{
  getAttributeBlock()->createStringAttribute("templateName", toUTF8(templateName).c_str());
  getAttributeBlock()->createStringAttribute("type", type);
  getAttributeBlock()->createDoubleAttribute("absEndFraction", AbsEndFraction);
}

//----------------------------------------------------------------------------------------------------------------------
TemplateInfoXML::~TemplateInfoXML()
{
}

//----------------------------------------------------------------------------------------------------------------------
double TemplateInfoXML::getAbsEndFraction() const
{
  double endFrac = 0.0;
  getAttributeBlock()->getDoubleAttribute("absEndFraction", endFrac);
  return endFrac;
}

//----------------------------------------------------------------------------------------------------------------------
const char* TemplateInfoXML::getTemplateName() const
{
  const char* result = "";
  getAttributeBlock()->getStringAttribute("templateName", result);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const char* TemplateInfoXML::getType() const // one of Nil, Stationary, Hinge, Point
{
  const char* result = "";
  getAttributeBlock()->getStringAttribute("type", result);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// ConstraintEventXML
//----------------------------------------------------------------------------------------------------------------------
ConstraintEventXML::ConstraintEventXML(XMLElement* xmlElement) : EventBaseExportXML(xmlElement)
{
  BuildNodeList(TemplateInfoXML, m_templateInfos, m_xmlElement, "Template");
}

//----------------------------------------------------------------------------------------------------------------------
ConstraintEventXML::ConstraintEventXML(
  unsigned int index,
  float        normalisedTime,
  float        duration,
  GUID         guid,
  unsigned int userData) :
  EventBaseExportXML("ConstraintEvent", index, userData)
{
  if (getAttributeBlock() != 0)
  {
    getAttributeBlock()->createDoubleAttribute("startTime", normalisedTime);
    getAttributeBlock()->createDoubleAttribute("duration", duration);
  }
  m_xmlElement->setGUID(guid);
}

//----------------------------------------------------------------------------------------------------------------------
ConstraintEventXML::~ConstraintEventXML()
{

  for (unsigned int i = 0; i < m_templateInfos.size(); ++i)
  {
    delete m_templateInfos[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool ConstraintEventXML::isAttributeDynamic(AttributeExport* attribute) const
{
  const AttributeBlockExport* attributeBlock = getAttributeBlock();
  if (attribute == attributeBlock->getAttributeByName("startTime"))
  {
    return false;
  }
  else if (attribute == attributeBlock->getAttributeByName("duration"))
  {
    return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int ConstraintEventXML::getNumTemplateInfo() const
{
  return (unsigned int) m_templateInfos.size();
}

//----------------------------------------------------------------------------------------------------------------------
const TemplateInfoXML* ConstraintEventXML::getTemplateInfo(unsigned int eventIdx) const
{
  if (eventIdx < m_templateInfos.size())
  {
    return m_templateInfos[eventIdx];
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
TemplateInfoXML* ConstraintEventXML::getTemplateInfo(unsigned int eventIdx)
{
  if (eventIdx < m_templateInfos.size())
  {
    return m_templateInfos[eventIdx];
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
TemplateInfoXML* ConstraintEventXML::createTemplateInfo(
  const wchar_t* templateName,
  const char*    type,
  double         AbsEndFraction)
{
  TemplateInfoXML* tempInfo = new TemplateInfoXML(templateName, type, AbsEndFraction);
  m_templateInfos.push_back(tempInfo);
  m_xmlElement->LinkEndChild(tempInfo->getXMLElement());
  return tempInfo;
}

//----------------------------------------------------------------------------------------------------------------------
GUID ConstraintEventXML::getGUID() const
{
  return m_xmlElement->getGUID();
}

//----------------------------------------------------------------------------------------------------------------------
float ConstraintEventXML::getNormalisedStartTime() const
{
  double time = 0.0f;
  getAttributeBlock()->getDoubleAttribute("startTime", time);
  return (float)time;
}

//----------------------------------------------------------------------------------------------------------------------
float ConstraintEventXML::getNormalisedDuration() const
{
  double duration = 0.0f;
  getAttributeBlock()->getDoubleAttribute("duration", duration);
  return (float)duration;
}

//----------------------------------------------------------------------------------------------------------------------
// CustomEventTrackExportXML
//----------------------------------------------------------------------------------------------------------------------
CustomEventTrackExportXML::CustomEventTrackExportXML(XMLElement* xmlElement, const TakeExportXML* eventTrackList) :
  m_xmlElement(xmlElement),
  m_eventTrackList(eventTrackList)
{
  BuildNodeList(ConstraintEventXML, m_constraintEvents, m_xmlElement, "ConstraintEvent");
  XMLElement* attributesBlockElement = m_xmlElement->findChild("Attributes");
  if (attributesBlockElement != 0)
  {
    m_xmlAttributeBlock = new AttributeBlockXML(attributesBlockElement);
  }
}

//----------------------------------------------------------------------------------------------------------------------
CustomEventTrackExportXML::CustomEventTrackExportXML(
  GUID                 guid,
  const wchar_t*       name,
  unsigned int         eventTrackChannelID,
  unsigned int         userData,
  const TakeExportXML* eventTrackList) :
  m_eventTrackList(eventTrackList)
{
  m_xmlElement = new XMLElement("CustomEventTrack");
  m_xmlElement->setAttribute("name", name);
  m_xmlElement->setGUID(guid);
  m_xmlElement->setUIntAttribute("channelID", eventTrackChannelID);

  m_xmlAttributeBlock = new AttributeBlockXML();
  m_xmlElement->LinkEndChild(m_xmlAttributeBlock->getXMLElement());
  m_xmlAttributeBlock->createUIntAttribute("userData", userData);
}

//----------------------------------------------------------------------------------------------------------------------
CustomEventTrackExportXML::~CustomEventTrackExportXML()
{
  for (ConstraintEventXMLList::iterator it = m_constraintEvents.begin(); it != m_constraintEvents.end(); ++it)
  {
    ConstraintEventXML* eventExportXml = *it;
    delete eventExportXml;
  }
  delete m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
GUID CustomEventTrackExportXML::getGUID() const
{
  GUID result = m_xmlElement->getGUID();
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const char* CustomEventTrackExportXML::getName() const
{
  const char* result = m_xmlElement->getAttribute("name");
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const TakeExport* CustomEventTrackExportXML::getTake() const
{
  return m_eventTrackList;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int CustomEventTrackExportXML::getEventTrackChannelID() const
{
  unsigned int result = 0xffffffff;
  m_xmlElement->getUIntAttribute("channelID", result);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
unsigned int CustomEventTrackExportXML::getNumEvents() const
{
  return (unsigned int) m_constraintEvents.size();
}

//----------------------------------------------------------------------------------------------------------------------
const EventBaseExport* CustomEventTrackExportXML::getEvent(unsigned int eventIdx) const
{
  if (eventIdx < m_constraintEvents.size())
  {
    return m_constraintEvents[eventIdx];
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
EventBaseExport* CustomEventTrackExportXML::getEvent(unsigned int eventIdx)
{
  if (eventIdx < m_constraintEvents.size())
  {
    return m_constraintEvents[eventIdx];
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
ConstraintEventXML* CustomEventTrackExportXML::createConstraintEvent(
  unsigned int index,
  float        normalisedTime,
  float        duration,
  GUID         guid,
  unsigned int userData)
{
  ConstraintEventXML* newConstraint = new ConstraintEventXML(index, normalisedTime, duration, guid, userData);
  m_constraintEvents.push_back(newConstraint);
  m_xmlElement->LinkEndChild(newConstraint->getXMLElement());
  return newConstraint;
}

//----------------------------------------------------------------------------------------------------------------------
const AttributeBlockExport* CustomEventTrackExportXML::getAttributeBlock() const
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeBlockExport* CustomEventTrackExportXML::getAttributeBlock()
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
// TakeExportXML
//----------------------------------------------------------------------------------------------------------------------

TakeExportXML::TakeExportXML(XMLElement* xmlElement, const std::string& destFilename) :
  m_xmlElement(xmlElement),
  m_destFilename(destFilename)
{
  initFromElement();
}

TakeExportXML::TakeExportXML(XMLElement* xmlElement) :
  m_xmlElement(xmlElement)
{
  // this case the take is embedded in a list and not in its own file
  initFromElement();
}

void TakeExportXML::initFromElement()
{
  m_eventTracks.clear();

  for (NM::TiXmlNode* curChild = m_xmlElement->FirstChild(); curChild != 0; curChild = curChild->NextSibling())
  {
    XMLElement* element = (XMLElement*) curChild->ToElement();
    if (element)
    {
      if (strcmp(element->Value(), "CurveEventTrack") == 0)
      {
        CurveEventTrackExportXML* trackExport = new CurveEventTrackExportXML(element, this);
        m_eventTracks.push_back(trackExport);
      }
      else if (strcmp(element->Value(), "DurationEventTrack") == 0)
      {
        DurationEventTrackExportXML* trackExport = new DurationEventTrackExportXML(element, this);
        m_eventTracks.push_back(trackExport);
      }
      else if (strcmp(element->Value(), "DiscreteEventTrack") == 0)
      {
        DiscreteEventTrackExportXML* trackExport = new DiscreteEventTrackExportXML(element, this);
        m_eventTracks.push_back(trackExport);
      }
      else if (strcmp(element->Value(), "CustomEventTrack") == 0)
      {
        CustomEventTrackExportXML* trackExport = new CustomEventTrackExportXML(element, this);
        m_eventTracks.push_back(trackExport);
      }
    }
  }

  XMLElement* attributesBlockElement = m_xmlElement->findChild("Attributes");
  if (attributesBlockElement != 0)
  {
    m_xmlAttributeBlock = new AttributeBlockXML(attributesBlockElement);
  }
}
//----------------------------------------------------------------------------------------------------------------------
TakeExportXML::TakeExportXML(
  const wchar_t* takeName,
  float          secDuration,
  float          fps,
  const wchar_t* destFilename) :
  m_xmlElement(new XMLElement("Take")),
  m_destFilename(toUTF8(destFilename))
{
  m_xmlElement->setAttribute("name", takeName);
  m_xmlAttributeBlock = new AttributeBlockXML();
  m_xmlElement->LinkEndChild(m_xmlAttributeBlock->getXMLElement());
  getAttributeBlock()->setDoubleAttribute("secondDuration", (double) secDuration);
  getAttributeBlock()->setDoubleAttribute("fps", (double) fps);
}

//----------------------------------------------------------------------------------------------------------------------
TakeExportXML::~TakeExportXML()
{
  for (uint32_t i = 0 ; i < m_eventTracks.size() ; ++i)
  {
    delete m_eventTracks[i];
  }
  delete m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
TakeExport* TakeExportXML::createTake(
  const wchar_t* name,
  float          secDuration,
  float          fps,
  bool           loop,
  float          clipStart,
  float          clipEnd)
{
  TakeExportXML* newTake = new TakeExportXML(name, secDuration, fps, L"");
  newTake->getAttributeBlock()->setDoubleAttribute("clipStart", clipStart);
  newTake->getAttributeBlock()->setDoubleAttribute("clipEnd", clipEnd);
  newTake->getAttributeBlock()->setBoolAttribute("loop", loop);
  return newTake;
}

//----------------------------------------------------------------------------------------------------------------------
EventTrackExport* TakeExportXML::createEventTrack(
  EventTrackExport::EventTrackType trackType,
  GUID           guid,
  const wchar_t* name,
  unsigned int   eventTrackChannelID,
  unsigned int   userData)
{
  EventTrackExport* trackExport = 0;
  switch (trackType)
  {
  case EventTrackExport::EVENT_TRACK_TYPE_DISCRETE :
    {
      DiscreteEventTrackExportXML* tickTrack = new DiscreteEventTrackExportXML(guid, name, eventTrackChannelID, userData, this);
      m_xmlElement->LinkEndChild(tickTrack->m_xmlElement);
      trackExport = tickTrack;
    }
    break;

  case EventTrackExport::EVENT_TRACK_TYPE_DURATION :
    {
      DurationEventTrackExportXML* durationTack = new DurationEventTrackExportXML(guid, name, eventTrackChannelID, userData, this);
      m_xmlElement->LinkEndChild(durationTack->m_xmlElement);
      trackExport = durationTack;
    }
    break;

  case EventTrackExport::EVENT_TRACK_TYPE_CURVE :
    {
      CurveEventTrackExportXML* curveTack = new CurveEventTrackExportXML(guid, name, eventTrackChannelID, userData, this);
      m_xmlElement->LinkEndChild(curveTack->m_xmlElement);
      trackExport = curveTack;
    }
    break;

  case EventTrackExport::EVENT_TRACK_TYPE_CUSTOM :
    {
      CustomEventTrackExportXML* custTrack = new CustomEventTrackExportXML(guid, name, eventTrackChannelID, userData, this);
      m_xmlElement->LinkEndChild(custTrack->m_xmlElement);
      trackExport = custTrack;
    }
    break;
  }

  m_eventTracks.push_back(trackExport);

  return trackExport;
}

//----------------------------------------------------------------------------------------------------------------------
const AttributeBlockExport* TakeExportXML::getAttributeBlock() const
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeBlockExport* TakeExportXML::getAttributeBlock()
{
  return m_xmlAttributeBlock;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int TakeExportXML::getNumEventTracks() const
{
  unsigned int result = (unsigned int)m_eventTracks.size();
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
EventTrackExport* TakeExportXML::getEventTrack(unsigned int index)
{
  unsigned int numEventTracks = getNumEventTracks();
  if (index >= numEventTracks)
  {
    return 0;
  }

  EventTrackExport* result = m_eventTracks.at(index);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const EventTrackExport* TakeExportXML::getEventTrack(unsigned int index) const
{
  unsigned int numEventTracks = getNumEventTracks();
  if (index >= numEventTracks)
  {
    return 0;
  }

  const EventTrackExport* result = m_eventTracks.at(index);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
EventTrackExport* TakeExportXML::getEventTrack(GUID guid)
{
  for (EventTrackExportList::iterator it = m_eventTracks.begin(); it != m_eventTracks.end(); ++it)
  {
    EventTrackExport* trackExport = *it;
    GUID testGuid = trackExport->getGUID();

    if (strcmp(guid, testGuid) == 0)
    {
      return trackExport;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
float TakeExportXML::getCachedTakeSecondsDuration() const
{
  double duration;
  getAttributeBlock()->getDoubleAttribute("secondDuration", duration);
  return (float) duration;
}

//----------------------------------------------------------------------------------------------------------------------
float TakeExportXML::getCachedTakeFPS() const
{
  double fps;
  getAttributeBlock()->getDoubleAttribute("fps", fps);
  return (float) fps;
}

//----------------------------------------------------------------------------------------------------------------------
bool TakeExportXML::getLoop() const
{
  bool loop;
  getAttributeBlock()->getBoolAttribute("loop", loop);
  return loop;
}

//----------------------------------------------------------------------------------------------------------------------
float TakeExportXML::getClipStart() const
{
  double clipStart;
  getAttributeBlock()->getDoubleAttribute("clipStart", clipStart);
  return (float) clipStart;
}

//----------------------------------------------------------------------------------------------------------------------
float TakeExportXML::getClipEnd() const
{
  double clipEnd;
  getAttributeBlock()->getDoubleAttribute("clipEnd", clipEnd);
  return (float) clipEnd;
}

//----------------------------------------------------------------------------------------------------------------------
const EventTrackExport* TakeExportXML::getEventTrack(GUID guid) const
{
  for (EventTrackExportList::const_iterator it = m_eventTracks.begin(); it != m_eventTracks.end(); ++it)
  {
    const EventTrackExport* trackExport = *it;
    GUID testGuid = trackExport->getGUID();

    if (strcmp(guid, testGuid) == 0)
    {
      return trackExport;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool TakeExportXML::write()
{
  return m_xmlElement->saveFile(m_destFilename.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
const char* TakeExportXML::getName() const
{
  return m_xmlElement->getAttribute("name");
}

//----------------------------------------------------------------------------------------------------------------------
GUID TakeExportXML::getGUID() const
{
  // takes don't need a guid
  GUID result = "";
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const char* TakeExportXML::getDestFilename() const
{
  const char* result = m_destFilename.c_str();
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
/// TakeListXML
//----------------------------------------------------------------------------------------------------------------------
TakeListXML::TakeListXML(XMLElement* xmlElement, const std::string& destFilename) :
  m_xmlElement(xmlElement),
  m_destFilename(destFilename)
{
  BuildNodeList(TakeExportXML, m_takes, m_xmlElement, "Take");
}

TakeListXML::TakeListXML(const wchar_t* sourceAnimFile, unsigned int formatVersion, const wchar_t* destFilename) :
  m_xmlElement(new XMLElement("TakeList")),
  m_destFilename(toUTF8(destFilename))
{
  m_xmlElement->setAttribute("sourceAnimFile", sourceAnimFile);
  m_xmlElement->setUIntAttribute("version", formatVersion);
}

//----------------------------------------------------------------------------------------------------------------------
TakeListXML::~TakeListXML()
{
  for (uint32_t i = 0 ; i < m_takes.size() ; ++i)
  {
    delete m_takes[i];
  }
  delete m_xmlElement;
}

//----------------------------------------------------------------------------------------------------------------------
TakeExport* TakeListXML::createTake(
  const wchar_t* name,
  float secDuration,
  float fps,
  bool  loop,
  float clipStart,
  float clipEnd)
{
  TakeExportXML* newTake = new TakeExportXML(name, secDuration, fps, L""); // these takes are not stored in a file by themselves but in a list
  m_xmlElement->LinkEndChild(newTake->m_xmlElement);
  newTake->getAttributeBlock()->setDoubleAttribute("clipStart", clipStart);
  newTake->getAttributeBlock()->setDoubleAttribute("clipEnd", clipEnd);
  newTake->getAttributeBlock()->setBoolAttribute("loop", loop);
  m_takes.push_back(newTake);
  return newTake;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int TakeListXML::getNumTakes() const
{
  return (unsigned int) m_takes.size();
}

//----------------------------------------------------------------------------------------------------------------------
TakeExport* TakeListXML::getTake(unsigned int index)
{
  if (index < m_takes.size())
  {
    return m_takes[index];
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const TakeExport* TakeListXML::getTake(unsigned int index) const
{
  if (index < m_takes.size())
  {
    return m_takes[index];
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const char* TakeListXML::getName() const
{
  return getSourceAnimFilename();
}

//----------------------------------------------------------------------------------------------------------------------
GUID TakeListXML::getGUID() const
{
  GUID result = "";
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const char* TakeListXML::getSourceAnimFilename() const
{
  return m_xmlElement->getAttribute("sourceAnimFile");
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int TakeListXML::getFormatVersion() const
{
  unsigned int version = 1;
  m_xmlElement->getUIntAttribute("version", version);
  return version;
}

//----------------------------------------------------------------------------------------------------------------------
const char* TakeListXML::getDestFilename() const
{
  return m_destFilename.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
bool TakeListXML::write()
{
  return m_xmlElement->saveFile(m_destFilename.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
// AnimationEntryExportXML
//----------------------------------------------------------------------------------------------------------------------
AnimationEntryExportXML::AnimationEntryExportXML(
  XMLElement*                  element,
  ExportFactoryXML*            factory,
  const AnimationSetExportXML* animationSet) :
  m_xmlElement(element),
  m_exportFactory(factory),
  m_animationSet(animationSet),
  m_loadedTakeExport(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
AnimationEntryExportXML::AnimationEntryExportXML(
  unsigned int                 index,
  const wchar_t*               animFile,
  const wchar_t*               takeFile,
  const wchar_t*               take,
  const wchar_t*               syncTrack,
  const char*                  format,
  const char*                  options,
  const AnimationSetExportXML* animationSet) :
  m_xmlElement(new XMLElement("AnimationEntry")),
  m_animationSet(animationSet),
  m_exportFactory(0),
  m_loadedTakeExport(0)
{
  m_xmlElement->setUIntAttribute("index", index);
  m_xmlElement->setChildText("animFile", animFile);
  m_xmlElement->setChildText("takeFile", takeFile);
  m_xmlElement->setChildText("take", take);
  m_xmlElement->setChildText("syncTrack", syncTrack);
  m_xmlElement->setAttribute("format", format);
  m_xmlElement->setAttribute("options", options);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int AnimationEntryExportXML::getIndex() const
{
  unsigned int val = 0xffffffff;
  m_xmlElement->getUIntAttribute("index", val);
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
const char* AnimationEntryExportXML::getAnimationFilename() const
{
  return m_xmlElement->getChildText("animFile");
}

//----------------------------------------------------------------------------------------------------------------------
const char* AnimationEntryExportXML::getTakeFilename() const
{
  return m_xmlElement->getChildText("takeFile");
}

//----------------------------------------------------------------------------------------------------------------------
const char* AnimationEntryExportXML::getTakeName() const
{
  return m_xmlElement->getChildText("take");
}

//----------------------------------------------------------------------------------------------------------------------
const char* AnimationEntryExportXML::getSyncTrack() const
{
  return m_xmlElement->getChildText("syncTrack");
}

//----------------------------------------------------------------------------------------------------------------------
const char* AnimationEntryExportXML::getOptions() const
{
  return m_xmlElement->getAttribute("options");
}

//----------------------------------------------------------------------------------------------------------------------
const char* AnimationEntryExportXML::getFormat() const
{
  return m_xmlElement->getAttribute("format");
}

//----------------------------------------------------------------------------------------------------------------------
TakeExport* AnimationEntryExportXML::getTakeExportData(const char* rootDir)
{
  if (m_loadedTakeExport == 0)
  {
    const char* takeString = m_xmlElement->getChildText("takeFile");
    if (takeString)
    {
      std::string loadFile = demacroizeString(takeString, rootDir);
      AssetExport* asset = m_exportFactory->loadAsset(loadFile.c_str());
      NMP_VERIFY_MSG(asset != NULL, "Could not find event markup file '%s'", takeString);

      TakeList*    takeList = dynamic_cast<TakeList*> (asset);
      if (takeList)
      {
        m_loadedTakeExport = takeList->getTake(getTakeName());
      }
    }
  }
  return m_loadedTakeExport;
}

//----------------------------------------------------------------------------------------------------------------------
const AnimationSetExportXML* AnimationEntryExportXML::getAnimationSet() const
{
  return m_animationSet;
}

//----------------------------------------------------------------------------------------------------------------------
// AnimationSetExportXML
//----------------------------------------------------------------------------------------------------------------------
AnimationSetExportXML::AnimationSetExportXML(
  XMLElement*                element,
  ExportFactoryXML*          factory,
  const AnimationLibraryXML* animLibrary) :
  m_xmlElement(element),
  m_animationLibrary(animLibrary),
  m_exportFactory(factory)
{
  BuildIndexedNodeListAndPassParent(AnimationEntryExportXML,
                                    m_animationEntries,
                                    m_xmlElement,
                                    "AnimationEntry",
                                    factory,
                                    this);
  BuildIndexedNodeList(IntMirrorMappingXML, m_mirroredEventUserdatas, m_xmlElement, "EventMirrorMapping");
  BuildIndexedNodeList(IntMirrorMappingXML, m_mirroredEventTracks, m_xmlElement, "TrackMirrorMapping");
}

//----------------------------------------------------------------------------------------------------------------------
AnimationSetExportXML::AnimationSetExportXML(
  unsigned int               index,
  const wchar_t*             animSetName,
  const AnimationLibraryXML* animLibrary) :
  m_xmlElement(new XMLElement("AnimationSet")),
  m_animationLibrary(animLibrary),
  m_exportFactory(0)
{
  m_xmlElement->setUIntAttribute("index", index);
  m_xmlElement->setAttribute("name", animSetName);
}

//----------------------------------------------------------------------------------------------------------------------
AnimationEntryExport* AnimationSetExportXML::createAnimationEntry(
  unsigned int   index,
  const wchar_t* animFile,
  const wchar_t* takeFile,
  const wchar_t* take,
  const wchar_t* syncTrack,
  const char*    format,
  const char*    options)
{
  AnimationEntryExportXML* animEntry = new AnimationEntryExportXML(index, animFile, takeFile, take, syncTrack, format, options, this);

  m_xmlElement->LinkEndChild(animEntry->m_xmlElement);
  m_animationEntries.push_back(animEntry);

  return animEntry;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimationSetExportXML::createEventUserdataMirrorMapping(
  unsigned int index,
  int          firstUserdata,
  int          secondUserdata)
{
  IntMirrorMappingXML* mapping =
    new IntMirrorMappingXML("EventMirrorMapping", index, firstUserdata, secondUserdata);
  InsertIntoIndexedList(m_mirroredEventUserdatas, index, mapping);
  m_xmlElement->LinkEndChild(mapping->m_xmlElement);
}

//----------------------------------------------------------------------------------------------------------------------
void AnimationSetExportXML::createEventTrackMirrorMapping(
  unsigned int index,
  int          firstTrack,
  int          secondTrack)
{
  IntMirrorMappingXML* mapping =
    new IntMirrorMappingXML("TrackMirrorMapping", index, firstTrack, secondTrack);
  InsertIntoIndexedList(m_mirroredEventTracks, index, mapping);
  m_xmlElement->LinkEndChild(mapping->m_xmlElement);
}

//----------------------------------------------------------------------------------------------------------------------
void AnimationSetExportXML::setRig(const RigExport* rig)
{
  XMLElement* currentRig = m_xmlElement->findChild("RigRef");

  if (currentRig != 0)
  {
    m_xmlElement->RemoveChild(currentRig);
  }

  m_xmlElement->LinkEndChild(new RigRef(rig->getGUID(),
                                        rig->getDestFilename(),
                                        rig->getName()));
}

//----------------------------------------------------------------------------------------------------------------------
void AnimationSetExportXML::setPhysicsRigDef(const PhysicsRigDefExport* physicsRigDef)
{
  XMLElement* currentRig = m_xmlElement->findChild("PhysicsRigDefRef");

  if (currentRig != 0)
  {
    m_xmlElement->RemoveChild(currentRig);
  }

  m_xmlElement->LinkEndChild(new PhysicsRigDefRef(physicsRigDef->getGUID(),
                             physicsRigDef->getDestFilename(),
                             physicsRigDef->getName()));
}

//----------------------------------------------------------------------------------------------------------------------
void AnimationSetExportXML::setBodyDef(const BodyDefExport* bodyDef)
{
  XMLElement* currentBody = m_xmlElement->findChild("BodyDefRef");

  if (currentBody != 0)
  {
    m_xmlElement->RemoveChild(currentBody);
  }

  m_xmlElement->LinkEndChild(new BodyDefRef(bodyDef->getGUID(),
                             bodyDef->getDestFilename(),
                             bodyDef->getName()));
}

//----------------------------------------------------------------------------------------------------------------------
void AnimationSetExportXML::setCharacterController(const CharacterControllerExport* characterController)
{
  XMLElement* currentController = m_xmlElement->findChild("CharacterControllerRef");

  if (currentController != 0)
  {
    m_xmlElement->RemoveChild(currentController);
  }

  m_xmlElement->LinkEndChild(
    new CharacterControllerRef(characterController->getGUID(),
                               characterController->getDestFilename(),
                               characterController->getName()));
}

//----------------------------------------------------------------------------------------------------------------------
void AnimationSetExportXML::setInteractionProxy(const InteractionProxyExport* proxy)
{
  XMLElement* currentProxy = m_xmlElement->findChild("InteractionProxyRef");

  if (currentProxy != 0)
  {
    m_xmlElement->RemoveChild(currentProxy);
  }

  m_xmlElement->LinkEndChild(
    new InteractionProxyRef(proxy->getGUID(),
    proxy->getDestFilename(),
    proxy->getName()));
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int AnimationSetExportXML::getNumAnimationEntries() const
{
  return (unsigned int)m_animationEntries.size();
}

//----------------------------------------------------------------------------------------------------------------------
AnimationEntryExport* AnimationSetExportXML::getAnimationEntry(unsigned int index)
{
  return m_animationEntries[index];
}

//----------------------------------------------------------------------------------------------------------------------
const AnimationEntryExport* AnimationSetExportXML::getAnimationEntry(unsigned int index) const
{
  return m_animationEntries[index];
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int AnimationSetExportXML::getNumEventUserDataMirrorMappings() const
{
  return (unsigned int)m_mirroredEventUserdatas.size();
}

//----------------------------------------------------------------------------------------------------------------------
void AnimationSetExportXML::getEventUserDataMirrorMapping(unsigned int index, int& firstUserData, int& secondUserData) const
{
  IntMirrorMappingXML* mapping = m_mirroredEventUserdatas[index];
  firstUserData = mapping->getFirst();
  secondUserData = mapping->getSecond();
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int AnimationSetExportXML::getNumEventTrackMirrorMappings() const
{
  return (unsigned int)m_mirroredEventTracks.size();
}

//----------------------------------------------------------------------------------------------------------------------
void AnimationSetExportXML::getEventTrackMirrorMapping(unsigned int index, int& firstTrack, int& secondTrack) const
{
  IntMirrorMappingXML* mapping = m_mirroredEventTracks[index];
  firstTrack = mapping->getFirst();
  secondTrack = mapping->getSecond();
}

//----------------------------------------------------------------------------------------------------------------------
RigExport* AnimationSetExportXML::getRig()
{
  std::string fileName = m_animationLibrary->getDestFilename();
  std::string directory = GetDirectory(fileName.c_str());
  RigExport* rig = 0;

  RigRef* rigRef = (RigRef*)m_xmlElement->findChild("RigRef");
  if (rigRef != 0)
  {
    std::string fullName = directory + rigRef->getFilename();
    AssetExport* asset = m_exportFactory->loadAsset(fullName.c_str());

    if (asset != 0)
    {
      rig = dynamic_cast<RigExport*>(asset);
    }
  }

  return rig;
}

//----------------------------------------------------------------------------------------------------------------------
const RigExport* AnimationSetExportXML::getRig() const
{
  return const_cast<AnimationSetExportXML*>(this)->getRig();
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigDefExport* AnimationSetExportXML::getPhysicsRigDef()
{
  std::string fileName = m_animationLibrary->getDestFilename();
  std::string directory = GetDirectory(fileName.c_str());
  PhysicsRigDefExport* rig = 0;

  PhysicsRigDefRef* rigRef = (PhysicsRigDefRef*)m_xmlElement->findChild("PhysicsRigDefRef");
  if (rigRef != 0)
  {
    std::string fullName = directory + rigRef->getFilename();
    AssetExport* asset = m_exportFactory->loadAsset(fullName.c_str());

    if (asset != 0)
    {
      rig = dynamic_cast<PhysicsRigDefExport*>(asset);
    }
  }

  return rig;
}

//----------------------------------------------------------------------------------------------------------------------
const PhysicsRigDefExport* AnimationSetExportXML::getPhysicsRigDef() const
{
  std::string fileName = m_animationLibrary->getDestFilename();
  std::string directory = GetDirectory(fileName.c_str());

  PhysicsRigDefRef* rigRef = (PhysicsRigDefRef*)m_xmlElement->findChild("PhysicsRigDefRef");
  if (rigRef != 0)
  {
    std::string fullName = directory + rigRef->getFilename();

    return static_cast<const PhysicsRigDefExport*>(m_exportFactory->getCachedAsset(fullName));
  }
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
BodyDefExport* AnimationSetExportXML::getBodyDef()
{
  std::string fileName = m_animationLibrary->getDestFilename();
  std::string directory = GetDirectory(fileName.c_str());
  BodyDefExport* body = 0;

  BodyDefRef* bodyRef = (BodyDefRef*)m_xmlElement->findChild("BodyDefRef");
  if (bodyRef != 0)
  {
    std::string fullName = directory + bodyRef->getFilename();
    AssetExport* asset = m_exportFactory->loadAsset(fullName.c_str());

    if (asset != 0)
    {
      body = dynamic_cast<BodyDefExport*>(asset);
    }
  }

  return body;
}

//----------------------------------------------------------------------------------------------------------------------
const BodyDefExport* AnimationSetExportXML::getBodyDef() const
{
  std::string fileName = m_animationLibrary->getDestFilename();
  std::string directory = GetDirectory(fileName.c_str());

  BodyDefRef* bodyRef = (BodyDefRef*)m_xmlElement->findChild("BodyDefRef");
  if (bodyRef != 0)
  {
    std::string fullName = directory + bodyRef->getFilename();

    return static_cast<const BodyDefExport*>(m_exportFactory->getCachedAsset(fullName));
  }
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
CharacterControllerExport* AnimationSetExportXML::getCharacterController()
{
  CharacterControllerExport* controller = 0;
  std::string fileName = m_animationLibrary->getDestFilename();
  std::string directory = GetDirectory(fileName.c_str());

  CharacterControllerRef* controllerRef = (CharacterControllerRef*)m_xmlElement->findChild("CharacterControllerRef");
  if (controllerRef != 0)
  {
    std::string fullName = directory + controllerRef->getFilename();
    AssetExport* asset = m_exportFactory->loadAsset(fullName.c_str());
    NMP_VERIFY_MSG(asset != 0, "Character Controller %s could not be loaded - further processing might fail", fullName.c_str());
    if (asset != 0)
    {
      controller = dynamic_cast<CharacterControllerExport*>(asset);
    }
  }

  return controller;
}

//----------------------------------------------------------------------------------------------------------------------
const CharacterControllerExport* AnimationSetExportXML::getCharacterController() const
{
  std::string fileName = m_animationLibrary->getDestFilename();
  std::string directory = GetDirectory(fileName.c_str());

  CharacterControllerRef* controllerRef = (CharacterControllerRef*)m_xmlElement->findChild("CharacterControllerRef");
  if (controllerRef != 0)
  {
    std::string fullName = directory + controllerRef->getFilename();

    return static_cast<const CharacterControllerExport*>(m_exportFactory->getCachedAsset(fullName));
  }
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
InteractionProxyExport* AnimationSetExportXML::getInteractionProxy()
{
  InteractionProxyExport* proxy = 0;
  std::string fileName = m_animationLibrary->getDestFilename();
  std::string directory = GetDirectory(fileName.c_str());

  InteractionProxyRef* proxyRef = (InteractionProxyRef*)m_xmlElement->findChild("InteractionProxyRef");
  if (proxyRef != 0)
  {
    std::string fullName = directory + proxyRef->getFilename();
    AssetExport* asset = m_exportFactory->loadAsset(fullName.c_str());
    NMP_VERIFY_MSG(asset != 0, "Interaction Proxy %s could not be loaded - further processing might fail", fullName.c_str());
    if (asset != 0)
    {
      proxy = dynamic_cast<InteractionProxyExport*>(asset);
    }
  }

  return proxy;
}

//----------------------------------------------------------------------------------------------------------------------
const InteractionProxyExport* AnimationSetExportXML::getInteractionProxy() const
{
  std::string fileName = m_animationLibrary->getDestFilename();
  std::string directory = GetDirectory(fileName.c_str());

  InteractionProxyRef* proxyRef = (InteractionProxyRef*)m_xmlElement->findChild("InteractionProxyRef");
  if (proxyRef != 0)
  {
    std::string fullName = directory + proxyRef->getFilename();

    return static_cast<const InteractionProxyExport*>(m_exportFactory->getCachedAsset(fullName));
  }
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
const char* AnimationSetExportXML::getName() const
{
  return m_xmlElement->getAttribute("name");
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int AnimationSetExportXML::getIndex() const
{
  unsigned int val = 0xffffffff;
  m_xmlElement->getUIntAttribute("index", val);
  return val;
}

//----------------------------------------------------------------------------------------------------------------------

const AnimationLibraryXML* AnimationSetExportXML::getAnimationLibrary() const
{
  return m_animationLibrary;
}

//----------------------------------------------------------------------------------------------------------------------
AnimationSetExportXML::~AnimationSetExportXML()
{
  for (uint32_t i = 0 ; i < m_animationEntries.size() ; ++i)
  {
    delete m_animationEntries[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
// AnimationLibrary
//----------------------------------------------------------------------------------------------------------------------
AnimationLibraryXML::AnimationLibraryXML() : m_xmlElement(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
AnimationLibraryXML::AnimationLibraryXML(
  XMLElement*        element,
  const std::string& destFilename,
  ExportFactoryXML*  factory) :
  m_xmlElement(element),
  m_destFilename(destFilename)
{
  BuildIndexedNodeListAndPassParent(AnimationSetExportXML,
                                    m_animationSets,
                                    m_xmlElement,
                                    "AnimationSet",
                                    factory,
                                    this);
}

AnimationLibraryXML::AnimationLibraryXML(
  GUID           guid,
  const wchar_t* libraryName,
  const wchar_t* destFilename) :
  m_xmlElement(new XMLElement("AnimationLibrary")),
  m_destFilename(toUTF8(destFilename))
{
  m_xmlElement->setGUID(guid);
  m_xmlElement->setAttribute("name", libraryName);
}

//----------------------------------------------------------------------------------------------------------------------
AnimationLibraryXML::~AnimationLibraryXML()
{
  for (uint32_t i = 0 ; i < m_animationSets.size() ; ++i)
  {
    delete m_animationSets[i];
  }
  delete m_xmlElement;
}

//----------------------------------------------------------------------------------------------------------------------
const char* AnimationLibraryXML::getName() const
{
  return m_xmlElement->getAttribute("name");
}

//----------------------------------------------------------------------------------------------------------------------
GUID AnimationLibraryXML::getGUID() const
{
  return m_xmlElement->getGUID();
}

//----------------------------------------------------------------------------------------------------------------------
const char* AnimationLibraryXML::getDestFilename() const
{
  return m_destFilename.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
AnimationSetExport* AnimationLibraryXML::createAnimationSet(unsigned int index, const wchar_t* setName)
{
  AnimationSetExportXML* animSet = new AnimationSetExportXML(index, setName, this);

  m_xmlElement->LinkEndChild(animSet->m_xmlElement);
  m_animationSets.push_back(animSet);

  return animSet;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int AnimationLibraryXML::getNumAnimationSets() const
{
  return (unsigned int)m_animationSets.size();
}

//----------------------------------------------------------------------------------------------------------------------
AnimationSetExport* AnimationLibraryXML::getAnimationSet(unsigned int index)
{
  return m_animationSets[index];
}

//----------------------------------------------------------------------------------------------------------------------
const AnimationSetExport* AnimationLibraryXML::getAnimationSet(unsigned int index) const
{
  return m_animationSets[index];
}

//----------------------------------------------------------------------------------------------------------------------
bool AnimationLibraryXML::write()
{
  return m_xmlElement->saveFile(m_destFilename.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
XMLElement* AnimationLibraryXML::getXMLElement() const
{
  return m_xmlElement;
}

//----------------------------------------------------------------------------------------------------------------------
// NetworkDefExportXML
//----------------------------------------------------------------------------------------------------------------------
NetworkDefExportXML::NetworkDefExportXML(
  XMLElement*        element,
  const std::string& destFilename,
  ExportFactoryXML*  factory) :
  m_xmlElement(element),
  m_destFilename(destFilename),
  m_exportFactory(factory)
{
  mcExport_MSG("Building network.\n");
  BuildIndexedNodeList(NodeExportXML, m_nodes, m_xmlElement, "Node");
  BuildNodeList(MessageExportXML, m_messages, m_xmlElement, "Message");
}

//----------------------------------------------------------------------------------------------------------------------
NetworkDefExportXML::NetworkDefExportXML(
  GUID           revGUID,
  const wchar_t* networkName,
  const wchar_t* destFilename) :
  m_xmlElement(new XMLElement("NetworkDefinition")),
  m_destFilename(toUTF8(destFilename))
{
  m_xmlElement->setGUID(revGUID);
  m_xmlElement->setAttribute("name", networkName);
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefExportXML::setDestFilename(const wchar_t* destFilename)
{
  m_destFilename = toUTF8(destFilename);
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefExportXML::setRootNodeNetworkID(unsigned int rootNodeNetworkID)
{
  m_xmlElement->setChildUInt("rootNodeNetworkID", rootNodeNetworkID);
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefExportXML::setNetworkWorldOrientation(
  const NMP::Vector3& forwardVector,
  const NMP::Vector3& rightVector,
  const NMP::Vector3& upVector)
{
  m_xmlElement->setChildVector3("networkWorldForwardVector", forwardVector);
  m_xmlElement->setChildVector3("networkWorldRightVector", rightVector);
  m_xmlElement->setChildVector3("networkWorldUpVector", upVector);
}

//----------------------------------------------------------------------------------------------------------------------
NodeExport* NetworkDefExportXML::createNode(
  unsigned int networkID,
  unsigned int typeID,
  unsigned int downstreamParentID,
  bool         downstreamParentMultiplyConnected,
  const char*  name,
  bool         persistent)
{
  NodeExportXML* node = new NodeExportXML(networkID, typeID, downstreamParentID, downstreamParentMultiplyConnected, name, persistent);
  m_xmlElement->LinkEndChild(node->m_xmlElement);
  m_nodes.push_back(node);
  return node;
}

//----------------------------------------------------------------------------------------------------------------------
NodeExport* NetworkDefExportXML::createNode(
  unsigned int   networkID,
  unsigned int   typeID,
  unsigned int   downstreamParentID,
  bool           downstreamParentMultiplyConnected,
  const wchar_t* name,
  bool           persistent)
{
  NodeExportXML* node = new NodeExportXML(networkID, typeID, downstreamParentID, downstreamParentMultiplyConnected, name, persistent);
  m_xmlElement->LinkEndChild(node->m_xmlElement);
  m_nodes.push_back(node);
  return node;
}

//----------------------------------------------------------------------------------------------------------------------
MessageExport* NetworkDefExportXML::createMessage(
  const wchar_t* messageName,
  MR::MessageType messageType,
  MR::MessageID messageID)
{
  MessageExportXML* message = new MessageExportXML(messageName, messageType, messageID);
  m_xmlElement->LinkEndChild(message->m_xmlElement);
  m_messages.push_back(message);
  return message;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefExportXML::setAnimationLibrary(const AnimationLibraryExport* animationLibrary)
{
  const AnimationLibraryXML* animLibraryXML =
    dynamic_cast<const AnimationLibraryXML*>(animationLibrary);
  std::string asFilename = ConvertToRelativePath(animLibraryXML->getDestFilename());
  mcExport_MSG("animlibraryxml = %p.\n", animLibraryXML);
  fflush(stdout);

  AnimLibraryRef* ref = new AnimLibraryRef(animLibraryXML->m_xmlElement->getGUID(), asFilename.c_str());

  m_xmlElement->LinkEndChild(ref);
}

//----------------------------------------------------------------------------------------------------------------------
AnimationLibraryExport* NetworkDefExportXML::getAnimationLibrary()
{
  AnimationLibraryExport* lib = 0;
  AnimLibraryRef* libRef = (AnimLibraryRef*) m_xmlElement->findChild("AnimLibraryRef");
  if (libRef != 0)
  {
    std::string loadFile = GetDirectory(m_destFilename.c_str());
    loadFile.append(libRef->getFilename());
    AssetExport* asset = m_exportFactory->loadAsset(loadFile.c_str());

    if (asset != 0)
    {
      NMP_ASSERT_MSG(dynamic_cast<AnimationLibraryExport*>(asset) != NULL, "Invalid usage - loaded asset is not an AnimationLibraryExport instance!");
      AnimationLibraryExport* animationLibrary = static_cast<AnimationLibraryExport*>(asset);

      lib = animationLibrary;
    }
  }
  return lib;
}

//----------------------------------------------------------------------------------------------------------------------
const AnimationLibraryExport* NetworkDefExportXML::getAnimationLibrary() const
{
  AnimLibraryRef* libRef = (AnimLibraryRef*) m_xmlElement->findChild("AnimLibraryRef");
  if (libRef != 0)
  {
    std::string loadFile = GetDirectory(m_destFilename.c_str());
    loadFile.append(libRef->getFilename());
    return static_cast<const AnimationLibraryExport*>(m_exportFactory->getCachedAsset(loadFile));
  }
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefExportXML::setMessagePresetLibrary(const MessagePresetLibraryExport* presetLibrary)
{
  std::string presetLibraryFilename = ConvertToRelativePath(presetLibrary->getDestFilename());

  const MessagePresetLibraryExportXML* messagePresetLibraryXML =
    dynamic_cast<const MessagePresetLibraryExportXML*>(presetLibrary);

  MessagePresetLibraryRef* ref =
    new MessagePresetLibraryRef(messagePresetLibraryXML->m_xmlElement->getGUID(), presetLibraryFilename.c_str());

  m_xmlElement->LinkEndChild(ref);
}

//----------------------------------------------------------------------------------------------------------------------
MessagePresetLibraryExport* NetworkDefExportXML::getMessagePresetLibrary()
{
  MessagePresetLibraryRef* libraryRef = (MessagePresetLibraryRef*)m_xmlElement->findChild("MessagePresetLibraryRef");
  if (libraryRef != 0)
  {
    std::string loadFilename = GetDirectory(m_destFilename.c_str());
    const char* filename = libraryRef->getFilename();
    loadFilename.append(filename);
    AssetExport* asset = m_exportFactory->loadAsset(loadFilename.c_str());

    if (asset != 0)
    {
      NMP_ASSERT_MSG(
        dynamic_cast<MessagePresetLibraryExport*>(asset) != NULL,
        "Invalid usage - loaded asset is not an AnimationLibraryExport instance!");
      MessagePresetLibraryExport* messagePresetLibrary = static_cast<MessagePresetLibraryExport*>(asset);

      return messagePresetLibrary;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const MessagePresetLibraryExport* NetworkDefExportXML::getMessagePresetLibrary() const
{
  MessagePresetLibraryRef* libraryRef = (MessagePresetLibraryRef*)m_xmlElement->findChild("MessagePresetLibraryRef");
  if (libraryRef != 0)
  {
    std::string loadFilename = GetDirectory(m_destFilename.c_str());
    const char* filename = libraryRef->getFilename();
    loadFilename.append(filename);
    const MessagePresetLibraryExport* messagePresetLibrary =
      static_cast<const MessagePresetLibraryExport*>(m_exportFactory->getCachedAsset(loadFilename));
    return messagePresetLibrary;
  }
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
const char* NetworkDefExportXML::getName() const
{
  return m_xmlElement->getAttribute("name");
}

//----------------------------------------------------------------------------------------------------------------------
GUID NetworkDefExportXML::getGUID() const
{
  return m_xmlElement->getGUID();
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefExportXML::setGUID(const GUID newGuid)
{
  return m_xmlElement->setGUID(newGuid);
}

//----------------------------------------------------------------------------------------------------------------------
const char* NetworkDefExportXML::getDestFilename() const
{
  return m_destFilename.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int NetworkDefExportXML::getRootNodeID() const
{
  unsigned int val;
  m_xmlElement->getChildUInt("rootNodeNetworkID", val);
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefExportXML::getNetworkWorldOrientation(
  NMP::Vector3& forwardVector,
  NMP::Vector3& rightVector,
  NMP::Vector3& upVector)
{
  m_xmlElement->getChildVector3("networkWorldForwardVector", forwardVector);
  m_xmlElement->getChildVector3("networkWorldRightVector", rightVector);
  m_xmlElement->getChildVector3("networkWorldUpVector", upVector);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int NetworkDefExportXML::getNumNodes() const
{
  return (unsigned int)m_nodes.size();
}

//----------------------------------------------------------------------------------------------------------------------
NodeExport* NetworkDefExportXML::getNode(unsigned int index)
{
  return m_nodes[index];
}

//----------------------------------------------------------------------------------------------------------------------
const NodeExport* NetworkDefExportXML::getNode(unsigned int index) const
{
  return m_nodes[index];
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int NetworkDefExportXML::getNumMessages() const
{
  size_t count = m_messages.size();
  return static_cast<unsigned int>(count);
}

//----------------------------------------------------------------------------------------------------------------------
MessageExport* NetworkDefExportXML::getMessage(unsigned int index)
{
  return m_messages[index];
}

//----------------------------------------------------------------------------------------------------------------------
const MessageExport* NetworkDefExportXML::getMessage(unsigned int index) const
{
  return m_messages[index];
}

//----------------------------------------------------------------------------------------------------------------------
bool NetworkDefExportXML::write()
{
  return m_xmlElement->saveFile(m_destFilename.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
NetworkDefExportXML::~NetworkDefExportXML()
{
  for (unsigned int i = 0 ; i < m_nodes.size() ; ++i)
  {
    delete m_nodes[i];
  }

  for (unsigned int i = 0 ; i < m_messages.size() ; ++i)
  {
    delete m_messages[i];
  }

  delete m_xmlElement;
}

//----------------------------------------------------------------------------------------------------------------------
const char* NetworkDefExportXML::getAnimationLibraryFileName() const
{
  AnimLibraryRef* libRef = (AnimLibraryRef*) m_xmlElement->findChild("AnimLibraryRef");
  if (libRef != 0)
  {
    return libRef->getFilename();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const char* NetworkDefExportXML::getAnimationLibraryGuid() const
{
  AnimLibraryRef* libRef = (AnimLibraryRef*) m_xmlElement->findChild("AnimLibraryRef");
  if (libRef != 0)
  {
    return libRef->getGUID();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const char* NetworkDefExportXML::getPresetLibraryGuid() const
{
  MessagePresetLibraryRef* libRef = (MessagePresetLibraryRef*) m_xmlElement->findChild("MessagePresetLibraryRef");
  if (libRef != 0)
  {
    return libRef->getGUID();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
// JointExportXML
//----------------------------------------------------------------------------------------------------------------------
JointExportXML::JointExportXML(XMLElement* element) :
  m_xmlElement(element)
{
}

//----------------------------------------------------------------------------------------------------------------------
JointExportXML::JointExportXML(
  unsigned int   index,
  const wchar_t* jointName,
  unsigned int   parentIndex) :
  m_xmlElement(new XMLElement("Joint"))
{
  m_xmlElement->setUIntAttribute("index", index);
  m_xmlElement->setAttribute("name", jointName);
  m_xmlElement->setUIntAttribute("parent", parentIndex);
}

//----------------------------------------------------------------------------------------------------------------------
void JointExportXML::setName(const char* name)
{
  m_xmlElement->setAttribute("name", name);
}

//----------------------------------------------------------------------------------------------------------------------
void JointExportXML::setRotation(float x, float y, float z, float w)
{
  static char buf[128];
  sprintf(buf, "%f, %f, %f, %f", x, y, z, w);
  m_xmlElement->setChildText("Rotation", buf);
}

//----------------------------------------------------------------------------------------------------------------------
void JointExportXML::setTranslation(float x, float y, float z)
{
  static char buf[128];
  sprintf(buf, "%f, %f, %f", x, y, z);
  m_xmlElement->setChildText("Translation", buf);
}

//----------------------------------------------------------------------------------------------------------------------
void JointExportXML::setVisiblity(bool visible)
{
  if (visible)
  {
    m_xmlElement->setAttribute("visible", "true");
  }
  else
  {
    m_xmlElement->setAttribute("visible", "false");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void JointExportXML::setBodyPlanTag(const char* tag)
{
  if (tag)
  {
    m_xmlElement->setAttribute("BodyPlanTag", tag);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void JointExportXML::setRetargetingTag(const char* tag)
{
  if (tag)
  {
    m_xmlElement->setAttribute("RetargetTag", tag);
  }
}

//----------------------------------------------------------------------------------------------------------------------
const char* JointExportXML::getName() const
{
  return m_xmlElement->getAttribute("name");
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int JointExportXML::getIndex() const
{
  unsigned int val = 0;
  m_xmlElement->getUIntAttribute("index", val);
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int JointExportXML::getParentIndex() const
{
  unsigned int val = 0;
  m_xmlElement->getUIntAttribute("parent", val);
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
void JointExportXML::getRotation(float& x, float& y, float& z, float& w) const
{
  sscanf(m_xmlElement->getChildText("Rotation"),
         "%f, %f, %f, %f",
         &x, &y, &z, &w);
}

//----------------------------------------------------------------------------------------------------------------------
void JointExportXML::getTranslation(float& x, float& y, float& z) const
{
  sscanf(m_xmlElement->getChildText("Translation"),
         "%f, %f, %f",
         &x, &y, &z);
}

//----------------------------------------------------------------------------------------------------------------------
bool JointExportXML::getVisiblity() const
{
  std::string result = m_xmlElement->getAttribute("visible");
  return (result == "true");
}

//----------------------------------------------------------------------------------------------------------------------
const char* JointExportXML::getBodyPlanTag() const
{
  return m_xmlElement->getAttribute("BodyPlanTag");
}

//----------------------------------------------------------------------------------------------------------------------
const char* JointExportXML::getRetargetingTag() const
{
  return m_xmlElement->getAttribute("RetargetTag");
}

//----------------------------------------------------------------------------------------------------------------------
// JointLimitExportXML
//----------------------------------------------------------------------------------------------------------------------
void JointLimitExportXML::setLimitType(JointLimitExport::LimitType type)
{
  switch (type)
  {
  case JointLimitExport::kHingeLimitType:
    m_xmlElement->setAttribute("JointType", "Hinge");
    break;
  case JointLimitExport::kBallSocketLimitType:
    m_xmlElement->setAttribute("JointType", "BallSocket");
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void JointLimitExportXML::setJointIndex(unsigned int index)
{
  m_xmlElement->setUIntAttribute("JointIndex", index);
}

//----------------------------------------------------------------------------------------------------------------------
void JointLimitExportXML::setOrientation(float x, float y, float z, float w)
{
  static char buf[128];
  sprintf(buf, "%f, %f, %f, %f", x, y, z, w);
  m_xmlElement->setChildText("Orientation", buf);
}

//----------------------------------------------------------------------------------------------------------------------
void JointLimitExportXML::setOffsetOrientation(float x, float y, float z, float w)
{
  static char buf[128];
  sprintf(buf, "%f, %f, %f, %f", x, y, z, w);
  m_xmlElement->setChildText("OffsetOrientation", buf);
}

//----------------------------------------------------------------------------------------------------------------------
void JointLimitExportXML::setTwistLow(float twist) const
{
  m_xmlElement->setFloatAttribute("TwistLow", twist);
}

//----------------------------------------------------------------------------------------------------------------------
void JointLimitExportXML::setTwistHigh(float twist) const
{
  m_xmlElement->setFloatAttribute("TwistHigh", twist);
}

//----------------------------------------------------------------------------------------------------------------------
void JointLimitExportXML::setSwing1(float swing) const
{
  m_xmlElement->setFloatAttribute("Swing1", swing);
}

//----------------------------------------------------------------------------------------------------------------------
void JointLimitExportXML::setSwing2(float swing) const
{
  m_xmlElement->setFloatAttribute("Swing2", swing);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int JointLimitExportXML::getIndex() const
{
  unsigned int val = 0;
  m_xmlElement->getUIntAttribute("index", val);
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
JointLimitExport::LimitType JointLimitExportXML::getLimitType() const
{
  const char* type = m_xmlElement->getAttribute("JointType");
  JointLimitExport::LimitType result = JointLimitExport::kHingeLimitType;
  if (strcmp(type, "Hinge") == 0)
  {
    result = JointLimitExport::kHingeLimitType;
  }
  else if (strcmp(type, "BallSocket") == 0)
  {
    result = JointLimitExport::kBallSocketLimitType;
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int JointLimitExportXML::getJointIndex() const
{
  unsigned int index = JointExport::WORLD_JOINT_INDEX;
  m_xmlElement->getUIntAttribute("JointIndex", index);
  return index;
}

//----------------------------------------------------------------------------------------------------------------------
void JointLimitExportXML::getOrientation(float& x, float& y, float& z, float& w) const
{
  sscanf(m_xmlElement->getChildText("Orientation"),
         "%f, %f, %f, %f",
         &x, &y, &z, &w);
}

//----------------------------------------------------------------------------------------------------------------------
void JointLimitExportXML::getOffsetOrientation(float& x, float& y, float& z, float& w) const
{
  sscanf(m_xmlElement->getChildText("OffsetOrientation"),
         "%f, %f, %f, %f",
         &x, &y, &z, &w);
}

//----------------------------------------------------------------------------------------------------------------------
float JointLimitExportXML::getTwistLow() const
{
  float twist = 0.0f;
  m_xmlElement->getFloatAttribute("TwistLow", twist);
  return twist;
}

//----------------------------------------------------------------------------------------------------------------------
float JointLimitExportXML::getTwistHigh() const
{
  float twist = 0.0f;
  m_xmlElement->getFloatAttribute("TwistHigh", twist);
  return twist;
}

//----------------------------------------------------------------------------------------------------------------------
float JointLimitExportXML::getSwing1() const
{
  float swing = 0.0f;
  m_xmlElement->getFloatAttribute("Swing1", swing);
  return swing;
}

//----------------------------------------------------------------------------------------------------------------------
float JointLimitExportXML::getSwing2() const
{
  float swing = 0.0f;
  m_xmlElement->getFloatAttribute("Swing2", swing);
  return swing;
}

//----------------------------------------------------------------------------------------------------------------------
JointLimitExportXML::JointLimitExportXML(XMLElement* element) :
  m_xmlElement(element)
{

}

//----------------------------------------------------------------------------------------------------------------------
JointLimitExportXML::JointLimitExportXML(unsigned int index) :
  m_xmlElement(new XMLElement("JointLimit"))
{
  m_xmlElement->setUIntAttribute("index", index);
}

//----------------------------------------------------------------------------------------------------------------------
/// IntMirrorMappingXML
//----------------------------------------------------------------------------------------------------------------------
unsigned int IntMirrorMappingXML::getIndex() const
{
  unsigned int val = 0;
  m_xmlElement->getUIntAttribute("index", val);
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int IntMirrorMappingXML::getFirst() const
{
  unsigned int val = 0;
  m_xmlElement->getUIntAttribute("first", val);
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int IntMirrorMappingXML::getSecond() const
{
  unsigned int val = 0;
  m_xmlElement->getUIntAttribute("second", val);
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
IntMirrorMappingXML::IntMirrorMappingXML(XMLElement* element) :
  m_xmlElement(element)
{

}

//----------------------------------------------------------------------------------------------------------------------
IntMirrorMappingXML::IntMirrorMappingXML(
  const char*  name,
  unsigned int index,
  unsigned int firstJointIndex,
  unsigned int secondJointIndex) :
  m_xmlElement(new XMLElement(name))
{
  m_xmlElement->setUIntAttribute("index", index);
  m_xmlElement->setUIntAttribute("first", firstJointIndex);
  m_xmlElement->setUIntAttribute("second", secondJointIndex);
}

//----------------------------------------------------------------------------------------------------------------------
// RigExportXML
//----------------------------------------------------------------------------------------------------------------------
RigExportXML::~RigExportXML()
{
  for (uint32_t i = 0 ; i < m_joints.size() ; ++i)
  {
    delete m_joints[i];
  }
  for (uint32_t i = 0 ; i < m_jointLimits.size() ; ++i)
  {
    delete m_jointLimits[i];
  }
  for (uint32_t i = 0 ; i < m_jointMirrorMappings.size() ; ++i)
  {
    delete m_jointMirrorMappings[i];
  }
  for (uint32_t i = 0 ; i < m_jointMappings.size() ; ++i)
  {
    delete m_jointMappings[i];
  }
  delete m_xmlElement;
}

//----------------------------------------------------------------------------------------------------------------------
RigExportXML::RigExportXML(XMLElement* element, const std::string& destFilename, ExportFactoryXML* factory) :
  m_xmlElement(element),
  m_destFilename(destFilename)
{
  BuildIndexedNodeList(JointExportXML, m_joints, m_xmlElement, "Joint");
  BuildIndexedNodeList(JointLimitExportXML, m_jointLimits, m_xmlElement, "JointLimit");
  BuildIndexedNodeList(IntMirrorMappingXML, m_jointMirrorMappings, m_xmlElement, "JointMirrorMapping");
  BuildIndexedNodeList(IntMirrorMappingXML, m_jointMappings, m_xmlElement, "JointMapping");
}

//----------------------------------------------------------------------------------------------------------------------
RigExportXML::RigExportXML(GUID guid, const wchar_t* destFilename, const char* rigName) :
  m_xmlElement(new XMLElement("Rig")),
  m_destFilename(toUTF8(destFilename))
{
  m_xmlElement->setGUID(guid);
  m_xmlElement->setAttribute("name", rigName);
}

//----------------------------------------------------------------------------------------------------------------------
void RigExportXML::setGUID(const GUID guid)
{
  m_xmlElement->setGUID(guid);
}

//----------------------------------------------------------------------------------------------------------------------
void RigExportXML::setDestFilename(const char* filename)
{
  m_destFilename = filename;
}

//----------------------------------------------------------------------------------------------------------------------
JointExport* RigExportXML::createJoint(
  unsigned int   index,
  const wchar_t* jointName,
  unsigned int   parentIndex)
{
  JointExportXML* joint = new JointExportXML(index, jointName, parentIndex);
  joint->m_xmlElement->setChildText("OrientationOffset", "0.0, 0.0, 0.0, 1.0");
  joint->m_xmlElement->setChildText("TranslationOffset", "0.0, 0.0, 0.0");
  InsertIntoIndexedList(m_joints, index, joint);
  m_xmlElement->LinkEndChild(joint->m_xmlElement);
  return joint;
}

//----------------------------------------------------------------------------------------------------------------------
JointLimitExport* RigExportXML::createJointLimit(unsigned int index)
{
  JointLimitExportXML* jointLimit = new JointLimitExportXML(index);
  InsertIntoIndexedList(m_jointLimits, index, jointLimit);
  m_xmlElement->LinkEndChild(jointLimit->m_xmlElement);
  return jointLimit;
}

//----------------------------------------------------------------------------------------------------------------------
void RigExportXML::createMirrorMapping(
  unsigned     index,
  unsigned int firstJointIndex,
  unsigned int secondJointIndex)
{
  IntMirrorMappingXML* mapping =
    new IntMirrorMappingXML("JointMirrorMapping", index, firstJointIndex, secondJointIndex);
  InsertIntoIndexedList(m_jointMirrorMappings, index, mapping);
  m_xmlElement->LinkEndChild(mapping->m_xmlElement);
}

//----------------------------------------------------------------------------------------------------------------------
void RigExportXML::createJointMapping(
  unsigned     index,
  unsigned int firstJointIndex,
  unsigned int secondJointIndex)
{
  IntMirrorMappingXML* mapping =
    new IntMirrorMappingXML("JointMapping", index, firstJointIndex, secondJointIndex);
  InsertIntoIndexedList(m_jointMappings, index, mapping);
  m_xmlElement->LinkEndChild(mapping->m_xmlElement);
}

//----------------------------------------------------------------------------------------------------------------------
void RigExportXML::setJointOffsetTransform(
  unsigned int        index,
  const NMP::Vector3& translation,
  const NMP::Quat&    orientation)
{
  static char buffer[128];
  sprintf(buffer, "%f, %f, %f", translation.x, translation.y, translation.z);
  m_joints[index]->m_xmlElement->setChildText("TranslationOffset", buffer);
  sprintf(buffer, "%f, %f, %f, %f", orientation.x, orientation.y, orientation.z, orientation.w);
  m_joints[index]->m_xmlElement->setChildText("OrientationOffset", buffer);
}


//----------------------------------------------------------------------------------------------------------------------
void RigExportXML::setJointPostOffsetTransform(
  unsigned int        index,
  const NMP::Quat&    postOffsetOrient)
{
  static char buffer[128];
  sprintf(buffer, "%f, %f, %f, %f", postOffsetOrient.x, postOffsetOrient.y, postOffsetOrient.z, postOffsetOrient.w);
  m_joints[index]->m_xmlElement->setChildText("PostOrientationOffset", buffer);
}

//----------------------------------------------------------------------------------------------------------------------
void RigExportXML::setHipIndex(unsigned int hipIndex)
{
  m_xmlElement->setChildUInt("hipIndex", hipIndex);
}

//----------------------------------------------------------------------------------------------------------------------
void RigExportXML::setTrajectoryIndex(unsigned int trajectoryIndex)
{
  m_xmlElement->setChildUInt("trajectoryIndex", trajectoryIndex);
}

//----------------------------------------------------------------------------------------------------------------------
void RigExportXML::setBlendFrameOrientationQuat(float x, float y, float z, float w)
{
  static char buf[128];
  sprintf(buf, "%f, %f, %f, %f", x, y, z, w);
  m_xmlElement->setChildText("orientationQuat", buf);
}

//----------------------------------------------------------------------------------------------------------------------
void RigExportXML::setBlendFramePositionVec(float x, float y, float z)
{
  static char buf[128];
  sprintf(buf, "%f, %f, %f", x, y, z);
  m_xmlElement->setChildText("positionVec", buf);
}

//----------------------------------------------------------------------------------------------------------------------
void RigExportXML::setRigSourceFileName(const char* sourceFileName)
{
  XMLElement* childElement = m_xmlElement->setChildText("rigSourceFileName", sourceFileName);
  NM::TiXmlNode* childTextN = childElement->FirstChild();
  NM::TiXmlText* childText = childTextN->ToText();
  if (childText)
  {
    childText->SetCDATA(true);
  }
}

//----------------------------------------------------------------------------------------------------------------------
GUID RigExportXML::getGUID() const
{
  return m_xmlElement->getGUID();
}

//----------------------------------------------------------------------------------------------------------------------
const char* RigExportXML::getDestFilename() const
{
  return m_destFilename.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
const char* RigExportXML::getName() const
{
  return m_xmlElement->getAttribute("name");
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int RigExportXML::getHipIndex() const
{
  unsigned int val = (unsigned int) - 1;
  m_xmlElement->getChildUInt("hipIndex", val);
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int RigExportXML::getTrajectoryIndex() const
{
  unsigned int val = (unsigned int) - 1;
  m_xmlElement->getChildUInt("trajectoryIndex", val);
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
void RigExportXML::getBlendFrameOrientationQuat(float& x, float& y, float& z, float& w) const
{
  sscanf(m_xmlElement->getChildText("orientationQuat"),
         "%f, %f, %f, %f",
         &x, &y, &z, &w);
}

//----------------------------------------------------------------------------------------------------------------------
void RigExportXML::getBlendFramePositionVec(float& x, float& y, float& z) const
{
  sscanf(m_xmlElement->getChildText("positionVec"),
         "%f, %f, %f",
         &x, &y, &z);
}

//----------------------------------------------------------------------------------------------------------------------
const char* RigExportXML::getRigSourceFileName() const
{
  return m_xmlElement->getChildText("rigSourceFileName");
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int RigExportXML::getNumJoints() const
{
  return (unsigned int)m_joints.size();
}

//----------------------------------------------------------------------------------------------------------------------
JointExport* RigExportXML::getJoint(unsigned int index)
{
  return m_joints[index];
}

//----------------------------------------------------------------------------------------------------------------------
const JointExport* RigExportXML::getJoint(unsigned int index) const
{
  return m_joints[index];
}

//----------------------------------------------------------------------------------------------------------------------
float RigExportXML::getRigScaleFactor() const
{
  float rigScale = 1.0f;
  m_xmlElement->getFloatAttribute("RigScale", rigScale);
  return rigScale;
}

//----------------------------------------------------------------------------------------------------------------------
void RigExportXML::setRigScaleFactor(float rigScale)
{
  m_xmlElement->setFloatAttribute("RigScale", rigScale);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int RigExportXML::getNumJointLimits() const
{
  return (unsigned int)m_jointLimits.size();
}

//----------------------------------------------------------------------------------------------------------------------
JointLimitExport* RigExportXML::getJointLimit(unsigned int index)
{
  return m_jointLimits[index];
}

//----------------------------------------------------------------------------------------------------------------------
const JointLimitExport* RigExportXML::getJointLimit(unsigned int index) const
{
  return m_jointLimits[index];
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int RigExportXML::getMirrorMappingCount() const
{
  return (unsigned int)m_jointMirrorMappings.size();
}

//----------------------------------------------------------------------------------------------------------------------
void RigExportXML::getMirrorMapping(
  unsigned int  index,
  unsigned int& firstJointIndex,
  unsigned int& secondJointIndex) const
{
  IntMirrorMappingXML* mapping = m_jointMirrorMappings[index];
  firstJointIndex = mapping->getFirst();
  secondJointIndex = mapping->getSecond();
}

//----------------------------------------------------------------------------------------------------------------------
float RigExportXML::getRigRetargetScale() const
{
  float scale = 1.0f;
  m_xmlElement->getChildFloat("rigRetargetScale", scale);
  return scale;
}

//----------------------------------------------------------------------------------------------------------------------
void RigExportXML::setRigRetargetScale(float scale)
{
  m_xmlElement->setChildFloat("rigRetargetScale", scale);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int RigExportXML::getJointMappingCount() const
{
  return (unsigned int)m_jointMappings.size();
}

//----------------------------------------------------------------------------------------------------------------------
void RigExportXML::getJointMapping(
  unsigned int  index,
  unsigned int& firstJointIndex,
  unsigned int& secondJointIndex) const
{
  IntMirrorMappingXML* mapping = m_jointMappings[index];
  firstJointIndex = mapping->getFirst();
  secondJointIndex = mapping->getSecond();
}

//----------------------------------------------------------------------------------------------------------------------
void RigExportXML::getJointOffsetTransform(
  unsigned int  index,
  NMP::Vector3& translation,
  NMP::Quat&    orientation) const
{
  JointExportXML* joint = m_joints[index];
  sscanf(
    joint->m_xmlElement->getChildText("TranslationOffset"),
    "%f, %f, %f",
    &translation.x,
    &translation.y,
    &translation.z);
  sscanf(
    joint->m_xmlElement->getChildText("OrientationOffset"),
    "%f, %f, %f, %f",
    &orientation.x,
    &orientation.y,
    &orientation.z,
    &orientation.w);
}

//----------------------------------------------------------------------------------------------------------------------
void RigExportXML::getJointPostOffsetTransform(
  unsigned int  index,
  NMP::Quat&    postOrientation) const
{
  JointExportXML* joint = m_joints[index];
  sscanf(
    joint->m_xmlElement->getChildText("PostOrientationOffset"),
    "%f, %f, %f, %f",
    &postOrientation.x,
    &postOrientation.y,
    &postOrientation.z,
    &postOrientation.w);
}

//----------------------------------------------------------------------------------------------------------------------
bool RigExportXML::write()
{
  return m_xmlElement->saveFile(m_destFilename.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t RigExportXML::getMirrorPlane() const
{
  unsigned int mirrorPlane = 0;
  m_xmlElement->getChildUInt("mirrorPlane", mirrorPlane);
  return mirrorPlane;
}

//----------------------------------------------------------------------------------------------------------------------
void RigExportXML::setMirrorPlane(uint32_t plane)
{
  m_xmlElement->setChildUInt("mirrorPlane", plane);
}

//----------------------------------------------------------------------------------------------------------------------
/// AnimationPoseDefExportXML
//----------------------------------------------------------------------------------------------------------------------
const char* AnimationPoseDefExportXML::getName() const
{
  const char* name = m_xmlElement->getAttribute("name");
  return name;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimationPoseDefExportXML::setName(const char* name)
{
  m_xmlElement->setChildText("name", name);
}

//----------------------------------------------------------------------------------------------------------------------
void AnimationPoseDefExportXML::setAnimationFile(const wchar_t* file)
{
  m_xmlElement->setChildText("File", file);
}

//----------------------------------------------------------------------------------------------------------------------
void AnimationPoseDefExportXML::setAnimationTake(const wchar_t* take)
{
  m_xmlElement->setChildText("Take", take);
}

//----------------------------------------------------------------------------------------------------------------------
void AnimationPoseDefExportXML::setPoseFrameIndex(int32_t frame)
{
  m_xmlElement->setChildUInt("Frame", static_cast<int32_t>(frame));
}

//----------------------------------------------------------------------------------------------------------------------
const char* AnimationPoseDefExportXML::getAnimationFile() const
{
  const char* file = m_xmlElement->getChildText("File");
  return file;
}

//----------------------------------------------------------------------------------------------------------------------
const char* AnimationPoseDefExportXML::getAnimationTake() const
{
  const char* take = m_xmlElement->getChildText("Take");
  return take;
}

//----------------------------------------------------------------------------------------------------------------------
int32_t AnimationPoseDefExportXML::getPoseFrameIndex() const
{
  unsigned int frame = 0;
  m_xmlElement->getChildUInt("Frame", frame);
  return static_cast<int32_t>(frame);
}

//----------------------------------------------------------------------------------------------------------------------
AnimationPoseDefExportXML::AnimationPoseDefExportXML(XMLElement* element)
: m_xmlElement(element)
{
}

//----------------------------------------------------------------------------------------------------------------------
AnimationPoseDefExportXML::AnimationPoseDefExportXML(XMLElement* parent, const char* poseName)
: m_xmlElement(new XMLElement("AnimationPose"))
{
  parent->LinkEndChild(m_xmlElement);
  m_xmlElement->setAttribute("name", poseName);
}

//----------------------------------------------------------------------------------------------------------------------
AnimationPoseDefExportXML::~AnimationPoseDefExportXML()
{
  delete m_xmlElement;
}

//----------------------------------------------------------------------------------------------------------------------
// ExportFactoryXML
//----------------------------------------------------------------------------------------------------------------------
ExportFactoryXML::ExportFactoryXML()
{

}

//----------------------------------------------------------------------------------------------------------------------
ExportFactoryXML::~ExportFactoryXML()
{
  std::map<std::string, XMLAsset>::iterator assetIt = m_fileAssetMap.begin();

  while (assetIt != m_fileAssetMap.end())
  {
    NM::TiXmlDocument* doc = (*assetIt).second.m_document;
    doc->UnlinkChild(doc->RootElement());
    delete doc;
    delete (*assetIt).second.m_asset;

    ++assetIt;
  }

  m_fileAssetMap.clear();
}

//----------------------------------------------------------------------------------------------------------------------
NetworkDefExport* ExportFactoryXML::createNetworkDef(
  const GUID     revisionGUID,
  const wchar_t* networkName,
  const wchar_t* destFilename)
{
  NetworkDefExportXML* netDef = new NetworkDefExportXML(revisionGUID, networkName, destFilename);
  return netDef;
}

//----------------------------------------------------------------------------------------------------------------------
void ExportFactoryXML::destroyNetworkDef(NetworkDefExport* netDef)
{
  delete netDef;
}

//----------------------------------------------------------------------------------------------------------------------
TakeExport* ExportFactoryXML::createTake(
  const wchar_t* listName,
  float          secDuration,
  float          fps,
  const wchar_t* destFilename)

{
  return new TakeExportXML(listName, secDuration, fps, destFilename);
}

//----------------------------------------------------------------------------------------------------------------------
void ExportFactoryXML::destroyTake(TakeExport* list)
{
  delete list;
}

//----------------------------------------------------------------------------------------------------------------------
TakeList* ExportFactoryXML::createTakeList(const wchar_t* sourceAnimFile,
    const wchar_t* destFilename)
{
  static const int TAKE_LIST_FORMAT = 1; // update this number if we ever change how event data is stored
  return new TakeListXML(sourceAnimFile, TAKE_LIST_FORMAT, destFilename);
}

//----------------------------------------------------------------------------------------------------------------------
void ExportFactoryXML::destroyTakeList(TakeList* takeList)
{
  delete takeList;
}

//----------------------------------------------------------------------------------------------------------------------
AnimationLibraryExport* ExportFactoryXML::createAnimationLibrary(
  GUID           guid,
  const wchar_t* animLibraryName,
  const wchar_t* destFilename)
{
  return new AnimationLibraryXML(guid, animLibraryName, destFilename);
}

//----------------------------------------------------------------------------------------------------------------------
void ExportFactoryXML::destroyAnimationLibrary(AnimationLibraryExport* animLibrary)
{
  delete animLibrary;
}

//----------------------------------------------------------------------------------------------------------------------
RigExport* ExportFactoryXML::createRig(GUID guid, const wchar_t* destFilename, const char* rigName)
{
  return new RigExportXML(guid, destFilename, rigName);
}

//----------------------------------------------------------------------------------------------------------------------
void ExportFactoryXML::destroyRig(RigExport* rig)
{
  delete rig;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigDefExport* ExportFactoryXML::createPhysicsRigDef(GUID guid, const wchar_t* destFilename, const char* rigName)
{
  return new PhysicsRigDefExportXML(guid, destFilename, rigName);
}

//----------------------------------------------------------------------------------------------------------------------
void ExportFactoryXML::destroyPhysicsRigDef(PhysicsRigDefExport* physicsRigDef)
{
  delete physicsRigDef;
}

//----------------------------------------------------------------------------------------------------------------------
BodyDefExport* ExportFactoryXML::createBodyDef(GUID guid, const wchar_t* destFilename, const char* bodyName)
{
  return new BodyDefExportXML(guid, destFilename, bodyName);
}

//----------------------------------------------------------------------------------------------------------------------
void ExportFactoryXML::destroyBodyDef(BodyDefExport* bodyDef)
{
  delete bodyDef;
}

//----------------------------------------------------------------------------------------------------------------------
CharacterControllerExport* ExportFactoryXML::createCharacterController(GUID guid, const wchar_t* destFilename, const char* controllerName)
{
  return new CharacterControllerExportXML(guid, destFilename, controllerName);
}

//----------------------------------------------------------------------------------------------------------------------
void ExportFactoryXML::destroyCharacterController(CharacterControllerExport* characterController)
{
  delete characterController;
}

//----------------------------------------------------------------------------------------------------------------------
InteractionProxyExport* ExportFactoryXML::createInteractionProxy(GUID guid, const wchar_t* destFilename, const char* proxyName)
{
  return new InteractionProxyExportXML(guid, destFilename, proxyName);
}

//----------------------------------------------------------------------------------------------------------------------
void ExportFactoryXML::destroyInteractionProxy(InteractionProxyExport* interactionProxy)
{
  delete interactionProxy;
}

//----------------------------------------------------------------------------------------------------------------------
MessagePresetLibraryExport* ExportFactoryXML::createMessagePresetLibrary(GUID guid, const wchar_t* destinationFilename)
{
  return new MessagePresetLibraryExportXML(guid, destinationFilename);
}

//----------------------------------------------------------------------------------------------------------------------
void ExportFactoryXML::destroyMessagePresetLibrary(MessagePresetLibraryExport* presetsLibrary)
{
  delete presetsLibrary;
}


//----------------------------------------------------------------------------------------------------------------------
AnalysisLibraryExport* ExportFactoryXML::createAnalysisLibrary(GUID guid, const wchar_t* destinationFilename, const wchar_t* resultFilename)
{
  return new AnalysisLibraryExportXML(guid, destinationFilename, resultFilename);
}

//----------------------------------------------------------------------------------------------------------------------
void ExportFactoryXML::destroyAnalysisLibrary(AnalysisLibraryExport* analysisLibrary)
{
  delete analysisLibrary;
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisResultLibraryExport* ExportFactoryXML::createAnalysisResultLibrary(GUID guid, const wchar_t* destinationFilename)
{
  return new AnalysisResultLibraryExportXML(guid, toUTF8(destinationFilename).c_str());
}
  
//----------------------------------------------------------------------------------------------------------------------
void ExportFactoryXML::destroyAnalysisResultLibrary(AnalysisResultLibraryExport* analysisResultLibrary)
{
  delete analysisResultLibrary;
}

//----------------------------------------------------------------------------------------------------------------------
AssetExport* ExportFactoryXML::loadAsset(const char* srcFilename)
{
  std::string fileStr(srcFilename);

  std::map<std::string, XMLAsset>::iterator assetIt = m_fileAssetMap.find(fileStr);
  if (assetIt != m_fileAssetMap.end())
  {
    return (*assetIt).second.m_asset;
  }

  size_t xmlFileSize = 0;

  // Load asset source xml
  const char* assetSourceXML = doLoadAssetSource(srcFilename, xmlFileSize);
  if (assetSourceXML == 0 || xmlFileSize == 0)
  {
    return 0;
  }

  // Parse the xml
  NM::TiXmlDocument* doc = new NM::TiXmlDocument();
  doc->Parse(assetSourceXML);

  // Free the source data
  doFreeAssetSource(assetSourceXML);

  // Check the root tag if it's valid
  const char* rootTag = "";
  XMLElement* root = (XMLElement*)doc->RootElement();
  AssetExport* retAsset = 0;

  if (root != 0)
  {
    rootTag = root->Value();
  }

  if (strcmp(rootTag, "NetworkDefinition") == 0)
  {
    retAsset = new NetworkDefExportXML(root, srcFilename, this);
  }
  else if (strcmp(rootTag, "Take") == 0)
  {
    retAsset = new TakeExportXML(root, srcFilename);
  }
  else if (strcmp(rootTag, "TakeList") == 0)
  {
    retAsset = new TakeListXML(root, srcFilename);
  }
  else if (strcmp(rootTag, "AnimationLibrary") == 0)
  {
    retAsset = new AnimationLibraryXML(root, srcFilename, this);
  }
  else if (strcmp(rootTag, "MessagePresetLibrary") == 0)
  {
    retAsset = new MessagePresetLibraryExportXML(root, srcFilename);
  }
  else if (strcmp(rootTag, "Rig") == 0)
  {
    retAsset = new RigExportXML(root, srcFilename, this);
  }
  else if (strcmp(rootTag, "PhysicsRigDef") == 0)
  {
    retAsset = new PhysicsRigDefExportXML(root, srcFilename);
  }
  else if (strcmp(rootTag, "BodyDef") == 0)
  {
    retAsset = new BodyDefExportXML(root, this, srcFilename);
  }
  else if (strcmp(rootTag, "CharacterController") == 0)
  {
    retAsset = new CharacterControllerExportXML(root, srcFilename);
  }
  else if (strcmp(rootTag, "InteractionProxy") == 0)
  {
    retAsset = new InteractionProxyExportXML(root, srcFilename);
  }
  else if (strcmp(rootTag, "AnalysisLibrary") == 0)
  {
    retAsset = new AnalysisLibraryExportXML(root, srcFilename);
  }
  else if (strcmp(rootTag, "AnalysisResultLibrary") == 0)
  {
    retAsset = new AnalysisResultLibraryExportXML(root, srcFilename);
  }

  // Record this asset if it was loaded correctly
  if (retAsset != 0)
  {
    XMLAsset xmlAsset;
    xmlAsset.m_document = doc;
    xmlAsset.m_asset = retAsset;

    m_fileAssetMap.insert(std::pair<std::string, XMLAsset>(fileStr, xmlAsset));
  }
  else
  {
    delete doc;
  }

  return retAsset;
}

//----------------------------------------------------------------------------------------------------------------------
std::string ExportFactoryXML::findGUIDForAsset(const std::string& srcFilename)
{
  std::map<std::string, XMLAsset>::iterator assetIt = m_fileAssetMap.find(srcFilename);
  if (assetIt != m_fileAssetMap.end())
  {
    return (*assetIt).second.m_document->RootElement()->Attribute("guid");
  }


  // Load asset source xml
  FILE* inFile = fopen(srcFilename.c_str(), "rb");
  if (inFile == 0)
  {
    return "";
  }

  char textBuffer[32]; // more than enough space to contain the first tag
  fseek(inFile, 0, SEEK_SET);

  std::string guid;
  guid.reserve(36);
  bool gotGuid = false;
  size_t patternIndex = 0;
  size_t patternPosition = 0;

  size_t numBeginPatterns = 2;
  const char *beginPatterns[] = { "guid", "=" };

  bool endOfBuffer = false;
  while(!gotGuid && !endOfBuffer)
  {
    endOfBuffer = fread(textBuffer, 1, (size_t)sizeof(textBuffer), inFile) != sizeof(textBuffer);
    char *position = textBuffer;
    
    for(size_t i=0; i<sizeof(textBuffer); ++i, ++position)
    {
      if(patternIndex > numBeginPatterns)
      {
        if(guid.length() >= 36)
        {
          if(*position == '\'' || *position == '"')
          {
            // guid found, break.
            gotGuid = true;
            break;
          }
          else
          {
            guid.clear();
            patternPosition = 0;
            patternIndex = 0;
          }
        }
        // strip the guid characters
        else
        {
          guid.push_back(*position);
        }
      }
      // up to < numBeginPatterns we match patterns, after that we expect a "/' character, then the guid.
      else
      {
        // just ignore spaces between patterns
        if(!isspace(*position))
        {
          if(patternIndex == numBeginPatterns)
          {
            // this should be our first quote.
            if(*position == '\'' || *position == '"')
            {
              patternIndex++;
            }
          }          
          // the current character does not equal the pattern, reset the match.
          else if(*position != beginPatterns[patternIndex][patternPosition])
          {
            patternIndex = 0;
            patternPosition = 0;
          }
          // expect a space before the first pattern, so if this fails, it isnt a real match
          else if(patternIndex != 0 || patternPosition != 0 || isspace(*(position-1)))
          {
            patternPosition++;

            // is a pattern complete? if so check the next one.
            if(patternIndex < numBeginPatterns && beginPatterns[patternIndex][patternPosition] == '\0')
            {
              patternPosition = 0;
              patternIndex++;
            }
          }

        }
        else if(patternPosition != 0)
        {
          patternIndex = 0;
          patternPosition = 0;
        }
      }
    }
  }
  _ASSERT(guid.length() == 36);

  fclose(inFile);
  return guid;
}

//----------------------------------------------------------------------------------------------------------------------
bool ExportFactoryXML::unloadAsset(const char* srcFileName)
{
  std::string fileStr(srcFileName);
  // Is the asset already loaded?
  std::map<std::string, XMLAsset>::iterator assetIt = m_fileAssetMap.find(fileStr);
  if (assetIt != m_fileAssetMap.end())
  {
    NM::TiXmlDocument* doc = (*assetIt).second.m_document;
    doc->UnlinkChild(doc->RootElement());
    delete doc;
    delete (*assetIt).second.m_asset;

    m_fileAssetMap.erase(assetIt);
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
const char* ExportFactoryXML::doLoadAssetSource(const char* srcFilename, size_t& dataSize)
{
  FILE* inFile = fopen(srcFilename, "rb");
  if (inFile == 0)
  {
    return 0;
  }

  fseek(inFile, 0, SEEK_END);

  char* textBuffer = 0;
  dataSize = (size_t)ftell(inFile);

  if (dataSize >= 0)
  {
    textBuffer = (char*)NMPMemoryAllocAligned(dataSize + 1, 4);
    if (textBuffer != 0)
    {
      fseek(inFile, 0, SEEK_SET);
#ifdef NMP_ENABLE_ASSERTS
      size_t bytes =
#endif
        fread(textBuffer, 1, (size_t)dataSize, inFile);
      NMP_ASSERT(bytes == dataSize);
      textBuffer[dataSize] = 0; // null terminate
    }
  }

  fclose(inFile);
  return textBuffer;
}

//----------------------------------------------------------------------------------------------------------------------
void ExportFactoryXML::doFreeAssetSource(const char* assetSource)
{
  NMP::Memory::memFree((void*)assetSource);
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace ME
