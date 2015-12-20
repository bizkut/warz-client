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
#include "assetProcessor/AnimSource/QuantisationSetQSA.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// CoefSetQSA
//----------------------------------------------------------------------------------------------------------------------
CoefSetQSA::CoefSetQSA(uint32_t maxBitsOfPrecision) : NMP::UniformQuantisationCoefSetBase(maxBitsOfPrecision)
{
  m_distCount = new uint32_t[m_numQuantisers];
  m_deltaCount = new uint32_t[m_numQuantisers];
  m_delta = new float[m_numQuantisers];
  m_deltaToDistWeight = new float[m_numQuantisers];
}

//----------------------------------------------------------------------------------------------------------------------
CoefSetQSA::~CoefSetQSA()
{
  delete[] m_distCount;
  delete[] m_deltaCount;
  delete[] m_delta;
  delete[] m_deltaToDistWeight;
}

//----------------------------------------------------------------------------------------------------------------------
void CoefSetQSA::clearRateAndDistortion()
{
  clearRateAndDelta();
  clearDeltaToDistMap();
}

//----------------------------------------------------------------------------------------------------------------------
void CoefSetQSA::computeRateAndDistortion()
{
  // Check if the rate and distortion has already been computed for this coefficient set
  if (m_rateDistComputed)
    return;

  // Compute the distortion errors through the delta to dist weight mapping
  NMP_VERIFY(hasValidDeltaToDistWeightMap());
  for (uint32_t i = 0; i < m_numQuantisers; ++i)
  {
    // Compute the delta distortion
    NMP_VERIFY(m_deltaCount[i] > 0);
    m_delta[i] = sqrt(m_delta[i] / m_deltaCount[i]);

    // Compute the mapped distortion
    m_dists[i] = m_deltaToDistWeight[i] * m_delta[i];
    m_distCount[i] = m_deltaCount[i];
  }
  makeDistortionConvex();
  NMP_VERIFY(isDistortionConvex());

  // Set the rate and distortion computed for this coefficient set
  m_rateDistComputed = true;
}

//----------------------------------------------------------------------------------------------------------------------
void CoefSetQSA::updateQuantisationInfo()
{
  if (m_qInfo.m_precision == 0)
  {
    m_qInfo.m_qMax = m_qInfo.m_qMin = 0.0f;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CoefSetQSA::quantise(NMP::BitStreamEncoder& NMP_UNUSED(encoder))
{
}

//----------------------------------------------------------------------------------------------------------------------
void CoefSetQSA::dequantise(NMP::BitStreamDecoder& NMP_UNUSED(decoder))
{
}

//----------------------------------------------------------------------------------------------------------------------
void CoefSetQSA::setBounds(float qMin, float qMax)
{
  NMP_VERIFY(qMin <= qMax);
  m_qInfo.m_qMin = qMin;
  m_qInfo.m_qMax = qMax;
}

//----------------------------------------------------------------------------------------------------------------------
void CoefSetQSA::clearRateAndDelta()
{
  UniformQuantisationCoefSetBase::clearRateAndDistortion();
  for (uint32_t i = 0; i < m_numQuantisers; ++i)
  {
    m_distCount[i] = 0;
    m_deltaCount[i] = 0;
    m_delta[i] = 0.0f;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CoefSetQSA::updateRate(uint32_t prec)
{
  NMP_VERIFY(prec < m_numQuantisers);
  m_rates[prec] += (float)prec;
}

//----------------------------------------------------------------------------------------------------------------------
void CoefSetQSA::updateRate(uint32_t prec, uint32_t n)
{
  NMP_VERIFY(prec < m_numQuantisers);
  m_rates[prec] += (float)(n * prec);
}

//----------------------------------------------------------------------------------------------------------------------
void CoefSetQSA::updateDelta(uint32_t prec, float delta)
{
  NMP_VERIFY(prec < m_numQuantisers);
  m_deltaCount[prec]++;
  m_delta[prec] += delta;
}

//----------------------------------------------------------------------------------------------------------------------
void CoefSetQSA::updateDistortion(uint32_t prec, float dist)
{
  NMP_VERIFY(prec < m_numQuantisers);
  m_distCount[prec]++;
  m_dists[prec] += dist;
}

//----------------------------------------------------------------------------------------------------------------------
void CoefSetQSA::clearDeltaToDistMap()
{
  for (uint32_t i = 0; i < m_numQuantisers; ++i)
  {
    m_deltaToDistWeight[i] = 0.0f;
  }
  m_deltaToDistWeightCount = 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool CoefSetQSA::hasValidDeltaToDistWeightMap() const
{
  return m_deltaToDistWeightCount > 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool CoefSetQSA::makeDeltaToDistWeightMap()
{
  uint32_t A[3];
  float iA[3], b[2], x[2];
  uint32_t det;
  float desc;

  // Initialise the 2x2 linear system
  A[0] = A[1] = A[2] = 0;
  b[0] = b[1] = 0;

  //-----------------------
  // Update the delta to dist weight map
  for (uint32_t i = 0; i < m_numQuantisers; ++i)
  {
    if (m_distCount[i] > 0)
    {
      // Compute the distortion
      NMP_VERIFY(i == 0 || m_rates[i] > 0.0f);
      m_dists[i] = sqrt(m_dists[i] / m_distCount[i]);

      // Compute the delta distortion
      NMP_VERIFY(m_deltaCount[i] > 0);
      m_delta[i] = sqrt(m_delta[i] / m_deltaCount[i]);

      // Compute the mapping weight: dist = w * delta
      if (m_delta[i] > 0.0f)
        m_deltaToDistWeight[i] = m_dists[i] / m_delta[i];
      else
        m_deltaToDistWeight[i] = 1.0f; // delta can be zero if values lie on either qMin, qMax

      // Update the linear system
      if (i > 0)
      {
        A[0] += (i * i);
        A[1] += i;
        A[2] += 1;
        b[0] += i * m_deltaToDistWeight[i];
        b[1] += m_deltaToDistWeight[i];
      }
    }
  }

  if (A[2] == 0)
    return false;

  //-----------------------
  // Compute the delta to dist weight mapping
  if (A[2] == 1)
  {
    // Constant value
    x[0] = 0;
    x[1] = b[1] / A[2];
  }
  else
  {
    // Invert matrix A
    det = A[0] * A[2] - A[1] * A[1];
    NMP_VERIFY(det != 0);
    desc = 1.0f / det;
    iA[0] = desc * A[2];
    iA[1] = -desc * A[1];
    iA[2] = desc * A[0];

    // Solve for x
    x[0] = iA[0] * b[0] + iA[1] * b[1];
    x[1] = iA[1] * b[0] + iA[2] * b[1];
  }

  // Compute the weights
  for (uint32_t i = 1; i < m_numQuantisers; ++i)
  {
    m_deltaToDistWeight[i] = x[0] * i + x[1];

    if (m_deltaToDistWeight[i] < 0.0f)
      m_deltaToDistWeight[i] = 0.0f;
  }
  m_deltaToDistWeightCount = A[2];

  // Check if we need to compute a weight mapping for zero precision
  if (m_distCount[0] == 0)
  {
    // Anything under 4-bits of precision is pretty much useless
    m_deltaToDistWeight[0] = m_deltaToDistWeight[4];
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void CoefSetQSA::setDeltaToDistWeightMap(const CoefSetQSA& coefSet)
{
  NMP_VERIFY(m_numQuantisers == coefSet.m_numQuantisers);
  for (uint32_t i = 0; i < m_numQuantisers; ++i)
    m_deltaToDistWeight[i] = coefSet.m_deltaToDistWeight[i];
  m_deltaToDistWeightCount = coefSet.m_deltaToDistWeightCount;
}

//----------------------------------------------------------------------------------------------------------------------
void CoefSetQSA::setDeltaToDistWeightMap(float w)
{
  for (uint32_t i = 0; i < m_numQuantisers; ++i)
    m_deltaToDistWeight[i] = w;
  m_deltaToDistWeightCount = m_numQuantisers;
}

//----------------------------------------------------------------------------------------------------------------------
void CoefSetQSA::updateDeltaToDistWeightMapAverage(const CoefSetQSA& coefSet)
{
  NMP_VERIFY(m_numQuantisers == coefSet.m_numQuantisers);
  for (uint32_t i = 0; i < m_numQuantisers; ++i)
  {
    m_deltaToDistWeight[i] += coefSet.m_deltaToDistWeight[i];
  }
  m_deltaToDistWeightCount++;
}

//----------------------------------------------------------------------------------------------------------------------
void CoefSetQSA::computeDeltaToDistWeightMapAverage()
{
  NMP_VERIFY(m_deltaToDistWeightCount > 0);
  for (uint32_t i = 0; i < m_numQuantisers; ++i)
  {
    m_deltaToDistWeight[i] /= m_deltaToDistWeightCount;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// QuantisationSetQSA
//----------------------------------------------------------------------------------------------------------------------
QuantisationSetQSA::QuantisationSetQSA()
{
  m_quantiserCoefSetIndex = 0;
  m_coefSetX = NULL;
  m_coefSetY = NULL;
  m_coefSetZ = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
QuantisationSetQSA::~QuantisationSetQSA()
{
}

//----------------------------------------------------------------------------------------------------------------------
void QuantisationSetQSA::computeRateAndDistortion()
{
  NMP_VERIFY(m_coefSetX);
  NMP_VERIFY(m_coefSetY);
  NMP_VERIFY(m_coefSetZ);
  m_coefSetX->computeRateAndDistortion();
  m_coefSetY->computeRateAndDistortion();
  m_coefSetZ->computeRateAndDistortion();
}

//----------------------------------------------------------------------------------------------------------------------
bool QuantisationSetQSA::getHasFullPrec() const
{
  NMP_VERIFY(m_coefSetX);
  NMP_VERIFY(m_coefSetY);
  NMP_VERIFY(m_coefSetZ);
  if (!m_coefSetX->isMaxPrecision())
    return false;
  if (!m_coefSetY->isMaxPrecision())
    return false;
  if (!m_coefSetZ->isMaxPrecision())
    return false;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
// QuantisationInfoQSABuilder
//----------------------------------------------------------------------------------------------------------------------
void QuantisationInfoQSABuilder::init(
  MR::QuantisationInfoQSA& qInfo,
  const NMP::Vector3&      qMin,
  const NMP::Vector3&      qMax)
{
  qInfo.m_qMin[0] = qMin.x;
  qInfo.m_qMin[1] = qMin.y;
  qInfo.m_qMin[2] = qMin.z;

  qInfo.m_qMax[0] = qMax.x;
  qInfo.m_qMax[1] = qMax.y;
  qInfo.m_qMax[2] = qMax.z;
}

//----------------------------------------------------------------------------------------------------------------------
// QuantisationDataQSABuilder
//----------------------------------------------------------------------------------------------------------------------
void QuantisationDataQSABuilder::init(
  MR::QuantisationDataQSA& qData,
  uint32_t meanX,
  uint32_t meanY,
  uint32_t meanZ)
{
  QuantisationDataQSABuilder& qDataBuilder = (QuantisationDataQSABuilder&)qData;

  NMP_VERIFY(meanX <= 0xff);
  NMP_VERIFY(meanY <= 0xff);
  NMP_VERIFY(meanZ <= 0xff);
  qDataBuilder.m_mean[0] = (uint8_t)meanX;
  qDataBuilder.m_mean[1] = (uint8_t)meanY;
  qDataBuilder.m_mean[2] = (uint8_t)meanZ;

  // Precisions
  qDataBuilder.m_prec[0] = 0;
  qDataBuilder.m_prec[1] = 0;
  qDataBuilder.m_prec[2] = 0;

  // Quantisation sets
  qDataBuilder.m_qSet[0] = 0;
  qDataBuilder.m_qSet[1] = 0;
  qDataBuilder.m_qSet[2] = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void QuantisationDataQSABuilder::init(
  MR::QuantisationDataQSA& qData,
  uint32_t meanX,
  uint32_t meanY,
  uint32_t meanZ,
  uint32_t precX,
  uint32_t precY,
  uint32_t precZ,
  uint32_t qSetX,
  uint32_t qSetY,
  uint32_t qSetZ)
{
  QuantisationDataQSABuilder& qDataBuilder = (QuantisationDataQSABuilder&)qData;

  NMP_VERIFY(meanX <= 0xff);
  NMP_VERIFY(meanY <= 0xff);
  NMP_VERIFY(meanZ <= 0xff);
  qDataBuilder.m_mean[0] = (uint8_t)meanX;
  qDataBuilder.m_mean[1] = (uint8_t)meanY;
  qDataBuilder.m_mean[2] = (uint8_t)meanZ;

  // Precisions
  NMP_VERIFY(precX <= 16);
  NMP_VERIFY(precY <= 16);
  NMP_VERIFY(precZ <= 16);
  qDataBuilder.m_prec[0] = (uint8_t)precX;
  qDataBuilder.m_prec[1] = (uint8_t)precY;
  qDataBuilder.m_prec[2] = (uint8_t)precZ;

  // qSets
  NMP_VERIFY(qSetX <= 0xff);
  NMP_VERIFY(qSetY <= 0xff);
  NMP_VERIFY(qSetZ <= 0xff);
  qDataBuilder.m_qSet[0] = (uint8_t)qSetX;
  qDataBuilder.m_qSet[1] = (uint8_t)qSetY;
  qDataBuilder.m_qSet[2] = (uint8_t)qSetZ;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
