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
#include "NMNumerics/NMQuatSplineFitterTangents.h"
#include "NMNumerics/NMVector3Utils.h"
#include "NMNumerics/NMQuatUtils.h"
#include "NMNumerics/NMRigidMotionTMJacobian.h"
#include "assetProcessor/AnimSource/AnimSourceCompressorQSA.h"
#include "assetProcessor/AnimSource/ChannelSetInfoCompressorQSA.h"
#include "assetProcessor/AnimSource/AnimSectionCompressorQSA.h"
#include "assetProcessor/AnimSource/SplineQuatCompressorQSA.h"

//----------------------------------------------------------------------------------------------------------------------

// For debugging transforms
// #define DEBUG_TRANSFORMS

// Disable warnings about deprecated functions (sprintf, fopen)
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning(disable : 4996)
#endif

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// SplineQuatCompressorQSA Functions
//----------------------------------------------------------------------------------------------------------------------
SplineQuatCompressorQSA::SplineQuatCompressorQSA(AnimSectionCompressorQSA* animSectionCompressor)
{
  NMP_VERIFY(animSectionCompressor);
  m_animSectionCompressor = animSectionCompressor;

  //-----------------------
  // Init the data pointers
  m_knots = NULL;
  m_qspSpan1 = NULL;
  m_splineQuatTableKeys = NULL;
  m_splineQuatTableTangentsA = NULL;
  m_splineQuatTableTangentsB = NULL;
  m_splineQuatKeysMeans = NULL;
  m_splineQuatTangentsMeans = NULL;
  m_splineQuatKeysQuantTable = NULL;
  m_splineQuatTangentsQuantTable = NULL;
  m_splineQuatKeysQuantisationInfo = NULL;
  m_splineQuatKeysQuantisationData = NULL;
  m_splineQuatKeysWNegsData = NULL;
  m_splineQuatKeysData = NULL;
  m_splineQuatTangentsQuantisationInfo = NULL;
  m_splineQuatTangentsQuantisationData = NULL;
  m_splineQuatTangentsData = NULL;

  //-----------------------
  // Init the quat spline fits
  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t numAnimChannels = animSourceCompressor->getNumAnimChannelSets();
  uint32_t numSectionFrames = m_animSectionCompressor->getNumSectionFrames();

  //-----------------------
  // Allocate the memory for the simple knots vector
  uint32_t maxNumKnots = numSectionFrames >> 1;
  if (maxNumKnots < 2)
    maxNumKnots = 2;
  m_knots = new NMP::SimpleKnotVector(maxNumKnots);

  //-----------------------
  // Allocate the memory for the single span spline curve
  m_qspSpan1 = new NMP::QuatSpline(2);

  //-----------------------
  // Allocate the memory for the quat spline channels
  m_channelQuatSplines.resize(numAnimChannels);
  for (uint32_t i = 0; i < numAnimChannels; ++i)
  {
    ChannelSetInfoCompressorQSA::chanMethodType chanMethod;

    chanMethod = channelSetInfoCompressor->getCompChanMethodForQuatChan(i);
    if (chanMethod == ChannelSetInfoCompressorQSA::Spline)
    {
      m_channelQuatSplines[i] = new NMP::QuatSpline(maxNumKnots);
    }
    else
    {
      m_channelQuatSplines[i] = NULL;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
SplineQuatCompressorQSA::~SplineQuatCompressorQSA()
{
  //-----------------------
  // Quaternion splines
  if (m_knots)
  {
    delete m_knots;
    m_knots = NULL;
  }

  if (m_qspSpan1)
  {
    delete m_qspSpan1;
    m_qspSpan1 = NULL;
  }

  uint32_t numChannels = (uint32_t)m_channelQuatSplines.size();
  for (uint32_t i = 0; i < numChannels; ++i)
  {
    if (m_channelQuatSplines[i])
    {
      delete m_channelQuatSplines[i];
    }
  }
  m_channelQuatSplines.clear();

  // Keys relative sample table
  if (m_splineQuatTableKeys)
  {
    NMP::Memory::memFree(m_splineQuatTableKeys);
    m_splineQuatTableKeys = NULL;
  }

  // TangentsA relative sample table
  if (m_splineQuatTableTangentsA)
  {
    NMP::Memory::memFree(m_splineQuatTableTangentsA);
    m_splineQuatTableTangentsA = NULL;
  }

  // TangentsB relative sample table
  if (m_splineQuatTableTangentsB)
  {
    NMP::Memory::memFree(m_splineQuatTableTangentsB);
    m_splineQuatTableTangentsB = NULL;
  }

  // Keys means
  if (m_splineQuatKeysMeans)
  {
    NMP::Memory::memFree(m_splineQuatKeysMeans);
    m_splineQuatKeysMeans = NULL;
  }

  // Tangents means
  if (m_splineQuatTangentsMeans)
  {
    NMP::Memory::memFree(m_splineQuatTangentsMeans);
    m_splineQuatTangentsMeans = NULL;
  }

  // Keys quantiser table
  if (m_splineQuatKeysQuantTable)
  {
    NMP::Memory::memFree(m_splineQuatKeysQuantTable);
    m_splineQuatKeysQuantTable = NULL;
  }

  // Tangents quantiser table
  if (m_splineQuatTangentsQuantTable)
  {
    NMP::Memory::memFree(m_splineQuatTangentsQuantTable);
    m_splineQuatTangentsQuantTable = NULL;
  }

  // Keys Quantisation sets
  m_splineQuatKeysCompToQSetMapX.clear();
  m_splineQuatKeysCompToQSetMapY.clear();
  m_splineQuatKeysCompToQSetMapZ.clear();

  // Tangents quantisation sets
  m_splineQuatTangentsCompToQSetMapX.clear();
  m_splineQuatTangentsCompToQSetMapY.clear();
  m_splineQuatTangentsCompToQSetMapZ.clear();

  //-----------------------
  // Compiled Data
  if (m_splineQuatKeysQuantisationInfo)
  {
    NMP::Memory::memFree(m_splineQuatKeysQuantisationInfo);
    m_splineQuatKeysQuantisationInfo = NULL;
  }

  if (m_splineQuatKeysQuantisationData)
  {
    NMP::Memory::memFree(m_splineQuatKeysQuantisationData);
    m_splineQuatKeysQuantisationData = NULL;
  }

  if (m_splineQuatKeysWNegsData)
  {
    NMP::Memory::memFree(m_splineQuatKeysWNegsData);
    m_splineQuatKeysWNegsData = NULL;
  }

  if (m_splineQuatKeysData)
  {
    NMP::Memory::memFree(m_splineQuatKeysData);
    m_splineQuatKeysData = NULL;
  }

  if (m_splineQuatTangentsQuantisationInfo)
  {
    NMP::Memory::memFree(m_splineQuatTangentsQuantisationInfo);
    m_splineQuatTangentsQuantisationInfo = NULL;
  }

  if (m_splineQuatTangentsQuantisationData)
  {
    NMP::Memory::memFree(m_splineQuatTangentsQuantisationData);
    m_splineQuatTangentsQuantisationData = NULL;
  }

  if (m_splineQuatTangentsData)
  {
    NMP::Memory::memFree(m_splineQuatTangentsData);
    m_splineQuatTangentsData = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SplineQuatCompressorQSA::computeWNegsByteStride()
{
  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t splineQuatNumChannels = channelSetInfoCompressor->getSplineQuatNumChans();
  return (splineQuatNumChannels + 0x07) >> 3;
}

//----------------------------------------------------------------------------------------------------------------------
void SplineQuatCompressorQSA::computeDataStrides()
{
  //-----------------------
  // Info
  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t splineQuatNumChannels = channelSetInfoCompressor->getSplineQuatNumChans();
  NMP_VERIFY(splineQuatNumChannels > 0);
  m_splineQuatKeysNumQuantisationSets = m_animSectionCompressor->getNumQuantisationSets(CoefSetQSA::CoefSetSplineQuatKey);
  m_splineQuatTangentsNumQuantisationSets = m_animSectionCompressor->getNumQuantisationSets(CoefSetQSA::CoefSetSplineQuatTangent);

  QuantisationSetQSA* qSet;
  uint32_t strideBits;

  // WNegs
  m_splineQuatWNegsByteStride = computeWNegsByteStride();

  // Keys
  strideBits = 0;
  for (uint32_t iChan = 0; iChan < splineQuatNumChannels; ++iChan)
  {
    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatKey, m_splineQuatKeysCompToQSetMapX[iChan]);
    const NMP::UniformQuantisationInfo& qInfoX = qSet->getCoefSetX()->getQuantisationInfo();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatKey, m_splineQuatKeysCompToQSetMapY[iChan]);
    const NMP::UniformQuantisationInfo& qInfoY = qSet->getCoefSetY()->getQuantisationInfo();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatKey, m_splineQuatKeysCompToQSetMapZ[iChan]);
    const NMP::UniformQuantisationInfo& qInfoZ = qSet->getCoefSetZ()->getQuantisationInfo();

    strideBits += qInfoX.m_precision;
    strideBits += qInfoY.m_precision;
    strideBits += qInfoZ.m_precision;
  }
  m_splineQuatKeysByteStride = (strideBits + 0x07) >> 3; // Align stride to a byte boundary

  // Tangents (stride of a single tangent plane)
  // i.e. there are two tangent planes: tangentsA, tangentsB which are stacked on top
  // of each other in the memory bitstream layout: [tangentsA(all spans); tangentsB(all spans)]
  strideBits = 0;
  for (uint32_t iChan = 0; iChan < splineQuatNumChannels; ++iChan)
  {
    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatTangent, m_splineQuatTangentsCompToQSetMapX[iChan]);
    const NMP::UniformQuantisationInfo& qInfoX = qSet->getCoefSetX()->getQuantisationInfo();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatTangent, m_splineQuatTangentsCompToQSetMapY[iChan]);
    const NMP::UniformQuantisationInfo& qInfoY = qSet->getCoefSetY()->getQuantisationInfo();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatTangent, m_splineQuatTangentsCompToQSetMapZ[iChan]);
    const NMP::UniformQuantisationInfo& qInfoZ = qSet->getCoefSetZ()->getQuantisationInfo();

    strideBits += qInfoX.m_precision;
    strideBits += qInfoY.m_precision;
    strideBits += qInfoZ.m_precision;
  }
  m_splineQuatTangentsByteStride = (strideBits + 0x07) >> 3; // Align stride to a byte boundary
}

//----------------------------------------------------------------------------------------------------------------------
void SplineQuatCompressorQSA::computeSplines(const NMP::SimpleKnotVector& knots)
{
  NMP_VERIFY(knots.getNumKnots() >= 2);
  NMP_VERIFY(m_knots);
  *m_knots = knots;

  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const MR::RigToAnimEntryMap* rigToAnimEntryMap = animSourceCompressor->getConglomerateRigToAnimEntryMap();
  NMP_VERIFY(rigToAnimEntryMap);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t splineQuatNumChans = channelSetInfoCompressor->getSplineQuatNumChans();
  NMP_VERIFY(splineQuatNumChans > 0);

  // Information
  const std::vector<uint32_t>& splineQuatCompToAnimMap = channelSetInfoCompressor->getSplineQuatCompToAnimMap();

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
  uint32_t numAnimFrames = channelSets->getNumKeyFrames();

  // Create a new quat spline fitter
  NMP::QuatSplineFitterTangents fitter(numAnimFrames); // All anim sample frames
  fitter.setEnableConditioning(false);
  fitter.setKnots(*m_knots); // Fits between the start and end knots of this section

  // Fit quat splines to the required channels
  for (uint32_t iChan = 0; iChan < splineQuatNumChans; ++iChan)
  {
    // Get the rig channel index for the compression channel
    uint16_t animChannelIndex = (uint16_t)splineQuatCompToAnimMap[iChan];
    uint16_t rigChannelIndex;
    bool status = rigToAnimEntryMap->getRigIndexForAnimIndex(animChannelIndex, rigChannelIndex);
    NMP_VERIFY(status);
  
    NMP::QuatSpline* qsp = m_channelQuatSplines[animChannelIndex];
    NMP_VERIFY(qsp);
    const NMP::Quat* quatKeys = channelSets->getChannelQuat(rigChannelIndex);
    NMP_VERIFY(quatKeys);

    fitter.fitRegular(numAnimFrames, quatKeys);
    *qsp = fitter.getQuatSpline();
  }

#ifdef NM_DEBUG
  #ifdef DEBUG_TRANSFORMS
  char buffer[256];
  sprintf(buffer, "C:/quatSplines_%d.dat", m_knots->getNumKnots());
  FILE* filePointer = fopen(buffer, "w");
  NMP_VERIFY(filePointer);
  if (filePointer)
  {
    writeQuatSplines(filePointer);
    fclose(filePointer);
  }
  #endif
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void SplineQuatCompressorQSA::writeDebug(FILE* filePointer)
{
  NMP_VERIFY(filePointer);
  fprintf(filePointer, "\n-----------------------------------------------------------------------------------------------\n");
  fprintf(filePointer, "Quat Spline Compressor Dump\n");
  fprintf(filePointer, "-----------------------------------------------------------------------------------------------\n");

  writeQuatSplines(filePointer);
  writeQuatTableKeys(filePointer);
  writeQuatTableTangentsA(filePointer);
  writeQuatTableTangentsB(filePointer);
  writeQuantisationInfo(filePointer);
  writeGlobalMeans(filePointer);
  writeQuatKeysMeans(filePointer);
  writeQuatTangentsMeans(filePointer);
  writeRecomposedQuatSplines(filePointer);
}

//----------------------------------------------------------------------------------------------------------------------
void SplineQuatCompressorQSA::writeQuatSplines(FILE* filePointer) const
{
  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t splineQuatNumChans = channelSetInfoCompressor->getSplineQuatNumChans();
  NMP_VERIFY(splineQuatNumChans > 0);
  const std::vector<uint32_t>& splineQuatCompToAnimMap = channelSetInfoCompressor->getSplineQuatCompToAnimMap();

  for (uint32_t i = 0; i < splineQuatNumChans; ++i)
  {
    uint32_t animChannelIndex = splineQuatCompToAnimMap[i];
    const NMP::QuatSpline* qsp = m_channelQuatSplines[animChannelIndex];
    NMP_VERIFY(qsp);

    fprintf(filePointer, "\n_______________ Quat splines: anim chan = %d, (comp chan = %d) _______________\n",
            animChannelIndex, i);

    uint32_t numKnots =  qsp->getNumKnots();
    const float* knots = qsp->getKnots();
    const NMP::Quat* keys = qsp->getKeys();
    const NMP::Vector3* velsA = qsp->getVelsA();
    const NMP::Vector3* velsB = qsp->getVelsB();

    fprintf(filePointer, "knots = \n");
    for (uint32_t k = 0; k <  numKnots; ++k)
    {
      fprintf(filePointer, "%4f\n", knots[k]);
    }

    fprintf(filePointer, "\nkeys = \n");
    for (uint32_t k = 0; k < numKnots; ++k)
    {
      fprintf(filePointer, "[%3.5f, %3.5f, %3.5f, %3.5f]\n",
              keys[k].x, keys[k].y, keys[k].z, keys[k].w);
    }

    fprintf(filePointer, "\nvels A = \n");
    for (uint32_t k = 0; k < numKnots - 1; ++k)
    {
      fprintf(filePointer, "[%3.5f, %3.5f, %3.5f]\n",
              velsA[k].x, velsA[k].y, velsA[k].z);
    }

    fprintf(filePointer, "\nvels B = \n");
    for (uint32_t k = 0; k < numKnots - 1; ++k)
    {
      fprintf(filePointer, "[%3.5f, %3.5f, %3.5f]\n",
              velsB[k].x, velsB[k].y, velsB[k].z);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SplineQuatCompressorQSA::writeQuatTableKeys(FILE* filePointer) const
{
  NMP_VERIFY(filePointer);
  fprintf(filePointer, "\n_________________ splineQuatTableKeys _________________\n");
  m_splineQuatTableKeys->writeTableByFrames(filePointer);
}

//----------------------------------------------------------------------------------------------------------------------
void SplineQuatCompressorQSA::writeQuatTableTangentsA(FILE* filePointer) const
{
  NMP_VERIFY(filePointer);
  fprintf(filePointer, "\n_________________ splineQuatTableTangentsA _________________\n");
  m_splineQuatTableTangentsA->writeTableByFrames(filePointer);
}

//----------------------------------------------------------------------------------------------------------------------
void SplineQuatCompressorQSA::writeQuatTableTangentsB(FILE* filePointer) const
{
  NMP_VERIFY(filePointer);
  fprintf(filePointer, "\n_________________ splineQuatTableTangentsB _________________\n");
  m_splineQuatTableTangentsB->writeTableByFrames(filePointer);
}

//----------------------------------------------------------------------------------------------------------------------
void SplineQuatCompressorQSA::writeQuantisationInfo(FILE* filePointer) const
{
  NMP_VERIFY(filePointer);

  uint32_t splineQuatKeysNumQuantisationSets = getSplineQuatKeysNumQuantisationSets();
  uint32_t splineQuatTangentsNumQuantisationSets = getSplineQuatTangentsNumQuantisationSets();
  const MR::QuantisationInfoQSA* qSetInfoKeys = getSplineQuatKeysQuantisationInfo();
  NMP_VERIFY(qSetInfoKeys);
  const MR::QuantisationInfoQSA* qSetInfoTangents = getSplineQuatTangentsQuantisationInfo();
  NMP_VERIFY(qSetInfoTangents);

  for (uint32_t i = 0; i < splineQuatKeysNumQuantisationSets; ++i)
  {
    fprintf(filePointer, "\n_________________ Quantisation Info: qSet = %d _________________\n", i);

    fprintf(filePointer, "qSetInfoKeys: qMin = [%3.5f, %3.5f, %3.5f], qMax = [%3.5f, %3.5f, %3.5f]\n",
            qSetInfoKeys[i].m_qMin[0],
            qSetInfoKeys[i].m_qMin[1],
            qSetInfoKeys[i].m_qMin[2],
            qSetInfoKeys[i].m_qMax[0],
            qSetInfoKeys[i].m_qMax[1],
            qSetInfoKeys[i].m_qMax[2]);
  }

  for (uint32_t i = 0; i < splineQuatTangentsNumQuantisationSets; ++i)
  {
    fprintf(filePointer, "\n_________________ Quantisation Info: qSet = %d _________________\n", i);

    fprintf(filePointer, "qSetInfoTangents: qMin = [%3.5f, %3.5f, %3.5f], qMax = [%3.5f, %3.5f, %3.5f]\n",
            qSetInfoTangents[i].m_qMin[0],
            qSetInfoTangents[i].m_qMin[1],
            qSetInfoTangents[i].m_qMin[2],
            qSetInfoTangents[i].m_qMax[0],
            qSetInfoTangents[i].m_qMax[1],
            qSetInfoTangents[i].m_qMax[2]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SplineQuatCompressorQSA::writeGlobalMeans(FILE* filePointer) const
{
  NMP_VERIFY(filePointer);
  fprintf(filePointer, "\n_________________ Global Means _________________\n");

  fprintf(filePointer, "m_splineQuatTangentMeansInfo: qMin = [%3.5f, %3.5f, %3.5f], qMax = [%3.5f, %3.5f, %3.5f]\n",
          m_splineQuatTangentMeansInfo.m_qMin[0],
          m_splineQuatTangentMeansInfo.m_qMin[1],
          m_splineQuatTangentMeansInfo.m_qMin[2],
          m_splineQuatTangentMeansInfo.m_qMax[0],
          m_splineQuatTangentMeansInfo.m_qMax[1],
          m_splineQuatTangentMeansInfo.m_qMax[2]);
}

//----------------------------------------------------------------------------------------------------------------------
void SplineQuatCompressorQSA::writeQuatKeysMeans(FILE* filePointer) const
{
  NMP_VERIFY(filePointer);
  fprintf(filePointer, "\n_________________ splineQuatKeysMeans _________________\n");

  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t splineQuatNumChans = channelSetInfoCompressor->getSplineQuatNumChans();
  NMP_VERIFY(splineQuatNumChans > 0);

  for (uint32_t iChan = 0; iChan < splineQuatNumChans; ++iChan)
  {
    fprintf(
      filePointer,
      "comp chan = %4d, qVec = [%3.5f, %3.5f, %3.5f, %3.5f]\n",
      iChan,
      m_splineQuatKeysMeans[iChan].x,
      m_splineQuatKeysMeans[iChan].y,
      m_splineQuatKeysMeans[iChan].z,
      m_splineQuatKeysMeans[iChan].w);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SplineQuatCompressorQSA::writeQuatTangentsMeans(FILE* filePointer) const
{
  NMP_VERIFY(filePointer);
  fprintf(filePointer, "\n_________________ splineQuatTangentsMeans _________________\n");

  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t splineQuatNumChans = channelSetInfoCompressor->getSplineQuatNumChans();
  NMP_VERIFY(splineQuatNumChans > 0);

  for (uint32_t iChan = 0; iChan < splineQuatNumChans; ++iChan)
  {
    fprintf(
      filePointer,
      "comp chan = %4d, qVec = [%3.5f, %3.5f, %3.5f]\n",
      iChan,
      m_splineQuatTangentsMeans[iChan].x,
      m_splineQuatTangentsMeans[iChan].y,
      m_splineQuatTangentsMeans[iChan].z);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SplineQuatCompressorQSA::writeRecomposedQuatSplines(FILE* filePointer)
{
  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t splineQuatNumChans = channelSetInfoCompressor->getSplineQuatNumChans();
  NMP_VERIFY(splineQuatNumChans > 0);
  const std::vector<uint32_t>& splineQuatCompToAnimMap = channelSetInfoCompressor->getSplineQuatCompToAnimMap();

  float        ta, tb;
  NMP::Quat    qa, qb;
  NMP::Vector3 wa, wb;

  // To avoid initialisation warnings
  ta = tb = 0;
  qb.identity();

  for (uint32_t i = 0; i < splineQuatNumChans; ++i)
  {
    uint32_t animChannelIndex = splineQuatCompToAnimMap[i];
    uint32_t numKnots = m_knots->getNumKnots();
    uint32_t numSpans = numKnots - 1;

    fprintf(filePointer, "\n_______________ Recomposed Quat splines: anim chan = %d, (comp chan = %d) _______________\n",
            animChannelIndex, i);

    //-----------------------
    // Knots
    fprintf(filePointer, "knots = \n");
    for (uint32_t k = 0; k < numSpans; ++k)
    {
      recompose(i, k, ta, tb, qa, wa, wb, qb);
      fprintf(filePointer, "%4f\n", ta);
    }
    fprintf(filePointer, "%4f\n", tb);

    //-----------------------
    // Keys
    fprintf(filePointer, "\nkeys = \n");
    for (uint32_t k = 0; k < numSpans; ++k)
    {
      recompose(i, k, ta, tb, qa, wa, wb, qb);
      fprintf(filePointer, "[%3.5f, %3.5f, %3.5f, %3.5f]\n",
              qa.x, qa.y, qa.z, qa.w);
    }
    fprintf(filePointer, "[%3.5f, %3.5f, %3.5f, %3.5f]\n",
            qb.x, qb.y, qb.z, qb.w);

    //-----------------------
    // TangentsA
    fprintf(filePointer, "\nvels A = \n");
    for (uint32_t k = 0; k < numSpans; ++k)
    {
      recompose(i, k, ta, tb, qa, wa, wb, qb);
      fprintf(filePointer, "[%3.5f, %3.5f, %3.5f]\n",
              wa.x, wa.y, wa.z);
    }

    //-----------------------
    // TangentsB
    fprintf(filePointer, "\nvels B = \n");
    for (uint32_t k = 0; k < numSpans; ++k)
    {
      recompose(i, k, ta, tb, qa, wa, wb, qb);
      fprintf(filePointer, "[%3.5f, %3.5f, %3.5f]\n",
              wb.x, wb.y, wb.z);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SplineQuatCompressorQSA::computeTangentMeansQuantisationInfo()
{
  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t splineQuatNumChans = channelSetInfoCompressor->getSplineQuatNumChans();
  NMP_VERIFY(splineQuatNumChans > 0);

  const std::vector<uint32_t>& splineQuatCompToAnimMap = channelSetInfoCompressor->getSplineQuatCompToAnimMap();
  uint32_t numKnots = m_knots->getNumKnots();

  //-----------------------
  // Allocate temporary memory for the quantisation table
  NMP::Memory::Format memReqsQuantMeans = AP::Vector3QuantisationTable::getMemoryRequirements(splineQuatNumChans);
  NMP::Memory::Resource memResQuantMeans = NMPMemoryAllocateFromFormat(memReqsQuantMeans);
  Vector3QuantisationTable* quantMeans = AP::Vector3QuantisationTable::init(memResQuantMeans, splineQuatNumChans);

  //-----------------------
  // Iterate over the quat spline channels
  for (uint32_t i = 0; i < splineQuatNumChans; ++i)
  {
    uint32_t animChannelIndex = splineQuatCompToAnimMap[i];
    const NMP::QuatSpline* qsp = m_channelQuatSplines[animChannelIndex];
    NMP_VERIFY(qsp);
    const NMP::Vector3* tangentsA = qsp->getVelsA();
    const NMP::Vector3* tangentsB = qsp->getVelsB();

    quantMeans->setQuantisationBounds(i, numKnots - 1, tangentsA);
    quantMeans->updateQuantisationBounds(i, numKnots - 1, tangentsB);
  }

  //-----------------------
  // Compute the global quantisation info
  NMP::Vector3 qMin, qMax;
  quantMeans->getGlobalQuantisationBounds(qMin, qMax);
  m_splineQuatTangentMeansInfo.m_qMin[0] = qMin.x;
  m_splineQuatTangentMeansInfo.m_qMin[1] = qMin.y;
  m_splineQuatTangentMeansInfo.m_qMin[2] = qMin.z;
  m_splineQuatTangentMeansInfo.m_qMax[0] = qMax.x;
  m_splineQuatTangentMeansInfo.m_qMax[1] = qMax.y;
  m_splineQuatTangentMeansInfo.m_qMax[2] = qMax.z;

  //-----------------------
  // Tidy up
  NMP::Memory::memFree(quantMeans);
}

//----------------------------------------------------------------------------------------------------------------------
void SplineQuatCompressorQSA::computeRateAndDistortion()
{
  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const AnimSourceMotionAnalyser* motionAnalyser = animSourceCompressor->getMotionAnalyser();
  NMP_VERIFY(motionAnalyser);
  float averageBoneLength = motionAnalyser->getAverageBoneLength();

  computeRelativeTransforms();
  assignQuantisationSets();

  // Check if we are trying to compile a set of delta transforms for additive animation
  bool status = false;
  if (animSourceCompressor->getUseWorldSpace() && averageBoneLength > MR::ERROR_LIMIT)
  {
    status = analyseRateAndDistortion();
  }

  // If we failed to compute the analysis use the default channel distribution weightings
  if (!status)
    simpleRateAndDistortion();

  computeRateAndDistortionMapped();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Quat SplineQuatCompressorQSA::evaluateQuantised(uint32_t iChan, uint32_t span, float animFrame)
{
  NMP::Quat    qa, qb, q;
  NMP::Vector3 wa, wb;
  float        ta, tb;

  // Recover the quat spline parameters
  recompose(iChan, span, ta, tb, qa, wa, wb, qb);

  // Set the single span quat spline
  m_qspSpan1->set(ta, tb, qa, wa, wb, qb);

  // Recover the channel quat sample
  q = m_qspSpan1->evaluate(animFrame);

  return q;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Quat SplineQuatCompressorQSA::evaluateQuantised(uint32_t iChan, float t)
{
  NMP::Quat    qa, qb, q;
  NMP::Vector3 wa, wb;
  float        ta, tb;
  uint32_t span;

  m_knots->interval(t, span);

  // Recover the quat spline parameters
  recompose(iChan, span, ta, tb, qa, wa, wb, qb);

  // Set the single span quat spline
  m_qspSpan1->set(ta, tb, qa, wa, wb, qb);

  // Recover the channel quat sample
  q = m_qspSpan1->evaluate(t);

  return q;
}

//----------------------------------------------------------------------------------------------------------------------
void SplineQuatCompressorQSA::recompose(
  uint32_t      iChan,
  uint32_t      span,
  float&        ta,
  float&        tb,
  NMP::Quat&    qa,
  NMP::Vector3& wa,
  NMP::Vector3& wb,
  NMP::Quat&    qb)
{
  NMP::Vector3 qMinKeys, qMaxKeys, qMinTangents, qMaxTangents;
  NMP::Quat    quatKey, keyA, keyB;
  NMP::Vector3 tangentA, tangentB;
  NMP::Vector3 keyARel, keyARel_, keyBRel, keyBRel_;
  NMP::Vector3 tangentARel, tangentARel_, tangentBRel, tangentBRel_;
  uint32_t     wNegKeyA, wNegKeyB;
  uint32_t     qx, qy, qz;
  QuantisationSetQSA* qSet;
  CoefSetQSA* coefSetKeysX;
  CoefSetQSA* coefSetKeysY;
  CoefSetQSA* coefSetKeysZ;
  CoefSetQSA* coefSetTangentsX;
  CoefSetQSA* coefSetTangentsY;
  CoefSetQSA* coefSetTangentsZ;

  //-----------------------
  // Keys: Get the quantisation set corresponding to this compression channel
  qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatKey, m_splineQuatKeysCompToQSetMapX[iChan]);
  coefSetKeysX = qSet->getCoefSetX();
  const NMP::UniformQuantisationInfo& qInfoKeysX = coefSetKeysX->getQuantisationInfo();

  qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatKey, m_splineQuatKeysCompToQSetMapY[iChan]);
  coefSetKeysY = qSet->getCoefSetY();
  const NMP::UniformQuantisationInfo& qInfoKeysY = coefSetKeysY->getQuantisationInfo();

  qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatKey, m_splineQuatKeysCompToQSetMapZ[iChan]);
  coefSetKeysZ = qSet->getCoefSetZ();
  const NMP::UniformQuantisationInfo& qInfoKeysZ = coefSetKeysZ->getQuantisationInfo();

  qMinKeys.set(qInfoKeysX.m_qMin, qInfoKeysY.m_qMin, qInfoKeysZ.m_qMin);
  qMaxKeys.set(qInfoKeysX.m_qMax, qInfoKeysY.m_qMax, qInfoKeysZ.m_qMax);

  // Set the quantiser precision and [qMin, qMax] range
  m_splineQuatKeysQuantTable->setPrecisionX(iChan, qInfoKeysX.m_precision);
  m_splineQuatKeysQuantTable->setPrecisionY(iChan, qInfoKeysY.m_precision);
  m_splineQuatKeysQuantTable->setPrecisionZ(iChan, qInfoKeysZ.m_precision);
  m_splineQuatKeysQuantTable->setQuantisationBounds(iChan, qMinKeys, qMaxKeys);

  //-----------------------
  // Tangents: Get the quantisation set corresponding to this compression channel
  qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatTangent, m_splineQuatTangentsCompToQSetMapX[iChan]);
  coefSetTangentsX = qSet->getCoefSetX();
  const NMP::UniformQuantisationInfo& qInfoTangentsX = coefSetTangentsX->getQuantisationInfo();

  qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatTangent, m_splineQuatTangentsCompToQSetMapY[iChan]);
  coefSetTangentsY = qSet->getCoefSetY();
  const NMP::UniformQuantisationInfo& qInfoTangentsY = coefSetTangentsY->getQuantisationInfo();

  qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatTangent, m_splineQuatTangentsCompToQSetMapZ[iChan]);
  coefSetTangentsZ = qSet->getCoefSetZ();
  const NMP::UniformQuantisationInfo& qInfoTangentsZ = coefSetTangentsZ->getQuantisationInfo();

  qMinTangents.set(qInfoTangentsX.m_qMin, qInfoTangentsY.m_qMin, qInfoTangentsZ.m_qMin);
  qMaxTangents.set(qInfoTangentsX.m_qMax, qInfoTangentsY.m_qMax, qInfoTangentsZ.m_qMax);

  // Set the quantiser precision and [qMin, qMax] range
  m_splineQuatTangentsQuantTable->setPrecisionX(iChan, qInfoTangentsX.m_precision);
  m_splineQuatTangentsQuantTable->setPrecisionY(iChan, qInfoTangentsY.m_precision);
  m_splineQuatTangentsQuantTable->setPrecisionZ(iChan, qInfoTangentsZ.m_precision);
  m_splineQuatTangentsQuantTable->setQuantisationBounds(iChan, qMinTangents, qMaxTangents);

  //-----------------------
  // Quantise the relative samples
  m_splineQuatTableKeys->getKey(iChan, span, keyARel, wNegKeyA);
  m_splineQuatKeysQuantTable->quantise(iChan, keyARel, qx, qy, qz);
  m_splineQuatKeysQuantTable->dequantise(iChan, keyARel_, qx, qy, qz);

  m_splineQuatTableTangentsA->getKey(iChan, span, tangentARel);
  m_splineQuatTangentsQuantTable->quantise(iChan, tangentARel, qx, qy, qz);
  m_splineQuatTangentsQuantTable->dequantise(iChan, tangentARel_, qx, qy, qz);

  m_splineQuatTableTangentsB->getKey(iChan, span, tangentBRel);
  m_splineQuatTangentsQuantTable->quantise(iChan, tangentBRel, qx, qy, qz);
  m_splineQuatTangentsQuantTable->dequantise(iChan, tangentBRel_, qx, qy, qz);

  m_splineQuatTableKeys->getKey(iChan, span + 1, keyBRel, wNegKeyB);
  m_splineQuatKeysQuantTable->quantise(iChan, keyBRel, qx, qy, qz);
  m_splineQuatKeysQuantTable->dequantise(iChan, keyBRel_, qx, qy, qz);

  //-----------------------
  // Recover the quat spline parameters
  ta = m_knots->getKnot(span);
  tb = m_knots->getKnot(span + 1);

  AP::RotVecTable::fromRotationVector(keyA, keyARel_, wNegKeyA, true);
  qa = m_splineQuatKeysMeans[iChan] * keyA;

  wa = m_splineQuatTangentsMeans[iChan] + tangentARel_;

  wb = m_splineQuatTangentsMeans[iChan] + tangentBRel_;

  AP::RotVecTable::fromRotationVector(keyB, keyBRel_, wNegKeyB, true);
  qb = m_splineQuatKeysMeans[iChan] * keyB;
}

//----------------------------------------------------------------------------------------------------------------------
void SplineQuatCompressorQSA::computeRelativeTransforms()
{
  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t splineQuatNumChans = channelSetInfoCompressor->getSplineQuatNumChans();
  NMP_VERIFY(splineQuatNumChans > 0);
  NMP_VERIFY(m_knots);
  uint32_t numKnots = m_knots->getNumKnots();
  NMP_VERIFY(numKnots >= 2);

  const float qMinTQA[3] = { -1.0f, -1.0f, -1.0f };
  const float qMaxTQA[3] = { 1.0f, 1.0f, 1.0f };

  NMP::Vector3 qMin, qMax;
  uint32_t vecSizes[2];
  const NMP::Vector3* vecArrays[2];
  NMP::Quat quatKey, qkbar, invQKbar;
  NMP::Vector3 v, vbar;
  uint32_t qx = 0, qy = 0, qz = 0;

  // Information
  const std::vector<uint32_t>& splineQuatCompToAnimMap = channelSetInfoCompressor->getSplineQuatCompToAnimMap();

  //-----------------------
  // Allocate the memory for the spline quat key table (for relative transforms)
  NMP_VERIFY(!m_splineQuatTableKeys);
  NMP::Memory::Format memReqsTableKeys = AP::RotVecTable::getMemoryRequirements(splineQuatNumChans, numKnots);
  NMP::Memory::Resource memResTableKeys = NMPMemoryAllocateFromFormat(memReqsTableKeys);
  m_splineQuatTableKeys = AP::RotVecTable::init(memResTableKeys, splineQuatNumChans, numKnots);

  //-----------------------
  // Allocate the memory for the channel quat keys means
  NMP_VERIFY(!m_splineQuatKeysMeans);
  NMP::Memory::Format memReqsKeysMeans(0, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqsKeysMeans.size = NMP::Memory::align(sizeof(NMP::Quat) * splineQuatNumChans, NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::Resource memResKeysMeans = NMPMemoryAllocateFromFormat(memReqsKeysMeans);
  m_splineQuatKeysMeans = (NMP::Quat*)memResKeysMeans.ptr;

  //-----------------------
  // Allocate the memory for the spline quat tangent table (for relative transforms)
  NMP_VERIFY(!m_splineQuatTableTangentsA);
  NMP::Memory::Format memReqsTableTangents = AP::Vector3Table::getMemoryRequirements(splineQuatNumChans, numKnots - 1);
  NMP::Memory::Resource memResTableTangentsA = NMPMemoryAllocateFromFormat(memReqsTableTangents);
  m_splineQuatTableTangentsA = AP::Vector3Table::init(memResTableTangentsA, splineQuatNumChans, numKnots - 1);

  NMP_VERIFY(!m_splineQuatTableTangentsB);
  NMP::Memory::Resource memResTableTangentsB = NMPMemoryAllocateFromFormat(memReqsTableTangents);
  m_splineQuatTableTangentsB = AP::Vector3Table::init(memResTableTangentsB, splineQuatNumChans, numKnots - 1);

  //-----------------------
  // Allocate the memory for the channel quat tangent means
  NMP_VERIFY(!m_splineQuatTangentsMeans);
  NMP::Memory::Format memReqsTangentsMeans(0, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqsTangentsMeans.size = NMP::Memory::align(sizeof(NMP::Vector3) * splineQuatNumChans, NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::Resource memResTangentsMeans = NMPMemoryAllocateFromFormat(memReqsTangentsMeans);
  m_splineQuatTangentsMeans = (NMP::Vector3*)memResTangentsMeans.ptr;

  //-----------------------
  // Allocate the memory for the compiled channel quantisation key data (means, precisions, qSets)
  NMP_VERIFY(!m_splineQuatKeysQuantisationData);
  NMP::Memory::Format memReqsQKeyData(0, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqsQKeyData.size = NMP::Memory::align(MR::QuantisationDataQSA::getMemoryRequirementsSize(splineQuatNumChans), NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::Resource memResQKeyData = NMPMemoryAllocateFromFormat(memReqsQKeyData);
  m_splineQuatKeysQuantisationData = (MR::QuantisationDataQSA*)memResQKeyData.ptr;
  m_splineQuatKeysQuantisationDataSize = memReqsQKeyData.size;

  //-----------------------
  // Allocate the memory for the compiled channel quantisation tangent data (means, precisions, qSets)
  NMP_VERIFY(!m_splineQuatTangentsQuantisationData);
  NMP::Memory::Format memReqsQTangentData(0, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqsQTangentData.size = NMP::Memory::align(MR::QuantisationDataQSA::getMemoryRequirementsSize(splineQuatNumChans), NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::Resource memResQTangentData = NMPMemoryAllocateFromFormat(memReqsQTangentData);
  m_splineQuatTangentsQuantisationData = (MR::QuantisationDataQSA*)memResQTangentData.ptr;
  m_splineQuatTangentsQuantisationDataSize = memReqsQTangentData.size;

  //-----------------------
  // Allocate the memory for the quantisation table
  NMP::Memory::Format memReqsQuantQuat = AP::Vector3QuantisationTable::getMemoryRequirements(splineQuatNumChans);
  NMP::Memory::Resource memResQuantQuatKeys = NMPMemoryAllocateFromFormat(memReqsQuantQuat);
  NMP::Memory::Resource memResQuantQuatTangents = NMPMemoryAllocateFromFormat(memReqsQuantQuat);
  m_splineQuatKeysQuantTable = AP::Vector3QuantisationTable::init(memResQuantQuatKeys, splineQuatNumChans);
  m_splineQuatTangentsQuantTable = AP::Vector3QuantisationTable::init(memResQuantQuatTangents, splineQuatNumChans);

  //-----------------------
  // Set the global quat key quantisation bounds
  m_splineQuatKeysQuantTable->setGlobalQuantisationBounds(qMinTQA, qMaxTQA);

  // Set 8-bits of precision for quantising the channel means
  m_splineQuatKeysQuantTable->setPrecisions(8);

  // Compute the spline quat key transforms
  for (uint32_t iChan = 0; iChan < splineQuatNumChans; ++iChan)
  {
    uint32_t animChannelIndex = splineQuatCompToAnimMap[iChan];
    const NMP::QuatSpline* qsp = m_channelQuatSplines[animChannelIndex];
    NMP_VERIFY(qsp);
    const NMP::Quat* keys = qsp->getKeys();
    NMP_VERIFY(keys);

    // Compute the mean of the quat keys data
    NMP::quatMean(numKnots, keys, qkbar);

    // Convert the mean to tan quarter angle rotation vector
    v = qkbar.toRotationVector(true);

    // Quantise the channel mean into 8-bits
    m_splineQuatKeysQuantTable->quantise(iChan, v, qx, qy, qz);
    QuantisationDataQSABuilder::init(m_splineQuatKeysQuantisationData[iChan], qx, qy, qz);

    // Recover the corresponding quantised mean value
    m_splineQuatKeysQuantTable->dequantise(iChan, v, qx, qy, qz);
    m_splineQuatKeysMeans[iChan].fromRotationVector(v, true);

    // Remove the mean from the quat keys data
    invQKbar = ~m_splineQuatKeysMeans[iChan];
    for (uint32_t k = 0; k < numKnots; ++k)
    {
      quatKey = invQKbar * keys[k];
      m_splineQuatTableKeys->setKey(iChan, k, quatKey, true);
    }
  }
  for (uint32_t iChan = splineQuatNumChans; iChan < NMP::nmAlignedValue4(splineQuatNumChans); ++iChan)
  {
    QuantisationDataQSABuilder::init(m_splineQuatKeysQuantisationData[iChan], qx, qy, qz);
  }

  //-----------------------
  // Set the global quat tangent quantisation bounds
  m_splineQuatTangentsQuantTable->setGlobalQuantisationBounds(
    m_splineQuatTangentMeansInfo.m_qMin,
    m_splineQuatTangentMeansInfo.m_qMax);

  // Set 8-bits of precision for quantising the channel means
  m_splineQuatTangentsQuantTable->setPrecisions(8);

  // Compute the spline quat tangent transforms
  for (uint32_t iChan = 0; iChan < splineQuatNumChans; ++iChan)
  {
    uint32_t animChannelIndex = splineQuatCompToAnimMap[iChan];
    const NMP::QuatSpline* qsp = m_channelQuatSplines[animChannelIndex];
    NMP_VERIFY(qsp);
    const NMP::Vector3* tangentsA = qsp->getVelsA();
    NMP_VERIFY(tangentsA);
    const NMP::Vector3* tangentsB = qsp->getVelsB();
    NMP_VERIFY(tangentsB);

    // Compute the mean of the quat tangents data
    vecSizes[0] = numKnots - 1;
    vecArrays[0] = tangentsA;
    vecSizes[1] = numKnots - 1;
    vecArrays[1] = tangentsB;
    NMP::vecMean(2, vecSizes, vecArrays, vbar);

    // Quantise the channel mean into 8-bits
    m_splineQuatTangentsQuantTable->quantise(iChan, vbar, qx, qy, qz);
    QuantisationDataQSABuilder::init(m_splineQuatTangentsQuantisationData[iChan], qx, qy, qz);

    // Recover the corresponding quantised mean value
    m_splineQuatTangentsQuantTable->dequantise(iChan, m_splineQuatTangentsMeans[iChan], qx, qy, qz);

    // Remove the mean from the quat tangents data
    for (uint32_t k = 0; k < numKnots - 1; ++k)
    {
      v = tangentsA[k] - m_splineQuatTangentsMeans[iChan];
      m_splineQuatTableTangentsA->setKey(iChan, k, v);
      v = tangentsB[k] - m_splineQuatTangentsMeans[iChan];
      m_splineQuatTableTangentsB->setKey(iChan, k, v);
    }
  }
  for (uint32_t iChan = splineQuatNumChans; iChan < NMP::nmAlignedValue4(splineQuatNumChans); ++iChan)
  {
    QuantisationDataQSABuilder::init(m_splineQuatTangentsQuantisationData[iChan], qx, qy, qz);
  }

  //-----------------------
  // Compute the quantisation bounds for the relative samples
  m_splineQuatKeysQuantTable->setQuantisationBounds(m_splineQuatTableKeys);
  m_splineQuatTangentsQuantTable->setQuantisationBounds(m_splineQuatTableTangentsA);
  m_splineQuatTangentsQuantTable->updateQuantisationBounds(m_splineQuatTableTangentsB);
}

//----------------------------------------------------------------------------------------------------------------------
void SplineQuatCompressorQSA::assignQuantisationSets()
{
  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t splineQuatNumChans = channelSetInfoCompressor->getSplineQuatNumChans();
  NMP_VERIFY(splineQuatNumChans > 0);
  NMP::Vector3 qMin, qMax;

  //-----------------------
  // Assign the quat key channels to the quantisation sets
  uint32_t numQuantisationSetsKeys = m_animSectionCompressor->getNumQuantisationSets(CoefSetQSA::CoefSetSplineQuatKey);
  NMP_VERIFY(numQuantisationSetsKeys > 0);
  m_splineQuatKeysCompToQSetMapX.resize(splineQuatNumChans);
  m_splineQuatKeysCompToQSetMapY.resize(splineQuatNumChans);
  m_splineQuatKeysCompToQSetMapZ.resize(splineQuatNumChans);

  NMP::UniformQuantisationInfoCompounder qSetCompounderX(splineQuatNumChans);
  NMP::UniformQuantisationInfoCompounder qSetCompounderY(splineQuatNumChans);
  NMP::UniformQuantisationInfoCompounder qSetCompounderZ(splineQuatNumChans);

  for (uint32_t iChan = 0; iChan < splineQuatNumChans; ++iChan)
  {
    m_splineQuatKeysQuantTable->getQuantisationBounds(iChan, qMin, qMax);
    qSetCompounderX.setQuantisationInfo(iChan, qMin.x, qMax.x);
    qSetCompounderY.setQuantisationInfo(iChan, qMin.y, qMax.y);
    qSetCompounderZ.setQuantisationInfo(iChan, qMin.z, qMax.z);
  }

  qSetCompounderX.compoundQuantisationInfo(numQuantisationSetsKeys);
  qSetCompounderY.compoundQuantisationInfo(numQuantisationSetsKeys);
  qSetCompounderZ.compoundQuantisationInfo(numQuantisationSetsKeys);

  for (uint32_t iChan = 0; iChan < splineQuatNumChans; ++iChan)
  {
    m_splineQuatKeysCompToQSetMapX[iChan] = qSetCompounderX.getMapFromInputToCompounded(iChan);
    m_splineQuatKeysCompToQSetMapY[iChan] = qSetCompounderY.getMapFromInputToCompounded(iChan);
    m_splineQuatKeysCompToQSetMapZ[iChan] = qSetCompounderZ.getMapFromInputToCompounded(iChan);
  }

  for (uint32_t qSetIndex = 0; qSetIndex < numQuantisationSetsKeys; ++qSetIndex)
  {
    // Get the coefficient sets
    QuantisationSetQSA* qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatKey, qSetIndex);
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

  //-----------------------
  // Assign the quat tangent channels to the quantisation sets
  uint32_t numQuantisationSetsTangents = m_animSectionCompressor->getNumQuantisationSets(CoefSetQSA::CoefSetSplineQuatTangent);
  NMP_VERIFY(numQuantisationSetsTangents > 0);
  m_splineQuatTangentsCompToQSetMapX.resize(splineQuatNumChans);
  m_splineQuatTangentsCompToQSetMapY.resize(splineQuatNumChans);
  m_splineQuatTangentsCompToQSetMapZ.resize(splineQuatNumChans);

  for (uint32_t iChan = 0; iChan < splineQuatNumChans; ++iChan)
  {
    m_splineQuatTangentsQuantTable->getQuantisationBounds(iChan, qMin, qMax);
    qSetCompounderX.setQuantisationInfo(iChan, qMin.x, qMax.x);
    qSetCompounderY.setQuantisationInfo(iChan, qMin.y, qMax.y);
    qSetCompounderZ.setQuantisationInfo(iChan, qMin.z, qMax.z);
  }

  qSetCompounderX.compoundQuantisationInfo(numQuantisationSetsTangents);
  qSetCompounderY.compoundQuantisationInfo(numQuantisationSetsTangents);
  qSetCompounderZ.compoundQuantisationInfo(numQuantisationSetsTangents);

  for (uint32_t iChan = 0; iChan < splineQuatNumChans; ++iChan)
  {
    m_splineQuatTangentsCompToQSetMapX[iChan] = qSetCompounderX.getMapFromInputToCompounded(iChan);
    m_splineQuatTangentsCompToQSetMapY[iChan] = qSetCompounderY.getMapFromInputToCompounded(iChan);
    m_splineQuatTangentsCompToQSetMapZ[iChan] = qSetCompounderZ.getMapFromInputToCompounded(iChan);
  }

  for (uint32_t qSetIndex = 0; qSetIndex < numQuantisationSetsTangents; ++qSetIndex)
  {
    // Get the coefficient sets
    QuantisationSetQSA* qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatTangent, qSetIndex);
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
void SplineQuatCompressorQSA::simpleRateAndDistortion()
{
  // Clear the rate and distortion information
  m_animSectionCompressor->clearRateAndDistortion(CoefSetQSA::CoefSetSplineQuatKey);
  m_animSectionCompressor->clearRateAndDistortion(CoefSetQSA::CoefSetSplineQuatTangent);

  // Compute the delta to dist mapping
  m_animSectionCompressor->makeSimpleDeltaToDistWeightMap(CoefSetQSA::CoefSetSplineQuatKey);
  m_animSectionCompressor->makeSimpleDeltaToDistWeightMap(CoefSetQSA::CoefSetSplineQuatTangent);
}

//----------------------------------------------------------------------------------------------------------------------
bool SplineQuatCompressorQSA::analyseRateAndDistortion()
{
  NMP::Vector3 qMinKeys, qMaxKeys, qMinTangents, qMaxTangents;
  NMP::Vector3 keyARel, keyARel_, keyBRel, keyBRel_;
  NMP::Vector3 tangentARel, tangentARel_, tangentBRel, tangentBRel_;
  uint32_t wNegKeyA, wNegKeyB;
  NMP::Vector3 dy, quatRel, quatRel_, bonePos;
  NMP::RigidMotionTMJacobian tmJac;
  uint32_t qx, qy, qz;
  float delta, dist;
  QuantisationSetQSA* qSet;
  CoefSetQSA* coefSetKeysX;
  CoefSetQSA* coefSetKeysY;
  CoefSetQSA* coefSetKeysZ;
  CoefSetQSA* coefSetTangentsX;
  CoefSetQSA* coefSetTangentsY;
  CoefSetQSA* coefSetTangentsZ;

  //-----------------------
  // Information
  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t splineQuatNumChans = channelSetInfoCompressor->getSplineQuatNumChans();
  NMP_VERIFY(splineQuatNumChans > 0);
  const std::vector<uint32_t>& splineQuatCompToAnimMap = channelSetInfoCompressor->getSplineQuatCompToAnimMap();
  uint32_t numKnots = m_knots->getNumKnots();
  NMP_VERIFY(numKnots >= 2);
  uint32_t numSpans = numKnots - 1;
  const std::vector<uint32_t>& analysisPrecisions = m_animSectionCompressor->getAnalysisPrecisions();
  NMP_VERIFY(!analysisPrecisions.empty());
  const std::vector<Vector3Table*>& bonePosTables = m_animSectionCompressor->getBonePosTables();

  //-----------------------
  // Clear the rate and distortion information
  m_animSectionCompressor->clearRateAndDistortion(CoefSetQSA::CoefSetSplineQuatKey);
  m_animSectionCompressor->clearRateAndDistortion(CoefSetQSA::CoefSetSplineQuatTangent);

  // Check if we have any child bone tables to analyse
  uint32_t count = 0;
  for (uint32_t iChan = 0; iChan < splineQuatNumChans; ++iChan)
  {
    uint32_t animChannelIndex = splineQuatCompToAnimMap[iChan];
    const Vector3Table* bonePosTable = bonePosTables[animChannelIndex];
    if (bonePosTable)
      ++count;
  }
  if (count == 0)
    return false;

  //-----------------------
  // Iterate over the curve spans
  for (uint32_t span = 0; span < numSpans; ++span)
  {
    //-----------------------
    // Iterate over all compression channels
    for (uint32_t iChan = 0; iChan < splineQuatNumChans; ++iChan)
    {
      // Set the current channel's quat spline
      m_splineQuatTableKeys->getKey(iChan, span, keyARel, wNegKeyA);
      m_splineQuatTableKeys->getKey(iChan, span + 1, keyBRel, wNegKeyB);
      m_splineQuatTableTangentsA->getKey(iChan, span, tangentARel);
      m_splineQuatTableTangentsB->getKey(iChan, span, tangentBRel);

      tmJac.setQuatSpline(
        m_knots->getKnot(span),
        m_knots->getKnot(span + 1),
        m_splineQuatKeysMeans[iChan],
        m_splineQuatTangentsMeans[iChan],
        wNegKeyA != 0,
        wNegKeyB != 0,
        keyARel,
        tangentARel,
        tangentBRel,
        keyBRel);

      //-----------------------
      // Get the table of normalised bone positions for the child joints
      // of the current channel
      uint32_t animChannelIndex = splineQuatCompToAnimMap[iChan];
      uint32_t numBonePosChildren = 0;
      const Vector3Table* bonePosTable = bonePosTables[animChannelIndex];
      if (bonePosTable)
        numBonePosChildren = bonePosTable->getNumChannels(); // Note: End joints have no children

      //-----------------------
      // Keys: Get the quantisation set corresponding to this compression channel
      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatKey, m_splineQuatKeysCompToQSetMapX[iChan]);
      coefSetKeysX = qSet->getCoefSetX();
      const NMP::UniformQuantisationInfo& qInfoKeysX = coefSetKeysX->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatKey, m_splineQuatKeysCompToQSetMapY[iChan]);
      coefSetKeysY = qSet->getCoefSetY();
      const NMP::UniformQuantisationInfo& qInfoKeysY = coefSetKeysY->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatKey, m_splineQuatKeysCompToQSetMapZ[iChan]);
      coefSetKeysZ = qSet->getCoefSetZ();
      const NMP::UniformQuantisationInfo& qInfoKeysZ = coefSetKeysZ->getQuantisationInfo();

      qMinKeys.set(qInfoKeysX.m_qMin, qInfoKeysY.m_qMin, qInfoKeysZ.m_qMin);
      qMaxKeys.set(qInfoKeysX.m_qMax, qInfoKeysY.m_qMax, qInfoKeysZ.m_qMax);

      //-----------------------
      // Tangents: Get the quantisation set corresponding to this compression channel
      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatTangent, m_splineQuatTangentsCompToQSetMapX[iChan]);
      coefSetTangentsX = qSet->getCoefSetX();
      const NMP::UniformQuantisationInfo& qInfoTangentsX = coefSetTangentsX->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatTangent, m_splineQuatTangentsCompToQSetMapY[iChan]);
      coefSetTangentsY = qSet->getCoefSetY();
      const NMP::UniformQuantisationInfo& qInfoTangentsY = coefSetTangentsY->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatTangent, m_splineQuatTangentsCompToQSetMapZ[iChan]);
      coefSetTangentsZ = qSet->getCoefSetZ();
      const NMP::UniformQuantisationInfo& qInfoTangentsZ = coefSetTangentsZ->getQuantisationInfo();

      qMinTangents.set(qInfoTangentsX.m_qMin, qInfoTangentsY.m_qMin, qInfoTangentsZ.m_qMin);
      qMaxTangents.set(qInfoTangentsX.m_qMax, qInfoTangentsY.m_qMax, qInfoTangentsZ.m_qMax);

      //-----------------------
      // Error propagation for the remaining bits of precision
      std::vector<uint32_t>::const_iterator pit = analysisPrecisions.begin();
      std::vector<uint32_t>::const_iterator pit_end = analysisPrecisions.end();
      for (; pit != pit_end; ++pit)
      {
        uint32_t prec = *pit;

        //-----------------------
        // Set the quantiser precision and [qMin, qMax] range
        m_splineQuatKeysQuantTable->setPrecision(iChan, prec);
        m_splineQuatTangentsQuantTable->setPrecision(iChan, prec);
        if (prec > 0)
        {
          m_splineQuatKeysQuantTable->setQuantisationBounds(iChan, qMinKeys, qMaxKeys);
          m_splineQuatTangentsQuantTable->setQuantisationBounds(iChan, qMinTangents, qMaxTangents);
        }
        else
        {
          // Use the mean of the distribution (i.e. zero mean)
          m_splineQuatKeysQuantTable->zeroQuantisationBounds(iChan);
          m_splineQuatTangentsQuantTable->zeroQuantisationBounds(iChan);
        }

        //-----------------------
        // Quantise the relative samples
        m_splineQuatKeysQuantTable->quantise(iChan, keyARel, qx, qy, qz);
        m_splineQuatKeysQuantTable->dequantise(iChan, keyARel_, qx, qy, qz);

        m_splineQuatTangentsQuantTable->quantise(iChan, tangentARel, qx, qy, qz);
        m_splineQuatTangentsQuantTable->dequantise(iChan, tangentARel_, qx, qy, qz);

        m_splineQuatTangentsQuantTable->quantise(iChan, tangentBRel, qx, qy, qz);
        m_splineQuatTangentsQuantTable->dequantise(iChan, tangentBRel_, qx, qy, qz);

        m_splineQuatKeysQuantTable->quantise(iChan, keyBRel, qx, qy, qz);
        m_splineQuatKeysQuantTable->dequantise(iChan, keyBRel_, qx, qy, qz);

        //-----------------------
        // Update the rate of the coefficient sets
        coefSetKeysX->updateRate(prec);
        coefSetKeysY->updateRate(prec); // Start Key
        coefSetKeysZ->updateRate(prec);
        coefSetTangentsX->updateRate(prec, 2);
        coefSetTangentsY->updateRate(prec, 2); // Two tangents
        coefSetTangentsZ->updateRate(prec, 2);

        //-----------------------
        // Iterate over the frames within the curve span
        uint32_t ta = (uint32_t)m_knots->getKnot(span);
        uint32_t tb = (uint32_t)m_knots->getKnot(span + 1);
        for (uint32_t t = ta + 1; t < tb; ++t)
        {
          // Evaluate the quat spline at the current frame
          tmJac.evaluateQuatSpline((float)t);

          // Get the frame within the section
          uint32_t iFrame = t - ta;

          // Iterate over the child bone points of the current channel
          for (uint32_t boneChild = 0; boneChild < numBonePosChildren; ++boneChild)
          {
            // Get the child bone position
            bonePosTable->getKey(boneChild, iFrame, bonePos);
            float dispWeight = bonePos.w;

            // Apply the rigid motion y' = R'*x + t of the current joint to the
            // child bone point to recover its worldspace position at the current frame.
            // We can then compute the error between the quantised y' and the
            // unquantised y positions dy = y' - y. Instead of doing this explicitly
            // (i.e. this is a forward difference Jacobian) we propagate the error
            // through our analytically computed Jacobian: dy = Dy/Dq * dq
            tmJac.transformQuatSplineJac(bonePos); // Compute Dy/Dq at x

            //-----------------------
            // keyARel
            //-----------------------
            // Recover the propagated error: dy = Dy/Dq * dq caused by the quantisation
            // error in keyARel X only
            delta = keyARel_.x - keyARel.x;
            coefSetKeysX->updateDelta(prec, delta * delta);
            dy = tmJac.getErrorQuatSplineKeyA(0, delta);
            dy *= dispWeight; // Apply displacement weight

            // Update the distortion error ||y' - y||^2 for the coefficient set X
            dist = dy.magnitudeSquared();
            coefSetKeysX->updateDistortion(prec, dist);

            //-----------------------
            // Recover the propagated error: dy = Dy/Dq * dq caused by the quantisation
            // error in keyARel Y only
            delta = keyARel_.y - keyARel.y;
            coefSetKeysY->updateDelta(prec, delta * delta);
            dy = tmJac.getErrorQuatSplineKeyA(1, delta);
            dy *= dispWeight; // Apply displacement weight

            // Update the distortion error ||y' - y||^2 for the coefficient set Y
            dist = dy.magnitudeSquared();
            coefSetKeysY->updateDistortion(prec, dist);

            //-----------------------
            // Recover the propagated error: dy = Dy/Dq * dq caused by the quantisation
            // error in keyARel Z only
            delta = keyARel_.z - keyARel.z;
            coefSetKeysZ->updateDelta(prec, delta * delta);
            dy = tmJac.getErrorQuatSplineKeyA(2, delta);
            dy *= dispWeight; // Apply displacement weight

            // Update the distortion error ||y' - y||^2 for the coefficient set Z
            dist = dy.magnitudeSquared();
            coefSetKeysZ->updateDistortion(prec, dist);

            //-----------------------
            // tangentARel
            //-----------------------
            // Recover the propagated error: dy = Dy/Dq * dq caused by the quantisation
            // error in tangentARel X only
            delta = tangentARel_.x - tangentARel.x;
            coefSetTangentsX->updateDelta(prec, delta * delta);
            dy = tmJac.getErrorQuatSplineTangentA(0, delta);
            dy *= dispWeight; // Apply displacement weight

            // Update the distortion error ||y' - y||^2 for the coefficient set X
            dist = dy.magnitudeSquared();
            coefSetTangentsX->updateDistortion(prec, dist);

            //-----------------------
            // Recover the propagated error: dy = Dy/Dq * dq caused by the quantisation
            // error in tangentARel Y only
            delta = tangentARel_.y - tangentARel.y;
            coefSetTangentsY->updateDelta(prec, delta * delta);
            dy = tmJac.getErrorQuatSplineTangentA(1, delta);
            dy *= dispWeight; // Apply displacement weight

            // Update the distortion error ||y' - y||^2 for the coefficient set Y
            dist = dy.magnitudeSquared();
            coefSetTangentsY->updateDistortion(prec, dist);

            //-----------------------
            // Recover the propagated error: dy = Dy/Dq * dq caused by the quantisation
            // error in tangentARel Z only
            delta = tangentARel_.z - tangentARel.z;
            coefSetTangentsZ->updateDelta(prec, delta * delta);
            dy = tmJac.getErrorQuatSplineTangentA(2, delta);
            dy *= dispWeight; // Apply displacement weight

            // Update the distortion error ||y' - y||^2 for the coefficient set Z
            dist = dy.magnitudeSquared();
            coefSetTangentsZ->updateDistortion(prec, dist);

            //-----------------------
            // tangentBRel
            //-----------------------
            // Recover the propagated error: dy = Dy/Dq * dq caused by the quantisation
            // error in tangentBRel X only
            delta = tangentBRel_.x - tangentBRel.x;
            coefSetTangentsX->updateDelta(prec, delta * delta);
            dy = tmJac.getErrorQuatSplineTangentB(0, delta);
            dy *= dispWeight; // Apply displacement weight

            // Update the distortion error ||y' - y||^2 for the coefficient set X
            dist = dy.magnitudeSquared();
            coefSetTangentsX->updateDistortion(prec, dist);

            //-----------------------
            // Recover the propagated error: dy = Dy/Dq * dq caused by the quantisation
            // error in tangentBRel Y only
            delta = tangentBRel_.y - tangentBRel.y;
            coefSetTangentsY->updateDelta(prec, delta * delta);
            dy = tmJac.getErrorQuatSplineTangentB(1, delta);
            dy *= dispWeight; // Apply displacement weight

            // Update the distortion error ||y' - y||^2 for the coefficient set Y
            dist = dy.magnitudeSquared();
            coefSetTangentsY->updateDistortion(prec, dist);

            //-----------------------
            // Recover the propagated error: dy = Dy/Dq * dq caused by the quantisation
            // error in tangentBRel Z only
            delta = tangentBRel_.z - tangentBRel.z;
            coefSetTangentsZ->updateDelta(prec, delta * delta);
            dy = tmJac.getErrorQuatSplineTangentB(2, delta);
            dy *= dispWeight; // Apply displacement weight

            // Update the distortion error ||y' - y||^2 for the coefficient set Z
            dist = dy.magnitudeSquared();
            coefSetTangentsZ->updateDistortion(prec, dist);

            //-----------------------
            // keyBRel
            //-----------------------
            // Recover the propagated error: dy = Dy/Dq * dq caused by the quantisation
            // error in keyBRel X only
            delta = keyBRel_.x - keyBRel.x;
            coefSetKeysX->updateDelta(prec, delta * delta);
            dy = tmJac.getErrorQuatSplineKeyB(0, delta);
            dy *= dispWeight; // Apply displacement weight

            // Update the distortion error ||y' - y||^2 for the coefficient set X
            dist = dy.magnitudeSquared();
            coefSetKeysX->updateDistortion(prec, dist);

            //-----------------------
            // Recover the propagated error: dy = Dy/Dq * dq caused by the quantisation
            // error in keyBRel Y only
            delta = keyBRel_.y - keyBRel.y;
            coefSetKeysY->updateDelta(prec, delta * delta);
            dy = tmJac.getErrorQuatSplineKeyB(1, delta);
            dy *= dispWeight; // Apply displacement weight

            // Update the distortion error ||y' - y||^2 for the coefficient set Y
            dist = dy.magnitudeSquared();
            coefSetKeysY->updateDistortion(prec, dist);

            //-----------------------
            // Recover the propagated error: dy = Dy/Dq * dq caused by the quantisation
            // error in keyBRel Z only
            delta = keyBRel_.z - keyBRel.z;
            coefSetKeysZ->updateDelta(prec, delta * delta);
            dy = tmJac.getErrorQuatSplineKeyB(2, delta);
            dy *= dispWeight; // Apply displacement weight

            // Update the distortion error ||y' - y||^2 for the coefficient set Z
            dist = dy.magnitudeSquared();
            coefSetKeysZ->updateDistortion(prec, dist);

          } // Child bones
        } // Frames
      } // Precisions
    } // Channels
  } // Spans

  //-----------------------
  // Rate for last quat spline keys
  for (uint32_t iChan = 0; iChan < splineQuatNumChans; ++iChan)
  {
    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatKey, m_splineQuatKeysCompToQSetMapX[iChan]);
    coefSetKeysX = qSet->getCoefSetX();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatKey, m_splineQuatKeysCompToQSetMapY[iChan]);
    coefSetKeysY = qSet->getCoefSetY();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatKey, m_splineQuatKeysCompToQSetMapZ[iChan]);
    coefSetKeysZ = qSet->getCoefSetZ();

    // Iterate over the precisions
    std::vector<uint32_t>::const_iterator pit = analysisPrecisions.begin();
    std::vector<uint32_t>::const_iterator pit_end = analysisPrecisions.end();
    for (; pit != pit_end; ++pit)
    {
      uint32_t prec = *pit;
      coefSetKeysX->updateRate(prec);
      coefSetKeysY->updateRate(prec);
      coefSetKeysZ->updateRate(prec);
    }
  }

  //-----------------------
  // Compute the delta to dist mapping
  m_animSectionCompressor->makeDeltaToDistWeightMap(CoefSetQSA::CoefSetSplineQuatKey);
  m_animSectionCompressor->makeDeltaToDistWeightMap(CoefSetQSA::CoefSetSplineQuatTangent);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void SplineQuatCompressorQSA::computeRateAndDistortionMapped()
{
  NMP::Vector3 qMinKeys, qMaxKeys, qMinTangents, qMaxTangents;
  NMP::Vector3 keyARel, keyARel_, keyBRel, keyBRel_;
  NMP::Vector3 tangentARel, tangentARel_, tangentBRel, tangentBRel_;
  uint32_t wNegKeyA, wNegKeyB;
  NMP::Vector3 dy, quatRel, quatRel_, bonePos;
  uint32_t qx, qy, qz;
  float delta;
  QuantisationSetQSA* qSet;
  CoefSetQSA* coefSetKeysX;
  CoefSetQSA* coefSetKeysY;
  CoefSetQSA* coefSetKeysZ;
  CoefSetQSA* coefSetTangentsX;
  CoefSetQSA* coefSetTangentsY;
  CoefSetQSA* coefSetTangentsZ;
  uint32_t maxPrec = m_animSectionCompressor->getMaxPrec();

  //-----------------------
  // Information
  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t splineQuatNumChans = channelSetInfoCompressor->getSplineQuatNumChans();
  NMP_VERIFY(splineQuatNumChans > 0);
  uint32_t numKnots = m_knots->getNumKnots();
  NMP_VERIFY(numKnots >= 2);
  uint32_t numSpans = numKnots - 1;

  //-----------------------
  // Clear the rate and delta information
  m_animSectionCompressor->clearRateAndDelta(CoefSetQSA::CoefSetSplineQuatKey);
  m_animSectionCompressor->clearRateAndDelta(CoefSetQSA::CoefSetSplineQuatTangent);

  //-----------------------
  // Iterate over the curve spans
  for (uint32_t span = 0; span < numSpans; ++span)
  {
    //-----------------------
    // Iterate over all compression channels
    for (uint32_t iChan = 0; iChan < splineQuatNumChans; ++iChan)
    {
      // Get the current channel's quat spline parameters
      m_splineQuatTableKeys->getKey(iChan, span, keyARel, wNegKeyA);
      m_splineQuatTableKeys->getKey(iChan, span + 1, keyBRel, wNegKeyB);
      m_splineQuatTableTangentsA->getKey(iChan, span, tangentARel);
      m_splineQuatTableTangentsB->getKey(iChan, span, tangentBRel);

      //-----------------------
      // Keys: Get the quantisation set corresponding to this compression channel
      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatKey, m_splineQuatKeysCompToQSetMapX[iChan]);
      coefSetKeysX = qSet->getCoefSetX();
      const NMP::UniformQuantisationInfo& qInfoKeysX = coefSetKeysX->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatKey, m_splineQuatKeysCompToQSetMapY[iChan]);
      coefSetKeysY = qSet->getCoefSetY();
      const NMP::UniformQuantisationInfo& qInfoKeysY = coefSetKeysY->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatKey, m_splineQuatKeysCompToQSetMapZ[iChan]);
      coefSetKeysZ = qSet->getCoefSetZ();
      const NMP::UniformQuantisationInfo& qInfoKeysZ = coefSetKeysZ->getQuantisationInfo();

      qMinKeys.set(qInfoKeysX.m_qMin, qInfoKeysY.m_qMin, qInfoKeysZ.m_qMin);
      qMaxKeys.set(qInfoKeysX.m_qMax, qInfoKeysY.m_qMax, qInfoKeysZ.m_qMax);

      //-----------------------
      // Tangents: Get the quantisation set corresponding to this compression channel
      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatTangent, m_splineQuatTangentsCompToQSetMapX[iChan]);
      coefSetTangentsX = qSet->getCoefSetX();
      const NMP::UniformQuantisationInfo& qInfoTangentsX = coefSetTangentsX->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatTangent, m_splineQuatTangentsCompToQSetMapY[iChan]);
      coefSetTangentsY = qSet->getCoefSetY();
      const NMP::UniformQuantisationInfo& qInfoTangentsY = coefSetTangentsY->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatTangent, m_splineQuatTangentsCompToQSetMapZ[iChan]);
      coefSetTangentsZ = qSet->getCoefSetZ();
      const NMP::UniformQuantisationInfo& qInfoTangentsZ = coefSetTangentsZ->getQuantisationInfo();

      qMinTangents.set(qInfoTangentsX.m_qMin, qInfoTangentsY.m_qMin, qInfoTangentsZ.m_qMin);
      qMaxTangents.set(qInfoTangentsX.m_qMax, qInfoTangentsY.m_qMax, qInfoTangentsZ.m_qMax);

      //-----------------------
      // Error propagation for the remaining bits of precision
      for (uint32_t prec = 0; prec <= maxPrec; ++prec)
      {
        //-----------------------
        // Set the quantiser precision and [qMin, qMax] range
        m_splineQuatKeysQuantTable->setPrecision(iChan, prec);
        m_splineQuatTangentsQuantTable->setPrecision(iChan, prec);
        if (prec > 0)
        {
          m_splineQuatKeysQuantTable->setQuantisationBounds(iChan, qMinKeys, qMaxKeys);
          m_splineQuatTangentsQuantTable->setQuantisationBounds(iChan, qMinTangents, qMaxTangents);
        }
        else
        {
          // Use the mean of the distribution (i.e. zero mean)
          m_splineQuatKeysQuantTable->zeroQuantisationBounds(iChan);
          m_splineQuatTangentsQuantTable->zeroQuantisationBounds(iChan);
        }

        //-----------------------
        // Quantise the relative samples
        m_splineQuatKeysQuantTable->quantise(iChan, keyARel, qx, qy, qz);
        m_splineQuatKeysQuantTable->dequantise(iChan, keyARel_, qx, qy, qz);

        m_splineQuatTangentsQuantTable->quantise(iChan, tangentARel, qx, qy, qz);
        m_splineQuatTangentsQuantTable->dequantise(iChan, tangentARel_, qx, qy, qz);

        m_splineQuatTangentsQuantTable->quantise(iChan, tangentBRel, qx, qy, qz);
        m_splineQuatTangentsQuantTable->dequantise(iChan, tangentBRel_, qx, qy, qz);

        m_splineQuatKeysQuantTable->quantise(iChan, keyBRel, qx, qy, qz);
        m_splineQuatKeysQuantTable->dequantise(iChan, keyBRel_, qx, qy, qz);

        //-----------------------
        // Update the rate of the coefficient sets
        coefSetKeysX->updateRate(prec);
        coefSetKeysY->updateRate(prec); // Start Key
        coefSetKeysZ->updateRate(prec);
        coefSetTangentsX->updateRate(prec, 2);
        coefSetTangentsY->updateRate(prec, 2); // Two tangents
        coefSetTangentsZ->updateRate(prec, 2);

        //-----------------------
        // Update the distortion of the coefficient sets
        delta = keyARel_.x - keyARel.x;
        coefSetKeysX->updateDelta(prec, delta * delta);
        delta = keyARel_.y - keyARel.y;
        coefSetKeysY->updateDelta(prec, delta * delta);
        delta = keyARel_.z - keyARel.z;
        coefSetKeysZ->updateDelta(prec, delta * delta);

        delta = tangentARel_.x - tangentARel.x;
        coefSetTangentsX->updateDelta(prec, delta * delta);
        delta = tangentARel_.y - tangentARel.y;
        coefSetTangentsY->updateDelta(prec, delta * delta);
        delta = tangentARel_.z - tangentARel.z;
        coefSetTangentsZ->updateDelta(prec, delta * delta);

        delta = tangentBRel_.x - tangentBRel.x;
        coefSetTangentsX->updateDelta(prec, delta * delta);
        delta = tangentBRel_.y - tangentBRel.y;
        coefSetTangentsY->updateDelta(prec, delta * delta);
        delta = tangentBRel_.z - tangentBRel.z;
        coefSetTangentsZ->updateDelta(prec, delta * delta);

        delta = keyBRel_.x - keyBRel.x;
        coefSetKeysX->updateDelta(prec, delta * delta);
        delta = keyBRel_.y - keyBRel.y;
        coefSetKeysY->updateDelta(prec, delta * delta);
        delta = keyBRel_.z - keyBRel.z;
        coefSetKeysZ->updateDelta(prec, delta * delta);

      } // Precisions
    } // Channels
  } // Spans

  //-----------------------
  // Rate for last quat spline keys
  for (uint32_t iChan = 0; iChan < splineQuatNumChans; ++iChan)
  {
    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatKey, m_splineQuatKeysCompToQSetMapX[iChan]);
    coefSetKeysX = qSet->getCoefSetX();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatKey, m_splineQuatKeysCompToQSetMapY[iChan]);
    coefSetKeysY = qSet->getCoefSetY();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatKey, m_splineQuatKeysCompToQSetMapZ[iChan]);
    coefSetKeysZ = qSet->getCoefSetZ();

    // Iterate over the precisions
    for (uint32_t prec = 0; prec <= maxPrec; ++prec)
    {
      coefSetKeysX->updateRate(prec);
      coefSetKeysY->updateRate(prec);
      coefSetKeysZ->updateRate(prec);
    }
  }

  //-----------------------
  // Compute the mapped distortion
  m_animSectionCompressor->computeRateAndDistortion(CoefSetQSA::CoefSetSplineQuatKey);
  m_animSectionCompressor->computeRateAndDistortion(CoefSetQSA::CoefSetSplineQuatTangent);
}

//----------------------------------------------------------------------------------------------------------------------
void SplineQuatCompressorQSA::compileData()
{
  NMP::Vector3 qMinKeys, qMaxKeys;
  uint32_t     meanKeysX = 0, meanKeysY = 0, meanKeysZ = 0;
  uint32_t     precKeysX = 0, precKeysY = 0, precKeysZ = 0;
  uint32_t     qSetKeysX = 0, qSetKeysY = 0, qSetKeysZ = 0;
  NMP::Vector3 qMinTangents, qMaxTangents;
  uint32_t     meanTangentsX = 0, meanTangentsY = 0, meanTangentsZ = 0;
  uint32_t     precTangentsX = 0, precTangentsY = 0, precTangentsZ = 0;
  uint32_t     qSetTangentsX = 0, qSetTangentsY = 0, qSetTangentsZ = 0;
  NMP::Vector3 keyRel, tangentRel;
  uint32_t wNegKey;
  uint32_t qx, qy, qz;
  QuantisationSetQSA* qSet;
  CoefSetQSA* coefSetKeysX;
  CoefSetQSA* coefSetKeysY;
  CoefSetQSA* coefSetKeysZ;
  CoefSetQSA* coefSetTangentsX;
  CoefSetQSA* coefSetTangentsY;
  CoefSetQSA* coefSetTangentsZ;

  //-----------------------
  // Information
  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t splineQuatNumChans = channelSetInfoCompressor->getSplineQuatNumChans();
  NMP_VERIFY(splineQuatNumChans > 0);
  uint32_t numKnots = m_knots->getNumKnots();
  NMP_VERIFY(numKnots >= 2);
  uint32_t numSpans = numKnots - 1;

  //-----------------------
  // Compute the stride of the sample data
  computeDataStrides();

  //-----------------------
  // Compile the quantisation info ranges
  NMP::Memory::Format memReqsKeysQInfo(0, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqsKeysQInfo.size = NMP::Memory::align(sizeof(MR::QuantisationInfoQSA) * m_splineQuatKeysNumQuantisationSets, NMP_NATURAL_TYPE_ALIGNMENT);

  // Keys
  NMP::Memory::Resource memResKeysQInfo = NMPMemoryAllocateFromFormat(memReqsKeysQInfo);
  m_splineQuatKeysQuantisationInfo = (MR::QuantisationInfoQSA*)memResKeysQInfo.ptr;
  m_splineQuatKeysQuantisationInfoSize = memReqsKeysQInfo.size;

  for (uint32_t i = 0; i < m_splineQuatKeysNumQuantisationSets; ++i)
  {
    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatKey, i);
    coefSetKeysX = qSet->getCoefSetX();
    const NMP::UniformQuantisationInfo& qInfoKeysX = coefSetKeysX->getQuantisationInfo();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatKey, i);
    coefSetKeysY = qSet->getCoefSetY();
    const NMP::UniformQuantisationInfo& qInfoKeysY = coefSetKeysY->getQuantisationInfo();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatKey, i);
    coefSetKeysZ = qSet->getCoefSetZ();
    const NMP::UniformQuantisationInfo& qInfoKeysZ = coefSetKeysZ->getQuantisationInfo();

    qMinKeys.set(qInfoKeysX.m_qMin, qInfoKeysY.m_qMin, qInfoKeysZ.m_qMin);
    qMaxKeys.set(qInfoKeysX.m_qMax, qInfoKeysY.m_qMax, qInfoKeysZ.m_qMax);

    QuantisationInfoQSABuilder::init(m_splineQuatKeysQuantisationInfo[i], qMinKeys, qMaxKeys);
  }

  // Tangents
  NMP::Memory::Format memReqsTangentsQInfo(0, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqsTangentsQInfo.size = NMP::Memory::align(sizeof(MR::QuantisationInfoQSA) * m_splineQuatTangentsNumQuantisationSets, NMP_NATURAL_TYPE_ALIGNMENT);

  NMP::Memory::Resource memResTangentsQInfo = NMPMemoryAllocateFromFormat(memReqsTangentsQInfo);
  m_splineQuatTangentsQuantisationInfo = (MR::QuantisationInfoQSA*)memResTangentsQInfo.ptr;
  m_splineQuatTangentsQuantisationInfoSize = memReqsTangentsQInfo.size;

  for (uint32_t i = 0; i < m_splineQuatTangentsNumQuantisationSets; ++i)
  {
    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatTangent, i);
    coefSetTangentsX = qSet->getCoefSetX();
    const NMP::UniformQuantisationInfo& qInfoTangentsX = coefSetTangentsX->getQuantisationInfo();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatTangent, i);
    coefSetTangentsY = qSet->getCoefSetY();
    const NMP::UniformQuantisationInfo& qInfoTangentsY = coefSetTangentsY->getQuantisationInfo();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatTangent, i);
    coefSetTangentsZ = qSet->getCoefSetZ();
    const NMP::UniformQuantisationInfo& qInfoTangentsZ = coefSetTangentsZ->getQuantisationInfo();

    qMinTangents.set(qInfoTangentsX.m_qMin, qInfoTangentsY.m_qMin, qInfoTangentsZ.m_qMin);
    qMaxTangents.set(qInfoTangentsX.m_qMax, qInfoTangentsY.m_qMax, qInfoTangentsZ.m_qMax);

    QuantisationInfoQSABuilder::init(m_splineQuatTangentsQuantisationInfo[i], qMinTangents, qMaxTangents);
  }

  //-----------------------
  // Compile the quat quantisation data (means, precisions, qSets)
  for (uint32_t iChan = 0; iChan < splineQuatNumChans; ++iChan)
  {
    //-----------------------
    // Keys: Get the quantisation set corresponding to this compression channel
    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatKey, m_splineQuatKeysCompToQSetMapX[iChan]);
    coefSetKeysX = qSet->getCoefSetX();
    const NMP::UniformQuantisationInfo& qInfoKeysX = coefSetKeysX->getQuantisationInfo();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatKey, m_splineQuatKeysCompToQSetMapY[iChan]);
    coefSetKeysY = qSet->getCoefSetY();
    const NMP::UniformQuantisationInfo& qInfoKeysY = coefSetKeysY->getQuantisationInfo();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatKey, m_splineQuatKeysCompToQSetMapZ[iChan]);
    coefSetKeysZ = qSet->getCoefSetZ();
    const NMP::UniformQuantisationInfo& qInfoKeysZ = coefSetKeysZ->getQuantisationInfo();

    qMinKeys.set(qInfoKeysX.m_qMin, qInfoKeysY.m_qMin, qInfoKeysZ.m_qMin);
    qMaxKeys.set(qInfoKeysX.m_qMax, qInfoKeysY.m_qMax, qInfoKeysZ.m_qMax);

    // Keys: Means
    meanKeysX = m_splineQuatKeysQuantisationData[iChan].getMeanX();
    meanKeysY = m_splineQuatKeysQuantisationData[iChan].getMeanY();
    meanKeysZ = m_splineQuatKeysQuantisationData[iChan].getMeanZ();

    // Keys: Precisions
    precKeysX = qInfoKeysX.m_precision;
    precKeysY = qInfoKeysY.m_precision;
    precKeysZ = qInfoKeysZ.m_precision;

    // Keys: Quantisation sets
    qSetKeysX = m_splineQuatKeysCompToQSetMapX[iChan];
    qSetKeysY = m_splineQuatKeysCompToQSetMapY[iChan];
    qSetKeysZ = m_splineQuatKeysCompToQSetMapZ[iChan];

    // Keys: Compile the quantisation data
    QuantisationDataQSABuilder::init(
      m_splineQuatKeysQuantisationData[iChan],
      meanKeysX, meanKeysY, meanKeysZ,
      precKeysX, precKeysY, precKeysZ,
      qSetKeysX, qSetKeysY, qSetKeysZ);

    //-----------------------
    // Tangents: Get the quantisation set corresponding to this compression channel
    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatTangent, m_splineQuatTangentsCompToQSetMapX[iChan]);
    coefSetTangentsX = qSet->getCoefSetX();
    const NMP::UniformQuantisationInfo& qInfoTangentsX = coefSetTangentsX->getQuantisationInfo();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatTangent, m_splineQuatTangentsCompToQSetMapY[iChan]);
    coefSetTangentsY = qSet->getCoefSetY();
    const NMP::UniformQuantisationInfo& qInfoTangentsY = coefSetTangentsY->getQuantisationInfo();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatTangent, m_splineQuatTangentsCompToQSetMapZ[iChan]);
    coefSetTangentsZ = qSet->getCoefSetZ();
    const NMP::UniformQuantisationInfo& qInfoTangentsZ = coefSetTangentsZ->getQuantisationInfo();

    qMinTangents.set(qInfoTangentsX.m_qMin, qInfoTangentsY.m_qMin, qInfoTangentsZ.m_qMin);
    qMaxTangents.set(qInfoTangentsX.m_qMax, qInfoTangentsY.m_qMax, qInfoTangentsZ.m_qMax);

    // Tangents: Means
    meanTangentsX = m_splineQuatTangentsQuantisationData[iChan].getMeanX();
    meanTangentsY = m_splineQuatTangentsQuantisationData[iChan].getMeanY();
    meanTangentsZ = m_splineQuatTangentsQuantisationData[iChan].getMeanZ();

    // Tangents: Precisions
    precTangentsX = qInfoTangentsX.m_precision;
    precTangentsY = qInfoTangentsY.m_precision;
    precTangentsZ = qInfoTangentsZ.m_precision;

    // Tangents: Quantisation sets
    qSetTangentsX = m_splineQuatTangentsCompToQSetMapX[iChan];
    qSetTangentsY = m_splineQuatTangentsCompToQSetMapY[iChan];
    qSetTangentsZ = m_splineQuatTangentsCompToQSetMapZ[iChan];

    // Tangents: Compile the quantisation data
    QuantisationDataQSABuilder::init(
      m_splineQuatTangentsQuantisationData[iChan],
      meanTangentsX, meanTangentsY, meanTangentsZ,
      precTangentsX, precTangentsY, precTangentsZ,
      qSetTangentsX, qSetTangentsY, qSetTangentsZ);
  }
  for (uint32_t iChan = splineQuatNumChans; iChan < NMP::nmAlignedValue4(splineQuatNumChans); ++iChan)
  {
    // Keys: Compile the quantisation data
    QuantisationDataQSABuilder::init(
      m_splineQuatKeysQuantisationData[iChan],
      meanKeysX, meanKeysY, meanKeysZ,
      precKeysX, precKeysY, precKeysZ,
      qSetKeysX, qSetKeysY, qSetKeysZ);

    // Tangents: Compile the quantisation data
    QuantisationDataQSABuilder::init(
      m_splineQuatTangentsQuantisationData[iChan],
      meanTangentsX, meanTangentsY, meanTangentsZ,
      precTangentsX, precTangentsY, precTangentsZ,
      qSetTangentsX, qSetTangentsY, qSetTangentsZ);
  }

  //-----------------------
  // Keys wNegs: Compile the bitstream data
  NMP::BitStreamEncoder encoderKeysWNegs;

  size_t splineQuatKeysWNegsDataSize = m_splineQuatWNegsByteStride * numKnots;
  NMP::Memory::Format memReqsKeysWNegsData = NMP::BitStreamEncoder::getMemoryRequirementsForBuffer(splineQuatKeysWNegsDataSize);
  NMP::Memory::Resource memResKeysWNegsData = NMPMemoryAllocateFromFormat(memReqsKeysWNegsData);
  m_splineQuatKeysWNegsData = (uint8_t*)memResKeysWNegsData.ptr;
  m_splineQuatKeysWNegsDataSize = splineQuatKeysWNegsDataSize;

  encoderKeysWNegs.init(splineQuatKeysWNegsDataSize, m_splineQuatKeysWNegsData);

  for (uint32_t knt = 0; knt < numKnots; ++knt)
  {
    // Check that the bitstream is byte aligned
    NMP_VERIFY(encoderKeysWNegs.isAligned());

#ifdef NMP_ENABLE_ASSERTS
    size_t bytesWrittenStart = encoderKeysWNegs.getBytesWritten();
#endif

    for (uint32_t iChan = 0; iChan < splineQuatNumChans; ++iChan)
    {
      m_splineQuatTableKeys->getKey(iChan, knt, keyRel, wNegKey);
      encoderKeysWNegs.writeBit(wNegKey);
    }

    // Flush the bitstream to the nearest byte boundary
    encoderKeysWNegs.flush();

#ifdef NMP_ENABLE_ASSERTS
    size_t bytesWrittenEnd = encoderKeysWNegs.getBytesWritten();
    size_t channelByteStride = bytesWrittenEnd - bytesWrittenStart;
    NMP_VERIFY(channelByteStride == m_splineQuatWNegsByteStride);
#endif
  }

  // Check we have written the correct amount of data
  NMP_VERIFY(encoderKeysWNegs.getBytesWritten() == splineQuatKeysWNegsDataSize);

  //-----------------------
  // Keys: Compile the bitstream data
  NMP::BitStreamEncoder encoderKeys;

  size_t splineQuatKeysDataSize = m_splineQuatKeysByteStride * numKnots;
  NMP::Memory::Format memReqsKeysData = NMP::BitStreamEncoder::getMemoryRequirementsForBuffer(splineQuatKeysDataSize);
  NMP::Memory::Resource memResKeysData = NMPMemoryAllocateFromFormat(memReqsKeysData);
  m_splineQuatKeysData = (uint8_t*)memResKeysData.ptr;
  m_splineQuatKeysDataSize = splineQuatKeysDataSize;

  encoderKeys.init(splineQuatKeysDataSize, m_splineQuatKeysData);

  for (uint32_t knt = 0; knt < numKnots; ++knt)
  {
    // Check that the bitstream is byte aligned
    NMP_VERIFY(encoderKeys.isAligned());

#ifdef NMP_ENABLE_ASSERTS
    size_t bytesWrittenStart = encoderKeys.getBytesWritten();
#endif

    //-----------------------
    // Iterate over all compression channels
    for (uint32_t iChan = 0; iChan < splineQuatNumChans; ++iChan)
    {
      //-----------------------
      // Keys: Get the quantisation set corresponding to this compression channel
      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatKey, m_splineQuatKeysCompToQSetMapX[iChan]);
      coefSetKeysX = qSet->getCoefSetX();
      const NMP::UniformQuantisationInfo& qInfoKeysX = coefSetKeysX->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatKey, m_splineQuatKeysCompToQSetMapY[iChan]);
      coefSetKeysY = qSet->getCoefSetY();
      const NMP::UniformQuantisationInfo& qInfoKeysY = coefSetKeysY->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatKey, m_splineQuatKeysCompToQSetMapZ[iChan]);
      coefSetKeysZ = qSet->getCoefSetZ();
      const NMP::UniformQuantisationInfo& qInfoKeysZ = coefSetKeysZ->getQuantisationInfo();

      qMinKeys.set(qInfoKeysX.m_qMin, qInfoKeysY.m_qMin, qInfoKeysZ.m_qMin);
      qMaxKeys.set(qInfoKeysX.m_qMax, qInfoKeysY.m_qMax, qInfoKeysZ.m_qMax);

      // Set the quantiser precision and [qMin, qMax] range
      m_splineQuatKeysQuantTable->setPrecisionX(iChan, qInfoKeysX.m_precision);
      m_splineQuatKeysQuantTable->setPrecisionY(iChan, qInfoKeysY.m_precision);
      m_splineQuatKeysQuantTable->setPrecisionZ(iChan, qInfoKeysZ.m_precision);
      m_splineQuatKeysQuantTable->setQuantisationBounds(iChan, qMinKeys, qMaxKeys);

      // Quantise the relative quat sample
      m_splineQuatTableKeys->getKey(iChan, knt, keyRel, wNegKey);
      m_splineQuatKeysQuantTable->quantise(iChan, keyRel, qx, qy, qz);

      // Write the quantised data to the bitstream
      encoderKeys.write(qInfoKeysX.m_precision, qx);
      encoderKeys.write(qInfoKeysY.m_precision, qy);
      encoderKeys.write(qInfoKeysZ.m_precision, qz);
    }

    // Flush the bitstream to the nearest byte boundary
    encoderKeys.flush();

#ifdef NMP_ENABLE_ASSERTS
    size_t bytesWrittenEnd = encoderKeys.getBytesWritten();
    size_t channelByteStride = bytesWrittenEnd - bytesWrittenStart;
    NMP_VERIFY(channelByteStride == m_splineQuatKeysByteStride);
#endif
  } // Knots

  // Check we have written the correct amount of data
  NMP_VERIFY(encoderKeys.getBytesWritten() == splineQuatKeysDataSize);

  //-----------------------
  // Tangents: Compile the bitstream data
  NMP::BitStreamEncoder encoderTangents;

  size_t splineQuatTangentsDataSize = m_splineQuatTangentsByteStride * numSpans * 2;
  NMP::Memory::Format memReqsTangentsData = NMP::BitStreamEncoder::getMemoryRequirementsForBuffer(splineQuatTangentsDataSize);
  NMP::Memory::Resource memResTangentsData = NMPMemoryAllocateFromFormat(memReqsTangentsData);
  m_splineQuatTangentsData = (uint8_t*)memResTangentsData.ptr;
  m_splineQuatTangentsDataSize = splineQuatTangentsDataSize;

  encoderTangents.init(splineQuatTangentsDataSize, m_splineQuatTangentsData);

  //-----------------------
  // TangentsA
  for (uint32_t span = 0; span < numSpans; ++span)
  {
    // Check that the bitstream is byte aligned
    NMP_VERIFY(encoderTangents.isAligned());

#ifdef NMP_ENABLE_ASSERTS
    size_t bytesWrittenStart = encoderTangents.getBytesWritten();
#endif

    //-----------------------
    // Iterate over all compression channels
    for (uint32_t iChan = 0; iChan < splineQuatNumChans; ++iChan)
    {
      //-----------------------
      // Tangents: Get the quantisation set corresponding to this compression channel
      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatTangent, m_splineQuatTangentsCompToQSetMapX[iChan]);
      coefSetTangentsX = qSet->getCoefSetX();
      const NMP::UniformQuantisationInfo& qInfoTangentsX = coefSetTangentsX->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatTangent, m_splineQuatTangentsCompToQSetMapY[iChan]);
      coefSetTangentsY = qSet->getCoefSetY();
      const NMP::UniformQuantisationInfo& qInfoTangentsY = coefSetTangentsY->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatTangent, m_splineQuatTangentsCompToQSetMapZ[iChan]);
      coefSetTangentsZ = qSet->getCoefSetZ();
      const NMP::UniformQuantisationInfo& qInfoTangentsZ = coefSetTangentsZ->getQuantisationInfo();

      qMinTangents.set(qInfoTangentsX.m_qMin, qInfoTangentsY.m_qMin, qInfoTangentsZ.m_qMin);
      qMaxTangents.set(qInfoTangentsX.m_qMax, qInfoTangentsY.m_qMax, qInfoTangentsZ.m_qMax);

      // Set the quantiser precision and [qMin, qMax] range
      m_splineQuatTangentsQuantTable->setPrecisionX(iChan, qInfoTangentsX.m_precision);
      m_splineQuatTangentsQuantTable->setPrecisionY(iChan, qInfoTangentsY.m_precision);
      m_splineQuatTangentsQuantTable->setPrecisionZ(iChan, qInfoTangentsZ.m_precision);
      m_splineQuatTangentsQuantTable->setQuantisationBounds(iChan, qMinTangents, qMaxTangents);

      // Quantise the relative quat tangent A
      m_splineQuatTableTangentsA->getKey(iChan, span, tangentRel);
      m_splineQuatTangentsQuantTable->quantise(iChan, tangentRel, qx, qy, qz);

      // Write the quantised data to the bitstream
      encoderTangents.write(qInfoTangentsX.m_precision, qx);
      encoderTangents.write(qInfoTangentsY.m_precision, qy);
      encoderTangents.write(qInfoTangentsZ.m_precision, qz);
    }

    // Flush the bitstream to the nearest byte boundary
    encoderTangents.flush();

#ifdef NMP_ENABLE_ASSERTS
    size_t bytesWrittenEnd = encoderTangents.getBytesWritten();
    size_t channelByteStride = bytesWrittenEnd - bytesWrittenStart;
    NMP_VERIFY(channelByteStride == m_splineQuatTangentsByteStride);
#endif
  }

  //-----------------------
  // TangentsB
  for (uint32_t span = 0; span < numSpans; ++span)
  {
    // Check that the bitstream is byte aligned
    NMP_VERIFY(encoderTangents.isAligned());

#ifdef NMP_ENABLE_ASSERTS
    size_t bytesWrittenStart = encoderTangents.getBytesWritten();
#endif

    //-----------------------
    // Iterate over all compression channels
    for (uint32_t iChan = 0; iChan < splineQuatNumChans; ++iChan)
    {
      //-----------------------
      // Tangents: Get the quantisation set corresponding to this compression channel
      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatTangent, m_splineQuatTangentsCompToQSetMapX[iChan]);
      coefSetTangentsX = qSet->getCoefSetX();
      const NMP::UniformQuantisationInfo& qInfoTangentsX = coefSetTangentsX->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatTangent, m_splineQuatTangentsCompToQSetMapY[iChan]);
      coefSetTangentsY = qSet->getCoefSetY();
      const NMP::UniformQuantisationInfo& qInfoTangentsY = coefSetTangentsY->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplineQuatTangent, m_splineQuatTangentsCompToQSetMapZ[iChan]);
      coefSetTangentsZ = qSet->getCoefSetZ();
      const NMP::UniformQuantisationInfo& qInfoTangentsZ = coefSetTangentsZ->getQuantisationInfo();

      qMinTangents.set(qInfoTangentsX.m_qMin, qInfoTangentsY.m_qMin, qInfoTangentsZ.m_qMin);
      qMaxTangents.set(qInfoTangentsX.m_qMax, qInfoTangentsY.m_qMax, qInfoTangentsZ.m_qMax);

      // Set the quantiser precision and [qMin, qMax] range
      m_splineQuatTangentsQuantTable->setPrecisionX(iChan, qInfoTangentsX.m_precision);
      m_splineQuatTangentsQuantTable->setPrecisionY(iChan, qInfoTangentsY.m_precision);
      m_splineQuatTangentsQuantTable->setPrecisionZ(iChan, qInfoTangentsZ.m_precision);
      m_splineQuatTangentsQuantTable->setQuantisationBounds(iChan, qMinTangents, qMaxTangents);

      // Quantise the relative quat tangent B
      m_splineQuatTableTangentsB->getKey(iChan, span, tangentRel);
      m_splineQuatTangentsQuantTable->quantise(iChan, tangentRel, qx, qy, qz);

      // Write the quantised data to the bitstream
      encoderTangents.write(qInfoTangentsX.m_precision, qx);
      encoderTangents.write(qInfoTangentsY.m_precision, qy);
      encoderTangents.write(qInfoTangentsZ.m_precision, qz);
    }

    // Flush the bitstream to the nearest byte boundary
    encoderTangents.flush();

#ifdef NMP_ENABLE_ASSERTS
    size_t bytesWrittenEnd = encoderTangents.getBytesWritten();
    size_t channelByteStride = bytesWrittenEnd - bytesWrittenStart;
    NMP_VERIFY(channelByteStride == m_splineQuatTangentsByteStride);
#endif
  }

  // Check we have written the correct amount of data
  NMP_VERIFY(encoderTangents.getBytesWritten() == splineQuatTangentsDataSize);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif
//----------------------------------------------------------------------------------------------------------------------
