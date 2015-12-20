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
#ifndef SAMPLED_QUAT_COMPRESSOR_QSA_H
#define SAMPLED_QUAT_COMPRESSOR_QSA_H
//----------------------------------------------------------------------------------------------------------------------
#include <vector>
#include "NMPlatform/NMQuat.h"
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
/// \class AP::SampledQuatCompressorQSA
/// \brief This class is responsible for compiling a sampled quat channel in the QSA format.
/// \ingroup CompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class SampledQuatCompressorQSA
{
public:
  SampledQuatCompressorQSA(AnimSectionCompressorQSA* animSectionCompressor);
  ~SampledQuatCompressorQSA();

  void computeRateAndDistortion();

  NMP::Quat evaluateQuantised(uint32_t channel, uint32_t frame) const;

  void computeDataStrides();

  void compileData();

  // Debugging functions
  void writeDebug(FILE* filePointer);
  void writeQuatKeysMeans(FILE* filePointer) const;
  void writeQuatTableKeys(FILE* filePointer) const;
  void writeQuantisationInfo(FILE* filePointer) const;

  //-----------------------
  // Compiled data
  NM_INLINE uint32_t getSampledQuatByteStride() const;
  NM_INLINE uint32_t getSampledQuatNumQuantisationSets() const;

  NM_INLINE size_t getSampledQuatQuantisationInfoSize() const;
  NM_INLINE const MR::QuantisationInfoQSA* getSampledQuatQuantisationInfo() const;

  NM_INLINE size_t getSampledQuatQuantisationDataSize() const;
  NM_INLINE const MR::QuantisationDataQSA* getSampledQuatQuantisationData() const;

  NM_INLINE size_t getSampledQuatDataSize() const;
  NM_INLINE const uint8_t* getSampledQuatData() const;

protected:
  void computeRelativeTransforms();
  void assignQuantisationSets();
  void simpleRateAndDistortion();
  void analyseRateAndDistortion();
  void computeRateAndDistortionMapped();

protected:
  AnimSectionCompressorQSA*           m_animSectionCompressor;

  //-----------------------
  // Sampled quat channel
  RotVecTable*                        m_sampledQuatTable;           ///< Channel table for the sampled relative quat channels, stored as
                                                                    ///< tan quarter angle rotation vectors
  NMP::Quat*                          m_sampledQuatMeans;           ///< Channel means for the sampled quat data
  Vector3QuantisationTable*           m_sampledQuatQuantTable;

  //-----------------------
  // Quantisation sets
  std::vector<uint32_t>               m_sampledQuatCompToQSetMapX;
  std::vector<uint32_t>               m_sampledQuatCompToQSetMapY;
  std::vector<uint32_t>               m_sampledQuatCompToQSetMapZ;

  //-----------------------
  // Compiled
  uint32_t                            m_sampledQuatByteStride;
  uint32_t                            m_sampledQuatNumQuantisationSets;

  size_t                              m_sampledQuatQuantisationInfoSize;
  MR::QuantisationInfoQSA*            m_sampledQuatQuantisationInfo; ///< Quantisation info about ranges of all sampled quat channels

  size_t                              m_sampledQuatQuantisationDataSize;
  MR::QuantisationDataQSA*            m_sampledQuatQuantisationData; ///< Array of quantisation data for the sampled quat channels

  size_t                              m_sampledQuatDataSize;         ///< Size in bytes of the sampled quat bitstream data
  uint8_t*                            m_sampledQuatData;             ///< Buffer for the sampled quat bitstream data
};

//----------------------------------------------------------------------------------------------------------------------
// SampledQuatCompressorQSA inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SampledQuatCompressorQSA::getSampledQuatByteStride() const
{
  return m_sampledQuatByteStride;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SampledQuatCompressorQSA::getSampledQuatNumQuantisationSets() const
{
  return m_sampledQuatNumQuantisationSets;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE size_t SampledQuatCompressorQSA::getSampledQuatQuantisationInfoSize() const
{
  return m_sampledQuatQuantisationInfoSize;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const MR::QuantisationInfoQSA* SampledQuatCompressorQSA::getSampledQuatQuantisationInfo() const
{
  return m_sampledQuatQuantisationInfo;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE size_t SampledQuatCompressorQSA::getSampledQuatQuantisationDataSize() const
{
  return m_sampledQuatQuantisationDataSize;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const MR::QuantisationDataQSA* SampledQuatCompressorQSA::getSampledQuatQuantisationData() const
{
  return m_sampledQuatQuantisationData;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE size_t SampledQuatCompressorQSA::getSampledQuatDataSize() const
{
  return m_sampledQuatDataSize;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint8_t* SampledQuatCompressorQSA::getSampledQuatData() const
{
  return m_sampledQuatData;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // SAMPLED_QUAT_COMPRESSOR_QSA_H
//----------------------------------------------------------------------------------------------------------------------
