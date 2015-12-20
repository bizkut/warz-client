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
#include "NMNumerics/NMQuatSplineFitterTangents.h"
#include "NMNumerics/NMSimpleSplineFittingUtils.h"
#include "NMNumerics/NMQuatUtils.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// QuatSplineFitterSingleSpanTangentsFuncJac
//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterSingleSpanTangentsFuncJac::
QuatSplineFitterSingleSpanTangentsFuncJac(uint32_t maxNumSamples) :
  NonLinearLeastSquaresFuncJac<double>(6, 4 * maxNumSamples),
  m_mat4_3(4, 3)
{
  NMP_ASSERT(maxNumSamples > 0);
}

//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterSingleSpanTangentsFuncJac::~QuatSplineFitterSingleSpanTangentsFuncJac()
{

}

//----------------------------------------------------------------------------------------------------------------------
bool QuatSplineFitterSingleSpanTangentsFuncJac::func(
  const NMP::NonLinearOptimiserBase<double>& NMP_UNUSED(optimiser),
  const NMP::Vector<double>& P,
  NMP::Vector<double>&       fRes,
  NMP::JacobianBase<double>* J)
{
  NMP_ASSERT(m_spanTs);
  NMP_ASSERT(m_spanQuats);
  NMP_ASSERT(m_numSpanSamples > 0);

  NMP::Jacobian<double>& Jac = static_cast< NMP::Jacobian<double>& >(*J);
  NMP::Matrix<double>& A = Jac.getMatrix();

  Quat q, qt;
  Vector3 wa, wb;
  uint32_t indx;

  // Unpack the parameter vector: P = [wa, wb]
  wa.x = (float)P[0];
  wa.y = (float)P[1];
  wa.z = (float)P[2];
  wb.x = (float)P[3];
  wb.y = (float)P[4];
  wb.z = (float)P[5];

  // Compute the Bezier control points
  m_JacBuilder.w2Jac(m_qa, wa, wb, m_qb);

  // Compute the Jacobian and residuals corresponding to each data site
  // We don't include the first and last samples because they exactly
  // interpolate the keyframes (residuals and Jacobians are zero)
  indx = 0;
  for (uint32_t i = 0; i < m_numSpanSamples; ++i, indx += 4)
  {
    // Compute the Bezier quaternion spline q(t) and its Jacobian Dq(t) / D[w1, w2, w3]
    m_JacBuilder.valJac(m_qa, wa, m_JacBuilder.getW2(), wb, m_spanTs[i], m_ta, m_tb);

    // Compute the Jacobian Dq(t) / Dwa
    m_mat4_3.setFromMultiplication(m_JacBuilder.getDqtByDw2(), m_JacBuilder.getDw2ByDwa());
    m_mat4_3 += m_JacBuilder.getDqtByDw1();
    A.setSubMatrix(indx, 0, m_mat4_3);

    // Compute the Jacobian Dq(t) / Dwb
    m_mat4_3.setFromMultiplication(m_JacBuilder.getDqtByDw2(), m_JacBuilder.getDw2ByDwb());
    m_mat4_3 += m_JacBuilder.getDqtByDw3();
    A.setSubMatrix(indx, 3, m_mat4_3);

    // Compute the residuals
    qt = m_JacBuilder.getQt();
    q = m_spanQuats[i];
    fRes[indx] = (double)(qt.x - q.x);
    fRes[indx+1] = (double)(qt.y - q.y);
    fRes[indx+2] = (double)(qt.z - q.z);
    fRes[indx+3] = (double)(qt.w - q.w);
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void QuatSplineFitterSingleSpanTangentsFuncJac::
setSpanInfo(
  uint32_t     numSpanSamples,
  const float* spanTs,
  const Quat*  spanQuats,
  float        ta,
  float        tb,
  const Quat&  qa,
  const Quat&  qb)
{
  NMP_ASSERT(numSpanSamples > 0);
  NMP_ASSERT(spanTs);
  NMP_ASSERT(spanQuats);
  NMP_ASSERT(ta < tb);

  // Set the span information data
  m_numSpanSamples = numSpanSamples;
  m_spanTs = spanTs;
  m_spanQuats = spanQuats;
  m_ta = ta;
  m_tb = tb;
  m_qa = qa;
  m_qb = qb;

  // Set the actual number of residuals for optimisation
  setNumResiduals(4 * m_numSpanSamples);
}

//----------------------------------------------------------------------------------------------------------------------
// QuatSplineFitterTangents
//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterTangents::
QuatSplineFitterTangents(uint32_t numSamples, const float* ts, const Quat* quats, uint32_t numKnots, const float* knots) :
  QuatSplineFitterBase(numSamples),
  m_qspan(2),
  m_P(6)
{
  NMP_ASSERT(m_maxNumSamples >= 2);

  // Allocate a new non-linear fitting function
  m_lsqFunc = new QuatSplineFitterSingleSpanTangentsFuncJac(m_maxNumSamples);

  // Allocate a new optimiser
  m_optimiser = new NonLinearOptimiser<double>(*m_lsqFunc);
  m_optimiser->setMaxIterations(10);
  m_optimiser->setMaxFuncEvals(10);
  m_optimiser->setTolP(1e-6);
  m_optimiser->setTolFunc(1e-6);

  // Set the knot vector
  setKnots(numKnots, knots);

  // Fit a quaternion spline to the data samples
  fit(numSamples, ts, quats);
}

//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterTangents::
QuatSplineFitterTangents(uint32_t numSamples, const Quat* quats, uint32_t numKnots, const float* knots) :
  QuatSplineFitterBase(numSamples),
  m_qspan(2),
  m_P(6)
{
  NMP_ASSERT(m_maxNumSamples >= 2);

  // Allocate a new non-linear fitting function
  m_lsqFunc = new QuatSplineFitterSingleSpanTangentsFuncJac(m_maxNumSamples);

  // Allocate a new optimiser
  m_optimiser = new NonLinearOptimiser<double>(*m_lsqFunc);
  m_optimiser->setMaxIterations(10);
  m_optimiser->setMaxFuncEvals(10);
  m_optimiser->setTolP(1e-6);
  m_optimiser->setTolFunc(1e-6);

  // Set the knot vector
  setKnots(numKnots, knots);

  // Fit a quaternion spline to the data samples
  fitRegular(numSamples, quats);
}

//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterTangents::QuatSplineFitterTangents(uint32_t maxNumSamples) :
  QuatSplineFitterBase(maxNumSamples),
  m_qspan(2),
  m_P(6)
{
  NMP_ASSERT(m_maxNumSamples >= 2);

  // Allocate a new non-linear fitting function
  m_lsqFunc = new QuatSplineFitterSingleSpanTangentsFuncJac(m_maxNumSamples);

  // Allocate a new optimiser
  m_optimiser = new NonLinearOptimiser<double>(*m_lsqFunc);
  m_optimiser->setMaxIterations(10);
  m_optimiser->setMaxFuncEvals(10);
  m_optimiser->setTolP(1e-6);
  m_optimiser->setTolFunc(1e-6);
}

//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterTangents::~QuatSplineFitterTangents()
{
  delete m_lsqFunc;
  delete m_optimiser;
}

//----------------------------------------------------------------------------------------------------------------------
bool QuatSplineFitterTangents::fit(uint32_t numSamples, const float* ts, const Quat* quats)
{
  NMP_ASSERT(ts);
  NMP_ASSERT(quats);
  NMP_ASSERT(numSamples >= 2);

  bool     status, result;
  Quat     qa, qb, qm;
  Vector3  wa, wb;
  float    ta, tb, tm, tau;
  uint32_t ia, ib, im;
  uint32_t numSpanSamples;
  const float* spanTs;
  const Quat* spanQuats;
  const float tm_tol = 0.2f;

  //------------------------------
  // Get the knot vector
  uint32_t numKnots = m_qsp.getNumKnots();
  const float* knots = m_qsp.getKnots();
  NMP_ASSERT(SimpleSplineFittingUtils::isKnotVectorValid(numKnots, knots));

  // Set the time samples
  setTs(numSamples, ts);

  // Condition the quaternion data so that consecutive quaternions lie in the same hemisphere
  setQuatsConditioned(numSamples, quats);

  // Information
  uint32_t numSpans = numKnots - 1;

  // First quaternion key
  ta = knots[0];
  qa = SimpleSplineFittingUtils::quatInterpolate(ta, m_numSamples, m_ts, m_quats);
  m_qsp.setKey(0, qa);

  //------------------------------
  // Iterate over the knot spans and fit single span quaternion curves
  result = true;
  for (uint32_t i = 0; i < numSpans; ++i)
  {
    // Recover the knot sites
    tb = knots[i+1];

    // Compute the interpolated quaternions at the knot sites
    qb = SimpleSplineFittingUtils::quatInterpolate(tb, m_numSamples, m_ts, m_quats);

    // Recover the data samples within the knot vector. We exclude samples that are located at
    // ta or tb since the Jacobian will be zero
    status = SimpleSplineFittingUtils::findSamplesExclusive(ta, tb, ia, ib, m_numSamples, m_ts);
    if (!status)
    {
      // Get the SLERP angular velocity
      m_qspan.set(ta, tb, qa, qb);
      wa = m_qspan.getVelA(0);
      wb = wa;
    }
    else
    {
      // Get the span data
      numSpanSamples = ib - ia + 1;
      spanTs = &m_ts[ia];
      spanQuats = &m_quats[ia];

      // Check for the identity channel
      if (SimpleSplineFittingUtils::isQuatChannelUnchanging(qa, qb) &&
          SimpleSplineFittingUtils::isQuatChannelUnchanging(qa, numSpanSamples, spanQuats))
      {
        wa.setToZero();
        wb.setToZero();
      }
      else
      {
        //------------------------------
        // General case
        //

        // Find the data sample index closest to the mid time value
        status = false;
        tm = 0.5f * (ta + tb);
        if (SimpleSplineFittingUtils::findSampleSpan(tm, im, ia, ib, m_numSamples, m_ts) == 0)
        {
          tm = m_ts[im]; // Closest sample
          qm = m_quats[im];

          // Check that the time sample mid point is not close to either endpoint
          tau = (tm - ta) / (tb - ta);
          if (tau > tm_tol && 1 - tau > tm_tol)
          {
            status = true; // Ok to use mid point to split into two halves
          }
        }

        // Compute the angular velocity vectors
        if (status)
        {
          // Compute the SLERP control points that approximate the first half
          // of the samples
          m_qspan.set(ta, tm, qa, qm);
          wa = m_qspan.getVelA(0);

          // Compute the SLERP control points that approximate the second half
          // of the samples
          m_qspan.set(tm, tb, qm, qb);
          wb = m_qspan.getVelB(0);
        }
        else
        {
          // Compute the SLERP control points that approximate the both halves
          // of the samples
          m_qspan.set(ta, tb, qa, qb);
          wa = m_qspan.getVelA(0);
          wb = wa;
        }

        //------------------------------
        // Set the quaternion span information
        m_lsqFunc->setSpanInfo(
          numSpanSamples,
          spanTs,
          spanQuats,
          ta,
          tb,
          qa,
          qb);

        //------------------------------
        // Set the parameter vector to optimise
        m_P[0] = (double)wa.x;
        m_P[1] = (double)wa.y;
        m_P[2] = (double)wa.z;
        m_P[3] = (double)wb.x;
        m_P[4] = (double)wb.y;
        m_P[5] = (double)wb.z;

        //------------------------------
        // Fit a single span to the quaternion sample data. This is achieved by
        // performing non-linear parameter estimation using the Levenberg-Marquardt
        // optimisation method
        m_optimiser->compute(m_P);
        status = m_optimiser->getStatus() != NonLinearOptimiserStatus::Invalid;
        result &= status;
        NMP_ASSERT(status);

        //------------------------------
        // Recover the optimised angular velocities
        m_P = m_optimiser->getP();
        wa.x = (float)m_P[0];
        wa.y = (float)m_P[1];
        wa.z = (float)m_P[2];
        wb.x = (float)m_P[3];
        wb.y = (float)m_P[4];
        wb.z = (float)m_P[5];

      } // End of general case
    }

    //------------------------------
    // Append the single span curve segment
    m_qsp.setVelA(i, wa);
    m_qsp.setVelB(i, wb);
    m_qsp.setKey(i + 1, qb);

    // Update the iteration parameters
    ta = tb;
    qa = qb;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
