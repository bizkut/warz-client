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
#include "export/mcExport.h"

#if defined(_MSC_VER)
  #pragma warning(disable:4996)
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace ME
{

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExport::getAsFloat(float& value) const
{
  double dVal = (double) value;
  bool result = getAsDouble(dVal);
  value = (float) dVal;
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExport::setAsFloat(float value)
{
  return setAsDouble((double)value);
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExport::isNameEqual(const char* destName) const
{
  return (strcmp(getName(), destName) == 0);
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExport::isInt() const
{
  return getType() == ATTR_TYPE_INT;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExport::isUInt() const
{
  return getType() == ATTR_TYPE_UINT;
}
//----------------------------------------------------------------------------------------------------------------------
bool AttributeExport::isDouble() const
{
  return getType() == ATTR_TYPE_DOUBLE;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExport::isString() const
{
  return getType() == ATTR_TYPE_STRING;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExport::isBool() const
{
  return getType() == ATTR_TYPE_BOOL;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExport::isVector3() const
{
  return getType() == ATTR_TYPE_VECTOR3;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExport::isQuat() const
{
  return getType() == ATTR_TYPE_QUAT;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExport::isMatrix34() const
{
  return getType() == ATTR_TYPE_MATRIX34;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExport::isBoolArray() const
{
  return getType() == ATTR_TYPE_BOOL_ARRAY;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExport::isUIntArray() const
{
  return getType() == ATTR_TYPE_UINT_ARRAY;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExport::isFloatArray() const
{
  return getType() == ATTR_TYPE_FLOAT_ARRAY;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExport::isVector3Array() const
{
  return getType() == ATTR_TYPE_VECTOR3_ARRAY;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExport::isStringArray() const
{
  return getType() == ATTR_TYPE_STRING_ARRAY;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeExport::isArray() const
{
  return isUIntArray() || isVector3Array() || isStringArray() || isFloatArray() || isBoolArray();
}

//----------------------------------------------------------------------------------------------------------------------
const AttributeExport* AttributeBlockExport::getAttributeByName(const char* name) const
{
  for (unsigned int i = 0; i < getNumAttributes(); ++i)
  {
    if (getAttributeByIndex(i)->isNameEqual(name)) return getAttributeByIndex(i);
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
AttributeExport* AttributeBlockExport::getAttributeByName(const char* name)
{
  for (unsigned int i = 0; i < getNumAttributes(); ++i)
  {
    if (getAttributeByIndex(i)->isNameEqual(name)) return getAttributeByIndex(i);
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------

bool AttributeBlockExport::hasAttribute(const char* name) const
{
  return getAttributeByName(name) != 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeBlockExport::getIntAttribute(const char* name, int& value) const
{
  const AttributeExport* targetAttr = getAttributeByName(name);
  if (targetAttr)
  {
    if (targetAttr->isInt())
    {
      targetAttr->getAsInt(value);
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeBlockExport::getUIntAttribute(const char* name, unsigned int& value) const
{
  const AttributeExport* targetAttr = getAttributeByName(name);
  if (targetAttr)
  {
    if (targetAttr->isUInt())
    {
      targetAttr->getAsUInt(value);
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeBlockExport::getDoubleAttribute(const char* name, double& value) const
{
  const AttributeExport* targetAttr = getAttributeByName(name);
  if (targetAttr)
  {
    if (targetAttr->isDouble())
    {
      targetAttr->getAsDouble(value);
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeBlockExport::getStringAttribute(const char* name, const char *& value) const
{
  const AttributeExport* targetAttr = getAttributeByName(name);
  if (targetAttr)
  {
    if (targetAttr->isString())
    {
      targetAttr->getAsString(value);
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeBlockExport::getBoolAttribute(const char* name, bool& value) const
{
  const AttributeExport* targetAttr = getAttributeByName(name);
  if (targetAttr)
  {
    if (targetAttr->isBool())
    {
      targetAttr->getAsBool(value);
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeBlockExport::getVector3Attribute(const char* name, NMP::Vector3& value) const
{
  const AttributeExport* targetAttr = getAttributeByName(name);
  if (targetAttr)
  {
    if (targetAttr->isVector3())
    {
      targetAttr->getAsVector3(value);
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeBlockExport::getQuatAttribute(const char* name, NMP::Quat& value) const
{
  const AttributeExport* targetAttr = getAttributeByName(name);
  if (targetAttr)
  {
    if (targetAttr->isQuat())
    {
      targetAttr->getAsQuat(value);
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeBlockExport::getMatrix34Attribute(const char* name, NMP::Matrix34& value) const
{
  const AttributeExport* targetAttr = getAttributeByName(name);
  if (targetAttr)
  {
    if (targetAttr->isMatrix34())
    {
      // Set the value to identity so that any values not assigned (usually the w component) by getAsMatrix34() are
      // not left unitialised.
      value.identity();
      targetAttr->getAsMatrix34(value);
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeBlockExport::getAttributeArraySize(const char* name, unsigned int& size) const
{
  const AttributeExport* targetAttr = getAttributeByName(name);
  if (targetAttr)
  {
    if (targetAttr->isArray())
    {
      size = targetAttr->getSize();
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeBlockExport::getBoolAttributeArray(const char* name, bool values[], unsigned int size) const
{
  const AttributeExport* targetAttr = getAttributeByName(name);
  if (targetAttr)
  {
    if (targetAttr->isBoolArray())
    {
      targetAttr->getAsBoolArray(values, size);
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeBlockExport::getUIntAttributeArray(const char* name, unsigned int values[], unsigned int size) const
{
  const AttributeExport* targetAttr = getAttributeByName(name);
  if (targetAttr)
  {
    if (targetAttr->isUIntArray())
    {
      targetAttr->getAsUIntArray(values, size);
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeBlockExport::getFloatAttributeArray(const char* name, float values[], unsigned int size) const
{
  const AttributeExport* targetAttr = getAttributeByName(name);
  if (targetAttr)
  {
    if (targetAttr->isFloatArray())
    {
      targetAttr->getAsFloatArray(values, size);
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeBlockExport::getVector3AttributeArray(const char* name, NMP::Vector3 values[], unsigned int size) const
{
  const AttributeExport* targetAttr = getAttributeByName(name);
  if (targetAttr)
  {
    if (targetAttr->isVector3Array())
    {
      targetAttr->getAsVector3Array(values, size);
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeBlockExport::getStringAttributeArray(const char* name, const char* values[], unsigned int size) const
{
  const AttributeExport* targetAttr = getAttributeByName(name);
  if (targetAttr)
  {
    if (targetAttr->isStringArray())
    {
      targetAttr->getAsStringArray(values, size);
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeBlockExport::setIntAttribute(const char* name, int value, bool autoCreate)
{
  AttributeExport* targetAttr = getAttributeByName(name);
  if (targetAttr)
  {
    if (targetAttr->isInt())
    {
      targetAttr->setAsInt(value);
      return true;
    }
  }
  else if (autoCreate)
  {
    targetAttr = createIntAttribute(name, value);
    return (targetAttr != 0);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeBlockExport::setUIntAttribute(const char* name, unsigned int value, bool autoCreate)
{
  AttributeExport* targetAttr = getAttributeByName(name);
  if (targetAttr)
  {
    if (targetAttr->isUInt())
    {
      targetAttr->setAsUInt(value);
      return true;
    }
  }
  else if (autoCreate)
  {
    targetAttr = createUIntAttribute(name, value);
    return (targetAttr != 0);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeBlockExport::setDoubleAttribute(const char* name, double value, bool autoCreate)
{
  AttributeExport* targetAttr = getAttributeByName(name);
  if (targetAttr)
  {
    if (targetAttr->isDouble())
    {
      targetAttr->setAsDouble(value);
      return true;
    }
  }
  else if (autoCreate)
  {
    targetAttr = createDoubleAttribute(name, value);
    return (targetAttr != 0);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeBlockExport::setStringAttribute(const char* name, const char* value, bool autoCreate)
{
  AttributeExport* targetAttr = getAttributeByName(name);
  if (targetAttr)
  {
    if (targetAttr->isString())
    {
      targetAttr->setAsString(value);
      return true;
    }
  }
  else if (autoCreate)
  {
    targetAttr = createStringAttribute(name, value);
    return (targetAttr != 0);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeBlockExport::setBoolAttribute(const char* name, bool value, bool autoCreate)
{
  AttributeExport* targetAttr = getAttributeByName(name);
  if (targetAttr)
  {
    if (targetAttr->isBool())
    {
      targetAttr->setAsBool(value);
      return true;
    }
  }
  else if (autoCreate)
  {
    targetAttr = createBoolAttribute(name, value);
    return (targetAttr != 0);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeBlockExport::setVector3Attribute(const char* name, const NMP::Vector3& value, bool autoCreate)
{
  AttributeExport* targetAttr = getAttributeByName(name);
  if (targetAttr)
  {
    if (targetAttr->isVector3())
    {
      targetAttr->setAsVector3(value);
      return true;
    }
  }
  else if (autoCreate)
  {
    targetAttr = createVector3Attribute(name, value);
    return (targetAttr != 0);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeBlockExport::setQuatAttribute(const char* name, const NMP::Quat& value, bool autoCreate)
{
  AttributeExport* targetAttr = getAttributeByName(name);
  if (targetAttr)
  {
    if (targetAttr->isQuat())
    {
      targetAttr->setAsQuat(value);
      return true;
    }
  }
  else if (autoCreate)
  {
    targetAttr = createQuatAttribute(name, value);
    return (targetAttr != 0);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeBlockExport::setMatrix34Attribute(const char* name, const NMP::Matrix34& value, bool autoCreate)
{
  AttributeExport* targetAttr = getAttributeByName(name);
  if (targetAttr)
  {
    if (targetAttr->isMatrix34())
    {
      targetAttr->setAsMatrix34(value);
      return true;
    }
  }
  else if (autoCreate)
  {
    targetAttr = createMatrix34Attribute(name, value);
    return (targetAttr != 0);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeBlockExport::setBoolAttributeArray(
  const char*        name,
  const bool         values[],
  unsigned int       size,
  bool               autoCreate)
{
  AttributeExport* targetAttr = getAttributeByName(name);
  if (targetAttr)
  {
    if (targetAttr->isBoolArray())
    {
      targetAttr->setAsBoolArray(values, size);
      return true;
    }
  }
  else if (autoCreate)
  {
    targetAttr = createBoolAttributeArray(name, values, size);
    return (targetAttr != 0);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeBlockExport::setUIntAttributeArray(
  const char*        name,
  const unsigned int values[],
  unsigned int       size,
  bool               autoCreate)
{
  AttributeExport* targetAttr = getAttributeByName(name);
  if (targetAttr)
  {
    if (targetAttr->isUIntArray())
    {
      targetAttr->setAsUIntArray(values, size);
      return true;
    }
  }
  else if (autoCreate)
  {
    targetAttr = createUIntAttributeArray(name, values, size);
    return (targetAttr != 0);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeBlockExport::setFloatAttributeArray(
  const char*        name,
  const float        values[],
  unsigned int       size,
  bool               autoCreate)
{
  AttributeExport* targetAttr = getAttributeByName(name);
  if (targetAttr)
  {
    if (targetAttr->isFloatArray())
    {
      targetAttr->setAsFloatArray(values, size);
      return true;
    }
  }
  else if (autoCreate)
  {
    targetAttr = createFloatAttributeArray(name, values, size);
    return (targetAttr != 0);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeBlockExport::setVector3AttributeArray(
  const char*        name,
  const NMP::Vector3 values[],
  unsigned int       size,
  bool               autoCreate)
{
  AttributeExport* targetAttr = getAttributeByName(name);
  if (targetAttr)
  {
    if (targetAttr->isVector3Array())
    {
      targetAttr->setAsVector3Array(values, size);
      return true;
    }
  }
  else if (autoCreate)
  {
    targetAttr = createVector3AttributeArray(name, values, size);
    return (targetAttr != 0);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttributeBlockExport::setStringAttributeArray(
  const char*  name,
  const char*  values[],
  unsigned int size,
  bool         autoCreate)
{
  AttributeExport* targetAttr = getAttributeByName(name);
  if (targetAttr)
  {
    if (targetAttr->isStringArray())
    {
      targetAttr->setAsStringArray(values, size);
      return true;
    }
  }
  else if (autoCreate)
  {
    targetAttr = createStringAttributeArray(name, values, size);
    return (targetAttr != 0);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int EventBaseExport::getUserData() const
{
  unsigned int userData = 0;
  getAttributeBlock()->getUIntAttribute("userData", userData);
  return userData;
}

//----------------------------------------------------------------------------------------------------------------------
bool EventBaseExport::isAttributeDynamic(AttributeExport* attribute) const
{
  if (attribute == getAttributeBlock()->getAttributeByName("userData"))
  {
    return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
EventTrackExport* TakeExport::findEventTrack(const char* name)
{
  uint32_t numEventTracks = getNumEventTracks();
  for (uint32_t i = 0; i < numEventTracks; ++i)
  {
    ME::EventTrackExport* eventTrackExport = getEventTrack(i);
    if (stricmp(eventTrackExport->getName(), name) == 0)
    {
      return eventTrackExport;
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
float DiscreteEventExport::getNormalisedTime() const
{
  double time = 0.0f;
  getAttributeBlock()->getDoubleAttribute("startTime", time);
  return (float) time;
}

//----------------------------------------------------------------------------------------------------------------------
bool DiscreteEventExport::isAttributeDynamic(AttributeExport* attribute) const
{
  if (attribute == getAttributeBlock()->getAttributeByName("userData"))
  {
    return false;
  }
  else if (attribute == getAttributeBlock()->getAttributeByName("startTime"))
  {
    return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
float DurationEventExport::getNormalisedStartTime() const
{
  double time = 0.0f;
  getAttributeBlock()->getDoubleAttribute("startTime", time);
  return (float)time;
}

//----------------------------------------------------------------------------------------------------------------------
float DurationEventExport::getNormalisedDuration() const
{
  double duration = 0.0f;
  getAttributeBlock()->getDoubleAttribute("duration", duration);
  return (float)duration;
}

//----------------------------------------------------------------------------------------------------------------------
bool DurationEventExport::isAttributeDynamic(AttributeExport* attribute) const
{
  if (attribute == getAttributeBlock()->getAttributeByName("userData"))
  {
    return false;
  }
  else if (attribute == getAttributeBlock()->getAttributeByName("startTime"))
  {
    return false;
  }
  else if (attribute == getAttributeBlock()->getAttributeByName("duration"))
  {
    return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
float CurveEventExport::getNormalisedStartTime() const
{
  double time = 0.0f;
  getAttributeBlock()->getDoubleAttribute("startTime", time);
  return (float)time;
}

//----------------------------------------------------------------------------------------------------------------------
float CurveEventExport::getFloatValue() const
{
  double fval = 0.0f;
  getAttributeBlock()->getDoubleAttribute("floatVal", fval);
  return (float)fval;
}

//----------------------------------------------------------------------------------------------------------------------
bool CurveEventExport::isAttributeDynamic(AttributeExport* attribute) const
{
  if (attribute == getAttributeBlock()->getAttributeByName("userData"))
  {
    return false;
  }
  else if (attribute == getAttributeBlock()->getAttributeByName("startTime"))
  {
    return false;
  }
  else if (attribute == getAttributeBlock()->getAttributeByName("floatVal"))
  {
    return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int EventTrackExport::getUserData() const
{
  unsigned int userData = 0;
  getAttributeBlock()->getUIntAttribute("userData", userData);
  return userData;
}

//----------------------------------------------------------------------------------------------------------------------
const char* EventTrackExport::getDestFilename() const
{
  return "";
}

//----------------------------------------------------------------------------------------------------------------------
bool EventTrackExport::write()
{
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool EventTrackExport::isAttributeDynamic(AttributeExport* attribute) const
{
  if (attribute == getAttributeBlock()->getAttributeByName("userData"))
  {
    return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
TakeExport* TakeList::getTake(const char* name)
{
  for (unsigned int i = 0; i < getNumTakes(); ++i)
  {
    TakeExport* curtake = getTake(i);
    if (strcmp(curtake->getName(), name) == 0)
    {
      return curtake;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const TakeExport* TakeList::getTake(const char* name) const
{
  for (unsigned int i = 0; i < getNumTakes(); ++i)
  {
    const TakeExport* curtake = getTake(i);
    if (strcmp(curtake->getName(), name) == 0)
    {
      return curtake;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
NodeExport* NetworkDefExport::findNode(const char* nodeName)
{
  for (unsigned int i = 0; i < getNumNodes(); ++i)
  {
    NodeExport* curNode = getNode(i);
    if (strcmp(curNode->getName(), nodeName) == 0)
    {
      return curNode;
    }
  }
  
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const NodeExport* NetworkDefExport::findNode(const char* nodeName) const
{
  for (unsigned int i = 0; i < getNumNodes(); ++i)
  {
    const NodeExport* curNode = getNode(i);
    if (strcmp(curNode->getName(), nodeName) == 0)
    {
      return curNode;
    }
  }

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace ME
