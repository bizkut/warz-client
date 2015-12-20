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
#include "export/apExportLUA.h"

//----------------------------------------------------------------------------------------------------------------------
/// LUA implementation of processed data export classes
//----------------------------------------------------------------------------------------------------------------------
namespace ME
{

//----------------------------------------------------------------------------------------------------------------------
// ProcessedDataStreamLUA
//----------------------------------------------------------------------------------------------------------------------
ProcessedDataStreamLUA::ProcessedDataStreamLUA() : m_filePointer(0), m_indentLevel(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
ProcessedDataStreamLUA::~ProcessedDataStreamLUA()
{
  if (m_filePointer)
    close();
}

//----------------------------------------------------------------------------------------------------------------------
bool ProcessedDataStreamLUA::open(const char* fileName)
{
  NMP_VERIFY(!m_filePointer);
  m_filePointer = fopen(fileName, "w");
  return m_filePointer != 0;
}

//----------------------------------------------------------------------------------------------------------------------
void ProcessedDataStreamLUA::writeLine(const char* buffer)
{
  NMP_VERIFY(m_filePointer);
  
  // Get the whitespace for the indent
  char indentText[256];
  const unsigned int numSpacesPerLevel = 2;
  unsigned int count = m_indentLevel * numSpacesPerLevel;
  NMP_VERIFY(count < 256);
  for (unsigned int i = 0; i < count; ++i)
    indentText[i] = ' ';
  indentText[count] = 0;
  
  // Output the line
  fprintf(m_filePointer, "%s%s\n", indentText, buffer);
}

//----------------------------------------------------------------------------------------------------------------------
void ProcessedDataStreamLUA::writeLine()
{
  NMP_VERIFY(m_filePointer);
  fprintf(m_filePointer, "\n");
}

//----------------------------------------------------------------------------------------------------------------------
void ProcessedDataStreamLUA::close()
{
  if (m_filePointer)
  {
    fclose(m_filePointer);
    m_filePointer = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ProcessedDataStreamLUA::incIndentLevel()
{
  m_indentLevel++;
}

//----------------------------------------------------------------------------------------------------------------------
void ProcessedDataStreamLUA::decIndentLevel()
{
  NMP_VERIFY(m_indentLevel > 0)
  m_indentLevel--;
}

//----------------------------------------------------------------------------------------------------------------------
// ProcessedDataBlockExportLUA::Element
//----------------------------------------------------------------------------------------------------------------------
ProcessedDataBlockExportLUA::Element::Element(
  const void*  data,
  size_t       length,
  const char*  type,
  const char*  desc)
{
  size_t numBytes = 0;

  //---------------------------
  if (strcmp(type, "bool") == 0)
  {
    numBytes = sizeof(bool);
  }
  else if (strcmp(type, "int") == 0)
  {
    numBytes = sizeof(int);
  }
  else if (strcmp(type, "uint") == 0)
  {
    numBytes = sizeof(unsigned int);
  }
  else if (strcmp(type, "float") == 0)
  {
    numBytes = sizeof(float);
  }
  //---------------------------
  else if (strcmp(type, "boolArray") == 0)
  {
    numBytes = sizeof(bool) * length;
  }
  else if (strcmp(type, "intArray") == 0)
  {
    numBytes = sizeof(int) * length;
  }
  else if (strcmp(type, "uintArray") == 0)
  {
    numBytes = sizeof(unsigned int) * length;
  }
  else if (strcmp(type, "floatArray") == 0)
  {
    numBytes = sizeof(float) * length;
  }
  //---------------------------
  else
  {
    NMP_ASSERT_FAIL_MSG("Unknown element type: %s", type);    
  }
  
  // Copy the data buffer
  if (numBytes > 0)
  {
    m_data = (void*) new char[numBytes];
    memcpy(m_data, data, numBytes);
  }
  else
  {
    m_data = NULL;
  }
  
  m_length = length;
  
  // Description
  m_type = type;
  m_desc = desc;
}

//----------------------------------------------------------------------------------------------------------------------
ProcessedDataBlockExportLUA::Element::~Element()
{
  if (m_data)
  {
    char* buffer = (char*)m_data;
    delete[] buffer;
  }
}

//----------------------------------------------------------------------------------------------------------------------
const char* ProcessedDataBlockExportLUA::Element::getType() const
{
  return m_type.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
const char* ProcessedDataBlockExportLUA::Element::getDesc() const
{
  return m_desc.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
size_t ProcessedDataBlockExportLUA::Element::getLength() const
{
  return m_length;
}

//----------------------------------------------------------------------------------------------------------------------
void ProcessedDataBlockExportLUA::Element::writeData(ProcessedDataStreamLUA& stream) const
{
  const char* type = getType();
  const char* desc = getDesc();

  //---------------------------
  if (strcmp(type, "bool") == 0)
  {
    writeBool(stream, *(const bool*)m_data, desc);
  }
  else if (strcmp(type, "int") == 0)
  {
    writeInt(stream, *(const int*)m_data, desc);
  }
  else if (strcmp(type, "uint") == 0)
  {
    writeUInt(stream, *(const unsigned int*)m_data, desc);
  }
  else if (strcmp(type, "float") == 0)
  {
    writeFloat(stream, *(const float*)m_data, desc);
  }
  //---------------------------
  else if (strcmp(type, "boolArray") == 0)
  {
    writeBoolArray(stream, (const bool*)m_data, m_length, desc);
  }
  else if (strcmp(type, "intArray") == 0)
  {
    writeIntArray(stream, (const int*)m_data, m_length, desc);
  }
  else if (strcmp(type, "uintArray") == 0)
  {
    writeUIntArray(stream, (const unsigned int*)m_data, m_length, desc);
  }
  else if (strcmp(type, "floatArray") == 0)
  {
    writeFloatArray(stream, (const float*)m_data, m_length, desc);
  }
  //---------------------------
  else
  {
    NMP_ASSERT_FAIL_MSG("Unknown element type: %s", type);    
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ProcessedDataBlockExportLUA::Element::writeBool(ProcessedDataStreamLUA& stream, bool data, const char* desc) const
{
  static const char* boolDesc[2] = {"false,", "true,"};
  std::string buffer = std::string(desc) + std::string(" = ") + std::string(boolDesc[(int) data]);
  stream.writeLine(buffer.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
void ProcessedDataBlockExportLUA::Element::writeBoolArray(ProcessedDataStreamLUA& stream, const bool* data, size_t length, const char* desc) const
{
  static const char* boolDesc[2] = {"false,", "true,"};
  
  std::string buffer = std::string(desc) + std::string(" =");
  stream.writeLine(buffer.c_str());
  stream.writeLine("{");
  stream.incIndentLevel();
  for (size_t i = 0; i < length; ++i)
  {
    stream.writeLine(boolDesc[(int)data[i]]);
  }
  stream.decIndentLevel();
  stream.writeLine("},");
}

//----------------------------------------------------------------------------------------------------------------------
void ProcessedDataBlockExportLUA::Element::writeInt(ProcessedDataStreamLUA& stream, int data, const char* desc) const
{
  char valueBuffer[256];
  NMP_SPRINTF(valueBuffer, 256, "%d,", data);

  std::string buffer = std::string(desc) + std::string(" = ") + std::string(valueBuffer);
  stream.writeLine(buffer.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
void ProcessedDataBlockExportLUA::Element::writeIntArray(ProcessedDataStreamLUA& stream, const int* data, size_t length, const char* desc) const
{
  char valueBuffer[256];

  std::string buffer = std::string(desc) + std::string(" =");
  stream.writeLine(buffer.c_str());
  stream.writeLine("{");
  stream.incIndentLevel();
  for (size_t i = 0; i < length; ++i)
  {
    NMP_SPRINTF(valueBuffer, 256, "%d,", data[i]);
    stream.writeLine(valueBuffer);
  }
  stream.decIndentLevel();
  stream.writeLine("},");
}

//----------------------------------------------------------------------------------------------------------------------
void ProcessedDataBlockExportLUA::Element::writeUInt(ProcessedDataStreamLUA& stream, unsigned int data, const char* desc) const
{
  char valueBuffer[256];
  NMP_SPRINTF(valueBuffer, 256, "%u,", data);

  std::string buffer = std::string(desc) + std::string(" = ") + std::string(valueBuffer);
  stream.writeLine(buffer.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
void ProcessedDataBlockExportLUA::Element::writeUIntArray(ProcessedDataStreamLUA& stream, const unsigned int* data, size_t length, const char* desc) const
{
  char valueBuffer[256];

  std::string buffer = std::string(desc) + std::string(" =");
  stream.writeLine(buffer.c_str());
  stream.writeLine("{");
  stream.incIndentLevel();
  for (size_t i = 0; i < length; ++i)
  {
    NMP_SPRINTF(valueBuffer, 256, "%u,", data[i]);
    stream.writeLine(valueBuffer);
  }
  stream.decIndentLevel();
  stream.writeLine("},");
}

//----------------------------------------------------------------------------------------------------------------------
void ProcessedDataBlockExportLUA::Element::writeFloat(ProcessedDataStreamLUA& stream, float data, const char* desc) const
{
  char valueBuffer[256];
  NMP_SPRINTF(valueBuffer, 256, "%f,", data);

  std::string buffer = std::string(desc) + std::string(" = ") + std::string(valueBuffer);
  stream.writeLine(buffer.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
void ProcessedDataBlockExportLUA::Element::writeFloatArray(ProcessedDataStreamLUA& stream, const float* data, size_t length, const char* desc) const
{
  char valueBuffer[256];

  std::string buffer = std::string(desc) + std::string(" =");
  stream.writeLine(buffer.c_str());
  stream.writeLine("{");
  stream.incIndentLevel();
  for (size_t i = 0; i < length; ++i)
  {
    NMP_SPRINTF(valueBuffer, 256, "%f,", data[i]);
    stream.writeLine(valueBuffer);
  }
  stream.decIndentLevel();
  stream.writeLine("},");
}

//----------------------------------------------------------------------------------------------------------------------
// ProcessedDataBlockExportLUA
//----------------------------------------------------------------------------------------------------------------------
ProcessedDataBlockExportLUA::ProcessedDataBlockExportLUA()
{
}

//----------------------------------------------------------------------------------------------------------------------
ProcessedDataBlockExportLUA::~ProcessedDataBlockExportLUA()
{
  for (size_t i = 0; i < m_elements.size(); ++i)
  {
    delete m_elements[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ProcessedDataBlockExportLUA::writeData(ProcessedDataStreamLUA& stream) const
{
  for (size_t i = 0; i < m_elements.size(); ++i)
  {
    m_elements[i]->writeData(stream);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ProcessedDataBlockExportLUA::writeBool(bool data, const char* desc)
{
  Element* e = new Element(&data, 1, "bool", desc);
  m_elements.push_back(e);
}

//----------------------------------------------------------------------------------------------------------------------
void ProcessedDataBlockExportLUA::writeBoolArray(const bool* data, size_t length, const char* desc)
{
  Element* e = new Element(data, length, "boolArray", desc);
  m_elements.push_back(e);
}

//----------------------------------------------------------------------------------------------------------------------
void ProcessedDataBlockExportLUA::writeInt(int data, const char* desc)
{
  Element* e = new Element(&data, 1, "int", desc);
  m_elements.push_back(e);
}

//----------------------------------------------------------------------------------------------------------------------
void ProcessedDataBlockExportLUA::writeIntArray(const int* data, size_t length, const char* desc)
{
  Element* e = new Element(data, length, "intArray", desc);
  m_elements.push_back(e);
}

//----------------------------------------------------------------------------------------------------------------------
void ProcessedDataBlockExportLUA::writeUInt(unsigned int data, const char* desc)
{
  Element* e = new Element(&data, 1, "uint", desc);
  m_elements.push_back(e);
}

//----------------------------------------------------------------------------------------------------------------------
void ProcessedDataBlockExportLUA::writeUIntArray(const unsigned int* data, size_t length, const char* desc)
{
  Element* e = new Element(data, length, "uintArray", desc);
  m_elements.push_back(e);
}

//----------------------------------------------------------------------------------------------------------------------
void ProcessedDataBlockExportLUA::writeFloat(float data, const char* desc)
{
  Element* e = new Element(&data, 1, "float", desc);
  m_elements.push_back(e);
}

//----------------------------------------------------------------------------------------------------------------------
void ProcessedDataBlockExportLUA::writeFloatArray(const float* data, size_t length, const char* desc)
{
  Element* e = new Element(data, length, "floatArray", desc);
  m_elements.push_back(e);
}

//----------------------------------------------------------------------------------------------------------------------
// NodeDefProcessedDataExportLUA
//----------------------------------------------------------------------------------------------------------------------
NodeDefProcessedDataExportLUA::NodeDefProcessedDataExportLUA(const char* name)
{
  NMP_VERIFY(name);
  m_name = name;
  m_dataBlock = new ProcessedDataBlockExportLUA;
}

//----------------------------------------------------------------------------------------------------------------------
NodeDefProcessedDataExportLUA::~NodeDefProcessedDataExportLUA()
{
  delete m_dataBlock;
}

//----------------------------------------------------------------------------------------------------------------------
const char* NodeDefProcessedDataExportLUA::getName() const
{
  return m_name.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
ProcessedDataBlockExport* NodeDefProcessedDataExportLUA::getProcessedDataBlock()
{
  return m_dataBlock;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeDefProcessedDataExportLUA::writeData(ProcessedDataStreamLUA& stream) const
{
  // Pre-amble
  stream.writeLine("{");
  stream.incIndentLevel();

  // Name
  std::string buffer = std::string("name = \"") + std::string(getName()) + std::string("\",");
  stream.writeLine(buffer.c_str());
  
  // Attributes
  stream.writeLine("attributes =");
  stream.writeLine("{");
  stream.incIndentLevel();
  
  // Write the data block
  ProcessedDataBlockExportLUA* dataBlock = (ProcessedDataBlockExportLUA*)m_dataBlock;
  NMP_VERIFY(dataBlock);
  dataBlock->writeData(stream);
  
  stream.decIndentLevel();
  stream.writeLine("},");

  // Post-amble
  stream.decIndentLevel();
  stream.writeLine("},");
}

//----------------------------------------------------------------------------------------------------------------------
// NetworkDefProcessedDataExportLUA
//----------------------------------------------------------------------------------------------------------------------
NetworkDefProcessedDataExportLUA::NetworkDefProcessedDataExportLUA(
  const char* networkName,
  const char* destFilename)
{
  m_name = networkName;
  m_destFilename = destFilename;
}

//----------------------------------------------------------------------------------------------------------------------
NetworkDefProcessedDataExportLUA::~NetworkDefProcessedDataExportLUA()
{
  for (size_t i = 0 ; i < m_nodes.size() ; ++i)
  {
    delete m_nodes[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
NodeDefProcessedDataExport* NetworkDefProcessedDataExportLUA::createNodeDefProcessedData(
  const char* nodeName)
{
  NodeDefProcessedDataExport* result = new NodeDefProcessedDataExportLUA(nodeName);
  m_nodes.push_back(result);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const char* NetworkDefProcessedDataExportLUA::getName() const
{
  return m_name.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
const char* NetworkDefProcessedDataExportLUA::getDestFilename() const
{
  return m_destFilename.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
bool NetworkDefProcessedDataExportLUA::write()
{
  ProcessedDataStreamLUA stream;
  if (!stream.open(getDestFilename()))
    return false;

  // Pre-amble
  stream.writeLine("local dataBlock =");
  stream.writeLine("{");
  stream.incIndentLevel();
  
  // Network
  std::string buffer = std::string("name = \"") + std::string(getName()) + std::string("\",");
  stream.writeLine(buffer.c_str());
  
  stream.writeLine("nodeDefs =");
  stream.writeLine("{");
  stream.incIndentLevel();
  
  // Nodes
  for (size_t i = 0; i < m_nodes.size(); ++i)
  {
    NodeDefProcessedDataExportLUA* processedNodeExport = (NodeDefProcessedDataExportLUA*)m_nodes[i];
    NMP_VERIFY(processedNodeExport);
    processedNodeExport->writeData(stream);
  }
 
  stream.decIndentLevel();
  stream.writeLine("},");
  
  // Post-amble
  stream.decIndentLevel();
  stream.writeLine("}");
  stream.writeLine();
  stream.writeLine("return dataBlock");

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
/// ProcessedDataExportFactoryLUA
//----------------------------------------------------------------------------------------------------------------------
ProcessedDataExportFactoryLUA::~ProcessedDataExportFactoryLUA()
{
}

//----------------------------------------------------------------------------------------------------------------------
NetworkDefProcessedDataExport* ProcessedDataExportFactoryLUA::createNetworkDefProcessedData(
  const char* networkName,
  const char* destFilename)
{
  NetworkDefProcessedDataExportLUA* netDefProcessed = new NetworkDefProcessedDataExportLUA(networkName, destFilename);
  return netDefProcessed;
}

//----------------------------------------------------------------------------------------------------------------------
void ProcessedDataExportFactoryLUA::destroyNetworkDefProcessedData(NetworkDefProcessedDataExport* netDefProcessed)
{
  delete netDefProcessed;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace ME
