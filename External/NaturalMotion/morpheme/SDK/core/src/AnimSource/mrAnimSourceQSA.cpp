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
#include "NMPlatform/NMMathUtils.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMProfiler.h"
#include "morpheme/AnimSource/mrAnimSourceQSA.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

AnimFunctionTable AnimSourceQSA::m_functionTable =
{
  AnimSourceQSA::computeAtTime,
  AnimSourceQSA::computeAtTimeSingleTransform,
  AnimSourceQSA::getDuration,
  AnimSourceQSA::getNumChannelSets,
  AnimSourceQSA::getTrajectoryChannelData,
  AnimSourceQSA::getChannelNameTable
};

//----------------------------------------------------------------------------------------------------------------------
// AnimSourceQSA functions.
//----------------------------------------------------------------------------------------------------------------------
AnimSourceQSA::AnimSourceQSA()
{
  // This constructor should never be called at runtime.
  NMP_ASSERT_FAIL();
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AnimSourceQSA::knotInterval(
  uint16_t        numKnots,
  const uint16_t* knots,
  uint16_t        animFrameIndex)
{
  NMP_ASSERT(numKnots >= 2);
  NMP_ASSERT(knots);
  NMP_ASSERT(animFrameIndex >= knots[0] && animFrameIndex <= knots[numKnots-1]);

  uint32_t intv, low;
  uint32_t high = numKnots - 2;

  // Special case for the last knot interval
  if (animFrameIndex >= knots[high])
  {
    intv = high;
  }
  else
  {
    // Determine the correct interval by binary search
    low = 0;
    intv = high >> 1;
    for (;;)
    {
      if (animFrameIndex < knots[intv])
      {
        high = intv;
      }
      else if (animFrameIndex >= knots[intv+1])
      {
        low = intv;
      }
      else
      {
        break; // Found the interval
      }
      intv = (high + low) >> 1;
    }
  }

  return intv;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceQSA::initOutputBuffer(
  const AnimRigDef* rig,
  InternalDataQSA* NMP_UNUSED(internalData),
  NMP::DataBuffer* outputTransformBuffer) const
{
  NMP_ASSERT(rig);
  NMP_ASSERT(outputTransformBuffer);

  if (m_useDefaultPose)
  {
    // Initialise the output buffer with the default pose
    if (m_generateDeltas)
    {
      outputTransformBuffer->setPosQuatBufferIdentity();
    }
    else
    {
      const AttribDataTransformBuffer* bindPose = rig->getBindPose();
      NMP_ASSERT(bindPose);
      NMP_ASSERT(bindPose->m_transformBuffer);
      bindPose->m_transformBuffer->copyTo(outputTransformBuffer);
    }
  }
  // Otherwise all rig channels must be stored in the compressed animation
}

//----------------------------------------------------------------------------------------------------------------------
#if defined(NMP_PLATFORM_SIMD) && !defined(NM_HOST_CELL_PPU) && !defined(NM_HOST_CELL_SPU)
void AnimSourceQSA::convertToLocalSpace(
  const AnimRigDef* rig,
  const AnimToRigTableMap* animToRigTableMap,
  InternalDataQSA* internalData,
  NMP::DataBuffer* outputTransformBuffer) const
{
  NMP_ASSERT(rig);
  NMP_ASSERT(animToRigTableMap);
  NMP_ASSERT(internalData);
  NMP_ASSERT(outputTransformBuffer);
  const NMP::Hierarchy* hierarchy = rig->getHierarchy();
  NMP_ASSERT(hierarchy);
  uint32_t numRigBones = outputTransformBuffer->getLength();
  NMP::Quat* qOut = outputTransformBuffer->getPosQuatChannelQuat(0);
  NMP_ASSERT(qOut);
  uint8_t* chanQuatSpaces = internalData->m_chanQuatSpaces;
  NMP_ASSERT(chanQuatSpaces);

  //-----------------------
  // Set the worldspace quat of the parent rig bone to the topmost animation channel
  // that has keyframe data. This may not be the CharacterWorldSpaceTM rig channel (0).
  // This has been stored since animations may contain keyframe data for specific body
  // parts only (head only, left arm only, etc). This would mean that we would have to
  // accumulate all bind pose transforms above the topmost bone channel with keyframe
  // data up until the character root.
  uint16_t animChannelRootID = m_channelSetsInfo->getWorldspaceRootID();
  uint32_t rigChannelRootID = animToRigTableMap->getAnimToRigMapEntry(animChannelRootID);
  NMP::Quat rootQuat = qOut[rigChannelRootID];
  qOut[rigChannelRootID] = m_channelSetsInfo->getWorldspaceRootQuat();
  chanQuatSpaces[rigChannelRootID] = QSA_CHAN_WORLD_FLAG;

  // Paranoid check to ensure that the topmost CharacterWorldSpaceTM channel is
  // set to worldspace to avoid a possible infinite loop when unaccumulating the
  // transforms.
  chanQuatSpaces[0] = QSA_CHAN_WORLD_FLAG;

  //-----------------------
  NMP::vpu::vector4_t conVec = NMP::vpu::negOne4f();
  conVec = NMP::vpu::setwf(conVec, NMP::vpu::one4f());

  // Recover the local-space quaternion channel transforms
  for (uint32_t rigChannelIndex = numRigBones - 1; rigChannelIndex > rigChannelRootID; --rigChannelIndex)
  {
    // Check if the quaternion data corresponds to a worldspace orientation
    if (chanQuatSpaces[rigChannelIndex] == QSA_CHAN_WORLD_FLAG)
    {
      // Compute the parent worldspace quaternion. We may have to accumulate any local
      // channels, or even local channels without data (must use bind pose).
      uint32_t rigParentIndex = hierarchy->getParentIndex(rigChannelIndex);
      NMP_ASSERT(rigParentIndex < outputTransformBuffer->getLength());

      // Accumulate the local transforms
      NMP::vpu::vector4_t parentQuatWS = NMP::vpu::load4f((float*)&qOut[rigParentIndex]);
      while (chanQuatSpaces[rigParentIndex] == QSA_CHAN_LOCAL_FLAG)
      {
        // Get the next parent and its flags
        rigParentIndex = hierarchy->getParentIndex(rigParentIndex);
        NMP_ASSERT(rigParentIndex != INVALID_HIERARCHY_INDEX);
        NMP_ASSERT(rigParentIndex < outputTransformBuffer->getLength());

        // Accumulate the transform
        NMP::vpu::vector4_t PParentQuatWS = NMP::vpu::load4f((float*)&qOut[rigParentIndex]);
        parentQuatWS = NMP::vpu::qqMul(PParentQuatWS, parentQuatWS);
      }

      // Recover the local space transform of the current rig bone
      parentQuatWS = NMP::vpu::mul4f(parentQuatWS, conVec);
      NMP::vpu::vector4_t rigChanQuat = NMP::vpu::load4f((float*)&qOut[rigChannelIndex]);
      parentQuatWS = NMP::vpu::qqMul(parentQuatWS, rigChanQuat);
      NMP::vpu::store4f((float*)&qOut[rigChannelIndex], parentQuatWS);
    }
  }

  // The parent rig bone to the topmost animation channel may have contained decompressed
  // unchanging data. We must restore its original state.
  qOut[rigChannelRootID] = rootQuat;
}
#else
//----------------------------------------------------------------------------------------------------------------------
void AnimSourceQSA::convertToLocalSpace(
  const AnimRigDef* rig,
  const AnimToRigTableMap* animToRigTableMap,
  InternalDataQSA* internalData,
  NMP::DataBuffer* outputTransformBuffer) const
{
  NMP_ASSERT(rig);
  NMP_ASSERT(internalData);
  NMP_ASSERT(outputTransformBuffer);
  const NMP::Hierarchy* hierarchy = rig->getHierarchy();
  NMP_ASSERT(hierarchy);
  uint32_t numRigBones = outputTransformBuffer->getLength();
  NMP::Quat* qOut = outputTransformBuffer->getPosQuatChannelQuat(0);
  NMP_ASSERT(qOut);
  uint8_t* chanQuatSpaces = internalData->m_chanQuatSpaces;
  NMP_ASSERT(chanQuatSpaces);

  //-----------------------
  // Set the worldspace quat of the parent rig bone to the topmost animation channel
  // that has keyframe data. This may not be the CharacterWorldSpaceTM rig channel (0).
  // This has been stored since animations may contain keyframe data for specific body
  // parts only (head only, left arm only, etc). This would mean that we would have to
  // accumulate all bind pose transforms above the topmost bone channel with keyframe
  // data up until the character root.
  uint16_t animChannelRootID = m_channelSetsInfo->getWorldspaceRootID();
  uint32_t rigChannelRootID = animToRigTableMap->getAnimToRigMapEntry(animChannelRootID);
  NMP::Quat rootQuat = qOut[rigChannelRootID];
  qOut[rigChannelRootID] = m_channelSetsInfo->getWorldspaceRootQuat();
  chanQuatSpaces[rigChannelRootID] = QSA_CHAN_WORLD_FLAG;

  // Paranoid check to ensure that the topmost CharacterWorldSpaceTM channel is
  // set to worldspace to avoid a possible infinite loop when unaccumulating the
  // transforms.
  chanQuatSpaces[0] = QSA_CHAN_WORLD_FLAG;

  //-----------------------
  // Recover the local-space quaternion channel transforms
  for (uint32_t rigChannelIndex = numRigBones - 1; rigChannelIndex > rigChannelRootID; --rigChannelIndex)
  {
    // Check if the quaternion data corresponds to a worldspace orientation
    if (chanQuatSpaces[rigChannelIndex] == QSA_CHAN_WORLD_FLAG)
    {
      // Compute the parent worldspace quaternion. We may have to accumulate any local
      // channels, or even local channels without data (must use bind pose).
      uint32_t rigParentIndex = hierarchy->getParentIndex(rigChannelIndex);
      NMP_ASSERT(rigParentIndex < outputTransformBuffer->getLength());

      // Accumulate the local transforms
      NMP::Quat parentQuatWS = qOut[rigParentIndex];
      while (chanQuatSpaces[rigParentIndex] == QSA_CHAN_LOCAL_FLAG)
      {
        // Get the next parent and its flags
        rigParentIndex = hierarchy->getParentIndex(rigParentIndex);
        NMP_ASSERT(rigParentIndex != INVALID_HIERARCHY_INDEX);
        NMP_ASSERT(rigParentIndex < outputTransformBuffer->getLength());

        // Accumulate the transform
        parentQuatWS = qOut[rigParentIndex] * parentQuatWS;
      }

      // Recover the local space transform of the current rig bone
      parentQuatWS.conjugate();
      qOut[rigChannelIndex] = parentQuatWS * qOut[rigChannelIndex];
    }
  }

  // The parent rig bone to the topmost animation channel may have contained decompressed
  // unchanging data. We must restore its original state.
  qOut[rigChannelRootID] = rootQuat;
}
#endif

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceQSA::computeAtTime(
  const AnimSourceBase* sourceAnimation,       // IN: Animation to sample.
  float                 time,                  // IN: Time at which to sample the animation (seconds).
  const AnimRigDef*     rig,                   // IN: Hierarchy and bind poses
  const RigToAnimMap*   mapFromRigToAnim,      // IN: So that results from sampled anim channels can be
                                               //     stored in the correct rig bone ordered transform buffer slot.
  uint32_t              outputSubsetSize,      // IN: \see outputSubsetArray
  const uint16_t*       outputSubsetArray,     // IN: Channel set values will only be calculated if
                                               //     output indexes appear both in this array and the
                                               //     mapping array. Ignored if size is zero.
  NMP::DataBuffer*      outputTransformBuffer, // OUT: Calculated transform data is stored and returned
                                               //      in this provided set.
  NMP::MemoryAllocator* allocator)
{
  NMP_ASSERT(sourceAnimation);
  NMP_ASSERT(rig);
  NMP_ASSERT(mapFromRigToAnim);
  NMP_ASSERT(outputTransformBuffer);

  // Partial channel decompression is not yet supported for this animation format
  NMP_ASSERT(outputSubsetSize == 0);
  (void)outputSubsetSize;
  (void)outputSubsetArray;

  const AnimSourceQSA* compressedSource = static_cast<const AnimSourceQSA*> (sourceAnimation);
  NMP_ASSERT(time >= 0.0 && time <= compressedSource->m_duration);

  NMP_ASSERT(mapFromRigToAnim->getRigToAnimMapType() == RigToAnimMap::AnimToRig);
  const AnimToRigTableMap* animToRigTableMap = (const AnimToRigTableMap*) mapFromRigToAnim->getRigToAnimMapData();
  NMP_ASSERT(animToRigTableMap);

  NM_ANIM_BEGIN_PROFILING("QSA_DECOMPRESS");

  //--------------------------------
  // Allocate the memory for the internal QSA data
  uint32_t numRigBones = mapFromRigToAnim->getNumRigBones();
  NMP::Memory::Format memReqsInternalData = InternalDataQSA::getMemoryRequirements(numRigBones);
  NMP::Memory::Resource memResInternalData = NMPAllocatorAllocateFromFormat(allocator, memReqsInternalData);
  InternalDataQSA* internalData = InternalDataQSA::init(memResInternalData, numRigBones);
  if (compressedSource->m_useWorldSpace)
    internalData->m_useSpace = QSA_CHAN_WORLD_FLAG;
  else
    internalData->m_useSpace = QSA_CHAN_LOCAL_FLAG;

  // Initialise the bind pose and channel flags
  NM_ANIM_BEGIN_PROFILING("QSA_INIT_OUTPUT");
  compressedSource->initOutputBuffer(rig, internalData, outputTransformBuffer);
  NM_ANIM_END_PROFILING();

  //--------------------------------
  // Decompress the unchanging compression channels
  const ChannelSetInfoQSA* unchangingData = compressedSource->m_channelSetsInfo;
  NMP_ASSERT(unchangingData);

  NM_ANIM_BEGIN_PROFILING("QSA_UNCHANGING_CHANS");

  //--------------------------------
  // Unchanging pos
  NM_ANIM_BEGIN_PROFILING("NSA_UNCHANGING_POS");
  NMP_ASSERT(compressedSource->m_unchangingPosCompToAnimMap);

  unchangingData->unchangingPosDecompress(
    animToRigTableMap,
    compressedSource->m_unchangingPosCompToAnimMap,
    outputTransformBuffer);

  NM_ANIM_END_PROFILING(); // NSA_UNCHANGING_POS

  //--------------------------------
  // Unchanging quat
  NM_ANIM_BEGIN_PROFILING("NSA_UNCHANGING_QUAT");
  NMP_ASSERT(compressedSource->m_unchangingQuatCompToAnimMap);

  unchangingData->unchangingQuatDecompress(
    animToRigTableMap,
    compressedSource->m_unchangingQuatCompToAnimMap,
    outputTransformBuffer);

  NM_ANIM_END_PROFILING(); // NSA_UNCHANGING_QUAT
  NM_ANIM_END_PROFILING(); // NSA_UNCHANGING_CHANS
  
  if (compressedSource->m_numSections > 0)
  {
    NM_ANIM_BEGIN_PROFILING("QSA_SECTION_DATA");

    //--------------------------------
    float frame = time * compressedSource->m_sampleFrequency;  
    uint32_t animFrameIndex = (uint32_t)frame;
    float interpolant = frame - animFrameIndex;

    // Find which section this frame lies within.
    uint32_t sectionIndex = compressedSource->findSectionIndexFromFrameIndex(animFrameIndex);
    NMP_ASSERT(sectionIndex < compressedSource->m_numSections);

    // Get the section data
    void* sectionDataRef = compressedSource->m_sections[sectionIndex].getData(
      NMP::Memory::Format(compressedSource->m_sectionsInfo[sectionIndex].getSectionSize(), NMP_VECTOR_ALIGNMENT),
      allocator);
    NMP_ASSERT(sectionDataRef);

#ifdef NM_HOST_CELL_SPU
    ((AnimSectionQSA*)sectionDataRef)->relocate();
#endif // NM_HOST_CELL_SPU
    const AnimSectionQSA* sectionData = (const AnimSectionQSA*)sectionDataRef;

    //--------------------------------
    // Sampled channels
    NM_ANIM_BEGIN_PROFILING("QSA_INTERPOLANTS");

    // Adjust the sample frame index if it is the last frame in order to
    // avoid interpolating beyond the last frame
    uint32_t sectionAnimStartFrame = sectionData->getSectionAnimStartFrame();
    uint32_t sectionNumAnimFrames = sectionData->getSectionNumAnimFrames();
    uint32_t lastSectionFrame = sectionNumAnimFrames - 1;
    uint32_t sectionFrameIndex = animFrameIndex - sectionAnimStartFrame;

    uint32_t sampledIndex = sectionFrameIndex;
    float sampledUParam = interpolant;
    if (sampledIndex == lastSectionFrame)
    {
      sampledIndex--;
      sampledUParam = 1.0f;
    }

    internalData->m_sampledIndex = sampledIndex;
    internalData->m_sampledUParam = sampledUParam;
    

    //-----------------------
    // Spline channel interpolants
    uint16_t splineNumKnots = sectionData->getSplineNumKnots();
    if (splineNumKnots > 0)
    {
      // Compute the interval on which the animation frame index lies with the knot vector
      const uint16_t* splineKnots = sectionData->getSplineKnots();
      uint32_t knotIndex = knotInterval(splineNumKnots, splineKnots, (uint16_t)animFrameIndex);
      internalData->m_knotIndex = knotIndex;

      // Compute the u-parameter on the specified knot span
      float u = ((float)(animFrameIndex - splineKnots[knotIndex]) + interpolant) / (float)(splineKnots[knotIndex + 1] - splineKnots[knotIndex]);
      internalData->m_splineUParam = u;

      // Basis function info
      float omu = 1.0f - u;
      float omu2 = omu * omu;
      float u2 = u * u;
      float u3 = u2 * u;
      float omu3 = omu2 * omu;

      // Compute the pos spline basis functions
      internalData->m_splinePosBasis[0] = omu3;
      internalData->m_splinePosBasis[1] = 3.0f * u * omu2;
      internalData->m_splinePosBasis[2] = 3.0f * u2 * omu;
      internalData->m_splinePosBasis[3] = u3;

      // Compute the quat spline basis functions
      internalData->m_splineQuatBasis[0] = 1.0f;
      internalData->m_splineQuatBasis[1] = 1.0f - omu3;
      internalData->m_splineQuatBasis[2] = 3.0f * u2 - 2.0f * u3;
      internalData->m_splineQuatBasis[3] = u3;
    }
    NM_ANIM_END_PROFILING(); // QSA_INTERPOLANTS
    
    //--------------------------------
    // Decompress the sampled pos channels in the section
    NM_ANIM_BEGIN_PROFILING("QSA_SAMPLED_POS");

    sectionData->sampledPosDecompress(
      animToRigTableMap,
      compressedSource->m_sampledPosCompToAnimMap,
      internalData,
      outputTransformBuffer);

    NM_ANIM_END_PROFILING(); // QSA_SAMPLED_POS
    
    //--------------------------------
    // Decompress the sampled quat channels in the section
    NM_ANIM_BEGIN_PROFILING("QSA_SAMPLED_QUAT");

    sectionData->sampledQuatDecompress(
      animToRigTableMap,
      compressedSource->m_sampledQuatCompToAnimMap,
      internalData,
      outputTransformBuffer);

    NM_ANIM_END_PROFILING(); // QSA_SAMPLED_QUAT

    //--------------------------------
    // Decompress the spline pos channels in the section
    NM_ANIM_BEGIN_PROFILING("QSA_SPLINE_POS");

    sectionData->splinePosDecompress(
      animToRigTableMap,
      compressedSource->m_splinePosCompToAnimMap,
      internalData,
      outputTransformBuffer);

    NM_ANIM_END_PROFILING(); // QSA_SPLINE_POS

    //--------------------------------
    // Decompress the spline quat channels in the section
    NM_ANIM_BEGIN_PROFILING("QSA_SPLINE_QUAT");

    sectionData->splineQuatDecompress(
      animToRigTableMap,
      compressedSource->m_splineQuatCompToAnimMap,
      internalData,
      outputTransformBuffer);

    NM_ANIM_END_PROFILING(); // QSA_SPLINE_QUAT

    NM_ANIM_END_PROFILING(); // QSA_SECTION_DATA
  }

  //-----------------------
  // Compute the local space quat transforms
  if (compressedSource->m_useWorldSpace)
  {
    NM_ANIM_BEGIN_PROFILING("QSA_CONVERT_TO_LOCAL");
    compressedSource->convertToLocalSpace(rig, animToRigTableMap, internalData, outputTransformBuffer);
    NM_ANIM_END_PROFILING();
  }  

  // Tidy up
  allocator->memFree(internalData);

  //--------------------------------
  // Set the channel used flags required for this animation rig. Make sure the first entry, which
  // not specified in animation files, is initialised to the identity and flagged as used.
  outputTransformBuffer->getPosQuatChannelPos(0)->setToZero();
  outputTransformBuffer->getPosQuatChannelQuat(0)->identity();
  outputTransformBuffer->padOutPosQuat();
  outputTransformBuffer->getUsedFlags()->copy(mapFromRigToAnim->getUsedFlags());
  outputTransformBuffer->setChannelUsed(0);
  outputTransformBuffer->calculateFullFlag();
  NMP_ASSERT(outputTransformBuffer->isFull() == (mapFromRigToAnim->getNumRigBones() - animToRigTableMap->getNumUsedEntries() <= 1));

  NM_ANIM_END_PROFILING(); // QSA_DECOMPRESS
}

//----------------------------------------------------------------------------------------------------------------------
// Samples the request source animation channel at the specified time through its playback duration.
void AnimSourceQSA::computeAtTimeSingleTransform(
  const AnimSourceBase* sourceAnimation,  // IN: Animation to sample.
  float                 time,             // IN: Time at which to sample the animation (seconds).
  uint32_t              rigChannelIndex,  // IN: Index of the rig bone to evaluate
  const AnimRigDef*     rig,              // IN: Hierarchy and bind poses
  const RigToAnimMap*   mapFromRigToAnim, // IN: So that results from sampled anim channels can be
                                          //     stored in the correct rig bone ordered transform buffer slot.
  NMP::Vector3*         pos,              // OUT: The resulting position is filled in here.
  NMP::Quat*            quat,             // OUT: The resulting orientation is filled in here.
  NMP::MemoryAllocator* allocator)
{
  (void)sourceAnimation;
  (void)time;
  (void)rigChannelIndex;
  (void)rig;
  (void)mapFromRigToAnim;
  (void)pos;
  (void)quat;
  (void)allocator;
  NMP_ASSERT_FAIL(); // Not supported
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AnimSourceQSA::findSectionIndexFromFrameIndex(uint32_t frameIndex) const
{
  NMP_ASSERT(m_numSections);
  for (uint32_t i = 1; i < m_numSections; ++i)
  {
    if (frameIndex < m_sectionsInfo[i].getStartFrame())
    {
      return i - 1;
    }
  }
  return m_numSections - 1;
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AnimSourceQSA::locate()
{
  // Verify alignment
  NMP_ASSERT_MSG(
    NMP_IS_ALIGNED(this, NMP_VECTOR_ALIGNMENT),
    "Animation sources must be aligned to %d bytes.",
    NMP_VECTOR_ALIGNMENT);

  //-----------------------
  // Header information
  AnimSourceBase::locate();
  m_funcTable = &m_functionTable;
  NMP::endianSwap(m_duration);
  NMP::endianSwap(m_sampleFrequency);
  NMP::endianSwap(m_numChannelSets);
  NMP::endianSwap(m_numSections);
  NMP::endianSwap(m_useDefaultPose);
  NMP::endianSwap(m_useWorldSpace);
  NMP::endianSwap(m_generateDeltas);
  
  //-----------------------
  // Compression to animation channel maps
  NMP::endianSwap(m_maxNumCompChannels);
  REFIX_SWAP_PTR(CompToAnimChannelMap, m_unchangingPosCompToAnimMap);
  m_unchangingPosCompToAnimMap->locate();
  REFIX_SWAP_PTR(CompToAnimChannelMap, m_unchangingQuatCompToAnimMap);
  m_unchangingQuatCompToAnimMap->locate();
  REFIX_SWAP_PTR(CompToAnimChannelMap, m_sampledPosCompToAnimMap);
  m_sampledPosCompToAnimMap->locate();
  REFIX_SWAP_PTR(CompToAnimChannelMap, m_sampledQuatCompToAnimMap);
  m_sampledQuatCompToAnimMap->locate();
  REFIX_SWAP_PTR(CompToAnimChannelMap, m_splinePosCompToAnimMap);
  m_splinePosCompToAnimMap->locate();
  REFIX_SWAP_PTR(CompToAnimChannelMap, m_splineQuatCompToAnimMap);
  m_splineQuatCompToAnimMap->locate();

  //-----------------------
  // Sections information (i.e. start frame and section size)
  REFIX_SWAP_PTR(AnimSectionInfoQSA, m_sectionsInfo);
  NMP_ASSERT(NMP_IS_ALIGNED(m_sectionsInfo, NMP_NATURAL_TYPE_ALIGNMENT));
  for (uint32_t i = 0; i < m_numSections; ++i)
    m_sectionsInfo[i].locate();

  //-----------------------
  // Sections DataRef table (pointers to sections)
  REFIX_SWAP_PTR(DataRef, m_sections);
  NMP_ASSERT(NMP_IS_ALIGNED(m_sections, NMP_NATURAL_TYPE_ALIGNMENT));

  //-----------------------
  // Channel sets quantisation information
  REFIX_SWAP_PTR(ChannelSetInfoQSA, m_channelSetsInfo);
  m_channelSetsInfo->locate();

  //-----------------------
  // Section data (DMA alignment)
  for (uint32_t i = 0; i < m_numSections; ++i)
  {
    REFIX_SWAP_PTR(void, m_sections[i].m_data);
    AnimSectionQSA* section = (AnimSectionQSA*) m_sections[i].m_data;
    section->locate();
  }

  //-----------------------
  // Trajectory (DMA alignment)
  if (m_trajectoryData)
  {
    REFIX_SWAP_PTR(TrajectorySourceQSA, m_trajectoryData);
    m_trajectoryData->locate();
  }

  //-----------------------
  // Channel name table
  if (m_channelNames)
  {
    REFIX_SWAP_PTR(NMP::OrderedStringTable, m_channelNames);
    m_channelNames->locate();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceQSA::dislocate()
{
  //-----------------------
  // Channel name table
  if (m_channelNames)
  {
    m_channelNames->dislocate();
    UNFIX_SWAP_PTR(NMP::OrderedStringTable, m_channelNames);
  }

  //-----------------------
  // Trajectory
  if (m_trajectoryData)
  {
    m_trajectoryData->dislocate();
    UNFIX_SWAP_PTR(TrajectorySourceQSA, m_trajectoryData);
  }

  //-----------------------
  // Sections
  for (uint32_t i = 0; i < m_numSections; ++i)
  {
    AnimSectionQSA* section = (AnimSectionQSA*)m_sections[i].m_data;
    section->dislocate();
    UNFIX_SWAP_PTR(void, m_sections[i].m_data);
  }

  //-----------------------
  // Channel sets quantisation information
  m_channelSetsInfo->dislocate();
  UNFIX_SWAP_PTR(ChannelSetInfoQSA, m_channelSetsInfo);

  //-----------------------
  // Sections DataRef table (pointers to sections)
  UNFIX_SWAP_PTR(DataRef, m_sections);

  //-----------------------
  // Sections information (i.e. start frame and section size)
  for (uint32_t i = 0; i < m_numSections; ++i)
    m_sectionsInfo[i].dislocate();
  UNFIX_SWAP_PTR(AnimSectionInfoQSA, m_sectionsInfo);
  
  //-----------------------
  // Compression to animation channel maps
  m_splineQuatCompToAnimMap->dislocate();
  UNFIX_SWAP_PTR(CompToAnimChannelMap, m_splineQuatCompToAnimMap);
  m_splinePosCompToAnimMap->dislocate();
  UNFIX_SWAP_PTR(CompToAnimChannelMap, m_splinePosCompToAnimMap);
  m_sampledQuatCompToAnimMap->dislocate();
  UNFIX_SWAP_PTR(CompToAnimChannelMap, m_sampledQuatCompToAnimMap);
  m_sampledPosCompToAnimMap->dislocate();
  UNFIX_SWAP_PTR(CompToAnimChannelMap, m_sampledPosCompToAnimMap);
  m_unchangingQuatCompToAnimMap->dislocate();
  UNFIX_SWAP_PTR(CompToAnimChannelMap, m_unchangingQuatCompToAnimMap);
  m_unchangingPosCompToAnimMap->dislocate();
  UNFIX_SWAP_PTR(CompToAnimChannelMap, m_unchangingPosCompToAnimMap);
  NMP::endianSwap(m_maxNumCompChannels);

  //-----------------------
  // Header information
  NMP::endianSwap(m_generateDeltas);
  NMP::endianSwap(m_useWorldSpace);
  NMP::endianSwap(m_useDefaultPose);
  NMP::endianSwap(m_numSections);
  NMP::endianSwap(m_numChannelSets);
  NMP::endianSwap(m_sampleFrequency);
  NMP::endianSwap(m_duration);
  m_funcTable = NULL;
  AnimSourceBase::dislocate();
}
#endif //NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceQSA::relocate()
{
  // Verify alignment
  NMP_ASSERT_MSG(
    NMP_IS_ALIGNED(this, NMP_VECTOR_ALIGNMENT),
    "Animation sources must be aligned to %d bytes.",
    NMP_VECTOR_ALIGNMENT);

  //-----------------------
  // Header information
  void* ptr = this;
  NMP::Memory::Format memReqsHdr(sizeof(AnimSourceQSA), NMP_VECTOR_ALIGNMENT);
  NMP::Memory::alignAndIncrement(ptr, memReqsHdr);

  m_funcTable = &m_functionTable;
  
  //-----------------------
  // Compression to animation channel maps
  m_unchangingPosCompToAnimMap = CompToAnimChannelMap::relocate(ptr);
  m_unchangingQuatCompToAnimMap = CompToAnimChannelMap::relocate(ptr);
  m_sampledPosCompToAnimMap = CompToAnimChannelMap::relocate(ptr);
  m_sampledQuatCompToAnimMap = CompToAnimChannelMap::relocate(ptr);
  m_splinePosCompToAnimMap = CompToAnimChannelMap::relocate(ptr);
  m_splineQuatCompToAnimMap = CompToAnimChannelMap::relocate(ptr);

  //-----------------------
  // Sections information (i.e. start frame and section size)
  NMP::Memory::Format memReqsSectionInfo(sizeof(AnimSectionInfoQSA) * m_numSections, NMP_NATURAL_TYPE_ALIGNMENT);
  m_sectionsInfo = (AnimSectionInfoQSA*) NMP::Memory::alignAndIncrement(ptr, memReqsSectionInfo);

  //-----------------------
  // Sections DataRef table (pointers to sections)
  NMP::Memory::Format memReqsSectionRefs(sizeof(DataRef) * m_numSections, NMP_NATURAL_TYPE_ALIGNMENT);
  m_sections = (DataRef*) NMP::Memory::alignAndIncrement(ptr, memReqsSectionRefs);

  //-----------------------
  // Channel sets quantisation information
  ptr = (void*) NMP::Memory::align(ptr, NMP_VECTOR_ALIGNMENT);
  m_channelSetsInfo = (MR::ChannelSetInfoQSA*)ptr;
  m_channelSetsInfo->relocate();

  //-----------------------
  // Sections are relocated as required (DMA aligned)
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
