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
#include <string.h>
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMathUtils.h"
#include "NMNumerics/NMVector3Utils.h"
#include "NMNumerics/NMQuatUtils.h"
#include "NMNumerics/NMUniformQuantisation.h"
#include "morpheme/mrDefines.h"
#include "morpheme/AnimSource/mrAnimSourceNSA.h"
#include "assetProcessor/AnimSource/AnimSourceCompressor.h"
#include "assetProcessor/AnimSource/SectionCompilerNSA.h"
#include "assetProcessor/AnimSource/Vector3QuantisationSetBasis.h"
//----------------------------------------------------------------------------------------------------------------------

// Disable warnings about deprecated functions (sprintf, fopen)
#ifdef NM_COMPILER_MSVC
#pragma warning (push)
#pragma warning(disable : 4996)
#endif

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// SubSectionIntermediateNSA
//----------------------------------------------------------------------------------------------------------------------
SubSectionIntermediateNSA::SubSectionIntermediateNSA() :
  m_sampledPosMeansX(NULL),
  m_sampledPosMeansY(NULL),
  m_sampledPosMeansZ(NULL),
  m_sampledPosRelTable(NULL),
  m_sampledPosCompToQSetMapX(NULL),
  m_sampledPosCompToQSetMapY(NULL),
  m_sampledPosCompToQSetMapZ(NULL),
  m_sampledPosRelQuantisedTable(NULL),
  m_sampledQuatMeansX(NULL),
  m_sampledQuatMeansY(NULL),
  m_sampledQuatMeansZ(NULL),
  m_sampledQuatRelTable(NULL),
  m_sampledQuatCompToQSetMapX(NULL),
  m_sampledQuatCompToQSetMapY(NULL),
  m_sampledQuatCompToQSetMapZ(NULL),
  m_sampledQuatRelQuantisedTable(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
SubSectionIntermediateNSA::~SubSectionIntermediateNSA()
{
  tidy();
}

//----------------------------------------------------------------------------------------------------------------------
void SubSectionIntermediateNSA::tidy()
{
  // Pos
  if (m_sampledPosMeansX)
  {
    NMP::Memory::memFree(m_sampledPosMeansX);
    m_sampledPosMeansX = NULL;
  }
  
  if (m_sampledPosMeansY)
  {
    NMP::Memory::memFree(m_sampledPosMeansY);
    m_sampledPosMeansY = NULL;
  }
  
  if (m_sampledPosMeansZ)
  {
    NMP::Memory::memFree(m_sampledPosMeansZ);
    m_sampledPosMeansZ = NULL;
  }
  
  if (m_sampledPosRelTable)
  {
    NMP::Memory::memFree(m_sampledPosRelTable);
    m_sampledPosRelTable = NULL;
  }
  
  if (m_sampledPosCompToQSetMapX)
  {
    NMP::Memory::memFree(m_sampledPosCompToQSetMapX);
    m_sampledPosCompToQSetMapX = NULL;
  }
  
  if (m_sampledPosCompToQSetMapY)
  {
    NMP::Memory::memFree(m_sampledPosCompToQSetMapY);
    m_sampledPosCompToQSetMapY = NULL;
  }
  
  if (m_sampledPosCompToQSetMapZ)
  {
    NMP::Memory::memFree(m_sampledPosCompToQSetMapZ);
    m_sampledPosCompToQSetMapZ = NULL;
  }

  if (m_sampledPosRelQuantisedTable)
  {
    NMP::Memory::memFree(m_sampledPosRelQuantisedTable);
    m_sampledPosRelQuantisedTable = NULL;
  }
  
  // Quat
  if (m_sampledQuatMeansX)
  {
    NMP::Memory::memFree(m_sampledQuatMeansX);
    m_sampledQuatMeansX = NULL;
  }

  if (m_sampledQuatMeansY)
  {
    NMP::Memory::memFree(m_sampledQuatMeansY);
    m_sampledQuatMeansY = NULL;
  }

  if (m_sampledQuatMeansZ)
  {
    NMP::Memory::memFree(m_sampledQuatMeansZ);
    m_sampledQuatMeansZ = NULL;
  }

  if (m_sampledQuatRelTable)
  {
    NMP::Memory::memFree(m_sampledQuatRelTable);
    m_sampledQuatRelTable = NULL;
  }

  if (m_sampledQuatCompToQSetMapX)
  {
    NMP::Memory::memFree(m_sampledQuatCompToQSetMapX);
    m_sampledQuatCompToQSetMapX = NULL;
  }

  if (m_sampledQuatCompToQSetMapY)
  {
    NMP::Memory::memFree(m_sampledQuatCompToQSetMapY);
    m_sampledQuatCompToQSetMapY = NULL;
  }

  if (m_sampledQuatCompToQSetMapZ)
  {
    NMP::Memory::memFree(m_sampledQuatCompToQSetMapZ);
    m_sampledQuatCompToQSetMapZ = NULL;
  }

  if (m_sampledQuatRelQuantisedTable)
  {
    NMP::Memory::memFree(m_sampledQuatRelQuantisedTable);
    m_sampledQuatRelQuantisedTable = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SubSectionIntermediateNSA::computeSampledPosRelativeTransforms(
  const AnimSourceUncompressed* animSource,
  const SampledDataLayoutNSA* sampledDataLayout,
  uint32_t iFrameSection,
  uint32_t iChannelSection,
  const Vector3QuantisationTable* quantPosMeans)
{
  NMP_VERIFY(sampledDataLayout);
  const CompToAnimChannelMapLayout* sampledPosCompToAnimMap = sampledDataLayout->getSampledPosCompToAnimMap(iChannelSection);
  NMP_VERIFY(sampledPosCompToAnimMap);
  uint32_t numChannelEntries = sampledPosCompToAnimMap->getNumEntries();
  if (numChannelEntries > 0)
  {
    NMP_VERIFY(animSource); 
    NMP_VERIFY(quantPosMeans);
    const ChannelSetTable* channelSetsTable = animSource->getChannelSets();
    NMP_VERIFY(channelSetsTable);
    uint32_t sectionStartFrame = sampledDataLayout->getSectionStartFrame(iFrameSection);
    uint32_t sectionEndFrame = sampledDataLayout->getSectionEndFrame(iFrameSection);
    uint32_t numSectionFrames = sectionEndFrame - sectionStartFrame + 1;

    // Allocate the memory for the channel means
    NMP::Memory::Format memReqsMeans(sizeof(uint32_t) * numChannelEntries, NMP_NATURAL_TYPE_ALIGNMENT);
    NMP::Memory::Resource memResMeans;
    memResMeans = NMPMemoryAllocateFromFormat(memReqsMeans);
    ZeroMemory(memResMeans.ptr, memReqsMeans.size);
    m_sampledPosMeansX = (uint32_t*)memResMeans.ptr;
    NMP_VERIFY(m_sampledPosMeansX);
    memResMeans = NMPMemoryAllocateFromFormat(memReqsMeans);
    ZeroMemory(memResMeans.ptr, memReqsMeans.size);
    m_sampledPosMeansY = (uint32_t*)memResMeans.ptr;
    NMP_VERIFY(m_sampledPosMeansY);
    memResMeans = NMPMemoryAllocateFromFormat(memReqsMeans);
    ZeroMemory(memResMeans.ptr, memReqsMeans.size);
    m_sampledPosMeansZ = (uint32_t*)memResMeans.ptr;
    NMP_VERIFY(m_sampledPosMeansZ);

    // Allocate the memory for the sampled pos table (for relative transforms)
    NMP::Memory::Format memReqsPosRelTable = AP::Vector3Table::getMemoryRequirements(numChannelEntries, numSectionFrames);
    NMP::Memory::Resource memResPosRelTable = NMPMemoryAllocateFromFormat(memReqsPosRelTable);
    ZeroMemory(memResPosRelTable.ptr, memReqsPosRelTable.size);
    m_sampledPosRelTable = AP::Vector3Table::init(memResPosRelTable, numChannelEntries, numSectionFrames);

    //-----------------------
    // Compute the quantised means
    for (uint32_t iChan = 0; iChan < numChannelEntries; ++iChan)
    {
      uint32_t animChannelIndex = sampledPosCompToAnimMap->getAnimChannel(iChan);
      const NMP::Vector3* posKeys = channelSetsTable->getChannelPos(animChannelIndex);
      NMP_VERIFY(posKeys);
      const NMP::Vector3* posSectionKeys = &posKeys[sectionStartFrame];

      // Compute the mean of the pos sample data
      NMP::Vector3 pbar;
      NMP::vecMean(numSectionFrames, posSectionKeys, pbar);

      // Quantise the channel mean
      quantPosMeans->quantise(
        iChan,
        pbar,
        m_sampledPosMeansX[iChan],
        m_sampledPosMeansY[iChan],
        m_sampledPosMeansZ[iChan]);
    }

    //-----------------------
    // Compute the relative transforms
    for (uint32_t iChan = 0; iChan < numChannelEntries; ++iChan)
    {
      uint32_t animChannelIndex = sampledPosCompToAnimMap->getAnimChannel(iChan);
      const NMP::Vector3* posKeys = channelSetsTable->getChannelPos(animChannelIndex);
      NMP_VERIFY(posKeys);
      const NMP::Vector3* posSectionKeys = &posKeys[sectionStartFrame];

      // Recover the corresponding quantised mean value
      NMP::Vector3 pbar;
      quantPosMeans->dequantise(
        iChan,
        pbar,
        m_sampledPosMeansX[iChan],
        m_sampledPosMeansY[iChan],
        m_sampledPosMeansZ[iChan]);

      // Remove the channel mean from the pos sample data
      for (uint32_t iFrame = 0; iFrame < numSectionFrames; ++iFrame)
      {
        NMP::Vector3 posKeyRel = posSectionKeys[iFrame] - pbar;
        m_sampledPosRelTable->setKey(iChan, iFrame, posKeyRel);
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SubSectionIntermediateNSA::computeSampledPosCompToQSetMaps(
  const uint32_t* sectionChanToQSetMapX,
  const uint32_t* sectionChanToQSetMapY,
  const uint32_t* sectionChanToQSetMapZ)
{
  NMP_VERIFY(sectionChanToQSetMapX);
  NMP_VERIFY(sectionChanToQSetMapY);
  NMP_VERIFY(sectionChanToQSetMapZ);
  NMP_VERIFY(m_sampledPosRelTable);
  
  uint32_t numChannelEntries = m_sampledPosRelTable->getNumChannels();
  if (numChannelEntries > 0)
  {
    // Allocate the memory for the channel to quantisation set maps
    NMP::Memory::Format memReqs(sizeof(uint32_t) * numChannelEntries, NMP_NATURAL_TYPE_ALIGNMENT);
    NMP::Memory::Resource memRes;
    memRes = NMPMemoryAllocateFromFormat(memReqs);
    ZeroMemory(memRes.ptr, memReqs.size);
    m_sampledPosCompToQSetMapX = (uint32_t*)memRes.ptr;
    memRes = NMPMemoryAllocateFromFormat(memReqs);
    ZeroMemory(memRes.ptr, memReqs.size);
    m_sampledPosCompToQSetMapY = (uint32_t*)memRes.ptr;
    memRes = NMPMemoryAllocateFromFormat(memReqs);
    ZeroMemory(memRes.ptr, memReqs.size);
    m_sampledPosCompToQSetMapZ = (uint32_t*)memRes.ptr;
    
    // Set the entries
    for (uint32_t i = 0; i < numChannelEntries; ++i)
    {
      m_sampledPosCompToQSetMapX[i] = sectionChanToQSetMapX[i];
      m_sampledPosCompToQSetMapY[i] = sectionChanToQSetMapY[i];
      m_sampledPosCompToQSetMapZ[i] = sectionChanToQSetMapZ[i];
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SubSectionIntermediateNSA::computeSampledPosQuantisedData(
  const AnimSourceUncompressed* animSource,
  const SampledDataLayoutNSA* sampledDataLayout,
  uint32_t iFrameSection,
  uint32_t iChannelSection,
  const NMP::Vector3* qSetPosMin,
  const NMP::Vector3* qSetPosMax)
{
  NMP_VERIFY(animSource);
  NMP_VERIFY(sampledDataLayout);
  NMP_VERIFY(qSetPosMin);
  NMP_VERIFY(qSetPosMax);  
  const CompToAnimChannelMapLayout* sampledPosCompToAnimMap = sampledDataLayout->getSampledPosCompToAnimMap(iChannelSection);
  NMP_VERIFY(sampledPosCompToAnimMap);
  uint32_t numChannelEntries = sampledPosCompToAnimMap->getNumEntries();
  if (numChannelEntries > 0)
  {
    uint32_t sectionStartFrame = sampledDataLayout->getSectionStartFrame(iFrameSection);
    uint32_t sectionEndFrame = sampledDataLayout->getSectionEndFrame(iFrameSection);
    uint32_t numSectionFrames = sectionEndFrame - sectionStartFrame + 1;
    NMP_VERIFY(m_sampledPosRelTable);
    NMP_VERIFY(numChannelEntries == m_sampledPosRelTable->getNumChannels());
    NMP_VERIFY(numSectionFrames == m_sampledPosRelTable->getNumKeyFrames());

    // Allocate the memory for the quantisation table
    NMP::Memory::Format memReqsQSets = Vector3QuantisationTable::getMemoryRequirements(numChannelEntries);
    NMP::Memory::Resource memResQSets = NMPMemoryAllocateFromFormat(memReqsQSets);
    ZeroMemory(memResQSets.ptr, memReqsQSets.size);
    Vector3QuantisationTable* qSetTable = Vector3QuantisationTable::init(memResQSets, numChannelEntries);
    qSetTable->setPrecisionsX(11);
    qSetTable->setPrecisionsY(11);
    qSetTable->setPrecisionsZ(10);

    // Allocate the memory for the sampled pos table (for quantised relative transforms)
    NMP::Memory::Format memReqsPosRelQTable = IntVector3Table::getMemoryRequirements(numChannelEntries, numSectionFrames);
    NMP::Memory::Resource memResPosRelQTable = NMPMemoryAllocateFromFormat(memReqsPosRelQTable);
    ZeroMemory(memResPosRelQTable.ptr, memReqsPosRelQTable.size);
    m_sampledPosRelQuantisedTable = IntVector3Table::init(memResPosRelQTable, numChannelEntries, numSectionFrames);

    // Project the relative transforms onto the basis vectors and quantise the resulting components
    for (uint32_t iChan = 0; iChan < numChannelEntries; ++iChan)
    {
      // Set the quantisation set bounds
      uint32_t qSetX = m_sampledPosCompToQSetMapX[iChan];
      uint32_t qSetY = m_sampledPosCompToQSetMapY[iChan];
      uint32_t qSetZ = m_sampledPosCompToQSetMapZ[iChan];
      NMP::Vector3 qMin, qMax;
      qMin.x = qSetPosMin[qSetX].x;
      qMin.y = qSetPosMin[qSetY].y;
      qMin.z = qSetPosMin[qSetZ].z;
      qMax.x = qSetPosMax[qSetX].x;
      qMax.y = qSetPosMax[qSetY].y;
      qMax.z = qSetPosMax[qSetZ].z;
      qSetTable->setQuantisationBounds(iChan, qMin, qMax);

      // Project and quantise the channel data samples
      for (uint32_t iFrame = 0; iFrame < numSectionFrames; ++iFrame)
      {
        NMP::Vector3 posKey;
        m_sampledPosRelTable->getKey(iChan, iFrame, posKey);
        uint32_t qKey[3];
        qSetTable->quantise(iChan, posKey, qKey[0], qKey[1], qKey[2]);
        m_sampledPosRelQuantisedTable->setKey(iChan, iFrame, (const int32_t*)qKey);
      }
    }

    // Tidy up
    NMP::Memory::memFree(qSetTable);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SubSectionIntermediateNSA::computeSampledQuatRelativeTransforms(
  const AnimSourceUncompressed* animSource,
  const SampledDataLayoutNSA* sampledDataLayout,
  uint32_t iFrameSection,
  uint32_t iChannelSection,
  const Vector3QuantisationTable* quantQuatMeans)
{
  NMP_VERIFY(animSource);
  NMP_VERIFY(sampledDataLayout);
  NMP_VERIFY(quantQuatMeans);
  const ChannelSetTable* channelSetsTable = animSource->getChannelSets();
  NMP_VERIFY(channelSetsTable);
  const CompToAnimChannelMapLayout* sampledQuatCompToAnimMap = sampledDataLayout->getSampledQuatCompToAnimMap(iChannelSection);
  NMP_VERIFY(sampledQuatCompToAnimMap);
  uint32_t numChannelEntries = sampledQuatCompToAnimMap->getNumEntries();
  if (numChannelEntries > 0)
  {
    uint32_t sectionStartFrame = sampledDataLayout->getSectionStartFrame(iFrameSection);
    uint32_t sectionEndFrame = sampledDataLayout->getSectionEndFrame(iFrameSection);
    uint32_t numSectionFrames = sectionEndFrame - sectionStartFrame + 1;

    // Allocate the memory for the channel means
    NMP::Memory::Format memReqsMeans(sizeof(uint32_t) * numChannelEntries, NMP_NATURAL_TYPE_ALIGNMENT);
    NMP::Memory::Resource memResMeans;
    memResMeans = NMPMemoryAllocateFromFormat(memReqsMeans);
    ZeroMemory(memResMeans.ptr, memReqsMeans.size);
    m_sampledQuatMeansX = (uint32_t*)memResMeans.ptr;
    NMP_VERIFY(m_sampledQuatMeansX);
    memResMeans = NMPMemoryAllocateFromFormat(memReqsMeans);
    ZeroMemory(memResMeans.ptr, memReqsMeans.size);
    m_sampledQuatMeansY = (uint32_t*)memResMeans.ptr;
    NMP_VERIFY(m_sampledQuatMeansY);
    memResMeans = NMPMemoryAllocateFromFormat(memReqsMeans);
    ZeroMemory(memResMeans.ptr, memReqsMeans.size);
    m_sampledQuatMeansZ = (uint32_t*)memResMeans.ptr;
    NMP_VERIFY(m_sampledQuatMeansZ);

    // Allocate the memory for the sampled quat table (for relative transforms)
    NMP::Memory::Format memReqsQuatRelTable = AP::RotVecTable::getMemoryRequirements(numChannelEntries, numSectionFrames);
    NMP::Memory::Resource memResQuatRelTable = NMPMemoryAllocateFromFormat(memReqsQuatRelTable);
    ZeroMemory(memResQuatRelTable.ptr, memReqsQuatRelTable.size);
    m_sampledQuatRelTable = AP::RotVecTable::init(memResQuatRelTable, numChannelEntries, numSectionFrames);

    //-----------------------
    // Compute the quantised means
    for (uint32_t iChan = 0; iChan < numChannelEntries; ++iChan)
    {
      uint32_t animChannelIndex = sampledQuatCompToAnimMap->getAnimChannel(iChan);
      const NMP::Quat* quatKeys = channelSetsTable->getChannelQuat(animChannelIndex);
      NMP_VERIFY(quatKeys);
      const NMP::Quat* quatSectionKeys = &quatKeys[sectionStartFrame];

      // Compute the mean of the quat sample data
      NMP::Quat qbar;
      NMP::quatMean(numSectionFrames, quatSectionKeys, qbar);

      // Convert the mean to tan quarter angle rotation vector
      NMP::Vector3 qbarTQA;
      qbarTQA = qbar.toRotationVector(true);

      // Quantise the channel mean
      quantQuatMeans->quantise(
        iChan,
        qbarTQA,
        m_sampledQuatMeansX[iChan],
        m_sampledQuatMeansY[iChan],
        m_sampledQuatMeansZ[iChan]);
    }

    //-----------------------
    // Compute the relative transforms
    for (uint32_t iChan = 0; iChan < numChannelEntries; ++iChan)
    {
      uint32_t animChannelIndex = sampledQuatCompToAnimMap->getAnimChannel(iChan);
      const NMP::Quat* quatKeys = channelSetsTable->getChannelQuat(animChannelIndex);
      NMP_VERIFY(quatKeys);
      const NMP::Quat* quatSectionKeys = &quatKeys[sectionStartFrame];

      // Recover the corresponding quantised mean value
      NMP::Vector3 qbarTQA;
      quantQuatMeans->dequantise(
        iChan,
        qbarTQA,
        m_sampledQuatMeansX[iChan],
        m_sampledQuatMeansY[iChan],
        m_sampledQuatMeansZ[iChan]);

      // Convert back into a quaternion
      NMP::Quat qbar;
      qbar.fromRotationVector(qbarTQA, true);

      // Remove the channel mean from the quat sample data and store as
      // tan quarter angle rotation vectors
      NMP::Quat invQbar = ~qbar;
      for (uint32_t iFrame = 0; iFrame < numSectionFrames; ++iFrame)
      {
        NMP::Quat quatKeyRel = invQbar * quatSectionKeys[iFrame];
        quatKeyRel.normalise(); // Important to make sure that ||w|| <= 1
        m_sampledQuatRelTable->setKey(iChan, iFrame, quatKeyRel, true);
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SubSectionIntermediateNSA::computeSampledQuatCompToQSetMaps(
  const uint32_t* sectionChanToQSetMapX,
  const uint32_t* sectionChanToQSetMapY,
  const uint32_t* sectionChanToQSetMapZ)
{
  NMP_VERIFY(sectionChanToQSetMapX);
  NMP_VERIFY(sectionChanToQSetMapY);
  NMP_VERIFY(sectionChanToQSetMapZ);
  NMP_VERIFY(m_sampledQuatRelTable);

  uint32_t numChannelEntries = m_sampledQuatRelTable->getNumChannels();
  if (numChannelEntries > 0)
  {
    // Allocate the memory for the channel to quantisation set maps
    NMP::Memory::Format memReqs(sizeof(uint32_t) * numChannelEntries, NMP_NATURAL_TYPE_ALIGNMENT);
    NMP::Memory::Resource memRes;
    memRes = NMPMemoryAllocateFromFormat(memReqs);
    ZeroMemory(memRes.ptr, memReqs.size);
    m_sampledQuatCompToQSetMapX = (uint32_t*)memRes.ptr;
    memRes = NMPMemoryAllocateFromFormat(memReqs);
    ZeroMemory(memRes.ptr, memReqs.size);
    m_sampledQuatCompToQSetMapY = (uint32_t*)memRes.ptr;
    memRes = NMPMemoryAllocateFromFormat(memReqs);
    ZeroMemory(memRes.ptr, memReqs.size);
    m_sampledQuatCompToQSetMapZ = (uint32_t*)memRes.ptr;

    // Set the entries
    for (uint32_t i = 0; i < numChannelEntries; ++i)
    {
      m_sampledQuatCompToQSetMapX[i] = sectionChanToQSetMapX[i];
      m_sampledQuatCompToQSetMapY[i] = sectionChanToQSetMapY[i];
      m_sampledQuatCompToQSetMapZ[i] = sectionChanToQSetMapZ[i];
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SubSectionIntermediateNSA::computeSampledQuatQuantisedData(
  const AnimSourceUncompressed* animSource,
  const SampledDataLayoutNSA* sampledDataLayout,
  uint32_t iFrameSection,
  uint32_t iChannelSection,
  const NMP::Vector3* qSetQuatMin,
  const NMP::Vector3* qSetQuatMax)
{
  NMP_VERIFY(animSource);
  NMP_VERIFY(sampledDataLayout);
  NMP_VERIFY(qSetQuatMin);
  NMP_VERIFY(qSetQuatMax);  
  const CompToAnimChannelMapLayout* sampledQuatCompToAnimMap = sampledDataLayout->getSampledQuatCompToAnimMap(iChannelSection);
  NMP_VERIFY(sampledQuatCompToAnimMap);
  uint32_t numChannelEntries = sampledQuatCompToAnimMap->getNumEntries();
  if (numChannelEntries > 0)
  {
    uint32_t sectionStartFrame = sampledDataLayout->getSectionStartFrame(iFrameSection);
    uint32_t sectionEndFrame = sampledDataLayout->getSectionEndFrame(iFrameSection);
    uint32_t numSectionFrames = sectionEndFrame - sectionStartFrame + 1;
    NMP_VERIFY(m_sampledQuatRelTable);
    NMP_VERIFY(numChannelEntries == m_sampledQuatRelTable->getNumChannels());
    NMP_VERIFY(numSectionFrames == m_sampledQuatRelTable->getNumKeyFrames());

    // Allocate the memory for the quantisation table
    NMP::Memory::Format memReqsQSets = Vector3QuantisationTable::getMemoryRequirements(numChannelEntries);
    NMP::Memory::Resource memResQSets = NMPMemoryAllocateFromFormat(memReqsQSets);
    ZeroMemory(memResQSets.ptr, memReqsQSets.size);
    Vector3QuantisationTable* qSetTable = Vector3QuantisationTable::init(memResQSets, numChannelEntries);
    qSetTable->setPrecisionsX(16);
    qSetTable->setPrecisionsY(16);
    qSetTable->setPrecisionsZ(16);

    // Allocate the memory for the sampled quat table (for quantised relative transforms)
    NMP::Memory::Format memReqsQuatRelQTable = IntVector3Table::getMemoryRequirements(numChannelEntries, numSectionFrames);
    NMP::Memory::Resource memResQuatRelQTable = NMPMemoryAllocateFromFormat(memReqsQuatRelQTable);
    ZeroMemory(memResQuatRelQTable.ptr, memReqsQuatRelQTable.size);
    m_sampledQuatRelQuantisedTable = IntVector3Table::init(memResQuatRelQTable, numChannelEntries, numSectionFrames);

    // Project the relative transforms onto the basis vectors and quantise the resulting components
    for (uint32_t iChan = 0; iChan < numChannelEntries; ++iChan)
    {
      // Set the quantisation set bounds
      uint32_t qSetX = m_sampledQuatCompToQSetMapX[iChan];
      uint32_t qSetY = m_sampledQuatCompToQSetMapY[iChan];
      uint32_t qSetZ = m_sampledQuatCompToQSetMapZ[iChan];
      NMP::Vector3 qMin, qMax;
      qMin.x = qSetQuatMin[qSetX].x;
      qMin.y = qSetQuatMin[qSetY].y;
      qMin.z = qSetQuatMin[qSetZ].z;
      qMax.x = qSetQuatMax[qSetX].x;
      qMax.y = qSetQuatMax[qSetY].y;
      qMax.z = qSetQuatMax[qSetZ].z;
      qSetTable->setQuantisationBounds(iChan, qMin, qMax);

      // Project and quantise the channel data samples
      for (uint32_t iFrame = 0; iFrame < numSectionFrames; ++iFrame)
      {
        NMP::Vector3 quatKeyTQA;
        m_sampledQuatRelTable->getKey(iChan, iFrame, quatKeyTQA);
        uint32_t qKey[3];
        qSetTable->quantise(iChan, quatKeyTQA, qKey[0], qKey[1], qKey[2]);
        m_sampledQuatRelQuantisedTable->setKey(iChan, iFrame, (const int32_t*)qKey);
      }
    }

    // Tidy up
    NMP::Memory::memFree(qSetTable);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// SampledDataNSAIntermediate
//----------------------------------------------------------------------------------------------------------------------
SampledDataIntermediateNSA::SampledDataIntermediateNSA() :
  m_numFrameSections(0),
  m_numChannelSections(0),
  m_subSections(NULL),
  m_quantPosMeans(NULL),
  m_quantQuatMeans(NULL),
  m_qSetPosMin(NULL),
  m_qSetPosMax(NULL),
  m_qSetQuatMin(NULL),
  m_qSetQuatMax(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
SampledDataIntermediateNSA::~SampledDataIntermediateNSA()
{
  tidy();
}

//----------------------------------------------------------------------------------------------------------------------
void SampledDataIntermediateNSA::tidy()
{
  //-----------------------
  // Sections
  if (m_subSections)
  {
    uint32_t numEntries = m_numFrameSections * m_numChannelSections;
    for (uint32_t i = 0; i < numEntries; ++i)
    {
      SubSectionIntermediateNSA* subSection = m_subSections[i];
      if (subSection)
        delete subSection;
    }
    delete m_subSections;
  }
  
  m_subSections = NULL;
  m_numFrameSections = 0;
  m_numChannelSections = 0;
  
  //-----------------------
  // Global data
  if (m_quantPosMeans)
  {
    NMP::Memory::memFree(m_quantPosMeans);
    m_quantPosMeans = NULL;
  }
  
  if (m_quantQuatMeans)
  {
    NMP::Memory::memFree(m_quantQuatMeans);
    m_quantQuatMeans = NULL;
  }
  
  if (m_qSetPosMin)
  {
    NMP::Memory::memFree(m_qSetPosMin);
    m_qSetPosMin = NULL;
  }
  
  if (m_qSetPosMax)
  {
    NMP::Memory::memFree(m_qSetPosMax);
    m_qSetPosMax = NULL;
  }

  if (m_qSetQuatMin)
  {
    NMP::Memory::memFree(m_qSetQuatMin);
    m_qSetQuatMin = NULL;
  }

  if (m_qSetQuatMax)
  {
    NMP::Memory::memFree(m_qSetQuatMax);
    m_qSetQuatMax = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
const SubSectionIntermediateNSA* SampledDataIntermediateNSA::getSubSection(
  uint32_t iFrameSection,
  uint32_t iChannelSection) const
{
  NMP_VERIFY(iFrameSection < m_numFrameSections && iChannelSection < m_numChannelSections);
  return m_subSections[iFrameSection * m_numChannelSections + iChannelSection];
}

//----------------------------------------------------------------------------------------------------------------------
SubSectionIntermediateNSA* SampledDataIntermediateNSA::getSubSection(
  uint32_t iFrameSection,
  uint32_t iChannelSection)
{
  NMP_VERIFY(iFrameSection < m_numFrameSections && iChannelSection < m_numChannelSections);
  return m_subSections[iFrameSection * m_numChannelSections + iChannelSection];
}

//----------------------------------------------------------------------------------------------------------------------
void SampledDataIntermediateNSA::computeSampledData(
  const AnimSourceUncompressed* animSource,
  const SampledDataLayoutNSA* sampledDataLayout)
{
  NMP_VERIFY(animSource);
  NMP_VERIFY(sampledDataLayout);
  m_numFrameSections = sampledDataLayout->getNumFrameSections();
  NMP_VERIFY(m_numFrameSections > 0);
  m_numChannelSections = sampledDataLayout->getNumChannelSections();
  NMP_VERIFY(m_numChannelSections > 0);
  
  //-----------------------
  // Allocate the memory for the sub sections
  uint32_t numEntries = m_numFrameSections * m_numChannelSections;  
  m_subSections = new SubSectionIntermediateNSA* [numEntries];
  for (uint32_t i = 0; i < numEntries; ++i)
    m_subSections[i] = new SubSectionIntermediateNSA;
  
  //-----------------------
  // Pos
  uint32_t numSampledPosChannels = sampledDataLayout->getNumSampledPosChannels();
  if (numSampledPosChannels > 0)
  {
    // Compile the global pos quantisation [qScale, qOffset] bounds for the coarse
    // pos means info. i.e. all pos sample data is encoded relative to these means.
    computeSampledPosMeansQuantisationInfo(animSource, sampledDataLayout);

    // Compute the sampled pos means and relative transforms
    computeSampledPosRelativeTransforms(animSource, sampledDataLayout);

    // Compute the sampled pos quantisation set basis vectors
    computeSampledPosQuantisationSetBasisVectors(animSource, sampledDataLayout);

    // Compute the sampled pos quantised data
    computeSampledPosQuantisedData(animSource, sampledDataLayout);
  }

  //-----------------------
  // Quat
  uint32_t numSampledQuatChannels = sampledDataLayout->getNumSampledQuatChannels();
  if (numSampledQuatChannels > 0)
  {
    // Compile the global quat quantisation [qScale, qOffset] bounds for the coarse
    // quat means info. i.e. all quat sample data is encoded relative to these means.
    computeSampledQuatMeansQuantisationInfo(animSource, sampledDataLayout);

    // Compute the sampled quat means and relative transforms
    computeSampledQuatRelativeTransforms(animSource, sampledDataLayout);

    // Compute the sampled quat quantisation set basis vectors
    computeSampledQuatQuantisationSetBasisVectors(animSource, sampledDataLayout);

    // Compute the sampled quat quantised data
    computeSampledQuatQuantisedData(animSource, sampledDataLayout);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SampledDataIntermediateNSA::computeSampledPosMeansQuantisationInfo(
  const AnimSourceUncompressed* animSource,
  const SampledDataLayoutNSA* sampledDataLayout)
{
  NMP_VERIFY(m_numChannelSections == sampledDataLayout->getNumChannelSections());
  const ChannelSetTable* channelSetsTable = animSource->getChannelSets();
  NMP_VERIFY(channelSetsTable);
  uint32_t numAnimFrames = channelSetsTable->getNumKeyFrames();
  uint32_t numSampledPosChannels = sampledDataLayout->getNumSampledPosChannels();
  
  // Allocate the memory for the quantisation means table
  NMP::Memory::Format memReqsQuantMeans = Vector3QuantisationTable::getMemoryRequirements(numSampledPosChannels);
  NMP::Memory::Resource memResQuantMeans = NMPMemoryAllocateFromFormat(memReqsQuantMeans);
  ZeroMemory(memResQuantMeans.ptr, memReqsQuantMeans.size);
  m_quantPosMeans = Vector3QuantisationTable::init(memResQuantMeans, numSampledPosChannels);

  // Setup precisions for the quantisation means
  m_quantPosMeans->setPrecisionsX(8);
  m_quantPosMeans->setPrecisionsY(8);
  m_quantPosMeans->setPrecisionsZ(8);

  // Compute the quantisation bounds from the channel data
  uint32_t iChanIndx = 0;
  for (uint32_t iChannelSection = 0; iChannelSection < m_numChannelSections; ++iChannelSection)
  {
    const CompToAnimChannelMapLayout* sampledPosCompToAnimMap = sampledDataLayout->getSampledPosCompToAnimMap(iChannelSection);
    NMP_VERIFY(sampledPosCompToAnimMap);
    uint32_t numEntries = sampledPosCompToAnimMap->getNumEntries();
    for (uint32_t i = 0; i < numEntries; ++i, ++iChanIndx)
    {
      uint32_t animChannelIndex = sampledPosCompToAnimMap->getAnimChannel(i);
      const NMP::Vector3* posKeys = channelSetsTable->getChannelPos(animChannelIndex);
      NMP_VERIFY(posKeys);
      m_quantPosMeans->setQuantisationBounds(iChanIndx, numAnimFrames, posKeys);
    }
  }

  // Compute the global quantisation info
  m_quantPosMeans->setGlobalQuantisationBounds();
}

//----------------------------------------------------------------------------------------------------------------------
void SampledDataIntermediateNSA::computeSampledPosRelativeTransforms(
  const AnimSourceUncompressed* animSource,
  const SampledDataLayoutNSA* sampledDataLayout)
{
  NMP_VERIFY(animSource);
  NMP_VERIFY(sampledDataLayout);

  for (uint32_t iFrameSection = 0; iFrameSection < m_numFrameSections; ++iFrameSection)
  {
    for (uint32_t iChannelSection = 0; iChannelSection < m_numChannelSections; ++iChannelSection)
    {
      SubSectionIntermediateNSA* subSection = getSubSection(iFrameSection, iChannelSection);
      NMP_VERIFY(subSection);
      subSection->computeSampledPosRelativeTransforms(
        animSource,
        sampledDataLayout,
        iFrameSection,
        iChannelSection,
        m_quantPosMeans);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SampledDataIntermediateNSA::computeSampledPosQuantisationSetBasisVectors(
  const AnimSourceUncompressed* animSource,
  const SampledDataLayoutNSA* sampledDataLayout)
{
  NMP_VERIFY(animSource);
  NMP_VERIFY(sampledDataLayout);
  const ChannelSetTable* channelSetsTable = animSource->getChannelSets();
  NMP_VERIFY(channelSetsTable);

  //-----------------------
  // Compute the number of initial quantisation channel sets
  uint32_t numChannelSets = 0;
  for (uint32_t iFrameSection = 0; iFrameSection < m_numFrameSections; ++iFrameSection)
  {
    for (uint32_t iChannelSection = 0; iChannelSection < m_numChannelSections; ++iChannelSection)
    {
      // Get the sampled pos relative transforms table
      SubSectionIntermediateNSA* subSection = getSubSection(iFrameSection, iChannelSection);
      NMP_VERIFY(subSection);
      const Vector3Table* sampledPosRelTable = subSection->getSampledPosRelTable();
      if (sampledPosRelTable)
      {
        numChannelSets += sampledPosRelTable->getNumChannels();
      }
    }
  }
  
  if (numChannelSets > 0)
  {
    //-----------------------
    // Allocate the memory for a flattened table of position samples
    NMP::Memory::Format memReqsV(sizeof(NMP::Vector3*) * numChannelSets, NMP_NATURAL_TYPE_ALIGNMENT);
    NMP::Memory::Resource memResV = NMPMemoryAllocateFromFormat(memReqsV);
    ZeroMemory(memResV.ptr, memReqsV.size);
    const NMP::Vector3** sectionPosKeysTable = (const NMP::Vector3**)memResV.ptr;
    NMP_VERIFY(sectionPosKeysTable);

    // Allocate the memory for the flattened table keyframe counts
    NMP::Memory::Format memReqsC(sizeof(uint32_t) * numChannelSets, NMP_NATURAL_TYPE_ALIGNMENT);
    NMP::Memory::Resource memResC = NMPMemoryAllocateFromFormat(memReqsC);
    ZeroMemory(memResC.ptr, memReqsC.size);
    uint32_t* sectionPosKeyCountsTable = (uint32_t*)memResC.ptr;
    NMP_VERIFY(sectionPosKeyCountsTable);

    //-----------------------
    // Fill in the flattened table
    uint32_t indx = 0;
    for (uint32_t iFrameSection = 0; iFrameSection < m_numFrameSections; ++iFrameSection)
    {
      for (uint32_t iChannelSection = 0; iChannelSection < m_numChannelSections; ++iChannelSection)
      {
        // Get the sampled pos relative transforms table
        SubSectionIntermediateNSA* subSection = getSubSection(iFrameSection, iChannelSection);
        NMP_VERIFY(subSection);
        const Vector3Table* sampledPosRelTable = subSection->getSampledPosRelTable();
        if (sampledPosRelTable)
        {
          uint32_t numChannelEntries = sampledPosRelTable->getNumChannels();

          // Set the flattened table
          for (uint32_t iChan = 0; iChan < numChannelEntries; ++iChan, ++indx)
          {
            sectionPosKeysTable[indx] = sampledPosRelTable->getChannel(iChan);
            sectionPosKeyCountsTable[indx] = sampledPosRelTable->getNumKeyFrames();
          }
        }
      }
    }

    //-----------------------
    // Compound the set of quantisation sets
    uint32_t sampledPosNumQuantisationSets = sampledDataLayout->getSampledPosNumQuantisationSets();
    NMP::Memory::Format memReqsQSetBasis = Vector3QuantisationSetBasis::getMemoryRequirements(
      numChannelSets,
      sampledPosNumQuantisationSets);
    NMP::Memory::Resource memResQSetBasis = NMPMemoryAllocateFromFormat(memReqsQSetBasis);
    ZeroMemory(memResQSetBasis.ptr, memReqsQSetBasis.size);
    Vector3QuantisationSetBasis* qSetPosBasis = Vector3QuantisationSetBasis::init(
      memResQSetBasis,
      numChannelSets,
      sampledPosNumQuantisationSets);

    qSetPosBasis->computeBasisVectors(
      numChannelSets,
      sectionPosKeysTable,
      sectionPosKeyCountsTable);

    const NMP::Vector3* qSetMin = qSetPosBasis->getQSetMin();
    const NMP::Vector3* qSetMax = qSetPosBasis->getQSetMax();
    const uint32_t* chanSetToQSetMapX = qSetPosBasis->getChanSetToQSetMapX();
    const uint32_t* chanSetToQSetMapY = qSetPosBasis->getChanSetToQSetMapY();
    const uint32_t* chanSetToQSetMapZ = qSetPosBasis->getChanSetToQSetMapZ();

    //-----------------------
    // Store the quantisation set basis vectors
    NMP::Memory::Format memReqsQSetMinMax(sizeof(NMP::Vector3) * sampledPosNumQuantisationSets, NMP_VECTOR_ALIGNMENT);
    NMP::Memory::Resource memResQSetMin = NMPMemoryAllocateFromFormat(memReqsQSetMinMax);
    ZeroMemory(memResQSetMin.ptr, memReqsQSetMinMax.size);
    m_qSetPosMin = (NMP::Vector3*)memResQSetMin.ptr;
    NMP_VERIFY(m_qSetPosMin);
    NMP::Memory::Resource memResQSetMax = NMPMemoryAllocateFromFormat(memReqsQSetMinMax);
    ZeroMemory(memResQSetMax.ptr, memReqsQSetMinMax.size);
    m_qSetPosMax = (NMP::Vector3*)memResQSetMax.ptr;
    NMP_VERIFY(m_qSetPosMax);

    for (uint32_t i = 0; i < sampledPosNumQuantisationSets; ++i)
    {
      m_qSetPosMin[i] = qSetMin[i];
      m_qSetPosMax[i] = qSetMax[i];
    }

    //-----------------------
    // Un-flatten the compounded channel set quantisation mappings
    indx = 0;
    for (uint32_t iFrameSection = 0; iFrameSection < m_numFrameSections; ++iFrameSection)
    {
      for (uint32_t iChannelSection = 0; iChannelSection < m_numChannelSections; ++iChannelSection)
      {
        // Get the sampled pos relative transforms table
        SubSectionIntermediateNSA* subSection = getSubSection(iFrameSection, iChannelSection);
        const Vector3Table* sampledPosRelTable = subSection->getSampledPosRelTable();
        if (sampledPosRelTable)
        {
          uint32_t numChannelEntries = sampledPosRelTable->getNumChannels();

          // Get the section channel to quantisation set map
          const uint32_t* sectionChanToQSetMapX = &chanSetToQSetMapX[indx];
          const uint32_t* sectionChanToQSetMapY = &chanSetToQSetMapY[indx];
          const uint32_t* sectionChanToQSetMapZ = &chanSetToQSetMapZ[indx];
          indx += numChannelEntries;

          // Store the channel to quantisation set maps
          subSection->computeSampledPosCompToQSetMaps(
            sectionChanToQSetMapX,
            sectionChanToQSetMapY,
            sectionChanToQSetMapZ);
        }
      }
    }

    //-----------------------
    // Tidy up
    NMP::Memory::memFree(sectionPosKeysTable);
    NMP::Memory::memFree(sectionPosKeyCountsTable);
    NMP::Memory::memFree(qSetPosBasis);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SampledDataIntermediateNSA::computeSampledPosQuantisedData(
  const AnimSourceUncompressed* animSource,
  const SampledDataLayoutNSA* sampledDataLayout)
{
  NMP_VERIFY(animSource);
  NMP_VERIFY(sampledDataLayout);

  // Quantise the channel set data using the quantisation set basis vectors
  for (uint32_t iFrameSection = 0; iFrameSection < m_numFrameSections; ++iFrameSection)
  {
    for (uint32_t iChannelSection = 0; iChannelSection < m_numChannelSections; ++iChannelSection)
    {
      SubSectionIntermediateNSA* subSection = getSubSection(iFrameSection, iChannelSection);
      NMP_VERIFY(subSection);
      subSection->computeSampledPosQuantisedData(
        animSource,
        sampledDataLayout,
        iFrameSection,
        iChannelSection,
        m_qSetPosMin,
        m_qSetPosMax);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SampledDataIntermediateNSA::computeSampledQuatMeansQuantisationInfo(
  const AnimSourceUncompressed* NMP_UNUSED(animSource),
  const SampledDataLayoutNSA* sampledDataLayout)
{
  NMP_VERIFY(sampledDataLayout);
  uint32_t numSampledQuatChannels = sampledDataLayout->getNumSampledQuatChannels();

  // Allocate the memory for the quantisation means table
  NMP::Memory::Format memReqsQuantMeans = Vector3QuantisationTable::getMemoryRequirements(numSampledQuatChannels);
  NMP::Memory::Resource memResQuantMeans = NMPMemoryAllocateFromFormat(memReqsQuantMeans);
  ZeroMemory(memResQuantMeans.ptr, memReqsQuantMeans.size);
  m_quantQuatMeans = Vector3QuantisationTable::init(memResQuantMeans, numSampledQuatChannels);

  // Setup precisions for the quantisation means
  m_quantQuatMeans->setPrecisionsX(8);
  m_quantQuatMeans->setPrecisionsY(8);
  m_quantQuatMeans->setPrecisionsZ(8);

  // Set the global quantisation bounds (tan quarter angle rot-vec)
  NMP::Vector3 qMin(-1.0f, -1.0f, -1.0f);
  NMP::Vector3 qMax(1.0f, 1.0f, 1.0f);
  m_quantQuatMeans->setGlobalQuantisationBounds(qMin, qMax);
}

//----------------------------------------------------------------------------------------------------------------------
void SampledDataIntermediateNSA::computeSampledQuatRelativeTransforms(
  const AnimSourceUncompressed* animSource,
  const SampledDataLayoutNSA* sampledDataLayout)
{
  NMP_VERIFY(animSource);
  NMP_VERIFY(sampledDataLayout);

  for (uint32_t iFrameSection = 0; iFrameSection < m_numFrameSections; ++iFrameSection)
  {
    for (uint32_t iChannelSection = 0; iChannelSection < m_numChannelSections; ++iChannelSection)
    {
      SubSectionIntermediateNSA* subSection = getSubSection(iFrameSection, iChannelSection);
      NMP_VERIFY(subSection);
      subSection->computeSampledQuatRelativeTransforms(
        animSource,
        sampledDataLayout,
        iFrameSection,
        iChannelSection,
        m_quantQuatMeans);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SampledDataIntermediateNSA::computeSampledQuatQuantisationSetBasisVectors(
  const AnimSourceUncompressed* animSource,
  const SampledDataLayoutNSA* sampledDataLayout)
{
  NMP_VERIFY(animSource);
  NMP_VERIFY(sampledDataLayout);
  const ChannelSetTable* channelSetsTable = animSource->getChannelSets();
  NMP_VERIFY(channelSetsTable);

  //-----------------------
  // Compute the number of initial quantisation channel sets
  uint32_t numChannelSets = 0;
  for (uint32_t iFrameSection = 0; iFrameSection < m_numFrameSections; ++iFrameSection)
  {
    for (uint32_t iChannelSection = 0; iChannelSection < m_numChannelSections; ++iChannelSection)
    {
      // Get the sampled pos relative transforms table
      SubSectionIntermediateNSA* subSection = getSubSection(iFrameSection, iChannelSection);
      NMP_VERIFY(subSection);
      const RotVecTable* sampledQuatRelTable = subSection->getSampledQuatRelTable();
      if (sampledQuatRelTable)
      {
        numChannelSets += sampledQuatRelTable->getNumChannels();
      }
    }
  }
  
  if (numChannelSets > 0)
  {
    //-----------------------
    // Allocate the memory for a flattened table of quat samples
    NMP::Memory::Format memReqsV(sizeof(NMP::Vector3*) * numChannelSets, NMP_NATURAL_TYPE_ALIGNMENT);
    NMP::Memory::Resource memResV = NMPMemoryAllocateFromFormat(memReqsV);
    ZeroMemory(memResV.ptr, memReqsV.size);
    const NMP::Vector3** sectionQuatKeysTable = (const NMP::Vector3**)memResV.ptr;
    NMP_VERIFY(sectionQuatKeysTable);

    // Allocate the memory for the flattened table keyframe counts
    NMP::Memory::Format memReqsC(sizeof(uint32_t) * numChannelSets, NMP_NATURAL_TYPE_ALIGNMENT);
    NMP::Memory::Resource memResC = NMPMemoryAllocateFromFormat(memReqsC);
    ZeroMemory(memResC.ptr, memReqsC.size);
    uint32_t* sectionQuatKeyCountsTable = (uint32_t*)memResC.ptr;
    NMP_VERIFY(sectionQuatKeyCountsTable);

    //-----------------------
    // Fill in the flattened table
    uint32_t indx = 0;
    for (uint32_t iFrameSection = 0; iFrameSection < m_numFrameSections; ++iFrameSection)
    {
      for (uint32_t iChannelSection = 0; iChannelSection < m_numChannelSections; ++iChannelSection)
      {
        // Get the sampled quat relative transforms table
        SubSectionIntermediateNSA* subSection = getSubSection(iFrameSection, iChannelSection);
        NMP_VERIFY(subSection);
        const RotVecTable* sampledQuatRelTable = subSection->getSampledQuatRelTable();
        if (sampledQuatRelTable)
        {
          uint32_t numChannelEntries = sampledQuatRelTable->getNumChannels();

          // Set the flattened table
          for (uint32_t iChan = 0; iChan < numChannelEntries; ++iChan, ++indx)
          {
            sectionQuatKeysTable[indx] = sampledQuatRelTable->getChannel(iChan);
            sectionQuatKeyCountsTable[indx] = sampledQuatRelTable->getNumKeyFrames();
          }
        }
      }
    }

    //-----------------------
    // Compound the set of quantisation sets
    uint32_t sampledQuatNumQuantisationSets = sampledDataLayout->getSampledQuatNumQuantisationSets();
    NMP::Memory::Format memReqsQSetBasis = Vector3QuantisationSetBasis::getMemoryRequirements(
      numChannelSets,
      sampledQuatNumQuantisationSets);
    NMP::Memory::Resource memResQSetBasis = NMPMemoryAllocateFromFormat(memReqsQSetBasis);
    ZeroMemory(memResQSetBasis.ptr, memReqsQSetBasis.size);
    Vector3QuantisationSetBasis* qSetQuatBasis = Vector3QuantisationSetBasis::init(
      memResQSetBasis,
      numChannelSets,
      sampledQuatNumQuantisationSets);

    qSetQuatBasis->computeBasisVectors(
      numChannelSets,
      sectionQuatKeysTable,
      sectionQuatKeyCountsTable);

    const NMP::Vector3* qSetMin = qSetQuatBasis->getQSetMin();
    const NMP::Vector3* qSetMax = qSetQuatBasis->getQSetMax();
    const uint32_t* chanSetToQSetMapX = qSetQuatBasis->getChanSetToQSetMapX();
    const uint32_t* chanSetToQSetMapY = qSetQuatBasis->getChanSetToQSetMapY();
    const uint32_t* chanSetToQSetMapZ = qSetQuatBasis->getChanSetToQSetMapZ();

    //-----------------------
    // Store the quantisation set basis vectors
    NMP::Memory::Format memReqsQSetMinMax(sizeof(NMP::Vector3) * sampledQuatNumQuantisationSets, NMP_VECTOR_ALIGNMENT);
    NMP::Memory::Resource memResQSetMin = NMPMemoryAllocateFromFormat(memReqsQSetMinMax);
    ZeroMemory(memResQSetMin.ptr, memReqsQSetMinMax.size);
    m_qSetQuatMin = (NMP::Vector3*)memResQSetMin.ptr;
    NMP_VERIFY(m_qSetQuatMin);
    NMP::Memory::Resource memResQSetMax = NMPMemoryAllocateFromFormat(memReqsQSetMinMax);
    ZeroMemory(memResQSetMax.ptr, memReqsQSetMinMax.size);
    m_qSetQuatMax = (NMP::Vector3*)memResQSetMax.ptr;
    NMP_VERIFY(m_qSetQuatMax);

    for (uint32_t i = 0; i < sampledQuatNumQuantisationSets; ++i)
    {
      m_qSetQuatMin[i] = qSetMin[i];
      m_qSetQuatMax[i] = qSetMax[i];
    }

    //-----------------------
    // Un-flatten the compounded channel set quantisation mappings
    indx = 0;
    for (uint32_t iFrameSection = 0; iFrameSection < m_numFrameSections; ++iFrameSection)
    {
      for (uint32_t iChannelSection = 0; iChannelSection < m_numChannelSections; ++iChannelSection)
      {
        // Get the sampled pos relative transforms table
        SubSectionIntermediateNSA* subSection = getSubSection(iFrameSection, iChannelSection);
        const RotVecTable* sampledQuatRelTable = subSection->getSampledQuatRelTable();
        if (sampledQuatRelTable)
        {
          uint32_t numChannelEntries = sampledQuatRelTable->getNumChannels();

          // Get the section channel to quantisation set map
          const uint32_t* sectionChanToQSetMapX = &chanSetToQSetMapX[indx];
          const uint32_t* sectionChanToQSetMapY = &chanSetToQSetMapY[indx];
          const uint32_t* sectionChanToQSetMapZ = &chanSetToQSetMapZ[indx];
          indx += numChannelEntries;

          // Store the channel to quantisation set maps
          subSection->computeSampledQuatCompToQSetMaps(
            sectionChanToQSetMapX,
            sectionChanToQSetMapY,
            sectionChanToQSetMapZ);
        }
      }
    }

    //-----------------------
    // Tidy up
    NMP::Memory::memFree(sectionQuatKeysTable);
    NMP::Memory::memFree(sectionQuatKeyCountsTable);
    NMP::Memory::memFree(qSetQuatBasis);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SampledDataIntermediateNSA::computeSampledQuatQuantisedData(
  const AnimSourceUncompressed* animSource,
  const SampledDataLayoutNSA* sampledDataLayout)
{
  NMP_VERIFY(animSource);
  NMP_VERIFY(sampledDataLayout);

  // Quantise the channel set data using the quantisation set basis vectors
  for (uint32_t iFrameSection = 0; iFrameSection < m_numFrameSections; ++iFrameSection)
  {
    for (uint32_t iChannelSection = 0; iChannelSection < m_numChannelSections; ++iChannelSection)
    {
      SubSectionIntermediateNSA* subSection = getSubSection(iFrameSection, iChannelSection);
      NMP_VERIFY(subSection);
      subSection->computeSampledQuatQuantisedData(
        animSource,
        sampledDataLayout,
        iFrameSection,
        iChannelSection,
        m_qSetQuatMin,
        m_qSetQuatMax);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// ChannelCompressionOptionsNSA
//----------------------------------------------------------------------------------------------------------------------
ChannelCompressionOptionsNSA::ChannelCompressionOptionsNSA()
{
  // Default options
  m_quantisationSetQuality = 1.0f / 4.0f; // Keep only a quarter of the quantisation range information
  m_byteBudgetPerSection = MR::getMaxAnimSectionSize();
  m_maxFramesPerSection = 30;
}

//----------------------------------------------------------------------------------------------------------------------
ChannelCompressionOptionsNSA::~ChannelCompressionOptionsNSA()
{
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelCompressionOptionsNSA::parseOptions(
  const char* options,
  NMP::BasicLogger* NMP_UNUSED(errorLogger))
{
  // Temporary buffer used to tokenise the options string
  NMP_VERIFY(options);
  std::string stringOptions = options;
  char* buffer = (char*)stringOptions.c_str();

  // Parse the options string
  const char seps[] = " ,\t\n";
  char* token;
  token = strtok(buffer, seps);
  while (token)
  {
    bool status = true; // Eat any unrecognised tokens

    if (stricmp(token, "-mfps") == 0)
    {
      //-----------------------
      // Max frames per section: -mfps <numFrames>
      status = false;
      token = strtok(0, seps);
      if (token && token[0] != '-')
      {
        m_maxFramesPerSection = (uint32_t)atoi(token);
        m_maxFramesPerSection = NMP::maximum(m_maxFramesPerSection, static_cast<uint32_t>(2));
        status = true;
      }
    }
    else if (stricmp(token, "-qset") == 0)
    {
      //-----------------------
      // Quantisation set quality: -qset <value>
      status = false;
      token = strtok(0, seps);
      if (token && token[0] != '-')
      {
        m_quantisationSetQuality = (float)atof(token);
        m_quantisationSetQuality = NMP::clampValue(m_quantisationSetQuality, 0.0f, 1.0f);
        status = true;
      }
    }
  
    // Get the next token if required
    if (status)
    {
      token = strtok(0, seps);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelCompressionOptionsNSA::initChannelMethods(const AnimSourceUncompressed* animSource)
{
  NMP_VERIFY(animSource);
  const ChannelSetRequirements* channelSetReqs = animSource->getChannelSetRequirements();
  NMP_VERIFY(channelSetReqs);
  uint32_t numAnimChannels = animSource->getNumChannelSets();

  for (uint32_t i = 0; i < numAnimChannels; ++i)
  {
    // Pos
    if (channelSetReqs->isChannelPosUnchanging(i))
      m_posChannelMethods.push_back(Unchanging);
    else
      m_posChannelMethods.push_back(Sampled);

    // Quat
    if (channelSetReqs->isChannelQuatUnchanging(i))
      m_quatChannelMethods.push_back(Unchanging);
    else
      m_quatChannelMethods.push_back(Sampled);
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool ChannelCompressionOptionsNSA::hasSampledChannelMethods() const
{
  uint32_t numAnimChannels = (uint32_t)m_posChannelMethods.size();

  // Pos
  for (uint32_t i = 0; i < numAnimChannels; ++i)
  {
    if (m_posChannelMethods[i] == ChannelCompressionOptionsNSA::Sampled)
      return true;
  }

  // Quat
  for (uint32_t i = 0; i < numAnimChannels; ++i)
  {
    if (m_quatChannelMethods[i] == ChannelCompressionOptionsNSA::Sampled)
      return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
// UnchangingDataLayoutNSA
//----------------------------------------------------------------------------------------------------------------------
UnchangingDataLayoutNSA::UnchangingDataLayoutNSA() :
  m_unchangingPosCompToAnimMap(NULL),
  m_unchangingQuatCompToAnimMap(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
UnchangingDataLayoutNSA::~UnchangingDataLayoutNSA()
{
  tidy();
}

//----------------------------------------------------------------------------------------------------------------------
void UnchangingDataLayoutNSA::tidy()
{
  if (m_unchangingPosCompToAnimMap)
  {
    delete m_unchangingPosCompToAnimMap;
    m_unchangingPosCompToAnimMap = NULL;
  }

  if (m_unchangingQuatCompToAnimMap)
  {
    delete m_unchangingQuatCompToAnimMap;
    m_unchangingQuatCompToAnimMap = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void UnchangingDataLayoutNSA::computeLayout(
  const AnimSourceUncompressed* animSource,
  const ChannelCompressionOptionsNSA* channelOptions)
{
  NMP_VERIFY(animSource);
  NMP_VERIFY(channelOptions);
  uint32_t numAnimChannels = channelOptions->getNumAnimChannels();
  const std::vector<ChannelCompressionOptionsNSA::ChannelCompressionType>& posChannelMethods = channelOptions->getPosChannelMethods();
  const std::vector<ChannelCompressionOptionsNSA::ChannelCompressionType>& quatChannelMethods = channelOptions->getQuatChannelMethods();
  
  // Gather the set of unchanging pos channels
  std::vector<uint32_t> unchangingPosAnimChans;
  for (uint32_t i = 0; i < numAnimChannels; ++i)
  {
    if (posChannelMethods[i] == ChannelCompressionOptionsNSA::Unchanging)
      unchangingPosAnimChans.push_back(i);
  }
  
  // Gather the set of unchanging quat channels
  std::vector<uint32_t> unchangingQuatAnimChans;
  for (uint32_t i = 0; i < numAnimChannels; ++i)
  {
    if (quatChannelMethods[i] == ChannelCompressionOptionsNSA::Unchanging)
      unchangingQuatAnimChans.push_back(i);
  }
  
  // Pos
  NMP_VERIFY(!m_unchangingPosCompToAnimMap);
  m_unchangingPosCompToAnimMap = new CompToAnimChannelMapLayout;
  m_unchangingPosCompToAnimMap->setAnimChannels(unchangingPosAnimChans);

  // Quat
  NMP_VERIFY(!m_unchangingQuatCompToAnimMap);
  m_unchangingQuatCompToAnimMap = new CompToAnimChannelMapLayout;
  m_unchangingQuatCompToAnimMap->setAnimChannels(unchangingQuatAnimChans);
}

//----------------------------------------------------------------------------------------------------------------------
// UnchangingDataIntermediateNSA
//----------------------------------------------------------------------------------------------------------------------
UnchangingDataIntermediateNSA::UnchangingDataIntermediateNSA() :
  m_unchangingPosTable(NULL),
  m_unchangingPosQuantisation(NULL),
  m_unchangingPosQuantisedTable(NULL),
  m_unchangingQuatTable(NULL),
  m_unchangingQuatQuantisation(NULL),
  m_unchangingQuatQuantisedTable(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
UnchangingDataIntermediateNSA::~UnchangingDataIntermediateNSA()
{
  tidy();
}

//----------------------------------------------------------------------------------------------------------------------
void UnchangingDataIntermediateNSA::tidy()
{
  if (m_unchangingPosTable)
  {
    NMP::Memory::memFree(m_unchangingPosTable);
    m_unchangingPosTable = NULL;
  }

  if (m_unchangingPosQuantisation)
  {
    NMP::Memory::memFree(m_unchangingPosQuantisation);
    m_unchangingPosQuantisation = NULL;
  }

  if (m_unchangingPosQuantisedTable)
  {
    NMP::Memory::memFree(m_unchangingPosQuantisedTable);
    m_unchangingPosQuantisedTable = NULL;
  }

  if (m_unchangingQuatTable)
  {
    NMP::Memory::memFree(m_unchangingQuatTable);
    m_unchangingQuatTable = NULL;
  }

  if (m_unchangingQuatQuantisation)
  {
    NMP::Memory::memFree(m_unchangingQuatQuantisation);
    m_unchangingQuatQuantisation = NULL;
  }

  if (m_unchangingQuatQuantisedTable)
  {
    NMP::Memory::memFree(m_unchangingQuatQuantisedTable);
    m_unchangingQuatQuantisedTable = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void UnchangingDataIntermediateNSA::computeUnchangingData(
  const AnimSourceUncompressed* animSource,
  const UnchangingDataLayoutNSA* unchangingDataLayout)
{
  computeUnchangingPosQuantisedData(animSource, unchangingDataLayout);
  computeUnchangingQuatQuantisedData(animSource, unchangingDataLayout);
}

//----------------------------------------------------------------------------------------------------------------------
void UnchangingDataIntermediateNSA::computeUnchangingPosQuantisedData(
  const AnimSourceUncompressed* animSource,
  const UnchangingDataLayoutNSA* unchangingDataLayout)
{
  NMP_VERIFY(animSource);
  NMP_VERIFY(unchangingDataLayout);
  const ChannelSetTable* channelSets = animSource->getChannelSets();
  NMP_VERIFY(channelSets);
  
  const CompToAnimChannelMapLayout* unchangingPosCompToAnimMap = unchangingDataLayout->getUnchangingPosCompToAnimMap();
  NMP_VERIFY(unchangingPosCompToAnimMap);
  uint32_t unchangingPosNumChannels = unchangingPosCompToAnimMap->getNumEntries();
  if (unchangingPosNumChannels > 0)
  {
    //-----------------------
    // Allocate the memory for the unchanging pos keys table
    NMP_VERIFY(!m_unchangingPosTable);
    NMP::Memory::Format memReqsPosTable = AP::Vector3Table::getMemoryRequirements(unchangingPosNumChannels, 1);
    NMP::Memory::Resource memResPosTable = NMPMemoryAllocateFromFormat(memReqsPosTable);
    ZeroMemory(memResPosTable.ptr, memReqsPosTable.size);
    m_unchangingPosTable = AP::Vector3Table::init(memResPosTable, unchangingPosNumChannels, 1);
  
    // Set the unchanging table data
    NMP::Vector3 posKey;
    for (uint32_t iChan = 0; iChan < unchangingPosNumChannels; ++iChan)
    {
      uint32_t animChanIndx = unchangingPosCompToAnimMap->getAnimChannel(iChan);
      channelSets->getPosKey(animChanIndx, 0, posKey);
      m_unchangingPosTable->setKey(iChan, 0, posKey);
    }
    
    //-----------------------
    // Allocate the memory for the global unchanging pos key table quantisation info
    NMP_VERIFY(!m_unchangingPosQuantisation);
    NMP::Memory::Format memReqsPosQuat = AP::Vector3QuantisationTable::getMemoryRequirements(unchangingPosNumChannels);
    NMP::Memory::Resource memResPosQuat = NMPMemoryAllocateFromFormat(memReqsPosQuat);
    ZeroMemory(memResPosQuat.ptr, memReqsPosQuat.size);
    m_unchangingPosQuantisation = AP::Vector3QuantisationTable::init(memResPosQuat, unchangingPosNumChannels);
    m_unchangingPosQuantisation->setPrecisionsX(16);
    m_unchangingPosQuantisation->setPrecisionsY(16);
    m_unchangingPosQuantisation->setPrecisionsZ(16);
    
    // qMin, qMax of each channel globally
    m_unchangingPosQuantisation->setQuantisationBounds(m_unchangingPosTable);
    m_unchangingPosQuantisation->setGlobalQuantisationBounds();
    
    //-----------------------
    // Allocate the memory for the quantised unchanging pos keys
    NMP_VERIFY(!m_unchangingPosQuantisedTable);
    NMP::Memory::Format memReqsPos = IntVector3Table::getMemoryRequirements(unchangingPosNumChannels, 1);
    NMP::Memory::Resource memResPos = NMPMemoryAllocateFromFormat(memReqsPos);
    ZeroMemory(memResPos.ptr, memReqsPos.size);
    m_unchangingPosQuantisedTable = IntVector3Table::init(memResPos, unchangingPosNumChannels, 1);
    
    // Quantise the unchanging pos samples
    uint32_t qPosKey[3];
    for (uint32_t iChan = 0; iChan < unchangingPosNumChannels; ++iChan)
    {
      m_unchangingPosTable->getKey(iChan, 0, posKey);
      m_unchangingPosQuantisation->quantise(iChan, posKey, qPosKey[0], qPosKey[1], qPosKey[2]);
      m_unchangingPosQuantisedTable->setKey(iChan, 0, (const int32_t*)qPosKey);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void UnchangingDataIntermediateNSA::computeUnchangingQuatQuantisedData(
  const AnimSourceUncompressed* animSource,
  const UnchangingDataLayoutNSA* unchangingDataLayout)
{  
  NMP_VERIFY(animSource);
  NMP_VERIFY(unchangingDataLayout);
  const ChannelSetTable* channelSets = animSource->getChannelSets();
  NMP_VERIFY(channelSets);

  const CompToAnimChannelMapLayout* unchangingQuatCompToAnimMap = unchangingDataLayout->getUnchangingQuatCompToAnimMap();
  NMP_VERIFY(unchangingQuatCompToAnimMap);
  uint32_t unchangingQuatNumChannels = unchangingQuatCompToAnimMap->getNumEntries();
  if (unchangingQuatNumChannels > 0)
  {
    //-----------------------
    // Allocate the memory for the unchanging quat keys table
    NMP_VERIFY(!m_unchangingQuatTable);
    NMP::Memory::Format memReqsQuatTable = AP::RotVecTable::getMemoryRequirements(unchangingQuatNumChannels, 1);
    NMP::Memory::Resource memResQuatTable = NMPMemoryAllocateFromFormat(memReqsQuatTable);
    ZeroMemory(memResQuatTable.ptr, memReqsQuatTable.size);
    m_unchangingQuatTable = AP::RotVecTable::init(memResQuatTable, unchangingQuatNumChannels, 1);

    // Set the unchanging table data
    NMP::Quat quatKey;
    for (uint32_t iChan = 0; iChan < unchangingQuatNumChannels; ++iChan)
    {
      uint32_t animChanIndx = unchangingQuatCompToAnimMap->getAnimChannel(iChan);
      channelSets->getQuatKey(animChanIndx, 0, quatKey);
      m_unchangingQuatTable->setKey(iChan, 0, quatKey, true); // tan quarter angle rotation vector
    }

    //-----------------------
    // Allocate the memory for the global unchanging quat key table quantisation info
    NMP_VERIFY(!m_unchangingQuatQuantisation);
    NMP::Memory::Format memReqsQuatQuat = AP::Vector3QuantisationTable::getMemoryRequirements(unchangingQuatNumChannels);
    NMP::Memory::Resource memResQuatQuat = NMPMemoryAllocateFromFormat(memReqsQuatQuat);
    ZeroMemory(memResQuatQuat.ptr, memReqsQuatQuat.size);
    m_unchangingQuatQuantisation = AP::Vector3QuantisationTable::init(memResQuatQuat, unchangingQuatNumChannels);
    m_unchangingQuatQuantisation->setPrecisionsX(16);
    m_unchangingQuatQuantisation->setPrecisionsY(16);
    m_unchangingQuatQuantisation->setPrecisionsZ(16);

    // qMin, qMax of each channel globally
    m_unchangingQuatQuantisation->setQuantisationBounds(m_unchangingQuatTable);
    m_unchangingQuatQuantisation->setGlobalQuantisationBounds();

    //-----------------------
    // Allocate the memory for the quantised unchanging pos keys
    NMP_VERIFY(!m_unchangingQuatQuantisedTable);
    NMP::Memory::Format memReqsQuat = IntVector3Table::getMemoryRequirements(unchangingQuatNumChannels, 1);
    NMP::Memory::Resource memResQuat = NMPMemoryAllocateFromFormat(memReqsQuat);
    ZeroMemory(memResQuat.ptr, memReqsQuat.size);
    m_unchangingQuatQuantisedTable = IntVector3Table::init(memResQuat, unchangingQuatNumChannels, 1);

    // Quantise the unchanging pos samples
    NMP::Vector3 quatKetTQA;
    uint32_t qQuatKey[3];
    for (uint32_t iChan = 0; iChan < unchangingQuatNumChannels; ++iChan)
    {
      m_unchangingQuatTable->getKey(iChan, 0, quatKetTQA);
      m_unchangingQuatQuantisation->quantise(iChan, quatKetTQA, qQuatKey[0], qQuatKey[1], qQuatKey[2]);
      m_unchangingQuatQuantisedTable->setKey(iChan, 0, (const int32_t*)qQuatKey);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// UnchangingDataCompiledNSA
//----------------------------------------------------------------------------------------------------------------------
UnchangingDataCompiledNSA::UnchangingDataCompiledNSA() :
  m_unchangingPosCompToAnimMap(NULL),
  m_unchangingQuatCompToAnimMap(NULL),
  m_unchangingData(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
UnchangingDataCompiledNSA::~UnchangingDataCompiledNSA()
{
  tidy();
}

//----------------------------------------------------------------------------------------------------------------------
void UnchangingDataCompiledNSA::tidy()
{
  if (m_unchangingPosCompToAnimMap)
  {
    NMP::Memory::memFree(m_unchangingPosCompToAnimMap);
    m_unchangingPosCompToAnimMap = NULL;
  }
  
  if (m_unchangingQuatCompToAnimMap)
  {
    NMP::Memory::memFree(m_unchangingQuatCompToAnimMap);
    m_unchangingQuatCompToAnimMap = NULL;
  }

  if (m_unchangingData)
  {
    NMP::Memory::memFree(m_unchangingData);
    m_unchangingData = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void UnchangingDataCompiledNSA::compileUnchangingData(
  const AnimSourceUncompressed* animSource,
  const UnchangingDataLayoutNSA* unchangingDataLayout)
{
  NMP_VERIFY(animSource);
  NMP_VERIFY(unchangingDataLayout); 

  //-----------------------
  // Compute the intermediate data used to compile the binary components of the unchanging data
  UnchangingDataIntermediateNSA* unchangingDataIntermediate = new UnchangingDataIntermediateNSA;
  unchangingDataIntermediate->computeUnchangingData(
    animSource,
    unchangingDataLayout);
    
  //----------------------- 
  // Compile the unchanging pos components
  MR::QuantisationScaleAndOffsetVec3 unchangingPosQuantisationInfo;
  MR::UnchangingKeyVec3* unchangingPosData = NULL;
  const CompToAnimChannelMapLayout* unchangingPosCompToAnimMap = unchangingDataLayout->getUnchangingPosCompToAnimMap();
  NMP_VERIFY(unchangingPosCompToAnimMap);
  uint32_t unchangingPosNumChannels = unchangingPosCompToAnimMap->getNumEntries();
  if (unchangingPosNumChannels > 0)
  {
    // Quantisation basis
    const Vector3QuantisationTable* unchangingPosQuantisation = unchangingDataIntermediate->getUnchangingPosQuantisation();
    NMP_VERIFY(unchangingPosQuantisation);
    NMP::Vector3 qScale = unchangingPosQuantisation->getQuantisationScales(0);
    NMP::Vector3 qOffset = unchangingPosQuantisation->getQuantisationOffsets(0);
    unchangingPosQuantisationInfo.pack(qScale, qOffset);

    // Quantised unchanging data
    const IntVector3Table* unchangingPosQuantisedTable = unchangingDataIntermediate->getUnchangingPosQuantisedTable();
    NMP_VERIFY(unchangingPosQuantisedTable);
    NMP::Memory::Format memReqsPos(sizeof(MR::UnchangingKeyVec3) * unchangingPosNumChannels, NMP_NATURAL_TYPE_ALIGNMENT);
    NMP::Memory::Resource memResPos = NMPMemoryAllocateFromFormat(memReqsPos);
    ZeroMemory(memResPos.ptr, memReqsPos.size);
    unchangingPosData = (MR::UnchangingKeyVec3*) memResPos.ptr;
    NMP_VERIFY(unchangingPosData);

    uint32_t qPosKey[3];
    for (uint32_t iChan = 0; iChan < unchangingPosNumChannels; ++iChan)
    {
      unchangingPosQuantisedTable->getKey(iChan, 0, (int32_t*)qPosKey);
      unchangingPosData[iChan].pack(qPosKey);
    }
  }
  else
  {
    unchangingPosQuantisationInfo.zero();
  }
  
  //----------------------- 
  // Compile the unchanging quat components
  MR::QuantisationScaleAndOffsetVec3 unchangingQuatQuantisationInfo;
  MR::UnchangingKeyVec3* unchangingQuatData = NULL;
  const CompToAnimChannelMapLayout* unchangingQuatCompToAnimMap = unchangingDataLayout->getUnchangingQuatCompToAnimMap();
  NMP_VERIFY(unchangingQuatCompToAnimMap);
  uint32_t unchangingQuatNumChannels = unchangingQuatCompToAnimMap->getNumEntries();
  if (unchangingQuatNumChannels > 0)
  {
    // Quantisation basis
    const Vector3QuantisationTable* unchangingQuatQuantisation = unchangingDataIntermediate->getUnchangingQuatQuantisation();
    NMP_VERIFY(unchangingQuatQuantisation);
    NMP::Vector3 qScale = unchangingQuatQuantisation->getQuantisationScales(0);
    NMP::Vector3 qOffset = unchangingQuatQuantisation->getQuantisationOffsets(0);
    unchangingQuatQuantisationInfo.pack(qScale, qOffset);

    // Quantised unchanging data
    const IntVector3Table* unchangingQuatQuantisedTable = unchangingDataIntermediate->getUnchangingQuatQuantisedTable();
    NMP_VERIFY(unchangingQuatQuantisedTable);
    NMP::Memory::Format memReqsQuat(sizeof(MR::UnchangingKeyVec3) * unchangingQuatNumChannels, NMP_NATURAL_TYPE_ALIGNMENT);
    NMP::Memory::Resource memResQuat = NMPMemoryAllocateFromFormat(memReqsQuat);
    ZeroMemory(memResQuat.ptr, memReqsQuat.size);
    unchangingQuatData = (MR::UnchangingKeyVec3*) memResQuat.ptr;
    NMP_VERIFY(unchangingQuatData);

    uint32_t qQuatKey[3];
    for (uint32_t iChan = 0; iChan < unchangingQuatNumChannels; ++iChan)
    {
      unchangingQuatQuantisedTable->getKey(iChan, 0, (int32_t*)qQuatKey);
      unchangingQuatData[iChan].pack(qQuatKey);
    }
  }
  else
  {
    unchangingQuatQuantisationInfo.zero();
  }

  //-----------------------
  // Allocate the memory for the unchanging data
  NMP::Memory::Format memReqsUnchangingData = UnchangingDataNSABuilder::getMemoryRequirements(
    unchangingPosNumChannels,
    unchangingQuatNumChannels);
  
  NMP::Memory::Resource memResUnchangingData = NMPMemoryAllocateFromFormat(memReqsUnchangingData);
  ZeroMemory(memResUnchangingData.ptr, memReqsUnchangingData.size);

  // Build the unchanging data
  NMP_VERIFY(!m_unchangingData);
  m_unchangingData = UnchangingDataNSABuilder::init(
    memResUnchangingData,
    unchangingPosNumChannels,
    unchangingQuatNumChannels,
    unchangingPosQuantisationInfo,
    unchangingQuatQuantisationInfo,
    unchangingPosData,
    unchangingQuatData);

  // Check all the memory was used correctly
  NMP_VERIFY_MSG(
    memResUnchangingData.format.size == 0,
    "Not all the allocated memory was used when compiling the unchanging data : %d bytes remain.",
    memResUnchangingData.format.size);

  //-----------------------
  // Tidy up
  if (unchangingPosNumChannels > 0)
  {
    NMP_VERIFY(unchangingPosData);
    NMP::Memory::memFree(unchangingPosData);
  }

  if (unchangingQuatNumChannels > 0)
  {
    NMP_VERIFY(unchangingQuatData);
    NMP::Memory::memFree(unchangingQuatData);
  }

  delete unchangingDataIntermediate;
}

//----------------------------------------------------------------------------------------------------------------------
void UnchangingDataCompiledNSA::compileCompToAnimMaps(const UnchangingDataLayoutNSA* unchangingDataLayout)
{
  NMP_VERIFY(unchangingDataLayout);
  
  //-----------------------
  // Pos
  const CompToAnimChannelMapLayout* unchangingPosCompToAnimMap = unchangingDataLayout->getUnchangingPosCompToAnimMap();
  NMP_VERIFY(unchangingPosCompToAnimMap);
  const uint32_t* animChannelsPosData = NULL;
  uint32_t unchangingPosNumChannels = unchangingPosCompToAnimMap->getNumEntries();
  if (unchangingPosNumChannels > 0)
  {
    animChannelsPosData = &(unchangingPosCompToAnimMap->getAnimChannels()[0]);
  }
  
  NMP::Memory::Format memReqsPosCompToAnimMap = MR::CompToAnimChannelMap::getMemoryRequirements(unchangingPosNumChannels);
  NMP::Memory::Resource memResPosCompToAnimMap = NMPMemoryAllocateFromFormat(memReqsPosCompToAnimMap);
  ZeroMemory(memResPosCompToAnimMap.ptr, memReqsPosCompToAnimMap.size);
  m_unchangingPosCompToAnimMap = MR::CompToAnimChannelMap::init(
    memResPosCompToAnimMap,
    unchangingPosNumChannels,
    animChannelsPosData);
  NMP_VERIFY(m_unchangingPosCompToAnimMap);
  
  //-----------------------
  // Quat
  const CompToAnimChannelMapLayout* unchangingQuatCompToAnimMap = unchangingDataLayout->getUnchangingQuatCompToAnimMap();
  NMP_VERIFY(unchangingQuatCompToAnimMap);
  const uint32_t* animChannelsQuatData = NULL;
  uint32_t unchangingQuatNumChannels = unchangingQuatCompToAnimMap->getNumEntries();
  if (unchangingQuatNumChannels > 0)
  {
    animChannelsQuatData = &(unchangingQuatCompToAnimMap->getAnimChannels()[0]);
  }

  NMP::Memory::Format memReqsQuatCompToAnimMap = MR::CompToAnimChannelMap::getMemoryRequirements(unchangingQuatNumChannels);
  NMP::Memory::Resource memResQuatCompToAnimMap = NMPMemoryAllocateFromFormat(memReqsQuatCompToAnimMap);
  ZeroMemory(memResQuatCompToAnimMap.ptr, memReqsQuatCompToAnimMap.size);
  m_unchangingQuatCompToAnimMap = MR::CompToAnimChannelMap::init(
    memResQuatCompToAnimMap,
    unchangingQuatNumChannels,
    animChannelsQuatData);
  NMP_VERIFY(m_unchangingQuatCompToAnimMap);
}

//----------------------------------------------------------------------------------------------------------------------
// SampledDataNSALayout
//----------------------------------------------------------------------------------------------------------------------
SampledDataLayoutNSA::SampledDataLayoutNSA() :
  m_numFrameSections(0),
  m_numChannelSections(0),
  m_sampledPosNumQuantisationSets(0),
  m_sampledQuatNumQuantisationSets(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
SampledDataLayoutNSA::~SampledDataLayoutNSA()
{
  tidy();
}

//----------------------------------------------------------------------------------------------------------------------
void SampledDataLayoutNSA::tidy()
{
  m_numFrameSections = 0;
  m_numChannelSections = 0;
  
  m_sampledPosNumQuantisationSets = 0;
  m_sampledQuatNumQuantisationSets = 0;

  m_sectionStartFrames.clear();
  m_sectionEndFrames.clear();

  // Pos
  for (uint32_t i = 0; i < m_sampledPosCompToAnimMaps.size(); ++i)
    delete m_sampledPosCompToAnimMaps[i];
  m_sampledPosCompToAnimMaps.clear();

  // Quat
  for (uint32_t i = 0; i < m_sampledQuatCompToAnimMaps.size(); ++i)
    delete m_sampledQuatCompToAnimMaps[i];
  m_sampledQuatCompToAnimMaps.clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool SampledDataLayoutNSA::computeLayoutSingleChannelSection(
  const AnimSourceUncompressed* animSource,
  const ChannelCompressionOptionsNSA* channelOptions,
  const std::vector<uint32_t>& sampledPosAnimChans,
  const std::vector<uint32_t>& sampledQuatAnimChans)
{
  NMP_VERIFY(animSource);
  NMP_VERIFY(channelOptions);

  // Channel type information
  const uint32_t numChannelTypes = 2;
  const std::vector<uint32_t>* channelTypeAnimIds[numChannelTypes];
  channelTypeAnimIds[0] = &sampledPosAnimChans;
  channelTypeAnimIds[1] = &sampledQuatAnimChans;
  size_t memReqsPerChannelType[numChannelTypes];
  memReqsPerChannelType[0] = sizeof(MR::SampledPosKey);
  memReqsPerChannelType[1] = sizeof(MR::SampledQuatKeyTQA);

  uint32_t numAnimFrames = animSource->getMaxNumKeyframes();
  NMP_VERIFY(numAnimFrames > 0);
  uint32_t numSampledPosChannels = (uint32_t)sampledPosAnimChans.size();
  uint32_t numSampledQuatChannels = (uint32_t)sampledQuatAnimChans.size();
  uint32_t totalNumChannels = numSampledPosChannels + numSampledQuatChannels;
  NMP_VERIFY(totalNumChannels > 0);
  
  //-----------------------
  size_t byteBudgetPerSection = channelOptions->getByteBudgetPerSection();
  NMP_VERIFY(byteBudgetPerSection > 0);

  // Calculate the distribution of channels (Single section)
  SectionDataChannelLayout* sectionDataLayout = new SectionDataChannelLayout;
  NMP_VERIFY(sectionDataLayout);
  sectionDataLayout->computeLayout(
    1,
    numChannelTypes,
    channelTypeAnimIds,
    memReqsPerChannelType);

  // Calculate the distribution of frames to achieve the frames per section limit.
  // Use truncation so that we do not exceed the limit.
  uint32_t maxFramesPerSection = channelOptions->getMaxFramesPerSection();
  std::vector<uint32_t> sectionStartFrames;
  std::vector<uint32_t> sectionEndFrames;
  uint32_t numFrameSections = sectionAnimFramesBySamplesPerSection(
    maxFramesPerSection,
    numAnimFrames,
    false,
    sectionStartFrames,
    sectionEndFrames);
    
  // Compute the byte budget of the largest channel section
  NMP::Memory::Format memReqsSection = getLayoutMemoryRequirements(
    sectionStartFrames,
    sectionEndFrames,
    sectionDataLayout);
    
  bool result = false;
  if (memReqsSection.size <= byteBudgetPerSection)
  {
    // The data fits into a single channel section with the required frames per section.
    // Find the fewest number of sections that can fit the data into a single channel section.
    for (uint32_t iSections = numFrameSections - 1; iSections > 0; --iSections)
    {
      // Calculate the new distribution of frames
      sectionAnimFramesByNumSections(
        iSections,
        numAnimFrames,
        sectionStartFrames,
        sectionEndFrames);
        
      // Compute the byte budget of the largest channel section
      memReqsSection = getLayoutMemoryRequirements(
        sectionStartFrames,
        sectionEndFrames,
        sectionDataLayout);
        
      // Check if we are within the byte budget
      if (memReqsSection.size > byteBudgetPerSection)
        break;
        
      // Update the number of frame sections
      numFrameSections = iSections;
    }
    
    // Re-compute the distribution of anim frames
    sectionAnimFramesByNumSections(
      numFrameSections,
      numAnimFrames,
      sectionStartFrames,
      sectionEndFrames);
      
    // Set the layout
    setLayout(
      sectionStartFrames,
      sectionEndFrames,
      sectionDataLayout);
      
    result = true;
  }

  // Tidy up
  delete sectionDataLayout;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool SampledDataLayoutNSA::computeLayoutMultipleChannelSections(
  const AnimSourceUncompressed* animSource,
  const ChannelCompressionOptionsNSA* channelOptions,
  const std::vector<uint32_t>& sampledPosAnimChans,
  const std::vector<uint32_t>& sampledQuatAnimChans)
{
  NMP_VERIFY(animSource);
  NMP_VERIFY(channelOptions);

  // Channel type information
  const uint32_t numChannelTypes = 2;
  const std::vector<uint32_t>* channelTypeAnimIds[numChannelTypes];
  channelTypeAnimIds[0] = &sampledPosAnimChans;
  channelTypeAnimIds[1] = &sampledQuatAnimChans;
  size_t memReqsPerChannelType[numChannelTypes];
  memReqsPerChannelType[0] = sizeof(MR::SampledPosKey);
  memReqsPerChannelType[1] = sizeof(MR::SampledQuatKeyTQA);

  uint32_t numAnimFrames = animSource->getMaxNumKeyframes();
  NMP_VERIFY(numAnimFrames > 0);
  uint32_t numSampledPosChannels = (uint32_t)sampledPosAnimChans.size();
  uint32_t numSampledQuatChannels = (uint32_t)sampledQuatAnimChans.size();
  uint32_t totalNumChannels = numSampledPosChannels + numSampledQuatChannels;
  NMP_VERIFY(totalNumChannels > 0);

  //-----------------------
  size_t byteBudgetPerSection = channelOptions->getByteBudgetPerSection();
  NMP_VERIFY(byteBudgetPerSection > 0);

  // Calculate the distribution of frames to achieve the frames per section limit.
  // Use truncation so that we do not exceed the limit.
  uint32_t maxFramesPerSection = channelOptions->getMaxFramesPerSection();
  std::vector<uint32_t> sectionStartFrames;
  std::vector<uint32_t> sectionEndFrames;
  sectionAnimFramesBySamplesPerSection(
    maxFramesPerSection,
    numAnimFrames,
    false,
    sectionStartFrames,
    sectionEndFrames);

  SectionDataChannelLayout* sectionDataLayout = NULL;
  for (uint32_t iChanSections = 1; iChanSections < totalNumChannels; ++iChanSections)
  {
    // Delete the old section data layout
    if (sectionDataLayout)
    {
      delete sectionDataLayout;
      sectionDataLayout = NULL;
    }

    // Calculate the distribution of channels (Single section)
    sectionDataLayout = new SectionDataChannelLayout;
    NMP_VERIFY(sectionDataLayout);
    sectionDataLayout->computeLayout(
      iChanSections,
      numChannelTypes,
      channelTypeAnimIds,
      memReqsPerChannelType);

    // Compute the byte budget of the largest channel section
    NMP::Memory::Format memReqsSection = getLayoutMemoryRequirements(
      sectionStartFrames,
      sectionEndFrames,
      sectionDataLayout);

    if (memReqsSection.size <= byteBudgetPerSection)
    {
      // Set the layout
      setLayout(
        sectionStartFrames,
        sectionEndFrames,
        sectionDataLayout);
        
      // Tidy up
      delete sectionDataLayout;
      return true;
    }
  }

  // Tidy up
  if (sectionDataLayout)
    delete sectionDataLayout;
  
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void SampledDataLayoutNSA::computeLayout(
  const AnimSourceUncompressed* animSource,
  const ChannelCompressionOptionsNSA* channelOptions)
{
  NMP_VERIFY(animSource);
  NMP_VERIFY(channelOptions);
  uint32_t numAnimFrames = animSource->getMaxNumKeyframes();
  NMP_VERIFY(numAnimFrames > 0);
  uint32_t numAnimChannels = channelOptions->getNumAnimChannels();
  const std::vector<ChannelCompressionOptionsNSA::ChannelCompressionType>& posChannelMethods = channelOptions->getPosChannelMethods();
  const std::vector<ChannelCompressionOptionsNSA::ChannelCompressionType>& quatChannelMethods = channelOptions->getQuatChannelMethods();
  const uint32_t minNumQSets = 1;
  const uint32_t maxNumQSets = 16;

  //-----------------------
  // Compute the set of sampled pos channels
  std::vector<uint32_t> sampledPosAnimChans;
  for (uint32_t i = 0; i < numAnimChannels; ++i)
  {
    if (posChannelMethods[i] == ChannelCompressionOptionsNSA::Sampled)
      sampledPosAnimChans.push_back(i);
  }
  uint32_t numSampledPosChannels = (uint32_t)sampledPosAnimChans.size();

  // Gather the set of sampled quat channels
  std::vector<uint32_t> sampledQuatAnimChans;
  for (uint32_t i = 0; i < numAnimChannels; ++i)
  {
    if (quatChannelMethods[i] == ChannelCompressionOptionsNSA::Sampled)
      sampledQuatAnimChans.push_back(i);
  }
  uint32_t numSampledQuatChannels = (uint32_t)sampledQuatAnimChans.size();

  //-----------------------
  // Compute the number of quantisation sets
  float quantisationSetQuality = channelOptions->getQuantisationSetQuality();
  if (numSampledPosChannels > 0)
  {
    uint32_t numQSets = (uint32_t)(quantisationSetQuality * numSampledPosChannels + 0.5f);
    m_sampledPosNumQuantisationSets = NMP::clampValue(numQSets, minNumQSets, maxNumQSets);
  }

  if (numSampledQuatChannels > 0)
  {
    uint32_t numQSets = (uint32_t)(quantisationSetQuality * numSampledQuatChannels + 0.5f);
    m_sampledQuatNumQuantisationSets = NMP::clampValue(numQSets, minNumQSets, maxNumQSets);
  }

  // Try and compute the layout with a single channel section with as few
  // frame sections as possible
  bool result = computeLayoutSingleChannelSection(
    animSource,
    channelOptions,
    sampledPosAnimChans,
    sampledQuatAnimChans);
    
  if (!result)
  {
    // We can't fit the layout into a single channel section with the required
    // frames per section limit so compute the number of channel sections required
    // to fit data into the required byte budget.
    result = computeLayoutMultipleChannelSections(
      animSource,
      channelOptions,
      sampledPosAnimChans,
      sampledQuatAnimChans);
    NMP_VERIFY(result);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format SampledDataLayoutNSA::getLayoutMemoryRequirements(
  const std::vector<uint32_t>& sectionStartFrames,
  const std::vector<uint32_t>& sectionEndFrames,
  const SectionDataChannelLayout* sectionDataLayout)
{
  //-----------------------
  // Frame sections
  NMP_VERIFY(sectionStartFrames.size() == sectionEndFrames.size());
  uint32_t numFrameSections = (uint32_t)sectionStartFrames.size();
  NMP_VERIFY(numFrameSections > 0);
  
  // Get the largest section frame count
  uint32_t maxNumSectionFrames = 0;
  for (uint32_t iFrameSection = 0; iFrameSection < numFrameSections; ++iFrameSection)
  {
    uint32_t numSectionFrames = sectionEndFrames[iFrameSection] - sectionStartFrames[iFrameSection] + 1;
    maxNumSectionFrames = NMP::maximum(maxNumSectionFrames, numSectionFrames);
  }

  //-----------------------
  // Channel sections
  NMP_VERIFY(sectionDataLayout);
  uint32_t numChannelSections = sectionDataLayout->getNumSections();
  NMP_VERIFY(numChannelSections > 0);
  uint32_t sectionIndex = sectionDataLayout->getLargestSectionLayoutIndex();
  const SectionDataChannelLayout::SectionLayout* sectionLayout = sectionDataLayout->getSectionLayout(sectionIndex);
  NMP_VERIFY(sectionLayout);

  const CompToAnimChannelMapLayout* compToAnimMapPos = sectionLayout->getChannelTypeLayout(0);
  NMP_VERIFY(compToAnimMapPos);
  const CompToAnimChannelMapLayout* compToAnimMapQuat = sectionLayout->getChannelTypeLayout(1);
  NMP_VERIFY(compToAnimMapQuat);

  NMP::Memory::Format memReqsSection = SectionDataNSABuilder::getMemoryRequirements(
    maxNumSectionFrames,
    compToAnimMapPos->getNumEntries(),
    compToAnimMapQuat->getNumEntries());
    
  return memReqsSection;
}

//----------------------------------------------------------------------------------------------------------------------
void SampledDataLayoutNSA::setLayout(
  const std::vector<uint32_t>& sectionStartFrames,
  const std::vector<uint32_t>& sectionEndFrames,
  const SectionDataChannelLayout* sectionDataLayout)
{
  //-----------------------
  // Frame sections
  NMP_VERIFY(sectionStartFrames.size() == sectionEndFrames.size());
  m_numFrameSections = (uint32_t)sectionStartFrames.size();
  NMP_VERIFY(m_numFrameSections > 0);
  NMP_VERIFY(m_sectionStartFrames.size() == 0);
  NMP_VERIFY(m_sectionEndFrames.size() == 0);
  m_sectionStartFrames = sectionStartFrames;
  m_sectionEndFrames = sectionEndFrames;
  
  //-----------------------
  // Channel sections 
  NMP_VERIFY(sectionDataLayout);
  m_numChannelSections = sectionDataLayout->getNumSections();
  NMP_VERIFY(m_numChannelSections > 0);
  NMP_VERIFY(m_sampledPosCompToAnimMaps.size() == 0);
  NMP_VERIFY(m_sampledQuatCompToAnimMaps.size() == 0);
  for (uint32_t iChanSection = 0; iChanSection < m_numChannelSections; ++iChanSection)
  {
    const SectionDataChannelLayout::SectionLayout* sectionLayout = sectionDataLayout->getSectionLayout(iChanSection);
    NMP_VERIFY(sectionLayout);

    // Pos
    const CompToAnimChannelMapLayout* compToAnimMapPos = sectionLayout->getChannelTypeLayout(0);
    NMP_VERIFY(compToAnimMapPos);
    CompToAnimChannelMapLayout* sampledPosCompToAnimMap = new CompToAnimChannelMapLayout(*compToAnimMapPos);
    m_sampledPosCompToAnimMaps.push_back(sampledPosCompToAnimMap);

    // Quat
    const CompToAnimChannelMapLayout* compToAnimMapQuat = sectionLayout->getChannelTypeLayout(1);
    NMP_VERIFY(compToAnimMapQuat);
    CompToAnimChannelMapLayout* sampledQuatCompToAnimMap = new CompToAnimChannelMapLayout(*compToAnimMapQuat);
    m_sampledQuatCompToAnimMaps.push_back(sampledQuatCompToAnimMap);
  }
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SampledDataLayoutNSA::getSectionStartFrame(uint32_t iFrameSection) const
{
  NMP_VERIFY(iFrameSection < m_numFrameSections);
  return m_sectionStartFrames[iFrameSection];
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SampledDataLayoutNSA::getSectionEndFrame(uint32_t iFrameSection) const
{
  NMP_VERIFY(iFrameSection < m_numFrameSections);
  return m_sectionEndFrames[iFrameSection];
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SampledDataLayoutNSA::getNumSampledPosChannels() const
{
  NMP_VERIFY(m_sampledPosCompToAnimMaps.size() == m_numChannelSections);
  uint32_t result = 0;
  for (uint32_t i = 0; i < m_numChannelSections; ++i)
  {
    const CompToAnimChannelMapLayout* compToAnimMap = m_sampledPosCompToAnimMaps[i];
    NMP_VERIFY(compToAnimMap);
    result += compToAnimMap->getNumEntries();
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const CompToAnimChannelMapLayout* SampledDataLayoutNSA::getSampledPosCompToAnimMap(uint32_t iChannelSection) const
{
  NMP_VERIFY(iChannelSection < m_numChannelSections);
  return m_sampledPosCompToAnimMaps[iChannelSection];
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SampledDataLayoutNSA::getNumSampledQuatChannels() const
{
  NMP_VERIFY(m_sampledQuatCompToAnimMaps.size() == m_numChannelSections);
  uint32_t result = 0;
  for (uint32_t i = 0; i < m_numChannelSections; ++i)
  {
    const CompToAnimChannelMapLayout* compToAnimMap = m_sampledQuatCompToAnimMaps[i];
    NMP_VERIFY(compToAnimMap);
    result += compToAnimMap->getNumEntries();
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const CompToAnimChannelMapLayout* SampledDataLayoutNSA::getSampledQuatCompToAnimMap(uint32_t iChannelSection) const
{
  NMP_VERIFY(iChannelSection < m_numChannelSections);
  return m_sampledQuatCompToAnimMaps[iChannelSection];
}

//----------------------------------------------------------------------------------------------------------------------
// SubSectionNSACompiled
//----------------------------------------------------------------------------------------------------------------------
SubSectionNSACompiled::SubSectionNSACompiled() :
  m_sectionData(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
SubSectionNSACompiled::~SubSectionNSACompiled()
{
  tidy();
}

//----------------------------------------------------------------------------------------------------------------------
void SubSectionNSACompiled::tidy()
{
  if (m_sectionData)
  {
    NMP::Memory::memFree(m_sectionData);
    m_sectionData = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SubSectionNSACompiled::compileSectionData(
  const SampledDataIntermediateNSA* sampledDataIntermediate,
  const SampledDataLayoutNSA* sampledDataLayout,
  uint32_t iFrameSection,
  uint32_t iChannelSection)
{
  NMP_VERIFY(sampledDataIntermediate);
  NMP_VERIFY(sampledDataLayout);
  const SubSectionIntermediateNSA* subSectionIntermediate = sampledDataIntermediate->getSubSection(iFrameSection, iChannelSection);
  NMP_VERIFY(subSectionIntermediate);
  const CompToAnimChannelMapLayout* sampledPosCompToAnimMap = sampledDataLayout->getSampledPosCompToAnimMap(iChannelSection);
  NMP_VERIFY(sampledPosCompToAnimMap);
  uint32_t sampledPosNumChannels = sampledPosCompToAnimMap->getNumEntries();
  const CompToAnimChannelMapLayout* sampledQuatCompToAnimMap = sampledDataLayout->getSampledQuatCompToAnimMap(iChannelSection);
  NMP_VERIFY(sampledQuatCompToAnimMap);
  uint32_t sampledQuatNumChannels = sampledQuatCompToAnimMap->getNumEntries();
  uint32_t sectionStartFrame = sampledDataLayout->getSectionStartFrame(iFrameSection);
  uint32_t sectionEndFrame = sampledDataLayout->getSectionEndFrame(iFrameSection);
  uint32_t numSectionFrames = sectionEndFrame - sectionStartFrame + 1;

  // Compile the pos section data  
  MR::QuantisationMeanAndSetVec3* sampledPosQuantisationData = NULL;
  MR::SampledPosKey* sampledPosData = NULL;
  if (sampledPosNumChannels > 0)
  {
    //-----------------------
    // Compile the mean and qSet data
    const uint32_t* sampledPosMeansX = subSectionIntermediate->getSampledPosMeansX();
    NMP_VERIFY(sampledPosMeansX);
    const uint32_t* sampledPosMeansY = subSectionIntermediate->getSampledPosMeansY();
    NMP_VERIFY(sampledPosMeansY);
    const uint32_t* sampledPosMeansZ = subSectionIntermediate->getSampledPosMeansZ();
    NMP_VERIFY(sampledPosMeansZ);
    const uint32_t* sampledPosCompToQSetMapX = subSectionIntermediate->getSampledPosCompToQSetMapX();
    NMP_VERIFY(sampledPosCompToQSetMapX);
    const uint32_t* sampledPosCompToQSetMapY = subSectionIntermediate->getSampledPosCompToQSetMapY();
    NMP_VERIFY(sampledPosCompToQSetMapY);
    const uint32_t* sampledPosCompToQSetMapZ = subSectionIntermediate->getSampledPosCompToQSetMapZ();
    NMP_VERIFY(sampledPosCompToQSetMapZ);

    // Allocate the memory for the channel mean and qSet indices
    NMP::Memory::Format memReqsMeanAndSetVec3 = MR::SectionDataNSA::getMemoryReqsMeanAndSetVec3(sampledPosNumChannels);
    NMP::Memory::Resource memResPosMeanQSet = NMPMemoryAllocateFromFormat(memReqsMeanAndSetVec3);
    ZeroMemory(memResPosMeanQSet.ptr, memReqsMeanAndSetVec3.size);
    sampledPosQuantisationData = (MR::QuantisationMeanAndSetVec3*) memResPosMeanQSet.ptr;
    NMP_VERIFY(sampledPosQuantisationData);

    // Pack the mean and qSet data
    uint32_t qMean[3], qSet[3];
    for (uint32_t iChan = 0; iChan < sampledPosNumChannels; ++iChan)
    {
      qMean[0] = sampledPosMeansX[iChan];
      qMean[1] = sampledPosMeansY[iChan];
      qMean[2] = sampledPosMeansZ[iChan];
      qSet[0] = sampledPosCompToQSetMapX[iChan];
      qSet[1] = sampledPosCompToQSetMapY[iChan];
      qSet[2] = sampledPosCompToQSetMapZ[iChan];
      sampledPosQuantisationData[iChan].pack(qMean, qSet);
    }
    // The quantization data is padded to a multiple of 4 entries.
    // Check MR::SectionDataNSA::getMemoryReqsMeanAndSetVec3.
    // The last entry is duplicated to fill in the extra entries.
    uint32_t sampledPosNumChannels4 = NMP::nmAlignedValue4(sampledPosNumChannels);
    for (uint32_t iChan = sampledPosNumChannels; iChan < sampledPosNumChannels4; ++iChan)
    {
      sampledPosQuantisationData[iChan].pack(qMean, qSet);
    }

    //-----------------------
    // Compile the quantised relative pos data
    const IntVector3Table* sampledPosRelQuantisedTable = subSectionIntermediate->getSampledPosRelQuantisedTable();
    NMP_VERIFY(sampledPosRelQuantisedTable);

    // Stride of single frame of sampled pos data
    NMP::Memory::Format memReqsPos;
    size_t keyFrameDataStride;
    MR::SectionDataNSA::getMemoryReqsSampledPosData(
      numSectionFrames,
      sampledPosNumChannels,
      memReqsPos.alignment,
      keyFrameDataStride,
      memReqsPos.size);
    NMP::Memory::Resource memResPos = NMPMemoryAllocateFromFormat(memReqsPos);
    ZeroMemory(memResPos.ptr, memReqsPos.size);
    sampledPosData = (MR::SampledPosKey*) memResPos.ptr;
    NMP_VERIFY(sampledPosData);

    // Pack the quantised sample data
    uint32_t qPosKey[3];
    for (uint32_t iFrame = 0; iFrame < numSectionFrames; ++iFrame)
    {
      uint8_t* data = (uint8_t*) sampledPosData;
      data += (iFrame * keyFrameDataStride);
      MR::SampledPosKey* posKeysAtFrame = (MR::SampledPosKey*)data;
    
      for (uint32_t iChan = 0; iChan < sampledPosNumChannels; ++iChan)
      {
        sampledPosRelQuantisedTable->getKey(iChan, iFrame, (int32_t *)qPosKey);
        posKeysAtFrame[iChan].pack(qPosKey);
      }
    }
  }

  // Compile the quat section data  
  MR::QuantisationMeanAndSetVec3* sampledQuatQuantisationData = NULL;
  MR::SampledQuatKeyTQA* sampledQuatData = NULL;
  if (sampledQuatNumChannels > 0)
  {
    //-----------------------
    // Compile the mean and qSet data
    const uint32_t* sampledQuatMeansX = subSectionIntermediate->getSampledQuatMeansX();
    NMP_VERIFY(sampledQuatMeansX);
    const uint32_t* sampledQuatMeansY = subSectionIntermediate->getSampledQuatMeansY();
    NMP_VERIFY(sampledQuatMeansY);
    const uint32_t* sampledQuatMeansZ = subSectionIntermediate->getSampledQuatMeansZ();
    NMP_VERIFY(sampledQuatMeansZ);
    const uint32_t* sampledQuatCompToQSetMapX = subSectionIntermediate->getSampledQuatCompToQSetMapX();
    NMP_VERIFY(sampledQuatCompToQSetMapX);
    const uint32_t* sampledQuatCompToQSetMapY = subSectionIntermediate->getSampledQuatCompToQSetMapY();
    NMP_VERIFY(sampledQuatCompToQSetMapY);
    const uint32_t* sampledQuatCompToQSetMapZ = subSectionIntermediate->getSampledQuatCompToQSetMapZ();
    NMP_VERIFY(sampledQuatCompToQSetMapZ);

    // Allocate the memory for the channel mean and qSet indices
    NMP::Memory::Format memReqsMeanAndSetVec3 = MR::SectionDataNSA::getMemoryReqsMeanAndSetVec3(sampledQuatNumChannels);
    NMP::Memory::Resource memResQuatMeanQSet = NMPMemoryAllocateFromFormat(memReqsMeanAndSetVec3);
    ZeroMemory(memResQuatMeanQSet.ptr, memReqsMeanAndSetVec3.size);
    sampledQuatQuantisationData = (MR::QuantisationMeanAndSetVec3*) memResQuatMeanQSet.ptr;
    NMP_VERIFY(sampledQuatQuantisationData);

    // Pack the mean and qSet data
    uint32_t qMean[3], qSet[3];
    for (uint32_t iChan = 0; iChan < sampledQuatNumChannels; ++iChan)
    {
      qMean[0] = sampledQuatMeansX[iChan];
      qMean[1] = sampledQuatMeansY[iChan];
      qMean[2] = sampledQuatMeansZ[iChan];
      qSet[0] = sampledQuatCompToQSetMapX[iChan];
      qSet[1] = sampledQuatCompToQSetMapY[iChan];
      qSet[2] = sampledQuatCompToQSetMapZ[iChan];
      sampledQuatQuantisationData[iChan].pack(qMean, qSet);
    }
    // The quantization data is padded to a multiple of 4 entries.
    // Check MR::SectionDataNSA::getMemoryReqsMeanAndSetVec3.
    // The last entry is duplicated to fill in the extra entries.
    uint32_t sampledQuatNumChannels4 = NMP::nmAlignedValue4(sampledQuatNumChannels);
    for (uint32_t iChan = sampledQuatNumChannels; iChan < sampledQuatNumChannels4; ++iChan)
    {
      sampledQuatQuantisationData[iChan].pack(qMean, qSet);
    }

    //-----------------------
    // Compile the quantised relative quat data
    const IntVector3Table* sampledQuatRelQuantisedTable = subSectionIntermediate->getSampledQuatRelQuantisedTable();
    NMP_VERIFY(sampledQuatRelQuantisedTable);
    
    // Stride of single frame of sampled quat data
    NMP::Memory::Format memReqsQuat;
    size_t keyFrameDataStride;
    MR::SectionDataNSA::getMemoryReqsSampledQuatData(
      numSectionFrames,
      sampledQuatNumChannels,
      memReqsQuat.alignment,
      keyFrameDataStride,
      memReqsQuat.size);
    NMP::Memory::Resource memResQuat = NMPMemoryAllocateFromFormat(memReqsQuat);
    ZeroMemory(memResQuat.ptr, memReqsQuat.size);
    sampledQuatData = (MR::SampledQuatKeyTQA*) memResQuat.ptr;
    NMP_VERIFY(sampledQuatData);

    // Pack the quantised sample data
    uint32_t qQuatKey[3];
    for (uint32_t iFrame = 0; iFrame < numSectionFrames; ++iFrame)
    {
      uint8_t* data = (uint8_t*) sampledQuatData;
      data += (iFrame * keyFrameDataStride);
      MR::SampledQuatKeyTQA* quatKeysAtFrame = (MR::SampledQuatKeyTQA*)data;

      for (uint32_t iChan = 0; iChan < sampledQuatNumChannels; ++iChan)
      {
        sampledQuatRelQuantisedTable->getKey(iChan, iFrame, (int32_t *)qQuatKey);
        quatKeysAtFrame[iChan].pack(qQuatKey);
      }
    }
  }

  //-----------------------
  // Allocate the memory for the section data  
  NMP::Memory::Format memReqsSectionData = SectionDataNSABuilder::getMemoryRequirements(
    numSectionFrames,
    sampledPosNumChannels,
    sampledQuatNumChannels);
    
  NMP::Memory::Resource memResSectionData = NMPMemoryAllocateFromFormat(memReqsSectionData);
  ZeroMemory(memResSectionData.ptr, memReqsSectionData.size);
  
  // Build the section data
  NMP_VERIFY(!m_sectionData);
  m_sectionData = SectionDataNSABuilder::init(
    memResSectionData,
    numSectionFrames,
    sampledPosNumChannels,
    sampledQuatNumChannels,
    sampledPosQuantisationData,
    sampledPosData,
    sampledQuatQuantisationData,
    sampledQuatData);
    
  // Check all the memory was used correctly
  NMP_VERIFY_MSG(
    memResSectionData.format.size == 0,
    "Not all the allocated memory was used when compiling section (%d, %d) : %d bytes remain.",
    iFrameSection,
    iChannelSection,
    memResSectionData.format.size);
    
  //-----------------------
  // Tidy up
  if (sampledPosNumChannels > 0)
  {
    NMP_VERIFY(sampledPosQuantisationData);
    NMP_VERIFY(sampledPosData);
    NMP::Memory::memFree(sampledPosQuantisationData);
    NMP::Memory::memFree(sampledPosData);
  }
  
  if (sampledQuatNumChannels > 0)
  {
    NMP_VERIFY(sampledQuatQuantisationData);
    NMP_VERIFY(sampledQuatData);
    NMP::Memory::memFree(sampledQuatQuantisationData);
    NMP::Memory::memFree(sampledQuatData);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// SampledDataNSACompiled
//----------------------------------------------------------------------------------------------------------------------
SampledDataCompiledNSA::SampledDataCompiledNSA() :
  m_sampledPosCompToAnimMaps(NULL),
  m_sampledQuatCompToAnimMaps(NULL),
  m_sampledPosNumQuantisationSets(0),
  m_sampledQuatNumQuantisationSets(0),
  m_sampledPosQuantisationInfo(NULL),
  m_sampledQuatQuantisationInfo(NULL),
  m_numFrameSections(0),
  m_numChannelSections(0),
  m_subSections(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
SampledDataCompiledNSA::~SampledDataCompiledNSA()
{
  tidy();
}

//----------------------------------------------------------------------------------------------------------------------
const SubSectionNSACompiled* SampledDataCompiledNSA::getSubSection(
  uint32_t iFrameSection,
  uint32_t iChannelSection) const
{
  NMP_VERIFY(iFrameSection < m_numFrameSections && iChannelSection < m_numChannelSections);
  return m_subSections[iFrameSection * m_numChannelSections + iChannelSection];
}

//----------------------------------------------------------------------------------------------------------------------
SubSectionNSACompiled* SampledDataCompiledNSA::getSubSection(
  uint32_t iFrameSection,
  uint32_t iChannelSection)
{
  NMP_VERIFY(iFrameSection < m_numFrameSections && iChannelSection < m_numChannelSections);
  return m_subSections[iFrameSection * m_numChannelSections + iChannelSection];
}

//----------------------------------------------------------------------------------------------------------------------
void SampledDataCompiledNSA::tidy()
{
  //-----------------------
  // Comp to anim maps
  if (m_sampledPosCompToAnimMaps)
  {
    for (uint32_t iChanSection = 0; iChanSection < m_numChannelSections; ++iChanSection)
    {
      MR::CompToAnimChannelMap* compToAnimMap = m_sampledPosCompToAnimMaps[iChanSection];
      if (compToAnimMap)
      {
        NMP::Memory::memFree(compToAnimMap);
      }
    }
    NMP::Memory::memFree(m_sampledPosCompToAnimMaps);
    m_sampledPosCompToAnimMaps = NULL;
  }
  
  if (m_sampledQuatCompToAnimMaps)
  {
    for (uint32_t iChanSection = 0; iChanSection < m_numChannelSections; ++iChanSection)
    {
      MR::CompToAnimChannelMap* compToAnimMap = m_sampledQuatCompToAnimMaps[iChanSection];
      if (compToAnimMap)
      {
        NMP::Memory::memFree(compToAnimMap);
      }
    }
    NMP::Memory::memFree(m_sampledQuatCompToAnimMaps);
    m_sampledQuatCompToAnimMaps = NULL;
  }

  //-----------------------
  // Global data
  if (m_sectionStartFrames)
  {
    NMP::Memory::memFree(m_sectionStartFrames);
    m_sectionStartFrames = NULL;
  }
  
  if (m_sampledPosQuantisationInfo)
  {
    NMP::Memory::memFree(m_sampledPosQuantisationInfo);
    m_sampledPosQuantisationInfo = NULL;
  }
  
  if (m_sampledQuatQuantisationInfo)
  {
    NMP::Memory::memFree(m_sampledQuatQuantisationInfo);
    m_sampledQuatQuantisationInfo = NULL;
  }

  m_sampledPosNumQuantisationSets = 0;
  m_sampledQuatNumQuantisationSets = 0;

  //-----------------------
  // Sections
  if (m_subSections)
  {
    uint32_t numEntries = m_numFrameSections * m_numChannelSections;
    for (uint32_t i = 0; i < numEntries; ++i)
    {
      SubSectionNSACompiled* subSection = m_subSections[i];
      if (subSection)
        delete subSection;
    }
    delete m_subSections;
  }

  m_subSections = NULL;
  m_numFrameSections = 0;
  m_numChannelSections = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SampledDataCompiledNSA::compileSampledData(
  const AnimSourceUncompressed* animSource,
  const SampledDataLayoutNSA* sampledDataLayout)
{
  NMP_VERIFY(animSource);
  NMP_VERIFY(sampledDataLayout);
  m_numFrameSections = sampledDataLayout->getNumFrameSections();
  NMP_VERIFY(m_numFrameSections > 0);
  m_numChannelSections = sampledDataLayout->getNumChannelSections();
  NMP_VERIFY(m_numChannelSections > 0);

  //-----------------------
  // Compute the intermediate data used to compile the binary components of the sampled data
  SampledDataIntermediateNSA* sampledDataIntermediate = new SampledDataIntermediateNSA;
  sampledDataIntermediate->computeSampledData(
    animSource,
    sampledDataLayout);
  
  //-----------------------
  // Compile the intermediate data into the final binary components
  uint32_t numEntries = m_numFrameSections * m_numChannelSections;
  m_subSections = new SubSectionNSACompiled* [numEntries];
  for (uint32_t i = 0; i < numEntries; ++i)
    m_subSections[i] = new SubSectionNSACompiled;
    
  //-----------------------
  // Compile the global data
  compileGlobalData(sampledDataIntermediate, sampledDataLayout);
  
  //-----------------------
  // Compile the section data
  compileSectionData(sampledDataIntermediate, sampledDataLayout);

  //-----------------------
  // Tidy up
  delete sampledDataIntermediate;
}

//----------------------------------------------------------------------------------------------------------------------
void SampledDataCompiledNSA::compileGlobalData(
  const SampledDataIntermediateNSA* sampledDataIntermediate,
  const SampledDataLayoutNSA* sampledDataLayout)
{
  NMP_VERIFY(sampledDataIntermediate);
  NMP_VERIFY(sampledDataLayout);
  
  //-----------------------
  // Section start frames
  uint32_t numFrameSections = sampledDataLayout->getNumFrameSections();
  NMP::Memory::Format memReqsStartFrames(sizeof(uint32_t) * (numFrameSections + 1));
  NMP::Memory::Resource memResStartFrames = NMPMemoryAllocateFromFormat(memReqsStartFrames);
  ZeroMemory(memResStartFrames.ptr, memReqsStartFrames.size);

  m_sectionStartFrames = (uint32_t*) memResStartFrames.ptr;
  NMP_VERIFY(m_sectionStartFrames);
  for (uint32_t i = 0; i < numFrameSections; ++i)
  {
    m_sectionStartFrames[i] = sampledDataLayout->getSectionStartFrame(i);
  }
  m_sectionStartFrames[numFrameSections] = sampledDataLayout->getSectionEndFrame(numFrameSections-1);

  //-----------------------
  // Sampled pos means quantisation information
  const Vector3QuantisationTable* quantPosMeans = sampledDataIntermediate->getQuantisationPosMeans();
  if (quantPosMeans)
  {
    // Get global scale and offset
    NMP::Vector3 qScale = quantPosMeans->getQuantisationScales(0);
    NMP::Vector3 qOffset = quantPosMeans->getQuantisationOffsets(0);
    m_posMeansQuantisationInfo.pack(qScale, qOffset);
  }
  else
  {
    m_posMeansQuantisationInfo.zero();
  }
    
  //-----------------------
  // Sampled pos quantisation basis information
  m_sampledPosNumQuantisationSets = sampledDataLayout->getSampledPosNumQuantisationSets();
  if (m_sampledPosNumQuantisationSets > 0)
  {
    NMP_VERIFY(sampledDataLayout->getNumSampledPosChannels() > 0);
  
    // Allocate the memory for the sampled pos quantisation set basis
    NMP::Memory::Format memReqsPosQBasis(
      sizeof(MR::QuantisationScaleAndOffsetVec3) * m_sampledPosNumQuantisationSets,
      NMP_NATURAL_TYPE_ALIGNMENT);
    NMP::Memory::Resource memResPosQBasis = NMPMemoryAllocateFromFormat(memReqsPosQBasis);
    ZeroMemory(memResPosQBasis.ptr, memReqsPosQBasis.size);
    m_sampledPosQuantisationInfo = (MR::QuantisationScaleAndOffsetVec3*) memResPosQBasis.ptr;
    NMP_VERIFY(m_sampledPosQuantisationInfo);
    
    // Compile the sampled pos quantisation set basis
    const NMP::Vector3* qSetPosMin = sampledDataIntermediate->getQuantisationSetPosMin();
    NMP_VERIFY(qSetPosMin);
    const NMP::Vector3* qSetPosMax = sampledDataIntermediate->getQuantisationSetPosMax();
    NMP_VERIFY(qSetPosMax);
    const uint32_t prec[3] = {11, 11, 10}; // X, Y, Z
    
    NMP::Vector3 qScale, qOffset;
    for (uint32_t iSet = 0; iSet < m_sampledPosNumQuantisationSets; ++iSet)
    {
      // Compute the quantisation scale and offset
      qScale.x = NMP::UniformQuantisation::stepSize(qSetPosMin[iSet].x, qSetPosMax[iSet].x, prec[0]);
      qScale.y = NMP::UniformQuantisation::stepSize(qSetPosMin[iSet].y, qSetPosMax[iSet].y, prec[1]);
      qScale.z = NMP::UniformQuantisation::stepSize(qSetPosMin[iSet].z, qSetPosMax[iSet].z, prec[2]);
      qOffset.x = qSetPosMin[iSet].x;
      qOffset.y = qSetPosMin[iSet].y;
      qOffset.z = qSetPosMin[iSet].z;
      
      // Pack the data
      m_sampledPosQuantisationInfo[iSet].pack(qScale, qOffset);
    }
  }

  //-----------------------
  // Sampled quat quantisation basis information
  m_sampledQuatNumQuantisationSets = sampledDataLayout->getSampledQuatNumQuantisationSets();
  if (m_sampledQuatNumQuantisationSets > 0)
  {
    NMP_VERIFY(sampledDataLayout->getNumSampledQuatChannels() > 0);

    // Allocate the memory for the sampled pos quantisation set basis
    NMP::Memory::Format memReqsQuatQBasis(
      sizeof(MR::QuantisationScaleAndOffsetVec3) * m_sampledQuatNumQuantisationSets,
      NMP_NATURAL_TYPE_ALIGNMENT);
    NMP::Memory::Resource memResQuatQBasis = NMPMemoryAllocateFromFormat(memReqsQuatQBasis);
    ZeroMemory(memResQuatQBasis.ptr, memReqsQuatQBasis.size);
    m_sampledQuatQuantisationInfo = (MR::QuantisationScaleAndOffsetVec3*) memResQuatQBasis.ptr;
    NMP_VERIFY(m_sampledQuatQuantisationInfo);

    // Compile the sampled quat quantisation set basis
    const NMP::Vector3* qSetQuatMin = sampledDataIntermediate->getQuantisationSetQuatMin();
    NMP_VERIFY(qSetQuatMin);
    const NMP::Vector3* qSetQuatMax = sampledDataIntermediate->getQuantisationSetQuatMax();
    NMP_VERIFY(qSetQuatMax);
    const uint32_t prec[3] = {16, 16, 16}; // X, Y, Z

    NMP::Vector3 qScale, qOffset;
    for (uint32_t iSet = 0; iSet < m_sampledQuatNumQuantisationSets; ++iSet)
    {
      // Compute the quantisation scale and offset
      qScale.x = NMP::UniformQuantisation::stepSize(qSetQuatMin[iSet].x, qSetQuatMax[iSet].x, prec[0]);
      qScale.y = NMP::UniformQuantisation::stepSize(qSetQuatMin[iSet].y, qSetQuatMax[iSet].y, prec[1]);
      qScale.z = NMP::UniformQuantisation::stepSize(qSetQuatMin[iSet].z, qSetQuatMax[iSet].z, prec[2]);
      qOffset.x = qSetQuatMin[iSet].x;
      qOffset.y = qSetQuatMin[iSet].y;
      qOffset.z = qSetQuatMin[iSet].z;

      // Pack the data
      m_sampledQuatQuantisationInfo[iSet].pack(qScale, qOffset);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SampledDataCompiledNSA::compileSectionData(
  const SampledDataIntermediateNSA* sampledDataIntermediate,
  const SampledDataLayoutNSA* sampledDataLayout)
{
  NMP_VERIFY(sampledDataIntermediate);
  NMP_VERIFY(sampledDataLayout);

  for (uint32_t iFrameSection = 0; iFrameSection < m_numFrameSections; ++iFrameSection)
  {
    for (uint32_t iChannelSection = 0; iChannelSection < m_numChannelSections; ++iChannelSection)
    {
      SubSectionNSACompiled* subSection = getSubSection(iFrameSection, iChannelSection);
      NMP_VERIFY(subSection);
      subSection->compileSectionData(
        sampledDataIntermediate,
        sampledDataLayout,
        iFrameSection,
        iChannelSection);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SampledDataCompiledNSA::getTotalNumAnimSamples() const
{
  uint32_t numAnimSamples = 0;
  for (uint32_t iFrameSection = 0; iFrameSection < m_numFrameSections; ++iFrameSection)
  {
    uint32_t numSamples = m_sectionStartFrames[iFrameSection + 1] - m_sectionStartFrames[iFrameSection] + 1;
    numAnimSamples += numSamples;
  }
  return numAnimSamples;
}

//----------------------------------------------------------------------------------------------------------------------
void SampledDataCompiledNSA::getUnpackedSampledPosQuantisationData(
  uint32_t iChannelSection,
  uint32_t iChan,
  IntVector3Table* qMeanTable,
  IntVector3Table* qSetTable) const
{
  NMP_VERIFY(qMeanTable);
  NMP_VERIFY(qSetTable);
  NMP_VERIFY(qMeanTable->getNumKeyFrames() == m_numFrameSections);
  NMP_VERIFY(qSetTable->getNumKeyFrames() == m_numFrameSections);

  for (uint32_t iFrameSection = 0; iFrameSection < m_numFrameSections; ++iFrameSection)
  {
    const SubSectionNSACompiled* subSection = getSubSection(iFrameSection, iChannelSection);
    NMP_VERIFY(subSection);
    const SectionDataNSABuilder* sectionDataBuilder = (const SectionDataNSABuilder*) subSection->getSectionData();
    NMP_VERIFY(sectionDataBuilder);
    const MR::QuantisationMeanAndSetVec3* sampledPosQuantisationData = sectionDataBuilder->getSampledPosQuantisationData();
    NMP_VERIFY(sampledPosQuantisationData);
    uint32_t qMean[3], qSet[3];
    NMP_VERIFY(iChan < sectionDataBuilder->getSampledPosNumChannels());
    sampledPosQuantisationData[iChan].unpack(qMean, qSet);
    qMeanTable->setKey(0, iFrameSection, (const int32_t*)qMean);
    qSetTable->setKey(0, iFrameSection, (const int32_t*)qSet);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SampledDataCompiledNSA::getUnpackedSampledPosData(
  uint32_t iChannelSection,
  uint32_t iChan,
  IntVector3Table* qDataTable) const
{
  NMP_VERIFY(qDataTable);
  NMP_VERIFY(qDataTable->getNumKeyFrames() == getTotalNumAnimSamples());

  uint32_t indx = 0;
  for (uint32_t iFrameSection = 0; iFrameSection < m_numFrameSections; ++iFrameSection)
  {
    const SubSectionNSACompiled* subSection = getSubSection(iFrameSection, iChannelSection);
    NMP_VERIFY(subSection);
    const SectionDataNSABuilder* sectionDataBuilder = (const SectionDataNSABuilder*) subSection->getSectionData();
    NMP_VERIFY(sectionDataBuilder);
    const MR::SampledPosKey* sampledPosData = sectionDataBuilder->getSampledPosData();
    NMP_VERIFY(sampledPosData);

    // Get the memory stride for a frame of animation data
    uint32_t numSectionAnimFrames = sectionDataBuilder->getNumSectionAnimFrames();
    uint32_t sampledPosNumChannels = sectionDataBuilder->getSampledPosNumChannels();
    size_t alignment;
    size_t keyFrameDataStride;
    size_t keyFrameDataMemReqs;
    MR::SectionDataNSA::getMemoryReqsSampledPosData(
      numSectionAnimFrames,
      sampledPosNumChannels,
      alignment,
      keyFrameDataStride,
      keyFrameDataMemReqs);

    // Recover the unpacked animation frames
    uint32_t qData[3];
    for (uint32_t iFrame = 0; iFrame < numSectionAnimFrames; ++iFrame, ++indx)
    {
      const uint8_t* data = (const uint8_t*) sampledPosData;
      data += (iFrame * keyFrameDataStride);
      const MR::SampledPosKey* sampledPosKeys = (const MR::SampledPosKey*)data;
      sampledPosKeys[iChan].unpack(qData);
      qDataTable->setKey(0, indx, (const int32_t*)qData);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SampledDataCompiledNSA::getUnpackedSampledQuatQuantisationData(
  uint32_t iChannelSection,
  uint32_t iChan,
  IntVector3Table* qMeanTable,
  IntVector3Table* qSetTable) const
{
  NMP_VERIFY(qMeanTable);
  NMP_VERIFY(qSetTable);
  NMP_VERIFY(qMeanTable->getNumKeyFrames() == m_numFrameSections);
  NMP_VERIFY(qSetTable->getNumKeyFrames() == m_numFrameSections);

  for (uint32_t iFrameSection = 0; iFrameSection < m_numFrameSections; ++iFrameSection)
  {
    const SubSectionNSACompiled* subSection = getSubSection(iFrameSection, iChannelSection);
    NMP_VERIFY(subSection);
    const SectionDataNSABuilder* sectionDataBuilder = (const SectionDataNSABuilder*) subSection->getSectionData();
    NMP_VERIFY(sectionDataBuilder);
    const MR::QuantisationMeanAndSetVec3* sampledQuatQuantisationData = sectionDataBuilder->getSampledQuatQuantisationData();
    NMP_VERIFY(sampledQuatQuantisationData);
    uint32_t qMean[3], qSet[3];
    NMP_VERIFY(iChan < sectionDataBuilder->getSampledQuatNumChannels());
    sampledQuatQuantisationData[iChan].unpack(qMean, qSet);
    qMeanTable->setKey(0, iFrameSection, (const int32_t*)qMean);
    qSetTable->setKey(0, iFrameSection, (const int32_t*)qSet);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SampledDataCompiledNSA::getUnpackedSampledQuatData(
  uint32_t iChannelSection,
  uint32_t iChan,
  IntVector3Table* qDataTable) const
{
  NMP_VERIFY(qDataTable);
  NMP_VERIFY(qDataTable->getNumKeyFrames() == getTotalNumAnimSamples());

  uint32_t indx = 0;
  for (uint32_t iFrameSection = 0; iFrameSection < m_numFrameSections; ++iFrameSection)
  {
    const SubSectionNSACompiled* subSection = getSubSection(iFrameSection, iChannelSection);
    NMP_VERIFY(subSection);
    const SectionDataNSABuilder* sectionDataBuilder = (const SectionDataNSABuilder*) subSection->getSectionData();
    NMP_VERIFY(sectionDataBuilder);
    const MR::SampledQuatKeyTQA* sampledQuatData = sectionDataBuilder->getSampledQuatData();
    NMP_VERIFY(sampledQuatData);

    // Get the memory stride for a frame of animation data
    uint32_t numSectionAnimFrames = sectionDataBuilder->getNumSectionAnimFrames();
    uint32_t sampledQuatNumChannels = sectionDataBuilder->getSampledQuatNumChannels();
    size_t alignment;
    size_t keyFrameDataStride;
    size_t keyFrameDataMemReqs;
    MR::SectionDataNSA::getMemoryReqsSampledQuatData(
      numSectionAnimFrames,
      sampledQuatNumChannels,
      alignment,
      keyFrameDataStride,
      keyFrameDataMemReqs);

    // Recover the unpacked animation frames
    uint32_t qData[3];
    for (uint32_t iFrame = 0; iFrame < numSectionAnimFrames; ++iFrame, ++indx)
    {
      const uint8_t* data = (const uint8_t*) sampledQuatData;
      data += (iFrame * keyFrameDataStride);
      const MR::SampledQuatKeyTQA* sampledQuatKeys = (const MR::SampledQuatKeyTQA*)data;
      sampledQuatKeys[iChan].unpack(qData);
      qDataTable->setKey(0, indx, (const int32_t*)qData);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool SampledDataCompiledNSA::checkForUnchangingChannels(
  const SampledDataLayoutNSA* sampledDataLayout,
  ChannelCompressionOptionsNSA* channelCompressionOptions) const
{
  NMP_VERIFY(sampledDataLayout);
  NMP_VERIFY(channelCompressionOptions);
  NMP_VERIFY(m_numFrameSections > 0);
  NMP_VERIFY(m_numChannelSections > 0);
  NMP_VERIFY(m_sectionStartFrames);

  bool result = false;
  const uint32_t qMeanEps = 0;
  const uint32_t qSetEps = 0;
  const uint32_t qDataEps = 1;  

  //-----------------------
  // Allocate the memory for the unpacked quantisation data
  uint32_t numAnimSamples = getTotalNumAnimSamples();
  NMP::Memory::Format memReqsQMean = IntVector3Table::getMemoryRequirements(1, m_numFrameSections);
  NMP::Memory::Resource memResQMean = NMPMemoryAllocateFromFormat(memReqsQMean);
  ZeroMemory(memResQMean.ptr, memReqsQMean.size);
  IntVector3Table* qMeanTable = IntVector3Table::init(memResQMean, 1, m_numFrameSections);

  NMP::Memory::Format memReqsQSet = IntVector3Table::getMemoryRequirements(1, m_numFrameSections);
  NMP::Memory::Resource memResQSet = NMPMemoryAllocateFromFormat(memReqsQSet);
  ZeroMemory(memResQSet.ptr, memReqsQSet.size);
  IntVector3Table* qSetTable = IntVector3Table::init(memResQSet, 1, m_numFrameSections);

  NMP::Memory::Format memReqsQData = IntVector3Table::getMemoryRequirements(1, numAnimSamples);
  NMP::Memory::Resource memResQData = NMPMemoryAllocateFromFormat(memReqsQData);
  ZeroMemory(memResQData.ptr, memReqsQData.size);
  IntVector3Table* qDataTable = IntVector3Table::init(memResQData, 1, numAnimSamples);

  //-----------------------
  // Pos
  for (uint32_t iChannelSection = 0; iChannelSection < m_numChannelSections; ++iChannelSection)
  {
    const CompToAnimChannelMapLayout* compToAnimMap = sampledDataLayout->getSampledPosCompToAnimMap(iChannelSection);
    NMP_VERIFY(compToAnimMap);
    uint32_t sampledPosNumChannels = compToAnimMap->getNumEntries();

    for (uint32_t iChan = 0; iChan < sampledPosNumChannels; ++iChan)
    {
      getUnpackedSampledPosQuantisationData(
        iChannelSection,
        iChan,
        qMeanTable,
        qSetTable);

      // Check that the unpacked mean and quantisation sets are exactly the same across
      // the frame sections  
      if (qMeanTable->isChannelUnchanging(0, qMeanEps) && qSetTable->isChannelUnchanging(0, qSetEps))
      {
        getUnpackedSampledPosData(
          iChannelSection,
          iChan,
          qDataTable);

        // Check if the quantised channel data are unvarying across the frame sections. Due
        // to numerical rounding error use a tolerance of a single quantisation interval to
        // check for unchanging channel data.
        if (qDataTable->isChannelUnchanging(0, qDataEps))
        {
          channelCompressionOptions->setPosChannelMethod(
            compToAnimMap->getAnimChannel(iChan),
            ChannelCompressionOptionsNSA::Unchanging);

          result = true;
        }
      }
    }
  }

  //-----------------------
  // Quat
  for (uint32_t iChannelSection = 0; iChannelSection < m_numChannelSections; ++iChannelSection)
  {
    const CompToAnimChannelMapLayout* compToAnimMap = sampledDataLayout->getSampledQuatCompToAnimMap(iChannelSection);
    NMP_VERIFY(compToAnimMap);
    uint32_t sampledQuatNumChannels = compToAnimMap->getNumEntries();

    for (uint32_t iChan = 0; iChan < sampledQuatNumChannels; ++iChan)
    {
      getUnpackedSampledQuatQuantisationData(
        iChannelSection,
        iChan,
        qMeanTable,
        qSetTable);

      // Check that the unpacked mean and quantisation sets are exactly the same across
      // the frame sections  
      if (qMeanTable->isChannelUnchanging(0, qMeanEps) && qSetTable->isChannelUnchanging(0, qSetEps))
      {
        getUnpackedSampledQuatData(
          iChannelSection,
          iChan,
          qDataTable);

        // Check if the quantised channel data are unvarying across the frame sections. Due
        // to numerical rounding error use a tolerance of a single quantisation interval to
        // check for unchanging channel data.
        if (qDataTable->isChannelUnchanging(0, qDataEps))
        {
          channelCompressionOptions->setQuatChannelMethod(
            compToAnimMap->getAnimChannel(iChan),
            ChannelCompressionOptionsNSA::Unchanging);

          result = true;
        }
      }
    }
  }

  //-----------------------
  // Tidy up
  NMP::Memory::memFree(qMeanTable);
  NMP::Memory::memFree(qSetTable);
  NMP::Memory::memFree(qDataTable);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void SampledDataCompiledNSA::compileCompToAnimMaps(const SampledDataLayoutNSA* sampledDataLayout)
{
  NMP_VERIFY(sampledDataLayout);
  uint32_t numChannelSections = sampledDataLayout->getNumChannelSections();
  NMP_VERIFY(m_numChannelSections == numChannelSections);
  
  //-----------------------
  // Pos
  NMP_VERIFY(!m_sampledPosCompToAnimMaps);
  NMP::Memory::Format memReqsPosPtrTable(sizeof(MR::CompToAnimChannelMap*) * numChannelSections, NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::Resource memResPosPtrTable = NMPMemoryAllocateFromFormat(memReqsPosPtrTable);
  ZeroMemory(memResPosPtrTable.ptr, memReqsPosPtrTable.size);
  m_sampledPosCompToAnimMaps = (MR::CompToAnimChannelMap**) memResPosPtrTable.ptr;
  NMP_VERIFY(m_sampledPosCompToAnimMaps);
  
  for (uint32_t iChanSection = 0; iChanSection < numChannelSections; ++iChanSection)
  {
    // Information
    const CompToAnimChannelMapLayout* compToAnimMapLayout = sampledDataLayout->getSampledPosCompToAnimMap(iChanSection);
    NMP_VERIFY(compToAnimMapLayout);
    const uint32_t* animChannelsPosData = NULL;
    uint32_t sampledPosNumChannels = compToAnimMapLayout->getNumEntries();
    if (sampledPosNumChannels > 0)
    {
      animChannelsPosData = &(compToAnimMapLayout->getAnimChannels()[0]);
    }
    
    // Allocate the memory for the comp to anim map
    NMP::Memory::Format memReqsPosCompToAnimMap = MR::CompToAnimChannelMap::getMemoryRequirements(sampledPosNumChannels);
    NMP::Memory::Resource memResPosCompToAnimMap = NMPMemoryAllocateFromFormat(memReqsPosCompToAnimMap);
    ZeroMemory(memResPosCompToAnimMap.ptr, memReqsPosCompToAnimMap.size);
    MR::CompToAnimChannelMap* compToAnimMap = MR::CompToAnimChannelMap::init(
      memResPosCompToAnimMap,
      sampledPosNumChannels,
      animChannelsPosData);
    NMP_VERIFY(compToAnimMap);
    m_sampledPosCompToAnimMaps[iChanSection] = compToAnimMap;
  }
  
  //-----------------------
  // Quat
  NMP_VERIFY(!m_sampledQuatCompToAnimMaps);
  NMP::Memory::Format memReqsQuatPtrTable(sizeof(MR::CompToAnimChannelMap*) * numChannelSections, NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::Resource memResQuatPtrTable = NMPMemoryAllocateFromFormat(memReqsQuatPtrTable);
  ZeroMemory(memResQuatPtrTable.ptr, memReqsQuatPtrTable.size);
  m_sampledQuatCompToAnimMaps = (MR::CompToAnimChannelMap**) memResQuatPtrTable.ptr;
  NMP_VERIFY(m_sampledQuatCompToAnimMaps);

  for (uint32_t iChanSection = 0; iChanSection < numChannelSections; ++iChanSection)
  {
    // Information
    const CompToAnimChannelMapLayout* compToAnimMapLayout = sampledDataLayout->getSampledQuatCompToAnimMap(iChanSection);
    NMP_VERIFY(compToAnimMapLayout);
    const uint32_t* animChannelsQuatData = NULL;
    uint32_t sampledQuatNumChannels = compToAnimMapLayout->getNumEntries();
    if (sampledQuatNumChannels > 0)
    {
      animChannelsQuatData = &(compToAnimMapLayout->getAnimChannels()[0]);
    }

    // Allocate the memory for the comp to anim map
    NMP::Memory::Format memReqsQuatCompToAnimMap = MR::CompToAnimChannelMap::getMemoryRequirements(sampledQuatNumChannels);
    NMP::Memory::Resource memResQuatCompToAnimMap = NMPMemoryAllocateFromFormat(memReqsQuatCompToAnimMap);
    ZeroMemory(memResQuatCompToAnimMap.ptr, memReqsQuatCompToAnimMap.size);
    MR::CompToAnimChannelMap* compToAnimMap = MR::CompToAnimChannelMap::init(
      memResQuatCompToAnimMap,
      sampledQuatNumChannels,
      animChannelsQuatData);
    NMP_VERIFY(compToAnimMap);
    m_sampledQuatCompToAnimMaps[iChanSection] = compToAnimMap;
  }
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
