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
#ifndef AP_SCATTERBLEND2D_ANNOTATION_SAMPLES_H
#define AP_SCATTERBLEND2D_ANNOTATION_SAMPLES_H
//----------------------------------------------------------------------------------------------------------------------

#include "export/mcExport.h"
#include "NMPlatform/NMMemory.h"
#include "assetProcessor/AnalysisProcessor.h"
#include "Analysis/AnalysisParameteriser.h"

namespace AP
{

class AssetProcessor;
class NetworkControlPlayer;
class SB2DAnnotation;
class SB2DAnnotationExportDataUnpacked;

//----------------------------------------------------------------------------------------------------------------------
class ScatterBlendTriangleAnnotationAnalyser : public AnalysisProcessor
{
public:
  virtual bool analyse(
    AP::AssetProcessor* processor,
    const ME::AnalysisTaskExport* task,
    ME::DataBlockExport* resultData) NM_OVERRIDE;

private:
  static bool evaluateInteriorSubSamples(
    NetworkControlPlayer* networkControlPlayer,
    AnalysisParameteriser* analysisParameteriser,
    MR::AnimSetIndex analysisAnimSetIndex,
    const std::vector<MR::NodeID>& sourceNodeIDs,
    SB2DAnnotation& annotation);

  static void writeIntermediateExportData(
    const SB2DAnnotationExportDataUnpacked& annotationExportData,
    ME::DataBlockExport* exportData);
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // AP_SCATTERBLEND2D_ANNOTATION_SAMPLES_H
//----------------------------------------------------------------------------------------------------------------------
