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
#ifndef ScalarType
  #error This file should not be compiled directly
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t removeMultiplicities(Matrix<ScalarType>& Y_, Vector<ScalarType>& x_)
{
  uint32_t n_ = x_.numElements();
  uint32_t dims = Y_.numColumns();
  NMP_ASSERT(Y_.numRows() == n_);

  // Find the initial data sample difference vector
  Vector<ScalarType> dx_(n_ - 1);
  uint32_t n = 1;
  for (uint32_t i = 0; i < n_ - 1; ++i)
  {
    dx_[i] = x_[i+1] - x_[i];
    NMP_ASSERT(dx_[i] >= 0);
    if (dx_[i] > NMSPLUTLS_EPS) n++;
  }
  if (n == n_) return 0;

  Vector<ScalarType> va(dims);
  Vector<ScalarType> vb(dims);

  // Remove all the repeated data samples by replacing repeated samples with the
  // average of the corresponding Y_ data.
  Vector<ScalarType> x(n);
  Matrix<ScalarType> Y(n, dims);

  n = 0;
  for (uint32_t i = 0; i < n_; )
  {
    // Current values
    x[n] = x_[i];
    Y_.getRowVector(i, va);

    // Compute summation of repeated elements
    uint32_t k = 1;
    for (++i; i < n_; ++i)
    {
      if (dx_[i-1] > NMSPLUTLS_EPS) break;
      Y_.getRowVector(i, vb);
      va += vb;
      k++;
    }

    // Set data sample
    va /= (ScalarType)k;
    Y.setRowVector(n, va);

    // Update data sample count
    n++;
  }

  // Swap contents of data samples
  Y_.swap(Y);
  x_.swap(x);

  return n_ - n;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t removeMultiplicities(Matrix<ScalarType>& Y_, Vector<ScalarType>& x_, Vector<ScalarType>& w_)
{
  uint32_t n_ = x_.numElements();
  uint32_t dims = Y_.numColumns();
  NMP_ASSERT(Y_.numRows() == n_);
  NMP_ASSERT(w_.numElements() == n_);

  // Find the initial data sample difference vector
  Vector<ScalarType> dx_(n_ - 1);
  uint32_t n = 1;
  for (uint32_t i = 0; i < n_ - 1; ++i)
  {
    dx_[i] = x_[i+1] - x_[i];
    NMP_ASSERT(dx_[i] >= 0);
    if (dx_[i] > NMSPLUTLS_EPS) n++;
  }
  if (n == n_) return 0;

  Vector<ScalarType> va(dims);
  Vector<ScalarType> vb(dims);

  // Remove all the repeated data samples by replacing repeated samples with the
  // weighted average of the corresponding Y data. The weight factor is replaced with
  // the corresponding weight sum.
  Vector<ScalarType> x(n);
  Matrix<ScalarType> Y(n, dims);
  Vector<ScalarType> w(n);

  n = 0;
  for (uint32_t i = 0; i < n_; )
  {
    // Current values
    ScalarType sx = x_[i];
    Y_.getRowVector(i, va);
    va *= w_[i];
    ScalarType sw = w_[i];

    // Compute summation of repeated elements
    uint32_t k = 1;
    for (++i; i < n_; ++i)
    {
      if (dx_[i-1] > NMSPLUTLS_EPS) break;
      sx += x_[i];
      Y_.getRowVector(i, vb);
      vb *= w_[i];
      va += vb;
      sw += w_[i];
      k++;
    }

    // Set data sample
    x[n] = sx / k;
    va /= sw;
    Y.setRowVector(n, va);
    w[n] = sw;

    // Update data sample count
    n++;
  }

  // Swap contents of data samples
  Y_.swap(Y);
  x_.swap(x);
  w_.swap(w);

  return n_ - n;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
