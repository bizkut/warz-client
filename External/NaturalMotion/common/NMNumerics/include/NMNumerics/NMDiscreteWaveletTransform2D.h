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
#ifndef NM_DWT_2D_H
#define NM_DWT_2D_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMatrix.h"
#include "NMNumerics/NMDiscreteWaveletTransform.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief Class for computing the discrete wavelet transform of a 2D signal using
/// a Daubechies 4-Tap filter
template <typename _T>
class DiscreteWaveletTransform2D
{
public:
  //---------------------------------------------------------------------
  /// \brief Constructor to preallocate the memory for the transform
  DiscreteWaveletTransform2D(uint32_t numRows, uint32_t numCols);

  //---------------------------------------------------------------------
  ~DiscreteWaveletTransform2D();

  //---------------------------------------------------------------------
  /// \name   Transform functions
  /// \brief  Functions to perform a discrete wavelet transform or its inverse
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief Compute the Discrete Wavelet Transform of the supplied
  /// input matrix, placing the computed wavelet coefficients into m_data.
  ///
  /// \param x - The discretely sampled signal data
  void transform(const Matrix<_T>& X, uint32_t numSteps);

  //---------------------------------------------------------------------
  /// \brief Compute the inverse Discrete Wavelet Transform of the
  /// supplied wavelet coefficients, placing the reconstructed signal
  /// data into m_data.
  ///
  /// \param coefs - The wavelet coefficients
  void inverse(const Matrix<_T>& coefs, uint32_t numSteps);
  //@}

  //---------------------------------------------------------------------
  /// \name   Data and sub-band functions
  /// \brief  Functions to get information or sub-band data
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief Get the number of rows
  uint32_t getNumRows() const;

  //---------------------------------------------------------------------
  /// \brief Get the number of columns
  uint32_t getNumColumns() const;

  //---------------------------------------------------------------------
  /// \brief Get the coefficient data matrix
  const Matrix<_T>& getData() const;
  Matrix<_T>& getData();

  //---------------------------------------------------------------------
  /// \brief Get the number of sub-bands created by the DWT transform
  uint32_t getNumSubBands(uint32_t numSteps) const;

  //---------------------------------------------------------------------
  /// \brief Get the position and size information for the specified sub-band
  /// created by the DWT transform.
  void getSubBandSize(
    uint32_t  numSteps,
    uint32_t  subBandIndex,      ///< 0 is the coarsest sub-band
    uint32_t& subBandRow,
    uint32_t& subBandCol,
    uint32_t& subBandRowSize,
    uint32_t& subBandColSize) const;

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
  Matrix<_T>                      m_data;   ///< Transformed data from the DWT

  //---------------------------------------------------------------------
  DiscreteWaveletTransform<_T>    m_dwt;    ///< 1D Discrete wavelet transform class
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_DWT_2D_H
