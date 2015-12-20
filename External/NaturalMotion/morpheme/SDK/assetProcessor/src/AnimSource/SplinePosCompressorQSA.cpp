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
#include "NMNumerics/NMPosSplineFitterTangents.h"
#include "NMNumerics/NMVector3Utils.h"
#include "NMNumerics/NMRigidMotionTMJacobian.h"
#include "assetProcessor/AnimSource/AnimSourceCompressorQSA.h"
#include "assetProcessor/AnimSource/ChannelSetInfoCompressorQSA.h"
#include "assetProcessor/AnimSource/AnimSectionCompressorQSA.h"
#include "assetProcessor/AnimSource/SplinePosCompressorQSA.h"

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
// SplinePosCompressorQSA Functions
//----------------------------------------------------------------------------------------------------------------------
SplinePosCompressorQSA::SplinePosCompressorQSA(AnimSectionCompressorQSA* animSectionCompressor)
{
  NMP_VERIFY(animSectionCompressor);
  m_animSectionCompressor = animSectionCompressor;

  //-----------------------
  // Init the data pointers
  m_knots = NULL;
  m_pspSpan1 = NULL;
  m_splinePosTableKeys = NULL;
  m_splinePosTableTangentsA = NULL;
  m_splinePosTableTangentsB = NULL;
  m_splinePosMeans = NULL;
  m_splinePosQuantTable = NULL;
  m_splinePosQuantisationInfo = NULL;
  m_splinePosQuantisationData = NULL;
  m_splinePosData = NULL;

  //-----------------------
  // Init the pos spline fits
  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t numAnimChannels = animSourceCompressor->getNumAnimChannelSets();
  uint32_t numSectionFrames = m_animSectionCompressor->getNumSectionFrames();

  //-----------------------
  // Allocate the memory for the simple knots vector
  NMP_VERIFY(!m_knots);
  uint32_t maxNumKnots = numSectionFrames >> 1;
  if (maxNumKnots < 2)
    maxNumKnots = 2;
  m_knots = new NMP::SimpleKnotVector(maxNumKnots);

  //-----------------------
  // Allocate the memory for the single span spline curve
  m_pspSpan1 = new NMP::PosSpline(2);

  //-----------------------
  // Allocate the memory for the pos spline channels
  m_channelPosSplines.resize(numAnimChannels);
  for (uint32_t i = 0; i < numAnimChannels; ++i)
  {
    ChannelSetInfoCompressorQSA::chanMethodType chanMethod;

    chanMethod = channelSetInfoCompressor->getCompChanMethodForPosChan(i);
    if (chanMethod == ChannelSetInfoCompressorQSA::Spline)
    {
      m_channelPosSplines[i] = new NMP::PosSpline(maxNumKnots);
    }
    else
    {
      m_channelPosSplines[i] = NULL;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
SplinePosCompressorQSA::~SplinePosCompressorQSA()
{
  //-----------------------
  // Pos splines
  if (m_knots)
  {
    delete m_knots;
    m_knots = NULL;
  }

  if (m_pspSpan1)
  {
    delete m_pspSpan1;
    m_pspSpan1 = NULL;
  }

  uint32_t numChannels = (uint32_t)m_channelPosSplines.size();
  for (uint32_t i = 0; i < numChannels; ++i)
  {
    if (m_channelPosSplines[i])
    {
      delete m_channelPosSplines[i];
    }
  }
  m_channelPosSplines.clear();

  if (m_splinePosTableKeys)
  {
    NMP::Memory::memFree(m_splinePosTableKeys);
    m_splinePosTableKeys = NULL;
  }

  if (m_splinePosTableTangentsA)
  {
    NMP::Memory::memFree(m_splinePosTableTangentsA);
    m_splinePosTableTangentsA = NULL;
  }

  if (m_splinePosTableTangentsB)
  {
    NMP::Memory::memFree(m_splinePosTableTangentsB);
    m_splinePosTableTangentsB = NULL;
  }

  if (m_splinePosMeans)
  {
    NMP::Memory::memFree(m_splinePosMeans);
    m_splinePosMeans = NULL;
  }

  if (m_splinePosQuantisationInfo)
  {
    NMP::Memory::memFree(m_splinePosQuantisationInfo);
    m_splinePosQuantisationInfo = NULL;
  }

  if (m_splinePosQuantisationData)
  {
    NMP::Memory::memFree(m_splinePosQuantisationData);
    m_splinePosQuantisationData = NULL;
  }

  if (m_splinePosQuantTable)
  {
    NMP::Memory::memFree(m_splinePosQuantTable);
    m_splinePosQuantTable = NULL;
  }

  // Quantisation sets
  m_splinePosCompToQSetMapX.clear();
  m_splinePosCompToQSetMapY.clear();
  m_splinePosCompToQSetMapZ.clear();

  //-----------------------
  // Compiled Data
  if (m_splinePosQuantisationInfo)
  {
    NMP::Memory::memFree(m_splinePosQuantisationInfo);
    m_splinePosQuantisationInfo = NULL;
  }

  if (m_splinePosQuantisationData)
  {
    NMP::Memory::memFree(m_splinePosQuantisationData);
    m_splinePosQuantisationData = NULL;
  }

  if (m_splinePosData)
  {
    NMP::Memory::memFree(m_splinePosData);
    m_splinePosData = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SplinePosCompressorQSA::computeDataStrides()
{
  //-----------------------
  // Info
  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t splinePosNumChannels = channelSetInfoCompressor->getSplinePosNumChans();
  NMP_VERIFY(splinePosNumChannels > 0);
  m_splinePosNumQuantisationSets = m_animSectionCompressor->getNumQuantisationSets(CoefSetQSA::CoefSetSplinePos);

  QuantisationSetQSA* qSet;
  uint32_t strideBits;

  strideBits = 0;
  for (uint32_t iChan = 0; iChan < splinePosNumChannels; ++iChan)
  {
    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, m_splinePosCompToQSetMapX[iChan]);
    const NMP::UniformQuantisationInfo& qInfoX = qSet->getCoefSetX()->getQuantisationInfo();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, m_splinePosCompToQSetMapY[iChan]);
    const NMP::UniformQuantisationInfo& qInfoY = qSet->getCoefSetY()->getQuantisationInfo();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, m_splinePosCompToQSetMapZ[iChan]);
    const NMP::UniformQuantisationInfo& qInfoZ = qSet->getCoefSetZ()->getQuantisationInfo();

    strideBits += qInfoX.m_precision;
    strideBits += qInfoY.m_precision;
    strideBits += qInfoZ.m_precision;
  }
  m_splinePosByteStride = (strideBits + 0x07) >> 3; // Align stride to a byte boundary
}

//----------------------------------------------------------------------------------------------------------------------
void SplinePosCompressorQSA::computeSplines(const NMP::SimpleKnotVector& knots)
{
  NMP_VERIFY(knots.getNumKnots() >= 2);
  NMP_VERIFY(m_knots);
  *m_knots = knots;

  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t splinePosNumChans = channelSetInfoCompressor->getSplinePosNumChans();
  NMP_VERIFY(splinePosNumChans > 0);

  // Information
  const std::vector<uint32_t>& splinePosCompToAnimMap = channelSetInfoCompressor->getSplinePosCompToAnimMap();
  const ChannelSetTable* animChannelSets = animSourceCompressor->getAnimChannelSets();
  NMP_VERIFY(animChannelSets);
  uint32_t numAnimFrames = animChannelSets->getNumKeyFrames();

  // Create a new pos spline fitter
  NMP::PosSplineFitterTangents fitter(numAnimFrames); // All anim sample frames
  fitter.setKnots(*m_knots); // Fits between the start and end knots of this section

  // Fit pos splines to the required channels
  for (uint32_t i = 0; i < splinePosNumChans; ++i)
  {
    uint32_t animChannelIndex = splinePosCompToAnimMap[i];
    NMP::PosSpline* psp = m_channelPosSplines[animChannelIndex];
    NMP_VERIFY(psp);

    const NMP::Vector3* posKeys = animChannelSets->getChannelPos(animChannelIndex);
    NMP_VERIFY(posKeys);

    fitter.fitRegular(numAnimFrames, posKeys);
    *psp = fitter.getPosSpline();
  }

#ifdef NM_DEBUG
  #ifdef DEBUG_TRANSFORMS
  char buffer[256];
  sprintf(buffer, "C:/posSplines_%d.dat", m_knots->getNumKnots());
  FILE* filePointer = fopen(buffer, "w");
  NMP_VERIFY(filePointer);
  if (filePointer)
  {
    for (uint32_t i = 0; i < splinePosNumChans; ++i)
    {
      uint32_t animChannelIndex = splinePosCompToAnimMap[i];
      const NMP::PosSpline* psp = m_channelPosSplines[animChannelIndex];
      NMP_VERIFY(psp);

      fprintf(filePointer, "\n_______________ Pos spline anim channel %d _______________\n", animChannelIndex);

      uint32_t numKnots =  psp->getNumKnots();
      const float* knots = psp->getKnots();
      const NMP::Vector3* keys = psp->getKeys();
      const NMP::Vector3* tangentsA = psp->getTangentsA();
      const NMP::Vector3* tangentsB = psp->getTangentsB();

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

      fprintf(filePointer, "\ntangents A = \n");
      for (uint32_t k = 0; k < numKnots - 1; ++k)
      {
        fprintf(filePointer, "[%3.5f, %3.5f, %3.5f]\n",
                tangentsA[k].x, tangentsA[k].y, tangentsA[k].z);
      }

      fprintf(filePointer, "\ntangents B = \n");
      for (uint32_t k = 0; k < numKnots - 1; ++k)
      {
        fprintf(filePointer, "[%3.5f, %3.5f, %3.5f]\n",
                tangentsB[k].x, tangentsB[k].y, tangentsB[k].z);
      }
    }
    fclose(filePointer);
  }
  #endif
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void SplinePosCompressorQSA::writeDebug(FILE* filePointer)
{
  NMP_VERIFY(filePointer);
  fprintf(filePointer, "\n-----------------------------------------------------------------------------------------------\n");
  fprintf(filePointer, "Pos Spline Compressor Dump\n");
  fprintf(filePointer, "-----------------------------------------------------------------------------------------------\n");

  writePosSplines(filePointer);
  writePosTableKeys(filePointer);
  writePosTableTangentsA(filePointer);
  writePosTableTangentsB(filePointer);
  writeQuantisationInfo(filePointer);
  writeGlobalMeans(filePointer);
  writePosMeans(filePointer);
  writeRecomposedPosSplines(filePointer);
}

//----------------------------------------------------------------------------------------------------------------------
void SplinePosCompressorQSA::writePosSplines(FILE* filePointer) const
{
  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t splinePosNumChans = channelSetInfoCompressor->getSplinePosNumChans();
  NMP_VERIFY(splinePosNumChans > 0);
  const std::vector<uint32_t>& splinePosCompToAnimMap = channelSetInfoCompressor->getSplinePosCompToAnimMap();

  for (uint32_t i = 0; i < splinePosNumChans; ++i)
  {
    uint32_t animChannelIndex = splinePosCompToAnimMap[i];
    const NMP::PosSpline* psp = m_channelPosSplines[animChannelIndex];
    NMP_VERIFY(psp);

    fprintf(filePointer, "\n_______________ Pos splines: anim chan = %d, (comp chan = %d) _______________\n",
            animChannelIndex, i);

    uint32_t numKnots =  psp->getNumKnots();
    const float* knots = psp->getKnots();
    const NMP::Vector3* keys = psp->getKeys();
    const NMP::Vector3* velsA = psp->getTangentsA();
    const NMP::Vector3* velsB = psp->getTangentsB();

    fprintf(filePointer, "knots = \n");
    for (uint32_t k = 0; k <  numKnots; ++k)
    {
      fprintf(filePointer, "%4f\n", knots[k]);
    }

    fprintf(filePointer, "\nkeys = \n");
    for (uint32_t k = 0; k < numKnots; ++k)
    {
      fprintf(filePointer, "[%3.5f, %3.5f, %3.5f]\n",
              keys[k].x, keys[k].y, keys[k].z);
    }

    fprintf(filePointer, "\ntangents A = \n");
    for (uint32_t k = 0; k < numKnots - 1; ++k)
    {
      fprintf(filePointer, "[%3.5f, %3.5f, %3.5f]\n",
              velsA[k].x, velsA[k].y, velsA[k].z);
    }

    fprintf(filePointer, "\ntangents B = \n");
    for (uint32_t k = 0; k < numKnots - 1; ++k)
    {
      fprintf(filePointer, "[%3.5f, %3.5f, %3.5f]\n",
              velsB[k].x, velsB[k].y, velsB[k].z);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SplinePosCompressorQSA::writePosTableKeys(FILE* filePointer) const
{
  NMP_VERIFY(filePointer);
  fprintf(filePointer, "\n_________________ splinePosTableKeys _________________\n");
  m_splinePosTableKeys->writeTableByFrames(filePointer);
}

//----------------------------------------------------------------------------------------------------------------------
void SplinePosCompressorQSA::writePosTableTangentsA(FILE* filePointer) const
{
  NMP_VERIFY(filePointer);
  fprintf(filePointer, "\n_________________ splinePosTableTangentsA _________________\n");
  m_splinePosTableTangentsA->writeTableByFrames(filePointer);
}

//----------------------------------------------------------------------------------------------------------------------
void SplinePosCompressorQSA::writePosTableTangentsB(FILE* filePointer) const
{
  NMP_VERIFY(filePointer);
  fprintf(filePointer, "\n_________________ splinePosTableTangentsB _________________\n");
  m_splinePosTableTangentsB->writeTableByFrames(filePointer);
}

//----------------------------------------------------------------------------------------------------------------------
void SplinePosCompressorQSA::writeQuantisationInfo(FILE* filePointer) const
{
  NMP_VERIFY(filePointer);

  uint32_t splinePosNumQuantisationSets = getSplinePosNumQuantisationSets();
  const MR::QuantisationInfoQSA* qSetInfo = getSplinePosQuantisationInfo();
  NMP_VERIFY(qSetInfo);

  for (uint32_t i = 0; i < splinePosNumQuantisationSets; ++i)
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
void SplinePosCompressorQSA::writeGlobalMeans(FILE* filePointer) const
{
  NMP_VERIFY(filePointer);
  fprintf(filePointer, "\n_________________ Global Means _________________\n");

  const MR::QuantisationInfoQSA& posMeansQuantisationInfo = m_animSectionCompressor->getPosMeansQuantisationInfo();
  fprintf(filePointer, "posMeansQuantisationInfo: qMin = [%3.5f, %3.5f, %3.5f], qMax = [%3.5f, %3.5f, %3.5f]\n",
          posMeansQuantisationInfo.m_qMin[0],
          posMeansQuantisationInfo.m_qMin[1],
          posMeansQuantisationInfo.m_qMin[2],
          posMeansQuantisationInfo.m_qMax[0],
          posMeansQuantisationInfo.m_qMax[1],
          posMeansQuantisationInfo.m_qMax[2]);
}

//----------------------------------------------------------------------------------------------------------------------
void SplinePosCompressorQSA::writePosMeans(FILE* filePointer) const
{
  NMP_VERIFY(filePointer);
  fprintf(filePointer, "\n_________________ splinePosKeysMeans _________________\n");

  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t splinePosNumChans = channelSetInfoCompressor->getSplinePosNumChans();
  NMP_VERIFY(splinePosNumChans > 0);

  for (uint32_t iChan = 0; iChan < splinePosNumChans; ++iChan)
  {
    fprintf(
      filePointer,
      "comp chan = %4d, qVec = [%3.5f, %3.5f, %3.5f]\n",
      iChan,
      m_splinePosMeans[iChan].x,
      m_splinePosMeans[iChan].y,
      m_splinePosMeans[iChan].z);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SplinePosCompressorQSA::writeRecomposedPosSplines(FILE* filePointer)
{
  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t splinePosNumChans = channelSetInfoCompressor->getSplinePosNumChans();
  NMP_VERIFY(splinePosNumChans > 0);
  const std::vector<uint32_t>& splinePosCompToAnimMap = channelSetInfoCompressor->getSplinePosCompToAnimMap();

  float ta, tb;
  NMP::Vector3 p0, p1, p2, p3;

  // To avoid initialisation warnings
  ta = tb = 0;
  p3.setToZero();

  for (uint32_t i = 0; i < splinePosNumChans; ++i)
  {
    uint32_t animChannelIndex = splinePosCompToAnimMap[i];
    uint32_t numKnots = m_knots->getNumKnots();
    uint32_t numSpans = numKnots - 1;

    fprintf(filePointer, "\n_______________ Recomposed Pos splines: anim chan = %d, (comp chan = %d) _______________\n",
            animChannelIndex, i);

    //-----------------------
    // Knots
    fprintf(filePointer, "knots = \n");
    for (uint32_t k = 0; k < numSpans; ++k)
    {
      recompose(i, k, ta, tb, p0, p1, p2, p3);
      fprintf(filePointer, "%4f\n", ta);
    }
    fprintf(filePointer, "%4f\n", tb);

    //-----------------------
    // Keys
    fprintf(filePointer, "\nkeys = \n");
    for (uint32_t k = 0; k < numSpans; ++k)
    {
      recompose(i, k, ta, tb, p0, p1, p2, p3);
      fprintf(filePointer, "[%3.5f, %3.5f, %3.5f]\n",
              p0.x, p0.y, p0.z);
    }
    fprintf(filePointer, "[%3.5f, %3.5f, %3.5f]\n",
            p0.x, p0.y, p0.z);

    //-----------------------
    // TangentsA
    fprintf(filePointer, "\ntangents A = \n");
    for (uint32_t k = 0; k < numSpans; ++k)
    {
      recompose(i, k, ta, tb, p0, p1, p2, p3);
      fprintf(filePointer, "[%3.5f, %3.5f, %3.5f]\n",
              p1.x, p1.y, p1.z);
    }

    //-----------------------
    // TangentsB
    fprintf(filePointer, "\ntangents B = \n");
    for (uint32_t k = 0; k < numSpans; ++k)
    {
      recompose(i, k, ta, tb, p0, p1, p2, p3);
      fprintf(filePointer, "[%3.5f, %3.5f, %3.5f]\n",
              p2.x, p2.y, p2.z);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SplinePosCompressorQSA::computeRateAndDistortion()
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
NMP::Vector3 SplinePosCompressorQSA::evaluateQuantised(uint32_t iChan, uint32_t span, float animFrame)
{
  NMP::Vector3 p0, p1, p2, p3, x;
  float ta, tb;

  // Recover the quat spline parameters
  recompose(iChan, span, ta, tb, p0, p1, p2, p3);

  // Set the single span quat spline
  m_pspSpan1->set(ta, tb, p0, p1, p2, p3);

  // Recover the iChan pos sample
  x = m_pspSpan1->evaluate(animFrame);

  return x;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 SplinePosCompressorQSA::evaluateQuantised(uint32_t iChan, float t)
{
  NMP::Vector3 p0, p1, p2, p3, x;
  float ta, tb;
  uint32_t span;

  m_knots->interval(t, span);

  // Recover the quat spline parameters
  recompose(iChan, span, ta, tb, p0, p1, p2, p3);

  // Set the single span pos spline
  m_pspSpan1->set(ta, tb, p0, p1, p2, p3);

  // Recover the iChan quat sample
  x = m_pspSpan1->evaluate(t);

  return x;
}

//----------------------------------------------------------------------------------------------------------------------
void SplinePosCompressorQSA::recompose(
  uint32_t      iChan,
  uint32_t      span,
  float&        ta,
  float&        tb,
  NMP::Vector3& p0,
  NMP::Vector3& p1,
  NMP::Vector3& p2,
  NMP::Vector3& p3)
{
  NMP::Vector3 qMin, qMax;
  NMP::Vector3 posKey, keyA, keyB;
  NMP::Vector3 tangentA, tangentB;
  NMP::Vector3 keyARel, keyARel_, keyBRel, keyBRel_;
  NMP::Vector3 tangentARel, tangentARel_, tangentBRel, tangentBRel_;
  uint32_t     qx, qy, qz;
  QuantisationSetQSA* qSet;
  CoefSetQSA* coefSetX;
  CoefSetQSA* coefSetY;
  CoefSetQSA* coefSetZ;

  //-----------------------
  // Get the quantisation set corresponding to this compression channel
  qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, m_splinePosCompToQSetMapX[iChan]);
  coefSetX = qSet->getCoefSetX();
  const NMP::UniformQuantisationInfo& qInfoX = coefSetX->getQuantisationInfo();

  qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, m_splinePosCompToQSetMapY[iChan]);
  coefSetY = qSet->getCoefSetY();
  const NMP::UniformQuantisationInfo& qInfoY = coefSetY->getQuantisationInfo();

  qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, m_splinePosCompToQSetMapZ[iChan]);
  coefSetZ = qSet->getCoefSetZ();
  const NMP::UniformQuantisationInfo& qInfoZ = coefSetZ->getQuantisationInfo();

  qMin.set(qInfoX.m_qMin, qInfoY.m_qMin, qInfoZ.m_qMin);
  qMax.set(qInfoX.m_qMax, qInfoY.m_qMax, qInfoZ.m_qMax);

  // Set the quantiser precision and [qMin, qMax] range
  m_splinePosQuantTable->setPrecisionX(iChan, qInfoX.m_precision);
  m_splinePosQuantTable->setPrecisionY(iChan, qInfoY.m_precision);
  m_splinePosQuantTable->setPrecisionZ(iChan, qInfoZ.m_precision);
  m_splinePosQuantTable->setQuantisationBounds(iChan, qMin, qMax);

  //-----------------------
  // Quantise the relative samples
  m_splinePosTableKeys->getKey(iChan, span, keyARel);
  m_splinePosQuantTable->quantise(iChan, keyARel, qx, qy, qz);
  m_splinePosQuantTable->dequantise(iChan, keyARel_, qx, qy, qz);

  m_splinePosTableTangentsA->getKey(iChan, span, tangentARel);
  m_splinePosQuantTable->quantise(iChan, tangentARel, qx, qy, qz);
  m_splinePosQuantTable->dequantise(iChan, tangentARel_, qx, qy, qz);

  m_splinePosTableTangentsB->getKey(iChan, span, tangentBRel);
  m_splinePosQuantTable->quantise(iChan, tangentBRel, qx, qy, qz);
  m_splinePosQuantTable->dequantise(iChan, tangentBRel_, qx, qy, qz);

  m_splinePosTableKeys->getKey(iChan, span + 1, keyBRel);
  m_splinePosQuantTable->quantise(iChan, keyBRel, qx, qy, qz);
  m_splinePosQuantTable->dequantise(iChan, keyBRel_, qx, qy, qz);

  //-----------------------
  // Recover the pos spline parameters
  ta = m_knots->getKnot(span);
  tb = m_knots->getKnot(span + 1);

  p0 = m_splinePosMeans[iChan] + keyARel_;
  p1 = m_splinePosMeans[iChan] + tangentARel_;
  p2 = m_splinePosMeans[iChan] + tangentBRel_;
  p3 = m_splinePosMeans[iChan] + keyBRel_;
}

//----------------------------------------------------------------------------------------------------------------------
void SplinePosCompressorQSA::computeRelativeTransforms()
{
  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t splinePosNumChans = channelSetInfoCompressor->getSplinePosNumChans();
  NMP_VERIFY(splinePosNumChans > 0);
  NMP_VERIFY(m_knots);
  uint32_t numKnots = m_knots->getNumKnots();
  NMP_VERIFY(numKnots >= 2);
  uint32_t numSpans = numKnots - 1;

  NMP::Vector3 qMin, qMax;
  uint32_t vecSizes[3];
  const NMP::Vector3* vecArrays[3];
  NMP::Vector3 v, vbar;
  NMP_VERIFY(numKnots >= 2);
  uint32_t qx = 0, qy = 0, qz = 0;

  // Information
  const std::vector<uint32_t>& splinePosCompToAnimMap = channelSetInfoCompressor->getSplinePosCompToAnimMap();
  const MR::QuantisationInfoQSA& posMeansInfo = m_animSectionCompressor->getPosMeansQuantisationInfo();

  //-----------------------
  // Allocate the memory for the spline control points table (for relative transforms)
  NMP_VERIFY(!m_splinePosTableKeys);
  NMP::Memory::Format memReqsTableKeys = AP::Vector3Table::getMemoryRequirements(splinePosNumChans, numKnots);
  NMP::Memory::Resource memResTableKeys = NMPMemoryAllocateFromFormat(memReqsTableKeys);
  m_splinePosTableKeys = AP::Vector3Table::init(memResTableKeys, splinePosNumChans, numKnots);

  //-----------------------
  // Allocate the memory for the channel control point means
  NMP_VERIFY(!m_splinePosMeans);
  NMP::Memory::Format memReqsMeans(0, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqsMeans.size = NMP::Memory::align(sizeof(NMP::Vector3) * splinePosNumChans, NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::Resource memResMeans = NMPMemoryAllocateFromFormat(memReqsMeans);
  m_splinePosMeans = (NMP::Vector3*)memResMeans.ptr;

  //-----------------------
  // Allocate the memory for the spline pos tangent table (for relative transforms)
  NMP_VERIFY(!m_splinePosTableTangentsA);
  NMP::Memory::Format memReqsTableTangents = AP::Vector3Table::getMemoryRequirements(splinePosNumChans, numKnots - 1);
  NMP::Memory::Resource memResTableTangentsA = NMPMemoryAllocateFromFormat(memReqsTableTangents);
  m_splinePosTableTangentsA = AP::Vector3Table::init(memResTableTangentsA, splinePosNumChans, numKnots - 1);

  NMP_VERIFY(!m_splinePosTableTangentsB);
  NMP::Memory::Resource memResTableTangentsB = NMPMemoryAllocateFromFormat(memReqsTableTangents);
  m_splinePosTableTangentsB = AP::Vector3Table::init(memResTableTangentsB, splinePosNumChans, numKnots - 1);

  //-----------------------
  // Allocate the memory for the compiled channel quantisation data (means, precisions, qSets)
  NMP_VERIFY(!m_splinePosQuantisationData);
  NMP::Memory::Format memReqsQData(0, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqsQData.size = NMP::Memory::align(MR::QuantisationDataQSA::getMemoryRequirementsSize(splinePosNumChans), NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::Resource memResQData = NMPMemoryAllocateFromFormat(memReqsQData);
  m_splinePosQuantisationData = (MR::QuantisationDataQSA*)memResQData.ptr;
  m_splinePosQuantisationDataSize = memReqsQData.size;

  //-----------------------
  // Allocate the memory for the quantisation table
  NMP::Memory::Format memReqsQuantPos = AP::Vector3QuantisationTable::getMemoryRequirements(splinePosNumChans);
  NMP::Memory::Resource memResQuantPos = NMPMemoryAllocateFromFormat(memReqsQuantPos);
  m_splinePosQuantTable = AP::Vector3QuantisationTable::init(memResQuantPos, splinePosNumChans);

  // Set the global pos quantisation bounds
  m_splinePosQuantTable->setGlobalQuantisationBounds(posMeansInfo.m_qMin, posMeansInfo.m_qMax);

  // Set 8-bits of precision for quantising the channel means
  m_splinePosQuantTable->setPrecisions(8);

  //-----------------------
  // Compute the spline control point transforms
  for (uint32_t iChan = 0; iChan < splinePosNumChans; ++iChan)
  {
    uint32_t animChannelIndex = splinePosCompToAnimMap[iChan];
    const NMP::PosSpline* psp = m_channelPosSplines[animChannelIndex];
    NMP_VERIFY(psp);
    const NMP::Vector3* keys = psp->getKeys();
    NMP_VERIFY(keys);
    const NMP::Vector3* tangentsA = psp->getTangentsA();
    NMP_VERIFY(tangentsA);
    const NMP::Vector3* tangentsB = psp->getTangentsB();
    NMP_VERIFY(tangentsB);

    // Compute the mean of the pos control points
    vecSizes[0] = numKnots;
    vecArrays[0] = keys;
    vecSizes[1] = numSpans;
    vecArrays[1] = tangentsA;
    vecSizes[2] = numSpans;
    vecArrays[2] = tangentsB;
    NMP::vecMean(3, vecSizes, vecArrays, vbar);

    // Quantise the channel mean into 8-bits
    m_splinePosQuantTable->quantise(iChan, vbar, qx, qy, qz);
    QuantisationDataQSABuilder::init(m_splinePosQuantisationData[iChan], qx, qy, qz);

    // Recover the corresponding quantised mean value
    m_splinePosQuantTable->dequantise(iChan, m_splinePosMeans[iChan], qx, qy, qz);

    // Remove the mean from the pos control points
    for (uint32_t k = 0; k < numSpans; ++k)
    {
      v = keys[k] - m_splinePosMeans[iChan];
      m_splinePosTableKeys->setKey(iChan, k, v);
      v = tangentsA[k] - m_splinePosMeans[iChan];
      m_splinePosTableTangentsA->setKey(iChan, k, v);
      v = tangentsB[k] - m_splinePosMeans[iChan];
      m_splinePosTableTangentsB->setKey(iChan, k, v);
    }
    v = keys[numSpans] - m_splinePosMeans[iChan];
    m_splinePosTableKeys->setKey(iChan, numSpans, v);
  }
  for (uint32_t iChan = splinePosNumChans; iChan < NMP::nmAlignedValue4(splinePosNumChans); ++iChan)
  {
    QuantisationDataQSABuilder::init(m_splinePosQuantisationData[iChan], qx, qy, qz);
  }

  // Compute the quantisation bounds for the relative samples
  m_splinePosQuantTable->setQuantisationBounds(m_splinePosTableKeys);
  m_splinePosQuantTable->updateQuantisationBounds(m_splinePosTableTangentsA);
  m_splinePosQuantTable->updateQuantisationBounds(m_splinePosTableTangentsB);
}

//----------------------------------------------------------------------------------------------------------------------
void SplinePosCompressorQSA::assignQuantisationSets()
{
  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t splinePosNumChans = channelSetInfoCompressor->getSplinePosNumChans();
  NMP_VERIFY(splinePosNumChans > 0);
  NMP::Vector3 qMin, qMax;

  //-----------------------
  // Assign the spline pos channels to the quantisation sets
  uint32_t numQuantisationSets = m_animSectionCompressor->getNumQuantisationSets(CoefSetQSA::CoefSetSplinePos);
  NMP_VERIFY(numQuantisationSets > 0);
  m_splinePosCompToQSetMapX.resize(splinePosNumChans);
  m_splinePosCompToQSetMapY.resize(splinePosNumChans);
  m_splinePosCompToQSetMapZ.resize(splinePosNumChans);

  NMP::UniformQuantisationInfoCompounder qSetCompounderX(splinePosNumChans);
  NMP::UniformQuantisationInfoCompounder qSetCompounderY(splinePosNumChans);
  NMP::UniformQuantisationInfoCompounder qSetCompounderZ(splinePosNumChans);

  for (uint32_t iChan = 0; iChan < splinePosNumChans; ++iChan)
  {
    m_splinePosQuantTable->getQuantisationBounds(iChan, qMin, qMax);
    qSetCompounderX.setQuantisationInfo(iChan, qMin.x, qMax.x);
    qSetCompounderY.setQuantisationInfo(iChan, qMin.y, qMax.y);
    qSetCompounderZ.setQuantisationInfo(iChan, qMin.z, qMax.z);
  }

  qSetCompounderX.compoundQuantisationInfo(numQuantisationSets);
  qSetCompounderY.compoundQuantisationInfo(numQuantisationSets);
  qSetCompounderZ.compoundQuantisationInfo(numQuantisationSets);

  for (uint32_t iChan = 0; iChan < splinePosNumChans; ++iChan)
  {
    m_splinePosCompToQSetMapX[iChan] = qSetCompounderX.getMapFromInputToCompounded(iChan);
    m_splinePosCompToQSetMapY[iChan] = qSetCompounderY.getMapFromInputToCompounded(iChan);
    m_splinePosCompToQSetMapZ[iChan] = qSetCompounderZ.getMapFromInputToCompounded(iChan);
  }

  for (uint32_t qSetIndex = 0; qSetIndex < numQuantisationSets; ++qSetIndex)
  {
    // Get the coefficient sets
    QuantisationSetQSA* qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, qSetIndex);
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
void SplinePosCompressorQSA::simpleRateAndDistortion()
{
  // Clear the rate and distortion information
  m_animSectionCompressor->clearRateAndDistortion(CoefSetQSA::CoefSetSplinePos);

  // Compute the delta to dist mapping
  m_animSectionCompressor->makeSimpleDeltaToDistWeightMap(CoefSetQSA::CoefSetSplinePos);
}

//----------------------------------------------------------------------------------------------------------------------
void SplinePosCompressorQSA::analyseRateAndDistortion()
{
  NMP::Vector3 qMin, qMax;
  NMP::Vector3 keyARel, keyARel_, keyBRel, keyBRel_;
  NMP::Vector3 tangentARel, tangentARel_, tangentBRel, tangentBRel_;
  NMP::Vector3 dy;
  NMP::RigidMotionTMJacobian tmJac;
  uint32_t qx, qy, qz;
  float delta, dist;
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
  uint32_t splinePosNumChans = channelSetInfoCompressor->getSplinePosNumChans();
  NMP_VERIFY(splinePosNumChans > 0);
  const std::vector<uint32_t>& splinePosCompToAnimMap = channelSetInfoCompressor->getSplinePosCompToAnimMap();
  uint32_t numKnots = m_knots->getNumKnots();
  NMP_VERIFY(numKnots >= 2);
  uint32_t numSpans = numKnots - 1;
  const NMP::Matrix<float>* bonePosWeights = m_animSectionCompressor->getBonePosWeights();
  NMP_VERIFY(bonePosWeights);
  const std::vector<uint32_t>& analysisPrecisions = m_animSectionCompressor->getAnalysisPrecisions();
  NMP_VERIFY(!analysisPrecisions.empty());

  //-----------------------
  // Clear the rate and distortion information
  m_animSectionCompressor->clearRateAndDistortion(CoefSetQSA::CoefSetSplinePos);

  //-----------------------
  // Iterate over the curve spans
  for (uint32_t span = 0; span < numSpans; ++span)
  {
    //-----------------------
    // Iterate over all compression channels
    for (uint32_t iChan = 0; iChan < splinePosNumChans; ++iChan)
    {
      // Get the rig channel index for the compression channel
      uint16_t animChannelIndex = (uint16_t)splinePosCompToAnimMap[iChan];
      uint16_t rigChannelIndex;
      bool status = rigToAnimEntryMap->getRigIndexForAnimIndex(animChannelIndex, rigChannelIndex);
      NMP_VERIFY(status);

      // Set the current channel's pos spline
      m_splinePosTableKeys->getKey(iChan, span, keyARel);
      m_splinePosTableKeys->getKey(iChan, span + 1, keyBRel);
      m_splinePosTableTangentsA->getKey(iChan, span, tangentARel);
      m_splinePosTableTangentsB->getKey(iChan, span, tangentBRel);

      tmJac.setPosSpline(
        m_knots->getKnot(span),
        m_knots->getKnot(span + 1),
        m_splinePosMeans[iChan],
        keyARel,
        tangentARel,
        tangentBRel,
        keyBRel);

      //-----------------------
      // Get the quantisation set corresponding to this compression channel
      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, m_splinePosCompToQSetMapX[iChan]);
      coefSetX = qSet->getCoefSetX();
      const NMP::UniformQuantisationInfo& qInfoX = coefSetX->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, m_splinePosCompToQSetMapY[iChan]);
      coefSetY = qSet->getCoefSetY();
      const NMP::UniformQuantisationInfo& qInfoY = coefSetY->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, m_splinePosCompToQSetMapZ[iChan]);
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

        //-----------------------
        // Set the quantiser precision and [qMin, qMax] range
        m_splinePosQuantTable->setPrecision(iChan, prec);
        if (prec > 0)
        {
          m_splinePosQuantTable->setQuantisationBounds(iChan, qMin, qMax);
        }
        else
        {
          // Use the mean of the distribution (i.e. zero mean)
          m_splinePosQuantTable->zeroQuantisationBounds(iChan);
        }

        //-----------------------
        // Quantise the relative samples
        m_splinePosQuantTable->quantise(iChan, keyARel, qx, qy, qz);
        m_splinePosQuantTable->dequantise(iChan, keyARel_, qx, qy, qz);

        m_splinePosQuantTable->quantise(iChan, tangentARel, qx, qy, qz);
        m_splinePosQuantTable->dequantise(iChan, tangentARel_, qx, qy, qz);

        m_splinePosQuantTable->quantise(iChan, tangentBRel, qx, qy, qz);
        m_splinePosQuantTable->dequantise(iChan, tangentBRel_, qx, qy, qz);

        m_splinePosQuantTable->quantise(iChan, keyBRel, qx, qy, qz);
        m_splinePosQuantTable->dequantise(iChan, keyBRel_, qx, qy, qz);

        //-----------------------
        // Update the rate of the coefficient sets
        coefSetX->updateRate(prec, 3);
        coefSetY->updateRate(prec, 3); // Start key + two tangents
        coefSetZ->updateRate(prec, 3);

        //-----------------------
        // Iterate over the frames within the curve span
        uint32_t ta = (uint32_t)m_knots->getKnot(span);
        uint32_t tb = (uint32_t)m_knots->getKnot(span + 1);
        for (uint32_t t = ta + 1; t < tb; ++t)
        {
          // Evaluate the Pos spline at the current frame
          tmJac.evaluatePosSpline((float)t);

          // Get the frame within the section
          uint32_t iFrame = t - ta;

          // Get the displacement weight factor at the current frame for this channel
          float dispWeight = bonePosWeights->element(iFrame, rigChannelIndex);

          // Apply the rigid motion y' = R*x + t' of the current joint to the
          // child bone point to recover its worldspace position at the current frame.
          // We can then compute the error between the quantised y' and the
          // unquantised y positions dy = y' - y. Instead of doing this explicitly
          // (i.e. this is a forward difference Jacobian) we propagate the error
          // through our analytically computed Jacobian: dy = Dy/Dx(u) * Dx(u)/D[p0, p1, p2, p3]
          // We note that Dy/Dx(u) = I and update the rate and distortion directly

          //-----------------------
          // keyARel
          //-----------------------
          delta = keyARel_.x - keyARel.x;
          coefSetX->updateDelta(prec, delta * delta);
          dy = tmJac.getErrorPosSplineKeyA(0, delta);
          dist = dy.x * dispWeight;
          coefSetX->updateDistortion(prec, dist * dist);

          delta = keyARel_.y - keyARel.y;
          coefSetY->updateDelta(prec, delta * delta);
          dy = tmJac.getErrorPosSplineKeyA(1, delta);
          dist = dy.y * dispWeight;
          coefSetY->updateDistortion(prec, dist * dist);

          delta = keyARel_.z - keyARel.z;
          coefSetZ->updateDelta(prec, delta * delta);
          dy = tmJac.getErrorPosSplineKeyA(2, delta);
          dist = dy.z * dispWeight;
          coefSetZ->updateDistortion(prec, dist * dist);

          //-----------------------
          // tangentARel
          //-----------------------
          delta = tangentARel_.x - tangentARel.x;
          coefSetX->updateDelta(prec, delta * delta);
          dy = tmJac.getErrorPosSplineTangentA(0, delta);
          dist = dy.x * dispWeight;
          coefSetX->updateDistortion(prec, dist * dist);

          delta = tangentARel_.y - tangentARel.y;
          coefSetY->updateDelta(prec, delta * delta);
          dy = tmJac.getErrorPosSplineTangentA(1, delta);
          dist = dy.y * dispWeight;
          coefSetY->updateDistortion(prec, dist * dist);

          delta = tangentARel_.z - tangentARel.z;
          coefSetZ->updateDelta(prec, delta * delta);
          dy = tmJac.getErrorPosSplineTangentA(2, delta);
          dist = dy.z * dispWeight;
          coefSetZ->updateDistortion(prec, dist * dist);

          //-----------------------
          // tangentBRel
          //-----------------------
          delta = tangentBRel_.x - tangentBRel.x;
          coefSetX->updateDelta(prec, delta * delta);
          dy = tmJac.getErrorPosSplineTangentB(0, delta);
          dist = dy.x * dispWeight;
          coefSetX->updateDistortion(prec, dist * dist);

          delta = tangentBRel_.y - tangentBRel.y;
          coefSetY->updateDelta(prec, delta * delta);
          dy = tmJac.getErrorPosSplineTangentB(1, delta);
          dist = dy.y * dispWeight;
          coefSetY->updateDistortion(prec, dist * dist);

          delta = tangentBRel_.z - tangentBRel.z;
          coefSetZ->updateDelta(prec, delta * delta);
          dy = tmJac.getErrorPosSplineTangentB(2, delta);
          dist = dy.z * dispWeight;
          coefSetZ->updateDistortion(prec, dist * dist);

          //-----------------------
          // keyBRel
          //-----------------------
          delta = keyBRel_.x - keyBRel.x;
          coefSetX->updateDelta(prec, delta * delta);
          dy = tmJac.getErrorPosSplineKeyB(0, delta);
          dist = dy.x * dispWeight;
          coefSetX->updateDistortion(prec, dist * dist);

          delta = keyBRel_.y - keyBRel.y;
          coefSetY->updateDelta(prec, delta * delta);
          dy = tmJac.getErrorPosSplineKeyB(1, delta);
          dist = dy.y * dispWeight;
          coefSetY->updateDistortion(prec, dist * dist);

          delta = keyBRel_.z - keyBRel.z;
          coefSetZ->updateDelta(prec, delta * delta);
          dy = tmJac.getErrorPosSplineKeyB(2, delta);
          dist = dy.z * dispWeight;
          coefSetZ->updateDistortion(prec, dist * dist);

        } // Frames
      } // Precisions
    } // Channels
  } // Spans

  //-----------------------
  // Rate for last pos spline keys
  for (uint32_t iChan = 0; iChan < splinePosNumChans; ++iChan)
  {
    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, m_splinePosCompToQSetMapX[iChan]);
    coefSetX = qSet->getCoefSetX();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, m_splinePosCompToQSetMapY[iChan]);
    coefSetY = qSet->getCoefSetY();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, m_splinePosCompToQSetMapZ[iChan]);
    coefSetZ = qSet->getCoefSetZ();

    // Error propagation for the remaining bits of precision
    std::vector<uint32_t>::const_iterator pit = analysisPrecisions.begin();
    std::vector<uint32_t>::const_iterator pit_end = analysisPrecisions.end();
    for (; pit != pit_end; ++pit)
    {
      uint32_t prec = *pit;
      coefSetX->updateRate(prec);
      coefSetY->updateRate(prec);
      coefSetZ->updateRate(prec);
    }
  }

  //-----------------------
  // Compute the delta to dist mapping
  m_animSectionCompressor->makeDeltaToDistWeightMap(CoefSetQSA::CoefSetSplinePos);
}

//----------------------------------------------------------------------------------------------------------------------
void SplinePosCompressorQSA::computeRateAndDistortionMapped()
{
  NMP::Vector3 qMin, qMax;
  NMP::Vector3 keyARel, keyARel_, keyBRel, keyBRel_;
  NMP::Vector3 tangentARel, tangentARel_, tangentBRel, tangentBRel_;
  NMP::Vector3 dy;
  uint32_t qx, qy, qz;
  float delta;
  QuantisationSetQSA* qSet;
  CoefSetQSA* coefSetX;
  CoefSetQSA* coefSetY;
  CoefSetQSA* coefSetZ;
  uint32_t maxPrec = m_animSectionCompressor->getMaxPrec();

  //-----------------------
  // Information
  AnimSourceCompressorQSA* animSourceCompressor = m_animSectionCompressor->getAnimSourceCompressor();
  NMP_VERIFY(animSourceCompressor);
  const ChannelSetInfoCompressorQSA* channelSetInfoCompressor = animSourceCompressor->getChannelSetInfoCompressor();
  NMP_VERIFY(channelSetInfoCompressor);
  uint32_t splinePosNumChans = channelSetInfoCompressor->getSplinePosNumChans();
  NMP_VERIFY(splinePosNumChans > 0);
  uint32_t numKnots = m_knots->getNumKnots();
  NMP_VERIFY(numKnots >= 2);
  uint32_t numSpans = numKnots - 1;

  //-----------------------
  // Clear the rate and delta information
  m_animSectionCompressor->clearRateAndDelta(CoefSetQSA::CoefSetSplinePos);

  //-----------------------
  // Iterate over the curve spans
  for (uint32_t span = 0; span < numSpans; ++span)
  {
    //-----------------------
    // Iterate over all compression channels
    for (uint32_t iChan = 0; iChan < splinePosNumChans; ++iChan)
    {
      // Set the current iChan's pos spline
      m_splinePosTableKeys->getKey(iChan, span, keyARel);
      m_splinePosTableKeys->getKey(iChan, span + 1, keyBRel);
      m_splinePosTableTangentsA->getKey(iChan, span, tangentARel);
      m_splinePosTableTangentsB->getKey(iChan, span, tangentBRel);

      //-----------------------
      // Get the quantisation set corresponding to this compression channel
      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, m_splinePosCompToQSetMapX[iChan]);
      coefSetX = qSet->getCoefSetX();
      const NMP::UniformQuantisationInfo& qInfoX = coefSetX->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, m_splinePosCompToQSetMapY[iChan]);
      coefSetY = qSet->getCoefSetY();
      const NMP::UniformQuantisationInfo& qInfoY = coefSetY->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, m_splinePosCompToQSetMapZ[iChan]);
      coefSetZ = qSet->getCoefSetZ();
      const NMP::UniformQuantisationInfo& qInfoZ = coefSetZ->getQuantisationInfo();

      qMin.set(qInfoX.m_qMin, qInfoY.m_qMin, qInfoZ.m_qMin);
      qMax.set(qInfoX.m_qMax, qInfoY.m_qMax, qInfoZ.m_qMax);

      //-----------------------
      // Error propagation for the remaining bits of precision
      for (uint32_t prec = 0; prec <= maxPrec; ++prec)
      {
        //-----------------------
        // Set the quantiser precision and [qMin, qMax] range
        m_splinePosQuantTable->setPrecision(iChan, prec);
        if (prec > 0)
        {
          m_splinePosQuantTable->setQuantisationBounds(iChan, qMin, qMax);
        }
        else
        {
          // Use the mean of the distribution (i.e. zero mean)
          m_splinePosQuantTable->zeroQuantisationBounds(iChan);
        }

        //-----------------------
        // Quantise the relative samples
        m_splinePosQuantTable->quantise(iChan, keyARel, qx, qy, qz);
        m_splinePosQuantTable->dequantise(iChan, keyARel_, qx, qy, qz);

        m_splinePosQuantTable->quantise(iChan, tangentARel, qx, qy, qz);
        m_splinePosQuantTable->dequantise(iChan, tangentARel_, qx, qy, qz);

        m_splinePosQuantTable->quantise(iChan, tangentBRel, qx, qy, qz);
        m_splinePosQuantTable->dequantise(iChan, tangentBRel_, qx, qy, qz);

        m_splinePosQuantTable->quantise(iChan, keyBRel, qx, qy, qz);
        m_splinePosQuantTable->dequantise(iChan, keyBRel_, qx, qy, qz);

        //-----------------------
        // Update the rate of the coefficient sets
        coefSetX->updateRate(prec, 3);
        coefSetY->updateRate(prec, 3); // Start key + two tangents
        coefSetZ->updateRate(prec, 3);

        //-----------------------
        // Update the distortion of the coefficient sets
        delta = keyARel_.x - keyARel.x;
        coefSetX->updateDelta(prec, delta * delta);
        delta = keyARel_.y - keyARel.y;
        coefSetY->updateDelta(prec, delta * delta);
        delta = keyARel_.z - keyARel.z;
        coefSetZ->updateDelta(prec, delta * delta);

        delta = tangentARel_.x - tangentARel.x;
        coefSetX->updateDelta(prec, delta * delta);
        delta = tangentARel_.y - tangentARel.y;
        coefSetY->updateDelta(prec, delta * delta);
        delta = tangentARel_.z - tangentARel.z;
        coefSetZ->updateDelta(prec, delta * delta);

        delta = tangentBRel_.x - tangentBRel.x;
        coefSetX->updateDelta(prec, delta * delta);
        delta = tangentBRel_.y - tangentBRel.y;
        coefSetY->updateDelta(prec, delta * delta);
        delta = tangentBRel_.z - tangentBRel.z;
        coefSetZ->updateDelta(prec, delta * delta);

        delta = keyBRel_.x - keyBRel.x;
        coefSetX->updateDelta(prec, delta * delta);
        delta = keyBRel_.y - keyBRel.y;
        coefSetY->updateDelta(prec, delta * delta);
        delta = keyBRel_.z - keyBRel.z;
        coefSetZ->updateDelta(prec, delta * delta);

      } // Precisions
    } // Channels
  } // Spans

  //-----------------------
  // Rate for last pos spline keys
  for (uint32_t iChan = 0; iChan < splinePosNumChans; ++iChan)
  {
    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, m_splinePosCompToQSetMapX[iChan]);
    coefSetX = qSet->getCoefSetX();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, m_splinePosCompToQSetMapY[iChan]);
    coefSetY = qSet->getCoefSetY();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, m_splinePosCompToQSetMapZ[iChan]);
    coefSetZ = qSet->getCoefSetZ();

    // Iterate over the precisions
    for (uint32_t prec = 0; prec <= maxPrec; ++prec)
    {
      coefSetX->updateRate(prec);
      coefSetY->updateRate(prec);
      coefSetZ->updateRate(prec);
    }
  }

  //-----------------------
  // Compute the mapped distortion
  m_animSectionCompressor->computeRateAndDistortion(CoefSetQSA::CoefSetSplinePos);
}

//----------------------------------------------------------------------------------------------------------------------
void SplinePosCompressorQSA::compileData()
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
  uint32_t splinePosNumChans = channelSetInfoCompressor->getSplinePosNumChans();
  NMP_VERIFY(splinePosNumChans > 0);
  uint32_t numKnots = m_knots->getNumKnots();
  NMP_VERIFY(numKnots >= 2);
  uint32_t numSpans = numKnots - 1;
  QuantisationSetQSA* qSet;

  //-----------------------
  // Compute the stride of the spline data
  computeDataStrides();

  //-----------------------
  // Compile the quantisation info ranges
  NMP::Memory::Format memReqsQInfo(0, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqsQInfo.size = NMP::Memory::align(sizeof(MR::QuantisationInfoQSA) * m_splinePosNumQuantisationSets, NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::Resource memResQInfo = NMPMemoryAllocateFromFormat(memReqsQInfo);
  m_splinePosQuantisationInfo = (MR::QuantisationInfoQSA*)memResQInfo.ptr;
  m_splinePosQuantisationInfoSize = memReqsQInfo.size;

  for (uint32_t i = 0; i < m_splinePosNumQuantisationSets; ++i)
  {
    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, i);
    const NMP::UniformQuantisationInfo& qInfoX = qSet->getCoefSetX()->getQuantisationInfo();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, i);
    const NMP::UniformQuantisationInfo& qInfoY = qSet->getCoefSetY()->getQuantisationInfo();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, i);
    const NMP::UniformQuantisationInfo& qInfoZ = qSet->getCoefSetZ()->getQuantisationInfo();

    qMin.set(qInfoX.m_qMin, qInfoY.m_qMin, qInfoZ.m_qMin);
    qMax.set(qInfoX.m_qMax, qInfoY.m_qMax, qInfoZ.m_qMax);

    QuantisationInfoQSABuilder::init(m_splinePosQuantisationInfo[i], qMin, qMax);
  }

  //-----------------------
  // Compile the pos quantisation data (means, precisions, qSets)
  for (uint32_t iChan = 0; iChan < splinePosNumChans; ++iChan)
  {
    // Get the quantisation info corresponding to this compression channel
    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, m_splinePosCompToQSetMapX[iChan]);
    const NMP::UniformQuantisationInfo& qInfoX = qSet->getCoefSetX()->getQuantisationInfo();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, m_splinePosCompToQSetMapY[iChan]);
    const NMP::UniformQuantisationInfo& qInfoY = qSet->getCoefSetY()->getQuantisationInfo();

    qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, m_splinePosCompToQSetMapZ[iChan]);
    const NMP::UniformQuantisationInfo& qInfoZ = qSet->getCoefSetZ()->getQuantisationInfo();

    // Means
    meanX = m_splinePosQuantisationData[iChan].getMeanX();
    meanY = m_splinePosQuantisationData[iChan].getMeanY();
    meanZ = m_splinePosQuantisationData[iChan].getMeanZ();

    // Precisions
    precX = qInfoX.m_precision;
    precY = qInfoY.m_precision;
    precZ = qInfoZ.m_precision;

    // Quantisation sets
    qSetX = m_splinePosCompToQSetMapX[iChan];
    qSetY = m_splinePosCompToQSetMapY[iChan];
    qSetZ = m_splinePosCompToQSetMapZ[iChan];

    //-----------------------
    // Compile the quantisation data
    QuantisationDataQSABuilder::init(
      m_splinePosQuantisationData[iChan],
      meanX, meanY, meanZ,
      precX, precY, precZ,
      qSetX, qSetY, qSetZ);
  }
  for (uint32_t iChan = splinePosNumChans; iChan < NMP::nmAlignedValue4(splinePosNumChans); ++iChan)
  {
    QuantisationDataQSABuilder::init(
      m_splinePosQuantisationData[iChan],
      meanX, meanY, meanZ,
      precX, precY, precZ,
      qSetX, qSetY, qSetZ);
  }

  //-----------------------
  // Compile the bitstream data
  NMP::BitStreamEncoder encoder;

  uint32_t splinePosDataSize = m_splinePosByteStride * (numKnots + 2 * numSpans);
  NMP::Memory::Format memReqsData = NMP::BitStreamEncoder::getMemoryRequirementsForBuffer(splinePosDataSize);
  NMP::Memory::Resource memResData = NMPMemoryAllocateFromFormat(memReqsData);
  m_splinePosData = (uint8_t*)memResData.ptr;
  m_splinePosDataSize = splinePosDataSize;

  encoder.init(splinePosDataSize, m_splinePosData);

  //-----------------------
  // Keys
  for (uint32_t knt = 0; knt < numKnots; ++knt)
  {
    // Check that the bitstream is byte aligned
    NMP_VERIFY(encoder.isAligned());

#ifdef NMP_ENABLE_ASSERTS
    size_t bytesWrittenStart = encoder.getBytesWritten();
#endif

    //-----------------------
    // Iterate over all compression channels
    for (uint32_t iChan = 0; iChan < splinePosNumChans; ++iChan)
    {
      // Set the current channel's frame pos transform
      m_splinePosTableKeys->getKey(iChan, knt, posRel);

      // Get the quantisation set corresponding to this compression channel
      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, m_splinePosCompToQSetMapX[iChan]);
      const NMP::UniformQuantisationInfo& qInfoX = qSet->getCoefSetX()->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, m_splinePosCompToQSetMapY[iChan]);
      const NMP::UniformQuantisationInfo& qInfoY = qSet->getCoefSetY()->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, m_splinePosCompToQSetMapZ[iChan]);
      const NMP::UniformQuantisationInfo& qInfoZ = qSet->getCoefSetZ()->getQuantisationInfo();

      qMin.set(qInfoX.m_qMin, qInfoY.m_qMin, qInfoZ.m_qMin);
      qMax.set(qInfoX.m_qMax, qInfoY.m_qMax, qInfoZ.m_qMax);

      // Set the quantiser precision and [qMin, qMax] range
      m_splinePosQuantTable->setPrecisionX(iChan, qInfoX.m_precision);
      m_splinePosQuantTable->setPrecisionY(iChan, qInfoY.m_precision);
      m_splinePosQuantTable->setPrecisionZ(iChan, qInfoZ.m_precision);
      m_splinePosQuantTable->setQuantisationBounds(iChan, qMin, qMax);

      // Quantise the relative pos sample
      m_splinePosQuantTable->quantise(iChan, posRel, qx, qy, qz);

      // Write the quantised data to the bitstream
      encoder.write(qInfoX.m_precision, qx);
      encoder.write(qInfoY.m_precision, qy);
      encoder.write(qInfoZ.m_precision, qz);

    }

    // Flush the bitstream to the nearest byte boundary
    encoder.flush();

#ifdef NMP_ENABLE_ASSERTS
    size_t bytesWrittenEnd = encoder.getBytesWritten();
    size_t channelByteStride = bytesWrittenEnd - bytesWrittenStart;
    NMP_VERIFY(channelByteStride == m_splinePosByteStride);
#endif

  }

  //-----------------------
  // TangentsA
  for (uint32_t span = 0; span < numSpans; ++span)
  {
    // Check that the bitstream is byte aligned
    NMP_VERIFY(encoder.isAligned());

#ifdef NMP_ENABLE_ASSERTS
    size_t bytesWrittenStart = encoder.getBytesWritten();
#endif

    //-----------------------
    // Iterate over all compression channels
    for (uint32_t iChan = 0; iChan < splinePosNumChans; ++iChan)
    {
      // Set the current channel's frame pos transform
      m_splinePosTableTangentsA->getKey(iChan, span, posRel);

      // Get the quantisation set corresponding to this compression channel
      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, m_splinePosCompToQSetMapX[iChan]);
      const NMP::UniformQuantisationInfo& qInfoX = qSet->getCoefSetX()->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, m_splinePosCompToQSetMapY[iChan]);
      const NMP::UniformQuantisationInfo& qInfoY = qSet->getCoefSetY()->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, m_splinePosCompToQSetMapZ[iChan]);
      const NMP::UniformQuantisationInfo& qInfoZ = qSet->getCoefSetZ()->getQuantisationInfo();

      qMin.set(qInfoX.m_qMin, qInfoY.m_qMin, qInfoZ.m_qMin);
      qMax.set(qInfoX.m_qMax, qInfoY.m_qMax, qInfoZ.m_qMax);

      // Set the quantiser precision and [qMin, qMax] range
      m_splinePosQuantTable->setPrecisionX(iChan, qInfoX.m_precision);
      m_splinePosQuantTable->setPrecisionY(iChan, qInfoY.m_precision);
      m_splinePosQuantTable->setPrecisionZ(iChan, qInfoZ.m_precision);
      m_splinePosQuantTable->setQuantisationBounds(iChan, qMin, qMax);

      // Quantise the relative pos sample
      m_splinePosQuantTable->quantise(iChan, posRel, qx, qy, qz);

      // Write the quantised data to the bitstream
      encoder.write(qInfoX.m_precision, qx);
      encoder.write(qInfoY.m_precision, qy);
      encoder.write(qInfoZ.m_precision, qz);

    }

    // Flush the bitstream to the nearest byte boundary
    encoder.flush();

#ifdef NMP_ENABLE_ASSERTS
    size_t bytesWrittenEnd = encoder.getBytesWritten();
    size_t channelByteStride = bytesWrittenEnd - bytesWrittenStart;
    NMP_VERIFY(channelByteStride == m_splinePosByteStride);
#endif

  }

  //-----------------------
  // TangentsB
  for (uint32_t span = 0; span < numSpans; ++span)
  {
    // Check that the bitstream is byte aligned
    NMP_VERIFY(encoder.isAligned());

#ifdef NMP_ENABLE_ASSERTS
    size_t bytesWrittenStart = encoder.getBytesWritten();
#endif

    //-----------------------
    // Iterate over all compression channels
    for (uint32_t iChan = 0; iChan < splinePosNumChans; ++iChan)
    {
      // Set the current channel's frame pos transform
      m_splinePosTableTangentsB->getKey(iChan, span, posRel);

      // Get the quantisation set corresponding to this compression channel
      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, m_splinePosCompToQSetMapX[iChan]);
      const NMP::UniformQuantisationInfo& qInfoX = qSet->getCoefSetX()->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, m_splinePosCompToQSetMapY[iChan]);
      const NMP::UniformQuantisationInfo& qInfoY = qSet->getCoefSetY()->getQuantisationInfo();

      qSet = m_animSectionCompressor->getQuantisationSet(CoefSetQSA::CoefSetSplinePos, m_splinePosCompToQSetMapZ[iChan]);
      const NMP::UniformQuantisationInfo& qInfoZ = qSet->getCoefSetZ()->getQuantisationInfo();

      qMin.set(qInfoX.m_qMin, qInfoY.m_qMin, qInfoZ.m_qMin);
      qMax.set(qInfoX.m_qMax, qInfoY.m_qMax, qInfoZ.m_qMax);

      // Set the quantiser precision and [qMin, qMax] range
      m_splinePosQuantTable->setPrecisionX(iChan, qInfoX.m_precision);
      m_splinePosQuantTable->setPrecisionY(iChan, qInfoY.m_precision);
      m_splinePosQuantTable->setPrecisionZ(iChan, qInfoZ.m_precision);
      m_splinePosQuantTable->setQuantisationBounds(iChan, qMin, qMax);

      // Quantise the relative pos sample
      m_splinePosQuantTable->quantise(iChan, posRel, qx, qy, qz);

      // Write the quantised data to the bitstream
      encoder.write(qInfoX.m_precision, qx);
      encoder.write(qInfoY.m_precision, qy);
      encoder.write(qInfoZ.m_precision, qz);

    }

    // Flush the bitstream to the nearest byte boundary
    encoder.flush();

#ifdef NMP_ENABLE_ASSERTS
    size_t bytesWrittenEnd = encoder.getBytesWritten();
    size_t channelByteStride = bytesWrittenEnd - bytesWrittenStart;
    NMP_VERIFY(channelByteStride == m_splinePosByteStride);
#endif
  }

  // Check we have written the correct amount of data
  NMP_VERIFY(encoder.getBytesWritten() == splinePosDataSize);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif
//----------------------------------------------------------------------------------------------------------------------
