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
#ifdef _MSC_VER
#pragma once
#endif
#ifndef APEXPORT_LUA_H
#define APEXPORT_LUA_H
//----------------------------------------------------------------------------------------------------------------------
#include "apExport.h"

//----------------------------------------------------------------------------------------------------------------------
/// LUA implementation of processed data export classes
//----------------------------------------------------------------------------------------------------------------------
namespace ME
{

//----------------------------------------------------------------------------------------------------------------------
// ProcessedDataStreamLUA
//----------------------------------------------------------------------------------------------------------------------
class ProcessedDataStreamLUA
{
public:
  ProcessedDataStreamLUA();
  ~ProcessedDataStreamLUA();

  bool open(const char* fileName);
  void writeLine(const char* buffer);
  void writeLine();
  void close();
  
  void incIndentLevel();
  void decIndentLevel();

protected:
  FILE*           m_filePointer;
  unsigned int    m_indentLevel;
};

//----------------------------------------------------------------------------------------------------------------------
// ProcessedDataBlockExportLUA
//----------------------------------------------------------------------------------------------------------------------
class ProcessedDataBlockExportLUA : public ProcessedDataBlockExport
{
  friend class NodeDefProcessedDataExportLUA;
  
public:
  class Element
  {
  public:
    Element(
      const void*  data,
      size_t       length,
      const char*  type,
      const char*  desc);
      
    ~Element();

    const char* getType() const;
    const char* getDesc() const;
    size_t getLength() const;

    void writeData(ProcessedDataStreamLUA& stream) const;

  protected:
    void writeBool(ProcessedDataStreamLUA& stream, bool data, const char* desc) const;
    void writeBoolArray(ProcessedDataStreamLUA& stream, const bool* data, size_t length, const char* desc) const;
    void writeInt(ProcessedDataStreamLUA& stream, int data, const char* desc) const;
    void writeIntArray(ProcessedDataStreamLUA& stream, const int* data, size_t length, const char* desc) const;
    void writeUInt(ProcessedDataStreamLUA& stream, unsigned int data, const char* desc) const;
    void writeUIntArray(ProcessedDataStreamLUA& stream, const unsigned int* data, size_t length, const char* desc) const;
    void writeFloat(ProcessedDataStreamLUA& stream, float data, const char* desc) const;
    void writeFloatArray(ProcessedDataStreamLUA& stream, const float* data, size_t length, const char* desc) const;

  protected:
    std::string     m_type;
    std::string     m_desc;
    size_t          m_length;
    void*           m_data;
  };

public:
  ProcessedDataBlockExportLUA();
  ~ProcessedDataBlockExportLUA();
  
  void writeData(ProcessedDataStreamLUA& stream) const;

  void writeBool(bool data, const char* desc) NM_OVERRIDE;
  void writeBoolArray(const bool* data, size_t length, const char* desc) NM_OVERRIDE;
  void writeInt(int data, const char* desc) NM_OVERRIDE;
  void writeIntArray(const int* data, size_t length, const char* desc) NM_OVERRIDE;
  void writeUInt(unsigned int data, const char* desc) NM_OVERRIDE;
  void writeUIntArray(const unsigned int* data, size_t length, const char* desc) NM_OVERRIDE;
  void writeFloat(float data, const char* desc) NM_OVERRIDE;
  void writeFloatArray(const float* data, size_t length, const char* desc) NM_OVERRIDE;

protected:
  std::vector<Element*>   m_elements;
};

//----------------------------------------------------------------------------------------------------------------------
// NodeDefProcessedDataExportLUA
//----------------------------------------------------------------------------------------------------------------------
class NodeDefProcessedDataExportLUA : public NodeDefProcessedDataExport
{
  friend class NetworkDefProcessedDataExportLUA;

public:
  ~NodeDefProcessedDataExportLUA();
  
  const char* getName() const NM_OVERRIDE;
  
  ProcessedDataBlockExport* getProcessedDataBlock() NM_OVERRIDE;
  
  void writeData(ProcessedDataStreamLUA& stream) const;
  
protected:
  NodeDefProcessedDataExportLUA(const char* name);
  
  std::string                 m_name;
  ProcessedDataBlockExport*   m_dataBlock;
};

//----------------------------------------------------------------------------------------------------------------------
// NetworkDefProcessedDataExportLUA
//----------------------------------------------------------------------------------------------------------------------
class NetworkDefProcessedDataExportLUA : public NetworkDefProcessedDataExport
{
  friend class ProcessedDataExportFactoryLUA;

public:
  ~NetworkDefProcessedDataExportLUA();
  
  NodeDefProcessedDataExport* createNodeDefProcessedData(
    const char* nodeName) NM_OVERRIDE;
    
  const char* getName() const NM_OVERRIDE;
  const char* getDestFilename() const NM_OVERRIDE;
  bool write() NM_OVERRIDE;
  
protected:
  NetworkDefProcessedDataExportLUA(const char* networkName, const char* destFilename);
  
  std::string                                 m_name;
  std::string                                 m_destFilename;
  std::vector<NodeDefProcessedDataExport*>    m_nodes;
};

//----------------------------------------------------------------------------------------------------------------------
/// ProcessedDataExportFactoryLUA
//----------------------------------------------------------------------------------------------------------------------
class ProcessedDataExportFactoryLUA : public ProcessedDataExportFactory
{
public:
  ~ProcessedDataExportFactoryLUA();
  
  NetworkDefProcessedDataExport* createNetworkDefProcessedData(
    const char* networkName,
    const char* destFilename) NM_OVERRIDE;

  void destroyNetworkDefProcessedData(NetworkDefProcessedDataExport* expObject) NM_OVERRIDE;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace ME

#endif // APEXPORT_LUA_H
