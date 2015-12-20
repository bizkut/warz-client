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
#ifndef MR_PHYSX_H
#define MR_PHYSX_H
//----------------------------------------------------------------------------------------------------------------------
#include "mrPhysX2Includes.h"

#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMMatrix34.h"
#include "NMPlatform/NMQuat.h"

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
static inline NxMat34 nmMatrix34ToNxMat34(const NMP::Matrix34& m)
{
  float mArray[16];
  NxMat34 actMtx;
  m.getColumnMajorStride4(mArray);
  actMtx.setRowMajor44((NxF32*)mArray);
  return actMtx;
}

//----------------------------------------------------------------------------------------------------------------------
static inline NMP::Matrix34 nmNxMat34ToNmMatrix34(const NxMat34& nxM)
{
  float mArray[16];
  NMP::Matrix34 m;
  nxM.getRowMajor44(mArray);
  m.setColumnMajorStride4(mArray);
  return m;
}

//--- -------------------------------------------------------------------------------------------------------------------
static inline NxMat33 nmMatrix34ToNxMat33(const NMP::Matrix34& m)
{
  float mArray[9];
  NxMat33 actMtx;

  // m.getColumnMajorStride4(mArray); on 3x3 data (temporarily)
  mArray[0] = m.r[0].x; mArray[3] = m.r[0].y; mArray[6] = m.r[0].z;
  mArray[1] = m.r[1].x; mArray[4] = m.r[1].y; mArray[7] = m.r[1].z;
  mArray[2] = m.r[2].x; mArray[5] = m.r[2].y; mArray[8] = m.r[2].z;

  actMtx.setRowMajor((NxF32*)mArray);
  return actMtx;
}

//--- -------------------------------------------------------------------------------------------------------------------
static inline NMP::Matrix34 nmNxMat33ToNmMatrix34(const NxMat33& nxM)
{
  float mArray[9];
  NMP::Matrix34 m;
  nxM.getRowMajor(mArray);

  //m.setColumnMajorStride4(mArray); on 3x3 data (temporarily)
  m.r[0].set(mArray[0], mArray[3], mArray[6]);
  m.r[1].set(mArray[1], mArray[4], mArray[7]);
  m.r[2].set(mArray[2], mArray[5], mArray[8]);
  m.r[3].setToZero();

  return m;
}

//----------------------------------------------------------------------------------------------------------------------
static inline NxVec3 nmVector3ToNxVec3(const NMP::Vector3& v)
{
  return NxVec3(v.x, v.y, v.z);
}

//----------------------------------------------------------------------------------------------------------------------
static inline NMP::Vector3 nmNxVec3ToVector3(const NxVec3& v)
{
  return NMP::Vector3(v.x, v.y, v.z);
}

//----------------------------------------------------------------------------------------------------------------------
static inline NxQuat nmQuatToNxQuat(const NMP::Quat& q)
{
  return NxQuat(NxVec3(q.x, q.y, q.z), q.w);
}

//----------------------------------------------------------------------------------------------------------------------
static inline NMP::Quat nmNxQuatToQuat(const NxQuat& q)
{
  return NMP::Quat(q.x, q.y, q.z, q.w);
}

}

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_PHYSX_H
//----------------------------------------------------------------------------------------------------------------------
