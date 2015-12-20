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
#include "NMPlatform/NMPlatform.h"
#include "NMNumerics/NMVector3Utils.h"
#include "NMNumerics/NMUniformQuantisation.h"
#include "assetProcessor/AnimSource/Vector3QuantisationSetBasis.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// Vector3QuantisationSetBasis
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format Vector3QuantisationSetBasis::getMemoryRequirements(
  uint32_t numChannelSets,
  uint32_t numQuantisationSets)
{
  // Header
  NMP::Memory::Format result(sizeof(Vector3QuantisationSetBasis), NMP_NATURAL_TYPE_ALIGNMENT);
  
  // Quantisation set min and max
  NMP::Memory::Format memReqsQSetMinMax(sizeof(NMP::Vector3) * numQuantisationSets, NMP_VECTOR_ALIGNMENT);
  result += memReqsQSetMinMax; // min
  result += memReqsQSetMinMax; // max
  
  // Channel set to quantisation set map
  NMP::Memory::Format memReqsQSetMap(sizeof(uint32_t) * numChannelSets, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsQSetMap; // X
  result += memReqsQSetMap; // Y
  result += memReqsQSetMap; // Z

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
Vector3QuantisationSetBasis* Vector3QuantisationSetBasis::init(
  NMP::Memory::Resource&  resource,
  uint32_t                numChannelSets,
  uint32_t                numQuantisationSets)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(Vector3QuantisationSetBasis), NMP_NATURAL_TYPE_ALIGNMENT);
  Vector3QuantisationSetBasis* result = (Vector3QuantisationSetBasis*) resource.alignAndIncrement(memReqsHdr);
  result->m_numChannelSets = numChannelSets;
  result->m_numQuantisationSets = numQuantisationSets;

  // Quantisation set min and max
  NMP::Memory::Format memReqsQSetMinMax(sizeof(NMP::Vector3) * numQuantisationSets, NMP_VECTOR_ALIGNMENT);
  result->m_qSetMin = (NMP::Vector3*) resource.alignAndIncrement(memReqsQSetMinMax);
  result->m_qSetMax = (NMP::Vector3*) resource.alignAndIncrement(memReqsQSetMinMax);

  // Channel set to quantisation set map
  NMP::Memory::Format memReqsQSetMap(sizeof(uint32_t) * numChannelSets, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_chanSetToQSetMapX = (uint32_t*) resource.alignAndIncrement(memReqsQSetMap);
  result->m_chanSetToQSetMapY = (uint32_t*) resource.alignAndIncrement(memReqsQSetMap);
  result->m_chanSetToQSetMapZ = (uint32_t*) resource.alignAndIncrement(memReqsQSetMap);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Vector3QuantisationSetBasis::getNumChannelSets() const
{
  return m_numChannelSets;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Vector3QuantisationSetBasis::getNumQuantisationSets() const
{
  return m_numQuantisationSets;
}

//----------------------------------------------------------------------------------------------------------------------
const NMP::Vector3* Vector3QuantisationSetBasis::getQSetMin() const
{
  return m_qSetMin;
}

//----------------------------------------------------------------------------------------------------------------------
const NMP::Vector3* Vector3QuantisationSetBasis::getQSetMax() const
{
  return m_qSetMax;
}

//----------------------------------------------------------------------------------------------------------------------
const uint32_t* Vector3QuantisationSetBasis::getChanSetToQSetMapX() const
{
  return m_chanSetToQSetMapX;
}

//----------------------------------------------------------------------------------------------------------------------
const uint32_t* Vector3QuantisationSetBasis::getChanSetToQSetMapY() const
{
  return m_chanSetToQSetMapY;
}

//----------------------------------------------------------------------------------------------------------------------
const uint32_t* Vector3QuantisationSetBasis::getChanSetToQSetMapZ() const
{
  return m_chanSetToQSetMapZ;
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3QuantisationSetBasis::computeBasisVectors(
  uint32_t numEntries,
  const NMP::Vector3* const* keysTable,
  const uint32_t* keysTableCounts)
{
  NMP_VERIFY(numEntries > 0);
  NMP_VERIFY(keysTable);
  NMP_VERIFY(keysTableCounts);
  NMP_VERIFY(m_numQuantisationSets <= numEntries);

  // Setup quantisation set compounders
  NMP::UniformQuantisationInfoCompounder qSetCompounderX(numEntries);
  NMP::UniformQuantisationInfoCompounder qSetCompounderY(numEntries);
  NMP::UniformQuantisationInfoCompounder qSetCompounderZ(numEntries);
  NMP::Vector3 qMin, qMax;

  // Compute the quantisation set bounds
  for (uint32_t iSet = 0; iSet < numEntries; ++iSet)
  {
    const NMP::Vector3* qSetKeys = keysTable[iSet];
    NMP_VERIFY(qSetKeys);
    uint32_t numKeyFrames = keysTableCounts[iSet];

    NMP::vBounds(numKeyFrames, qSetKeys, qMin, qMax);

    qSetCompounderX.setQuantisationInfo(iSet, qMin.x, qMax.x);
    qSetCompounderY.setQuantisationInfo(iSet, qMin.y, qMax.y);
    qSetCompounderZ.setQuantisationInfo(iSet, qMin.z, qMax.z);
  }

  qSetCompounderX.compoundQuantisationInfo(m_numQuantisationSets);
  qSetCompounderY.compoundQuantisationInfo(m_numQuantisationSets);
  qSetCompounderZ.compoundQuantisationInfo(m_numQuantisationSets);

  //-----------------------
  // Quantisation range info
  for (uint32_t qSetIndex = 0; qSetIndex < m_numQuantisationSets; ++qSetIndex)
  {
    qSetCompounderX.getQuantisationInfoCompounded(qSetIndex, qMin.x, qMax.x);
    qSetCompounderY.getQuantisationInfoCompounded(qSetIndex, qMin.y, qMax.y);
    qSetCompounderZ.getQuantisationInfoCompounded(qSetIndex, qMin.z, qMax.z);
    m_qSetMin[qSetIndex] = qMin;
    m_qSetMax[qSetIndex] = qMax;
  }

  //-----------------------
  // Channel set to quantisation set map
  for (uint32_t iChan = 0; iChan < numEntries; ++iChan)
  {
    uint32_t qSetX = qSetCompounderX.getMapFromInputToCompounded(iChan);
    uint32_t qSetY = qSetCompounderY.getMapFromInputToCompounded(iChan);
    uint32_t qSetZ = qSetCompounderZ.getMapFromInputToCompounded(iChan);
    m_chanSetToQSetMapX[iChan] = qSetX;
    m_chanSetToQSetMapY[iChan] = qSetY;
    m_chanSetToQSetMapZ[iChan] = qSetZ;
  }
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
