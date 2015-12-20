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
#ifndef AP_ANALYSIS_PROCESSOR_H
#define AP_ANALYSIS_PROCESSOR_H
//----------------------------------------------------------------------------------------------------------------------
#include <map>
#include <vector>
#include "export/mcExport.h"
#include "NMPlatform/NMMemory.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

class AssetProcessor;

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::AnalysisProcessor
/// \brief A manager for processing analysis libraries into results to return to the caller.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class AnalysisProcessor
{
public:
  virtual bool analyse(
    AP::AssetProcessor* processor,
    const ME::AnalysisTaskExport* task,
    ME::DataBlockExport* resultData) = 0;

  static const ME::AnalysisNodeExport* findAnalysisNodeExport(
    const ME::AnalysisTaskExport* task,
    const char* name);

  static const ME::AnalysisNodeExport* findAnalysisNodeExport(
    const ME::NodeExport* nodeDefExport,
    const char* name);
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // AP_ANALYSIS_PROCESSOR_H
//----------------------------------------------------------------------------------------------------------------------
