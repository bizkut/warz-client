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
#ifndef NM_QR_H
#define NM_QR_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMNumerics/NMHouseholder.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Class for doing QR decomposition
///
template <typename _T>
class QR : public Householder<_T>
{
public:
  //---------------------------------------------------------------------
  /// Constructors
  QR(const Matrix<_T>& A);
  QR(uint32_t m, uint32_t n);

  /// Destructor
  ~QR();

  //---------------------------------------------------------------------
  /// \brief  QR Decomposition of matrix A
  void decompose(const Matrix<_T>& A);

  //---------------------------------------------------------------------
  /// \brief  Get the upper triangular matrix R
  void getR(Matrix<_T>& R);
  Matrix<_T> getR();

  //---------------------------------------------------------------------
  /// \brief  Get the orthogonal matrix Q
  void getQ(Matrix<_T>& Q);
  Matrix<_T> getQ();

  //---------------------------------------------------------------------
  /// \brief  Get the rank of the matrix A
  uint32_t getRank();

  //---------------------------------------------------------------------
  /// \brief  Machine precision tolerance for singularity
  _T getETol() const;
  void setETol(_T eps);

private:
  _T      m_eps;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_QR_H
