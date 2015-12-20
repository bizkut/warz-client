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
#ifndef AP_PREDICTION_MODEL_ND_MESH_ANALYSER_H
#define AP_PREDICTION_MODEL_ND_MESH_ANALYSER_H
//----------------------------------------------------------------------------------------------------------------------
#include "export/mcExport.h"
#include "NMPlatform/NMMemory.h"
#include "assetProcessor/AnalysisProcessor.h"
#include "Analysis/NetworkControlPlayer.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
class PredictionModelNDMesh;
}

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// Forward declarations
class AssetProcessor;
class ControlParameterRegularSampleGrid;
class AnalysisParameteriser;
class NDMeshResampler;
class NDMeshAnalysisData;

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::PredictionModelNDMeshAnalyser
/// \brief Takes a "PredictionModelNDMesh" analysis task from the AnalysisLibrary.xml,
///   that was exported from connect, and generates all of the data necessary for constructing a PredictionModelNDMesh.
///   A "PredictionModelNDMesh" analysis task consists of a:
///     + "PredictionModelND" analysis node,
///     + "RunNetworkScriptedAssetCompiler" that controls playback of the network for generation of each sample.
///     + set of AnalysisConditions that determine the interval for each sample.
///   The resulting data is returned to connect in a DataBlockExport for caching on the PredictionModelNDMesh".
///   This data is then sent to the PredictionModelNDMeshBuilder on final asset compilation.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class PredictionModelNDMeshAnalyser : public AnalysisProcessor
{
public:
  bool analyse(
    AP::AssetProcessor*           processor,
    const ME::AnalysisTaskExport* task,
    ME::DataBlockExport*          resultData) NM_OVERRIDE;

private:
  /// \brief Clean up any working data.
  void tidyWorkingData(AP::AssetProcessor* processor);

  /// \brief Tidy working data, output a message to the error log, set the AssetProcessor error flag.
  bool analysisValidationFail(
    AP::AssetProcessor* processor,
    const char*         format,      ///< If non-zero, a printf-style formatted explanation of the validation error.
    ...);
  
  // Working data structures references which are created at some point; 
  //  declared here so that the tidyWorkingData function can be called safely at any point.
  ControlParameterRegularSampleGrid*  m_regularSampleGrid;
  AnalysisParameteriser*              m_analysisParameteriser;
  NDMeshResampler*                    m_ndMeshResampler;
  NDMeshAnalysisData*                 m_resultDataNDMesh;
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // AP_PREDICTION_MODEL_ND_MESH_ANALYSER_H
//----------------------------------------------------------------------------------------------------------------------
