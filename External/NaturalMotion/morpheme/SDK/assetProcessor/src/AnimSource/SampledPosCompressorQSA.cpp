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
#include "NMNumerics/NMVector3Utils.h"
#include "NMNumerics/NMRigidMotionTMJacobian.h"
#include "assetProcessor/AnimSource/AnimSourceCompressorQSA.h"
#include "assetProcessor/AnimSource/ChannelSetInfoCompressorQSA.h"
#include "assetProcessor/AnimSource/AnimSectionCompressorQSA.h"
#include "assetProcessor/AnimSource/SampledPosCompressorQSA.h"

//----------------------------------------------------------------------------------------------------------------------

// Disable warnings about deprecated functions (sprintf, fopen)
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning(disable : 4996)
#endif

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// SampledPosCompressorQSA Functions
//----------------------------------------------------------------------------------------------------------------------
SampledPosCompressorQSA::SampledPosCompressorQSA(AnimSectionCompressorQSA* animSectionCompressor)
{
  NMP_VERIFY(animSectionCompressor);
  m_animSectionCompressor = animSectionCompressor;

  //-----------------------
  // Init the data pointers
  m_sampledPosTable = NULL;
  m_sampledPosMeans = NULL;
  m_sampledPosQuantisationInfo = NULL;
  m_sampledPosQuantisationData = NULL;
  m_sampledPosQuantTable = NULL;
  m_sampledPosData = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
SampledPosCompressorQSA::~SampledPosCompressorQSA()
{
  if (m_sampledPosTable)
  {
    NMP::Memory::memFree(m_sampledPosTable);
    m_sampledPosTable = NULL;
  }

  if (m_sampledPosMeans)
  {
    NMP::Memory::memFree(m_sampledPosMeans);
    m_sampledPosMeans = NULL;
  }

  if (m_sampledPosQuantisationInfo)
  {
    NMP::Memory::memFree(m_sampledPosQuantisationInfo);
    m_sampledPosQuantisationInfo = NULL;
  }

  if (m_sampledPosQuantisationData)
  {
    NMP::Memory::memFree(m_sampledPosQuantisationData);
    m_sampledPosQuantisationData = NULL;
  }

  if (m_sampledPosQuantTable)
  {
    NMP::Memory::memFree(m_sampledPosQuantTable);
    m_sampledPosQuantTable = NULL;
  }

  //-----------------------
  // Quantisation sets
  m_sampledPosCompToQSetMapX.clear();
  m_sampledPosCompToQSetMapY.clear();
  m_sampledPosCompToQSetMapZ.clear();

  //-----------------------
  // Bitstream data
  if (m_sampledPosData)
  {
    NMP::Memory::memFree(m_sampledPosData);
    m_sampledPosData = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SampledPosCompressorQSA::computeDataStrides()
{
  // Info
  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t sampledPosNumChannels = channelSetInfoCompressor->getSampledPosNumChans();
  NMP_VERIFY(sampledPosNumChannels > 0);
  m_sampledPosNumQuantisationSets = m_animSectionCompressor->getNumQuantisationSets(CoefSetQSA::CoefSetSampledPos);

  QuantisationSetQSA* qSet;
  uint32_t strideBits;

  strideBits = 0;
  for (uint32_t iChan = 0; iChan < sampledPosNumChannels; ++iChan)
  {
    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledPos, m_sampledPosCompToQSetMapX[iChan]);
    const NMP::UniformQuantisationInfo& qInfoX = qSet->getCoefSetX()->getQuantisationInfo();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledPos, m_sampledPosCompToQSetMapY[iChan]);
    const NMP::UniformQuantisationInfo& qInfoY = qSet->getCoefSetY()->getQuantisationInfo();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledPos, m_sampledPosCompToQSetMapZ[iChan]);
    const NMP::UniformQuantisationInfo& qInfoZ = qSet->getCoefSetZ()->getQuantisationInfo();

    strideBits += qInfoX.m_precision;
    strideBits += qInfoY.m_precision;
    strideBits += qInfoZ.m_precision;
  }
  m_sampledPosByteStride = (strideBits + 0x07) >> 3; // Align stride to a byte boundary
}

//----------------------------------------------------------------------------------------------------------------------
void SampledPosCompressorQSA::computeRateAndDistortion()
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
NMP::Vector3 SampledPosCompressorQSA::evaluateQuantised(uint32_t iChan, uint32_t iFrame) const
{
  NMP::Vector3 posKey, posRel, posRel_;
  NMP::Vector3 qMin, qMax;
  uint32_t     qx, qy, qz;
  QuantisationSetQSA* qSet;
  CoefSetQSA* coefSetX;
  CoefSetQSA* coefSetY;
  CoefSetQSA* coefSetZ;

  // Get the quantisation set corresponding to this compression channel
  qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledPos, m_sampledPosCompToQSetMapX[iChan]);
  coefSetX = qSet->getCoefSetX();
  const NMP::UniformQuantisationInfo& qInfoX = coefSetX->getQuantisationInfo();

  qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledPos, m_sampledPosCompToQSetMapY[iChan]);
  coefSetY = qSet->getCoefSetY();
  const NMP::UniformQuantisationInfo& qInfoY = coefSetY->getQuantisationInfo();

  qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledPos, m_sampledPosCompToQSetMapZ[iChan]);
  coefSetZ = qSet->getCoefSetZ();
  const NMP::UniformQuantisationInfo& qInfoZ = coefSetZ->getQuantisationInfo();

  qMin.set(qInfoX.m_qMin, qInfoY.m_qMin, qInfoZ.m_qMin);
  qMax.set(qInfoX.m_qMax, qInfoY.m_qMax, qInfoZ.m_qMax);

  // Set the quantiser precision and [qMin, qMax] range
  m_sampledPosQuantTable->setPrecisionX(iChan, qInfoX.m_precision);
  m_sampledPosQuantTable->setPrecisionY(iChan, qInfoY.m_precision);
  m_sampledPosQuantTable->setPrecisionZ(iChan, qInfoZ.m_precision);
  m_sampledPosQuantTable->setQuantisationBounds(iChan, qMin, qMax);

  // Quantise the relative pos sample (for the specified quantisation set)
  m_sampledPosTable->getKey(iChan, iFrame, posRel);
  m_sampledPosQuantTable->quantise(iChan, posRel, qx, qy, qz);

  // Recover the corresponding quantised relative pos sample
  m_sampledPosQuantTable->dequantise(iChan, posRel_, qx, qy, qz);

  // Recover the channel pos sample
  posKey = m_sampledPosMeans[iChan] + posRel_;

  return posKey;
}

//----------------------------------------------------------------------------------------------------------------------
void SampledPosCompressorQSA::computeRelativeTransforms()
{
  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t sampledPosNumChans = channelSetInfoCompressor->getSampledPosNumChans();
  NMP_VERIFY(sampledPosNumChans > 0);
  const ChannelSetTable* animChannelSets = animSourceCompressor->getAnimChannelSets();
  NMP_VERIFY(animChannelSets);
  uint32_t numSectionFrames = m_animSectionCompressor->getNumSectionFrames();
  uint32_t sectionStartFrame = m_animSectionCompressor->getSectionStartFrame();

  NMP::Vector3 pbar, dy, qMin, qMax;
  NMP::Vector3 posKey, posRel, posRel_;
  uint32_t     qx = 0, qy = 0, qz = 0;

  // Information
  const std::vector<uint32_t>& sampledPosCompToAnimMap = channelSetInfoCompressor->getSampledPosCompToAnimMap();

  //-----------------------
  // Get the global pos quantisation bounds
  const MR::QuantisationInfoQSA& posMeansInfo = m_animSectionCompressor->getPosMeansQuantisationInfo();

  //-----------------------
  // Allocate the memory for the sampled pos table (for relative transforms)
  NMP_VERIFY(!m_sampledPosTable);
  NMP::Memory::Format memReqsTable = AP::Vector3Table::getMemoryRequirements(sampledPosNumChans, numSectionFrames);
  NMP::Memory::Resource memResTable = NMPMemoryAllocateFromFormat(memReqsTable);
  m_sampledPosTable = AP::Vector3Table::init(memResTable, sampledPosNumChans, numSectionFrames);

  //-----------------------
  // Allocate the memory for the channel means
  NMP_VERIFY(!m_sampledPosMeans);
  NMP::Memory::Format memReqsMeans(0, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqsMeans.size = NMP::Memory::align(sizeof(NMP::Vector3) * sampledPosNumChans, NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::Resource memResMeans = NMPMemoryAllocateFromFormat(memReqsMeans);
  m_sampledPosMeans = (NMP::Vector3*)memResMeans.ptr;

  //-----------------------
  // Allocate the memory for the compiled channel quantisation data (means, precisions, qsets)
  NMP_VERIFY(!m_sampledPosQuantisationData);
  NMP::Memory::Format memReqsQData(0, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqsQData.size = NMP::Memory::align(MR::QuantisationDataQSA::getMemoryRequirementsSize(sampledPosNumChans), NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::Resource memResQData = NMPMemoryAllocateFromFormat(memReqsQData);
  m_sampledPosQuantisationData = (MR::QuantisationDataQSA*)memResQData.ptr;
  m_sampledPosQuantisationDataSize = memReqsQData.size;

  //-----------------------
  // Allocate temporary memory for the quantisation table (qMin, qMax, prec)
  NMP::Memory::Format memReqsQuantPos = AP::Vector3QuantisationTable::getMemoryRequirements(sampledPosNumChans);
  NMP::Memory::Resource memResQuantPos = NMPMemoryAllocateFromFormat(memReqsQuantPos);
  m_sampledPosQuantTable = AP::Vector3QuantisationTable::init(memResQuantPos, sampledPosNumChans);

  // Set the global pos quantisation bounds
  m_sampledPosQuantTable->setGlobalQuantisationBounds(
    posMeansInfo.m_qMin,
    posMeansInfo.m_qMax);

  // Set 8-bits of precision for quantising the channel means
  m_sampledPosQuantTable->setPrecisions(8);

  //-----------------------
  // Compute the sampled pos transforms
  for (uint32_t iChan = 0; iChan < sampledPosNumChans; ++iChan)
  {
    uint32_t animChannelIndex = sampledPosCompToAnimMap[iChan];
    const NMP::Vector3* posKeys = animChannelSets->getChannelPos(animChannelIndex);
    NMP_VERIFY(posKeys);
    const NMP::Vector3* sectionPosKeys = &posKeys[sectionStartFrame];

    // Compute the mean of the pos sample data
    NMP::vecMean(numSectionFrames, sectionPosKeys, pbar);

    // Quantise the channel mean into 8-bits
    m_sampledPosQuantTable->quantise(iChan, pbar, qx, qy, qz);
    QuantisationDataQSABuilder::init(m_sampledPosQuantisationData[iChan], qx, qy, qz);

    // Recover the corresponding quantised mean value
    m_sampledPosQuantTable->dequantise(iChan, m_sampledPosMeans[iChan], qx, qy, qz);

    // Remove the mean from the pos sample data
    for (uint32_t iFrame = 0; iFrame < numSectionFrames; ++iFrame)
    {
      posKey = sectionPosKeys[iFrame] - m_sampledPosMeans[iChan];
      m_sampledPosTable->setKey(iChan, iFrame, posKey);
    }
  }
  // The quantization data is padded to a multiple of 4 entries.
  // Check QuantisationDataQSA::getMemoryRequirementsSize.
  // The last entry is duplicated to fill in the extra entries.
  uint32_t sampledPosNumChans4 = NMP::nmAlignedValue4(sampledPosNumChans);
  for (uint32_t iChan = sampledPosNumChans; iChan < sampledPosNumChans4; ++iChan)
  {
    QuantisationDataQSABuilder::init(m_sampledPosQuantisationData[iChan], qx, qy, qz);
  }

  // Compute the quantisation bounds for the relative samples
  m_sampledPosQuantTable->setQuantisationBounds(m_sampledPosTable);
}

//----------------------------------------------------------------------------------------------------------------------
void SampledPosCompressorQSA::assignQuantisationSets()
{
  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t sampledPosNumChans = channelSetInfoCompressor->getSampledPosNumChans();
  NMP_VERIFY(sampledPosNumChans > 0);
  NMP::Vector3 qMin, qMax;

  //-----------------------
  // Assign the sampled pos channels to the quantisation sets
  uint32_t numQuantisationSets = m_animSectionCompressor->getNumQuantisationSets(CoefSetQSA::CoefSetSampledPos);
  NMP_VERIFY(numQuantisationSets > 0);
  m_sampledPosCompToQSetMapX.resize(sampledPosNumChans);
  m_sampledPosCompToQSetMapY.resize(sampledPosNumChans);
  m_sampledPosCompToQSetMapZ.resize(sampledPosNumChans);

  NMP::UniformQuantisationInfoCompounder qSetCompounderX(sampledPosNumChans);
  NMP::UniformQuantisationInfoCompounder qSetCompounderY(sampledPosNumChans);
  NMP::UniformQuantisationInfoCompounder qSetCompounderZ(sampledPosNumChans);

  for (uint32_t iChan = 0; iChan < sampledPosNumChans; ++iChan)
  {
    m_sampledPosQuantTable->getQuantisationBounds(iChan, qMin, qMax);
    qSetCompounderX.setQuantisationInfo(iChan, qMin.x, qMax.x);
    qSetCompounderY.setQuantisationInfo(iChan, qMin.y, qMax.y);
    qSetCompounderZ.setQuantisationInfo(iChan, qMin.z, qMax.z);
  }

  qSetCompounderX.compoundQuantisationInfo(numQuantisationSets);
  qSetCompounderY.compoundQuantisationInfo(numQuantisationSets);
  qSetCompounderZ.compoundQuantisationInfo(numQuantisationSets);

  for (uint32_t iChan = 0; iChan < sampledPosNumChans; ++iChan)
  {
    m_sampledPosCompToQSetMapX[iChan] = qSetCompounderX.getMapFromInputToCompounded(iChan);
    m_sampledPosCompToQSetMapY[iChan] = qSetCompounderY.getMapFromInputToCompounded(iChan);
    m_sampledPosCompToQSetMapZ[iChan] = qSetCompounderZ.getMapFromInputToCompounded(iChan);
  }

  for (uint32_t qSetIndex = 0; qSetIndex < numQuantisationSets; ++qSetIndex)
  {
    // Get the coefficient sets
    QuantisationSetQSA* qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledPos, qSetIndex);
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
void SampledPosCompressorQSA::simpleRateAndDistortion()
{
  // Clear the rate and distortion information
  m_animSectionCompressor->clearRateAndDistortion(CoefSetQSA::CoefSetSampledPos);

  // Compute the delta to dist mapping
  m_animSectionCompressor->makeSimpleDeltaToDistWeightMap(CoefSetQSA::CoefSetSampledPos);
}

//----------------------------------------------------------------------------------------------------------------------
void SampledPosCompressorQSA::analyseRateAndDistortion()
{
  NMP::Vector3 qMin, qMax;
  NMP::Vector3 posRel, posRel_;
  uint32_t     qx, qy, qz;
  float        delta, dist;

  //-----------------------
  // Information
  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const MR::RigToAnimEntryMap* rigToAnimEntryMap = animSourceCompressor->getConglomerateRigToAnimEntryMap();
  NMP_VERIFY(rigToAnimEntryMap);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t sampledPosNumChans = channelSetInfoCompressor->getSampledPosNumChans();
  NMP_VERIFY(sampledPosNumChans > 0);
  const std::vector<uint32_t>& sampledPosCompToAnimMap = channelSetInfoCompressor->getSampledPosCompToAnimMap();  
  uint32_t numSectionFrames = m_animSectionCompressor->getNumSectionFrames();  
  const NMP::Matrix<float>* bonePosWeights = m_animSectionCompressor->getBonePosWeights();
  NMP_VERIFY(bonePosWeights);
  const std::vector<uint32_t>& analysisPrecisions = m_animSectionCompressor->getAnalysisPrecisions();
  NMP_VERIFY(!analysisPrecisions.empty());
  QuantisationSetQSA* qSet;
  CoefSetQSA* coefSetX;
  CoefSetQSA* coefSetY;
  CoefSetQSA* coefSetZ;

  //-----------------------
  // Clear the rate and distortion information
  m_animSectionCompressor->clearRateAndDistortion(CoefSetQSA::CoefSetSampledPos);

  //-----------------------
  // Iterate over the section frame samples
  for (uint32_t iFrame = 0; iFrame < numSectionFrames; ++iFrame)
  {
    //-----------------------
    // Iterate over all compression channels
    for (uint32_t iChan = 0; iChan < sampledPosNumChans; ++iChan)
    {
      // Set the current channel's frame pos transform
      m_sampledPosTable->getKey(iChan, iFrame, posRel);
      
      // Get the rig channel for this compression channel
      uint16_t animChannelIndex = (uint16_t)sampledPosCompToAnimMap[iChan];
      uint16_t rigChannelIndex;
      bool status = rigToAnimEntryMap->getRigIndexForAnimIndex(animChannelIndex, rigChannelIndex);
      NMP_VERIFY(status);

      // Get the displacement weight factor at the current frame for this channel
      float dispWeight = bonePosWeights->element(iFrame, rigChannelIndex);

      // Get the quantisation set corresponding to this compression channel
      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledPos, m_sampledPosCompToQSetMapX[iChan]);
      coefSetX = qSet->getCoefSetX();
      const NMP::UniformQuantisationInfo& qInfoX = coefSetX->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledPos, m_sampledPosCompToQSetMapY[iChan]);
      coefSetY = qSet->getCoefSetY();
      const NMP::UniformQuantisationInfo& qInfoY = coefSetY->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledPos, m_sampledPosCompToQSetMapZ[iChan]);
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
        m_sampledPosQuantTable->setPrecision(iChan, prec);
        if (prec > 0)
          m_sampledPosQuantTable->setQuantisationBounds(iChan, qMin, qMax);
        else
          m_sampledPosQuantTable->zeroQuantisationBounds(iChan); // Use the mean of the distribution (i.e. zero mean)

        // Quantise the relative pos sample
        m_sampledPosQuantTable->quantise(iChan, posRel, qx, qy, qz);

        // Recover the corresponding quantised relative pos sample
        m_sampledPosQuantTable->dequantise(iChan, posRel_, qx, qy, qz);

        // Apply the rigid motion y' = R*x + t' of the current joint to the
        // child bone point to recover its worldspace position at the current frame.
        // We can then compute the error between the quantised y' and the
        // unquantised y positions dy = y' - y. Instead of doing this explicitly
        // (i.e. this is a forward difference Jacobian) we propagate the error
        // through our analytically computed Jacobian: dy = Dy/Dt * dt
        // We note that Dy/Dt = I and update the rate and distortion directly
        delta = posRel_.x - posRel.x;
        dist = delta * dispWeight;
        coefSetX->updateRate(prec);
        coefSetX->updateDelta(prec, delta * delta);
        coefSetX->updateDistortion(prec, dist * dist);

        delta = posRel_.y - posRel.y;
        dist = delta * dispWeight;
        coefSetY->updateRate(prec);
        coefSetY->updateDelta(prec, delta * delta);
        coefSetY->updateDistortion(prec, dist * dist);

        delta = posRel_.z - posRel.z;
        dist = delta * dispWeight;
        coefSetZ->updateRate(prec);
        coefSetZ->updateDelta(prec, delta * delta);
        coefSetZ->updateDistortion(prec, dist * dist);

      } // Precisions
    } // Channels
  } // Frames

  //-----------------------
  // Compute the delta to dist mapping
  m_animSectionCompressor->makeDeltaToDistWeightMap(CoefSetQSA::CoefSetSampledPos);
}

//----------------------------------------------------------------------------------------------------------------------
void SampledPosCompressorQSA::computeRateAndDistortionMapped()
{
  NMP::Vector3 qMin, qMax;
  NMP::Vector3 posRel, posRel_;
  uint32_t     qx, qy, qz;
  float        delta;

  //-----------------------
  // Information
  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t sampledPosNumChans = channelSetInfoCompressor->getSampledPosNumChans();
  NMP_VERIFY(sampledPosNumChans > 0);
  uint32_t numSectionFrames = m_animSectionCompressor->getNumSectionFrames();
  QuantisationSetQSA* qSet;
  CoefSetQSA* coefSetX;
  CoefSetQSA* coefSetY;
  CoefSetQSA* coefSetZ;
  uint32_t maxPrec = m_animSectionCompressor->getMaxPrec();

  //-----------------------
  // Clear the rate and delta information
  m_animSectionCompressor->clearRateAndDelta(CoefSetQSA::CoefSetSampledPos);

  //-----------------------
  // Iterate over the section frame samples
  for (uint32_t iFrame = 0; iFrame < numSectionFrames; ++iFrame)
  {
    //-----------------------
    // Iterate over all compression channels
    for (uint32_t iChan = 0; iChan < sampledPosNumChans; ++iChan)
    {
      // Set the current channel's frame pos transform
      m_sampledPosTable->getKey(iChan, iFrame, posRel);

      // Get the quantisation set corresponding to this compression channel
      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledPos, m_sampledPosCompToQSetMapX[iChan]);
      coefSetX = qSet->getCoefSetX();
      const NMP::UniformQuantisationInfo& qInfoX = coefSetX->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledPos, m_sampledPosCompToQSetMapY[iChan]);
      coefSetY = qSet->getCoefSetY();
      const NMP::UniformQuantisationInfo& qInfoY = coefSetY->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledPos, m_sampledPosCompToQSetMapZ[iChan]);
      coefSetZ = qSet->getCoefSetZ();
      const NMP::UniformQuantisationInfo& qInfoZ = coefSetZ->getQuantisationInfo();

      qMin.set(qInfoX.m_qMin, qInfoY.m_qMin, qInfoZ.m_qMin);
      qMax.set(qInfoX.m_qMax, qInfoY.m_qMax, qInfoZ.m_qMax);

      // Error propagation for the remaining bits of precision
      for (uint32_t prec = 0; prec <= maxPrec; ++prec)
      {
        // Set the quantiser precision and [qMin, qMax] range
        m_sampledPosQuantTable->setPrecision(iChan, prec);
        if (prec > 0)
          m_sampledPosQuantTable->setQuantisationBounds(iChan, qMin, qMax);
        else
          m_sampledPosQuantTable->zeroQuantisationBounds(iChan); // Use the mean of the distribution (i.e. zero mean)

        // Quantise the relative pos sample
        m_sampledPosQuantTable->quantise(iChan, posRel, qx, qy, qz);

        // Recover the corresponding quantised relative pos sample
        m_sampledPosQuantTable->dequantise(iChan, posRel_, qx, qy, qz);

        // Apply the rigid motion y' = R*x + t' of the current joint to the
        // child bone point to recover its worldspace position at the current frame.
        // We can then compute the error between the quantised y' and the
        // unquantised y positions dy = y' - y. Instead of doing this explicitly
        // (i.e. this is a forward difference Jacobian) we propagate the error
        // through our analytically computed Jacobian: dy = Dy/Dt * dt
        // We note that Dy/Dt = I and update the rate and distortion directly
        delta = posRel_.x - posRel.x;
        coefSetX->updateRate(prec);
        coefSetX->updateDelta(prec, delta * delta);

        delta = posRel_.y - posRel.y;
        coefSetY->updateRate(prec);
        coefSetY->updateDelta(prec, delta * delta);

        delta = posRel_.z - posRel.z;
        coefSetZ->updateRate(prec);
        coefSetZ->updateDelta(prec, delta * delta);

      } // Precisions
    } // Channels
  } // Frames

  //-----------------------
  // Compute the mapped distortion
  m_animSectionCompressor->computeRateAndDistortion(CoefSetQSA::CoefSetSampledPos);
}

//----------------------------------------------------------------------------------------------------------------------
void SampledPosCompressorQSA::compileData()
{
  NMP::Vector3 qMin, qMax;
  NMP::Vector3 posRel;
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
  uint32_t sampledPosNumChans = channelSetInfoCompressor->getSampledPosNumChans();
  NMP_VERIFY(sampledPosNumChans > 0);
  uint32_t numSectionFrames = m_animSectionCompressor->getNumSectionFrames();
  QuantisationSetQSA* qSet;

  //-----------------------
  // Compute the stride of the sample data
  computeDataStrides();

  //-----------------------
  // Compile the quantisation info ranges
  NMP::Memory::Format memReqsQInfo(0, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqsQInfo.size = NMP::Memory::align(sizeof(MR::QuantisationInfoQSA) * m_sampledPosNumQuantisationSets, NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::Resource memResQInfo = NMPMemoryAllocateFromFormat(memReqsQInfo);
  m_sampledPosQuantisationInfo = (MR::QuantisationInfoQSA*)memResQInfo.ptr;
  m_sampledPosQuantisationInfoSize = memReqsQInfo.size;

  for (uint32_t i = 0; i < m_sampledPosNumQuantisationSets; ++i)
  {
    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledPos, i);
    const NMP::UniformQuantisationInfo& qInfoX = qSet->getCoefSetX()->getQuantisationInfo();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledPos, i);
    const NMP::UniformQuantisationInfo& qInfoY = qSet->getCoefSetY()->getQuantisationInfo();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledPos, i);
    const NMP::UniformQuantisationInfo& qInfoZ = qSet->getCoefSetZ()->getQuantisationInfo();

    qMin.set(qInfoX.m_qMin, qInfoY.m_qMin, qInfoZ.m_qMin);
    qMax.set(qInfoX.m_qMax, qInfoY.m_qMax, qInfoZ.m_qMax);

    QuantisationInfoQSABuilder::init(m_sampledPosQuantisationInfo[i], qMin, qMax);
  }

  //-----------------------
  // Compile the pos quantisation data (means, precisions, qSets)
  for (uint32_t iChan = 0; iChan < sampledPosNumChans; ++iChan)
  {
    // Get the quantisation info corresponding to this compression channel
    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledPos, m_sampledPosCompToQSetMapX[iChan]);
    const NMP::UniformQuantisationInfo& qInfoX = qSet->getCoefSetX()->getQuantisationInfo();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledPos, m_sampledPosCompToQSetMapY[iChan]);
    const NMP::UniformQuantisationInfo& qInfoY = qSet->getCoefSetY()->getQuantisationInfo();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledPos, m_sampledPosCompToQSetMapZ[iChan]);
    const NMP::UniformQuantisationInfo& qInfoZ = qSet->getCoefSetZ()->getQuantisationInfo();

    // Means
    meanX = m_sampledPosQuantisationData[iChan].getMeanX();
    meanY = m_sampledPosQuantisationData[iChan].getMeanY();
    meanZ = m_sampledPosQuantisationData[iChan].getMeanZ();

    // Precisions
    precX = qInfoX.m_precision;
    precY = qInfoY.m_precision;
    precZ = qInfoZ.m_precision;

    // Quantisation sets
    qSetX = m_sampledPosCompToQSetMapX[iChan];
    qSetY = m_sampledPosCompToQSetMapY[iChan];
    qSetZ = m_sampledPosCompToQSetMapZ[iChan];

    //-----------------------
    // Compile the quantisation data
    QuantisationDataQSABuilder::init(
      m_sampledPosQuantisationData[iChan],
      meanX, meanY, meanZ,
      precX, precY, precZ,
      qSetX, qSetY, qSetZ);
  }
  // The quantization data is padded to a multiple of 4 entries.
  // Check QuantisationDataQSA::getMemoryRequirementsSize.
  // The last entry is duplicated to fill in the extra entries.
  uint32_t sampledPosNumChans4 = NMP::nmAlignedValue4(sampledPosNumChans);
  for (uint32_t iChan = sampledPosNumChans; iChan < sampledPosNumChans4; ++iChan)
  {
    QuantisationDataQSABuilder::init(
      m_sampledPosQuantisationData[iChan],
      meanX, meanY, meanZ,
      precX, precY, precZ,
      qSetX, qSetY, qSetZ);
  }

  //-----------------------
  // Compile the bitstream data
  NMP::BitStreamEncoder encoder;

  uint32_t sampledPosDataSize = m_sampledPosByteStride * numSectionFrames;
  NMP::Memory::Format memReqsData = NMP::BitStreamEncoder::getMemoryRequirementsForBuffer(sampledPosDataSize);
  NMP::Memory::Resource memResData = NMPMemoryAllocateFromFormat(memReqsData);
  m_sampledPosData = (uint8_t*)memResData.ptr;
  m_sampledPosDataSize = sampledPosDataSize;

  encoder.init(sampledPosDataSize, m_sampledPosData);

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
    for (uint32_t iChan = 0; iChan < sampledPosNumChans; ++iChan)
    {
      // Set the current channel's frame pos transform
      m_sampledPosTable->getKey(iChan, iFrame, posRel);

      // Get the quantisation set corresponding to this compression channel
      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledPos, m_sampledPosCompToQSetMapX[iChan]);
      const NMP::UniformQuantisationInfo& qInfoX = qSet->getCoefSetX()->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledPos, m_sampledPosCompToQSetMapY[iChan]);
      const NMP::UniformQuantisationInfo& qInfoY = qSet->getCoefSetY()->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSampledPos, m_sampledPosCompToQSetMapZ[iChan]);
      const NMP::UniformQuantisationInfo& qInfoZ = qSet->getCoefSetZ()->getQuantisationInfo();

      qMin.set(qInfoX.m_qMin, qInfoY.m_qMin, qInfoZ.m_qMin);
      qMax.set(qInfoX.m_qMax, qInfoY.m_qMax, qInfoZ.m_qMax);

      // Set the quantiser precision and [qMin, qMax] range
      m_sampledPosQuantTable->setPrecisionX(iChan, qInfoX.m_precision);
      m_sampledPosQuantTable->setPrecisionY(iChan, qInfoY.m_precision);
      m_sampledPosQuantTable->setPrecisionZ(iChan, qInfoZ.m_precision);
      m_sampledPosQuantTable->setQuantisationBounds(iChan, qMin, qMax);

      // Quantise the relative pos sample
      m_sampledPosQuantTable->quantise(iChan, posRel, qx, qy, qz);

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
    NMP_VERIFY(channelByteStride == m_sampledPosByteStride);
#endif
  } // Frames

  // Check we have written the correct amount of data
  NMP_VERIFY(encoder.getBytesWritten() == sampledPosDataSize);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif
//----------------------------------------------------------------------------------------------------------------------
