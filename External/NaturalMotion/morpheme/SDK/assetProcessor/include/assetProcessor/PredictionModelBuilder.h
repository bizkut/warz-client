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
#ifndef MR_PREDICTION_MODEL_BUILDER_H
#define MR_PREDICTION_MODEL_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"
#include "morpheme/mrNetworkDef.h"
#include "export/mcExport.h"
#include "AssetProcessor.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::PredictionModelBuilder
/// \brief For construction of PredictionModelDef in the asset compiler - virtual base class.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class PredictionModelBuilder
{
public:

  // \brief Allocate and build a PredictionModelDef.
  /// Returns a resource holding the compiled PredictionModelDef.
  virtual NMP::Memory::Resource init(
    const ME::NodeExport*         nodeDefExport,
    const ME::NetworkDefExport*   netDefExport,
    AssetProcessor*               processor,
    MR::NetworkDef*               netDef,
    const ME::AnalysisNodeExport* analysisExport,
    MR::AnimSetIndex              animSetIndex) = 0;

  /// \brief Relocate a PredictionModelDef that has been copied to a new block of memory.
  virtual void relocate(MR::PredictionModelDef* model) = 0;
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_PREDICTION_MODEL_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
