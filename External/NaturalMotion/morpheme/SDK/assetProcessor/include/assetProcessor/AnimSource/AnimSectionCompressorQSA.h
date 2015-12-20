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
#ifndef ANIM_SECTION_COMPRESSOR_QSA_H
#define ANIM_SECTION_COMPRESSOR_QSA_H
//----------------------------------------------------------------------------------------------------------------------
#include <vector>
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMMatrix.h"
#include "NMNumerics/NMSimpleKnotVector.h"
#include "NMNumerics/NMPosSpline.h"
#include "NMNumerics/NMQuatSpline.h"
#include "NMNumerics/NMUniformQuantisation.h"
#include "morpheme/AnimSource/mrAnimSectionQSA.h"
#include "assetProcessor/AnimSource/QuantisationSetQSA.h"
#include "assetProcessor/AnimSource/SampledPosCompressorQSA.h"
#include "assetProcessor/AnimSource/SampledQuatCompressorQSA.h"
#include "assetProcessor/AnimSource/SplinePosCompressorQSA.h"
#include "assetProcessor/AnimSource/SplineQuatCompressorQSA.h"
#include "assetProcessor/AnimSource/ChannelSetTableBuilder.h"
#include "assetProcessor/AnimSource/RotVecTableBuilder.h"
#include "assetProcessor/AnimSource/Vector3QuantisationTableBuilder.h"
#include "assetProcessor/AnimSource/TransformsAccumulator.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
class AnimSourceCompressorQSA;

//----------------------------------------------------------------------------------------------------------------------
/// \class    AP::AnimSectionCompressorQSA
/// \brief    This class is responsible for compiling a section of the animation keyframes into the QSA
///           animation format.
/// \ingroup  CompressedAnimationAssetProcessorModule
///
/// \details  A section of QSA data is composed of:
///   - Header data block.
///   - Sampled position channel quantisation min, max information.
///   - Uniformly quantised sampled position data
///   - Sampled rotation vector channel quantisation min, max information.
///   - Uniformly quantised sampled rotation vector data.
///   - A knot vector for the spline channels
///   - Spline position channel quantisation min, max information.
///   - Uniformly quantised spline position control point data
///   - Spline quat key quantisation min, max information.
///   - Uniformly quantised spline quat key data
///   - Spline quat tangent quantisation min, max information.
///   - Uniformly quantised spline quat tangent data
///
/// The packed data is stored continuous on channels for each frame in memory to ensure that the
/// data is not fragmented which can lead to cache miss performance hits at decompression time.
//----------------------------------------------------------------------------------------------------------------------
class AnimSectionCompressorQSA
{
public:
  //-----------------------
  // Constructor / Destructor
  AnimSectionCompressorQSA(AnimSourceCompressorQSA* manager, uint32_t sectionID);
  ~AnimSectionCompressorQSA();

  /// \brief Main function to fit splines to the frame data, transform the data
  /// into the required representation for compression then compile the resulting
  /// data into binary format by finding the optimal assignment of bits for each
  /// channel.
  bool compileSection();

  /// \brief Function to get the sum of squared errors between the reconstructed data
  /// and the original.
  float computeError(
    const ChannelSetTable* channelSets,
    bool useWorldSpace,
    bool generateDeltas);

  /// \brief Function to write debug information about the section to a file.
  void writeDebug(FILE* filePointer) const;

  void writeQuantiser(FILE* filePointer) const;

  void writeRecomposedWS(FILE* filePointer);
  void writeRecomposedDiffWS(FILE* filePointer);

  //---------------------------------------------------------------------
  /// \name   Accessors
  /// \brief  Functions that retrieve information for the section data.
  //---------------------------------------------------------------------
  //@{

  /// \brief Function to get the start frame index of this section.
  uint32_t getSectionStartFrame() const { return m_sectionStartFrame; }

  /// \brief Function to get the end frame index of this section.
  uint32_t getSectionEndFrame() const { return m_sectionEndFrame; }

  /// \brief Function to compute the number of frames in this section.
  uint32_t getNumSectionFrames() const;

  /// \brief Function to get the desired section byte budget.
  size_t getSectionBudgetSize() const { return m_sectionSize; }

  /// \brief Function to check if the section coefficient sets were allocated with full precision.
  bool getAnimSectionHasFullPrec() const;

  /// \brief Function to get the parent animation compressor to which this section belongs.
  AnimSourceCompressorQSA* getAnimSourceCompressor() { return m_manager; }

  /// \brief Function to get the bone weight table used to compare worldspace bone position
  /// errors due to the quantision error between position and orientation channels.
  const NMP::Matrix<float>* getBonePosWeights() const { return m_bonePosWeights; }

  /// \brief Function to get the worldspace bone position tables used to compare the quantisation errors
  /// between position and orientation channels.
  const std::vector<Vector3Table*>& getBonePosTables() const { return m_bonePosTables; }

  /// \brief Function to get the frame data means of the animated position channels.
  const MR::QuantisationInfoQSA& getPosMeansQuantisationInfo() const { return m_posMeansQuantisationInfo; }

  // Compiled section
  size_t getAnimSectionHdrSize() const { return m_animSectionHdrSize; }
  size_t getSampledPosQuantisationInfoSize() const { return m_sampledPosQuantisationInfoSize; }
  size_t getSampledPosQuantisationDataSize() const { return m_sampledPosQuantisationDataSize; }
  size_t getSampledPosDataSize() const { return m_sampledPosDataSize; }
  size_t getSampledQuatQuantisationInfoSize() const { return m_sampledQuatQuantisationInfoSize; }
  size_t getSampledQuatQuantisationDataSize() const { return m_sampledQuatQuantisationDataSize; }
  size_t getSampledQuatDataSize() const { return m_sampledQuatDataSize; }
  size_t getSplineKnotsSize() const { return m_splineKnotsSize; }
  size_t getSplinePosQuantisationInfoSize() const { return m_splinePosQuantisationInfoSize; }
  size_t getSplinePosQuantisationDataSize() const { return m_splinePosQuantisationDataSize; }
  size_t getSplinePosDataSize() const { return m_splinePosDataSize; }
  size_t getSplineQuatKeysQuantisationInfoSize() const { return m_splineQuatKeysQuantisationInfoSize; }
  size_t getSplineQuatKeysQuantisationDataSize() const { return m_splineQuatKeysQuantisationDataSize; }
  size_t getSplineQuatKeysWNegsDataSize() const { return m_splineQuatKeysWNegsDataSize; }
  size_t getSplineQuatKeysDataSize() const { return m_splineQuatKeysDataSize; }
  size_t getSplineQuatTangentsQuantisationInfoSize() const { return m_splineQuatTangentsQuantisationInfoSize; }
  size_t getSplineQuatTangentsQuantisationDataSize() const { return m_splineQuatTangentsQuantisationDataSize; }
  size_t getSplineQuatTangentsDataSize() const { return m_splineQuatTangentsDataSize; }

  /// \brief Function to get the section size in bytes.
  size_t getSectionDataSize() const { return m_sectionDataSize; }

  /// \brief Function to retrieve the compiled section data.
  const MR::AnimSectionQSA* getSectionData() const { return m_sectionData; }
  //@}

  //---------------------------------------------------------------------
  /// \name   Quantisation set functions
  /// \brief  Functions that manipulate the state of the quantisation coefficient sets.
  ///
  /// In order to optimally assign bits from a given byte budget amongst the quantisation
  /// coefficient sets, the compressor must compute a rate and distortion error analysis on the
  /// sample / spline data. Quantisation errors in position and orientation channels are compared
  /// in a common frame (worldspace position of bones). Individually quantising each channel and
  /// accumulating the worldspace transforms of the bone positions is an expensive process. Instead
  /// of performing a full analysis over all precision levels, we perform it over a few levels and
  /// compute a mapping between the quantistion delta error and the worldspace bone error by linear
  /// regression. This is then used to approximate the remaining precision levels.
  //---------------------------------------------------------------------
  //@{

  /// \brief Retrieves the maximum number of bits that can be used to quantise a data sample
  uint32_t getMaxPrec() const { return m_maxPrec; }

  /// \brief Retrieves the vector of precision values that are used to run a complete rate / distortion
  /// analysis on for the optimal allocation of bits of to the sample data.
  const std::vector<uint32_t>& getAnalysisPrecisions() const { return m_analysisPrecisions; }

  /// \brief Get the number of quantisation sets associated with the coefficient set type.
  /// \see AP::CoefSetQSA
  uint32_t getNumQuantisationSets(uint32_t setType) const;

  /// \brief Get the quantisation set corresponding to the set type and index
  QuantisationSetQSA* getQuantisationSet(
    uint32_t setType,
    uint32_t qSetIndex);

  float getQuantisationSetWeight(uint32_t setType) const;

  /// \brief Reset the rate and distortion analysis errors ans well as the delta mapping approximation weights.
  void clearRateAndDistortion(uint32_t setType);

  /// \brief Reset the delta mapping approximation weights only for the quantisation analysis.
  void clearRateAndDelta(uint32_t setType);

  /// \brief Set the delta mapping approximation by assuming the quantisation delta error is directly
  /// proportional to the worldspace bone position error.
  void makeSimpleDeltaToDistWeightMap(uint32_t setType);

  /// \brief Having computed the full rate and distortion analysis at the specified precision levels,
  /// compute the linear weight maps that approximate the worldspace bone position errors from the
  /// quantisation delta errors.
  void makeDeltaToDistWeightMap(uint32_t setType);

  /// \brief Having computed the delta mapping approximation compute the rate and distortion for all
  /// coefficient sets in the specified quantisation set type.
  void computeRateAndDistortion(uint32_t setType);
  //@}

  //---------------------------------------------------------------------
  /// \name   Compressor callback functions
  /// \brief  Function handlers that are called by the sectioning compressor framework
  //---------------------------------------------------------------------
  //@{

  // Callback function to initialise the compressor
  void initCompressor();

  // Callback function to perform cleanup prior to terminating the compressor
  void termCompressor();
  //@}

protected:
  //---------------------------------------------------------------------
  /// \name   Helper functions
  /// \brief  Internal processing functions.
  //---------------------------------------------------------------------
  //@{
  void initQuantisation();
  void termQuantisation();
  void saveQuantisationState();
  void restoreQuantisationState();

  void computeBonePosWeights();
  void computeBonePosTables();

  void recompose(
    uint32_t frame,
    TransformsAccumulator* tmAccumulator,
    bool useWorldSpace,
    bool generateDeltas) const;

  void computePosMeansQuantisationInfo();

  void currentSectionHeaderSize();
  void currentSectionDataSize();
  void currentSectionSize();

  void buildSection();
  //@}

protected:
  //---------------------------------------------------------------------
  AnimSourceCompressorQSA*            m_manager;
  uint32_t                            m_sectionID;

  //-----------------------
  // Section Information
  uint32_t                            m_numChannelSets;
  uint32_t                            m_sectionStartFrame;
  uint32_t                            m_sectionEndFrame;
  size_t                              m_sectionSize;

  //-----------------------
  // Spline fitting
  NMP::SimpleKnotVector*              m_knots;                      ///< The knot vector for fitting
  NMP::SimpleKnotVector*              m_knotsSaved;

  //-----------------------
  // Uniform quantisation
  NMP::UniformQuantisation*           m_quantiser;                  ///< Uniform quantiser

  // Quantisation sets
  uint32_t                            m_maxPrec;
  std::vector<uint32_t>               m_analysisPrecisions;
  std::vector<QuantisationSetQSA*>    m_quantisationSets[CoefSetQSA::CoefSetNumTypes];
  float                               m_quantisationSetWeights[CoefSetQSA::CoefSetNumTypes];

  // Error analysis
  uint32_t*                           m_hierarchyDists;
  std::vector<Vector3Table*>          m_bonePosTables;
  TransformsAccumulator*              m_tmAccumulatorA;
  TransformsAccumulator*              m_tmAccumulatorB;
  NMP::Matrix<float>*                 m_bonePosWeights; // frames by rig chans

  // Section byte budget
  size_t                              m_curSectionHeaderSize;
  size_t                              m_curSectionDataSize;
  size_t                              m_curSectionSize;

  //-----------------------
  // Quantisation means info
  MR::QuantisationInfoQSA             m_posMeansQuantisationInfo;               ///< Global quantisation range for the pos keys in this section

  //-----------------------
  // Sampled pos channel
  SampledPosCompressorQSA*            m_sampledPosCompressor;

  //-----------------------
  // Sampled quat channel
  SampledQuatCompressorQSA*           m_sampledQuatCompressor;

  //-----------------------
  // Spline pos channel
  SplinePosCompressorQSA*             m_splinePosCompressor;
  SplinePosCompressorQSA*             m_splinePosCompressorSaved;

  //-----------------------
  // Spline quat channel
  SplineQuatCompressorQSA*            m_splineQuatCompressor;
  SplineQuatCompressorQSA*            m_splineQuatCompressorSaved;

  // Compiled section
  size_t                              m_animSectionHdrSize;

  size_t                              m_sampledPosQuantisationInfoSize;
  size_t                              m_sampledPosQuantisationDataSize;
  size_t                              m_sampledPosDataSize;

  size_t                              m_sampledQuatQuantisationInfoSize;
  size_t                              m_sampledQuatQuantisationDataSize;
  size_t                              m_sampledQuatDataSize;

  size_t                              m_splineKnotsSize;

  size_t                              m_splinePosQuantisationInfoSize;
  size_t                              m_splinePosQuantisationDataSize;
  size_t                              m_splinePosDataSize;

  size_t                              m_splineQuatKeysQuantisationInfoSize;
  size_t                              m_splineQuatKeysQuantisationDataSize;
  size_t                              m_splineQuatKeysWNegsDataSize;
  size_t                              m_splineQuatKeysDataSize;

  size_t                              m_splineQuatTangentsQuantisationInfoSize;
  size_t                              m_splineQuatTangentsQuantisationDataSize;
  size_t                              m_splineQuatTangentsDataSize;

  size_t                              m_sectionDataSize;
  MR::AnimSectionQSA*                 m_sectionData;                ///< Compiled section data
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // ANIM_SECTION_COMPRESSOR_QSA_H
//----------------------------------------------------------------------------------------------------------------------
