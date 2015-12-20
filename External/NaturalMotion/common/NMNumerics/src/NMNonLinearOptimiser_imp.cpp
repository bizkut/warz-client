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
// Jacobian
//----------------------------------------------------------------------------------------------------------------------
template <>
Jacobian<ScalarType>::Jacobian(uint32_t numRows, uint32_t numColumns) :
  JacobianBase(numRows, numColumns),
  m_matrix(numRows, numColumns)
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Jacobian<ScalarType>::~Jacobian()
{
}

//----------------------------------------------------------------------------------------------------------------------
// Hessian
//----------------------------------------------------------------------------------------------------------------------
template <>
Hessian<ScalarType>::Hessian(uint32_t size) :
  HessianBase(size),
  m_matrix(size, size)
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Hessian<ScalarType>::~Hessian()
{
}

//----------------------------------------------------------------------------------------------------------------------
// NonLinearOptimiser
//----------------------------------------------------------------------------------------------------------------------
template <>
NonLinearOptimiser<ScalarType>::NonLinearOptimiser(NonLinearLeastSquaresFunc<ScalarType>& func)
  :
  NonLinearOptimiserBase<ScalarType>(func),
  m_solver(func.getNumUnknowns()),
  m_Pfd(func.getNumUnknowns()),
  m_fRes_(func.getMaxNumResiduals())
{
  uint32_t numRows = func.getMaxNumResiduals();
  uint32_t numColumns = func.getNumUnknowns();
  m_J = new Jacobian<ScalarType>(numRows, numColumns);
  m_H = new Hessian<ScalarType>(numColumns);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
NonLinearOptimiser<ScalarType>::NonLinearOptimiser(NonLinearLeastSquaresFuncJac<ScalarType>& funcJac)
  :
  NonLinearOptimiserBase<ScalarType>(funcJac),
  m_solver(funcJac.getNumUnknowns()),
  m_Pfd(funcJac.getNumUnknowns()),
  m_fRes_(funcJac.getMaxNumResiduals())
{
  uint32_t numRows = funcJac.getMaxNumResiduals();
  uint32_t numColumns = funcJac.getNumUnknowns();
  m_J = new Jacobian<ScalarType>(numRows, numColumns);
  m_H = new Hessian<ScalarType>(numColumns);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
NonLinearOptimiser<ScalarType>::~NonLinearOptimiser()
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool NonLinearOptimiser<ScalarType>::computeJByFiniteDifference(
  const NMP::Vector<ScalarType>&         P,
  const NMP::Vector<ScalarType>&         fdDeltas,
  const NMP::Vector<ScalarType>&         fRes,
  NonLinearLeastSquaresFunc<ScalarType>& lsqFn,
  NMP::JacobianBase<ScalarType>*         J)
{
  NMP_ASSERT(J);
  NMP::Jacobian<ScalarType>& Jac = static_cast< NMP::Jacobian<ScalarType>& >(*J);
  NMP::Matrix<ScalarType>& A = Jac.getMatrix();
  uint32_t numUnknowns = P.numElements();
  uint32_t numResiduals = lsqFn.getNumResiduals();
  bool fnStatus;

  m_Pfd = P;
  for (uint32_t i = 0; i < numUnknowns; ++i)
  {
    // Residual function error from finite difference of parameter vector P
    m_Pfd[i] = P[i] + fdDeltas[i];
    fnStatus = lsqFn.func(*this, m_Pfd, m_fRes_);
    m_numFuncEvals++;
    m_Pfd[i] = P[i];

    // Check for user termination
    if (!fnStatus) return false;

    // Set the column of the Jacobian. Note that numResiduals can be different
    // to the preallocated size of the residuals vector maxNumResiduals
    for (uint32_t k = 0; k < numResiduals; ++k)
    {
      m_fRes_[k] = (m_fRes_[k] - fRes[k]) / fdDeltas[i];
    }

    A.setColumnVector(i, 0, numResiduals, m_fRes_.data());
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void NonLinearOptimiser<ScalarType>::getJabsRowSums(
  uint32_t                             numResiduals,
  const NMP::JacobianBase<ScalarType>* J,
  NMP::Vector<ScalarType>&             result)
{
  NMP_ASSERT(J);
  const NMP::Jacobian<ScalarType>& Jac = static_cast< const NMP::Jacobian<ScalarType>& >(*J);
  const NMP::Matrix<ScalarType>& A = Jac.getMatrix();

  for (uint32_t i = 0; i < numResiduals; ++i)
  {
    // Get current row of Jacobian
    A.getRowVector(i, m_Pfd);

    // Compute the sum of absolute values
    m_Pfd.absolute();
    result[i] = m_Pfd.sum();
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void NonLinearOptimiser<ScalarType>::computeJtr(
  uint32_t                             numResiduals,
  const NMP::JacobianBase<ScalarType>* J,
  const NMP::Vector<ScalarType>&       r,
  NMP::Vector<ScalarType>&             result)
{
  NMP_ASSERT(J);
  const NMP::Jacobian<ScalarType>& Jac = static_cast< const NMP::Jacobian<ScalarType>& >(*J);
  const NMP::Matrix<ScalarType>& A = Jac.getMatrix();

  A.preMultiplyTranspose(numResiduals, result.data(), r.data());
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void NonLinearOptimiser<ScalarType>::computeJb(
  uint32_t                             numResiduals,
  const NMP::JacobianBase<ScalarType>* J,
  const NMP::Vector<ScalarType>&       b,
  NMP::Vector<ScalarType>&             result)
{
  NMP_ASSERT(J);
  const NMP::Jacobian<ScalarType>& Jac = static_cast< const NMP::Jacobian<ScalarType>& >(*J);
  const NMP::Matrix<ScalarType>& A = Jac.getMatrix();

  A.preMultiply(numResiduals, result.data(), b.data());
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void NonLinearOptimiser<ScalarType>::computeJtJ(
  uint32_t                             numResiduals,
  const NMP::JacobianBase<ScalarType>* J,
  NMP::HessianBase<ScalarType>*        H)
{
  NMP_ASSERT(J);
  NMP_ASSERT(H);
  const NMP::Jacobian<ScalarType>& Jac = static_cast< const NMP::Jacobian<ScalarType>& >(*J);
  const NMP::Matrix<ScalarType>& A = Jac.getMatrix();
  NMP::Hessian<ScalarType>& Hess = static_cast< NMP::Hessian<ScalarType>& >(*H);
  NMP::Matrix<ScalarType>& AtA = Hess.getMatrix();

  AtA.setFromMultiplicationAtA(numResiduals, A);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void NonLinearOptimiser<ScalarType>::augmentJtJ(NMP::HessianBase<ScalarType>* H, ScalarType lambda)
{
  NMP_ASSERT(H);
  NMP::Hessian<ScalarType>& Hess = static_cast< NMP::Hessian<ScalarType>& >(*H);
  NMP::Matrix<ScalarType>& AtA = Hess.getMatrix();
  AtA.addDiagVector(0, lambda);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void NonLinearOptimiser<ScalarType>::solve(
  const NMP::HessianBase<ScalarType>* H,
  const NMP::Vector<ScalarType>&      b,
  NMP::Vector<ScalarType>&            searchDir)
{
  NMP_ASSERT(H);
  const NMP::Hessian<ScalarType>& Hess = static_cast< const NMP::Hessian<ScalarType>& >(*H);
  const NMP::Matrix<ScalarType>& AtA = Hess.getMatrix();

  m_solver.compute(AtA, true); // A = V * D * V^T
  NMP_ASSERT(!m_solver.fail());
  m_solver.solve(searchDir, b);
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
