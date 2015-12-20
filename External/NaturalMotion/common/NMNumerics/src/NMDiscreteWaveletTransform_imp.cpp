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
// Specializes template implementations for the type ScalarType.
#ifndef ScalarType
  #error This file should not be compiled directly
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// DiscreteWaveletTransform
//----------------------------------------------------------------------------------------------------------------------
template <>
DiscreteWaveletTransform<ScalarType>::DiscreteWaveletTransform(uint32_t numSamples) :
  m_data(numSamples), m_temp(numSamples)
{
  NMP_ASSERT(numSamples >= 4);

  // Check that the number of samples is a power of 2
  uint32_t n = 0;
  for (uint32_t i = numSamples; i > 1; i >>= 1, ++n);
  NMP_ASSERT(numSamples == (static_cast<uint32_t>(1) << n));

  // Setup the Daubechies 4-Tap filter coefficients
  const ScalarType s3 = sqrt(static_cast<ScalarType>(3));
  const ScalarType d = 4 * sqrt(static_cast<ScalarType>(2));

  // Low pass filter
  m_filterLow[0] = (1 + s3) / d;
  m_filterLow[1] = (3 + s3) / d;
  m_filterLow[2] = (3 - s3) / d;
  m_filterLow[3] = (1 - s3) / d;

  // High pass filter
  m_filterHigh[0] = m_filterLow[3];
  m_filterHigh[1] = -m_filterLow[2];
  m_filterHigh[2] = m_filterLow[1];
  m_filterHigh[3] = -m_filterLow[0];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
DiscreteWaveletTransform<ScalarType>::~DiscreteWaveletTransform()
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void DiscreteWaveletTransform<ScalarType>::transform(uint32_t numSamples, const ScalarType* x, uint32_t numSteps)
{
  NMP_ASSERT(numSamples >= 4);

  // Initialise the set of coefficients
  if (x != m_data.data())
    m_data.setSubVector(0, numSamples, x);
  NMP_ASSERT(numSamples >> numSteps != 0);

  //------------------------------
  // Iterate over the scaling levels
  uint32_t n = numSamples;
  for (uint32_t step = 0; step < numSteps; ++step)
  {
    // Apply Wavelet filters to the current sub-band
    for (uint32_t i = 0; i < n - 2; i += 2)
    {
      // Low pass filter
      m_temp[i] = m_filterLow[0] * m_data[i] + m_filterLow[1] * m_data[i+1]
                  + m_filterLow[2] * m_data[i+2] + m_filterLow[3] * m_data[i+3];

      // High pass filter
      m_temp[i+1] = m_filterHigh[0] * m_data[i] + m_filterHigh[1] * m_data[i+1]
                    + m_filterHigh[2] * m_data[i+2] + m_filterHigh[3] * m_data[i+3];
    }

    // Periodically wrap edge data
    m_temp[n-2] = m_filterLow[0] * m_data[n-2] + m_filterLow[1] * m_data[n-1]
                  + m_filterLow[2] * m_data[0] + m_filterLow[3] * m_data[1];

    m_temp[n-1] = m_filterHigh[0] * m_data[n-2] + m_filterHigh[1] * m_data[n-1]
                  + m_filterHigh[2] * m_data[0] + m_filterHigh[3] * m_data[1];

    // De-interleave the coarse and detail coefficients
    n >>= 1; // Next sub-band
    for (uint32_t i = 0; i < n; ++i)
    {
      uint32_t i2 = i << 1;
      m_data[i] = m_temp[i2]; // Coarse
      m_data[i+n] = m_temp[i2+1]; // Detail
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void DiscreteWaveletTransform<ScalarType>::transform(const Vector<ScalarType>& x, uint32_t numSteps)
{
  transform(x.numElements(), x.data(), numSteps);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void DiscreteWaveletTransform<ScalarType>::inverse(uint32_t numSamples, const ScalarType* coefs, uint32_t numSteps)
{
  NMP_ASSERT(numSamples >= 4);

  // Initialise the set of coefficients
  if (coefs != m_data.data())
    m_data.setSubVector(0, numSamples, coefs);

  // Compute the size of the lowest level sub-band
  uint32_t n = numSamples;
  NMP_ASSERT(n >> numSteps != 0);
  n >>= numSteps;

  for (uint32_t step = 0; step < numSteps; ++step)
  {
    // Interleave the coarse and detail coefficients
    for (uint32_t i = 0; i < n; ++i)
    {
      uint32_t i2 = i << 1;
      m_temp[i2] = m_data[i]; // Coarse
      m_temp[i2+1] = m_data[i+n]; // Detail
    }
    n <<= 1; // Next sub-band

    // Apply inverse Wavelet filters to the current sub-band
    for (uint32_t i = 0; i < n; ++i)
      m_data[i] = static_cast<ScalarType>(0);

    for (uint32_t i = 0; i < n - 2; i += 2)
    {
      for (uint32_t k = 0; k < 4; ++k)
      {
        m_data[i+k] += m_filterLow[k] * m_temp[i]; // Low pass filter
        m_data[i+k] += m_filterHigh[k] * m_temp[i+1]; // High pass filter
      }
    }

    // Periodically wrap edge data
    m_data[n-2] += m_filterLow[0] * m_temp[n-2];
    m_data[n-1] += m_filterLow[1] * m_temp[n-2];
    m_data[0] += m_filterLow[2] * m_temp[n-2];
    m_data[1] += m_filterLow[3] * m_temp[n-2];

    m_data[n-2] += m_filterHigh[0] * m_temp[n-1];
    m_data[n-1] += m_filterHigh[1] * m_temp[n-1];
    m_data[0] += m_filterHigh[2] * m_temp[n-1];
    m_data[1] += m_filterHigh[3] * m_temp[n-1];
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void DiscreteWaveletTransform<ScalarType>::inverse(const Vector<ScalarType>& coefs, uint32_t numSteps)
{
  inverse(coefs.numElements(), coefs.data(), numSteps);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t DiscreteWaveletTransform<ScalarType>::getDataSize() const
{
  return m_data.numElements();
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const Vector<ScalarType>&
DiscreteWaveletTransform<ScalarType>::getData() const
{
  return m_data;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType>& DiscreteWaveletTransform<ScalarType>::getData()
{
  return m_data;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t DiscreteWaveletTransform<ScalarType>::getNumSubBands(uint32_t numSteps) const
{
  return numSteps + 1;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void DiscreteWaveletTransform<ScalarType>::getSubBandSize(
  uint32_t numSteps, uint32_t subBandIndex, uint32_t& subBandPos, uint32_t& subBandSize) const
{
  NMP_ASSERT(subBandIndex <= numSteps);
  uint32_t numSamples = m_data.numElements();

  subBandPos = 0;
  subBandSize = numSamples;
  for (; numSteps > 0; --numSteps)
  {
    // Split the current band in half
    subBandSize >>= 1;

    // Continue with sub-band splitting?
    if (subBandIndex == numSteps)
    {
      subBandPos = subBandSize;
      break;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t DiscreteWaveletTransform<ScalarType>::getSubBandSize(uint32_t numSteps, uint32_t subBandIndex) const
{
  uint32_t subBandPos, subBandSize;
  getSubBandSize(numSteps, subBandIndex, subBandPos, subBandSize);
  return subBandSize;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void DiscreteWaveletTransform<ScalarType>::getSubBandData(uint32_t numSteps, uint32_t subBandIndex, ScalarType* data) const
{
  uint32_t subBandPos, subBandSize;
  getSubBandSize(numSteps, subBandIndex, subBandPos, subBandSize);
  m_data.getSubVector(subBandPos, subBandSize, data);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void DiscreteWaveletTransform<ScalarType>::setSubBandData(uint32_t numSteps, uint32_t subBandIndex, const ScalarType* data)
{
  uint32_t subBandPos, subBandSize;
  getSubBandSize(numSteps, subBandIndex, subBandPos, subBandSize);
  m_data.setSubVector(subBandPos, subBandSize, data);
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
