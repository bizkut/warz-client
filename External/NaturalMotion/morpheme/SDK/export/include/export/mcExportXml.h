// Copyright (c) 2010 NaturalMotion.  All Righttatis Reserved.
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
#ifndef MCEXPORTXML_H
#define MCEXPORTXML_H

//----------------------------------------------------------------------------------------------------------------------
#include <string>
#include <vector>
#include <map>
#include "mcExport.h"
#include "mcXML.h"

#ifdef _MSC_VER
  #pragma warning(push)
  #pragma warning(disable:4481)
  #pragma warning(disable:4616)
  #pragma warning(disable:4250)
  #pragma warning(disable:4275)
#endif

//----------------------------------------------------------------------------------------------------------------------
/// XML implementation of Export classes
namespace ME
{

//----------------------------------------------------------------------------------------------------------------------
class EventTrackExportXML;
class TakeExportXML;
class ExportFactoryXML;
class AnimationSetExportXML;
class AnimationLibraryXML;
class AttributeBlockXML;
class TakeListXML;

//----------------------------------------------------------------------------------------------------------------------
/// Store a reference to an object in another file by GUID, filename
/// and optionally asset name.
//----------------------------------------------------------------------------------------------------------------------
class XMLReference : public XMLElement
{
public:
  XMLReference(
    const char* referenceType, GUID guid,
    const char* filename, const char* name = 0);

  GUID getGUID() const;
  const char* getFilename() const;
  const char* getName() const;
};

//----------------------------------------------------------------------------------------------------------------------
///
//----------------------------------------------------------------------------------------------------------------------
class MessageNameInfoXML : public XMLElement
{
public:
  MessageNameInfoXML(unsigned int messageID, const wchar_t* messageName) :
    XMLElement("MessageNameInfo")
  {
    setUIntAttribute("messageID", messageID);
    setAttribute("messageName", messageName);
  }

  unsigned int getMessageID() const;
  const char* getMessageName() const;
};

//----------------------------------------------------------------------------------------------------------------------
///
//----------------------------------------------------------------------------------------------------------------------
class InstanceInfoXML : public XMLElement
{
public:
  InstanceInfoXML(
    const char*  tagName,
    unsigned int typeID,
    unsigned int maxInstances,
    unsigned int totalInstances) :
    XMLElement(tagName)
  {
    setUIntAttribute("typeID", typeID);
    setUIntAttribute("maxInstances", maxInstances);
    setUIntAttribute("totalInstances", totalInstances);
  }
  unsigned int getTypeID() const;
  unsigned int getMaxInstances() const;
  unsigned int getTotalInstances() const;
};

//----------------------------------------------------------------------------------------------------------------------
/// A generic attribute with a simple type and value format in xml
//----------------------------------------------------------------------------------------------------------------------
class AttributeExportXML : public AttributeExport
{
public:
  virtual ~AttributeExportXML();

  const char* getName() const NM_OVERRIDE;

  bool getAsInt(int& value) const NM_OVERRIDE;
  bool getAsUInt(unsigned int& value) const NM_OVERRIDE;
  bool getAsDouble(double& value) const NM_OVERRIDE;
  bool getAsString(const char *&value) const NM_OVERRIDE;
  bool getAsBool(bool& value) const NM_OVERRIDE;
  bool getAsVector3(NMP::Vector3& value) const NM_OVERRIDE;
  bool getAsQuat(NMP::Quat& value) const NM_OVERRIDE;
  bool getAsMatrix34(NMP::Matrix34& value) const NM_OVERRIDE;
  bool getAsBoolArray(bool values[], unsigned int size) const NM_OVERRIDE;
  bool getAsUIntArray(unsigned int values[], unsigned int size) const NM_OVERRIDE;
  bool getAsFloatArray(float values[], unsigned int size) const NM_OVERRIDE;
  bool getAsVector3Array(NMP::Vector3 values[], unsigned int size) const NM_OVERRIDE;
  bool getAsStringArray(const char* values[], unsigned int size) const NM_OVERRIDE;

  bool setAsInt(int value) NM_OVERRIDE;
  bool setAsUInt(unsigned int value) NM_OVERRIDE;
  bool setAsDouble(double value) NM_OVERRIDE;
  bool setAsString(const char* value) NM_OVERRIDE;
  bool setAsStringArray(const char* value[], unsigned int size) NM_OVERRIDE;
  bool setAsBool(bool value) NM_OVERRIDE;
  bool setAsVector3(const NMP::Vector3& value) NM_OVERRIDE;
  bool setAsQuat(const NMP::Quat& value) NM_OVERRIDE;
  bool setAsMatrix34(const NMP::Matrix34& value) NM_OVERRIDE;
  bool setAsBoolArray(const bool values[], unsigned int size) NM_OVERRIDE;
  bool setAsUIntArray(const unsigned int values[], unsigned int size) NM_OVERRIDE;
  bool setAsFloatArray(const float values[], unsigned int size) NM_OVERRIDE;
  bool setAsVector3Array(const NMP::Vector3 values[], unsigned int size) NM_OVERRIDE;

  bool appendToUIntArray(unsigned int value) NM_OVERRIDE;
  bool appendToFloatArray(float value) NM_OVERRIDE;
  bool appendToVector3Array(const NMP::Vector3& value) NM_OVERRIDE;
  bool appendToStringArray(const char* value) NM_OVERRIDE;

  AttributeType getType() const NM_OVERRIDE;
  unsigned int getSize() const NM_OVERRIDE;

protected:
  friend class AttributeBlockXML;
  AttributeExportXML(XMLElement* element);
  AttributeExportXML(const char* name);

  XMLElement*                    m_attributeElement;
  AttributeExport::AttributeType m_type;
};

//----------------------------------------------------------------------------------------------------------------------
/// All classes that use attributes contain them in this block
//----------------------------------------------------------------------------------------------------------------------
class AttributeBlockXML : public AttributeBlockExport
{
public:
  AttributeBlockXML(XMLElement* element);
  AttributeBlockXML();
  virtual ~AttributeBlockXML();

  unsigned int getNumAttributes() const NM_OVERRIDE;
  const AttributeExport* getAttributeByIndex(unsigned int attrIdx) const NM_OVERRIDE;
  AttributeExport* getAttributeByIndex(unsigned int attrIdx) NM_OVERRIDE;
  AttributeExport* createDoubleAttribute(const char* name, double value) NM_OVERRIDE;
  AttributeExport* createIntAttribute(const char* name, int value) NM_OVERRIDE;
  AttributeExport* createUIntAttribute(const char* name, unsigned int value) NM_OVERRIDE;
  AttributeExport* createStringAttribute(const char* name, const char* value) NM_OVERRIDE;
  AttributeExport* createBoolAttribute(const char* name, bool value) NM_OVERRIDE;
  AttributeExport* createVector3Attribute(const char* name, const NMP::Vector3& value) NM_OVERRIDE;
  AttributeExport* createQuatAttribute(const char* name, const NMP::Quat& value) NM_OVERRIDE;
  AttributeExport* createMatrix34Attribute(const char* name, const NMP::Matrix34& value) NM_OVERRIDE;
  AttributeExport* createBoolAttributeArray(
    const char*        name,
    const bool         values[],
    unsigned int       size) NM_OVERRIDE;
  AttributeExport* createUIntAttributeArray(
    const char*        name,
    const unsigned int values[],
    unsigned int       size) NM_OVERRIDE;
  AttributeExport* createFloatAttributeArray(
    const char*        name,
    const float        values[],
    unsigned int       size) NM_OVERRIDE;
  AttributeExport* createVector3AttributeArray(
    const char*        name,
    const NMP::Vector3 values[],
    unsigned int       size) NM_OVERRIDE;
  AttributeExport* createStringAttributeArray(
    const char*  name,
    const char*  values[],
    unsigned int size) NM_OVERRIDE;

  XMLElement* getXMLElement() { return m_xmlElement; } // many classes need to link this

protected:
  XMLElement*                      m_xmlElement;
  std::vector<AttributeExportXML*> m_attributes;
};

//----------------------------------------------------------------------------------------------------------------------
///  WrappedAttributeBlockXML
//
// A simple base class for objects that just need a tag and a block of attributes
//----------------------------------------------------------------------------------------------------------------------
class WrappedAttributeBlockXML
{
public:
  virtual const AttributeBlockExport* getAttributeBlock() const;
  virtual AttributeBlockExport* getAttributeBlock();
  virtual ~WrappedAttributeBlockXML();

  XMLElement* getXMLElement() { return m_xmlElement; } // many classes need to link this

protected:
  WrappedAttributeBlockXML(XMLElement* xmlElement);
  WrappedAttributeBlockXML(const char* elementName);

  XMLElement*        m_xmlElement;
  AttributeBlockXML* m_xmlAttributeBlock;
};

//----------------------------------------------------------------------------------------------------------------------
/// A data block written out by the LUA serialize functions
/// and passed into Node/Condition create functions.
//----------------------------------------------------------------------------------------------------------------------
class DataBlockExportXML : public DataBlockExport
{
  friend class ConditionExportXML;
  friend class AnalysisTaskExportXML;
  friend class AnalysisNodeExportXML;
  friend class NodeExportXML;
  friend class MessagePresetExportXML;

public:
  class Element : public XMLElement
  {
  public:
    Element(
      const void*  data,
      size_t       length,
      const char*  type,
      const char*  desc, 
      bool includeLengthXmlAttribute = false);

    unsigned int getIndex() const;
    const char* getType() const;
    const char* getDesc() const;
    unsigned int getLength() const;
    void writeData(void* destBuffer) const;
    bool getBytes(char* data_out, size_t bufferSize, size_t& length_out) const;
    size_t getByteCount() const;
  };

  virtual ~DataBlockExportXML();

  virtual size_t getByteCount(const char* desc) const NM_OVERRIDE;

  bool readString(std::string& data_out, const char* desc) const NM_OVERRIDE;
  bool readUnicodeString(std::wstring& data_out, const char* desc) const NM_OVERRIDE;
  bool readChar(char& data_out, const char* desc) const NM_OVERRIDE;
  bool readCharArray(char* data_out, size_t buffer_size, size_t& length_out, const char* desc) const NM_OVERRIDE;
  bool readBool(bool& data_out, const char* desc) const NM_OVERRIDE;
  bool readBoolArray(bool* data_out, size_t buffer_size, size_t& length_out, const char* desc) const NM_OVERRIDE;
  bool readInt(int& data_out, const char* desc) const NM_OVERRIDE;
  bool readIntArray(int* data_out, size_t buffer_size, size_t& length_out, const char* desc) const NM_OVERRIDE;
  bool readUInt(unsigned int& data_out, const char* desc) const NM_OVERRIDE;
  bool readUIntArray(unsigned int* data_out, size_t buffer_size, size_t& length_out, const char* desc) const NM_OVERRIDE;
  bool readUInt64(uint64_t& data_out, const char* desc) const NM_OVERRIDE;
  bool readUInt64Array(uint64_t* data_out, size_t buffer_size, size_t& length_out, const char* desc) const NM_OVERRIDE;
  bool readFloat(float& data_out, const char* desc) const NM_OVERRIDE;
  bool readFloatArray(float* data_out, size_t buffer_size, size_t& length_out, const char* desc) const NM_OVERRIDE;
  bool readDouble(double& data_out, const char* desc) const NM_OVERRIDE;
  bool readDoubleArray(double* data_out, size_t buffer_size, size_t& length_out, const char* desc) const NM_OVERRIDE;
  bool readIntAttribute(int& data_out, const char* elementDesc, const char* attribDesc) const NM_OVERRIDE;  // Reads an int attribute from an element.
  bool readUndefined(const char* desc) const NM_OVERRIDE;
  bool readNetworkNodeId(int& data_out, const char* desc) const NM_OVERRIDE;
  bool readVector3(const NMP::Vector3& data, const char* desc) const NM_OVERRIDE;
  bool readVector4(const NMP::Quat& data, const char* desc) const NM_OVERRIDE;

  void writeString(const std::string& data, const char* desc = 0) NM_OVERRIDE;
  void writeUnicodeString(const wchar_t* data, const char* desc = 0) NM_OVERRIDE;
  void writeChar(char data, const char* desc = 0) NM_OVERRIDE;
  void writeCharArray(const char* data, size_t length, const char* desc = 0) NM_OVERRIDE;
  void writeBool(bool data, const char* desc = 0) NM_OVERRIDE;
  void writeBoolArray(const bool* data, size_t length, const char* desc = 0) NM_OVERRIDE;
  void writeInt(int data, const char* desc = 0) NM_OVERRIDE;
  void writeNetworkNodeId(int nodeId, const char* desc = 0) NM_OVERRIDE;
  void writeMessageId(int messageId, const char* desc = 0) NM_OVERRIDE;
  void writeNetworkNodeIdWithPinIndex(int nodeId, int pinIndex, const char * desc = 0) NM_OVERRIDE; 
  void writeAnimationId(int animId, const char* desc = 0) NM_OVERRIDE;
  void writeIntArray(const int* data, size_t length, const char* desc = 0) NM_OVERRIDE;
  void writeUInt(unsigned int data, const char* desc = 0) NM_OVERRIDE;
  void writeUIntArray(const unsigned int* data, size_t length, const char* desc = 0) NM_OVERRIDE;
  void writeFloat(float data, const char* desc = 0) NM_OVERRIDE;
  void writeFloatArray(const float* data, size_t length, const char* desc = 0) NM_OVERRIDE;
  void writeDouble(double data, const char* desc = 0) NM_OVERRIDE;
  void writeDoubleArray(const double* data, size_t length, const char* desc = 0) NM_OVERRIDE;
  void writeUndefined(const char* desc) NM_OVERRIDE;
  void writeVector3(const NMP::Vector3& data, const char* desc) NM_OVERRIDE;
  void writeVector4(const NMP::Quat& data, const char* desc) NM_OVERRIDE;

  bool hasElement(const char* desc) const;
  bool hasElementVerifyType(const char* desc, const char* requiredType) const;

  void padToAlignment(unsigned int alignmentValue) NM_OVERRIDE;

  unsigned int getDataBlockSize() const NM_OVERRIDE;
  void writeDataBlock(void*) const NM_OVERRIDE;

protected:
  DataBlockExportXML();
  DataBlockExportXML(XMLElement* element);
  DataBlockExportXML(const DataBlockExportXML& other);
  bool readElement(
    char*       data_out,
    size_t      bufferSize,
    size_t&     length_out,
    const char* desc) const;
  bool readElementVerifyType(
    char*       data_out,
    size_t      bufferSize,
    size_t&     length_out,
    const char* desc,
    const char* requiredType) const;

  Element* addElement(
    const void* data,
    size_t      length,
    const char* type,
    const char* desc, 
    bool includeLengthAttribute);
  bool readElementIntAttrib(
    int*        data_out,
    const char* elementDesc,
    const char* attribDesc) const;

  XMLElement* m_xmlElement;
};

//----------------------------------------------------------------------------------------------------------------------
/// Condition
//----------------------------------------------------------------------------------------------------------------------
class ConditionExportXML : public ConditionExport
{
  friend class NodeExportXML;

public:
  virtual ~ConditionExportXML();

  const char* getName() const NM_OVERRIDE;

  DataBlockExport* getDataBlock() NM_OVERRIDE;
  const DataBlockExport* getDataBlock() const NM_OVERRIDE;

  unsigned int getIndex() const NM_OVERRIDE;
  unsigned int getTypeID() const NM_OVERRIDE;

protected:
  ConditionExportXML(XMLElement* element);
  ConditionExportXML(unsigned int index, unsigned int typeID);
  ConditionExportXML(const ConditionExportXML& other);

  /// should only every be called by derived classes
  ConditionExportXML();

  /// to get around C++ friendship rules this method needs to be called
  XMLElement* getDataBlockXMLElement() const;

  XMLElement*         m_xmlElement;
  DataBlockExportXML* m_dataBlock;
};

//----------------------------------------------------------------------------------------------------------------------
/// AnalysisNode
//----------------------------------------------------------------------------------------------------------------------
class AnalysisNodeExportXML : public AnalysisNodeExport
{
  friend class NodeExportXML;
  friend class AnalysisTaskExportXML;
  friend class AnalysisResultLibraryExportXML;

public:
  virtual ~AnalysisNodeExportXML();

  const char* getName() const NM_OVERRIDE;

  const char* getTypeName() const NM_OVERRIDE;
  DataBlockExport* getDataBlock() NM_OVERRIDE;
  const DataBlockExport* getDataBlock() const NM_OVERRIDE;

protected:
  AnalysisNodeExportXML(XMLElement* element);
  AnalysisNodeExportXML(const char* name, const char* typeName);
  AnalysisNodeExportXML(const AnalysisNodeExportXML& other);

  /// should only every be called by derived classes
  AnalysisNodeExportXML();

  /// to get around C++ friendship rules this method needs to be called
  XMLElement* getDataBlockXMLElement() const;

  XMLElement*         m_xmlElement;
  DataBlockExportXML* m_dataBlock;
};

//----------------------------------------------------------------------------------------------------------------------
/// AnalysisTask
//----------------------------------------------------------------------------------------------------------------------
class AnalysisTaskExportXML : public ME::AnalysisTaskExport
{
  friend class AnalysisLibraryExportXML;

public:
  virtual ~AnalysisTaskExportXML();

  const char* getName() const NM_OVERRIDE;
  const char* getAnalysisType() const NM_OVERRIDE;

  DataBlockExport* getDataBlock() NM_OVERRIDE;
  const DataBlockExport* getDataBlock() const NM_OVERRIDE;

  AnalysisNodeExport* createAnalysisNode(
    const char* name, 
    const char* typeName) NM_OVERRIDE;

  unsigned int getNumAnalysisNodes() const NM_OVERRIDE;
  AnalysisNodeExport* getAnalysisNode(unsigned int index) NM_OVERRIDE;
  const AnalysisNodeExport* getAnalysisNode(unsigned int index) const NM_OVERRIDE;

  void addUsedAnimSetsToTask(const std::vector<int32_t>& usedAnimSetIndices);

protected:
  AnalysisTaskExportXML(XMLElement* element);
  AnalysisTaskExportXML(const char* name, const char* analysisType);

  /// to get around C++ friendship rules this method needs to be called
  XMLElement* getDataBlockXMLElement() const;

  XMLElement*         m_xmlElement;
  DataBlockExportXML* m_dataBlock;
  std::vector<AnalysisNodeExportXML*> m_analysisNodes;
};

//----------------------------------------------------------------------------------------------------------------------
/// AnalysisLibrary
//----------------------------------------------------------------------------------------------------------------------
class AnalysisLibraryExportXML : public ME::AnalysisLibraryExport
{
  friend class ExportFactoryXML;
public:
  virtual ~AnalysisLibraryExportXML();

  bool write() NM_OVERRIDE;

  const char* getName() const NM_OVERRIDE;

  ME::GUID getGUID() const NM_OVERRIDE;
  const char* getDestFilename() const NM_OVERRIDE;
  const char* getResultFilename() const NM_OVERRIDE;

  AnalysisResultLibraryExport* createResultLibrary() NM_OVERRIDE;
  void destroyResultLibrary(AnalysisResultLibraryExport* library) NM_OVERRIDE;

  AnalysisTaskExport* createAnalysisTask(const char* name, const char* analysisType) NM_OVERRIDE;

  unsigned int getNumAnalysisTasks() const NM_OVERRIDE;
  AnalysisTaskExport* getAnalysisTask(unsigned int index) NM_OVERRIDE;
  const AnalysisTaskExport* getAnalysisTask(unsigned int index) const NM_OVERRIDE;

protected:
  AnalysisLibraryExportXML(XMLElement* element, const std::string& destFilename);
  AnalysisLibraryExportXML(GUID guid, const wchar_t* destFilename, const wchar_t* resultFilename);

  XMLElement*         m_xmlElement;
  std::string         m_destinationFilename;
  std::vector<AnalysisTaskExportXML*> m_tasks;
};

//----------------------------------------------------------------------------------------------------------------------
/// AnalysisResultLibrary
//----------------------------------------------------------------------------------------------------------------------
class AnalysisResultLibraryExportXML : public AnalysisResultLibraryExport
{
  friend class ExportFactoryXML;
  friend class AnalysisLibraryExportXML;
public:
  virtual ~AnalysisResultLibraryExportXML();

  bool write() NM_OVERRIDE;

  const char* getName() const NM_OVERRIDE;
  ME::GUID getGUID() const NM_OVERRIDE;
  const char* getDestFilename() const NM_OVERRIDE;

  AnalysisNodeExport* createTaskResult(const char* name) NM_OVERRIDE;

  unsigned int getNumTaskResult() const NM_OVERRIDE;
  AnalysisNodeExport* getTaskResult(unsigned int index) NM_OVERRIDE;
  const AnalysisNodeExport* getTaskResult(unsigned int index) const NM_OVERRIDE;

protected:
  AnalysisResultLibraryExportXML(GUID guid, const char* destFilename);
  AnalysisResultLibraryExportXML(XMLElement* element, const std::string& destFilename);

  XMLElement*         m_xmlElement;
  std::string         m_destinationFilename;
  std::vector<AnalysisNodeExportXML*> m_results;
};

//----------------------------------------------------------------------------------------------------------------------
/// ConditionSet
//----------------------------------------------------------------------------------------------------------------------
class ConditionSetExportXML : public ConditionSetExport
{
  friend class NodeExportXML;

public:
  virtual ~ConditionSetExportXML();

  virtual void setConditionIndices(const std::vector<unsigned int>& indices);

  unsigned int getIndex() const NM_OVERRIDE;
  unsigned int getTargetNodeID() const NM_OVERRIDE;
  std::vector<unsigned int> getConditionIndices() const NM_OVERRIDE;

protected:
  ConditionSetExportXML(XMLElement* element);
  ConditionSetExportXML(
    unsigned int index,
    unsigned int targetNodeID,
    const std::vector<unsigned int>& indices);

  XMLElement* m_xmlElement;
};

//----------------------------------------------------------------------------------------------------------------------
/// ExportXMLed Node
//----------------------------------------------------------------------------------------------------------------------
class NodeExportXML : public NodeExport
{
  friend class NetworkDefExportXML;

public:
  virtual ~NodeExportXML();

  ConditionExport* createCondition(
    unsigned int index,
    unsigned int typeID) NM_OVERRIDE;

  AnalysisNodeExport* createAnalysisNode(
    const char* name, 
    const char * typeName) NM_OVERRIDE;

  ConditionSetExport* createConditionSet(
    unsigned int index,
    unsigned int targetNodeID,
    const std::vector<unsigned int>& indices) NM_OVERRIDE;

  ConditionExport* createCommonCondition(
    unsigned int index,
    unsigned int typeID) NM_OVERRIDE;

  ConditionSetExport* createCommonConditionSet(
    unsigned int index,
    unsigned int targetNodeID,
    const std::vector<unsigned int>& indices) NM_OVERRIDE;

  unsigned int getIndex() const NM_OVERRIDE;
  unsigned int getNodeID() const NM_OVERRIDE;
  unsigned int getDownstreamParentID() const NM_OVERRIDE;
  bool isDownstreamParentMultiplyConnected() const NM_OVERRIDE;
  unsigned int getTypeID() const NM_OVERRIDE;
  const char* getName() const NM_OVERRIDE;
  bool isPersistent() const NM_OVERRIDE;

  DataBlockExport* getDataBlock() NM_OVERRIDE;
  const DataBlockExport* getDataBlock() const NM_OVERRIDE;

  unsigned int getNumConditions() const NM_OVERRIDE;
  ConditionExport* getCondition(unsigned int index) NM_OVERRIDE;
  const ConditionExport* getCondition(unsigned int index) const NM_OVERRIDE;

  unsigned int getNumAnalysisNodes() const NM_OVERRIDE;
  AnalysisNodeExport* getAnalysisNode(unsigned int index) NM_OVERRIDE;
  const AnalysisNodeExport* getAnalysisNode(unsigned int index) const NM_OVERRIDE;

  unsigned int getNumConditionSets() const NM_OVERRIDE;
  ConditionSetExport* getConditionSet(unsigned int index) NM_OVERRIDE;
  const ConditionSetExport* getConditionSet(unsigned int index) const NM_OVERRIDE;

  unsigned int getNumCommonConditions() const NM_OVERRIDE;
  ConditionExport* getCommonCondition(unsigned int index) NM_OVERRIDE;
  const ConditionExport* getCommonCondition(unsigned int index) const NM_OVERRIDE;

  unsigned int getNumCommonConditionSets() const NM_OVERRIDE;
  ConditionSetExport* getCommonConditionSet(unsigned int index) NM_OVERRIDE;
  const ConditionSetExport* getCommonConditionSet(unsigned int index) const NM_OVERRIDE;

protected:
  NodeExportXML(XMLElement* element);
  NodeExportXML(
    unsigned int networkID,
    unsigned int typeID,
    unsigned int downstreamParentID,
    bool         downstreamParentMultiplyConnected,
    const char*  name,
    bool         persistent);

  NodeExportXML(
    unsigned int   networkID,
    unsigned int   typeID,
    unsigned int   downstreamParentID,
    bool           downstreamParentMultiplyConnected,
    const wchar_t* name,
    bool           persistent);

  // should only every be called by derived classes
  NodeExportXML();

  // to get around C++ friendship rules this method needs to be called
  XMLElement* getDataBlockXMLElement() const;

  XMLElement*                         m_xmlElement;
  DataBlockExportXML*                 m_dataBlock;
  std::vector<ConditionExportXML*>    m_conditions;
  std::vector<AnalysisNodeExportXML*> m_statistics;
  std::vector<ConditionSetExportXML*> m_conditionSets;
  std::vector<ConditionExportXML*>    m_commonConditions;
  std::vector<ConditionSetExportXML*> m_commonConditionSets;
};

//----------------------------------------------------------------------------------------------------------------------
///
//----------------------------------------------------------------------------------------------------------------------
class MessageExportXML : public MessageExport
{
  friend class NetworkDefExportXML;

public:

  const char* getName() const NM_OVERRIDE;
  MR::MessageType getMessageType() const NM_OVERRIDE;
  MR::MessageID getMessageID() const NM_OVERRIDE;

protected:
  MessageExportXML(XMLElement* element);
  MessageExportXML(
    const wchar_t*  messageName,
    MR::MessageType messageTypeID,
    MR::MessageID   messageID);

  XMLElement* m_xmlElement;
};

//----------------------------------------------------------------------------------------------------------------------
///
//----------------------------------------------------------------------------------------------------------------------
class EventBaseExportXML : public  virtual EventBaseExport
{
  friend class EventTrackExportXML;

public:
  virtual ~EventBaseExportXML();

  unsigned int getIndex() const NM_OVERRIDE;
  const AttributeBlockExport* getAttributeBlock() const NM_OVERRIDE;
  AttributeBlockExport* getAttributeBlock() NM_OVERRIDE;
  XMLElement* getXMLElement() { return m_xmlElement; }  // many classes need to link this

protected:
  EventBaseExportXML(XMLElement* xmlElement);
  EventBaseExportXML(const char* elementName, unsigned int index, unsigned int userData);

  XMLElement*        m_xmlElement;
  AttributeBlockXML* m_xmlAttributeBlock;
};

//----------------------------------------------------------------------------------------------------------------------
class DiscreteEventExportXML : public virtual EventBaseExportXML, public virtual DiscreteEventExport
{
public:
  /// default implementation requires an attribute called "startTime"
  DiscreteEventExportXML(XMLElement* xmlElement);
  DiscreteEventExportXML(unsigned int index, unsigned int userData, float normalisedTime);
};

//----------------------------------------------------------------------------------------------------------------------
class DurationEventExportXML : public virtual EventBaseExportXML, public virtual DurationEventExport
{
public:
  /// default implementation requires attributes called "startTime" and "duration" in the attribute block
  DurationEventExportXML(XMLElement* xmlElement);
  DurationEventExportXML(unsigned int index, unsigned int userData, float normalisedTime, float normalisedDuration);
};

//----------------------------------------------------------------------------------------------------------------------
class CurveEventExportXML : public virtual EventBaseExportXML, public virtual CurveEventExport
{
public:
  /// default implementation requires attributes called "startTime" and "floatVal" in the attribute block
  CurveEventExportXML(XMLElement* xmlElement);
  CurveEventExportXML(unsigned int index, unsigned int userData, float normalisedTime, float floatValue);
};

//----------------------------------------------------------------------------------------------------------------------
// DiscreteEventTrackExportXML
//----------------------------------------------------------------------------------------------------------------------
class DiscreteEventTrackExportXML : public DiscreteEventTrackExport
{
  friend class TakeExportXML;

public:
  virtual ~DiscreteEventTrackExportXML();

  GUID getGUID() const NM_OVERRIDE;
  const char* getName() const NM_OVERRIDE;
  unsigned int getEventTrackChannelID() const NM_OVERRIDE;
  const TakeExport* getTake() const NM_OVERRIDE;
  const AttributeBlockExport* getAttributeBlock() const NM_OVERRIDE;
  AttributeBlockExport* getAttributeBlock() NM_OVERRIDE;

  unsigned int getNumEvents() const NM_OVERRIDE;
  const DiscreteEventExport* getEvent(unsigned int eventIdx) const NM_OVERRIDE;
  DiscreteEventExport* getEvent(unsigned int eventIdx) NM_OVERRIDE;
  DiscreteEventExport* createEvent(unsigned int index, float normalisedTime, unsigned int userData) NM_OVERRIDE;

protected:
  DiscreteEventTrackExportXML(XMLElement* xmlElement, const TakeExportXML* eventTrackList);
  DiscreteEventTrackExportXML(
    GUID                 guid,
    const wchar_t*       name,
    unsigned int         eventTrackChannelID,
    unsigned int         userData,
    const TakeExportXML* eventTrackList);

  typedef std::vector<DiscreteEventExportXML*> DiscreteEventExportXMLList;

  XMLElement*                 m_xmlElement;
  DiscreteEventExportXMLList  m_events;
  const TakeExportXML*        m_eventTrackList;
  AttributeBlockXML*          m_xmlAttributeBlock;
};

//----------------------------------------------------------------------------------------------------------------------
// DurationEventTrackExportXML
//----------------------------------------------------------------------------------------------------------------------
class DurationEventTrackExportXML : public DurationEventTrackExport
{
  friend class TakeExportXML;

public:
  virtual ~DurationEventTrackExportXML();

  GUID getGUID() const NM_OVERRIDE;
  const char* getName() const NM_OVERRIDE;
  unsigned int getEventTrackChannelID() const NM_OVERRIDE;
  const TakeExport* getTake() const NM_OVERRIDE;
  const AttributeBlockExport* getAttributeBlock() const NM_OVERRIDE;
  AttributeBlockExport* getAttributeBlock() NM_OVERRIDE;

  unsigned int getNumEvents() const NM_OVERRIDE;
  const DurationEventExport* getEvent(unsigned int eventIdx) const NM_OVERRIDE;
  DurationEventExport* getEvent(unsigned int eventIdx) NM_OVERRIDE;
  DurationEventExport* createEvent(
    unsigned int index,
    float        normalisedTime,
    float        duration,
    unsigned int userData) NM_OVERRIDE;

protected:
  DurationEventTrackExportXML(XMLElement* xmlElement, const TakeExportXML* eventTrackList);
  DurationEventTrackExportXML(
    GUID                 guid,
    const wchar_t*       name,
    unsigned int         eventTrackChannelID,
    unsigned int         userData,
    const TakeExportXML* eventTrackList);

  typedef std::vector<DurationEventExportXML*> DurationEventExportXMLList;

  XMLElement*                 m_xmlElement;
  DurationEventExportXMLList  m_events;
  const TakeExportXML*        m_eventTrackList;
  AttributeBlockXML*          m_xmlAttributeBlock;
};

//----------------------------------------------------------------------------------------------------------------------
// CurveEventTrackExportXML
//----------------------------------------------------------------------------------------------------------------------
class CurveEventTrackExportXML : public CurveEventTrackExport
{
  friend class TakeExportXML;

public:
  virtual ~CurveEventTrackExportXML();

  GUID getGUID() const NM_OVERRIDE;
  const char* getName() const NM_OVERRIDE;
  unsigned int getEventTrackChannelID() const NM_OVERRIDE;
  const TakeExport* getTake() const NM_OVERRIDE;
  const AttributeBlockExport* getAttributeBlock() const NM_OVERRIDE;
  AttributeBlockExport* getAttributeBlock() NM_OVERRIDE;

  unsigned int getNumEvents() const;
  const CurveEventExport* getEvent(unsigned int eventIdx) const;
  CurveEventExport* getEvent(unsigned int eventIdx);
  CurveEventExport* createEvent(
    unsigned int index,
    float        normalisedTime,
    float        value,
    unsigned int userData) NM_OVERRIDE;

protected:
  CurveEventTrackExportXML(XMLElement* xmlElement, const TakeExportXML* eventTrackList);
  CurveEventTrackExportXML(
    GUID                 guid,
    const wchar_t*       name,
    unsigned int         eventTrackChannelID,
    unsigned int         userData,
    const TakeExportXML* eventTrackList);

  typedef std::vector<CurveEventExportXML*> CurveEventTrackExportXMLList;

  XMLElement*                   m_xmlElement;
  CurveEventTrackExportXMLList  m_events;
  const TakeExportXML*          m_eventTrackList;
  AttributeBlockXML*            m_xmlAttributeBlock;
};

//----------------------------------------------------------------------------------------------------------------------
// TemplateInfoXML
//----------------------------------------------------------------------------------------------------------------------

class TemplateInfoXML : public WrappedAttributeBlockXML
{
public:
  TemplateInfoXML(XMLElement* xmlElement);
  TemplateInfoXML(const wchar_t* templateName, const char* type, double AbsEndFraction);
  virtual ~TemplateInfoXML();

  double getAbsEndFraction() const;
  const char* getTemplateName() const;
  const char* getType() const; // one of Nil, Stationary, Hinge, Point
};

//----------------------------------------------------------------------------------------------------------------------
// ConstraintEventXML
//----------------------------------------------------------------------------------------------------------------------
class ConstraintEventXML : public EventBaseExportXML
{
  friend class CustomEventTrackExportXML;

public:
  virtual ~ConstraintEventXML();

  virtual bool isAttributeDynamic(AttributeExport* attribute) const;

  virtual unsigned int getNumTemplateInfo() const;
  virtual const TemplateInfoXML* getTemplateInfo(unsigned int eventIdx) const;
  virtual TemplateInfoXML* getTemplateInfo(unsigned int eventIdx);
  virtual TemplateInfoXML* createTemplateInfo(
    const wchar_t* templateName,
    const char*    type,
    double         AbsEndFraction);

  virtual float getNormalisedStartTime() const;
  virtual float getNormalisedDuration() const;
  virtual GUID getGUID() const;

protected:
  ConstraintEventXML(XMLElement* xmlElement);
  ConstraintEventXML(
    unsigned int index,
    float        normalisedTime,
    float        duration,
    GUID         guid,
    unsigned int userData);

  typedef std::vector<TemplateInfoXML*> TemplateInfoXMLList;

  TemplateInfoXMLList m_templateInfos;
};

//----------------------------------------------------------------------------------------------------------------------
// CustomEventTrackExportXML
//----------------------------------------------------------------------------------------------------------------------
class CustomEventTrackExportXML : public CustomEventTrackExport
{
  friend class TakeExportXML;

public:
  virtual ~CustomEventTrackExportXML();

  GUID getGUID() const NM_OVERRIDE;
  const char* getName() const NM_OVERRIDE;
  unsigned int getEventTrackChannelID() const NM_OVERRIDE;
  const TakeExport* getTake() const NM_OVERRIDE;

  unsigned int getNumEvents() const NM_OVERRIDE;
  const EventBaseExport* getEvent(unsigned int eventIdx) const NM_OVERRIDE;
  EventBaseExport* getEvent(unsigned int eventIdx) NM_OVERRIDE;
  virtual ConstraintEventXML* createConstraintEvent(
    unsigned int index,
    float        normalisedTime,
    float        duration,
    GUID         guid,
    unsigned int userData);
  const AttributeBlockExport* getAttributeBlock() const NM_OVERRIDE;
  AttributeBlockExport* getAttributeBlock() NM_OVERRIDE;

protected:
  CustomEventTrackExportXML(XMLElement* xmlElement, const TakeExportXML* eventTrackList);
  CustomEventTrackExportXML(
    GUID                 guid,
    const wchar_t*       name,
    unsigned int         eventTrackChannelID,
    unsigned int         userData,
    const TakeExportXML* eventTrackList);

  typedef std::vector<ConstraintEventXML*> ConstraintEventXMLList;

  ConstraintEventXMLList  m_constraintEvents;
  XMLElement*             m_xmlElement;
  const TakeExportXML*    m_eventTrackList;
  AttributeBlockXML*      m_xmlAttributeBlock;
};

//----------------------------------------------------------------------------------------------------------------------
// TakeExportXML
//----------------------------------------------------------------------------------------------------------------------
class TakeExportXML : public TakeExport
{
  friend class ExportFactoryXML;
  friend class TakeListXML;

public:
  virtual ~TakeExportXML();

  EventTrackExport* createEventTrack(
    EventTrackExport::EventTrackType trackType,
    GUID           guid,
    const wchar_t* name,
    unsigned int   eventTrackChannelID,
    unsigned int   userData) NM_OVERRIDE;

  static TakeExport* createTake(
    const wchar_t* name,
    float          secDuration,
    float          fps,
    bool           loop,
    float          clipStart,
    float          clipEnd);

  const AttributeBlockExport* getAttributeBlock() const NM_OVERRIDE;
  AttributeBlockExport* getAttributeBlock() NM_OVERRIDE;
  unsigned int getNumEventTracks() const NM_OVERRIDE;
  EventTrackExport* getEventTrack(unsigned int index) NM_OVERRIDE;
  const EventTrackExport* getEventTrack(unsigned int index) const NM_OVERRIDE;
  EventTrackExport* getEventTrack(GUID guid) NM_OVERRIDE;
  const EventTrackExport* getEventTrack(GUID guid) const NM_OVERRIDE;
  float getCachedTakeSecondsDuration() const NM_OVERRIDE;
  float getCachedTakeFPS() const NM_OVERRIDE;
  bool getLoop() const NM_OVERRIDE;
  float getClipStart() const NM_OVERRIDE;
  float getClipEnd() const NM_OVERRIDE;
  bool write() NM_OVERRIDE;

  // AssetExport functions
  const char* getName() const NM_OVERRIDE;
  GUID getGUID() const NM_OVERRIDE;
  const char* getDestFilename() const NM_OVERRIDE;

protected:
  TakeExportXML(XMLElement* xmlElement, const std::string& destFilename);
  TakeExportXML(XMLElement* xmlElement);
  TakeExportXML(const wchar_t* listName, float secDuration, float fps, const wchar_t* destFilename);
  void initFromElement();

  typedef std::vector<EventTrackExport*> EventTrackExportList;

  XMLElement*           m_xmlElement;
  EventTrackExportList  m_eventTracks;
  std::string           m_destFilename;
  AttributeBlockXML*    m_xmlAttributeBlock;
};

//----------------------------------------------------------------------------------------------------------------------
/// TakeListXML
//----------------------------------------------------------------------------------------------------------------------
class TakeListXML : public TakeList
{
  friend class ExportFactoryXML;

public:
  virtual ~TakeListXML();

  TakeExport* createTake(
    const wchar_t* name,
    float          secDuration,
    float          fps,
    bool           loop,
    float          clipStart = 0.0,
    float          clipEnd = 1.0) NM_OVERRIDE;

  unsigned int getNumTakes() const NM_OVERRIDE;
  TakeExport* getTake(unsigned int index) NM_OVERRIDE;
  const TakeExport* getTake(unsigned int index) const NM_OVERRIDE;
  const char* getName() const NM_OVERRIDE;
  GUID getGUID() const NM_OVERRIDE;
  const char* getSourceAnimFilename() const NM_OVERRIDE;
  unsigned int getFormatVersion() const NM_OVERRIDE;
  const char* getDestFilename() const NM_OVERRIDE;
  bool write() NM_OVERRIDE;

protected:
  TakeListXML(XMLElement* xmlElement, const std::string& destFilename);
  TakeListXML(const wchar_t* sourceAnimFile, unsigned int formatVersion, const wchar_t* destFilename);

  typedef std::vector<TakeExportXML*> TakeExportXMLList;

  XMLElement*       m_xmlElement;
  TakeExportXMLList m_takes;
  std::string       m_destFilename;
};

//----------------------------------------------------------------------------------------------------------------------
///
//----------------------------------------------------------------------------------------------------------------------
class JointExportXML : public JointExport
{
  friend class RigExportXML;

public:
  void setName(const char* name) NM_OVERRIDE;
  void setRotation(float x, float y, float z, float w) NM_OVERRIDE;
  void setTranslation(float x, float y, float z) NM_OVERRIDE;
  void setVisiblity(bool visible) NM_OVERRIDE;
  void setBodyPlanTag(const char* tag) NM_OVERRIDE;
  void setRetargetingTag(const char* tag) NM_OVERRIDE;

  const char* getName() const NM_OVERRIDE;
  unsigned int getIndex() const NM_OVERRIDE;
  unsigned int getParentIndex() const NM_OVERRIDE;
  void getRotation(float& x, float& y, float& z, float& w) const NM_OVERRIDE;
  void getTranslation(float& x, float& y, float& z) const NM_OVERRIDE;
  bool getVisiblity() const NM_OVERRIDE;
  const char* getBodyPlanTag() const NM_OVERRIDE;
  const char* getRetargetingTag() const NM_OVERRIDE;

protected:
  JointExportXML(XMLElement* element);
  JointExportXML(unsigned int index, const wchar_t* jointName, unsigned int parentIndex);

  XMLElement* m_xmlElement;
};

//----------------------------------------------------------------------------------------------------------------------
///
//----------------------------------------------------------------------------------------------------------------------
class JointLimitExportXML : public JointLimitExport
{
  friend class RigExportXML;

public:
  void setLimitType(LimitType type) NM_OVERRIDE;

  void setJointIndex(unsigned int index) NM_OVERRIDE;
  void setOrientation(float x, float y, float z, float w) NM_OVERRIDE;
  void setOffsetOrientation(float x, float y, float z, float w) NM_OVERRIDE;

  void setTwistLow(float twist) const NM_OVERRIDE;
  void setTwistHigh(float twist) const NM_OVERRIDE;
  void setSwing1(float swing) const NM_OVERRIDE;
  void setSwing2(float swing) const NM_OVERRIDE;

  unsigned int getIndex() const NM_OVERRIDE;

  LimitType getLimitType() const NM_OVERRIDE;

  unsigned int getJointIndex() const NM_OVERRIDE;
  void getOrientation(float& x, float& y, float& z, float& w) const NM_OVERRIDE;
  void getOffsetOrientation(float& x, float& y, float& z, float& w) const NM_OVERRIDE;

  float getTwistLow() const NM_OVERRIDE;
  float getTwistHigh() const NM_OVERRIDE;
  float getSwing1() const NM_OVERRIDE;
  float getSwing2() const NM_OVERRIDE;

protected:
  JointLimitExportXML(XMLElement* element);
  JointLimitExportXML(unsigned int index);

  XMLElement* m_xmlElement;
};

//----------------------------------------------------------------------------------------------------------------------
/// IntMirrorMappingXML
//----------------------------------------------------------------------------------------------------------------------
class IntMirrorMappingXML
{
  friend class AnimationSetExportXML;
  friend class RigExportXML;

public:
  unsigned int getIndex() const;
  unsigned int getFirst() const;
  unsigned int getSecond() const;

protected:
  IntMirrorMappingXML(XMLElement* element);
  IntMirrorMappingXML(
    const char*  name,
    unsigned int index,
    unsigned int firstJointIndex,
    unsigned int secondJointIndex);

  XMLElement* m_xmlElement;
};

//----------------------------------------------------------------------------------------------------------------------
/// Rig
//----------------------------------------------------------------------------------------------------------------------
class RigExportXML : public RigExport
{
  friend class ExportFactoryXML;

public:
  // AssetExport functions
  const char* getName() const NM_OVERRIDE;
  GUID getGUID() const NM_OVERRIDE;
  const char* getDestFilename() const NM_OVERRIDE;

  // RigExport functions
  JointExport* createJoint(
    unsigned int   index,
    const wchar_t* jointName,
    unsigned int   parentIndex) NM_OVERRIDE;

  JointLimitExport* createJointLimit(unsigned int index) NM_OVERRIDE;

  void createMirrorMapping(
    unsigned int index,
    unsigned int firstJointIndex,
    unsigned int secondJointIndex) NM_OVERRIDE;

  void createJointMapping(
    unsigned int index,
    unsigned int firstJointIndex,
    unsigned int secondJointIndex) NM_OVERRIDE;

  void setJointOffsetTransform(
    unsigned int        index,
    const NMP::Vector3& translation,
    const NMP::Quat&    orientation) NM_OVERRIDE;

  void setJointPostOffsetTransform(
    unsigned int        index,
    const NMP::Quat&    postOffsetOrient) NM_OVERRIDE;

  void setGUID(const GUID guid) NM_OVERRIDE;
  void setDestFilename(const char* filename) NM_OVERRIDE;

  void setHipIndex(unsigned int hipIndex) NM_OVERRIDE;
  void setTrajectoryIndex(unsigned int trajectoryIndex) NM_OVERRIDE;
  void setBlendFrameOrientationQuat(float x, float y, float z, float w) NM_OVERRIDE;
  void setBlendFramePositionVec(float x, float y, float z) NM_OVERRIDE;
  void setRigSourceFileName(const char* sourceFileName) NM_OVERRIDE;

  unsigned int getHipIndex() const NM_OVERRIDE;
  unsigned int getTrajectoryIndex() const NM_OVERRIDE;
  void getBlendFrameOrientationQuat(float& x, float& y, float& z, float& w) const NM_OVERRIDE;
  void getBlendFramePositionVec(float& x, float& y, float& z) const NM_OVERRIDE;
  const char* getRigSourceFileName() const NM_OVERRIDE;

  unsigned int getNumJoints() const NM_OVERRIDE;
  JointExport* getJoint(unsigned int index) NM_OVERRIDE;
  const JointExport* getJoint(unsigned int index) const NM_OVERRIDE;

  float getRigScaleFactor() const NM_OVERRIDE;
  void setRigScaleFactor(float scale) NM_OVERRIDE;

  unsigned int getNumJointLimits() const NM_OVERRIDE;
  JointLimitExport* getJointLimit(unsigned int index) NM_OVERRIDE;
  const JointLimitExport* getJointLimit(unsigned int index) const NM_OVERRIDE;

  unsigned int getMirrorMappingCount() const NM_OVERRIDE;
  void getMirrorMapping(
    unsigned int  mappingIndex,
    unsigned int& firstJointIndex,
    unsigned int& secondJointIndex) const NM_OVERRIDE;

  float getRigRetargetScale() const NM_OVERRIDE;
  void setRigRetargetScale(float scale) NM_OVERRIDE;

  unsigned int getJointMappingCount() const NM_OVERRIDE;
  void getJointMapping(
    unsigned int  mappingIndex,
    unsigned int& firstJointIndex,
    unsigned int& secondJointIndex) const NM_OVERRIDE;

  void getJointOffsetTransform(
    unsigned int  index,
    NMP::Vector3& translation,
    NMP::Quat&    orientation) const NM_OVERRIDE;

  void getJointPostOffsetTransform(
    unsigned int  index,
    NMP::Quat& postOffsetOrient) const NM_OVERRIDE;

  bool write() NM_OVERRIDE;

  virtual ~RigExportXML();

  /// \brief  returns the mirror plane in which to mirror the character
  /// \return 0=YZ plane, 1=ZX plane, 2=XY plane
  virtual uint32_t getMirrorPlane() const NM_OVERRIDE;

  /// \brief  sets the mirror plane in which to mirror the character
  /// \param  plane - 0=YZ plane, 1=ZX plane, 2=XY plane
  virtual void setMirrorPlane(uint32_t plane) NM_OVERRIDE;

protected:
  RigExportXML(XMLElement* element, const std::string& destFilename, ExportFactoryXML* factory);
  RigExportXML(const GUID guid, const wchar_t* destFilename, const char* rigName);

  XMLElement*                       m_xmlElement;
  std::string                       m_destFilename;
  std::vector<JointExportXML*>      m_joints;
  std::vector<JointLimitExportXML*> m_jointLimits;
  std::vector<IntMirrorMappingXML*> m_jointMirrorMappings;
  std::vector<IntMirrorMappingXML*> m_jointMappings;
};

//----------------------------------------------------------------------------------------------------------------------
/// AnimationPoseDefExportXML
//----------------------------------------------------------------------------------------------------------------------
class AnimationPoseDefExportXML : public AnimationPoseDefExport
{
  friend class AssetExport;
public:
  virtual ~AnimationPoseDefExportXML();

  // AnimationPoseDefExport overrides.
  void setName(const char* name) NM_OVERRIDE;
  void setAnimationFile(const wchar_t* file) NM_OVERRIDE;
  void setAnimationTake(const wchar_t* take) NM_OVERRIDE;
  void setPoseFrameIndex(int32_t frame) NM_OVERRIDE;

  const char* getName() const NM_OVERRIDE;
  const char* getAnimationFile() const NM_OVERRIDE;
  const char* getAnimationTake() const NM_OVERRIDE;
  int32_t     getPoseFrameIndex() const NM_OVERRIDE;

  AnimationPoseDefExportXML(XMLElement* parent, const char* );
  AnimationPoseDefExportXML(XMLElement* element);
protected:
  XMLElement* m_xmlElement;
};

//----------------------------------------------------------------------------------------------------------------------
/// AnimationEntryExportXML
//----------------------------------------------------------------------------------------------------------------------
class AnimationEntryExportXML : public AnimationEntryExport
{
  friend class AnimationSetExportXML;

public:
  virtual ~AnimationEntryExportXML() { }

  unsigned int getIndex() const NM_OVERRIDE;
  const char* getAnimationFilename() const NM_OVERRIDE;
  const char* getTakeFilename() const NM_OVERRIDE;
  const char* getTakeName() const NM_OVERRIDE;
  const char* getSyncTrack() const NM_OVERRIDE;
  const char* getOptions() const NM_OVERRIDE;
  const char* getFormat() const NM_OVERRIDE;
  TakeExport* getTakeExportData(const char* rootDir) NM_OVERRIDE;

  const AnimationSetExportXML* getAnimationSet() const;

protected:
  AnimationEntryExportXML(
    XMLElement*                  element,
    ExportFactoryXML*            factory,
    const AnimationSetExportXML* animationSet);

  AnimationEntryExportXML(
    unsigned int                 index,
    const wchar_t*               animFile,
    const wchar_t*               takeFile,
    const wchar_t*               take,
    const wchar_t*               syncTrack,
    const char*                  format,
    const char*                  options,
    const AnimationSetExportXML* animationSet);

  XMLElement*                   m_xmlElement;
  ExportFactoryXML*             m_exportFactory;
  const AnimationSetExportXML*  m_animationSet;
  TakeExport*                   m_loadedTakeExport;
};

//----------------------------------------------------------------------------------------------------------------------
/// AnimationSet
//----------------------------------------------------------------------------------------------------------------------
class AnimationSetExportXML : public AnimationSetExport
{
  friend class AnimationLibraryXML;
  friend class NetworkDefExportXML;

public:
  class RigRef : public XMLReference
  {
  public:
    RigRef(GUID guid, const char* rigFilename, const char* rigName) :
      XMLReference("RigRef", guid, rigFilename, rigName) { }
  };

  class PhysicsRigDefRef : public XMLReference
  {
  public:
    PhysicsRigDefRef(GUID guid, const char* rigFilename, const char* rigName) :
      XMLReference("PhysicsRigDefRef", guid, rigFilename, rigName) { }
  };

  class BodyDefRef : public XMLReference
  {
  public:
    BodyDefRef(GUID guid, const char* bodyFilename, const char* bodyName) :
      XMLReference("BodyDefRef", guid, bodyFilename, bodyName) { }
  };

  class CharacterControllerRef : public XMLReference
  {
  public:
    CharacterControllerRef(GUID guid, const char* controllerFilename, const char* controllerName) :
      XMLReference("CharacterControllerRef", guid, controllerFilename, controllerName) { }
  };

  class InteractionProxyRef : public XMLReference
  {
  public:
    InteractionProxyRef(GUID guid, const char* proxyFilename, const char* proxyName) :
        XMLReference("InteractionProxyRef", guid, proxyFilename, proxyName) { }
  };

  virtual ~AnimationSetExportXML();

  AnimationEntryExport* createAnimationEntry(
    unsigned int   index,
    const wchar_t* animFile,
    const wchar_t* takeFile,
    const wchar_t* take,
    const wchar_t* syncTrack,
    const char*    format,
    const char*    options) NM_OVERRIDE;

  void createEventUserdataMirrorMapping(
    unsigned int index,
    int          firstUserdata,
    int          secondUserdata) NM_OVERRIDE;

  void createEventTrackMirrorMapping(
    unsigned int index,
    int          firstTrack,
    int          secondTrack) NM_OVERRIDE;

  void setRig(const RigExport* rig) NM_OVERRIDE;
  void setPhysicsRigDef(const PhysicsRigDefExport* physicsRigDef) NM_OVERRIDE;
  void setBodyDef(const BodyDefExport* bodyDef) NM_OVERRIDE;
  void setCharacterController(const CharacterControllerExport* characterController) NM_OVERRIDE;
  void setInteractionProxy(const InteractionProxyExport* proxy) NM_OVERRIDE;

  unsigned int getNumAnimationEntries() const NM_OVERRIDE;
  AnimationEntryExport* getAnimationEntry(unsigned int index) NM_OVERRIDE;
  const AnimationEntryExport* getAnimationEntry(unsigned int index) const NM_OVERRIDE;

  unsigned int getNumEventUserDataMirrorMappings() const NM_OVERRIDE;
  void getEventUserDataMirrorMapping(unsigned int index, int& firstUserData, int& secondUserData) const NM_OVERRIDE;

  unsigned int getNumEventTrackMirrorMappings() const NM_OVERRIDE;
  void getEventTrackMirrorMapping(unsigned int index, int& firstTrack, int& secondTrack) const NM_OVERRIDE;

  RigExport* getRig() NM_OVERRIDE;
  const RigExport* getRig() const NM_OVERRIDE;

  PhysicsRigDefExport* getPhysicsRigDef() NM_OVERRIDE;
  const PhysicsRigDefExport* getPhysicsRigDef() const NM_OVERRIDE;

  BodyDefExport* getBodyDef() NM_OVERRIDE;
  const BodyDefExport* getBodyDef() const NM_OVERRIDE;

  CharacterControllerExport* getCharacterController() NM_OVERRIDE;
  const CharacterControllerExport* getCharacterController() const NM_OVERRIDE;

  InteractionProxyExport* getInteractionProxy() NM_OVERRIDE;
  const InteractionProxyExport* getInteractionProxy() const NM_OVERRIDE;

  const char* getName() const NM_OVERRIDE;
  unsigned int getIndex() const NM_OVERRIDE;

  const AnimationLibraryXML* getAnimationLibrary() const;

protected:
  AnimationSetExportXML(XMLElement* element, ExportFactoryXML* factory, const AnimationLibraryXML* animLibrary);
  AnimationSetExportXML(unsigned int index, const wchar_t* animSetName, const AnimationLibraryXML* animLibrary);

  XMLElement*                           m_xmlElement;
  std::vector<AnimationEntryExportXML*> m_animationEntries;
  std::vector<IntMirrorMappingXML*>     m_mirroredEventUserdatas;
  std::vector<IntMirrorMappingXML*>     m_mirroredEventTracks;
  ExportFactoryXML*                     m_exportFactory;
  const AnimationLibraryXML*            m_animationLibrary;
};

//----------------------------------------------------------------------------------------------------------------------
/// AnimationLibrary
//----------------------------------------------------------------------------------------------------------------------
class AnimationLibraryXML : public AnimationLibraryExport
{
  friend class ExportFactoryXML;
  friend class NetworkDefExportXML;

public:
  AnimationSetExport* createAnimationSet(unsigned int index, const wchar_t* setName) NM_OVERRIDE;
  unsigned int getNumAnimationSets() const NM_OVERRIDE;
  AnimationSetExport* getAnimationSet(unsigned int index) NM_OVERRIDE;
  const AnimationSetExport* getAnimationSet(unsigned int index) const NM_OVERRIDE;
  bool write() NM_OVERRIDE;
  virtual ~AnimationLibraryXML();

  // AssetExport functions
  const char* getName() const NM_OVERRIDE;
  GUID getGUID() const NM_OVERRIDE;
  const char* getDestFilename() const NM_OVERRIDE;

  // this should only be used in very limited circumstances
  XMLElement* getXMLElement() const;

protected:
  AnimationLibraryXML();
  AnimationLibraryXML(XMLElement* element, const std::string& destFilename, ExportFactoryXML* factory);
  AnimationLibraryXML(GUID guid, const wchar_t* libraryName, const wchar_t* destFilename);

  XMLElement*                         m_xmlElement;
  std::vector<AnimationSetExportXML*> m_animationSets;
  std::string                         m_destFilename;
};

//----------------------------------------------------------------------------------------------------------------------
/// Exported NetworkDef
//----------------------------------------------------------------------------------------------------------------------
class NetworkDefExportXML : public NetworkDefExport
{
  friend class ExportFactoryXML;

public:
  class AnimLibraryRef : public XMLReference
  {
  public:
    AnimLibraryRef(GUID guid, const char* animLibraryFile);
  };

  class MessagePresetLibraryRef : public XMLReference
  {
  public:
    MessagePresetLibraryRef(GUID guid, const char* messagePresetLibraryFile);
  };

  // Specific to this implementation
  ~NetworkDefExportXML();

  void setRootNodeNetworkID(unsigned int rootNodeNetworkID) NM_OVERRIDE;

  void setNetworkWorldOrientation(
    const NMP::Vector3& forwardVector,
    const NMP::Vector3& rightVector,
    const NMP::Vector3& upVector) NM_OVERRIDE;

  NodeExport* createNode(
    unsigned int networkID,
    unsigned int typeID,
    unsigned int downstreamParentID,
    bool         downstreamParentMultiplyConnected,
    const char*  name,
    bool         persistent = false) NM_OVERRIDE;

  NodeExport* createNode(
    unsigned int   networkID,
    unsigned int   typeID,
    unsigned int   downstreamParentID,
    bool           downstreamParentMultiplyConnected,
    const wchar_t* name,
    bool           persistent = false) NM_OVERRIDE;

  MessageExport* createMessage(
    const wchar_t* messageName,
    MR::MessageType messageType,
    MR::MessageID messageID) NM_OVERRIDE;

  void setAnimationLibrary(const AnimationLibraryExport* animationLibrary) NM_OVERRIDE;
  AnimationLibraryExport* getAnimationLibrary() NM_OVERRIDE;
  const AnimationLibraryExport* getAnimationLibrary() const NM_OVERRIDE;

  void setMessagePresetLibrary(const MessagePresetLibraryExport* presetLibrary) NM_OVERRIDE;
  MessagePresetLibraryExport* getMessagePresetLibrary() NM_OVERRIDE;
  const MessagePresetLibraryExport* getMessagePresetLibrary() const NM_OVERRIDE;

  // AssetExport functions
  const char* getName() const NM_OVERRIDE;
  GUID getGUID() const NM_OVERRIDE;
  void setGUID(const GUID newGuid);
  const char* getDestFilename() const NM_OVERRIDE;

  unsigned int getRootNodeID() const;

  void getNetworkWorldOrientation(
    NMP::Vector3& forwardVector,
    NMP::Vector3& rightVector,
    NMP::Vector3& upVector) NM_OVERRIDE;

  unsigned int getNumNodes() const NM_OVERRIDE;
  NodeExport* getNode(unsigned int networkID) NM_OVERRIDE;
  const NodeExport* getNode(unsigned int networkID) const NM_OVERRIDE;

  unsigned int getNumMessages() const NM_OVERRIDE;
  MessageExport* getMessage(unsigned int index) NM_OVERRIDE;
  const MessageExport* getMessage(unsigned int index) const NM_OVERRIDE;

  bool write() NM_OVERRIDE;

  const char* getAnimationLibraryFileName() const;
  const char* getAnimationLibraryGuid() const;
  const char* getPresetLibraryGuid() const;
  void setDestFilename(const wchar_t* destFilename);
protected:
  NetworkDefExportXML(XMLElement* element, const std::string& destFilename, ExportFactoryXML* factory);
  NetworkDefExportXML(GUID revGUID, const wchar_t* networkName, const wchar_t* destFilename);

  XMLElement*                 m_xmlElement;
  std::string                 m_destFilename;
  ExportFactoryXML*           m_exportFactory;
  std::vector<NodeExport*>    m_nodes;
  std::vector<MessageExport*> m_messages;
};

//----------------------------------------------------------------------------------------------------------------------
inline NetworkDefExportXML::AnimLibraryRef::AnimLibraryRef(GUID guid, const char* animLibraryFile)
: XMLReference("AnimLibraryRef", guid, animLibraryFile)
{

}

//----------------------------------------------------------------------------------------------------------------------
inline NetworkDefExportXML::MessagePresetLibraryRef::MessagePresetLibraryRef(
  GUID guid,
  const char* messagePresetLibraryFile)
: XMLReference("MessagePresetLibraryRef", guid, messagePresetLibraryFile)
{
}

//----------------------------------------------------------------------------------------------------------------------
/// ExportFactoryXML
//----------------------------------------------------------------------------------------------------------------------
class ExportFactoryXML : public ExportFactory
{
public:
  ExportFactoryXML();
  ~ExportFactoryXML();

  NetworkDefExport* createNetworkDef(
    GUID           revisionGUID,
    const wchar_t* networkName,
    const wchar_t* destFilename) NM_OVERRIDE;

  void destroyNetworkDef(NetworkDefExport* expObject) NM_OVERRIDE;

  TakeExport* createTake(
    const wchar_t* listName,
    float          secDuration,
    float          fps,
    const wchar_t* destFilename) NM_OVERRIDE;
  void destroyTake(TakeExport* expObject) NM_OVERRIDE;

  TakeList* createTakeList(
    const wchar_t* sourceAnimFile,
    const wchar_t* destFilename) NM_OVERRIDE;
  void destroyTakeList(TakeList* takeList) NM_OVERRIDE;

  AnimationLibraryExport* createAnimationLibrary(
    GUID           guid,
    const wchar_t* animLibraryName,
    const wchar_t* destFilename) NM_OVERRIDE;
  void destroyAnimationLibrary(AnimationLibraryExport* expObject) NM_OVERRIDE;

  RigExport* createRig(GUID guid, const wchar_t* destFilename, const char* rigName) NM_OVERRIDE;
  void destroyRig(RigExport* rig) NM_OVERRIDE;

  PhysicsRigDefExport* createPhysicsRigDef(GUID guid, const wchar_t* destFilename, const char* rigName) NM_OVERRIDE;
  void destroyPhysicsRigDef(PhysicsRigDefExport* physicsRigDef) NM_OVERRIDE;

  BodyDefExport* createBodyDef(GUID guid, const wchar_t* destFilename, const char* bodyName) NM_OVERRIDE;
  void destroyBodyDef(BodyDefExport* bodyDef) NM_OVERRIDE;

  CharacterControllerExport* createCharacterController(GUID guid, const wchar_t* destFilename, const char* controllerName) NM_OVERRIDE;
  void destroyCharacterController(CharacterControllerExport* characterController) NM_OVERRIDE;

  InteractionProxyExport* createInteractionProxy(GUID guid, const wchar_t* destFilename, const char* proxyName) NM_OVERRIDE;
  void destroyInteractionProxy(InteractionProxyExport* interactionProxy) NM_OVERRIDE;

  MessagePresetLibraryExport* createMessagePresetLibrary(GUID guid, const wchar_t* destinationFilename) NM_OVERRIDE;
  void destroyMessagePresetLibrary(MessagePresetLibraryExport* presetsLibrary) NM_OVERRIDE;

  AnalysisLibraryExport* createAnalysisLibrary(GUID guid, const wchar_t* destFilename, const wchar_t* resultFilename) NM_OVERRIDE;
  void destroyAnalysisLibrary(AnalysisLibraryExport* analysisLibrary) NM_OVERRIDE;

  AnalysisResultLibraryExport* createAnalysisResultLibrary(GUID guid, const wchar_t* destFilename) NM_OVERRIDE;
  void destroyAnalysisResultLibrary(AnalysisResultLibraryExport* analysisResultLibrary) NM_OVERRIDE;

  /// Loads a AssetExport and all files it depends upon.  It calls into the
  /// doLoadAssetSource function to actually get the xml text (the default implementation
  /// simply loads the file from disk)
  ///
  /// The assets returned are managed within ExportFactoryXML.
  AssetExport* loadAsset(const char* srcFilename) NM_OVERRIDE;

  /// \brief Optimised path to retrieve only the guid of a rig or network, this function doesn't load the entire asset into memory, only
  ///        the first tag, and then strips the guid attribute from it.
  std::string findGUIDForAsset(const std::string& srcFilename);

  // unloads a previously loaded asset and removes it from the cache.
  virtual bool unloadAsset(const char* srcFileName);

  /// \brief returns true if the asset filename queried is already cached.
  NM_INLINE bool hasAlreadyLoaded(const std::string& assetFn)
  {
    return m_fileAssetMap.find(assetFn) == m_fileAssetMap.end();
  }

  /// \brief returns the cached asset if available
  NM_INLINE const AssetExport* getCachedAsset(const std::string& assetFn) const
  {
    std::map<std::string, XMLAsset>::const_iterator assetIt = m_fileAssetMap.find(assetFn);
    NMP_ASSERT_MSG(assetIt != m_fileAssetMap.end(), "Asset not loaded, yet!? make sure to call a non-const method to \
                                                     trigger ExportFactoryXML::loadAsset(%s) first!", assetFn.c_str());
    return (*assetIt).second.m_asset;
  }

protected:
  /// This function handles bringing the data (xml text in this case) into memory.
  /// The default implementation simply loads data from a file, but could
  /// easily be overloaded to lookup xml text that already resides in memory.
  /// dataSize should be set to the length of the returned xml data.
  virtual const char* doLoadAssetSource(const char* srcFilename, size_t& dataSize);

  /// Frees asset source data (xml text) that was allocated via the
  /// doLoadAssetSource() function.  This is called once the AssetExport has been
  /// created.  Naturally, this can be overloaded to do nothing if the xml source
  /// is to persist in memory.
  virtual void doFreeAssetSource(const char* assetSource);

  struct XMLAsset
  {
    NM::TiXmlDocument* m_document;
    AssetExport*       m_asset;
  };

  std::map<std::string, XMLAsset> m_fileAssetMap;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace ME

#ifdef _MSC_VER
  #pragma warning(pop)
#endif

#endif // MCEXPORTXML_H
