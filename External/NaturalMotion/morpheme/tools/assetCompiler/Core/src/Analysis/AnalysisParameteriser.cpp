// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "assetProcessor/AssetProcessor.h"
#include "assetProcessor/AnalysisProcessor.h"
#include "Analysis/AnalysisParameteriser.h"
#include "NMPlatform/NMString.h"
#include "export/mcExport.h"
//----------------------------------------------------------------------------------------------------------------------

using namespace ME;

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// AnalysisParameteriser
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnalysisParameteriser::getMemoryRequirements(uint32_t maxNumProperties)
{
  NMP::Memory::Format result(sizeof(AnalysisParameteriser), NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Format memReqsProps = NMP::VectorContainer<AnalysisProperty*>::getMemoryRequirements(maxNumProperties);
  result += memReqsProps;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisParameteriser* AnalysisParameteriser::init(NMP::Memory::Resource& resource, uint32_t maxNumProperties)
{
  NMP::Memory::Format memReqsHdr(sizeof(AnalysisParameteriser), NMP_VECTOR_ALIGNMENT);
  AnalysisParameteriser* result = (AnalysisParameteriser*)resource.alignAndIncrement(memReqsHdr);

  NMP::Memory::Format memReqsProps = NMP::VectorContainer<AnalysisProperty*>::getMemoryRequirements(maxNumProperties);
  result->m_analysisProperties = NMP::VectorContainer<AnalysisProperty*>::init(resource, maxNumProperties);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisParameteriser* AnalysisParameteriser::create(uint32_t maxNumProperties)
{
  NMP::Memory::Format memReqs = getMemoryRequirements(maxNumProperties);
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  AnalysisParameteriser* result = init(memRes, maxNumProperties);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisParameteriser::releaseAndDestroy()
{
  // Release all of the analysis properties
  NMP::VectorContainer<AnalysisProperty*>::iterator it = m_analysisProperties->begin();
  for (; it != m_analysisProperties->end(); ++it)
  {
    AnalysisProperty* analysisProperty = *it;
    NMP_VERIFY(analysisProperty);
    analysisProperty->releaseAndDestroy();
  }

  // Release the analysis parameteriser
  NMP::Memory::memFree(this);
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisParameteriser::readAnalysisProperties(
  AP::AssetProcessor* processor,
  MR::NetworkDef* networkDef,
  float sampleFrequency,
  uint32_t numFrames,
  const ME::AnalysisTaskExport* task,
  const ME::AnalysisNodeExport* analyserExport)
{
  NMP_VERIFY(analyserExport);
  const ME::DataBlockExport* analysisNodeDataBlock = analyserExport->getDataBlock();
  bool status;

  uint32_t analysisPropertyCount = 0;
  status = analysisNodeDataBlock->readUInt(analysisPropertyCount, "AnalysisPropertyCount");
  NMP_VERIFY_MSG(
    status,
    "Could not find AnalysisPropertyCount attribute in the export");
  NMP_VERIFY_MSG(
    analysisPropertyCount > 0,
    "Could not find any analysis properties");

  for (uint32_t i = 0; i < analysisPropertyCount; ++i)
  {
    char attrName[256];
    sprintf_s(attrName, 256 - 1, "AnalysisProperty%d", i);

    std::string analysisPropertyPath;
    status = analysisNodeDataBlock->readString(analysisPropertyPath, attrName);
    NMP_VERIFY_MSG(
      status,
      "Could not find %s attribute in the export",
      attrName);

    const ME::AnalysisNodeExport* analysisExport = AnalysisProcessor::findAnalysisNodeExport(
      task,
      analysisPropertyPath.c_str());
    NMP_VERIFY_MSG(
      analysisExport,
      "Unable to locate analysis property export %s",
      analysisPropertyPath.c_str());

    AnalysisPropertyBuilder* analysisPropertyBuilder = processor->getAnalysisPropertyBuilderForType(analysisExport->getTypeName());
    NMP_VERIFY_MSG(
      analysisPropertyBuilder,
      "Unable to locate the builder for the analysis property %s",
      analysisPropertyPath.c_str());

    // Create the analysis property
    AnalysisProperty* analysisProperty = analysisPropertyBuilder->create(
      processor,
      networkDef,
      analysisExport,
      sampleFrequency,
      numFrames);

    m_analysisProperties->push_back(analysisProperty);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisParameteriser::instanceInit(MR::Network* network)
{
  NMP::VectorContainer<AnalysisProperty*>::iterator it = m_analysisProperties->begin();
  for (; it != m_analysisProperties->end(); ++it)
  {
    AnalysisProperty* analysisProperty = *it;
    NMP_VERIFY(analysisProperty);
    analysisProperty->instanceInit(network);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisParameteriser::instanceUpdate(MR::Network* network)
{
  NMP::VectorContainer<AnalysisProperty*>::iterator it = m_analysisProperties->begin();
  for (; it != m_analysisProperties->end(); ++it)
  {
    AnalysisProperty* analysisProperty = *it;
    NMP_VERIFY(analysisProperty);
    analysisProperty->instanceUpdate(network);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisParameteriser::instanceRelease(MR::Network* network)
{
  NMP::VectorContainer<AnalysisProperty*>::iterator it = m_analysisProperties->begin();
  for (; it != m_analysisProperties->end(); ++it)
  {
    AnalysisProperty* analysisProperty = *it;
    NMP_VERIFY(analysisProperty);
    analysisProperty->instanceRelease(network);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisParameteriser::evaluateProperties()
{
  NMP::VectorContainer<AnalysisProperty*>::iterator it = m_analysisProperties->begin();
  for (; it != m_analysisProperties->end(); ++it)
  {
    AnalysisProperty* analysisProperty = *it;
    NMP_VERIFY(analysisProperty);
    analysisProperty->evaluateProperty();
  }
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisProperty* AnalysisParameteriser::getAnlysisProperty(uint32_t idx) const
{
  NMP::VectorContainer<AnalysisProperty*>::iterator it = m_analysisProperties->begin();
  for (uint32_t i = 0; i < idx; ++i)
  {
    if (it == m_analysisProperties->end())
    {
      return NULL;
    }
    ++it;
  }

  AnalysisProperty* result = *it;
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisPropertyBuilder* AnalysisParameteriser::getAnalysisPropertyBuilder(
  AP::AssetProcessor* processor,
  uint32_t idx,
  const ME::AnalysisTaskExport* task,
  const ME::AnalysisNodeExport* analyserExport) const
{
  NMP_VERIFY(analyserExport);
  const ME::DataBlockExport* analysisNodeDataBlock = analyserExport->getDataBlock();

  uint32_t analysisPropertyCount = 0;
  analysisNodeDataBlock->readUInt(analysisPropertyCount, "AnalysisPropertyCount");
  NMP_VERIFY_MSG(
    idx < analysisPropertyCount,
    "Invalid analysis property index");

  char attrName[256];
  sprintf_s(attrName, 256 - 1, "AnalysisProperty%d", idx);

  std::string analysisPropertyPath;
  analysisNodeDataBlock->readString(analysisPropertyPath, attrName);
  NMP_VERIFY_MSG(
    analysisPropertyPath.length(),
    "Invalid analysis property export path");

  const ME::AnalysisNodeExport* analysisExport = AnalysisProcessor::findAnalysisNodeExport(
    task,
    analysisPropertyPath.c_str());

  NMP_VERIFY_MSG(
    analysisExport,
    "Unable to locate analysis property export %s",
    analysisPropertyPath.c_str());

  AnalysisPropertyBuilder* analysisPropertyBuilder = processor->getAnalysisPropertyBuilderForType(analysisExport->getTypeName());
  NMP_VERIFY_MSG(
    analysisPropertyBuilder,
    "Unable to locate the builder for the analysis property %s",
    analysisPropertyPath.c_str());

  return analysisPropertyBuilder;
}

}
