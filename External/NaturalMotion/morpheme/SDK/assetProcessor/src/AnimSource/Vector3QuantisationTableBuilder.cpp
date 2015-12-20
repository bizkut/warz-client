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
#include <limits>
#include "NMPlatform/NMMemory.h"
#include "NMNumerics/NMUniformQuantisation.h"
#include "assetProcessor/AnimSource/Vector3QuantisationTableBuilder.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// Vector3QuantisationTable
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format Vector3QuantisationTable::getMemoryRequirements(uint32_t numChannels)
{
  // Header
  NMP::Memory::Format memReqs(sizeof(Vector3QuantisationTable), NMP_NATURAL_TYPE_ALIGNMENT);
  
  // Precisions
  NMP::Memory::Format memReqsPrec(sizeof(uint32_t) * numChannels, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqs += (memReqsPrec * 3); // X, Y, Z
  
  // Bounds info
  NMP::Memory::Format memReqsBounds(sizeof(NMP::Vector3) * numChannels, NMP_VECTOR_ALIGNMENT);
  memReqs += (memReqsBounds * 2); // qMin, qMax

  return memReqs;
}

//----------------------------------------------------------------------------------------------------------------------
Vector3QuantisationTable* Vector3QuantisationTable::init(
  NMP::Memory::Resource&  resource,
  uint32_t                numChannels)
{
  // Header
  NMP::Memory::Format memReqs(sizeof(Vector3QuantisationTable), NMP_NATURAL_TYPE_ALIGNMENT);
  Vector3QuantisationTable* result = (Vector3QuantisationTable*) resource.alignAndIncrement(memReqs);
  NMP_VERIFY(result);
  result->m_numChannels = numChannels;
  
  // Precisions
  NMP::Memory::Format memReqsPrec(sizeof(uint32_t) * numChannels, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_precisionX = (uint32_t*) resource.alignAndIncrement(memReqsPrec);
  result->m_precisionY = (uint32_t*) resource.alignAndIncrement(memReqsPrec);
  result->m_precisionZ = (uint32_t*) resource.alignAndIncrement(memReqsPrec);

  // Bounds info
  NMP::Memory::Format memReqsBounds(sizeof(NMP::Vector3) * numChannels, NMP_VECTOR_ALIGNMENT);
  result->m_qMin = (NMP::Vector3*) resource.alignAndIncrement(memReqsBounds);
  result->m_qMax = (NMP::Vector3*) resource.alignAndIncrement(memReqsBounds);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3QuantisationTable::copy(const Vector3QuantisationTable& src)
{
  NMP_VERIFY(m_numChannels == src.m_numChannels);

  if (&src != this)
  {
    for (uint32_t i = 0; i < m_numChannels; ++i)
    {
      // Precisions
      m_precisionX[i] = src.m_precisionX[i];
      m_precisionY[i] = src.m_precisionY[i];
      m_precisionZ[i] = src.m_precisionZ[i];

      // Bounds info
      m_qMin[i] = src.m_qMin[i];
      m_qMax[i] = src.m_qMax[i];
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
Vector3QuantisationTable& Vector3QuantisationTable::operator=(const Vector3QuantisationTable& src)
{
  copy(src);
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3QuantisationTable::setQuantisationBounds(const Vector3Table* table)
{
  NMP_VERIFY(table);
  NMP_VERIFY(m_numChannels == table->getNumChannels());
  NMP_VERIFY(m_numChannels > 0);
  uint32_t numFrames = table->getNumKeyFrames();

  for (uint32_t i = 0; i < m_numChannels; ++i)
  {
    NMP::vBounds(numFrames, table->getChannel(i), m_qMin[i], m_qMax[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3QuantisationTable::setQuantisationBounds(const RotVecTable* table)
{
  NMP_VERIFY(table);
  NMP_VERIFY(m_numChannels == table->getNumChannels());
  NMP_VERIFY(m_numChannels > 0);
  uint32_t numFrames = table->getNumKeyFrames();

  for (uint32_t i = 0; i < m_numChannels; ++i)
  {
    setQuantisationBounds(i, numFrames, table->getChannel(i));
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3QuantisationTable::setQuantisationBounds(uint32_t channel, const NMP::Vector3& key)
{
  NMP_VERIFY(channel < m_numChannels);
  m_qMin[channel] = key;
  m_qMax[channel] = key;
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3QuantisationTable::setQuantisationBounds(uint32_t channel, uint32_t numFrames, const NMP::Vector3* keys)
{
  NMP_VERIFY(channel < m_numChannels);
  NMP::vBounds(numFrames, keys, m_qMin[channel], m_qMax[channel]);
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3QuantisationTable::setQuantisationBounds(
  uint32_t            channel,
  const NMP::Vector3& qMin,
  const NMP::Vector3& qMax)
{
  NMP_VERIFY(channel < m_numChannels);
  m_qMin[channel] = qMin;
  m_qMax[channel] = qMax;
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3QuantisationTable::zeroQuantisationBounds(uint32_t channel)
{
  NMP_VERIFY(channel < m_numChannels);
  m_qMin[channel].setToZero();
  m_qMax[channel].setToZero();
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3QuantisationTable::invalidQuantisationBounds(NMP::Vector3& qMin, NMP::Vector3& qMax) const
{
  qMin.x = std::numeric_limits<float>::infinity();
  qMin.y = std::numeric_limits<float>::infinity();
  qMin.z = std::numeric_limits<float>::infinity();
  qMax.x = -std::numeric_limits<float>::infinity();
  qMax.y = -std::numeric_limits<float>::infinity();
  qMax.z = -std::numeric_limits<float>::infinity();
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3QuantisationTable::getQuantisationBounds(
  uint32_t      channel,
  NMP::Vector3& qMin,
  NMP::Vector3& qMax) const
{
  NMP_VERIFY(channel < m_numChannels);
  qMin = m_qMin[channel];
  qMax = m_qMax[channel];
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3QuantisationTable::updateQuantisationBounds(
  uint32_t      channel,
  NMP::Vector3& qMin,
  NMP::Vector3& qMax) const
{
  NMP_VERIFY(channel < m_numChannels);
  qMin.x = NMP::minimum(qMin.x, m_qMin[channel].x);
  qMin.y = NMP::minimum(qMin.y, m_qMin[channel].y);
  qMin.z = NMP::minimum(qMin.z, m_qMin[channel].z);
  qMax.x = NMP::maximum(qMax.x, m_qMax[channel].x);
  qMax.y = NMP::maximum(qMax.y, m_qMax[channel].y);
  qMax.z = NMP::maximum(qMax.z, m_qMax[channel].z);
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3QuantisationTable::updateQuantisationBoundsX(uint32_t channel, float& qMinX, float& qMaxX) const
{
  NMP_VERIFY(channel < m_numChannels);
  qMinX = NMP::minimum(qMinX, m_qMin[channel].x);
  qMaxX = NMP::maximum(qMaxX, m_qMax[channel].x);
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3QuantisationTable::updateQuantisationBoundsY(uint32_t channel, float& qMinY, float& qMaxY) const
{
  NMP_VERIFY(channel < m_numChannels);
  qMinY = NMP::minimum(qMinY, m_qMin[channel].y);
  qMaxY = NMP::maximum(qMaxY, m_qMax[channel].y);
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3QuantisationTable::updateQuantisationBoundsZ(uint32_t channel, float& qMinZ, float& qMaxZ) const
{
  NMP_VERIFY(channel < m_numChannels);
  qMinZ = NMP::minimum(qMinZ, m_qMin[channel].z);
  qMaxZ = NMP::maximum(qMaxZ, m_qMax[channel].z);
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3QuantisationTable::updateQuantisationBounds(const Vector3Table* table)
{
  NMP_VERIFY(table);
  NMP_VERIFY(m_numChannels == table->getNumChannels());
  NMP_VERIFY(m_numChannels > 0);
  uint32_t numFrames = table->getNumKeyFrames();

  NMP::Vector3 qMin, qMax;
  for (uint32_t i = 0; i < m_numChannels; ++i)
  {
    NMP::vBounds(numFrames, table->getChannel(i), qMin, qMax);
    m_qMin[i].x = NMP::minimum(qMin.x, m_qMin[i].x);
    m_qMin[i].y = NMP::minimum(qMin.y, m_qMin[i].y);
    m_qMin[i].z = NMP::minimum(qMin.z, m_qMin[i].z);
    m_qMax[i].x = NMP::maximum(qMax.x, m_qMax[i].x);
    m_qMax[i].y = NMP::maximum(qMax.y, m_qMax[i].y);
    m_qMax[i].z = NMP::maximum(qMax.z, m_qMax[i].z);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3QuantisationTable::updateQuantisationBounds(uint32_t channel, const NMP::Vector3& key)
{
  NMP_VERIFY(channel < m_numChannels);
  m_qMin[channel].x = NMP::minimum(key.x, m_qMin[channel].x);
  m_qMin[channel].y = NMP::minimum(key.y, m_qMin[channel].y);
  m_qMin[channel].z = NMP::minimum(key.z, m_qMin[channel].z);
  m_qMax[channel].x = NMP::maximum(key.x, m_qMax[channel].x);
  m_qMax[channel].y = NMP::maximum(key.y, m_qMax[channel].y);
  m_qMax[channel].z = NMP::maximum(key.z, m_qMax[channel].z);
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3QuantisationTable::updateQuantisationBounds(uint32_t channel, uint32_t numFrames, const NMP::Vector3* keys)
{
  NMP_VERIFY(channel < m_numChannels);
  NMP::Vector3 qMin, qMax;
  NMP::vBounds(numFrames, keys, qMin, qMax);
  m_qMin[channel].x = NMP::minimum(qMin.x, m_qMin[channel].x);
  m_qMin[channel].y = NMP::minimum(qMin.y, m_qMin[channel].y);
  m_qMin[channel].z = NMP::minimum(qMin.z, m_qMin[channel].z);
  m_qMax[channel].x = NMP::maximum(qMax.x, m_qMax[channel].x);
  m_qMax[channel].y = NMP::maximum(qMax.y, m_qMax[channel].y);
  m_qMax[channel].z = NMP::maximum(qMax.z, m_qMax[channel].z);
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3QuantisationTable::getGlobalQuantisationBounds(NMP::Vector3& qMin, NMP::Vector3& qMax) const
{
  qMin = m_qMin[0];
  qMax = m_qMax[0];
  for (uint32_t i = 1; i < m_numChannels; ++i)
  {
    qMin.x = NMP::minimum(qMin.x, m_qMin[i].x);
    qMin.y = NMP::minimum(qMin.y, m_qMin[i].y);
    qMin.z = NMP::minimum(qMin.z, m_qMin[i].z);
    qMax.x = NMP::maximum(qMax.x, m_qMax[i].x);
    qMax.y = NMP::maximum(qMax.y, m_qMax[i].y);
    qMax.z = NMP::maximum(qMax.z, m_qMax[i].z);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3QuantisationTable::setGlobalQuantisationBounds(const NMP::Vector3& qMin, const NMP::Vector3& qMax)
{
  for (uint32_t i = 0; i < m_numChannels; ++i)
  {
    m_qMin[i] = qMin;
    m_qMax[i] = qMax;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3QuantisationTable::setGlobalQuantisationBounds(const float* qMin, const float* qMax)
{
  NMP_VERIFY(qMin);
  NMP_VERIFY(qMax);

  for (uint32_t i = 0; i < m_numChannels; ++i)
  {
    m_qMin[i].set(qMin[0], qMin[1], qMin[2]);
    m_qMax[i].set(qMax[0], qMax[1], qMax[2]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3QuantisationTable::quantise(
  uint32_t            channel,
  const NMP::Vector3& v,
  uint32_t&           qx,
  uint32_t&           qy,
  uint32_t&           qz) const
{
  NMP_VERIFY(channel < m_numChannels);

  qx = NMP::UniformQuantisation::quantise(
         m_qMin[channel].x,
         m_qMax[channel].x,
         m_precisionX[channel],
         v.x);

  qy = NMP::UniformQuantisation::quantise(
         m_qMin[channel].y,
         m_qMax[channel].y,
         m_precisionY[channel],
         v.y);

  qz = NMP::UniformQuantisation::quantise(
         m_qMin[channel].z,
         m_qMax[channel].z,
         m_precisionZ[channel],
         v.z);
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3QuantisationTable::dequantise(
  uint32_t      channel,
  NMP::Vector3& v,
  uint32_t      qx,
  uint32_t      qy,
  uint32_t      qz) const
{
  NMP_VERIFY(channel < m_numChannels);

  v.x = NMP::UniformQuantisation::dequantise(
          m_qMin[channel].x,
          m_qMax[channel].x,
          m_precisionX[channel],
          qx);

  v.y = NMP::UniformQuantisation::dequantise(
          m_qMin[channel].y,
          m_qMax[channel].y,
          m_precisionY[channel],
          qy);

  v.z = NMP::UniformQuantisation::dequantise(
          m_qMin[channel].z,
          m_qMax[channel].z,
          m_precisionZ[channel],
          qz);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Vector3QuantisationTable::getPrecisionsBytes() const
{
  uint32_t precBits = getPrecisions();
  uint32_t precBytes = (precBits + 0x07) >> 3;
  return precBytes;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Vector3QuantisationTable::getPrecisions() const
{
  uint32_t sum = 0;
  for (uint32_t i = 0; i < m_numChannels; ++i)
  {
    sum += m_precisionX[i];
    sum += m_precisionY[i];
    sum += m_precisionZ[i];
  }
  return sum;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Vector3QuantisationTable::getPrecisionsX() const
{
  uint32_t sum = 0;
  for (uint32_t i = 0; i < m_numChannels; ++i)
  {
    sum += m_precisionX[i];
  }
  return sum;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Vector3QuantisationTable::getPrecisionsY() const
{
  uint32_t sum = 0;
  for (uint32_t i = 0; i < m_numChannels; ++i)
  {
    sum += m_precisionY[i];
  }
  return sum;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Vector3QuantisationTable::getPrecisionsZ() const
{
  uint32_t sum = 0;
  for (uint32_t i = 0; i < m_numChannels; ++i)
  {
    sum += m_precisionZ[i];
  }
  return sum;
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3QuantisationTable::setPrecisions(uint32_t prec)
{
  NMP_VERIFY(prec < 32);
  for (uint32_t i = 0; i < m_numChannels; ++i)
  {
    m_precisionX[i] = prec;
    m_precisionY[i] = prec;
    m_precisionZ[i] = prec;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3QuantisationTable::setPrecisionsX(uint32_t prec)
{
  NMP_VERIFY(prec < 32);
  for (uint32_t i = 0; i < m_numChannels; ++i)
  {
    m_precisionX[i] = prec;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3QuantisationTable::setPrecisionsY(uint32_t prec)
{
  NMP_VERIFY(prec < 32);
  for (uint32_t i = 0; i < m_numChannels; ++i)
  {
    m_precisionY[i] = prec;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3QuantisationTable::setPrecisionsZ(uint32_t prec)
{
  NMP_VERIFY(prec < 32);
  for (uint32_t i = 0; i < m_numChannels; ++i)
  {
    m_precisionZ[i] = prec;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 Vector3QuantisationTable::getQuantisationScales(uint32_t channel) const
{
  NMP_VERIFY(channel < m_numChannels);

  NMP::Vector3 scales;
  scales.x = NMP::UniformQuantisation::stepSize(m_qMin[channel].x, m_qMax[channel].x, m_precisionX[channel]);
  scales.y = NMP::UniformQuantisation::stepSize(m_qMin[channel].y, m_qMax[channel].y, m_precisionY[channel]);
  scales.z = NMP::UniformQuantisation::stepSize(m_qMin[channel].z, m_qMax[channel].z, m_precisionZ[channel]);
  return scales;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 Vector3QuantisationTable::getQuantisationOffsets(uint32_t channel) const
{
  NMP_VERIFY(channel < m_numChannels);
  return m_qMin[channel];
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 Vector3QuantisationTable::getQuantisationDiffs(uint32_t channel) const
{
  NMP_VERIFY(channel < m_numChannels);
  NMP::Vector3 result = m_qMax[channel] - m_qMin[channel];
  return result;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
