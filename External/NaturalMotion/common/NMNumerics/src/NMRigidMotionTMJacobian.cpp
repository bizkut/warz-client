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
#include "NMNumerics/NMRigidMotionTMJacobian.h"
#include "NMNumerics/NMQuatUtils.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
RigidMotionTMJacobian::RigidMotionTMJacobian() :
  m_mat4_3(4, 3),
  m_mat4_4(4, 4),
  m_vec4(4),
  m_dq_by_dv(4, 3),
  m_dy_by_dq(3, 4),
  m_dy_by_dv(3, 3),
  m_dqa_by_dva(4, 3),
  m_dqb_by_dvb(4, 3),
  m_dw2_by_dva(3, 3),
  m_dw2_by_dvb(3, 3),
  m_dqt_by_dva(4, 3),
  m_dqt_by_dua(4, 3),
  m_dqt_by_dub(4, 3),
  m_dqt_by_dvb(4, 3),
  m_dy_by_dva(3, 3),
  m_dy_by_dua(3, 3),
  m_dy_by_dub(3, 3),
  m_dy_by_dvb(3, 3)
{
  //-----------------------
  m_posMethod = PosTM;
  m_quatMethod = QuatTM;

  //-----------------------
  // Pos
  m_posMean.setToZero();
  m_posRel.setToZero();

  //-----------------------
  // Quat
  m_quatMean.identity();
  m_quatRel.setToZero();

  //-----------------------
  // Pos spline
  m_pspTa = 0.0f;
  m_pspTb = 0.0f;
  m_pspMean.setToZero();
  m_pspKeyARel.setToZero();
  m_pspTangentARel.setToZero();
  m_pspTangentBRel.setToZero();
  m_pspKeyBRel.setToZero();

  //-----------------------
  // Quat spline
  m_qspTa = 0.0f;
  m_qspTb = 0.0f;
  m_qspKeyMean.identity();
  m_qspTangentMean.setToZero();
  m_qspWNegKeyA = false;
  m_qspWNegKeyB = false;
  m_qspKeyARel.setToZero();
  m_qspTangentARel.setToZero();
  m_qspTangentBRel.setToZero();
  m_qspKeyBRel.setToZero();

  //-----------------------
  // Evaluated transform
  m_pos.setToZero();
  m_quat.identity();
  m_y.setToZero();
}

//----------------------------------------------------------------------------------------------------------------------
RigidMotionTMJacobian::~RigidMotionTMJacobian()
{
}

//----------------------------------------------------------------------------------------------------------------------
RigidMotionTMJacobian::tmPosMethodType
RigidMotionTMJacobian::getPosMethod() const
{
  return m_posMethod;
}

//----------------------------------------------------------------------------------------------------------------------
const Vector3& RigidMotionTMJacobian::getTransformPos() const
{
  return m_pos;
}

//----------------------------------------------------------------------------------------------------------------------
void RigidMotionTMJacobian::setPos(const Vector3& pos)
{
  m_posMethod = PosTM;
  m_posMean.setToZero();
  m_posRel = pos;
  m_pos = m_posRel;
}

//----------------------------------------------------------------------------------------------------------------------
void RigidMotionTMJacobian::setPos(
  const Vector3& posMean,
  const Vector3& posRel)
{
  m_posMethod = PosTM;
  m_posMean = posMean;
  m_posRel = posRel;
  m_pos = m_posMean + m_posRel;
}

//----------------------------------------------------------------------------------------------------------------------
void RigidMotionTMJacobian::setPosSpline(
  float          ta,
  float          tb,
  const Vector3& keyA,
  const Vector3& tangentA,
  const Vector3& tangentB,
  const Vector3& keyB)
{
  // Set the pos spline parameters
  m_posMethod = PosSplineTM;
  m_pspTa = ta;
  m_pspTb = tb;
  m_pspMean.setToZero();
  m_pspKeyARel = keyA;
  m_pspTangentARel = tangentA;
  m_pspTangentBRel = tangentB;
  m_pspKeyBRel = keyB;
}

//----------------------------------------------------------------------------------------------------------------------
void RigidMotionTMJacobian::setPosSpline(
  float          ta,
  float          tb,
  const Vector3& posMean,
  const Vector3& keyARel,
  const Vector3& tangentARel,
  const Vector3& tangentBRel,
  const Vector3& keyBRel)
{
  // Set the pos spline parameters
  m_posMethod = PosSplineTM;
  m_pspTa = ta;
  m_pspTb = tb;
  m_pspMean = posMean;
  m_pspKeyARel = keyARel;
  m_pspTangentARel = tangentARel;
  m_pspTangentBRel = tangentBRel;
  m_pspKeyBRel = keyBRel;
}

//----------------------------------------------------------------------------------------------------------------------
void RigidMotionTMJacobian::evaluatePosSpline(float t)
{
  NMP_ASSERT(m_posMethod == PosSplineTM);

  // Compute the Bezier basis functions
  NMP_ASSERT(m_pspTb > m_pspTa);
  float u = (t - m_pspTa) / (m_pspTb - m_pspTa);

  float omu = 1.0f - u;
  float omu2 = omu * omu;
  float u2 = u * u;

  m_dxt_by_dp[0] = omu2 * omu;
  m_dxt_by_dp[1] = 3.0f * u * omu2;
  m_dxt_by_dp[2] = 3.0f * u2 * omu;
  m_dxt_by_dp[3] = u2 * u;

  // Evaluate the curve
  m_pos = (m_pspKeyARel * m_dxt_by_dp[0]) +
          (m_pspTangentARel * m_dxt_by_dp[1]) +
          (m_pspTangentBRel * m_dxt_by_dp[2]) +
          (m_pspKeyBRel * m_dxt_by_dp[3]);

  m_pos += m_pspMean;
}

//----------------------------------------------------------------------------------------------------------------------
RigidMotionTMJacobian::tmQuatMethodType
RigidMotionTMJacobian::getQuatMethod() const
{
  return m_quatMethod;
}

//----------------------------------------------------------------------------------------------------------------------
const Quat& RigidMotionTMJacobian::getTransformQuat() const
{
  return m_quat;
}

//----------------------------------------------------------------------------------------------------------------------
void RigidMotionTMJacobian::setQuat(const Vector3& quat)
{
  // Set the quat parameters
  m_quatMethod = QuatTM;
  m_quatMean.identity();
  m_quatRel = quat;

  // Compute the quat conversion derivatives
  NMP::fromRotationVectorTQAJac(m_quat, m_dq_by_dv, m_quatRel);
}

//----------------------------------------------------------------------------------------------------------------------
void RigidMotionTMJacobian::setQuat(
  const Quat&    quatMean,
  const Vector3& quatRel)
{
  Quat qr;

  // Set the quat parameters
  m_quatMethod = QuatTM;
  m_quatMean = quatMean;
  m_quatRel = quatRel;

  // Compute the quat conversion derivatives
  NMP::fromRotationVectorTQAJac(qr, m_mat4_3, m_quatRel);

  // Apply the mean rotation
  NMP::quatMultMatrixL(m_quatMean, m_mat4_4);
  m_quat = m_quatMean * qr;

  // Derivative dq / dv
  m_dq_by_dv.setFromMultiplication(m_mat4_4, m_mat4_3);
}

//----------------------------------------------------------------------------------------------------------------------
void RigidMotionTMJacobian::setQuatSpline(
  float          ta,
  float          tb,
  bool           wNegKeyA,
  bool           wNegKeyB,
  const Vector3& keyA,
  const Vector3& tangentA,
  const Vector3& tangentB,
  const Vector3& keyB)
{
  // Set the quat spline parameters
  m_quatMethod = QuatSplineTM;
  m_qspTa = ta;
  m_qspTb = tb;
  m_qspKeyMean.identity();
  m_qspTangentMean.setToZero();
  m_qspWNegKeyA = wNegKeyA;
  m_qspWNegKeyB = wNegKeyB;
  m_qspKeyARel = keyA;
  m_qspTangentARel = tangentA;
  m_qspTangentBRel = tangentB;
  m_qspKeyBRel = keyB;

  //-----------------------
  // Convert the va key to a quat
  NMP::fromRotationVectorTQAJac(m_qa, m_dqa_by_dva, m_qspKeyARel);

  // Apply the w component sign transform to the quat
  if (m_qspWNegKeyA)
  {
    m_qa = -m_qa;
    m_dqa_by_dva *= -1.0f;
  }

  //-----------------------
  // Convert the vb key to a quat
  NMP::fromRotationVectorTQAJac(m_qb, m_dqb_by_dvb, m_qspKeyBRel);

  // Apply the w component sign transform to the quat
  if (m_qspWNegKeyB)
  {
    m_qb = -m_qb;
    m_dqb_by_dvb *= -1.0f;
  }

  //-----------------------
  // Convert the ua tangent
  m_wa = m_qspTangentARel;

  //-----------------------
  // Convert the ub tangent
  m_wb = m_qspTangentBRel;

  //-----------------------
  // Compute the Bezier control points for the curve span
  m_qspJ.w2Jac(m_qa, m_wa, m_wb, m_qb);
  m_dw2_by_dva.setFromMultiplication(m_qspJ.getDw2ByDqa(), m_dqa_by_dva);
  m_dw2_by_dvb.setFromMultiplication(m_qspJ.getDw2ByDqb(), m_dqb_by_dvb);
}

//----------------------------------------------------------------------------------------------------------------------
void RigidMotionTMJacobian::setQuatSpline(
  float          ta,
  float          tb,
  const Quat&    keyMean,
  const Vector3& tangentMean,
  bool           wNegKeyA,
  bool           wNegKeyB,
  const Vector3& keyARel,
  const Vector3& tangentARel,
  const Vector3& tangentBRel,
  const Vector3& keyBRel)
{
  Quat ra, rb;

  // Set the quat spline parameters
  m_quatMethod = QuatSplineTM;
  m_qspTa = ta;
  m_qspTb = tb;
  m_qspKeyMean = keyMean;
  m_qspTangentMean = tangentMean;
  m_qspWNegKeyA = wNegKeyA;
  m_qspWNegKeyB = wNegKeyB;
  m_qspKeyARel = keyARel;
  m_qspTangentARel = tangentARel;
  m_qspTangentBRel = tangentBRel;
  m_qspKeyBRel = keyBRel;

  // dqa / dra  i.e. [qm]_L
  NMP::quatMultMatrixL(m_qspKeyMean, m_mat4_4);

  //-----------------------
  // Convert the va key to a quat
  NMP::fromRotationVectorTQAJac(ra, m_mat4_3, m_qspKeyARel);

  // Apply the w component sign transform to the quat
  if (m_qspWNegKeyA)
  {
    ra = -ra;
    m_mat4_3 *= -1.0f;
  }

  // Apply the mean rotation
  m_qa = m_qspKeyMean * ra;
  m_dqa_by_dva.setFromMultiplication(m_mat4_4, m_mat4_3);

  //-----------------------
  // Convert the vb key to a quat
  NMP::fromRotationVectorTQAJac(rb, m_mat4_3, m_qspKeyBRel);

  // Apply the w component sign transform to the quat
  if (m_qspWNegKeyB)
  {
    rb = -rb;
    m_mat4_3 *= -1.0f;
  }

  // Apply the mean rotation
  m_qb = m_qspKeyMean * rb;
  m_dqb_by_dvb.setFromMultiplication(m_mat4_4, m_mat4_3);

  //-----------------------
  // Convert the ua tangent
  m_wa = m_qspTangentMean + m_qspTangentARel;

  //-----------------------
  // Convert the ub tangent
  m_wb = m_qspTangentMean + m_qspTangentBRel;

  //-----------------------
  // Compute the Bezier control points for the curve span
  m_qspJ.w2Jac(m_qa, m_wa, m_wb, m_qb);
  m_dw2_by_dva.setFromMultiplication(m_qspJ.getDw2ByDqa(), m_dqa_by_dva);
  m_dw2_by_dvb.setFromMultiplication(m_qspJ.getDw2ByDqb(), m_dqb_by_dvb);
}

//----------------------------------------------------------------------------------------------------------------------
void RigidMotionTMJacobian::evaluateQuatSpline(float t)
{
  NMP_ASSERT(m_quatMethod == QuatSplineTM);

  // Compute the quaternion spline Jacobian: D(q(t)) / D[q0, w1, w2, w3]
  m_qspJ.valJac(m_qa, m_wa, m_qspJ.getW2(), m_wb, t, m_qspTa, m_qspTb);
  m_quat = m_qspJ.getQt();

  // dq(t) / dva = dq(t)/dq0 * dqa/dva + dq(t)/dw2 * dw2/dva
  m_dqt_by_dva.setFromMultiplication(m_qspJ.getDqtByDq0(), m_dqa_by_dva);
  m_mat4_3.setFromMultiplication(m_qspJ.getDqtByDw2(), m_dw2_by_dva);
  m_dqt_by_dva += m_mat4_3;

  // dq(t) / dua = dq(t)/dw1 + dq(t)/dw2 * dw2/dwa
  m_dqt_by_dua.setFromMultiplication(m_qspJ.getDqtByDw2(), m_qspJ.getDw2ByDwa());
  m_dqt_by_dua += m_qspJ.getDqtByDw1();

  // dq(t) / dub = dq(t)/dw2 * dw2/dwb + dq(t)/dw3
  m_dqt_by_dub.setFromMultiplication(m_qspJ.getDqtByDw2(), m_qspJ.getDw2ByDwb());
  m_dqt_by_dub += m_qspJ.getDqtByDw3();

  // dq(t) / dvb = dq(t)/dw2 * dw2/dvb
  m_dqt_by_dvb.setFromMultiplication(m_qspJ.getDqtByDw2(), m_dw2_by_dvb);
}

//----------------------------------------------------------------------------------------------------------------------
void RigidMotionTMJacobian::transformQuatJac(const Vector3& x)
{
  // Compute the transform: y = Q x
  NMP::qvmulJac(m_quat, x, m_dy_by_dq, m_y);

  // Derivative dy / dv
  m_dy_by_dv.setFromMultiplication(m_dy_by_dq, m_dq_by_dv);
}

//----------------------------------------------------------------------------------------------------------------------
void RigidMotionTMJacobian::transformQuatSplineJac(const Vector3& x)
{
  // Compute the transform: y = Q x
  NMP::qvmulJac(m_quat, x, m_dy_by_dq, m_y);

  // dy / dva
  m_dy_by_dva.setFromMultiplication(m_dy_by_dq, m_dqt_by_dva);

  // dy / dua
  m_dy_by_dua.setFromMultiplication(m_dy_by_dq, m_dqt_by_dua);

  // dy / dub
  m_dy_by_dub.setFromMultiplication(m_dy_by_dq, m_dqt_by_dub);

  // dy / dvb
  m_dy_by_dvb.setFromMultiplication(m_dy_by_dq, m_dqt_by_dvb);
}

//----------------------------------------------------------------------------------------------------------------------
void RigidMotionTMJacobian::transformPosJac(const Vector3& NMP_UNUSED(x))
{
  // dy/dt = I
  m_y += m_pos; // Update the transform
}

//----------------------------------------------------------------------------------------------------------------------
void RigidMotionTMJacobian::transformPosSplineJac(const Vector3& NMP_UNUSED(x))
{
  // dy/d[p0,p1,p2,p3] = dy/dp(u) * dp(u)/d[p0,p1,p2,p3]
  m_y += m_pos; // Update the transform
}

//----------------------------------------------------------------------------------------------------------------------
void RigidMotionTMJacobian::transformPoint(const Vector3& x)
{
  // Quat part of the TM
  if (m_quatMethod == QuatTM)
    transformQuatJac(x);
  else
    transformQuatSplineJac(x);

  // Pos part of the TM
  if (m_posMethod == PosTM)
    transformPosJac(x);
  else
    transformPosSplineJac(x);
}

//----------------------------------------------------------------------------------------------------------------------
const Vector3& RigidMotionTMJacobian::getTransformedPoint() const
{
  return m_y;
}

//----------------------------------------------------------------------------------------------------------------------
Vector3 RigidMotionTMJacobian::getErrorPos(uint32_t component, float delta)
{
  Vector3 dy;
  dy.setToZero();
  dy.f[component] = delta;
  return dy;
}

//----------------------------------------------------------------------------------------------------------------------
Vector3 RigidMotionTMJacobian::getErrorPosSplineKeyA(uint32_t component, float delta)
{
  Vector3 dy;
  dy.setToZero();
  dy.f[component] = m_dxt_by_dp[0] * delta;
  return dy;
}

//----------------------------------------------------------------------------------------------------------------------
Vector3 RigidMotionTMJacobian::getErrorPosSplineTangentA(uint32_t component, float delta)
{
  Vector3 dy;
  dy.setToZero();
  dy.f[component] = m_dxt_by_dp[1] * delta;
  return dy;
}

//----------------------------------------------------------------------------------------------------------------------
Vector3 RigidMotionTMJacobian::getErrorPosSplineTangentB(uint32_t component, float delta)
{
  Vector3 dy;
  dy.setToZero();
  dy.f[component] = m_dxt_by_dp[2] * delta;
  return dy;
}

//----------------------------------------------------------------------------------------------------------------------
Vector3 RigidMotionTMJacobian::getErrorPosSplineKeyB(uint32_t component, float delta)
{
  Vector3 dy;
  dy.setToZero();
  dy.f[component] = m_dxt_by_dp[3] * delta;
  return dy;
}

//----------------------------------------------------------------------------------------------------------------------
Vector3 RigidMotionTMJacobian::getErrorQuat(uint32_t component, float delta)
{
  Vector3 dy;
  m_dy_by_dv.getColumnVector(component, m_vec4.data());
  dy.x = (float)m_vec4[0];
  dy.y = (float)m_vec4[1];
  dy.z = (float)m_vec4[2];
  dy *= delta;

  return dy;
}

//----------------------------------------------------------------------------------------------------------------------
Vector3 RigidMotionTMJacobian::getErrorQuatSplineKeyA(uint32_t component, float delta)
{
  Vector3 dy;
  m_dy_by_dva.getColumnVector(component, m_vec4.data());
  dy.x = (float)m_vec4[0];
  dy.y = (float)m_vec4[1];
  dy.z = (float)m_vec4[2];
  dy *= delta;

  return dy;
}

//----------------------------------------------------------------------------------------------------------------------
Vector3 RigidMotionTMJacobian::getErrorQuatSplineTangentA(uint32_t component, float delta)
{
  Vector3 dy;
  m_dy_by_dua.getColumnVector(component, m_vec4.data());
  dy.x = (float)m_vec4[0];
  dy.y = (float)m_vec4[1];
  dy.z = (float)m_vec4[2];
  dy *= delta;

  return dy;
}

//----------------------------------------------------------------------------------------------------------------------
Vector3 RigidMotionTMJacobian::getErrorQuatSplineTangentB(uint32_t component, float delta)
{
  Vector3 dy;
  m_dy_by_dub.getColumnVector(component, m_vec4.data());
  dy.x = (float)m_vec4[0];
  dy.y = (float)m_vec4[1];
  dy.z = (float)m_vec4[2];
  dy *= delta;

  return dy;
}

//----------------------------------------------------------------------------------------------------------------------
Vector3 RigidMotionTMJacobian::getErrorQuatSplineKeyB(uint32_t component, float delta)
{
  Vector3 dy;
  m_dy_by_dvb.getColumnVector(component, m_vec4.data());
  dy.x = (float)m_vec4[0];
  dy.y = (float)m_vec4[1];
  dy.z = (float)m_vec4[2];
  dy *= delta;

  return dy;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
