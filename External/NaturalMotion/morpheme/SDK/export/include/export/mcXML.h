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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef MCXML_H
#define MCXML_H
//----------------------------------------------------------------------------------------------------------------------
#include <string>
#include "mcExport.h"

#if defined(_MSC_VER)
  // Disable deprecated warnings
  #pragma warning(push)
  #pragma warning(disable:4996)
#endif

#include "NMTinyXML.h"
#include "NMTinyFastHeap.h"

#if defined(_MSC_VER)
  // Re-enable deprecated warnings
  #pragma warning(pop)
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace ME
{

//----------------------------------------------------------------------------------------------------------------------
#define BuildNodeList(_T, vec, xmlElement, tagName) \
{ \
  XMLElement *it = xmlElement->findChild(tagName); \
  vec.clear(); \
  while (it != 0) \
  { \
    _T *obj = new _T(it); \
    vec.push_back(obj); \
    it = xmlElement->nextChild(tagName, it); \
  } \
}

//----------------------------------------------------------------------------------------------------------------------
#define BuildNodeListAndPassParent(_T, vec, xmlElement, tagName, ...) \
{ \
  XMLElement *it = xmlElement->findChild(tagName); \
  vec.clear(); \
  while (it != 0) \
  { \
    _T *obj = new _T(it, __VA_ARGS__); \
    vec.push_back(obj); \
    it = xmlElement->nextChild(tagName, it); \
  } \
}

//----------------------------------------------------------------------------------------------------------------------
#define BuildIndexedNodeList(_T, vec, xmlElement, tagName) \
{ \
  unsigned int capacity = 0; \
  vec.clear(); \
  XMLElement *it = xmlElement->findChild(tagName); \
 \
  while (it != 0) \
  { \
    _T *obj = new _T(it); \
    unsigned int index = obj->getIndex(); \
    if (index >= capacity) \
    { \
      capacity = index + 1; \
      vec.resize(capacity); \
    } \
    vec[index] = obj; \
    it = xmlElement->nextChild(tagName, it); \
  } \
}

//----------------------------------------------------------------------------------------------------------------------
#define BuildIndexedNodeListAndPassParent(_T, vec, xmlElement, tagName, ...) \
{ \
  unsigned int capacity = 0; \
  vec.clear(); \
  XMLElement *it = xmlElement->findChild(tagName); \
  while (it != 0) \
  { \
    _T *obj = new _T(it, __VA_ARGS__); \
    unsigned int index = obj->getIndex(); \
    if (index >= capacity) \
    { \
      capacity = index + 1; \
      vec.resize(capacity); \
    } \
    vec[index] = obj; \
    it = xmlElement->nextChild(tagName, it); \
  } \
}

//----------------------------------------------------------------------------------------------------------------------
#define InsertIntoIndexedList(vec, index, obj) \
{ \
    if (index+1 > vec.size()) \
    { \
      vec.resize(index + 1); \
    } \
    vec[index] = obj; \
}

//----------------------------------------------------------------------------------------------------------------------
std::string toStr(float val);
std::string toStr(double val);
std::string toStr(int val);
std::string toStr(unsigned int val);
std::string toStr(int64_t val);
std::string toStr(uint64_t val);
std::string toStr(const NMP::Vector3& val);
std::string toStr(const NMP::Quat& val);

float toFloat(const char* buf);
double toDouble(const char* buf);
int toInt(const char* buf);
unsigned int toUInt(const char* buf);
NMP::Vector3 toVector3(const char* buf);
NMP::Quat toQuat(const char* buf);
std::string toUTF8(const wchar_t* wstr);

//----------------------------------------------------------------------------------------------------------------------
/// XMLElement
/// Simple wrapper around a basic XML element
//----------------------------------------------------------------------------------------------------------------------
class XMLElement : public NM::TiXmlElement
{
public:

  // Create "<name/>"
  XMLElement(const char* name);

  XMLElement(const XMLElement& other);

  // Create "<name>value</name>"
  XMLElement(const char* name, const char* value);

  // Create "<name>value</name>"
  XMLElement(const char* name, const wchar_t* value);

  // Create "<name>value</name>"
  XMLElement(const char* name, unsigned int value);

  bool saveFile(const char* filename, int* error = NULL, size_t initialSize=1024);

  void setAttribute(const char* name, const std::string& val);
  void setAttribute(const char* name, const char* val);
  void setAttribute(const char* name, const wchar_t* val);
  void setFloatAttribute(const char* name, float val);
  void setUIntAttribute(const char* name, unsigned int val);
  void setInt64Attribute(const char* name, int64_t val);
  void setUInt64Attribute(const char* name, uint64_t val);
  void setVector3Attribute(const char* name, const NMP::Vector3& val);
  void setQuatAttribute(const char* name, const NMP::Quat& val);

  const char* getAttribute(const char* name) const;
  bool getFloatAttribute(const char* name, float& val) const;
  bool getUIntAttribute(const char* name, unsigned int& val) const;
  bool getInt64Attribute(const char* name, int64_t& val) const;
  bool getUInt64Attribute(const char* name, uint64_t& val) const;
  bool getIntAttribute(const char* name, int& val) const;
  bool getVector3Attribute(const char* name, NMP::Vector3& val) const;
  bool getQuatAttribute(const char* name, NMP::Quat& val) const;

  //------------------------------------------------------------
  void setText(const std::string& text);
  void setText(const char* text);
  void setText(const wchar_t* text);
  void setBytes(const void* data, size_t length);
  void setFloat(float val);
  void setDouble(double val);
  void setUInt(unsigned int val);
  void setInt(int val);
  void setVector3(const NMP::Vector3& val);
  void setQuat(const NMP::Quat& val);

  const char* getText() const;
  bool getBool() const;
  float getFloat() const;
  double getDouble() const;
  unsigned int getUInt() const;
  int getInt() const;
  int64_t getInt64() const;
  uint64_t getUInt64() const;
  NMP::Vector3 getVector3() const;
  NMP::Quat getQuat() const;

  XMLElement* findChild(const char* name);
  XMLElement* nextChild(const char* name, XMLElement* previous);
  const XMLElement* findChild(const char* name) const;
  const XMLElement* nextChild(const char* name, const XMLElement* previous) const;
  unsigned int getChildCount(const char* name) const;
  XMLElement* findChild(const char* name, unsigned int index);
  const XMLElement* findChild(const char* name, unsigned int index) const;
  const XMLElement* findChildWithUIntAttribute(const char* name, const char* attribute, unsigned int value) const;
  XMLElement* addChild(const char* childName);
  XMLElement* addEndChild(const XMLElement& other);

  // Set text on the first child element with the given name.
  // Insert a new child element if one isn't already there.
  XMLElement* setChildText(const char* childName, const std::string& text);
  XMLElement* setChildText(const char* childName, const char* text);
  XMLElement* setChildText(const char* childName, const wchar_t* wtext);

  // Add a child of the given name with the appropriate text, even
  // if one already exists.
  XMLElement* addChildText(const char* childName, const std::string& text);
  XMLElement* addChildText(const char* childName, const char* text);
  XMLElement* setChildFloat(const char* childName, float val);
  XMLElement* addChildFloat(const char* childName, float value);
  XMLElement* setChildUInt(const char* childName, unsigned int val);
  XMLElement* addChildUInt(const char* childName, unsigned int value);
  XMLElement* setChildVector3(const char* childName, const NMP::Vector3& val);
  XMLElement* addChildVector3(const char* childName, const NMP::Vector3& val);

  const char* getChildText(const char* childName) const;
  bool getChildFloat(const char* name, float& val) const;
  bool getChildUInt(const char* name, unsigned int& val) const;
  bool getChildVector3(const char* name, NMP::Vector3& val) const;

  // Utility functions.
  void setGUID(const GUID guid);
  GUID getGUID() const;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace ME

#endif // MCXML_H
