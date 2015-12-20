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
#ifndef AP_SCATTERBLEND2D_BSP_TREE_H
#define AP_SCATTERBLEND2D_BSP_TREE_H
//----------------------------------------------------------------------------------------------------------------------
#include "export/mcExport.h"
#include "NMPlatform/NMMemory.h"
#include "assetProcessor/AnalysisProcessor.h"
#include "Analysis/NetworkControlPlayer.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

class AssetProcessor;

class ScatterBlend2DBSPTreeAnalyser : public AnalysisProcessor
{
public:
  bool analyse(
    AP::AssetProcessor* processor,
    const ME::AnalysisTaskExport* task,
    ME::DataBlockExport* resultData) NM_OVERRIDE;
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // AP_SCATTERBLEND2D_BSP_TREE_H
//----------------------------------------------------------------------------------------------------------------------
