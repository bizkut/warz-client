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
#ifndef SPLINE_QUAT_COMPRESSOR_QSA_H
#define SPLINE_QUAT_COMPRESSOR_QSA_H
//----------------------------------------------------------------------------------------------------------------------
#include <vector>
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMVector3.h"
#include "NMNumerics/NMSimpleKnotVector.h"
#include "NMNumerics/NMQuatSpline.h"
#include "morpheme/AnimSource/mrAnimSectionQSA.h"
#include "assetProcessor/AnimSource/ChannelSetTableBuilder.h"
#include "assetProcessor/AnimSource/RotVecTableBuilder.h"
#include "assetProcessor/AnimSource/Vector3QuantisationTableBuilder.h"
#include "assetProcessor/AnimSource/QuantisationSetQSA.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
class AnimSectionCompressorQSA;

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::SplineQuatCompressorQSA
/// \brief This class is responsible for compiling a spline quat channel in the QSA format.
/// \ingroup CompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class SplineQuatCompressorQSA
{
public:
  SplineQuatCompressorQSA(AnimSectionCompressorQSA* animSectionCompressor);
  ~SplineQuatCompressorQSA();

  /// \brief Function for computing spline curves that fit the quat channel sample data
  /// with a given knot vector
  void computeSplines(const NMP::SimpleKnotVector& knots);

  void computeTangentMeansQuantisationInfo();

  void computeRateAndDistortion();

  void recompose(
    uint32_t      channel,
    uint32_t      span,
    float&        ta,
    float&        tb,
    NMP::Quat&    qa,
    NMP::Vector3& wa,
    NMP::Vector3& wb,
    NMP::Quat&    qb);

  NMP::Quat evaluateQuantised(
    uint32_t channel,
    uint32_t span,
    float    animFrame);

  NMP::Quat evaluateQuantised(
    uint32_t channel,
    float    t);

  uint32_t computeWNegsByteStride();

  void computeDataStrides();

  void compileData();

  // Debugging functions
  void writeDebug(FILE* filePointer);
  void writeQuatSplines(FILE* filePointer) const;
  void writeQuatTableKeys(FILE* filePointer) const;
  void writeQuatTableTangentsA(FILE* filePointer) const;
  void writeQuatTableTangentsB(FILE* filePointer) const;
  void writeQuantisationInfo(FILE* filePointer) const;
  void writeGlobalMeans(FILE* filePointer) const;
  void writeQuatKeysMeans(FILE* filePointer) const;
  void writeQuatTangentsMeans(FILE* filePointer) const;
  void writeRecomposedQuatSplines(FILE* filePointer);

  //-----------------------
  // Compiled data
  NM_INLINE const MR::QuantisationInfoQSA& getSplineQuatTangentMeansInfo() const;

  NM_INLINE uint32_t getSplineQuatWNegsByteStride() const;
  NM_INLINE uint32_t getSplineQuatKeysByteStride() const;
  NM_INLINE uint32_t getSplineQuatTangentsByteStride() const;

  NM_INLINE uint32_t getSplineQuatKeysNumQuantisationSets() const;
  NM_INLINE uint32_t getSplineQuatTangentsNumQuantisationSets() const;

  NM_INLINE size_t getSplineQuatKeysQuantisationInfoSize() const;
  NM_INLINE const MR::QuantisationInfoQSA* getSplineQuatKeysQuantisationInfo() const;

  NM_INLINE size_t getSplineQuatKeysQuantisationDataSize() const;
  NM_INLINE const MR::QuantisationDataQSA* getSplineQuatKeysQuantisationData() const;

  NM_INLINE size_t getSplineQuatWNegsDataSize() const;
  NM_INLINE const uint8_t* getSplineQuatWNegsData() const;

  NM_INLINE size_t getSplineQuatKeysDataSize() const;
  NM_INLINE const uint8_t* getSplineQuatKeysData() const;

  NM_INLINE size_t getSplineQuatTangentsQuantisationInfoSize() const;
  NM_INLINE const MR::QuantisationInfoQSA* getSplineQuatTangentsQuantisationInfo() const;

  NM_INLINE size_t getSplineQuatTangentsQuantisationDataSize() const;
  NM_INLINE const MR::QuantisationDataQSA* getSplineQuatTangentsQuantisationData() const;

  NM_INLINE size_t getSplineQuatTangentsDataSize() const;
  NM_INLINE const uint8_t* getSplineQuatTangentsData() const;

protected:
  void computeRelativeTransforms();
  void assignQuantisationSets();
  void simpleRateAndDistortion();
  bool analyseRateAndDistortion();
  void makeDeltaToDistWeightMap();
  void computeRateAndDistortionMapped();

protected:
  AnimSectionCompressorQSA*           m_animSectionCompressor;

  //-----------------------
  // Spline fitting
  NMP::SimpleKnotVector*              m_knots;                      ///< The knot vector for fitting
  NMP::QuatSpline*                    m_qspSpan1;                   ///< Single span spline for error analysis
  std::vector<NMP::QuatSpline*>       m_channelQuatSplines;         ///< Anim channels of fitted quat splines (can be NULL)

  // Global [qMin, qMax] range for the tangent means
  MR::QuantisationInfoQSA             m_splineQuatTangentMeansInfo;

  //-----------------------
  // Spline quat channel
  RotVecTable*                        m_splineQuatTableKeys;        ///< Channel table for the spline quat keys stored as
                                                                    ///< tan quarter angle rotation vectors
  Vector3Table*                       m_splineQuatTableTangentsA;   ///< Channel table for the spline quat tangentsA
  Vector3Table*                       m_splineQuatTableTangentsB;   ///< Channel table for the spline quat tangentsB

  NMP::Quat*                          m_splineQuatKeysMeans;        ///< Channel means for the spline quat key data
  NMP::Vector3*                       m_splineQuatTangentsMeans;    ///< Channel means for the spline tangents data

  Vector3QuantisationTable*           m_splineQuatKeysQuantTable;
  Vector3QuantisationTable*           m_splineQuatTangentsQuantTable;

  //-----------------------
  // Quantisation sets
  std::vector<uint32_t>               m_splineQuatKeysCompToQSetMapX;
  std::vector<uint32_t>               m_splineQuatKeysCompToQSetMapY;
  std::vector<uint32_t>               m_splineQuatKeysCompToQSetMapZ;

  std::vector<uint32_t>               m_splineQuatTangentsCompToQSetMapX;
  std::vector<uint32_t>               m_splineQuatTangentsCompToQSetMapY;
  std::vector<uint32_t>               m_splineQuatTangentsCompToQSetMapZ;

  //-----------------------
  // Compiled Data
  uint32_t                            m_splineQuatWNegsByteStride;
  uint32_t                            m_splineQuatKeysByteStride;
  uint32_t                            m_splineQuatTangentsByteStride;
  uint32_t                            m_splineQuatKeysNumQuantisationSets;
  uint32_t                            m_splineQuatTangentsNumQuantisationSets;

  size_t                              m_splineQuatKeysQuantisationInfoSize;
  MR::QuantisationInfoQSA*            m_splineQuatKeysQuantisationInfo;     ///< Quantisation info about ranges of all spline quat key channels

  size_t                              m_splineQuatKeysQuantisationDataSize;
  MR::QuantisationDataQSA*            m_splineQuatKeysQuantisationData;     ///< Array of quantisation data for the spline quat key channels

  size_t                              m_splineQuatKeysWNegsDataSize;        ///< Size in bytes of the spline quat keys wNeg bitstream data
  uint8_t*                            m_splineQuatKeysWNegsData;            ///< Buffer for the spline quat keys wNeg bitstream data

  size_t                              m_splineQuatKeysDataSize;             ///< Size in bytes of the spline quat key bitstream data
  uint8_t*                            m_splineQuatKeysData;                 ///< Buffer for the spline quat key bitstream data

  size_t                              m_splineQuatTangentsQuantisationInfoSize;
  MR::QuantisationInfoQSA*            m_splineQuatTangentsQuantisationInfo; ///< Quantisation info about ranges of all spline quat tangent channels

  size_t                              m_splineQuatTangentsQuantisationDataSize;
  MR::QuantisationDataQSA*            m_splineQuatTangentsQuantisationData; ///< Array of quantisation data for the spline quat tangent channels

  size_t                              m_splineQuatTangentsDataSize;         ///< Size in bytes of the spline quat tangent bitstream data
  uint8_t*                            m_splineQuatTangentsData;             ///< Buffer for the spline quat tangent bitstream data
};

//----------------------------------------------------------------------------------------------------------------------
// SplineQuatCompressorQSA inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const MR::QuantisationInfoQSA& SplineQuatCompressorQSA::getSplineQuatTangentMeansInfo() const
{
  return m_splineQuatTangentMeansInfo;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SplineQuatCompressorQSA::getSplineQuatWNegsByteStride() const
{
  return m_splineQuatWNegsByteStride;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SplineQuatCompressorQSA::getSplineQuatKeysByteStride() const
{
  return m_splineQuatKeysByteStride;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SplineQuatCompressorQSA::getSplineQuatTangentsByteStride() const
{
  return m_splineQuatTangentsByteStride;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SplineQuatCompressorQSA::getSplineQuatKeysNumQuantisationSets() const
{
  return m_splineQuatKeysNumQuantisationSets;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t SplineQuatCompressorQSA::getSplineQuatTangentsNumQuantisationSets() const
{
  return m_splineQuatTangentsNumQuantisationSets;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE size_t SplineQuatCompressorQSA::getSplineQuatKeysQuantisationInfoSize() const
{
  return m_splineQuatKeysQuantisationInfoSize;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const MR::QuantisationInfoQSA* SplineQuatCompressorQSA::getSplineQuatKeysQuantisationInfo() const
{
  return m_splineQuatKeysQuantisationInfo;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE size_t SplineQuatCompressorQSA::getSplineQuatKeysQuantisationDataSize() const
{
  return m_splineQuatKeysQuantisationDataSize;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const MR::QuantisationDataQSA* SplineQuatCompressorQSA::getSplineQuatKeysQuantisationData() const
{
  return m_splineQuatKeysQuantisationData;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE size_t SplineQuatCompressorQSA::getSplineQuatWNegsDataSize() const
{
  return m_splineQuatKeysWNegsDataSize;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint8_t* SplineQuatCompressorQSA::getSplineQuatWNegsData() const
{
  return m_splineQuatKeysWNegsData;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE size_t SplineQuatCompressorQSA::getSplineQuatKeysDataSize() const
{
  return m_splineQuatKeysDataSize;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint8_t* SplineQuatCompressorQSA::getSplineQuatKeysData() const
{
  return m_splineQuatKeysData;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE size_t SplineQuatCompressorQSA::getSplineQuatTangentsQuantisationInfoSize() const
{
  return m_splineQuatTangentsQuantisationInfoSize;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const MR::QuantisationInfoQSA* SplineQuatCompressorQSA::getSplineQuatTangentsQuantisationInfo() const
{
  return m_splineQuatTangentsQuantisationInfo;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE size_t SplineQuatCompressorQSA::getSplineQuatTangentsQuantisationDataSize() const
{
  return m_splineQuatTangentsQuantisationDataSize;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const MR::QuantisationDataQSA* SplineQuatCompressorQSA::getSplineQuatTangentsQuantisationData() const
{
  return m_splineQuatTangentsQuantisationData;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE size_t SplineQuatCompressorQSA::getSplineQuatTangentsDataSize() const
{
  return m_splineQuatTangentsDataSize;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint8_t* SplineQuatCompressorQSA::getSplineQuatTangentsData() const
{
  return m_splineQuatTangentsData;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // SPLINE_QUAT_COMPRESSOR_QSA_H
//----------------------------------------------------------------------------------------------------------------------
