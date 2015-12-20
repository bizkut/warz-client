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
#ifndef NMSOLVER_ETOL
  #error Machine precision tolerance undefined
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
extern uint32_t getSizeX(uint32_t Ac, const Vector<int32_t>& offsets);

//----------------------------------------------------------------------------------------------------------------------
// SolverQR
//----------------------------------------------------------------------------------------------------------------------
template <>
SolverQR<ScalarType>::~SolverQR()
{
}

//----------------------------------------------------------------------------------------------------------------------
// Compute the inverse of the possibly non square matrix A
template <>
SolverQR<ScalarType>::SolverQR(const Matrix<ScalarType>& A) :
  m_A(A.numColumns(), A.numColumns()), m_B(A.numColumns(), A.numColumns()),
  m_X(A.numColumns(), A.numRows()),
  m_sizes(1), m_offsets(1), m_diffs(1),
  m_v(A.numColumns()), m_vc(A.numColumns()), m_p(A.numColumns())
{
  // Initialize defaults
  init();

  // Set the sizes for a single block
  m_sizes[0] = A.numColumns();
  m_offsets[0] = 0;

  // Check if the matrix is square
  if (A.numRows() != A.numColumns())
  {
    Matrix<ScalarType> At(A.numColumns(), A.numRows());
    Matrix<ScalarType> iAtA(A.numColumns(), A.numColumns());

    // Swap contents
    m_At.swap(At);
    m_iAtA.swap(iAtA);
  }

  // Solve the linear system
  solve(A);
}

//----------------------------------------------------------------------------------------------------------------------
// Preallocate memory for matrix inverse system
template <>
SolverQR<ScalarType>::SolverQR(uint32_t Ar, uint32_t Ac) :
  m_A(Ac, Ac), m_B(Ac, Ac), m_X(Ac, Ar),
  m_sizes(1), m_offsets(1), m_diffs(1),
  m_v(Ac), m_vc(Ac), m_p(Ac)
{
  // Initialize defaults
  init();

  // Set the sizes for a single block
  m_sizes[0] = Ac;
  m_offsets[0] = 0;

  // Check if the matrix is square
  if (Ar != Ac)
  {
    Matrix<ScalarType> At(Ac, Ar);
    Matrix<ScalarType> iAtA(Ac, Ac);

    // Swap contents
    m_At.swap(At);
    m_iAtA.swap(iAtA);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Compute the simple linear least squares system A X = B, where A
// is a single non-sparse matrix (generally non-square).
template <>
SolverQR<ScalarType>::SolverQR(
  const Matrix<ScalarType>& A, const Matrix<ScalarType>& B) :
  m_A(A), m_B(B), m_X(A.numColumns(), B.numColumns()),
  m_sizes(1), m_offsets(1), m_diffs(1),
  m_v(A.numRows()), m_vc(A.numRows()), m_p(A.numRows())
{
  // Initialize defaults
  init();

  // Set the sizes for a single block
  m_sizes[0] = A.numRows();
  m_offsets[0] = 0;

  // Compute the solution to the linear system
  m_status = doSolve(m_X);
}

//----------------------------------------------------------------------------------------------------------------------
// Preallocate memory for linear system
template <>
SolverQR<ScalarType>::SolverQR(uint32_t Ar, uint32_t Ac, uint32_t Bc) :
  m_A(Ar, Ac), m_B(Ar, Bc), m_X(Ac, Bc),
  m_sizes(1), m_offsets(1), m_diffs(1),
  m_v(Ar), m_vc(Ar), m_p(Ar)
{
  // Initialize defaults
  init();

  // Set the sizes for a single block
  m_sizes[0] = Ar;
  m_offsets[0] = 0;
}

//----------------------------------------------------------------------------------------------------------------------
// Compute the sparse linear least squares system A X = B, where A is input as a
// stacked set of matrix blocks (each with same width and one block per segment).
// The number of rows in each block is specified by the vector sizes, and the
// corresponding column offset of each block by the vector offsets.
template <>
SolverQR<ScalarType>::SolverQR(
  const Matrix<ScalarType>& A, const Matrix<ScalarType>& B,
  const Vector<int32_t>& sizes, const Vector<int32_t>& offsets) :
  m_A(A), m_B(B),
  m_X(getSizeX(A.numColumns(), offsets), B.numColumns()),
  m_sizes(sizes), m_offsets(offsets), m_diffs(sizes.numElements()),
  m_v(A.numRows()), m_vc(A.numRows()), m_p(A.numRows())
{
  // Initialize defaults
  init();

  // Compute the solution to the block sparse linear system
  m_status = doSolve(m_X);
}

//----------------------------------------------------------------------------------------------------------------------
// Preallocate memory for sparse linear system (using offset vector)
template <>
SolverQR<ScalarType>::SolverQR(
  uint32_t Ar, uint32_t Ac, uint32_t Bc, const Vector<int32_t>& offsets) :
  m_A(Ar, Ac), m_B(Ar, Bc), m_X(getSizeX(Ac, offsets), Bc),
  m_sizes(offsets.numElements()), m_offsets(offsets), m_diffs(offsets.numElements()),
  m_v(Ar), m_vc(Ar), m_p(Ar)
{
  // Initialize defaults
  init();
}

//----------------------------------------------------------------------------------------------------------------------
// Preallocate memory for sparse linear system (using known solution size)
template <>
SolverQR<ScalarType>::SolverQR(
  uint32_t Ar, uint32_t Ac, uint32_t Bc, uint32_t numBlocks, uint32_t Xr) :
  m_A(Ar, Ac), m_B(Ar, Bc), m_X(Xr, Bc),
  m_sizes(numBlocks), m_offsets(numBlocks), m_diffs(numBlocks),
  m_v(Ar), m_vc(Ar), m_p(Ar)
{
  // Check for valid size and offset data
  NMP_ASSERT(numBlocks > 0);

  // Initialize defaults
  init();
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void SolverQR<ScalarType>::init()
{
  m_rank = 0;
  m_status = SolverQRStatus::BadParams;

  // Machine precision tolerance for singularity
  m_eTol = NMSOLVER_ETOL;
}

//----------------------------------------------------------------------------------------------------------------------
template <> SolverQRStatus::eType
SolverQR<ScalarType>::solve(const Matrix<ScalarType>& A)
{
  // Check the preallocated matrices are the correct size
  NMP_ASSERT(A.numRows() == m_X.numColumns());
  NMP_ASSERT(A.numColumns() == m_A.numColumns());

  // Set system of the form: A X = I
  m_B.setIdentity();

  // Check if the matrix is square
  if (A.numRows() != A.numColumns())
  {
    // Compute the pseudo-inverse solution X = (A^T A)^-1 A^T
    m_At.setTranspose(A);
    m_A.setFromMultiplication(m_At, A);

    // Compute the solution to the linear system (A^T A) X = I
    m_status = doSolve(m_iAtA);
    if (m_status != SolverQRStatus::Success) return m_status;

    // Compute (A^T A)^-1 A^T
    m_X.setFromMultiplication(m_iAtA, m_At);
  }
  else
  {
    // Copy the input matrix
    m_A = A;

    // Compute the solution to the linear system
    m_status = doSolve(m_X);
  }

  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
template <> SolverQRStatus::eType
SolverQR<ScalarType>::solve(
  const Matrix<ScalarType>& A, const Matrix<ScalarType>& B)
{
  // Copy the input matrices
  m_A = A;
  m_B = B;

  // Compute the solution to the linear system
  m_status = doSolve(m_X);
  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
template <> SolverQRStatus::eType
SolverQR<ScalarType>::solve(
  const Matrix<ScalarType>& A, const Matrix<ScalarType>& B,
  const Vector<int32_t>& sizes, const Vector<int32_t>& offsets)
{
  // Set the A and B matrix data
  m_A = A;
  m_B = B;

  // Set the block info data
  m_sizes = sizes;
  m_offsets = offsets;

  // Compute the solution to the block sparse linear system
  m_status = doSolve(m_X);
  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
// Perform a sparse linear least squares solve to compute X
template <>
SolverQRStatus::eType SolverQR<ScalarType>::doSolve(Matrix<ScalarType>& X)
{
  NMP_ASSERT(m_A.numRows() == m_B.numRows());

  // Get the basic information
  uint32_t numBlocks = m_sizes.numElements();
  uint32_t width = m_A.numColumns();
  uint32_t nx = X.numRows();

  // Check sizes
  NMP_ASSERT(numBlocks > 0 && m_offsets.numElements() == numBlocks);

  // Zero the output matrix
  X.set(static_cast<ScalarType>(0));

  // Check that the blocks are in the correct range
  NMP_ASSERT(m_offsets[0] == 0);
  NMP_ASSERT((uint32_t)m_offsets[numBlocks-1] == (nx - width));

  // Compute the offset difference vector
  uint32_t n = numBlocks - 1;
  for (uint32_t i = 0; i < n; ++i)
  {
    m_diffs[i] = m_offsets[i+1] - m_offsets[i];
    NMP_ASSERT(m_diffs[i] > 0);
  }
  m_diffs[n] = m_A.numColumns();

  //
  // Decompose matrix A to upper triangular form
  //
  m_rank = 0;

  // Set up the iteration parameters
  int32_t f = 0; // first row index
  int32_t l = -1; // last row index
  uint32_t elim = 0; // column separation between current and next blocks

  // Iterate over the blocks of matrix A
  for (uint32_t j = 0; j < numBlocks; ++j)
  {
    uint32_t blksize;

    // Shift the rows still remaining from the previous block by elim entries to
    // the left, filling the remaining entries on the right with zeros.
    if (f <= l)
    {
      blksize = l - f + 1;
      m_A.shiftColumns(-(int32_t)elim, f, blksize, static_cast<ScalarType>(0));
    }

    // Update the row index of the last element in this block
    l += m_sizes[j];

    // Get the column separation between this block and the next
    elim = m_diffs[j];

    // Perform a QR decomposition on the elements of the current
    // block that we wish to eliminate
    //   [q,r] = qr( A(f:l, 0:elim-1) )
    // Then apply the transpose of q to both sides
    //   A(f:l,:) = q'*A(f:l,:)
    //   B(f:l,:) = q'*B(f:l,:)
    // We can do this elimination explicitly without storing the
    // possibly very large square matrix q, by using Householder
    // reflections. i.e. for each column below the diagonal
    // that we wish to eliminate, compute the transform that zeros
    // the elements and apply the same transform to all other columns
    // on both sides of the equations
    for (uint32_t k = 0; k < elim; ++k, ++f)
    {
      if (l < f) continue; // Note system is singular
      blksize = l - f + 1;

      // Get the norm of the column
      m_A.getColumnVector(k, f, blksize, &m_v[0]);
      ScalarType a = NMP::enorm<ScalarType, ScalarType>(blksize, &m_v[0]);

      // Check for singular condition
      if (a < m_eTol) continue;
      m_rank++;

      // Decide what sign to use for the diagonal element:
      // Avoid the situation where ||a1|| = -a11
      ScalarType c;
      if (m_v[0] < 0)
      {
        m_v[0] -= a;
        c = -m_v[0] * a;
      }
      else
      {
        m_v[0] += a;
        c = m_v[0] * a;
      }

      // Compute the reflection plane vector
      for (uint32_t i = 0; i < blksize; ++i) m_vc[i] = m_v[i] / c;

      // Apply the transform to the A matrix block
      for (uint32_t i = 0; i < width; ++i)
      {
        // Get the column vector p within the block
        m_A.getColumnVector(i, f, blksize, &m_p[0]);

        // v^T * p
        ScalarType qtp = 0;
        for (uint32_t ii = 0; ii < blksize; ++ii) qtp += m_v[ii] * m_p[ii];

        // p - (v^T *p)*vc
        for (uint32_t ii = 0; ii < blksize; ++ii) m_p[ii] -= qtp * m_vc[ii];

        // Replace the column vector in the A matrix
        m_A.setColumnVector(i, f, blksize, &m_p[0]);
      }

      // Apply the transform to the B matrix block
      for (uint32_t i = 0; i < m_B.numColumns(); ++i)
      {
        // Get the column vector p within the block
        m_B.getColumnVector(i, f, blksize, &m_p[0]);

        // v^T * p
        ScalarType qtp = 0;
        for (uint32_t ii = 0; ii < blksize; ++ii) qtp += m_v[ii] * m_p[ii];

        // p - (v^T *p)*vc
        for (uint32_t ii = 0; ii < blksize; ++ii) m_p[ii] -= qtp * m_vc[ii];

        // Replace the column vector in the B matrix
        m_B.setColumnVector(i, f, blksize, &m_p[0]);
      }
    }
  }

  // Check if the system is singular
  if (m_rank < nx) return SolverQRStatus::Singular;

  //
  // Back-substitution step to compute the solution X
  //
  f = nx;
  for (int32_t j = numBlocks - 1; j >= 0; --j)
  {
    // Get the column separation between this block and the next
    elim = m_diffs[j];

    // Get the index of the last element in the current X solution block
    l = f - 1;

    // Get the index of the first element in the current X solution block
    f = f - elim;

    // The current block contains [ Ra Rb 0 ] * [ Xa; Xb; Xc ] = [Ba; Bb; Bc]
    // where this block and the next block overlap in: Rb * Xb = Bb
    // We have already calculated [Xb; Xc] from the previous step, so we can
    // eliminate by back substitution the contribution from Rb * Xb = Bb
    // [ Ra 0 0 ] * [Xa; Xb; Xc] = [(Ba - Rb*Xb); Bb; Bc]
    if (elim < width)
    {
      uint32_t blksize = width - elim;
      for (uint32_t i = 0; i < m_B.numColumns(); ++i)
      {
        // Get the column vector p within the B block that corresponds
        // to the elim elements of Ra
        m_B.getColumnVector(i, f, elim, &m_p[0]);

        // Get the column vector vc within the X block that corresponds
        // to the width-elim elements of Rb
        X.getColumnVector(i, f + elim, blksize, &m_vc[0]);

        // Iterate over the elim rows of Ra
        for (uint32_t k = 0; k < elim; ++k)
        {
          // Get the row vector v within the A block that corresponds
          // to the width-elim elements of Rb
          m_A.getRowVector(f + k, elim, blksize, &m_v[0]);

          // Compute Ba~ = Ba - Rb*Xb
          for (uint32_t kk = 0; kk < blksize; ++kk) m_p[k] -= m_v[kk] * m_vc[kk];
        }

        // Set back the column vector p within the B block that corresponds
        // to the updated elim elements of Ra
        m_B.setColumnVector(i, f, elim, &m_p[0]);
      }
    }

    // Solve the current block Ra*Xa = Ba~, where Ra is upper triangular
    for (uint32_t i = 0; i < m_B.numColumns(); ++i)
    {
      // Get the column vector p within the B block that corresponds
      // to the elim elements of Ra
      m_B.getColumnVector(i, f, elim, &m_p[0]);

      // Iterate over the elim rows of Ra (bottom up)
      for (int32_t k = elim - 1; k >= 0; --k)
      {
        // Get the column vector v within the A block that corresponds
        // the set of elements in Ra that we wish to eliminate
        m_A.getColumnVector(k, f, k + 1, &m_v[0]);

        // Compute the solution to the equation a*x = b
        m_vc[k] = m_p[k] / m_v[k];

        // Eliminate the x coefficient from the left hand side by
        // back-substituting it into the right hand side
        for (int32_t kk = 0; kk < k; ++kk) m_p[kk] -= m_v[kk] * m_vc[k];
      }

      // Set the solution vector X
      X.setColumnVector(i, f, elim, &m_vc[0]);
    }
  }

  return SolverQRStatus::Success;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const Matrix<ScalarType>& SolverQR<ScalarType>::getA() const
{
  return m_A;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType>& SolverQR<ScalarType>::getA()
{
  return m_A;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const Matrix<ScalarType>& SolverQR<ScalarType>::getB() const
{
  return m_B;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType>& SolverQR<ScalarType>::getB()
{
  return m_B;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const Matrix<ScalarType>& SolverQR<ScalarType>::getX() const
{
  return m_X;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType>& SolverQR<ScalarType>::getX()
{
  return m_X;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const Vector<int32_t>& SolverQR<ScalarType>::getSizes() const
{
  return m_sizes;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<int32_t>& SolverQR<ScalarType>::getSizes()
{
  return m_sizes;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const Vector<int32_t>& SolverQR<ScalarType>::getOffsets() const
{
  return m_offsets;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<int32_t>& SolverQR<ScalarType>::getOffsets()
{
  return m_offsets;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType SolverQR<ScalarType>::getETol() const
{
  return m_eTol;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void SolverQR<ScalarType>::setETol(ScalarType eps)
{
  m_eTol = eps;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
SolverQRStatus::eType SolverQR<ScalarType>::getStatus() const
{
  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool SolverQR<ScalarType>::fail() const
{
  return m_status != SolverQRStatus::Success;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t SolverQR<ScalarType>::getRank() const
{
  return m_rank;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
