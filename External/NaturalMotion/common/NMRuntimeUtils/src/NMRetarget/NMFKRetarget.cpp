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
#include "NMPlatform/NMProfiler.h"
#include "NMPlatform/NMMemory.h"
#include "NMRetarget/NMFKRetarget.h"
#include "NMGeomUtils/NMGeomUtilsMP.h"
//----------------------------------------------------------------------------------------------------------------------

#define VALIDATE_TRANSFORMSx
#define ISFINITE_QUAT(q) ((q).x * 0 == 0 && (q).y * 0 == 0 && (q).z * 0 == 0 && (q).w * 0 == 0)
#define ISFINITE_VECTOR3(v) ((v).x * 0 == 0 && (v).y * 0 == 0 && (v).z * 0 == 0)
#define ISNORMAL_QUAT(q) (NMP::nmfabs((q).magnitudeSquared() - 1.0f) < 1e-2f)
#define ISVALID_TM(t, q) ISFINITE_VECTOR3(t) && ISFINITE_QUAT(q) && ISNORMAL_QUAT(q)

#define LOW_LEVEL_PROFILINGx
#ifdef LOW_LEVEL_PROFILING
  #define LOW_LEVEL_PROFILE_BEGIN(ev) NMIK_PROFILE_BEGIN(ev)
  #define LOW_LEVEL_PROFILE_END() NMIK_PROFILE_END()
#else
  #define LOW_LEVEL_PROFILE_BEGIN(ev) {}
  #define LOW_LEVEL_PROFILE_END() {}
#endif

namespace NMRU
{

//----------------------------------------------------------------------------------------------------------------------
void FKRetarget::Solver::copy(Solver* dest)
{
  NMP_ASSERT(dest->m_memoryRequirements == m_memoryRequirements);

  NMP::Memory::memcpy(dest, this, m_memoryRequirements.size);
  dest->relocate();
}

//----------------------------------------------------------------------------------------------------------------------
void FKRetarget::Solver::retarget1to1(
  const Params* sourceParams,
  const NMP::DataBuffer* sourceTransforms,
  const Params* targetParams,
  NMP::DataBuffer* targetTransforms)
{
  NMP_ASSERT(sourceParams);
  NMP_ASSERT(sourceTransforms);
  NMP_ASSERT(targetParams);
  NMP_ASSERT(targetTransforms);
  (void)sourceParams; // Will be used eventually

  // Transfer target rig transforms from source rig where there is a map
  //  pi is the package index
  for (uint32_t pi = 0; pi < m_mpSourceLength; ++pi)
  {

    // Handy aliases for the joint indices
    const int32_t& s0 = m_mpSourceMappings[pi].jointIndex[0];
    const int32_t& s1 = m_mpSourceMappings[pi].jointIndex[1];
    const int32_t& s2 = m_mpSourceMappings[pi].jointIndex[2];
    const int32_t& s3 = m_mpSourceMappings[pi].jointIndex[3];
    const int32_t& t0 = m_mpTargetMappings[pi].jointIndex[0];
    const int32_t& t1 = m_mpTargetMappings[pi].jointIndex[1];
    const int32_t& t2 = m_mpTargetMappings[pi].jointIndex[2];
    const int32_t& t3 = m_mpTargetMappings[pi].jointIndex[3];

    // Only proceed if at least one mapping is one-to-one.  We can check this by just testing
    // the first channel in the package.  This is because the way we've packed the packages
    // ensures all the one-to-one mappings are first.
    if (!(m_intermediateRigData[sourceParams->indexMap[s0]].sourceSequenceLength == 1 &&
          m_intermediateRigData[sourceParams->indexMap[s0]].targetSequenceLength == 1))
    {
      continue;
    }

    // Set the used flags on the target geometry
    bool isFull = sourceTransforms->isFull();
    bool used[4] = {
      m_mpTargetMappings[pi].valid[0] && (isFull || sourceTransforms->hasChannel(s0)),
      m_mpTargetMappings[pi].valid[1] && (isFull || sourceTransforms->hasChannel(s1)),
      m_mpTargetMappings[pi].valid[2] && (isFull || sourceTransforms->hasChannel(s2)),
      m_mpTargetMappings[pi].valid[3] && (isFull || sourceTransforms->hasChannel(s3))
    };
    //  Must not set used flag on invalid channels, or we will be overriding the used status on some
    //  random other channel (usually channel 0).
    for (uint32_t el = 0; el < 4; ++el)
    {
      if (m_mpTargetMappings[pi].valid[el])
      {
        targetTransforms->setChannelUsedFlag(m_mpTargetMappings[pi].jointIndex[el], used[el]);
      }
    }

    // If all the input geometry is invalid, be sure to skip so as to avoid unnecessary calculation
    if (!(used[0] || used[1] || used[2] || used[3]))
    {
      continue;
    }

    //  Create mask for selecting valid channels
    NMP::vpu::vector4_t maskUnusedChannels = NMP::vpu::mask4i(
      used[0],
      used[1],
      used[2],
      used[3]);
    //  Create mask for selecting non-rigid bones (so we keep the retargeted translations)
    NMP::vpu::vector4_t maskRigidBones = NMP::vpu::mask4i(
      !targetParams->perJointParams[t0].isRigid,
      !targetParams->perJointParams[t1].isRigid,
      !targetParams->perJointParams[t2].isRigid,
      !targetParams->perJointParams[t3].isRigid
      );

    //----------------------------------------------------------------------------------------------------------------------
    // Pack the geometry for these four joints
    //  Source geom
    NMP::vpu::Vector3MP sourcePosMP;
    sourcePosMP.pack(
      *(NMP::vpu::vector4_t*)sourceTransforms->getPosQuatChannelPos(s0),
      *(NMP::vpu::vector4_t*)sourceTransforms->getPosQuatChannelPos(s1),
      *(NMP::vpu::vector4_t*)sourceTransforms->getPosQuatChannelPos(s2),
      *(NMP::vpu::vector4_t*)sourceTransforms->getPosQuatChannelPos(s3));
    NMP::vpu::QuatMP sourceQuatMP;
    sourceQuatMP.pack(
      *(NMP::vpu::vector4_t*)sourceTransforms->getPosQuatChannelQuat(s0),
      *(NMP::vpu::vector4_t*)sourceTransforms->getPosQuatChannelQuat(s1),
      *(NMP::vpu::vector4_t*)sourceTransforms->getPosQuatChannelQuat(s2),
      *(NMP::vpu::vector4_t*)sourceTransforms->getPosQuatChannelQuat(s3));
    //  Target input geom - we may keep this and discard the retarget result
    MPTransform targetTransformIn;
    targetTransformIn.t.pack(
      *(NMP::vpu::vector4_t*)targetTransforms->getPosQuatChannelPos(t0),
      *(NMP::vpu::vector4_t*)targetTransforms->getPosQuatChannelPos(t1),
      *(NMP::vpu::vector4_t*)targetTransforms->getPosQuatChannelPos(t2),
      *(NMP::vpu::vector4_t*)targetTransforms->getPosQuatChannelPos(t3));
    targetTransformIn.q.pack(
      *(NMP::vpu::vector4_t*)targetTransforms->getPosQuatChannelQuat(t0),
      *(NMP::vpu::vector4_t*)targetTransforms->getPosQuatChannelQuat(t1),
      *(NMP::vpu::vector4_t*)targetTransforms->getPosQuatChannelQuat(t2),
      *(NMP::vpu::vector4_t*)targetTransforms->getPosQuatChannelQuat(t3));

    //----------------------------------------------------------------------------------------------------------------------
    // Do the Retarget

    // Working transforms
    NMP::vpu::Vector3MP targetPosMP;
    NMP::vpu::QuatMP targetQuatMP;

    // First apply offsets to get the offset joints for the source and place in the target
    targetPosMP =  sourcePosMP + m_mpSourceGeom[pi].offset.t;
    targetQuatMP = m_mpSourceGeom[pi].offset.q * sourceQuatMP;

    // Now move the joints into the parent reference frame
    //  This a post-multiply-inverse
    targetQuatMP = m_mpSourceGeom[pi].parentReference.q.conjugate() * targetQuatMP;
    targetPosMP =
      m_mpSourceGeom[pi].parentReference.q.inverseRotateVector(targetPosMP - m_mpSourceGeom[pi].parentReference.t);
    // Now do a local transform into the reference frame
    //  This is a pre-multiply
    targetPosMP += targetQuatMP.rotateVector(m_mpSourceGeom[pi].reference.t);
    targetQuatMP = targetQuatMP * m_mpSourceGeom[pi].reference.q;

    // Apply weighting and mirroring

    // Now do a local transform from the target reference frame into the target offset frame
    //  This is a pre-multiply-inverse
    targetQuatMP = targetQuatMP * m_mpTargetGeom[pi].reference.q.conjugate();
    targetPosMP -= targetQuatMP.rotateVector(m_mpTargetGeom[pi].reference.t);
    // And move the joint back out of the parent reference frame into the parent frame
    //  This is a post-multiply
    targetQuatMP = m_mpTargetGeom[pi].parentReference.q * targetQuatMP;
    targetPosMP =
      m_mpTargetGeom[pi].parentReference.q.rotateVector(targetPosMP) + m_mpTargetGeom[pi].parentReference.t;

    // Now un-apply target offsets to get the target joint
    targetPosMP -= m_mpTargetGeom[pi].offset.t;
    targetQuatMP = m_mpTargetGeom[pi].offset.q.conjugate() * targetQuatMP;

    //----------------------------------------------------------------------------------------------------------------------
    // Unpack the result into the output
    //  Copy the start transforms so we can choose whether or not to replace them with the retargeted
    //  versions, based on whether channels are valid etc.
    //  Create output transform package which selects from the input or the retargeted value
    MPTransform targetTransformOut;
    //  Create mask for masking out channels that are not both used and have non-rigid bones
    NMP::vpu::vector4_t mask = NMP::vpu::and4(maskUnusedChannels, maskRigidBones);
    //  Select translations
    targetTransformOut.t = targetPosMP.merge(targetTransformIn.t, mask);
    //  Select rotations
    targetTransformOut.q = targetQuatMP.merge(targetTransformIn.q, maskUnusedChannels);
    //  Unpack into output buffer
    targetTransformOut.t.unpack(
      *(NMP::vpu::vector4_t*)targetTransforms->getPosQuatChannelPos(t0),
      *(NMP::vpu::vector4_t*)targetTransforms->getPosQuatChannelPos(t1),
      *(NMP::vpu::vector4_t*)targetTransforms->getPosQuatChannelPos(t2),
      *(NMP::vpu::vector4_t*)targetTransforms->getPosQuatChannelPos(t3));
    targetTransformOut.q.unpack(
      *(NMP::vpu::vector4_t*)targetTransforms->getPosQuatChannelQuat(t0),
      *(NMP::vpu::vector4_t*)targetTransforms->getPosQuatChannelQuat(t1),
      *(NMP::vpu::vector4_t*)targetTransforms->getPosQuatChannelQuat(t2),
      *(NMP::vpu::vector4_t*)targetTransforms->getPosQuatChannelQuat(t3));

#ifdef NM_DEBUG
    for (uint32_t i = 0; i < 4; ++i)
    {
      const NMP::Vector3& pos = *targetTransforms->getPosQuatChannelPos(m_mpTargetMappings[pi].jointIndex[i]);
      const NMP::Quat& quat = *targetTransforms->getPosQuatChannelQuat(m_mpTargetMappings[pi].jointIndex[i]);
      NMP_ASSERT(ISFINITE_VECTOR3(pos));
      NMP_ASSERT(ISNORMAL_QUAT(quat));
      NMP_ASSERT(ISFINITE_QUAT(quat));
    }
#endif

    //----------------------------------------------------------------------------------------------------------------------
    // Clamp
    for (uint32_t i = 0; i < 4; ++i)
    {
      uint32_t j = m_mpTargetMappings[pi].jointIndex[i];
      const PerJointParams& targetJointParams = targetParams->perJointParams[j];
      if (m_mpTargetMappings[pi].valid[i] && targetJointParams.isLimited)
      {
        JointLimits::clamp(targetJointParams.limits, *targetTransforms->getPosQuatChannelQuat(j));
      }
    }

    // If the last joint in the package is not a 1-to-1 mapping, then we know that none will be
    // from here on, because of the way we did the packing.  So we can exit.
    if (!(m_intermediateRigData[sourceParams->indexMap[s3]].sourceSequenceLength == 1 &&
          m_intermediateRigData[sourceParams->indexMap[s3]].targetSequenceLength == 1))
    {
      break;
    }

  } // End of loop through intermediate joint indices in groups of four

} // end of FKRetarget::Solver::retarget1to1() [1-to-1 version]

//----------------------------------------------------------------------------------------------------------------------
void FKRetarget::Solver::retargetSource(
  const NMP::DataBuffer* sourceTransforms,
  MPTransform* mpSourceTransforms)
{
  NMP_ASSERT(sourceTransforms);

  LOW_LEVEL_PROFILE_BEGIN("retargetSource");

  // Generate intermediate rig transforms from source rig
  //  pi is the package index
  for (uint32_t pi = 0; pi < m_mpSourceLength; ++pi)
  {

    // Handy aliases for the joint indices
    const int32_t& s0 = m_mpSourceMappings[pi].jointIndex[0];
    const int32_t& s1 = m_mpSourceMappings[pi].jointIndex[1];
    const int32_t& s2 = m_mpSourceMappings[pi].jointIndex[2];
    const int32_t& s3 = m_mpSourceMappings[pi].jointIndex[3];

    // OPTIMISE This code is quite slow but it's touch-and-go whether it's slower than processing
    // lots of unused channels on, say, a split rig.
#define SKIP_UNUSED_CHANNEL_BLOCKSx
#ifdef SKIP_UNUSED_CHANNEL_BLOCKS
    if (!sourceTransforms->isFull() &&
        !(sourceTransforms->hasChannel(s0) ||
          sourceTransforms->hasChannel(s1) ||
          sourceTransforms->hasChannel(s2) ||
          sourceTransforms->hasChannel(s3)))
    {
      continue;
    }
#endif

    // Get mask for valid entries
    const vector4& validMask = m_mpSourceGeom[pi].validMask;

    // Sanity check the input transforms
#ifdef NM_DEBUG
    for (uint32_t el = 0; el < 4; ++el)
    {
      uint32_t j = m_mpSourceMappings[pi].jointIndex[el];
      if (m_mpSourceMappings[pi].valid[el] && sourceTransforms->hasChannel(j))
      {
        const NMP::Vector3& pos = *sourceTransforms->getPosQuatChannelPos(j);
        const NMP::Quat& quat = *sourceTransforms->getPosQuatChannelQuat(j);
        NMP_ASSERT(ISVALID_TM(pos, quat));
      }
    }
#endif

    //----------------------------------------------------------------------------------------------------------------------
    // Pack the geometry for these four joints
    NMP::vpu::Vector3MP& sourcePosMP = mpSourceTransforms[pi].t;
    sourcePosMP.pack(
      *(NMP::vpu::vector4_t*)sourceTransforms->getPosQuatChannelPos(s0),
      *(NMP::vpu::vector4_t*)sourceTransforms->getPosQuatChannelPos(s1),
      *(NMP::vpu::vector4_t*)sourceTransforms->getPosQuatChannelPos(s2),
      *(NMP::vpu::vector4_t*)sourceTransforms->getPosQuatChannelPos(s3));
    NMP::vpu::QuatMP& sourceQuatMP = mpSourceTransforms[pi].q;
    sourceQuatMP.pack(
      *(NMP::vpu::vector4_t*)sourceTransforms->getPosQuatChannelQuat(s0),
      *(NMP::vpu::vector4_t*)sourceTransforms->getPosQuatChannelQuat(s1),
      *(NMP::vpu::vector4_t*)sourceTransforms->getPosQuatChannelQuat(s2),
      *(NMP::vpu::vector4_t*)sourceTransforms->getPosQuatChannelQuat(s3));
    // Swap out invalid entries for the identity.  This is essential because otherwise the invalid data
    // propagates into the accumulated transforms higher up the chain
    NMP::vpu::Vector3MP zeroPosMP(NMP::vpu::zero4f(), NMP::vpu::zero4f(), NMP::vpu::zero4f());
    sourcePosMP = sourcePosMP.merge(zeroPosMP, validMask);
    NMP::vpu::QuatMP identityQuatMP(NMP::vpu::zero4f(), NMP::vpu::zero4f(), NMP::vpu::zero4f(), NMP::vpu::one4f());
    sourceQuatMP = sourceQuatMP.merge(identityQuatMP, validMask);

    //----------------------------------------------------------------------------------------------------------------------
    // Do the Retarget

    // First apply offsets to get the offset joints for the source and place in the target
    sourcePosMP +=  m_mpSourceGeom[pi].offset.t;
    sourceQuatMP = m_mpSourceGeom[pi].offset.q * sourceQuatMP;

    // Apply rig scale
    sourcePosMP *= m_sourceInverseRigScale;

    // Now move the joints into the parent reference frame
    //  This a post-multiply-inverse
    sourceQuatMP = m_mpSourceGeom[pi].parentReference.q.conjugate() * sourceQuatMP;
    sourcePosMP =
      m_mpSourceGeom[pi].parentReference.q.inverseRotateVector(sourcePosMP - m_mpSourceGeom[pi].parentReference.t);
    // Now do a local transform into the reference frame
    //  This is a pre-multiply
    sourcePosMP += sourceQuatMP.rotateVector(m_mpSourceGeom[pi].reference.t);
    sourceQuatMP = sourceQuatMP * m_mpSourceGeom[pi].reference.q;

    //----------------------------------------------------------------------------------------------------------------------
    // Mirror where appropriate

    MPMirrorMasks& mirrorMasks = m_mpParams[pi].sourceMirrorMasks;
    sourceQuatMP.x = vNeg(sourceQuatMP.x, mirrorMasks.mirrorXRotation);
    sourceQuatMP.y = vNeg(sourceQuatMP.y, mirrorMasks.mirrorYRotation);
    sourceQuatMP.z = vNeg(sourceQuatMP.z, mirrorMasks.mirrorZRotation);
    sourcePosMP.x = vNeg(sourcePosMP.x, mirrorMasks.mirrorXTranslation);
    sourcePosMP.y = vNeg(sourcePosMP.y, mirrorMasks.mirrorYTranslation);
    sourcePosMP.z = vNeg(sourcePosMP.z, mirrorMasks.mirrorZTranslation);

  } // End of loop through package indices

  LOW_LEVEL_PROFILE_END();

} // end of FKRetarget::Solver::retargetSource()

//----------------------------------------------------------------------------------------------------------------------
void FKRetarget::Solver::accumulateSource(MPTransform* mpSourceTransforms)
{
  LOW_LEVEL_PROFILE_BEGIN("accumulateSource");

  for (uint32_t pi = m_mpFirstNToMPackageInSource; pi < m_mpSourceLength; ++pi)
  {
    // Ignore packages that are not the root of the sequence, or are the end of the sequence
    if (m_mpSourceMappings[pi].rootPackage == (signed)pi && m_mpSourceMappings[pi].endPackage != (signed)pi)
    {
      // Accumulate from root to end effector to get each bunch of transforms in the space of the
      // parent of the root-most in the sequence
      int32_t childIndex = m_mpSourceMappings[pi].childPackage;
      int32_t parentIndex = pi;
      while (childIndex >= 0)
      {
        // tchild = qparent*tchild + tparent
        mpSourceTransforms[childIndex].t =
          mpSourceTransforms[parentIndex].q.rotateVector(mpSourceTransforms[childIndex].t) +
          mpSourceTransforms[parentIndex].t;
        // qchild = qparent * qchild
        mpSourceTransforms[childIndex].q =
          mpSourceTransforms[parentIndex].q * mpSourceTransforms[childIndex].q;
        // Must normalise quats when accumulated due to numerical drift
        mpSourceTransforms[childIndex].q.normalise();

        // Move up sequence
        parentIndex = childIndex;
        childIndex = m_mpSourceMappings[parentIndex].childPackage;
      }
    }
  }

  LOW_LEVEL_PROFILE_END();
}

//----------------------------------------------------------------------------------------------------------------------
void FKRetarget::Solver::unpackSource(NMP::DataBuffer* workingGeom, MPTransform* mpSourceTransforms)
{
  LOW_LEVEL_PROFILE_BEGIN("unpackSource");

  for (uint32_t pi = m_mpFirstNToMPackageInSource; pi < m_mpSourceLength; ++pi)
  {
    // Handy aliases for valid flags
    const bool& v0 = m_mpSourceMappings[pi].valid[0];
    const bool& v1 = m_mpSourceMappings[pi].valid[1];
    const bool& v2 = m_mpSourceMappings[pi].valid[2];
    const bool& v3 = m_mpSourceMappings[pi].valid[3];

    // Handy aliases for the source joint indices
    const int32_t& s0 = m_mpSourceMappings[pi].jointIndex[0];
    const int32_t& s1 = m_mpSourceMappings[pi].jointIndex[1];
    const int32_t& s2 = m_mpSourceMappings[pi].jointIndex[2];
    const int32_t& s3 = m_mpSourceMappings[pi].jointIndex[3];

    // We shouldn't have packed any unmapped source joints
    NMP_ASSERT(s0 >= 0);
    NMP_ASSERT(s1 >= 0);
    NMP_ASSERT(s2 >= 0);
    NMP_ASSERT(s3 >= 0);

    // Unpack the transforms from this package, or dump
    NMP::vpu::vector4_t dumpVec[4];
    mpSourceTransforms[pi].t.unpack(
      v0 ? *(NMP::vpu::vector4_t*)workingGeom->getPosQuatChannelPos(s0) : dumpVec[0],
      v1 ? *(NMP::vpu::vector4_t*)workingGeom->getPosQuatChannelPos(s1) : dumpVec[1],
      v2 ? *(NMP::vpu::vector4_t*)workingGeom->getPosQuatChannelPos(s2) : dumpVec[2],
      v3 ? *(NMP::vpu::vector4_t*)workingGeom->getPosQuatChannelPos(s3) : dumpVec[3]);
    mpSourceTransforms[pi].q.unpack(
      v0 ? *(NMP::vpu::vector4_t*)workingGeom->getPosQuatChannelQuat(s0) : dumpVec[0],
      v1 ? *(NMP::vpu::vector4_t*)workingGeom->getPosQuatChannelQuat(s1) : dumpVec[1],
      v2 ? *(NMP::vpu::vector4_t*)workingGeom->getPosQuatChannelQuat(s2) : dumpVec[2],
      v3 ? *(NMP::vpu::vector4_t*)workingGeom->getPosQuatChannelQuat(s3) : dumpVec[3]);
  }

  LOW_LEVEL_PROFILE_END();
}

//----------------------------------------------------------------------------------------------------------------------
void FKRetarget::Solver::interpolateSequences(
  const NMP::DataBuffer* sourceTransforms,
  NMP::DataBuffer* targetTransforms,
  NMP::DataBuffer* workingGeom,
  MPTransform* mpSourceTransforms,
  MPTransform* mpTargetTransforms)
{
  LOW_LEVEL_PROFILE_BEGIN("interpolateSequences");

  // Unpack source geometry and re-pack into blend locations for the target geometry
  for (uint32_t pi = 0; pi < m_mpTargetLength; pi = m_mpTargetMappings[pi].rootPackage + 1)
  {
    // The way we are organising this loop is that we are going to go through the sequences
    // internally, so at this stage we should only be looking at packages that are at the end
    // of the sequence
    NMP_ASSERT(m_mpTargetMappings[pi].endPackage == (signed)pi);

    // Handy aliases for valid flags, joint indices, intermediate joint indices, and source sequence
    // end joint indices
    bool* v = m_mpTargetMappings[pi].valid; // Valid
    int32_t* t = m_mpTargetMappings[pi].jointIndex; // Target joint index
    int32_t* s = m_mpTargetMappings[pi].jointPartnerBlendTo; // Source sequence end joint index

    // Decide if these output channels are used, based on the input
    for (uint32_t el = 0; el < 4; ++el)
    {
      if (v[el])
      {
        targetTransforms->setChannelUsedFlag(t[el], sourceTransforms->isFull() || sourceTransforms->hasChannel(s[el]));
      }
    }

    // For 1-to-1 mappings, copy packaged transforms across directly and skip on
    if (m_mpTargetMappings[pi].hasDirectMapping)
    {
      mpTargetTransforms[pi] = mpSourceTransforms[pi];

#ifdef NM_DEBUG
      {
        NMP::Quat q[4];
        mpTargetTransforms[pi].q.unpack(
          *(NMP::vpu::vector4_t*)(&q[0]),
          *(NMP::vpu::vector4_t*)(&q[1]),
          *(NMP::vpu::vector4_t*)(&q[2]),
          *(NMP::vpu::vector4_t*)(&q[3]));
        for (uint32_t el = 0; el < 4; ++el)
        {
          if (m_mpTargetMappings[pi].valid[el] && targetTransforms->hasChannel(t[el]))
          {
            NMP_ASSERT(ISFINITE_QUAT(q[el]));
            NMP_ASSERT(ISNORMAL_QUAT(q[el]));
          }
        }
      }
#endif

      continue;
    }

    //----------------------------------------------------------------------------------------------------------------------
    // Go through the package sequence, interpolating each in turn.

    // For the end joint in the sequence, just pack the source geometry straight in without interpolation
    // because they always map directly
    mpTargetTransforms[pi].t.pack(
      *(NMP::vpu::vector4_t*)workingGeom->getPosQuatChannelPos(s[0]),
      *(NMP::vpu::vector4_t*)workingGeom->getPosQuatChannelPos(s[1]),
      *(NMP::vpu::vector4_t*)workingGeom->getPosQuatChannelPos(s[2]),
      *(NMP::vpu::vector4_t*)workingGeom->getPosQuatChannelPos(s[3]));
    mpTargetTransforms[pi].q.pack(
      *(NMP::vpu::vector4_t*)workingGeom->getPosQuatChannelQuat(s[0]),
      *(NMP::vpu::vector4_t*)workingGeom->getPosQuatChannelQuat(s[1]),
      *(NMP::vpu::vector4_t*)workingGeom->getPosQuatChannelQuat(s[2]),
      *(NMP::vpu::vector4_t*)workingGeom->getPosQuatChannelQuat(s[3]));

    // Now move through the sequence from the end downwards to the root
    int32_t localPackageIndex = m_mpTargetMappings[pi].parentPackage;
    while (localPackageIndex >= 0)
    {
      // Get the begin and end source joint indices for the source sequence, plus the blend weights
      const int32_t* siBlendFrom = m_mpTargetMappings[localPackageIndex].jointPartnerBlendFrom;
      const int32_t* siBlendTo = m_mpTargetMappings[localPackageIndex].jointPartnerBlendTo;
      const float* weights = m_mpTargetMappings[localPackageIndex].blendWeights;

      // Note invalid indices in masks
      const bool* validFrom = m_mpTargetMappings[localPackageIndex].blendFromValid;
      const bool* valid = m_mpTargetMappings[localPackageIndex].valid;
      const vector4& validMask = m_mpTargetGeom[localPackageIndex].validMask;
      NMP::vpu::vector4_t validFromMask = NMP::vpu::and4(validMask,
        NMP::vpu::mask4i(validFrom[0], validFrom[1], validFrom[2], validFrom[3]));

#ifdef NM_DEBUG
      {
        NMP::Quat q[4];
        for (uint32_t el = 0; el < 4; ++el)
        {
          NMP::Quat* qp = workingGeom->getPosQuatChannelQuat(siBlendTo[el]);
          q[el] = *workingGeom->getPosQuatChannelQuat(siBlendTo[el]);
          if (valid[el] && siBlendTo[el] > 0 && sourceTransforms->hasChannel(siBlendTo[el]))
          {
            NMP_ASSERT(ISFINITE_VECTOR3(*workingGeom->getPosQuatChannelPos(siBlendTo[el])));
            NMP_ASSERT(ISFINITE_QUAT(q[el]));
            NMP_ASSERT(ISNORMAL_QUAT(q[el]));
          }
          (void)qp;
        }
      }
      {
        NMP::Quat q[4];
        for (uint32_t el = 0; el < 4; ++el)
        {
          NMP::Quat* qp = workingGeom->getPosQuatChannelQuat(siBlendFrom[el]);
          q[el] = *workingGeom->getPosQuatChannelQuat(siBlendFrom[el]);
          if (valid[el] && validFrom[el] && siBlendFrom[el] > 0 && sourceTransforms->hasChannel(siBlendFrom[el]))
          {
            NMP_ASSERT(ISFINITE_VECTOR3(*workingGeom->getPosQuatChannelPos(siBlendFrom[el])));
            NMP_ASSERT(ISFINITE_QUAT(q[el]));
            NMP_ASSERT(ISNORMAL_QUAT(q[el]));
          }
          (void)qp;
        }
      }
#endif

      // Pack the begin and end source transforms now we know where they are
      MPTransform targetBlendEnd;
      targetBlendEnd.t.pack(
        *(NMP::vpu::vector4_t*)workingGeom->getPosQuatChannelPos(siBlendTo[0]),
        *(NMP::vpu::vector4_t*)workingGeom->getPosQuatChannelPos(siBlendTo[1]),
        *(NMP::vpu::vector4_t*)workingGeom->getPosQuatChannelPos(siBlendTo[2]),
        *(NMP::vpu::vector4_t*)workingGeom->getPosQuatChannelPos(siBlendTo[3]));
      targetBlendEnd.q.pack(
        *(NMP::vpu::vector4_t*)workingGeom->getPosQuatChannelQuat(siBlendTo[0]),
        *(NMP::vpu::vector4_t*)workingGeom->getPosQuatChannelQuat(siBlendTo[1]),
        *(NMP::vpu::vector4_t*)workingGeom->getPosQuatChannelQuat(siBlendTo[2]),
        *(NMP::vpu::vector4_t*)workingGeom->getPosQuatChannelQuat(siBlendTo[3]));
      MPTransform targetBlendStart;
      targetBlendStart.t.pack(
        *(NMP::vpu::vector4_t*)workingGeom->getPosQuatChannelPos(siBlendFrom[0]),
        *(NMP::vpu::vector4_t*)workingGeom->getPosQuatChannelPos(siBlendFrom[1]),
        *(NMP::vpu::vector4_t*)workingGeom->getPosQuatChannelPos(siBlendFrom[2]),
        *(NMP::vpu::vector4_t*)workingGeom->getPosQuatChannelPos(siBlendFrom[3]));
      targetBlendStart.q.pack(
        *(NMP::vpu::vector4_t*)workingGeom->getPosQuatChannelQuat(siBlendFrom[0]),
        *(NMP::vpu::vector4_t*)workingGeom->getPosQuatChannelQuat(siBlendFrom[1]),
        *(NMP::vpu::vector4_t*)workingGeom->getPosQuatChannelQuat(siBlendFrom[2]),
        *(NMP::vpu::vector4_t*)workingGeom->getPosQuatChannelQuat(siBlendFrom[3]));

      // Replace invalid start transforms with the identity
      //  translations
      NMP::vpu::Vector3MP zeroPosMP(NMP::vpu::zero4f(), NMP::vpu::zero4f(), NMP::vpu::zero4f());
      targetBlendStart.t = targetBlendStart.t.merge(zeroPosMP, validFromMask);
      //  rotations
      NMP::vpu::QuatMP identityQuatMP(NMP::vpu::zero4f(), NMP::vpu::zero4f(), NMP::vpu::zero4f(), NMP::vpu::one4f());
      targetBlendStart.q = targetBlendStart.q.merge(identityQuatMP, validFromMask);
      //
      // Replace invalid end transforms with the identity
      //  translations
      targetBlendEnd.t = targetBlendEnd.t.merge(zeroPosMP, validMask);
      //  rotations
      targetBlendEnd.q = targetBlendEnd.q.merge(identityQuatMP, validMask);

      //----------------------------------------------------------------------------------------------------------------------
      // Interpolate to get the target geometry
      NMP::vpu::vector4_t weightsVec = NMP::vpu::set4f(weights[0], weights[1], weights[2], weights[3]);
      mpTargetTransforms[localPackageIndex].t.lerp(
        targetBlendStart.t, targetBlendEnd.t, weightsVec);
      mpTargetTransforms[localPackageIndex].q.interpBlend(
        targetBlendStart.q, targetBlendEnd.q, weightsVec);

      // Set channel used flags for rest of sequence
      for (uint32_t el = 0; el < 4; ++el)
      {
        if (valid[el])
        {
          targetTransforms->setChannelUsedFlag(
            m_mpTargetMappings[localPackageIndex].jointIndex[el],
            (sourceTransforms->isFull() || sourceTransforms->hasChannel(siBlendTo[el])) &&
            (!validFrom[el] || sourceTransforms->isFull() || sourceTransforms->hasChannel(siBlendFrom[el])));
        }
      }

      // Check result
#ifdef NM_DEBUG
      {
        NMP::Vector3 temp[4];
        mpTargetTransforms[localPackageIndex].t.unpack(
          *(NMP::vpu::vector4_t*)(&temp[0]),
          *(NMP::vpu::vector4_t*)(&temp[1]),
          *(NMP::vpu::vector4_t*)(&temp[2]),
          *(NMP::vpu::vector4_t*)(&temp[3]));
        NMP::Quat q[4];
        mpTargetTransforms[localPackageIndex].q.unpack(
          *(NMP::vpu::vector4_t*)(&q[0]),
          *(NMP::vpu::vector4_t*)(&q[1]),
          *(NMP::vpu::vector4_t*)(&q[2]),
          *(NMP::vpu::vector4_t*)(&q[3]));
        for (uint32_t el = 0; el < 4; ++el)
        {
          int32_t tel = m_mpTargetMappings[localPackageIndex].jointIndex[el];
          if (tel > 0 && m_mpTargetMappings[localPackageIndex].valid[el] && targetTransforms->hasChannel(tel))
          {
            NMP_ASSERT(ISVALID_TM(temp[el], q[el]));
          }
        }
      }
#endif

      // Move to parent
      localPackageIndex = m_mpTargetMappings[localPackageIndex].parentPackage;
    }
  }

  // We now know all the channel Used statuses, so we can set the Full flag
  targetTransforms->setFullFlag(targetTransforms->getUsedFlags()->calculateAreAllSet());

  LOW_LEVEL_PROFILE_END();
}

//----------------------------------------------------------------------------------------------------------------------
void FKRetarget::Solver::unaccumulateTarget(NMP::DataBuffer* NMIK_DEBUG_ONLY_PARAM(targetTransforms),
  MPTransform* mpTargetTransforms)
{
  LOW_LEVEL_PROFILE_BEGIN("unaccumulateTarget");

  for (uint32_t pi = 0; pi < m_mpTargetLength; ++pi)
  {
#ifdef NM_DEBUG
      {
        NMP::Vector3 t[4];
        mpTargetTransforms[pi].t.unpack(
          *(NMP::vpu::vector4_t*)(&t[0]),
          *(NMP::vpu::vector4_t*)(&t[1]),
          *(NMP::vpu::vector4_t*)(&t[2]),
          *(NMP::vpu::vector4_t*)(&t[3]));
        NMP::Quat q[4];
        mpTargetTransforms[pi].q.unpack(
          *(NMP::vpu::vector4_t*)(&q[0]),
          *(NMP::vpu::vector4_t*)(&q[1]),
          *(NMP::vpu::vector4_t*)(&q[2]),
          *(NMP::vpu::vector4_t*)(&q[3]));
        for (uint32_t el = 0; el < 4; ++el)
        {
          int32_t tel = m_mpTargetMappings[pi].jointIndex[el];
          if (tel > 0 && m_mpTargetMappings[pi].valid[el] && targetTransforms->hasChannel(tel))
          {
            NMP_ASSERT(ISVALID_TM(t[el], q[el]));
          }
        }
      }
#endif

    // Ignore packages that are not the end of the sequence, or are the root of the sequence
    if (m_mpTargetMappings[pi].rootPackage != (signed)pi && m_mpTargetMappings[pi].endPackage == (signed)pi)
    {
      // Unaccumulate from end effector to root to get each bunch of transforms in the correct space
      // local to its parent
      int32_t childIndex = pi;
      int32_t parentIndex = m_mpTargetMappings[pi].parentPackage;
      while (parentIndex >= 0)
      {
        // qchild = ~qparent * qchild
        mpTargetTransforms[childIndex].q =
          mpTargetTransforms[parentIndex].q.conjugate() * mpTargetTransforms[childIndex].q;
        // Must normalise quats when unaccumulating due to numerical drift
        mpTargetTransforms[childIndex].q.normalise();
        // tchild = ~qparent * (tchild - tparent)
        mpTargetTransforms[childIndex].t =
          mpTargetTransforms[parentIndex].q.inverseRotateVector(
            mpTargetTransforms[childIndex].t - mpTargetTransforms[parentIndex].t);

        // Check result
#ifdef NM_DEBUG
        {
          NMP::Vector3 t[4];
          mpTargetTransforms[childIndex].t.unpack(
            *(NMP::vpu::vector4_t*)(&t[0]),
            *(NMP::vpu::vector4_t*)(&t[1]),
            *(NMP::vpu::vector4_t*)(&t[2]),
            *(NMP::vpu::vector4_t*)(&t[3]));
          NMP::Quat q[4];
          mpTargetTransforms[childIndex].q.unpack(
            *(NMP::vpu::vector4_t*)(&q[0]),
            *(NMP::vpu::vector4_t*)(&q[1]),
            *(NMP::vpu::vector4_t*)(&q[2]),
            *(NMP::vpu::vector4_t*)(&q[3]));
          for (uint32_t el = 0; el < 4; ++el)
          {
            int32_t tel = m_mpTargetMappings[childIndex].jointIndex[el];
            if (tel > 0 && m_mpTargetMappings[childIndex].valid[el] && targetTransforms->hasChannel(tel))
            {
              NMP_ASSERT(ISVALID_TM(t[el], q[el]));
            }
          }
        }
#endif

        // Move down sequence
        childIndex = parentIndex;
        parentIndex = m_mpTargetMappings[childIndex].parentPackage;
      }
    } // Restrict to sequences only

  } // for loop through packages

  LOW_LEVEL_PROFILE_END();
}

//----------------------------------------------------------------------------------------------------------------------
void FKRetarget::Solver::retargetTarget(
  NMP::DataBuffer* targetTransforms,
  MPTransform* mpSourceTransforms,
  MPTransform* mpTargetTransforms)
{
  NMP_ASSERT(targetTransforms);

  LOW_LEVEL_PROFILE_BEGIN("retargetTarget");

  //------------------------------------------------------------------------------------------------------------------
  // Retarget and weight
  for (uint32_t pi = 0; pi < m_mpTargetLength; ++pi)
  {
    // Handy aliases for the joint indices
    const int32_t& t0 = m_mpTargetMappings[pi].jointIndex[0];
    const int32_t& t1 = m_mpTargetMappings[pi].jointIndex[1];
    const int32_t& t2 = m_mpTargetMappings[pi].jointIndex[2];
    const int32_t& t3 = m_mpTargetMappings[pi].jointIndex[3];

    // Target input geom - the zero mark for weighting
    // We are storing this in the source geometry array because we don't need that any more
    // at this stage of retargeting, and it is a sensible reuse of allocated memory
    MPTransform& targetTransformIn = mpSourceTransforms[pi];
    targetTransformIn.t.pack(
      *(NMP::vpu::vector4_t*)targetTransforms->getPosQuatChannelPos(t0),
      *(NMP::vpu::vector4_t*)targetTransforms->getPosQuatChannelPos(t1),
      *(NMP::vpu::vector4_t*)targetTransforms->getPosQuatChannelPos(t2),
      *(NMP::vpu::vector4_t*)targetTransforms->getPosQuatChannelPos(t3));
    targetTransformIn.q.pack(
      *(NMP::vpu::vector4_t*)targetTransforms->getPosQuatChannelQuat(t0),
      *(NMP::vpu::vector4_t*)targetTransforms->getPosQuatChannelQuat(t1),
      *(NMP::vpu::vector4_t*)targetTransforms->getPosQuatChannelQuat(t2),
      *(NMP::vpu::vector4_t*)targetTransforms->getPosQuatChannelQuat(t3));

    NMP::vpu::Vector3MP& targetPosMP = mpTargetTransforms[pi].t;
    NMP::vpu::QuatMP& targetQuatMP = mpTargetTransforms[pi].q;

    //----------------------------------------------------------------------------------------------------------------------
    // Retarget

    // Do a local transform from the target reference frame into the target offset frame
    //  This is a pre-multiply-inverse
    targetQuatMP = targetQuatMP * m_mpTargetGeom[pi].reference.q.conjugate();
    targetPosMP -= targetQuatMP.rotateVector(m_mpTargetGeom[pi].reference.t);
    // And move the joint back out of the parent reference frame into the parent frame
    //  This is a post-multiply
    targetQuatMP = m_mpTargetGeom[pi].parentReference.q * targetQuatMP;
    targetPosMP =
      m_mpTargetGeom[pi].parentReference.q.rotateVector(targetPosMP) + m_mpTargetGeom[pi].parentReference.t;

    // Un-apply rig scale
    targetPosMP *= m_targetRigScale;

    // Now un-apply target offsets to get the target joint
    targetPosMP -= m_mpTargetGeom[pi].offset.t;
    targetQuatMP = m_mpTargetGeom[pi].offset.q.conjugate() * targetQuatMP;

    //----------------------------------------------------------------------------------------------------------------------
    // Mirror where appropriate

    MPMirrorMasks& mirrorMasks = m_mpParams[pi].targetMirrorMasks;
    targetQuatMP.x = vNeg(targetQuatMP.x, mirrorMasks.mirrorXRotation);
    targetQuatMP.y = vNeg(targetQuatMP.y, mirrorMasks.mirrorYRotation);
    targetQuatMP.z = vNeg(targetQuatMP.z, mirrorMasks.mirrorZRotation);
    targetPosMP.x = vNeg(targetPosMP.x, mirrorMasks.mirrorXTranslation);
    targetPosMP.y = vNeg(targetPosMP.y, mirrorMasks.mirrorYTranslation);
    targetPosMP.z = vNeg(targetPosMP.z, mirrorMasks.mirrorZTranslation);

    //----------------------------------------------------------------------------------------------------------------------
    // Apply joint weights

    // OPTIMISE - we could avoid testing this condition for every package by moving the condition outside the
    // loop, but this would probably require re-implementing this function.
    if (m_applyJointWeights)
    {
      if (!m_mpTargetMappings[pi].allUnitPositionWeights)
      {
        NMP::vpu::vector4_t positionWeights = NMP::vpu::set4f(
          m_mpTargetMappings[pi].positionWeights[0],
          m_mpTargetMappings[pi].positionWeights[1],
          m_mpTargetMappings[pi].positionWeights[2],
          m_mpTargetMappings[pi].positionWeights[3]);
        targetPosMP.lerp(targetTransformIn.t, targetPosMP, positionWeights);
      }
      if (!m_mpTargetMappings[pi].allUnitRotationWeights)
      {
        NMP::vpu::vector4_t rotationWeights = NMP::vpu::set4f(
          m_mpTargetMappings[pi].rotationWeights[0],
          m_mpTargetMappings[pi].rotationWeights[1],
          m_mpTargetMappings[pi].rotationWeights[2],
          m_mpTargetMappings[pi].rotationWeights[3]);
        targetQuatMP.interpBlend(targetTransformIn.q, targetQuatMP, rotationWeights);
      }
    }
  }

  //------------------------------------------------------------------------------------------------------------------
  // Clamp and redistribute
  for (uint32_t pi = 0; pi < m_mpTargetLength; ++pi)
  {
    NMP::vpu::QuatMP& targetQuatMP = mpTargetTransforms[pi].q;

    if (m_mpParams[pi].isLimited)
    {
      // Clamp
      NMP::vpu::QuatMP qClamped = targetQuatMP;
      JointLimitsMP::clamp(m_mpParams[pi].limits, qClamped);
      // Mask out unlimited joints
      qClamped = qClamped.merge(targetQuatMP, m_mpParams[pi].applyLimitsMask);

      // Redistribute clamped-off motion
      const int32_t& ppi = m_mpTargetMappings[pi].parentPackage;
      if (m_mpParams[pi].doRedistribution)
      {
        // Redistribute down chain
        if (ppi >= 0)
        {
          // To redistribute motion down the chain, we only need modify parent transforms, because
          // the sequence is ordered in hierarchy order
          NMP::vpu::QuatMP qRedistribute = targetQuatMP * qClamped.conjugate();

          // Apply to the parent where appropriate
          NMP::vpu::QuatMP newParentQ = mpTargetTransforms[ppi].q * qRedistribute;
          mpTargetTransforms[ppi].q =
            newParentQ.merge(mpTargetTransforms[ppi].q, m_mpParams[pi].redistributeMask);
        }
        // If this is the root package, redistribute up the chain
        else
        {
          // For each child package, apply redistributed motion, reclamp and continue up.
          int32_t cpi = m_mpTargetMappings[pi].childPackage;
          const vector4& rootPackageMask = m_mpParams[pi].redistributeMask;
          vector4 parentPackageMask = NMP::vpu::and4(m_mpParams[pi].redistributeMask, m_mpTargetGeom[pi].validMask);
          NMP::vpu::QuatMP qRedistribute = qClamped.conjugate() * mpTargetTransforms[pi].q;
          NMP::vpu::QuatMP childQuatClamped = qClamped;
          while (cpi >= 0)
          {
            // Redistribute to child
            NMP::vpu::QuatMP& childQuatMP = mpTargetTransforms[cpi].q;
            NMP::vpu::QuatMP newChildQ = qRedistribute * childQuatMP;
            //  Mask out joints for which redistribution is not being applied
            newChildQ = newChildQ.merge(childQuatMP, NMP::vpu::and4(rootPackageMask, parentPackageMask));

            // Get clamped child and calculate redistributed motion
            childQuatClamped = newChildQ;
            JointLimitsMP::clamp(m_mpParams[cpi].limits, childQuatClamped);
            //  Mask out unlimited joints
            childQuatClamped = childQuatClamped.merge(newChildQ, m_mpParams[cpi].applyLimitsMask);
            //  Apply clamp to joint
            childQuatMP = childQuatClamped;
            //  New redistribution
            qRedistribute = childQuatClamped.conjugate() * newChildQ;

            // Move up
            parentPackageMask = NMP::vpu::and4(m_mpParams[cpi].redistributeMask, m_mpTargetGeom[cpi].validMask);
            cpi = m_mpTargetMappings[cpi].childPackage;
          }
        }
      }

      // Copy clamped result to output
      targetQuatMP = qClamped;
    }
  }

  //------------------------------------------------------------------------------------------------------------------
  // Unpack to output buffer
  for (uint32_t pi = 0; pi < m_mpTargetLength; ++pi)
  {
    // Handy aliases for the joint indices
    const int32_t& t0 = m_mpTargetMappings[pi].jointIndex[0];
    const int32_t& t1 = m_mpTargetMappings[pi].jointIndex[1];
    const int32_t& t2 = m_mpTargetMappings[pi].jointIndex[2];
    const int32_t& t3 = m_mpTargetMappings[pi].jointIndex[3];

    // Target input geom - we may keep this and discard the retarget result
    // Retrieve this from the source geometry where we stored it earlier.  Remember: we are just
    // using the source geometry as a handy block of allocated memory of sufficient size.
    MPTransform& targetTransformIn = mpSourceTransforms[pi];

    // Create a mask for valid entries
    bool isFull = targetTransforms->isFull();
    vector4 validMask = NMP::vpu::mask4i(
      m_mpTargetMappings[pi].valid[0] && (isFull || targetTransforms->hasChannel(t0)),
      m_mpTargetMappings[pi].valid[1] && (isFull || targetTransforms->hasChannel(t1)),
      m_mpTargetMappings[pi].valid[2] && (isFull || targetTransforms->hasChannel(t2)),
      m_mpTargetMappings[pi].valid[3] && (isFull || targetTransforms->hasChannel(t3)));

    NMP::vpu::Vector3MP& targetPosMP = mpTargetTransforms[pi].t;
    NMP::vpu::QuatMP& targetQuatMP = mpTargetTransforms[pi].q;

    //------------------------------------------------------------------------------------------------------------------
    // Unpack the result into the output
    //  Copy the start transforms so we can choose whether or not to replace them with the retargeted
    //  versions, based on whether channels are valid etc.
    //  Create output transform package which selects from the input or the retargeted value
    MPTransform targetTransformOut;
    //  Create mask for masking out channels that are not both valid and have non-rigid bones
    NMP::vpu::vector4_t mask = NMP::vpu::and4(validMask, m_mpParams[pi].isRigidMask);
    //  Select translations
    targetTransformOut.t = targetPosMP.merge(targetTransformIn.t, mask);
    //  Select rotations
    targetTransformOut.q = targetQuatMP.merge(targetTransformIn.q, validMask);
    //  Unpack into output buffer
    //  Avoid unpacking to the same address by using a temporary copy
    NMP::Vector3 targetOutPosTemp[4];
    targetTransformOut.t.unpack(
      *(NMP::vpu::vector4_t*)&targetOutPosTemp[0],
      *(NMP::vpu::vector4_t*)&targetOutPosTemp[1],
      *(NMP::vpu::vector4_t*)&targetOutPosTemp[2],
      *(NMP::vpu::vector4_t*)&targetOutPosTemp[3]);
    NMP::Quat targetOutQuatTemp[4];
    targetTransformOut.q.unpack(
      *(NMP::vpu::vector4_t*)&targetOutQuatTemp[0],
      *(NMP::vpu::vector4_t*)&targetOutQuatTemp[1],
      *(NMP::vpu::vector4_t*)&targetOutQuatTemp[2],
      *(NMP::vpu::vector4_t*)&targetOutQuatTemp[3]);
    targetTransforms->setPosQuatChannelPos(t0, targetOutPosTemp[0]);
    targetTransforms->setPosQuatChannelPos(t1, targetOutPosTemp[1]);
    targetTransforms->setPosQuatChannelPos(t2, targetOutPosTemp[2]);
    targetTransforms->setPosQuatChannelPos(t3, targetOutPosTemp[3]);
    targetTransforms->setPosQuatChannelQuat(t0, targetOutQuatTemp[0]);
    targetTransforms->setPosQuatChannelQuat(t1, targetOutQuatTemp[1]);
    targetTransforms->setPosQuatChannelQuat(t2, targetOutQuatTemp[2]);
    targetTransforms->setPosQuatChannelQuat(t3, targetOutQuatTemp[3]);

#ifdef NM_DEBUG
    {
      NMP::Quat q[4];
      targetTransformOut.q.unpack(
        *(NMP::vpu::vector4_t*)(&q[0]),
        *(NMP::vpu::vector4_t*)(&q[1]),
        *(NMP::vpu::vector4_t*)(&q[2]),
        *(NMP::vpu::vector4_t*)(&q[3]));
      NMP_ASSERT(t0 == 0 || !targetTransforms->hasChannel(t0) || ISNORMAL_QUAT(q[0]));
      NMP_ASSERT(t1 == 0 || !targetTransforms->hasChannel(t1) || ISNORMAL_QUAT(q[1]));
      NMP_ASSERT(t2 == 0 || !targetTransforms->hasChannel(t2) || ISNORMAL_QUAT(q[2]));
      NMP_ASSERT(t3 == 0 || !targetTransforms->hasChannel(t3) || ISNORMAL_QUAT(q[3]));
    }
#endif

  } // End of loop through package indices

  LOW_LEVEL_PROFILE_END();
}

#ifdef NMRU_FKRETARGET_VPU_N_TO_M
//----------------------------------------------------------------------------------------------------------------------
void FKRetarget::Solver::retarget(
  const NMP::DataBuffer* sourceTransforms,
  NMP::DataBuffer* targetTransforms,
  NMP::TempMemoryAllocator* tempAllocator)
{
  NMP_ASSERT(sourceTransforms);
  NMP_ASSERT(targetTransforms);
  NMP_ASSERT_MSG(tempAllocator, "FKRetarget::Solver::retarget requires a temp memory allocator!");

  NMIK_PROFILE_BEGIN("FKRetarget::Solver::retarget");

  // Create a working geometry to unpack into (note that subTaskRetargetTransforms creates a rollback point for the
  //  temp memory allocator so we don't need to do it here).
  NMP::DataBuffer *workingGeom = NULL;
  {
    NMP::Memory::Format workingGeomFormat = NMP::DataBuffer::getPosQuatMemoryRequirements(m_largestRigSize);
    NMP::Memory::Resource workingGeomResource = tempAllocator->allocateFromFormat(workingGeomFormat NMP_MEMORY_TRACKING_ARGS);

    workingGeom = NMP::DataBuffer::initPosQuat(workingGeomResource, workingGeomFormat, m_largestRigSize);
  }

  // Create source and destination multiplexed transforms
  MPTransform* mpSourceTransforms = NULL;
  MPTransform* mpTargetTransforms = NULL;
  {
    NMP::Memory::Format transformFormat(sizeof(MPTransform), NMP_VECTOR_ALIGNMENT);
    // Add one extra package to store identity matrices for defaults
    transformFormat *= m_mpMaxLength + 1;
    mpSourceTransforms = (MPTransform*)(tempAllocator->allocateFromFormat(transformFormat NMP_MEMORY_TRACKING_ARGS)).ptr;
    mpTargetTransforms = (MPTransform*)(tempAllocator->allocateFromFormat(transformFormat NMP_MEMORY_TRACKING_ARGS)).ptr;
    //  We actually want to point to the second member, so the first is index -1
    mpSourceTransforms++;
    mpTargetTransforms++;
  }

  // Convert source rig into source reference rig (or intermediate rig)
  retargetSource(sourceTransforms, mpSourceTransforms);

  // Put transforms of joints in sequences into the coordinate frame of their root-most joint's parent
  accumulateSource(mpSourceTransforms);

  // Take accumulated transforms and put them into a local data buffer so they can be repackaged
  unpackSource(workingGeom, mpSourceTransforms);

  // Interpolate the accumulated sequences to convert from the number of joints in the source sequence
  // to the number of joints in the target sequence
  interpolateSequences(sourceTransforms, targetTransforms, workingGeom, mpSourceTransforms, mpTargetTransforms);

  // Take the interpolated joint transforms back into the coordinate frame of their parent joints
  unaccumulateTarget(targetTransforms, mpTargetTransforms);

  // Convert target reference rig (or intermediate rig) into the target rig
  retargetTarget(targetTransforms, mpSourceTransforms, mpTargetTransforms);

#ifdef NM_DEBUG
  // Sanity check output
  {
    for (uint32_t j = 1; j < targetTransforms->getLength(); ++j)
    {
      if (targetTransforms->hasChannel(j))
      {
        NMP_ASSERT(ISFINITE_VECTOR3(*targetTransforms->getPosQuatChannelPos(j)));
        NMP_ASSERT(ISFINITE_QUAT(*targetTransforms->getPosQuatChannelQuat(j)));
        NMP_ASSERT(ISNORMAL_QUAT(*targetTransforms->getPosQuatChannelQuat(j)));
      }
    }
  }
#endif

  NMIK_PROFILE_END();
}

#else // !defined(NMRU_FKRETARGET_VPU_N_TO_M)
//----------------------------------------------------------------------------------------------------------------------
void FKRetarget::Solver::retarget(
  const Params* sourceParams,
  const NMP::DataBuffer* sourceTransforms,
  const NMP::Hierarchy* sourceHierarchy,
  const Params* targetParams,
  NMP::DataBuffer* targetTransforms,
  const NMP::Hierarchy* targetHierarchy)
{
  NMP_ASSERT(sourceParams);
  NMP_ASSERT(sourceTransforms);
  NMP_ASSERT(sourceHierarchy);
  NMP_ASSERT(targetParams);
  NMP_ASSERT(targetTransforms);
  NMP_ASSERT(targetHierarchy);

  NMIK_PROFILE_BEGIN("FKRetarget::Solver::retarget");

  NMIK_PROFILE_BEGIN("retarget_1_to_1");

  // Start by calling the 1-to-1 map function, for handling all the simple cases.  It is faster to handle the
  // 1-to-1 cases separately than to deal with them alongside the other mappings.
  retarget1to1(sourceParams, sourceTransforms, targetParams, targetTransforms);

  NMIK_PROFILE_END();
  NMIK_PROFILE_BEGIN("retarget_n_to_m");

  //----------------------------------------------------------------------------------------------------------------------
  // Handle n-to-m mapping (where n and m != 1)
  bool thereAreReferencedJoints = false;
  for (uint32_t i = 0; i < m_intermediateRigSize; ++i)
  {
    // Gather useful mapping information
    int32_t sourceJointIndex = m_intermediateRigData[i].sourceIndex;
    int32_t targetJointIndex = m_intermediateRigData[i].targetIndex;
    const uint32_t& sourceSequenceLength = m_intermediateRigData[i].sourceSequenceLength;
    const uint32_t& targetSequenceLength = m_intermediateRigData[i].targetSequenceLength;
    //
    // Ignore joints which
    //   - have no mapping for this particular pair of rigs
    //   - are 1-to-1 mappings - we've already dealt with those
    //   - have source channels that are not used
    if (sourceJointIndex < 0 || targetJointIndex < 0 ||
        (sourceSequenceLength == 1 && targetSequenceLength == 1) ||
        !sourceTransforms->hasChannel(sourceJointIndex))
    {
      continue;
    }
    //
    int32_t targetReferent = targetParams->jointReferenceIndex[targetJointIndex];
    int32_t ri = targetReferent >= 0 ? targetParams->indexMap[targetReferent] : -1;
    int32_t sourceReferent = ri >= 0 ? m_intermediateRigData[ri].sourceIndex : -1;
    bool isReferencedJoint =
      targetReferent != (signed)targetJointIndex &&
      targetReferent != targetHierarchy->getParentIndex(targetJointIndex) &&
      (sourceReferent >= 0 || targetReferent < 0);
    if (isReferencedJoint)
      thereAreReferencedJoints = true;
    //

    //----------------------------------------------------------------------------------------------------------------------
    // The algorithm takes much the same form as the one-to-one mapping, except that after calculating
    // the source reference rig for the sequence, we accumulate all the joints in the sequence to put
    // their transforms in the space of the sequence parent.  In this frame we can interpolate to calculate
    // the target reference rig for the sequence, convert back to local space and then back to the target rig.

    // Calculate the source reference rig for the sequence
    //   si means 'source index'.  It is the channel index of the current joint.
    //   ssqi means 'source sequence index'.  It is an index into the sequence rather than the transform buffer.
    //   Sequence indices are one-indexed to take account of interpolation between the root-most joint and
    //   it's parent, which will have an sequence position less than 1.
    int32_t si = sourceJointIndex;
    for (int32_t ssqi = sourceSequenceLength; ssqi > 0; --ssqi, si = sourceHierarchy->getParentIndex(si))
    {
      const PerJointParams& sourceJointParams = sourceParams->perJointParams[si];

      // Get the source transform as a PosQuat
      GeomUtils::PosQuat jointTM = {
        *sourceTransforms->getPosQuatChannelPos(si),
        *sourceTransforms->getPosQuatChannelQuat(si)
      };

      // First apply offsets to get the offset joint for the source
      jointTM.t += sourceJointParams.offsetTransform.t;
      jointTM.q = sourceJointParams.offsetTransform.q * jointTM.q;

      // Now move the joint into the parent reference frame
      int32_t sourceParentIndex = sourceHierarchy->getParentIndex(si);
      if (sourceParentIndex >= 0)
      {
        GeomUtils::multiplyInversePosQuat(
          jointTM, sourceParams->perJointParams[sourceParentIndex].referenceTransform);
      }
      // Now do a local transform into the reference frame
      GeomUtils::premultiplyPosQuat(sourceJointParams.referenceTransform, jointTM);

      // Copy the result into our temporary buffer for accumulation
      m_sourceAccumulatedGeom->setPosQuatChannelPos(ssqi, jointTM.t);
      m_sourceAccumulatedGeom->setPosQuatChannelQuat(ssqi, jointTM.q);
    }

    // Accumulate transforms to put everything in the coordinate frame of the sequence parent
    for (uint32_t ssqi = 2; ssqi <= sourceSequenceLength; ++ssqi)
    {
      NMP::Vector3& jointPos = *m_sourceAccumulatedGeom->getPosQuatChannelPos(ssqi);
      NMP::Quat& jointQuat = *m_sourceAccumulatedGeom->getPosQuatChannelQuat(ssqi);
      const NMP::Vector3& parentPos = *m_sourceAccumulatedGeom->getPosQuatChannelPos(ssqi - 1);
      const NMP::Quat& parentQuat = *m_sourceAccumulatedGeom->getPosQuatChannelQuat(ssqi - 1);

      jointPos = parentQuat.rotateVector(jointPos) + parentPos;
      jointQuat = parentQuat * jointQuat;
    }

    // For blending, put the identity into the zeroth source joint transform in the sequence
    m_sourceAccumulatedGeom->getPosQuatChannelPos(0)->setToZero();
    m_sourceAccumulatedGeom->getPosQuatChannelQuat(0)->identity();

    // Interpolate to get the target reference sequence in the coordinate frame of the sequence parent
    //   ti means 'target index'.  It is the channel index of the current joint.
    //   tsqi means 'target sequence index'.  It is an index into the sequence rather than the transform buffer.
    //   Sequence indices are one-indexed to take account of interpolation between the root-most joint and
    //   it's parent, which will have an sequence position less than 1.
    int32_t ti = targetJointIndex;
    //   First, copy the end-most transform straight across - it always matches
    m_targetAccumulatedGeom->setPosQuatChannelPos(
      ti, *m_sourceAccumulatedGeom->getPosQuatChannelPos(sourceSequenceLength));
    m_targetAccumulatedGeom->setPosQuatChannelQuat(
      ti, *m_sourceAccumulatedGeom->getPosQuatChannelQuat(sourceSequenceLength));
    //   Now interpolate the others
    ti = targetHierarchy->getParentIndex(ti);
    float invHighestIndex = 1.0f / (float)targetSequenceLength;
    for (int32_t tsqi = targetSequenceLength - 1; tsqi > 0; --tsqi, ti = targetHierarchy->getParentIndex(ti))
    {
      // Get the source sequence index of the joints either side, and the blend weight, for interpolation
      float sequenceFraction = tsqi * invHighestIndex;
      float sourceSequencePosition = sequenceFraction * sourceSequenceLength;
      int32_t ssqiLow = (int32_t)NMP::nmfloor(sourceSequencePosition);
      int32_t ssqiHigh = ssqiLow + 1;
      float blendWeight = sourceSequencePosition - ssqiLow;

      // Interpolate to get the target reference joint (accumulated)
      NMP::Vector3& jointPos = *m_targetAccumulatedGeom->getPosQuatChannelPos(ti);
      NMP::Quat& jointQuat = *m_targetAccumulatedGeom->getPosQuatChannelQuat(ti);
      jointPos.lerp(
        *m_sourceAccumulatedGeom->getPosQuatChannelPos(ssqiLow),
        *m_sourceAccumulatedGeom->getPosQuatChannelPos(ssqiHigh), blendWeight);
      jointQuat = *m_sourceAccumulatedGeom->getPosQuatChannelQuat(ssqiLow);
      NMRU::GeomUtils::shortestArcFastSlerp(
        jointQuat,
        *m_sourceAccumulatedGeom->getPosQuatChannelQuat(ssqiHigh), blendWeight);
    }

    // Un-accumulate back into the local space of the reference frame of each joint's parent
    ti = targetJointIndex;
    for (int32_t tsqi = targetSequenceLength; tsqi > 1; --tsqi, ti = targetHierarchy->getParentIndex(ti))
    {
      uint32_t targetParentIndex = targetHierarchy->getParentIndex(ti);

      NMP::Vector3& jointPos = *m_targetAccumulatedGeom->getPosQuatChannelPos(ti);
      NMP::Quat& jointQuat = *m_targetAccumulatedGeom->getPosQuatChannelQuat(ti);
      const NMP::Vector3& parentPos = *m_targetAccumulatedGeom->getPosQuatChannelPos(targetParentIndex);
      const NMP::Quat& parentQuat = *m_targetAccumulatedGeom->getPosQuatChannelQuat(targetParentIndex);

      jointPos = parentQuat.inverseRotateVector(jointPos - parentPos);
      jointQuat = ~parentQuat * jointQuat;

      bool breakpoint = true;
      (void)breakpoint;
    }

    // Calculate target sequence from target reference sequence
    ti = targetJointIndex;
    for (int32_t tsqi = targetSequenceLength; tsqi > 0; --tsqi, ti = targetHierarchy->getParentIndex(ti))
    {
      const PerJointParams& targetJointParams = targetParams->perJointParams[ti];

      // Get the target transform as a PosQuat
      GeomUtils::PosQuat jointTM = {
        *m_targetAccumulatedGeom->getPosQuatChannelPos(ti),
        *m_targetAccumulatedGeom->getPosQuatChannelQuat(ti)
      };

      // Do a local transform from the target reference frame into the target offset frame
      GeomUtils::premultiplyInversePosQuat(targetJointParams.referenceTransform, jointTM);
      // And move the joint back out of the parent reference frame into the parent frame
      int32_t targetParentIndex = targetHierarchy->getParentIndex(ti);
      if (targetParentIndex >= 0)
      {
        GeomUtils::multiplyPosQuat(
          jointTM, targetParams->perJointParams[targetParentIndex].referenceTransform);
      }

      // Now un-apply target offsets to get the target joint
      jointTM.t -= targetJointParams.offsetTransform.t;
      jointTM.q = ~targetJointParams.offsetTransform.q * jointTM.q;

      // Clamp
      if (targetJointParams.isLimited)
      {
        JointLimits::clamp(targetJointParams.limits, jointTM.q);
      }

      // And set in the output transforms again
      if (!targetJointParams.isRigid)
      {
        targetTransforms->setPosQuatChannelPos(ti, jointTM.t);
      }
      targetTransforms->setPosQuatChannelQuat(ti, jointTM.q);
    }

  } // end of loop through intermediate joints

  NMIK_PROFILE_END();

#ifdef NM_DEBUG
  // Sanity check output
  {
    for (uint32_t j = 1; j < targetTransforms->getLength(); ++j)
    {
      if (targetTransforms->hasChannel(j))
      {
        NMP_ASSERT(ISFINITE_VECTOR3(*targetTransforms->getPosQuatChannelPos(j)));
        NMP_ASSERT(ISFINITE_QUAT(*targetTransforms->getPosQuatChannelQuat(j)));
        NMP_ASSERT(ISNORMAL_QUAT(*targetTransforms->getPosQuatChannelQuat(j)));
      }
    }
  }
#endif

  NMIK_PROFILE_END();

} // end of FKRetarget::Solver::retarget() [n-to-m version]

#endif // defined(NMRU_FKRETARGET_VPU_N_TO_M)

//----------------------------------------------------------------------------------------------------------------------
void FKRetarget::Solver::prepare(
  const Params* sourceParams,
  uint32_t numSourceJoints,
  const NMP::Hierarchy* sourceHierarchy,
  const Params* targetParams,
  uint32_t numTargetJoints,
  const NMP::Hierarchy* targetHierarchy)
{
  // Clear the intermediate rig
  for (uint32_t j = 0; j < m_intermediateRigSize; ++j)
  {
    m_intermediateRigData[j].sourceIndex = -1;
    m_intermediateRigData[j].targetIndex = -1;
    m_intermediateRigData[j].sourceSequenceLength = 0;
    m_intermediateRigData[j].targetSequenceLength = 0;
    m_intermediateRigData[j].isRootJoint = true;
  }

  // Size checks
  // If these fail then we didn't pass in the right storage stats when initialising the solver.
  NMP_ASSERT(numSourceJoints <= m_largestRigSize);
  NMP_ASSERT(numTargetJoints <= m_largestRigSize);

  //----------------------------------------------------------------------------------------------------------------------
  // Index maps and sequence length counting

  // Set intermediate rig parameters for the source rig
  for (uint32_t j = 0; j < numSourceJoints; ++j)
  {
    int32_t intermediateRigIndex = sourceParams->indexMap[j];
    // If this fails we didn't pass in the correct storage stats when initialising the solver.
    NMP_ASSERT(intermediateRigIndex < (signed)m_intermediateRigSize);
    if (intermediateRigIndex == -1)
    {
      continue;
    }

    uint32_t sourceSequenceLength = 1;
    int32_t jointIndex = sourceHierarchy->getParentIndex(j);
    // Step up through hierarchy and find the length of the sequence.  If we've already
    // recorded a longer length then don't change the sequence end joint, otherwise do.
    while (jointIndex >= 0 && sourceParams->indexMap[jointIndex] >= 0 &&
           sourceParams->indexMap[jointIndex] == intermediateRigIndex)
    {
      ++sourceSequenceLength;
      jointIndex = sourceHierarchy->getParentIndex(jointIndex);
    }
    if (sourceSequenceLength > m_intermediateRigData[intermediateRigIndex].sourceSequenceLength)
    {
      m_intermediateRigData[intermediateRigIndex].sourceSequenceLength = sourceSequenceLength;
      m_intermediateRigData[intermediateRigIndex].sourceIndex = j;
    }
  }

  // Set intermediate rig parameters for the target rig
  for (uint32_t j = 0; j < numTargetJoints; ++j)
  {
    int32_t intermediateRigIndex = targetParams->indexMap[j];
    // If this fails we didn't pass in the correct storage stats when initialising the solver.
    NMP_ASSERT(intermediateRigIndex < (signed)m_intermediateRigSize);
    if (intermediateRigIndex == -1)
    {
      continue;
    }

    uint32_t targetSequenceLength = 1;
    int32_t jointIndex = targetHierarchy->getParentIndex(j);
    // Step up through hierarchy and find the length of the sequence.  If we've already
    // recorded a longer length then don't change the sequence end joint, otherwise do.
    while (jointIndex >= 0 && targetParams->indexMap[jointIndex] >= 0 &&
           targetParams->indexMap[jointIndex] == intermediateRigIndex)
    {
      ++targetSequenceLength;
      jointIndex = targetHierarchy->getParentIndex(jointIndex);
    }
    if (targetSequenceLength > m_intermediateRigData[intermediateRigIndex].targetSequenceLength)
    {
      m_intermediateRigData[intermediateRigIndex].targetSequenceLength = targetSequenceLength;
      m_intermediateRigData[intermediateRigIndex].targetIndex = j;
    }
  }

  //----------------------------------------------------------------------------------------------------------------------
  // Referenced joint roots
  // We need to find the 'master reference joints' the joints to which others are referenced that themselves
  // do not have any other reference joints as ancestor joints.  This is so we know where to in the hierarchy
  // we need to accumulate transforms when resolving referenced joints.
  for (uint32_t j = 0; j < numTargetJoints; ++j)
  {
    // Clear all roots between this joint and its reference
    int32_t ref = targetParams->jointReferenceIndex[j];
    int32_t jj = j;
    while (jj != ref && jj >= 0)
    {
      m_intermediateRigData[targetParams->indexMap[jj]].isRootJoint = false;
      jj = targetHierarchy->getParentIndex(jj);
    }
  }

  // Now clear any root joints for intermediate rig joints that /only/ map to the source rig, because
  // they may prevent correct transform accumulation
  for (uint32_t j = 0; j < m_intermediateRigSize; ++j)
  {
    if (m_intermediateRigData[j].targetIndex < 0)
      m_intermediateRigData[j].isRootJoint = false;
  }

  prepareMP(sourceParams, sourceHierarchy, targetParams, targetHierarchy);

} // end of FKRetarget::Solver::prepare()

//----------------------------------------------------------------------------------------------------------------------
void FKRetarget::Solver::prepareMappings(
  const Params* params,
  const NMP::Hierarchy* hierarchy,
  const Params* otherParams,
  const NMP::Hierarchy* otherHierarchy,
  bool isSource)
{
  //  Find the longest sequence in the rig
  uint32_t maxSequenceLength = 1;
  for (uint32_t i = 0; i < m_intermediateRigSize; ++i)
  {
    uint32_t sequenceLength = isSource ?
      m_intermediateRigData[i].sourceSequenceLength :
      m_intermediateRigData[i].targetSequenceLength;
    if (sequenceLength > maxSequenceLength)
    {
      maxSequenceLength = sequenceLength;
    }
  }

  //----------------------------------------------------------------------------------------------------------------------
  // Go through all the mapped joints and record their joint indices, sequence info and blend weights,
  // packing them in order of sequence length for the tightest packing.

  MPIndexMap* mappings = isSource ? m_mpSourceMappings : m_mpTargetMappings;
  uint32_t pi = 0; // Package index
  uint32_t el = 0; // Package element index (0 to 3)

  // Put valid indices into every package because some locations are not filled
  memset(mappings, 0, sizeof(MPIndexMap)*m_mpMaxLength);
  // Initialise the mapping data
  for (uint32_t i = 0; i < m_mpMaxLength; ++i)
  {
    for (uint32_t elI = 0; elI < 4; ++elI)
    {
      mappings[i].valid[elI] = false;
      mappings[i].parentValid[elI] = false;
      mappings[i].blendFromValid[elI] = false;
      // Ensure the interpolation info is valid, even for invalid joints
      mappings[i].jointPartnerBlendFrom[elI] = 0;
      mappings[i].jointPartnerBlendTo[elI] = 0;
      mappings[i].blendWeights[elI] = 0;
      mappings[i].positionWeights[elI] = 1.0f;
      mappings[i].rotationWeights[elI] = 1.0f;
    }
    mappings[i].hasDirectMapping = true;
    mappings[i].allUnitPositionWeights = true;
    mappings[i].allUnitRotationWeights = true;
  }
  // Set the first n-to-m package as too long to indicate that currently we are aware of no
  // non 1-to-1 mappings
  uint32_t* firstNToMPackage = isSource ? &m_mpFirstNToMPackageInSource : &m_mpFirstNToMPackageInTarget;
  *firstNToMPackage = m_mpMaxLength;
  // Set a flag to say we aren't aware of any joints that have non-unit rotation or position weights
  m_applyJointWeights = false;

  // Do this repeatedly for each sequence length of interest.
  // Use sequenceLength of zero to mark that we are interested in sequences of length 1 which
  // map to sequences of length 1 - these are special and must be packed together first
  int32_t highestPackageIndex = -1;
  for (uint32_t sequenceLength = 0; sequenceLength <= maxSequenceLength; ++sequenceLength)
  {
    uint32_t i = 0; // Intermediate rig index
    while (i < m_intermediateRigSize)
    {
      //----------------------------------------------------------------------------------------------------------------------
      // Get the next joint and its parent we are going to process in this package, as well as
      // the sequencing information
      const Params* targetParams = isSource ? otherParams : params;

      const int32_t& sourceJointIndex = m_intermediateRigData[i].sourceIndex;
      const int32_t& targetJointIndex = m_intermediateRigData[i].targetIndex;
      const uint32_t& sourceSequenceLength = m_intermediateRigData[i].sourceSequenceLength;
      const uint32_t& targetSequenceLength = m_intermediateRigData[i].targetSequenceLength;
      int32_t jointIndex = isSource ? sourceJointIndex : targetJointIndex;
      int32_t jointPartner = isSource ? targetJointIndex : sourceJointIndex;
      uint32_t thisSequenceLength = isSource ? sourceSequenceLength : targetSequenceLength;
      uint32_t partnerSequenceLength = isSource ? targetSequenceLength : sourceSequenceLength;

      // Store bias values for this sequence
      const float& bias = params->perJointParams[jointIndex].bias;
      const float& partnerBias = otherParams->perJointParams[jointPartner].bias;

      // Mark this sequence to be packed first if it is a 1-to-1 map.
      // Also n-to-n maps (same length sequences) since these are treated the same as 1-to-1s.
      // If the bias of the two sequences is different then we have to process them as an n-to-m map
      // because the different biases will mean that a target joint will not get its transform from
      // a single source joint - except for 1 to 1.
      // If the target sequence is marked for redistribution then we also need to treat it as n-to-m in
      // order to ensure it is packed as a sequence
      bool oneToOneMap =
        sourceSequenceLength == targetSequenceLength && (bias == partnerBias || sourceSequenceLength == 1);
      oneToOneMap = oneToOneMap &&
        !(targetSequenceLength > 1 && targetParams->perJointParams[jointIndex].redistributeClampedMotion);
      bool sequenceLengthMatches =
        (sequenceLength == 0 && oneToOneMap) || (sequenceLength == thisSequenceLength && !oneToOneMap);

      // We don't record channels where there is no valid mapping between rigs.
      // We are currently packing for a particular sequence length.
      if (sourceJointIndex >= 0 && targetJointIndex >= 0 && sequenceLengthMatches)
      {
        // We pack the first joint in the sequence into the first package, but we pack
        // its parent into the next package, and so on for the whole sequence.
        // EXCEPT in the case of n-to-n mappings, which we are just packing sequentially
        // into the elements of this package (or the next ones if more are needed).
        //
        uint32_t rootPackage = pi + thisSequenceLength - 1;
        uint32_t localPackageIndex = pi;
        int32_t childPackage = -1;
        //
        // Note:
        // sqi means sequence index, ji means joint index
        // The prefix j means this joint, p means partner joint
        int32_t jsqi = thisSequenceLength;
        float invSequenceLength = 1.0f / (float)thisSequenceLength;
        //
        do
        {
          // We know we're going to enter a mapping here, so we can update our maximum index
          highestPackageIndex = localPackageIndex;
          // If this fails then most likely the wrong storage stats were passed in when initialising
          // the solver, so not enough memory has been set aside for it.
          NMP_ASSERT(localPackageIndex < m_mpMaxLength);

          // Get a reference to the weights, because we need to refer back to them later
          float& positionWeight = mappings[localPackageIndex].positionWeights[el];
          float& rotationWeight = mappings[localPackageIndex].rotationWeights[el];
          // Store the current package index, which we need for setting time-saving flags later
          uint32_t thisPackageIndex = localPackageIndex;

          // It should be impossible to create sequences longer than the hierarchy allows,
          // given that we counted the sequence length using the hierarchy.
          NMP_ASSERT(jointIndex >= 0);
          NMP_ASSERT(jointPartner >= 0);

          mappings[localPackageIndex].jointIndex[el] = jointIndex;
          int32_t parentIndex = hierarchy->getParentIndex(jointIndex);
          mappings[localPackageIndex].jointParent[el] = NMP::maximum(parentIndex, (int32_t)0);

          // Set the valid flags
          mappings[localPackageIndex].valid[el] = true;
          mappings[localPackageIndex].parentValid[el] = parentIndex >= 0;

          // For n-to-n mappings with the same bias, treat each element of the sequence as an independent joint
          // and record sequence information and partner joint as if each were a 1-to-1 mapping.
          if (oneToOneMap)
          {
            // Record sequence information - i.e. there is no sequence
            mappings[localPackageIndex].childPackage = -1;
            mappings[localPackageIndex].parentPackage = -1;
            mappings[localPackageIndex].endPackage = pi;
            mappings[localPackageIndex].rootPackage = pi;

            // Get the interpolation info - for 1-to-1 maps everything is a direct mapping (and this
            // information will not be used anyway)
            mappings[localPackageIndex].jointPartnerBlendFrom[el] = jointPartner;
            mappings[localPackageIndex].jointPartnerBlendTo[el] = jointPartner;
            mappings[localPackageIndex].blendWeights[el] = 0;

            // Get weights
            positionWeight = GeomUtils::safelyDivide(params->perJointParams[jointIndex].positionWeight,
              otherParams->perJointParams[jointPartner].positionWeight);
            rotationWeight = GeomUtils::safelyDivide(params->perJointParams[jointIndex].rotationWeight,
              otherParams->perJointParams[jointPartner].rotationWeight);

            // Go to next element, and possibly package
            if (el == 3)
            {
              ++localPackageIndex;
              ++pi;
              el = 0;
            }
            else
            {
              ++el;
            }

            // Go up to next partner joint
            jointPartner = otherHierarchy->getParentIndex(jointPartner);
          }

          // For n-to-m mappings, record package sequence info and move through the packages.
          else
          {
            // Record sequence information
            mappings[localPackageIndex].childPackage = childPackage;
            mappings[localPackageIndex].parentPackage = localPackageIndex + 1;
            mappings[localPackageIndex].endPackage = pi;
            mappings[localPackageIndex].rootPackage = rootPackage;

            mappings[localPackageIndex].hasDirectMapping = false;
            *firstNToMPackage = NMP::minimum(*firstNToMPackage, localPackageIndex);

            // Get the interpolation info
            //
            // Get the begin and end sequence indices for the partner sequence, plus the blend weights
            float sequenceFraction = jsqi * invSequenceLength;
            // Apply bias for this joint to adjust sequenceFraction
            float& x = sequenceFraction;
            sequenceFraction = bias * x * x + (1.0f - bias) * x;
            // Inverse-apply bias for the partner joint (avoid zero bias)
            if (NMP::nmfabs(partnerBias) > NMRU::GeomUtils::nTol)
            {
              float& y = sequenceFraction;
              float twoBias = 2.0f * partnerBias;
              float oneMinusBias = 1.0f - partnerBias;
              sequenceFraction =
                (partnerBias - 1.0f + NMP::fastSqrt(oneMinusBias * oneMinusBias + 4.0f * partnerBias * y)) / twoBias;
            }
            float partnerSequencePosition = sequenceFraction * partnerSequenceLength;
            int32_t psqiFrom = (int32_t)NMP::nmfloor(partnerSequencePosition);
            int32_t psqiTo = psqiFrom + 1;
            mappings[localPackageIndex].blendWeights[el] = partnerSequencePosition - psqiFrom;
            //
            // Convert the partner sequence index into joint indices in the partner sequence, by traversing
            // the partner hierarchy.
            // OPTIMISE  This is rather inefficient because we traverse the hierarchy every time instead
            // of keeping markers as we progress through the sequence.
            int32_t& pjiFrom = mappings[localPackageIndex].jointPartnerBlendFrom[el];
            int32_t& pjiTo = mappings[localPackageIndex].jointPartnerBlendTo[el];
            pjiFrom = jointPartner;
            pjiTo = jointPartner;
            for (int32_t psqiLocal = partnerSequenceLength; psqiLocal >= psqiTo; --psqiLocal)
            {
              // Advance
              pjiTo = pjiFrom;
              pjiFrom = otherHierarchy->getParentIndex(pjiFrom);
              NMP_ASSERT(pjiTo >= 0); // Sanity check
            }
            //
            // Set valid flag for the blend from index.  It is invalid if it refers to sequence index 0,
            // or it's off the rig
            mappings[localPackageIndex].blendFromValid[el] = true;
            if (psqiFrom == 0 || pjiFrom < 0)
            {
              mappings[localPackageIndex].blendFromValid[el] = false;
              // Make sure the index recorded maps to something valid to avoid accessing illegal memory
              pjiFrom = 0;
            }

            // Get weights
            const float& w2 = mappings[localPackageIndex].blendWeights[el];
            float w1 = 1.0f - w2;
            positionWeight = GeomUtils::safelyDivide(params->perJointParams[jointIndex].positionWeight,
              (w1 * (mappings[localPackageIndex].blendFromValid[el] ?
              otherParams->perJointParams[pjiFrom].positionWeight : 1.0f) +
              w2 * otherParams->perJointParams[pjiTo].positionWeight));
            rotationWeight = GeomUtils::safelyDivide(params->perJointParams[jointIndex].rotationWeight,
              (w1 * (mappings[localPackageIndex].blendFromValid[el] ?
              otherParams->perJointParams[pjiFrom].rotationWeight : 1.0f) +
              w2 * otherParams->perJointParams[pjiTo].rotationWeight));

            // Go to next package
            childPackage = localPackageIndex;
            ++localPackageIndex;
          }

          // Update joint weight time saving flags
          if (positionWeight != 1.0f)
          {
            m_applyJointWeights = true;
            mappings[thisPackageIndex].allUnitPositionWeights = false;
          }
          if (rotationWeight != 1.0f)
          {
            m_applyJointWeights = true;
            mappings[thisPackageIndex].allUnitRotationWeights = false;
          }

          // Go to parent
          jointIndex = parentIndex;
          NMP_ASSERT(jsqi > 0);
          --jsqi;
        }
        while (--thisSequenceLength > 0);

        // If this was an n-to-m map I now need to move to the next element or package, as well as
        // doing some tidying of the sequence info.
        if (!oneToOneMap)
        {
          // Fix up sequence parent info at end of sequence
          mappings[localPackageIndex-1].parentPackage = -1;

          // Go to next element index, and possibly package, if we haven't already.
          if (el == 3)
          {
            pi = localPackageIndex;
            el = 0;
          }
          else
          {
            ++el;
          }
        }
      }

      // Go to next joint
      ++i;

    } // End of loop through rig for this particular sequence length

  } // End of loop through different sequence lengths

  // Record package array size
  if (isSource)
  {
    m_mpSourceLength = highestPackageIndex + 1;
    // If this fails then most likely the wrong storage stats were passed in when initialising
    // the solver, so not enough memory has been set aside for the mappings and other data.
    NMP_ASSERT(m_mpSourceLength <= m_mpMaxLength);
  }
  else
  {
    m_mpTargetLength = highestPackageIndex + 1;
    // If this fails then most likely the wrong storage stats were passed in when initialising
    // the solver, so not enough memory has been set aside for the mappings and other data.
    NMP_ASSERT(m_mpTargetLength <= m_mpMaxLength);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void FKRetarget::Solver::prepareGeom(
  const Params* params,
  bool isSource)
{
  MPIndexMap* mappings = isSource ? m_mpSourceMappings : m_mpTargetMappings;
  MPGeometry* geometry = isSource ? m_mpSourceGeom : m_mpTargetGeom;
  uint32_t length = isSource ? m_mpSourceLength : m_mpTargetLength;

  //----------------------------------------------------------------------------------------------------------------------
  // Go through all the packed indices of the source rig and pack the geometrical parameters
  // in the multiplex member variables
  for (uint32_t pi = 0; pi < length; ++pi)
  {
    // Get handy references to the joint indices
    const int32_t& i0 = mappings[pi].jointIndex[0];
    const int32_t& i1 = mappings[pi].jointIndex[1];
    const int32_t& i2 = mappings[pi].jointIndex[2];
    const int32_t& i3 = mappings[pi].jointIndex[3];
    const int32_t& p0 = mappings[pi].jointParent[0];
    const int32_t& p1 = mappings[pi].jointParent[1];
    const int32_t& p2 = mappings[pi].jointParent[2];
    const int32_t& p3 = mappings[pi].jointParent[3];

    // Create masks for valid entries
    vector4& validMask = geometry[pi].validMask;
    validMask = NMP::vpu::mask4i(
      mappings[pi].valid[0],
      mappings[pi].valid[1],
      mappings[pi].valid[2],
      mappings[pi].valid[3]
    );
    NMP::vpu::vector4_t parentValidMask = NMP::vpu::mask4i(
      mappings[pi].parentValid[0],
      mappings[pi].parentValid[1],
      mappings[pi].parentValid[2],
      mappings[pi].parentValid[3]
    );

    //----------------------------------------------------------------------------------------------------------------------
    // Pack the geometry data for these four joints
    //  Offset TMs
    geometry[pi].offset.t.pack(
      *(NMP::vpu::vector4_t*)&params->perJointParams[i0].offsetTransform.t,
      *(NMP::vpu::vector4_t*)&params->perJointParams[i1].offsetTransform.t,
      *(NMP::vpu::vector4_t*)&params->perJointParams[i2].offsetTransform.t,
      *(NMP::vpu::vector4_t*)&params->perJointParams[i3].offsetTransform.t);
    geometry[pi].offset.q.pack(
      *(NMP::vpu::vector4_t*)&params->perJointParams[i0].offsetTransform.q,
      *(NMP::vpu::vector4_t*)&params->perJointParams[i1].offsetTransform.q,
      *(NMP::vpu::vector4_t*)&params->perJointParams[i2].offsetTransform.q,
      *(NMP::vpu::vector4_t*)&params->perJointParams[i3].offsetTransform.q);
    // Swap out invalid entries for the identity
    NMP::vpu::Vector3MP zeroPosMP(NMP::vpu::zero4f(), NMP::vpu::zero4f(), NMP::vpu::zero4f());
    geometry[pi].offset.t = geometry[pi].offset.t.merge(zeroPosMP, validMask);
    NMP::vpu::QuatMP identityQuatMP(NMP::vpu::zero4f(), NMP::vpu::zero4f(), NMP::vpu::zero4f(), NMP::vpu::one4f());
    geometry[pi].offset.q = geometry[pi].offset.q.merge(identityQuatMP, validMask);
    //  Reference TMs
    geometry[pi].reference.t.pack(
      *(NMP::vpu::vector4_t*)&params->perJointParams[i0].referenceTransform.t,
      *(NMP::vpu::vector4_t*)&params->perJointParams[i1].referenceTransform.t,
      *(NMP::vpu::vector4_t*)&params->perJointParams[i2].referenceTransform.t,
      *(NMP::vpu::vector4_t*)&params->perJointParams[i3].referenceTransform.t);
    geometry[pi].reference.q.pack(
      *(NMP::vpu::vector4_t*)&params->perJointParams[i0].referenceTransform.q,
      *(NMP::vpu::vector4_t*)&params->perJointParams[i1].referenceTransform.q,
      *(NMP::vpu::vector4_t*)&params->perJointParams[i2].referenceTransform.q,
      *(NMP::vpu::vector4_t*)&params->perJointParams[i3].referenceTransform.q);
    // Swap out invalid entries for the identity
    geometry[pi].reference.t = geometry[pi].reference.t.merge(zeroPosMP, validMask);
    geometry[pi].reference.q = geometry[pi].reference.q.merge(identityQuatMP, validMask);
    //  Parent reference TM
    geometry[pi].parentReference.t.pack(
      *(NMP::vpu::vector4_t*)&params->perJointParams[p0].referenceTransform.t,
      *(NMP::vpu::vector4_t*)&params->perJointParams[p1].referenceTransform.t,
      *(NMP::vpu::vector4_t*)&params->perJointParams[p2].referenceTransform.t,
      *(NMP::vpu::vector4_t*)&params->perJointParams[p3].referenceTransform.t);
    geometry[pi].parentReference.q.pack(
      *(NMP::vpu::vector4_t*)&params->perJointParams[p0].referenceTransform.q,
      *(NMP::vpu::vector4_t*)&params->perJointParams[p1].referenceTransform.q,
      *(NMP::vpu::vector4_t*)&params->perJointParams[p2].referenceTransform.q,
      *(NMP::vpu::vector4_t*)&params->perJointParams[p3].referenceTransform.q);
    // Swap out invalid entries for the identity
    geometry[pi].parentReference.t = geometry[pi].parentReference.t.merge(zeroPosMP, parentValidMask);
    geometry[pi].parentReference.q = geometry[pi].parentReference.q.merge(identityQuatMP, parentValidMask);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void FKRetarget::Solver::prepareParams(const Params* sourceParams, const Params* targetParams)
{
  PerJointParams* sourceJointParams = sourceParams->perJointParams;
  PerJointParams* targetJointParams = targetParams->perJointParams;

  // Target parameters
  m_targetRigScale = NMP::vpu::set4f(targetParams->rigScale);
  for (uint32_t pi = 0; pi < m_mpTargetLength; ++pi)
  {
    // Handy aliases for joint indices
    int32_t* jointIndex = m_mpTargetMappings[pi].jointIndex;
    const int32_t& t0 = jointIndex[0];
    const int32_t& t1 = jointIndex[1];
    const int32_t& t2 = jointIndex[2];
    const int32_t& t3 = jointIndex[3];

    // Packaged joint limits
    m_mpParams[pi].isLimited = false;
    if (
      (targetJointParams[t0].isLimited && m_mpTargetMappings[pi].valid[0]) ||
      (targetJointParams[t1].isLimited && m_mpTargetMappings[pi].valid[1]) ||
      (targetJointParams[t2].isLimited && m_mpTargetMappings[pi].valid[2]) ||
      (targetJointParams[t3].isLimited && m_mpTargetMappings[pi].valid[3]))
    {
      m_mpParams[pi].isLimited = true;
      m_mpParams[pi].applyLimitsMask = NMP::vpu::mask4i(
        targetJointParams[t0].isLimited && m_mpTargetMappings[pi].valid[0],
        targetJointParams[t1].isLimited && m_mpTargetMappings[pi].valid[1],
        targetJointParams[t2].isLimited && m_mpTargetMappings[pi].valid[2],
        targetJointParams[t3].isLimited && m_mpTargetMappings[pi].valid[3]);

#ifdef NM_MSVC_2010
      const JointLimits::Params tempA = targetJointParams[t0].limits;
      const JointLimits::Params tempB = targetJointParams[t1].limits;
      const JointLimits::Params tempC = targetJointParams[t2].limits;
      const JointLimits::Params tempD = targetJointParams[t3].limits;

      m_mpParams[pi].limits.pack(tempA, tempB, tempC, tempD);
#else
      m_mpParams[pi].limits.pack(
        targetJointParams[t0].limits,
        targetJointParams[t1].limits,
        targetJointParams[t2].limits,
        targetJointParams[t3].limits);
#endif
    }

    // Packaged redistribution flags
    //  For sequences that are shorter than the length of this package sequence, there will be
    //  invalid channels in packages towards the root.  However, we need to mark these packages
    //  as handling redistribution if the root channel in those shorter sequences does redistribution -
    //  i.e. we effectively extend the redistribution parameter out to those additional invalid spaces.
    //  To do this, we search up the sequence for the root channel to find out whether this channel
    //  is effectively redistributed.
    bool redistribute[4];
    for (uint32_t el = 0; el < 4; ++el)
    {
      int32_t cpi = pi;
      redistribute[el] =
        targetJointParams[m_mpTargetMappings[cpi].jointIndex[el]].redistributeClampedMotion &&
        m_mpTargetMappings[cpi].valid[el];
      // Find root of this sequence's redistribute flag

      while (!m_mpTargetMappings[cpi].valid[el] && cpi >= 0)
      {
        NMP_ASSERT(cpi != m_mpTargetMappings[cpi].childPackage); // infinite loop if the childpackage is this index
        cpi = m_mpTargetMappings[cpi].childPackage;
        if (cpi >= 0)
        {
          redistribute[el] = targetJointParams[m_mpTargetMappings[cpi].jointIndex[el]].redistributeClampedMotion;
        }
      }
    }
    m_mpParams[pi].doRedistribution = false;
    if (redistribute[0] || redistribute[1] || redistribute[2] || redistribute[3])
    {
      m_mpParams[pi].doRedistribution = true;
      m_mpParams[pi].redistributeMask = NMP::vpu::mask4i(
        redistribute[0], redistribute[1], redistribute[2], redistribute[3]);
    }

    // Packaged bone rigidity mask
    m_mpParams[pi].isRigidMask = NMP::vpu::mask4i(
      !targetJointParams[t0].isRigid,
      !targetJointParams[t1].isRigid,
      !targetJointParams[t2].isRigid,
      !targetJointParams[t3].isRigid);

    // Package mirroring params
    MPMirrorMasks& mirrorMasks = m_mpParams[pi].targetMirrorMasks;
    mirrorMasks.mirrorXRotation = NMP::vpu::mask4i(
      targetJointParams[t0].mirrorFlags.isSet(RETARGETMIRROR_ROTATION_X),
      targetJointParams[t1].mirrorFlags.isSet(RETARGETMIRROR_ROTATION_X),
      targetJointParams[t2].mirrorFlags.isSet(RETARGETMIRROR_ROTATION_X),
      targetJointParams[t3].mirrorFlags.isSet(RETARGETMIRROR_ROTATION_X));
    mirrorMasks.mirrorYRotation = NMP::vpu::mask4i(
      targetJointParams[t0].mirrorFlags.isSet(RETARGETMIRROR_ROTATION_Y),
      targetJointParams[t1].mirrorFlags.isSet(RETARGETMIRROR_ROTATION_Y),
      targetJointParams[t2].mirrorFlags.isSet(RETARGETMIRROR_ROTATION_Y),
      targetJointParams[t3].mirrorFlags.isSet(RETARGETMIRROR_ROTATION_Y));
    mirrorMasks.mirrorZRotation = NMP::vpu::mask4i(
      targetJointParams[t0].mirrorFlags.isSet(RETARGETMIRROR_ROTATION_Z),
      targetJointParams[t1].mirrorFlags.isSet(RETARGETMIRROR_ROTATION_Z),
      targetJointParams[t2].mirrorFlags.isSet(RETARGETMIRROR_ROTATION_Z),
      targetJointParams[t3].mirrorFlags.isSet(RETARGETMIRROR_ROTATION_Z));
    mirrorMasks.mirrorXTranslation = NMP::vpu::mask4i(
      targetJointParams[t0].mirrorFlags.isSet(RETARGETMIRROR_TRANSLATION_X),
      targetJointParams[t1].mirrorFlags.isSet(RETARGETMIRROR_TRANSLATION_X),
      targetJointParams[t2].mirrorFlags.isSet(RETARGETMIRROR_TRANSLATION_X),
      targetJointParams[t3].mirrorFlags.isSet(RETARGETMIRROR_TRANSLATION_X));
    mirrorMasks.mirrorYTranslation = NMP::vpu::mask4i(
      targetJointParams[t0].mirrorFlags.isSet(RETARGETMIRROR_TRANSLATION_Y),
      targetJointParams[t1].mirrorFlags.isSet(RETARGETMIRROR_TRANSLATION_Y),
      targetJointParams[t2].mirrorFlags.isSet(RETARGETMIRROR_TRANSLATION_Y),
      targetJointParams[t3].mirrorFlags.isSet(RETARGETMIRROR_TRANSLATION_Y));
    mirrorMasks.mirrorZTranslation = NMP::vpu::mask4i(
      targetJointParams[t0].mirrorFlags.isSet(RETARGETMIRROR_TRANSLATION_Z),
      targetJointParams[t1].mirrorFlags.isSet(RETARGETMIRROR_TRANSLATION_Z),
      targetJointParams[t2].mirrorFlags.isSet(RETARGETMIRROR_TRANSLATION_Z),
      targetJointParams[t3].mirrorFlags.isSet(RETARGETMIRROR_TRANSLATION_Z));
  }

  // Source parameters
  m_sourceInverseRigScale = NMP::vpu::set4f(1.0f / sourceParams->rigScale);
  for (uint32_t pi = 0; pi < m_mpSourceLength; ++pi)
  {
    // Handy aliases for joint indices
    int32_t* jointIndex = m_mpSourceMappings[pi].jointIndex;
    const int32_t& s0 = jointIndex[0];
    const int32_t& s1 = jointIndex[1];
    const int32_t& s2 = jointIndex[2];
    const int32_t& s3 = jointIndex[3];

    // Package mirroring params
    MPMirrorMasks& mirrorMasks = m_mpParams[pi].sourceMirrorMasks;
    mirrorMasks.mirrorXRotation = NMP::vpu::mask4i(
      sourceJointParams[s0].mirrorFlags.isSet(RETARGETMIRROR_ROTATION_X),
      sourceJointParams[s1].mirrorFlags.isSet(RETARGETMIRROR_ROTATION_X),
      sourceJointParams[s2].mirrorFlags.isSet(RETARGETMIRROR_ROTATION_X),
      sourceJointParams[s3].mirrorFlags.isSet(RETARGETMIRROR_ROTATION_X));
    mirrorMasks.mirrorYRotation = NMP::vpu::mask4i(
      sourceJointParams[s0].mirrorFlags.isSet(RETARGETMIRROR_ROTATION_Y),
      sourceJointParams[s1].mirrorFlags.isSet(RETARGETMIRROR_ROTATION_Y),
      sourceJointParams[s2].mirrorFlags.isSet(RETARGETMIRROR_ROTATION_Y),
      sourceJointParams[s3].mirrorFlags.isSet(RETARGETMIRROR_ROTATION_Y));
    mirrorMasks.mirrorZRotation = NMP::vpu::mask4i(
      sourceJointParams[s0].mirrorFlags.isSet(RETARGETMIRROR_ROTATION_Z),
      sourceJointParams[s1].mirrorFlags.isSet(RETARGETMIRROR_ROTATION_Z),
      sourceJointParams[s2].mirrorFlags.isSet(RETARGETMIRROR_ROTATION_Z),
      sourceJointParams[s3].mirrorFlags.isSet(RETARGETMIRROR_ROTATION_Z));
    mirrorMasks.mirrorXTranslation = NMP::vpu::mask4i(
      sourceJointParams[s0].mirrorFlags.isSet(RETARGETMIRROR_TRANSLATION_X),
      sourceJointParams[s1].mirrorFlags.isSet(RETARGETMIRROR_TRANSLATION_X),
      sourceJointParams[s2].mirrorFlags.isSet(RETARGETMIRROR_TRANSLATION_X),
      sourceJointParams[s3].mirrorFlags.isSet(RETARGETMIRROR_TRANSLATION_X));
    mirrorMasks.mirrorYTranslation = NMP::vpu::mask4i(
      sourceJointParams[s0].mirrorFlags.isSet(RETARGETMIRROR_TRANSLATION_Y),
      sourceJointParams[s1].mirrorFlags.isSet(RETARGETMIRROR_TRANSLATION_Y),
      sourceJointParams[s2].mirrorFlags.isSet(RETARGETMIRROR_TRANSLATION_Y),
      sourceJointParams[s3].mirrorFlags.isSet(RETARGETMIRROR_TRANSLATION_Y));
    mirrorMasks.mirrorZTranslation = NMP::vpu::mask4i(
      sourceJointParams[s0].mirrorFlags.isSet(RETARGETMIRROR_TRANSLATION_Z),
      sourceJointParams[s1].mirrorFlags.isSet(RETARGETMIRROR_TRANSLATION_Z),
      sourceJointParams[s2].mirrorFlags.isSet(RETARGETMIRROR_TRANSLATION_Z),
      sourceJointParams[s3].mirrorFlags.isSet(RETARGETMIRROR_TRANSLATION_Z));
  }

}

//----------------------------------------------------------------------------------------------------------------------
void FKRetarget::Solver::prepareMP(
  const Params* sourceParams,
  const NMP::Hierarchy* sourceHierarchy,
  const Params* targetParams,
  const NMP::Hierarchy* targetHierarchy)
{
  NMIK_PROFILE_BEGIN("FKRetarget::Solver::prepareMP");

  // Set the special packages at index -1 to have identity transforms
  //m_mpSourceGeom[-1].transform.t.x = NMP::vpu::zero4f();
  // etc

  //----------------------------------------------------------------------------------------------------------------------
  // Package multiplex data for the source rig

  // Package the joint indices and sequence data
  prepareMappings(sourceParams, sourceHierarchy, targetParams, targetHierarchy, true);

  // Package the geometrical parameters
  prepareGeom(sourceParams, true);

  //----------------------------------------------------------------------------------------------------------------------
  // Package multiplex data for the target rig

  // Package the joint indices and sequence data
  prepareMappings(targetParams, targetHierarchy, sourceParams, sourceHierarchy, false);

  // Package the geometrical parameters
  prepareGeom(targetParams, false);

  //----------------------------------------------------------------------------------------------------------------------
  // Package other multiplexed parameters
  prepareParams(sourceParams, targetParams);

  NMIK_PROFILE_END();
}

//----------------------------------------------------------------------------------------------------------------------
void FKRetarget::accumulateTransforms(
  const NMP::DataBuffer* transformBuffer,
  NMP::DataBuffer* worldBuffer,
  const NMP::Hierarchy* hierarchy)
{
  // MORPH-21308: Deal with unused channels

  // NB This algorithm relies on hierarchies being correctly organised so that parent
  // joints always have lower indices than their children
  NMP_ASSERT(worldBuffer->getLength() == transformBuffer->getLength());

  uint32_t numJoints = transformBuffer->getLength();
  for (uint32_t j = 0; j < numJoints; ++j)
  {
    const NMP::Vector3& jointPos = *transformBuffer->getPosQuatChannelPos(j);
    const NMP::Quat& jointQuat = *transformBuffer->getPosQuatChannelQuat(j);
    NMP::Vector3& worldPos = *worldBuffer->getPosQuatChannelPos(j);
    NMP::Quat& worldQuat = *worldBuffer->getPosQuatChannelQuat(j);
    worldPos = jointPos;
    worldQuat = jointQuat;

    // Multiply by parent worldspace transform (already calculated)
    int32_t parentJointIndex = hierarchy->getParentIndex(j);
    if (parentJointIndex >= 0)
    {
      const NMP::Vector3& parentJointPos = *worldBuffer->getPosQuatChannelPos(parentJointIndex);
      const NMP::Quat& parentJointQuat = *worldBuffer->getPosQuatChannelQuat(parentJointIndex);
      worldPos = parentJointQuat.rotateVector(worldPos) + parentJointPos;
      worldQuat = parentJointQuat * worldQuat;
    }

    // Must normalise otherwise there's bad numerical drift
    worldQuat.normalise();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void FKRetarget::accumulateOffsetTransforms(
  const FKRetarget::Params* params,
  const NMP::DataBuffer* transformBuffer,
  NMP::DataBuffer* worldBuffer,
  const NMP::Hierarchy* hierarchy)
{
  // MORPH-21308: Deal with unused channels

  // NB This algorithm relies on hierarchies being correctly organised so that parent
  // joints always have lower indices than their children
  NMP_ASSERT(worldBuffer->getLength() >= transformBuffer->getLength());

  uint32_t numJoints = transformBuffer->getLength();
  for (uint32_t j = 0; j < numJoints; ++j)
  {
    const NMP::Vector3& jointPos = *transformBuffer->getPosQuatChannelPos(j);
    const NMP::Quat& jointQuat = *transformBuffer->getPosQuatChannelQuat(j);
    const NMP::Vector3& offsetPos = params->perJointParams[j].offsetTransform.t;
    const NMP::Quat& offsetQuat = params->perJointParams[j].offsetTransform.q;
    NMP::Vector3& worldPos = *worldBuffer->getPosQuatChannelPos(j);
    NMP::Quat& worldQuat = *worldBuffer->getPosQuatChannelQuat(j);

    // Apply offsets
    worldPos = offsetPos + jointPos;
    worldQuat = offsetQuat * jointQuat;

    // Multiply by parent worldspace transform (already calculated)
    int32_t parentJointIndex = hierarchy->getParentIndex(j);
    if (parentJointIndex >= 0)
    {
      const NMP::Vector3& parentJointPos = *worldBuffer->getPosQuatChannelPos(parentJointIndex);
      const NMP::Quat& parentJointQuat = *worldBuffer->getPosQuatChannelQuat(parentJointIndex);
      worldPos = parentJointQuat.rotateVector(worldPos) + parentJointPos;
      worldQuat = parentJointQuat * worldQuat;
    }

    // Must normalise otherwise there's bad numerical drift
    worldQuat.normalise();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void FKRetarget::accumulateTransform(
  NMRU::GeomUtils::PosQuat& transform,
  const NMP::Hierarchy* hierarchy,
  const NMP::DataBuffer* transformBuffer,
  uint32_t jointIndex,
  int32_t referenceJointIndex)
{
  transform.t = *transformBuffer->getPosQuatChannelPos(jointIndex);
  transform.q = *transformBuffer->getPosQuatChannelQuat(jointIndex);
  int32_t j = jointIndex;
  while ((j = hierarchy->getParentIndex(j)) != referenceJointIndex)
  {
    if (j < 0)
      break;
    NMP::Vector3& jointPos = *transformBuffer->getPosQuatChannelPos(j);
    NMP::Quat& jointQuat = *transformBuffer->getPosQuatChannelQuat(j);
    transform.t = jointQuat.rotateVector(transform.t) + jointPos;
    transform.q = jointQuat * transform.q;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void FKRetarget::accumulateOffsetTransform(
  const FKRetarget::Params* params,
  NMRU::GeomUtils::PosQuat& transform,
  const NMP::Hierarchy* hierarchy,
  const NMP::DataBuffer* transformBuffer,
  uint32_t jointIndex,
  int32_t referenceJointIndex)
{
  transform.t = params->perJointParams[jointIndex].offsetTransform.t +
    *transformBuffer->getPosQuatChannelPos(jointIndex);
  transform.q = params->perJointParams[jointIndex].offsetTransform.q *
    *transformBuffer->getPosQuatChannelQuat(jointIndex);
  int32_t j = jointIndex;
  while ((j = hierarchy->getParentIndex(j)) != referenceJointIndex)
  {
    if (j < 0)
      break;
    NMP::Vector3 jointPos = params->perJointParams[j].offsetTransform.t +
      *transformBuffer->getPosQuatChannelPos(j);
    NMP::Quat jointQuat = params->perJointParams[j].offsetTransform.q *
      *transformBuffer->getPosQuatChannelQuat(j);
    transform.t = jointQuat.rotateVector(transform.t) + jointPos;
    transform.q = jointQuat * transform.q;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void FKRetarget::approximateReferenceTransforms(
  FKRetarget::Params* params,
  const NMP::DataBuffer* transforms,
  const NMP::Hierarchy* hierarchy,
  NMP::DataBuffer* worldTransforms /* = 0 */)
{
  // MORPH-21309: This assumes z is forwards and y is up.  Needs to be passed in instead.
  NMP::Vector3 worldUpVector = NMP::Vector3YAxis();
  NMP::Vector3 worldForwardsVector = NMP::Vector3ZAxis();
  NMP::Vector3 worldLeftVector;
  worldLeftVector.cross(worldUpVector, worldForwardsVector);

  if (worldTransforms)
  {
    accumulateOffsetTransforms(params, transforms, worldTransforms, hierarchy);
    worldTransforms->getUsedFlags()->clearAll();
  }

  // Go through every joint and get the reference frame for its parent
  uint32_t numJoints = transforms->getLength();
  for (uint32_t childJoint = 0; childJoint < numJoints; ++childJoint)
  {
    // Parent joint index
    int32_t j = hierarchy->getParentIndex(childJoint);

    if (j >= 0 && (!worldTransforms || !worldTransforms->hasChannel(j)))
    {
      // Reference frame we are calculating - we're only doing orientation
      NMP::Matrix34 referenceMatrix;

      // Get the child joint offset transform
      const NMP::Vector3& childJointPos = *transforms->getPosQuatChannelPos(childJoint);
      const NMP::Quat& childJointQuat = *transforms->getPosQuatChannelQuat(childJoint);
      const NMP::Vector3& childOffsetPos = params->perJointParams[childJoint].offsetTransform.t;
      const NMP::Quat& childOffsetQuat = params->perJointParams[childJoint].offsetTransform.q;
      GeomUtils::PosQuat childTransform = {
        childOffsetPos + childJointPos,
        childOffsetQuat* childJointQuat
      };

      // Get the worldspace offset joint
      GeomUtils::PosQuat worldTransform;
      if (worldTransforms)
      {
        worldTransform.t = *worldTransforms->getPosQuatChannelPos(j);
        worldTransform.q = *worldTransforms->getPosQuatChannelQuat(j);
        worldTransforms->setChannelUsed(j);
      }
      else
      {
        accumulateOffsetTransform(params, worldTransform, hierarchy, transforms, j, -1);
      }

      // Get the child bone direction for the y-axis
      NMP::Vector3& refYAxis = referenceMatrix.r[1];
      refYAxis = childTransform.t;
      // MORPH-21310: Use parent bone if there is no bone.
      refYAxis.normalise();

      // Get the z-axis as the world z-axis
      NMP::Vector3& refZAxis = referenceMatrix.r[2];
      refZAxis = worldTransform.q.inverseRotateVector(worldForwardsVector);
      // Orthogonalise - remove parallel component
      refZAxis -= refYAxis * refYAxis.dot(refZAxis);
      // Poorly conditioned?  Calculate from world left instead, via world up and forwards
      const float conditioningTolerance = 0.5f;
      const float conditioningToleranceSq = conditioningTolerance * conditioningTolerance;
      if (refZAxis.magnitudeSquared() > conditioningToleranceSq)
      {
        // Well-conditioned from the forwards direction
        refZAxis.normalise();
        // Calculate x axis from the other two
        referenceMatrix.r[0].cross(refYAxis, refZAxis);
      }
      else
      {
        // Poorly conditioned from the forwards direction - use world left for x-axis instead
        NMP::Vector3& refXAxis = referenceMatrix.r[0];
        refXAxis = worldTransform.q.inverseRotateVector(worldLeftVector);
        // Orthogonalise - remove parallel component
        refXAxis -= refYAxis * refYAxis.dot(refXAxis);
        // Calculate z axis from the other two
        refXAxis.normalise();
        referenceMatrix.r[2].cross(refXAxis, refYAxis);
      }

      // Convert into a reference quat, leave pos as zero
      params->perJointParams[j].referenceTransform.q = referenceMatrix.toQuat();

      // Copy reference frame orientation to child in case the child has no children to define
      // the bone direction
      params->perJointParams[childJoint].referenceTransform.q =
        ~*transforms->getPosQuatChannelQuat(childJoint) * params->perJointParams[j].referenceTransform.q;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void FKRetarget::Solver::overwriteTargetOffsetsMP(
  const NMP::DataBuffer* offsetTransforms,
  const NMP::DataBuffer* referenceTransforms)
{
  MPIndexMap* mappings = m_mpTargetMappings;
  MPGeometry* geometry = m_mpTargetGeom;
  uint32_t& length = m_mpTargetLength;

  //----------------------------------------------------------------------------------------------------------------------
  // Go through all the packed indices of the target rig and repack the offset parameters
  // in the multiplex member variables
  for (uint32_t pi = 0; pi < length; ++pi)
  {
    // Get handy references to the joint indices
    const int32_t& i0 = mappings[pi].jointIndex[0];
    const int32_t& i1 = mappings[pi].jointIndex[1];
    const int32_t& i2 = mappings[pi].jointIndex[2];
    const int32_t& i3 = mappings[pi].jointIndex[3];
    const int32_t& p0 = mappings[pi].jointParent[0];
    const int32_t& p1 = mappings[pi].jointParent[1];
    const int32_t& p2 = mappings[pi].jointParent[2];
    const int32_t& p3 = mappings[pi].jointParent[3];

    // Retrieve/create masks for valid entries
    vector4& validMask = geometry[pi].validMask;
    NMP::vpu::vector4_t parentValidMask = NMP::vpu::mask4i(
      mappings[pi].parentValid[0],
      mappings[pi].parentValid[1],
      mappings[pi].parentValid[2],
      mappings[pi].parentValid[3]
    );

    // Validate input
#ifdef VALIDATE_TRANSFORMS
    NMP_ASSERT(ISFINITE_QUAT(*offsetTransforms->getPosQuatChannelQuat(i0)));
    NMP_ASSERT(ISFINITE_QUAT(*offsetTransforms->getPosQuatChannelQuat(i1)));
    NMP_ASSERT(ISFINITE_QUAT(*offsetTransforms->getPosQuatChannelQuat(i2)));
    NMP_ASSERT(ISFINITE_QUAT(*offsetTransforms->getPosQuatChannelQuat(i3)));
    NMP_ASSERT(ISFINITE_VECTOR3(*offsetTransforms->getPosQuatChannelPos(i0)));
    NMP_ASSERT(ISFINITE_VECTOR3(*offsetTransforms->getPosQuatChannelPos(i1)));
    NMP_ASSERT(ISFINITE_VECTOR3(*offsetTransforms->getPosQuatChannelPos(i2)));
    NMP_ASSERT(ISFINITE_VECTOR3(*offsetTransforms->getPosQuatChannelPos(i3)));
    NMP_ASSERT(ISFINITE_QUAT(*referenceTransforms->getPosQuatChannelQuat(i0)));
    NMP_ASSERT(ISFINITE_QUAT(*referenceTransforms->getPosQuatChannelQuat(i1)));
    NMP_ASSERT(ISFINITE_QUAT(*referenceTransforms->getPosQuatChannelQuat(i2)));
    NMP_ASSERT(ISFINITE_QUAT(*referenceTransforms->getPosQuatChannelQuat(i3)));
    NMP_ASSERT(ISFINITE_QUAT(*referenceTransforms->getPosQuatChannelQuat(p0)));
    NMP_ASSERT(ISFINITE_QUAT(*referenceTransforms->getPosQuatChannelQuat(p1)));
    NMP_ASSERT(ISFINITE_QUAT(*referenceTransforms->getPosQuatChannelQuat(p2)));
    NMP_ASSERT(ISFINITE_QUAT(*referenceTransforms->getPosQuatChannelQuat(p3)));
#endif

    //----------------------------------------------------------------------------------------------------------------------
    // Pack the geometry data for these four joints
    //  Offset TMs
    geometry[pi].offset.t.pack(
      *(NMP::vpu::vector4_t*)offsetTransforms->getPosQuatChannelPos(i0),
      *(NMP::vpu::vector4_t*)offsetTransforms->getPosQuatChannelPos(i1),
      *(NMP::vpu::vector4_t*)offsetTransforms->getPosQuatChannelPos(i2),
      *(NMP::vpu::vector4_t*)offsetTransforms->getPosQuatChannelPos(i3));
    geometry[pi].offset.q.pack(
      *(NMP::vpu::vector4_t*)offsetTransforms->getPosQuatChannelQuat(i0),
      *(NMP::vpu::vector4_t*)offsetTransforms->getPosQuatChannelQuat(i1),
      *(NMP::vpu::vector4_t*)offsetTransforms->getPosQuatChannelQuat(i2),
      *(NMP::vpu::vector4_t*)offsetTransforms->getPosQuatChannelQuat(i3));
    // Swap out invalid entries for the identity
    NMP::vpu::Vector3MP zeroPosMP(NMP::vpu::zero4f(), NMP::vpu::zero4f(), NMP::vpu::zero4f());
    geometry[pi].offset.t = geometry[pi].offset.t.merge(zeroPosMP, validMask);
    NMP::vpu::QuatMP identityQuatMP(NMP::vpu::zero4f(), NMP::vpu::zero4f(), NMP::vpu::zero4f(), NMP::vpu::one4f());
    geometry[pi].offset.q = geometry[pi].offset.q.merge(identityQuatMP, validMask);
    //  Reference TMs
    geometry[pi].reference.q.pack(
      *(NMP::vpu::vector4_t*)referenceTransforms->getPosQuatChannelQuat(i0),
      *(NMP::vpu::vector4_t*)referenceTransforms->getPosQuatChannelQuat(i1),
      *(NMP::vpu::vector4_t*)referenceTransforms->getPosQuatChannelQuat(i2),
      *(NMP::vpu::vector4_t*)referenceTransforms->getPosQuatChannelQuat(i3));
    // Swap out invalid entries for the identity
    geometry[pi].reference.t = geometry[pi].reference.t.merge(zeroPosMP, validMask);
    geometry[pi].reference.q = geometry[pi].reference.q.merge(identityQuatMP, validMask);
    //  Parent reference TM
    geometry[pi].parentReference.q.pack(
      *(NMP::vpu::vector4_t*)referenceTransforms->getPosQuatChannelQuat(p0),
      *(NMP::vpu::vector4_t*)referenceTransforms->getPosQuatChannelQuat(p1),
      *(NMP::vpu::vector4_t*)referenceTransforms->getPosQuatChannelQuat(p2),
      *(NMP::vpu::vector4_t*)referenceTransforms->getPosQuatChannelQuat(p3));
    // Swap out invalid entries for the identity
    geometry[pi].parentReference.t = geometry[pi].parentReference.t.merge(zeroPosMP, parentValidMask);
    geometry[pi].parentReference.q = geometry[pi].parentReference.q.merge(identityQuatMP, parentValidMask);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void FKRetarget::Solver::overwriteTargetOffsetMP(
  int32_t jointIndex,
  const NMP::PosQuat& offsetTransform,
  const NMP::PosQuat& referenceTransform)
{
  MPIndexMap* mappings = m_mpTargetMappings;
  MPGeometry* geometry = m_mpTargetGeom;
  uint32_t& length = m_mpTargetLength;

  NMP_ASSERT(ISFINITE_QUAT(offsetTransform.m_quat));
  NMP_ASSERT(ISFINITE_VECTOR3(offsetTransform.m_pos));
  NMP_ASSERT(ISFINITE_QUAT(referenceTransform.m_quat));
  NMP_ASSERT(ISFINITE_VECTOR3(referenceTransform.m_pos));

  //----------------------------------------------------------------------------------------------------------------------
  // Go through all the packed indices of the target rig and repack the offset parameters
  // in the multiplex member variables
  for (uint32_t pi = 0; pi < length; ++pi)
  {
    // Get handy references to the joint indices
    const int32_t& i0 = mappings[pi].jointIndex[0];
    const int32_t& i1 = mappings[pi].jointIndex[1];
    const int32_t& i2 = mappings[pi].jointIndex[2];
    const int32_t& i3 = mappings[pi].jointIndex[3];
    const int32_t& p0 = mappings[pi].jointParent[0];
    const int32_t& p1 = mappings[pi].jointParent[1];
    const int32_t& p2 = mappings[pi].jointParent[2];
    const int32_t& p3 = mappings[pi].jointParent[3];

    // Retrieve/create masks for valid entries
    vector4& validMask = geometry[pi].validMask;
    NMP::vpu::vector4_t parentValidMask = NMP::vpu::mask4i(
      mappings[pi].parentValid[0],
      mappings[pi].parentValid[1],
      mappings[pi].parentValid[2],
      mappings[pi].parentValid[3]);

    //----------------------------------------------------------------------------------------------------------------------
    // Pack the geometry data for these four joints
    //  Offset TMs
    NMP::vpu::vector4_t t0, t1, t2, t3;
    NMP::vpu::vector4_t q0, q1, q2, q3;
    NMP::vpu::vector4_t r0, r1, r2, r3;

    geometry[pi].offset.t.unpack(t0, t1, t2, t3);
    geometry[pi].offset.q.unpack(q0, q1, q2, q3);
    geometry[pi].reference.q.unpack(r0, r1, r2, r3);

    if (i0 == jointIndex)
    {
      t0 = *(NMP::vpu::vector4_t*)&offsetTransform.m_pos;
      q0 = *(NMP::vpu::vector4_t*)&offsetTransform.m_quat;
      r0 = *(NMP::vpu::vector4_t*)&referenceTransform.m_quat;
    }
    else if (i1 == jointIndex)
    {
      t1 = *(NMP::vpu::vector4_t*)&offsetTransform.m_pos;
      q1 = *(NMP::vpu::vector4_t*)&offsetTransform.m_quat;
      r1 = *(NMP::vpu::vector4_t*)&referenceTransform.m_quat;
    }
    else if (i2 == jointIndex)
    {
      t2 = *(NMP::vpu::vector4_t*)&offsetTransform.m_pos;
      q2 = *(NMP::vpu::vector4_t*)&offsetTransform.m_quat;
      r2 = *(NMP::vpu::vector4_t*)&referenceTransform.m_quat;
    }
    else if (i3 == jointIndex)
    {
      t3 = *(NMP::vpu::vector4_t*)&offsetTransform.m_pos;
      q3 = *(NMP::vpu::vector4_t*)&offsetTransform.m_quat;
      r3 = *(NMP::vpu::vector4_t*)&referenceTransform.m_quat;
    }

    geometry[pi].offset.t.pack(t0, t1, t2, t3);
    geometry[pi].offset.q.pack(q0, q1, q2, q3);
    geometry[pi].reference.q.pack(r0, r1, r2, r3);

    // Swap out invalid entries for the identity
    NMP::vpu::Vector3MP zeroPosMP(NMP::vpu::zero4f(), NMP::vpu::zero4f(), NMP::vpu::zero4f());
    NMP::vpu::QuatMP identityQuatMP(NMP::vpu::zero4f(), NMP::vpu::zero4f(), NMP::vpu::zero4f(), NMP::vpu::one4f());
    geometry[pi].offset.t = geometry[pi].offset.t.merge(zeroPosMP, validMask);
    geometry[pi].offset.q = geometry[pi].offset.q.merge(identityQuatMP, validMask);
    geometry[pi].reference.t = geometry[pi].reference.t.merge(zeroPosMP, validMask);
    geometry[pi].reference.q = geometry[pi].reference.q.merge(identityQuatMP, validMask);

    geometry[pi].parentReference.q.unpack(q0, q1, q2, q3);

    if (p0 == jointIndex)
    {
      q0 = *(NMP::vpu::vector4_t*)&referenceTransform.m_quat;
    }
    else if (p1 == jointIndex)
    {
      q1 = *(NMP::vpu::vector4_t*)&referenceTransform.m_quat;
    }
    else if (p2 == jointIndex)
    {
      q2 = *(NMP::vpu::vector4_t*)&referenceTransform.m_quat;
    }
    else if (p3 == jointIndex)
    {
      q3 = *(NMP::vpu::vector4_t*)&referenceTransform.m_quat;
    }

    geometry[pi].parentReference.q.pack(q0, q1, q2, q3);

    // Swap out invalid entries for the identity
    geometry[pi].parentReference.t = geometry[pi].parentReference.t.merge(zeroPosMP, parentValidMask);
    geometry[pi].parentReference.q = geometry[pi].parentReference.q.merge(identityQuatMP, parentValidMask);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void FKRetarget::Solver::overwriteTargetScale(float scale)
{
  m_targetRigScale = NMP::vpu::set4f(scale);
}

} // end of namespace NMRU
