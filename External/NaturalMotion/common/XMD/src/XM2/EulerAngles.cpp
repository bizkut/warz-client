//----------------------------------------------------------------------------------------------------------------------
///  \file    EulerAngles.cpp
/// \note    (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Quaternion.h"
#include "XM2/Matrix34.h"
#include "XM2/EulerAngles.h"
#include <math.h>
//----------------------------------------------------------------------------------------------------------------------
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XQuaternion XEulerAngles::AsQuaternion() const
{
  XQuaternion rotx,roty,rotz,rot1,rot2;
  rotx.fromAxisAngleR(XVector3(1.0f,0.0f,0.0f),x);
  roty.fromAxisAngleR(XVector3(0.0f,1.0f,0.0f),y);
  rotz.fromAxisAngleR(XVector3(0.0f,0.0f,1.0f),z);


  // build final rotation Dependant on rotation order
  switch(m_RotationOrder)
  {
  case XROT_XYZ:
    rot1.mul(rotx,roty);
    rot2.mul(rot1,rotz); 
    break;
  case XROT_XZY:
    rot1.mul(rotx,rotz);
    rot2.mul(rot1,roty);
    break;
  case XROT_YZX:
    rot1.mul(roty,rotz);
    rot2.mul(rot1,rotx);
    break;
  case XROT_YXZ:
    rot1.mul(roty,rotx);
    rot2.mul(rot1,rotz);
    break;
  case XROT_ZXY:
    rot1.mul(rotz,rotx);
    rot2.mul(rot1,roty);
    break;
  case XROT_ZYX:
    rot1.mul(rotz,roty);
    rot2.mul(rot1,rotx);
    break;

  case XROT_XYX:
    rot1.mul(rotx,roty);
    rot2.mul(rot1,rotx);
    break;
  case XROT_YZY:
    rot1.mul(roty,rotz);
    rot2.mul(rot1,roty);
    break;
  case XROT_ZXZ:
    rot1.mul(rotz,rotx);
    rot2.mul(rot1,rotz);
    break;
  default:
    XM2_ASSERT(0 && "[XEulerAngles::AsQuaternion] Invalid rotation order!");
    break;
  }
  return rot2;
}
//----------------------------------------------------------------------------------------------------------------------
XMatrix XEulerAngles::AsMatrix() const
{
  XMatrix rotx,roty,rotz,m1,m2;

  // perform rotations on individual matrices
  rotx.rotXR(m_xRotation);
  roty.rotYR(m_yRotation);
  rotz.rotZR(m_zRotation);

  // build final rotation Dependant on rotation order
  switch(m_RotationOrder)
  {
  case XROT_XYZ:
    m1.quickMult(rotx,roty);
    m2.quickMult(m1,rotz);
    break;

  case XROT_XZY:
    m1.quickMult(rotx,rotz);
    m2.quickMult(m1,roty);
    break;

  case XROT_YZX:
    m1.quickMult(roty,rotz);
    m2.quickMult(m1,rotx);
    break;

  case XROT_YXZ:
    m1.quickMult(roty,rotx);
    m2.quickMult(m1,rotz);
    break;

  case XROT_ZXY:
    m1.quickMult(rotz,rotx);
    m2.quickMult(m1,roty);
    break;

  case XROT_ZYX:
    m1.quickMult(rotz,roty);
    m2.quickMult(m1,rotx);
    break;

  case XROT_XYX:
    m1.quickMult(rotx,roty);
    m2.quickMult(m1,rotx);
    break;

  case XROT_YZY:
    m1.quickMult(roty,rotz);
    m2.quickMult(m1,roty);
    break;

  case XROT_ZXZ:
    m1.quickMult(rotz,rotx);
    m2.quickMult(m1,rotz);
    break;

  default:
    XM2_ASSERT(0 && "[XEulerAngles::AsMatrix] Invalid rotation order!");
    break;
  }
  return m2;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XEulerAngles::fromMatrix(const XMatrix& q,const XRotationOrder order)
{
  (void)q;
  m_RotationOrder=order;

  // build final rotation Dependant on rotation order
  switch(m_RotationOrder)
  {
  case XROT_XYZ:
    break;

  case XROT_XZY:
    break;

  case XROT_YZX:
    break;

  case XROT_YXZ:
    break;

  case XROT_ZXY:
    break;

  case XROT_ZYX:
    break;

  default:
    XM2_ASSERT(0 && "[XEulerAngles::AsMatrix] Invalid rotation order!");
    break;
  }

}
}
