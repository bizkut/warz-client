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
#include "NMNumerics/NMQuatSplineFitterContinuityC1.h"
#include "NMNumerics/NMSimpleSplineFittingUtils.h"
#include "NMNumerics/NMQuatUtils.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// QuatSplineFitterContinuityC1FuncJac::SpanInfoC1
//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterContinuityC1FuncJac::SpanInfoC1::SpanInfoC1()
{

}

//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterContinuityC1FuncJac::SpanInfoC1::~SpanInfoC1()
{

}

//----------------------------------------------------------------------------------------------------------------------
void QuatSplineFitterContinuityC1FuncJac::SpanInfoC1::clear()
{
  m_numSpanSamples = 0;
  m_spanTs = 0;
  m_spanQuats = 0;
  m_ta = 0;
  m_tb = 0;
  m_jRow = 0;
  m_jSize = 0;
}

//----------------------------------------------------------------------------------------------------------------------
// QuatSplineFitterContinuityC1FuncJac
//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterContinuityC1FuncJac::
QuatSplineFitterContinuityC1FuncJac(uint32_t maxNumSamples, uint32_t numKnots) :
  NonLinearLeastSquaresFuncJac<double>(6 * numKnots, 4 * maxNumSamples),
  m_numKnots(numKnots),
  m_spanInfo(0),
  m_dqa_by_dra(4, 3),
  m_dqb_by_drb(4, 3),
  m_dw2_by_dra(3, 3),
  m_dw2_by_drb(3, 3),
  m_matA4_3(4, 3),
  m_matB4_3(4, 3)
{
  NMP_ASSERT(maxNumSamples > 0);
  NMP_ASSERT(numKnots >= 2);

  // Allocate the memory for the span information data
  uint32_t numSpans = numKnots - 1;
  m_spanInfo = new SpanInfoC1[numSpans];
  for (uint32_t i = 0; i < numSpans; ++i)
  {
    m_spanInfo[i].clear();
  }
}

//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterContinuityC1FuncJac::~QuatSplineFitterContinuityC1FuncJac()
{
  if (m_spanInfo)
  {
    delete[] m_spanInfo;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool QuatSplineFitterContinuityC1FuncJac::func(
  const NMP::NonLinearOptimiserBase<double>& NMP_UNUSED(optimiser),
  const NMP::Vector<double>& P,
  NMP::Vector<double>&       fRes,
  NMP::JacobianBase<double>* J)
{
  NMP_ASSERT(m_numKnots >= 2);
  NMP_ASSERT(m_spanInfo);

  Quat    qa, qb, qt, q;
  Vector3 ra, rb, wa, wb;

  // Jacobian blocks are stored as a (numRows by blockWidth) matrix
  NMP::JacobianBlockDiag<double>& Jac = static_cast< NMP::JacobianBlockDiag<double>& >(*J);
  NMP::Matrix<double>& A = Jac.getMatrix();

  // Information
  uint32_t numSpans = m_numKnots - 1;

  // Convert the first quat log vector ra back to a quaternion qa
  ra.x = (float)P[0];
  ra.y = (float)P[1];
  ra.z = (float)P[2];
  quatExpJac(ra, m_dqa_by_dra, qa);

  // Iterate over the curve spans
  uint32_t indx = 0;
  for (uint32_t i = 0; i < numSpans; ++i, indx += 6)
  {
    // Set the Jacobian block data information
    const SpanInfoC1& info = m_spanInfo[i];
    Jac.setBlockInfo(i, info.m_jRow, indx, info.m_jSize);

    // Get the parameters within P for the current curve span: [ra, va, rb, vb]
    wa.x = (float)P[indx+3];
    wa.y = (float)P[indx+4];
    wa.z = (float)P[indx+5];
    wa *= info.m_alpha_a;

    rb.x = (float)P[indx+6];
    rb.y = (float)P[indx+7];
    rb.z = (float)P[indx+8];

    wb.x = (float)P[indx+9];
    wb.y = (float)P[indx+10];
    wb.z = (float)P[indx+11];
    wb *= info.m_alpha_b;

    // Convert the quat log vector rb back to a quaternion qb
    quatExpJac(rb, m_dqb_by_drb, qb);

    // Compute the Bezier control points for the curve span
    m_JacBuilder.w2Jac(qa, wa, wb, qb);

    // Jacobians Dw2 / Dra and Dw2 / Drb
    m_dw2_by_dra.setFromMultiplication(m_JacBuilder.getDw2ByDqa(), m_dqa_by_dra);
    m_dw2_by_drb.setFromMultiplication(m_JacBuilder.getDw2ByDqb(), m_dqb_by_drb);

    // Samples
    uint32_t numSpanSamples = info.m_numSpanSamples;
    for (uint32_t k = 0; k < numSpanSamples; ++k)
    {
      uint32_t kindx = 4 * k + info.m_jRow;

      // Compute the Bezier quaternion spline q(t) and its Jacobian Dq(t) / D[q0, w1, w2, w3]
      m_JacBuilder.valJac(qa, wa, m_JacBuilder.getW2(), wb, info.m_spanTs[k], info.m_ta, info.m_tb);

      // Dq(t) / Dra
      m_matA4_3.setFromMultiplication(m_JacBuilder.getDqtByDq0(), m_dqa_by_dra);
      m_matB4_3.setFromMultiplication(m_JacBuilder.getDqtByDw2(), m_dw2_by_dra);
      m_matA4_3 += m_matB4_3;
      A.setSubMatrix(kindx, 0, m_matA4_3);

      // Dq(t) / Dwa
      m_matA4_3.setFromMultiplication(m_JacBuilder.getDqtByDw2(), m_JacBuilder.getDw2ByDwa());
      m_matA4_3 += m_JacBuilder.getDqtByDw1();
      m_matA4_3 *= info.m_alpha_a;
      A.setSubMatrix(kindx, 3, m_matA4_3);

      // Dq(t) / Drb
      m_matA4_3.setFromMultiplication(m_JacBuilder.getDqtByDw2(), m_dw2_by_drb);
      A.setSubMatrix(kindx, 6, m_matA4_3);

      // Dq(t) / Dwb
      m_matA4_3.setFromMultiplication(m_JacBuilder.getDqtByDw2(), m_JacBuilder.getDw2ByDwb());
      m_matA4_3 += m_JacBuilder.getDqtByDw3();
      m_matA4_3 *= info.m_alpha_b;
      A.setSubMatrix(kindx, 9, m_matA4_3);

      // Compute the residuals
      qt = m_JacBuilder.getQt();
      q = info.m_spanQuats[k];
      fRes[kindx] = (double)(qt.x - q.x);
      fRes[kindx+1] = (double)(qt.y - q.y);
      fRes[kindx+2] = (double)(qt.z - q.z);
      fRes[kindx+3] = (double)(qt.w - q.w);
    }

    // Update the current quat key and its Jacobian
    m_dqa_by_dra = m_dqb_by_drb;
    qa = qb;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void QuatSplineFitterContinuityC1FuncJac::
setSpanInfo(
  uint32_t     spanIndex,
  uint32_t     numSpanSamples,
  const float* spanTs,
  const Quat*  spanQuats,
  float        ta,
  float        tb,
  float        alpha_a,
  float        alpha_b,
  uint32_t     jRow,
  uint32_t     jSize)
{
  NMP_ASSERT(spanIndex < m_numKnots - 1);
  NMP_ASSERT(ta < tb);

  SpanInfoC1& info = m_spanInfo[spanIndex];
  info.m_numSpanSamples = numSpanSamples;
  info.m_spanTs = spanTs;
  info.m_spanQuats = spanQuats;
  info.m_ta = ta;
  info.m_tb = tb;
  info.m_alpha_a = alpha_a;
  info.m_alpha_b = alpha_b;
  info.m_jRow = jRow;
  info.m_jSize = jSize;
}

//----------------------------------------------------------------------------------------------------------------------
const QuatSplineFitterContinuityC1FuncJac::SpanInfoC1&
QuatSplineFitterContinuityC1FuncJac::getSpanInfo(uint32_t spanIndex) const
{
  NMP_ASSERT(spanIndex < m_numKnots - 1);
  return m_spanInfo[spanIndex];
}

//----------------------------------------------------------------------------------------------------------------------
// QuatSplineFitterContinuityC1
//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterContinuityC1::
QuatSplineFitterContinuityC1(uint32_t numSamples, const float* ts, const Quat* quats, uint32_t numKnots, const float* knots) :
  QuatSplineFitterBase(numSamples),
  m_P(6 * numKnots),
  m_smoothFitter(numSamples, numKnots),
  m_alphas(numKnots)
{
  NMP_ASSERT(m_maxNumSamples >= 2);

  // Allocate a new non-linear fitting function
  m_lsqFunc = new QuatSplineFitterContinuityC1FuncJac(m_maxNumSamples, numKnots);

  // Allocate a new optimiser
  uint32_t numSpans = numKnots - 1;
  uint32_t n = 10 + numSpans;
  m_optimiser = new NonLinearOptimiserBandDiag<double>(numSpans, 12, *m_lsqFunc);
  m_optimiser->setMaxIterations(n);
  m_optimiser->setMaxFuncEvals(n);
  m_optimiser->setTolP(1e-6);
  m_optimiser->setTolFunc(1e-6);

  // Set the knot vector
  setKnots(numKnots, knots);

  // Fit a quaternion spline to the data samples
  fit(numSamples, ts, quats);
}

//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterContinuityC1::
QuatSplineFitterContinuityC1(uint32_t numSamples, const Quat* quats, uint32_t numKnots, const float* knots) :
  QuatSplineFitterBase(numSamples),
  m_P(6 * numKnots),
  m_smoothFitter(numSamples, numKnots),
  m_alphas(numKnots)
{
  NMP_ASSERT(m_maxNumSamples >= 2);

  // Allocate a new non-linear fitting function
  m_lsqFunc = new QuatSplineFitterContinuityC1FuncJac(m_maxNumSamples, numKnots);

  // Allocate a new optimiser
  uint32_t numSpans = numKnots - 1;
  uint32_t n = 10 + numSpans;
  m_optimiser = new NonLinearOptimiserBandDiag<double>(numSpans, 12, *m_lsqFunc);
  m_optimiser->setMaxIterations(n);
  m_optimiser->setMaxFuncEvals(n);
  m_optimiser->setTolP(1e-6);
  m_optimiser->setTolFunc(1e-6);

  // Set the knot vector
  setKnots(numKnots, knots);

  // Fit a quaternion spline to the data samples
  fitRegular(numSamples, quats);
}

//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterContinuityC1::QuatSplineFitterContinuityC1(uint32_t maxNumSamples, uint32_t numKnots) :
  QuatSplineFitterBase(maxNumSamples),
  m_P(6 * numKnots),
  m_smoothFitter(maxNumSamples, numKnots),
  m_alphas(numKnots)
{
  NMP_ASSERT(m_maxNumSamples >= 2);

  // Allocate a new non-linear fitting function
  m_lsqFunc = new QuatSplineFitterContinuityC1FuncJac(m_maxNumSamples, numKnots);

  // Allocate a new optimiser
  uint32_t numSpans = numKnots - 1;
  uint32_t n = 10 + numSpans;
  m_optimiser = new NonLinearOptimiserBandDiag<double>(numSpans, 12, *m_lsqFunc);
  m_optimiser->setMaxIterations(n);
  m_optimiser->setMaxFuncEvals(n);
  m_optimiser->setTolP(1e-6);
  m_optimiser->setTolFunc(1e-6);
}

//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterContinuityC1::~QuatSplineFitterContinuityC1()
{
  delete m_lsqFunc;
  delete m_optimiser;
}

//----------------------------------------------------------------------------------------------------------------------
bool QuatSplineFitterContinuityC1::fit(uint32_t numSamples, const float* ts, const Quat* quats)
{
  NMP_ASSERT(ts);
  NMP_ASSERT(quats);
  NMP_ASSERT(numSamples >= 2);

  float    ta, tb;
  uint32_t ia, ib;
  uint32_t numSpans, numSpanSamples;
  uint32_t jRow, jSize, indx;
  float    alpha_a, alpha_b;
  bool status;
  Quat q;
  Vector3 r, v, wa, wb;

  // Get the knot vector
  uint32_t numKnots = m_qsp.getNumKnots();
  const float* knots = m_qsp.getKnots();
  NMP_ASSERT(SimpleSplineFittingUtils::isKnotVectorValid(numKnots, knots));

  //------------------------------
  // Compute an initial estimate of the quaternion spline
  m_smoothFitter.setKnots(numKnots, knots);
  m_smoothFitter.fit(numSamples, ts, quats);

  // Set the time samples
  setTs(numSamples, ts);

  // Set the conditioned quaternion samples
  setQuatsRaw(numSamples, m_smoothFitter.getQuats());

  // Set the output quaternion spline
  m_qsp = m_smoothFitter.getQuatSpline();

  // Check for a single span quaternion curve
  numSpans = numKnots - 1;
  if (numSpans == 1) return true;

  //------------------------------
  // Compute the knot continuity alphas
  m_alphas[0] = 1.0f;
  for (uint32_t i = 1; i < numSpans; ++i)
  {
    m_alphas[i] = (knots[i] - knots[i-1]) / (knots[i+1] - knots[i-1]);
  }
  m_alphas[numSpans] = 0.0f;

  //------------------------------
  // Set the span information data
  jRow = 0;
  for (uint32_t i = 0; i < numSpans; ++i)
  {
    // Knot sites
    ta = knots[i];
    tb = knots[i+1];

    // Knot continuity alphas
    alpha_a = 1 / m_alphas[i];
    alpha_b = 1 / (1 - m_alphas[i+1]);

    // Recover the data samples within the knot vector
    if (i < numSpans - 1)
    {
      // Exclude sample at tb to avoid computing the Jacobian twice
      status = SimpleSplineFittingUtils::findSamplesInclusiveTa(ta, tb, ia, ib, m_numSamples, m_ts);
    }
    else
    {
      // Last span: include samples at both ta and tb
      status = SimpleSplineFittingUtils::findSamplesInclusive(ta, tb, ia, ib, m_numSamples, m_ts);
    }

    // Compute the number of span samples
    if (status)
    {
      numSpanSamples = 1 + ib - ia;
    }
    else
    {
      numSpanSamples = 0;
    }
    jSize = 4 * numSpanSamples;

    // Set the span information data
    m_lsqFunc->setSpanInfo(
      i,
      numSpanSamples,
      &m_ts[ia],
      &m_quats[ia],
      ta,
      tb,
      alpha_a,
      alpha_b,
      jRow,
      jSize);

    // Update the Jacobian row index
    jRow += jSize;
  }

  // Set the number of residuals
  m_lsqFunc->setNumResiduals(jRow);

  //------------------------------
  // Pack parameter vector for optimisation:
  // P = [ r_0, v_0, r_1, v_1, ... , v_{n-1}, r_n, v_{n} ]
  indx = 0;
  for (uint32_t i = 0; i < numSpans; ++i, indx += 6)
  {
    // Quat spline key r_i (as log vector)
    q = m_qsp.getKey(i);
    r = q.log();
    m_P[indx] = (double)r.x;
    m_P[indx+1] = (double)r.y;
    m_P[indx+2] = (double)r.z;

    // Quat spline tangent (parameterised)
    v = m_qsp.getVelA(i);
    v *= m_alphas[i];
    m_P[indx+3] = (double)v.x;
    m_P[indx+4] = (double)v.y;
    m_P[indx+5] = (double)v.z;
  }

  // Last Quat spline key
  q = m_qsp.getKey(numSpans);
  r = q.log();
  m_P[indx] = (double)r.x;
  m_P[indx+1] = (double)r.y;
  m_P[indx+2] = (double)r.z;

  // Last Quat spline tangent (parameterised)
  v = m_qsp.getVelB(numSpans - 1);
  v *= (1 - m_alphas[numSpans]);
  m_P[indx+3] = (double)v.x;
  m_P[indx+4] = (double)v.y;
  m_P[indx+5] = (double)v.z;

  //------------------------------
  // Perform a sparse Levenberg-Marquardt optimisation to refine the parameters
  m_optimiser->compute(m_P);
  status = m_optimiser->getStatus() != NonLinearOptimiserStatus::Invalid;
  NMP_ASSERT(status);

  //------------------------------
  // Unpack the parameter vector after optimisation
  m_P = m_optimiser->getP();
  indx = 0;
  for (uint32_t i = 0; i < numSpans; ++i, indx += 6)
  {
    const QuatSplineFitterContinuityC1FuncJac::SpanInfoC1& info = m_lsqFunc->getSpanInfo(i);

    // Quat spline key r_i (as log vector)
    r.x = (float)m_P[indx];
    r.y = (float)m_P[indx+1];
    r.z = (float)m_P[indx+2];
    q.exp(r);
    m_qsp.setKey(i, q);

    // Quat spline velA
    v.x = (float)m_P[indx+3];
    v.y = (float)m_P[indx+4];
    v.z = (float)m_P[indx+5];
    wa = v * info.m_alpha_a;
    m_qsp.setVelA(i, wa);

    // Quat spline velB
    v.x = (float)m_P[indx+9];
    v.y = (float)m_P[indx+10];
    v.z = (float)m_P[indx+11];
    wb = v * info.m_alpha_b;
    m_qsp.setVelB(i, wb);
  }

  // Last Quat spline key
  r.x = (float)m_P[indx];
  r.y = (float)m_P[indx+1];
  r.z = (float)m_P[indx+2];
  q.exp(r);
  m_qsp.setKey(numSpans, q);

  return status;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
