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
#include "morpheme/AnimSource/mrAnimSourceUtils.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

uint32_t s_maxAnimSectionSize = (1024 * 1024 * 4);

//----------------------------------------------------------------------------------------------------------------------
// UnchangingKeyVec3
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void UnchangingKeyVec3::locate()
{
  NMP::endianSwapArray(m_data, 3);
}

//----------------------------------------------------------------------------------------------------------------------
void UnchangingKeyVec3::dislocate()
{
  NMP::endianSwapArray(m_data, 3);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
// SampledPosKey
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void SampledPosKey::locate()
{
  NMP::endianSwap(m_data);
}

//----------------------------------------------------------------------------------------------------------------------
void SampledPosKey::dislocate()
{
  NMP::endianSwap(m_data);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
// SampledQuatKeyTQA
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void SampledQuatKeyTQA::locate()
{
  NMP::endianSwapArray(m_data, 3);
}

//----------------------------------------------------------------------------------------------------------------------
void SampledQuatKeyTQA::dislocate()
{
  NMP::endianSwapArray(m_data, 3);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
// QuantisationScaleAndOffsetVec3
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void QuantisationScaleAndOffsetVec3::locate()
{
  NMP::endianSwapArray(m_qOffset, 3);
  NMP::endianSwapArray(m_qScale, 3);
}

//----------------------------------------------------------------------------------------------------------------------
void QuantisationScaleAndOffsetVec3::dislocate()
{
  NMP::endianSwapArray(m_qScale, 3);
  NMP::endianSwapArray(m_qOffset, 3);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
// QuantisationMeanAndSetVec3
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void QuantisationMeanAndSetVec3::locate()
{
  NMP::endianSwapArray(m_qMean, 3);
  NMP::endianSwapArray(m_qSet, 3);
}

//----------------------------------------------------------------------------------------------------------------------
void QuantisationMeanAndSetVec3::dislocate()
{
  NMP::endianSwapArray(m_qSet, 3);
  NMP::endianSwapArray(m_qMean, 3);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
// CompToAnimChannelMap
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
NMP::Memory::Format CompToAnimChannelMap::getMemoryRequirements(uint32_t numChannels)
{
  // Header and map data
  // The component to rig maps are padded to a multiple of 4 for SIMD optimisation.
  // During asset compilation the conglomerate rig is sorted so that the most frequently
  // shared bones in the rig set are placed first. Computing transforms for different
  // LODs is achieved by only decompressing those bones appearing before the highest valid
  // animation channel in the RigToAnimMap. To avoid a secondary test during the decompression
  // loop to check for this condition, an extra entry is instead allocated at the end with the
  // invalid value 0xffff. Since component and anim channels are strictly increasing a single
  // LOD test of the first components of each multiple of 4 is sufficient.
  //
  // See also:
  // MR::SectionDataNSA::getMemoryReqsMeanAndSetVec3
  uint32_t paddedNumChannels = NMP::nmAlignedValue4(numChannels);
  NMP::Memory::Format result(
    sizeof(MR::CompToAnimChannelMap) + sizeof(uint16_t) * paddedNumChannels,
    NMP_NATURAL_TYPE_ALIGNMENT);

  // Multiple of the alignment
  result.size = NMP::Memory::align(result.size, NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::CompToAnimChannelMap* CompToAnimChannelMap::init(
  NMP::Memory::Resource& resource,
  uint32_t numChannels,
  const uint32_t* animChannels)
{
  // Header and map data
  // The component to rig maps are padded to a multiple of 4 for SIMD optimisation.
  // An extra entry is also allocated for faster decompression.
  // See getMemoryRequirements.
  uint32_t paddedNumChannels = NMP::nmAlignedValue4(numChannels);
  NMP::Memory::Format memReqs(
    sizeof(MR::CompToAnimChannelMap) + sizeof(uint16_t) * paddedNumChannels,
    NMP_NATURAL_TYPE_ALIGNMENT);

  CompToAnimChannelMap* result = (CompToAnimChannelMap*) resource.alignAndIncrement(memReqs);
  result->m_numChannels = (uint16_t)numChannels;

  // Anim channel index table
  for (uint32_t i = 0; i < numChannels; ++i)
  {
    result->m_animChannels[i] = (uint16_t)animChannels[i];
  }

  // Invalid entries. Note that the extra allocated entry represents the
  // first component of a multiple of 4 and serves as a sentinel to terminate
  // the decompression loop.
  for (uint32_t i = numChannels; i < paddedNumChannels + 1; ++i)
  {
    result->m_animChannels[i] = 0xffff;
  }

  // Multiple of the alignment
  resource.align(NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void CompToAnimChannelMap::locate()
{
  // Verify alignment
  NMP_ASSERT_MSG(
    NMP_IS_ALIGNED(this, NMP_NATURAL_TYPE_ALIGNMENT),
    "Comp to Anim map must be aligned to %d bytes.",
    NMP_NATURAL_TYPE_ALIGNMENT);

  // The component to rig maps are padded to a multiple of 4 for SIMD optimisation.
  // An extra entry is also allocated for faster decompression.
  // See getMemoryRequirements.
  NMP::endianSwap(m_numChannels);
  uint32_t paddedNumChannels = NMP::nmAlignedValue4(m_numChannels);
  NMP::endianSwapArray(m_animChannels, paddedNumChannels + 1);
}

//----------------------------------------------------------------------------------------------------------------------
void CompToAnimChannelMap::dislocate()
{
  // The component to rig maps are padded to a multiple of 4 for SIMD optimisation.
  // An extra entry is also allocated for faster decompression.
  // See getMemoryRequirements.
  uint32_t paddedNumChannels = NMP::nmAlignedValue4(m_numChannels);
  NMP::endianSwapArray(m_animChannels, paddedNumChannels + 1);
  NMP::endianSwap(m_numChannels);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
CompToAnimChannelMap* CompToAnimChannelMap::relocate(void*& ptr)
{
  // Header and map data
  // The component to rig maps are padded to a multiple of 4 for SIMD optimisation.
  // An extra entry is also allocated for faster decompression.
  // See getMemoryRequirements.
  ptr = (void*) NMP::Memory::align(ptr, NMP_NATURAL_TYPE_ALIGNMENT);
  CompToAnimChannelMap* result = (CompToAnimChannelMap*)ptr;

  uint32_t paddedNumChannels = NMP::nmAlignedValue4(result->m_numChannels);
  size_t memReqs = sizeof(MR::CompToAnimChannelMap) + sizeof(uint16_t) * paddedNumChannels;
  ptr = NMP::Memory::increment(ptr, memReqs);

  // Multiple of the alignment
  ptr = (void*) NMP::Memory::align(ptr, NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
