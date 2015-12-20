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
#ifndef APEXPORT_H
#define APEXPORT_H
//----------------------------------------------------------------------------------------------------------------------
#include <vector>
#include <string>
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMatrix34.h"
#include "NMPlatform/NMVector3.h"

//----------------------------------------------------------------------------------------------------------------------
/// Classes that wrap the underlying representation of
/// exported assets.
///
/// These classes are used by the asset compiler to write out
/// exported data in the chosen format to provide feedback to
/// morpheme:connect about the processed assets
//----------------------------------------------------------------------------------------------------------------------
namespace ME
{

//----------------------------------------------------------------------------------------------------------------------
// ProcessedDataExport
//----------------------------------------------------------------------------------------------------------------------
class ProcessedDataExport
{
public:
  virtual ~ProcessedDataExport() { };
  
  virtual const char* getName() const = 0;
  virtual const char* getDestFilename() const = 0;
  virtual bool write() = 0;
};

//----------------------------------------------------------------------------------------------------------------------
// ProcessedDataBlockExport
//----------------------------------------------------------------------------------------------------------------------
class ProcessedDataBlockExport
{
public:
  virtual ~ProcessedDataBlockExport() { };
  
  virtual void writeBool(bool data, const char* desc) = 0;
  virtual void writeBoolArray(const bool* data, size_t length, const char* desc) = 0;
  virtual void writeInt(int data, const char* desc) = 0;
  virtual void writeIntArray(const int* data, size_t length, const char* desc) = 0;
  virtual void writeUInt(unsigned int data, const char* desc) = 0;
  virtual void writeUIntArray(const unsigned int* data, size_t length, const char* desc) = 0;
  virtual void writeFloat(float data, const char* desc) = 0;
  virtual void writeFloatArray(const float* data, size_t length, const char* desc) = 0;
};

//----------------------------------------------------------------------------------------------------------------------
// NodeDefProcessedDataExport
//----------------------------------------------------------------------------------------------------------------------
class NodeDefProcessedDataExport
{
public:
  virtual ~NodeDefProcessedDataExport() { };
  
  virtual const char* getName() const = 0;
  
  virtual ProcessedDataBlockExport* getProcessedDataBlock() = 0;
};

//----------------------------------------------------------------------------------------------------------------------
// NetworkDefProcessedDataExport
//----------------------------------------------------------------------------------------------------------------------
class NetworkDefProcessedDataExport : public ProcessedDataExport
{
public:
  virtual ~NetworkDefProcessedDataExport() { }
  
  virtual NodeDefProcessedDataExport* createNodeDefProcessedData(
    const char* nodeName) = 0;
};

//----------------------------------------------------------------------------------------------------------------------
/// ProcessedDataExportFactory
//----------------------------------------------------------------------------------------------------------------------
class ProcessedDataExportFactory
{
public:
  virtual ~ProcessedDataExportFactory() { }
  
  virtual NetworkDefProcessedDataExport* createNetworkDefProcessedData(
    const char* networkName,
    const char* destFilename) = 0;

  virtual void destroyNetworkDefProcessedData(NetworkDefProcessedDataExport* expObject) = 0;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace ME

#endif // APEXPORT_H
