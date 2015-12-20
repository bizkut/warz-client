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
#include "assetProcessor/AnimSource/AnimSourceBuilderUtils.h"
#include "assetProcessor/AnimSource/AnimSourceBuilderNSA.h"
#include "assetProcessor/AnimSource/TrajectorySourceCompressorNSA.h"
#include "assetProcessor/AnimSource/AnimSourceCompressorNSA.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// TrajectorySourceCompressorNSA Functions
//----------------------------------------------------------------------------------------------------------------------
TrajectorySourceCompressorNSA::TrajectorySourceCompressorNSA() : TrajectorySourceCompressor()
{
  // Compiled components
  m_sampledTrajectoryDeltaPosKeys = NULL;
  m_sampledTrajectoryDeltaQuatKeys = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
TrajectorySourceCompressorNSA::~TrajectorySourceCompressorNSA()
{
  termCompressor();
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceCompressorNSA::initCompressor()
{
  NMP_VERIFY(m_inputTrajUncompressed);
  const ChannelSetTable* trajChannelSet = m_inputTrajUncompressed->getDeltaTrajectroyChannelSet();
  NMP_VERIFY(trajChannelSet);
  const ChannelSetRequirements* deltaTrajChanSetReqs = m_inputTrajUncompressed->getDeltaTrajectroyChannelSetRequirements();
  NMP_VERIFY(deltaTrajChanSetReqs);
  uint32_t numFrames = trajChannelSet->getNumKeyFrames();
  
  //-----------------------
  // Pos
  if (!deltaTrajChanSetReqs->isChannelPosUnchanging(0))
  {
    // Allocate the memory for the sampled pos key table
    NMP::Memory::Format memReqsTable = AP::Vector3Table::getMemoryRequirements(1, numFrames);
    NMP::Memory::Resource memResTable = NMPMemoryAllocateFromFormat(memReqsTable);
    ZeroMemory(memResTable.ptr, memReqsTable.size);
    Vector3Table* sampledTrajectoryDeltaPosTable = AP::Vector3Table::init(memResTable, 1, numFrames);

    // Allocate the memory for the delta pos key table quantisation info
    NMP::Memory::Format memReqsInfo = AP::Vector3QuantisationTable::getMemoryRequirements(1);
    NMP::Memory::Resource memResInfo = NMPMemoryAllocateFromFormat(memReqsInfo);
    ZeroMemory(memResInfo.ptr, memReqsInfo.size);
    Vector3QuantisationTable* sampledTrajectoryDeltaPosQuantisation = AP::Vector3QuantisationTable::init(memResInfo, 1);
    // Compute the quantisation info
    sampledTrajectoryDeltaPosQuantisation->setPrecisionsX(11);
    sampledTrajectoryDeltaPosQuantisation->setPrecisionsY(11);
    sampledTrajectoryDeltaPosQuantisation->setPrecisionsZ(10);
    
    // Compile the sampled pos quantisation info
    compileSampledPosQuantisationInfo(
      sampledTrajectoryDeltaPosTable,
      sampledTrajectoryDeltaPosQuantisation);
    
    // Compile the sampled pos data
    compileSampledPosData(
      sampledTrajectoryDeltaPosTable,
      sampledTrajectoryDeltaPosQuantisation);
      
    // Check for an unchanging channel after quantisation
    checkForUnchangingChannelPos();
      
    // Tidy up
    NMP::Memory::memFree(sampledTrajectoryDeltaPosTable);
    NMP::Memory::memFree(sampledTrajectoryDeltaPosQuantisation);
  }
  else
  {
    // Compile the unchanging pos quantisation info
    compileUnchangingPosData();
  }
  
  //-----------------------
  // Quat
  if (!deltaTrajChanSetReqs->isChannelQuatUnchanging(0))
  {
    // Allocate the memory for the sampled quat key table
    NMP::Memory::Format memReqsTable = AP::RotVecTable::getMemoryRequirements(1, numFrames);
    NMP::Memory::Resource memResTable = NMPMemoryAllocateFromFormat(memReqsTable);
    ZeroMemory(memResTable.ptr, memReqsTable.size);
    RotVecTable* sampledTrajectoryDeltaQuatTable = AP::RotVecTable::init(memResTable, 1, numFrames);

    // Allocate the memory for the delta quat key table quantisation info
    NMP::Memory::Format memReqsInfo = AP::Vector3QuantisationTable::getMemoryRequirements(1);
    NMP::Memory::Resource memResInfo = NMPMemoryAllocateFromFormat(memReqsInfo);
    ZeroMemory(memResInfo.ptr, memReqsInfo.size);
    Vector3QuantisationTable* sampledTrajectoryDeltaQuatQuantisation = AP::Vector3QuantisationTable::init(memResInfo, 1);
    // Compute the quantisation info
    sampledTrajectoryDeltaQuatQuantisation->setPrecisionsX(16);
    sampledTrajectoryDeltaQuatQuantisation->setPrecisionsY(16);
    sampledTrajectoryDeltaQuatQuantisation->setPrecisionsZ(16);

    // Compile the sampled quat quantisation info
    compileSampledQuatQuantisationInfo(
      sampledTrajectoryDeltaQuatTable,
      sampledTrajectoryDeltaQuatQuantisation);
      
    // Compile the sampled quat data
    compileSampledQuatData(
      sampledTrajectoryDeltaQuatTable,
      sampledTrajectoryDeltaQuatQuantisation);
      
    // Check for an unchanging channel after quantisation
    checkForUnchangingChannelQuat();

    // Tidy up
    NMP::Memory::memFree(sampledTrajectoryDeltaQuatTable);
    NMP::Memory::memFree(sampledTrajectoryDeltaQuatQuantisation);
  }
  else
  {
    // Compile the unchanging pos quantisation info
    compileUnchangingQuatData();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceCompressorNSA::termCompressor()
{
  tidyComponentsPos();
  tidyComponentsQuat();
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceCompressorNSA::compileSampledPosQuantisationInfo(
  Vector3Table* sampledTrajectoryDeltaPosTable,
  Vector3QuantisationTable* sampledTrajectoryDeltaPosQuantisation)
{
  NMP_VERIFY(m_inputTrajUncompressed);
  const ChannelSetTable* trajChannelSet = m_inputTrajUncompressed->getDeltaTrajectroyChannelSet();
  NMP_VERIFY(trajChannelSet);

  // Set the sampled table data
  sampledTrajectoryDeltaPosTable->setChannel(0, trajChannelSet->getChannelPos(0));

  // Compute the quantisation info
  sampledTrajectoryDeltaPosQuantisation->setQuantisationBounds(sampledTrajectoryDeltaPosTable);

  // Compile the sampled pos quantisation information
  NMP::Vector3 qScale = sampledTrajectoryDeltaPosQuantisation->getQuantisationScales(0);
  NMP::Vector3 qOffset = sampledTrajectoryDeltaPosQuantisation->getQuantisationOffsets(0);
  m_sampledTrajectoryDeltaPosKeysInfo.pack(qScale, qOffset);
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceCompressorNSA::compileSampledPosData(
  Vector3Table* sampledTrajectoryDeltaPosTable,
  Vector3QuantisationTable* sampledTrajectoryDeltaPosQuantisation)
{
  NMP_VERIFY(sampledTrajectoryDeltaPosTable);
  NMP_VERIFY(sampledTrajectoryDeltaPosQuantisation);
  NMP_VERIFY(m_inputTrajUncompressed);
  uint32_t numFrames = m_inputTrajUncompressed->getMaxNumKeyframes();
  
  // Compute the stride of the delta pos keys (channel XYZ components)
  uint32_t strideBytes = sampledTrajectoryDeltaPosQuantisation->getPrecisionsBytes();
  NMP_VERIFY(strideBytes == sizeof(MR::SampledPosKey)); // 32 bits (x - 11 bits, y - 11 bits, z - 10 bits)
  NMP::Memory::Format memReqsPos(strideBytes * numFrames, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqsPos.size = NMP::Memory::align(memReqsPos.size, NMP_NATURAL_TYPE_ALIGNMENT);

  // Allocate the memory for the compiled delta pos keys
  NMP_VERIFY(!m_sampledTrajectoryDeltaPosKeys);
  NMP::Memory::Resource memResPos = NMPMemoryAllocateFromFormat(memReqsPos);
  ZeroMemory(memResPos.ptr, memReqsPos.size);
  m_sampledTrajectoryDeltaPosKeys = (MR::SampledPosKey*) memResPos.ptr;

  // Quantise the delta pos samples
  NMP::Vector3 v;
  uint32_t qs[3];
  for (uint32_t i = 0; i < numFrames; ++i)
  {
    sampledTrajectoryDeltaPosTable->getKey(0, i, v);
    sampledTrajectoryDeltaPosQuantisation->quantise(0, v, qs[0], qs[1], qs[2]);
    m_sampledTrajectoryDeltaPosKeys[i].pack(qs);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceCompressorNSA::compileUnchangingPosData()
{
  NMP_VERIFY(m_inputTrajUncompressed);
  const ChannelSetTable* trajChannelSet = m_inputTrajUncompressed->getDeltaTrajectroyChannelSet();
  NMP_VERIFY(trajChannelSet);

  // Set the unchanging pos key quantisation data
  NMP::Vector3 posKey;
  trajChannelSet->getPosKey(0, 0, posKey);
  m_sampledTrajectoryDeltaPosKeysInfo.pack3(posKey.getPtr());
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceCompressorNSA::checkForUnchangingChannelPos()
{
  NMP_VERIFY(m_sampledTrajectoryDeltaPosKeys);
  NMP_VERIFY(m_inputTrajUncompressed);
  uint32_t numFrames = m_inputTrajUncompressed->getMaxNumKeyframes();

  // Allocate the memory for the unpacked quantisation data
  NMP::Memory::Format memReqs(sizeof(uint32_t) * numFrames, NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::Resource memRes;
  memRes = NMPMemoryAllocateFromFormat(memReqs);
  uint32_t* chanX = (uint32_t*)memRes.ptr;
  memRes = NMPMemoryAllocateFromFormat(memReqs);
  uint32_t* chanY = (uint32_t*)memRes.ptr;
  memRes = NMPMemoryAllocateFromFormat(memReqs);
  uint32_t* chanZ = (uint32_t*)memRes.ptr;

  // Unpack the quantised data
  uint32_t qs[3];
  for (uint32_t i = 0; i < numFrames; ++i)
  {
    m_sampledTrajectoryDeltaPosKeys[i].unpack(qs);
    chanX[i] = qs[0];
    chanY[i] = qs[1];
    chanZ[i] = qs[2];
  }

  // Check for an unchanging channel
  bool isUnchanging = checkForUnchangingChannelQuantised(numFrames, chanX);
  if (isUnchanging)
    isUnchanging = checkForUnchangingChannelQuantised(numFrames, chanY);
  if (isUnchanging)
    isUnchanging = checkForUnchangingChannelQuantised(numFrames, chanZ);
  if (isUnchanging)
  {
    tidyComponentsPos();
    compileUnchangingPosData();
  }

  // Tidy up the unpacked quantisation data
  NMP::Memory::memFree(chanX);
  NMP::Memory::memFree(chanY);
  NMP::Memory::memFree(chanZ);
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceCompressorNSA::compileSampledQuatQuantisationInfo(
  RotVecTable* sampledTrajectoryDeltaQuatTable,
  Vector3QuantisationTable* sampledTrajectoryDeltaQuatQuantisation)
{
  NMP_VERIFY(m_inputTrajUncompressed);
  const ChannelSetTable* trajChannelSet = m_inputTrajUncompressed->getDeltaTrajectroyChannelSet();
  NMP_VERIFY(trajChannelSet);

  // Set the sampled table data
  sampledTrajectoryDeltaQuatTable->initChannel(0, trajChannelSet->getChannelQuat(0), true);

  // Compute the quantisation info
  sampledTrajectoryDeltaQuatQuantisation->setQuantisationBounds(sampledTrajectoryDeltaQuatTable);

  // Compile the sampled quat quantisation information
  NMP::Vector3 qScale = sampledTrajectoryDeltaQuatQuantisation->getQuantisationScales(0);
  NMP::Vector3 qOffset = sampledTrajectoryDeltaQuatQuantisation->getQuantisationOffsets(0);
  m_sampledTrajectoryDeltaQuatKeysInfo.pack(qScale, qOffset);
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceCompressorNSA::compileSampledQuatData(
  RotVecTable* sampledTrajectoryDeltaQuatTable,
  Vector3QuantisationTable* sampledTrajectoryDeltaQuatQuantisation)
{
  NMP_VERIFY(sampledTrajectoryDeltaQuatTable);
  NMP_VERIFY(sampledTrajectoryDeltaQuatQuantisation);
  NMP_VERIFY(m_inputTrajUncompressed);
  uint32_t numFrames = m_inputTrajUncompressed->getMaxNumKeyframes();

  // Compute the stride of the delta quat keys
  uint32_t strideBytes = sampledTrajectoryDeltaQuatQuantisation->getPrecisionsBytes();
  NMP_VERIFY(strideBytes == sizeof(MR::SampledQuatKeyTQA)); // 48 bits (x - 16 bits, y - 16 bits, z - 16 bits)
  NMP::Memory::Format memReqsQuat(strideBytes * numFrames, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqsQuat.size = NMP::Memory::align(memReqsQuat.size, NMP_NATURAL_TYPE_ALIGNMENT);

  // Allocate the memory for the compiled delta quat keys
  NMP_VERIFY(!m_sampledTrajectoryDeltaQuatKeys);
  NMP::Memory::Resource memResQuat = NMPMemoryAllocateFromFormat(memReqsQuat);
  ZeroMemory(memResQuat.ptr, memReqsQuat.size);
  m_sampledTrajectoryDeltaQuatKeys = (MR::SampledQuatKeyTQA*) memResQuat.ptr;

  // Quantise the delta quat samples
  NMP::Vector3 v;
  uint32_t qs[3];
  for (uint32_t i = 0; i < numFrames; ++i)
  {
    sampledTrajectoryDeltaQuatTable->getKey(0, i, v);
    sampledTrajectoryDeltaQuatQuantisation->quantise(0, v, qs[0], qs[1], qs[2]);
    m_sampledTrajectoryDeltaQuatKeys[i].pack(qs);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceCompressorNSA::compileUnchangingQuatData()
{
  NMP_VERIFY(m_inputTrajUncompressed);
  const ChannelSetTable* trajChannelSet = m_inputTrajUncompressed->getDeltaTrajectroyChannelSet();
  NMP_VERIFY(trajChannelSet);

  // Set the unchanging quat key quantisation data
  NMP::Quat quatKey;
  trajChannelSet->getQuatKey(0, 0, quatKey);
  m_sampledTrajectoryDeltaQuatKeysInfo.pack4(quatKey.getPtr());
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceCompressorNSA::checkForUnchangingChannelQuat()
{
  NMP_VERIFY(m_sampledTrajectoryDeltaQuatKeys);
  NMP_VERIFY(m_inputTrajUncompressed);
  uint32_t numFrames = m_inputTrajUncompressed->getMaxNumKeyframes();

  // Allocate the memory for the unpacked quantisation data
  NMP::Memory::Format memReqs(sizeof(uint32_t) * numFrames, NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::Resource memRes;
  memRes = NMPMemoryAllocateFromFormat(memReqs);
  uint32_t* chanX = (uint32_t*)memRes.ptr;
  memRes = NMPMemoryAllocateFromFormat(memReqs);
  uint32_t* chanY = (uint32_t*)memRes.ptr;
  memRes = NMPMemoryAllocateFromFormat(memReqs);
  uint32_t* chanZ = (uint32_t*)memRes.ptr;

  // Unpack the quantised data
  uint32_t qs[3];
  for (uint32_t i = 0; i < numFrames; ++i)
  {
    m_sampledTrajectoryDeltaQuatKeys[i].unpack(qs);
    chanX[i] = qs[0];
    chanY[i] = qs[1];
    chanZ[i] = qs[2];
  }

  // Check for an unchanging channel
  bool isUnchanging = checkForUnchangingChannelQuantised(numFrames, chanX);
  if (isUnchanging)
    isUnchanging = checkForUnchangingChannelQuantised(numFrames, chanY);
  if (isUnchanging)
    isUnchanging = checkForUnchangingChannelQuantised(numFrames, chanZ);
  if (isUnchanging)
  {
    tidyComponentsQuat();
    compileUnchangingQuatData();
  }

  // Tidy up the unpacked quantisation data
  NMP::Memory::memFree(chanX);
  NMP::Memory::memFree(chanY);
  NMP::Memory::memFree(chanZ);
}

//----------------------------------------------------------------------------------------------------------------------
size_t TrajectorySourceCompressorNSA::computeTrajectoryRequirements() const
{
  NMP_VERIFY(m_inputTrajUncompressed);
  const ChannelSetTable* trajChannelSet = m_inputTrajUncompressed->getDeltaTrajectroyChannelSet();
  NMP_VERIFY(trajChannelSet);
  uint32_t numFrames = trajChannelSet->getNumKeyFrames();
  
  bool isDeltaPosChanging = (m_sampledTrajectoryDeltaPosKeys != NULL);
  bool isDeltaQuatChanging = (m_sampledTrajectoryDeltaQuatKeys != NULL);
  NMP::Memory::Format memReqs = TrajectorySourceNSABuilder::getMemoryRequirements(
    numFrames,
    isDeltaPosChanging,
    isDeltaQuatChanging);
    
  return memReqs.size;
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceCompressorNSA::buildTrajectorySource(NMP::Memory::Resource& resource) const
{
  NMP_VERIFY(m_inputTrajUncompressed);
  const ChannelSetTable* trajChannelSet = m_inputTrajUncompressed->getDeltaTrajectroyChannelSet();
  NMP_VERIFY(trajChannelSet);
  uint32_t numFrames = trajChannelSet->getNumKeyFrames();
  
  TrajectorySourceNSABuilder::init(
    resource,
    numFrames,
    m_inputTrajUncompressed->getSampleFrequency(),
    m_sampledTrajectoryDeltaPosKeysInfo,
    m_sampledTrajectoryDeltaQuatKeysInfo,
    m_sampledTrajectoryDeltaPosKeys,
    m_sampledTrajectoryDeltaQuatKeys);
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceCompressorNSA::tidyComponentsPos()
{
  if (m_sampledTrajectoryDeltaPosKeys)
  {
    NMP::Memory::memFree(m_sampledTrajectoryDeltaPosKeys);
    m_sampledTrajectoryDeltaPosKeys = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceCompressorNSA::tidyComponentsQuat()
{
  if (m_sampledTrajectoryDeltaQuatKeys)
  {
    NMP::Memory::memFree(m_sampledTrajectoryDeltaQuatKeys);
    m_sampledTrajectoryDeltaQuatKeys = NULL;
  }
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
