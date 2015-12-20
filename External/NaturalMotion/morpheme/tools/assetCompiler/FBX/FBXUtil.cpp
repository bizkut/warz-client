// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#include "FBXUtil.h"
#include <fbxsdk.h>

using namespace XMD;

//----------------------------------------------------------------------------------------------------------------------
XBone::RotationOrder FBX_XMD_PLUGIN::ConvertRotOrder(FBXSDK_NAMESPACE::EFbxRotationOrder rotationOrder)
{
  switch (rotationOrder)
  {
  case eEulerXYZ: return XBone::kXYZ;
  case eEulerXZY: return XBone::kXZY;
  case eEulerYXZ: return XBone::kYXZ;
  case eEulerYZX: return XBone::kYZX;
  case eEulerZXY: return XBone::kZXY;
  case eEulerZYX: return XBone::kZYX;
  default: return XBone::kXYZ;
  }
}           

//----------------------------------------------------------------------------------------------------------------------
XM2::XRotationOrder FBX_XMD_PLUGIN::ConvertRotOrderX(FBXSDK_NAMESPACE::EFbxRotationOrder rotationOrder)
{
  switch (rotationOrder)
  {
  case eEulerXYZ: return XM2::XROT_XYZ;
  case eEulerXZY: return XM2::XROT_XZY;
  case eEulerYXZ: return XM2::XROT_YXZ;
  case eEulerYZX: return XM2::XROT_YZX;
  case eEulerZXY: return XM2::XROT_ZXY;
  case eEulerZYX: return XM2::XROT_ZYX;
  default: return XM2::XROT_XYZ;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void FBX_XMD_PLUGIN::ConvertFBXMatrix(const FBXSDK_NAMESPACE::FbxMatrix& fbxMat, XM2::XMatrix& matrix)
{
  for (int elemIndex = 0; elemIndex < 16; ++elemIndex)
  {
    matrix.data[elemIndex] = (XReal)(fbxMat.Get(elemIndex / 4, elemIndex % 4));
  }
}