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
#include "NMNumerics/NMQuatSplineFitterBase.h"
#include "NMNumerics/NMSimpleSplineFittingUtils.h"
#include "NMNumerics/NMQuatUtils.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// QuatSplineJacobian
//----------------------------------------------------------------------------------------------------------------------
QuatSplineJacobian::QuatSplineJacobian() :
  m_mat4_4(4, 4),
  m_mat4_3(4, 3),
  m_matA3_4(3, 4),
  m_matB3_4(3, 4),
  m_dea_by_dwa(4, 3),
  m_deb_by_dwb(4, 3),
  m_dw2_by_dq12(3, 4),
  m_dw2_by_dqa(3, 4),
  m_dw2_by_dwa(3, 3),
  m_dw2_by_dwb(3, 3),
  m_dw2_by_dqb(3, 4),
  m_de1_by_dw1(4, 3),
  m_de2_by_dw2(4, 3),
  m_de3_by_dw3(4, 3),
  m_dqt_by_dq0(4, 4),
  m_dqt_by_dw1(4, 3),
  m_dqt_by_dw2(4, 3),
  m_dqt_by_dw3(4, 3)
{

}

//----------------------------------------------------------------------------------------------------------------------
QuatSplineJacobian::~QuatSplineJacobian()
{

}

//----------------------------------------------------------------------------------------------------------------------
void QuatSplineJacobian::w2Jac(const Quat& qa, const Vector3& wa, const Vector3& wb, const Quat& qb)
{
  // w2 = qlog(q1^{ -1 } * q2)
  Quat ea, eb, iq1, ieb, q2, q12;
  static const double diag4[4] = { -1, -1, -1, 1 };

  // Exponential maps for wa and wb
  quatExpJac(wa, m_dea_by_dwa, ea);
  quatExpJac(wb, m_deb_by_dwb, eb);

  // Compute q1^{ -1 } = qinv(qqmul(qa, qexp(wa)))
  iq1 = qa * ea;
  iq1.conjugate();

  // Compute q2 = qqmul(qb, qinv(qexp(wb)))
  ieb = eb;
  ieb.conjugate();
  q2 = qb * ieb;

  // Compute q12
  q12 = iq1 * q2;

  // Compute w2 = qlog(q12)
  quatLogJac(q12, m_dw2_by_dq12, m_w2);

  // Setup for building the Jacobian
  quatMultMatrixR(q2, m_mat4_4);
  m_mat4_4.postMultiplyDiag(diag4);
  m_matA3_4.setFromMultiplication(m_dw2_by_dq12, m_mat4_4);
  quatMultMatrixL(iq1, m_mat4_4);
  m_matB3_4.setFromMultiplication(m_dw2_by_dq12, m_mat4_4);

  // Compute the Jacobian: Dw2 / Dqa
  quatMultMatrixR(ea, m_mat4_4);
  m_dw2_by_dqa.setFromMultiplication(m_matA3_4, m_mat4_4);

  // Compute the Jacobian: Dw2 / Dwa
  quatMultMatrixL(qa, m_mat4_4);
  m_mat4_3.setFromMultiplication(m_mat4_4, m_dea_by_dwa);
  m_dw2_by_dwa.setFromMultiplication(m_matA3_4, m_mat4_3);

  // Compute the Jacobian: Dw2 / Dwb
  quatMultMatrixL(qb, m_mat4_4);
  m_mat4_4.postMultiplyDiag(diag4);
  m_mat4_3.setFromMultiplication(m_mat4_4, m_deb_by_dwb);
  m_dw2_by_dwb.setFromMultiplication(m_matB3_4, m_mat4_3);

  // Compute the Jacobian: Dw2 / Dqb
  quatMultMatrixR(ieb, m_mat4_4);
  m_dw2_by_dqb.setFromMultiplication(m_matB3_4, m_mat4_4);
}

//----------------------------------------------------------------------------------------------------------------------
void QuatSplineJacobian::valJac(
  const Quat&    q0,
  const Vector3& w1,
  const Vector3& w2,
  const Vector3& w3,
  float          t,
  float          ta,
  float          tb)
{
  NMP_ASSERT(ta < tb);

  float u, omu, u2, omu2, C1, C2, C3;
  Quat  e1, e2, e3, q01, q012, q23, q123;

  // Compute the u-parameter from the appropriate knot interval
  u = (t - ta) / (tb - ta);
  NMP_ASSERT(u >= 0.0f && u <= 1.0f);

  // Compute the cumulative basis functions
  omu = 1.0f - u;
  u2 = u * u;
  omu2 = omu * omu;
  C3 = u2 * u;
  C2 = 3.0f * u2 - 2.0f * C3;
  C1 = 1.0f - omu2 * omu;

  // Exponential maps
  quatExpJac(w1 * C1, m_de1_by_dw1, e1);
  m_de1_by_dw1 *= C1;
  quatExpJac(w2 * C2, m_de2_by_dw2, e2);
  m_de2_by_dw2 *= C2;
  quatExpJac(w3 * C3, m_de3_by_dw3, e3);
  m_de3_by_dw3 *= C3;

  // Compute the quaternion spline value
  // qt = qqmul(q0, qqmul(qexp(w1*C1), qqmul(qexp(w2*C2), qexp(w3*C3))))
  q01 = q0 * e1;
  q012 = q01 * e2;
  m_qt = q012 * e3;

  // Interim quaternions
  q23 = e2 * e3;
  q123 = e1 * q23;

  // Compute the Jacobian: Dq(t) / Dq0
  quatMultMatrixR(q123, m_dqt_by_dq0);

  // Compute the Jacobian: Dq(t) / Dw1
  quatMultMatrixR(q23, m_mat4_4);
  m_mat4_3.setFromMultiplication(m_mat4_4, m_de1_by_dw1);
  quatMultMatrixL(q0, m_mat4_4);
  m_dqt_by_dw1.setFromMultiplication(m_mat4_4, m_mat4_3);

  // Compute the Jacobian: Dq(t) / Dw2
  quatMultMatrixR(e3, m_mat4_4);
  m_mat4_3.setFromMultiplication(m_mat4_4, m_de2_by_dw2);
  quatMultMatrixL(q01, m_mat4_4);
  m_dqt_by_dw2.setFromMultiplication(m_mat4_4, m_mat4_3);

  // Compute the Jacobian: Dq(t) / Dw3
  quatMultMatrixL(q012, m_mat4_4);
  m_dqt_by_dw3.setFromMultiplication(m_mat4_4, m_de3_by_dw3);
}

//----------------------------------------------------------------------------------------------------------------------
// QuatSplineFitterBaseBase
//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterBase::QuatSplineFitterBase(uint32_t maxNumSamples) :
  m_maxNumSamples(maxNumSamples),
  m_numSamples(0),
  m_qsp(((maxNumSamples + 1) >> 1) + 1), // extra one for possible last span containing two samples
  m_conditionFlag(true)
{
  // Allocate the memory for the data samples
  m_ts = new float[m_maxNumSamples];
  m_quats = new Quat[m_maxNumSamples];
}

//----------------------------------------------------------------------------------------------------------------------
QuatSplineFitterBase::~QuatSplineFitterBase()
{
  delete[] m_ts;
  delete[] m_quats;
}

//----------------------------------------------------------------------------------------------------------------------
bool QuatSplineFitterBase::fit(
  uint32_t NMP_UNUSED(numSamples),
  const float* NMP_UNUSED(ts),
  const Quat* NMP_UNUSED(quats))
{
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool QuatSplineFitterBase::fitRegular(uint32_t numSamples, const Quat* quats)
{
  NMP_ASSERT(numSamples <= m_maxNumSamples);

  // Set the time samples
  for (uint32_t i = 0; i < numSamples; ++i)
  {
    m_ts[i] = (float)i;
  }

  // Perform the fitting
  return fit(numSamples, m_ts, quats);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t QuatSplineFitterBase::getNumSamples() const
{
  return m_numSamples;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t QuatSplineFitterBase::getMaxNumSamples() const
{
  return m_maxNumSamples;
}

//----------------------------------------------------------------------------------------------------------------------
float* QuatSplineFitterBase::getTs()
{
  return m_ts;
}

//----------------------------------------------------------------------------------------------------------------------
void QuatSplineFitterBase::setTs(uint32_t numSamples, const float* ts)
{
  NMP_ASSERT(ts);
  NMP_ASSERT(numSamples <= m_maxNumSamples);

  if (ts != m_ts)
  {
    m_numSamples = numSamples;
    m_ts[0] = ts[0];
    for (uint32_t i = 1; i < m_numSamples; ++i)
    {
      m_ts[i] = ts[i];
      NMP_ASSERT(m_ts[i] > m_ts[i-1]);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
const float* QuatSplineFitterBase::getTs() const
{
  return m_ts;
}

//----------------------------------------------------------------------------------------------------------------------
Quat* QuatSplineFitterBase::getQuats()
{
  return m_quats;
}

//----------------------------------------------------------------------------------------------------------------------
const Quat* QuatSplineFitterBase::getQuats() const
{
  return m_quats;
}

//----------------------------------------------------------------------------------------------------------------------
void QuatSplineFitterBase::setQuatsRaw(uint32_t numSamples, const Quat* quats)
{
  NMP_ASSERT(quats);
  NMP_ASSERT(numSamples <= m_maxNumSamples);

  if (quats != m_quats)
  {
    m_numSamples = numSamples;
    for (uint32_t i = 0; i < m_numSamples; ++i)
    {
      m_quats[i] = quats[i];
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void QuatSplineFitterBase::setQuatsConditioned(uint32_t numSamples, const Quat* quats)
{
  NMP_ASSERT(quats);
  NMP_ASSERT(numSamples <= m_maxNumSamples);

  if (m_conditionFlag)
  {
    if (quats != m_quats)
    {
      // Condition the quaternion data so that consecutive quaternions lie in the same hemisphere
      m_numSamples = numSamples;
      Quat q = quats[0];
      q.normalise();
      if (q.w < 0.0f) q = -q;
      m_quats[0] = q;
      for (uint32_t i = 1; i < m_numSamples; ++i)
      {
        q = quats[i];
        q.normalise();
        if (q.dot(m_quats[i-1]) < 0.0f) q = -q;
        m_quats[i] = q;
      }
    }
  }
  else
  {
    setQuatsRaw(numSamples, quats);
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool QuatSplineFitterBase::getEnableConditioning() const
{
  return m_conditionFlag;
}

//----------------------------------------------------------------------------------------------------------------------
void QuatSplineFitterBase::setEnableConditioning(bool enable)
{
  m_conditionFlag = enable;
}

//----------------------------------------------------------------------------------------------------------------------
const QuatSpline& QuatSplineFitterBase::getQuatSpline() const
{
  return m_qsp;
}

//----------------------------------------------------------------------------------------------------------------------
void QuatSplineFitterBase::setQuatSpline(const QuatSpline& qsp)
{
  m_qsp = qsp;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t QuatSplineFitterBase::getNumKnots() const
{
  return m_qsp.getNumKnots();
}

//----------------------------------------------------------------------------------------------------------------------
void QuatSplineFitterBase::setNumKnots(uint32_t numKnots)
{
  m_qsp.setNumKnots(numKnots);
}

//----------------------------------------------------------------------------------------------------------------------
const float* QuatSplineFitterBase::getKnots() const
{
  return m_qsp.getKnots();
}

//----------------------------------------------------------------------------------------------------------------------
void QuatSplineFitterBase::setKnots(uint32_t numKnots, const float* knots)
{
  NMP_ASSERT(SimpleSplineFittingUtils::isKnotVectorValid(numKnots, knots));
  m_qsp.setKnots(numKnots, knots);
}

//----------------------------------------------------------------------------------------------------------------------
void QuatSplineFitterBase::setKnots(uint32_t numKnots, const uint32_t* knots)
{
  NMP_ASSERT(SimpleSplineFittingUtils::isKnotVectorValid(numKnots, knots));
  m_qsp.setKnots(numKnots, knots);
}

//----------------------------------------------------------------------------------------------------------------------
void QuatSplineFitterBase::setKnots(const SimpleKnotVector& knots)
{
  NMP_ASSERT(SimpleSplineFittingUtils::isKnotVectorValid(knots.getNumKnots(), knots.getKnots()));
  m_qsp.setKnots(knots.getNumKnots(), knots.getKnots());
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
