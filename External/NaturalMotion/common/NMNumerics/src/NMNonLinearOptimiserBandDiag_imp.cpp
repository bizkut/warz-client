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

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// JacobianBlockDiag
//----------------------------------------------------------------------------------------------------------------------
template <>
JacobianBlockDiag<ScalarType>::JacobianBlockDiag(
  uint32_t numRows, uint32_t numColumns, uint32_t numBlocks, uint32_t blockWidth) :
  JacobianBase(numRows, numColumns),
  m_matrix(numRows, blockWidth),
  m_numBlocks(numBlocks),
  m_blockInfo(0)
{
  NMP_ASSERT(numColumns > 1);

  // Allocate the memory for the Jacobian block information
  NMP_ASSERT(numBlocks > 0);
  m_blockInfo = new JacobianBlockDiagInfo[numBlocks];

  for (uint32_t i = 0; i < numBlocks; ++i)
  {
    m_blockInfo[i].clear();
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
JacobianBlockDiag<ScalarType>::~JacobianBlockDiag()
{
  if (m_blockInfo)
  {
    delete[] m_blockInfo;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void JacobianBlockDiag<ScalarType>::setBlockInfo(uint32_t i, uint32_t jRow, uint32_t jColumn, uint32_t jSize)
{
  NMP_ASSERT(i < m_numBlocks);
  JacobianBlockDiagInfo& info = m_blockInfo[i];
  info.m_jRow = jRow;
  info.m_jColumn = jColumn;
  info.m_jSize = jSize;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void JacobianBlockDiag<ScalarType>::getBlocksInColumn(uint32_t col, uint32_t& startBlock, uint32_t& endBlock) const
{
  NMP_ASSERT(col < m_numColumns);

  // Find a block that bounds the column index (binary search)
  uint32_t width = m_matrix.numColumns();
  uint32_t high = m_numBlocks;
  uint32_t low = 0;
  uint32_t indx, ca;

  indx = (high + low) >> 1;
  while (true)
  {
    ca = m_blockInfo[indx].m_jColumn;
    if (col >= ca && col < ca + width) break; // Found bounding block

    if (col < ca)
    {
      high = indx;
    }
    else
    {
      low = indx;
    }

    // Update next search block index
    NMP_ASSERT(low != high);
    indx = (high + low) >> 1;
  }

  // Find the first block that contains the column index
  for (startBlock = indx; startBlock > 0; --startBlock)
  {
    // Test preceding block
    ca = m_blockInfo[startBlock-1].m_jColumn;
    if (ca + width <= col) break;
  }

  // Find the last block that contains the column index
  high = m_numBlocks - 1;
  for (endBlock = indx; endBlock < high; ++endBlock)
  {
    // Test proceeding block
    ca = m_blockInfo[endBlock+1].m_jColumn;
    if (ca > col) break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void JacobianBlockDiag<ScalarType>::makeFull(NMP::Matrix<ScalarType>& result) const
{
  uint32_t numRows = getNumRows();
  uint32_t numColumns = getNumColumns();
  uint32_t numBlocks = getNumBlocks();
  uint32_t blockWidth = getBlockWidth();
  NMP::Matrix<ScalarType> A(numRows, numColumns);
  A.set(0.0);

  const NMP::Matrix<double>& JacA = getMatrix();
  const JacobianBlockDiagInfo* blockInfo = getBlockInfo();
  for (uint32_t i = 0; i < numBlocks; ++i)
  {
    uint32_t ra = blockInfo[i].m_jRow;
    uint32_t jSize = blockInfo[i].m_jSize;
    uint32_t ca = blockInfo[i].m_jColumn;
    A.setSubMatrix(ra, 0, ra, ca, jSize, blockWidth, JacA);
  }

  result.swap(A);
}

//----------------------------------------------------------------------------------------------------------------------
// HessianBandDiag
//----------------------------------------------------------------------------------------------------------------------
template <>
HessianBandDiag<ScalarType>::HessianBandDiag(uint32_t size, uint32_t super) :
  HessianBase(size), m_matrix(size, size, 0, super)
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
HessianBandDiag<ScalarType>::~HessianBandDiag()
{
}

//----------------------------------------------------------------------------------------------------------------------
// NonLinearOptimiserBandDiag
//----------------------------------------------------------------------------------------------------------------------
template <>
NonLinearOptimiserBandDiag<ScalarType>::NonLinearOptimiserBandDiag(
  uint32_t numBlocks,
  uint32_t blockWidth,
  NonLinearLeastSquaresFuncJac<ScalarType>& funcJac) :
  NonLinearOptimiserBase<ScalarType>(funcJac),
  m_solver(funcJac.getNumUnknowns(), blockWidth - 1, 1),
  m_temp(funcJac.getMaxNumResiduals()),
  m_temp2(funcJac.getMaxNumResiduals())
{
  NMP_ASSERT(blockWidth > 1);
  uint32_t numRows = funcJac.getMaxNumResiduals();
  uint32_t numColumns = funcJac.getNumUnknowns();

  // Allocate a new Jacobian matrix that has an almost block diagonal form
  m_J = new JacobianBlockDiag<ScalarType>(numRows, numColumns, numBlocks, blockWidth);

  // Allocate a new Hessian matrix that has a band-diagonal form resulting from the
  // Jacobian multiplication H = J^T * J
  m_H = new HessianBandDiag<ScalarType>(numColumns, blockWidth - 1);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
NonLinearOptimiserBandDiag<ScalarType>::~NonLinearOptimiserBandDiag()
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool NonLinearOptimiserBandDiag<ScalarType>::computeJByFiniteDifference(
  const NMP::Vector<ScalarType>& NMP_UNUSED(P),
  const NMP::Vector<ScalarType>& NMP_UNUSED(fdDeltas),
  const NMP::Vector<ScalarType>& NMP_UNUSED(fRes),
  NonLinearLeastSquaresFunc<ScalarType>& NMP_UNUSED(lsqFn),
  NMP::JacobianBase<ScalarType>* NMP_UNUSED(J))
{
  NMP_ASSERT_FAIL();
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void NonLinearOptimiserBandDiag<ScalarType>::getJabsRowSums(
  uint32_t NMP_UNUSED(numResiduals),
  const NMP::JacobianBase<ScalarType>* NMP_UNUSED(J),
  NMP::Vector<ScalarType>& NMP_UNUSED(result))
{
  NMP_ASSERT_FAIL();
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void NonLinearOptimiserBandDiag<ScalarType>::computeJtr(
  uint32_t                             numResiduals,
  const NMP::JacobianBase<ScalarType>* J,
  const NMP::Vector<ScalarType>&       r,
  NMP::Vector<ScalarType>&             result)
{
  NMP_ASSERT(J);
  const NMP::JacobianBlockDiag<ScalarType>& Jac = static_cast< const NMP::JacobianBlockDiag<ScalarType>& >(*J);
  const NMP::Matrix<ScalarType>& A = Jac.getMatrix();
  NMP_ASSERT(Jac.getNumBlocks() > 0);
  const JacobianBlockDiagInfo* blockInfo = Jac.getBlockInfo();
  NMP_ASSERT(blockInfo);

  uint32_t c, ca, cb, ra, rb, nr;
  ScalarType* u = m_temp.data();
  const ScalarType* v;

  uint32_t numColumns = Jac.getNumColumns();
  result.set(static_cast<ScalarType>(0));

  for (uint32_t j = 0; j < numColumns; ++j)
  {
    // Find the first and last blocks within the Jacobian that contain data in column j
    Jac.getBlocksInColumn(j, ca, cb);

    // Multiply blocks
    for (uint32_t i = ca; i <= cb; ++i)
    {
      // Get row range to multiply over
      ra = blockInfo[i].m_jRow;
      if (ra >= numResiduals) break;
      rb = ra + blockInfo[i].m_jSize;
      if (rb > numResiduals) rb = numResiduals; // Clamp to residual range
      nr = rb - ra; // number of elements to multiply

      c = j - blockInfo[i].m_jColumn; // local column index in block
      A.getColumnVector(c, ra, nr, u); // Column sub-vector from J
      v = &r[ra]; // Column sub-vector from r

      for (uint32_t k = 0; k < nr; ++k) result[j] += u[k] * v[k];
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void NonLinearOptimiserBandDiag<ScalarType>::computeJb(
  uint32_t                             numResiduals,
  const NMP::JacobianBase<ScalarType>* J,
  const NMP::Vector<ScalarType>&       b,
  NMP::Vector<ScalarType>&             result)
{
  NMP_ASSERT(J);
  const NMP::JacobianBlockDiag<ScalarType>& Jac = static_cast< const NMP::JacobianBlockDiag<ScalarType>& >(*J);
  const NMP::Matrix<ScalarType>& A = Jac.getMatrix();
  uint32_t numBlocks = Jac.getNumBlocks();
  NMP_ASSERT(numBlocks > 0);
  const JacobianBlockDiagInfo* blockInfo = Jac.getBlockInfo();
  NMP_ASSERT(blockInfo);

  uint32_t width = A.numColumns();
  uint32_t c, ra, rb, nr;
  ScalarType* u = m_temp.data();
  ScalarType* p;
  const ScalarType* v;

  result.set(static_cast<ScalarType>(0));

  for (uint32_t j = 0; j < numBlocks; ++j)
  {
    ra = blockInfo[j].m_jRow;
    if (ra >= numResiduals) break;
    rb = ra + blockInfo[j].m_jSize;
    if (rb > numResiduals) rb = numResiduals; // Clamp to residual range
    nr = rb - ra; // number of elements to multiply

    c = blockInfo[j].m_jColumn;
    v = &b[c]; // Column sub-vector from b
    p = &result[ra]; // Subsection within the output vector

    for (uint32_t i = 0; i < nr; ++i)
    {
      A.getRowVector(ra + i, 0, width, u); // Row vector from data block
      for (uint32_t k = 0; k < width; ++k) p[i] += u[k] * v[k];
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void NonLinearOptimiserBandDiag<ScalarType>::computeJtJ(
  uint32_t                             numResiduals,
  const NMP::JacobianBase<ScalarType>* J,
  NMP::HessianBase<ScalarType>*        H)
{
  NMP_ASSERT(J);
  NMP_ASSERT(H);
  const NMP::JacobianBlockDiag<ScalarType>& Jac = static_cast< const NMP::JacobianBlockDiag<ScalarType>& >(*J);
  const NMP::Matrix<ScalarType>& A = Jac.getMatrix();
  NMP_ASSERT(Jac.getNumBlocks() > 0);
  const JacobianBlockDiagInfo* blockInfo = Jac.getBlockInfo();
  NMP_ASSERT(blockInfo);
  NMP::HessianBandDiag<ScalarType>& Hess = static_cast< NMP::HessianBandDiag<ScalarType>& >(*H);
  NMP::BandDiagMatrix<ScalarType>& AtA = Hess.getMatrix();

  uint32_t ia, ib, ia_, ib_, ca, cb;
  uint32_t ra, rb, c1, c2, nr, ext;
  uint32_t numColumns = Jac.getNumColumns();
  uint32_t blockWidth = Jac.getBlockWidth();
  ScalarType* u = m_temp.data();
  ScalarType* v = m_temp2.data();
  ScalarType val;

  AtA.set(static_cast<ScalarType>(0));

  for (uint32_t r = 0; r < numColumns; ++r)
  {
    // Find the first and last blocks within the Jacobian that contain data in column r
    Jac.getBlocksInColumn(r, ia, ib);

    ext = NMP::minimum(r + blockWidth, numColumns);
    for (uint32_t c = r; c < ext; ++c)
    {
      // Find the first and last blocks within the Jacobian that contain data in column c
      Jac.getBlocksInColumn(c, ia_, ib_);
      if (ia_ > ib) break;

      // Find the common set of blocks
      ca = NMP::maximum(ia, ia_);
      cb = NMP::minimum(ib, ib_);

      // Multiply over the set of blocks
      val = 0;
      for (uint32_t i = ca; i <= cb; ++i)
      {
        ra = blockInfo[i].m_jRow; // Start row within the current common block
        if (ra >= numResiduals) break;
        rb = ra + blockInfo[i].m_jSize;
        if (rb > numResiduals) rb = numResiduals; // Clamp to residual range
        nr = rb - ra; // number of elements to multiply
        c1 = r - blockInfo[i].m_jColumn; // local column index in block (row loop)
        c2 = c - blockInfo[i].m_jColumn; // local column index in block (column loop)

        // Get the column vector data within the common block
        A.getColumnVector(c1, ra, nr, u);
        A.getColumnVector(c2, ra, nr, v);
        for (uint32_t k = 0; k < nr; ++k) val += u[k] * v[k]; // Dot product of the vectors
      }
      AtA.element(r, c, val); // Set the matrix value within the Hessian
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void NonLinearOptimiserBandDiag<ScalarType>::augmentJtJ(NMP::HessianBase<ScalarType>* H, ScalarType lambda)
{
  NMP_ASSERT(H);
  NMP::HessianBandDiag<ScalarType>& Hess = static_cast< NMP::HessianBandDiag<ScalarType>& >(*H);
  NMP::BandDiagMatrix<ScalarType>& AtA = Hess.getMatrix();
  AtA.addDiagVector(0, lambda);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void NonLinearOptimiserBandDiag<ScalarType>::solve(
  const NMP::HessianBase<ScalarType>* H,
  const NMP::Vector<ScalarType>&      b,
  NMP::Vector<ScalarType>&            searchDir)
{
  NMP_ASSERT(H);
  const NMP::HessianBandDiag<ScalarType>& Hess = static_cast< const NMP::HessianBandDiag<ScalarType>& >(*H);
  const NMP::BandDiagMatrix<ScalarType>& AtA = Hess.getMatrix();

  m_solver.solve(AtA, b); // A x = b
  NMP_ASSERT(!m_solver.fail());
  m_solver.getX(searchDir);
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
