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
#ifndef SAMPLED_POS_COMPRESSOR_QSA_H
#define SAMPLED_POS_COMPRESSOR_QSA_H
//----------------------------------------------------------------------------------------------------------------------
#include <vector>
#include "NMPlatform/NMVector3.h"
#include "morpheme/AnimSource/mrAnimSectionQSA.h"
#include "assetProcessor/AnimSource/ChannelSetTableBuilder.h"
#include "assetProcessor/AnimSource/Vector3QuantisationTableBuilder.h"
#include "assetProcessor/AnimSource/QuantisationSetQSA.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
class AnimSectionCompressorQSA;

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::SampledPosCompressorQSA
/// \brief This class is responsible for compiling a sampled position channel in the QSA format.
/// \ingroup CompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class SampledPosCompressorQSA
{
public:
  SampledPosCompressorQSA(AnimSectionCompressorQSA* animSectionCompressor);
  ~SampledPosCompressorQSA();

  void computeRateAndDistortion();

  NMP::Vector3 evaluateQuantised(uint32_t channel, uint32_t frame) const;

  void computeDataStrides();

  void compileData();

  //-----------------------
  // Compiled data
  NM_INLINE uint32_t getSampledPosByteStride() const;
  NM_INLINE uint32_t getSampledPosNumQuantisationSets() const;

  NM_INLINE size_t getSampledPosQuantisationInfoSize() const;
  NM_INLINE const MR::QuantisationInfoQSA* getSampledPosQuantisationInfo() const;

  NM_INLINE size_t getSampledPosQuantisationDataSize() const;
  NM_INLINE const MR::QuantisationDataQSA* getSampledPosQuantisationData() const;

  NM_INLINE size_t getSampledPosDataSize() const;
  NM_INLINE const uint8_t* getSampledPosData() const;

protected:
  void computeRelativeTransforms();
  void assignQuantisationSets();
  void simpleRateAndDistortion();
  void analyseRateAndDistortion();
  void computeRateAndDistortionMapped();

protected:
  AnimSectionCompressorQSA*           m_animSectionCompressor;

  //-----------------------
  // Sampled pos channel
  Vector3Table*                       m_sampledPosTable;            ///< Channel table for the sampled pos channels
  NMP::Vector3*                       m_sampledPosMeans;            ///< Channel means for the sampled pos data
  Vector3QuantisationTable*           m_sampledPosQuantTable;

  //-----------------------
  // Quantisation sets
  std::vector<uint32_t>               m_sampledPosCompToQSetMapX;
  std::vector<uint32_t>               m_sampledPosCompToQSetMapY;
  std::vector<uint32_t>               m_sampledPosCompToQSetMapZ;

  //-----------------------
  // Compiled Data
  uint32_t                            m_sampledPosByteStride;
  uint32_t                            m_sampledPosNumQuantisationSets;

  size_t                              m_sampledPosQuantisationInfoSize;
  MR::QuantisationInfoQSA*            m_sampledPosQuantisationInfo; ///< Quantisation info about ranges of all sampled pos channels

  size_t                              m_sampledPosQuantisationDataSize;
  MR::QuantisationDataQSA*            m_sampledPosQuantisationData; ///< Array of quantisation data for the sampled pos channels

  size_t                              m_sampledPosDataSize;         ///< Size in bytes of the sampled pos bitstream data
  uint8_t*                            m_sampledPosData;             ///< Buffer for the sampled pos bitstream data
};

//----------------------------------------------------------------------------------------------------------------------
// SampledPosCompressorQSA inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SampledPosCompressorQSA::getSampledPosByteStride() const
{
  return m_sampledPosByteStride;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SampledPosCompressorQSA::getSampledPosNumQuantisationSets() const
{
  return m_sampledPosNumQuantisationSets;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE size_t SampledPosCompressorQSA::getSampledPosQuantisationInfoSize() const
{
  return m_sampledPosQuantisationInfoSize;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const MR::QuantisationInfoQSA* SampledPosCompressorQSA::getSampledPosQuantisationInfo() const
{
  return m_sampledPosQuantisationInfo;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE size_t SampledPosCompressorQSA::getSampledPosQuantisationDataSize() const
{
  return m_sampledPosQuantisationDataSize;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const MR::QuantisationDataQSA* SampledPosCompressorQSA::getSampledPosQuantisationData() const
{
  return m_sampledPosQuantisationData;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE size_t SampledPosCompressorQSA::getSampledPosDataSize() const
{
  return m_sampledPosDataSize;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint8_t* SampledPosCompressorQSA::getSampledPosData() const
{
  return m_sampledPosData;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // SAMPLED_POS_COMPRESSOR_QSA_H
//----------------------------------------------------------------------------------------------------------------------
