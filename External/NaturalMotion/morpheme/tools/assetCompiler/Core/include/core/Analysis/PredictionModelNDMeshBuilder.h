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
#ifndef MR_PREDICTION_MODEL_ND_MESH_BUILDER_H
#define MR_PREDICTION_MODEL_ND_MESH_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "assetProcessor/AssetProcessor.h"
#include "assetProcessor/PredictionModelBuilder.h"
#include "morpheme/Prediction/mrPredictionModelNDMesh.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

class NDMeshAnalysisData;
class NDMeshResampler;

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::PredictionModelNDMeshBuilder
/// \brief For construction of PredictionModelNDMesh in the asset compiler.
//----------------------------------------------------------------------------------------------------------------------
class PredictionModelNDMeshBuilder : public PredictionModelBuilder
{
public:
  /// \brief Allocate and build a PredictionModelDef.
  /// Returns a resource holding the compiled PredictionModelDef.
  static NMP::Memory::Resource init(
    const NDMeshAnalysisData* resultDataNDMesh,
    const NDMeshResampler* resultDataResampler);

  /// \brief Allocate and build a PredictionModelDef.
  /// Returns a resource holding the compiled PredictionModelDef.
  virtual NMP::Memory::Resource init(
    const ME::NodeExport*         nodeDefExport,
    const ME::NetworkDefExport*   netDefExport,
    AssetProcessor*               processor,
    MR::NetworkDef*               netDef,
    const ME::AnalysisNodeExport* analysisExport,
    MR::AnimSetIndex              animSetIndex) NM_OVERRIDE;

  /// \brief Relocate a PredictionModelDef that has been copied to a new block of memory.
  virtual void relocate(MR::PredictionModelDef* model) NM_OVERRIDE;
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_PREDICTION_MODEL_ND_MESH_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
