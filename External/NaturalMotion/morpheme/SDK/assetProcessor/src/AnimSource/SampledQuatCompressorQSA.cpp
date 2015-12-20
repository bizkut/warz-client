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
#include "NMNumerics/NMQuatUtils.h"
#include "NMNumerics/NMRigidMotionTMJacobian.h"
#include "assetProcessor/AnimSource/AnimSourceCompressorQSA.h"
#include "assetProcessor/AnimSource/ChannelSetInfoCompressorQSA.h"
#include "assetProcessor/AnimSource/AnimSectionCompressorQSA.h"
#include "assetProcessor/AnimSource/SampledQuatCompressorQSA.h"

//----------------------------------------------------------------------------------------------------------------------

// Disable warnings about deprecated functions (sprintf, fopen)
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning(disable : 4996)
#endif

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// SampledQuatCompressorQSA Functions
//----------------------------------------------------------------------------------------------------------------------
SampledQuatCompressorQSA::SampledQuatCompressorQSA(AnimSectionCompressorQSA* animSectionCompressor)
{
  NMP_VERIFY(animSectionCompressor);
  m_animSectionCompressor = animSectionCompressor;

  //-----------------------
  // Init the data pointers
  m_sampledQuatTable = NULL;
  m_sampledQuatMeans = NULL;
  m_sampledQuatQuantisationInfo = NULL;
  m_sampledQuatQuantisationData = NULL;
  m_sampledQuatQuantTable = NULL;
  m_sampledQuatData = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
SampledQuatCompressorQSA::~SampledQuatCompressorQSA()
{
  if (m_sampledQuatTable)
  {
    NMP::Memory::memFree(m_sampledQuatTable);
    m_sampledQuatTable = NULL;
  }

  if (m_sampledQuatMeans)
  {
    NMP::Memory::memFree(m_sampledQuatMeans);
    m_sampledQuatMeans = NULL;
  }

  if (m_sampledQuatQuantisationInfo)
  {
    NMP::Memory::memFree(m_sampledQuatQuantisationInfo);
    m_sampledQuatQuantisationInfo = NULL;
  }

  if (m_sampledQuatQuantisationData)
  {
    NMP::Memory::memFree(m_sampledQuatQuantisationData);
    m_sampledQuatQuantisationData = NULL;
  }

  if (m_sampledQuatQuantTable)
  {
    NMP::Memory::memFree(m_sampledQuatQuantTable);
    m_sampledQuatQuantTable = NULL;
  }

  //-----------------------
  // Quantisation sets
  m_sampledQuatCompToQSetMapX.clear();
  m_sampledQuatCompToQSetMapY.clear();
  m_sampledQuatCompToQSetMapZ.clear();

  //-----------------------
  // Bitstream data
  if (m_sampledQuatData)
  {
    NMP::Memory::memFree(m_sampledQuatData);
    m_sampledQuatData = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SampledQuatCompressorQSA::computeDataStrides()
{
  // Info
  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t sampledQuatNumChannels = channelSetInfoCompressor->getSampledQuatNumChans();
  NMP_VERIFY(sampledQuatNumChannels > 0);
  m_sampledQuatNumQuantisationSets = m_animSectionCompressor->getNumQuantisationSets(CoefSetQSA::CoefSetSampledQuat);

  QuantisationSetQSA* qSet;
  uint32_t strideBits;

  strideBits = 0;
  for (uint32_t iChan = 0; iChan < sampledQuatNumChannels; ++iChan)
  {
    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledQuat, m_sampledQuatCompToQSetMapX[iChan]);
    const NMP::UniformQuantisationInfo& qInfoX = qSet->getCoefSetX()->getQuantisationInfo();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledQuat, m_sampledQuatCompToQSetMapY[iChan]);
    const NMP::UniformQuantisationInfo& qInfoY = qSet->getCoefSetY()->getQuantisationInfo();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledQuat, m_sampledQuatCompToQSetMapZ[iChan]);
    const NMP::UniformQuantisationInfo& qInfoZ = qSet->getCoefSetZ()->getQuantisationInfo();

    strideBits += qInfoX.m_precision;
    strideBits += qInfoY.m_precision;
    strideBits += qInfoZ.m_precision;
  }
  m_sampledQuatByteStride = (strideBits + 0x07) >> 3; // Align stride to a byte boundary
}

//----------------------------------------------------------------------------------------------------------------------
void SampledQuatCompressorQSA::writeDebug(FILE* filePointer)
{
  NMP_VERIFY(filePointer);
  fprintf(filePointer, "\n-----------------------------------------------------------------------------------------------\n");
  fprintf(filePointer, "Sampled Quat Compressor Dump\n");
  fprintf(filePointer, "-----------------------------------------------------------------------------------------------\n");

  writeQuatKeysMeans(filePointer);
  writeQuatTableKeys(filePointer);
  writeQuantisationInfo(filePointer);
}

//----------------------------------------------------------------------------------------------------------------------
void SampledQuatCompressorQSA::writeQuatKeysMeans(FILE* filePointer) const
{
  NMP_VERIFY(filePointer);
  fprintf(filePointer, "\n_________________ sampledQuatKeysMeans _________________\n");

  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t sampledQuatNumChans = channelSetInfoCompressor->getSampledQuatNumChans();
  NMP_VERIFY(sampledQuatNumChans > 0);

  for (uint32_t iChan = 0; iChan < sampledQuatNumChans; ++iChan)
  {
    fprintf(
      filePointer,
      "comp chan = %4d, qVec = [%3.5f, %3.5f, %3.5f, %3.5f]\n",
      iChan,
      m_sampledQuatMeans[iChan].x,
      m_sampledQuatMeans[iChan].y,
      m_sampledQuatMeans[iChan].z,
      m_sampledQuatMeans[iChan].w);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SampledQuatCompressorQSA::writeQuatTableKeys(FILE* filePointer) const
{
  NMP_VERIFY(filePointer);
  fprintf(filePointer, "\n_________________ sampledQuatTable Keys _________________\n");
  m_sampledQuatTable->writeTableByFrames(filePointer);
}

//----------------------------------------------------------------------------------------------------------------------
void SampledQuatCompressorQSA::writeQuantisationInfo(FILE* filePointer) const
{
  NMP_VERIFY(filePointer);

  uint32_t sampledQuatNumQuantisationSets = getSampledQuatNumQuantisationSets();
  const MR::QuantisationInfoQSA* qSetInfo = getSampledQuatQuantisationInfo();
  NMP_VERIFY(qSetInfo);

  for (uint32_t i = 0; i < sampledQuatNumQuantisationSets; ++i)
  {
    fprintf(filePointer, "\n_________________ Quantisation Info: qSet = %d _________________\n", i);

    fprintf(filePointer, "qSetInfo: qMin = [%3.5f, %3.5f, %3.5f], qMax = [%3.5f, %3.5f, %3.5f]\n",
            qSetInfo[i].m_qMin[0],
            qSetInfo[i].m_qMin[1],
            qSetInfo[i].m_qMin[2],
            qSetInfo[i].m_qMax[0],
            qSetInfo[i].m_qMax[1],
            qSetInfo[i].m_qMax[2]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SampledQuatCompressorQSA::computeRateAndDistortion()
{
  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const AnimSourceMotionAnalyser* motionAnalyser = animSourceCompressor->getMotionAnalyser();
  NMP_VERIFY(motionAnalyser);
  float averageBoneLength = motionAnalyser->getAverageBoneLength();

  computeRelativeTransforms();
  assignQuantisationSets();

  // Check if we are trying to compile a set of delta transforms for additive animation
  if (animSourceCompressor->getUseWorldSpace() && averageBoneLength > MR::ERROR_LIMIT)
  {
    analyseRateAndDistortion();
  }
  else
  {
    simpleRateAndDistortion();
  }

  computeRateAndDistortionMapped();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Quat SampledQuatCompressorQSA::evaluateQuantised(uint32_t iChan, uint32_t iFrame) const
{
  NMP::Quat quatKey;
  NMP::Vector3 quatRel, quatRel_;
  NMP::Vector3 qMin, qMax;
  uint32_t qx, qy, qz;
  QuantisationSetQSA* qSet;
  CoefSetQSA* coefSetX;
  CoefSetQSA* coefSetY;
  CoefSetQSA* coefSetZ;

  // Get the quantisation set corresponding to this compression channel
  qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledQuat, m_sampledQuatCompToQSetMapX[iChan]);
  coefSetX = qSet->getCoefSetX();
  const NMP::UniformQuantisationInfo& qInfoX = coefSetX->getQuantisationInfo();

  qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledQuat, m_sampledQuatCompToQSetMapY[iChan]);
  coefSetY = qSet->getCoefSetY();
  const NMP::UniformQuantisationInfo& qInfoY = coefSetY->getQuantisationInfo();

  qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledQuat, m_sampledQuatCompToQSetMapZ[iChan]);
  coefSetZ = qSet->getCoefSetZ();
  const NMP::UniformQuantisationInfo& qInfoZ = coefSetZ->getQuantisationInfo();

  qMin.set(qInfoX.m_qMin, qInfoY.m_qMin, qInfoZ.m_qMin);
  qMax.set(qInfoX.m_qMax, qInfoY.m_qMax, qInfoZ.m_qMax);

  // Set the quantiser precision and [qMin, qMax] range
  m_sampledQuatQuantTable->setPrecisionX(iChan, qInfoX.m_precision);
  m_sampledQuatQuantTable->setPrecisionY(iChan, qInfoY.m_precision);
  m_sampledQuatQuantTable->setPrecisionZ(iChan, qInfoZ.m_precision);
  m_sampledQuatQuantTable->setQuantisationBounds(iChan, qMin, qMax);

  // Quantise the relative quat sample (for the specified quantisation set)
  m_sampledQuatTable->getKey(iChan, iFrame, quatRel);
  m_sampledQuatQuantTable->quantise(iChan, quatRel, qx, qy, qz);

  // Recover the corresponding quantised relative quat sample
  m_sampledQuatQuantTable->dequantise(iChan, quatRel_, qx, qy, qz);

  // Recover the channel quat sample
  quatKey.fromRotationVector(quatRel_, true);
  quatKey = m_sampledQuatMeans[iChan] * quatKey;

  return quatKey;
}

//----------------------------------------------------------------------------------------------------------------------
void SampledQuatCompressorQSA::computeRelativeTransforms()
{
  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const MR::RigToAnimEntryMap* rigToAnimEntryMap = animSourceCompressor->getConglomerateRigToAnimEntryMap();
  NMP_VERIFY(rigToAnimEntryMap);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t sampledQuatNumChans = channelSetInfoCompressor->getSampledQuatNumChans();
  NMP_VERIFY(sampledQuatNumChans > 0);
  const std::vector<uint32_t>& sampledQuatCompToAnimMap = channelSetInfoCompressor->getSampledQuatCompToAnimMap();
  uint32_t numSectionFrames = m_animSectionCompressor->getNumSectionFrames();
  uint32_t sectionStartFrame = m_animSectionCompressor->getSectionStartFrame();

  const ChannelSetTable* channelSets = NULL;
  if (animSourceCompressor->getUseWorldSpace())
  {
    channelSets = animSourceCompressor->getChannelSetsWS();
  }
  else
  {
    channelSets = animSourceCompressor->getChannelSetsLS();
  }
  NMP_VERIFY(channelSets);

  const float qMinTQA[3] = { -1.0f, -1.0f, -1.0f };
  const float qMaxTQA[3] = { 1.0f, 1.0f, 1.0f };

  NMP::Vector3 qMin, qMax;
  NMP::Quat quatKey, qbar, invQbar;
  NMP::Vector3 v;
  uint32_t qx = 0, qy = 0, qz = 0;

  //-----------------------
  // Allocate the memory for the sampled quat table (for relative transforms)
  NMP_VERIFY(!m_sampledQuatTable);
  NMP::Memory::Format memReqsTable = AP::RotVecTable::getMemoryRequirements(sampledQuatNumChans, numSectionFrames);
  NMP::Memory::Resource memResTable = NMPMemoryAllocateFromFormat(memReqsTable);
  m_sampledQuatTable = AP::RotVecTable::init(memResTable, sampledQuatNumChans, numSectionFrames);

  //-----------------------
  // Allocate the memory for the channel means
  NMP_VERIFY(!m_sampledQuatMeans);
  NMP::Memory::Format memReqsMeans(0, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqsMeans.size = NMP::Memory::align(sizeof(NMP::Quat) * sampledQuatNumChans, NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::Resource memResMeans = NMPMemoryAllocateFromFormat(memReqsMeans);
  m_sampledQuatMeans = (NMP::Quat*)memResMeans.ptr;

  //-----------------------
  // Allocate the memory for the compiled channel quantisation data (means, precisions, qsets)
  NMP_VERIFY(!m_sampledQuatQuantisationData);
  NMP::Memory::Format memReqsQData(0, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqsQData.size = NMP::Memory::align(MR::QuantisationDataQSA::getMemoryRequirementsSize(sampledQuatNumChans), NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::Resource memResQData = NMPMemoryAllocateFromFormat(memReqsQData);
  m_sampledQuatQuantisationData = (MR::QuantisationDataQSA*)memResQData.ptr;
  m_sampledQuatQuantisationDataSize = memReqsQData.size;

  //-----------------------
  // Allocate memory for the quantisation table
  NMP::Memory::Format memReqsQuantQuat = AP::Vector3QuantisationTable::getMemoryRequirements(sampledQuatNumChans);
  NMP::Memory::Resource memResQuantQuat = NMPMemoryAllocateFromFormat(memReqsQuantQuat);
  m_sampledQuatQuantTable = AP::Vector3QuantisationTable::init(memResQuantQuat, sampledQuatNumChans);

  // Set the global quat quantisation bounds (for tan quarter angle rotation vectors)
  m_sampledQuatQuantTable->setGlobalQuantisationBounds(qMinTQA, qMaxTQA);

  // Set 8-bits of precision for quantising the channel means
  m_sampledQuatQuantTable->setPrecisions(8);

  //-----------------------
  // Compute the sampled quat transforms
  for (uint32_t iChan = 0; iChan < sampledQuatNumChans; ++iChan)
  {
    // Get the rig channel index for the compression channel
    uint16_t animChannelIndex = (uint16_t)sampledQuatCompToAnimMap[iChan];
    uint16_t rigChannelIndex;
    bool status = rigToAnimEntryMap->getRigIndexForAnimIndex(animChannelIndex, rigChannelIndex);
    NMP_VERIFY(status);
  
    // Get the section quat keys
    const NMP::Quat* quatKeys = channelSets->getChannelQuat(rigChannelIndex);
    NMP_VERIFY(quatKeys);
    const NMP::Quat* sectionQuatKeys = &quatKeys[sectionStartFrame];

    // Compute the mean of the quat sample data
    NMP::quatMean(numSectionFrames, sectionQuatKeys, qbar);

    // Convert the mean to tan quarter angle rotation vector
    v = qbar.toRotationVector(true);

    // Quantise the channel mean into 8-bits
    m_sampledQuatQuantTable->quantise(iChan, v, qx, qy, qz);
    QuantisationDataQSABuilder::init(m_sampledQuatQuantisationData[iChan], qx, qy, qz);

    // Recover the corresponding quantised mean value
    m_sampledQuatQuantTable->dequantise(iChan, v, qx, qy, qz);
    m_sampledQuatMeans[iChan].fromRotationVector(v, true);

    // Remove the mean from the quat sample data
    invQbar = ~m_sampledQuatMeans[iChan];
    for (uint32_t iFrame = 0; iFrame < numSectionFrames; ++iFrame)
    {
      quatKey = invQbar * sectionQuatKeys[iFrame];
      m_sampledQuatTable->setKey(iChan, iFrame, quatKey, true);
    }
  }
  // The quantization data is padded to a multiple of 4 entries.
  // Check QuantisationDataQSA::getMemoryRequirementsSize.
  // The last entry is duplicated to fill in the extra entries.
  uint32_t sampledQuatNumChans4 = NMP::nmAlignedValue4(sampledQuatNumChans);
  for (uint32_t iChan = sampledQuatNumChans; iChan < sampledQuatNumChans4; ++iChan)
  {
    QuantisationDataQSABuilder::init(m_sampledQuatQuantisationData[iChan], qx, qy, qz);
  }

  // Compute the quantisation bounds for the relative samples
  m_sampledQuatQuantTable->setQuantisationBounds(m_sampledQuatTable);
}

//----------------------------------------------------------------------------------------------------------------------
void SampledQuatCompressorQSA::assignQuantisationSets()
{
  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t sampledQuatNumChans = channelSetInfoCompressor->getSampledQuatNumChans();
  NMP_VERIFY(sampledQuatNumChans > 0);
  NMP::Vector3 qMin, qMax;

  //-----------------------
  // Assign the sampled quat channels to the quantisation sets
  uint32_t numQuantisationSets = m_animSectionCompressor->getNumQuantisationSets(CoefSetQSA::CoefSetSampledQuat);
  NMP_VERIFY(numQuantisationSets > 0);
  m_sampledQuatCompToQSetMapX.resize(sampledQuatNumChans);
  m_sampledQuatCompToQSetMapY.resize(sampledQuatNumChans);
  m_sampledQuatCompToQSetMapZ.resize(sampledQuatNumChans);

  NMP::UniformQuantisationInfoCompounder qSetCompounderX(sampledQuatNumChans);
  NMP::UniformQuantisationInfoCompounder qSetCompounderY(sampledQuatNumChans);
  NMP::UniformQuantisationInfoCompounder qSetCompounderZ(sampledQuatNumChans);

  for (uint32_t iChan = 0; iChan < sampledQuatNumChans; ++iChan)
  {
    m_sampledQuatQuantTable->getQuantisationBounds(iChan, qMin, qMax);
    qSetCompounderX.setQuantisationInfo(iChan, qMin.x, qMax.x);
    qSetCompounderY.setQuantisationInfo(iChan, qMin.y, qMax.y);
    qSetCompounderZ.setQuantisationInfo(iChan, qMin.z, qMax.z);
  }

  qSetCompounderX.compoundQuantisationInfo(numQuantisationSets);
  qSetCompounderY.compoundQuantisationInfo(numQuantisationSets);
  qSetCompounderZ.compoundQuantisationInfo(numQuantisationSets);

  for (uint32_t iChan = 0; iChan < sampledQuatNumChans; ++iChan)
  {
    m_sampledQuatCompToQSetMapX[iChan] = qSetCompounderX.getMapFromInputToCompounded(iChan);
    m_sampledQuatCompToQSetMapY[iChan] = qSetCompounderY.getMapFromInputToCompounded(iChan);
    m_sampledQuatCompToQSetMapZ[iChan] = qSetCompounderZ.getMapFromInputToCompounded(iChan);
  }

  for (uint32_t qSetIndex = 0; qSetIndex < numQuantisationSets; ++qSetIndex)
  {
    // Get the coefficient sets
    QuantisationSetQSA* qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledQuat, qSetIndex);
    NMP_VERIFY(qSet);
    CoefSetQSA* coefSetX = qSet->getCoefSetX();
    CoefSetQSA* coefSetY = qSet->getCoefSetY();
    CoefSetQSA* coefSetZ = qSet->getCoefSetZ();

    // Set the coefficient set quantisation bounds
    qSetCompounderX.getQuantisationInfoCompounded(qSetIndex, qMin.x, qMax.x);
    qSetCompounderY.getQuantisationInfoCompounded(qSetIndex, qMin.y, qMax.y);
    qSetCompounderZ.getQuantisationInfoCompounded(qSetIndex, qMin.z, qMax.z);
    coefSetX->setBounds(qMin.x, qMax.x);
    coefSetY->setBounds(qMin.y, qMax.y);
    coefSetZ->setBounds(qMin.z, qMax.z);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SampledQuatCompressorQSA::simpleRateAndDistortion()
{
  // Clear the rate and distortion information
  m_animSectionCompressor->clearRateAndDistortion(CoefSetQSA::CoefSetSampledQuat);

  // Compute the delta to dist mapping
  m_animSectionCompressor->makeSimpleDeltaToDistWeightMap(CoefSetQSA::CoefSetSampledQuat);
}

//----------------------------------------------------------------------------------------------------------------------
void SampledQuatCompressorQSA::analyseRateAndDistortion()
{
  NMP::Vector3 qMin, qMax;
  NMP::Vector3 dy, quatRel, quatRel_, bonePos;
  NMP::RigidMotionTMJacobian tmJac;
  uint32_t qx, qy, qz;
  float delta, dist, dispWeight;
  QuantisationSetQSA* qSet;
  CoefSetQSA* coefSetX;
  CoefSetQSA* coefSetY;
  CoefSetQSA* coefSetZ;

  //-----------------------
  // Information
  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const MR::RigToAnimEntryMap* rigToAnimEntryMap = animSourceCompressor->getConglomerateRigToAnimEntryMap();
  NMP_VERIFY(rigToAnimEntryMap);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t sampledQuatNumChans = channelSetInfoCompressor->getSampledQuatNumChans();
  NMP_VERIFY(sampledQuatNumChans > 0);
  uint32_t numSectionFrames = m_animSectionCompressor->getNumSectionFrames();
  const AnimSourceMotionAnalyser* motionAnalyser = animSourceCompressor->getMotionAnalyser();
  NMP_VERIFY(motionAnalyser);
  float averageBoneLength = motionAnalyser->getAverageBoneLength();
  float averageMaxBoundsWS = motionAnalyser->getAverageMaxBoundsWS();
  const std::vector<uint32_t>& sampledQuatCompToAnimMap = channelSetInfoCompressor->getSampledQuatCompToAnimMap();
  const std::vector<uint32_t>& analysisPrecisions = m_animSectionCompressor->getAnalysisPrecisions();
  NMP_VERIFY(!analysisPrecisions.empty());
  const std::vector<Vector3Table*>& bonePosTables = m_animSectionCompressor->getBonePosTables();
  const NMP::Matrix<float>* bonePosWeights = m_animSectionCompressor->getBonePosWeights();
  NMP_VERIFY(bonePosWeights);

  //-----------------------
  // Select a suitable average bone length for error distortion
  if (averageBoneLength == 0.0f)
  {
    if (averageMaxBoundsWS > 0.0f)
      averageBoneLength = averageMaxBoundsWS;
    else
      averageBoneLength = 0.001f;
  }

  //-----------------------
  // Clear the rate and distortion information
  m_animSectionCompressor->clearRateAndDistortion(CoefSetQSA::CoefSetSampledQuat);

  //-----------------------
  // Iterate over the section frame samples
  for (uint32_t iFrame = 0; iFrame < numSectionFrames; ++iFrame)
  {
    //-----------------------
    // Iterate over all compression channels
    for (uint32_t iChan = 0; iChan < sampledQuatNumChans; ++iChan)
    {
      // Get the rig channel for this compression channel
      uint16_t animChannelIndex = (uint16_t)sampledQuatCompToAnimMap[iChan];
      uint16_t rigChannelIndex;
      bool status = rigToAnimEntryMap->getRigIndexForAnimIndex(animChannelIndex, rigChannelIndex);
      NMP_VERIFY(status);
    
      // Get the table of normalised bone positions for the child joints
      // of the current channel
      uint32_t numBonePosChildren = 0;
      const Vector3Table* bonePosTable = bonePosTables[animChannelIndex];
      if (bonePosTable)
        numBonePosChildren = bonePosTable->getNumChannels(); // Note: End joints have no children

      // Set the current channel's frame quat transform
      m_sampledQuatTable->getKey(iChan, iFrame, quatRel);
      tmJac.setQuat(m_sampledQuatMeans[iChan], quatRel);

      // Get the quantisation set corresponding to this compression channel
      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledQuat, m_sampledQuatCompToQSetMapX[iChan]);
      coefSetX = qSet->getCoefSetX();
      const NMP::UniformQuantisationInfo& qInfoX = coefSetX->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledQuat, m_sampledQuatCompToQSetMapY[iChan]);
      coefSetY = qSet->getCoefSetY();
      const NMP::UniformQuantisationInfo& qInfoY = coefSetY->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledQuat, m_sampledQuatCompToQSetMapZ[iChan]);
      coefSetZ = qSet->getCoefSetZ();
      const NMP::UniformQuantisationInfo& qInfoZ = coefSetZ->getQuantisationInfo();

      qMin.set(qInfoX.m_qMin, qInfoY.m_qMin, qInfoZ.m_qMin);
      qMax.set(qInfoX.m_qMax, qInfoY.m_qMax, qInfoZ.m_qMax);

      // Error propagation for the remaining bits of precision
      std::vector<uint32_t>::const_iterator pit = analysisPrecisions.begin();
      std::vector<uint32_t>::const_iterator pit_end = analysisPrecisions.end();
      for (; pit != pit_end; ++pit)
      {
        uint32_t prec = *pit;

        // Set the quantiser precision and [qMin, qMax] range
        m_sampledQuatQuantTable->setPrecision(iChan, prec);
        if (prec > 0)
          m_sampledQuatQuantTable->setQuantisationBounds(iChan, qMin, qMax);
        else
          m_sampledQuatQuantTable->zeroQuantisationBounds(iChan); // Use the mean of the distribution (i.e. zero mean)

        // Quantise the relative quat sample (for the specified quantisation set)
        m_sampledQuatQuantTable->quantise(iChan, quatRel, qx, qy, qz);

        // Recover the corresponding quantised relative quat sample
        m_sampledQuatQuantTable->dequantise(iChan, quatRel_, qx, qy, qz);

        // Update the rate of the coefficient sets
        coefSetX->updateRate(prec);
        coefSetY->updateRate(prec);
        coefSetZ->updateRate(prec);

        //-----------------------
        // Compute a default distortion based on the average bone length. We compute
        // the error based on rotating the triad of average bone length points in the
        // directions of the canonical axes.
        dispWeight = bonePosWeights->element(iFrame, rigChannelIndex);
        for (uint32_t k = 0; k < 3; ++k)
        {
          bonePos.setToZero();
          bonePos.f[k] = averageBoneLength;
          tmJac.transformQuatJac(bonePos);

          delta = quatRel_.x - quatRel.x;
          coefSetX->updateDelta(prec, delta * delta);
          dy = tmJac.getErrorQuat(0, delta);
          dy *= dispWeight;
          dist = dy.magnitudeSquared();
          coefSetX->updateDistortion(prec, dist);

          delta = quatRel_.y - quatRel.y;
          coefSetY->updateDelta(prec, delta * delta);
          dy = tmJac.getErrorQuat(1, delta);
          dy *= dispWeight;
          dist = dy.magnitudeSquared();
          coefSetY->updateDistortion(prec, dist);

          delta = quatRel_.z - quatRel.z;
          coefSetZ->updateDelta(prec, delta * delta);
          dy = tmJac.getErrorQuat(2, delta);
          dy *= dispWeight;
          dist = dy.magnitudeSquared();
          coefSetZ->updateDistortion(prec, dist);
        }

        //-----------------------
        // Iterate over the child bone points of the current channel
        for (uint32_t boneChild = 0; boneChild < numBonePosChildren; ++boneChild)
        {
          // Get the child bone position
          bonePosTable->getKey(boneChild, iFrame, bonePos);
          dispWeight = bonePos.w;

          // Apply the rigid motion y' = R'*x + t of the current joint to the
          // child bone point to recover its worldspace position at the current frame.
          // We can then compute the error between the quantised y' and the
          // unquantised y positions dy = y' - y. Instead of doing this explicitly
          // (i.e. this is a forward difference Jacobian) we propagate the error
          // through our analytically computed Jacobian: dy = Dy/Dq * dq
          tmJac.transformQuatJac(bonePos); // Compute Dy/Dq at x

          //-----------------------
          // Recover the propagated error: dy = Dy/Dq * dq caused by the quantisation
          // error in X only
          delta = quatRel_.x - quatRel.x;
          coefSetX->updateDelta(prec, delta * delta);
          dy = tmJac.getErrorQuat(0, delta);
          dy *= dispWeight; // Apply the displacement weight

          // Update the distortion error ||y' - y||^2 for the coefficient set X
          dist = dy.magnitudeSquared();
          coefSetX->updateDistortion(prec, dist);

          //-----------------------
          // Recover the propagated error: dy = Dy/Dq * dq caused by the quantisation
          // error in Y only
          delta = quatRel_.y - quatRel.y;
          coefSetY->updateDelta(prec, delta * delta);
          dy = tmJac.getErrorQuat(1, delta);
          dy *= dispWeight; // Apply the displacement weight

          // Update the distortion error ||y' - y||^2 for the coefficient set Y
          dist = dy.magnitudeSquared();
          coefSetY->updateDistortion(prec, dist);

          //-----------------------
          // Recover the propagated error: dy = Dy/Dq * dq caused by the quantisation
          // error in Z only
          delta = quatRel_.z - quatRel.z;
          coefSetZ->updateDelta(prec, delta * delta);
          dy = tmJac.getErrorQuat(2, delta);
          dy *= dispWeight; // Apply the displacement weight

          // Update the distortion error ||y' - y||^2 for the coefficient set Z
          dist = dy.magnitudeSquared();
          coefSetZ->updateDistortion(prec, dist);

        } // Child bones
      } // Precisions
    } // Channels
  } // Frames

  //-----------------------
  // Compute the delta to dist mapping
  m_animSectionCompressor->makeDeltaToDistWeightMap(CoefSetQSA::CoefSetSampledQuat);
}

//----------------------------------------------------------------------------------------------------------------------
void SampledQuatCompressorQSA::computeRateAndDistortionMapped()
{
  NMP::Vector3 qMin, qMax;
  NMP::Vector3 dy, quatRel, quatRel_, bonePos;
  uint32_t qx, qy, qz;
  float delta;

  //-----------------------
  // Information
  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t sampledQuatNumChans = channelSetInfoCompressor->getSampledQuatNumChans();
  NMP_VERIFY(sampledQuatNumChans > 0);
  uint32_t numSectionFrames = m_animSectionCompressor->getNumSectionFrames();
  uint32_t maxPrec = m_animSectionCompressor->getMaxPrec();

  QuantisationSetQSA* qSet;
  CoefSetQSA* coefSetX;
  CoefSetQSA* coefSetY;
  CoefSetQSA* coefSetZ;

  //-----------------------
  // Clear the rate and delta information
  m_animSectionCompressor->clearRateAndDelta(CoefSetQSA::CoefSetSampledQuat);

  //-----------------------
  // Iterate over the section frame samples
  for (uint32_t iFrame = 0; iFrame < numSectionFrames; ++iFrame)
  {
    //-----------------------
    // Iterate over all compression channels
    for (uint32_t iChan = 0; iChan < sampledQuatNumChans; ++iChan)
    {
      // Set the current channel's frame quat transform
      m_sampledQuatTable->getKey(iChan, iFrame, quatRel);

      // Get the quantisation set corresponding to this compression channel
      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledQuat, m_sampledQuatCompToQSetMapX[iChan]);
      coefSetX = qSet->getCoefSetX();
      const NMP::UniformQuantisationInfo& qInfoX = coefSetX->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledQuat, m_sampledQuatCompToQSetMapY[iChan]);
      coefSetY = qSet->getCoefSetY();
      const NMP::UniformQuantisationInfo& qInfoY = coefSetY->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledQuat, m_sampledQuatCompToQSetMapZ[iChan]);
      coefSetZ = qSet->getCoefSetZ();
      const NMP::UniformQuantisationInfo& qInfoZ = coefSetZ->getQuantisationInfo();

      qMin.set(qInfoX.m_qMin, qInfoY.m_qMin, qInfoZ.m_qMin);
      qMax.set(qInfoX.m_qMax, qInfoY.m_qMax, qInfoZ.m_qMax);

      //-----------------------
      // Error propagation for the remaining bits of precision
      for (uint32_t prec = 0; prec <= maxPrec; ++prec)
      {
        // Set the quantiser precision and [qMin, qMax] range
        m_sampledQuatQuantTable->setPrecision(iChan, prec);
        if (prec > 0)
          m_sampledQuatQuantTable->setQuantisationBounds(iChan, qMin, qMax);
        else
          m_sampledQuatQuantTable->zeroQuantisationBounds(iChan); // Use the mean of the distribution (i.e. zero mean)

        // Quantise the relative quat sample (for the specified quantisation set)
        m_sampledQuatQuantTable->quantise(iChan, quatRel, qx, qy, qz);

        // Recover the corresponding quantised relative quat sample
        m_sampledQuatQuantTable->dequantise(iChan, quatRel_, qx, qy, qz);

        // Update the rate and distortion the coefficient sets
        delta = quatRel_.x - quatRel.x;
        coefSetX->updateRate(prec);
        coefSetX->updateDelta(prec, delta * delta);

        delta = quatRel_.y - quatRel.y;
        coefSetY->updateRate(prec);
        coefSetY->updateDelta(prec, delta * delta);

        delta = quatRel_.z - quatRel.z;
        coefSetZ->updateRate(prec);
        coefSetZ->updateDelta(prec, delta * delta);

      } // Precisions
    } // Channels
  } // Frames

  //-----------------------
  // Compute the mapped distortion
  m_animSectionCompressor->computeRateAndDistortion(CoefSetQSA::CoefSetSampledQuat);
}

//----------------------------------------------------------------------------------------------------------------------
void SampledQuatCompressorQSA::compileData()
{
  NMP::Vector3 qMin, qMax;
  NMP::Vector3 quatRel;
  uint32_t meanX = 0, meanY = 0, meanZ = 0;
  uint32_t precX = 0, precY = 0, precZ = 0;
  uint32_t qSetX = 0, qSetY = 0, qSetZ = 0;
  uint32_t qx, qy, qz;

  //-----------------------
  // Information
  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t sampledQuatNumChans = channelSetInfoCompressor->getSampledQuatNumChans();
  NMP_VERIFY(sampledQuatNumChans > 0);
  uint32_t numSectionFrames = m_animSectionCompressor->getNumSectionFrames();
  QuantisationSetQSA* qSet;

  //-----------------------
  // Compute the stride of the sample data
  computeDataStrides();

  //-----------------------
  // Compile the quantisation info ranges
  NMP::Memory::Format memReqsQInfo(0, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqsQInfo.size = NMP::Memory::align(sizeof(MR::QuantisationInfoQSA) * m_sampledQuatNumQuantisationSets, NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::Resource memResQInfo = NMPMemoryAllocateFromFormat(memReqsQInfo);
  m_sampledQuatQuantisationInfo = (MR::QuantisationInfoQSA*)memResQInfo.ptr;
  m_sampledQuatQuantisationInfoSize = memReqsQInfo.size;

  for (uint32_t i = 0; i < m_sampledQuatNumQuantisationSets; ++i)
  {
    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledQuat, i);
    const NMP::UniformQuantisationInfo& qInfoX = qSet->getCoefSetX()->getQuantisationInfo();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledQuat, i);
    const NMP::UniformQuantisationInfo& qInfoY = qSet->getCoefSetY()->getQuantisationInfo();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledQuat, i);
    const NMP::UniformQuantisationInfo& qInfoZ = qSet->getCoefSetZ()->getQuantisationInfo();

    qMin.set(qInfoX.m_qMin, qInfoY.m_qMin, qInfoZ.m_qMin);
    qMax.set(qInfoX.m_qMax, qInfoY.m_qMax, qInfoZ.m_qMax);

    QuantisationInfoQSABuilder::init(m_sampledQuatQuantisationInfo[i], qMin, qMax);
  }

  //-----------------------
  // Compile the quat quantisation data (means, precisions, qSets)
  for (uint32_t iChan = 0; iChan < sampledQuatNumChans; ++iChan)
  {
    // Get the quantisation info corresponding to this compression channel
    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledQuat, m_sampledQuatCompToQSetMapX[iChan]);
    const NMP::UniformQuantisationInfo& qInfoX = qSet->getCoefSetX()->getQuantisationInfo();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledQuat, m_sampledQuatCompToQSetMapY[iChan]);
    const NMP::UniformQuantisationInfo& qInfoY = qSet->getCoefSetY()->getQuantisationInfo();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledQuat, m_sampledQuatCompToQSetMapZ[iChan]);
    const NMP::UniformQuantisationInfo& qInfoZ = qSet->getCoefSetZ()->getQuantisationInfo();

    // Means
    meanX = m_sampledQuatQuantisationData[iChan].getMeanX();
    meanY = m_sampledQuatQuantisationData[iChan].getMeanY();
    meanZ = m_sampledQuatQuantisationData[iChan].getMeanZ();

    // Precisions
    precX = qInfoX.m_precision;
    precY = qInfoY.m_precision;
    precZ = qInfoZ.m_precision;

    // Quantisation sets
    qSetX = m_sampledQuatCompToQSetMapX[iChan];
    qSetY = m_sampledQuatCompToQSetMapY[iChan];
    qSetZ = m_sampledQuatCompToQSetMapZ[iChan];

    //-----------------------
    // Compile the quantisation data
    QuantisationDataQSABuilder::init(
      m_sampledQuatQuantisationData[iChan],
      meanX, meanY, meanZ,
      precX, precY, precZ,
      qSetX, qSetY, qSetZ);
  }
  // The quantization data is padded to a multiple of 4 entries.
  // Check QuantisationDataQSA::getMemoryRequirementsSize.
  // The last entry is duplicated to fill in the extra entries.
  uint32_t sampledQuatNumChans4 = NMP::nmAlignedValue4(sampledQuatNumChans);
  for (uint32_t iChan = sampledQuatNumChans; iChan < sampledQuatNumChans4; ++iChan)
  {
    QuantisationDataQSABuilder::init(
      m_sampledQuatQuantisationData[iChan],
      meanX, meanY, meanZ,
      precX, precY, precZ,
      qSetX, qSetY, qSetZ);
  }

  //-----------------------
  // Compile the bitstream data
  NMP::BitStreamEncoder encoder;

  uint32_t sampledQuatDataSize = m_sampledQuatByteStride * numSectionFrames;
  NMP::Memory::Format memReqsData = NMP::BitStreamEncoder::getMemoryRequirementsForBuffer(sampledQuatDataSize);
  NMP::Memory::Resource memResData = NMPMemoryAllocateFromFormat(memReqsData);
  m_sampledQuatData = (uint8_t*)memResData.ptr;
  m_sampledQuatDataSize = sampledQuatDataSize;

  encoder.init(sampledQuatDataSize, m_sampledQuatData);

  //-----------------------
  // Iterate over the section frame samples
  for (uint32_t iFrame = 0; iFrame < numSectionFrames; ++iFrame)
  {
    // Check that the bitstream is byte aligned
    NMP_VERIFY(encoder.isAligned());

#ifdef NMP_ENABLE_ASSERTS
    size_t bytesWrittenStart = encoder.getBytesWritten();
#endif

    //-----------------------
    // Iterate over all compression channels
    for (uint32_t iChan = 0; iChan < sampledQuatNumChans; ++iChan)
    {
      // Set the current channel's frame quat transform
      m_sampledQuatTable->getKey(iChan, iFrame, quatRel);

      // Get the quantisation set corresponding to this compression channel
      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledQuat, m_sampledQuatCompToQSetMapX[iChan]);
      const NMP::UniformQuantisationInfo& qInfoX = qSet->getCoefSetX()->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledQuat, m_sampledQuatCompToQSetMapY[iChan]);
      const NMP::UniformQuantisationInfo& qInfoY = qSet->getCoefSetY()->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledQuat, m_sampledQuatCompToQSetMapZ[iChan]);
      const NMP::UniformQuantisationInfo& qInfoZ = qSet->getCoefSetZ()->getQuantisationInfo();

      qMin.set(qInfoX.m_qMin, qInfoY.m_qMin, qInfoZ.m_qMin);
      qMax.set(qInfoX.m_qMax, qInfoY.m_qMax, qInfoZ.m_qMax);

      // Set the quantiser precision and [qMin, qMax] range
      m_sampledQuatQuantTable->setPrecisionX(iChan, qInfoX.m_precision);
      m_sampledQuatQuantTable->setPrecisionY(iChan, qInfoY.m_precision);
      m_sampledQuatQuantTable->setPrecisionZ(iChan, qInfoZ.m_precision);
      m_sampledQuatQuantTable->setQuantisationBounds(iChan, qMin, qMax);

      // Quantise the relative quat sample
      m_sampledQuatQuantTable->quantise(iChan, quatRel, qx, qy, qz);

      // Write the quantised data to the bitstream
      encoder.write(qInfoX.m_precision, qx);
      encoder.write(qInfoY.m_precision, qy);
      encoder.write(qInfoZ.m_precision, qz);

    } // Channels

    // Flush the bitstream to the nearest byte boundary
    encoder.flush();

#ifdef NMP_ENABLE_ASSERTS
    size_t bytesWrittenEnd = encoder.getBytesWritten();
    size_t channelByteStride = bytesWrittenEnd - bytesWrittenStart;
    NMP_VERIFY(channelByteStride == m_sampledQuatByteStride);
#endif

  } // Frames

  // Check we have written the correct amount of data
  NMP_VERIFY(encoder.getBytesWritten() == sampledQuatDataSize);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif
//----------------------------------------------------------------------------------------------------------------------
