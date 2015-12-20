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
// DiscreteWaveletTransform2D
//----------------------------------------------------------------------------------------------------------------------
template <>
DiscreteWaveletTransform2D<ScalarType>::DiscreteWaveletTransform2D(uint32_t numRows, uint32_t numCols)
  : m_data(numRows, numCols),
    m_dwt(NMP::maximum(numRows, numCols))
{
  uint32_t n;
  NMP_ASSERT(NMP::minimum(numRows, numCols) >= 4);

  // Check that the number of samples is a power of 2
  n = 0;
  for (uint32_t i = numRows; i > 1; i >>= 1, ++n);
  NMP_ASSERT(numRows == (static_cast<uint32_t>(1) << n));
  n = 0;
  for (uint32_t i = numCols; i > 1; i >>= 1, ++n);
  NMP_ASSERT(numCols == (static_cast<uint32_t>(1) << n));
}

//----------------------------------------------------------------------------------------------------------------------
template <>
DiscreteWaveletTransform2D<ScalarType>::~DiscreteWaveletTransform2D()
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t DiscreteWaveletTransform2D<ScalarType>::getNumRows() const
{
  return m_data.numRows();
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t DiscreteWaveletTransform2D<ScalarType>::getNumColumns() const
{
  return m_data.numColumns();
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const Matrix<ScalarType>& DiscreteWaveletTransform2D<ScalarType>::getData() const
{
  return m_data;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType>& DiscreteWaveletTransform2D<ScalarType>::getData()
{
  return m_data;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void DiscreteWaveletTransform2D<ScalarType>::transform(const Matrix<ScalarType>& X, uint32_t numSteps)
{
  uint32_t numRows = m_data.numRows();
  uint32_t numCols = m_data.numColumns();
  Vector<ScalarType>& buffer = m_dwt.getData();
  ScalarType* v = buffer.data();

  m_data = X;
  for (uint32_t i = 0; i < numSteps; ++i)
  {
    // Apply individual 1D DWT to all coefficient row-vectors
    for (uint32_t j = 0; j < numRows; ++j)
    {
      m_data.getRowVector(j, 0, numCols, v);
      m_dwt.transform(numCols, v, 1);
      m_data.setRowVector(j, 0, numCols, v);
    }

    // Apply individual 1D DWT to all coefficient column-vectors
    for (uint32_t j = 0; j < numCols; ++j)
    {
      m_data.getColumnVector(j, 0, numRows, v);
      m_dwt.transform(numRows, v, 1);
      m_data.setColumnVector(j, 0, numRows, v);
    }

    // Next sub-band block
    numRows >>= 1;
    numCols >>= 1;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void DiscreteWaveletTransform2D<ScalarType>::inverse(const Matrix<ScalarType>& coefs, uint32_t numSteps)
{
  uint32_t numRows = m_data.numRows();
  uint32_t numCols = m_data.numColumns();
  Vector<ScalarType>& buffer = m_dwt.getData();
  ScalarType* v = buffer.data();

  // Compute the size of the lowest level sub-band
  NMP_ASSERT(numRows >> numSteps != 0);
  NMP_ASSERT(numCols >> numSteps != 0);
  numRows >>= numSteps;
  numCols >>= numSteps;

  m_data = coefs;
  for (uint32_t i = 0; i < numSteps; ++i)
  {
    // Next sub-band block
    numRows <<= 1;
    numCols <<= 1;

    // Apply individual 1D inverse DWT to all coefficient row-vectors
    for (uint32_t j = 0; j < numRows; ++j)
    {
      m_data.getRowVector(j, 0, numCols, v);
      m_dwt.inverse(numCols, v, 1);
      m_data.setRowVector(j, 0, numCols, v);
    }

    // Apply individual 1D inverse DWT to all coefficient column-vectors
    for (uint32_t j = 0; j < numCols; ++j)
    {
      m_data.getColumnVector(j, 0, numRows, v);
      m_dwt.inverse(numRows, v, 1);
      m_data.setColumnVector(j, 0, numRows, v);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t DiscreteWaveletTransform2D<ScalarType>::getNumSubBands(uint32_t numSteps) const
{
  return 1 + 3 * numSteps;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void DiscreteWaveletTransform2D<ScalarType>::getSubBandSize(
  uint32_t  numSteps,
  uint32_t  subBandIndex,
  uint32_t& subBandRow,
  uint32_t& subBandCol,
  uint32_t& subBandRowSize,
  uint32_t& subBandColSize) const
{
  NMP_ASSERT(subBandIndex < getNumSubBands(numSteps));

  subBandRowSize = m_data.numRows();
  subBandColSize = m_data.numColumns();
  subBandRow = 0;
  subBandCol = 0;

  for (; numSteps > 0; --numSteps)
  {
    uint32_t numLowerSubBands = getNumSubBands(numSteps - 1);

    // Split the current band in half
    subBandRowSize >>= 1;
    subBandColSize >>= 1;

    // Continue with sub-band splitting?
    if (subBandIndex >= numLowerSubBands)
    {
      NMP_ASSERT(subBandIndex - numLowerSubBands < 3);
      switch (subBandIndex - numLowerSubBands)
      {
      case 0: // HL
        subBandRow = subBandRowSize;
        break;

      case 1: // LH
        subBandCol = subBandColSize;
        break;

      case 2: // HH
        subBandRow = subBandRowSize;
        subBandCol = subBandColSize;
        break;
      }
      break;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t DiscreteWaveletTransform2D<ScalarType>::getSubBandSize(uint32_t numSteps, uint32_t subBandIndex) const
{
  uint32_t subBandRow, subBandCol;
  uint32_t subBandRowSize, subBandColSize;
  getSubBandSize(numSteps, subBandIndex, subBandRow, subBandCol, subBandRowSize, subBandColSize);
  return subBandRowSize * subBandColSize;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void DiscreteWaveletTransform2D<ScalarType>::getSubBandData(
  uint32_t numSteps, uint32_t subBandIndex, ScalarType* data) const
{
  uint32_t subBandRow, subBandCol;
  uint32_t subBandRowSize, subBandColSize;
  getSubBandSize(numSteps, subBandIndex, subBandRow, subBandCol, subBandRowSize, subBandColSize);
  m_data.getSubMatrix(subBandRow, subBandCol, subBandRowSize, subBandColSize, data);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void DiscreteWaveletTransform2D<ScalarType>::setSubBandData(
  uint32_t numSteps, uint32_t subBandIndex, const ScalarType* data)
{
  uint32_t subBandRow, subBandCol;
  uint32_t subBandRowSize, subBandColSize;
  getSubBandSize(numSteps, subBandIndex, subBandRow, subBandCol, subBandRowSize, subBandColSize);
  m_data.setSubMatrix(subBandRow, subBandCol, subBandRowSize, subBandColSize, data);
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
