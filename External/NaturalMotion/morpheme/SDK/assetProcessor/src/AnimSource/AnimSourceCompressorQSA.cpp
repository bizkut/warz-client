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
#define _WCTYPE_INLINE_DEFINED
#include <string.h>
#include <sstream>
#include <algorithm>
#ifdef WIN32
  #include <crtdbg.h>
#endif

#include <limits>
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMBitStreamCoder.h"
#include "morpheme/mrDefines.h"
#include "morpheme/AnimSource/mrAnimSourceQSA.h"
#include "assetProcessor/AnimSource/ChannelSetInfoCompressorQSA.h"
#include "assetProcessor/AnimSource/AnimSectionCompressorQSA.h"
#include "assetProcessor/AnimSource/TrajectorySourceCompressorQSA.h"
#include "assetProcessor/AnimSource/AnimSourceCompressorQSA.h"
//----------------------------------------------------------------------------------------------------------------------

// Disable warnings about deprecated functions (sprintf, fopen)
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning(disable : 4996)
#endif

// For testing with users desired compression method only
#define TEST_SPLINESx
#define TEST_SAMPLEDx

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
class AnimSourceQSABuilder : private MR::AnimSourceQSA
{
public:
  friend class AnimSourceCompressorQSA; // Allow the compressors access to the internal parameters
};

//----------------------------------------------------------------------------------------------------------------------
// AnimSourceCompressorQSA Functions
//----------------------------------------------------------------------------------------------------------------------
AnimSourceCompressorQSA::AnimSourceCompressorQSA()
{
  //-----------------------
  // Compressor options
  m_maxFramesPerSection = 0;
  m_desiredCompressionRate = 0.0f;  

  //-----------------------
  // Motion analyser
  m_motionAnalyser = NULL;

  //-----------------------
  // Compressor channel set information
  m_channelSetInfoCompressor = NULL;
  m_channelSetInfoCompressorSaved = NULL;

  //-----------------------
  // Compressor trajectory source
  m_trajectorySourceCompressor = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
AnimSourceCompressorQSA::~AnimSourceCompressorQSA()
{
  termCompressor();
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AnimSourceCompressorQSA::getMaxNumKeyframes() const
{
  NMP_VERIFY(m_inputAnimResampled);
  return m_inputAnimResampled->getMaxNumKeyframes();
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AnimSourceCompressorQSA::getNumAnimChannelSets() const
{
  NMP_VERIFY(m_inputAnimResampled);
  return m_inputAnimResampled->getNumChannelSets();
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AnimSourceCompressorQSA::getNumRigChannelSets() const
{
  const MR::AnimRigDef* rig = getConglomerateRig();
  return rig->getNumBones();
}

//----------------------------------------------------------------------------------------------------------------------
bool AnimSourceCompressorQSA::getGenerateDeltas() const
{
  NMP_VERIFY(m_inputAnimResampled);
  return m_inputAnimResampled->getGenerateDeltas();
}

//----------------------------------------------------------------------------------------------------------------------
const ChannelSetTable* AnimSourceCompressorQSA::getAnimChannelSets() const
{
  NMP_VERIFY(m_inputAnimResampled);
  return m_inputAnimResampled->getChannelSets();
}

//----------------------------------------------------------------------------------------------------------------------
const ChannelSetTable* AnimSourceCompressorQSA::getChannelSetsLS() const
{
  NMP_VERIFY(m_motionAnalyser);
  return m_motionAnalyser->getChannelSetsLS();
}

//----------------------------------------------------------------------------------------------------------------------
const ChannelSetTable* AnimSourceCompressorQSA::getChannelSetsWS() const
{
  NMP_VERIFY(m_motionAnalyser);
  return m_motionAnalyser->getChannelSetsWS();
}

//----------------------------------------------------------------------------------------------------------------------
AnimSectionCompressorQSA* AnimSourceCompressorQSA::getAnimSectionCompressor(uint32_t i) const
{
  NMP_VERIFY(i < (uint32_t)m_animSectionCompressors.size());
  return m_animSectionCompressors[i];
}

//----------------------------------------------------------------------------------------------------------------------
// Callback Functions
//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorQSA::initCompressor()
{
  //-----------------------
  // Information
  const MR::AnimRigDef* rig = getConglomerateRig();
  NMP_VERIFY(rig);
  const MR::RigToAnimEntryMap* rigToAnimEntryMap = getConglomerateRigToAnimEntryMap();
  NMP_VERIFY(rigToAnimEntryMap);

  //-----------------------
  // Analyse the motion for discontinuities and potential jitter. This converts the
  // XMD animation into worldspace transforms and evaluates a perceptual error metric
  // to determine which frames of the animation are most likely to have noticeable error.
  NMP_VERIFY(!m_motionAnalyser);
  m_motionAnalyser = new AnimSourceMotionAnalyser;
  m_motionAnalyser->analyse(
    m_inputAnimResampled,
    rig,
    rigToAnimEntryMap,
    m_messageLogger,
    m_errorLogger); // Does nothing but convert to worldspace transforms at the moment

  //-----------------------
  // Create a new trajectory source compressor and compile the trajectory data
  NMP_VERIFY(!m_trajectorySourceCompressor);
  if (m_inputAnimResampled->hasTrajectoryData())
  {
    m_trajectorySourceCompressor = new TrajectorySourceCompressorQSA(this);
    m_trajectorySourceCompressor->initCompressor();
    m_trajectorySourceCompressor->compileTrajectorySource();
  }

#if defined(TEST_SPLINES) || defined(TEST_SAMPLED)
  compileAnimSourceWithOptions();
  if (m_status)
    return;
  else
    NMP_ASSERT_FAIL();
#endif

  //-----------------------  
  // Compile the animation by forcing the channels to be sampled and in local space
  compileAnimSourceSampledLS();

  // Check if the animation was compiled with full precision with a given compression rate
  if (getAnimSourceHasFullPrec())
    return;

  // Wipe down the current compressor state
  clearCompressorState();

  // Compile the animation by forcing the channels to be sampled and in world space
  compileAnimSourceSampledWS();

  // Save the current compressor state
  saveCompressorState();

  // Compile the animation using the users input options
  compileAnimSourceWithOptions();

  // Compare the mean squared error between the sampled and spline compression.
  // Restore the sampled compressor state if required.
  compareAndRestoreCompressorState();
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorQSA::compileAnimSourceSampledLS()
{
  //-----------------------
  // Create a new channel set information compressor and initialise
  // the memory for the channel sets compression methods (required for the options)
  NMP_VERIFY(!m_channelSetInfoCompressor);
  m_channelSetInfoCompressor = new ChannelSetInfoCompressorQSA(this);
  m_channelSetInfoCompressor->initCompressor();

  //-----------------------
  // Process the compressor options
  setDefaults();  
  parseOptions();
  m_useWorldSpace = false; // Local space
  validateOptions();
  setOptionsSampled();

  //-----------------------
  // Compile the channel set information and section data
  compileSections();

  // Don't need to compute the error
  m_error = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorQSA::compileAnimSourceSampledWS()
{
  //-----------------------
  // Create a new channel set information compressor and initialise
  // the memory for the channel sets compression methods (required for the options)
  NMP_VERIFY(!m_channelSetInfoCompressor);
  m_channelSetInfoCompressor = new ChannelSetInfoCompressorQSA(this);
  m_channelSetInfoCompressor->initCompressor();

  //-----------------------
  // Process the compressor options
  setDefaults();  
  parseOptions();
  validateOptions();
  setOptionsSampled();

  //-----------------------
  // Compile the channel set information and section data
  compileSections();

  // Compute the error
  computeError();
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorQSA::compileAnimSourceWithOptions()
{
  //-----------------------
  // Create a new channel set information compressor and initialise
  // the memory for the channel sets compression methods (required for the options)
  NMP_VERIFY(!m_channelSetInfoCompressor);
  m_channelSetInfoCompressor = new ChannelSetInfoCompressorQSA(this);
  m_channelSetInfoCompressor->initCompressor();

  //-----------------------
  // Process the compressor options
  setDefaults();  
  parseOptions();
  validateOptions();

  //-----------------------
  // Compile the channel set information and section data
  compileSections();

  // Compute the error
  computeError();
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorQSA::compareAndRestoreCompressorState()
{
  if (m_status)
  {
    // Check if extra budget required to compile valid sections is the same
    if (m_budgetExtraSaved == m_budgetExtra)
    {
      if (m_errorSaved <= m_error)
        restoreCompressorState();
    }
    else
    {
      if (m_budgetExtraSaved <= m_budgetExtra)
        restoreCompressorState();
    }
  }
  else
  {
    restoreCompressorState();
    m_status = true;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorQSA::compileSections()
{
  m_status = true;
  m_budgetExtra = 0;

  //-----------------------
  // Compile the channel set information
  m_channelSetInfoCompressor->compileChannelSetInfo();

  //-----------------------
  // Compute the section size budgets and frame offsets
  computeSectionBudgets();

  //-----------------------
  // Create new section compressors and compile the section data
  uint32_t numSections = (uint32_t)m_startFrames.size();
  for (uint32_t i = 0; i < numSections; ++i)
  {
    AnimSectionCompressorQSA* section = new AnimSectionCompressorQSA(this, i);
    m_animSectionCompressors.push_back(section);
    section->initCompressor();

    // Compile the section data within the allocated byte budget (or nearest valid budget)
    section->compileSection();

    // Check if the section was compiled within the allocated byte budget
    size_t sectionBudget = section->getSectionDataSize();
    if (sectionBudget > m_sectionSizes[i])
      m_budgetExtra += (sectionBudget - m_sectionSizes[i]);

    // Update the actual section byte budget
    m_sectionSizes[i] = (uint32_t)sectionBudget;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool AnimSourceCompressorQSA::getAnimSourceHasFullPrec() const
{
  std::vector<AnimSectionCompressorQSA*>::const_iterator sit = m_animSectionCompressors.begin();
  std::vector<AnimSectionCompressorQSA*>::const_iterator sit_end = m_animSectionCompressors.end();
  while (sit != sit_end)
  {
    const AnimSectionCompressorQSA* compressorSection = *sit;
    NMP_VERIFY(compressorSection);
    if (!compressorSection->getAnimSectionHasFullPrec())
      return false;

    sit++;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorQSA::computeError()
{
  if (!m_status)
  {
    m_error = std::numeric_limits<float>::infinity();
    return;
  }

  m_error = 0.0f;
  uint32_t numSections = (uint32_t)m_animSectionCompressors.size();

  const ChannelSetTable* channelSets = NULL;
  bool useWorldSpace = getUseWorldSpace();
  bool generateDeltas = getGenerateDeltas();

  if (useWorldSpace)
    channelSets = getChannelSetsWS();
  else
    channelSets = getChannelSetsLS();

  for (uint32_t i = 0; i < numSections; ++i)
  {
    NMP_VERIFY(m_animSectionCompressors[i]);
    m_error += m_animSectionCompressors[i]->computeError(channelSets, useWorldSpace, generateDeltas);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorQSA::saveCompressorState()
{
  clearCompressorStateSaved();

  m_errorSaved = m_error;
  m_budgetExtraSaved = m_budgetExtra;
  m_memReqsChannelNamesTableSaved = m_memReqsChannelNamesTable;
  m_memReqsTrajectorySourceSaved = m_memReqsTrajectorySource;
  m_memReqsChannelSetInfoSaved = m_memReqsChannelSetInfo;
  m_memReqsAnimSourceHdrSaved = m_memReqsAnimSourceHdr;

  m_startFramesSaved = m_startFrames;
  m_startFrames.clear();
  m_endFramesSaved = m_endFrames;
  m_endFrames.clear();
  m_sectionSizesSaved = m_sectionSizes;
  m_sectionSizes.clear();

  NMP_VERIFY(!m_channelSetInfoCompressorSaved);
  m_channelSetInfoCompressorSaved = m_channelSetInfoCompressor;
  m_channelSetInfoCompressor = NULL;

  NMP_VERIFY(m_animSectionCompressorsSaved.empty());
  m_animSectionCompressorsSaved = m_animSectionCompressors;
  m_animSectionCompressors.clear();
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorQSA::restoreCompressorState()
{
  clearCompressorState();

  m_error = m_errorSaved;
  m_budgetExtra = m_budgetExtraSaved;
  m_memReqsChannelNamesTable = m_memReqsChannelNamesTableSaved;
  m_memReqsTrajectorySource = m_memReqsTrajectorySourceSaved;
  m_memReqsChannelSetInfo = m_memReqsChannelSetInfoSaved;
  m_memReqsAnimSourceHdr = m_memReqsAnimSourceHdrSaved;

  m_startFrames = m_startFramesSaved;
  m_startFramesSaved.clear();
  m_endFrames = m_endFramesSaved;
  m_endFramesSaved.clear();
  m_sectionSizes = m_sectionSizesSaved;
  m_sectionSizesSaved.clear();

  NMP_VERIFY(!m_channelSetInfoCompressor);
  m_channelSetInfoCompressor = m_channelSetInfoCompressorSaved;
  m_channelSetInfoCompressorSaved = NULL;

  NMP_VERIFY(m_animSectionCompressors.empty());
  m_animSectionCompressors = m_animSectionCompressorsSaved;
  m_animSectionCompressorsSaved.clear();
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorQSA::clearTrajectory()
{
  //-----------------------
  // Trajectory source
  if (m_trajectorySourceCompressor)
  {
    m_trajectorySourceCompressor->termCompressor();
    delete m_trajectorySourceCompressor;
    m_trajectorySourceCompressor = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorQSA::clearCompressorState()
{
  //-----------------------
  // Compressor sections
  std::vector<AnimSectionCompressorQSA*>::iterator sit = m_animSectionCompressors.begin();
  std::vector<AnimSectionCompressorQSA*>::iterator sit_end = m_animSectionCompressors.end();
  while (sit != sit_end)
  {
    AnimSectionCompressorQSA* compressorSection = *sit;
    NMP_VERIFY(compressorSection);
    compressorSection->termCompressor();
    delete compressorSection;
    sit++;
  }
  m_animSectionCompressors.clear();

  //-----------------------
  // Channel sets information
  if (m_channelSetInfoCompressor)
  {
    m_channelSetInfoCompressor->termCompressor();
    delete m_channelSetInfoCompressor;
    m_channelSetInfoCompressor = NULL;
  }

  //-----------------------
  m_startFrames.clear();
  m_endFrames.clear();
  m_sectionSizes.clear();
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorQSA::clearCompressorStateSaved()
{
  //-----------------------
  // Compressor sections
  std::vector<AnimSectionCompressorQSA*>::iterator sit = m_animSectionCompressorsSaved.begin();
  std::vector<AnimSectionCompressorQSA*>::iterator sit_end = m_animSectionCompressorsSaved.end();
  while (sit != sit_end)
  {
    AnimSectionCompressorQSA* compressorSection = *sit;
    NMP_VERIFY(compressorSection);
    compressorSection->termCompressor();
    delete compressorSection;
    sit++;
  }
  m_animSectionCompressorsSaved.clear();

  //-----------------------
  // Channel sets information
  if (m_channelSetInfoCompressorSaved)
  {
    m_channelSetInfoCompressorSaved->termCompressor();
    delete m_channelSetInfoCompressorSaved;
    m_channelSetInfoCompressorSaved = NULL;
  }

  //-----------------------
  m_startFramesSaved.clear();
  m_endFramesSaved.clear();
  m_sectionSizesSaved.clear();
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorQSA::termCompressor()
{
  //-----------------------
  clearTrajectory();
  clearCompressorState();
  clearCompressorStateSaved();

  //-----------------------
  // Motion analyser
  if (m_motionAnalyser)
  {
    delete m_motionAnalyser;
    m_motionAnalyser = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
size_t AnimSourceCompressorQSA::computeTrajectoryRequirements() const
{
  NMP::Memory::Format memReqs(0, NMP_VECTOR_ALIGNMENT);

  if (m_trajectorySourceCompressor)
  {
    memReqs.size = m_trajectorySourceCompressor->computeTrajectoryRequirements();
  }

  return memReqs.size;
}

//----------------------------------------------------------------------------------------------------------------------
size_t AnimSourceCompressorQSA::computeAdditionalDataRequirements() const
{
  // We have to make sure that we have enough padding for any trailing keyframes to
  // prevent reading beyond the allocated animation memory. There are at most 3
  // keyframes that need to be padded, each with XYZ components of uint16_t. Note that
  // each XYZ component is loaded using unpacku4i16, unpacku4i16f. i.e. 4 uint16_t's
  // so we need at most 20 bytes of padding.
  const size_t keyframePadding = 20;
  size_t sizeAfterLastSection = m_memReqsTrajectorySource.size + m_memReqsChannelNamesTable.size;
  if (sizeAfterLastSection < keyframePadding)
    return keyframePadding;
    
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
size_t AnimSourceCompressorQSA::computeAnimSourceHeaderRequirements() const
{
  uint32_t numSections = (uint32_t)m_sectionSizes.size();
  return computeAnimSourceHeaderRequirements(numSections);
}

//----------------------------------------------------------------------------------------------------------------------
size_t AnimSourceCompressorQSA::computeChannelSetInfoRequirements() const
{
  NMP_VERIFY(m_channelSetInfoCompressor);
  return m_channelSetInfoCompressor->computeChannelSetInfoRequirements();
}

//----------------------------------------------------------------------------------------------------------------------
bool AnimSourceCompressorQSA::computeSectionRequirements()
{
  // Nothing to do since we have already assigned the section budgets and frame offsets
  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
bool AnimSourceCompressorQSA::computeSectionRequirements(
  uint32_t NMP_UNUSED(startFrame),
  uint32_t NMP_UNUSED(endFrame),
  size_t& NMP_UNUSED(sectionSize)) const
{
  NMP_VERIFY_FAIL("AnimSourceCompressorQSA::computeSectionRequirements should never be called");
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorQSA::buildAnimSourceHeader(
  NMP::Memory::Resource& buffer,
  uint8_t*               data) const
{
  // Check if the animation successfully compiled
  if (!m_status)
    return;

  AnimSourceQSABuilder* anim = (AnimSourceQSABuilder*)buffer.ptr;
  NMP_VERIFY(anim);
  NMP_VERIFY((AnimSourceQSABuilder*)data == anim);

  // Animation information
  uint32_t numChannelSets = m_inputAnimResampled->getNumChannelSets();
  uint32_t numSections = (uint32_t)m_sectionSizes.size();

  //-----------------------
  // Header
  data += NMP::Memory::align(sizeof(MR::AnimSourceQSA), NMP_NATURAL_TYPE_ALIGNMENT);
  anim->m_animType = ANIM_TYPE_QSA;
  anim->m_funcTable = &MR::AnimSourceQSA::m_functionTable;
  anim->m_isLocated = true;
  anim->m_duration = m_inputAnimResampled->getDuration();
  anim->m_sampleFrequency = m_inputAnimResampled->getSampleFrequency();
  anim->m_numSections = numSections;
  anim->m_useDefaultPose = m_useDefaultPose;
  anim->m_useWorldSpace = m_useWorldSpace;
  anim->m_generateDeltas = m_inputAnimResampled->getGenerateDeltas();
  anim->m_numChannelSets = numChannelSets;
  anim->m_sectionsInfo = NULL;
  anim->m_sections = NULL;
  anim->m_channelSetsInfo = NULL;
  anim->m_trajectoryData = NULL;
  anim->m_channelNames = NULL;

  // Store the memory requirements of the animation class minus any section data.
  anim->m_animInstanceMemReqs = NMP::Memory::Format(m_sectionsByteOffset, NMP_VECTOR_ALIGNMENT);
  
  //-----------------------
  // Compression to animation channel maps
  uint32_t unchangingPosNumChans = m_channelSetInfoCompressor->getUnchangingPosNumChans();
  uint32_t unchangingQuatNumChans = m_channelSetInfoCompressor->getUnchangingQuatNumChans();
  uint32_t sampledPosNumChans = m_channelSetInfoCompressor->getSampledPosNumChans();
  uint32_t sampledQuatNumChans = m_channelSetInfoCompressor->getSampledQuatNumChans();
  uint32_t splinePosNumChans = m_channelSetInfoCompressor->getSplinePosNumChans();
  uint32_t splineQuatNumChans = m_channelSetInfoCompressor->getSplineQuatNumChans();
  uint32_t* unchangingPosCompToAnimMap = NULL;
  uint32_t* unchangingQuatCompToAnimMap = NULL;
  uint32_t* sampledPosCompToAnimMap = NULL;
  uint32_t* sampledQuatCompToAnimMap = NULL;
  uint32_t* splinePosCompToAnimMap = NULL;
  uint32_t* splineQuatCompToAnimMap = NULL;
  if (unchangingPosNumChans > 0)
    unchangingPosCompToAnimMap = (uint32_t*)&m_channelSetInfoCompressor->getUnchangingPosCompToAnimMap()[0];
  if (unchangingQuatNumChans > 0)
    unchangingQuatCompToAnimMap = (uint32_t*)&m_channelSetInfoCompressor->getUnchangingQuatCompToAnimMap()[0];
  if (sampledPosNumChans > 0)
    sampledPosCompToAnimMap = (uint32_t*)&m_channelSetInfoCompressor->getSampledPosCompToAnimMap()[0];
  if (sampledQuatNumChans > 0)
    sampledQuatCompToAnimMap = (uint32_t*)&m_channelSetInfoCompressor->getSampledQuatCompToAnimMap()[0];
  if (splinePosNumChans > 0)
    splinePosCompToAnimMap = (uint32_t*)&m_channelSetInfoCompressor->getSplinePosCompToAnimMap()[0];
  if (splineQuatNumChans > 0)
    splineQuatCompToAnimMap = (uint32_t*)&m_channelSetInfoCompressor->getSplineQuatCompToAnimMap()[0];
    
  anim->m_maxNumCompChannels = unchangingPosNumChans;
  anim->m_maxNumCompChannels = NMP::maximum(anim->m_maxNumCompChannels, unchangingQuatNumChans);
  anim->m_maxNumCompChannels = NMP::maximum(anim->m_maxNumCompChannels, sampledPosNumChans);
  anim->m_maxNumCompChannels = NMP::maximum(anim->m_maxNumCompChannels, sampledQuatNumChans);
  anim->m_maxNumCompChannels = NMP::maximum(anim->m_maxNumCompChannels, splinePosNumChans);
  anim->m_maxNumCompChannels = NMP::maximum(anim->m_maxNumCompChannels, splineQuatNumChans);  

  NMP::Memory::Resource memResCompToAnimMaps;
  memResCompToAnimMaps.format = MR::CompToAnimChannelMap::getMemoryRequirements(unchangingPosNumChans);
  memResCompToAnimMaps.format += MR::CompToAnimChannelMap::getMemoryRequirements(unchangingQuatNumChans);
  memResCompToAnimMaps.format += MR::CompToAnimChannelMap::getMemoryRequirements(sampledPosNumChans);
  memResCompToAnimMaps.format += MR::CompToAnimChannelMap::getMemoryRequirements(sampledQuatNumChans);
  memResCompToAnimMaps.format += MR::CompToAnimChannelMap::getMemoryRequirements(splinePosNumChans);
  memResCompToAnimMaps.format += MR::CompToAnimChannelMap::getMemoryRequirements(splineQuatNumChans);
  memResCompToAnimMaps.ptr = (void*)data;
  anim->m_unchangingPosCompToAnimMap = MR::CompToAnimChannelMap::init(memResCompToAnimMaps, unchangingPosNumChans, unchangingPosCompToAnimMap);
  anim->m_unchangingQuatCompToAnimMap = MR::CompToAnimChannelMap::init(memResCompToAnimMaps, unchangingQuatNumChans, unchangingQuatCompToAnimMap);
  anim->m_sampledPosCompToAnimMap = MR::CompToAnimChannelMap::init(memResCompToAnimMaps, sampledPosNumChans, sampledPosCompToAnimMap);
  anim->m_sampledQuatCompToAnimMap = MR::CompToAnimChannelMap::init(memResCompToAnimMaps, sampledQuatNumChans, sampledQuatCompToAnimMap);
  anim->m_splinePosCompToAnimMap = MR::CompToAnimChannelMap::init(memResCompToAnimMaps, splinePosNumChans, splinePosCompToAnimMap);
  anim->m_splineQuatCompToAnimMap = MR::CompToAnimChannelMap::init(memResCompToAnimMaps, splineQuatNumChans, splineQuatCompToAnimMap);
  NMP_VERIFY(memResCompToAnimMaps.format.size == 0);
  data = (uint8_t*)memResCompToAnimMaps.ptr;

  //-----------------------
  // AnimSectionInfo array (start frames and section sizes)
  anim->m_sectionsInfo = (MR::AnimSectionInfoQSA*) data;
  data += NMP::Memory::align(sizeof(MR::AnimSectionInfoQSA) * numSections, NMP_NATURAL_TYPE_ALIGNMENT);

  // Fill in section info data.
  for (uint32_t i = 0; i < numSections; ++i)
  {
    anim->m_sectionsInfo[i].setStartFrame(m_startFrames[i]);
    anim->m_sectionsInfo[i].setSectionSize((uint32_t)m_sectionSizes[i]);
  }

  //-----------------------
  // AnimSectionCompressedQSA (section pointers array)
  anim->m_sections = (MR::DataRef*) data;  // Values filled in later
  data += NMP::Memory::align(sizeof(MR::DataRef) * numSections, NMP_NATURAL_TYPE_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorQSA::buildChannelSetInfo(
  NMP::Memory::Resource& buffer,
  uint8_t*               data) const
{
  AnimSourceQSABuilder* anim = (AnimSourceQSABuilder*)buffer.ptr;
  NMP_VERIFY(anim);

  NMP_VERIFY(m_channelSetInfoCompressor);
  size_t channelSetInfoDataSize = m_channelSetInfoCompressor->getChannelSetInfoDataSize();
  MR::ChannelSetInfoQSA* channelSetInfoData = m_channelSetInfoCompressor->getChannelSetInfoData();
  NMP_VERIFY(channelSetInfoData);

  // Set the channel set information
  NMP_VERIFY(data);
  MR::ChannelSetInfoQSA* channelSetsInfo = (MR::ChannelSetInfoQSA*)data;
  anim->m_channelSetsInfo = (MR::ChannelSetInfoQSA*)channelSetsInfo;
  NMP::Memory::memcpy(
    channelSetsInfo,
    channelSetInfoData,
    channelSetInfoDataSize);

  // Relocate the data to its new location
  channelSetsInfo->relocate();
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorQSA::buildSection(
  NMP::Memory::Resource& buffer,
  uint32_t               sectionIndx,
  uint8_t*               data) const
{
  NMP_VERIFY(sectionIndx < (uint32_t)m_animSectionCompressors.size());
  AnimSourceQSABuilder* anim = (AnimSourceQSABuilder*)buffer.ptr;
  NMP_VERIFY(anim);

  AnimSectionCompressorQSA* sectionCompressor = m_animSectionCompressors[sectionIndx];
  NMP_VERIFY(sectionCompressor);
  size_t sectionDataSize = sectionCompressor->getSectionDataSize();
  const MR::AnimSectionQSA* sectionData = sectionCompressor->getSectionData();
  NMP_VERIFY(sectionData);

  // Set the channel set information
  NMP_VERIFY(data);
  MR::AnimSectionQSA* section = (MR::AnimSectionQSA*)data;
  anim->m_sections[sectionIndx].m_data = section;
  NMP::Memory::memcpy(
    section,
    sectionData,
    sectionDataSize);

  // Relocate the data to its new location
  section->relocate();
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorQSA::buildTrajectorySource(
  NMP::Memory::Resource& buffer,
  uint8_t*               data) const
{
  AnimSourceQSABuilder* anim = (AnimSourceQSABuilder*)buffer.ptr;
  NMP_VERIFY(anim);
  anim->m_trajectoryData = NULL;

  if (m_trajectorySourceCompressor)
  {
    // Build the trajectory source
    m_trajectorySourceCompressor->buildTrajectorySource(buffer, data);

    // Set the anim source header pointer to the trajectory source
    anim->m_trajectoryData = (MR::TrajectorySourceQSA*)data;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorQSA::buildChannelNamesTable(
  NMP::Memory::Resource& buffer,
  uint8_t*               data) const
{
  AnimSourceQSABuilder* anim = (AnimSourceQSABuilder*)buffer.ptr;
  NMP_VERIFY(anim);

  if (getAddChannelNamesTable())
  {
    anim->m_channelNames = (NMP::OrderedStringTable*)data;
    AnimSourceSectioningCompressor::buildChannelNamesTable(buffer, data);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorQSA::buildAdditionalData(
  NMP::Memory::Resource& buffer,
  uint8_t* data) const
{
  // We have to make sure that we have enough padding for any trailing keyframes to
  // prevent reading beyond the allocated animation memory.
  // See computeAdditionalDataRequirements
  NMP_VERIFY(buffer.ptr);
  NMP_VERIFY(data);
  (void)buffer;
  (void)data;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorQSA::writeDebugQSAInfo(
  FILE* filePointer,
  NMP::Memory::Resource& buffer) const
{
  NMP_VERIFY(filePointer);
  MR::AnimSourceQSA* anim = (MR::AnimSourceQSA*)buffer.ptr;
  NMP_VERIFY(anim);

  //-----------------------
  // Basic info
  const MR::AnimRigDef* rig = getConglomerateRig();
  NMP_VERIFY(rig);
  const MR::RigToAnimEntryMap* rigToAnimEntryMap = getConglomerateRigToAnimEntryMap();
  NMP_VERIFY(rigToAnimEntryMap);
  const NMP::OrderedStringTable* channelNameTable = m_inputAnimResampled->getChannelNames();
  NMP_VERIFY(channelNameTable);
  uint32_t numChannelSets = m_inputAnimResampled->getNumChannelSets();
  uint32_t numSections = (uint32_t)m_animSectionCompressors.size();
  const ChannelSetRequirements* chanSetReqs = m_inputAnimResampled->getChannelSetRequirements();
  NMP_VERIFY(chanSetReqs);
  NMP_VERIFY(chanSetReqs->getChannelSetRequirements());
  uint32_t numFrames = chanSetReqs->getMaxNumKeyframes();

  uint32_t unchangingPosNumChans = m_channelSetInfoCompressor->getUnchangingPosNumChans();
  uint32_t unchangingQuatNumChans = m_channelSetInfoCompressor->getUnchangingQuatNumChans();
  uint32_t sampledPosNumChans = m_channelSetInfoCompressor->getSampledPosNumChans();
  uint32_t sampledQuatNumChans = m_channelSetInfoCompressor->getSampledQuatNumChans();
  uint32_t splinePosNumChans = m_channelSetInfoCompressor->getSplinePosNumChans();
  uint32_t splineQuatNumChans = m_channelSetInfoCompressor->getSplineQuatNumChans();

  //-----------------------
  // Information
  float duration = anim->getDuration();
  float sampleFreq = anim->getSampleFrequency();
  fprintf(filePointer, "\n_______________ Animation Information _______________\n");
  fprintf(filePointer, "Num channel sets = %d\n", numChannelSets);
  fprintf(filePointer, "Num sections = %d\n", numSections);
  fprintf(filePointer, "Num frames = %d\n", numFrames);
  fprintf(filePointer, "Duration = %f\n", duration);
  fprintf(filePointer, "Sample Frequency = %f\n", sampleFreq);
  fprintf(filePointer, "qSet quality = %f\n", m_quantisationSetQuality);

  fprintf(filePointer, "Num unchanging pos chans = %d\n", unchangingPosNumChans);
  fprintf(filePointer, "Num sampled pos chans = %d\n", sampledPosNumChans);
  fprintf(filePointer, "Num spline pos chans = %d\n", splinePosNumChans);
  fprintf(filePointer, "Num unchanging quat chans = %d\n", unchangingQuatNumChans);
  fprintf(filePointer, "Num sampled quat chans = %d\n", sampledQuatNumChans);
  fprintf(filePointer, "Num spline quat chans = %d\n", splineQuatNumChans);

  fprintf(filePointer, "\n_______________ Compression Information _______________\n");

  float animSizeFac = 100.0f / m_memReqsAnimSource.size;

  float compressionRate = getCompressionRate();
  fprintf(filePointer, "Compression rate = %3.3f\n", compressionRate);

  // Anim source
  fprintf(filePointer, "\n_______________ Anim source _______________\n");
  fprintf(filePointer, "animSourceHdr = %d (%3.3f %%)\n",
          m_memReqsAnimSourceHdr.size,
          animSizeFac * m_memReqsAnimSourceHdr.size);

  // Channel set information
  fprintf(filePointer, "\n_______________ Channel set information _______________\n");
  size_t channelSetInfoDataSize = m_channelSetInfoCompressor->getChannelSetInfoDataSize();
  size_t unchangingPosDataSize = m_channelSetInfoCompressor->getUnchangingPosDataSize();
  size_t unchangingQuatDataSize = m_channelSetInfoCompressor->getUnchangingQuatDataSize();
  size_t channelSetInfoHdrSize = channelSetInfoDataSize - unchangingPosDataSize - unchangingQuatDataSize;
  fprintf(filePointer, "channelSetInfoHdrSize = %zd (%3.3f %%)\n",
          channelSetInfoHdrSize,
          animSizeFac * channelSetInfoHdrSize);
  fprintf(filePointer, "unchangingPosDataSize = %zd (%3.3f %%)\n",
          unchangingPosDataSize,
          animSizeFac * unchangingPosDataSize);
  fprintf(filePointer, "unchangingQuatDataSize = %zd (%3.3f %%)\n",
          unchangingQuatDataSize,
          animSizeFac * unchangingQuatDataSize);

  // Sections
  size_t animSectionHdrSizeAll = 0;
  size_t sampledPosDataSizeAll = 0;
  size_t sampledQuatDataSizeAll = 0;
  size_t splinePosDataSizeAll = 0;
  size_t splineQuatDataSizeAll = 0;
  for (uint32_t i = 0; i < numSections; ++i)
  {
    AnimSectionCompressorQSA* section = m_animSectionCompressors[i];
    NMP_VERIFY(section);

    fprintf(filePointer, "\n_______________ Section %d _______________\n", i);

    fprintf(filePointer, "sectionStartFrame = %4d, sectionSize = %4d\n",
            section->getSectionStartFrame(),
            section->getSectionDataSize());

    uint32_t sampledPosNumQSets = section->getNumQuantisationSets(CoefSetQSA::CoefSetSampledPos);
    uint32_t sampledQuatNumQSets = section->getNumQuantisationSets(CoefSetQSA::CoefSetSampledQuat);
    uint32_t splinePosNumQSets = section->getNumQuantisationSets(CoefSetQSA::CoefSetSplinePos);
    uint32_t splineQuatKeyNumQSets = section->getNumQuantisationSets(CoefSetQSA::CoefSetSplineQuatKey);
    uint32_t splineQuatTangentNumQSets = section->getNumQuantisationSets(CoefSetQSA::CoefSetSplineQuatTangent);

    fprintf(filePointer, "sampledPosNumQSets = %4d\n", sampledPosNumQSets);
    fprintf(filePointer, "sampledQuatNumQSets = %4d\n", sampledQuatNumQSets);
    fprintf(filePointer, "splinePosNumQSets = %4d\n", splinePosNumQSets);
    fprintf(filePointer, "splineQuatKeyNumQSets = %4d\n", splineQuatKeyNumQSets);
    fprintf(filePointer, "splineQuatTangentNumQSets = %4d\n\n", splineQuatTangentNumQSets);

    size_t animSectionHdrSize = section->getAnimSectionHdrSize();
    fprintf(filePointer, "animSectionHdrSize = %zd (%3.3f %%)\n",
            animSectionHdrSize,
            animSizeFac * animSectionHdrSize);

    size_t sampledPosQuantisationInfoSize = section->getSampledPosQuantisationInfoSize();
    size_t sampledPosQuantisationDataSize = section->getSampledPosQuantisationDataSize();
    size_t sampledPosDataSize = section->getSampledPosDataSize();
    fprintf(filePointer, "sampledPosQuantisationInfoSize = %zd (%3.3f %%)\n",
            sampledPosQuantisationInfoSize,
            animSizeFac * sampledPosQuantisationInfoSize);
    fprintf(filePointer, "sampledPosQuantisationDataSize = %zd (%3.3f %%)\n",
            sampledPosQuantisationDataSize,
            animSizeFac * sampledPosQuantisationDataSize);
    fprintf(filePointer, "sampledPosDataSize = %zd (%3.3f %%)\n",
            sampledPosDataSize,
            animSizeFac * sampledPosDataSize);

    size_t sampledQuatQuantisationInfoSize = section->getSampledQuatQuantisationInfoSize();
    size_t sampledQuatQuantisationDataSize = section->getSampledQuatQuantisationDataSize();
    size_t sampledQuatDataSize = section->getSampledQuatDataSize();
    fprintf(filePointer, "sampledQuatQuantisationInfoSize = %zd (%3.3f %%)\n",
            sampledQuatQuantisationInfoSize,
            animSizeFac * sampledQuatQuantisationInfoSize);
    fprintf(filePointer, "sampledQuatQuantisationDataSize = %zd (%3.3f %%)\n",
            sampledQuatQuantisationDataSize,
            animSizeFac * sampledQuatQuantisationDataSize);
    fprintf(filePointer, "sampledQuatDataSize = %zd (%3.3f %%)\n",
            sampledQuatDataSize,
            animSizeFac * sampledQuatDataSize);

    size_t splineKnotsSize = section->getSplineKnotsSize();
    fprintf(filePointer, "splineKnotsSize = %u (%3.3f %%)\n",
            splineKnotsSize,
            animSizeFac * splineKnotsSize);

    size_t splinePosQuantisationInfoSize = section->getSplinePosQuantisationInfoSize();
    size_t splinePosQuantisationDataSize = section->getSplinePosQuantisationDataSize();
    size_t splinePosDataSize = section->getSplinePosDataSize();
    fprintf(filePointer, "splinePosQuantisationInfoSize = %zd (%3.3f %%)\n",
            splinePosQuantisationInfoSize,
            animSizeFac * splinePosQuantisationInfoSize);
    fprintf(filePointer, "splinePosQuantisationDataSize = %zd (%3.3f %%)\n",
            splinePosQuantisationDataSize,
            animSizeFac * splinePosQuantisationDataSize);
    fprintf(filePointer, "splinePosDataSize = %zd (%3.3f %%)\n",
            splinePosDataSize,
            animSizeFac * splinePosDataSize);

    size_t splineQuatKeysQuantisationInfoSize = section->getSplineQuatKeysQuantisationInfoSize();
    size_t splineQuatKeysQuantisationDataSize = section->getSplineQuatKeysQuantisationDataSize();
    size_t splineQuatKeysWNegsDataSize = section->getSplineQuatKeysWNegsDataSize();
    size_t splineQuatKeysDataSize = section->getSplineQuatKeysDataSize();
    size_t splineQuatTangentsQuantisationInfoSize = section->getSplineQuatTangentsQuantisationInfoSize();
    size_t splineQuatTangentsQuantisationDataSize = section->getSplineQuatTangentsQuantisationDataSize();
    size_t splineQuatTangentsDataSize = section->getSplineQuatTangentsDataSize();
    fprintf(filePointer, "splineQuatKeysQuantisationInfoSize = %zd (%3.3f %%)\n",
            splineQuatKeysQuantisationInfoSize,
            animSizeFac * splineQuatKeysQuantisationInfoSize);
    fprintf(filePointer, "splineQuatKeysQuantisationDataSize = %zd (%3.3f %%)\n",
            splineQuatKeysQuantisationDataSize,
            animSizeFac * splineQuatKeysQuantisationDataSize);
    fprintf(filePointer, "splineQuatKeysWNegsDataSize = %zd (%3.3f %%)\n",
            splineQuatKeysWNegsDataSize,
            animSizeFac * splineQuatKeysWNegsDataSize);
    fprintf(filePointer, "splineQuatKeysDataSize = %zd (%3.3f %%)\n",
            splineQuatKeysDataSize,
            animSizeFac * splineQuatKeysDataSize);
    fprintf(filePointer, "splineQuatTangentsQuantisationInfoSize = %zd (%3.3f %%)\n",
            splineQuatTangentsQuantisationInfoSize,
            animSizeFac * splineQuatTangentsQuantisationInfoSize);
    fprintf(filePointer, "splineQuatTangentsQuantisationDataSize = %zd (%3.3f %%)\n",
            splineQuatTangentsQuantisationDataSize,
            animSizeFac * splineQuatTangentsQuantisationDataSize);
    fprintf(filePointer, "splineQuatTangentsDataSize = %zd (%3.3f %%)\n",
            splineQuatTangentsDataSize,
            animSizeFac * splineQuatTangentsDataSize);

    // Summary info
    animSectionHdrSizeAll += animSectionHdrSize;

    sampledPosDataSizeAll += (sampledPosQuantisationInfoSize + sampledPosQuantisationDataSize + sampledPosDataSize);

    sampledQuatDataSizeAll += (sampledQuatQuantisationInfoSize + sampledQuatQuantisationDataSize + sampledQuatDataSize);

    splinePosDataSizeAll += (splinePosQuantisationInfoSize + splinePosQuantisationDataSize + splinePosDataSize);

    splineQuatDataSizeAll += splineKnotsSize;
    splineQuatDataSizeAll += splineQuatKeysQuantisationInfoSize;
    splineQuatDataSizeAll += splineQuatKeysQuantisationDataSize;
    splineQuatDataSizeAll += splineQuatKeysWNegsDataSize;
    splineQuatDataSizeAll += splineQuatTangentsQuantisationInfoSize;
    splineQuatDataSizeAll += splineQuatTangentsQuantisationDataSize;
    splineQuatDataSizeAll += splineQuatTangentsDataSize;
  }

  fprintf(filePointer, "\n_______________ Sections summary _______________\n");

  fprintf(filePointer, "animSectionHdrSize = %zd (%3.3f %%)\n",
          animSectionHdrSizeAll,
          animSizeFac * animSectionHdrSizeAll);

  fprintf(filePointer, "sampledPosDataSize = %zd (%3.3f %%)\n",
          sampledPosDataSizeAll,
          animSizeFac * sampledPosDataSizeAll);

  fprintf(filePointer, "sampledQuatDataSize = %zd (%3.3f %%)\n",
          sampledQuatDataSizeAll,
          animSizeFac * sampledQuatDataSizeAll);

  fprintf(filePointer, "splinePosDataSize = %zd (%3.3f %%)\n",
          splinePosDataSizeAll,
          animSizeFac * splinePosDataSizeAll);

  fprintf(filePointer, "splineQuatDataSize = %zd (%3.3f %%)\n",
          splineQuatDataSizeAll,
          animSizeFac * splineQuatDataSizeAll);

  // Trajectory
  fprintf(filePointer, "\n_______________ Trajectory _______________\n");
  fprintf(filePointer, "trajectorySource = %d (%3.3f %%)\n",
          m_memReqsTrajectorySource.size,
          animSizeFac * m_memReqsTrajectorySource.size);

  // Channel names table
  fprintf(filePointer, "\n_______________ Channel names table _______________\n");
  fprintf(filePointer, "channelNamesTable = %d (%3.3f %%)\n",
          m_memReqsChannelNamesTable.size,
          animSizeFac * m_memReqsChannelNamesTable.size);

  //-----------------------
  // Rig channels
  fprintf(filePointer, "\n_______________ Rig Channels _______________\n");
  const NMP::Hierarchy* hierarchy = rig->getHierarchy();
  uint32_t numBones = hierarchy->getNumEntries();
  for (uint32_t i = 0; i < numBones; ++i)
  {
    const char* name = rig->getBoneName(i);
    int32_t parentID = hierarchy->getParentIndex(i);
    fprintf(filePointer, "rig chan = %4d, parent rig chan = %4d, name = %s\n", i, parentID, name);
  }

  //-----------------------
  // Rig to animation map
  fprintf(filePointer, "\n_______________ Rig To Anim Map _______________\n");
  uint32_t numEntries = rigToAnimEntryMap->getNumEntries();
  for (uint32_t i = 0; i < numEntries; ++i)
  {
    uint16_t rigIndex = rigToAnimEntryMap->getEntryRigChannelIndex(i);
    uint16_t animIndex = rigToAnimEntryMap->getEntryAnimChannelIndex(i);
    fprintf(filePointer, "rig chan index = %4d, anim chan index = %4d\n", rigIndex, animIndex);
  }

  //-----------------------
  // Rig root bone information
  fprintf(filePointer, "\n_______________ Rig root bone info _______________\n");
  uint32_t charRootBoneID = rig->getCharacterRootBoneIndex();
  uint32_t trajBoneID = rig->getTrajectoryBoneIndex();
  fprintf(filePointer, "charRootBoneID = %4d\n", charRootBoneID);
  fprintf(filePointer, "trajBoneID = %4d\n", trajBoneID);

  //-----------------------
  // Channel set overrides
  fprintf(filePointer, "\n_______________ Rig channel overrides _______________\n");
  const ChannelSetOverrides* overrides = m_inputAnimResampled->getChannelSetOverrides();
  NMP_VERIFY(overrides);
  uint32_t numOverrides = overrides->getNumChannelSetOverrides();
  const uint32_t* buffer = overrides->getChannelSetOverrides();
  for (uint32_t i = 0; i < numOverrides; ++i)
  {
    fprintf(filePointer, "rig chan = %4d\n", buffer[i]);
  }

  //-----------------------
  // Channel set names
  fprintf(filePointer, "\n_______________ Animation Channel Set Name Table _______________\n");
  const NMP::OrderedStringTable* nameTable = MR::AnimSourceQSA::getChannelNameTable(anim);
  if (nameTable)
  {
    uint32_t numChannelSets = nameTable->getNumEntries();
    for (uint32_t i = 0; i < numChannelSets; ++i)
    {
      const char* name = nameTable->getStringForID(i);
      NMP_VERIFY(name);

      fprintf(filePointer, "anim chan = %4d, name = %s\n", i, name);
    }
  }
  else
  {
    fprintf(filePointer, "Not stored\n");
  }

  //-----------------------
  // Position channel compression methods
  fprintf(filePointer, "\n_______________ Position channel compression methods _______________\n");

  fprintf(filePointer, "Num unchanging chans = %d\n", unchangingPosNumChans);
  fprintf(filePointer, "Num sampled chans = %d\n", sampledPosNumChans);
  fprintf(filePointer, "Num spline chans = %d\n", splinePosNumChans);

  for (uint32_t i = 0; i < numChannelSets; ++i)
  {
    const char* name = channelNameTable->getStringForID(i);
    NMP_VERIFY(name);

    switch (m_channelSetInfoCompressor->getCompChanMethodForPosChan(i))
    {
    case ChannelSetInfoCompressorQSA::Unchanging:
      fprintf(
        filePointer,
        "anim chan = %4d, method = Unchanging, name = %s\n",
        i,
        name);
      break;

    case ChannelSetInfoCompressorQSA::Sampled:
      fprintf(
        filePointer,
        "anim chan = %4d, method = Sampled, name = %s\n",
        i,
        name);
      break;

    case ChannelSetInfoCompressorQSA::Spline:
      fprintf(
        filePointer,
        "anim chan = %4d, method = Spline, name = %s\n",
        i,
        name);
      break;
    }
  }

  //-----------------------
  // Quaternion channel compression methods
  fprintf(filePointer, "\n_______________ Quaternion channel compression methods _______________\n");

  fprintf(filePointer, "Num unchanging chans = %d\n", unchangingQuatNumChans);
  fprintf(filePointer, "Num sampled chans = %d\n", sampledQuatNumChans);
  fprintf(filePointer, "Num spline chans = %d\n", splineQuatNumChans);

  for (uint32_t i = 0; i < numChannelSets; ++i)
  {
    const char* name = channelNameTable->getStringForID(i);
    NMP_VERIFY(name);

    switch (m_channelSetInfoCompressor->getCompChanMethodForQuatChan(i))
    {
    case ChannelSetInfoCompressorQSA::Unchanging:
      fprintf(
        filePointer,
        "anim chan = %4d, method = Unchanging, name = %s\n",
        i,
        name);
      break;

    case ChannelSetInfoCompressorQSA::Sampled:
      fprintf(
        filePointer,
        "anim chan = %4d, method = Sampled, name = %s\n",
        i,
        name);
      break;

    case ChannelSetInfoCompressorQSA::Spline:
      fprintf(
        filePointer,
        "anim chan = %4d, method = Spline, name = %s\n",
        i,
        name);
      break;
    }
  }

  //-----------------------
  // Channel set info
  m_channelSetInfoCompressor->writeDebug(filePointer);

  //-----------------------
  // Sections
  for (uint32_t i = 0; i < numSections; ++i)
  {
    fprintf(filePointer, "\n_______________ Section %d _______________\n", i);

    AnimSectionCompressorQSA* section = m_animSectionCompressors[i];
    NMP_VERIFY(section);
    section->writeDebug(filePointer);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorQSA::testLocation(NMP::Memory::Resource& buffer) const
{
  MR::AnimSourceQSA* animSrc = (MR::AnimSourceQSA*)buffer.ptr;
  NMP_VERIFY(animSrc);
  uint8_t* src = (uint8_t*)buffer.ptr;

  // Create a memory copy of the source animation
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(buffer.format);
  MR::AnimSourceQSA* animDst = (MR::AnimSourceQSA*)memRes.ptr;
  NMP::Memory::memcpy(animDst, animSrc, buffer.format.size);

  // Create temporary working memory buffer
  NMP::Memory::Resource memResTemp = NMPMemoryAllocateFromFormat(buffer.format);
  MR::AnimSourceQSA* animTemp = (MR::AnimSourceQSA*)memResTemp.ptr;

  //-----------------------
  // TEST location / dislocation

  // All pointers are valid in the memory space of the source buffer
  animSrc->dislocate();
  animSrc->locate();

#ifdef NMP_ENABLE_ASSERTS
  uint8_t* dst = (uint8_t*)memRes.ptr;
  // Compare the data to ensure that it has not changed
  for (uint32_t i = 0; i < buffer.format.size; ++i)
  {
    NMP_VERIFY(src[i] == dst[i]);
  }
#endif

  // Reset the source buffer
  NMP::Memory::memcpy(animSrc, animDst, buffer.format.size);

  //-----------------------
  // TEST relocate

  // Copy to new location
  NMP::Memory::memcpy(animTemp, animSrc, buffer.format.size);

  // Relocate in new memory
  animTemp->relocate();

  // Clear source memory
  for (uint32_t i = 0; i < buffer.format.size; ++i)
    src[i] = 0;

  // Copy back to old location
  NMP::Memory::memcpy(animSrc, animTemp, buffer.format.size);

  // Relocate in original memory
  animSrc->relocate();

#ifdef NMP_ENABLE_ASSERTS
  // Compare the data to ensure that it has not changed
  for (uint32_t i = 0; i < buffer.format.size; ++i)
  {
    NMP_VERIFY(src[i] == dst[i]);
  }
#endif

  // Reset the source buffer
  NMP::Memory::memcpy(animSrc, animDst, buffer.format.size);

  // Tidy up
  NMP::Memory::memFree(memRes.ptr);
  NMP::Memory::memFree(memResTemp.ptr);
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorQSA::onEndBuildAnimation(NMP::Memory::Resource& buffer)
{
(void)buffer;
#ifdef TEST_TRANSFORMS
  //-----------------------
  // TEST location / dislocation
  MR::AnimSourceQSA* anim = (MR::AnimSourceQSA*)buffer.ptr;
  NMP_VERIFY(anim);
  testLocation(buffer);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorQSA::buildRigToAnimMaps()
{
  const std::vector<const MR::AnimRigDef*>& rigs = getRigs();
  const std::vector<const MR::RigToAnimEntryMap*>& rigToAnimEntryMaps = getRigToAnimEntryMaps();
  NMP_VERIFY(rigs.size() == rigToAnimEntryMaps.size());
  std::vector<MR::RigToAnimMap*>& rigToAnimMaps = AnimSourceCompressor::getRigToAnimMaps();
  NMP_VERIFY(rigToAnimMaps.empty());
  const MR::RigToAnimEntryMap* conglomerateRigToAnimEntryMap = getConglomerateRigToAnimEntryMap();
  NMP_VERIFY(conglomerateRigToAnimEntryMap);

  uint32_t numRigs = (uint32_t) rigs.size();
  for (uint32_t i = 0; i < numRigs; ++i)
  {
    const MR::AnimRigDef* rig = rigs[i];
    const MR::RigToAnimEntryMap* rigToAnimEntryMap = rigToAnimEntryMaps[i];
    MR::RigToAnimMap* rigToAnimMap = buildAnimToRigTableMap(rig, rigToAnimEntryMap, conglomerateRigToAnimEntryMap);
    rigToAnimMaps.push_back(rigToAnimMap);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Compression Functions
//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorQSA::setDefaults()
{
  // Default options
  m_maxFramesPerSection = 60;
  m_desiredCompressionRate = 100.0f;
  m_framesPerKey = 6; // 3 fpk has an approx break even byte budget as a sampled anim
  m_quantisationSetQuality = 1.0f / 3.0f; // Keep only a third of the quantisation range information
  m_useDefaultPose = true;
  m_useWorldSpace = !getGenerateDeltas();

  // Set default channel compression methods
  NMP_VERIFY(m_channelSetInfoCompressor);
  m_channelSetInfoCompressor->setDefaults();
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorQSA::parseOptions()
{
  std::string stringOptions = getOptions();
  char* buffer = (char*)stringOptions.c_str();

#if defined(TEST_SPLINES)
  std::string tempOptions = stringOptions + " -pmethod spline * -qmethod spline *";
  buffer = (char*)tempOptions.c_str();
#elif defined(TEST_SAMPLED)
  std::string tempOptions = stringOptions + " -pmethod sampled * -qmethod sampled *";
  buffer = (char*)tempOptions.c_str();
#endif

  //-----------------------
  // Information
  const MR::AnimRigDef* rig = getConglomerateRig();
  NMP_VERIFY(rig);
  const MR::RigToAnimEntryMap* rigToAnimEntryMap = getConglomerateRigToAnimEntryMap();
  NMP_VERIFY(rigToAnimEntryMap);
  uint32_t numRigToAnimMapEntries = rigToAnimEntryMap->getNumEntries();

  // Parse the options string
  const char seps[] = " ,\t\n";
  char* token;
  token = strtok(buffer, seps);
  while (token)
  {
    bool status = true; // Eat any unrecognised tokens

    if (stricmp(token, "-usedefpose") == 0)
    {
      //-----------------------
      // Use the default pose when determining the set of unchanging channels
      status = false;
      token = strtok(0, seps);
      if(token && token[0] != '-')
      {
        uint32_t value = (uint32_t)atoi(token);
        m_useDefaultPose = (value != 0);
        status = true;
      }
    }
    else if (stricmp(token, "-pmethod") == 0)
    {
      //-----------------------
      // Method: -pmethod [sampled | spline] <jointname>
      status = false;
      token = strtok(0, seps);
      if (token && token[0] != '-')
      {
        // Get the compression method
        bool valid = false;
        ChannelSetInfoCompressorQSA::chanMethodType method = ChannelSetInfoCompressorQSA::Spline;
        if (stricmp(token, "sampled") == 0)
        {
          method = ChannelSetInfoCompressorQSA::Sampled;
          valid = true;
        }
        else if (stricmp(token, "spline") == 0)
        {
          method = ChannelSetInfoCompressorQSA::Spline;
          valid = true;
        }

        // Get the joint name
        if (valid)
        {
          token = strtok(0, seps);
          if (token && token[0] != '-')
          {
            // Check if wish to set all joint sources
            if (stricmp(token, "*") == 0)
            {
              for (uint32_t i = 0; i < numRigToAnimMapEntries; ++i)
              {
                uint16_t animIndex = rigToAnimEntryMap->getEntryAnimChannelIndex(i);
                m_channelSetInfoCompressor->setDesiredCompChanMethodForPosChan(animIndex, method);
              }
            }
            else
            {
              // Get the animation channel for the rig bone name
              int16_t animIndex = rigToAnimEntryMap->getAnimChannelIndexFromRigBoneName(token, rig);
              if (animIndex >= 0)
              {
                m_channelSetInfoCompressor->setDesiredCompChanMethodForPosChan((uint32_t)animIndex, method);
              }
            }
            status = true;
          }
        }
      }
    }
    else if (stricmp(token, "-qmethod") == 0)
    {
      //-----------------------
      // Method: -qmethod [sampled | spline] <jointname>
      status = false;
      token = strtok(0, seps);
      if (token && token[0] != '-')
      {
        // Get the compression method
        bool valid = false;
        ChannelSetInfoCompressorQSA::chanMethodType method = ChannelSetInfoCompressorQSA::Spline;
        if (stricmp(token, "sampled") == 0)
        {
          method = ChannelSetInfoCompressorQSA::Sampled;
          valid = true;
        }
        else if (stricmp(token, "spline") == 0)
        {
          method = ChannelSetInfoCompressorQSA::Spline;
          valid = true;
        }

        // Get the joint name
        if (valid)
        {
          token = strtok(0, seps);
          if (token && token[0] != '-')
          {
            // Check if wish to set all joint sources
            if (stricmp(token, "*") == 0)
            {
              for (uint32_t i = 0; i < numRigToAnimMapEntries; ++i)
              {
                uint16_t animIndex = rigToAnimEntryMap->getEntryAnimChannelIndex(i);
                m_channelSetInfoCompressor->setDesiredCompChanMethodForQuatChan(animIndex, method);
              }
            }
            else
            {
              // Get the animation channel for the rig bone name
              int16_t animIndex = rigToAnimEntryMap->getAnimChannelIndexFromRigBoneName(token, rig);
              if (animIndex >= 0)
              {
                m_channelSetInfoCompressor->setDesiredCompChanMethodForQuatChan(animIndex, method);
              }
            }
            status = true;
          }
        }
      }
    }
    else if (stricmp(token, "-rate") == 0)
    {
      //-----------------------
      // Compression rate (bytes/bone/sec)
      status = false;
      token = strtok(0, seps);
      if (token && token[0] != '-')
      {
        m_desiredCompressionRate = (float)atof(token);
        NMP::clampValue(m_desiredCompressionRate, 0.0f, 1000.0f);
        status = true;
      }
    }
    else if (stricmp(token, "-mfps") == 0)
    {
      //-----------------------
      // Max frames per section
      status = false;
      token = strtok(0, seps);
      if (token && token[0] != '-')
      {
        m_maxFramesPerSection = (uint32_t)atoi(token);
        if (m_maxFramesPerSection < 2)
          m_maxFramesPerSection = 2;
        status = true;
      }
    }
    else if (stricmp(token, "-qset") == 0)
    {
      //-----------------------
      // Quantisation set quality
      status = false;
      token = strtok(0, seps);
      if (token && token[0] != '-')
      {
        m_quantisationSetQuality = (float)atof(token);
        NMP::clampValue(m_quantisationSetQuality, 0.0f, 1.0f);
        status = true;
      }
    }
    else if (stricmp(token, "-fpk") == 0)
    {
      //-----------------------
      // Frames per key pose
      status = false;
      token = strtok(0, seps);
      if (token && token[0] != '-')
      {
        const uint32_t minFPK = 4;
        const uint32_t maxFPK = 12;

        m_framesPerKey = (uint32_t)atoi(token);
        NMP::clampValue(m_framesPerKey, minFPK, maxFPK);
        status = true;
      }
    }

    // Get the next token if required
    if (status)
      token = strtok(0, seps);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorQSA::validateOptions()
{
  NMP_VERIFY(m_channelSetInfoCompressor);
  m_channelSetInfoCompressor->validateOptions();
}

//----------------------------------------------------------------------------------------------------------------------
bool AnimSourceCompressorQSA::getOptionsSampled() const
{
  uint32_t numAnimChannels = getNumAnimChannelSets();
  for (uint32_t i = 0; i < numAnimChannels; ++i)
  {
    if (m_channelSetInfoCompressor->getCompChanMethodForPosChan(i) == ChannelSetInfoCompressorQSA::Spline)
      return false;

    if (m_channelSetInfoCompressor->getCompChanMethodForQuatChan(i) == ChannelSetInfoCompressorQSA::Spline)
      return false;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorQSA::setOptionsSampled()
{
  uint32_t numAnimChannels = getNumAnimChannelSets();
  for (uint32_t i = 0; i < numAnimChannels; ++i)
  {
    // Set the channel method to sampled if it is not an unchanging channel
    m_channelSetInfoCompressor->setDesiredCompChanMethodForPosChan(i, ChannelSetInfoCompressorQSA::Sampled);
    m_channelSetInfoCompressor->setDesiredCompChanMethodForQuatChan(i, ChannelSetInfoCompressorQSA::Sampled);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorQSA::computeSectionBudgets()
{
  NMP_VERIFY(m_inputAnimResampled);
  NMP_VERIFY(m_channelSetInfoCompressor);
  uint32_t numFrames = m_inputAnimResampled->getMaxNumKeyframes();
  NMP_VERIFY(numFrames >= 2);
  uint32_t numChannelSets = m_inputAnimResampled->getNumChannelSets();
  NMP_VERIFY(numChannelSets > 0);
  NMP_VERIFY(m_maxFramesPerSection >= 2);
  NMP_VERIFY(m_desiredCompressionRate > 0.0f);
  uint32_t numSections, guessedNumSections, guessedNumSectionsOld;
  uint32_t framesPerSection;
  size_t budgetSections;
  const size_t budgetMaxSectionSize = MR::getMaxAnimSectionSize();

  //-----------------------
  // Clear out old section budget information
  m_startFrames.clear();
  m_endFrames.clear();
  m_sectionSizes.clear();

  m_memReqsChannelNamesTable.size = computeChannelNamesTableRequirements();
  m_memReqsTrajectorySource.size = computeTrajectoryRequirements();
  m_memReqsAdditionalData.size = computeAdditionalDataRequirements();
  m_memReqsChannelSetInfo.size = computeChannelSetInfoRequirements();

  // Check for an unchanging animation pose
  const ChannelSetRequirements* channelSetReqs = m_inputAnimResampled->getChannelSetRequirements();
  NMP_VERIFY(channelSetReqs);
  if (channelSetReqs->isUnchangingPose())
  {
    m_memReqsAnimSourceHdr.size = computeAnimSourceHeaderRequirements(0);
    return;
  }

  // Compute the desired compression rate in (bytes/bone/frame)
  float rateAnim = m_desiredCompressionRate / m_inputAnimResampled->getSampleFrequency();

  // Compute the desired byte budget for the entire animation
  size_t budgetAnim = (size_t)(rateAnim * numFrames * numChannelSets);

  //-----------------------
  // Compute the byte budget of the animation without any section data
  m_memReqsAnimSourceHdr.size = computeAnimSourceHeaderRequirements(1); // minimum of one section
  size_t budgetAnimExcludingSections =
    m_memReqsAnimSourceHdr.size +
    m_memReqsChannelSetInfo.size +
    m_memReqsTrajectorySource.size +
    m_memReqsChannelNamesTable.size +
    m_memReqsAdditionalData.size;

  //-----------------------
  // Check if the budget is big enough to compile an animation
  size_t budgetSectionsMin = NMP::Memory::align(sizeof(MR::AnimSectionQSA), NMP_VECTOR_ALIGNMENT); // Ensure DMA alignment
  size_t budgetAnimMin = budgetAnimExcludingSections + budgetSectionsMin;
  if (budgetAnimMin > budgetAnim)
  {
    // The animation can't be compressed with the desired compression rate into the
    // minimum required byte budget. Set the minimum allowed animation requirements.
    numSections = 1;
    budgetSections = budgetSectionsMin;
  }
  else
  {
    // Guess the number of sections required for this animation
    guessedNumSections = computeNumSections(m_maxFramesPerSection);
    guessedNumSectionsOld = guessedNumSections;

    //-----------------------
    // Guess the AnimSource header and the channel set information sizes since we don't know the exact
    // number of sections yet. This enables us to compute the byte budget for the section data given
    // the users specified compression rate.
    numSections = 0;
    framesPerSection = m_maxFramesPerSection;
    for (;;)
    {
      //-----------------------
      // Compute the remaining byte budget for all section data
      m_memReqsAnimSourceHdr.size = computeAnimSourceHeaderRequirements(guessedNumSections);
      budgetAnimExcludingSections =
        m_memReqsAnimSourceHdr.size +
        m_memReqsChannelSetInfo.size +
        m_memReqsTrajectorySource.size +
        m_memReqsChannelNamesTable.size +
        m_memReqsAdditionalData.size;

      NMP_VERIFY(budgetAnim > budgetAnimExcludingSections);
      budgetSections = budgetAnim - budgetAnimExcludingSections;

      //-----------------------
      // Compute the effective compression rate (bytes/bone/frame) of the complete sections budget.
      // Note that the boundary frames are repeated between different sections.
      uint32_t numSamples = numFrames + (guessedNumSections - 1); // Add repeated boundary frames
      float rateSections = (float)budgetSections / (float)(numSamples * numChannelSets);

      // Find the byte budget for the maxFramesPerSection limit (DMA aligned)
      size_t budgetMaxSectionFrames = (size_t)(rateSections * numChannelSets * m_maxFramesPerSection);

      // Check if this section byte budget is within the maximum allowed section size
      if (budgetMaxSectionFrames > budgetMaxSectionSize)
      {
        // The budget required for the max section frames is larger than the available max section byte limit.
        // Therefore, find the number of frames corresponding to the maxSectionSize limit
        // The clamp the minimum number of framesPerSection to be 2
        framesPerSection = (uint32_t)(budgetMaxSectionSize / (rateSections * numChannelSets));
        framesPerSection = NMP::maximum(framesPerSection, static_cast<uint32_t>(2));
      }

      //-----------------------
      // Compute the number of sections
      numSections = computeNumSections(framesPerSection);

      // Check for termination
      if (numSections == guessedNumSections)
        break;

      // Check that we are not bouncing between two section sizes
      if (numSections == guessedNumSectionsOld)
      {
        size_t maxSectionBudget0 = distributeSectionSizes(numSections, budgetSections);
        size_t maxSectionBudget1 = distributeSectionSizes(guessedNumSections, budgetSections);
        size_t minValue, maxValue;
        uint32_t minSectionNum, maxSectionNum;
        if (maxSectionBudget0 >= maxSectionBudget1)
        {
          minValue = maxSectionBudget1;
          maxValue = maxSectionBudget0;
          minSectionNum = guessedNumSections;
          maxSectionNum = numSections;
        }
        else
        {
          minValue = maxSectionBudget0;
          maxValue = maxSectionBudget1;
          minSectionNum = numSections;
          maxSectionNum = guessedNumSections;
        }

        // Update the number of sections
        if (maxValue <= budgetMaxSectionSize)
          numSections = maxSectionNum;
        else
          numSections = minSectionNum;

        break;
      }

      // Update the guessed number of sections
      guessedNumSectionsOld = guessedNumSections;
      guessedNumSections = numSections;
    }
  }

#ifdef NMP_ENABLE_ASSERTS
  size_t maxSectionBudget = distributeSectionSizes(numSections, budgetSections);
  NMP_VERIFY(maxSectionBudget <= budgetMaxSectionSize);
#else
  distributeSectionSizes(numSections, budgetSections);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
size_t AnimSourceCompressorQSA::computeAnimSourceHeaderRequirements(uint32_t numSections) const
{
  // Information
  NMP_VERIFY(m_channelSetInfoCompressor);
  uint32_t unchangingPosNumChans = m_channelSetInfoCompressor->getUnchangingPosNumChans();
  uint32_t unchangingQuatNumChans = m_channelSetInfoCompressor->getUnchangingQuatNumChans();
  uint32_t sampledPosNumChans = m_channelSetInfoCompressor->getSampledPosNumChans();
  uint32_t sampledQuatNumChans = m_channelSetInfoCompressor->getSampledQuatNumChans();
  uint32_t splinePosNumChans = m_channelSetInfoCompressor->getSplinePosNumChans();
  uint32_t splineQuatNumChans = m_channelSetInfoCompressor->getSplineQuatNumChans();

  // Header
  NMP::Memory::Format memReqs(sizeof(MR::AnimSourceQSA), NMP_VECTOR_ALIGNMENT);

  // Compression to animation channel maps
  memReqs += MR::CompToAnimChannelMap::getMemoryRequirements(unchangingPosNumChans);
  memReqs += MR::CompToAnimChannelMap::getMemoryRequirements(unchangingQuatNumChans);
  memReqs += MR::CompToAnimChannelMap::getMemoryRequirements(sampledPosNumChans);
  memReqs += MR::CompToAnimChannelMap::getMemoryRequirements(sampledQuatNumChans);
  memReqs += MR::CompToAnimChannelMap::getMemoryRequirements(splinePosNumChans);
  memReqs += MR::CompToAnimChannelMap::getMemoryRequirements(splineQuatNumChans);

  // AnimSectionInfo array (start frames and section sizes)
  NMP::Memory::Format memReqsSectionInfo(sizeof(MR::AnimSectionInfoQSA) * numSections, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqs += memReqsSectionInfo;

  // AnimSectionQSA (section pointers array)
  NMP::Memory::Format memReqsDataRefs(sizeof(MR::DataRef) * numSections, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqs += memReqsDataRefs;

  // Multiple of the DMA alignment
  memReqs.size = NMP::Memory::align(memReqs.size, NMP_VECTOR_ALIGNMENT);

  return memReqs.size;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif
//----------------------------------------------------------------------------------------------------------------------
