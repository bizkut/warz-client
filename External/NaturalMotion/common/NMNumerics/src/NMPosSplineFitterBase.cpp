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
#include "NMNumerics/NMPosSplineFitterBase.h"
#include "NMNumerics/NMSimpleSplineFittingUtils.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// PosSplineFitterBaseBase
//----------------------------------------------------------------------------------------------------------------------
PosSplineFitterBase::PosSplineFitterBase(uint32_t maxNumSamples) :
  m_maxNumSamples(maxNumSamples),
  m_numSamples(0),
  m_psp(((maxNumSamples + 1) >> 1) + 1) // extra one for possible last span containing two samples
{
  // Allocate the memory for the data samples
  m_ts = new float[m_maxNumSamples];
  m_posData = new Vector3[m_maxNumSamples];
}

//----------------------------------------------------------------------------------------------------------------------
PosSplineFitterBase::~PosSplineFitterBase()
{
  delete[] m_ts;
  delete[] m_posData;
}

//----------------------------------------------------------------------------------------------------------------------
bool PosSplineFitterBase::fit(
  uint32_t NMP_UNUSED(numSamples),
  const float* NMP_UNUSED(ts),
  const Vector3* NMP_UNUSED(data))
{
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool PosSplineFitterBase::fitRegular(uint32_t numSamples, const Vector3* data)
{
  NMP_ASSERT(numSamples <= m_maxNumSamples);

  // Set the time samples
  for (uint32_t i = 0; i < numSamples; ++i)
  {
    m_ts[i] = (float)i;
  }

  // Perform the fitting
  return fit(numSamples, m_ts, data);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t PosSplineFitterBase::getNumSamples() const
{
  return m_numSamples;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t PosSplineFitterBase::getMaxNumSamples() const
{
  return m_maxNumSamples;
}

//----------------------------------------------------------------------------------------------------------------------
float* PosSplineFitterBase::getTs()
{
  return m_ts;
}

//----------------------------------------------------------------------------------------------------------------------
void PosSplineFitterBase::setTs(uint32_t numSamples, const float* ts)
{
  NMP_ASSERT(ts);
  NMP_ASSERT(numSamples <= m_maxNumSamples);

  if (ts != m_ts)
  {
    m_numSamples = numSamples;
    m_ts[0] = ts[0];
    for (uint32_t i = 1; i < m_numSamples; ++i)
    {
      m_ts[i] = ts[i];
      NMP_ASSERT(m_ts[i] > m_ts[i-1]);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
const float* PosSplineFitterBase::getTs() const
{
  return m_ts;
}

//----------------------------------------------------------------------------------------------------------------------
Vector3* PosSplineFitterBase::getPosData()
{
  return m_posData;
}

//----------------------------------------------------------------------------------------------------------------------
const Vector3* PosSplineFitterBase::getPosData() const
{
  return m_posData;
}

//----------------------------------------------------------------------------------------------------------------------
void PosSplineFitterBase::setPosData(uint32_t numSamples, const Vector3* data)
{
  NMP_ASSERT(data);
  NMP_ASSERT(numSamples <= m_maxNumSamples);

  if (data != m_posData)
  {
    m_numSamples = numSamples;
    for (uint32_t i = 0; i < m_numSamples; ++i)
    {
      m_posData[i] = data[i];
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
const PosSpline& PosSplineFitterBase::getPosSpline() const
{
  return m_psp;
}

//----------------------------------------------------------------------------------------------------------------------
void PosSplineFitterBase::setPosSpline(const PosSpline& psp)
{
  m_psp = psp;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t PosSplineFitterBase::getNumKnots() const
{
  return m_psp.getNumKnots();
}

//----------------------------------------------------------------------------------------------------------------------
void PosSplineFitterBase::setNumKnots(uint32_t numKnots)
{
  m_psp.setNumKnots(numKnots);
}

//----------------------------------------------------------------------------------------------------------------------
const float* PosSplineFitterBase::getKnots() const
{
  return m_psp.getKnots();
}

//----------------------------------------------------------------------------------------------------------------------
void PosSplineFitterBase::setKnots(uint32_t numKnots, const float* knots)
{
  NMP_ASSERT(SimpleSplineFittingUtils::isKnotVectorValid(numKnots, knots));
  m_psp.setKnots(numKnots, knots);
}

//----------------------------------------------------------------------------------------------------------------------
void PosSplineFitterBase::setKnots(uint32_t numKnots, const uint32_t* knots)
{
  NMP_ASSERT(SimpleSplineFittingUtils::isKnotVectorValid(numKnots, knots));
  m_psp.setKnots(numKnots, knots);
}

//----------------------------------------------------------------------------------------------------------------------
void PosSplineFitterBase::setKnots(const SimpleKnotVector& knots)
{
  NMP_ASSERT(SimpleSplineFittingUtils::isKnotVectorValid(knots.getNumKnots(), knots.getKnots()));
  m_psp.setKnots(knots.getNumKnots(), knots.getKnots());
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
