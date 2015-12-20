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

// Check if the tolerances have been defined
#ifndef NMQR_ETOL
  #error Machine precision tolerance undefined
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// QR
//----------------------------------------------------------------------------------------------------------------------
template <>
QR<ScalarType>::~QR()
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
QR<ScalarType>::QR(const Matrix<ScalarType>& A) : Householder(A)
{
  // Machine precision tolerance for singularity
  m_eps = NMQR_ETOL;

  // Decompose matrix A
  decompose(m_A);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
QR<ScalarType>::QR(uint32_t m, uint32_t n) : Householder(m, n)
{
  // Machine precision tolerance for singularity
  m_eps = NMQR_ETOL;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void QR<ScalarType>::decompose(const Matrix<ScalarType>& A)
{
  // Decompose matrix A into upper triangular form
  m_A = A;
  transformLeft(0);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void QR<ScalarType>::getR(Matrix<ScalarType>& R)
{
  m_A.getUpper(R);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType> QR<ScalarType>::getR()
{
  uint32_t m = m_A.numRows();
  uint32_t n = m_A.numColumns();
  Matrix<ScalarType> R(m, n);
  m_A.getUpper(R);
  return R;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void QR<ScalarType>::getQ(Matrix<ScalarType>& Q)
{
  left(Q, 0);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType> QR<ScalarType>::getQ()
{
  return left(0);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t QR<ScalarType>::getRank()
{
  uint32_t m = m_A.numRows();
  uint32_t n = m_A.numColumns();
  uint32_t rc = minimum(m, n);
  m_A.getDiagVector(0, &m_x[0]);

  uint32_t rank = 0;
  for (uint32_t i = 0; i < rc; ++i)
  {
    if (fabs(m_x[i]) >= m_eps) rank++;
  }

  return rank;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType QR<ScalarType>::getETol() const
{
  return m_eps;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void QR<ScalarType>::setETol(ScalarType eps)
{
  m_eps = eps;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
