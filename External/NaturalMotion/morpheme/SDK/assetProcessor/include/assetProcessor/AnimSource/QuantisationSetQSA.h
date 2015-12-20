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
#ifndef QUANTISATION_SET_QSA_H
#define QUANTISATION_SET_QSA_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMNumerics/NMUniformQuantisation.h"
#include "morpheme/AnimSource/mrAnimSectionQSA.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::CoefSetQSA
/// \brief A class that represents a quantisation coefficient set for a channel of a QSA animation. This
/// class is used to build an approximation for the quantisation delta error to the worldspace bone
/// position error.
/// \ingroup CompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class CoefSetQSA : public NMP::UniformQuantisationCoefSetBase
{
public:
  enum CoefSetQSAIDs
  {
    CoefSetSampledPos,
    CoefSetSampledQuat,
    CoefSetSplinePos,
    CoefSetSplineQuatKey,
    CoefSetSplineQuatTangent,
    CoefSetNumTypes
  };

public:
  //---------------------------------------------------------------------
  CoefSetQSA(uint32_t maxBitsOfPrecision);
  virtual ~CoefSetQSA();

  const uint32_t* getDistortionCounts() const { return m_distCount; }

  virtual void clearRateAndDistortion();
  virtual void computeRateAndDistortion();
  virtual void updateQuantisationInfo();
  virtual void quantise(NMP::BitStreamEncoder& encoder);
  virtual void dequantise(NMP::BitStreamDecoder& decoder);

  void setBounds(float qMin, float qMax);

  // Rate and distortion functions
  void clearRateAndDelta();
  void updateRate(uint32_t prec);
  void updateRate(uint32_t prec, uint32_t n);
  void updateDelta(uint32_t prec, float delta);
  void updateDistortion(uint32_t prec, float dist);

  // Delta to dist weight map
  void clearDeltaToDistMap();
  bool hasValidDeltaToDistWeightMap() const;
  bool makeDeltaToDistWeightMap();
  void setDeltaToDistWeightMap(const CoefSetQSA& coefSet);
  void setDeltaToDistWeightMap(float w);

  // Average distortion functions
  void updateDeltaToDistWeightMapAverage(const CoefSetQSA& coefSet);
  void computeDeltaToDistWeightMapAverage();

protected:
  uint32_t*     m_distCount;
  uint32_t*     m_deltaCount;
  float*        m_delta;
  float*        m_deltaToDistWeight;
  uint32_t      m_deltaToDistWeightCount;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::QuantisationSetQSA
/// \brief A class that encapsulates three quantisation coefficient sets that correspond to
/// the three components of a Vector3.
/// \ingroup CompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class QuantisationSetQSA
{
public:
  QuantisationSetQSA();
  ~QuantisationSetQSA();

  // Flat coefficient set index within the quantiser that refers to the first
  // coefficient set within this grouping
  uint32_t getQuantiserCoefSetIndex() const { return m_quantiserCoefSetIndex; }
  void setQuantiserCoefSetIndex(uint32_t index) { m_quantiserCoefSetIndex = index; }

  // External data pointers to coefficient sets. The memory is not managed by this class.
  CoefSetQSA* getCoefSetX() { return m_coefSetX; }
  CoefSetQSA* getCoefSetY() { return m_coefSetY; }
  CoefSetQSA* getCoefSetZ() { return m_coefSetZ; }
  void setCoefSetX(CoefSetQSA* coefSetX) { m_coefSetX = coefSetX; }
  void setCoefSetY(CoefSetQSA* coefSetY) { m_coefSetY = coefSetY; }
  void setCoefSetZ(CoefSetQSA* coefSetZ) { m_coefSetZ = coefSetZ; }

  // Rate and distortion characteristics
  void computeRateAndDistortion();

  /// \brief Function to check if all the coefficient sets have been allocated with full precision
  bool getHasFullPrec() const;

protected:
  uint32_t                            m_quantiserCoefSetIndex;

  // Pointers to external coefficient sets
  CoefSetQSA*                         m_coefSetX;
  CoefSetQSA*                         m_coefSetY;
  CoefSetQSA*                         m_coefSetZ;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::QuantisationInfoQSABuilder
/// \brief Builds a MR::QuantisationInfoQSA structure.
/// \ingroup CompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class QuantisationInfoQSABuilder : public MR::QuantisationInfoQSA
{
public:
  static void init(
    MR::QuantisationInfoQSA& qInfo,
    const NMP::Vector3&      qMin,
    const NMP::Vector3&      qMax);
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::QuantisationDataQSABuilder
/// \brief Builds a MR::QuantisationDataQSA structure.
/// \ingroup CompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class QuantisationDataQSABuilder : public MR::QuantisationDataQSA
{
public:
  static void init(
    MR::QuantisationDataQSA& qData,
    uint32_t meanX,
    uint32_t meanY,
    uint32_t meanZ);

  static void init(
    MR::QuantisationDataQSA& qData,
    uint32_t meanX,
    uint32_t meanY,
    uint32_t meanZ,
    uint32_t precX,
    uint32_t precY,
    uint32_t precZ,
    uint32_t qSetX,
    uint32_t qSetY,
    uint32_t qSetZ);
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // QUANTISATION_SET_QSA_H
//----------------------------------------------------------------------------------------------------------------------
