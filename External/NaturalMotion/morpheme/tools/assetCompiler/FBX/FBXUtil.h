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
#ifndef FBXUTIL_H
#define FBXUTIL_H

#include "XMD/Bone.h"
#include "XM2/EulerAngles.h"

#pragma warning(push)
#pragma warning(disable : 4100)
#include <fbxsdk.h>
#pragma warning(pop)

//----------------------------------------------------------------------------------------------------------------------
/// \brief Utility functions for FBX-XMD conversions
//----------------------------------------------------------------------------------------------------------------------
namespace FBX_XMD_PLUGIN
{
//----------------------------------------------------------------------------------------------------------------------
/// \brief Converts a FBX ERotationOrder to a XMD XBone::RotationOrder
XMD::XBone::RotationOrder ConvertRotOrder(FBXSDK_NAMESPACE::EFbxRotationOrder rotationOrder);

//----------------------------------------------------------------------------------------------------------------------
/// \brief Converts a FBX ERotationOrder to a XMD XRotationOrder
XM2::XRotationOrder ConvertRotOrderX(FBXSDK_NAMESPACE::EFbxRotationOrder rotationOrder);

//----------------------------------------------------------------------------------------------------------------------
/// \brief Converts a FBX Matrix to a XMD Matrix
void ConvertFBXMatrix(const FBXSDK_NAMESPACE::FbxMatrix& fbxMat, XM2::XMatrix& matrix);

//----------------------------------------------------------------------------------------------------------------------
/// \brief Converts a FBX Vector to a XMD vector
template <typename source, typename dest, typename destLength>
void ConvertVector(const source& sourceData, dest& destData);

//----------------------------------------------------------------------------------------------------------------------
template <typename source, typename dest, int destLength>
void ConvertVector(const source& sourceData, dest& destData)
{
  for (int dataIndex = 0; dataIndex < destLength; ++dataIndex)
  {
    destData.data[dataIndex] = (XReal)sourceData.mData[dataIndex];
  }
}

} // namespace FBX_XMD_PLUGIN

#endif//FBXUTIL_H