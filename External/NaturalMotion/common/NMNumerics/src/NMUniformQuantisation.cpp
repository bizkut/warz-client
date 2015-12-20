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
#include <cmath>
#include <limits>
#include <list>
#include "NMPlatform/NMMathUtils.h"
#include "NMPlatform/NMMatrix.h"
#include "NMNumerics/NMUniformQuantisation.h"
#include "NMNumerics/NMMoments2D.h"

//----------------------------------------------------------------------------------------------------------------------
// Disable warnings constant conditionals
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning(disable : 4127)
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// UniformQuantisationInfo
//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisationInfo::clear()
{
  m_precision = 0;
  m_qMin = 0.0f;
  m_qMax = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
// UniformQuantisationInfoSet
//----------------------------------------------------------------------------------------------------------------------
UniformQuantisationInfoSet::UniformQuantisationInfoSet(uint32_t numChannelIndices)
{
  NMP_VERIFY(numChannelIndices > 0);
  m_channelIndices = new uint32_t[numChannelIndices];
  m_numChannelIndices = numChannelIndices;
}

//----------------------------------------------------------------------------------------------------------------------
UniformQuantisationInfoSet::UniformQuantisationInfoSet(
  const UniformQuantisationInfoSet* qSetA,
  const UniformQuantisationInfoSet* qSetB)
{
  // Allocate the memory for the channel indices
  NMP_VERIFY(qSetA);
  NMP_VERIFY(qSetB);
  uint32_t numChannelIndices = qSetA->m_numChannelIndices + qSetB->m_numChannelIndices;
  NMP_VERIFY(numChannelIndices > 1);
  m_channelIndices = new uint32_t[numChannelIndices];
  m_numChannelIndices = numChannelIndices;

  // Concatenate the channel indices
  for (uint32_t i = 0; i < qSetA->m_numChannelIndices; ++i)
  {
    m_channelIndices[i] = qSetA->m_channelIndices[i];
  }
  for (uint32_t i = 0; i < qSetB->m_numChannelIndices; ++i)
  {
    m_channelIndices[i + qSetA->m_numChannelIndices] = qSetB->m_channelIndices[i];
  }

  // Merge the [qMin, qMax] bracket
  m_qMin = NMP::minimum(qSetA->m_qMin, qSetB->m_qMin);
  m_qMax = NMP::maximum(qSetA->m_qMax, qSetB->m_qMax);
}

//----------------------------------------------------------------------------------------------------------------------
UniformQuantisationInfoSet::~UniformQuantisationInfoSet()
{
  delete[] m_channelIndices;
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisationInfoSet::computeDistribution(
  uint32_t NMP_USED_FOR_VERIFY(numCoefSets),
  const UniformQuantisationInfo* quantisationInfo)
{
  NMP_VERIFY(numCoefSets > 0);
  NMP_VERIFY(quantisationInfo);

  // Compute the covariance of the [qMin, qMax] point distribution
  NMP::Moments2D<float> M;
  for (uint32_t i = 0; i < m_numChannelIndices; ++i)
  {
    uint32_t channelIndex = m_channelIndices[i];
    NMP_VERIFY(channelIndex < numCoefSets);
    M.add(quantisationInfo[channelIndex].m_qMin, quantisationInfo[channelIndex].m_qMax);
  }

  // Perform PCA on the distribution
  float qVar[2];
  float qVc[2];
  M.principalComponents(
    m_qBar, // The mean of the distribution
    qVar,   // The variance along the principal directions of the distribution
    m_qVp,  // The principal direction with most variance
    qVc);   // The complementary direction with least variance

  // Store the variance of the principal direction
  m_qVar = qVar[0];
}

//----------------------------------------------------------------------------------------------------------------------
bool UniformQuantisationInfoSet::splitDistribution(
  uint32_t NMP_USED_FOR_VERIFY(numCoefSets),
  const UniformQuantisationInfo* quantisationInfo,
  uint32_t* channelIndexBuffer,
  UniformQuantisationInfoSet*& qSubSetA,
  UniformQuantisationInfoSet*& qSubSetB) const
{
  NMP_VERIFY(numCoefSets > 0);
  NMP_VERIFY(quantisationInfo);
  uint32_t endIndex = m_numChannelIndices - 1;
  uint32_t countA = 0;
  uint32_t countB = 0;

  // Determine which sets to assign the channels to
  for (uint32_t i = 0; i < m_numChannelIndices; ++i)
  {
    uint32_t channelIndex = m_channelIndices[i];
    NMP_VERIFY(channelIndex < numCoefSets);
    float x[2];
    x[0] = quantisationInfo[channelIndex].m_qMin - m_qBar[0];
    x[1] = quantisationInfo[channelIndex].m_qMax - m_qBar[1];

    // Compute the component of X in the most variant principal direction
    float val = x[0] * m_qVp[0] + x[1] * m_qVp[1];
    if (val >= 0.0f)
    {
      // Insert entries from the start of the temporary buffer
      channelIndexBuffer[countA] = channelIndex;
      countA++;
    }
    else
    {
      // Insert entries from the end of the temporary buffer
      channelIndexBuffer[endIndex - countB] = channelIndex;
      countB++;
    }
  }

  // Check if all the samples are coincident
  if (countA == 0 || countB == 0)
  {
    qSubSetA = NULL;
    qSubSetB = NULL;
    return false;
  }

  // Create sub-set A
  qSubSetA = new UniformQuantisationInfoSet(countA);
  for (uint32_t i = 0; i < countA; ++i)
  {
    qSubSetA->m_channelIndices[i] = channelIndexBuffer[i];
  }

  // Create sub-set B
  qSubSetB = new UniformQuantisationInfoSet(countB);
  for (uint32_t i = 0; i < countB; ++i)
  {
    qSubSetB->m_channelIndices[i] = channelIndexBuffer[endIndex - i];
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisationInfoSet::redistribute(
  UniformQuantisationInfoSet*& qSubSetA,
  UniformQuantisationInfoSet*& qSubSetB) const
{
  NMP_VERIFY(m_numChannelIndices >= 2);

  uint32_t numChannelsA = m_numChannelIndices >> 1;
  uint32_t numChannelsB = m_numChannelIndices - numChannelsA;
  UniformQuantisationInfoSet* qSetA = new UniformQuantisationInfoSet(numChannelsA);
  UniformQuantisationInfoSet* qSetB = new UniformQuantisationInfoSet(numChannelsB);

  // SetA
  qSetA->m_qMin = m_qMin;
  qSetA->m_qMax = m_qMax;
  for (uint32_t i = 0; i < numChannelsA; ++i)
  {
    qSetA->m_channelIndices[i] = m_channelIndices[i];
  }
  qSubSetA = qSetA;

  // SetB
  qSetB->m_qMin = m_qMin;
  qSetB->m_qMax = m_qMax;
  for (uint32_t i = 0; i < numChannelsB; ++i)
  {
    qSetB->m_channelIndices[i] = m_channelIndices[i + numChannelsA];
  }
  qSubSetB = qSetB;
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisationInfoSet::computeQMinMax(
  uint32_t NMP_USED_FOR_VERIFY(numCoefSets),
  const UniformQuantisationInfo* quantisationInfo)
{
  NMP_VERIFY(numCoefSets > 0);
  NMP_VERIFY(quantisationInfo);

  // First entry
  uint32_t channelIndex = m_channelIndices[0];
  NMP_VERIFY(channelIndex < numCoefSets);
  m_qMin = quantisationInfo[channelIndex].m_qMin;
  m_qMax = quantisationInfo[channelIndex].m_qMax;

  // Remaining entries
  for (uint32_t i = 1; i < m_numChannelIndices; ++i)
  {
    channelIndex = m_channelIndices[i];
    NMP_VERIFY(channelIndex < numCoefSets);
    m_qMin = NMP::minimum(quantisationInfo[channelIndex].m_qMin, m_qMin);
    m_qMax = NMP::maximum(quantisationInfo[channelIndex].m_qMax, m_qMax);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// UniformQuantisationInfoCompounder
//----------------------------------------------------------------------------------------------------------------------
UniformQuantisationInfoCompounder::UniformQuantisationInfoCompounder(uint32_t numCoefSets)
{
  NMP_VERIFY(numCoefSets > 0);
  m_numCoefSets = numCoefSets;
  m_quantisationInfo = new UniformQuantisationInfo[numCoefSets];
  m_mapFromInputToCompounded = new uint32_t[numCoefSets];

  m_numCoefSetsCompounded = 0;
  m_quantisationInfoCompounded = new UniformQuantisationInfo[numCoefSets];
}

//----------------------------------------------------------------------------------------------------------------------
UniformQuantisationInfoCompounder::~UniformQuantisationInfoCompounder()
{
  delete[] m_quantisationInfo;
  delete[] m_mapFromInputToCompounded;
  delete[] m_quantisationInfoCompounded;
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisationInfoCompounder::compoundQuantisationInfo(
  uint32_t desiredNumCoefSets,
  bool redistribute)
{
  NMP_VERIFY(desiredNumCoefSets > 0 && desiredNumCoefSets <= m_numCoefSets);

  //------------------------------
  // Special case for a single desired coefficient set
  if (desiredNumCoefSets == 1)
  {
    // Find the quantisation set range
    float qMin = m_quantisationInfo[0].m_qMin;
    float qMax = m_quantisationInfo[0].m_qMax;
    for (uint32_t i = 1; i < m_numCoefSets; ++i)
    {
      qMin = NMP::minimum(m_quantisationInfo[i].m_qMin, qMin);
      qMax = NMP::maximum(m_quantisationInfo[i].m_qMax, qMax);
    }

    // Set the quantisation set
    m_quantisationInfoCompounded[0].m_qMin = qMin;
    m_quantisationInfoCompounded[0].m_qMax = qMax;
    m_numCoefSetsCompounded = 1;

    // Set the map from input to compounded indices
    for (uint32_t i = 0; i < m_numCoefSets; ++i)
    {
      m_mapFromInputToCompounded[i] = 0;
    }

    return;
  }

  //------------------------------
  // PRE-STAGE
  std::list<UniformQuantisationInfoSet*> quantisationSets;

  uint32_t desiredNumPreStageQSets = 2 * desiredNumCoefSets;
  if (desiredNumPreStageQSets < m_numCoefSets)
  {
    // Allocate memory for the channel index buffer
    uint32_t* channelIndexBuffer = new uint32_t[m_numCoefSets];

    // Initialise a single quantisation set containing all of the channels
    UniformQuantisationInfoSet* qSet = new UniformQuantisationInfoSet(m_numCoefSets);
    for (uint32_t i = 0; i < m_numCoefSets; ++i)
    {
      qSet->setChannelIndex(i, i);
    }

    // Compute the mean and principal directions of the distribution
    qSet->computeDistribution(m_numCoefSets, m_quantisationInfo);

    // Add the quantisation set to the list
    quantisationSets.push_back(qSet);

    //------------------------------
    // Split the quantisation sets until we have at least the required number of sets
    // for the pre-stage. The pre-stage performs principal components analysis to
    // partition the [qMin, qMax] channel plot into separate geographic point clusters.
    // Starting with a single quantisation set with all the channel data, it partitions
    // sets with the most variance along it's principal direction. While this ensures
    // geographic separation, it does not guarantee optimal set clustering.
    while (quantisationSets.size() < desiredNumPreStageQSets)
    {
      //------------------------------
      // Find the quantisation set with the maximum variance
      float costMax = 0.0f;
      std::list<UniformQuantisationInfoSet*>::iterator it_max = quantisationSets.end();
      std::list<UniformQuantisationInfoSet*>::iterator it;
      for (it = quantisationSets.begin(); it != quantisationSets.end(); ++it)
      {
        // We can't split quantisation sets with only a single channel
        qSet = *it;
        if (qSet->getQVar() > costMax)
        {
          it_max = it;
          costMax = qSet->getQVar();
        }
      }

      // Check if all samples are coincident
      if (it_max == quantisationSets.end())
      {
        break;
      }

      //------------------------------
      // Split the quantisation set into four sub-sets
      qSet = *it_max;
      NMP_VERIFY(qSet->getNumChannelIndices() > 1); // Must make a split otherwise we end up in an infinite loop

      // Split the quantisation set using the distribution
      UniformQuantisationInfoSet* qSubSetA;
      UniformQuantisationInfoSet* qSubSetB;
      bool splitStatus = qSet->splitDistribution(
        m_numCoefSets, m_quantisationInfo,
        channelIndexBuffer,
        qSubSetA, qSubSetB);

      if (splitStatus)
      {
        // Remove the quantisation set from the list
        quantisationSets.erase(it_max);
        delete qSet;

        // Add the sub-sets to the list and recompute the sub-set distributions
        qSubSetA->computeDistribution(m_numCoefSets, m_quantisationInfo);
        qSubSetB->computeDistribution(m_numCoefSets, m_quantisationInfo);
        quantisationSets.push_back(qSubSetA);
        quantisationSets.push_back(qSubSetB);
      }
      else
      {
        // We are unable to split the distribution further because all the
        // samples are coincident
        break;
      }
    }

    // Initialise the quantisation set bracket
    std::list<UniformQuantisationInfoSet*>::iterator it;
    for (it = quantisationSets.begin(); it != quantisationSets.end(); ++it)
    {
      qSet = *it;
      NMP_VERIFY(qSet);
      qSet->computeQMinMax(m_numCoefSets, m_quantisationInfo);
    }

    // Tidy up
    delete[] channelIndexBuffer;
  }
  else
  {
    // NO PRE-STAGE REQUIRED
    for (uint32_t i = 0; i < m_numCoefSets; ++i)
    {
      UniformQuantisationInfoSet* qSet = new UniformQuantisationInfoSet(1);
      qSet->setChannelIndex(0, i);
      qSet->computeQMinMax(m_numCoefSets, m_quantisationInfo);
      quantisationSets.push_back(qSet);
    }
  }

  //------------------------------
  // Iterate until we have compounded the required number of quantisation sets.
  // This optimally merges [qMin, qMax] quantisation set data by brute force.
  // Note: running this without the pre-stage over a data set containing a large
  // amount of channel data will be very slow. It is best to over segment the
  // initial set of quantisation sets using PCA (but still to a relatively small set
  // when compared to the overall number of channels) then perform an optimal merge
  // until the desired number of sets have been found.
  while (quantisationSets.size() > desiredNumCoefSets)
  {
    // Initialise the cost metric for this iteration
    float costMin = FLT_MAX;
    std::list<UniformQuantisationInfoSet*>::iterator itAMin = quantisationSets.end();
    std::list<UniformQuantisationInfoSet*>::iterator itBMin = quantisationSets.end();

    // Find which two quantisation sets to merge by calculating which pair changes the
    // [qMin, qMax] quantisation set bracket the least.
    std::list<UniformQuantisationInfoSet*>::iterator itA = quantisationSets.begin();
    for (; itA != quantisationSets.end(); ++itA)
    {
      const UniformQuantisationInfoSet* qSetA = *itA;

      std::list<UniformQuantisationInfoSet*>::iterator itB = itA;
      for (++itB; itB != quantisationSets.end(); ++itB)
      {
        const UniformQuantisationInfoSet* qSetB = *itB;

        // Compute the cost error caused by merging the pair of quantisation sets

        float cost = qSetA->mergeCost(qSetB);

        // Update the minimum cost information
        if (cost < costMin)
        {
          itAMin = itA;
          itBMin = itB;
          costMin = cost;
        }
      }
    }

    // Compound the two quantisation sets
    NMP_VERIFY(itAMin != quantisationSets.end());
    NMP_VERIFY(itBMin != quantisationSets.end());
    NMP_VERIFY(itAMin != itBMin);

    UniformQuantisationInfoSet* qSetA = *itAMin;
    UniformQuantisationInfoSet* qSetB = *itBMin;
    UniformQuantisationInfoSet* qSet = new UniformQuantisationInfoSet(qSetA, qSetB);

    // Delete the two quantisation sets
    delete qSetA;
    delete qSetB;

    // Swap out set A with the compounded set
    *itAMin = qSet;
    // Remove set B from the quantisation list
    quantisationSets.erase(itBMin);
  }

  //------------------------------
  // REDISTRIBUTE
  if (redistribute)
  {
    while (quantisationSets.size() < desiredNumCoefSets)
    {
      // Find the set with the largest number of channel assignments
      uint32_t maxNumChannels = 1;
      std::list<UniformQuantisationInfoSet*>::iterator it_max = quantisationSets.end();
      std::list<UniformQuantisationInfoSet*>::iterator it;
      for (it = quantisationSets.begin(); it != quantisationSets.end(); ++it)
      {
        const UniformQuantisationInfoSet* qSet = *it;
        uint32_t numChannels = qSet->getNumChannelIndices();

        if (numChannels > maxNumChannels)
        {
          it_max = it;
          maxNumChannels = numChannels;
        }
      }
      NMP_VERIFY(it_max != quantisationSets.end());

      // Partition the set into two
      UniformQuantisationInfoSet* qSubSetA;
      UniformQuantisationInfoSet* qSubSetB;
      const UniformQuantisationInfoSet* qSet = *it_max;
      qSet->redistribute(qSubSetA, qSubSetB);

      // Remove the quantisation set from the list
      quantisationSets.erase(it_max);
      delete qSet;

      // Add the sub-sets to the list
      quantisationSets.push_back(qSubSetA);
      quantisationSets.push_back(qSubSetB);
    }
  }

  //------------------------------
  // Compute the mapping from input to compounded sets
  for (uint32_t i = 0; i < m_numCoefSets; ++i)
  {
    m_mapFromInputToCompounded[i] = 0xFFFFFFFF;
  }

  m_numCoefSetsCompounded = (uint32_t)quantisationSets.size();
  uint32_t setIndex = 0;
  std::list<UniformQuantisationInfoSet*>::iterator it;
  for (it = quantisationSets.begin(); it != quantisationSets.end(); ++it, ++setIndex)
  {
    const UniformQuantisationInfoSet* qSetA = *it;
    m_quantisationInfoCompounded[setIndex].m_qMin = qSetA->getQMin();
    m_quantisationInfoCompounded[setIndex].m_qMax = qSetA->getQMax();

    uint32_t numChannels = qSetA->getNumChannelIndices();
    for (uint32_t i = 0; i < numChannels; ++i)
    {
      uint32_t chanelIndex = qSetA->getChannelIndex(i);
      NMP_VERIFY(chanelIndex < m_numCoefSets);
      NMP_VERIFY(m_mapFromInputToCompounded[chanelIndex] == 0xFFFFFFFF);
      m_mapFromInputToCompounded[chanelIndex] = setIndex;
    }
  }

  //------------------------------
  // Check if all the mappings were assigned
#ifdef NM_DEBUG
  for (uint32_t i = 0; i < m_numCoefSets; ++i)
  {
    NMP_VERIFY(m_mapFromInputToCompounded[i] != 0xFFFFFFFF);
  }
#endif

  //------------------------------
  // Tidy up
  for (it = quantisationSets.begin(); it != quantisationSets.end(); ++it)
  {
    UniformQuantisationInfoSet* qSet = *it;
    NMP_VERIFY(qSet);
    delete qSet;
  }
  quantisationSets.clear();
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t UniformQuantisationInfoCompounder::getNumCoefSets() const
{
  return m_numCoefSets;
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisationInfoCompounder::getQuantisationInfo(uint32_t coefSetIndex, float& qMin, float& qMax) const
{
  NMP_VERIFY(coefSetIndex < m_numCoefSets);
  qMin = m_quantisationInfo[coefSetIndex].m_qMin;
  qMax = m_quantisationInfo[coefSetIndex].m_qMax;
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisationInfoCompounder::setQuantisationInfo(uint32_t coefSetIndex, float qMin, float qMax)
{
  NMP_VERIFY(coefSetIndex < m_numCoefSets);
  m_quantisationInfo[coefSetIndex].m_qMin = qMin;
  m_quantisationInfo[coefSetIndex].m_qMax = qMax;
}

//----------------------------------------------------------------------------------------------------------------------
const uint32_t* UniformQuantisationInfoCompounder::getMapFromInputToCompounded() const
{
  return m_mapFromInputToCompounded;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t UniformQuantisationInfoCompounder::getMapFromInputToCompounded(uint32_t coefSetIndex) const
{
  NMP_VERIFY(coefSetIndex < m_numCoefSets);
  return m_mapFromInputToCompounded[coefSetIndex];
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t UniformQuantisationInfoCompounder::getNumCoefSetsCompounded() const
{
  return m_numCoefSetsCompounded;
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisationInfoCompounder::getQuantisationInfoCompounded(
  uint32_t compoundedCoefSetIndex,
  float&   qMin,
  float&   qMax) const
{
  NMP_VERIFY(compoundedCoefSetIndex < m_numCoefSetsCompounded);
  qMin = m_quantisationInfoCompounded[compoundedCoefSetIndex].m_qMin;
  qMax = m_quantisationInfoCompounded[compoundedCoefSetIndex].m_qMax;
}

//----------------------------------------------------------------------------------------------------------------------
// UniformQuantisationCoefSetBase
//----------------------------------------------------------------------------------------------------------------------
UniformQuantisationCoefSetBase::UniformQuantisationCoefSetBase(uint32_t maxBitsOfPrecision)
{
  // The zeroth quantiser stores information for zero bits precision,
  // the n-th stores information for n bits precision,
  m_numQuantisers = maxBitsOfPrecision + 1;
  m_rates = new float[m_numQuantisers];
  m_dists = new float[m_numQuantisers];

  m_weight = 1.0f;
  m_rateDistComputed = false;
  m_coefSetId = 0;

  m_userID = 0;
  m_userValue = 0;
  m_userData = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
UniformQuantisationCoefSetBase::~UniformQuantisationCoefSetBase()
{
  delete[] m_rates;
  delete[] m_dists;
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisationCoefSetBase::clearRateAndDistortion()
{
  for (uint32_t i = 0; i < m_numQuantisers; ++i)
  {
    m_rates[i] = 0.0f;
    m_dists[i] = 0.0f;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisationCoefSetBase::updateQuantisationInfo()
{
  if (m_qInfo.m_precision == 0)
  {
    m_qInfo.m_qMin = (m_qInfo.m_qMin + m_qInfo.m_qMax) * 0.5f;
    m_qInfo.m_qMax = m_qInfo.m_qMin;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisationCoefSetBase::printRateAndDistortion(FILE* filePointer)
{
  NMP_VERIFY(filePointer);

  fprintf(filePointer, "Quantisation level, rate, distortion\n");
  for (uint32_t i = 0; i < m_numQuantisers; ++i)
  {
    fprintf(filePointer, "%4d %4f %4f\n", i, m_rates[i], m_dists[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisationCoefSetBase::makeDistortionConvex()
{
  const float eps = 1e-6f;
  const uint32_t maxIts = 100;

  // Don't check for the zero precision distortion
  if (m_numQuantisers > 2)
  {
    uint32_t lastIndex = m_numQuantisers - 1;

    // Iterate until the distortion errors are convex
    for (uint32_t its = 0; its < maxIts; ++its)
    {
      // Sort out the first distortion error
      if (m_dists[1] < m_dists[2])
        m_dists[1] = m_dists[2];

      // Sort out the last distortion error
      if (m_dists[lastIndex] > m_dists[lastIndex-1])
        m_dists[lastIndex] = m_dists[lastIndex-1];

      // Find the highest peak or trough
      float maxPeak = 0.0f;
      uint32_t indxPeak = 0;
      for (uint32_t i = 2; i < lastIndex; ++i)
      {
        float dl = m_dists[i] - m_dists[i-1];
        float dr = m_dists[i] - m_dists[i+1];

        // Peak
        if (dl >= 0 && dr > 0)
        {
          float curPeak = dl + dr;
          if (curPeak > maxPeak && curPeak > eps)
          {
            maxPeak = curPeak;
            indxPeak = i;
          }
        }

        // Trough
        if (dl < 0 && dr <= 0)
        {
          float curPeak = -(dl + dr);
          if (curPeak > maxPeak && curPeak > eps)
          {
            maxPeak = curPeak;
            indxPeak = i;
          }
        }
      }

      // Check for termination
      if (indxPeak == 0)
        break;

      // Interpolate the peak value
      m_dists[indxPeak] = 0.5f * (m_dists[indxPeak-1] + m_dists[indxPeak+1]);

      // Check if the left value is lower than the right value
      if (m_dists[indxPeak-1] < m_dists[indxPeak+1])
      {
        m_dists[indxPeak-1] = m_dists[indxPeak+1] = m_dists[indxPeak];
      }
    }

    // Make a final pass to flush all numerically small peaks and troughs
    for (uint32_t i = 2; i < m_numQuantisers; ++i)
    {
      if (m_dists[i] > m_dists[i-1])
        m_dists[i] = m_dists[i-1];
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool UniformQuantisationCoefSetBase::isDistortionConvex(float tol) const
{
  // Don't check for the zero precision distortion
  for (uint32_t i = 2; i < m_numQuantisers; ++i)
  {
    if (m_dists[i] > m_dists[i-1] + tol)
      return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool UniformQuantisationCoefSetBase::isMaxPrecision() const
{
  const float eps = 1.19209e-7f;
  uint32_t maxPrec = m_numQuantisers - 1;

  // Special case where there is no distortion error
  if (m_dists[m_qInfo.m_precision] < eps)
    return true;

  // General case
  return (m_qInfo.m_precision == maxPrec);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t UniformQuantisationCoefSetBase::getCoefSetId() const
{
  return m_coefSetId;
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisationCoefSetBase::setCoefSetId(uint32_t id)
{
  m_coefSetId = id;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t UniformQuantisationCoefSetBase::getUserID() const
{
  return m_userID;
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisationCoefSetBase::setUserID(uint32_t id)
{
  m_userID = id;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t UniformQuantisationCoefSetBase::getUserValue() const
{
  return m_userValue;
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisationCoefSetBase::setUserValue(uint32_t userValue)
{
  m_userValue = userValue;
}

//----------------------------------------------------------------------------------------------------------------------
void* UniformQuantisationCoefSetBase::getUserData() const
{
  return m_userData;
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisationCoefSetBase::setUserData(void* userData)
{
  m_userData = userData;
}

//----------------------------------------------------------------------------------------------------------------------
bool UniformQuantisationCoefSetBase::getRateDistComputed() const
{
  return m_rateDistComputed;
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisationCoefSetBase::resetRateDistComputed()
{
  m_rateDistComputed = false;
}

//----------------------------------------------------------------------------------------------------------------------
const UniformQuantisationInfo& UniformQuantisationCoefSetBase::getQuantisationInfo() const
{
  return m_qInfo;
}

//----------------------------------------------------------------------------------------------------------------------
UniformQuantisationInfo& UniformQuantisationCoefSetBase::getQuantisationInfo()
{
  return m_qInfo;
}

//----------------------------------------------------------------------------------------------------------------------
float UniformQuantisationCoefSetBase::getWeight() const
{
  return m_weight;
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisationCoefSetBase::setWeight(float w)
{
  m_weight = w;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t UniformQuantisationCoefSetBase::getNumQuantisers() const
{
  return m_numQuantisers;
}

//----------------------------------------------------------------------------------------------------------------------
const float* UniformQuantisationCoefSetBase::getRates() const
{
  return m_rates;
}

//----------------------------------------------------------------------------------------------------------------------
float UniformQuantisationCoefSetBase::getRate() const
{
  return m_rates[m_qInfo.m_precision];
}

//----------------------------------------------------------------------------------------------------------------------
float* UniformQuantisationCoefSetBase::getRates()
{
  return m_rates;
}

//----------------------------------------------------------------------------------------------------------------------
const float* UniformQuantisationCoefSetBase::getDistortions() const
{
  return m_dists;
}

//----------------------------------------------------------------------------------------------------------------------
float* UniformQuantisationCoefSetBase::getDistortions()
{
  return m_dists;
}

//----------------------------------------------------------------------------------------------------------------------
float UniformQuantisationCoefSetBase::getDistortion() const
{
  return m_dists[m_qInfo.m_precision];
}

//----------------------------------------------------------------------------------------------------------------------
// UniformQuantisationCoefSet
//----------------------------------------------------------------------------------------------------------------------
UniformQuantisationCoefSet::UniformQuantisationCoefSet(uint32_t numSamples, uint32_t maxBitsOfPrecision) :
  UniformQuantisationCoefSetBase(maxBitsOfPrecision)
{
  NMP_VERIFY(numSamples > 0);
  m_numSamples = numSamples;
  m_data = new float[numSamples];
}

//----------------------------------------------------------------------------------------------------------------------
UniformQuantisationCoefSet::~UniformQuantisationCoefSet()
{
  delete[] m_data;
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisationCoefSet::computeStatistics()
{
  NMP_VERIFY(m_numSamples > 0);
  float sum, sumSq;
  float v;

  // Compute mean and variance of coefficient set
  v = m_data[0];
  m_coefMin = m_coefMax = v;
  sum = v;
  sumSq = v * v;
  for (uint32_t i = 1; i < m_numSamples; ++i)
  {
    // Update the min / max bounds
    v = m_data[i];
    if (v < m_coefMin)
    {
      m_coefMin = v;
    }
    else if (v > m_coefMax)
    {
      m_coefMax = v;
    }

    // Update the statistics
    sum += v;
    sumSq += v * v;
  }

  // Compute the sample mean
  if (m_zeroMean)
    m_coefMean = 0.0f;
  else
    m_coefMean = sum / m_numSamples;

  // Compute the sample variance
  m_coefVar = (sumSq / m_numSamples) - (m_coefMean * m_coefMean);
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisationCoefSet::computeRateAndDistortion()
{
  // Check if the rate and distortion has already been computed for this coefficient set
  if (m_rateDistComputed)
    return;

  //------------------------------
  // Compute statistics of coefficient set data
  computeStatistics();

  //------------------------------
  // Set the quantisation range
  m_qInfo.m_qMin = m_coefMin;
  m_qInfo.m_qMax = m_coefMax;

  //------------------------------
  // Compute the rate and distortion over the range of bit precision levels
  for (uint32_t k = 0; k < m_numQuantisers; ++k)
  {
    computeRateAndDistortion(k, m_rates[k], m_dists[k]);
  }

  // Deal with numerical error by explicitly making the distortion convex
  NMP_VERIFY(isDistortionConvex(1e-4f));
  makeDistortionConvex();

  // Set the rate and distortion computed for this coefficient set
  m_rateDistComputed = true;
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisationCoefSet::computeRateAndDistortion(uint32_t prec, float& rate, float& dist)
{
  uint32_t x;
  float stepSize, offset, y, dy;
  float invStepSize, invOffset;

  rate = 0;
  dist = 0;

  if (prec > 0)
  {
    // Dequantisation transform: y = x * stepSize + qMin
    stepSize = UniformQuantisation::stepSize(m_qInfo.m_qMin, m_qInfo.m_qMax, prec);
    offset = m_qInfo.m_qMin;

    // Quantisation transform: unroll the offset x = (int)((y - qMin) / stepSize + 0.5)
    invStepSize = UniformQuantisation::recipStepSize(m_qInfo.m_qMin, m_qInfo.m_qMax, prec);
    invOffset = m_qInfo.m_qMin * invStepSize + 0.5f;

    // Compute the rate and distortion of the data samples
    for (uint32_t i = 0; i < m_numSamples; ++i)
    {
      // Quantise the coefficient set data sample
      x = (uint32_t)(m_data[i] * invStepSize - invOffset);

      // Update the bit rate. We do not perform any additional entropy encoding
      // so we update the rate with the bit precision level.
      rate += prec;

      // Dequantise the coefficient set data sample
      y = stepSize * x + offset;

      // Update the distortion error
      dy = y - m_data[i];
      dist += dy * dy;
    }
  }
  else
  {
    // Zero bits of precision
    for (uint32_t i = 0; i < m_numSamples; ++i)
    {
      dy = m_coefMean - m_data[i]; // Use the mean
      dist += dy * dy;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisationCoefSet::updateQuantisationInfo()
{
  // If the coefficient set has zero bits of precision then update
  // the quantisation information to represent the mean of the
  // data distribution
  if (m_qInfo.m_precision == 0)
  {
    if (m_zeroMean)
      m_qInfo.m_qMax = m_qInfo.m_qMin = 0.0f;
    else
      m_qInfo.m_qMax = m_qInfo.m_qMin = m_coefMean;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisationCoefSet::quantise(BitStreamEncoder& encoder)
{
  UniformQuantisation::quantise(encoder, m_qInfo, m_numSamples, m_data);
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisationCoefSet::dequantise(BitStreamDecoder& decoder)
{
  UniformQuantisation::dequantise(decoder, m_qInfo, m_numSamples, m_data);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t UniformQuantisationCoefSet::getNumSamples() const
{
  return m_numSamples;
}

//----------------------------------------------------------------------------------------------------------------------
const float* UniformQuantisationCoefSet::getSamples() const
{
  return m_data;
}

//----------------------------------------------------------------------------------------------------------------------
float* UniformQuantisationCoefSet::getSamples()
{
  return m_data;
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisationCoefSet::setZeroMeanDistribution(bool enable)
{
  m_zeroMean = enable;
}

//----------------------------------------------------------------------------------------------------------------------
bool UniformQuantisationCoefSet::getZeroMeanDistribution() const
{
  return m_zeroMean;
}

//----------------------------------------------------------------------------------------------------------------------
float UniformQuantisationCoefSet::getCoefMean() const
{
  return m_coefMean;
}

//----------------------------------------------------------------------------------------------------------------------
float UniformQuantisationCoefSet::getCoefVar() const
{
  return m_coefVar;
}

//----------------------------------------------------------------------------------------------------------------------
// UniformQuantisation
//----------------------------------------------------------------------------------------------------------------------
UniformQuantisation::UniformQuantisation(uint32_t numCoefSets)
{
  NMP_VERIFY(numCoefSets > 0);
  m_byteBudget = 0;
  m_numCoefSets = numCoefSets;

  // Coefficient sets
  m_coeffSets = new UniformQuantisationCoefSetBase*[m_numCoefSets];
  for (uint32_t i = 0; i < m_numCoefSets; ++i)
    m_coeffSets[i] = NULL;

  // Saved quantisation state
  m_qInfoState = new UniformQuantisationInfo[m_numCoefSets];
  for (uint32_t i = 0; i < m_numCoefSets; ++i)
    m_qInfoState[i].clear();
}

//----------------------------------------------------------------------------------------------------------------------
UniformQuantisation::~UniformQuantisation()
{
  // Coefficient sets
  for (uint32_t i = 0; i < m_numCoefSets; ++i)
  {
    if (m_coeffSets[i])
    {
      delete m_coeffSets[i];
      m_coeffSets[i] = NULL;
    }
  }
  delete[] m_coeffSets;

  // Saved quantisation state
  delete[] m_qInfoState;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t UniformQuantisation::quantise(
  float    qMin,
  float    qMax,
  float    recipStepSize,
  float    val)
{
  uint32_t result;
  val = NMP::clampValue(val, qMin, qMax);
  float qVal = (val - qMin) * recipStepSize;
  result = (uint32_t)(qVal + 0.5f);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t UniformQuantisation::quantiseFloor(
  float    qMin,
  float    qMax,
  float    recipStepSize,
  float    val)
{
  uint32_t result;
  val = NMP::clampValue(val, qMin, qMax);
  float qVal = (val - qMin) * recipStepSize;
  result = (uint32_t)(qVal + 0.01f); // Apply small tolerance for numerical error
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t UniformQuantisation::quantiseCeil(
  float    qMin,
  float    qMax,
  float    recipStepSize,
  float    val)
{
  uint32_t result;
  val = NMP::clampValue(val, qMin, qMax);
  float qVal = (val - qMin) * recipStepSize;
  result = (uint32_t)(qVal + 0.99f); // Apply small tolerance for numerical error
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t UniformQuantisation::quantise(
  float    qMin,
  float    qMax,
  uint32_t prec,
  float    val)
{
  float recipStepSize;
  uint32_t result;

  recipStepSize = UniformQuantisation::recipStepSize(qMin, qMax, prec);
  val = NMP::clampValue(val, qMin, qMax);
  float qVal = (val - qMin) * recipStepSize;
  result = (uint32_t)(qVal + 0.5f);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t UniformQuantisation::quantiseFloor(
  float    qMin,
  float    qMax,
  uint32_t prec,
  float    val)
{
  float recipStepSize;
  uint32_t result;

  recipStepSize = UniformQuantisation::recipStepSize(qMin, qMax, prec);
  val = NMP::clampValue(val, qMin, qMax);
  float qVal = (val - qMin) * recipStepSize;
  result = (uint32_t)(qVal + 0.01f); // Apply small tolerance for numerical error
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t UniformQuantisation::quantiseCeil(
  float    qMin,
  float    qMax,
  uint32_t prec,
  float    val)
{
  float recipStepSize;
  uint32_t result;

  recipStepSize = UniformQuantisation::recipStepSize(qMin, qMax, prec);
  val = NMP::clampValue(val, qMin, qMax);
  float qVal = (val - qMin) * recipStepSize;
  result = (uint32_t)(qVal + 0.99f); // Apply small tolerance for numerical error
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
float UniformQuantisation::dequantise(
  float    qMin,
  float    qMax,
  uint32_t prec,
  uint32_t val)
{
  float stepSize, result;

  stepSize = UniformQuantisation::stepSize(qMin, qMax, prec);
  result = val * stepSize + qMin;
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
float UniformQuantisation::dequantise(
  float    qStepSize,
  float    qMin,
  uint32_t val)
{
  float result = val * qStepSize + qMin;
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
float UniformQuantisation::stepSize(
  float    qMin,
  float    qMax,
  uint32_t prec)
{
  uint32_t numSteps;
  float stepSize;

  NMP_VERIFY(qMax >= qMin);
  NMP_VERIFY(prec < 32);
  if (prec > 0)
  {
    numSteps = (1 << prec) - 1;
    stepSize = (qMax - qMin) / (float)numSteps;
  }
  else
  {
    stepSize = 0.0f;
  }

  return stepSize;
}

//----------------------------------------------------------------------------------------------------------------------
float UniformQuantisation::recipStepSize(
  float    qMin,
  float    qMax,
  uint32_t prec)
{
  uint32_t numSteps;
  float diff, recipStepSize;

  NMP_VERIFY(qMax >= qMin);
  NMP_VERIFY(prec < 32);
  diff = qMax - qMin;
  if (prec > 0 && diff > 1.19209e-7f)
  {
    numSteps = (1 << prec) - 1;
    recipStepSize = (float)numSteps / diff;
  }
  else
  {
    recipStepSize = 0.0f;
  }

  return recipStepSize;
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisation::quantise(
  BitStreamEncoder&              encoder,
  const UniformQuantisationInfo& qInfo,
  uint32_t                       numSamples,
  const float*                   data)
{
  uint32_t y, numSteps;
  float diff, recipStepSize, val;
  NMP_VERIFY(qInfo.m_qMax >= qInfo.m_qMin);
  NMP_VERIFY(qInfo.m_precision < 32);

  if (qInfo.m_precision > 0)
  {
    diff = qInfo.m_qMax - qInfo.m_qMin;
    if (diff > 1.19209e-7f)
    {
      // Compute the step size for a single quantisation interval
      numSteps = (1 << qInfo.m_precision) - 1;
      recipStepSize = (float)numSteps / diff;
    }
    else
    {
      recipStepSize = 0.0f;
    }

    // Quantise the data samples
    for (uint32_t i = 0; i < numSamples; ++i)
    {
      val = NMP::clampValue(data[i], qInfo.m_qMin, qInfo.m_qMax);
      float qVal = (val - qInfo.m_qMin) * recipStepSize;
      y = (uint32_t)(qVal + 0.5f);
      encoder.write(qInfo.m_precision, y);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisation::quantiseDebug(
  FILE*                          filePointer,
  const UniformQuantisationInfo& qInfo,
  uint32_t                       numSamples,
  const float*                   data)
{
  uint32_t y, numSteps;
  float diff, recipStepSize, val;
  NMP_VERIFY(qInfo.m_qMax >= qInfo.m_qMin);
  NMP_VERIFY(qInfo.m_precision < 32);

  if (qInfo.m_precision > 0)
  {
    diff = qInfo.m_qMax - qInfo.m_qMin;
    if (diff > 1.19209e-7f)
    {
      // Compute the step size for a single quantisation interval
      numSteps = (1 << qInfo.m_precision) - 1;
      recipStepSize = (float)numSteps / diff;
    }
    else
    {
      recipStepSize = 0.0f;
    }

    // Quantise the data samples
    for (uint32_t i = 0; i < numSamples; ++i)
    {
      val = NMP::clampValue(data[i], qInfo.m_qMin, qInfo.m_qMax);
      float qVal = (val - qInfo.m_qMin) * recipStepSize;
      y = (uint32_t)(qVal + 0.5f);
      fprintf(filePointer, "%4d\n", y);
    }
  }
  else
  {
    fprintf(filePointer, "[Zero bits precision, numSamples = %4d]\n", numSamples);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisation::dequantise(
  BitStreamDecoder&              decoder,
  const UniformQuantisationInfo& qInfo,
  uint32_t                       numSamples,
  float*                         data)
{
  uint32_t y, numSteps;
  float stepSize;
  NMP_VERIFY(qInfo.m_qMax >= qInfo.m_qMin);
  NMP_VERIFY(qInfo.m_precision < 32);

  if (qInfo.m_precision > 0)
  {
    // Compute the step size for a single quantisation interval
    numSteps = (1 << qInfo.m_precision) - 1;
    stepSize = (qInfo.m_qMax - qInfo.m_qMin) / (float)numSteps;

    // Dequantise the data samples
    for (uint32_t i = 0; i < numSamples; ++i)
    {
      decoder.read(qInfo.m_precision, y);
      data[i] = y * stepSize + qInfo.m_qMin;
    }
  }
  else
  {
    // Set the default data value (mean of samples)
    for (uint32_t i = 0; i < numSamples; ++i)
      data[i] = qInfo.m_qMin;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisation::quantise(BitStreamEncoder& encoder)
{
  NMP_VERIFY(encoder.getBuffer());

  for (uint32_t k = 0; k < m_numCoefSets; ++k)
  {
    if (m_coeffSets[k])
      m_coeffSets[k]->quantise(encoder);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisation::dequantise(BitStreamDecoder& decoder)
{
  NMP_VERIFY(decoder.getBuffer());

  for (uint32_t k = 0; k < m_numCoefSets; ++k)
  {
    if (m_coeffSets[k])
      m_coeffSets[k]->dequantise(decoder);
  }
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t UniformQuantisation::getNumCoefSets() const
{
  return m_numCoefSets;
}

//----------------------------------------------------------------------------------------------------------------------
const UniformQuantisationCoefSetBase*
UniformQuantisation::getCoefficientSet(uint32_t i) const
{
  NMP_VERIFY(i < m_numCoefSets);
  return m_coeffSets[i];
}

//----------------------------------------------------------------------------------------------------------------------
UniformQuantisationCoefSetBase*
UniformQuantisation::getCoefficientSet(uint32_t i)
{
  NMP_VERIFY(i < m_numCoefSets);
  return m_coeffSets[i];
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisation::setCoefficientSet(uint32_t i, UniformQuantisationCoefSetBase* coefSet)
{
  NMP_VERIFY(i < m_numCoefSets);

  // Remove previous coefficient set
  if (m_coeffSets[i])
  {
    delete m_coeffSets[i];
    m_coeffSets[i] = NULL;
  }

  if (coefSet)
  {
    // Set the new coefficient set
    m_coeffSets[i] = coefSet;
    coefSet->setCoefSetId(i);
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool UniformQuantisation::distributeBits(size_t byteBudget)
{
  //------------------------------
  // Compute the rate and distortion characteristics of the coefficient sets
  NMP_VERIFY(m_coeffSets);
  for (uint32_t k = 0; k < m_numCoefSets; ++k)
  {
    if (m_coeffSets[k])
      m_coeffSets[k]->computeRateAndDistortion();
  }

  //------------------------------
  // Compute the optimal distribution of bits
  if (!optimiseBitAllocations(byteBudget))
    return false;

  //------------------------------
  // Update the quantisation data for any coefficient sets assigned
  // with zero bits of precision
  for (uint32_t k = 0; k < m_numCoefSets; ++k)
  {
    if (m_coeffSets[k])
      m_coeffSets[k]->updateQuantisationInfo();
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t UniformQuantisation::getNumCoefSetsWithUserId(uint32_t userId) const
{
  uint32_t n = 0;

  NMP_VERIFY(m_coeffSets);
  for (uint32_t k = 0; k < m_numCoefSets; ++k)
  {
    const UniformQuantisationCoefSetBase* coefSet = m_coeffSets[k];
    if (coefSet)
    {
      if (coefSet->getUserID() == userId)
        n++;
    }
  }

  return n;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t UniformQuantisation::getSumCoefSetPrecisionsWithUserId(uint32_t userId) const
{
  uint32_t n = 0;

  NMP_VERIFY(m_coeffSets);
  for (uint32_t k = 0; k < m_numCoefSets; ++k)
  {
    const UniformQuantisationCoefSetBase* coefSet = m_coeffSets[k];
    if (coefSet)
    {
      if (coefSet->getUserID() == userId)
      {
        const UniformQuantisationInfo& qInfo = coefSet->getQuantisationInfo();
        n += qInfo.m_precision;
      }
    }
  }

  return n;
}

//----------------------------------------------------------------------------------------------------------------------
size_t UniformQuantisation::getActualByteBudget() const
{
  return (size_t)ceil(getRate() / 8.0f);
}

//----------------------------------------------------------------------------------------------------------------------
float UniformQuantisation::getRate() const
{
  float rate = 0.0f;
  for (uint32_t k = 0; k < m_numCoefSets; ++k)
  {
    if (m_coeffSets[k])
      rate += m_coeffSets[k]->getRate();
  }

  return rate;
}

//----------------------------------------------------------------------------------------------------------------------
float UniformQuantisation::getDistortion() const
{
  float dist = 0.0f;

  for (uint32_t k = 0; k < m_numCoefSets; ++k)
  {
    if (m_coeffSets[k])
      dist += m_coeffSets[k]->getDistortion();
  }

  return dist;
}

//----------------------------------------------------------------------------------------------------------------------
bool UniformQuantisation::isMaxPrecision() const
{
  for (uint32_t k = 0; k < m_numCoefSets; ++k)
  {
    if (m_coeffSets[k])
    {
      if (!m_coeffSets[k]->isMaxPrecision())
        return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisation::resetRateDistComputed()
{
  for (uint32_t k = 0; k < m_numCoefSets; ++k)
  {
    if (m_coeffSets[k])
      m_coeffSets[k]->resetRateDistComputed();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisation::saveInfoState()
{
  for (uint32_t k = 0; k < m_numCoefSets; ++k)
  {
    if (m_coeffSets[k])
    {
      const UniformQuantisationInfo& qInfo = m_coeffSets[k]->getQuantisationInfo();
      m_qInfoState[k] = qInfo;
    }
    else
    {
      m_qInfoState[k].m_qMin = 0.0f;
      m_qInfoState[k].m_qMax = 0.0f;
      m_qInfoState[k].m_precision = 0xffffffff;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisation::restoreInfoState()
{
  for (uint32_t k = 0; k < m_numCoefSets; ++k)
  {
    if (m_coeffSets[k])
    {
      UniformQuantisationInfo& qInfo = m_coeffSets[k]->getQuantisationInfo();
      qInfo = m_qInfoState[k];
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
const UniformQuantisationInfo* UniformQuantisation::getInfoState() const
{
  return m_qInfoState;
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisation::printRateAndDistortion(FILE* filePointer)
{
  NMP_VERIFY(filePointer);

  for (uint32_t k = 0; k < m_numCoefSets; ++k)
  {
    if (m_coeffSets[k])
    {
      fprintf(filePointer, "\n____________ Coef Set %d ____________\n", k);
      m_coeffSets[k]->printRateAndDistortion(filePointer);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool UniformQuantisation::optimiseBitAllocations(size_t byteBudget)
{
  float lambda, lambdaLow, lambdaHigh;
  float rate, rateLambdaLow, rateLambdaHigh;
  float dist, distLambdaLow, distLambdaHigh;
  float bitBudget, lambdaTol;
  float deltaBitBudget, deltaRate, deltaDist, deltaRatio;
  float bestDeltaRatio, bestDeltaRate;
  uint32_t precision, bestCoefSet, bestPrecision;
  bool status;

  m_byteBudget = byteBudget;
  bitBudget = (float)(8 * m_byteBudget);
  lambdaTol = 1.19209e-7f;

  //------------------------------
  // Compute the optimal rate and distortion corresponding to the largest
  // possible allocation of bits. If this is within the bit budget then
  // we can stop here.
  lambdaLow = 0.0f;
  optimiseBitAllocationsLambdaStep(lambdaLow, rateLambdaLow, distLambdaLow);
  if (rateLambdaLow <= bitBudget)
    return true;

  //------------------------------
  // Compute the optimal rate and distortion corresponding to the smallest
  // possible allocation of bits within the byte budget. We first bracket
  // a value of lambda (high) that has an optimal assignment of bits within
  // the budget. Fast convergence is achieved by making a coarse adjustment
  // of lambda at each stage.
  status = false; // Bracket status
  lambdaHigh = 1e+7f; // Starting lambda
  while (true)
  {
    // Check on the bounds of lambda
    if (lambdaHigh == std::numeric_limits<float>::infinity())
      return false;

    // Find optimal allocation for the current value of lambda
    optimiseBitAllocationsLambdaStep(lambdaHigh, rateLambdaHigh, distLambdaHigh);

    // Check if we are within the bit budget
    if (rateLambdaHigh <= bitBudget)
      break;

    // Since the optimal bit rate is still over our bit budget we update the
    // lower bound rate and distortion
    lambdaLow = lambdaHigh;
    rateLambdaLow = rateLambdaHigh;
    distLambdaLow = distLambdaHigh;
    status = true; // Have bracketed lambda low

    // Increase the lambda parameter further
    lambdaHigh *= 100.0f;
  }

  //------------------------------
  // We next bracket a value of lambda (low) that has an optimal assignment of bits
  // just outside the specified budget. Fast convergence is achieved by making a
  // coarse adjustment of lambda at each stage.
  if (!status)
  {
    lambdaLow = lambdaHigh;
    while (true)
    {
      // Check on the bounds of lambda
      if (lambdaLow == 0)
        return false;

      // Decrease the lambda parameter further
      lambdaLow *= 0.01f;

      // Find optimal allocation for the current value of lambda
      optimiseBitAllocationsLambdaStep(lambdaLow, rateLambdaLow, distLambdaLow);

      // Check if we are now outside the bit budget
      if (rateLambdaLow > bitBudget)
        break;

      // Since the optimal bit rate is still under our bit budget we update the
      // higher bound rate and distortion
      lambdaHigh = lambdaLow;
      rateLambdaHigh = rateLambdaLow;
      distLambdaHigh = distLambdaLow;
      status = true; // Have bracketed lambda high
    }
  }

  // Sanity check to see if we have bracketed the bit budget
  NMP_VERIFY(rateLambdaHigh <= bitBudget && rateLambdaLow > bitBudget);
  NMP_VERIFY(rateLambdaHigh < rateLambdaLow);

  //------------------------------
  // Perform binary section search for the optimal value of lambda that has
  // a bit rate within the specified bit budget
  float lambdaOld = 0.0f;
  do
  {
    lambda = 0.5f * (lambdaLow + lambdaHigh);
    optimiseBitAllocationsLambdaStep(lambda, rate, dist);

    if (rate > bitBudget)
      lambdaLow = lambda;
    else
      lambdaHigh = lambda;

    // Check for termination on numerical precision
    if (lambda == lambdaOld)
      break;
    lambdaOld = lambda;

  } while (lambdaHigh - lambdaLow > lambdaTol);

  // Test if the converged lambda value has a rate within the specified bit budget
  if (rate > bitBudget)
  {
    lambda = lambdaHigh;
    optimiseBitAllocationsLambdaStep(lambda, rate, dist);
  }

  // Sanity check to see if we have found a value of lambda within the bit budget
  NMP_VERIFY(rate <= bitBudget);

  //------------------------------
  // The binary section search above guarantees that we find an allocation of
  // bits that *does not exceed* the bit budget. However, it is possible that
  // any remaining bits can be allocated by increasing the precision of some
  // suitable coefficient set. We use a brute force search to find the best
  // coefficient set.
  deltaBitBudget = bitBudget - rate;
  while (true)
  {
    // Reset the best parameter information
    bestCoefSet = m_numCoefSets;
    bestPrecision = 0;
    bestDeltaRatio = 0.0f;
    bestDeltaRate = 0.0f;

    // Find the best coefficient set to increment the precision with
    for (uint32_t k = 0; k < m_numCoefSets; ++k)
    {
      if (m_coeffSets[k])
      {
        UniformQuantisationInfo& qInfo = m_coeffSets[k]->getQuantisationInfo();
        uint32_t numQuantisers = m_coeffSets[k]->getNumQuantisers();
        float* rates = m_coeffSets[k]->getRates();
        NMP_VERIFY(rates);
        float* dists = m_coeffSets[k]->getDistortions();
        NMP_VERIFY(dists);
        float weight = m_coeffSets[k]->getWeight();

        precision = qInfo.m_precision;
        for (uint32_t i = precision + 1; i < numQuantisers; ++i)
        {
          // Compute the change in rate by incrementing the precision
          deltaRate = rates[i] - rates[precision];
          NMP_VERIFY(deltaRate > 0.0f);

          // Check if the increase in bit rate exceeds the specified budget
          if (deltaRate > deltaBitBudget)
            break;

          // Compute the change in distortion by incrementing the precision
          deltaDist = weight * (dists[precision] - dists[i]);

          // Update the best parameter information
          deltaRatio = deltaDist / deltaRate;
          if (deltaRatio > bestDeltaRatio)
          {
            bestCoefSet = k;
            bestPrecision = i;
            bestDeltaRatio = deltaRatio;
            bestDeltaRate = deltaRate;
          }
        }
      }
    }

    // Check if we have found a coefficient set for incrementing the precision
    if (bestCoefSet == m_numCoefSets)
      break;

    // Increment the precision of the best coefficient set
    UniformQuantisationInfo& qInfoBest = m_coeffSets[bestCoefSet]->getQuantisationInfo();
    qInfoBest.m_precision = bestPrecision;
    deltaBitBudget -= bestDeltaRate;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void UniformQuantisation::optimiseBitAllocationsLambdaStep(
  float  lambda,
  float& optimalRate,
  float& optimalDist)
{
  float fn, rate, dist;
  float minFn, minRate, minDist;

  optimalRate = 0.0f;
  optimalDist = 0.0f;

  // Compute the unconstrained optimisation problem: min \sum_i (d_i(r_i) + lambda*r_i)
  for (uint32_t k = 0; k < m_numCoefSets; ++k)
  {
    if (m_coeffSets[k])
    {
      UniformQuantisationInfo& qInfo = m_coeffSets[k]->getQuantisationInfo();
      uint32_t numQuantisers = m_coeffSets[k]->getNumQuantisers();
      float* rates = m_coeffSets[k]->getRates();
      NMP_VERIFY(rates);
      float* dists = m_coeffSets[k]->getDistortions();
      NMP_VERIFY(dists);
      float weight = m_coeffSets[k]->getWeight();

      // Zero bits precision
      minRate = rates[0];
      minDist = weight * dists[0];
      minFn = minDist + lambda * minRate;
      qInfo.m_precision = 0;

      // Find the optimal rate and distortion over the remaining precision levels
      for (uint32_t i = 1; i < numQuantisers; ++i)
      {
        rate = rates[i];
        dist = weight * dists[i];
        fn = dist + lambda * rate;
        if (fn < minFn)
        {
          minRate = rate;
          minDist = dist;
          minFn = fn;
          qInfo.m_precision = i; // Update the optimal precision
        }
      }

      // Update the optimal rate and distortion total
      optimalRate += minRate;
      optimalDist += minDist;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif
