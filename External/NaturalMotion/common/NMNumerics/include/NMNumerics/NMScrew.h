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
#ifndef NM_SCREW_H
#define NM_SCREW_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMNumerics/NMSVD.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Class for doing screw axis decomposition. i.e. find the set of points that
/// do not move under a rigid motion transform: R x + t = x
/// We solve (R - I) x = -t via singular value decomposition since R-I is singular.
template <typename _T>
class Screw : public SVD<_T>
{
public:
  // Constructors / Destructor
  Screw();
  Screw(const Matrix<_T>& R, const Vector<_T>& t);
  ~Screw();

  /// \brief  Compute the screw axis decomposition of the rigid
  /// motion transform [R | t]
  void compute(const Matrix<_T>& R, const Vector<_T>& t);

  /// \brief  Recompose the rigid motion transform [R | t] from the
  /// screw decomposition parameters
  void recompose(Matrix<_T>& R, Vector<_T>& t);

  /// \brief  Compute the closest point of approach x on the screw axis
  /// from a specified point u
  void closest(Vector<_T>& x, const Vector<_T>& u) const;
  void closest(_T* x, const _T* u) const;

  // Rotation axis direction
  const Vector<_T>& getAxisV() const;
  Vector<_T>& getAxisV();
  void getAxisV(_T* v) const;
  void getAxisV(Vector3& v) const;

  // Point on the screw axis
  const Vector<_T>& getAxisX() const;
  Vector<_T>& getAxisX();
  void getAxisX(_T* x) const;
  void getAxisX(Vector3& x) const;

  // Angle of rotation
  _T getTheta() const;

  // Parallel translation
  _T getLambda() const;

private:
  // Rotation axis direction
  Vector<_T>      m_v;

  // Closest point of approach x on the screw axis from the origin
  Vector<_T>      m_x;

  // Angle of rotation
  _T              m_theta;

  // Parallel translation factor along the screw axis
  _T              m_lambda;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_SCREW_H
