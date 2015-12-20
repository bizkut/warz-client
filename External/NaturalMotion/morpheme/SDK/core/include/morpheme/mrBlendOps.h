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
#ifndef MR_BLEND_OPS_H
#define MR_BLEND_OPS_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMBuffer.h"
#include "morpheme/mrRig.h"
#include "NMPlatform/NMvpu.h"
//----------------------------------------------------------------------------------------------------------------------

// To enable profiling output of blending operations.
#define NM_BLENDOPS_PROFILINGx
#if defined(NM_BLENDOPS_PROFILING)
  #define NM_BLENDOPS_BEGIN_PROFILING(name)    NM_BEGIN_PROFILING(name)
  #define NM_BLENDOPS_END_PROFILING()          NM_END_PROFILING()
#else // NM_BLENDOPS_PROFILING
  #define NM_BLENDOPS_BEGIN_PROFILING(name)
  #define NM_BLENDOPS_END_PROFILING()
#endif // NM_BLENDOPS_PROFILING

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \defgroup BufferBlendOperationsModule Buffer Blend Operations.
/// \ingroup CoreModule
///
/// Transform Buffer blend functions called by operations on the Network ops queue.
/// This is a shared resource for any calling operation to be able to use.
/// Many operations have more than one implementation for optimal operation on target platforms.
//----------------------------------------------------------------------------------------------------------------------

static const uint32_t BLEND_MODE_INTERP_ATT_INTERP_POS = 0; ///< Interpolate attitude and position dependent on weighting value.
static const uint32_t BLEND_MODE_INTERP_ATT_ADD_POS    = 1; ///< Interpolate attitude and add scaled position.
static const uint32_t BLEND_MODE_ADD_ATT_LEAVE_POS     = 2; ///< Add scaled attitude and do nothing with the position.
static const uint32_t BLEND_MODE_ADD_ATT_ADD_POS       = 3; ///< Add scaled attitude and position.
static const uint32_t BLEND_MODE_SUB_ATT_SUB_POS       = 4; ///< Subtract scaled attitude and position.

//----------------------------------------------------------------------------------------------------------------------
/// \class BlendOpsBase
/// \brief Transform Buffer blend functions called by operations on the Network ops queue.
/// \ingroup BufferBlendOperationsModule
///
/// This is a shared resource for any calling operation to be able to use.
/// Many operations have more than one implementation for optimal operation on target platforms.
//----------------------------------------------------------------------------------------------------------------------
class BlendOpsBase
{
public:

//**********************************************************************************************************************
// FULL BUFFER
//**********************************************************************************************************************

  /// \brief Interpolate 2 process buffers, whose channel sets can be partial sub-sets of each other.
  ///
  /// Any channels that exists only in one source is simply copied into the result with no scaling.
  /// Clears all existing entries in this buffer.
  static void interpQuatInterpPosPartial(
    NMP::DataBuffer*       destBuffer,
    const NMP::DataBuffer* sourceBuffer0,
    const NMP::DataBuffer* sourceBuffer1,
    const float            alpha                   ///< Global blend weight.
  );

  /// \brief For all channels in buffer1, add buffer1^alpha to buffer0
  ///
  /// All other channels from buffer 0 are propagated.  Channels existing only on buffer1 are ignored.
  /// Positions are not blended; they are passed directly from buffer0.
  static void addQuatLeavePosPartial(
    NMP::DataBuffer*       destBuffer,
    const NMP::DataBuffer* sourceBuffer0,
    const NMP::DataBuffer* sourceBuffer1,
    const float            alpha                   ///< Global blend weight.
  );

  /// \brief For all channels in buffer1, add buffer1^alpha to buffer0 and blend positions in the expected way.
  ///
  /// All other channels from buffer 0 are propagated.  Channels existing only on buffer1 are ignored.
  static void addQuatAddPosPartial(
    NMP::DataBuffer*       destBuffer,
    const NMP::DataBuffer* sourceBuffer0,
    const NMP::DataBuffer* sourceBuffer1,
    const float            alpha                   ///< Global blend weight.
  );

  /// \brief Interpolate the quaternions and add buffer1^alpha to buffer0 the positions of two buffers.
  static void interpQuatAddPosPartial(
    NMP::DataBuffer*       destBuffer,
    const NMP::DataBuffer* sourceBuffer0,
    const NMP::DataBuffer* sourceBuffer1,
    const float            alpha                   ///< Global blend weight.
  );

  /// \brief Subtract buffer1 from buffer0 in both the quaternions and positions
  static void subtractQuatSubtractPosPartial(
    NMP::DataBuffer*       destBuffer,
    const NMP::DataBuffer* sourceBuffer0,
    const NMP::DataBuffer* sourceBuffer1,
    const float            alpha                   ///< Global blend weight.
    );

  /// \brief Subtract buffer1 from buffer0 in the quaternions and interpolate the positions
  static void subtractQuatInterpPosPartial(
    NMP::DataBuffer*       destBuffer,
    const NMP::DataBuffer* sourceBuffer0,
    const NMP::DataBuffer* sourceBuffer1,
    const float            alpha                   ///< Global blend weight.
    );

  /// \brief Interpolate the quaternions and Subtract buffer1 from buffer0 in the positions
  static void interpQuatSubtractPosPartial(
    NMP::DataBuffer*       destBuffer,
    const NMP::DataBuffer* sourceBuffer0,
    const NMP::DataBuffer* sourceBuffer1,
    const float            alpha                   ///< Global blend weight.
    );

  /// \brief Interpolate 2 process buffers, whose channel sets can be partial sub-sets of each other,
  /// using per bone weighting as a scale.
  ///
  /// Any channels that exists only in one source is simply copied into the result with no scaling.
  /// Clears all existing entries in this buffer.
  static void interpQuatInterpPosPartialFeathered(
    NMP::DataBuffer*       destBuffer,
    const NMP::DataBuffer* sourceBuffer0,
    const NMP::DataBuffer* sourceBuffer1,
    const float            alpha,                  ///< Global blend weight.
    const uint32_t         numAlphaValues,
    const float*           alphaValues             ///< Per bone weights.
  );

  /// \brief For all attitude channels in buffer1, add buffer1^alpha to buffer0, using per bone weighting as a scale.
  ///
  /// All other channels from buffer 0 are propagated.  Channels existing only on buffer1 are ignored.
  /// Positions are not blended; They are passed directly from buffer0.
  static void addQuatLeavePosPartialFeathered(
    NMP::DataBuffer*       destBuffer,
    const NMP::DataBuffer* sourceBuffer0,
    const NMP::DataBuffer* sourceBuffer1,
    const float            alpha,                  ///< Global blend weight.
    const uint32_t         numAlphaValues,
    const float*           alphaValues             ///< Per bone blend weights.
  );

  /// \brief For all channels in buffer1, add buffer1^alpha to buffer0 and blend positions in the expected way,
  /// using per bone weighting as a scale.
  ///
  /// All other channels from buffer 0 are propagated.  Channels existing only on buffer1 are ignored.
  static void addQuatAddPosPartialFeathered(
    NMP::DataBuffer*       destBuffer,
    const NMP::DataBuffer* sourceBuffer0,
    const NMP::DataBuffer* sourceBuffer1,
    const float            alpha,                  ///< Global blend weight.
    const uint32_t         numAlphaValues,
    const float*           alphaValues             ///< Per bone blend weights.
  );

  /// \brief Interpolate the quaternions and add buffer1^alpha to buffer0 the positions of two buffers,
  /// using per bone weighting as a scale.
  static void interpQuatAddPosPartialFeathered(
    NMP::DataBuffer*       destBuffer,
    const NMP::DataBuffer* sourceBuffer0,
    const NMP::DataBuffer* sourceBuffer1,
    const float            alpha,                  ///< Global blend weight.
    const uint32_t         numAlphaValues,
    const float*           alphaValues             ///< Per bone blend weights.
  );

  /// \brief Apply the bind pose to any unused channels in the transforms buffer
  static void applyBindPoseToUnusedChannels(
    const AttribDataTransformBuffer* bindPose,
    NMP::DataBuffer*                 transformBuffer);

  /// \brief Copy all valid channels from sourceBuffer0 and sourceBuffer1 to the output buffer
  /// with priority to valid channels in sourceBuffer0.
  static void filterUsedChannels(
    NMP::DataBuffer*       destBuffer,
    const NMP::DataBuffer* sourceBuffer0,
    const NMP::DataBuffer* sourceBuffer1);

  /// \brief accumulate a full set of rig transforms against the given rig hierarchy.
  static void accumulateTransforms(
    const NMP::Vector3&    rootPos,               ///< Position to set the root of the hierarchy to.
    const NMP::Quat&       rootQuat,              ///< Attitude to set the root of the hierarchy to.
    const NMP::DataBuffer* sourceBuffer,
    const AnimRigDef*      rig,
    NMP::DataBuffer*       outBuffer);
    
  /// \brief Accumulates a partial set of rig transforms against the given rig hierarchy.
  static void accumulateTransformsPartial(
    const NMP::Vector3&    rootPos,               ///< Position to set the root of the hierarchy to.
    const NMP::Quat&       rootQuat,              ///< Attitude to set the root of the hierarchy to.
    const NMP::DataBuffer* sourceBuffer,          ///< The source buffer to accumulate from
    const AnimRigDef*      rig,                   ///< The animation rig and hierarchy
    uint32_t               numEntries,            ///< The number of rig bones to accumulate
    const uint16_t*        rigChannels,           ///< Array f rig bone indices to accumulate
    bool*                  validRigChannelFlags,  ///< Output array of valid accumulated channels
    NMP::Vector3*          outChannelPos,         ///< The output accumulated position channel
    NMP::Quat*             outChannelAtt          ///< The output accumulated orientation channel
  );

  /// \brief Accumulate all transforms from the starting bone index to the root transform.
  static void accumulateTransform(
    int32_t                boneIndex,             ///< Bone index within the animation rig.
    const NMP::Vector3&    rootPos,               ///< Position to set the root of the hierarchy to.
    const NMP::Quat&       rootQuat,              ///< Attitude to set the root of the hierarchy to.
    const NMP::DataBuffer* sourceBuffer,
    const AnimRigDef*      rig,
    NMP::Vector3&          bonePosOut,
    NMP::Quat&             boneQuatOut);

  /// \brief Recursively accumulate all transforms from the starting bone index to the root transform.
  /// A non Quat version
  static void accumulateTransformTM(
    int32_t                boneIndex,
    const NMP::Matrix34&   rootTM,
    const NMP::DataBuffer* sourceBuffer,
    const AnimRigDef*      rig,
    NMP::Matrix34&         boneTMOut);

  //**********************************************************************************************************************
  // SINGLE ENTRY
  //**********************************************************************************************************************

  /// \brief Interpolate a channel from 2 buffers.
  ///
  /// Leaves all other existing entries in this buffer alone.
  /// Copes with the case of one channel not being present in one of the buffers.
  NM_INLINE static void interpQuatInterpPosChannel(
    NMP::DataBuffer*        destBuffer,
    const uint32_t          channelID,
    const NMP::DataBuffer*  sourceBuffer0,
    const NMP::DataBuffer*  sourceBuffer1,
    const float             alpha);

  /// \brief Interpolative blending of 2 quaternion attitudes.
  NM_INLINE static void interpBlendQuats(
    NMP::Quat*              result,
    const NMP::Quat*        source0,
    const NMP::Quat*        source1,
    float                   alpha);

  /// \brief Additive blending of 2 quaternion attitudes.
  NM_INLINE static void additiveBlendQuats(
    NMP::Quat*              result,
    const NMP::Quat*        source0,
    const NMP::Quat*        source1,
    float                   alpha);

  /// \brief Subtractive blending of 2 quaternion attitudes.
  NM_INLINE static void subtractiveBlendQuats(
    NMP::Quat*              result,
    const NMP::Quat*        source0,
    const NMP::Quat*        source1,
    float                   alpha);

  /// \brief Interpolative blending of 2 quaternion attitudes.
  NM_INLINE static void blend2PartialInterpQuat(
    NMP::Quat*              attitude,
    bool*                   filteredOut,
    const NMP::Quat*        source0Att,
    bool                    source0FilteredOut,
    const NMP::Quat*        source1Att,
    bool                    source1FilteredOut,
    float                   alpha);

  /// \brief Interpolative blending of 2 position vectors.
  NM_INLINE static void blend2PartialInterpPos(
    NMP::Vector3*           pos,
    bool*                   filteredOut,
    const NMP::Vector3*     source0Pos,
    bool                    source0FilteredOut,
    const NMP::Vector3*     source1Pos,
    bool                    source1FilteredOut,
    float                   alpha);

  /// \brief Spherical interpolative blending of 2 position vectors.
  NM_INLINE static void blend2PartialSlerpPos(
    NMP::Vector3*           pos,
    bool*                   filteredOut,
    const NMP::Vector3*     source0Pos,
    bool                    source0FilteredOut,
    const NMP::Vector3*     source1Pos,
    bool                    source1FilteredOut,
    float                   alpha);

  /// \brief Interpolative blending of 2 quaternion attitudes and position vectors.
  NM_INLINE static void blend2PartialInterpQuatInterpPos(
    NMP::Quat*              attitude,
    NMP::Vector3*           pos,
    bool*                   filteredOut,
    const NMP::Quat*        source0Att,
    const NMP::Vector3*     source0Pos,
    bool                    source0FilteredOut,
    const NMP::Quat*        source1Att,
    const NMP::Vector3*     source1Pos,
    bool                    source1FilteredOut,
    float                   alpha);

  /// \brief Interpolative blending of 2 quaternion attitudes and position vectors.
  NM_INLINE static void blend2PartialInterpQuatSlerpPos(
    NMP::Quat*              attitude,
    NMP::Vector3*           pos,
    bool*                   filteredOut,
    const NMP::Quat*        source0Att,
    const NMP::Vector3*     source0Pos,
    bool                    source0FilteredOut,
    const NMP::Quat*        source1Att,
    const NMP::Vector3*     source1Pos,
    bool                    source1FilteredOut,
    float                   alpha);
 
  //**********************************************************************************************************************
  // SINGLE ENTRY
  //**********************************************************************************************************************

  /// \brief Adds (channel from buffer1)^alpha to the channel in buffer0.
  ///
  /// Leaves all other existing entries in this buffer.
  /// The specified channels must be present in both buffers.
  /// Positions are not blended.  They are passed directly from buffer0.
  NM_INLINE static void addQuatChannelMatching(
    NMP::DataBuffer*       destBuffer,
    uint32_t               channelID,
    const NMP::DataBuffer* sourceBuffer0,
    const NMP::DataBuffer* sourceBuffer1,
    float                  alpha);

  /// \brief Interpolate attitude and add position.
  NM_INLINE static void interpQuatChannelMatching(
    NMP::DataBuffer*       destBuffer,
    const uint32_t         channelID,
    const NMP::DataBuffer* sourceBuffer0,
    const NMP::DataBuffer* sourceBuffer1,
    const float            alpha);

  /// \brief Subtract attitude.
  NM_INLINE static void subtractQuatChannelMatching(
    NMP::DataBuffer*       destBuffer,
    const uint32_t         channelID,
    const NMP::DataBuffer* sourceBuffer0,
    const NMP::DataBuffer* sourceBuffer1,
    const float            alpha);
};

//----------------------------------------------------------------------------------------------------------------------
// BlendOpsBase functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BlendOpsBase::interpBlendQuats(
  NMP::Quat* result,
  const NMP::Quat* source0,
  const NMP::Quat* source1,
  float      alpha)
{
  NMP_ASSERT(source0 && source1 && result);
  NMP_ASSERT(alpha >= 0.0f && alpha <= 1.0f);

  // This check is needed here because, using quaternions, there are 2 possible representations of the same
  // attitude. Either representation could have been used for a bone channel in a source animation. This means that
  // when blending 2 source anim transforms they could have been defined in different spaces. The dot product being
  // negative give us an indication that the 2 transforms are so different that this is what has happened here and we
  // need to put them into the same frame of reference. This needs more thinking about.
  float fromDotTo = source0->dot(*source1);
  float sgn = NMP::floatSelect(fromDotTo, 1.0f, -1.0f);
  NMP::Quat source1Adj = (*source1) * sgn;
  fromDotTo *= sgn;

  // Perform the interpolation
  result->fastSlerp(*source0, source1Adj, alpha, fromDotTo);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BlendOpsBase::additiveBlendQuats(
  NMP::Quat* result,
  const NMP::Quat* source0,
  const NMP::Quat* source1,
  float      alpha)
{
  NMP::Quat scaledQuat;
  NMP::Quat identityQuat;

  NMP_ASSERT(source0 && source1 && result);

  // Interpolate between identity and the adding quat.
  identityQuat.identity();

  NMP::Quat source1Adj;

  float fromDotTo = identityQuat.dot(*source1);
  if (fromDotTo < 0)
  {
    // Take the shortest path between the 2.
    source1Adj = -(*source1);
    fromDotTo = -fromDotTo;
  }
  else
  {
    source1Adj = *source1;
  }
  scaledQuat.fastSlerp(identityQuat, source1Adj, alpha, fromDotTo);

  // Accumulate the result onto the to source quat.
  *result = scaledQuat * (*source0);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BlendOpsBase::subtractiveBlendQuats(
  NMP::Quat* result,
  const NMP::Quat* source0,
  const NMP::Quat* source1,
  float      alpha)
{
  NMP::Quat scaledQuat;
  NMP::Quat identityQuat;

  NMP_ASSERT(source0 && source1 && result);

  // Interpolate between identity and the adding quat.
  identityQuat.identity();

  NMP::Quat source0Adj;

  float fromDotTo = identityQuat.dot(*source0);
  if (fromDotTo < 0)
  {
    // Take the shortest path between the 2.
    source0Adj = -(*source0);
    fromDotTo = -fromDotTo;
  }
  else
  {
    source0Adj = *source0;
  }
  scaledQuat.fastSlerp(identityQuat, source0Adj, alpha, fromDotTo);

  // Accumulate the result onto the to conjugated source quat.
  NMP::Quat souce1Conjugate = ~(*source1);
  *result = scaledQuat * souce1Conjugate;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BlendOpsBase::blend2PartialInterpQuat(
  NMP::Quat*              attitude,
  bool*                   filteredOut,
  const NMP::Quat*        source0Att,
  bool                    source0FilteredOut,
  const NMP::Quat*        source1Att,
  bool                    source1FilteredOut,
  float                   alpha)
{
  NMP_ASSERT(attitude && filteredOut);
  NMP_ASSERT(source0Att && source1Att);
  NMP_ASSERT(alpha >= 0.0f && alpha <= 1.0f);

  if (source0FilteredOut && source1FilteredOut)
  {
    // Both sources have been filtered out.
    attitude->identity();
    *filteredOut = true;
  }
  else if (source0FilteredOut)
  {
    // Only source 1 exists.
    *attitude = *source1Att;
    *filteredOut = false;
  }
  else if (source1FilteredOut)
  {
    // Only source 0 exists.
    *attitude = *source0Att;
    *filteredOut = false;
  }
  else
  {
    // Both sources exist, do the blend.
    float fromDotTo = source0Att->dot(*source1Att);
    float sgn = NMP::floatSelect(fromDotTo, 1.0f, -1.0f);
    NMP::Quat source1AttAdj = (*source1Att) * sgn;
    fromDotTo *= sgn;

    // Perform the interpolation
    attitude->fastSlerp(*source0Att, source1AttAdj, alpha, fromDotTo);

    *filteredOut = false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BlendOpsBase::blend2PartialInterpPos(
  NMP::Vector3*           pos,
  bool*                   filteredOut,
  const NMP::Vector3*     source0Pos,
  bool                    source0FilteredOut,
  const NMP::Vector3*     source1Pos,
  bool                    source1FilteredOut,
  float                   alpha)
{
  NMP_ASSERT(pos && filteredOut);
  NMP_ASSERT(source0Pos && source1Pos);
  NMP_ASSERT(alpha >= 0.0f && alpha <= 1.0f);

  if (source0FilteredOut && source1FilteredOut)
  {
    // Both sources have been filtered out.
    pos->setToZero();
    *filteredOut = true;
  }
  else if (source0FilteredOut)
  {
    // Only source 1 exists.
    *pos = *source1Pos;
    *filteredOut = false;
  }
  else if (source1FilteredOut)
  {
    // Only source 0 exists.
    *pos = *source0Pos;
    *filteredOut = false;
  }
  else
  {
    // Both sources exist, do the blend.
    pos->lerp(
      *source0Pos,
      *source1Pos,
      alpha);
    *filteredOut = false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BlendOpsBase::blend2PartialSlerpPos(
  NMP::Vector3*           pos,
  bool*                   filteredOut,
  const NMP::Vector3*     source0Pos,
  bool                    source0FilteredOut,
  const NMP::Vector3*     source1Pos,
  bool                    source1FilteredOut,
  float                   alpha)
{
  NMP_ASSERT(pos && filteredOut);
  NMP_ASSERT(source0Pos && source1Pos);
  NMP_ASSERT(alpha >= 0.0f && alpha <= 1.0f);

  if (source0FilteredOut && source1FilteredOut)
  {
    // Both sources have been filtered out.
    pos->setToZero();
    *filteredOut = true;
  }
  else if (source0FilteredOut)
  {
    // Only source 1 exists.
    *pos = *source1Pos;
    *filteredOut = false;
  }
  else if (source1FilteredOut)
  {
    // Only source 0 exists.
    *pos = *source0Pos;
    *filteredOut = false;
  }
  else
  {
    // Both sources exist, do the blend.
    pos->slerp(
      *source0Pos,
      *source1Pos,
      alpha);
    *filteredOut = false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BlendOpsBase::blend2PartialInterpQuatInterpPos(
  NMP::Quat*              attitude,
  NMP::Vector3*           pos,
  bool*                   filteredOut,
  const NMP::Quat*        source0Att,
  const NMP::Vector3*     source0Pos,
  bool                    source0FilteredOut,
  const NMP::Quat*        source1Att,
  const NMP::Vector3*     source1Pos,
  bool                    source1FilteredOut,
  float                   alpha)
{
  NMP_ASSERT(attitude && pos && filteredOut);
  NMP_ASSERT(source0Att && source1Att);
  NMP_ASSERT(source0Pos && source1Pos);
  NMP_ASSERT(alpha >= 0.0f && alpha <= 1.0f);

  if (source0FilteredOut && source1FilteredOut)
  {
    // Both sources have been filtered out.
    attitude->identity();
    pos->setToZero();
    *filteredOut = true;
  }
  else if (source0FilteredOut)
  {
    // Only source 1 exists.
    *attitude = *source1Att;
    *pos = *source1Pos;
    *filteredOut = false;
  }
  else if (source1FilteredOut)
  {
    // Only source 0 exists.
    *attitude = *source0Att;
    *pos = *source0Pos;
    *filteredOut = false;
  }
  else
  {
    // Both sources exist, do the blend.
    float fromDotTo = source0Att->dot(*source1Att);
    float sgn = NMP::floatSelect(fromDotTo, 1.0f, -1.0f);
    NMP::Quat source1AttAdj = (*source1Att) * sgn;
    fromDotTo *= sgn;

    // Perform the interpolation
    attitude->fastSlerp(*source0Att, source1AttAdj, alpha, fromDotTo);

    pos->lerp(
      *source0Pos,
      *source1Pos,
      alpha);

    *filteredOut = false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BlendOpsBase::blend2PartialInterpQuatSlerpPos(
  NMP::Quat*              attitude,
  NMP::Vector3*           pos,
  bool*                   filteredOut,
  const NMP::Quat*        source0Att,
  const NMP::Vector3*     source0Pos,
  bool                    source0FilteredOut,
  const NMP::Quat*        source1Att,
  const NMP::Vector3*     source1Pos,
  bool                    source1FilteredOut,
  float                   alpha)
{
  NMP_ASSERT(attitude && pos && filteredOut);
  NMP_ASSERT(source0Att && source1Att);
  NMP_ASSERT(source0Pos && source1Pos);
  NMP_ASSERT(alpha >= 0.0f && alpha <= 1.0f);

  if (source0FilteredOut && source1FilteredOut)
  {
    // Both sources have been filtered out.
    attitude->identity();
    pos->setToZero();
    *filteredOut = true;
  }
  else if (source0FilteredOut)
  {
    // Only source 1 exists.
    *attitude = *source1Att;
    *pos = *source1Pos;
    *filteredOut = false;
  }
  else if (source1FilteredOut)
  {
    // Only source 0 exists.
    *attitude = *source0Att;
    *pos = *source0Pos;
    *filteredOut = false;
  }
  else
  {
    // Both sources exist, do the blend.
    float fromDotTo = source0Att->dot(*source1Att);
    float sgn = NMP::floatSelect(fromDotTo, 1.0f, -1.0f);
    NMP::Quat source1AttAdj = (*source1Att) * sgn;
    fromDotTo *= sgn;

    // Perform the interpolation
    attitude->fastSlerp(*source0Att, source1AttAdj, alpha, fromDotTo);

    pos->slerp(
      *source0Pos,
      *source1Pos,
      alpha);

    *filteredOut = false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BlendOpsBase::addQuatChannelMatching(
  NMP::DataBuffer*       destBuffer,
  uint32_t               channelID,
  const NMP::DataBuffer* sourceBuffer0,
  const NMP::DataBuffer* sourceBuffer1,
  float                  alpha)
{
  NMP_ASSERT(sourceBuffer0 && sourceBuffer1 && destBuffer);
  NMP_ASSERT(channelID < sourceBuffer0->getLength());
  NMP_ASSERT(channelID < sourceBuffer1->getLength());
  NMP_ASSERT(channelID < destBuffer->getLength());
  NMP_ASSERT(sourceBuffer0->hasChannel(channelID) && sourceBuffer1->hasChannel(channelID));

  ((NMP::Vector3*)destBuffer->getElementData(0))[channelID] =
    ((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID];
  additiveBlendQuats(
    &((NMP::Quat*)destBuffer->getElementData(1))[channelID],
    &((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID],
    &((NMP::Quat*)sourceBuffer1->getElementData(1))[channelID],
    alpha);

  destBuffer->getUsedFlags()->setBit(channelID);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BlendOpsBase::interpQuatChannelMatching(
  NMP::DataBuffer*        destBuffer,
  const uint32_t          channelID,
  const NMP::DataBuffer*  sourceBuffer0,
  const NMP::DataBuffer*  sourceBuffer1,
  const float             alpha)
{
  NMP_ASSERT(sourceBuffer0 && sourceBuffer1 && destBuffer);
  NMP_ASSERT(alpha >= 0.0f && alpha <= 1.0f);
  NMP_ASSERT(channelID < sourceBuffer0->getLength());
  NMP_ASSERT(channelID < sourceBuffer1->getLength());
  NMP_ASSERT(channelID < destBuffer->getLength());
  NMP_ASSERT(sourceBuffer0->hasChannel(channelID) && sourceBuffer1->hasChannel(channelID));

  // Check that this buffer is actually a pos-quat buffer
  NMP_ASSERT(destBuffer->getNumElements() == 2);
  NMP_ASSERT(destBuffer->getElementDescriptor(0).m_type == NMP::DataBuffer::NMP_ELEMENT_TYPE_VEC3);
  NMP_ASSERT(destBuffer->getElementDescriptor(1).m_type == NMP::DataBuffer::NMP_ELEMENT_TYPE_QUAT);

  interpBlendQuats(
    &((NMP::Quat*)destBuffer->getElementData(1))[channelID],
    &((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID],
    &((NMP::Quat*)sourceBuffer1->getElementData(1))[channelID],
    alpha);

  destBuffer->getUsedFlags()->setBit(channelID);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BlendOpsBase::subtractQuatChannelMatching( NMP::DataBuffer* destBuffer, const uint32_t channelID, const NMP::DataBuffer* sourceBuffer0, const NMP::DataBuffer* sourceBuffer1, const float alpha )
{
  NMP_ASSERT(sourceBuffer0 && sourceBuffer1 && destBuffer);
  NMP_ASSERT(alpha >= 0.0f && alpha <= 1.0f);
  NMP_ASSERT(channelID < sourceBuffer0->getLength());
  NMP_ASSERT(channelID < sourceBuffer1->getLength());
  NMP_ASSERT(channelID < destBuffer->getLength());
  NMP_ASSERT(sourceBuffer0->hasChannel(channelID) && sourceBuffer1->hasChannel(channelID));

  // Check that this buffer is actually a pos-quat buffer
  NMP_ASSERT(destBuffer->getNumElements() == 2);
  NMP_ASSERT(destBuffer->getElementDescriptor(0).m_type == NMP::DataBuffer::NMP_ELEMENT_TYPE_VEC3);
  NMP_ASSERT(destBuffer->getElementDescriptor(1).m_type == NMP::DataBuffer::NMP_ELEMENT_TYPE_QUAT);

  subtractiveBlendQuats(
    &((NMP::Quat*)destBuffer->getElementData(1))[channelID],
    &((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID],
    &((NMP::Quat*)sourceBuffer1->getElementData(1))[channelID],
    alpha);

  destBuffer->getUsedFlags()->setBit(channelID);
}


//**********************************************************************************************************************
// SINGLE ENTRY
//**********************************************************************************************************************

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BlendOpsBase::interpQuatInterpPosChannel(
  NMP::DataBuffer*       destBuffer,
  const uint32_t         channelID,
  const NMP::DataBuffer* sourceBuffer0,
  const NMP::DataBuffer* sourceBuffer1,
  const float            alpha)
{
  NMP_ASSERT(sourceBuffer0 && sourceBuffer1 && destBuffer);
  NMP_ASSERT(alpha >= 0.0f && alpha <= 1.0f);
  NMP_ASSERT(channelID < sourceBuffer0->getLength());
  NMP_ASSERT(channelID < sourceBuffer1->getLength());
  NMP_ASSERT(channelID < destBuffer->getLength());

  if (sourceBuffer0->hasChannel(channelID) && sourceBuffer1->hasChannel(channelID))
  {
    // Both buffers have this channel so interpolate.
    interpQuatChannelMatching(destBuffer, channelID, sourceBuffer0, sourceBuffer1, alpha);

    // Both buffers have this channel so interpolate.
    ((NMP::Vector3*)destBuffer->getElementData(0))[channelID].lerp(
      ((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID],
      ((NMP::Vector3*)sourceBuffer1->getElementData(0))[channelID],
      alpha);
  }
  else if (sourceBuffer0->hasChannel(channelID))
  {
    // Only buffer 0 has this channel so copy it into the output buffer.
    ((NMP::Vector3*)destBuffer->getElementData(0))[channelID] = ((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID];
    ((NMP::Quat*)destBuffer->getElementData(1))[channelID] = ((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID];
    destBuffer->getUsedFlags()->setBit(channelID);
  }
  else
  {
    NMP_ASSERT(sourceBuffer1->hasChannel(channelID));
    ((NMP::Vector3*)destBuffer->getElementData(0))[channelID] = ((NMP::Vector3*)sourceBuffer1->getElementData(0))[channelID];
    ((NMP::Quat*)destBuffer->getElementData(1))[channelID] = ((NMP::Quat*)sourceBuffer1->getElementData(1))[channelID];
    destBuffer->getUsedFlags()->setBit(channelID);
  }
}

#ifdef NMP_PLATFORM_SIMD
//----------------------------------------------------------------------------------------------------------------------
// SIMD optimized overrides
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \class BlendOpsSIMD
/// \brief Transform Buffer blend functions called by operations on the Network ops queue.
/// \Overrides BlendOpsBase with platform independent optimizations.
/// \ingroup BufferBlendOperationsModule
///
/// This is a shared resource for any calling operation to be able to use.
/// Many operations have more than one implementation for optimal operation on target platforms.
//----------------------------------------------------------------------------------------------------------------------
class BlendOpsSIMD : public BlendOpsBase
{
public:

  /// \brief Subtract buffer1 from buffer0 in both the quaternions and positions
  static void subtractQuatSubtractPosPartial(
    NMP::DataBuffer*       destBuffer,
    const NMP::DataBuffer* sourceBuffer0,
    const NMP::DataBuffer* sourceBuffer1,
    const float            alpha                   ///< Global blend weight.
    );

  /// \brief Subtract buffer1 from buffer0 in the quaternions and interpolate the positions
  static void subtractQuatInterpPosPartial(
    NMP::DataBuffer*       destBuffer,
    const NMP::DataBuffer* sourceBuffer0,
    const NMP::DataBuffer* sourceBuffer1,
    const float            alpha                   ///< Global blend weight.
    );

  /// \brief Interpolate the quaternions and Subtract buffer1 from buffer0 in the positions
  static void interpQuatSubtractPosPartial(
    NMP::DataBuffer*       destBuffer,
    const NMP::DataBuffer* sourceBuffer0,
    const NMP::DataBuffer* sourceBuffer1,
    const float            alpha                   ///< Global blend weight.
    );

  /// \brief For all channels in buffer1, add buffer1^alpha to buffer0 and blend positions in the expected way.
  ///
  /// All other channels from buffer 0 are propagated.  Channels existing only on buffer1 are ignored.
  static void addQuatAddPosPartial(
    NMP::DataBuffer*       destBuffer,
    const NMP::DataBuffer* sourceBuffer0,
    const NMP::DataBuffer* sourceBuffer1,
    const float            alpha                   ///< Global blend weight.
    );

  /// \brief For all channels in buffer1, add buffer1^alpha to buffer0 and blend positions in the expected way,
  /// using per bone weighting as a scale.
  ///
  /// All other channels from buffer 0 are propagated.  Channels existing only on buffer1 are ignored.
  static void addQuatAddPosPartialFeathered(
    NMP::DataBuffer*       destBuffer,
    const NMP::DataBuffer* sourceBuffer0,
    const NMP::DataBuffer* sourceBuffer1,
    const float            alpha,                  ///< Global blend weight.
    const uint32_t         numAlphaValues,
    const float*           alphaValues             ///< Per bone blend weights.
  );

  /// \brief For all channels in buffer1, add buffer1^alpha to buffer0
  ///
  /// All other channels from buffer 0 are propagated.  Channels existing only on buffer1 are ignored.
  /// Positions are not blended; they are passed directly from buffer0.
  static void addQuatLeavePosPartial(
    NMP::DataBuffer*       destBuffer,
    const NMP::DataBuffer* sourceBuffer0,
    const NMP::DataBuffer* sourceBuffer1,
    const float            alpha                   ///< Global blend weight.
  );

  /// \brief For all attitude channels in buffer1, add buffer1^alpha to buffer0, using per bone weighting as a scale.
  ///
  /// All other channels from buffer 0 are propagated.  Channels existing only on buffer1 are ignored.
  /// Positions are not blended; They are passed directly from buffer0.
  static void addQuatLeavePosPartialFeathered(
    NMP::DataBuffer*       destBuffer,
    const NMP::DataBuffer* sourceBuffer0,
    const NMP::DataBuffer* sourceBuffer1,
    const float            alpha,                  ///< Global blend weight.
    const uint32_t         numAlphaValues,
    const float*           alphaValues             ///< Per bone blend weights.
  );

  /// \brief Interpolate 2 process buffers, whose channel sets can be partial sub-sets of each other.
  ///
  /// Any channels that exists only in one source is simply copied into the result with no scaling.
  /// Clears all existing entries in this buffer.
  static void interpQuatInterpPosPartial(
    NMP::DataBuffer*       destBuffer,
    const NMP::DataBuffer* sourceBuffer0,
    const NMP::DataBuffer* sourceBuffer1,
    const float            alpha                   ///< Global blend weight.
  );

  /// \brief Interpolate 2 process buffers, whose channel sets can be partial sub-sets of each other,
  /// using per bone weighting as a scale.
  ///
  /// Any channels that exists only in one source is simply copied into the result with no scaling.
  /// Clears all existing entries in this buffer.
  static void interpQuatInterpPosPartialFeathered(
    NMP::DataBuffer*       destBuffer,
    const NMP::DataBuffer* sourceBuffer0,
    const NMP::DataBuffer* sourceBuffer1,
    const float            alpha,                  ///< Global blend weight.
    const uint32_t         numAlphaValues,
    const float*           alphaValues             ///< Per bone weights.
  );

  /// \brief Interpolate the quaternions and add buffer1^alpha to buffer0 the positions of two buffers.
  static void interpQuatAddPosPartial(
    NMP::DataBuffer*       destBuffer,
    const NMP::DataBuffer* sourceBuffer0,
    const NMP::DataBuffer* sourceBuffer1,
    const float            alpha                   ///< Global blend weight.
  );

  /// \brief Interpolate the quaternions and add buffer1^alpha to buffer0 the positions of two buffers,
  /// using per bone weighting as a scale.
  static void interpQuatAddPosPartialFeathered(
    NMP::DataBuffer*       destBuffer,
    const NMP::DataBuffer* sourceBuffer0,
    const NMP::DataBuffer* sourceBuffer1,
    const float            alpha,                  ///< Global blend weight.
    const uint32_t         numAlphaValues,
    const float*           alphaValues             ///< Per bone blend weights.
  );
};

class BlendOps : public BlendOpsSIMD  { /* SIMD optimizations */ };

//----------------------------------------------------------------------------------------------------------------------
#else

class BlendOps : public BlendOpsBase { /* default vanilla C */ };

#endif


//----------------------------------------------------------------------------------------------------------------------
// Trajectory blend Ops
//----------------------------------------------------------------------------------------------------------------------
#define MR_TRAJECTORY_BLEND_TASKS_FORCEINLINE NM_FORCEINLINE

typedef void (*Blend2QuatBlendFunc)(
  NMP::Quat* result,
  const NMP::Quat* source0,
  const NMP::Quat* source1,
  float      alpha);

enum Blend2WeightingType
{
  BLEND2_WEIGHTING_PER_RIG,
  BLEND2_WEIGHTING_PER_BONE,
};

enum Blend2PosUpdateType
{
  BLEND2_ADD_POS,
  BLEND2_LERP_POS,
  BLEND2_SLERP_POS,
  BLEND2_SUBTRACT_POS,
};

//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
// Disable 'Conditional expression is constant' warning, template parameters are deliberately compile-time constants.
#pragma warning(push)
#pragma warning(disable: 4127)
#endif // _MSC_VER

template <Blend2WeightingType _BlendWeighting, Blend2QuatBlendFunc _QuatBlendFunc, Blend2PosUpdateType _PosUpdateType>
MR_TRAJECTORY_BLEND_TASKS_FORCEINLINE void Blend2TrajectoryDeltaTransforms(
  NMP::Vector3*               pos,
  NMP::Quat*                  attitude,
  bool*                       filteredOut,
  const NMP::Vector3*         source0Pos,
  const NMP::Quat*            source0Att,
  bool                        source0FilteredOut,
  const NMP::Vector3*         source1Pos,
  const NMP::Quat*            source1Att,
  bool                        source1FilteredOut,
  float                       blendWeight,
  const AttribDataFeatherBlend2ChannelAlphas* boneWeights)
{
  if (source0FilteredOut && source1FilteredOut)
  {
    // Both sources have been filtered out.
    attitude->identity();
    pos->setToZero();
    *filteredOut = true;
  }
  else if (source0FilteredOut)
  {
    // Only source 1 exists.
    *attitude = *source1Att;
    *pos = *source1Pos;
    *filteredOut = false;
  }
  else if (source1FilteredOut)
  {
    // Only source 0 exists.
    *attitude = *source0Att;
    *pos = *source0Pos;
    *filteredOut = false;
  }
  else
  {
    // Both sources exist, do the blend.
    *filteredOut = false;

    float clampedWeighting;
    if (_BlendWeighting == BLEND2_WEIGHTING_PER_BONE)
    {
      NMP_ASSERT(boneWeights);
      uint32_t trajBoneIndex = boneWeights->m_trajectoryBoneIndex;
      NMP_ASSERT(trajBoneIndex <= boneWeights->m_numChannelAlphas);
      clampedWeighting = blendWeight * boneWeights->m_channelAlphas[trajBoneIndex];
      clampedWeighting = NMP::clampValue(clampedWeighting, 0.0f, 1.0f);
    }
    else
    {
      clampedWeighting = NMP::clampValue(blendWeight, 0.0f, 1.0f);
    }

    _QuatBlendFunc(
      attitude,
      source0Att,
      source1Att,
      clampedWeighting);

    if (_PosUpdateType == BLEND2_ADD_POS)
    {
      *pos = *source0Pos + (*source1Pos * clampedWeighting);
    }
    else if (_PosUpdateType == BLEND2_LERP_POS)
    {
      pos->lerp(*source0Pos, *source1Pos, clampedWeighting);
    }
    else if (_PosUpdateType == BLEND2_SUBTRACT_POS)
    {
      *pos = (*source0Pos - *source1Pos) * clampedWeighting;
    }
    else // if (_PosUpdateType == BLEND2_SLERP_POS)
    {
      pos->slerp(*source0Pos, *source1Pos, clampedWeighting);
    }
  }
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER
//----------------------------------------------------------------------------------------------------------------------

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_BLEND_OPS_H
//----------------------------------------------------------------------------------------------------------------------
