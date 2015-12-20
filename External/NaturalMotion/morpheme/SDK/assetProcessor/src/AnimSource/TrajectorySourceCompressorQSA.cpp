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
#include "assetProcessor/AnimSource/TrajectorySourceCompressorQSA.h"
#include "assetProcessor/AnimSource/AnimSourceCompressorQSA.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
class TrajectorySourceQSABuilder : private MR::TrajectorySourceQSA
{
public:
  friend class TrajectorySourceCompressorQSA; // Allow the compressor access to the internal parameters
};

//----------------------------------------------------------------------------------------------------------------------
// TrajectoryPosKeyQSABuilder
//----------------------------------------------------------------------------------------------------------------------
class TrajectoryPosKeyQSABuilder : public MR::TrajectoryPosKeyQSA
{
public:
  static void init(
    MR::TrajectoryPosKeyQSA& data,
    uint32_t x,
    uint32_t y,
    uint32_t z);

public:
  void writeDebug(FILE* filePointer) const;
};

//----------------------------------------------------------------------------------------------------------------------
void TrajectoryPosKeyQSABuilder::init(
  MR::TrajectoryPosKeyQSA& data,
  uint32_t x,
  uint32_t y,
  uint32_t z)
{
  TrajectoryPosKeyQSABuilder& keyBuilder = (TrajectoryPosKeyQSABuilder&)data;
  keyBuilder.m_data = (x << 21) | (y << 10) | z;
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectoryPosKeyQSABuilder::writeDebug(FILE* filePointer) const
{
  uint32_t v[3];  
  v[0] = ((m_data >> 21) & 0x07ff);  // 11 bits
  v[1] = ((m_data >> 10) & 0x07ff);  // 11 bits
  v[2] = (m_data & 0x03ff);   // 10 bits

  fprintf(
    filePointer,
    "[%4d, %4d, %4d] m_data = %x\n",
    v[0], v[1], v[2],
    m_data);
}

//----------------------------------------------------------------------------------------------------------------------
// TrajectoryQuatKeyQSABuilder
//----------------------------------------------------------------------------------------------------------------------
class TrajectoryQuatKeyQSABuilder : public MR::TrajectoryQuatKeyQSA
{
public:
  static void init(
    MR::TrajectoryQuatKeyQSA& data,
    uint32_t x,
    uint32_t y,
    uint32_t z);

public:
  void writeDebug(FILE* filePointer) const;
};

//----------------------------------------------------------------------------------------------------------------------
void TrajectoryQuatKeyQSABuilder::init(
  MR::TrajectoryQuatKeyQSA& data,
  uint32_t x,
  uint32_t y,
  uint32_t z)
{
  TrajectoryQuatKeyQSABuilder& keyBuilder = (TrajectoryQuatKeyQSABuilder&)data;
  keyBuilder.m_data[0] = (uint16_t)x;
  keyBuilder.m_data[1] = (uint16_t)y;
  keyBuilder.m_data[2] = (uint16_t)z;
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectoryQuatKeyQSABuilder::writeDebug(FILE* filePointer) const
{
  fprintf(
    filePointer,
    "[%4d, %4d, %4d]\n",
    m_data[0], m_data[1], m_data[2]);
}

//----------------------------------------------------------------------------------------------------------------------
// TrajectoryKeyInfoQSABuilder
//----------------------------------------------------------------------------------------------------------------------
class TrajectoryKeyInfoQSABuilder : public MR::TrajectoryKeyInfoQSA
{
public:
  static void init(
    MR::TrajectoryKeyInfoQSA& qInfo,
    const NMP::Vector3&       qScale,
    const NMP::Vector3&       qOffset);

  static void init(
    MR::TrajectoryKeyInfoQSA& qInfo,
    const NMP::Quat&          quat);

  static void init(
    MR::TrajectoryKeyInfoQSA& qInfo,
    const NMP::Vector3&       pos);
};

//----------------------------------------------------------------------------------------------------------------------
void TrajectoryKeyInfoQSABuilder::init(
  MR::TrajectoryKeyInfoQSA& qInfo,
  const NMP::Vector3&       qScale,
  const NMP::Vector3&       qOffset)
{
  qInfo.m_scales[0] = qScale.x;
  qInfo.m_scales[1] = qScale.y;
  qInfo.m_scales[2] = qScale.z;

  qInfo.m_offsets[0] = qOffset.x;
  qInfo.m_offsets[1] = qOffset.y;
  qInfo.m_offsets[2] = qOffset.z;
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectoryKeyInfoQSABuilder::init(
  MR::TrajectoryKeyInfoQSA& qInfo,
  const NMP::Quat&          quat)
{
  qInfo.x = quat.x;
  qInfo.y = quat.y;
  qInfo.z = quat.z;
  qInfo.w = quat.w;
  qInfo.m_offsets[1] = 0.0f;
  qInfo.m_offsets[2] = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectoryKeyInfoQSABuilder::init(
  MR::TrajectoryKeyInfoQSA& qInfo,
  const NMP::Vector3&       pos)
{
  qInfo.x = pos.x;
  qInfo.y = pos.y;
  qInfo.z = pos.z;
  qInfo.w = 0.0f;
  qInfo.m_offsets[1] = 0.0f;
  qInfo.m_offsets[2] = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
// TrajectorySourceCompressorQSA Functions
//----------------------------------------------------------------------------------------------------------------------
TrajectorySourceCompressorQSA::TrajectorySourceCompressorQSA(AnimSourceCompressorQSA* manager)
{
  NMP_VERIFY(manager);
  m_manager = manager;

  //-----------------------
  // Sampled delta pos channel
  m_sampledTrajectoryDeltaPosTable = NULL;
  m_sampledTrajectoryDeltaPosQuantisation = NULL;

  //-----------------------
  // Sampled delta quat channel
  m_sampledTrajectoryDeltaQuatTable = NULL;
  m_sampledTrajectoryDeltaQuatQuantisation = NULL;

  //-----------------------
  // Compiled trajectory source
  m_sampledTrajectoryDeltaPosKeys = NULL;
  m_sampledTrajectoryDeltaQuatKeys = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
TrajectorySourceCompressorQSA::~TrajectorySourceCompressorQSA()
{
  termCompressor();
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceCompressorQSA::initCompressor()
{
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceCompressorQSA::termCompressor()
{
  if (m_sampledTrajectoryDeltaPosTable)
  {
    NMP::Memory::memFree(m_sampledTrajectoryDeltaPosTable);
    m_sampledTrajectoryDeltaPosTable = NULL;
  }

  if (m_sampledTrajectoryDeltaPosQuantisation)
  {
    NMP::Memory::memFree(m_sampledTrajectoryDeltaPosQuantisation);
    m_sampledTrajectoryDeltaPosQuantisation = NULL;
  }

  if (m_sampledTrajectoryDeltaQuatTable)
  {
    NMP::Memory::memFree(m_sampledTrajectoryDeltaQuatTable);
    m_sampledTrajectoryDeltaQuatTable = NULL;
  }

  if (m_sampledTrajectoryDeltaQuatQuantisation)
  {
    NMP::Memory::memFree(m_sampledTrajectoryDeltaQuatQuantisation);
    m_sampledTrajectoryDeltaQuatQuantisation = NULL;
  }

  if (m_sampledTrajectoryDeltaPosKeys)
  {
    NMP::Memory::memFree(m_sampledTrajectoryDeltaPosKeys);
    m_sampledTrajectoryDeltaPosKeys = NULL;
  }

  if (m_sampledTrajectoryDeltaQuatKeys)
  {
    NMP::Memory::memFree(m_sampledTrajectoryDeltaQuatKeys);
    m_sampledTrajectoryDeltaQuatKeys = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceCompressorQSA::compileTrajectorySource()
{
  // Compile the delta pos channel
  sampledTrajectoryDeltaPosComputeInfo();
  sampledTrajectoryDeltaQuatComputeInfo();

  // Compile the delta quat channel
  sampledTrajectoryDeltaPosQuantise();
  sampledTrajectoryDeltaQuatQuantise();
}

//----------------------------------------------------------------------------------------------------------------------
size_t TrajectorySourceCompressorQSA::computeTrajectoryRequirements() const
{
  // Check for no trajectory source
  const AnimSourceUncompressed* inputAnim = m_manager->getInputAnimation();
  NMP_VERIFY(inputAnim);
  if (!inputAnim->hasTrajectoryData())
    return 0;

  // Header
  NMP::Memory::Format result(sizeof(MR::TrajectorySourceQSA), NMP_VECTOR_ALIGNMENT);

  // Pos
  NMP::Memory::Format memReqsPos(m_sampledTrajectoryDeltaPosSize, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsPos;

  // Quat
  NMP::Memory::Format memReqsQuat(m_sampledTrajectoryDeltaQuatSize, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsQuat;

  // Multiple of the alignment
  result.size = NMP::Memory::align(result.size, NMP_VECTOR_ALIGNMENT);
  
  return result.size;
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceCompressorQSA::buildTrajectorySource(
  NMP::Memory::Resource& NMP_UNUSED(buffer),
  uint8_t* data) const
{
  // Check for no trajectory source
  const AnimSourceUncompressed* inputAnim = m_manager->getInputAnimation();
  NMP_VERIFY(inputAnim);
  if (!inputAnim->hasTrajectoryData())
    return;

  TrajectorySourceQSABuilder* trajectorySourceBuilder = (TrajectorySourceQSABuilder*)data;
  NMP_VERIFY(trajectorySourceBuilder);
  void* ptr = data;

  // Verify alignment
  NMP_ASSERT_MSG(
    NMP_IS_ALIGNED(trajectorySourceBuilder, NMP_VECTOR_ALIGNMENT),
    "Trajectory sources must be aligned to %d bytes.",
    NMP_VECTOR_ALIGNMENT);

  uint32_t numFrames = m_manager->getMaxNumKeyframes();

  //-----------------------
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(MR::TrajectorySourceQSA), NMP_VECTOR_ALIGNMENT);
  NMP::Memory::alignAndIncrement(ptr, memReqsHdr);

  trajectorySourceBuilder->m_trajType = TRAJ_TYPE_QSA;
  trajectorySourceBuilder->m_getTrajAtTime = MR::TrajectorySourceQSA::computeTrajectoryTransformAtTime;
  trajectorySourceBuilder->m_trajectoryInstanceMemReqs = m_manager->getMemReqsTrajectorySource();

  trajectorySourceBuilder->m_sampleFrequency = inputAnim->getSampleFrequency();
  trajectorySourceBuilder->m_numAnimFrames = (uint16_t)numFrames;
  trajectorySourceBuilder->m_flags = 0x0000;
  trajectorySourceBuilder->m_sampledDeltaPosKeys = NULL;
  trajectorySourceBuilder->m_sampledDeltaQuatKeys = NULL;

  // Quantisation information for the delta trajectory channels
  trajectorySourceBuilder->m_sampledDeltaPosKeysInfo = m_sampledTrajectoryDeltaPosKeysInfo;
  trajectorySourceBuilder->m_sampledDeltaQuatKeysInfo = m_sampledTrajectoryDeltaQuatKeysInfo;

  //-----------------------
  // Pos keys
  if (m_sampledTrajectoryDeltaPosKeys)
  {
    NMP::Memory::Format memReqsPos(m_sampledTrajectoryDeltaPosSize, NMP_NATURAL_TYPE_ALIGNMENT);
    trajectorySourceBuilder->m_sampledDeltaPosKeys = (MR::TrajectoryPosKeyQSA*) NMP::Memory::alignAndIncrement(ptr, memReqsPos);

    trajectorySourceBuilder->m_flags |= (QSA_DELTA_CHAN_SAMPLED << QSA_DELTAPOS_CHAN_METHOD_SHIFT);
    NMP::Memory::memcpy(
      trajectorySourceBuilder->m_sampledDeltaPosKeys,
      m_sampledTrajectoryDeltaPosKeys,
      m_sampledTrajectoryDeltaPosSize);
  }

  // Quat keys
  if (m_sampledTrajectoryDeltaQuatKeys)
  {
    NMP::Memory::Format memReqsQuat(m_sampledTrajectoryDeltaQuatSize, NMP_NATURAL_TYPE_ALIGNMENT);
    trajectorySourceBuilder->m_sampledDeltaQuatKeys = (MR::TrajectoryQuatKeyQSA*) NMP::Memory::alignAndIncrement(ptr, memReqsQuat);

    trajectorySourceBuilder->m_flags |= (QSA_DELTA_CHAN_SAMPLED << QSA_DELTAQUAT_CHAN_METHOD_SHIFT);
    NMP::Memory::memcpy(
      trajectorySourceBuilder->m_sampledDeltaQuatKeys,
      m_sampledTrajectoryDeltaQuatKeys,
      m_sampledTrajectoryDeltaQuatSize);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceCompressorQSA::sampledTrajectoryDeltaPosComputeInfo()
{
  const AnimSourceUncompressed* inputAnim = m_manager->getInputAnimation();
  NMP_VERIFY(inputAnim);
  NMP_VERIFY(inputAnim->hasTrajectoryData());

  const TrajectorySourceUncompressed* uncompressedTrajectory = inputAnim->getTrajectorySource();
  NMP_VERIFY(uncompressedTrajectory);
  const ChannelSetRequirements* deltaTrajChanSetReqs = uncompressedTrajectory->getDeltaTrajectroyChannelSetRequirements();
  NMP_VERIFY(deltaTrajChanSetReqs);
  const ChannelSetTable* trajChannelSet = uncompressedTrajectory->getDeltaTrajectroyChannelSet();
  NMP_VERIFY(trajChannelSet);
  uint32_t numFrames = trajChannelSet->getNumKeyFrames();

  if (!deltaTrajChanSetReqs->isChannelPosUnchanging(0))
  {
    // Allocate the memory for the sampled pos key table
    NMP::Memory::Format memReqsTable = AP::Vector3Table::getMemoryRequirements(1, numFrames);
    NMP::Memory::Resource memResTable = NMPMemoryAllocateFromFormat(memReqsTable);
    m_sampledTrajectoryDeltaPosTable = AP::Vector3Table::init(memResTable, 1, numFrames);

    // Set the sampled table data
    m_sampledTrajectoryDeltaPosTable->setChannel(0, trajChannelSet->getChannelPos(0));

    // Allocate the memory for the delta pos key table quantisation info
    NMP::Memory::Format memReqsInfo = AP::Vector3QuantisationTable::getMemoryRequirements(1);
    NMP::Memory::Resource memResInfo = NMPMemoryAllocateFromFormat(memReqsInfo);
    m_sampledTrajectoryDeltaPosQuantisation = AP::Vector3QuantisationTable::init(memResInfo, 1);

    // Compute the quantisation info
    m_sampledTrajectoryDeltaPosQuantisation->setPrecisionsX(11);
    m_sampledTrajectoryDeltaPosQuantisation->setPrecisionsY(11);
    m_sampledTrajectoryDeltaPosQuantisation->setPrecisionsZ(10);
    m_sampledTrajectoryDeltaPosQuantisation->setQuantisationBounds(m_sampledTrajectoryDeltaPosTable);

    // Set the quantisation information
    NMP::Vector3 scales = m_sampledTrajectoryDeltaPosQuantisation->getQuantisationScales(0);
    NMP::Vector3 offsets = m_sampledTrajectoryDeltaPosQuantisation->getQuantisationOffsets(0);
    TrajectoryKeyInfoQSABuilder::init(m_sampledTrajectoryDeltaPosKeysInfo, scales, offsets);
  }
  else
  {
    // Set the unchanging pos key quantisation data
    NMP::Vector3 posKey;
    trajChannelSet->getPosKey(0, 0, posKey);
    TrajectoryKeyInfoQSABuilder::init(m_sampledTrajectoryDeltaPosKeysInfo, posKey);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceCompressorQSA::sampledTrajectoryDeltaQuatComputeInfo()
{
  const AnimSourceUncompressed* inputAnim = m_manager->getInputAnimation();
  NMP_VERIFY(inputAnim);
  NMP_VERIFY(inputAnim->hasTrajectoryData());

  const TrajectorySourceUncompressed* uncompressedTrajectory = inputAnim->getTrajectorySource();
  NMP_VERIFY(uncompressedTrajectory);
  const ChannelSetRequirements* deltaTrajChanSetReqs = uncompressedTrajectory->getDeltaTrajectroyChannelSetRequirements();
  NMP_VERIFY(deltaTrajChanSetReqs);
  const ChannelSetTable* trajChannelSet = uncompressedTrajectory->getDeltaTrajectroyChannelSet();
  NMP_VERIFY(trajChannelSet);
  uint32_t numFrames = trajChannelSet->getNumKeyFrames();

  if (!deltaTrajChanSetReqs->isChannelQuatUnchanging(0))
  {
    // Allocate the memory for the sampled quat key table
    NMP::Memory::Format memReqsTable = AP::RotVecTable::getMemoryRequirements(1, numFrames);
    NMP::Memory::Resource memResTable = NMPMemoryAllocateFromFormat(memReqsTable);
    m_sampledTrajectoryDeltaQuatTable = AP::RotVecTable::init(memResTable, 1, numFrames);

    // Set the sampled table data
    m_sampledTrajectoryDeltaQuatTable->initChannel(0, trajChannelSet->getChannelQuat(0), true);

    // Allocate the memory for the delta quat key table quantisation info
    NMP::Memory::Format memReqsInfo = AP::Vector3QuantisationTable::getMemoryRequirements(1);
    NMP::Memory::Resource memResInfo = NMPMemoryAllocateFromFormat(memReqsInfo);
    m_sampledTrajectoryDeltaQuatQuantisation = AP::Vector3QuantisationTable::init(memResInfo, 1);

    // Compute the quantisation info
    m_sampledTrajectoryDeltaQuatQuantisation->setPrecisionsX(16);
    m_sampledTrajectoryDeltaQuatQuantisation->setPrecisionsY(16);
    m_sampledTrajectoryDeltaQuatQuantisation->setPrecisionsZ(16);
    m_sampledTrajectoryDeltaQuatQuantisation->setQuantisationBounds(m_sampledTrajectoryDeltaQuatTable);

    // Set the quantisation information
    NMP::Vector3 scales = m_sampledTrajectoryDeltaQuatQuantisation->getQuantisationScales(0);
    NMP::Vector3 offsets = m_sampledTrajectoryDeltaQuatQuantisation->getQuantisationOffsets(0);
    TrajectoryKeyInfoQSABuilder::init(m_sampledTrajectoryDeltaQuatKeysInfo, scales, offsets);
  }
  else
  {
    // Set the unchanging quat key quantisation data
    NMP::Quat quatKey;
    trajChannelSet->getQuatKey(0, 0, quatKey);
    TrajectoryKeyInfoQSABuilder::init(m_sampledTrajectoryDeltaQuatKeysInfo, quatKey);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceCompressorQSA::sampledTrajectoryDeltaPosQuantise()
{
  const AnimSourceUncompressed* inputAnim = m_manager->getInputAnimation();
  NMP_VERIFY(inputAnim);

  if (m_sampledTrajectoryDeltaPosTable)
  {
    NMP::Vector3 v;
    uint32_t qx, qy, qz;

    const TrajectorySourceUncompressed* uncompressedTrajectory = inputAnim->getTrajectorySource();
    NMP_VERIFY(uncompressedTrajectory);
    const ChannelSetTable* trajChannelSet = uncompressedTrajectory->getDeltaTrajectroyChannelSet();
    NMP_VERIFY(trajChannelSet);
    uint32_t numFrames = trajChannelSet->getNumKeyFrames();

    // Compute the stride of the delta pos keys
    NMP_VERIFY(m_sampledTrajectoryDeltaPosQuantisation);
    uint32_t strideBytes = m_sampledTrajectoryDeltaPosQuantisation->getPrecisionsBytes();
    m_sampledTrajectoryDeltaPosSize =
      (uint32_t)NMP::Memory::align(strideBytes * numFrames, NMP_NATURAL_TYPE_ALIGNMENT);

    // Allocate the memory for the compiled delta pos keys
    m_sampledTrajectoryDeltaPosKeys =
      (MR::TrajectoryPosKeyQSA*) NMPMemoryAlloc(m_sampledTrajectoryDeltaPosSize);
    NMP_ASSERT(m_sampledTrajectoryDeltaPosKeys);

    // Quantise the delta pos samples
    for (uint32_t i = 0; i < numFrames; ++i)
    {
      m_sampledTrajectoryDeltaPosTable->getKey(0, i, v);
      m_sampledTrajectoryDeltaPosQuantisation->quantise(0, v, qx, qy, qz);
      TrajectoryPosKeyQSABuilder::init(m_sampledTrajectoryDeltaPosKeys[i], qx, qy, qz);
    }
  }
  else
  {
    m_sampledTrajectoryDeltaPosSize = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceCompressorQSA::sampledTrajectoryDeltaQuatQuantise()
{
  const AnimSourceUncompressed* inputAnim = m_manager->getInputAnimation();
  NMP_VERIFY(inputAnim);

  if (m_sampledTrajectoryDeltaQuatTable)
  {
    NMP::Vector3 v;
    uint32_t qx, qy, qz;

    const TrajectorySourceUncompressed* uncompressedTrajectory = inputAnim->getTrajectorySource();
    NMP_VERIFY(uncompressedTrajectory);
    const ChannelSetTable* trajChannelSet = uncompressedTrajectory->getDeltaTrajectroyChannelSet();
    NMP_VERIFY(trajChannelSet);
    uint32_t numFrames = trajChannelSet->getNumKeyFrames();

    // Compute the stride of the delta quat keys
    NMP_VERIFY(m_sampledTrajectoryDeltaQuatQuantisation);
    uint32_t strideBytes = m_sampledTrajectoryDeltaQuatQuantisation->getPrecisionsBytes();

    // Allocate an extra frame to pad SIMD optimizations
    m_sampledTrajectoryDeltaQuatSize =
      (uint32_t)NMP::Memory::align(strideBytes * (numFrames + 1), NMP_NATURAL_TYPE_ALIGNMENT);

    // Allocate the memory for the compiled delta quat keys
    m_sampledTrajectoryDeltaQuatKeys =
      (MR::TrajectoryQuatKeyQSA*) NMPMemoryAlloc(m_sampledTrajectoryDeltaQuatSize);
    NMP_ASSERT(m_sampledTrajectoryDeltaQuatKeys);

    // Quantise the delta quat samples
    for (uint32_t i = 0; i < numFrames; ++i)
    {
      m_sampledTrajectoryDeltaQuatTable->getKey(0, i, v);
      m_sampledTrajectoryDeltaQuatQuantisation->quantise(0, v, qx, qy, qz);
      TrajectoryQuatKeyQSABuilder::init(m_sampledTrajectoryDeltaQuatKeys[i], qx, qy, qz);
    }

    // Initialize SIMD padding to safe values
    TrajectoryQuatKeyQSABuilder::init(m_sampledTrajectoryDeltaQuatKeys[numFrames], 0, 0, 0);
  }
  else
  {
    m_sampledTrajectoryDeltaQuatSize = 0;
  }
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
