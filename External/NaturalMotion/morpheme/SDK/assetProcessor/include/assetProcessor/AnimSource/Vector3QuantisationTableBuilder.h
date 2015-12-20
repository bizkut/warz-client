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
#ifndef VECTOR3_QUANTISATION_TABLE_BUILDER_H
#define VECTOR3_QUANTISATION_TABLE_BUILDER_H

//----------------------------------------------------------------------------------------------------------------------
#include "assetProcessor/AnimSource/Vector3TableBuilder.h"
#include "assetProcessor/AnimSource/RotVecTableBuilder.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::Vector3QuantisationTable
/// \brief A table of quantisation information for a Vector3Table.
/// \ingroup UncompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class Vector3QuantisationTable
{
public:
  /// \brief Calculate the memory required to create a Vector3QuantisationTable from the supplied parameters.
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numChannels              ///< IN: The number of channels in the table
  );

  /// \brief Create a new Vector3QuantisationTable in the supplied memory.
  static Vector3QuantisationTable* init(
    NMP::Memory::Resource&  resource,     ///< IN: Resource description of where to build the table
    uint32_t                numChannels   ///< IN: The number of channels in the table
  );

public:
  Vector3QuantisationTable() {}
  ~Vector3QuantisationTable() {}

  /// \brief Function to set the minimum and maximum quantisation bound
  /// ranges for each channel.
  void setQuantisationBounds(const Vector3Table* table);
  void setQuantisationBounds(const RotVecTable* table);
  void setQuantisationBounds(uint32_t channel, const NMP::Vector3& key);
  void setQuantisationBounds(uint32_t channel, uint32_t numFrames, const NMP::Vector3* keys);
  void setQuantisationBounds(uint32_t channel, const NMP::Vector3& qMin, const NMP::Vector3& qMax);
  void zeroQuantisationBounds(uint32_t channel);

  /// \brief Function to get the quantisation bounds for a specified channel.
  void invalidQuantisationBounds(NMP::Vector3& qMin, NMP::Vector3& qMax) const;
  void getQuantisationBounds(uint32_t channel, NMP::Vector3& qMin, NMP::Vector3& qMax) const;
  void updateQuantisationBounds(uint32_t channel, NMP::Vector3& qMin, NMP::Vector3& qMax) const;
  void updateQuantisationBoundsX(uint32_t channel, float& qMinX, float& qMaxX) const;
  void updateQuantisationBoundsY(uint32_t channel, float& qMinY, float& qMaxY) const;
  void updateQuantisationBoundsZ(uint32_t channel, float& qMinZ, float& qMaxZ) const;

  /// \brief Function to update the minimum and maximum quantisation bound
  /// ranges for each channel with the channel data of another table.
  void updateQuantisationBounds(const Vector3Table* table);
  void updateQuantisationBounds(uint32_t channel, const NMP::Vector3& key);
  void updateQuantisationBounds(uint32_t channel, uint32_t numFrames, const NMP::Vector3* keys);

  /// \brief Function to compute the global quantisation bound info over all channels.
  void getGlobalQuantisationBounds(NMP::Vector3& qMin, NMP::Vector3& qMax) const;
  void setGlobalQuantisationBounds(const NMP::Vector3& qMin, const NMP::Vector3& qMax);
  void setGlobalQuantisationBounds(const float* qMin, const float* qMax);
  NM_INLINE void setGlobalQuantisationBounds();

  /// \brief Function for quantising a Vector3 using the specified channel quantisation info.
  void quantise(
    uint32_t            channel,
    const NMP::Vector3& v,
    uint32_t&           qx,
    uint32_t&           qy,
    uint32_t&           qz) const;

  /// \brief Function for dequantising a Vector3 using the specified channel quantisation info.
  void dequantise(
    uint32_t      channel,
    NMP::Vector3& v,
    uint32_t      qx,
    uint32_t      qy,
    uint32_t      qz) const;

  // Accessors
  NM_INLINE uint32_t getNumChannels() const;

  // Bit allocation precision
  NM_INLINE uint32_t* getPrecisionX() const;
  NM_INLINE uint32_t* getPrecisionY() const;
  NM_INLINE uint32_t* getPrecisionZ() const;

  NM_INLINE uint32_t getPrecisionX(uint32_t channel) const;
  NM_INLINE uint32_t getPrecisionY(uint32_t channel) const;
  NM_INLINE uint32_t getPrecisionZ(uint32_t channel) const;

  NM_INLINE void setPrecision(uint32_t channel, uint32_t prec);
  NM_INLINE void setPrecision(uint32_t channel, uint32_t precX, uint32_t precY, uint32_t precZ);
  NM_INLINE void setPrecisionX(uint32_t channel, uint32_t prec);
  NM_INLINE void setPrecisionY(uint32_t channel, uint32_t prec);
  NM_INLINE void setPrecisionZ(uint32_t channel, uint32_t prec);

  uint32_t getPrecisionsBytes() const;
  uint32_t getPrecisions() const;
  uint32_t getPrecisionsX() const;
  uint32_t getPrecisionsY() const;
  uint32_t getPrecisionsZ() const;

  void setPrecisions(uint32_t prec);
  void setPrecisionsX(uint32_t prec);
  void setPrecisionsY(uint32_t prec);
  void setPrecisionsZ(uint32_t prec);

  // Quantisation min and max information
  NM_INLINE NMP::Vector3* getQuantisationMins() const;
  NM_INLINE NMP::Vector3* getQuantisationMaxs() const;
  NM_INLINE NMP::Vector3 getQuantisationMins(uint32_t channel) const;
  NM_INLINE NMP::Vector3 getQuantisationMaxs(uint32_t channel) const;

  // Quantisation scale and offset information
  NMP::Vector3 getQuantisationScales(uint32_t channel) const;
  NMP::Vector3 getQuantisationOffsets(uint32_t channel) const;
  NMP::Vector3 getQuantisationDiffs(uint32_t channel) const;

  // Assignment
  void copy(const Vector3QuantisationTable& src);
  Vector3QuantisationTable& operator=(const Vector3QuantisationTable& src);

protected:
  uint32_t        m_numChannels;

  uint32_t*       m_precisionX;
  uint32_t*       m_precisionY;
  uint32_t*       m_precisionZ;

  NMP::Vector3*   m_qMin;
  NMP::Vector3*   m_qMax;
};

//----------------------------------------------------------------------------------------------------------------------
// Vector3QuantisationTable Inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Vector3QuantisationTable::setGlobalQuantisationBounds()
{
  NMP::Vector3 qMin, qMax;
  getGlobalQuantisationBounds(qMin, qMax);
  setGlobalQuantisationBounds(qMin, qMax);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t Vector3QuantisationTable::getNumChannels() const
{
  return m_numChannels;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t* Vector3QuantisationTable::getPrecisionX() const
{
  return m_precisionX;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t* Vector3QuantisationTable::getPrecisionY() const
{
  return m_precisionY;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t* Vector3QuantisationTable::getPrecisionZ() const
{
  return m_precisionZ;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t Vector3QuantisationTable::getPrecisionX(uint32_t channel) const
{
  NMP_VERIFY(channel < m_numChannels);
  return m_precisionX[channel];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t Vector3QuantisationTable::getPrecisionY(uint32_t channel) const
{
  NMP_VERIFY(channel < m_numChannels);
  return m_precisionY[channel];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t Vector3QuantisationTable::getPrecisionZ(uint32_t channel) const
{
  NMP_VERIFY(channel < m_numChannels);
  return m_precisionZ[channel];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Vector3QuantisationTable::setPrecision(uint32_t channel, uint32_t prec)
{
  NMP_VERIFY(channel < m_numChannels);
  m_precisionX[channel] = prec;
  m_precisionY[channel] = prec;
  m_precisionZ[channel] = prec;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Vector3QuantisationTable::setPrecision(
  uint32_t channel,
  uint32_t precX,
  uint32_t precY,
  uint32_t precZ)
{
  NMP_VERIFY(channel < m_numChannels);
  m_precisionX[channel] = precX;
  m_precisionY[channel] = precY;
  m_precisionZ[channel] = precZ;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Vector3QuantisationTable::setPrecisionX(uint32_t channel, uint32_t prec)
{
  NMP_VERIFY(channel < m_numChannels);
  m_precisionX[channel] = prec;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Vector3QuantisationTable::setPrecisionY(uint32_t channel, uint32_t prec)
{
  NMP_VERIFY(channel < m_numChannels);
  m_precisionY[channel] = prec;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Vector3QuantisationTable::setPrecisionZ(uint32_t channel, uint32_t prec)
{
  NMP_VERIFY(channel < m_numChannels);
  m_precisionZ[channel] = prec;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Vector3* Vector3QuantisationTable::getQuantisationMins() const
{
  return m_qMin;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Vector3* Vector3QuantisationTable::getQuantisationMaxs() const
{
  return m_qMax;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Vector3 Vector3QuantisationTable::getQuantisationMins(uint32_t channel) const
{
  NMP_VERIFY(channel < m_numChannels);
  return m_qMin[channel];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Vector3 Vector3QuantisationTable::getQuantisationMaxs(uint32_t channel) const
{
  NMP_VERIFY(channel < m_numChannels);
  return m_qMax[channel];
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // VECTOR3_QUANTISATION_TABLE_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
