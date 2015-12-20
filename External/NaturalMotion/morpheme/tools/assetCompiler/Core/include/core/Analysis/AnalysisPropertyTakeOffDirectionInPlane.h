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
#ifndef AP_ANALYSIS_PROPERTY_TAKEOFF_DIRECTION_IN_PLANE_H
#define AP_ANALYSIS_PROPERTY_TAKEOFF_DIRECTION_IN_PLANE_H
//----------------------------------------------------------------------------------------------------------------------
#include "Analysis/AnalysisProperty.h"
#include "Analysis/AnalysisPropertyUtils.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AnalysisPropertyTakeOffDirectionInPlane
//----------------------------------------------------------------------------------------------------------------------
class AnalysisPropertyTakeOffDirectionInPlane : public AnalysisProperty
{
public:
  // Definition data
  AnalysisPropertyAxis            m_planeNormal;
  AnalysisPropertyAxis            m_facingDirection;
  bool                            m_flipPlaneNormalDirection;
  bool                            m_flipFacingDirection;

  // Extracted features
  AnalysisPropertySampleBuffer*   m_sampleBuffer;
  AnalysisPropertySubRange*       m_subRange;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AnalysisPropertyTakeOffDirectionInPlaneBuilder
//----------------------------------------------------------------------------------------------------------------------
class AnalysisPropertyTakeOffDirectionInPlaneBuilder : public AnalysisPropertyBuilder
{
public:
  /// \brief Compute the memory requirements for this analysis condition definition
  virtual NMP::Memory::Format getMemoryRequirements(
    AssetProcessor*               processor,
    MR::NetworkDef*               networkDef,
    const ME::AnalysisNodeExport* analysisExport,
    uint32_t                      numFrames) NM_OVERRIDE;

  /// \brief Initialise the data for this analysis condition definition
  virtual AnalysisProperty* init(
    NMP::Memory::Resource&        memRes,
    AssetProcessor*               processor,
    MR::NetworkDef*               networkDef,
    const ME::AnalysisNodeExport* analysisExport,
    float                         sampleFrequency,
    uint32_t                      numFrames) NM_OVERRIDE;

  /// \brief Function to compute the appropriate range distribution from the
  /// analysis property sample data.
  virtual bool calculateRangeDistribution(
    AssetProcessor*               processor,
    uint32_t                      numSamples,
    const float*                  samples,
    float&                        minVal,
    float&                        maxVal,
    float&                        centreVal) NM_OVERRIDE;

  /// \brief Function to initialise the network instance for the analysis property
  static void instanceInit(
    AnalysisProperty* property,
    MR::Network* network);

  /// \brief Function to update the network instance for the analysis property
  static void instanceUpdate(
    AnalysisProperty* property,
    MR::Network* network);

  /// \brief Function to evaluate the property value from the extracted features
  static void evaluateProperty(AnalysisProperty* property);
};

}

//----------------------------------------------------------------------------------------------------------------------
#endif // AP_ANALYSIS_PROPERTY_TAKEOFF_DIRECTION_IN_PLANE_H
//----------------------------------------------------------------------------------------------------------------------
