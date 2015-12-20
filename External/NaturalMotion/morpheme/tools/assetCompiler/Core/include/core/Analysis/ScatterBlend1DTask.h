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
#ifdef _MSC_VER
#pragma once
#endif
#ifndef AP_SCATTER_BLEND_1D_TASK_H
#define AP_SCATTER_BLEND_1D_TASK_H
//----------------------------------------------------------------------------------------------------------------------

#include "export/mcExport.h"
#include "NMPlatform/NMMemory.h"
#include "assetProcessor/AnalysisProcessor.h"
#include "Analysis/AnalysisParameteriser.h"

namespace AP
{

class AssetProcessor;
class NetworkControlPlayer;
class SB1DAnnotationExportDataPacked;
class SB1DAnnotationSubSampleRangeData;

//----------------------------------------------------------------------------------------------------------------------
class ScatterBlend1DParamAttributes
{
public:
  // Sample data information
  uint32_t  m_numSubDivisions;
};

//----------------------------------------------------------------------------------------------------------------------
class ScatterBlend1DTask : public AnalysisProcessor
{
public:
  virtual bool analyse(
    AP::AssetProcessor* processor,
    const ME::AnalysisTaskExport* task,
    ME::DataBlockExport* resultData) NM_OVERRIDE;

  static void writeIntermediateExportData(
    ME::DataBlockExport* exportData,
    uint32_t numSubDivisions);

  static void writePerAnimSetIntermediateExportData(
    ME::DataBlockExport* exportData,
    MR::AnimSetIndex animSet,
    const SB1DAnnotationExportDataPacked& annotationExportData,
    const SB1DAnnotationSubSampleRangeData& annotationRangeData);

  static void readPerAnimSetIntermediateExportData(
    const ME::DataBlockExport* exportData,
    MR::AnimSetIndex animSet,
    SB1DAnnotationExportDataPacked& annotationExportData,
    SB1DAnnotationSubSampleRangeData& annotationRangeData);

private:
  static void annotateVertexSamples(
    SB1DAnnotationExportDataPacked& annotationExportData);

  static bool evaluateVertexSamples(
    NetworkControlPlayer* networkControlPlayer,
    MR::AnimSetIndex animSet,
    const ME::AnalysisTaskExport* task,
    AnalysisParameteriser* analysisParameteriser,
    ScatterBlend1DParamAttributes* paramAttribs,
    std::vector<float>& vertexSamples);

  static bool evaluateLineInteriorSubSamples(
    NetworkControlPlayer* networkControlPlayer,
    MR::AnimSetIndex animSet,
    AnalysisParameteriser* analysisParameteriser,
    uint32_t vertexSourceIndexA,
    uint32_t vertexSourceIndexB,
    uint32_t numSamples,
    const float* weights,
    float* apSamples);

  static bool calculateAdjustedWrappingSample(
    float& apValue, // IN / OUT
    float rangeMinVal,
    float rangeMaxVal,
    float vertexSourceValueA,
    float vertexSourceValueB);

  static void evaluateWrappingSamples(
    AP::AssetProcessor* processor,
    NetworkControlPlayer* networkControlPlayer,
    MR::AnimSetIndex animSet,
    AnalysisParameteriser* analysisParameteriser,
    AnalysisPropertyBuilder* apBuilder,
    const ScatterBlend1DParamAttributes* paramAttribs,
    std::vector<float>& vertexSamples,
    const float* weights,
    bool& leftWrappingSamplesValid,
    bool& rightWrappingSamplesValid,
    uint32_t& leftVertexSourceIndex,
    uint32_t& rightVertexSourceIndex,
    float* leftLineSamples,
    float* rightLineSamples);
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // AP_SCATTER_BLEND_1D_TASK_H
//----------------------------------------------------------------------------------------------------------------------
