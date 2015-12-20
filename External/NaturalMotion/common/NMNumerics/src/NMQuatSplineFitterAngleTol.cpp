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
#include "NMNumerics/NMQuatSplineFitterAngleTol.h"
#include "NMNumerics/NMSimpleSplineFittingUtils.h"

//----------------------------------------------------------------------------------------------------------------------
// Disable warnings constant conditionals
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning(disable : 4127)
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// QuatSplineFitterAngleTol
//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterAngleTol::
QuatSplineFitterAngleTol(uint32_t numSamples, const float* ts, const Quat* quats, float qTol) :
  QuatSplineFitterBase(numSamples),
  m_qTol(qTol),
  m_qspan(2), m_qspan_(2),
  m_P(6)
{
  // Allocate the memory for the data samples
  NMP_ASSERT(m_maxNumSamples >= 2);
  m_resampledQuats = new Quat[m_maxNumSamples];

  // Allocate a new non-linear fitting function
  m_lsqFunc = new QuatSplineFitterSingleSpanTangentsFuncJac(m_maxNumSamples);

  // Allocate a new optimiser
  m_optimiser = new NonLinearOptimiser<double>(*m_lsqFunc);
  m_optimiser->setMaxIterations(10);
  m_optimiser->setMaxFuncEvals(10);
  m_optimiser->setTolP(1e-6);
  m_optimiser->setTolFunc(1e-6);

  // Fit a quaternion spline to the data samples
  fit(numSamples, ts, quats);
}

//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterAngleTol::QuatSplineFitterAngleTol(uint32_t numSamples, const Quat* quats, float qTol) :
  QuatSplineFitterBase(numSamples),
  m_qTol(qTol),
  m_qspan(2), m_qspan_(2),
  m_P(6)
{
  // Allocate the memory for the data samples
  NMP_ASSERT(m_maxNumSamples >= 2);
  m_resampledQuats = new Quat[m_maxNumSamples];

  // Allocate a new non-linear fitting function
  m_lsqFunc = new QuatSplineFitterSingleSpanTangentsFuncJac(m_maxNumSamples);

  // Allocate a new optimiser
  m_optimiser = new NonLinearOptimiser<double>(*m_lsqFunc);
  m_optimiser->setMaxIterations(10);
  m_optimiser->setMaxFuncEvals(10);
  m_optimiser->setTolP(1e-6);
  m_optimiser->setTolFunc(1e-6);

  // Fit a quaternion spline to the data samples
  fitRegular(numSamples, quats);
}

//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterAngleTol::QuatSplineFitterAngleTol(uint32_t maxNumSamples) :
  QuatSplineFitterBase(maxNumSamples),
  m_qTol(0.026179938779915f), // 1.5 degrees
  m_qspan(2), m_qspan_(2),
  m_P(6)
{
  // Allocate the memory for the data samples
  NMP_ASSERT(m_maxNumSamples >= 2);
  m_resampledQuats = new Quat[m_maxNumSamples];

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
QuatSplineFitterAngleTol::~QuatSplineFitterAngleTol()
{
  delete[] m_resampledQuats;
  delete m_lsqFunc;
  delete m_optimiser;
}

//----------------------------------------------------------------------------------------------------------------------
float QuatSplineFitterAngleTol::getQTol() const
{
  return m_qTol;
}

//----------------------------------------------------------------------------------------------------------------------
void QuatSplineFitterAngleTol::setQTol(float qTol)
{
  NMP_ASSERT(qTol >= 0.0f);
  m_qTol = qTol;
}

//----------------------------------------------------------------------------------------------------------------------
Quat* QuatSplineFitterAngleTol::getResampledQuats()
{
  return m_resampledQuats;
}

//----------------------------------------------------------------------------------------------------------------------
const Quat* QuatSplineFitterAngleTol::getResampledQuats() const
{
  return m_resampledQuats;
}

//----------------------------------------------------------------------------------------------------------------------
bool QuatSplineFitterAngleTol::fit(uint32_t numSamples, const float* ts, const Quat* quats)
{
  uint32_t ka, kb, qspKb;
  Vector3  wa, wb;
  Quat q;
  bool status;

  // Validate the data samples
  NMP_ASSERT(numSamples >= 2 && numSamples <= m_maxNumSamples);

  // Initialise the fitting tolerance
  NMP_ASSERT(m_qTol >= 0.0f);
  float qTol = 1.0f - cosf(0.5f * m_qTol);

  // Set the time samples
  setTs(numSamples, ts);

  // Condition the quaternion data so that consecutive quaternions lie in the same hemisphere
  setQuatsConditioned(numSamples, quats);

  // Check for the identity channel
  if (SimpleSplineFittingUtils::isQuatChannelUnchanging(m_numSamples, m_quats))
  {
    m_qsp.set(m_ts[0], m_ts[m_numSamples-1], m_quats[0], m_quats[0]);
    for (uint32_t i = 0; i < m_numSamples; ++i) m_resampledQuats[i] = m_quats[0];
    return true;
  }

  // Initialise the result quaternion spline knots and coefficients
  ka = 0;
  m_qsp.begin(m_ts[0], m_quats[0]);
  m_resampledQuats[0] = m_quats[0];

  //------------------------------
  // Main iteration loop to build quaternion curve spans
  //
  while (ka < m_numSamples - 1)
  {
    // Set the initial parameters from the SLERP control points
    kb = ka + 1;
    m_qspan.set(m_ts[ka], m_ts[kb], m_quats[ka], m_quats[kb]);
    m_resampledQuats[kb] = m_quats[kb];

    // Check if quaternion curve interpolates all data samples
    kb++;
    if (kb >= m_numSamples)
    {
      m_qsp.append(m_qspan.getKnot(1), m_qspan.getVelA(0), m_qspan.getVelB(0), m_qspan.getKey(1));
      return true;
    }
    qspKb = kb;

    //------------------------------
    // Fit a single span quaternion SLERP curve to the data samples
    //
    while (true)
    {
      // Compute the SLERP control points
      m_qspan_.set(m_ts[ka], m_ts[kb], m_quats[ka], m_quats[kb]);
      m_resampledQuats[kb] = m_quats[kb];

      // Compute the angular error between the reconstructed quaternion
      // samples and the initial sample data. The errors should be largest
      // within the middle region of the fitted curve span
      status = false;
      for (uint32_t i = ka + 1; i < kb; ++i)
      {
        q = m_qspan_.evaluate(m_ts[i]);
        m_resampledQuats[i] = q;

        Quat dq;
        q.conjugate();
        dq.multiply(q, m_quats[i]);

        // Check for termination on the fitting angle tolerance. Note the test
        // for greater than (not >=) in SLERP test. Useful for zero tolerances
        float c = NMP::clampValue(fabs(dq.w), 0.0f, 1.0f);
        float d = 1.0f - c;
        if (d > qTol)
        {
          status = true;
          break;
        }
      }

      // Evaluate the user defined termination test
      if (!status)
      {
        // Resampled quats have been computed for all samples between ka and kb
        status = func(ka, kb);
      }

      // Check if the termination condition was met
      if (status)
      {
        // Compute the adjusted angular velocity that corresponds to the interpolation
        // with the valid quaternion spline qsp over the new interval [ka:kb]
        // i.e. qspan.vel interpolates the samples in the [ka:kb-1] interval, so
        // adjust qspan.vel to obtain the same interpolation but in the new
        // interval. If we use qa and the adjusted velA from qspan along with the
        // new qb and velB from qspan_ then this provides a better approximation
        // to the quaternion samples
        float u_ = (m_qspan_.getKnot(1) - m_qspan_.getKnot(0)) / (m_qspan.getKnot(1) - m_qspan.getKnot(0));
        wa = m_qspan.getVelA(0);
        wa *= u_;
        wb = m_qspan_.getVelB(0);
        m_P[0] = (double)wa.x;
        m_P[1] = (double)wa.y;
        m_P[2] = (double)wa.z;
        m_P[3] = (double)wb.x;
        m_P[4] = (double)wb.y;
        m_P[5] = (double)wb.z;
        break; // resampled quats are now invalid for parameter vector P
      }

      // Store the valid single span quaternion spline (resampled quats are valid)
      m_qspan = m_qspan_;
      qspKb = kb;

      // Increment the sample index
      kb++;

      // Check for termination on the number of data samples
      if (kb >= m_numSamples)
      {
        m_qsp.append(m_qspan.getKnot(1), m_qspan.getVelA(0), m_qspan.getVelB(0), m_qspan.getKey(1));
        return true; // resampled quats are valid
      }
    }

    //------------------------------
    // Fit a single span quaternion curve to the data samples
    //
    while (true)
    {
      // Set the quaternion span information
      m_lsqFunc->setSpanInfo(
        kb - ka - 1,
        &m_ts[ka+1],
        &m_quats[ka+1],
        m_ts[ka],
        m_ts[kb],
        m_quats[ka],
        m_quats[kb]);

      // Fit a single span to the quaternion sample data. This is achieved by
      // performing non-linear parameter estimation using the Levenberg-Marquardt
      // optimisation method
      m_optimiser->compute(m_P);
      NMP_ASSERT(m_optimiser->getStatus() != NonLinearOptimiserStatus::Invalid);

      // Make a single span quaternion spline
      m_P = m_optimiser->getP();
      wa.x = (float)m_P[0];
      wa.y = (float)m_P[1];
      wa.z = (float)m_P[2];
      wb.x = (float)m_P[3];
      wb.y = (float)m_P[4];
      wb.z = (float)m_P[5];
      m_qspan_.set(m_ts[ka], m_ts[kb], m_quats[ka], wa, wb, m_quats[kb]);
      m_resampledQuats[kb] = m_quats[kb];

      // Compute the angular error between the reconstructed quaternion
      // samples and the initial sample data. The errors should be largest
      // within the middle region of the fitted curve span
      status = false;
      for (uint32_t i = ka + 1; i < kb; ++i)
      {
        q = m_qspan_.evaluate(m_ts[i]);
        m_resampledQuats[i] = q;

        Quat dq;
        q.conjugate();
        dq.multiply(q, m_quats[i]);

        // Check for termination on the fitting angle tolerance
        float c = NMP::clampValue(fabs(dq.w), 0.0f, 1.0f);
        float d = 1.0f - c;
        if (d >= qTol)
        {
          status = true;
          break;
        }
      }

      // Evaluate the user defined termination test
      if (!status)
      {
        // Resampled quats have been computed for all samples between ka and kb
        status = func(ka, kb);
      }

      // Check if the termination condition was met
      if (status)
      {
        // You should be able to interpolate 3 quaternion keyframes exactly with
        // a cubic spline. The iteration step was terminated because qTol was zero.
        if (kb - ka == 2)
        {
          m_qspan = m_qspan_;
        }

        // Append the fitted quaternion spline span
        m_qsp.append(m_qspan.getKnot(1), m_qspan.getVelA(0), m_qspan.getVelB(0), m_qspan.getKey(1));

        // Recompute the resampled quat values
        m_resampledQuats[qspKb] = m_quats[qspKb];
        for (uint32_t i = ka + 1; i < qspKb; ++i)
        {
          m_resampledQuats[i] = m_qspan.evaluate(m_ts[i]);
        }

        // Update the start index
        ka = qspKb;
        break;
      }

      // Store the valid single span quaternion spline (resampled quats are valid)
      m_qspan = m_qspan_;
      qspKb = kb;

      // Increment the sample index
      kb++;

      // Check for termination on the number of data samples
      if (kb >= m_numSamples)
      {
        m_qsp.append(m_qspan.getKnot(1), m_qspan.getVelA(0), m_qspan.getVelB(0), m_qspan.getKey(1));
        return true; // resampled quats are valid
      }

    } // End of optimisation fitting

  } // End of main iteration loop

  //------------------------------
  // You get here if the last fitting operation failed one of the tolerance tests but
  // was a span containing 3 quaternion keyframes. Resampled quat values are valid.
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool QuatSplineFitterAngleTol::func(
  uint32_t NMP_UNUSED(ka),
  uint32_t NMP_UNUSED(kb))
{
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif
