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
#include "NMNumerics/NMQuatSplineFitterInsertKnot.h"
#include "NMNumerics/NMSimpleSplineFittingUtils.h"
#include "NMNumerics/NMQuatUtils.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// QuatSplineFitterInsertKnot
//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterInsertKnot::
QuatSplineFitterInsertKnot(
  uint32_t          numSamples,
  const float*      ts,
  const Quat*       quats,
  const QuatSpline& qsp,
  float             t)
//
  : QuatSplineFitterBase(numSamples),
    m_P(12),
    m_weight_wa(0.1f), // Try to preserve continuity at ta
    m_weight_wc(0.1f) // Try to preserve continuity at tc
{
  NMP_ASSERT(m_maxNumSamples >= 2);

  // Allocate a new non-linear fitting function
  m_lsqFunc = new QuatSplineFitterSmoothedC0FuncJac(m_maxNumSamples);

  // Allocate a new optimiser
  m_optimiser = new NonLinearOptimiser<double>(*m_lsqFunc);
  m_optimiser->setMaxIterations(10);
  m_optimiser->setMaxFuncEvals(10);
  m_optimiser->setTolP(1e-6);
  m_optimiser->setTolFunc(1e-6);

  // Set the data samples
  setTs(numSamples, ts);
  setQuatsRaw(numSamples, quats);

  // Set the quaternion spline
  setQuatSpline(qsp);

  // Insert the specified knot site
  insertKnot(t);
}

//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterInsertKnot::
QuatSplineFitterInsertKnot(uint32_t maxNumSamples) :
  QuatSplineFitterBase(maxNumSamples),
  m_P(12),
  m_weight_wa(0.1f), // Try to preserve continuity at ta
  m_weight_wc(0.1f) // Try to preserve continuity at tc
{
  NMP_ASSERT(m_maxNumSamples >= 2);

  // Allocate a new non-linear fitting function
  m_lsqFunc = new QuatSplineFitterSmoothedC0FuncJac(m_maxNumSamples);

  // Allocate a new optimiser
  m_optimiser = new NonLinearOptimiser<double>(*m_lsqFunc);
  m_optimiser->setMaxIterations(10);
  m_optimiser->setMaxFuncEvals(10);
  m_optimiser->setTolP(1e-6);
  m_optimiser->setTolFunc(1e-6);
}

//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterInsertKnot::~QuatSplineFitterInsertKnot()
{
  delete m_lsqFunc;
  delete m_optimiser;
}

//----------------------------------------------------------------------------------------------------------------------
bool QuatSplineFitterInsertKnot::insertKnot(float t)
{
  float    ta, tb, tc;
  float    alpha, omalpha, alpha_a, alpha_b;
  Quat     qa, qb, qc, dqb, iqb, q;
  Vector3  wa, w1b, w2a, wc, v, rb;
  uint32_t indx, ia, ib;
  bool status;

  //------------------------------
  // Insert a knot into the quaternion spline
  if (!m_qsp.insertKnot(t)) return false;

  // Find the index of the inserted knot
  indx = m_qsp.closestKnot(t);

  //------------------------------
  // Get the data samples for the left span
  status = SimpleSplineFittingUtils::findSamplesExclusive(
             m_qsp.getKnot(indx - 1),
             m_qsp.getKnot(indx),
             ia,
             ib,
             m_numSamples,
             m_ts);
  if (status)
  {
    m_spanInfo1.m_numSpanSamples = 1 + ib - ia;
    m_spanInfo1.m_spanTs = &m_ts[ia];
    m_spanInfo1.m_spanQuats = &m_quats[ia];
  }
  else
  {
    m_spanInfo1.clear();
  }

  //------------------------------
  // Get the data samples for the right span
  status = SimpleSplineFittingUtils::findSamplesInclusiveTa(
             m_qsp.getKnot(indx),
             m_qsp.getKnot(indx + 1),
             ia,
             ib,
             m_numSamples,
             m_ts);
  if (status)
  {
    m_spanInfo2.m_numSpanSamples = 1 + ib - ia;
    m_spanInfo2.m_spanTs = &m_ts[ia];
    m_spanInfo2.m_spanQuats = &m_quats[ia];
  }
  else
  {
    m_spanInfo2.clear();
  }

  // Parameter count: 4*x (quat data coefficients) >= 12 (parameters)
  // for a full rank system
  if (m_spanInfo1.m_numSpanSamples + m_spanInfo2.m_numSpanSamples < 3)
  {
    return true; // Will have an under conditioned system
  }

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
  // Parameterise the mid quaternion key and tangent vector
  v = w2a * alpha;
  rb = qb.log();

  // Set the span information data
  m_lsqFunc->setSpanInfo(
    &m_spanInfo1,
    &m_spanInfo2,
    ta,
    tb,
    tc,
    qa,
    qc,
    alpha_a,
    alpha_b,
    wa,
    wc,
    m_weight_wa,
    m_weight_wc);

  //------------------------------
  // Set the parameter vector to optimise: p = [wa; rb; v; wc]
  m_P[0] = (double)wa.x;
  m_P[1] = (double)wa.y;
  m_P[2] = (double)wa.z;
  m_P[3] = (double)rb.x;
  m_P[4] = (double)rb.y;
  m_P[5] = (double)rb.z;
  m_P[6] = (double)v.x;
  m_P[7] = (double)v.y;
  m_P[8] = (double)v.z;
  m_P[9] = (double)wc.x;
  m_P[10] = (double)wc.y;
  m_P[11] = (double)wc.z;

  //------------------------------
  // Optimise the parameter vector using Levenberg-Marquardt method
  m_optimiser->compute(m_P);
  status = m_optimiser->getStatus() != NonLinearOptimiserStatus::Invalid;
  NMP_ASSERT(status);

  //------------------------------
  // Extract the parameter vector: p = [wa; rb; v; wc]
  m_P = m_optimiser->getP();
  wa.x = (float)m_P[0];
  wa.y = (float)m_P[1];
  wa.z = (float)m_P[2];
  rb.x = (float)m_P[3];
  rb.y = (float)m_P[4];
  rb.z = (float)m_P[5];
  v.x = (float)m_P[6];
  v.y = (float)m_P[7];
  v.z = (float)m_P[8];
  wc.x = (float)m_P[9];
  wc.y = (float)m_P[10];
  wc.z = (float)m_P[11];

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

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
