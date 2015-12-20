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
#ifndef SPLINE_POS_COMPRESSOR_QSA_H
#define SPLINE_POS_COMPRESSOR_QSA_H
//----------------------------------------------------------------------------------------------------------------------
#include <vector>
#include "NMPlatform/NMVector3.h"
#include "NMNumerics/NMSimpleKnotVector.h"
#include "NMNumerics/NMPosSpline.h"
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
/// \class AP::SplinePosCompressorQSA
/// \brief This class is responsible for compiling a spline position channel in the QSA format.
/// \ingroup CompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class SplinePosCompressorQSA
{
public:
  SplinePosCompressorQSA(AnimSectionCompressorQSA* animSectionCompressor);
  ~SplinePosCompressorQSA();

  /// \brief Function for computing spline curves that fit the pos channel sample data
  /// with a given knot vector
  void computeSplines(const NMP::SimpleKnotVector& knots);

  void computeRateAndDistortion();

  void recompose(
    uint32_t      channel,
    uint32_t      span,
    float&        ta,
    float&        tb,
    NMP::Vector3& p0,
    NMP::Vector3& p1,
    NMP::Vector3& p2,
    NMP::Vector3& p3);

  NMP::Vector3 evaluateQuantised(
    uint32_t channel,
    uint32_t span,
    float    animFrame);

  NMP::Vector3 evaluateQuantised(
    uint32_t channel,
    float    t);

  void computeDataStrides();

  void compileData();

  // Debugging functions
  void writeDebug(FILE* filePointer);
  void writePosSplines(FILE* filePointer) const;
  void writePosTableKeys(FILE* filePointer) const;
  void writePosTableTangentsA(FILE* filePointer) const;
  void writePosTableTangentsB(FILE* filePointer) const;
  void writeQuantisationInfo(FILE* filePointer) const;
  void writeGlobalMeans(FILE* filePointer) const;
  void writePosMeans(FILE* filePointer) const;
  void writeRecomposedPosSplines(FILE* filePointer);

  //-----------------------
  // Compiled data
  NM_INLINE uint32_t getSplinePosByteStride() const;
  NM_INLINE uint32_t getSplinePosNumQuantisationSets() const;

  NM_INLINE size_t getSplinePosQuantisationInfoSize() const;
  NM_INLINE const MR::QuantisationInfoQSA* getSplinePosQuantisationInfo() const;

  NM_INLINE size_t getSplinePosQuantisationDataSize() const;
  NM_INLINE const MR::QuantisationDataQSA* getSplinePosQuantisationData() const;

  NM_INLINE size_t getSplinePosDataSize() const;
  NM_INLINE const uint8_t* getSplinePosData() const;

protected:
  void computeRelativeTransforms();
  void assignQuantisationSets();
  void simpleRateAndDistortion();
  void analyseRateAndDistortion();
  void computeRateAndDistortionMapped();

protected:
  AnimSectionCompressorQSA*           m_animSectionCompressor;

  //-----------------------
  // Spline fitting
  NMP::SimpleKnotVector*              m_knots;                      ///< The knot vector for fitting
  NMP::PosSpline*                     m_pspSpan1;                   ///< Single span spline for error analysis
  std::vector<NMP::PosSpline*>        m_channelPosSplines;          ///< Anim channels of fitted pos splines (can be NULL)

  // Spline pos channel
  Vector3Table*                       m_splinePosTableKeys;         ///< Channel table for the spline pos keys
  Vector3Table*                       m_splinePosTableTangentsA;    ///< Channel table for the spline pos tangentsA
  Vector3Table*                       m_splinePosTableTangentsB;    ///< Channel table for the spline pos tangentsB

  NMP::Vector3*                       m_splinePosMeans;             ///< Channel means for the spline control point data
  Vector3QuantisationTable*           m_splinePosQuantTable;

  // Quantisation sets
  std::vector<uint32_t>               m_splinePosCompToQSetMapX;
  std::vector<uint32_t>               m_splinePosCompToQSetMapY;
  std::vector<uint32_t>               m_splinePosCompToQSetMapZ;

  //-----------------------
  // Compiled
  uint32_t                            m_splinePosByteStride;
  uint32_t                            m_splinePosNumQuantisationSets;

  size_t                              m_splinePosQuantisationInfoSize;
  MR::QuantisationInfoQSA*            m_splinePosQuantisationInfo;  ///< Quantisation info about ranges of all spline pos channels

  size_t                              m_splinePosQuantisationDataSize;
  MR::QuantisationDataQSA*            m_splinePosQuantisationData;  ///< Array of quantisation data for the spline pos channels

  size_t                              m_splinePosDataSize;          ///< Size in bytes of the spline pos bitstream data
  uint8_t*                            m_splinePosData;              ///< Buffer for the spline pos bitstream data
};

//----------------------------------------------------------------------------------------------------------------------
// SplinePosCompressorQSA inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SplinePosCompressorQSA::getSplinePosByteStride() const
{
  return m_splinePosByteStride;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SplinePosCompressorQSA::getSplinePosNumQuantisationSets() const
{
  return m_splinePosNumQuantisationSets;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE size_t SplinePosCompressorQSA::getSplinePosQuantisationInfoSize() const
{
  return m_splinePosQuantisationInfoSize;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const MR::QuantisationInfoQSA* SplinePosCompressorQSA::getSplinePosQuantisationInfo() const
{
  return m_splinePosQuantisationInfo;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE size_t SplinePosCompressorQSA::getSplinePosQuantisationDataSize() const
{
  return m_splinePosQuantisationDataSize;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const MR::QuantisationDataQSA* SplinePosCompressorQSA::getSplinePosQuantisationData() const
{
  return m_splinePosQuantisationData;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE size_t SplinePosCompressorQSA::getSplinePosDataSize() const
{
  return m_splinePosDataSize;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint8_t* SplinePosCompressorQSA::getSplinePosData() const
{
  return m_splinePosData;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // SPLINE_POS_COMPRESSOR_QSA_H
//----------------------------------------------------------------------------------------------------------------------
