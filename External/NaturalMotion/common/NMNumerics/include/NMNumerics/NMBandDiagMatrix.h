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
#ifndef NM_BAND_DIAG_MATRIX_H
#define NM_BAND_DIAG_MATRIX_H

//----------------------------------------------------------------------------------------------------------------------
#include <ostream>
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMatrix.h"
#include "NMPlatform/NMVector.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{
/// \ingroup Maths
/// \brief An arbitrary sized, single banded diagonal matrix class which must contain
/// the main diagonal. The band size is specified by the number of sub diagonals
/// (below the main diagonal) and the number of super diagonals (above the main diagonal).
/// As with the Vector and Matrix classes, these must be constructed with the appropriate
/// dimensions and are never resized.
template <typename _T>
class BandDiagMatrix
{
public:
  /// Constructor / Destructor functions
  ~BandDiagMatrix();
  BandDiagMatrix(); // Empty matrix
  BandDiagMatrix(uint32_t rows, uint32_t cols, uint32_t sub, uint32_t super);
  BandDiagMatrix(const Matrix<_T>& A, uint32_t sub, uint32_t super); // Extract band matrix from A
  BandDiagMatrix(const BandDiagMatrix& copy);

  /// Basic operations (allocates memory)
  BandDiagMatrix operator + (const BandDiagMatrix& v) const;
  BandDiagMatrix operator - (const BandDiagMatrix& v) const;
  Matrix<_T> operator * (const Matrix<_T>& v) const;
  BandDiagMatrix operator * (const BandDiagMatrix& v) const;

  Vector<_T> operator * (const Vector<_T> v) const;
  BandDiagMatrix operator * (_T s) const;
  BandDiagMatrix operator / (_T s) const;

  /// Basic operations (no memory allocated)
  const BandDiagMatrix& operator = (const BandDiagMatrix& copy);
  bool operator == (const BandDiagMatrix& m) const;
  bool operator != (const BandDiagMatrix& m) const;

  void operator += (const BandDiagMatrix& v);
  void operator -= (const BandDiagMatrix& v);
  void operator += (_T s);
  void operator -= (_T s);
  void operator *= (_T s);
  void operator /= (_T s);

  /// Set matrix from multiplication: M = A * B (no memory allocated)
  /// Useful when A and B are non-square and banded. The multiplication
  /// produces a A.rows by B.cols matrix with a A.sub+B.sub, A.super+B.super
  /// diagonal band
  void setFromMultiplication(const BandDiagMatrix& A, const BandDiagMatrix& B);

  /// Set matrix from multiplication: M = A * B (no memory allocated)
  /// Useful when A and B are non-square. We also assume that the resulting
  /// matrix is square and symmetric, thus only half the multiplications are
  /// required to compute the upper band. The lower band is then copied from
  /// elements in the upper band. The multiplication produces a A.rows
  /// symmetric square matrix with a A.super+B.super upper & lower diagonal band
  void setFromMultiplicationSym(const BandDiagMatrix& A, const BandDiagMatrix& B);

  /// Multiply band matrix by diagonal matrix: M = Diag(v) * A (no memory allocated)
  void multiplyDiag(const _T* v);

  /// Compute band matrix - ? multiplication: v = A * u (no memory allocated)
  void multiply(_T* v, const _T* u) const;
  void multiply(Matrix<_T>& v, const Matrix<_T>& u) const;

  /// Compute ? - band matrix multiplication: v = u * A
  void postMultiply(_T* v, const _T* u) const; // (no memory allocated)
  void postMultiply(Matrix<_T>& v, const Matrix<_T>& u) const; // (allocates memory)

  /// Per element matrix functions (no memory allocated)
  // Threshold  -> 0 or 1
  void greater(const BandDiagMatrix& v);
  void greater(_T v);
  void less(const BandDiagMatrix& v);
  void less(_T v);

  /// absolute values
  void absolute();

  /// element-wise multiply
  void multiply(const BandDiagMatrix& v);

  /// Diagonal banded data is stored as a series of columns, so references
  /// to zero elements outside the band are not possible
  _T element(uint32_t rowIndex, uint32_t colIndex) const;
  void element(uint32_t rowIndex, uint32_t colIndex, _T val);

  uint32_t numRows() const;
  uint32_t numColumns() const;
  bool isSize(uint32_t r, uint32_t c) const;
  bool isEmpty() const;

  uint32_t numSub() const;
  uint32_t numSuper() const;
  uint32_t bandWidth() const;

  /// Get access to the internal storage data. Not sure we should allow this.
  /// However it's needed during the unit tests to check the data.
  const Matrix<_T>& data() const;

  /// Matrix property functions
  _T trace() const;

  /// \brief Whole matrix data functions
  void swap(BandDiagMatrix& A); // Swaps contents with band matrix A

  Matrix<_T> full() const;
  void get(Matrix<_T>& A) const; // extract the valid band to matrix A
  void set(const Matrix<_T>& A); // extract the valid band from matrix A

  void set(_T val);
  void setIdentity();

  /// \brief Column and row vector functions
  Vector<_T> getRowVector(uint32_t i) const;
  void getRowVector(uint32_t i, _T* data) const;
  void getRowVector(uint32_t r, uint32_t c, uint32_t cn, _T* data) const;

  Vector<_T> getColumnVector(uint32_t i) const;
  void getColumnVector(uint32_t i, _T* data) const;
  void getColumnVector(uint32_t c, uint32_t r, uint32_t rn, _T* data) const;

  /// Set elements within band
  void setRowVector(uint32_t i, _T val);
  void setRowVector(uint32_t i, const Vector<_T>& v);
  void setRowVector(uint32_t i, const _T* data);
  void setRowVector(uint32_t r, uint32_t c, uint32_t cn, const _T* data);
  void setRowVector(uint32_t r, uint32_t c, uint32_t cn, _T val);

  void setColumnVector(uint32_t i, _T val);
  void setColumnVector(uint32_t i, const Vector<_T>& v);
  void setColumnVector(uint32_t i, const _T* data);
  void setColumnVector(uint32_t c, uint32_t r, uint32_t rn, const _T* data);
  void setColumnVector(uint32_t c, uint32_t r, uint32_t rn, _T val);

  void multiplyRow(uint32_t i, _T s);
  void multiplyColumn(uint32_t, _T s);

  /// \brief Diagonal vector functions, where i is the sub/super diagonal offset
  /// from the main diagonal. sub (lower) diagonals i<0, super (upper) diagonals i>0
  uint32_t getDiagLength(int32_t i) const;
  int32_t getDiagIndex(uint32_t r, uint32_t c) const;
  void getDiagIndex(int32_t i, uint32_t& r, uint32_t& c) const;

  const _T& getDiagElement(int32_t i, uint32_t k) const;
  _T& getDiagElement(int32_t i, uint32_t k);

  void getDiagVector(int32_t i, _T* data) const;
  void getDiagVector(uint32_t rowIndex, uint32_t colIndex, uint32_t dn, _T* data) const;
  Vector<_T> getDiagVector(int32_t i) const;

  void setDiagVector(int32_t i, _T val);
  void setDiagVector(int32_t i, const _T* data);
  void setDiagVector(uint32_t rowIndex, uint32_t colIndex, uint32_t dn, const _T* data);
  void addDiagVector(int32_t i, _T val);

  /// \brief Frobenius norm of the matrix
  _T norm() const;

  /// \brief Transpose of matrix
  void transpose(); // inplace

  BandDiagMatrix getTranspose() const;
  void getTranspose(BandDiagMatrix& mat) const;
  void setTranspose(const BandDiagMatrix& mat);

  /// \brief Upper and lower triangle functions
  void getUpper(BandDiagMatrix& mat) const;
  void getLower(BandDiagMatrix& mat) const;
  BandDiagMatrix getUpper() const;
  BandDiagMatrix getLower() const;

  void setUpper(const BandDiagMatrix& mat);
  void setLower(const BandDiagMatrix& mat);
  void setUpper(_T s);
  void setLower(_T s);

  /// \brief Copy upper triangle band into lower triangle band
  void makeSymmetric();

  /// \brief Comparison function with tolerance
  bool isZero(_T threshold) const;
  bool isEqual(const BandDiagMatrix& mat, _T threshold) const;

protected:
  /// \brief Given the diagonal offset index i (from the main diagonal) this
  /// function returns the row, column indices within the storage matrix m_data
  /// and the size of the matrix diagonal.
  void getDiag(int32_t i, uint32_t& r, uint32_t& c, uint32_t& s) const;

  /// \brief Given the row index i this function returns the row, column indices (rs, cs)
  /// within the storage matrix m_data and the corresponding valid column, size (cm, sm)
  /// offsets within the real matrix. returns the required increment offset for elements.
  int32_t getRow(uint32_t i, uint32_t& rs, uint32_t& cs, uint32_t& cm, uint32_t& sm) const;

  ///\brief Given the column index i this function returns the row, column indices (rs, cs)
  /// within the storage matrix m_data and the corresponding valid row, size (rm, sm)
  /// offsets within the real matrix. returns the required increment offset for elements.
  int32_t getColumn(uint32_t i, uint32_t& rs, uint32_t& cs, uint32_t& rm, uint32_t& sm) const;

  /// \brief Inner loop for matrix - matrix multiplication that computes values
  /// down the specified diagonal i
  void setFromMultiplication(int32_t i, const BandDiagMatrix& A, const BandDiagMatrix& B);

protected:
  // Banded matrix size information
  uint32_t                m_numRows;
  uint32_t                m_numColumns;
  uint32_t                m_numSub, m_numSuper;

  // Storage data min(r,c) by (sub+super+1)
  // if r >= c then the super diagonals are stored in the lower columns of m_data,
  // and the sub diagonals are stored in the upper columns of m_data.
  // if r < c then the super diagonals are stored in the upper columns of m_data,
  // and the sub diagonals are stored in the lower columns of m_data.
  Matrix<_T>              m_data;
};

template <typename _T>
std::ostream& operator<<(std::ostream& os, const BandDiagMatrix<_T>& mat);

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_BAND_DIAG_MATRIX_H
