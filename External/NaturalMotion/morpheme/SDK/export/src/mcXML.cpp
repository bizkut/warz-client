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
#include "export/mcXML.h"
#include "NMTinyMMIO.h"

#ifdef _MSC_VER
  #pragma warning(disable:4996)
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace ME
{

#define SET_BYTES_BUFFER_LENGTH (256)
static char valueToStringBuffer[(33 * 4) + 7]; // enough for NMP::Quat 4 x float conversion + comma spacing + null term

//----------------------------------------------------------------------------------------------------------------------
std::string toStr(float val)
{
  sprintf_s(valueToStringBuffer, "%f", val);
  return std::string(valueToStringBuffer);
}

//----------------------------------------------------------------------------------------------------------------------
std::string toStr(double val)
{
  sprintf_s(valueToStringBuffer, "%g", val);
  return std::string(valueToStringBuffer);
}

//----------------------------------------------------------------------------------------------------------------------
std::string toStr(int val)
{
  sprintf_s(valueToStringBuffer, "%d", val);
  return std::string(valueToStringBuffer);
}

//----------------------------------------------------------------------------------------------------------------------
std::string toStr(unsigned int val)
{
  sprintf_s(valueToStringBuffer, "%u", val);
  return std::string(valueToStringBuffer);
}

//----------------------------------------------------------------------------------------------------------------------
std::string toStr(int64_t val)
{
  sprintf_s(valueToStringBuffer, "%I64d", val);
  return std::string(valueToStringBuffer);
}

//----------------------------------------------------------------------------------------------------------------------
std::string toStr(uint64_t val)
{
  sprintf_s(valueToStringBuffer, "%I64u", val);
  return std::string(valueToStringBuffer);
}

//----------------------------------------------------------------------------------------------------------------------
std::string toStr(const NMP::Vector3& val)
{
  sprintf_s(valueToStringBuffer, "%f, %f, %f", val.x, val.y, val.z);
  return std::string(valueToStringBuffer);
}

//----------------------------------------------------------------------------------------------------------------------
std::string toStr(const NMP::Quat& val)
{
  sprintf_s(valueToStringBuffer, "%f, %f, %f, %f", val.x, val.y, val.z, val.w);
  return std::string(valueToStringBuffer);
}

//----------------------------------------------------------------------------------------------------------------------
float toFloat(const char* buf)
{
  float val;
  sscanf(buf, "%f", &val);
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
double toDouble(const char* buf)
{
  double val;
  sscanf(buf, "%lf", &val);
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
int toInt(const char* buf)
{
  int val;
  sscanf(buf, "%d", &val);
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int toUInt(const char* buf)
{
  unsigned int val;
  sscanf(buf, "%u", &val);
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
std::string toUTF8(const wchar_t* wstr)
{
  std::string utf8str;
  unsigned int wlen = (unsigned int)wcslen(wstr);
  unsigned int ulen = 0;
  for (unsigned int i = 0; i < wlen && wstr[i]; ++i)
  {
    unsigned int uch = wstr[i];
    if (uch < 0x80)
      ulen++;
    else if (uch < 0x800)
      ulen += 2;
    else
      ulen += 3;
  }
  utf8str.resize(ulen + 1);

  int k = 0;
  for (unsigned int i = 0; i < wlen && wstr[i]; ++i)
  {
    unsigned int uch = wstr[i];
    if (uch < 0x80)
    {
      utf8str[k++] = static_cast<char>(uch);
    }
    else if (uch < 0x800)
    {
      utf8str[k++] = static_cast<char>(0xC0 | (uch >> 6));
      utf8str[k++] = static_cast<char>(0x80 | (uch & 0x3f));
    }
    else
    {
      utf8str[k++] = static_cast<char>(0xE0 | (uch >> 12));
      utf8str[k++] = static_cast<char>(0x80 | ((uch >> 6) & 0x3f));
      utf8str[k++] = static_cast<char>(0x80 | (uch & 0x3f));
    }
  }
  utf8str[ulen] = '\0';

  return utf8str;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 toVector3(const char* buf)
{
  NMP::Vector3 val;
  sscanf(buf, "%f, %f, %f", &val.x, &val.y, &val.z);
  val.w = 0.0f;
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Quat toQuat(const char* buf)
{
  NMP::Quat val;
  sscanf(buf, "%f, %f, %f, %f", &val.x, &val.y, &val.z, &val.w);
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
// XMLElement
//----------------------------------------------------------------------------------------------------------------------
XMLElement::XMLElement(const char* name) :
  TiXmlElement(name)
{
}

//----------------------------------------------------------------------------------------------------------------------
XMLElement::XMLElement(const XMLElement& other) : TiXmlElement(other)
{
}

//----------------------------------------------------------------------------------------------------------------------
XMLElement::XMLElement(const char* name, const char* value) :
  TiXmlElement(name)
{
  setText(value);
}

//----------------------------------------------------------------------------------------------------------------------
XMLElement::XMLElement(const char* name, const wchar_t* value) :
  TiXmlElement(name)
{
  setText(value);
}

//----------------------------------------------------------------------------------------------------------------------
XMLElement::XMLElement(const char* name, unsigned int value) :
  TiXmlElement(name)
{
  char buf[33];
  sprintf_s(buf, "%d", value);
  setText(buf);
}

//----------------------------------------------------------------------------------------------------------------------
bool XMLElement::saveFile(const char* filename, int* error, size_t initialSize)
{
  NM::TiXmlBufferedTextWriter writer(filename, (unsigned int)initialSize);
  Print(&writer, 0);
  return writer.writeToFile(error);
}

//----------------------------------------------------------------------------------------------------------------------
// Attributes
//----------------------------------------------------------------------------------------------------------------------
void XMLElement::setAttribute(const char* name, const std::string& val)
{
  SetAttribute(name, val.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
void XMLElement::setAttribute(const char* name, const char* val)
{
  SetAttribute(name, val);
}

//----------------------------------------------------------------------------------------------------------------------
void XMLElement::setAttribute(const char* name, const wchar_t* val)
{
  std::string ustr = toUTF8(val);
  SetAttribute(name, ustr.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
void XMLElement::setFloatAttribute(const char* name, float val)
{
  setAttribute(name, toStr(val));
}

//----------------------------------------------------------------------------------------------------------------------
void XMLElement::setUIntAttribute(const char* name, unsigned int val)
{
  setAttribute(name, toStr(val));
}

//----------------------------------------------------------------------------------------------------------------------
void XMLElement::setInt64Attribute(const char* name, int64_t val)
{
  setAttribute(name, toStr(val));
}

//----------------------------------------------------------------------------------------------------------------------
void XMLElement::setUInt64Attribute(const char* name, uint64_t val)
{
  setAttribute(name, toStr(val));
}

//----------------------------------------------------------------------------------------------------------------------
void XMLElement::setVector3Attribute(const char* name, const NMP::Vector3& val)
{
  setAttribute(name, toStr(val));
}

//----------------------------------------------------------------------------------------------------------------------
void XMLElement::setQuatAttribute(const char* name, const NMP::Quat& val)
{
  setAttribute(name, toStr(val));
}

//----------------------------------------------------------------------------------------------------------------------
const char* XMLElement::getAttribute(const char* name) const
{
  return Attribute(name);
}

//----------------------------------------------------------------------------------------------------------------------
bool XMLElement::getFloatAttribute(const char* name, float& val) const
{
  return (NM::TIXML_SUCCESS == QueryFloatAttribute(name, &val));
}

//----------------------------------------------------------------------------------------------------------------------
bool XMLElement::getUIntAttribute(const char* name, unsigned int& val) const
{
  return (NM::TIXML_SUCCESS == QueryIntAttribute(name, (int*)&val));
}

//----------------------------------------------------------------------------------------------------------------------
bool XMLElement::getInt64Attribute(const char* name, int64_t& val) const
{
  return NM::TIXML_SUCCESS == QueryInt64Attribute(name, &val);
}

//----------------------------------------------------------------------------------------------------------------------
bool XMLElement::getUInt64Attribute(const char* name, uint64_t& val) const
{
  return NM::TIXML_SUCCESS == QueryInt64Attribute(name, (int64_t*)&val);
}

//----------------------------------------------------------------------------------------------------------------------
bool XMLElement::getIntAttribute(const char* name, int& val) const
{
  return (NM::TIXML_SUCCESS == QueryIntAttribute(name, &val));
}

//----------------------------------------------------------------------------------------------------------------------
bool XMLElement::getVector3Attribute(const char* name, NMP::Vector3& val) const
{
  const char* text = Attribute(name);
  if (text)
  {
    val = toVector3(text);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
// Element data
//----------------------------------------------------------------------------------------------------------------------
void XMLElement::setText(const std::string& text)
{
  TiXmlNode* childTextN = FirstChild();
  if (childTextN != 0)
  {
    RemoveChild(childTextN);
  }

  // Insert our new text element
  NM::TiXmlText* childText = new NM::TiXmlText(text.c_str());
  LinkEndChild(childText);
}

//----------------------------------------------------------------------------------------------------------------------
void XMLElement::setText(const char* text)
{
  NM::TiXmlNode* childTextN = FirstChild();
  if (childTextN != 0)
  {
    NM::TiXmlText* childText = childTextN->ToText();
    if (childText)
    {
      childText->SetValue(text);
      return;
    }
    else
    {
      RemoveChild(childTextN);
    }

  }
  // Insert our new text element
  NM::TiXmlText* childText = new NM::TiXmlText(text);
  LinkEndChild(childText);
}

//----------------------------------------------------------------------------------------------------------------------
void XMLElement::setText(const wchar_t* text)
{
  std::string ustr = toUTF8(text);
  setText(ustr.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
void XMLElement::setBytes(const void* data, size_t length)
{
  if ((3 * length) < SET_BYTES_BUFFER_LENGTH - 1)
  {
    static char buffer[SET_BYTES_BUFFER_LENGTH];
    static char hexByteBuffer[4];
    const unsigned char* bytes = (const unsigned char*)data;
    int bufferWriteLoc = 0;
    for (size_t i = 0 ; i < length ; ++i)
    {
      // print the byte in lower case hex form always using two characters
      sprintf_s(hexByteBuffer, "%02x,", bytes[i]);
      buffer[bufferWriteLoc] = hexByteBuffer[0];
      buffer[bufferWriteLoc + 1] = hexByteBuffer[1];
      buffer[bufferWriteLoc + 2] = hexByteBuffer[2];
      bufferWriteLoc += 3;
    }

    buffer[bufferWriteLoc] = 0;

    setText(buffer);
  }
  else
  {
    // the static buffer wasn't big enough so we have to dynamically allocate.
    size_t bufferLength = (3 * length) + 1;
    char* buffer = new char[bufferLength];
    static char hexByteBuffer[4];
    const unsigned char* bytes = (const unsigned char*)data;

    int bufferWriteLoc = 0;
    for (size_t i = 0 ; i < length; ++i)
    {
      // print the byte in lower case hex form always using two characters
      sprintf_s(hexByteBuffer, "%02x,", bytes[i]);
      buffer[bufferWriteLoc] = hexByteBuffer[0];
      buffer[bufferWriteLoc + 1] = hexByteBuffer[1];
      buffer[bufferWriteLoc + 2] = hexByteBuffer[2];
      bufferWriteLoc += 3;
    }

    buffer[bufferWriteLoc] = 0;

    setText(buffer);

    delete [] buffer;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMLElement::setFloat(float val)
{
  setText(toStr(val));
}

//----------------------------------------------------------------------------------------------------------------------
void XMLElement::setDouble(double val)
{
  setText(toStr(val));
}

//----------------------------------------------------------------------------------------------------------------------
void XMLElement::setUInt(unsigned int val)
{
  setText(toStr(val));
}

//----------------------------------------------------------------------------------------------------------------------
void XMLElement::setInt(int val)
{
  setText(toStr(val));
}

//----------------------------------------------------------------------------------------------------------------------
void XMLElement::setVector3(const NMP::Vector3& val)
{
  setText(toStr(val));
}

//----------------------------------------------------------------------------------------------------------------------
void XMLElement::setQuat(const NMP::Quat& val)
{
  setText(toStr(val));
}

//----------------------------------------------------------------------------------------------------------------------
const char* XMLElement::getText() const
{
  const TiXmlNode* childTextN = FirstChild();
  if (childTextN == 0)
  {
    return 0;
  }
  const NM::TiXmlText* childText = childTextN->ToText();
  if (childText == 0)
  {
    return 0;
  }
  return childText->Value();
}

//----------------------------------------------------------------------------------------------------------------------
bool XMLElement::getBool() const
{
  const char* text = getText();
  if (text)
  {
    return strcmp(text, "0") != 0;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
float XMLElement::getFloat() const
{
  return toFloat(getText());
}

//----------------------------------------------------------------------------------------------------------------------
double XMLElement::getDouble() const
{
  return toDouble(getText());
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int XMLElement::getUInt() const
{
  return toUInt(getText());
}

//----------------------------------------------------------------------------------------------------------------------
int XMLElement::getInt() const
{
  return toInt(getText());
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 XMLElement::getVector3() const
{
  return toVector3(getText());
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Quat XMLElement::getQuat() const
{
  return toQuat(getText());
}

//----------------------------------------------------------------------------------------------------------------------
// Finding children
//----------------------------------------------------------------------------------------------------------------------
XMLElement* XMLElement::findChild(const char* name)
{
  TiXmlNode* node = FirstChild(name);
  if (node == 0)
  {
    return 0;
  }
  return (XMLElement*)(node->ToElement());
}

//----------------------------------------------------------------------------------------------------------------------
const XMLElement* XMLElement::findChild(const char* name) const
{
  const TiXmlNode* node = FirstChild(name);
  if (node == 0)
  {
    return 0;
  }
  return (const XMLElement*)(node->ToElement());
}

//----------------------------------------------------------------------------------------------------------------------
const XMLElement* XMLElement::nextChild(const char* name, const XMLElement* previous) const
{
  if (previous == LastChild(name))
  {
    return 0;
  }
  const TiXmlNode* nextNode = IterateChildren(name, previous);
  if (nextNode == 0)
  {
    return 0;
  }
  return (const XMLElement*)nextNode->ToElement();
}

//----------------------------------------------------------------------------------------------------------------------
XMLElement* XMLElement::nextChild(const char* name, XMLElement* previous)
{
  if (previous == LastChild(name))
  {
    return 0;
  }
  TiXmlNode* nextNode = IterateChildren(name, previous);
  if (nextNode == 0)
  {
    return 0;
  }
  return (XMLElement*)nextNode->ToElement();
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int XMLElement::getChildCount(const char* name) const
{
  unsigned int count = 0;
  const TiXmlNode* node = FirstChild(name);
  while (node != 0)
  {
    ++count;
    node = IterateChildren(name, node);
  }
  return count;
}

//----------------------------------------------------------------------------------------------------------------------
XMLElement* XMLElement::findChild(const char* name, unsigned int index)
{
  const TiXmlNode* node = FirstChild(name);
  while (index > 0)
  {
    node = IterateChildren(name, node);
    --index;

    if (node == 0)
    {
      return 0;
    }
  }
  return (XMLElement*)(node->ToElement());
}

//----------------------------------------------------------------------------------------------------------------------
const XMLElement* XMLElement::findChild(const char* name, unsigned int index) const
{
  return const_cast<XMLElement*>(this)->findChild(name, index);
}

//----------------------------------------------------------------------------------------------------------------------
const XMLElement* XMLElement::findChildWithUIntAttribute(const char* name, const char* attribute, unsigned int value) const
{
  const XMLElement* child = findChild(name);

  while (child != 0)
  {
    unsigned int attrVal;
    if (!child->getUIntAttribute(attribute, attrVal))
    {
      return 0;
    }

    if (attrVal == value)
    {
      return child;
    }

    child = nextChild(name, child);
  }

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
// Setting child elements
//----------------------------------------------------------------------------------------------------------------------
XMLElement* XMLElement::addChild(const char* childName)
{
  TiXmlElement* childEl = new TiXmlElement(childName);
  LinkEndChild(childEl);

  return (XMLElement*)childEl;
}

//----------------------------------------------------------------------------------------------------------------------
XMLElement* XMLElement::addEndChild(const XMLElement& other)
{
  TiXmlElement* childEl = (TiXmlElement*)InsertEndChild(other);

  return (XMLElement*)childEl;
}

//----------------------------------------------------------------------------------------------------------------------
XMLElement* XMLElement::setChildText(const char* childName, const std::string& text)
{
  TiXmlElement* childEl = 0;

  // Look for an existing child of the appropriate name
  TiXmlNode* childN = FirstChild(childName);
  if (childN != 0)
  {
    childEl = childN->ToElement();
    if (childEl == 0)
    {
      // Child of non-element.  Remove it.
      RemoveChild(childN);
    }
  }

  // Add one if none was found
  if (childEl == 0)
  {
    childEl = new XMLElement(childName);
    LinkEndChild(childEl);
  }

  // Set text
  ((XMLElement*)childEl)->setText(text);

  return (XMLElement*)childEl;
}

//----------------------------------------------------------------------------------------------------------------------
XMLElement* XMLElement::setChildText(const char* childName, const char* text)
{
  TiXmlElement* childEl = 0;

  // Look for an existing child of the appropriate name
  TiXmlNode* childN = FirstChild(childName);
  if (childN != 0)
  {
    childEl = childN->ToElement();
    if (childEl == 0)
    {
      // Child of non-element.  Remove it.
      RemoveChild(childN);
    }
  }

  // Add one if none was found
  if (childEl == 0)
  {
    childEl = new XMLElement(childName);
    LinkEndChild(childEl);
  }

  // Set text
  ((XMLElement*)childEl)->setText(text);

  return (XMLElement*)childEl;
}

//----------------------------------------------------------------------------------------------------------------------
XMLElement* XMLElement::setChildText(const char* childName, const wchar_t* wtext)
{
  std::string ustr = toUTF8(wtext);
  return setChildText(childName, ustr.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
XMLElement* XMLElement::addChildText(const char* childName, const std::string& text)
{
  TiXmlElement* childEl = new TiXmlElement(childName);
  LinkEndChild(childEl);

  NM::TiXmlText* childText = new NM::TiXmlText(text.c_str());
  childEl->LinkEndChild(childText);

  return (XMLElement*)childEl;
}

//----------------------------------------------------------------------------------------------------------------------
XMLElement* XMLElement::addChildText(const char* childName, const char* text)
{
  TiXmlElement* childEl = new TiXmlElement(childName);
  LinkEndChild(childEl);

  NM::TiXmlText* childText = new NM::TiXmlText(text);
  childEl->LinkEndChild(childText);

  return (XMLElement*)childEl;
}

//----------------------------------------------------------------------------------------------------------------------
XMLElement* XMLElement::setChildFloat(const char* childName, float val)
{
  return setChildText(childName, toStr(val));
}

//----------------------------------------------------------------------------------------------------------------------
XMLElement* XMLElement::addChildFloat(const char* childName, float val)
{
  return addChildText(childName, toStr(val));
}

//----------------------------------------------------------------------------------------------------------------------
XMLElement* XMLElement::addChildUInt(const char* childName, unsigned int val)
{
  return addChildText(childName, toStr(val));
}

//----------------------------------------------------------------------------------------------------------------------
XMLElement* XMLElement::setChildUInt(const char* childName, unsigned int val)
{
  return setChildText(childName, toStr(val));
}

//----------------------------------------------------------------------------------------------------------------------
XMLElement* XMLElement::setChildVector3(const char* childName, const NMP::Vector3& val)
{
  return setChildText(childName, toStr(val));
}

//----------------------------------------------------------------------------------------------------------------------
XMLElement* XMLElement::addChildVector3(const char* childName, const NMP::Vector3& val)
{
  return addChildText(childName, toStr(val));
}

//----------------------------------------------------------------------------------------------------------------------
const char* XMLElement::getChildText(const char* childName) const
{
  const TiXmlNode* childN = FirstChild(childName);
  if (childN == 0)
  {
    return 0;
  }

  const TiXmlElement* childEl = childN->ToElement();
  if (childEl == 0)
  {
    return 0;
  }

  if (childEl->FirstChild() == 0)
  {
    return 0;
  }
  const NM::TiXmlText* childText = childEl->FirstChild()->ToText();
  if (childText == 0)
  {
    return 0;
  }

  return childText->Value();
}

//----------------------------------------------------------------------------------------------------------------------
bool XMLElement::getChildFloat(const char* name, float& val) const
{
  const char* text = getChildText(name);
  if (0 == text)
  {
    return false;
  }
  return (1 == sscanf(text, "%f", &val));
}

//----------------------------------------------------------------------------------------------------------------------
bool XMLElement::getChildUInt(const char* name, unsigned int& val) const
{
  const char* text = getChildText(name);
  if (0 == text)
  {
    return false;
  }
  return (1 == sscanf(text, "%d", &val));
}

bool XMLElement::getChildVector3(const char* name, NMP::Vector3& val) const
{
  const char* text = getChildText(name);
  if (0 == text)
  {
    return false;
  }
  return (3 == sscanf(text, "%f, %f, %f", &val.x, &val.y, &val.z));
}

//----------------------------------------------------------------------------------------------------------------------
// Util functions
//----------------------------------------------------------------------------------------------------------------------
void XMLElement::setGUID(GUID guid)
{
  setAttribute("guid", guid);
}

//----------------------------------------------------------------------------------------------------------------------
GUID XMLElement::getGUID() const
{
  return getAttribute("guid");
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace ME
