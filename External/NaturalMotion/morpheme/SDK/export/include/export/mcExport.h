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
#ifndef MCEXPORT_H
#define MCEXPORT_H
//----------------------------------------------------------------------------------------------------------------------
#include <vector>
#include <string>
#include "../../../sharedDefines/mSharedDefines.h"
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMatrix34.h"
#include "NMPlatform/NMVector3.h"

//----------------------------------------------------------------------------------------------------------------------
/// Classes that wrap the underlying representation of
/// exported assets.
///
/// These classes are used by morpheme:connect to write out
/// exported data in the chosen format and by the assetcompiler
/// tool to read it back in.
//----------------------------------------------------------------------------------------------------------------------
namespace ME
{

//----------------------------------------------------------------------------------------------------------------------
class NodeExport;
class TakeExport;
class PhysicsRigDefExport;
class BodyDefExport;
class InteractionProxyExport;
class MessagePresetLibraryExport;

typedef const char* GUID;

//----------------------------------------------------------------------------------------------------------------------
/// Base class of all exported 1st class assets
//----------------------------------------------------------------------------------------------------------------------
class AssetExport
{
public:
  virtual ~AssetExport() { };

  virtual const char* getName() const = 0;
  virtual GUID getGUID() const = 0;
  virtual const char* getDestFilename() const = 0;
  virtual bool write() = 0;
};

//----------------------------------------------------------------------------------------------------------------------
/// All classes can use this for custom data
/// The asset compiler can convert this custom data to runtime data
//----------------------------------------------------------------------------------------------------------------------
class AttributeExport
{
public:
  enum AttributeType
  {
    ATTR_TYPE_INT,
    ATTR_TYPE_UINT,
    ATTR_TYPE_DOUBLE,
    ATTR_TYPE_STRING,
    ATTR_TYPE_BOOL,
    ATTR_TYPE_VECTOR3,
    ATTR_TYPE_QUAT,
    ATTR_TYPE_MATRIX34,
    ATTR_TYPE_BOOL_ARRAY,
    ATTR_TYPE_UINT_ARRAY,
    ATTR_TYPE_FLOAT_ARRAY,
    ATTR_TYPE_VECTOR3_ARRAY,
    ATTR_TYPE_STRING_ARRAY,
  };

  virtual const char* getName() const = 0;

  virtual bool getAsInt(int& value) const = 0;
  virtual bool getAsUInt(unsigned int& value) const = 0;
  virtual bool getAsDouble(double& value) const = 0;
  virtual bool getAsFloat(float& value) const; // default implementation casts from a stored double
  virtual bool getAsString(const char *& value) const = 0;
  virtual bool getAsBool(bool& value) const = 0;
  virtual bool getAsVector3(NMP::Vector3& value) const = 0;
  virtual bool getAsQuat(NMP::Quat& value) const = 0;
  virtual bool getAsMatrix34(NMP::Matrix34& value) const = 0;
  virtual bool getAsBoolArray(bool values[], unsigned int size) const = 0;
  virtual bool getAsUIntArray(unsigned int values[], unsigned int size) const = 0;
  virtual bool getAsFloatArray(float values[], unsigned int size) const = 0;
  virtual bool getAsVector3Array(NMP::Vector3 values[], unsigned int size) const = 0;
  virtual bool getAsStringArray(const char* values[], unsigned int size) const = 0;

  virtual bool setAsInt(int value) = 0;
  virtual bool setAsUInt(unsigned int value) = 0;
  virtual bool setAsDouble(double value) = 0;
  virtual bool setAsFloat(float value); // default implementation stores as a double
  virtual bool setAsString(const char* value) = 0;
  virtual bool setAsBool(bool value) = 0;
  virtual bool setAsVector3(const NMP::Vector3& value) = 0;
  virtual bool setAsQuat(const NMP::Quat& value) = 0;
  virtual bool setAsMatrix34(const NMP::Matrix34& value) = 0;
  virtual bool setAsBoolArray(const bool values[], unsigned int size) = 0;
  virtual bool setAsUIntArray(const unsigned int values[], unsigned int size) = 0;
  virtual bool setAsFloatArray(const float values[], unsigned int size) = 0;
  virtual bool setAsVector3Array(const NMP::Vector3 values[], unsigned int size) = 0;
  virtual bool setAsStringArray(const char* values[], unsigned int size) = 0;

  virtual bool appendToUIntArray(unsigned int value) = 0;
  virtual bool appendToFloatArray(float value) = 0;
  virtual bool appendToVector3Array(const NMP::Vector3& value) = 0;
  virtual bool appendToStringArray(const char* value) = 0;

  virtual AttributeType getType() const = 0;
  virtual unsigned int getSize() const = 0;

  bool isNameEqual(const char* destName) const;
  bool isInt() const;
  bool isUInt() const;
  bool isDouble() const;
  bool isString() const;
  bool isBool() const;
  bool isVector3() const;
  bool isQuat() const;
  bool isMatrix34() const;
  bool isBoolArray() const;
  bool isUIntArray() const;
  bool isFloatArray() const;
  bool isVector3Array() const;
  bool isStringArray() const;

  bool isArray() const;
};

//----------------------------------------------------------------------------------------------------------------------
/// All classes that use attributes contain them in this block
//----------------------------------------------------------------------------------------------------------------------
class AttributeBlockExport
{
public:
  virtual unsigned int getNumAttributes() const = 0;
  virtual const AttributeExport* getAttributeByIndex(unsigned int attrIdx) const = 0;
  virtual AttributeExport* getAttributeByIndex(unsigned int attrIdx) = 0;

  /// these functions have an implementation provided, but could be more efficient in some implementations
  virtual const AttributeExport* getAttributeByName(const char* name) const;
  virtual AttributeExport* getAttributeByName(const char* name);
  bool hasAttribute(const char* name) const;

  bool getIntAttribute(const char* name, int& value) const;
  bool getUIntAttribute(const char* name, unsigned int& value) const;
  bool getDoubleAttribute(const char* name, double& value) const;
  bool getStringAttribute(const char* name, const char *& value) const;
  bool getBoolAttribute(const char* name, bool& value) const;
  bool getVector3Attribute(const char* name, NMP::Vector3& value) const;
  bool getQuatAttribute(const char* name, NMP::Quat& value) const;
  bool getMatrix34Attribute(const char* name, NMP::Matrix34& value) const;
  bool getAttributeArraySize(const char* name, unsigned int& size) const;
  bool getBoolAttributeArray(const char* name, bool values[], unsigned int size) const;
  bool getUIntAttributeArray(const char* name, unsigned int values[], unsigned int size) const;
  bool getFloatAttributeArray(const char* name, float values[], unsigned int size) const;
  bool getVector3AttributeArray(const char* name, NMP::Vector3 values[], unsigned int size) const;
  bool getStringAttributeArray(const char* name, const char* values[], unsigned int size) const;

  bool setIntAttribute(const char* name, int value, bool autoCreate = true);
  bool setUIntAttribute(const char* name, unsigned int value, bool autoCreate = true);
  bool setDoubleAttribute(const char* name, double value, bool autoCreate = true);
  bool setStringAttribute(const char* name, const char* value, bool autoCreate = true);
  bool setBoolAttribute(const char* name, bool value, bool autoCreate = true);
  bool setVector3Attribute(const char* name, const NMP::Vector3& value, bool autoCreate = true);
  bool setQuatAttribute(const char* name, const NMP::Quat& value, bool autoCreate = true);
  bool setMatrix34Attribute(const char* name, const NMP::Matrix34& value, bool autoCreate = true);
  bool setBoolAttributeArray(
    const char*        name,
    const bool         values[],
    unsigned int       size,
    bool               autoCreate = true);
  bool setUIntAttributeArray(
    const char*        name,
    const unsigned int values[],
    unsigned int       size,
    bool               autoCreate = true);
  bool setFloatAttributeArray(
    const char*        name,
    const float values[],
    unsigned int       size,
    bool               autoCreate = true);
  bool setVector3AttributeArray(
    const char*        name,
    const NMP::Vector3 values[],
    unsigned int       size,
    bool               autoCreate = true);
  bool setStringAttributeArray(
    const char*  name,
    const char*  values[],
    unsigned int size,
    bool autoCreate = true);

  virtual AttributeExport* createDoubleAttribute(const char* name, double value) = 0;
  virtual AttributeExport* createIntAttribute(const char* name, int value) = 0;
  virtual AttributeExport* createUIntAttribute(const char* name, unsigned int value) = 0;
  virtual AttributeExport* createStringAttribute(const char* name, const char* value) = 0;
  virtual AttributeExport* createBoolAttribute(const char* name, bool value) = 0;
  virtual AttributeExport* createVector3Attribute(const char* name, const NMP::Vector3& value) = 0;
  virtual AttributeExport* createQuatAttribute(const char* name, const NMP::Quat& value) = 0;
  virtual AttributeExport* createMatrix34Attribute(const char* name, const NMP::Matrix34& value) = 0;
  virtual AttributeExport* createBoolAttributeArray(
    const char*        name,
    const bool         values[],
    unsigned int       size) = 0;
  virtual AttributeExport* createUIntAttributeArray(
    const char*        name,
    const unsigned int values[],
    unsigned int       size) = 0;
  virtual AttributeExport* createFloatAttributeArray(
    const char*        name,
    const float        values[],
    unsigned int       size) = 0;
  virtual AttributeExport* createVector3AttributeArray(
    const char*        name,
    const NMP::Vector3 values[],
    unsigned int       size) = 0;
  virtual AttributeExport* createStringAttributeArray(
    const char*  name,
    const char*  values[],
    unsigned int size) = 0;
};

//----------------------------------------------------------------------------------------------------------------------
/// Exception thrown when an error occurs
//----------------------------------------------------------------------------------------------------------------------
class ExportException
{
public:
  ExportException(const char* message) : m_message(message) { }
  const char* getMessage() const { return m_message; }

protected:
  const char* m_message;
};

//----------------------------------------------------------------------------------------------------------------------
/// A data block written out by the LUA serialize functions
/// and passed into Node/Condition create functions.
//----------------------------------------------------------------------------------------------------------------------
class DataBlockExport
{
public:
  virtual ~DataBlockExport() { };

  virtual size_t getByteCount(const char* desc) const = 0;

  virtual bool readString(std::string& data_out, const char* desc) const = 0;
  virtual bool readUnicodeString(std::wstring& data_out, const char* desc) const = 0;
  virtual bool readChar(char& data_out, const char* desc) const = 0;
  virtual bool readCharArray(char* data_out, size_t buffer_size, size_t& length_out, const char* desc) const = 0;
  virtual bool readBool(bool& data_out, const char* desc) const = 0;
  virtual bool readBoolArray(bool* data_out, size_t buffer_size, size_t& length_out, const char* desc) const = 0;
  virtual bool readInt(int& data_out, const char* desc) const = 0;
  virtual bool readIntArray(int* data_out, size_t buffer_size, size_t& length_out, const char* desc) const = 0;
  virtual bool readUInt(unsigned int& data_out, const char* desc) const = 0;
  virtual bool readUIntArray(unsigned int* data_out, size_t buffer_size, size_t& length_out, const char* desc) const = 0;
  virtual bool readUInt64(uint64_t& data_out, const char* desc) const = 0;
  virtual bool readUInt64Array(uint64_t* data_out, size_t buffer_size, size_t& length_out, const char* desc) const = 0;
  virtual bool readFloat(float& data_out, const char* desc) const = 0;
  virtual bool readFloatArray(float* data_out, size_t buffer_size, size_t& length_out, const char* desc) const = 0;
  virtual bool readDouble(double& data_out, const char* desc) const = 0;
  virtual bool readDoubleArray(double* data_out, size_t buffer_size, size_t& length_out, const char* desc) const = 0;
  virtual bool readIntAttribute(int& data_out, const char* elementDesc, const char* attribDesc) const = 0;  // Reads an int attribute from an element.
  virtual bool readUndefined(const char* desc) const = 0;
  virtual bool readNetworkNodeId(int& data_out, const char* desc) const = 0;
  virtual bool readVector3(const NMP::Vector3& data_out, const char* desc) const = 0;
  virtual bool readVector4(const NMP::Quat& data_out, const char* desc) const = 0;

  // NOTE:  all description strings are converted to UTF-8
  virtual void writeString(const std::string& data, const char* desc = 0) = 0;
  virtual void writeUnicodeString(const wchar_t* data, const char* desc = 0) = 0;
  virtual void writeChar(char data, const char* desc = 0) = 0;
  virtual void writeCharArray(const char* data, size_t length, const char* desc = 0) = 0;
  virtual void writeBool(bool data, const char* desc = 0) = 0;
  virtual void writeBoolArray(const bool* data, size_t length, const char* desc = 0) = 0;
  virtual void writeInt(int data, const char* desc = 0) = 0;
  virtual void writeIntArray(const int* data, size_t length, const char* desc = 0) = 0;
  virtual void writeUInt(unsigned int data, const char* desc = 0) = 0;
  virtual void writeNetworkNodeId(int nodeId, const char* desc = 0) = 0;
  virtual void writeNetworkNodeIdWithPinIndex(int nodeId, int pinIndex, const char * desc = 0) = 0; 
  virtual void writeMessageId(int messageId, const char* desc = 0) = 0; 
  virtual void writeAnimationId(int nodeId, const char* desc = 0) = 0;
  virtual void writeUIntArray(const unsigned int* data, size_t length, const char* desc = 0) = 0;
  virtual void writeFloat(float data, const char* desc = 0) = 0;
  virtual void writeFloatArray(const float* data, size_t length, const char* desc = 0) = 0;
  virtual void writeDouble(double data, const char* desc = 0) = 0;
  virtual void writeDoubleArray(const double* data, size_t length, const char* desc = 0) = 0;
  virtual void writeUndefined(const char* desc)  = 0;
  virtual void writeVector3(const NMP::Vector3& data, const char* desc) = 0;
  virtual void writeVector4(const NMP::Quat& data, const char* desc) = 0;
  virtual void padToAlignment(unsigned int alignmentValue) = 0;

  virtual unsigned int getDataBlockSize() const = 0;
  virtual void writeDataBlock(void*) const = 0;

protected:
  DataBlockExport() { }
};

//----------------------------------------------------------------------------------------------------------------------
/// AnalysisNode
//----------------------------------------------------------------------------------------------------------------------
class AnalysisNodeExport
{
public:
  virtual ~AnalysisNodeExport() { }

  virtual const char* getName() const = 0;
  virtual const char* getTypeName() const = 0;
  virtual DataBlockExport* getDataBlock() = 0;
  virtual const DataBlockExport* getDataBlock() const = 0;

protected:
  AnalysisNodeExport() { }
};

//----------------------------------------------------------------------------------------------------------------------
/// AnalysisTaskExport
//----------------------------------------------------------------------------------------------------------------------
class AnalysisTaskExport
{
public:
  virtual ~AnalysisTaskExport() { }

  virtual const char* getName() const = 0;
  virtual const char* getAnalysisType() const = 0;
  virtual DataBlockExport* getDataBlock() = 0;
  virtual const DataBlockExport* getDataBlock() const = 0;

  virtual AnalysisNodeExport* createAnalysisNode(
    const char* name, 
    const char* typeName) = 0;

  virtual unsigned int getNumAnalysisNodes() const = 0;
  virtual AnalysisNodeExport* getAnalysisNode(unsigned int index) = 0;
  virtual const AnalysisNodeExport* getAnalysisNode(unsigned int index) const = 0;

protected:
  AnalysisTaskExport() { }
};

//----------------------------------------------------------------------------------------------------------------------
/// AnalysisResultLibraryExport
//----------------------------------------------------------------------------------------------------------------------
class AnalysisResultLibraryExport : public AssetExport
{
public:
  virtual ~AnalysisResultLibraryExport() { }

  virtual const char* getName() const = 0;

  virtual AnalysisNodeExport* createTaskResult(const char* name) = 0;

  virtual unsigned int getNumTaskResult() const = 0;
  virtual AnalysisNodeExport* getTaskResult(unsigned int index) = 0;
  virtual const AnalysisNodeExport* getTaskResult(unsigned int index) const = 0;

protected:
  AnalysisResultLibraryExport() { }
};

//----------------------------------------------------------------------------------------------------------------------
/// AnalysisLibraryExport
//----------------------------------------------------------------------------------------------------------------------
class AnalysisLibraryExport : public AssetExport
{
public:
  virtual ~AnalysisLibraryExport() { }

  virtual const char* getName() const = 0;
  virtual const char* getDestFilename() const = 0;
  virtual const char* getResultFilename() const = 0;

  virtual AnalysisResultLibraryExport* createResultLibrary() = 0;
  virtual void destroyResultLibrary(AnalysisResultLibraryExport* library) = 0;

  virtual AnalysisTaskExport* createAnalysisTask(const char* name, const char* analysisType) = 0;

  virtual unsigned int getNumAnalysisTasks() const = 0;
  virtual AnalysisTaskExport* getAnalysisTask(unsigned int index) = 0;
  virtual const AnalysisTaskExport* getAnalysisTask(unsigned int index) const = 0;

protected:
  AnalysisLibraryExport() { }
};

//----------------------------------------------------------------------------------------------------------------------
/// Condition
//----------------------------------------------------------------------------------------------------------------------
class ConditionExport
{
public:
  virtual ~ConditionExport() { }

  virtual const char* getName() const = 0;

  virtual DataBlockExport* getDataBlock() = 0;
  virtual const DataBlockExport* getDataBlock() const = 0;

  virtual unsigned int getIndex() const = 0;
  virtual unsigned int getTypeID() const = 0;

protected:
  ConditionExport() { }
};

//----------------------------------------------------------------------------------------------------------------------
/// ConditionSet
//----------------------------------------------------------------------------------------------------------------------
class ConditionSetExport
{
public:
  virtual ~ConditionSetExport() { }

  virtual unsigned int getIndex() const = 0;
  virtual unsigned int getTargetNodeID() const = 0;
  virtual std::vector<unsigned int> getConditionIndices() const = 0;

protected:
  ConditionSetExport() { }
};

//----------------------------------------------------------------------------------------------------------------------
/// Exported Node
//----------------------------------------------------------------------------------------------------------------------
class NodeExport
{
public:
  virtual ~NodeExport() { }

  virtual ConditionExport* createCondition(
    unsigned int index,
    unsigned int typeID) = 0;

  virtual AnalysisNodeExport* createAnalysisNode(
    const char* name, 
    const char * typeName) = 0;

  virtual ConditionSetExport* createConditionSet(
    unsigned int index,
    unsigned int targetNodeID,
    const std::vector<unsigned int>& indices) = 0;

  virtual ConditionExport* createCommonCondition(
    unsigned int index,
    unsigned int typeID) = 0;
  virtual ConditionSetExport* createCommonConditionSet(
    unsigned int index,
    unsigned int targetNodeID,
    const std::vector<unsigned int>& indices) = 0;

  virtual unsigned int getIndex() const = 0;
  virtual unsigned int getNodeID() const = 0;
  virtual unsigned int getDownstreamParentID() const = 0;
  virtual bool isDownstreamParentMultiplyConnected() const = 0;
  virtual unsigned int getTypeID() const = 0;
  virtual const char* getName() const = 0;
  virtual bool isPersistent() const = 0;

  virtual DataBlockExport* getDataBlock() = 0;
  virtual const DataBlockExport* getDataBlock() const = 0;

  virtual unsigned int getNumAnalysisNodes() const = 0;
  virtual AnalysisNodeExport* getAnalysisNode(unsigned int index) = 0;
  virtual const AnalysisNodeExport* getAnalysisNode(unsigned int index) const = 0;

  virtual unsigned int getNumConditions() const = 0;
  virtual ConditionExport* getCondition(unsigned int index) = 0;
  virtual const ConditionExport* getCondition(unsigned int index) const = 0;

  virtual unsigned int getNumConditionSets() const = 0;
  virtual ConditionSetExport* getConditionSet(unsigned int index) = 0;
  virtual const ConditionSetExport* getConditionSet(unsigned int index) const = 0;

  virtual unsigned int getNumCommonConditions() const = 0;
  virtual ConditionExport* getCommonCondition(unsigned int index) = 0;
  virtual const ConditionExport* getCommonCondition(unsigned int index) const = 0;

  virtual unsigned int getNumCommonConditionSets() const = 0;
  virtual ConditionSetExport* getCommonConditionSet(unsigned int index) = 0;
  virtual const ConditionSetExport* getCommonConditionSet(unsigned int index) const = 0;

protected:
  NodeExport() { }
};

//----------------------------------------------------------------------------------------------------------------------
/// Interface to store message information
//----------------------------------------------------------------------------------------------------------------------
class MessageExport
{
public:
  virtual ~MessageExport() { }

  virtual const char* getName() const = 0;
  virtual MR::MessageType getMessageType() const = 0;
  virtual MR::MessageID getMessageID() const = 0;
};

//----------------------------------------------------------------------------------------------------------------------
/// Interface to stored event information
//----------------------------------------------------------------------------------------------------------------------
class EventBaseExport
{
public:
  virtual ~EventBaseExport() { }

  virtual unsigned int getIndex() const = 0;
  virtual const AttributeBlockExport* getAttributeBlock() const = 0;
  virtual AttributeBlockExport* getAttributeBlock() = 0;

  /// default implementation requires an attribute called "userData"
  virtual unsigned int getUserData() const;
  virtual bool isAttributeDynamic(AttributeExport* attribute) const;
};

//----------------------------------------------------------------------------------------------------------------------
class DiscreteEventExport : public virtual EventBaseExport
{
public:
  /// default implementation requires an attribute called "startTime"
  virtual float getNormalisedTime() const;
  virtual bool isAttributeDynamic(AttributeExport* attribute) const;
};

//----------------------------------------------------------------------------------------------------------------------
class DurationEventExport : public virtual EventBaseExport
{
public:
  /// default implementation requires attributes called "startTime" and "duration" in the attribute block
  virtual float getNormalisedStartTime() const;
  virtual float getNormalisedDuration() const;
  virtual bool isAttributeDynamic(AttributeExport* attribute) const;
};

//----------------------------------------------------------------------------------------------------------------------
class CurveEventExport : public virtual EventBaseExport
{
public:
  /// default implementation requires attributes called "startTime" and "floatVal" in the attribute block
  virtual float getNormalisedStartTime() const;
  virtual float getFloatValue() const;
  virtual bool isAttributeDynamic(AttributeExport* attribute) const;
};

//----------------------------------------------------------------------------------------------------------------------
/// Common base class for all event track types.
//----------------------------------------------------------------------------------------------------------------------
class EventTrackExport : public AssetExport
{
public:
  virtual ~EventTrackExport() { }

  enum EventTrackType
  {
    EVENT_TRACK_TYPE_DISCRETE, // tick events
    EVENT_TRACK_TYPE_DURATION, // duration events
    EVENT_TRACK_TYPE_CURVE,
    EVENT_TRACK_TYPE_CUSTOM    // all other types that could be exported by connect are of this type.  Ignore in runtime
  };

  virtual EventTrackType getEventTrackType() const = 0;

  /// default implementation requires an attribute called "userData"
  virtual unsigned int getUserData() const;

  virtual GUID getGUID() const = 0;
  virtual const char* getName() const = 0;
  virtual const char* getDestFilename() const;

  virtual bool write();

  virtual unsigned int getNumEvents() const = 0;

  virtual unsigned int getEventTrackChannelID() const = 0;

  virtual const TakeExport* getTake() const = 0;

  virtual const AttributeBlockExport* getAttributeBlock() const = 0;
  virtual AttributeBlockExport* getAttributeBlock() = 0;

  virtual bool isAttributeDynamic(AttributeExport* attribute) const;
};

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsRigDef
//----------------------------------------------------------------------------------------------------------------------
class DeprecatedPhysicsRigDefExport : public AssetExport
{
public:
  virtual ~DeprecatedPhysicsRigDefExport() { };

  virtual const char* getName() const = 0;
  virtual GUID getGUID() const = 0;
  virtual const char* getDestFilename() const = 0;

  virtual const class ExportPhysicsRigDef* getExportPhysicsRigDef() const = 0;

  virtual bool write() = 0;

protected:
  DeprecatedPhysicsRigDefExport() { }
};

//----------------------------------------------------------------------------------------------------------------------
/// AnimationEntryExport
//----------------------------------------------------------------------------------------------------------------------
class DiscreteEventTrackExport : public EventTrackExport
{
public:
  virtual EventTrackType getEventTrackType() const { return EventTrackExport::EVENT_TRACK_TYPE_DISCRETE; }
  virtual const DiscreteEventExport* getEvent(unsigned int eventIdx) const = 0;
  virtual DiscreteEventExport* getEvent(unsigned int eventIdx) = 0;

  virtual DiscreteEventExport* createEvent(
    unsigned int index,
    float        normalisedTime,
    unsigned int userData) = 0;
};

//----------------------------------------------------------------------------------------------------------------------
class DurationEventTrackExport : public EventTrackExport
{
public:
  virtual EventTrackType getEventTrackType() const { return EventTrackExport::EVENT_TRACK_TYPE_DURATION; }
  virtual const DurationEventExport* getEvent(unsigned int eventIdx) const = 0;
  virtual DurationEventExport* getEvent(unsigned int eventIdx) = 0;

  virtual DurationEventExport* createEvent(
    unsigned int index,
    float        normalisedTime,
    float        duration,
    unsigned int userData) = 0;
};

//----------------------------------------------------------------------------------------------------------------------
class CurveEventTrackExport : public EventTrackExport
{
public:
  virtual EventTrackType getEventTrackType() const { return EventTrackExport::EVENT_TRACK_TYPE_CURVE; }
  virtual const CurveEventExport* getEvent(unsigned int eventIdx) const = 0;
  virtual CurveEventExport* getEvent(unsigned int eventIdx) = 0;

  virtual CurveEventExport* createEvent(
    unsigned int index,
    float        normalisedTime,
    float        value,
    unsigned int userData) = 0;
};

//----------------------------------------------------------------------------------------------------------------------
class CustomEventTrackExport : public EventTrackExport
{
public:
  virtual EventTrackType getEventTrackType() const { return EventTrackExport::EVENT_TRACK_TYPE_CUSTOM; }
  virtual const EventBaseExport* getEvent(unsigned int eventIdx) const = 0;
  virtual EventBaseExport* getEvent(unsigned int eventIdx) = 0;
};

//----------------------------------------------------------------------------------------------------------------------
///
//----------------------------------------------------------------------------------------------------------------------
class TakeExport : public AssetExport
{
public:
  virtual ~TakeExport() { }

  virtual EventTrackExport* createEventTrack(
    EventTrackExport::EventTrackType trackType,
    GUID           guid,
    const wchar_t* name,
    unsigned int   eventTrackChannelID,
    unsigned int   userData) = 0;

  virtual const AttributeBlockExport* getAttributeBlock() const = 0;
  virtual AttributeBlockExport* getAttributeBlock() = 0;
  virtual unsigned int getNumEventTracks() const = 0;
  virtual EventTrackExport* getEventTrack(unsigned int index) = 0;
  virtual const EventTrackExport* getEventTrack(unsigned int index) const = 0;
  virtual EventTrackExport* getEventTrack(GUID guid) = 0;
  virtual EventTrackExport* findEventTrack(const char* name);
  virtual const EventTrackExport* getEventTrack(GUID guid) const = 0;
  virtual float getCachedTakeSecondsDuration() const = 0;
  virtual float getCachedTakeFPS() const = 0;
  virtual bool getLoop() const = 0;
  virtual float getClipStart() const = 0;
  virtual float getClipEnd() const = 0;
  virtual bool write() = 0;

protected:
  TakeExport() { }
};

//----------------------------------------------------------------------------------------------------------------------
/// TakeList
//----------------------------------------------------------------------------------------------------------------------
class TakeList : public AssetExport
{
public:
  virtual ~TakeList() { }

  virtual TakeExport* createTake(
    const wchar_t* name, float secDuration, float fps,  bool loop, float clipStart = 0.0, float clipEnd = 1.0) = 0;

  virtual unsigned int getNumTakes() const = 0;
  virtual TakeExport* getTake(unsigned int index) = 0;
  virtual const TakeExport* getTake(unsigned int index) const = 0;
  virtual TakeExport* getTake(const char* name);
  virtual const TakeExport* getTake(const char* name) const;

  /// short file name of the related animation file that this take data is for
  virtual const char* getSourceAnimFilename() const = 0;
  virtual unsigned int getFormatVersion() const = 0;
};

//----------------------------------------------------------------------------------------------------------------------
/// Joint
//----------------------------------------------------------------------------------------------------------------------
class JointExport
{
public:
  static const unsigned int WORLD_JOINT_INDEX = 0xffffffff;

  virtual ~JointExport() { }

  virtual void setName(const char* name) = 0;
  virtual void setRotation(float x, float y, float z, float w) = 0;
  virtual void setTranslation(float x, float y, float z) = 0;
  virtual void setVisiblity(bool visible) = 0;
  virtual void setBodyPlanTag(const char* tag) = 0;
  virtual void setRetargetingTag(const char* tag) = 0;

  virtual const char* getName() const = 0;
  virtual unsigned int getIndex() const = 0;
  virtual unsigned int getParentIndex() const = 0;
  virtual void getRotation(float& x, float& y, float& z, float& w) const = 0;
  virtual void getTranslation(float& x, float& y, float& z) const = 0;
  virtual bool getVisiblity() const = 0;
  virtual const char* getBodyPlanTag() const = 0;
  virtual const char* getRetargetingTag() const = 0;

protected:
  JointExport() { }
};

//----------------------------------------------------------------------------------------------------------------------
/// JointLimit
//----------------------------------------------------------------------------------------------------------------------
class JointLimitExport
{
public:
  enum LimitType
  {
    kHingeLimitType,
    kBallSocketLimitType
  };

  virtual ~JointLimitExport() { }

  virtual void setLimitType(LimitType type) = 0;

  virtual void setJointIndex(unsigned int index) = 0;
  virtual void setOrientation(float x, float y, float z, float w) = 0;
  virtual void setOffsetOrientation(float x, float y, float z, float w) = 0;

  virtual void setTwistLow(float twist) const = 0;
  virtual void setTwistHigh(float twist) const = 0;
  virtual void setSwing1(float swing) const = 0;
  virtual void setSwing2(float swing) const = 0;

  virtual unsigned int getIndex() const = 0;

  virtual LimitType getLimitType() const = 0;

  virtual unsigned int getJointIndex() const = 0;
  virtual void getOrientation(float& x, float& y, float& z, float& w) const = 0;
  virtual void getOffsetOrientation(float& x, float& y, float& z, float& w) const = 0;

  virtual float getTwistLow() const = 0;
  virtual float getTwistHigh() const = 0;
  virtual float getSwing1() const = 0;
  virtual float getSwing2() const = 0;

protected:
  JointLimitExport() { }
};

//----------------------------------------------------------------------------------------------------------------------
/// Rig
//----------------------------------------------------------------------------------------------------------------------
class RigExport : public AssetExport
{
public:
  virtual ~RigExport() { };

  virtual JointExport* createJoint(
    unsigned int   index,
    const wchar_t* jointName,
    unsigned int   parentIndex) = 0;

  virtual JointLimitExport* createJointLimit(unsigned int index) = 0;

  virtual void createMirrorMapping(
    unsigned int index,
    unsigned int firstJointIndex,
    unsigned int secondJointIndex) = 0;
  virtual void createJointMapping(
    unsigned int index,
    unsigned int firstJointIndex,
    unsigned int secondJointIndex) = 0;

  virtual void setJointOffsetTransform(
    unsigned int        index,
    const NMP::Vector3& translation,
    const NMP::Quat&    orientation) = 0;

  virtual void setJointPostOffsetTransform(
    unsigned int        index,
    const NMP::Quat&    postOffsetOrient) = 0;

  virtual void setGUID(const GUID guid) = 0;
  virtual void setDestFilename(const char* filename) = 0;

  virtual void setHipIndex(unsigned int hipIndex) = 0;
  virtual void setTrajectoryIndex(unsigned int trajectoryIndex) = 0;
  virtual void setBlendFrameOrientationQuat(float x, float y, float z, float w) = 0;
  virtual void setBlendFramePositionVec(float x, float y, float z) = 0;
  virtual void setRigSourceFileName(const char* sourceFileName) = 0;

  virtual unsigned int getHipIndex() const = 0;
  virtual unsigned int getTrajectoryIndex() const = 0;
  virtual void getBlendFrameOrientationQuat(float& x, float& y, float& z, float& w) const = 0;
  virtual void getBlendFramePositionVec(float& x, float& y, float& z) const = 0;
  virtual const char* getRigSourceFileName() const = 0;

  virtual unsigned int getNumJoints() const = 0;
  virtual JointExport* getJoint(unsigned int index) = 0;
  virtual const JointExport* getJoint(unsigned int index) const = 0;

  virtual unsigned int getNumJointLimits() const = 0;
  virtual JointLimitExport* getJointLimit(unsigned int index) = 0;
  virtual const JointLimitExport* getJointLimit(unsigned int index) const = 0;

  virtual unsigned int getMirrorMappingCount() const = 0;
  virtual void getMirrorMapping(
    unsigned int  mappingIndex,
    unsigned int& firstJointIndex,
    unsigned int& secondJointIndex) const = 0;

  virtual float getRigRetargetScale() const = 0;
  virtual void setRigRetargetScale(float scale) = 0;

  virtual unsigned int getJointMappingCount() const = 0;
  virtual void getJointMapping(
    unsigned int  mappingIndex,
    unsigned int& firstJointIndex,
    unsigned int& secondJointIndex) const = 0;

  virtual void getJointOffsetTransform(
    unsigned int index,
    NMP::Vector3& translation,
    NMP::Quat& orientation) const = 0;

  virtual void getJointPostOffsetTransform(
    unsigned int index,
    NMP::Quat& postOffsetOrient) const = 0;

  virtual bool write() = 0;

  /// \brief Returns the scale of the rig relative to connect internal units.
  virtual float getRigScaleFactor() const = 0;

  /// \brief Sets the scale of the rig relative to connect internal units.
  virtual void setRigScaleFactor(float scale) = 0;

  /// \brief  returns the mirror plane in which to mirror the character
  /// \return 0=YZ plane, 1=ZX plane, 2=XY plane
  virtual uint32_t getMirrorPlane() const = 0;

  /// \brief  sets the mirror plane in which to mirror the character
  /// \param  plane - 0=YZ plane, 1=ZX plane, 2=XY plane
  virtual void setMirrorPlane(uint32_t plane) = 0;

protected:
  RigExport() { }
};

//----------------------------------------------------------------------------------------------------------------------
/// CharacterControllerExport
//----------------------------------------------------------------------------------------------------------------------
class CharacterControllerExport : public AssetExport
{
public:
  virtual void setDestFilename(const char* filename) = 0;
  virtual const AttributeBlockExport*  getAttributeBlock() const = 0;
  virtual AttributeBlockExport*  getAttributeBlock() = 0;

protected:
  CharacterControllerExport() { }
};

//----------------------------------------------------------------------------------------------------------------------
/// InteractionProxy
//----------------------------------------------------------------------------------------------------------------------
class InteractionProxyExport : public AssetExport
{
public:
  virtual void setDestFilename(const char* filename) = 0;

  virtual void setParentIndex(unsigned int) = 0;
  virtual void setRotation(float x, float y, float z, float w) = 0;
  virtual void setTranslation(float x, float y, float z) = 0;
  virtual void setVisibility(bool visible) = 0;
  virtual void setUserData(unsigned int) = 0;
  virtual void setShape(const char* name) = 0; // Capsule, Box or Sphere
  virtual void setHeight(float x) = 0;
  virtual void setRadius(float x) = 0;
  virtual void setWidth(float x) = 0;
  virtual void setDepth(float x) = 0;

  virtual unsigned int getParentIndex() const = 0;
  virtual void getRotation(float& x, float& y, float& z, float& w) const = 0;
  virtual void getTranslation(float& x, float& y, float& z) const = 0;
  virtual bool getVisibility() const = 0;
  virtual unsigned int getUserData() const = 0;
  virtual const char* getShape() const = 0; // Capsule, Box or Sphere
  virtual float getHeight() const = 0;
  virtual float getRadius() const = 0;
  virtual float getWidth() const = 0;
  virtual float getDepth() const = 0;

protected:
  InteractionProxyExport() { }
};

//----------------------------------------------------------------------------------------------------------------------
/// AnimationPoseDefExport
//----------------------------------------------------------------------------------------------------------------------
class AnimationPoseDefExport
{
public:
  virtual void setName(const char* name) = 0;
  virtual void setAnimationFile(const wchar_t* file) = 0;
  virtual void setAnimationTake(const wchar_t* take) = 0;
  virtual void setPoseFrameIndex(int32_t frame) = 0;

  virtual const char* getName() const = 0;
  virtual const char* getAnimationFile() const = 0;
  virtual const char* getAnimationTake() const = 0;
  virtual int32_t     getPoseFrameIndex() const = 0;

protected:
  AnimationPoseDefExport() { }
};


//----------------------------------------------------------------------------------------------------------------------
/// AnimationEntryExport
//----------------------------------------------------------------------------------------------------------------------
class AnimationEntryExport
{
public:
  virtual ~AnimationEntryExport() { }

  /// get functions
  virtual unsigned int getIndex() const = 0;
  virtual const char* getAnimationFilename() const = 0;
  virtual const char* getTakeFilename() const = 0;
  virtual const char* getTakeName() const = 0;
  virtual const char* getSyncTrack() const = 0;
  virtual const char* getOptions() const = 0;
  virtual const char* getFormat() const = 0;
  virtual TakeExport* getTakeExportData(const char* rootDir) = 0;

protected:
  AnimationEntryExport() { }
};

//----------------------------------------------------------------------------------------------------------------------
/// AnimationSet
//----------------------------------------------------------------------------------------------------------------------
class AnimationSetExport
{
public:
  virtual ~AnimationSetExport() { }

  virtual AnimationEntryExport* createAnimationEntry(
    unsigned int   index,
    const wchar_t* animFile,
    const wchar_t* takeFile,
    const wchar_t* take,
    const wchar_t* syncTrack,
    const char*    format,
    const char*    options) = 0;

  virtual void createEventUserdataMirrorMapping(
    unsigned int index,
    int          firstUserdata,
    int          secondUserdata) = 0;

  virtual void createEventTrackMirrorMapping(
    unsigned int index,
    int          firstTrack,
    int          secondTrack) = 0;

  virtual void setRig(const RigExport* rig) = 0;
  virtual void setPhysicsRigDef(const PhysicsRigDefExport* physicsRigDef) = 0;
  virtual void setBodyDef(const BodyDefExport* bodyDef) = 0;
  virtual void setCharacterController(const CharacterControllerExport* characterController) = 0;
  virtual void setInteractionProxy(const InteractionProxyExport* proxy) = 0;
  virtual unsigned int getNumAnimationEntries() const = 0;
  virtual AnimationEntryExport* getAnimationEntry(unsigned int index) = 0;
  virtual const AnimationEntryExport* getAnimationEntry(unsigned int index) const = 0;

  virtual unsigned int getNumEventUserDataMirrorMappings() const = 0;
  virtual void getEventUserDataMirrorMapping(unsigned int index, int& firstUserData, int& secondUserData) const = 0;

  virtual unsigned int getNumEventTrackMirrorMappings() const = 0;
  virtual void getEventTrackMirrorMapping(unsigned int index, int& firstTrack, int& secondTrack) const = 0;

  virtual RigExport* getRig() = 0;
  virtual const RigExport* getRig() const = 0;

  virtual PhysicsRigDefExport* getPhysicsRigDef() = 0;
  virtual const PhysicsRigDefExport* getPhysicsRigDef() const = 0;

  virtual BodyDefExport* getBodyDef() = 0;
  virtual const BodyDefExport* getBodyDef() const = 0;

  virtual CharacterControllerExport* getCharacterController() = 0;
  virtual const CharacterControllerExport* getCharacterController() const = 0;

  virtual InteractionProxyExport* getInteractionProxy() = 0;
  virtual const InteractionProxyExport* getInteractionProxy() const = 0;

  virtual const char* getName() const = 0;
  virtual unsigned int getIndex() const = 0;

protected:
  AnimationSetExport() { }
};

//----------------------------------------------------------------------------------------------------------------------
/// AnimationLibrary
//----------------------------------------------------------------------------------------------------------------------
class AnimationLibraryExport : public AssetExport
{
public:
  virtual ~AnimationLibraryExport() { }

  virtual AnimationSetExport* createAnimationSet(unsigned int index, const wchar_t* setName) = 0;
  virtual unsigned int getNumAnimationSets() const = 0;
  virtual AnimationSetExport* getAnimationSet(unsigned int index) = 0;
  virtual const AnimationSetExport* getAnimationSet(unsigned int index) const = 0;
  virtual bool write() = 0;

protected:
  AnimationLibraryExport() {}
};

//----------------------------------------------------------------------------------------------------------------------
/// Exported NetworkDef
//----------------------------------------------------------------------------------------------------------------------
class NetworkDefExport : public AssetExport
{
public:

  virtual ~NetworkDefExport() { }

  virtual void setRootNodeNetworkID(unsigned int rootNodeNetworkID) = 0;

  virtual void setNetworkWorldOrientation(
    const NMP::Vector3& forwardVector,
    const NMP::Vector3& rightVector,
    const NMP::Vector3& upVector) = 0;

  virtual NodeExport* createNode(
    unsigned int   networkID,
    unsigned int   typeID,
    unsigned int   downstreamParentID,
    bool           downstreamParentMultiplyConnected,
    const wchar_t* nodeName,
    bool persistent = false) = 0;

  virtual NodeExport* createNode(
    unsigned int networkID,
    unsigned int typeID,
    unsigned int downstreamParentID,
    bool         downstreamParentMultiplyConnected,
    const char*  nodeName,
    bool persistent = false) = 0;

  virtual MessageExport* createMessage(
    const wchar_t* messageName,
    MR::MessageType messageType,
    MR::MessageID messageID) = 0;

  virtual void setAnimationLibrary(const AnimationLibraryExport* animationLibrary) = 0;
  virtual AnimationLibraryExport* getAnimationLibrary() = 0;
  virtual const AnimationLibraryExport* getAnimationLibrary() const = 0;

  virtual void setMessagePresetLibrary(const MessagePresetLibraryExport* presetLibrary) = 0;
  virtual MessagePresetLibraryExport* getMessagePresetLibrary() = 0;
  virtual const MessagePresetLibraryExport* getMessagePresetLibrary() const = 0;

  virtual unsigned int getRootNodeID() const = 0;

  virtual void getNetworkWorldOrientation(
    NMP::Vector3& forwardVector,
    NMP::Vector3& rightVector,
    NMP::Vector3& upVector) = 0;

  virtual unsigned int getNumNodes() const = 0;
  virtual NodeExport* getNode(unsigned int networkID) = 0;
  virtual const NodeExport* getNode(unsigned int networkID) const = 0;

  virtual NodeExport* findNode(const char* nodeName);
  virtual const NodeExport* findNode(const char* nodeName) const;

  virtual unsigned int getNumMessages() const = 0;
  virtual MessageExport* getMessage(unsigned int index) = 0;
  virtual const MessageExport* getMessage(unsigned int index) const = 0;

  virtual bool write() = 0;

protected:
  NetworkDefExport() { }
};

//----------------------------------------------------------------------------------------------------------------------
/// ExportFactory
//----------------------------------------------------------------------------------------------------------------------
class ExportFactory
{
public:
  virtual ~ExportFactory() { }

  virtual NetworkDefExport* createNetworkDef(
    GUID           revisionGUID,
    const wchar_t* networkName,
    const wchar_t* destFilename) = 0;
  virtual void destroyNetworkDef(NetworkDefExport* expObject) = 0;

  virtual TakeExport* createTake(
    const wchar_t* listName,
    float          secDuration,
    float          fps,
    const wchar_t* destFilename) = 0;

  virtual void destroyTake(TakeExport* expObject) = 0;

  virtual TakeList* createTakeList(
    const wchar_t* sourceAnimFile,
    const wchar_t* destFilename) = 0;
  virtual void destroyTakeList(TakeList* takeList) = 0;

  virtual AnimationLibraryExport* createAnimationLibrary(
    GUID           guid,
    const wchar_t* animLibraryName,
    const wchar_t* destFilename) = 0;
  virtual void destroyAnimationLibrary(AnimationLibraryExport* expObject) = 0;

  virtual RigExport* createRig(GUID guid, const wchar_t* destFilename, const char* rigName) = 0;
  virtual void destroyRig(RigExport* rig) = 0;

  virtual PhysicsRigDefExport* createPhysicsRigDef(GUID guid, const wchar_t* destFilename, const char* rigName) = 0;
  virtual void destroyPhysicsRigDef(PhysicsRigDefExport* physicsRigDef) = 0;

  virtual BodyDefExport* createBodyDef(GUID guid, const wchar_t* destFilename, const char* bodyName) = 0;
  virtual void destroyBodyDef(BodyDefExport* bodyDef) = 0;

  virtual CharacterControllerExport* createCharacterController(GUID guid, const wchar_t* destFilename, const char* controllerName) = 0;
  virtual void destroyCharacterController(CharacterControllerExport* characterController) = 0;

  virtual InteractionProxyExport* createInteractionProxy(GUID guid, const wchar_t* destFilename, const char* proxyName) = 0;
  virtual void destroyInteractionProxy(InteractionProxyExport* interactionProxy) = 0;

  virtual MessagePresetLibraryExport* createMessagePresetLibrary(GUID guid, const wchar_t* destFileName) = 0;
  virtual void destroyMessagePresetLibrary(MessagePresetLibraryExport* presetLibrary) = 0;

  virtual AnalysisLibraryExport* createAnalysisLibrary(GUID guid, const wchar_t* destFilename, const wchar_t* resultFilename) = 0;
  virtual void destroyAnalysisLibrary(AnalysisLibraryExport* analysisLibrary) = 0;

  virtual AnalysisResultLibraryExport* createAnalysisResultLibrary(GUID guid, const wchar_t* destFilename) = 0;
  virtual void destroyAnalysisResultLibrary(AnalysisResultLibraryExport* analysisResultLibrary) = 0;

  /// Loads a AssetExport and all files it depends upon.
  virtual AssetExport* loadAsset(const char* srcFilename) = 0;
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE std::string toChar(const std::wstring& str)
{
  std::string retVal;
  retVal.resize(str.length());
  for (size_t i = 0; i < str.length(); ++i)
  {
    NMP_ASSERT_MSG(str[i] < 0x80, "Invalid character - wide character not supported!");
    retVal[i] = static_cast<char>(str[i]);
  }
  return retVal;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace ME

#endif // MCEXPORT_H
