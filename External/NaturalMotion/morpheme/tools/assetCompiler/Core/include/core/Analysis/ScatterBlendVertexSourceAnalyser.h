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
#ifndef AP_TRAJECTORYANALYSER_H
#define AP_TRAJECTORYANALYSER_H
//----------------------------------------------------------------------------------------------------------------------

#include "export/mcExport.h"
#include "NMPlatform/NMMemory.h"
#include "assetProcessor/AnalysisProcessor.h"

namespace MR
{
  class NetworkDef;
  class Network;
}

namespace AP
{

class AssetProcessor;
class ControlTake;
class AnalysisInterval;
class AnalysisParameteriser;

class ScatterBlendVertexSourceAnalyser : public AnalysisProcessor
{
public:
  bool analyse(
    AP::AssetProcessor* processor,
    const ME::AnalysisTaskExport* task,
    ME::DataBlockExport* resultData) NM_OVERRIDE;

private:
  bool analyseVertexSource(
    AssetProcessor* processor,
    uint32_t setIndex,
    const ControlTake* take,
    uint32_t frameCount,
    float deltaTime,
    MR::Network *network,
    AnalysisInterval *analysisInterval,
    AnalysisParameteriser *analysisParameteriser,
    ME::DataBlockExport* resultData);
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // AP_TRAJECTORYANALYSER_H
//----------------------------------------------------------------------------------------------------------------------

