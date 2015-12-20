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
#include <limits>
#include "NMPlatform/NMTimer.h"
#include "assetProcessor/AnimSource/ChannelSetInfoCompressorQSA.h"
#include "assetProcessor/AnimSource/AnimSectionCompressorQSA.h"
#include "assetProcessor/AnimSource/AnimSourceCompressorQSA.h"
//----------------------------------------------------------------------------------------------------------------------

// For profiling compressor
// #define PROFILE_QSA
// #define DEBUG_TRANSFORMS

// Disable warnings about deprecated functions (sprintf, fopen)
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning(disable : 4996)
#endif

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
class AnimSectionQSABuilder : private MR::AnimSectionQSA
{
public:
  friend class AnimSectionCompressorQSA;  // Allow the compressor access to the internal parameters
};

//----------------------------------------------------------------------------------------------------------------------
// AnimSectionCompressorQSA Functions
//----------------------------------------------------------------------------------------------------------------------
AnimSectionCompressorQSA::AnimSectionCompressorQSA(AnimSourceCompressorQSA* manager, uint32_t sectionID)
{
  NMP_VERIFY(manager);
  m_manager = manager;
  m_sectionID = sectionID;
  m_numChannelSets = 0;

  // Quantisation sets
  m_maxPrec = 15;
  m_analysisPrecisions.push_back(8);
  m_analysisPrecisions.push_back(11);
  m_analysisPrecisions.push_back(14);

  //-----------------------
  // Spline fitting
  m_knots = NULL;
  m_knotsSaved = NULL;

  //-----------------------
  // Uniform quantisation
  m_quantiser = NULL;
  m_hierarchyDists = NULL;
  m_tmAccumulatorA = NULL;
  m_tmAccumulatorB = NULL;
  m_bonePosWeights = NULL;

  //-----------------------
  // Sampled pos channel
  m_sampledPosCompressor = NULL;

  //-----------------------
  // Sampled quat channel
  m_sampledQuatCompressor = NULL;

  //-----------------------
  // Spline pos channel
  m_splinePosCompressor = NULL;
  m_splinePosCompressorSaved = NULL;

  //-----------------------
  // Spline quat channel
  m_splineQuatCompressor = NULL;
  m_splineQuatCompressorSaved = NULL;

  //-----------------------
  // Compiled section data
  m_sectionData = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
AnimSectionCompressorQSA::~AnimSectionCompressorQSA()
{
  termCompressor();
}

//----------------------------------------------------------------------------------------------------------------------
// Callback Functions
//----------------------------------------------------------------------------------------------------------------------
void AnimSectionCompressorQSA::initCompressor()
{
  //-----------------------
  // Information
  m_numChannelSets = m_manager->getNumAnimChannelSets();
  NMP_VERIFY(m_numChannelSets > 0);
  const std::vector<uint32_t>& startFrames = m_manager->getSectionStartFrames();
  const std::vector<uint32_t>& endFrames = m_manager->getSectionEndFrames();
  const std::vector<size_t>& sectionSizes = m_manager->getSectionSizes();
  m_sectionStartFrame = startFrames[m_sectionID];
  m_sectionEndFrame = endFrames[m_sectionID];
  m_sectionSize = sectionSizes[m_sectionID];
  uint32_t numSectionFrames = m_sectionEndFrame - m_sectionStartFrame + 1;

  //-----------------------
  // Allocate the memory for the simple knots vector
  NMP_VERIFY(!m_knots);
  m_knots = new NMP::SimpleKnotVector(numSectionFrames);
  m_knots->setNumKnots(0);

  NMP_VERIFY(!m_knotsSaved);
  m_knotsSaved = new NMP::SimpleKnotVector(numSectionFrames);
  m_knotsSaved->setNumKnots(0);
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionCompressorQSA::termCompressor()
{
  //-----------------------
  // Spline fitting
  if (m_knots)
  {
    delete m_knots;
    m_knots = NULL;
  }

  if (m_knotsSaved)
  {
    delete m_knotsSaved;
    m_knotsSaved = NULL;
  }

  termQuantisation();

  //-----------------------
  // Clear the channel data
  if (m_sampledPosCompressor)
  {
    delete m_sampledPosCompressor;
    m_sampledPosCompressor = NULL;
  }

  if (m_sampledQuatCompressor)
  {
    delete m_sampledQuatCompressor;
    m_sampledQuatCompressor = NULL;
  }

  if (m_splinePosCompressor)
  {
    delete m_splinePosCompressor;
    m_splinePosCompressor = NULL;
  }

  if (m_splinePosCompressorSaved)
  {
    delete m_splinePosCompressorSaved;
    m_splinePosCompressorSaved = NULL;
  }

  if (m_splineQuatCompressor)
  {
    delete m_splineQuatCompressor;
    m_splineQuatCompressor = NULL;
  }

  if (m_splineQuatCompressorSaved)
  {
    delete m_splineQuatCompressorSaved;
    m_splineQuatCompressorSaved = NULL;
  }

  //-----------------------
  // Compiled section data
  if (m_sectionData)
  {
    NMP::Memory::memFree(m_sectionData);
    m_sectionData = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AnimSectionCompressorQSA::getNumSectionFrames() const
{
  return m_sectionEndFrame - m_sectionStartFrame + 1;
}

//----------------------------------------------------------------------------------------------------------------------
bool AnimSectionCompressorQSA::compileSection()
{
  //-----------------------
  // Information
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = m_manager->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  NMP_VERIFY(m_knots);
  uint32_t framesPerKey = m_manager->getFramesPerKey();
  uint32_t sampledPosNumChans = channelSetInfoCompressor->getSampledPosNumChans();
  uint32_t sampledQuatNumChans = channelSetInfoCompressor->getSampledQuatNumChans();
  uint32_t splinePosNumChans = channelSetInfoCompressor->getSplinePosNumChans();
  uint32_t splineQuatNumChans = channelSetInfoCompressor->getSplineQuatNumChans();

  //-----------------------
  // DEBUG
#ifdef NM_DEBUG
  #ifdef PROFILE_QSA
  NMP::Timer profilingTimer;
  profilingTimer.start();
  #endif // PROFILE_QSA
#endif // NM_DEBUG

  //-----------------------
  // Initialisation
  initQuantisation(); // Create a new quantiser and determine the coef set indices

  //-----------------------
  // Compute the normalised bone pos tables
  if (m_manager->getUseWorldSpace())
  {
    computeBonePosWeights();
    computeBonePosTables();
  }  

  //-----------------------
  // Compute the global pos quantisation [qMin, qMax] bounds for the
  // coarse pos means info. i.e. pos means are quantised into 8-bits
  // and all pos sample data is encoded relative to these means
  computePosMeansQuantisationInfo();

  //-----------------------
  // Build the rate and distortion characteristic tables for the sampled
  // channel coefficient sets
  if (sampledPosNumChans > 0)
  {
    NMP_VERIFY(!m_sampledPosCompressor);
    m_sampledPosCompressor = new SampledPosCompressorQSA(this);

    // Computes relative transforms and assigns the quantisation sets
    m_sampledPosCompressor->computeRateAndDistortion();
  }

  if (sampledQuatNumChans > 0)
  {
    NMP_VERIFY(!m_sampledQuatCompressor);
    m_sampledQuatCompressor = new SampledQuatCompressorQSA(this);

    // Computes relative transforms and assigns the quantisation sets
    m_sampledQuatCompressor->computeRateAndDistortion();
  }

  //-----------------------
  // Check if the animation has any spline channels
  if (splinePosNumChans > 0 || splineQuatNumChans > 0)
  {
    //-----------------------
    // Compute the knot vector with a specified frames per key level
    m_knots->setKnotsDiscrete(framesPerKey, m_sectionStartFrame, m_sectionEndFrame);

    // Pos splines
    if (splinePosNumChans > 0)
    {
      // Create a new compressor
      NMP_VERIFY(!m_splinePosCompressor);
      m_splinePosCompressor = new SplinePosCompressorQSA(this);
      m_splinePosCompressor->computeSplines(*m_knots);

      // Computes relative transforms and assigns the quantisation sets
      m_splinePosCompressor->computeRateAndDistortion();
    }

    // Quat splines
    if (splineQuatNumChans > 0)
    {
      // Create a new compressor
      NMP_VERIFY(!m_splineQuatCompressor);
      m_splineQuatCompressor = new SplineQuatCompressorQSA(this);
      m_splineQuatCompressor->computeSplines(*m_knots);
      m_splineQuatCompressor->computeTangentMeansQuantisationInfo();

      // Computes relative transforms and assigns the quantisation sets
      m_splineQuatCompressor->computeRateAndDistortion();
    }
  }

  //-----------------------
  // Compute the byte budget for the section data
  currentSectionHeaderSize(); // Header and channel means info
  currentSectionDataSize(); // Section data budget (May allocate extra byte budget)

  // Compute the optimal bit assignments for the coefficient sets that are within
  // the alloted section data byte budget
  if (m_quantiser)
    m_quantiser->distributeBits(m_curSectionDataSize);

  // Get the actual section size (coefficient set strides are byte aligned)
  currentSectionSize();

  //-----------------------
  // Optimise the coefficient set weight vector

  //-----------------------
  // There is no easy way of dealing with the alignment and padding so we iteratively
  // reduce the size of the byte budget until the distribution of channel bits fits into
  // the overall section size
  while (m_curSectionSize > m_sectionSize)
  {
    // Reduce the section data budget by the amount required for the padding
    size_t diffSize = NMP::maximum(m_curSectionSize - m_sectionSize, (size_t)4);
    if (m_curSectionDataSize >= diffSize)
      m_curSectionDataSize -= diffSize;
    else
      m_curSectionDataSize = 0;

    // Compute the bit assignments that fit into the current data budget
    m_quantiser->distributeBits(m_curSectionDataSize);

    // Get the actual section size (coefficient set strides are byte aligned)
    currentSectionSize();
  }

  //-----------------------
  // DEBUG
#ifdef NM_DEBUG
  #ifdef PROFILE_QSA
  //-----------------------
  float elapsedTime = profilingTimer.stop();
  FILE* filePointer = fopen("C:/qsa_profile.txt", "a");
  NMP_VERIFY(filePointer);
  if (filePointer)
  {
    fprintf(filePointer, "%f ms\n", elapsedTime);
    fclose(filePointer);
  }
  #endif // PROFILE_QSA
#endif // NM_DEBUG

  //-----------------------
  // Compile the individual section data pieces
  if (m_sampledPosCompressor)
    m_sampledPosCompressor->compileData();

  if (m_sampledQuatCompressor)
    m_sampledQuatCompressor->compileData();

  if (m_splinePosCompressor)
    m_splinePosCompressor->compileData();

  if (m_splineQuatCompressor)
    m_splineQuatCompressor->compileData();

  //-----------------------
  // Assemble the pieces to build the complete section
  buildSection();

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool AnimSectionCompressorQSA::getAnimSectionHasFullPrec() const
{
  for (uint32_t i = 0; i < CoefSetQSA::CoefSetNumTypes; ++i)
  {
    const std::vector<QuantisationSetQSA*>& qSets = m_quantisationSets[i];
    uint32_t numQSets = (uint32_t)qSets.size();

    for (uint32_t qSetIndex = 0; qSetIndex < numQSets; ++qSetIndex)
    {
      const QuantisationSetQSA* qSet = qSets[qSetIndex];
      NMP_VERIFY(qSet);
      if (!qSet->getHasFullPrec())
        return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionCompressorQSA::writeDebug(FILE* filePointer) const
{
  NMP_VERIFY(filePointer);

  writeQuantiser(filePointer);

  if (m_sampledQuatCompressor)
    m_sampledQuatCompressor->writeDebug(filePointer);

  if (m_splinePosCompressor)
    m_splinePosCompressor->writeDebug(filePointer);

  if (m_splineQuatCompressor)
    m_splineQuatCompressor->writeDebug(filePointer);
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionCompressorQSA::writeQuantiser(FILE* filePointer) const
{
  NMP_VERIFY(filePointer);

  fprintf(filePointer, "\n_______________ Byte budget _______________\n");
  fprintf(filePointer, "section byte budget = %d\n", (uint32_t)m_sectionSize);
  fprintf(filePointer, "actual used section budget = %d\n", m_curSectionSize);

  fprintf(filePointer, "\n_______________ Coef Set Quantiser _______________\n");

  // Write out the coefficient set bit allocations
  if (m_quantiser)
  {
    uint32_t numCoefSets = m_quantiser->getNumCoefSets();
    for (uint32_t i = 0; i < numCoefSets; ++i)
    {
      const NMP::UniformQuantisationCoefSetBase* coefSet = m_quantiser->getCoefficientSet(i);
      NMP_VERIFY(coefSet);
      const NMP::UniformQuantisationInfo& qInfo = coefSet->getQuantisationInfo();
      uint32_t coefSetType = coefSet->getUserID();
      uint32_t rate = (uint32_t)coefSet->getRate();
      float dist = coefSet->getDistortion();

      fprintf(filePointer, "CoefSet = %d, type = %d, prec = %d, qMin = %3.5f, qMax = %3.5f, rate = %d, dist = %f\n",
              i, coefSetType,
              qInfo.m_precision, qInfo.m_qMin, qInfo.m_qMax,
              rate,
              dist);
    }

    fprintf(filePointer, "\n_______________ Rate and Distortion tables _______________\n");
    for (uint32_t i = 0; i < numCoefSets; ++i)
    {
      const NMP::UniformQuantisationCoefSetBase* coefSet = m_quantiser->getCoefficientSet(i);
      NMP_VERIFY(coefSet);
      uint32_t numQuantisers = coefSet->getNumQuantisers();
      const float* rates = coefSet->getRates();
      const float* dists = coefSet->getDistortions();

      fprintf(filePointer, "\n_______________ CoefSet = %d _______________\n", i);
      for (uint32_t k = 0; k < numQuantisers; ++k)
      {
        fprintf(filePointer, "prec = %d, rate = %d, dist = %f\n",
                k, (uint32_t)rates[k], dists[k]);
      }
    }
  }
  else
  {
    fprintf(filePointer, "No coefficient sets: Unchanging data only!\n");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionCompressorQSA::writeRecomposedWS(FILE* filePointer)
{
  NMP_VERIFY(filePointer);
  fprintf(filePointer, "\n_______________ Recomposed WS _______________\n");

  fprintf(filePointer, "Section start frame = %d\n", m_sectionStartFrame);
  fprintf(filePointer, "Section end frame = %d\n", m_sectionEndFrame);
  uint32_t numKnots = m_knots->getNumKnots();
  fprintf(filePointer, "Num knots = %d\n", numKnots);
  for (uint32_t i = 0; i < numKnots; ++i)
  {
    fprintf(filePointer, "%3.5f\n", m_knots->getKnot(i));
  }

  NMP_VERIFY(m_tmAccumulatorA);

  const MR::AnimRigDef* rig = m_manager->getConglomerateRig();
  NMP_VERIFY(rig);
  const NMP::Hierarchy* hierarchy = rig->getHierarchy();
  NMP_VERIFY(hierarchy);
  uint32_t numRigChannels = hierarchy->getNumEntries();
  uint32_t numSectionFrames = getNumSectionFrames();

  NMP::Memory::Format memReqsWS = AP::ChannelSetTable::getMemoryRequirements(numRigChannels, numSectionFrames);
  NMP::Memory::Resource memResWS = NMPMemoryAllocateFromFormat(memReqsWS);
  ChannelSetTable* decompChanSetsWS = AP::ChannelSetTable::init(memResWS, numRigChannels, numSectionFrames);

  //-----------------------
  // Iterate over the frames in the section
  for (uint32_t frame = 0; frame < numSectionFrames; ++frame)
  {
    recompose(frame, m_tmAccumulatorA, true, false);
    decompChanSetsWS->setQuatKeysAtFrame(frame, m_tmAccumulatorA->getQuatKeys());
    decompChanSetsWS->setPosKeysAtFrame(frame, m_tmAccumulatorA->getPosKeys());
  }

  decompChanSetsWS->writeTableByChannelSets(filePointer);

  NMP::Memory::memFree(decompChanSetsWS);
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionCompressorQSA::writeRecomposedDiffWS(FILE* filePointer)
{
  NMP_VERIFY(filePointer);
  fprintf(filePointer, "\n_______________ Recomposed Diff WS _______________\n");

  fprintf(filePointer, "Section start frame = %d\n", m_sectionStartFrame);
  fprintf(filePointer, "Section end frame = %d\n", m_sectionEndFrame);
  uint32_t numKnots = m_knots->getNumKnots();
  fprintf(filePointer, "Num knots = %d\n", numKnots);
  for (uint32_t i = 0; i < numKnots; ++i)
  {
    fprintf(filePointer, "%3.5f\n", m_knots->getKnot(i));
  }

  NMP_VERIFY(m_tmAccumulatorA);
  NMP_VERIFY(m_tmAccumulatorB);

  const MR::AnimRigDef* rig = m_manager->getConglomerateRig();
  NMP_VERIFY(rig);
  uint32_t numRigChannels = rig->getNumBones();
  const ChannelSetTable* channelSetsWS = m_manager->getChannelSetsWS();
  NMP_VERIFY(channelSetsWS);
  uint32_t numSectionFrames = getNumSectionFrames();
  uint32_t animFrame;
  NMP::Vector3 posKey;
  NMP::Quat quatKey;
  NMP::Quat maxAbsQuat;
  NMP::Vector3 maxAbsPos;

  NMP::Memory::Format memReqsWS = AP::ChannelSetTable::getMemoryRequirements(numRigChannels, numSectionFrames);
  NMP::Memory::Resource memResWS = NMPMemoryAllocateFromFormat(memReqsWS);
  ChannelSetTable* decompChanSetsWS = AP::ChannelSetTable::init(memResWS, numRigChannels, numSectionFrames);

  //-----------------------
  // Iterate over the frames in the section
  for (uint32_t frame = 0; frame < numSectionFrames; ++frame)
  {
    // Compute the animation frame
    animFrame = m_sectionStartFrame + frame;

    fprintf(filePointer, "Anim frame = %d\n", animFrame);

    //-----------------------
    // Recover the worldspace channel set transforms for the uncompressed data
    for (uint32_t channel = 0; channel < numRigChannels; ++channel)
    {
      channelSetsWS->getQuatKey(channel, animFrame, quatKey);
      channelSetsWS->getPosKey(channel, animFrame, posKey);
      m_tmAccumulatorB->setChannelQuat(channel, quatKey, TransformsAccumulator::World);
      m_tmAccumulatorB->setChannelPos(channel, posKey, TransformsAccumulator::World);
    }

    // Recompose the worldspace transforms
    recompose(frame, m_tmAccumulatorA, true, false);

    // Compute the difference between transforms
    m_tmAccumulatorA->diff(*m_tmAccumulatorB);
    decompChanSetsWS->setQuatKeysAtFrame(frame, m_tmAccumulatorA->getQuatKeys());
    decompChanSetsWS->setPosKeysAtFrame(frame, m_tmAccumulatorA->getPosKeys());

    // Compute the maximum difference over channels
    m_tmAccumulatorA->maxAbsQuatElements(maxAbsQuat);
    m_tmAccumulatorA->maxAbsPosElements(maxAbsPos);
    fprintf(filePointer, "Max abs quat = [%3.5f, %3.5f, %3.5f, %3.5f]\n",
            maxAbsQuat.x, maxAbsQuat.y, maxAbsQuat.z, maxAbsQuat.w);
    fprintf(filePointer, "Max abs pos = [%3.5f, %3.5f, %3.5f]\n",
            maxAbsPos.x, maxAbsPos.y, maxAbsPos.z);
  }

  // Write out the difference table
  decompChanSetsWS->writeTableByChannelSets(filePointer);

  NMP::Memory::memFree(decompChanSetsWS);
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionCompressorQSA::buildSection()
{
  //-----------------------
  // Information
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = m_manager->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t sampledPosNumChans = channelSetInfoCompressor->getSampledPosNumChans();
  uint32_t sampledQuatNumChans = channelSetInfoCompressor->getSampledQuatNumChans();
  uint32_t splinePosNumChans = channelSetInfoCompressor->getSplinePosNumChans();
  uint32_t splineQuatNumChans = channelSetInfoCompressor->getSplineQuatNumChans();
  uint32_t splineNumKnots = m_knots->getNumKnots();
  uint32_t numSectionFrames = getNumSectionFrames();

  uint32_t sampledPosByteStride = 0;
  uint32_t sampledQuatByteStride = 0;
  uint32_t splinePosByteStride = 0;
  uint32_t splineQuatWNegsByteStride = 0;
  uint32_t splineQuatKeysByteStride = 0;
  uint32_t splineQuatTangentsByteStride = 0;

  uint32_t sampledPosNumQuantisationSets = 0;
  uint32_t sampledQuatNumQuantisationSets = 0;
  uint32_t splinePosNumQuantisationSets = 0;
  uint32_t splineQuatKeysNumQuantisationSets = 0;
  uint32_t splineQuatTangentsNumQuantisationSets = 0;

  //-----------------------
  m_animSectionHdrSize = NMP::Memory::align(sizeof(MR::AnimSectionQSA), NMP_NATURAL_TYPE_ALIGNMENT);
  m_sectionDataSize = m_animSectionHdrSize;

  //-----------------------
  if (m_sampledPosCompressor)
  {
    sampledPosByteStride = m_sampledPosCompressor->getSampledPosByteStride();
    sampledPosNumQuantisationSets = m_sampledPosCompressor->getSampledPosNumQuantisationSets();

    m_sampledPosQuantisationInfoSize = m_sampledPosCompressor->getSampledPosQuantisationInfoSize();
    m_sampledPosQuantisationDataSize = m_sampledPosCompressor->getSampledPosQuantisationDataSize();
    m_sampledPosDataSize = m_sampledPosCompressor->getSampledPosDataSize(); // Byte boundary aligned
    m_sampledPosDataSize = NMP::Memory::align(m_sampledPosDataSize, NMP_NATURAL_TYPE_ALIGNMENT);

    // Update actual section data size
    NMP_VERIFY(NMP_IS_ALIGNED((void*)m_sampledPosQuantisationInfoSize, NMP_NATURAL_TYPE_ALIGNMENT));
    NMP_VERIFY(NMP_IS_ALIGNED((void*)m_sampledPosQuantisationDataSize, NMP_NATURAL_TYPE_ALIGNMENT));
    NMP_VERIFY(NMP_IS_ALIGNED((void*)m_sampledPosDataSize, NMP_NATURAL_TYPE_ALIGNMENT));
    m_sectionDataSize += m_sampledPosQuantisationInfoSize;
    m_sectionDataSize += m_sampledPosQuantisationDataSize;
    m_sectionDataSize += m_sampledPosDataSize;
  }
  else
  {
    m_sampledPosQuantisationInfoSize = 0;
    m_sampledPosQuantisationDataSize = 0;
    m_sampledPosDataSize = 0;
  }

  //-----------------------
  if (m_sampledQuatCompressor)
  {
    sampledQuatByteStride = m_sampledQuatCompressor->getSampledQuatByteStride();
    sampledQuatNumQuantisationSets = m_sampledQuatCompressor->getSampledQuatNumQuantisationSets();

    m_sampledQuatQuantisationInfoSize = m_sampledQuatCompressor->getSampledQuatQuantisationInfoSize();
    m_sampledQuatQuantisationDataSize = m_sampledQuatCompressor->getSampledQuatQuantisationDataSize();
    m_sampledQuatDataSize = m_sampledQuatCompressor->getSampledQuatDataSize(); // Byte boundary aligned
    m_sampledQuatDataSize = NMP::Memory::align(m_sampledQuatDataSize, NMP_NATURAL_TYPE_ALIGNMENT);

    // Update actual section data size
    NMP_VERIFY(NMP_IS_ALIGNED((void*)m_sampledQuatQuantisationInfoSize, NMP_NATURAL_TYPE_ALIGNMENT));
    NMP_VERIFY(NMP_IS_ALIGNED((void*)m_sampledQuatQuantisationDataSize, NMP_NATURAL_TYPE_ALIGNMENT));
    NMP_VERIFY(NMP_IS_ALIGNED((void*)m_sampledQuatDataSize, NMP_NATURAL_TYPE_ALIGNMENT));
    m_sectionDataSize += m_sampledQuatQuantisationInfoSize;
    m_sectionDataSize += m_sampledQuatQuantisationDataSize;
    m_sectionDataSize += m_sampledQuatDataSize;
  }
  else
  {
    m_sampledQuatQuantisationInfoSize = 0;
    m_sampledQuatQuantisationDataSize = 0;
    m_sampledQuatDataSize = 0;
  }

  //-----------------------
  if (splineNumKnots > 0)
  {
    m_splineKnotsSize = NMP::Memory::align(sizeof(uint16_t) * splineNumKnots, NMP_NATURAL_TYPE_ALIGNMENT);

    // Update actual section data size
    m_sectionDataSize += m_splineKnotsSize;
  }
  else
  {
    m_splineKnotsSize = 0;
  }

  //-----------------------
  if (m_splinePosCompressor)
  {
    splinePosByteStride = m_splinePosCompressor->getSplinePosByteStride();
    splinePosNumQuantisationSets = m_splinePosCompressor->getSplinePosNumQuantisationSets();

    m_splinePosQuantisationInfoSize = m_splinePosCompressor->getSplinePosQuantisationInfoSize();
    m_splinePosQuantisationDataSize = m_splinePosCompressor->getSplinePosQuantisationDataSize();
    m_splinePosDataSize = m_splinePosCompressor->getSplinePosDataSize(); // Byte boundary aligned
    m_splinePosDataSize = NMP::Memory::align(m_splinePosDataSize, NMP_NATURAL_TYPE_ALIGNMENT);

    // Update actual section data size
    NMP_VERIFY(NMP_IS_ALIGNED((void*)m_splinePosQuantisationInfoSize, NMP_NATURAL_TYPE_ALIGNMENT));
    NMP_VERIFY(NMP_IS_ALIGNED((void*)m_splinePosQuantisationDataSize, NMP_NATURAL_TYPE_ALIGNMENT));
    NMP_VERIFY(NMP_IS_ALIGNED((void*)m_splinePosDataSize, NMP_NATURAL_TYPE_ALIGNMENT));
    m_sectionDataSize += m_splinePosQuantisationInfoSize;
    m_sectionDataSize += m_splinePosQuantisationDataSize;
    m_sectionDataSize += m_splinePosDataSize;
  }
  else
  {
    m_splinePosQuantisationInfoSize = 0;
    m_splinePosQuantisationDataSize = 0;
    m_splinePosDataSize = 0;
  }

  //-----------------------
  if (m_splineQuatCompressor)
  {
    splineQuatWNegsByteStride = m_splineQuatCompressor->getSplineQuatWNegsByteStride();
    splineQuatKeysByteStride = m_splineQuatCompressor->getSplineQuatKeysByteStride();
    splineQuatTangentsByteStride = m_splineQuatCompressor->getSplineQuatTangentsByteStride();
    splineQuatKeysNumQuantisationSets = m_splineQuatCompressor->getSplineQuatKeysNumQuantisationSets();
    splineQuatTangentsNumQuantisationSets = m_splineQuatCompressor->getSplineQuatTangentsNumQuantisationSets();

    m_splineQuatKeysQuantisationInfoSize = m_splineQuatCompressor->getSplineQuatKeysQuantisationInfoSize();
    m_splineQuatKeysQuantisationDataSize = m_splineQuatCompressor->getSplineQuatKeysQuantisationDataSize();
    m_splineQuatKeysWNegsDataSize = m_splineQuatCompressor->getSplineQuatWNegsDataSize(); // Byte boundary aligned
    m_splineQuatKeysWNegsDataSize = NMP::Memory::align(m_splineQuatKeysWNegsDataSize, NMP_NATURAL_TYPE_ALIGNMENT);
    m_splineQuatKeysDataSize = m_splineQuatCompressor->getSplineQuatKeysDataSize(); // Byte boundary aligned
    m_splineQuatKeysDataSize = NMP::Memory::align(m_splineQuatKeysDataSize, NMP_NATURAL_TYPE_ALIGNMENT);

    m_splineQuatTangentsQuantisationInfoSize = m_splineQuatCompressor->getSplineQuatTangentsQuantisationInfoSize();
    m_splineQuatTangentsQuantisationDataSize = m_splineQuatCompressor->getSplineQuatTangentsQuantisationDataSize();
    m_splineQuatTangentsDataSize = m_splineQuatCompressor->getSplineQuatTangentsDataSize(); // Byte boundary aligned
    m_splineQuatTangentsDataSize = NMP::Memory::align(m_splineQuatTangentsDataSize, NMP_NATURAL_TYPE_ALIGNMENT);

    // Update actual section data size
    NMP_VERIFY(NMP_IS_ALIGNED((void*)m_splineQuatKeysQuantisationInfoSize, NMP_NATURAL_TYPE_ALIGNMENT));
    NMP_VERIFY(NMP_IS_ALIGNED((void*)m_splineQuatKeysQuantisationDataSize, NMP_NATURAL_TYPE_ALIGNMENT));
    NMP_VERIFY(NMP_IS_ALIGNED((void*)m_splineQuatKeysWNegsDataSize, NMP_NATURAL_TYPE_ALIGNMENT));
    NMP_VERIFY(NMP_IS_ALIGNED((void*)m_splineQuatKeysDataSize, NMP_NATURAL_TYPE_ALIGNMENT));
    NMP_VERIFY(NMP_IS_ALIGNED((void*)m_splineQuatTangentsQuantisationInfoSize, NMP_NATURAL_TYPE_ALIGNMENT));
    NMP_VERIFY(NMP_IS_ALIGNED((void*)m_splineQuatTangentsQuantisationDataSize, NMP_NATURAL_TYPE_ALIGNMENT));
    NMP_VERIFY(NMP_IS_ALIGNED((void*)m_splineQuatTangentsDataSize, NMP_NATURAL_TYPE_ALIGNMENT));
    m_sectionDataSize += m_splineQuatKeysQuantisationInfoSize;
    m_sectionDataSize += m_splineQuatKeysQuantisationDataSize;
    m_sectionDataSize += m_splineQuatKeysWNegsDataSize;
    m_sectionDataSize += m_splineQuatKeysDataSize;
    m_sectionDataSize += m_splineQuatTangentsQuantisationInfoSize;
    m_sectionDataSize += m_splineQuatTangentsQuantisationDataSize;
    m_sectionDataSize += m_splineQuatTangentsDataSize;
  }
  else
  {
    m_splineQuatKeysQuantisationInfoSize = 0;
    m_splineQuatKeysQuantisationDataSize = 0;
    m_splineQuatKeysWNegsDataSize = 0;
    m_splineQuatKeysDataSize = 0;
    m_splineQuatTangentsQuantisationInfoSize = 0;
    m_splineQuatTangentsQuantisationDataSize = 0;
    m_splineQuatTangentsDataSize = 0;
  }

  //-----------------------
  // align the section data to DMA
  m_sectionDataSize = NMP::Memory::align(m_sectionDataSize, NMP_VECTOR_ALIGNMENT);
  NMP_VERIFY(m_sectionDataSize <= m_sectionSize);

  //-----------------------
  // Allocate the memory for the section data
  NMP::Memory::Format memReqs(m_sectionDataSize, NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  m_sectionData = (MR::AnimSectionQSA*)memRes.ptr;

  //-----------------------
  // Header
  AnimSectionQSABuilder* sectionBuilder = (AnimSectionQSABuilder*)m_sectionData;
  sectionBuilder->m_sectionSize = (uint16_t)m_sectionDataSize;
  sectionBuilder->m_numChannelSets = (uint16_t)m_numChannelSets;
  sectionBuilder->m_sectionAnimStartFrame = (uint16_t)m_sectionStartFrame;
  sectionBuilder->m_sectionNumAnimFrames = (uint16_t)numSectionFrames;

  sectionBuilder->m_sampledPosNumChannels = (uint16_t)sampledPosNumChans;
  sectionBuilder->m_sampledQuatNumChannels = (uint16_t)sampledQuatNumChans;

  sectionBuilder->m_splineNumKnots = (uint16_t)splineNumKnots;
  sectionBuilder->m_splinePosNumChannels = (uint16_t)splinePosNumChans;
  sectionBuilder->m_splineQuatNumChannels = (uint16_t)splineQuatNumChans;

  sectionBuilder->m_sampledPosByteStride = (uint16_t)sampledPosByteStride;
  sectionBuilder->m_sampledQuatByteStride = (uint16_t)sampledQuatByteStride;
  sectionBuilder->m_splinePosByteStride = (uint16_t)splinePosByteStride;
  sectionBuilder->m_splineQuatWNegsByteStride = (uint16_t)splineQuatWNegsByteStride;
  sectionBuilder->m_splineQuatKeysByteStride = (uint16_t)splineQuatKeysByteStride;
  sectionBuilder->m_splineQuatTangentsByteStride = (uint16_t)splineQuatTangentsByteStride;

  sectionBuilder->m_sampledPosNumQuantisationSets = (uint16_t)sampledPosNumQuantisationSets;
  sectionBuilder->m_sampledQuatNumQuantisationSets = (uint16_t)sampledQuatNumQuantisationSets;
  sectionBuilder->m_splinePosNumQuantisationSets = (uint16_t)splinePosNumQuantisationSets;
  sectionBuilder->m_splineQuatKeysNumQuantisationSets = (uint16_t)splineQuatKeysNumQuantisationSets;
  sectionBuilder->m_splineQuatTangentsNumQuantisationSets = (uint16_t)splineQuatTangentsNumQuantisationSets;

  //-----------------------
  // Relocate the data pointers
  sectionBuilder->relocate();

  //-----------------------
  // Global mean info
  sectionBuilder->m_posMeansQuantisationInfo = m_posMeansQuantisationInfo;

  if (m_splineQuatCompressor)
  {
    const MR::QuantisationInfoQSA& splineQuatTangentMeansInfo = m_splineQuatCompressor->getSplineQuatTangentMeansInfo();
    sectionBuilder->m_splineQuatTangentMeansQuantisationInfo = splineQuatTangentMeansInfo;
  }
  else
  {
    sectionBuilder->m_splineQuatTangentMeansQuantisationInfo.clear();
  }

  //-----------------------
  // Sampled pos channel
  if (m_sampledPosCompressor)
  {
    NMP::Memory::memcpy(
      sectionBuilder->m_sampledPosQuantisationInfo,
      m_sampledPosCompressor->getSampledPosQuantisationInfo(),
      m_sampledPosQuantisationInfoSize);

    NMP::Memory::memcpy(
      sectionBuilder->m_sampledPosQuantisationData,
      m_sampledPosCompressor->getSampledPosQuantisationData(),
      m_sampledPosQuantisationDataSize);

    NMP::Memory::memcpy(
      sectionBuilder->m_sampledPosData,
      m_sampledPosCompressor->getSampledPosData(),
      m_sampledPosDataSize);
  }

  //-----------------------
  // Sampled quat channel
  if (m_sampledQuatCompressor)
  {
    NMP::Memory::memcpy(
      sectionBuilder->m_sampledQuatQuantisationInfo,
      m_sampledQuatCompressor->getSampledQuatQuantisationInfo(),
      m_sampledQuatQuantisationInfoSize);

    NMP::Memory::memcpy(
      sectionBuilder->m_sampledQuatQuantisationData,
      m_sampledQuatCompressor->getSampledQuatQuantisationData(),
      m_sampledQuatQuantisationDataSize);

    NMP::Memory::memcpy(
      sectionBuilder->m_sampledQuatData,
      m_sampledQuatCompressor->getSampledQuatData(),
      m_sampledQuatDataSize);
  }

  //-----------------------
  // Knot vector
  if (splineNumKnots > 0)
  {
    for (uint32_t i = 0; i < splineNumKnots; ++i)
      sectionBuilder->m_splineKnots[i] = (uint16_t)m_knots->getKnot(i);
  }

  //-----------------------
  // Spline pos channel
  if (m_splinePosCompressor)
  {
    NMP::Memory::memcpy(
      sectionBuilder->m_splinePosQuantisationInfo,
      m_splinePosCompressor->getSplinePosQuantisationInfo(),
      m_splinePosQuantisationInfoSize);

    NMP::Memory::memcpy(
      sectionBuilder->m_splinePosQuantisationData,
      m_splinePosCompressor->getSplinePosQuantisationData(),
      m_splinePosQuantisationDataSize);

    NMP::Memory::memcpy(
      sectionBuilder->m_splinePosData,
      m_splinePosCompressor->getSplinePosData(),
      m_splinePosDataSize);
  }

  //-----------------------
  // Spline quat channel
  if (m_splineQuatCompressor)
  {
    NMP::Memory::memcpy(
      sectionBuilder->m_splineQuatKeysQuantisationInfo,
      m_splineQuatCompressor->getSplineQuatKeysQuantisationInfo(),
      m_splineQuatKeysQuantisationInfoSize);

    NMP::Memory::memcpy(
      sectionBuilder->m_splineQuatKeysQuantisationData,
      m_splineQuatCompressor->getSplineQuatKeysQuantisationData(),
      m_splineQuatKeysQuantisationDataSize);

    NMP::Memory::memcpy(
      sectionBuilder->m_splineQuatWNegsData,
      m_splineQuatCompressor->getSplineQuatWNegsData(),
      m_splineQuatKeysWNegsDataSize);

    NMP::Memory::memcpy(
      sectionBuilder->m_splineQuatKeysData,
      m_splineQuatCompressor->getSplineQuatKeysData(),
      m_splineQuatKeysDataSize);

    NMP::Memory::memcpy(
      sectionBuilder->m_splineQuatTangentsQuantisationInfo,
      m_splineQuatCompressor->getSplineQuatTangentsQuantisationInfo(),
      m_splineQuatTangentsQuantisationInfoSize);

    NMP::Memory::memcpy(
      sectionBuilder->m_splineQuatTangentsQuantisationData,
      m_splineQuatCompressor->getSplineQuatTangentsQuantisationData(),
      m_splineQuatTangentsQuantisationDataSize);

    NMP::Memory::memcpy(
      sectionBuilder->m_splineQuatTangentsData,
      m_splineQuatCompressor->getSplineQuatTangentsData(),
      m_splineQuatTangentsDataSize);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionCompressorQSA::initQuantisation()
{
  //-----------------------
  // Information
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = m_manager->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  float quantisationSetQuality = m_manager->getQuantisationSetQuality();
  NMP_VERIFY(quantisationSetQuality >= 0.0f && quantisationSetQuality <= 1.0f);
  uint32_t numRigChannels = m_manager->getNumRigChannelSets();
  const uint32_t minNumQSets = 1;
  const uint32_t maxNumQSets = 16;

  uint32_t sampledPosNumChans = channelSetInfoCompressor->getSampledPosNumChans();
  uint32_t sampledQuatNumChans = channelSetInfoCompressor->getSampledQuatNumChans();
  uint32_t splinePosNumChans = channelSetInfoCompressor->getSplinePosNumChans();
  uint32_t splineQuatNumChans = channelSetInfoCompressor->getSplineQuatNumChans();

  //-----------------------
  // Set the default quantisation set weights
  for (uint32_t i = 0; i < CoefSetQSA::CoefSetNumTypes; ++i)
  {
    m_quantisationSetWeights[i] = 1.0f;
  }

  //-----------------------
  // Create the quantisation sets
  QuantisationSetQSA* qSet;
  uint32_t numCoefSets = 0;

  if (sampledPosNumChans > 0)
  {
    // Check if we are compressing with per channel quantisation sets
    uint32_t numQSets = (uint32_t)(quantisationSetQuality * sampledPosNumChans + 0.5f);
    uint32_t numSampledPosQuantisationSets = NMP::clampValue(numQSets, minNumQSets, maxNumQSets);

    // Allocate the quantisation sets
    for (uint32_t i = 0; i < numSampledPosQuantisationSets; ++i)
    {
      qSet = new QuantisationSetQSA;
      qSet->setQuantiserCoefSetIndex(numCoefSets);
      m_quantisationSets[CoefSetQSA::CoefSetSampledPos].push_back(qSet);
      numCoefSets += 3;
    }
  }

  if (sampledQuatNumChans > 0)
  {
    // Check if we are compressing with per channel quantisation sets
    uint32_t numQSets = (uint32_t)(quantisationSetQuality * sampledQuatNumChans + 0.5f);
    uint32_t numSampledQuatQuantisationSets = NMP::clampValue(numQSets, minNumQSets, maxNumQSets);

    // Allocate the quantisation sets
    for (uint32_t i = 0; i < numSampledQuatQuantisationSets; ++i)
    {
      qSet = new QuantisationSetQSA;
      qSet->setQuantiserCoefSetIndex(numCoefSets);
      m_quantisationSets[CoefSetQSA::CoefSetSampledQuat].push_back(qSet);
      numCoefSets += 3;
    }
  }

  if (splinePosNumChans > 0)
  {
    // Check if we are compressing with per channel quantisation sets
    uint32_t numQSets = (uint32_t)(quantisationSetQuality * splinePosNumChans + 0.5f);
    uint32_t numSplinePosQuantisationSets = NMP::clampValue(numQSets, minNumQSets, maxNumQSets);

    // Allocate the quantisation sets
    for (uint32_t i = 0; i < numSplinePosQuantisationSets; ++i)
    {
      qSet = new QuantisationSetQSA;
      qSet->setQuantiserCoefSetIndex(numCoefSets);
      m_quantisationSets[CoefSetQSA::CoefSetSplinePos].push_back(qSet);
      numCoefSets += 3;
    }
  }

  if (splineQuatNumChans > 0)
  {
    uint32_t numQSets = (uint32_t)(quantisationSetQuality * splineQuatNumChans + 0.5f);
    uint32_t numSplineQuatKeysQuantisationSets = NMP::clampValue(numQSets, minNumQSets, maxNumQSets);

    //-----------------------
    // Keys: Allocate the quantisation sets

    for (uint32_t i = 0; i < numSplineQuatKeysQuantisationSets; ++i)
    {
      qSet = new QuantisationSetQSA;
      qSet->setQuantiserCoefSetIndex(numCoefSets);
      m_quantisationSets[CoefSetQSA::CoefSetSplineQuatKey].push_back(qSet);
      numCoefSets += 3;
    }

    //-----------------------
    // Tangents: Allocate the quantisation sets
    for (uint32_t i = 0; i < numSplineQuatKeysQuantisationSets; ++i)
    {
      qSet = new QuantisationSetQSA;
      qSet->setQuantiserCoefSetIndex(numCoefSets);
      m_quantisationSets[CoefSetQSA::CoefSetSplineQuatTangent].push_back(qSet);
      numCoefSets += 3;
    }
  }

  //-----------------------
  // Create a new quantiser
  NMP_VERIFY(!m_quantiser);
  if (numCoefSets > 0)
    m_quantiser = new NMP::UniformQuantisation(numCoefSets);

  //-----------------------
  // Create the coefficient sets and attach them to the quantiser
  for (uint32_t setType = 0; setType < CoefSetQSA::CoefSetNumTypes; ++setType)
  {
    uint32_t numQuantisationSets = getNumQuantisationSets(setType);
    for (uint32_t qSetIndex = 0; qSetIndex < numQuantisationSets; ++qSetIndex)
    {
      QuantisationSetQSA* qSet = getQuantisationSet(setType, qSetIndex);
      NMP_VERIFY(qSet);
      uint32_t coefSetIndex = qSet->getQuantiserCoefSetIndex();

      CoefSetQSA* coefSetX = new CoefSetQSA(m_maxPrec);
      CoefSetQSA* coefSetY = new CoefSetQSA(m_maxPrec);
      CoefSetQSA* coefSetZ = new CoefSetQSA(m_maxPrec);

      // X
      coefSetX->clearRateAndDistortion();
      coefSetX->setUserID(setType);
      coefSetX->setUserValue(qSetIndex);
      coefSetX->setWeight(m_quantisationSetWeights[setType]);
      qSet->setCoefSetX(coefSetX);
      m_quantiser->setCoefficientSet(coefSetIndex, coefSetX);

      // Y
      coefSetY->clearRateAndDistortion();
      coefSetY->setUserID(setType);
      coefSetY->setUserValue(qSetIndex);
      coefSetY->setWeight(m_quantisationSetWeights[setType]);
      qSet->setCoefSetY(coefSetY);
      m_quantiser->setCoefficientSet(coefSetIndex + 1, coefSetY);

      // Z
      coefSetZ->clearRateAndDistortion();
      coefSetZ->setUserID(setType);
      coefSetZ->setUserValue(qSetIndex);
      coefSetZ->setWeight(m_quantisationSetWeights[setType]);
      qSet->setCoefSetZ(coefSetZ);
      m_quantiser->setCoefficientSet(coefSetIndex + 2, coefSetZ);
    }
  }

  //-----------------------
  // Create a new distance table for hierarchy accumulation
  NMP_VERIFY(!m_hierarchyDists);
  NMP::Memory::Format memReqsTMDists(0, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqsTMDists.size = NMP::Memory::align(sizeof(uint32_t) * numRigChannels, NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::Resource memResTMDists = NMPMemoryAllocateFromFormat(memReqsTMDists);
  m_hierarchyDists = (uint32_t*)memResTMDists.ptr;

  //-----------------------
  // Create a new transforms accumulator
  NMP_VERIFY(!m_tmAccumulatorA);
  NMP_VERIFY(!m_tmAccumulatorB);
  NMP::Memory::Format memReqsTMAcc = AP::TransformsAccumulator::getMemoryRequirements(numRigChannels);
  NMP::Memory::Resource memResTMAccA = NMPMemoryAllocateFromFormat(memReqsTMAcc);
  m_tmAccumulatorA = AP::TransformsAccumulator::init(memResTMAccA, numRigChannels);
  NMP::Memory::Resource memResTMAccB = NMPMemoryAllocateFromFormat(memReqsTMAcc);
  m_tmAccumulatorB = AP::TransformsAccumulator::init(memResTMAccB, numRigChannels);
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionCompressorQSA::termQuantisation()
{
  //-----------------------
  // Uniform quantisation
  if (m_quantiser)
  {
    delete m_quantiser;
    m_quantiser = NULL;
  }

  //-----------------------
  // Delete the quantisation sets
  for (uint32_t i = 0; i < CoefSetQSA::CoefSetNumTypes; ++i)
  {
    std::vector<QuantisationSetQSA*>::iterator it = m_quantisationSets[i].begin();
    std::vector<QuantisationSetQSA*>::iterator it_end = m_quantisationSets[i].end();
    for (; it != it_end; ++it)
    {
      QuantisationSetQSA* qSet = *it;
      NMP_VERIFY(qSet);
      delete qSet;
    }
    m_quantisationSets[i].clear();
  }

  //-----------------------
  if (m_hierarchyDists)
  {
    NMP::Memory::memFree(m_hierarchyDists);
    m_hierarchyDists = NULL;
  }

  //-----------------------
  uint32_t numBonePosTables = (uint32_t)m_bonePosTables.size();
  for (uint32_t i = 0; i < numBonePosTables; ++i)
  {
    Vector3Table* bonePosTable = m_bonePosTables[i];
    if (bonePosTable)
      NMP::Memory::memFree(bonePosTable);
  }
  m_bonePosTables.clear();

  //-----------------------
  if (m_tmAccumulatorA)
  {
    NMP::Memory::memFree(m_tmAccumulatorA);
    m_tmAccumulatorA = NULL;
  }

  if (m_tmAccumulatorB)
  {
    NMP::Memory::memFree(m_tmAccumulatorB);
    m_tmAccumulatorB = NULL;
  }

  //-----------------------
  if (m_bonePosWeights)
  {
    delete m_bonePosWeights;
    m_bonePosWeights = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionCompressorQSA::saveQuantisationState()
{
  // Knot vector
  NMP::SimpleKnotVector* knotsTemp;
  SplinePosCompressorQSA* splinePosCompressorTemp;
  SplineQuatCompressorQSA* splineQuatCompressorTemp;

  // Knot vector
  knotsTemp = m_knots;
  m_knots = m_knotsSaved;
  m_knotsSaved = knotsTemp;

  // Pos spline compressor
  splinePosCompressorTemp = m_splinePosCompressor;
  m_splinePosCompressor = m_splinePosCompressorSaved;
  m_splinePosCompressorSaved = splinePosCompressorTemp;

  // Quat spline compressor
  splineQuatCompressorTemp = m_splineQuatCompressor;
  m_splineQuatCompressor = m_splineQuatCompressorSaved;
  m_splineQuatCompressorSaved = splineQuatCompressorTemp;

  // Quantiser information
  m_quantiser->saveInfoState();
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionCompressorQSA::restoreQuantisationState()
{
  // Knot vector
  NMP::SimpleKnotVector* knotsTemp;
  SplinePosCompressorQSA* splinePosCompressorTemp;
  SplineQuatCompressorQSA* splineQuatCompressorTemp;

  // Knot vector
  knotsTemp = m_knots;
  m_knots = m_knotsSaved;
  m_knotsSaved = knotsTemp;

  // Pos spline compressor
  splinePosCompressorTemp = m_splinePosCompressor;
  m_splinePosCompressor = m_splinePosCompressorSaved;
  m_splinePosCompressorSaved = splinePosCompressorTemp;

  // Quat spline compressor
  splineQuatCompressorTemp = m_splineQuatCompressor;
  m_splineQuatCompressor = m_splineQuatCompressorSaved;
  m_splineQuatCompressorSaved = splineQuatCompressorTemp;

  // Quantiser information
  m_quantiser->restoreInfoState();
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AnimSectionCompressorQSA::getNumQuantisationSets(uint32_t setType) const
{
  NMP_VERIFY(setType < CoefSetQSA::CoefSetNumTypes);
  const std::vector<QuantisationSetQSA*>& qSets = m_quantisationSets[setType];
  return (uint32_t)qSets.size();
}

//----------------------------------------------------------------------------------------------------------------------
float AnimSectionCompressorQSA::getQuantisationSetWeight(uint32_t setType) const
{
  NMP_VERIFY(setType < CoefSetQSA::CoefSetNumTypes);
  return m_quantisationSetWeights[setType];
}

//----------------------------------------------------------------------------------------------------------------------
QuantisationSetQSA* AnimSectionCompressorQSA::getQuantisationSet(uint32_t setType, uint32_t qSetIndex)
{
  NMP_VERIFY(setType < CoefSetQSA::CoefSetNumTypes);

  std::vector<QuantisationSetQSA*>& qSets = m_quantisationSets[setType];
  uint32_t numQuantisationSets = (uint32_t)qSets.size();
  if (qSetIndex < numQuantisationSets)
    return qSets[qSetIndex];

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionCompressorQSA::clearRateAndDistortion(uint32_t setType)
{
  NMP_VERIFY(setType < CoefSetQSA::CoefSetNumTypes);
  std::vector<QuantisationSetQSA*>& qSets = m_quantisationSets[setType];
  uint32_t numQuantisationSets = (uint32_t)qSets.size();

  QuantisationSetQSA* qSet;
  CoefSetQSA* coefSetX;
  CoefSetQSA* coefSetY;
  CoefSetQSA* coefSetZ;

  for (uint32_t qSetIndex = 0; qSetIndex < numQuantisationSets; ++qSetIndex)
  {
    qSet = qSets[qSetIndex];
    NMP_VERIFY(qSet);
    coefSetX = qSet->getCoefSetX();
    NMP_VERIFY(coefSetX);
    coefSetX->clearRateAndDistortion();

    qSet = qSets[qSetIndex];
    NMP_VERIFY(qSet);
    coefSetY = qSet->getCoefSetY();
    NMP_VERIFY(coefSetY);
    coefSetY->clearRateAndDistortion();

    qSet = qSets[qSetIndex];
    NMP_VERIFY(qSet);
    coefSetZ = qSet->getCoefSetZ();
    NMP_VERIFY(coefSetZ);
    coefSetZ->clearRateAndDistortion();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionCompressorQSA::clearRateAndDelta(uint32_t setType)
{
  NMP_VERIFY(setType < CoefSetQSA::CoefSetNumTypes);
  std::vector<QuantisationSetQSA*>& qSets = m_quantisationSets[setType];
  uint32_t numQuantisationSets = (uint32_t)qSets.size();

  QuantisationSetQSA* qSet;
  CoefSetQSA* coefSetX;
  CoefSetQSA* coefSetY;
  CoefSetQSA* coefSetZ;

  for (uint32_t qSetIndex = 0; qSetIndex < numQuantisationSets; ++qSetIndex)
  {
    qSet = qSets[qSetIndex];
    NMP_VERIFY(qSet);
    coefSetX = qSet->getCoefSetX();
    NMP_VERIFY(coefSetX);
    coefSetX->clearRateAndDelta();

    qSet = qSets[qSetIndex];
    NMP_VERIFY(qSet);
    coefSetY = qSet->getCoefSetY();
    NMP_VERIFY(coefSetY);
    coefSetY->clearRateAndDelta();

    qSet = qSets[qSetIndex];
    NMP_VERIFY(qSet);
    coefSetZ = qSet->getCoefSetZ();
    NMP_VERIFY(coefSetZ);
    coefSetZ->clearRateAndDelta();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionCompressorQSA::makeSimpleDeltaToDistWeightMap(uint32_t setType)
{
  //-----------------------
  // Information
  NMP_VERIFY(setType < CoefSetQSA::CoefSetNumTypes);
  std::vector<QuantisationSetQSA*>& qSets = m_quantisationSets[setType];
  uint32_t numQuantisationSets = (uint32_t)qSets.size();

  //-----------------------
  // Compute the delta to dist weight mappings
  for (uint32_t qSetIndex = 0; qSetIndex < numQuantisationSets; ++qSetIndex)
  {
    QuantisationSetQSA* qSet = qSets[qSetIndex];
    NMP_VERIFY(qSet);
    CoefSetQSA* coefSetX = qSet->getCoefSetX();
    NMP_VERIFY(coefSetX);
    CoefSetQSA* coefSetY = qSet->getCoefSetY();
    NMP_VERIFY(coefSetY);
    CoefSetQSA* coefSetZ = qSet->getCoefSetZ();
    NMP_VERIFY(coefSetZ);
    coefSetX->setDeltaToDistWeightMap(1.0f);
    coefSetY->setDeltaToDistWeightMap(1.0f);
    coefSetZ->setDeltaToDistWeightMap(1.0f);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionCompressorQSA::makeDeltaToDistWeightMap(uint32_t setType)
{
  //-----------------------
  // Information
  NMP_VERIFY(setType < CoefSetQSA::CoefSetNumTypes);
  std::vector<QuantisationSetQSA*>& qSets = m_quantisationSets[setType];
  uint32_t numQuantisationSets = (uint32_t)qSets.size();

  //-----------------------
  // Allocate the coefficient sets for the average distortions. We need to do this
  // since some quantisation sets may correspond to channels that have no child bones,
  // thus no distortions for this coefficient set can be computed
  CoefSetQSA* coefSetXav = new CoefSetQSA(m_maxPrec);
  CoefSetQSA* coefSetYav = new CoefSetQSA(m_maxPrec);
  CoefSetQSA* coefSetZav = new CoefSetQSA(m_maxPrec);

  //-----------------------
  // Compute the delta to dist weight mappings
  coefSetXav->clearRateAndDistortion();
  coefSetYav->clearRateAndDistortion();
  coefSetZav->clearRateAndDistortion();
  for (uint32_t qSetIndex = 0; qSetIndex < numQuantisationSets; ++qSetIndex)
  {
    QuantisationSetQSA* qSet = qSets[qSetIndex];
    NMP_VERIFY(qSet);
    CoefSetQSA* coefSetX = qSet->getCoefSetX();
    NMP_VERIFY(coefSetX);
    CoefSetQSA* coefSetY = qSet->getCoefSetY();
    NMP_VERIFY(coefSetY);
    CoefSetQSA* coefSetZ = qSet->getCoefSetZ();
    NMP_VERIFY(coefSetZ);

    if (coefSetX->makeDeltaToDistWeightMap())
      coefSetXav->updateDeltaToDistWeightMapAverage(*coefSetX);
    if (coefSetY->makeDeltaToDistWeightMap())
      coefSetYav->updateDeltaToDistWeightMapAverage(*coefSetY);
    if (coefSetZ->makeDeltaToDistWeightMap())
      coefSetZav->updateDeltaToDistWeightMapAverage(*coefSetZ);
  }
  coefSetXav->computeDeltaToDistWeightMapAverage();
  coefSetYav->computeDeltaToDistWeightMapAverage();
  coefSetZav->computeDeltaToDistWeightMapAverage();

  //-----------------------
  // Set any uncomputed delta to dist weight maps to the average
  for (uint32_t qSetIndex = 0; qSetIndex < numQuantisationSets; ++qSetIndex)
  {
    QuantisationSetQSA* qSet = qSets[qSetIndex];
    CoefSetQSA* coefSetX = qSet->getCoefSetX();
    CoefSetQSA* coefSetY = qSet->getCoefSetY();
    CoefSetQSA* coefSetZ = qSet->getCoefSetZ();

    if (!coefSetX->hasValidDeltaToDistWeightMap())
      coefSetX->setDeltaToDistWeightMap(*coefSetXav);
    if (!coefSetY->hasValidDeltaToDistWeightMap())
      coefSetY->setDeltaToDistWeightMap(*coefSetYav);
    if (!coefSetZ->hasValidDeltaToDistWeightMap())
      coefSetZ->setDeltaToDistWeightMap(*coefSetZav);
  }

  //-----------------------
  // Tidy up
  delete coefSetXav;
  delete coefSetYav;
  delete coefSetZav;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionCompressorQSA::computeRateAndDistortion(uint32_t setType)
{
  NMP_VERIFY(setType < CoefSetQSA::CoefSetNumTypes);
  std::vector<QuantisationSetQSA*>& qSets = m_quantisationSets[setType];
  uint32_t numQuantisationSets = (uint32_t)qSets.size();

  QuantisationSetQSA* qSet;
  CoefSetQSA* coefSetX;
  CoefSetQSA* coefSetY;
  CoefSetQSA* coefSetZ;

  for (uint32_t qSetIndex = 0; qSetIndex < numQuantisationSets; ++qSetIndex)
  {
    qSet = qSets[qSetIndex];
    NMP_VERIFY(qSet);
    coefSetX = qSet->getCoefSetX();
    NMP_VERIFY(coefSetX);
    coefSetX->computeRateAndDistortion();

    qSet = qSets[qSetIndex];
    NMP_VERIFY(qSet);
    coefSetY = qSet->getCoefSetY();
    NMP_VERIFY(coefSetY);
    coefSetY->computeRateAndDistortion();

    qSet = qSets[qSetIndex];
    NMP_VERIFY(qSet);
    coefSetZ = qSet->getCoefSetZ();
    NMP_VERIFY(coefSetZ);
    coefSetZ->computeRateAndDistortion();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionCompressorQSA::computeBonePosWeights()
{
  //-----------------------
  // Information
  NMP_VERIFY(m_manager);
  const AnimSourceMotionAnalyser* motionAnalyser = m_manager->getMotionAnalyser();
  NMP_VERIFY(motionAnalyser);
  uint32_t numRigBones = m_manager->getNumRigChannelSets();
  uint32_t numSectionFrames = getNumSectionFrames();
  float averageBoneLength = motionAnalyser->getAverageBoneLength();

  // Compute the normalisation weight factor for error analysis
  float w = 1000.0f;
  if (averageBoneLength > MR::ERROR_LIMIT)
    w /= averageBoneLength;

  //-----------------------
  // Create the bone pos displacement weight matrix (frames by rig chans)
  m_bonePosWeights = new NMP::Matrix<float>(numSectionFrames, numRigBones);
  m_bonePosWeights->set(w);
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionCompressorQSA::computeBonePosTables()
{
  //-----------------------
  // Information
  NMP_VERIFY(m_manager);
  NMP_VERIFY(m_manager->getUseWorldSpace());
  const AnimSourceUncompressed* inputAnim = m_manager->getInputAnimation();
  NMP_VERIFY(inputAnim);
  const ChannelSetRequirements* channelSetReqs = inputAnim->getChannelSetRequirements();
  NMP_VERIFY(channelSetReqs);
  const MR::RigToAnimEntryMap* rigToAnimEntryMap = m_manager->getConglomerateRigToAnimEntryMap();
  NMP_VERIFY(rigToAnimEntryMap);
  const MR::AnimRigDef* rig = m_manager->getConglomerateRig();
  NMP_VERIFY(rig);
  const NMP::Hierarchy* hierarchy = rig->getHierarchy();
  NMP_VERIFY(hierarchy);
  uint32_t numRigBones = hierarchy->getNumEntries();
  const ChannelSetTable* channelSetsWS = m_manager->getChannelSetsWS();
  NMP_VERIFY(channelSetsWS);
  uint32_t numSectionFrames = getNumSectionFrames();
  NMP_VERIFY(m_bonePosWeights);

  NMP::Quat invRigChanQuat;
  NMP::Vector3 rigChanPos;
  NMP::Vector3 posRel;
  Vector3Table* bonePosTable;
  const uint32_t maxDepth = 3;

  //-----------------------
  // Create the bone pos tables array
  NMP_VERIFY(m_bonePosTables.empty());
  m_bonePosTables.resize(m_numChannelSets);
  for (uint32_t i = 0; i < m_numChannelSets; ++i)
    m_bonePosTables[i] = NULL;

  //-----------------------
  // Create the bone pos tables
  for (uint32_t animChannelIndex = 0; animChannelIndex < m_numChannelSets; ++animChannelIndex)
  {
    // Check if this is an unchanging quat channel
    if (channelSetReqs->isChannelQuatUnchanging(animChannelIndex))
      continue;

    // Get the rig bone index for this animation channel
    uint16_t rigChannelIndex = 0;
#if defined(NMP_ENABLE_ASSERTS) || NM_ENABLE_EXCEPTIONS
    bool boneInAnim = 
#endif
      rigToAnimEntryMap->getRigIndexForAnimIndex((uint16_t)animChannelIndex, rigChannelIndex);
    NMP_VERIFY(boneInAnim); // Must be in the map

    //-----------------------
    // Compute the number of child rig bones that are siblings of this rig channel
    m_hierarchyDists[rigChannelIndex] = 0;
    uint32_t numBonePosChildren = 0;
    for (uint32_t rigID = rigChannelIndex + 1; rigID < numRigBones; ++rigID)
    {
      // Check if we've finished traversing all child bones of the current channel
      uint32_t parentID = hierarchy->getParentIndex(rigID);
      NMP_VERIFY(parentID != INVALID_HIERARCHY_INDEX);
      if (parentID < (uint32_t)rigChannelIndex)
        break;

      // Find out the depth of the child bone
      uint32_t depth = m_hierarchyDists[parentID] + 1;
      m_hierarchyDists[rigID] = depth;
      if (depth <= maxDepth)
        numBonePosChildren++;
    }

    //-----------------------
    // Compute the bone pos table for the current rig channel
    if (numBonePosChildren > 0)
    {
      // Get worldspace transforms for the current rig channel
      const NMP::Quat* quatKeys = channelSetsWS->getChannelQuat(rigChannelIndex);
      NMP_VERIFY(quatKeys);
      const NMP::Quat* sectionQuatKeys = &quatKeys[m_sectionStartFrame];
      const NMP::Vector3* posKeys = channelSetsWS->getChannelPos(rigChannelIndex);
      NMP_VERIFY(posKeys);
      const NMP::Vector3* sectionPosKeys = &posKeys[m_sectionStartFrame];

      // Create a new bone pos table
      NMP::Memory::Format memReqs =
        AP::Vector3Table::getMemoryRequirements(numBonePosChildren, numSectionFrames);
      NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
      bonePosTable = AP::Vector3Table::init(memRes, numBonePosChildren, numSectionFrames);
      m_bonePosTables[animChannelIndex] = bonePosTable;

      // Iterate over the section frames
      for (uint32_t frame = 0; frame < numSectionFrames; ++frame)
      {
        // Compute the inverse transform of the current rig channel: t'_i = R_p^T *(t_i - t_p)
        invRigChanQuat = ~sectionQuatKeys[frame];
        rigChanPos = sectionPosKeys[frame];

        // Iterate over the child bones
        m_hierarchyDists[rigChannelIndex] = 0;
        uint32_t bonePosIndex = 0;
        for (uint32_t rigID = rigChannelIndex + 1; rigID < numRigBones; ++rigID)
        {
          // Check if we've finished traversing all child bones of the current channel
          uint32_t parentID = hierarchy->getParentIndex(rigID);
          NMP_VERIFY(parentID != INVALID_HIERARCHY_INDEX);
          if (parentID < (uint32_t)rigChannelIndex)
            break;

          // Find out the depth of the child bone
          uint32_t depth = m_hierarchyDists[parentID] + 1;
          m_hierarchyDists[rigID] = depth;
          if (depth <= maxDepth)
          {
            // Get worldspace transforms for the child rig channel
            const NMP::Vector3* posKeysChild = channelSetsWS->getChannelPos(rigID);
            NMP_VERIFY(posKeysChild);
            const NMP::Vector3* sectionPosKeysChild = &posKeysChild[m_sectionStartFrame];

            // Compute the child bone position relative to the current rig channel
            posRel = sectionPosKeysChild[frame] - rigChanPos;
            posRel = invRigChanQuat.rotateVector(posRel);

            // Set the w component with the bone pos weight factor
            posRel.w = m_bonePosWeights->element(frame, rigID);

            // Set the bone pos table entry
            bonePosTable->setKey(bonePosIndex, frame, posRel);

            // Increment the bone pos index
            bonePosIndex++;
          }
        }
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
float AnimSectionCompressorQSA::computeError(
  const ChannelSetTable* channelSets,
  bool useWorldSpace,
  bool generateDeltas)
{
  NMP_VERIFY(m_tmAccumulatorA);
  NMP_VERIFY(m_tmAccumulatorB);
  uint32_t numRigChannels = channelSets->getNumChannelSets();
  uint32_t numSectionFrames = getNumSectionFrames();

  uint32_t rigChannelIndex;
  NMP::Vector3 posKey;
  NMP::Quat quatKey;
  float totalError, frameError;
  uint32_t animFrame;
  NMP::Vector3 dx, posKeyA, posKeyB;
  NMP::Quat quatKeyA, quatKeyB;
  float dispWeight, dq;

  TransformsAccumulator::chanSpaceType spaceType;
  if (useWorldSpace)
  {
    NMP_VERIFY(m_bonePosWeights);
    spaceType = TransformsAccumulator::World;
  }
  else
  {
    spaceType = TransformsAccumulator::Local;
  }

  //-----------------------
  // Iterate over the frames in the section
  totalError = 0.0f;
  for (uint32_t frame = 0; frame < numSectionFrames; ++frame)
  {
    // Compute the animation frame
    animFrame = m_sectionStartFrame + frame;

    //-----------------------
    // Recover the worldspace channel set transforms for the uncompressed data
    for (uint32_t channel = 0; channel < numRigChannels; ++channel)
    {
      channelSets->getPosKey(channel, animFrame, posKey);
      m_tmAccumulatorB->setChannelPos(channel, posKey, spaceType);
    }

    //-----------------------
    // Recover the worldspace transforms at the current frame
    recompose(frame, m_tmAccumulatorA, useWorldSpace, generateDeltas);

    //-----------------------
    // Compute the error with uncompressed worldspace transforms
    if (useWorldSpace)
    {
      // Use position error for worldspace
      frameError = 0.0f;
      for (rigChannelIndex = 0; rigChannelIndex < numRigChannels; ++rigChannelIndex)
      {
        m_tmAccumulatorA->getChannelPos(rigChannelIndex, posKeyA);
        m_tmAccumulatorB->getChannelPos(rigChannelIndex, posKeyB);
        dispWeight = m_bonePosWeights->element(frame, rigChannelIndex);
        dx = posKeyA - posKeyB;
        dx *= dispWeight;

        frameError += dx.magnitudeSquared();
      }
    }
    else
    {
      // Use orientation error for localspace
      frameError = 0.0f;
      for (rigChannelIndex = 0; rigChannelIndex < numRigChannels; ++rigChannelIndex)
      {
        m_tmAccumulatorA->getChannelQuat(rigChannelIndex, quatKeyA);
        m_tmAccumulatorB->getChannelQuat(rigChannelIndex, quatKeyB);
        dq = NMP::nmfabs(quatKeyA.dot(quatKeyB));

        frameError += dq;
      }
    }

    // Update the total error
    totalError += frameError;
  }

  return totalError;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionCompressorQSA::recompose(
  uint32_t frame,
  TransformsAccumulator* tmAccumulator,
  bool useWorldSpace,
  bool generateDeltas) const
{
  NMP_VERIFY(tmAccumulator);
  const MR::AnimRigDef* rig = m_manager->getConglomerateRig();
  NMP_VERIFY(rig);
  const NMP::Hierarchy* hierarchy = rig->getHierarchy();
  NMP_VERIFY(hierarchy);
  const MR::RigToAnimEntryMap* rigToAnimEntryMap = m_manager->getConglomerateRigToAnimEntryMap();
  NMP_VERIFY(rigToAnimEntryMap);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = m_manager->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  const NMP::Vector3* unchangingPosQuantised = channelSetInfoCompressor->getUnchangingPosQuantised();
  const NMP::Quat* unchangingQuatQuantised = channelSetInfoCompressor->getUnchangingQuatQuantised();

  TransformsAccumulator::chanSpaceType spaceType;
  if (useWorldSpace)
    spaceType = TransformsAccumulator::World;
  else
    spaceType = TransformsAccumulator::Local;

  uint32_t unchangingPosNumChans = channelSetInfoCompressor->getUnchangingPosNumChans();
  uint32_t unchangingQuatNumChans = channelSetInfoCompressor->getUnchangingQuatNumChans();
  uint32_t sampledPosNumChans = channelSetInfoCompressor->getSampledPosNumChans();
  uint32_t sampledQuatNumChans = channelSetInfoCompressor->getSampledQuatNumChans();
  uint32_t splinePosNumChans = channelSetInfoCompressor->getSplinePosNumChans();
  uint32_t splineQuatNumChans = channelSetInfoCompressor->getSplineQuatNumChans();
  const std::vector<uint32_t>& unchangingPosCompToAnimMap = channelSetInfoCompressor->getUnchangingPosCompToAnimMap();
  const std::vector<uint32_t>& unchangingQuatCompToAnimMap = channelSetInfoCompressor->getUnchangingQuatCompToAnimMap();
  const std::vector<uint32_t>& sampledPosCompToAnimMap = channelSetInfoCompressor->getSampledPosCompToAnimMap();
  const std::vector<uint32_t>& sampledQuatCompToAnimMap = channelSetInfoCompressor->getSampledQuatCompToAnimMap();
  const std::vector<uint32_t>& splinePosCompToAnimMap = channelSetInfoCompressor->getSplinePosCompToAnimMap();
  const std::vector<uint32_t>& splineQuatCompToAnimMap = channelSetInfoCompressor->getSplineQuatCompToAnimMap();

  // Compute the animation frame
  uint32_t animFrame = m_sectionStartFrame + frame;

  // Check if there are any spline channels
  uint32_t span = 0;
  if (splinePosNumChans > 0 || splineQuatNumChans > 0)
  {
    // Recover the knot vector span index
    m_knots->interval((float)animFrame, span);
  }

  //-----------------------
  // Set the bind pose channel transforms
  if (generateDeltas)
  {
    tmAccumulator->setTransformsIdentity();
  }
  else
  {
    NMP_VERIFY(rig->getBindPose());
    tmAccumulator->setTransforms(rig->getBindPose()->m_transformBuffer);
  }

  //-----------------------
  // Set the unchanging pos channels (LS)
  for (uint32_t iChan = 0; iChan < unchangingPosNumChans; ++iChan)
  {
    uint16_t animChannelIndex = (uint16_t)unchangingPosCompToAnimMap[iChan];
    uint16_t rigChannelIndex;
    bool status = rigToAnimEntryMap->getRigIndexForAnimIndex(animChannelIndex, rigChannelIndex);
    NMP_VERIFY(status);
    tmAccumulator->setChannelPos(rigChannelIndex, unchangingPosQuantised[iChan], TransformsAccumulator::Local);
  }

  //-----------------------
  // Set the unchanging quat channels (LS)
  for (uint32_t iChan = 0; iChan < unchangingQuatNumChans; ++iChan)
  {
    uint16_t animChannelIndex = (uint16_t)unchangingQuatCompToAnimMap[iChan];
    uint16_t rigChannelIndex;
    bool status = rigToAnimEntryMap->getRigIndexForAnimIndex(animChannelIndex, rigChannelIndex);
    NMP_VERIFY(status);
    tmAccumulator->setChannelQuat(rigChannelIndex, unchangingQuatQuantised[iChan], TransformsAccumulator::Local);
  }

  //-----------------------
  // Set the sampled pos channels (LS)
  for (uint32_t iChan = 0; iChan < sampledPosNumChans; ++iChan)
  {
    uint16_t animChannelIndex = (uint16_t)sampledPosCompToAnimMap[iChan];
    uint16_t rigChannelIndex;
    bool status = rigToAnimEntryMap->getRigIndexForAnimIndex(animChannelIndex, rigChannelIndex);
    NMP_VERIFY(status);
    NMP_VERIFY(m_sampledPosCompressor);
    NMP::Vector3 posKey = m_sampledPosCompressor->evaluateQuantised(iChan, frame);
    tmAccumulator->setChannelPos(rigChannelIndex, posKey, TransformsAccumulator::Local);
  }

  //-----------------------
  // Set the sampled quat channels
  for (uint32_t iChan = 0; iChan < sampledQuatNumChans; ++iChan)
  {
    uint16_t animChannelIndex = (uint16_t)sampledQuatCompToAnimMap[iChan];
    uint16_t rigChannelIndex;
    bool status = rigToAnimEntryMap->getRigIndexForAnimIndex(animChannelIndex, rigChannelIndex);
    NMP_VERIFY(status);
    NMP_VERIFY(m_sampledQuatCompressor);
    NMP::Quat quatKey = m_sampledQuatCompressor->evaluateQuantised(iChan, frame);
    tmAccumulator->setChannelQuat(rigChannelIndex, quatKey, spaceType);
  }

  //-----------------------
  // Set the spline pos channels (LS)
  for (uint32_t iChan = 0; iChan < splinePosNumChans; ++iChan)
  {
    uint16_t animChannelIndex = (uint16_t)splinePosCompToAnimMap[iChan];
    uint16_t rigChannelIndex;
    bool status = rigToAnimEntryMap->getRigIndexForAnimIndex(animChannelIndex, rigChannelIndex);
    NMP_VERIFY(status);
    NMP_VERIFY(m_splinePosCompressor);
    NMP::Vector3 posKey = m_splinePosCompressor->evaluateQuantised(iChan, span, (float)animFrame);
    tmAccumulator->setChannelPos(rigChannelIndex, posKey, TransformsAccumulator::Local);
  }

  //-----------------------
  // Set the spline quat channels
  for (uint32_t iChan = 0; iChan < splineQuatNumChans; ++iChan)
  {
    uint16_t animChannelIndex = (uint16_t)splineQuatCompToAnimMap[iChan];
    uint16_t rigChannelIndex;
    bool status = rigToAnimEntryMap->getRigIndexForAnimIndex(animChannelIndex, rigChannelIndex);
    NMP_VERIFY(status);
    NMP_VERIFY(m_splineQuatCompressor);
    NMP::Quat quatKey = m_splineQuatCompressor->evaluateQuantised(iChan, span, (float)animFrame);
    tmAccumulator->setChannelQuat(rigChannelIndex, quatKey, spaceType);
  }

  //-----------------------
  // Accumulate the worldspace transforms
  if (useWorldSpace)
    tmAccumulator->convertToWorldSpace(*hierarchy);
  else
    tmAccumulator->convertToLocalSpace(*hierarchy);
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionCompressorQSA::computePosMeansQuantisationInfo()
{
  const AnimSourceUncompressed* inputAnim = m_manager->getInputAnimation();
  NMP_VERIFY(inputAnim);
  const ChannelSetRequirements* channelSetReqs = inputAnim->getChannelSetRequirements();
  NMP_VERIFY(channelSetReqs);
  const ChannelSetTable* channelSetsLS = inputAnim->getChannelSets();
  NMP_VERIFY(channelSetsLS);
  uint32_t numChannelSets = channelSetReqs->getNumChannelSets();
  uint32_t numChangingPosChannels = channelSetReqs->getNumChangingPosChannels();
  uint32_t numSectionFrames = getNumSectionFrames();

  if (numChangingPosChannels > 0)
  {
    //-----------------------
    // Allocate temporary memory for the quantisation table
    NMP::Memory::Format memReqsQuantMeans = AP::Vector3QuantisationTable::getMemoryRequirements(numChangingPosChannels);
    NMP::Memory::Resource memResQuantMeans = NMPMemoryAllocateFromFormat(memReqsQuantMeans);
    Vector3QuantisationTable* quantMeans = AP::Vector3QuantisationTable::init(memResQuantMeans, numChangingPosChannels);

    //-----------------------
    // Iterate over the changing pos channels
    uint32_t indx = 0;
    for (uint32_t i = 0; i < numChannelSets; ++i)
    {
      if (!channelSetReqs->isChannelPosUnchanging(i))
      {
        const NMP::Vector3* posKeys = channelSetsLS->getChannelPos(i);
        NMP_VERIFY(posKeys);
        const NMP::Vector3* sectionPosKeys = &posKeys[m_sectionStartFrame];
        quantMeans->setQuantisationBounds(indx, numSectionFrames, sectionPosKeys);
        ++indx;
      }
    }

    //-----------------------
    // Compute the global quantisation info
    NMP::Vector3 qMin, qMax;
    quantMeans->getGlobalQuantisationBounds(qMin, qMax);
    m_posMeansQuantisationInfo.m_qMin[0] = qMin.x;
    m_posMeansQuantisationInfo.m_qMin[1] = qMin.y;
    m_posMeansQuantisationInfo.m_qMin[2] = qMin.z;
    m_posMeansQuantisationInfo.m_qMax[0] = qMax.x;
    m_posMeansQuantisationInfo.m_qMax[1] = qMax.y;
    m_posMeansQuantisationInfo.m_qMax[2] = qMax.z;

    //-----------------------
    // Tidy up
    NMP::Memory::memFree(quantMeans);
  }
  else
  {
    m_posMeansQuantisationInfo.clear();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionCompressorQSA::currentSectionHeaderSize()
{
  //-----------------------
  // Info
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = m_manager->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t sampledPosNumChannels = channelSetInfoCompressor->getSampledPosNumChans();
  uint32_t sampledQuatNumChannels = channelSetInfoCompressor->getSampledQuatNumChans();
  uint32_t splinePosNumChannels = channelSetInfoCompressor->getSplinePosNumChans();
  uint32_t splineQuatNumChannels = channelSetInfoCompressor->getSplineQuatNumChans();
  uint32_t splineNumKnots = m_knots->getNumKnots();
  uint32_t numQuantisationSets;

  //-----------------------
  // Section header info
  NMP::BitStreamDecoder decoder;
  decoder.init(0xffffffff, NULL);
  decoder.incrementBytes(sizeof(MR::AnimSectionQSA));
  decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);

  //-----------------------
  // Sampled Pos channels
  if (sampledPosNumChannels > 0)
  {
    numQuantisationSets = getNumQuantisationSets(CoefSetQSA::CoefSetSampledPos);

    // Quantisation info about ranges of all sampled pos channels in this section
    decoder.incrementBytes(sizeof(MR::QuantisationInfoQSA) * numQuantisationSets);
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);

    // Array of quantisation data for the sampled pos channels
    decoder.incrementBytes(MR::QuantisationDataQSA::getMemoryRequirementsSize(sampledPosNumChannels));
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);
  }

  //-----------------------
  // Sampled Quat channels
  if (sampledQuatNumChannels > 0)
  {
    numQuantisationSets = getNumQuantisationSets(CoefSetQSA::CoefSetSampledQuat);

    // Quantisation info about ranges of all sampled quat channels in this section
    decoder.incrementBytes(sizeof(MR::QuantisationInfoQSA) * numQuantisationSets);
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);

    // Array of quantisation data for the sampled quat channels
    decoder.incrementBytes(MR::QuantisationDataQSA::getMemoryRequirementsSize(sampledQuatNumChannels));
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);
  }

  //-----------------------
  // Spline Knots
  if (splineNumKnots > 0)
  {
    NMP_VERIFY(splineNumKnots >= 2);
    decoder.incrementBytes(sizeof(uint16_t) * splineNumKnots);
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);
  }

  //-----------------------
  // Spline Pos channels
  if (splinePosNumChannels > 0)
  {
    NMP_VERIFY(m_splinePosCompressor);

    // Quantisation info about ranges of all spline pos channels in this section
    numQuantisationSets = getNumQuantisationSets(CoefSetQSA::CoefSetSplinePos);
    decoder.incrementBytes(sizeof(MR::QuantisationInfoQSA) * numQuantisationSets);
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);

    // Array of quantisation data for the spline pos channels
    decoder.incrementBytes(MR::QuantisationDataQSA::getMemoryRequirementsSize(splinePosNumChannels));
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);
  }

  //-----------------------
  // Spline Quat channels
  if (splineQuatNumChannels > 0)
  {
    NMP_VERIFY(m_splineQuatCompressor);

    // Quantisation info about ranges of all spline quat key channels in this section
    numQuantisationSets = getNumQuantisationSets(CoefSetQSA::CoefSetSplineQuatKey);
    decoder.incrementBytes(sizeof(MR::QuantisationInfoQSA) * numQuantisationSets);
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);

    // Array of quantisation data for the spline quat key channels
    decoder.incrementBytes(MR::QuantisationDataQSA::getMemoryRequirementsSize(splineQuatNumChannels));
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);

    // WNegs for all quat keys channels
    uint32_t wNegsByteStride = m_splineQuatCompressor->computeWNegsByteStride();
    decoder.incrementBytes(wNegsByteStride * splineNumKnots);
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);

    // Quantisation info about ranges of all spline quat tangent channels in this section
    numQuantisationSets = getNumQuantisationSets(CoefSetQSA::CoefSetSplineQuatTangent);
    decoder.incrementBytes(sizeof(MR::QuantisationInfoQSA) * numQuantisationSets);
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);

    // Array of quantisation data for the spline quat tangent channels
    decoder.incrementBytes(MR::QuantisationDataQSA::getMemoryRequirementsSize(splineQuatNumChannels));
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);
  }

  NMP_VERIFY(decoder.isAligned());
  m_curSectionHeaderSize = decoder.getBytesRead();
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionCompressorQSA::currentSectionDataSize()
{
  // Check if we have enough space for the bitstream data
  if (m_curSectionHeaderSize > (uint32_t)m_sectionSize)
  {
    // Allocate extra byte budget
    m_sectionSize = NMP::Memory::align(m_curSectionHeaderSize, NMP_VECTOR_ALIGNMENT);
  }

  m_curSectionDataSize = (uint32_t)m_sectionSize - m_curSectionHeaderSize;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionCompressorQSA::currentSectionSize()
{
  //-----------------------
  // Info
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = m_manager->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t sampledPosNumChannels = channelSetInfoCompressor->getSampledPosNumChans();
  uint32_t sampledQuatNumChannels = channelSetInfoCompressor->getSampledQuatNumChans();
  uint32_t splinePosNumChannels = channelSetInfoCompressor->getSplinePosNumChans();
  uint32_t splineQuatNumChannels = channelSetInfoCompressor->getSplineQuatNumChans();
  uint32_t numSectionFrames = getNumSectionFrames();
  uint32_t splineNumKnots = m_knots->getNumKnots();

  //-----------------------
  // Section header info
  NMP::BitStreamDecoder decoder;
  decoder.init(0xffffffff, NULL);
  decoder.incrementBytes(m_curSectionHeaderSize);
  NMP_VERIFY(decoder.isAligned(NMP_NATURAL_TYPE_ALIGNMENT));

  //-----------------------
  // Sampled Pos channels
  if (sampledPosNumChannels > 0)
  {
    NMP_VERIFY(m_sampledPosCompressor);
    m_sampledPosCompressor->computeDataStrides();
    uint32_t sampledPosByteStride = m_sampledPosCompressor->getSampledPosByteStride();

    // Bitstream data for the sampled pos channels
    decoder.incrementBytes(sampledPosByteStride * numSectionFrames);
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);
  }

  //-----------------------
  // Sampled Quat channels
  if (sampledQuatNumChannels > 0)
  {
    NMP_VERIFY(m_sampledQuatCompressor);
    m_sampledQuatCompressor->computeDataStrides();
    uint32_t sampledQuatByteStride = m_sampledQuatCompressor->getSampledQuatByteStride();

    // Bitstream data for the sampled quat channels
    decoder.incrementBytes(sampledQuatByteStride * numSectionFrames);
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);
  }

  //-----------------------
  // Spline Pos channels
  if (splinePosNumChannels > 0)
  {
    NMP_VERIFY(m_splinePosCompressor);
    m_splinePosCompressor->computeDataStrides();
    uint32_t splinePosByteStride = m_splinePosCompressor->getSplinePosByteStride();

    // Bitstream data for the spline pos channels
    decoder.incrementBytes(splinePosByteStride * (3 * splineNumKnots - 2)); // All Keys and Tangents sections
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);
  }

  //-----------------------
  // Spline Quat channels
  if (splineQuatNumChannels > 0)
  {
    NMP_VERIFY(m_splineQuatCompressor);
    m_splineQuatCompressor->computeDataStrides();

    uint32_t splineQuatKeysByteStride = m_splineQuatCompressor->getSplineQuatKeysByteStride();
    decoder.incrementBytes(splineQuatKeysByteStride * splineNumKnots);
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);

    uint32_t splineQuatTangentsByteStride = m_splineQuatCompressor->getSplineQuatTangentsByteStride();
    decoder.incrementBytes(splineQuatTangentsByteStride * 2 * (splineNumKnots - 1));
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);
  }

  NMP_VERIFY(decoder.isAligned());
  m_curSectionSize = decoder.getBytesRead();
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif
//----------------------------------------------------------------------------------------------------------------------
