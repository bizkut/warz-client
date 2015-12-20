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
#include "NMNumerics/NMQuatSplineFitterSmoothedC1.h"
#include "NMNumerics/NMSimpleSplineFittingUtils.h"
#include "NMNumerics/NMQuatUtils.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// SpanInfoSmoothed
//----------------------------------------------------------------------------------------------------------------------
SpanInfoSmoothed::SpanInfoSmoothed()
{
}

//----------------------------------------------------------------------------------------------------------------------
SpanInfoSmoothed::~SpanInfoSmoothed()
{
}

//----------------------------------------------------------------------------------------------------------------------
void SpanInfoSmoothed::clear()
{
  m_numSpanSamples = 0;
  m_spanTs = 0;
  m_spanQuats = 0;
}

//----------------------------------------------------------------------------------------------------------------------
// QuatSplineFitterSmoothedC0FuncJac
//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterSmoothedC0FuncJac::
QuatSplineFitterSmoothedC0FuncJac(uint32_t maxNumSamples) :
  NonLinearLeastSquaresFuncJac<double>(12, 4 * maxNumSamples + 6),
  m_spanInfo1(0),
  m_spanInfo2(0),
  m_dqb_by_drb(4, 3),
  m_dw2_by_drb(3, 3),
  m_mat3_3(3, 3),
  m_matA4_3(4, 3),
  m_matB4_3(4, 3)
{
  NMP_ASSERT(maxNumSamples > 0);
}

//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterSmoothedC0FuncJac::~QuatSplineFitterSmoothedC0FuncJac()
{
}

//----------------------------------------------------------------------------------------------------------------------
bool QuatSplineFitterSmoothedC0FuncJac::func(
  const NMP::NonLinearOptimiserBase<double>& NMP_UNUSED(optimiser),
  const NMP::Vector<double>& P,
  NMP::Vector<double>&       fRes,
  NMP::JacobianBase<double>* J)
{
  NMP_ASSERT(m_spanInfo1);
  NMP_ASSERT(m_spanInfo2);
  NMP_ASSERT(m_spanInfo1->m_numSpanSamples + m_spanInfo2->m_numSpanSamples > 0);
  NMP_ASSERT(m_weight_wa >= 0);
  NMP_ASSERT(m_weight_wc >= 0);

  Quat qb, qt, q;
  Vector3 wa, wb, wc, v, rb;
  uint32_t indx;

  // Jacobian blocks are stored as a (numRows by blockWidth) matrix
  NMP::Jacobian<double>& Jac = static_cast< NMP::Jacobian<double>& >(*J);
  NMP::Matrix<double>& A = Jac.getMatrix();

  uint32_t numSamples1 = m_spanInfo1->m_numSpanSamples;
  uint32_t numSamples2 = m_spanInfo2->m_numSpanSamples;
  const float* ts1 = m_spanInfo1->m_spanTs;
  const Quat* quats1 = m_spanInfo1->m_spanQuats;
  const float* ts2 = m_spanInfo2->m_spanTs;
  const Quat* quats2 = m_spanInfo2->m_spanQuats;

  // Unpack the parameter vector: P = [wa; rb; v; wc]
  wa.x = (float)P[0];
  wa.y = (float)P[1];
  wa.z = (float)P[2];
  rb.x = (float)P[3];
  rb.y = (float)P[4];
  rb.z = (float)P[5];
  v.x = (float)P[6];
  v.y = (float)P[7];
  v.z = (float)P[8];
  wc.x = (float)P[9];
  wc.y = (float)P[10];
  wc.z = (float)P[11];

  // Convert the quat log vector rb back to a quaternion qb
  quatExpJac(rb, m_dqb_by_drb, qb);

  // Compute the Bezier control points for the first span
  wb = v * m_alpha_b;
  m_JacBuilder.w2Jac(m_qa, wa, wb, qb);

  // Jacobian Dw2 / Drb
  m_dw2_by_drb.setFromMultiplication(m_JacBuilder.getDw2ByDqb(), m_dqb_by_drb);

  // Compute the Jacobian and residuals for the first span
  indx = 0;
  for (uint32_t i = 0; i < numSamples1; ++i, indx += 4)
  {
    // Compute the Bezier quaternion spline q(t) and its Jacobian Dq(t) / D[q0, w1, w2, w3]
    m_JacBuilder.valJac(m_qa, wa, m_JacBuilder.getW2(), wb, ts1[i], m_ta, m_tb);

    // Jacobian Dq(t) / Dwa
    m_matA4_3.setFromMultiplication(m_JacBuilder.getDqtByDw2(), m_JacBuilder.getDw2ByDwa());
    m_matA4_3 += m_JacBuilder.getDqtByDw1();
    A.setSubMatrix(indx, 0, m_matA4_3);

    // Jacobian Dq(t) / Drb
    m_matA4_3.setFromMultiplication(m_JacBuilder.getDqtByDw2(), m_dw2_by_drb);
    A.setSubMatrix(indx, 3, m_matA4_3);

    // Jacobian Dq(t) / Dv
    m_matA4_3.setFromMultiplication(m_JacBuilder.getDqtByDw2(), m_JacBuilder.getDw2ByDwb());
    m_matA4_3 += m_JacBuilder.getDqtByDw3();
    m_matA4_3 *= m_alpha_b;
    A.setSubMatrix(indx, 6, m_matA4_3);

    // Jacobian Dq(t) / Dwc
    A.setSubMatrix(indx, 9, 4, 3, 0.0);

    // Compute the residuals
    qt = m_JacBuilder.getQt();
    q = quats1[i];
    fRes[indx] = (double)(qt.x - q.x);
    fRes[indx+1] = (double)(qt.y - q.y);
    fRes[indx+2] = (double)(qt.z - q.z);
    fRes[indx+3] = (double)(qt.w - q.w);
  }

  // Compute the Bezier control points for the second span
  wb = v * m_alpha_a;
  m_JacBuilder.w2Jac(qb, wb, wc, m_qc);

  // Jacobian Dw2 / Drb
  m_dw2_by_drb.setFromMultiplication(m_JacBuilder.getDw2ByDqa(), m_dqb_by_drb);

  // Compute the Jacobian and residuals for the second span
  for (uint32_t i = 0; i < numSamples2; ++i, indx += 4)
  {
    // Compute the Bezier quaternion spline q(t) and its Jacobian Dq(t) / D[q0, w1, w2, w3]
    m_JacBuilder.valJac(qb, wb, m_JacBuilder.getW2(), wc, ts2[i], m_tb, m_tc);

    // Jacobian Dq(t) / Dwa
    A.setSubMatrix(indx, 0, 4, 3, 0.0);

    // Jacobian Dq(t) / Drb
    m_matA4_3.setFromMultiplication(m_JacBuilder.getDqtByDq0(), m_dqb_by_drb);
    m_matB4_3.setFromMultiplication(m_JacBuilder.getDqtByDw2(), m_dw2_by_drb);
    m_matA4_3 += m_matB4_3;
    A.setSubMatrix(indx, 3, m_matA4_3);

    // Jacobian Dq(t) / Dv
    m_matA4_3.setFromMultiplication(m_JacBuilder.getDqtByDw2(), m_JacBuilder.getDw2ByDwa());
    m_matA4_3 += m_JacBuilder.getDqtByDw1();
    m_matA4_3 *= m_alpha_a;
    A.setSubMatrix(indx, 6, m_matA4_3);

    // Jacobian Dq(t) / Dwc
    m_matA4_3.setFromMultiplication(m_JacBuilder.getDqtByDw2(), m_JacBuilder.getDw2ByDwb());
    m_matA4_3 += m_JacBuilder.getDqtByDw3();
    A.setSubMatrix(indx, 9, m_matA4_3);

    // Compute the residuals
    qt = m_JacBuilder.getQt();
    q = quats2[i];
    fRes[indx] = (double)(qt.x - q.x);
    fRes[indx+1] = (double)(qt.y - q.y);
    fRes[indx+2] = (double)(qt.z - q.z);
    fRes[indx+3] = (double)(qt.w - q.w);
  }

  // Compute the Jacobian and residuals for the smoothness constraints wa, wc
  m_mat3_3.set(0.0);
  m_mat3_3.setDiagVector(0, m_weight_wa);
  A.setSubMatrix(indx, 0, m_mat3_3);
  A.setSubMatrix(indx, 3, 3, 9, 0.0);
  fRes[indx] = m_weight_wa * (wa.x - m_wa.x);
  fRes[indx+1] = m_weight_wa * (wa.y - m_wa.y);
  fRes[indx+2] = m_weight_wa * (wa.z - m_wa.z);

  indx += 3;
  m_mat3_3.setDiagVector(0, m_weight_wc);
  A.setSubMatrix(indx, 0, 3, 9, 0.0);
  A.setSubMatrix(indx, 9, m_mat3_3);
  fRes[indx] = m_weight_wc * (wc.x - m_wc.x);
  fRes[indx+1] = m_weight_wc * (wc.y - m_wc.y);
  fRes[indx+2] = m_weight_wc * (wc.z - m_wc.z);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void QuatSplineFitterSmoothedC0FuncJac::
setSpanInfo(
  const SpanInfoSmoothed* spanInfo1,
  const SpanInfoSmoothed* spanInfo2,
  float                   ta,
  float                   tb,
  float                   tc,
  const Quat&             qa,
  const Quat&             qc,
  float                   alpha_a,
  float                   alpha_b,
  const Vector3&          wa,
  const Vector3&          wc,
  float                   weight_wa,
  float                   weight_wc)
{
  NMP_ASSERT(spanInfo1);
  NMP_ASSERT(spanInfo2);
  NMP_ASSERT(spanInfo1->m_numSpanSamples + spanInfo2->m_numSpanSamples > 0);
  NMP_ASSERT(weight_wa >= 0);
  NMP_ASSERT(weight_wc >= 0);

  // Set the span information data
  m_spanInfo1 = spanInfo1;
  m_spanInfo2 = spanInfo2;
  m_ta = ta;
  m_tb = tb;
  m_tc = tc;
  m_qa = qa;
  m_qc = qc;
  m_alpha_a = alpha_a;
  m_alpha_b = alpha_b;
  m_wa = wa;
  m_wc = wc;
  m_weight_wa = weight_wa;
  m_weight_wc = weight_wc;

  // Set the number of residuals
  uint32_t numSamples = spanInfo1->m_numSpanSamples + spanInfo2->m_numSpanSamples;
  setNumResiduals(4 * numSamples + 6);
}

//----------------------------------------------------------------------------------------------------------------------
// QuatSplineFitterSmoothedC1FuncJac
//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterSmoothedC1FuncJac::
QuatSplineFitterSmoothedC1FuncJac(uint32_t maxNumSamples) :
  NonLinearLeastSquaresFuncJac<double>(6, 4 * maxNumSamples),
  m_spanInfo1(0),
  m_spanInfo2(0),
  m_dqb_by_drb(4, 3),
  m_dw2_by_drb(3, 3),
  m_mat3_3(3, 3),
  m_matA4_3(4, 3),
  m_matB4_3(4, 3)
{
  NMP_ASSERT(maxNumSamples > 0);
}

//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterSmoothedC1FuncJac::~QuatSplineFitterSmoothedC1FuncJac()
{
}

//----------------------------------------------------------------------------------------------------------------------
bool QuatSplineFitterSmoothedC1FuncJac::func(
  const NMP::NonLinearOptimiserBase<double>& NMP_UNUSED(optimiser),
  const NMP::Vector<double>& P,
  NMP::Vector<double>&       fRes,
  NMP::JacobianBase<double>* J)
{
  NMP_ASSERT(m_spanInfo1);
  NMP_ASSERT(m_spanInfo2);
  NMP_ASSERT(m_spanInfo1->m_numSpanSamples + m_spanInfo2->m_numSpanSamples > 0);

  Quat qb, qt, q;
  Vector3 wb, v, rb;
  uint32_t indx;

  // Jacobian blocks are stored as a (numRows by blockWidth) matrix
  NMP::Jacobian<double>& Jac = static_cast< NMP::Jacobian<double>& >(*J);
  NMP::Matrix<double>& A = Jac.getMatrix();

  uint32_t numSamples1 = m_spanInfo1->m_numSpanSamples;
  uint32_t numSamples2 = m_spanInfo2->m_numSpanSamples;
  const float* ts1 = m_spanInfo1->m_spanTs;
  const Quat* quats1 = m_spanInfo1->m_spanQuats;
  const float* ts2 = m_spanInfo2->m_spanTs;
  const Quat* quats2 = m_spanInfo2->m_spanQuats;

  // Unpack the parameter vector: P = [rb; v]
  rb.x = (float)P[0];
  rb.y = (float)P[1];
  rb.z = (float)P[2];
  v.x = (float)P[3];
  v.y = (float)P[4];
  v.z = (float)P[5];

  // Convert the quat log vector rb back to a quaternion qb
  quatExpJac(rb, m_dqb_by_drb, qb);

  // Compute the Bezier control points for the first span
  wb = v * m_alpha_b;
  m_JacBuilder.w2Jac(m_qa, m_wa, wb, qb);

  // Jacobian Dw2 / Drb
  m_dw2_by_drb.setFromMultiplication(m_JacBuilder.getDw2ByDqb(), m_dqb_by_drb);

  // Compute the Jacobian and residuals for the first span
  indx = 0;
  for (uint32_t i = 0; i < numSamples1; ++i, indx += 4)
  {
    // Compute the Bezier quaternion spline q(t) and its Jacobian Dq(t) / D[q0, w1, w2, w3]
    m_JacBuilder.valJac(m_qa, m_wa, m_JacBuilder.getW2(), wb, ts1[i], m_ta, m_tb);

    // Jacobian Dq(t) / Drb
    m_matA4_3.setFromMultiplication(m_JacBuilder.getDqtByDw2(), m_dw2_by_drb);
    A.setSubMatrix(indx, 0, m_matA4_3);

    // Jacobian Dq(t) / Dv
    m_matA4_3.setFromMultiplication(m_JacBuilder.getDqtByDw2(), m_JacBuilder.getDw2ByDwb());
    m_matA4_3 += m_JacBuilder.getDqtByDw3();
    m_matA4_3 *= m_alpha_b;
    A.setSubMatrix(indx, 3, m_matA4_3);

    // Compute the residuals
    qt = m_JacBuilder.getQt();
    q = quats1[i];
    fRes[indx] = (double)(qt.x - q.x);
    fRes[indx+1] = (double)(qt.y - q.y);
    fRes[indx+2] = (double)(qt.z - q.z);
    fRes[indx+3] = (double)(qt.w - q.w);
  }

  // Compute the Bezier control points for the second span
  wb = v * m_alpha_a;
  m_JacBuilder.w2Jac(qb, wb, m_wc, m_qc);

  // Jacobian Dw2 / Drb
  m_dw2_by_drb.setFromMultiplication(m_JacBuilder.getDw2ByDqa(), m_dqb_by_drb);

  // Compute the Jacobian and residuals for the second span
  for (uint32_t i = 0; i < numSamples2; ++i, indx += 4)
  {
    // Compute the Bezier quaternion spline q(t) and its Jacobian Dq(t) / D[q0, w1, w2, w3]
    m_JacBuilder.valJac(qb, wb, m_JacBuilder.getW2(), m_wc, ts2[i], m_tb, m_tc);

    // Jacobian Dq(t) / Drb
    m_matA4_3.setFromMultiplication(m_JacBuilder.getDqtByDq0(), m_dqb_by_drb);
    m_matB4_3.setFromMultiplication(m_JacBuilder.getDqtByDw2(), m_dw2_by_drb);
    m_matA4_3 += m_matB4_3;
    A.setSubMatrix(indx, 0, m_matA4_3);

    // Jacobian Dq(t) / Dv
    m_matA4_3.setFromMultiplication(m_JacBuilder.getDqtByDw2(), m_JacBuilder.getDw2ByDwa());
    m_matA4_3 += m_JacBuilder.getDqtByDw1();
    m_matA4_3 *= m_alpha_a;
    A.setSubMatrix(indx, 3, m_matA4_3);

    // Compute the residuals
    qt = m_JacBuilder.getQt();
    q = quats2[i];
    fRes[indx] = (double)(qt.x - q.x);
    fRes[indx+1] = (double)(qt.y - q.y);
    fRes[indx+2] = (double)(qt.z - q.z);
    fRes[indx+3] = (double)(qt.w - q.w);
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void QuatSplineFitterSmoothedC1FuncJac::
setSpanInfo(
  const SpanInfoSmoothed* spanInfo1,
  const SpanInfoSmoothed* spanInfo2,
  float                   ta,
  float                   tb,
  float                   tc,
  const Quat&             qa,
  const Quat&             qc,
  float                   alpha_a,
  float                   alpha_b,
  const Vector3&          wa,
  const Vector3&          wc)
{
  NMP_ASSERT(spanInfo1);
  NMP_ASSERT(spanInfo2);
  NMP_ASSERT(spanInfo1->m_numSpanSamples + spanInfo2->m_numSpanSamples > 0);

  // Set the span information data
  m_spanInfo1 = spanInfo1;
  m_spanInfo2 = spanInfo2;
  m_ta = ta;
  m_tb = tb;
  m_tc = tc;
  m_qa = qa;
  m_qc = qc;
  m_alpha_a = alpha_a;
  m_alpha_b = alpha_b;
  m_wa = wa;
  m_wc = wc;

  // Set the number of residuals
  uint32_t numSamples = spanInfo1->m_numSpanSamples + spanInfo2->m_numSpanSamples;
  setNumResiduals(4 * numSamples);
}

//----------------------------------------------------------------------------------------------------------------------
// QuatSplineFitterSmoothedC1
//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterSmoothedC1::
QuatSplineFitterSmoothedC1(uint32_t numSamples, const float* ts, const Quat* quats, uint32_t numKnots, const float* knots) :
  QuatSplineFitterBase(numSamples),
  m_P0(12),
  m_P1(6),
  m_spanInfoInc(0),
  m_spanInfoExc(0),
  m_tangentFitter(numSamples)
{
  NMP_ASSERT(m_maxNumSamples >= 2);

  // Allocate a new non-linear fitting function
  m_lsqFuncC0 = new QuatSplineFitterSmoothedC0FuncJac(m_maxNumSamples);

  // Allocate a new optimiser
  m_optimiserC0 = new NonLinearOptimiser<double>(*m_lsqFuncC0);
  m_optimiserC0->setMaxIterations(10);
  m_optimiserC0->setMaxFuncEvals(10);
  m_optimiserC0->setTolP(1e-6);
  m_optimiserC0->setTolFunc(1e-6);

  // Allocate a new non-linear fitting function
  m_lsqFuncC1 = new QuatSplineFitterSmoothedC1FuncJac(m_maxNumSamples);

  // Allocate a new optimiser
  m_optimiserC1 = new NonLinearOptimiser<double>(*m_lsqFuncC1);
  m_optimiserC1->setMaxIterations(5);
  m_optimiserC1->setMaxFuncEvals(5);
  m_optimiserC1->setTolP(1e-6);
  m_optimiserC1->setTolFunc(1e-6);

  // Set the knot vector
  setKnots(numKnots, knots);

  // Allocate the memory for the span information
  uint32_t numSpans = numKnots - 1;
  m_spanInfoInc = new SpanInfoSmoothed[numSpans];
  m_spanInfoExc = new SpanInfoSmoothed[numSpans];

  // Fit a quaternion spline to the data samples
  fit(numSamples, ts, quats);
}

//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterSmoothedC1::
QuatSplineFitterSmoothedC1(uint32_t numSamples, const Quat* quats, uint32_t numKnots, const float* knots) :
  QuatSplineFitterBase(numSamples),
  m_P0(12),
  m_P1(6),
  m_spanInfoInc(0),
  m_spanInfoExc(0),
  m_tangentFitter(numSamples)
{
  NMP_ASSERT(m_maxNumSamples >= 2);

  // Allocate a new non-linear fitting function
  m_lsqFuncC0 = new QuatSplineFitterSmoothedC0FuncJac(m_maxNumSamples);

  // Allocate a new optimiser
  m_optimiserC0 = new NonLinearOptimiser<double>(*m_lsqFuncC0);
  m_optimiserC0->setMaxIterations(10);
  m_optimiserC0->setMaxFuncEvals(10);
  m_optimiserC0->setTolP(1e-6);
  m_optimiserC0->setTolFunc(1e-6);

  // Allocate a new non-linear fitting function
  m_lsqFuncC1 = new QuatSplineFitterSmoothedC1FuncJac(m_maxNumSamples);

  // Allocate a new optimiser
  m_optimiserC1 = new NonLinearOptimiser<double>(*m_lsqFuncC1);
  m_optimiserC1->setMaxIterations(5);
  m_optimiserC1->setMaxFuncEvals(5);
  m_optimiserC1->setTolP(1e-6);
  m_optimiserC1->setTolFunc(1e-6);

  // Set the knot vector
  setKnots(numKnots, knots);

  // Allocate the memory for the span information
  uint32_t numSpans = numKnots - 1;
  m_spanInfoInc = new SpanInfoSmoothed[numSpans];
  m_spanInfoExc = new SpanInfoSmoothed[numSpans];

  // Fit a quaternion spline to the data samples
  fitRegular(numSamples, quats);
}

//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterSmoothedC1::QuatSplineFitterSmoothedC1(uint32_t maxNumSamples, uint32_t numKnots) :
  QuatSplineFitterBase(maxNumSamples),
  m_P0(12),
  m_P1(6),
  m_spanInfoInc(0),
  m_spanInfoExc(0),
  m_tangentFitter(maxNumSamples)
{
  NMP_ASSERT(m_maxNumSamples >= 2);
  NMP_ASSERT(numKnots >= 2);

  // Allocate a new non-linear fitting function
  m_lsqFuncC0 = new QuatSplineFitterSmoothedC0FuncJac(m_maxNumSamples);

  // Allocate a new optimiser
  m_optimiserC0 = new NonLinearOptimiser<double>(*m_lsqFuncC0);
  m_optimiserC0->setMaxIterations(10);
  m_optimiserC0->setMaxFuncEvals(10);
  m_optimiserC0->setTolP(1e-6);
  m_optimiserC0->setTolFunc(1e-6);

  // Allocate a new non-linear fitting function
  m_lsqFuncC1 = new QuatSplineFitterSmoothedC1FuncJac(m_maxNumSamples);

  // Allocate a new optimiser
  m_optimiserC1 = new NonLinearOptimiser<double>(*m_lsqFuncC1);
  m_optimiserC1->setMaxIterations(5);
  m_optimiserC1->setMaxFuncEvals(5);
  m_optimiserC1->setTolP(1e-6);
  m_optimiserC1->setTolFunc(1e-6);

  // Allocate the memory for the span information
  uint32_t numSpans = numKnots - 1;
  m_spanInfoInc = new SpanInfoSmoothed[numSpans];
  m_spanInfoExc = new SpanInfoSmoothed[numSpans];
}

//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterSmoothedC1::~QuatSplineFitterSmoothedC1()
{
  delete m_lsqFuncC0;
  delete m_optimiserC0;
  delete m_lsqFuncC1;
  delete m_optimiserC1;
  delete[] m_spanInfoInc;
  delete[] m_spanInfoExc;
}

//----------------------------------------------------------------------------------------------------------------------
bool QuatSplineFitterSmoothedC1::fit(uint32_t numSamples, const float* ts, const Quat* quats)
{
  NMP_ASSERT(ts);
  NMP_ASSERT(quats);
  NMP_ASSERT(numSamples >= 2);

  uint32_t numSpans;
  uint32_t ia, ib;
  bool status;

  // Get the knot vector
  uint32_t numKnots = m_qsp.getNumKnots();
  const float* knots = m_qsp.getKnots();
  NMP_ASSERT(SimpleSplineFittingUtils::isKnotVectorValid(numKnots, knots));

  // Compute an initial estimate of the quaternion spline
  m_tangentFitter.setKnots(numKnots, knots);
  m_tangentFitter.fit(numSamples, ts, quats);

  // Set the time samples
  setTs(numSamples, ts);

  // Set the conditioned quaternion samples
  setQuatsRaw(numSamples, m_tangentFitter.getQuats());

  // Set the output quaternion spline
  m_qsp = m_tangentFitter.getQuatSpline();

  // Check for a single span quaternion curve
  numSpans = numKnots - 1;
  if (numSpans == 1) return true;

  //------------------------------
  // Span information
  //
  for (uint32_t i = 0; i < numSpans; ++i)
  {
    // Exclude sample at tb to avoid computing the Jacobian twice
    SpanInfoSmoothed& infoInc = m_spanInfoInc[i];
    status = SimpleSplineFittingUtils::findSamplesInclusiveTa(
               m_qsp.getKnot(i),
               m_qsp.getKnot(i + 1),
               ia,
               ib,
               m_numSamples,
               m_ts);
    if (status)
    {
      infoInc.m_numSpanSamples = 1 + ib - ia;
      infoInc.m_spanTs = &m_ts[ia];
      infoInc.m_spanQuats = &m_quats[ia];
    }
    else
    {
      infoInc.clear();
    }

    // Exclude samples at ta and tb to avoid computing the Jacobian twice
    SpanInfoSmoothed& infoExc = m_spanInfoExc[i];
    status = SimpleSplineFittingUtils::findSamplesExclusive(
               m_qsp.getKnot(i),
               m_qsp.getKnot(i + 1),
               ia,
               ib,
               m_numSamples,
               m_ts);
    if (status)
    {
      infoExc.m_numSpanSamples = 1 + ib - ia;
      infoExc.m_spanTs = &m_ts[ia];
      infoExc.m_spanQuats = &m_quats[ia];
    }
    else
    {
      infoExc.clear();
    }
  }

  //------------------------------
  // Make a forward pass over the knot spans allowing full optimisation of keys
  // and tangent vectors in order to find an optimal solution for the keys with
  // some degree of smoothness
  smoothC0(1, 1.0, 0.0); // Weight wa in order to bias it not to move
  for (uint32_t i = 2; i < numSpans - 1; ++i)
  {
    smoothC0(i, 0.0, 0.0); // No weights on wa, wb to encourage an optimal solution
  }
  smoothC0(numSpans - 1, 0.0, 1.0); // Weight wb in order to bias it not to move

  //------------------------------
  // Make a backward pass over the knot spans with full optimisation of keys
  // but with smoothness constraints applied to the end tangent angular
  // velocities in order to help preserve the C1 continuity smoothness
  // Note: This does not mean that the end tangents can not move, only that
  // they are algebraically weighted to be equal to their initial values
  for (uint32_t i = numSpans - 2; i >= 1; --i)
  {
    smoothC0(i, 1.0, 1.0);
  }

  //------------------------------
  // Make a forward pass over the knot spans to optimise the mid keys and
  // the mid tangent vectors with C1 continuity. The end tangent vectors
  // are explicitly fixed in order not to undo the C1 continuity between
  // adjacent spans after each optimisation step.
  for (uint32_t i = 2; i < numSpans; ++i)
  {
    smoothC1(i);
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void QuatSplineFitterSmoothedC1::smoothC0(uint32_t indx, float weight_wa, float weight_wb)
{
  const SpanInfoSmoothed& info1 = m_spanInfoExc[indx-1]; // First span information
  const SpanInfoSmoothed& info2 = m_spanInfoInc[indx]; // Second span information
  NMP_ASSERT(info1.m_numSpanSamples + info2.m_numSpanSamples > 0);

  float   ta, tb, tc;
  float   alpha, omalpha, alpha_a, alpha_b;
  Quat    qa, qb, qc, dqb, iqb, q;
  Vector3 wa, w1b, w2a, wc;
  Vector3 va, vb, v, v_, rb;
  float   d, db, da, interp, beta;
  float knots_[2];
  bool status;

  //------------------------------
  // Get the quaternion spline parameter data for the two curve spans
  // Note that indx is the index of the mid knot point between the
  // adjacent curve spans
  ta = m_qsp.getKnot(indx - 1);
  tb = m_qsp.getKnot(indx);
  tc = m_qsp.getKnot(indx + 1);
  qa = m_qsp.getKey(indx - 1);
  qb = m_qsp.getKey(indx);
  qc = m_qsp.getKey(indx + 1);
  wa = m_qsp.getVelA(indx - 1);
  w1b = m_qsp.getVelB(indx - 1);
  w2a = m_qsp.getVelA(indx);
  wc = m_qsp.getVelB(indx);

  //------------------------------
  // Compute the continuity alpha between the two curve spans
  alpha = (tb - ta) / (tc - ta);
  omalpha = 1 - alpha;
  alpha_b = 1 / omalpha;
  alpha_a = 1 / alpha;

  //------------------------------
  // Compute the C1 continuous mid tangent vector (parameterised)
  // from the two mid tangents of both spans.
  // Note: (1-alpha) * w1b = alpha * w2a  (from first derivative at the mid knot site)
  vb = w1b * omalpha;
  va = w2a * alpha;
  d = vb.dot(va); // Check if tangent vectors have the same direction
  status = false;
  if (d >= 0)
  {
    // Compute the common tangent vector axis direction
    v_ = va + vb;
    v_.normalise();
    db = vb.dot(v_); // Component of vb in the common tangent direction
    da = va.dot(v_); // Component of va in the common tangent direction

    // Check if there is a large discontinuity between the tangents
    interp = da / (da + db);
    if (interp > 0.25f && interp < 0.75f)
    {
      // Set the C1 continuous mid tangent vector
      v = v_ * (0.5f * (db + da));
      status = true;
    }
  } // If not then tangents are unreliable

  //------------------------------
  // Deal with large differences between the tangent vectors. Large
  // differences generally have poor convergence during optimisation. Instead
  // of effectively averaging the coupled tangents we fit a curve to the
  // data between the mid points of the adjoining spans and compute the C1
  // continuous tangent vector and key at the mid knot site explicitly.
  if (!status)
  {
    // Fit a quaternion spline to the data samples
    knots_[0] = 0.5f * (ta + tb);
    knots_[1] = 0.5f * (tb + tc);
    m_tangentFitter.setKnots(2, knots_);

    // Fit the single span quaternion curve using the fitter's own copy of the
    // data samples in order to avoid reconditioning the quaternion data.
    m_tangentFitter.fit(
      m_tangentFitter.getNumSamples(),
      m_tangentFitter.getTs(),
      m_tangentFitter.getQuats());

    // Compute the quaternion key and derivative at tb
    const QuatSpline& qsp = m_tangentFitter.getQuatSpline();
    qb = qsp.evaluate(tb);
    dqb = qsp.derivative1(tb);

    // Recover the angular velocity vector w2a at tb by inserting a knot at tb
    // Note that the knot must be inserted as though it were on a curve with the
    // original spanning segments [t1,t2,t3] in order to retrieve the correct
    // tangents for it. dq(t)/dt = q2 * (3 * w2a) / (t3 - t2)
    beta = (tc - tb) / 3;
    iqb = qb;
    iqb.conjugate();
    q = iqb * dqb;

    // Compute the parameterised C1 continuous w vector
    beta *= alpha;
    v.set(beta * q.x, beta * q.y, beta * q.z);
  }

  //------------------------------
  // Parameterise the mid quaternion key
  rb = qb.log();

  // Set the span information data
  m_lsqFuncC0->setSpanInfo(
    &info1,
    &info2,
    ta,
    tb,
    tc,
    qa,
    qc,
    alpha_a,
    alpha_b,
    wa,
    wc,
    weight_wa,
    weight_wb);

  //------------------------------
  // Set the parameter vector to optimise: p = [wa; rb; v; wc]
  m_P0[0] = (double)wa.x;
  m_P0[1] = (double)wa.y;
  m_P0[2] = (double)wa.z;
  m_P0[3] = (double)rb.x;
  m_P0[4] = (double)rb.y;
  m_P0[5] = (double)rb.z;
  m_P0[6] = (double)v.x;
  m_P0[7] = (double)v.y;
  m_P0[8] = (double)v.z;
  m_P0[9] = (double)wc.x;
  m_P0[10] = (double)wc.y;
  m_P0[11] = (double)wc.z;

  //------------------------------
  // Optimise the parameter vector using Levenberg-Marquardt method
  m_optimiserC0->compute(m_P0);
  status = m_optimiserC0->getStatus() != NonLinearOptimiserStatus::Invalid;
  NMP_ASSERT(status);

  //------------------------------
  // Extract the parameter vector: p = [wa; rb; v; wc]
  m_P0 = m_optimiserC0->getP();
  wa.x = (float)m_P0[0];
  wa.y = (float)m_P0[1];
  wa.z = (float)m_P0[2];
  rb.x = (float)m_P0[3];
  rb.y = (float)m_P0[4];
  rb.z = (float)m_P0[5];
  v.x = (float)m_P0[6];
  v.y = (float)m_P0[7];
  v.z = (float)m_P0[8];
  wc.x = (float)m_P0[9];
  wc.y = (float)m_P0[10];
  wc.z = (float)m_P0[11];

  // Compute the tangent vectors and the mid key
  qb.exp(rb);
  w1b = v * alpha_b;
  w2a = v * alpha_a;

  //------------------------------
  // Update the components of the quaternion spline
  m_qsp.setKey(indx, qb);
  m_qsp.setVelA(indx - 1, wa);
  m_qsp.setVelB(indx - 1, w1b);
  m_qsp.setVelA(indx, w2a);
  m_qsp.setVelB(indx, wc);
}

//----------------------------------------------------------------------------------------------------------------------
void QuatSplineFitterSmoothedC1::smoothC1(uint32_t indx)
{
  const SpanInfoSmoothed& info1 = m_spanInfoExc[indx-1]; // First span information
  const SpanInfoSmoothed& info2 = m_spanInfoInc[indx]; // Second span information
  NMP_ASSERT(info1.m_numSpanSamples + info2.m_numSpanSamples > 0);

  float   ta, tb, tc;
  float   alpha, omalpha, alpha_a, alpha_b;
  Quat    qa, qb, qc, dqb, iqb, q;
  Vector3 wa, w1b, w2a, wc;
  Vector3 va, vb, v, v_, rb;
  float   d, db, da, interp, beta;
  float knots_[2];
  bool status;

  //------------------------------
  // Get the quaternion spline parameter data for the two curve spans
  // Note that indx is the index of the mid knot point between the
  // adjacent curve spans
  ta = m_qsp.getKnot(indx - 1);
  tb = m_qsp.getKnot(indx);
  tc = m_qsp.getKnot(indx + 1);
  qa = m_qsp.getKey(indx - 1);
  qb = m_qsp.getKey(indx);
  qc = m_qsp.getKey(indx + 1);
  wa = m_qsp.getVelA(indx - 1);
  w1b = m_qsp.getVelB(indx - 1);
  w2a = m_qsp.getVelA(indx);
  wc = m_qsp.getVelB(indx);

  //------------------------------
  // Compute the continuity alpha between the two curve spans
  alpha = (tb - ta) / (tc - ta);
  omalpha = 1 - alpha;
  alpha_b = 1 / omalpha;
  alpha_a = 1 / alpha;

  //------------------------------
  // Compute the C1 continuous mid tangent vector (parameterised)
  // from the two mid tangents of both spans.
  // Note: (1-alpha) * w1b = alpha * w2a  (from first derivative at the mid knot site)
  vb = w1b * omalpha;
  va = w2a * alpha;
  d = vb.dot(va); // Check if tangent vectors have the same direction
  status = false;
  if (d >= 0)
  {
    // Compute the common tangent vector axis direction
    v_ = va + vb;
    v_.normalise();
    db = vb.dot(v_); // Component of vb in the common tangent direction
    da = va.dot(v_); // Component of va in the common tangent direction

    // Check if there is a large discontinuity between the tangents
    interp = da / (da + db);
    if (interp > 0.25f && interp < 0.75f)
    {
      // Set the C1 continuous mid tangent vector
      v = v_ * (0.5f * (db + da));
      status = true;
    }
  } // If not then tangents are unreliable

  //------------------------------
  // Deal with large differences between the tangent vectors. Large
  // differences generally have poor convergence during optimisation. Instead
  // of effectively averaging the coupled tangents we fit a curve to the
  // data between the mid points of the adjoining spans and compute the C1
  // continuous tangent vector and key at the mid knot site explicitly.
  if (!status)
  {
    // Fit a quaternion spline to the data samples
    knots_[0] = 0.5f * (ta + tb);
    knots_[1] = 0.5f * (tb + tc);
    m_tangentFitter.setKnots(2, knots_);

    // Fit the single span quaternion curve using the fitter's own copy of the
    // data samples in order to avoid reconditioning the quaternion data.
    m_tangentFitter.fit(
      m_tangentFitter.getNumSamples(),
      m_tangentFitter.getTs(),
      m_tangentFitter.getQuats());

    // Compute the quaternion key and derivative at tb
    const QuatSpline& qsp = m_tangentFitter.getQuatSpline();
    qb = qsp.evaluate(tb);
    dqb = qsp.derivative1(tb);

    // Recover the angular velocity vector w2a at tb by inserting a knot at tb
    // Note that the knot must be inserted as though it were on a curve with the
    // original spanning segments [t1,t2,t3] in order to retrieve the correct
    // tangents for it. dq(t)/dt = q2 * (3 * w2a) / (t3 - t2)
    beta = (tc - tb) / 3;
    iqb = qb;
    iqb.conjugate();
    q = iqb * dqb;

    // Compute the parameterised C1 continuous w vector
    beta *= alpha;
    v.set(beta * q.x, beta * q.y, beta * q.z);
  }

  //------------------------------
  // Parameterise the mid quaternion key
  rb = qb.log();

  // Set the span information data
  m_lsqFuncC1->setSpanInfo(
    &info1,
    &info2,
    ta,
    tb,
    tc,
    qa,
    qc,
    alpha_a,
    alpha_b,
    wa,
    wc);

  //------------------------------
  // Set the parameter vector to optimise: p = [rb; v]
  m_P1[0] = (double)rb.x;
  m_P1[1] = (double)rb.y;
  m_P1[2] = (double)rb.z;
  m_P1[3] = (double)v.x;
  m_P1[4] = (double)v.y;
  m_P1[5] = (double)v.z;

  //------------------------------
  // Optimise the parameter vector using Levenberg-Marquardt method
  m_optimiserC1->compute(m_P1);
  status = m_optimiserC1->getStatus() != NonLinearOptimiserStatus::Invalid;
  NMP_ASSERT(status);

  //------------------------------
  // Extract the parameter vector: p = [rb; v]
  m_P1 = m_optimiserC1->getP();
  rb.x = (float)m_P1[0];
  rb.y = (float)m_P1[1];
  rb.z = (float)m_P1[2];
  v.x = (float)m_P1[3];
  v.y = (float)m_P1[4];
  v.z = (float)m_P1[5];

  // Compute the tangent vectors and the mid key
  qb.exp(rb);
  w1b = v * alpha_b;
  w2a = v * alpha_a;

  //------------------------------
  // Update the components of the quaternion spline
  m_qsp.setKey(indx, qb);
  m_qsp.setVelB(indx - 1, w1b);
  m_qsp.setVelA(indx, w2a);
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
