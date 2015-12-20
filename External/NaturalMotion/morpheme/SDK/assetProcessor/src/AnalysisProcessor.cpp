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
#include <algorithm>
#include "assetProcessor/AssetProcessor.h"
#include "assetProcessor/AnalysisProcessor.h"

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
const ME::AnalysisNodeExport* AnalysisProcessor::findAnalysisNodeExport(
  const ME::AnalysisTaskExport* task,
  const char* name)
{
  NMP_VERIFY(task);
  NMP_VERIFY(name);

  // Get the analysis properties
  uint32_t numStatistics = task->getNumAnalysisNodes();
  for (uint32_t i = 0; i < numStatistics; ++i)
  {
    const ME::AnalysisNodeExport* analysisExport = task->getAnalysisNode(i);
    NMP_VERIFY(analysisExport);
    if (strcmp(analysisExport->getName(), name) == 0)
    {
      return analysisExport;
    }
  }
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
const ME::AnalysisNodeExport* AnalysisProcessor::findAnalysisNodeExport(
  const ME::NodeExport* nodeDefExport,
  const char* name)
{
  NMP_VERIFY(nodeDefExport);
  NMP_VERIFY(name);

  // Get the analysis properties
  uint32_t numStatistics = nodeDefExport->getNumAnalysisNodes();
  for (uint32_t i = 0; i < numStatistics; ++i)
  {
    const ME::AnalysisNodeExport* analysisExport = nodeDefExport->getAnalysisNode(i);
    NMP_VERIFY(analysisExport);
    if (strcmp(analysisExport->getName(), name) == 0)
    {
      return analysisExport;
    }
  }
  return NULL;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
