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
#ifndef NM_DWT_H
#define NM_DWT_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMVector.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief Class for computing the discrete wavelet transform of a 1D signal using
/// a Daubechies 4-Tap filter
template <typename _T>
class DiscreteWaveletTransform
{
public:
  //---------------------------------------------------------------------
  /// \brief Constructor to preallocate the memory for the transform
  /// \param numSamples - The number of data samples (must be a power of 2).
  DiscreteWaveletTransform(uint32_t numSamples);

  //---------------------------------------------------------------------
  ~DiscreteWaveletTransform();

  //---------------------------------------------------------------------
  /// \name   Transform functions
  /// \brief  Functions to perform a discrete wavelet transform or its inverse
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief Compute the Discrete Wavelet Transform of the supplied
  /// input vector, placing the computed wavelet coefficients into m_data.
  ///
  /// \param x - The discretely sampled signal data
  void transform(uint32_t numSamples, const _T* x, uint32_t numSteps);
  void transform(const Vector<_T>& x, uint32_t numSteps);

  //---------------------------------------------------------------------
  /// \brief Compute the inverse Discrete Wavelet Transform of the
  /// supplied wavelet coefficients, placing the reconstructed signal
  /// data into m_data.
  ///
  /// \param coefs - The wavelet coefficients
  void inverse(uint32_t numSamples, const _T* coefs, uint32_t numSteps);
  void inverse(const Vector<_T>& coefs, uint32_t numSteps);
  //@}

  //---------------------------------------------------------------------
  /// \name   Data and sub-band functions
  /// \brief  Functions to get information or sub-band data
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief Get the size of the data vector
  uint32_t getDataSize() const;

  //---------------------------------------------------------------------
  /// \brief Get the coefficient data vector
  const Vector<_T>& getData() const;
  Vector<_T>& getData();

  //---------------------------------------------------------------------
  /// \brief Get the number of sub-bands created by the DWT transform
  uint32_t getNumSubBands(uint32_t numSteps) const;

  //---------------------------------------------------------------------
  /// \brief Get the position and size information for the specified sub-band
  /// created by the DWT transform.
  /// \param  subBandIndex - 0 is the coarsest sub-band
  void getSubBandSize(uint32_t numSteps, uint32_t subBandIndex, uint32_t& subBandPos, uint32_t& subBandSize) const;

  //---------------------------------------------------------------------
  /// \brief Get the flat size of the specified sub-band created by the DWT transform
  /// \param  subBandIndex - 0 is the coarsest sub-band
  uint32_t getSubBandSize(uint32_t numSteps, uint32_t subBandIndex) const;

  //---------------------------------------------------------------------
  /// \brief Get the flat data corresponding to the specified sub-band
  /// created by the DWT transform
  /// \param  subBandIndex - 0 is the coarsest sub-band
  void getSubBandData(uint32_t numSteps, uint32_t subBandIndex, _T* data) const;

  //---------------------------------------------------------------------
  /// \brief Set the flat data corresponding to the specified sub-band
  /// created by the DWT transform
  /// \param  subBandIndex - 0 is the coarsest sub-band
  void setSubBandData(uint32_t numSteps, uint32_t subBandIndex, const _T* data);
  //@}

protected:
  //---------------------------------------------------------------------
  Vector<_T> m_data;      ///< Transformed data from the DWT

  //---------------------------------------------------------------------
  Vector<_T> m_temp;      ///< Temporary workspace vector

  //---------------------------------------------------------------------
  _T m_filterLow[4];
  _T m_filterHigh[4];
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_DWT_H
