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
#ifndef MR_UTILS_H
#define MR_UTILS_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMHierarchy.h"
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMStringTable.h"
#include "morpheme/mrAttribData.h"

//----------------------------------------------------------------------------------------------------------------------
// Contains a set of functions that can be useful when updating morpheme networks.
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief Compute the world space transforms of the output of the given network, putting results in the given buffer.
/// \return The number of transforms accumulated in to the result.
///
/// Accumulation happens straight in to the output buffer.
extern uint32_t computeNetworkWorldTransforms(
  Network*            net,                    ///< Network to compute world transforms of.
  const NMP::Vector3& rootPos,                ///< Position to set the root of the hierarchy to.
  const NMP::Quat&    rootQuat,               ///< Attitude to set the root of the hierarchy to.
  NMP::DataBuffer*    outputBuffer);          ///< Where to put results.

//----------------------------------------------------------------------------------------------------------------------
/// \brief Compute the world space transforms of the output of the given network, putting results in the given buffer.
/// \return The number of transforms accumulated in to the result.
///
/// This method uses the network's temporary allocator for to allocate a temporary accumulation buffer.
extern uint32_t computeNetworkWorldTransforms(
  Network*            net,                    ///< Network to compute world transforms of.
  const NMP::Vector3& rootPos,                ///< Position to set the root of the hierarchy to.
  const NMP::Quat&    rootQuat,               ///< Attitude to set the root of the hierarchy to.
  uint32_t            numOutputBufferEntries, ///< Size of the output buffer.
  NMP::PosQuat*       outputBuffer);          ///< Where to put results.

//----------------------------------------------------------------------------------------------------------------------
/// \brief Compute the world space transforms of the output of the given network, putting results in the given buffer.
/// \return The number of transforms accumulated in to the result.
///
/// This method uses the network's temporary allocator for to allocate a temporary accumulation buffer.
extern uint32_t computeNetworkWorldTransforms(
  Network*            net,                    ///< Network to compute world transforms of.
  const NMP::Vector3& rootPos,                ///< Position to set the root of the hierarchy to.
  const NMP::Quat&    rootQuat,               ///< Attitude to set the root of the hierarchy to.
  uint32_t            numOutputBufferEntries, ///< Size of the output buffer.
  NMP::Matrix34*      outputBuffer);          ///< Where to put results.

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_UTILS_H
//----------------------------------------------------------------------------------------------------------------------
