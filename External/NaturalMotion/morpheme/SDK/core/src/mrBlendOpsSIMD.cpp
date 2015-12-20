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

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
#define BLEND_SIMD_PREFETCH \
  { \
  NMP::vpu::prefetchCacheLine(&((NMP::Vector3*)destBuffer->getElementData(0))[channelID], 0); \
  NMP::vpu::prefetchCacheLine(&((NMP::Vector3*)destBuffer->getElementData(0))[channelID], NMP_VPU_CACHESIZE); \
  NMP::vpu::prefetchCacheLine(&((NMP::Vector3*)destBuffer->getElementData(1))[channelID], 0); \
  NMP::vpu::prefetchCacheLine(&((NMP::Vector3*)destBuffer->getElementData(1))[channelID], NMP_VPU_CACHESIZE); \
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE uint32_t InterpSIMD_Pack4(
  const NMP::DataBuffer* destBuffer,
  const NMP::DataBuffer* sourceBuffer0,
  const NMP::DataBuffer* sourceBuffer1,
  NMP::vpu::vector4_t* v0, NMP::vpu::vector4_t* v1,
  NMP::vpu::vector4_t* q0, NMP::vpu::vector4_t* q1,
  uint32_t* cp, uint32_t& channelID, uint32_t numElements, bool& isFull)
{
  uint32_t i = 0;
  while (i < 4 && channelID < numElements)
  {
    // destBuffer usedFlags indicating lack of two streams
    if (destBuffer->hasChannel(channelID))
    {
      if (sourceBuffer0->hasChannel(channelID))
      {
        // Only buffer 0 has this channel so copy it into the output buffer.
        ((NMP::Vector3*)destBuffer->getElementData(0))[channelID] = ((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID];
        ((NMP::Quat*)destBuffer->getElementData(1))[channelID] = ((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID];
      }
      else if (sourceBuffer1->hasChannel(channelID))
      {
        // Only buffer 1 has this channel so copy it into the output buffer.
        ((NMP::Vector3*)destBuffer->getElementData(0))[channelID] = ((NMP::Vector3*)sourceBuffer1->getElementData(0))[channelID];
        ((NMP::Quat*)destBuffer->getElementData(1))[channelID] = ((NMP::Quat*)sourceBuffer1->getElementData(1))[channelID];
      }
      else
        isFull = false;
    }
    else
    {
      v0[i] = NMP::vpu::load4f((float*) & ((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID]);
      v1[i] = NMP::vpu::load4f((float*) & ((NMP::Vector3*)sourceBuffer1->getElementData(0))[channelID]);
      q0[i] = NMP::vpu::load4f((float*) & ((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID]);
      q1[i] = NMP::vpu::load4f((float*) & ((NMP::Quat*)sourceBuffer1->getElementData(1))[channelID]);
      cp[i] = channelID;
      i++;
    }
    channelID++;
  }

  return i;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE uint32_t AddSIMD_Pack4(
  const NMP::DataBuffer* destBuffer,
  const NMP::DataBuffer* sourceBuffer0,
  const NMP::DataBuffer* sourceBuffer1,
  NMP::vpu::vector4_t* v0, NMP::vpu::vector4_t* v1,
  NMP::vpu::vector4_t* q0, NMP::vpu::vector4_t* q1,
  uint32_t* cp, uint32_t& channelID, uint32_t numElements)
{
  uint32_t i = 0;
  while (i < 4 && channelID < numElements)
  {
    // destBuffer usedFlags indicating lack of two streams
    if (destBuffer->hasChannel(channelID))
    {
      if (sourceBuffer0->hasChannel(channelID))
      {
        // Only buffer 0 has this channel so copy it into the output buffer.
        ((NMP::Vector3*)destBuffer->getElementData(0))[channelID] = ((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID];
        ((NMP::Quat*)destBuffer->getElementData(1))[channelID] = ((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID];
      }
    }
    else
    {
      v0[i] = NMP::vpu::load4f((float*) & ((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID]);
      v1[i] = NMP::vpu::load4f((float*) & ((NMP::Vector3*)sourceBuffer1->getElementData(0))[channelID]);
      q0[i] = NMP::vpu::load4f((float*) & ((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID]);
      q1[i] = NMP::vpu::load4f((float*) & ((NMP::Quat*)sourceBuffer1->getElementData(1))[channelID]);
      cp[i] = channelID;
      i++;
    }
    channelID++;
  }

  return i;
}

//----------------------------------------------------------------------------------------------------------------------
void BlendOpsSIMD::interpQuatInterpPosPartial(
  NMP::DataBuffer*       destBuffer,
  const NMP::DataBuffer* sourceBuffer0,
  const NMP::DataBuffer* sourceBuffer1,
  const float            alpha)
{
  NMP_ASSERT(destBuffer && sourceBuffer0 && sourceBuffer1);
  NMP_ASSERT((destBuffer != sourceBuffer0) && (destBuffer != sourceBuffer1));
  NMP_ASSERT(destBuffer->getLength() == sourceBuffer0->getLength());
  NMP_ASSERT(destBuffer->getLength() == sourceBuffer1->getLength());

  NMP_ASSERT(sourceBuffer0->isFull() == sourceBuffer0->getUsedFlags()->calculateAreAllSet());
  NMP_ASSERT(sourceBuffer1->isFull() == sourceBuffer1->getUsedFlags()->calculateAreAllSet());

  NM_BLENDOPS_BEGIN_PROFILING("BLENDOPPS_interpQuatInterpPosPartial");

  float     clampedAlpha = NMP::clampValue(alpha, 0.0f, 1.0f);
  NMP::vpu::vector4_t calpha = NMP::vpu::load1fp(&clampedAlpha);

  uint32_t  channelID = 0;
  uint32_t  numElements = destBuffer->getLength();
  bool      isFull = true;

  if (sourceBuffer0->isFull() && sourceBuffer1->isFull())
  {
    while (channelID < numElements)
    {
      BLEND_SIMD_PREFETCH

      NMP::vpu::Vector3MP v0, v1, vd;

      v0.pack((float*)&((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID]);
      v1.pack((float*)&((NMP::Vector3*)sourceBuffer1->getElementData(0))[channelID]);

      NMP::vpu::QuatMP q0, q1, qd;

      q0.pack((float*)&((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID]);
      q1.pack((float*)&((NMP::Quat*)sourceBuffer1->getElementData(1))[channelID]);

      vd.lerp(v0, v1, calpha);
      qd.interpBlend(q0, q1, calpha);

      vd.unpack((float*)&((NMP::Vector3*)destBuffer->getElementData(0))[channelID]);
      qd.unpack((float*)&((NMP::Quat*)destBuffer->getElementData(1))[channelID]);

      // next
      channelID += 4;
    }
  }
  else
  {
    // temporarily use destBuffer usedFlags for indicating lack of two streams
    destBuffer->getUsedFlags()->setNotIntersect(sourceBuffer0->getUsedFlags(), sourceBuffer1->getUsedFlags());

    while (channelID < numElements)
    {
      NMP::vpu::vector4_t v0[4];
      NMP::vpu::vector4_t v1[4];

      NMP::vpu::vector4_t q0[4];
      NMP::vpu::vector4_t q1[4];

      uint32_t cp[4] = { 0, 0, 0, 0 };
      
      uint32_t numChannels = InterpSIMD_Pack4(destBuffer, sourceBuffer0, sourceBuffer1, v0, v1, q0, q1, cp, channelID, numElements, isFull);
      
      // process 4 vecs
      NMP::vpu::Vector3MP vv0, vv1, vd;

      vv0.pack(v0[0], v0[1], v0[2], v0[3]);
      vv1.pack(v1[0], v1[1], v1[2], v1[3]);

      vd.lerp(vv0, vv1, calpha);

      // process 4 quats
      NMP::vpu::QuatMP qv0, qv1, qd;

      qv0.pack(q0[0], q0[1], q0[2], q0[3]);
      qv1.pack(q1[0], q1[1], q1[2], q1[3]);

      qd.interpBlend(qv0, qv1, calpha);

      // store
      vd.unpack(v0[0], v0[1], v0[2], v0[3]);
      qd.unpack(q0[0], q0[1], q0[2], q0[3]);

      uint32_t ch = 0;
      while (numChannels--)
      {
        NMP::vpu::store4f((float*)&((NMP::Vector3*)destBuffer->getElementData(0))[cp[ch]], v0[ch]);
        NMP::vpu::store4f((float*)&((NMP::Quat*)destBuffer->getElementData(1))[cp[ch]], q0[ch]);
        ch++;
      }
    }
  }

  NMP_ASSERT(channelID >= numElements);

  // set used flags, and full status
  destBuffer->getUsedFlags()->setUnion(sourceBuffer0->getUsedFlags(), sourceBuffer1->getUsedFlags());
  destBuffer->setFullFlag(isFull);
  
  NMP_ASSERT(destBuffer->isFull() == destBuffer->getUsedFlags()->calculateAreAllSet());

  NM_BLENDOPS_END_PROFILING();
}

//----------------------------------------------------------------------------------------------------------------------
void BlendOpsSIMD::interpQuatAddPosPartial(
  NMP::DataBuffer*       destBuffer,
  const NMP::DataBuffer* sourceBuffer0,
  const NMP::DataBuffer* sourceBuffer1,
  const float            alpha)
{
  NMP_ASSERT(destBuffer && sourceBuffer0 && sourceBuffer1);
  NMP_ASSERT((destBuffer != sourceBuffer0) && (destBuffer != sourceBuffer1));
  NMP_ASSERT(destBuffer->getLength() == sourceBuffer0->getLength());
  NMP_ASSERT(destBuffer->getLength() == sourceBuffer1->getLength());

  NM_BLENDOPS_BEGIN_PROFILING("BLENDOPPS_interpQuatAddPosPartial");

  float clampedAlpha = NMP::clampValue(alpha, 0.0f, 1.0f);
  NMP::vpu::vector4_t calpha = NMP::vpu::load1fp(&clampedAlpha);

  uint32_t  channelID = 0;
  uint32_t  numElements = destBuffer->getLength();
  bool      isFull = true;

  if (sourceBuffer0->isFull() && sourceBuffer1->isFull())
  {
    while (channelID < numElements)
    {
      BLEND_SIMD_PREFETCH

      NMP::vpu::Vector3MP v0, v1, vd;

      v0.pack((float*)&((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID]);
      v1.pack((float*)&((NMP::Vector3*)sourceBuffer1->getElementData(0))[channelID]);

      NMP::vpu::QuatMP q0, q1, qd;

      q0.pack((float*)&((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID]);
      q1.pack((float*)&((NMP::Quat*)sourceBuffer1->getElementData(1))[channelID]);

      vd.interp(v0, v1, calpha);
      qd.interpBlend(q0, q1, calpha);

      vd.unpack((float*)&((NMP::Vector3*)destBuffer->getElementData(0))[channelID]);
      qd.unpack((float*)&((NMP::Quat*)destBuffer->getElementData(1))[channelID]);

      // next
      channelID += 4;
    }
  }
  else
  {
    // temporarily use destBuffer usedFlags for indicating lack of two streams
    destBuffer->getUsedFlags()->setNotIntersect(sourceBuffer0->getUsedFlags(), sourceBuffer1->getUsedFlags());

    while (channelID < numElements)
    {
      NMP::vpu::vector4_t v0[4];
      NMP::vpu::vector4_t v1[4];

      NMP::vpu::vector4_t q0[4];
      NMP::vpu::vector4_t q1[4];

      uint32_t cp[4] = { 0, 0, 0, 0 };

      uint32_t numChannels = InterpSIMD_Pack4(destBuffer, sourceBuffer0, sourceBuffer1, v0, v1, q0, q1, cp, channelID, numElements, isFull);

      // process 4 vecs
      NMP::vpu::Vector3MP vv0, vv1, vd;

      vv0.pack(v0[0], v0[1], v0[2], v0[3]);
      vv1.pack(v1[0], v1[1], v1[2], v1[3]);

      vd.interp(vv0, vv1, calpha);

      // process 4 quats
      NMP::vpu::QuatMP qv0, qv1, qd;

      qv0.pack(q0[0], q0[1], q0[2], q0[3]);
      qv1.pack(q1[0], q1[1], q1[2], q1[3]);

      qd.interpBlend(qv0, qv1, calpha);

      // store
      vd.unpack(v0[0], v0[1], v0[2], v0[3]);
      qd.unpack(q0[0], q0[1], q0[2], q0[3]);

      uint32_t ch = 0;
      while (numChannels--)
      {
        NMP::vpu::store4f((float*)&((NMP::Vector3*)destBuffer->getElementData(0))[cp[ch]], v0[ch]);
        NMP::vpu::store4f((float*)&((NMP::Quat*)destBuffer->getElementData(1))[cp[ch]], q0[ch]);
        ch++;
      }
    }
  }

  NMP_ASSERT(channelID >= numElements);

  // set used flags, and full status
  destBuffer->getUsedFlags()->setUnion(sourceBuffer0->getUsedFlags(), sourceBuffer1->getUsedFlags());
  destBuffer->setFullFlag(isFull);

  NMP_ASSERT(destBuffer->isFull() == destBuffer->getUsedFlags()->calculateAreAllSet());

  NM_BLENDOPS_END_PROFILING();
}

//----------------------------------------------------------------------------------------------------------------------
void BlendOpsSIMD::addQuatAddPosPartial(
  NMP::DataBuffer*       destBuffer,
  const NMP::DataBuffer* sourceBuffer0,
  const NMP::DataBuffer* sourceBuffer1,
  const float            alpha)
{
  NMP_ASSERT(destBuffer && sourceBuffer0 && sourceBuffer1);
  NMP_ASSERT((destBuffer != sourceBuffer0) && (destBuffer != sourceBuffer1));
  NMP_ASSERT(destBuffer->getLength() >= sourceBuffer0->getLength());

  NM_BLENDOPS_BEGIN_PROFILING("BLENDOPPS_addQuatAddPosPartial");

  NMP::vpu::vector4_t calpha = NMP::vpu::load1fp(&alpha);

  uint32_t  channelID = 0;
  uint32_t  numElements = destBuffer->getLength();

  if (sourceBuffer0->isFull() && sourceBuffer1->isFull())
  {
    while (channelID < numElements)
    {
      BLEND_SIMD_PREFETCH

      NMP::vpu::Vector3MP v0, v1, vd;

      v0.pack((float*)&((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID]);
      v1.pack((float*)&((NMP::Vector3*)sourceBuffer1->getElementData(0))[channelID]);

      NMP::vpu::QuatMP q0, q1, qd;

      q0.pack((float*)&((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID]);
      q1.pack((float*)&((NMP::Quat*)sourceBuffer1->getElementData(1))[channelID]);

      vd.interp(v0, v1, calpha);
      qd.additiveBlend(q0, q1, calpha);

      vd.unpack((float*)&((NMP::Vector3*)destBuffer->getElementData(0))[channelID]);
      qd.unpack((float*)&((NMP::Quat*)destBuffer->getElementData(1))[channelID]);

      // next
      channelID += 4;
    }
  }
  else
  {
    // temporarily use destBuffer usedFlags for indicating lack of two streams
    destBuffer->getUsedFlags()->setNotIntersect(sourceBuffer0->getUsedFlags(), sourceBuffer1->getUsedFlags());

    while (channelID < numElements)
    {
      NMP::vpu::vector4_t v0[4];
      NMP::vpu::vector4_t v1[4];

      NMP::vpu::vector4_t q0[4];
      NMP::vpu::vector4_t q1[4];

      uint32_t cp[4] = { 0, 0, 0, 0 };

      uint32_t numChannels = AddSIMD_Pack4(destBuffer, sourceBuffer0, sourceBuffer1, v0, v1, q0, q1, cp, channelID, numElements);

      // process 4 vecs
      NMP::vpu::Vector3MP vv0, vv1, vd;

      vv0.pack(v0[0], v0[1], v0[2], v0[3]);
      vv1.pack(v1[0], v1[1], v1[2], v1[3]);

      vd.interp(vv0, vv1, calpha);

      // process 4 quats
      NMP::vpu::QuatMP qv0, qv1, qd;

      qv0.pack(q0[0], q0[1], q0[2], q0[3]);
      qv1.pack(q1[0], q1[1], q1[2], q1[3]);

      qd.additiveBlend(qv0, qv1, calpha);

      // store
      vd.unpack(v0[0], v0[1], v0[2], v0[3]);
      qd.unpack(q0[0], q0[1], q0[2], q0[3]);

      uint32_t ch = 0;
      while (numChannels--)
      {
        NMP::vpu::store4f((float*)&((NMP::Vector3*)destBuffer->getElementData(0))[cp[ch]], v0[ch]);
        NMP::vpu::store4f((float*)&((NMP::Quat*)destBuffer->getElementData(1))[cp[ch]], q0[ch]);
        ch++;
      }
    }
  }

  NMP_ASSERT(channelID >= numElements);

  // set used flags, and full status
  destBuffer->getUsedFlags()->copy(sourceBuffer0->getUsedFlags());
  destBuffer->setFullFlag(sourceBuffer0->isFull());

  NMP_ASSERT(destBuffer->isFull() == destBuffer->getUsedFlags()->calculateAreAllSet());

  NM_BLENDOPS_END_PROFILING();
}

//----------------------------------------------------------------------------------------------------------------------
void BlendOpsSIMD::subtractQuatSubtractPosPartial(
  NMP::DataBuffer*       destBuffer,
  const NMP::DataBuffer* sourceBuffer0,
  const NMP::DataBuffer* sourceBuffer1,
  const float            alpha)
{
  NMP_ASSERT(destBuffer && sourceBuffer0 && sourceBuffer1);
  NMP_ASSERT((destBuffer != sourceBuffer0) && (destBuffer != sourceBuffer1));
  NMP_ASSERT(destBuffer->getLength() >= sourceBuffer0->getLength());

  NM_BLENDOPS_BEGIN_PROFILING("BLENDOPPS_subtractQuatSubtractPosPartial");

  NMP::vpu::vector4_t calpha = NMP::vpu::load1fp(&alpha);

  uint32_t  channelID = 0;
  uint32_t  numElements = destBuffer->getLength();

  if (sourceBuffer0->isFull() && sourceBuffer1->isFull())
  {
    while (channelID < numElements)
    {
      BLEND_SIMD_PREFETCH

      NMP::vpu::Vector3MP v0, v1, vd;

      v0.pack((float*)&((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID]);
      v1.pack((float*)&((NMP::Vector3*)sourceBuffer1->getElementData(0))[channelID]);

      NMP::vpu::QuatMP q0, q1, qd;

      q0.pack((float*)&((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID]);
      q1.pack((float*)&((NMP::Quat*)sourceBuffer1->getElementData(1))[channelID]);

      vd.subtract(v0, v1, calpha);
      qd.subtractiveBlend(q0, q1, calpha);

      vd.unpack((float*)&((NMP::Vector3*)destBuffer->getElementData(0))[channelID]);
      qd.unpack((float*)&((NMP::Quat*)destBuffer->getElementData(1))[channelID]);

      // next
      channelID += 4;
    }
  }
  else
  {
    // temporarily use destBuffer usedFlags for indicating lack of two streams
    destBuffer->getUsedFlags()->setNotIntersect(sourceBuffer0->getUsedFlags(), sourceBuffer1->getUsedFlags());

    while (channelID < numElements)
    {
      NMP::vpu::vector4_t v0[4];
      NMP::vpu::vector4_t v1[4];

      NMP::vpu::vector4_t q0[4];
      NMP::vpu::vector4_t q1[4];

      uint32_t cp[4] = { 0, 0, 0, 0 };

      uint32_t numChannels = AddSIMD_Pack4(destBuffer, sourceBuffer0, sourceBuffer1, v0, v1, q0, q1, cp, channelID, numElements);

      // process 4 vecs
      NMP::vpu::Vector3MP vv0, vv1, vd;

      vv0.pack(v0[0], v0[1], v0[2], v0[3]);
      vv1.pack(v1[0], v1[1], v1[2], v1[3]);

      vd.subtract(vv0, vv1, calpha);

      // process 4 quats
      NMP::vpu::QuatMP qv0, qv1, qd;

      qv0.pack(q0[0], q0[1], q0[2], q0[3]);
      qv1.pack(q1[0], q1[1], q1[2], q1[3]);

      qd.subtractiveBlend(qv0, qv1, calpha);

      // store
      vd.unpack(v0[0], v0[1], v0[2], v0[3]);
      qd.unpack(q0[0], q0[1], q0[2], q0[3]);

      uint32_t ch = 0;
      while (numChannels--)
      {
        NMP::vpu::store4f((float*)&((NMP::Vector3*)destBuffer->getElementData(0))[cp[ch]], v0[ch]);
        NMP::vpu::store4f((float*)&((NMP::Quat*)destBuffer->getElementData(1))[cp[ch]], q0[ch]);
        ch++;
      }
    }
  }

  NMP_ASSERT(channelID >= numElements);

  // set used flags, and full status
  destBuffer->getUsedFlags()->copy(sourceBuffer0->getUsedFlags());
  destBuffer->setFullFlag(sourceBuffer0->isFull());

  NMP_ASSERT(destBuffer->isFull() == destBuffer->getUsedFlags()->calculateAreAllSet());

  NM_BLENDOPS_END_PROFILING();
}

//----------------------------------------------------------------------------------------------------------------------
void BlendOpsSIMD::interpQuatSubtractPosPartial(
  NMP::DataBuffer*       destBuffer,
  const NMP::DataBuffer* sourceBuffer0,
  const NMP::DataBuffer* sourceBuffer1,
  const float            alpha)
{
  NMP_ASSERT(destBuffer && sourceBuffer0 && sourceBuffer1);
  NMP_ASSERT((destBuffer != sourceBuffer0) && (destBuffer != sourceBuffer1));
  NMP_ASSERT(destBuffer->getLength() >= sourceBuffer0->getLength());

  NM_BLENDOPS_BEGIN_PROFILING("BLENDOPPS_interpQuatSubtractPosPartial");

  float clampedAlpha = NMP::clampValue(alpha, 0.0f, 1.0f);
  NMP::vpu::vector4_t calpha = NMP::vpu::load1fp(&clampedAlpha);

  uint32_t  channelID = 0;
  uint32_t  numElements = destBuffer->getLength();

  if (sourceBuffer0->isFull() && sourceBuffer1->isFull())
  {
    while (channelID < numElements)
    {
      BLEND_SIMD_PREFETCH

      NMP::vpu::Vector3MP v0, v1, vd;

      v0.pack((float*)&((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID]);
      v1.pack((float*)&((NMP::Vector3*)sourceBuffer1->getElementData(0))[channelID]);

      NMP::vpu::QuatMP q0, q1, qd;

      q0.pack((float*)&((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID]);
      q1.pack((float*)&((NMP::Quat*)sourceBuffer1->getElementData(1))[channelID]);

      vd.subtract(v0, v1, calpha);
      qd.interpBlend(q0, q1, calpha);

      vd.unpack((float*)&((NMP::Vector3*)destBuffer->getElementData(0))[channelID]);
      qd.unpack((float*)&((NMP::Quat*)destBuffer->getElementData(1))[channelID]);

      // next
      channelID += 4;
    }
  }
  else
  {
    // temporarily use destBuffer usedFlags for indicating lack of two streams
    destBuffer->getUsedFlags()->setNotIntersect(sourceBuffer0->getUsedFlags(), sourceBuffer1->getUsedFlags());

    while (channelID < numElements)
    {
      NMP::vpu::vector4_t v0[4];
      NMP::vpu::vector4_t v1[4];

      NMP::vpu::vector4_t q0[4];
      NMP::vpu::vector4_t q1[4];

      uint32_t cp[4] = { 0, 0, 0, 0 };

      uint32_t numChannels = AddSIMD_Pack4(destBuffer, sourceBuffer0, sourceBuffer1, v0, v1, q0, q1, cp, channelID, numElements);

      // process 4 vecs
      NMP::vpu::Vector3MP vv0, vv1, vd;

      vv0.pack(v0[0], v0[1], v0[2], v0[3]);
      vv1.pack(v1[0], v1[1], v1[2], v1[3]);

      vd.subtract(vv0, vv1, calpha);

      // process 4 quats
      NMP::vpu::QuatMP qv0, qv1, qd;

      qv0.pack(q0[0], q0[1], q0[2], q0[3]);
      qv1.pack(q1[0], q1[1], q1[2], q1[3]);

      qd.interpBlend(qv0, qv1, calpha);

      // store
      vd.unpack(v0[0], v0[1], v0[2], v0[3]);
      qd.unpack(q0[0], q0[1], q0[2], q0[3]);

      uint32_t ch = 0;
      while (numChannels--)
      {
        NMP::vpu::store4f((float*)&((NMP::Vector3*)destBuffer->getElementData(0))[cp[ch]], v0[ch]);
        NMP::vpu::store4f((float*)&((NMP::Quat*)destBuffer->getElementData(1))[cp[ch]], q0[ch]);
        ch++;
      }
    }
  }

  NMP_ASSERT(channelID >= numElements);

  // set used flags, and full status
  destBuffer->getUsedFlags()->copy(sourceBuffer0->getUsedFlags());
  destBuffer->setFullFlag(sourceBuffer0->isFull());

  NMP_ASSERT(destBuffer->isFull() == destBuffer->getUsedFlags()->calculateAreAllSet());

  NM_BLENDOPS_END_PROFILING();
}

//----------------------------------------------------------------------------------------------------------------------
void BlendOpsSIMD::subtractQuatInterpPosPartial(
  NMP::DataBuffer*       destBuffer,
  const NMP::DataBuffer* sourceBuffer0,
  const NMP::DataBuffer* sourceBuffer1,
  const float            alpha)
{
  NMP_ASSERT(destBuffer && sourceBuffer0 && sourceBuffer1);
  NMP_ASSERT((destBuffer != sourceBuffer0) && (destBuffer != sourceBuffer1));
  NMP_ASSERT(destBuffer->getLength() >= sourceBuffer0->getLength());

  NM_BLENDOPS_BEGIN_PROFILING("BLENDOPPS_subtractQuatInterpPosPartial");

  float clampedAlpha = NMP::clampValue(alpha, 0.0f, 1.0f);
  NMP::vpu::vector4_t calpha = NMP::vpu::load1fp(&clampedAlpha);

  uint32_t  channelID = 0;
  uint32_t  numElements = destBuffer->getLength();

  if (sourceBuffer0->isFull() && sourceBuffer1->isFull())
  {
    while (channelID < numElements)
    {
      BLEND_SIMD_PREFETCH

      NMP::vpu::Vector3MP v0, v1, vd;

      v0.pack((float*)&((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID]);
      v1.pack((float*)&((NMP::Vector3*)sourceBuffer1->getElementData(0))[channelID]);

      NMP::vpu::QuatMP q0, q1, qd;

      q0.pack((float*)&((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID]);
      q1.pack((float*)&((NMP::Quat*)sourceBuffer1->getElementData(1))[channelID]);

      vd.lerp(v0, v1, calpha);
      qd.subtractiveBlend(q0, q1, calpha);

      vd.unpack((float*)&((NMP::Vector3*)destBuffer->getElementData(0))[channelID]);
      qd.unpack((float*)&((NMP::Quat*)destBuffer->getElementData(1))[channelID]);

      // next
      channelID += 4;
    }
  }
  else
  {
    // temporarily use destBuffer usedFlags for indicating lack of two streams
    destBuffer->getUsedFlags()->setNotIntersect(sourceBuffer0->getUsedFlags(), sourceBuffer1->getUsedFlags());

    while (channelID < numElements)
    {
      NMP::vpu::vector4_t v0[4];
      NMP::vpu::vector4_t v1[4];

      NMP::vpu::vector4_t q0[4];
      NMP::vpu::vector4_t q1[4];

      uint32_t cp[4] = { 0, 0, 0, 0 };

      uint32_t numChannels = AddSIMD_Pack4(destBuffer, sourceBuffer0, sourceBuffer1, v0, v1, q0, q1, cp, channelID, numElements);

      // process 4 vecs
      NMP::vpu::Vector3MP vv0, vv1, vd;

      vv0.pack(v0[0], v0[1], v0[2], v0[3]);
      vv1.pack(v1[0], v1[1], v1[2], v1[3]);

      vd.interp(vv0, vv1, calpha);

      // process 4 quats
      NMP::vpu::QuatMP qv0, qv1, qd;

      qv0.pack(q0[0], q0[1], q0[2], q0[3]);
      qv1.pack(q1[0], q1[1], q1[2], q1[3]);

      qd.subtractiveBlend(qv0, qv1, calpha);

      // store
      vd.unpack(v0[0], v0[1], v0[2], v0[3]);
      qd.unpack(q0[0], q0[1], q0[2], q0[3]);

      uint32_t ch = 0;
      while (numChannels--)
      {
        NMP::vpu::store4f((float*)&((NMP::Vector3*)destBuffer->getElementData(0))[cp[ch]], v0[ch]);
        NMP::vpu::store4f((float*)&((NMP::Quat*)destBuffer->getElementData(1))[cp[ch]], q0[ch]);
        ch++;
      }
    }
  }

  NMP_ASSERT(channelID >= numElements);

  // set used flags, and full status
  destBuffer->getUsedFlags()->copy(sourceBuffer0->getUsedFlags());
  destBuffer->setFullFlag(sourceBuffer0->isFull());

  NMP_ASSERT(destBuffer->isFull() == destBuffer->getUsedFlags()->calculateAreAllSet());

  NM_BLENDOPS_END_PROFILING();
}

//----------------------------------------------------------------------------------------------------------------------
void BlendOpsSIMD::addQuatLeavePosPartial(
  NMP::DataBuffer*       destBuffer,
  const NMP::DataBuffer* sourceBuffer0,
  const NMP::DataBuffer* sourceBuffer1,
  const float            alpha)
{
  NMP_ASSERT(destBuffer && sourceBuffer0 && sourceBuffer1);
  NMP_ASSERT((destBuffer != sourceBuffer0) && (destBuffer != sourceBuffer1));
  NMP_ASSERT(destBuffer->getLength() >= sourceBuffer0->getLength());

  NM_BLENDOPS_BEGIN_PROFILING("BLENDOPPS_addQuatLeavePosPartial");

  NMP::vpu::vector4_t calpha = NMP::vpu::load1fp(&alpha);

  uint32_t  channelID = 0;
  uint32_t  numElements = destBuffer->getLength();

  if (sourceBuffer0->isFull() && sourceBuffer1->isFull())
  {
    while (channelID < numElements)
    {
      BLEND_SIMD_PREFETCH

      // pos copy only
      NMP::vpu::vector4_t v00, v01, v02, v03;
      NMP::vpu::load4v((float*)&((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID], v00, v01, v02, v03);

      // quats
      NMP::vpu::QuatMP q0, q1, qd;

      q0.pack((float*)&((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID]);
      q1.pack((float*)&((NMP::Quat*)sourceBuffer1->getElementData(1))[channelID]);

      // store 4 vecs
      NMP::vpu::store4v((float*)&((NMP::Vector3*)destBuffer->getElementData(0))[channelID], v00, v01, v02, v03);

      // process 4 quats
      qd.additiveBlend(q0, q1, calpha);

      qd.unpack((float*)&((NMP::Quat*)destBuffer->getElementData(1))[channelID]);

      // next
      channelID += 4;
    }
  }
  else
  {
    // temporarily use destBuffer usedFlags for indicating lack of two streams
    destBuffer->getUsedFlags()->setNotIntersect(sourceBuffer0->getUsedFlags(), sourceBuffer1->getUsedFlags());

    while (channelID < numElements)
    {
      NMP::vpu::vector4_t v0[4];
      NMP::vpu::vector4_t q0[4];
      NMP::vpu::vector4_t q1[4];

      uint32_t cp[4] = { 0, 0, 0, 0 };

      uint32_t i = 0;
      while (i < 4 && channelID < numElements)
      {
        if (destBuffer->hasChannel(channelID))
        {
          if (sourceBuffer0->hasChannel(channelID))
          {
            // Only buffer 0 has this channel so copy it into the output buffer.
            ((NMP::Vector3*)destBuffer->getElementData(0))[channelID] = ((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID];
            ((NMP::Quat*)destBuffer->getElementData(1))[channelID] = ((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID];
          }
        }
        else
        {
          v0[i] = NMP::vpu::load4f((float*) & ((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID]);
          q0[i] = NMP::vpu::load4f((float*) & ((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID]);
          q1[i] = NMP::vpu::load4f((float*) & ((NMP::Quat*)sourceBuffer1->getElementData(1))[channelID]);
          cp[i] = channelID;
          i++;
        }
        channelID++;
      }

      // process 4 quats
      NMP::vpu::QuatMP qv0, qv1, qd;

      qv0.pack(q0[0], q0[1], q0[2], q0[3]);
      qv1.pack(q1[0], q1[1], q1[2], q1[3]);

      qd.additiveBlend(qv0, qv1, calpha);

      // store
      qd.unpack(q0[0], q0[1], q0[2], q0[3]);
      uint32_t ch = 0;
      while (i--)
      {
        NMP::vpu::store4f((float*)&((NMP::Vector3*)destBuffer->getElementData(0))[cp[ch]], v0[ch]);
        NMP::vpu::store4f((float*)&((NMP::Quat*)destBuffer->getElementData(1))[cp[ch]], q0[ch]);
        ch++;
      }
    }
  }

  NMP_ASSERT(channelID >= numElements);

  // set used flags, and full status
  destBuffer->getUsedFlags()->copy(sourceBuffer0->getUsedFlags());
  destBuffer->setFullFlag(sourceBuffer0->isFull());

  NMP_ASSERT(destBuffer->isFull() == destBuffer->getUsedFlags()->calculateAreAllSet());

  NM_BLENDOPS_END_PROFILING();
}

//----------------------------------------------------------------------------------------------------------------------
void BlendOpsSIMD::interpQuatInterpPosPartialFeathered(
  NMP::DataBuffer*       destBuffer,
  const NMP::DataBuffer* sourceBuffer0,
  const NMP::DataBuffer* sourceBuffer1,
  const float            alpha,
  const uint32_t         numAlphaValues,
  const float*           alphaValues)
{
  NMP_ASSERT(destBuffer && sourceBuffer0 && sourceBuffer1 && alphaValues);
  NMP_ASSERT((destBuffer != sourceBuffer0) && (destBuffer != sourceBuffer1));
  NMP_ASSERT(destBuffer->getLength() == sourceBuffer0->getLength());
  NMP_ASSERT(destBuffer->getLength() == sourceBuffer1->getLength());

  NM_BLENDOPS_BEGIN_PROFILING("BLENDOPPS_interpQuatInterpPosPartialFeathered");

  float clampedAlpha = NMP::clampValue(alpha, 0.0f, 1.0f);
  NMP::vpu::vector4_t calpha = NMP::vpu::load1fp(&clampedAlpha);
  NMP::vpu::vector4_t boneas;

  uint32_t  channelID;
  uint32_t  numElements = destBuffer->getLength();
  bool      isFull = true;

  if (sourceBuffer0->isFull() && sourceBuffer1->isFull())
  {
    channelID = 0;

    while (channelID < numElements)
    {
      BLEND_SIMD_PREFETCH

      if (numAlphaValues > channelID)
        boneas = mul_4f(calpha, NMP::vpu::load4f((float*)&alphaValues[channelID]));
      else
        boneas = calpha;

      NMP::vpu::Vector3MP v0, v1, vd;

      v0.pack((float*)&((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID]);
      v1.pack((float*)&((NMP::Vector3*)sourceBuffer1->getElementData(0))[channelID]);

      NMP::vpu::QuatMP q0, q1, qd;

      q0.pack((float*)&((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID]);
      q1.pack((float*)&((NMP::Quat*)sourceBuffer1->getElementData(1))[channelID]);

      vd.lerp(v0, v1, boneas);
      qd.interpBlend(q0, q1, boneas);

      vd.unpack((float*)&((NMP::Vector3*)destBuffer->getElementData(0))[channelID]);
      qd.unpack((float*)&((NMP::Quat*)destBuffer->getElementData(1))[channelID]);

      // next
      channelID += 4;
    }
  }
  else
  {
    // temporarily use destBuffer usedFlags for indicating lack of two streams
    destBuffer->getUsedFlags()->setNotIntersect(sourceBuffer0->getUsedFlags(), sourceBuffer1->getUsedFlags());

    NMP::vpu::vector4_t lvc = NMP::vpu::set4i(numAlphaValues);
    channelID = 0;

    while (channelID < numElements)
    {
      NMP::vpu::vector4_t v0[4];
      NMP::vpu::vector4_t v1[4];

      NMP::vpu::vector4_t q0[4];
      NMP::vpu::vector4_t q1[4];

      uint32_t cp[4] = { 0, 0, 0, 0 };

      uint32_t numChannels = InterpSIMD_Pack4(destBuffer, sourceBuffer0, sourceBuffer1, v0, v1, q0, q1, cp, channelID, numElements, isFull);

      NMP::vpu::vector4_t avc = NMP::vpu::set4f(alphaValues[cp[0]], alphaValues[cp[1]], alphaValues[cp[2]], alphaValues[cp[3]]);
      NMP::vpu::vector4_t cpv = NMP::vpu::set4i(cp[0], cp[1], cp[2], cp[3]);
      boneas = mul_4f(calpha, NMP::vpu::sel4cmpiLT(cpv, lvc, avc, NMP::vpu::one4f()));

      // process 4 vecs
      NMP::vpu::Vector3MP vv0, vv1, vd;

      vv0.pack(v0[0], v0[1], v0[2], v0[3]);
      vv1.pack(v1[0], v1[1], v1[2], v1[3]);

      vd.lerp(vv0, vv1, boneas);

      // process 4 quats
      NMP::vpu::QuatMP qv0, qv1, qd;

      qv0.pack(q0[0], q0[1], q0[2], q0[3]);
      qv1.pack(q1[0], q1[1], q1[2], q1[3]);

      qd.interpBlend(qv0, qv1, boneas);

      // store
      vd.unpack(v0[0], v0[1], v0[2], v0[3]);
      qd.unpack(q0[0], q0[1], q0[2], q0[3]);

      uint32_t ch = 0;
      while (numChannels--)
      {
        NMP::vpu::store4f((float*)&((NMP::Vector3*)destBuffer->getElementData(0))[cp[ch]], v0[ch]);
        NMP::vpu::store4f((float*)&((NMP::Quat*)destBuffer->getElementData(1))[cp[ch]], q0[ch]);
        ch++;
      }
    }
  }

  NMP_ASSERT(channelID >= numElements);

  // set used flags, and full status
  destBuffer->getUsedFlags()->setUnion(sourceBuffer0->getUsedFlags(), sourceBuffer1->getUsedFlags());
  destBuffer->setFullFlag(isFull);

  NMP_ASSERT(destBuffer->isFull() == destBuffer->getUsedFlags()->calculateAreAllSet());

  NM_BLENDOPS_END_PROFILING();
}

//----------------------------------------------------------------------------------------------------------------------
void BlendOpsSIMD::interpQuatAddPosPartialFeathered(
  NMP::DataBuffer*       destBuffer,
  const NMP::DataBuffer* sourceBuffer0,
  const NMP::DataBuffer* sourceBuffer1,
  const float            alpha,
  const uint32_t         numAlphaValues,
  const float*           alphaValues)
{
  NMP_ASSERT(destBuffer && sourceBuffer0 && sourceBuffer1 && alphaValues);
  NMP_ASSERT((destBuffer != sourceBuffer0) && (destBuffer != sourceBuffer1));
  NMP_ASSERT(destBuffer->getLength() == sourceBuffer0->getLength());
  NMP_ASSERT(destBuffer->getLength() == sourceBuffer1->getLength());

  NM_BLENDOPS_BEGIN_PROFILING("BLENDOPPS_interpQuatAddPosPartialFeathered");

  float     clampedAlpha = NMP::clampValue(alpha, 0.0f, 1.0f);
  NMP::vpu::vector4_t calpha = NMP::vpu::load1fp(&clampedAlpha);
  NMP::vpu::vector4_t boneas;

  uint32_t  channelID;
  uint32_t  numElements = destBuffer->getLength();
  bool      isFull = true;

  if (sourceBuffer0->isFull() && sourceBuffer1->isFull())
  {
    channelID = 0;

    while (channelID < numElements)
    {
      BLEND_SIMD_PREFETCH

      if (numAlphaValues > channelID)
        boneas = mul_4f(calpha, NMP::vpu::load4f((float*)&alphaValues[channelID]));
      else
        boneas = calpha;

      NMP::vpu::Vector3MP v0, v1, vd;

      v0.pack((float*)&((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID]);
      v1.pack((float*)&((NMP::Vector3*)sourceBuffer1->getElementData(0))[channelID]);

      NMP::vpu::QuatMP q0, q1, qd;

      q0.pack((float*)&((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID]);
      q1.pack((float*)&((NMP::Quat*)sourceBuffer1->getElementData(1))[channelID]);

      vd.interp(v0, v1, boneas);
      qd.interpBlend(q0, q1, boneas);

      vd.unpack((float*)&((NMP::Vector3*)destBuffer->getElementData(0))[channelID]);
      qd.unpack((float*)&((NMP::Quat*)destBuffer->getElementData(1))[channelID]);

      // next
      channelID += 4;
    }
  }
  else
  {
    // temporarily use destBuffer usedFlags for indicating lack of two streams
    destBuffer->getUsedFlags()->setNotIntersect(sourceBuffer0->getUsedFlags(), sourceBuffer1->getUsedFlags());

    NMP::vpu::vector4_t lvc = NMP::vpu::set4i(numAlphaValues);
    channelID = 0;

    while (channelID < numElements)
    {
      NMP::vpu::vector4_t v0[4];
      NMP::vpu::vector4_t v1[4];

      NMP::vpu::vector4_t q0[4];
      NMP::vpu::vector4_t q1[4];

      uint32_t cp[4] = { 0, 0, 0, 0 };

      uint32_t numChannels = InterpSIMD_Pack4(destBuffer, sourceBuffer0, sourceBuffer1, v0, v1, q0, q1, cp, channelID, numElements, isFull);

      NMP::vpu::vector4_t avc = NMP::vpu::set4f(alphaValues[cp[0]], alphaValues[cp[1]], alphaValues[cp[2]], alphaValues[cp[3]]);
      NMP::vpu::vector4_t cpv = NMP::vpu::set4i(cp[0], cp[1], cp[2], cp[3]);
      boneas = mul_4f(calpha, NMP::vpu::sel4cmpiLT(cpv, lvc, avc, NMP::vpu::one4f()));

      // process 4 vecs
      NMP::vpu::Vector3MP vv0, vv1, vd;

      vv0.pack(v0[0], v0[1], v0[2], v0[3]);
      vv1.pack(v1[0], v1[1], v1[2], v1[3]);

      vd.interp(vv0, vv1, boneas);

      // process 4 quats
      NMP::vpu::QuatMP qv0, qv1, qd;

      qv0.pack(q0[0], q0[1], q0[2], q0[3]);
      qv1.pack(q1[0], q1[1], q1[2], q1[3]);

      qd.interpBlend(qv0, qv1, boneas);

      // store
      vd.unpack(v0[0], v0[1], v0[2], v0[3]);
      qd.unpack(q0[0], q0[1], q0[2], q0[3]);

      uint32_t ch = 0;
      while (numChannels--)
      {
        NMP::vpu::store4f((float*)&((NMP::Vector3*)destBuffer->getElementData(0))[cp[ch]], v0[ch]);
        NMP::vpu::store4f((float*)&((NMP::Quat*)destBuffer->getElementData(1))[cp[ch]], q0[ch]);
        ch++;
      }
    }
  }

  NMP_ASSERT(channelID >= numElements);

  // set used flags, and full status
  destBuffer->getUsedFlags()->setUnion(sourceBuffer0->getUsedFlags(), sourceBuffer1->getUsedFlags());
  destBuffer->setFullFlag(isFull);

  NMP_ASSERT(destBuffer->isFull() == destBuffer->getUsedFlags()->calculateAreAllSet());

  NM_BLENDOPS_END_PROFILING();
}

//----------------------------------------------------------------------------------------------------------------------
void BlendOpsSIMD::addQuatAddPosPartialFeathered(
  NMP::DataBuffer*       destBuffer,
  const NMP::DataBuffer* sourceBuffer0,
  const NMP::DataBuffer* sourceBuffer1,
  const float            alpha,
  const uint32_t         numAlphaValues,
  const float*           alphaValues)
{
  NMP_ASSERT(destBuffer && sourceBuffer0 && sourceBuffer1 && alphaValues);
  NMP_ASSERT((destBuffer != sourceBuffer0) && (destBuffer != sourceBuffer1));
  NMP_ASSERT(destBuffer->getLength() >= sourceBuffer0->getLength());

  NM_BLENDOPS_BEGIN_PROFILING("BLENDOPPS_addQuatAddPosPartialFeathered");

  NMP::vpu::vector4_t calpha = NMP::vpu::load1fp(&alpha);
  NMP::vpu::vector4_t boneas;

  uint32_t  channelID;
  uint32_t  numElements = destBuffer->getLength();

  if (sourceBuffer0->isFull() && sourceBuffer1->isFull())
  {
    channelID = 0;

    while (channelID < numElements)
    {
      BLEND_SIMD_PREFETCH

      if (numAlphaValues > channelID)
        boneas = mul_4f(calpha, NMP::vpu::load4f((float*)&alphaValues[channelID]));
      else
        boneas = calpha;

      NMP::vpu::Vector3MP v0, v1, vd;

      v0.pack((float*)&((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID]);
      v1.pack((float*)&((NMP::Vector3*)sourceBuffer1->getElementData(0))[channelID]);

      NMP::vpu::QuatMP q0, q1, qd;

      q0.pack((float*)&((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID]);
      q1.pack((float*)&((NMP::Quat*)sourceBuffer1->getElementData(1))[channelID]);

      vd.interp(v0, v1, boneas);
      qd.additiveBlend(q0, q1, boneas);

      vd.unpack((float*)&((NMP::Vector3*)destBuffer->getElementData(0))[channelID]);
      qd.unpack((float*)&((NMP::Quat*)destBuffer->getElementData(1))[channelID]);

      // next
      channelID += 4;
    }
  }
  else
  {
    // temporarily use destBuffer usedFlags for indicating lack of two streams
    destBuffer->getUsedFlags()->setNotIntersect(sourceBuffer0->getUsedFlags(), sourceBuffer1->getUsedFlags());

    NMP::vpu::vector4_t lvc = NMP::vpu::set4i(numAlphaValues);
    channelID = 0;

    while (channelID < numElements)
    {
      NMP::vpu::vector4_t v0[4];
      NMP::vpu::vector4_t v1[4];

      NMP::vpu::vector4_t q0[4];
      NMP::vpu::vector4_t q1[4];

      uint32_t cp[4] = { 0, 0, 0, 0 };

      uint32_t numChannels = AddSIMD_Pack4(destBuffer, sourceBuffer0, sourceBuffer1, v0, v1, q0, q1, cp, channelID, numElements);

      NMP::vpu::vector4_t avc = NMP::vpu::set4f(alphaValues[cp[0]], alphaValues[cp[1]], alphaValues[cp[2]], alphaValues[cp[3]]);
      NMP::vpu::vector4_t cpv = NMP::vpu::set4i(cp[0], cp[1], cp[2], cp[3]);
      boneas = mul_4f(calpha, NMP::vpu::sel4cmpiLT(cpv, lvc, avc, NMP::vpu::one4f()));

      // process 4 vecs
      NMP::vpu::Vector3MP vv0, vv1, vd;

      vv0.pack(v0[0], v0[1], v0[2], v0[3]);
      vv1.pack(v1[0], v1[1], v1[2], v1[3]);

      vd.interp(vv0, vv1, boneas);

      // process 4 quats
      NMP::vpu::QuatMP qv0, qv1, qd;

      qv0.pack(q0[0], q0[1], q0[2], q0[3]);
      qv1.pack(q1[0], q1[1], q1[2], q1[3]);

      qd.additiveBlend(qv0, qv1, boneas);

      // store
      vd.unpack(v0[0], v0[1], v0[2], v0[3]);
      qd.unpack(q0[0], q0[1], q0[2], q0[3]);

      uint32_t ch = 0;
      while (numChannels--)
      {
        NMP::vpu::store4f((float*)&((NMP::Vector3*)destBuffer->getElementData(0))[cp[ch]], v0[ch]);
        NMP::vpu::store4f((float*)&((NMP::Quat*)destBuffer->getElementData(1))[cp[ch]], q0[ch]);
        ch++;
      }
    }
  }

  NMP_ASSERT(channelID >= numElements);

  // set used flags, and full status
  destBuffer->getUsedFlags()->copy(sourceBuffer0->getUsedFlags());
  destBuffer->setFullFlag(sourceBuffer0->isFull());

  NMP_ASSERT(destBuffer->isFull() == destBuffer->getUsedFlags()->calculateAreAllSet());

  NM_BLENDOPS_END_PROFILING();
}

//----------------------------------------------------------------------------------------------------------------------
void BlendOpsSIMD::addQuatLeavePosPartialFeathered(
  NMP::DataBuffer*       destBuffer,
  const NMP::DataBuffer* sourceBuffer0,
  const NMP::DataBuffer* sourceBuffer1,
  const float            alpha,
  const uint32_t         numAlphaValues,
  const float*           alphaValues)
{
  NMP_ASSERT(destBuffer && sourceBuffer0 && sourceBuffer1 && alphaValues);
  NMP_ASSERT((destBuffer != sourceBuffer0) && (destBuffer != sourceBuffer1));
  NMP_ASSERT(destBuffer->getLength() >= sourceBuffer0->getLength());

  NM_BLENDOPS_BEGIN_PROFILING("BLENDOPPS_addQuatLeavePosPartialFeathered");

  NMP::vpu::vector4_t calpha = NMP::vpu::load1fp(&alpha);
  NMP::vpu::vector4_t boneas;

  uint32_t  channelID;
  uint32_t  numElements = destBuffer->getLength();

  if (sourceBuffer0->isFull() && sourceBuffer1->isFull())
  {
    channelID = 0;

    while (channelID < numElements)
    {
      BLEND_SIMD_PREFETCH

      if (numAlphaValues > channelID)
        boneas = mul_4f(calpha, NMP::vpu::load4f((float*)&alphaValues[channelID]));
      else
        boneas = calpha;

      // pos copy only
      NMP::vpu::vector4_t v00, v01, v02, v03;
      NMP::vpu::load4v((float*)&((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID], v00, v01, v02, v03);

      // quats
      NMP::vpu::QuatMP q0, q1, qd;

      q0.pack((float*)&((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID]);
      q1.pack((float*)&((NMP::Quat*)sourceBuffer1->getElementData(1))[channelID]);

      // store 4 vecs
      NMP::vpu::store4v((float*)&((NMP::Vector3*)destBuffer->getElementData(0))[channelID], v00, v01, v02, v03);

      // process 4 quats
      qd.additiveBlend(q0, q1, boneas);

      qd.unpack((float*)&((NMP::Quat*)destBuffer->getElementData(1))[channelID]);

      // next
      channelID += 4;
    }
  }
  else
  {
    // temporarily use destBuffer usedFlags for indicating lack of two streams
    destBuffer->getUsedFlags()->setNotIntersect(sourceBuffer0->getUsedFlags(), sourceBuffer1->getUsedFlags());

    channelID = 0;

    while (channelID < numElements)
    {
      NMP::vpu::vector4_t v0[4];
      NMP::vpu::vector4_t q0[4];
      NMP::vpu::vector4_t q1[4];

      uint32_t cp[4] = { 0, 0, 0, 0 };
      float    bt[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

      uint32_t i = 0;
      while (i < 4 && channelID < numElements)
      {
        if (destBuffer->hasChannel(channelID))
        {
          if (sourceBuffer0->hasChannel(channelID))
          {
            // Only buffer 0 has this channel so copy it into the output buffer.
            ((NMP::Vector3*)destBuffer->getElementData(0))[channelID] = ((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID];
            ((NMP::Quat*)destBuffer->getElementData(1))[channelID] = ((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID];
          }
        }
        else
        {
          v0[i] = NMP::vpu::load4f((float*) & ((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID]);
          q0[i] = NMP::vpu::load4f((float*) & ((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID]);
          q1[i] = NMP::vpu::load4f((float*) & ((NMP::Quat*)sourceBuffer1->getElementData(1))[channelID]);
          cp[i] = channelID;
          if (numAlphaValues > channelID)
            bt[i] = alphaValues[channelID];
          i++;
        }
        channelID++;
      }
      boneas = mul_4f(calpha, NMP::vpu::loadu4f(bt));

      // process 4 quats
      NMP::vpu::QuatMP qv0, qv1, qd;

      qv0.pack(q0[0], q0[1], q0[2], q0[3]);
      qv1.pack(q1[0], q1[1], q1[2], q1[3]);

      qd.additiveBlend(qv0, qv1, boneas);

      // store    
      qd.unpack(q0[0], q0[1], q0[2], q0[3]);
      uint32_t ch = 0;
      while (i--)
      {
        NMP::vpu::store4f((float*)&((NMP::Vector3*)destBuffer->getElementData(0))[cp[ch]], v0[ch]);
        NMP::vpu::store4f((float*)&((NMP::Quat*)destBuffer->getElementData(1))[cp[ch]], q0[ch]);
        ch++;
      }
    }
  }

  NMP_ASSERT(channelID >= numElements);

  // set used flags, and full status
  destBuffer->getUsedFlags()->copy(sourceBuffer0->getUsedFlags());
  destBuffer->setFullFlag(sourceBuffer0->isFull());

  NMP_ASSERT(destBuffer->isFull() == destBuffer->getUsedFlags()->calculateAreAllSet());

  NM_BLENDOPS_END_PROFILING();
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
