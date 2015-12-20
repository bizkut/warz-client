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
#ifndef NM_MATRIX_H
#define NM_MATRIX_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMVector.h"

//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::Matrix
/// \ingroup NaturalMotionPlatformMath
/// \brief A generic Matrix class that can have arbitrary dimension.
///
/// This is NOT intended to be used for fast matrix operations, and Matrix34 and related classes
/// should be used where possible.  As with the Vector class, these must be
/// constructed with the appropriate dimensions and are never resized.
///
/// Note that since the size cannot be determined at compile time, operator *= must
/// allocate, and therefore mat1 *= mat2 is no faster than mat1 = mat1 * mat2.
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
class Matrix
{
public:
  /// \brief Constructor / Destructor functions.
  ~Matrix();
  Matrix(); // Empty matrix
  Matrix(uint32_t rows, uint32_t cols);
  Matrix(uint32_t rows, uint32_t cols, T value);
  Matrix(uint32_t rows, uint32_t cols, const T* data);
  Matrix(const Matrix& copy);

  /// \brief Constructors for all fixed-size types.
  Matrix(const Matrix34& matrix);

  /// \brief Allows a Matrix of one type to be constructed from one of another type
  template <typename Q>
  Matrix(const Matrix<Q>& other);

  /// \brief Output matrix in a file (for debug purpose mainly)
  void fwritef(const char* filename) const;

  /// \brief Matrix size and element access functions.
  ///
  /// Data is stored with as a series of columns (consistent with the fast math types),
  /// so operators can't easily be provided for [row][column] type indexing.
  T& element(uint32_t rowIndex, uint32_t colIndex);
  const T& element(uint32_t rowIndex, uint32_t colIndex) const;
  T& element(uint32_t index);
  const T& element(uint32_t index) const;

  T* data();
  const T* data() const;

  uint32_t numRows() const;
  uint32_t numColumns() const;
  uint32_t numElements() const;
  bool isSize(uint32_t r, uint32_t c) const;
  bool isEmpty() const;
  bool isSquare() const;

  /// \brief Basic operations (allocates memory).
  Matrix operator + (const Matrix& v) const;
  Matrix operator - (const Matrix& v) const;
  Matrix operator * (const Matrix& v) const;
  Vector<T> operator * (const Vector<T> v) const;
  Matrix operator * (T s) const;
  Matrix operator / (T s) const;

  /// \brief Square matrix-matrix multiply.
  void operator *= (const Matrix& v);

  /// \brief Basic operations (no memory allocated).
  const Matrix& operator = (const Matrix& copy);
  bool operator == (const Matrix& m) const;
  bool operator != (const Matrix& m) const;

  void operator += (const Matrix& v);
  void operator -= (const Matrix& v);
  void operator += (T s);
  void operator -= (T s);
  void operator *= (T s);
  void operator /= (T s);

  /// \brief Copy upper triangle into lower triangle.
  void makeSymmetric();

  /// \brief Set matrix from multiplication: M = A * B (no memory allocated).
  ///
  /// Useful when A and B are non-square.
  Matrix<T>& setFromMultiplication(const Matrix& A, const Matrix& B);

  /// \brief Set matrix from multiplication: M = A^T * A (no memory allocated).
  ///
  /// Only the first n rows of A are used in the multiplication.
  Matrix<T>& setFromMultiplicationAtA(uint32_t n, const Matrix& A);
  Matrix<T>& setFromMultiplicationAtA(const Matrix& A) { return setFromMultiplicationAtA(A.m_numRows, A); }

  /// \brief Set matrix from multiplication: M = coeff * A (no memory allocated).
  Matrix<T>& setFromMultiplication(T coeff, const Matrix& A);

  Matrix<T>& setFromDiff(const Matrix& A, const Matrix& B);
  Matrix<T>& setFromAdd(const Matrix& A, const Matrix& B);

  bool inverse(const Matrix& A, Matrix& tmpMat, T epsilon = (T)0.001);

  /// \brief Set matrix from multiplication: M = A * B (no memory allocated).
  ///
  /// Useful when A and B are non-square. We also assume that the resulting
  /// matrix is symmetric, thus only half the multiplications are required
  /// to compute the upper triangle. The lower triangle is then copied from
  /// elements in the upper triangle.
  void setFromMultiplicationSym(const Matrix& A, const Matrix& B);

  /// \brief Multiply matrix by diagonal matrix: M = Diag(v) * A (no memory allocated).
  void multiplyDiag(const T* v);

  /// \brief Multiply matrix by diagonal matrix: M = A * Diag(v) (no memory allocated).
  void postMultiplyDiag(const T* v);

  /// \brief Compute matrix - vector multiplication: v = A * u (no memory allocated).
  void preMultiply(uint32_t n, T* v, const T* u) const; // First n rows of multiply
  void preMultiply(T* v, const T* u) const { preMultiply(m_numRows, v, u); }
  void preMultiply(Vector<T>& v, const Vector<T>& u) const { preMultiply(m_numRows, v.data(), u.data()); }

  /// \brief Compute matrix - vector multiplication: v = A^T * u (no memory allocated).
  ///
  /// Where n is the number of elements in vector u (the remaining are assumed to be zero).
  void preMultiplyTranspose(uint32_t n, T* v, const T* u) const;
  void preMultiplyTranspose(Vector<T>& v, const Vector<T>& u) const { preMultiplyTranspose(m_numRows, v.data(), u.data()); }

  /// \brief Compute vector - matrix multiplication: v = u * A (no memory allocated).
  void postMultiply(T* v, const T* u) const { preMultiplyTranspose(m_numRows, v, u); }
  void postMultiply(Vector<T>& v, const Vector<T>& u) const { preMultiplyTranspose(m_numRows, v.data(), u.data()); }

  /// \brief Set matrix from the outer product of two vectors M = u * v^T
  void outer(const T* u, const T* v);

  /// \brief Update matrix from the outer product of two vectors M' = M + u * v^T
  void outerUpdate(const T* u, const T* v);

  /// \brief Per element matrix functions.
  ///
  /// Threshold  -> 0 or 1.
  void greater(const Matrix& v);
  void greater(T v);
  void less(const Matrix& v);
  void less(T v);
  void clamp(T valMin, T valMax);

  /// \brief Absolute values.
  void absolute();

  /// \brief Zero small absolute values below threshold.
  void zeroSmall(T v);

  /// \brief Element-wise multiply.
  void multiply(const Matrix& v);

  /// \brief Element-wise column vector multiply: M = U .* A
  ///
  /// Where U = v * ones(1, A.cols) and v is a A.rows by 1 column vector.
  void multiplyColumns(const T* v);

  /// \brief Element-wise row vector multiply: M = U .* A
  ///
  /// Where U = ones(A.rows, 1) * v and v is a 1 by A.cols row vector.
  void multiplyRows(const T* v);

  /// \brief Matrix property functions
  T determinant() const;
  T trace() const;

  T maximum() const;
  T minimum() const;

  /// \brief Whole matrix data functions
  void swap(Matrix& A); // Swaps contents with matrix A

  void get(T* data) const;
  void set(const T* data);

  template <typename Q>
  void get(Q* data) const;

  template <typename Q>
  void set(const Q* data);

  void set(T val);
  void setIdentity();
  void set(const Matrix34& src);

  void cumulativeRowSum();
  void cumulativeColumnSum();

  Matrix diffRows() const;
  void diffRows(Matrix& mat) const;

  Matrix diffColumns() const;
  void diffColumns(Matrix& mat) const;

  /// \brief  Sub-Matrix functions
  Matrix getMatrixWithoutRow(uint32_t rowIndex) const;
  Matrix getMatrixWithoutColumn(uint32_t colIndex) const;

  Matrix getSubMatrix(
    uint32_t startRow, uint32_t startColumn,
    uint32_t rows, uint32_t columns) const;

  void getSubMatrix(
    uint32_t startRow, uint32_t startColumn,
    uint32_t rows, uint32_t columns, T* data) const;

  void getSubMatrix(
    uint32_t startRow, uint32_t startColumn,
    Matrix& subMatrix) const;

  void setSubMatrix(
    uint32_t startRow, uint32_t startColumn,
    const Matrix& subMatrix);

  void setSubMatrix(
    uint32_t srcStartRow, uint32_t srcStartColumn,
    uint32_t dstStartRow, uint32_t dstStartColumn,
    uint32_t rows, uint32_t columns, const Matrix& subMatrix);

  void setSubMatrix(
    uint32_t startRow, uint32_t startColumn,
    uint32_t rows, uint32_t columns, const T* data);

  void setSubMatrix(
    uint32_t startRow, uint32_t startColumn,
    uint32_t rows, uint32_t columns, T s);

  /// \brief Column and row vector functions.
  Vector<T> getRowVector(uint32_t i) const;
  void getRowVector(uint32_t i, Vector<T>& v) const;
  void getRowVector(uint32_t i, T* data) const;
  void getRowVector(uint32_t r, uint32_t c, uint32_t cn, T* data) const;

  Vector<T> getColumnVector(uint32_t i) const;
  void getColumnVector(uint32_t i, Vector<T>& v) const;
  void getColumnVector(uint32_t i, T* data) const;
  void getColumnVector(uint32_t c, uint32_t r, uint32_t rn, T* data) const;

  void setRowVector(uint32_t i, T val);
  void setRowVector(uint32_t i, const Vector<T>& v);
  void setRowVector(uint32_t i, const T* data);
  void setRowVector(uint32_t r, uint32_t c, uint32_t cn, const T* data);
  void setRowVector(uint32_t r, uint32_t c, uint32_t cn, T val);

  void setColumnVector(uint32_t i, T val);
  void setColumnVector(uint32_t i, const Vector<T>& v);
  void setColumnVector(uint32_t i, const T* data);
  void setColumnVector(uint32_t c, uint32_t r, uint32_t rn, const T* data);
  void setColumnVector(uint32_t c, uint32_t r, uint32_t rn, T val);

  void multiplyRow(uint32_t r, T s);
  void multiplyRow(uint32_t r, uint32_t c, uint32_t cn, T s);

  void multiplyColumn(uint32_t c, T s);
  void multiplyColumn(uint32_t c, uint32_t r, uint32_t rn, T s);

  void addRow(uint32_t r, T s);
  void addRow(uint32_t r, uint32_t c, uint32_t cn, T s);

  void addColumn(uint32_t c, T s);
  void addColumn(uint32_t c, uint32_t r, uint32_t rn, T s);

  /// \brief Column and row vector summation functions.
  T getRowSum(uint32_t r) const;
  Vector<T> getColumnVectorSum() const;
  void getColumnVectorSum(T* data) const;
  void getColumnVectorSum(Vector<T>& v) const;

  T getColumnSum(uint32_t c) const;
  Vector<T> getRowVectorSum() const;
  void getRowVectorSum(T* data) const;
  void getRowVectorSum(Vector<T>& v) const;

  /// \brief Diagonal vector functions, where i is the sub/super diagonal offset
  /// from the main diagonal. sub (lower) diagonals i<0, super (upper) diagonals i>0
  uint32_t getDiagLength(int32_t i) const;
  void getDiagVector(int32_t i, T* data) const;
  void getDiagVector(uint32_t r, uint32_t c, uint32_t dn, T* data) const;
  Vector<T> getDiagVector(int32_t i) const;

  void setDiagVector(int32_t i, T val);
  void setDiagVector(int32_t i, const T* data);
  void setDiagVector(uint32_t r, uint32_t c, uint32_t dn, const T* data);

  void addDiagVector(int32_t i, T val);
  void multiplyDiagVector(int32_t i, T val);

  /// \brief Row and column shifting functions.
  ///
  /// Shift entire rows bands up/down.
  void shiftRows(int32_t shift, T fill);
  void shiftRows(int32_t shift, uint32_t c, uint32_t cn, T fill);

  /// \brief Shift entire column bands left/right.
  void shiftColumns(int32_t shift, T fill);
  void shiftColumns(int32_t shift, uint32_t r, uint32_t rn, T fill);

  /// \brief Shift elements in single row left/right.
  void shiftRowElements(uint32_t r, int32_t shift, T fill);

  /// \brief Shift elements in single column up/down.
  void shiftColumnElements(uint32_t c, int32_t shift, T fill);

  /// \brief Row and column swapping functions.
  void swapRows(uint32_t r1, uint32_t r2);
  void swapRows(uint32_t r1, uint32_t r2, uint32_t c, uint32_t cn);

  void swapColumns(uint32_t c1, uint32_t c2);
  void swapColumns(uint32_t c1, uint32_t c2, uint32_t r, uint32_t rn);

  /// \brief Symmetric and asymmetric parts of a square matrix.
  Matrix sym() const;
  Matrix asym() const;
  void sym(const Matrix& src);
  void asym(const Matrix& src);

  /// \brief Apply a Gauss transform M = I - v*(e_k)^{ T } to the matrix.
  ///
  /// Where I is the identity matrix; e_k is the canonical column vector containing all
  /// zeros with a single one in row k; and v is the Gauss vector (column) whose first k
  /// elements are zero. M is unit lower triangular, and if the Gauss vector has the form
  /// v = (0 ... 0 | a_{ k+1, k }/a_{ k, k }, ... , a_{ n-1, k }/a_{ k, k }) then the transform zeros
  /// elements below the diagonal in column k.
  /// \param k - the column in which to place the Gauss vector.
  /// \param v - the set of non zero multipliers within the Gauss vector, i.e. pointer to data
  ///  after the first k zeros. Vector v is assumed to be of length A.rows - k.
  /// \param c, cn - column index c and width cn of column band within A to apply transform to.
  void gaussTransform(uint32_t k, const T* v, uint32_t c, uint32_t cn);

  /// \brief Frobenius norm of the matrix.
  T norm() const;

  /// \brief Transpose of matrix.
  void transpose(); // in place, for square matrices.

  Matrix getTranspose() const;
  void getTranspose(Matrix& mat) const;
  void setTranspose(const Matrix& mat);

  /// \brief Upper and lower triangle functions.
  void getUpper(Matrix& mat) const;
  void getLower(Matrix& mat) const;
  Matrix getUpper() const;
  Matrix getLower() const;

  void setUpper(const Matrix& mat);
  void setLower(const Matrix& mat);
  void setUpper(T s);
  void setLower(T s);

  /// \brief Band diagonal matrix functions.
  void getBandDiag(Matrix& mat, uint32_t sub, uint32_t super) const;
  void setBandDiag(const Matrix& mat, uint32_t sub, uint32_t super);

  /// \brief TM and RM specific functions.
  Matrix tmInverse() const;
  Matrix rmInverse() const;

  /// \brief TM inversion function which inverts without allocation.
  ///
  /// tempRot should be a 3x3 mtx, tempPos0 a 3x1 and tempPos1 a 3x1
  void tmSetFromInverse(
    const Matrix& src,
    Matrix& tempRot,
    Matrix& tempPos0,
    Matrix& tempPos1);

  /// \brief Comparison function with tolerance.
  bool isZero(T threshold) const;
  bool isEqual(const Matrix& mat, T threshold) const;

protected:
  /// \brief Given the diagonal offset index i (from the main diagonal) this
  /// function returns the row, column indices and size of the matrix diagonal.
  void getDiag(int32_t i, uint32_t& r, uint32_t& c, uint32_t& s) const;

protected:

  uint32_t     m_numRows;
  uint32_t     m_numColumns;
  T*          m_elements;

};

template <typename T>
Matrix<T> operator* (T s, const Matrix<T>& mat); // inline

template <typename T>
Vector<T> operator* (const Vector<T>& v, const Matrix<T>& mat);

/// Utility functions such as decomposition, specialized inversions, etc.
template <typename T>
bool Inverse2x2(const Matrix<T>& A, Matrix<T>& iA);

template <typename T>
Matrix<T> Inverse2x2(const Matrix<T>& mat);

template <typename T>
Matrix<T> PseudoInverse(const Matrix<T>& mat);

} // namespace NMP

#include "NMPlatform/NMMatrix.inl"

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_MATRIX_H
//----------------------------------------------------------------------------------------------------------------------
