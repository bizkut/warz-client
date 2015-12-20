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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef RIGID_MOTION_TM_JAC_H
#define RIGID_MOTION_TM_JAC_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMMatrix.h"
#include "NMNumerics/NMQuatSpline.h"
#include "NMNumerics/NMQuatSplineFitterBase.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Jacobian class to compute the derivatives of the rigid motion
/// transform y = R*x + t for various representations of a pos and quat.
///
/// Pos:  y = R*x + t
///       dy/dt = I
///
/// PosSpline: y = R*x + p(u)
///       where p(u : p0, p1, p2, p3) is the Bezier curve representation of the pos
///       dy/d[p0, p1, p2, p3] = dy/dp(u) * dp(u)/d[p0, p1, p2, p3]
///
/// Quat: y = Q*x + t
///       dy/dv = dy/dq * dq/dv
///       where v is the tan quarter angle rotation vector representation of the quat
///
/// QuatSpline: y = q(u)*x + t
///       where q(u : qa, wa, wb, qb) is the Hermite form of the quaternion spline
///       dy/d[qa, wa, wb, qb] = dy/dq(u) * dq(u)/d[qa, wa, wb, qb]
///       the quat spline keys [qa, qb] are also encoded as tan quarter angle rotation
///       vectors.
class RigidMotionTMJacobian
{
public:
  //-----------------------
  // Enumeration for the TM pos part method
  enum tmPosMethodType
  {
    PosTM,
    PosSplineTM
  };

  // Enumeration for the TM quat part method
  enum tmQuatMethodType
  {
    QuatTM,
    QuatSplineTM
  };

public:
  //---------------------------------------------------------------------
  /// \name   Constructors
  /// \brief  Functions to initialise the Jacobian class
  //---------------------------------------------------------------------
  //@{
  RigidMotionTMJacobian();
  ~RigidMotionTMJacobian();
  //@}

  //---------------------------------------------------------------------
  /// \name   Transform component functions
  /// \brief  Functions to set the pos and quat parts of the transform
  //---------------------------------------------------------------------
  //@{

  /// \brief Function to get the current method for the pos part of the TM
  tmPosMethodType getPosMethod() const;

  /// \brief Function to retrieve the pos part of the TM
  const Vector3& getTransformPos() const;

  /// \brief Function to set the pos part of the TM.
  void setPos(const Vector3& pos);

  /// \brief Function to set the pos part of the TM as a relative
  /// transform from an offset mean: t = t_mean + t_rel
  void setPos(
    const Vector3& posMean,
    const Vector3& posRel);

  /// \brief Function to initialise the pos part of the TM from the
  /// parameters of a single span pos spline. You must evaluate the pos
  /// spline to set the pos part of the TM.
  ///
  /// \param ta - The knot value at the start of the span
  /// \param tb - The knot value at the end of the span
  /// \param keyA - The pos spline key at the start of the span
  /// \param tangentA - The pos spline tangent key for the start of the span
  /// \param tangentB - The pos spline tangent key for the end of the span
  /// \param keyB - The pos spline key at the end of the span
  void setPosSpline(
    float          ta,
    float          tb,
    const Vector3& keyA,
    const Vector3& tangentA,
    const Vector3& tangentB,
    const Vector3& keyB);

  /// \brief Function to initialise the pos part of the TM from the
  /// parameters of a single span pos spline, with the parameters set
  /// from their offset means. You must evaluate the pos spline to set
  /// the pos part of the TM.
  ///
  /// \param ta - The knot value at the start of the span
  /// \param tb - The knot value at the end of the span
  /// \param posMean - The offset mean pos key
  /// \param keyARel - The relative pos spline key at the start of the span
  /// \param tangentARel - The relative pos spline tangent key for the start of the span
  /// \param tangentBRel - The relative pos spline tangent key for the end of the span
  /// \param keyBRel - The relative pos spline key at the end of the span
  void setPosSpline(
    float          ta,
    float          tb,
    const Vector3& posMean,
    const Vector3& keyARel,
    const Vector3& tangentARel,
    const Vector3& tangentBRel,
    const Vector3& keyBRel);

  /// \brief Function to evaluate the pos spline
  void evaluatePosSpline(float t);

  //-----------------------
  /// \brief Function to get the current method for the quat part of the TM
  tmQuatMethodType getQuatMethod() const;

  /// \brief Function to retrieve the quat part of the TM
  const Quat& getTransformQuat() const;

  /// \brief Function to set the quat part of the TM.
  /// \param quat - A tan quarter angle rotation vector representing the quat
  void setQuat(const Vector3& quat);

  /// \brief Function to set the quat part of the TM as a relative
  /// transform from an offset mean: q = q_mean * q_rel
  ///
  /// \param quatMean - The offset mean quat
  /// \param quatRel - A tan quarter angle rotation vector representing the
  ///                  relative rotation from the offset mean
  void setQuat(
    const Quat&    quatMean,
    const Vector3& quatRel);

  /// \brief Function to initialise the quat part of the TM from the
  /// parameters of a single span quaternion spline in the Hermite form.
  /// You must evaluate the quaternion spline to set the quat part of the TM.
  ///
  /// \param ta - The knot value at the start of the span
  /// \param tb - The knot value at the end of the span
  /// \param wNegKeyA - Since a tan quarter angle rotation vector encodes
  ///                   a quaternion in the positive hemisphere (w>=0) this
  ///                   is a flag indicating if the quat key has (w<0)
  /// \param wNegKeyB - Since a tan quarter angle rotation vector encodes
  ///                   a quaternion in the positive hemisphere (w>=0) this
  ///                   is a flag indicating if the quat key has (w<0)
  /// \param keyA - A tan quarter angle rotation vector representing the
  ///               the key at the start of the span
  /// \param tangentA - The quat spline tangent at the start of the span
  /// \param tangentB - The quat spline tangent at the end of the span
  /// \param keyB - A tan quarter angle rotation vector representing the
  ///               the key at the end of the span
  void setQuatSpline(
    float          ta,
    float          tb,
    bool           wNegKeyA,
    bool           wNegKeyB,
    const Vector3& keyA,
    const Vector3& tangentA,
    const Vector3& tangentB,
    const Vector3& keyB);

  /// \brief Function to initialise the quat part of the TM from the
  /// parameters of a single span quaternion spline in the Hermite form,
  /// with the parameters set from their offset means. You must evaluate
  /// the quaternion spline to set the quat part of the TM.
  ///
  /// \param ta - The knot value at the start of the span
  /// \param tb - The knot value at the end of the span
  /// \param keyMean - The offset mean quat key
  /// \param tangentMean - The offset mean tangent
  /// \param wNegKeyA - Since a tan quarter angle rotation vector encodes
  ///                   a quaternion in the positive hemisphere (w>=0) this
  ///                   is a flag indicating if the quat key has (w<0)
  /// \param wNegKeyB - Since a tan quarter angle rotation vector encodes
  ///                   a quaternion in the positive hemisphere (w>=0) this
  ///                   is a flag indicating if the quat key has (w<0)
  /// \param keyARel - A tan quarter angle rotation vector representing the
  ///               the relative key at the start of the span
  /// \param tangentARel - The relative quat spline tangent at the start of the span
  /// \param tangentBRel - The relative quat spline tangent at the end of the span
  /// \param keyBRel - A tan quarter angle rotation vector representing the
  ///               the relative key at the end of the span
  void setQuatSpline(
    float          ta,
    float          tb,
    const Quat&    keyMean,
    const Vector3& tangentMean,
    bool           wNegKeyA,
    bool           wNegKeyB,
    const Vector3& keyARel,
    const Vector3& tangentARel,
    const Vector3& tangentBRel,
    const Vector3& keyBRel);

  /// \brief Function to evaluate the quaternion spline
  void evaluateQuatSpline(float t);
  //@}

  //---------------------------------------------------------------------
  /// \name   Transformation functions
  /// \brief  Functions to transform a point by the rigid motion
  ///         transform y = R*x + t
  //---------------------------------------------------------------------
  //@{

  /// \brief Function computes the Jacobian for the quat part of the
  /// rigid motion transform y = R*x + t
  void transformQuatJac(const Vector3& x);

  /// \brief Function computes the Jacobian for the quat spline part of
  /// the rigid motion transform y = R*x + t
  void transformQuatSplineJac(const Vector3& x);

  /// \brief Function computes the Jacobian for the pos part of the
  /// rigid motion transform y = R*x + t
  void transformPosJac(const Vector3& x);

  /// \brief Function computes the Jacobian for the pos spline part of
  /// the rigid motion transform y = R*x + t
  void transformPosSplineJac(const Vector3& x);

  /// \brief Function to transform the point x with the rigid motion tranform
  /// y = R*x + t using the specified pos and quat TM part methods
  void transformPoint(const Vector3& x);

  /// \brief Function to retrieve the transformed point y
  const Vector3& getTransformedPoint() const;
  //@}

  //---------------------------------------------------------------------
  /// \name   Error propagation functions
  /// \brief  Functions to compute the propagated error in the pos, quat
  ///         parameters through the rigid motion y = R*x + t.
  ///         If dy/dp = dR/dp is the Jacobian for the parameters that
  ///         represent the quat then: delta_y = dy/dp * delta_p is
  ///         the change in the transformed point y for the change in
  ///         the parameters of the rotation p.
  //---------------------------------------------------------------------
  //@{

  /// \brief Function to compute the error for a change in the components
  /// of the pos part of the TM
  Vector3 getErrorPos(uint32_t component, float delta);

  /// \brief Function to compute the error for a change in the components
  /// of the pos spline part of the TM
  Vector3 getErrorPosSplineKeyA(uint32_t component, float delta);
  Vector3 getErrorPosSplineTangentA(uint32_t component, float delta);
  Vector3 getErrorPosSplineTangentB(uint32_t component, float delta);
  Vector3 getErrorPosSplineKeyB(uint32_t component, float delta);

  /// \brief Function to compute the error for a change in the components
  /// of the quat part of the TM
  Vector3 getErrorQuat(uint32_t component, float delta);

  /// \brief Function to compute the error for a change in the components
  /// of the quat spline part of the TM
  Vector3 getErrorQuatSplineKeyA(uint32_t component, float delta);
  Vector3 getErrorQuatSplineTangentA(uint32_t component, float delta);
  Vector3 getErrorQuatSplineTangentB(uint32_t component, float delta);
  Vector3 getErrorQuatSplineKeyB(uint32_t component, float delta);
  //@}

protected:
  //-----------------------
  // Pos
  Vector3             m_posMean;
  Vector3             m_posRel;

  //-----------------------
  // Quat
  Quat                m_quatMean;
  Vector3             m_quatRel;          ///< Represents a quat (as a tan quarter angle rotation vector)

  //-----------------------
  // Pos spline
  Vector3             m_pspMean;
  Vector3             m_pspKeyARel;
  Vector3             m_pspTangentARel;
  Vector3             m_pspTangentBRel;
  Vector3             m_pspKeyBRel;

  //-----------------------
  // Quat spline
  Quat                m_qspKeyMean;
  Vector3             m_qspTangentMean;
  Vector3             m_qspKeyARel;
  Vector3             m_qspTangentARel;
  Vector3             m_qspTangentBRel;
  Vector3             m_qspKeyBRel;

  Quat                m_qa;
  Vector3             m_wa;
  Vector3             m_wb;
  Quat                m_qb;

  //-----------------------
  // Evaluated transform
  Vector3             m_pos;
  Quat                m_quat;
  Vector3             m_y;

  //-----------------------
  // Pos spline Derivatives
  float               m_dxt_by_dp[4]; // Bezier basis functions

  //-----------------------
  // Quat spline Derivatives
  Matrix<double>      m_mat4_3;
  Matrix<double>      m_mat4_4;
  Vector<double>      m_vec4;

  Matrix<double>      m_dq_by_dv;
  Matrix<double>      m_dy_by_dq;
  Matrix<double>      m_dy_by_dv;

  // Set quat spline
  QuatSplineJacobian  m_qspJ;
  Matrix<double>      m_dqa_by_dva;
  Matrix<double>      m_dqb_by_dvb;
  Matrix<double>      m_dw2_by_dva;
  Matrix<double>      m_dw2_by_dvb;

  // Evaluate quat spline
  Matrix<double>      m_dqt_by_dva;
  Matrix<double>      m_dqt_by_dua;
  Matrix<double>      m_dqt_by_dub;
  Matrix<double>      m_dqt_by_dvb;

  // Transform point quat spline
  Matrix<double>      m_dy_by_dva;
  Matrix<double>      m_dy_by_dua;
  Matrix<double>      m_dy_by_dub;
  Matrix<double>      m_dy_by_dvb;

  //-----------------------
  // Pos spline (moved here due to alignment padding issues)
  float               m_pspTa;
  float               m_pspTb;

  //-----------------------
  // Quat spline (moved here due to alignment padding issues)
  float               m_qspTa;
  float               m_qspTb;
  bool                m_qspWNegKeyA;
  bool                m_qspWNegKeyB;

  //-----------------------
  tmPosMethodType     m_posMethod;
  tmQuatMethodType    m_quatMethod;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // RIGID_MOTION_TM_JAC_H
