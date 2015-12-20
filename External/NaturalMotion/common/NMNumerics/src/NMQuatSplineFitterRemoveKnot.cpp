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
#include "NMNumerics/NMQuatSplineFitterRemoveKnot.h"
#include "NMNumerics/NMSimpleSplineFittingUtils.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// QuatSplineFitterRemoveKnot
//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterRemoveKnot::
QuatSplineFitterRemoveKnot(
  uint32_t          numSamples,
  const float*      ts,
  const Quat*       quats,
  const QuatSpline& qsp,
  uint32_t          indx)
//
  : QuatSplineFitterTangents(numSamples)
{
  // Set the data samples
  setTs(numSamples, ts);
  setQuatsRaw(numSamples, quats);

  // Set the quaternion spline
  setQuatSpline(qsp);

  // Remove the specified knot site
  removeKnotAtIndex(indx);
}

//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterRemoveKnot::
QuatSplineFitterRemoveKnot(uint32_t maxNumSamples) : QuatSplineFitterTangents(maxNumSamples)
{
}

//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterRemoveKnot::~QuatSplineFitterRemoveKnot()
{
}

//----------------------------------------------------------------------------------------------------------------------
bool QuatSplineFitterRemoveKnot::removeKnotAtIndex(uint32_t indx)
{
  uint32_t numKnots = m_qsp.getNumKnots();
  if (indx >= numKnots) return false;

  bool status;
  Quat     qa, qb;
  Vector3  wa, wb;
  float    ta, tb;
  uint32_t ia, ib;
  uint32_t numSpanSamples;
  const float* spanTs;
  const Quat* spanQuats;

  //------------------------------
  // Remove the knot from the quaternion spline by preserving the
  // continuity at the start and end of the affected span
  m_qsp.removeKnotAtIndex(indx);

  // Check if we are removed the start or end knot
  if (indx == 0 || indx == numKnots - 1) return true;

  //------------------------------
  // Get the knot range that is affected by removing the interior knot
  indx--; // index of knot at start of affected span
  ta = m_qsp.getKnot(indx);
  tb = m_qsp.getKnot(indx + 1);
  qa = m_qsp.getKey(indx);
  qb = m_qsp.getKey(indx + 1);
  wa = m_qsp.getVelA(indx);
  wb = m_qsp.getVelB(indx);

  //------------------------------
  // We are removing an interior knot so we must fit the affected curve span to
  // the data samples
  status = SimpleSplineFittingUtils::findSamplesExclusive(ta, tb, ia, ib, m_numSamples, m_ts);
  if (!status) return true; // No samples to fit

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
  }

  // Set the optimised tangent vectors
  m_qsp.setVelA(indx, wa);
  m_qsp.setVelB(indx, wb);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool QuatSplineFitterRemoveKnot::removeKnot(float t)
{
  const float tol = 1e-4f;
  uint32_t indx = m_qsp.closestKnot(t);

  // Check if t lies close to a knot site
  if (fabs(m_qsp.getKnot(indx) - t) > tol)
  {
    return false;
  }

  // Remove the knot
  return removeKnotAtIndex(indx);
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
