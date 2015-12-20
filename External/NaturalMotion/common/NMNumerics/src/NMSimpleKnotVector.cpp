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
#include "NMNumerics/NMSimpleKnotVector.h"

// Disable warnings constant conditionals
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning(disable : 4127)
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
SimpleKnotVector::SimpleKnotVector(uint32_t numKnots, const float* knots)
{
  NMP_ASSERT(numKnots >= 2);
  NMP_ASSERT(knots);

  m_numKnots = m_maxNumKnots = numKnots;

  // Allocate the memory for the knot vector data
  m_knots = new float[m_numKnots];

  // Set the knot vector data
  for (uint32_t i = 0; i < m_numKnots; ++i)
  {
    m_knots[i] = knots[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
SimpleKnotVector::SimpleKnotVector(uint32_t maxNumKnots)
{
  NMP_ASSERT(maxNumKnots >= 2);
  m_numKnots = m_maxNumKnots = maxNumKnots;

  // Allocate the memory for the knot vector data
  m_knots = new float[m_maxNumKnots];
}

//----------------------------------------------------------------------------------------------------------------------
SimpleKnotVector::SimpleKnotVector(const SimpleKnotVector& knots)
{
  NMP_ASSERT(knots.m_numKnots >= 2);
  m_numKnots = m_maxNumKnots = knots.m_numKnots;

  // Allocate the memory for the knot vector data
  m_knots = new float[m_maxNumKnots];

  // Set the knot vector data
  SimpleKnotVector::operator=(knots);
}

//----------------------------------------------------------------------------------------------------------------------
SimpleKnotVector::~SimpleKnotVector()
{
  delete[] m_knots;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleKnotVector::getNumSpans() const
{
  return m_numKnots - 1;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleKnotVector::getNumKnots() const
{
  return m_numKnots;
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleKnotVector::setNumKnots(uint32_t n)
{
  NMP_ASSERT(n <= m_maxNumKnots);
  m_numKnots = n;
}

//----------------------------------------------------------------------------------------------------------------------
float SimpleKnotVector::getKnot(uint32_t i) const
{
  NMP_ASSERT(i < m_numKnots);
  return m_knots[i];
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleKnotVector::setKnot(uint32_t i, float t)
{
  NMP_ASSERT(i < m_numKnots);
  m_knots[i] = t;
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleKnotVector::setKnots(uint32_t n, const float* knots)
{
  NMP_ASSERT(knots);
  NMP_ASSERT(n <= m_maxNumKnots);
  m_numKnots = n;
  for (uint32_t i = 0; i < m_numKnots; ++i)
  {
    m_knots[i] = knots[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleKnotVector::setKnots(uint32_t n, const uint32_t* knots)
{
  NMP_ASSERT(knots);
  NMP_ASSERT(n <= m_maxNumKnots);
  m_numKnots = n;
  for (uint32_t i = 0; i < m_numKnots; ++i)
  {
    m_knots[i] = (float)knots[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
float SimpleKnotVector::getStartKnot() const
{
  NMP_ASSERT(m_numKnots > 0);
  return m_knots[0];
}

//----------------------------------------------------------------------------------------------------------------------
float SimpleKnotVector::getEndKnot() const
{
  NMP_ASSERT(m_numKnots > 0);
  return m_knots[m_numKnots-1];
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleKnotVector::setKnots(uint32_t numKnots, float ta, float tb)
{
  NMP_ASSERT(numKnots >= 2);
  NMP_ASSERT(numKnots <= m_maxNumKnots);
  NMP_ASSERT(ta < tb);

  m_numKnots = numKnots;
  uint32_t numSpans = numKnots - 1;
  float inc = (tb - ta) / numSpans;

  m_knots[0] = ta;
  for (uint32_t i = 1; i < numSpans; ++i)
  {
    m_knots[i] = i * inc + ta;
  }
  m_knots[numSpans] = tb;
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleKnotVector::setKnotsRegular(uint32_t ta, uint32_t tb)
{
  NMP_ASSERT(ta < tb);
  m_numKnots = tb - ta + 1;
  NMP_ASSERT(m_numKnots <= m_maxNumKnots);

  for (uint32_t i = 0; i < m_numKnots; ++i)
    m_knots[i] = (float)(i + ta);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleKnotVector::getNumKnotsDiscrete(uint32_t samplesPerKnot, uint32_t ta, uint32_t tb)
{
  NMP_ASSERT(samplesPerKnot >= 1);
  NMP_ASSERT(ta < tb);

  float diff = (float)(tb - ta);
  uint32_t numSpans = (uint32_t)((diff / samplesPerKnot) + 0.5f);
  NMP_ASSERT(numSpans >= 1);
  return numSpans + 1;
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleKnotVector::setKnotsDiscrete(uint32_t samplesPerKnot, uint32_t ta, uint32_t tb)
{
  NMP_ASSERT(samplesPerKnot >= 1);
  NMP_ASSERT(ta < tb);

  float diff = (float)(tb - ta);
  uint32_t numSpans = (uint32_t)((diff / samplesPerKnot) + 0.5f);
  NMP_ASSERT(numSpans >= 1);
  float inc = diff / numSpans;
  NMP_ASSERT(inc >= 1.0f); // Ensure that no two consecutive knots are the same (rounding)
  m_numKnots = numSpans + 1;
  NMP_ASSERT(m_numKnots <= m_maxNumKnots);

  float offs = ta + 0.5f;
  m_knots[0] = (float)ta;
  for (uint32_t i = 1; i < numSpans; ++i)
  {
    uint32_t t = (uint32_t)(i * inc + offs);
    m_knots[i] = (float)t;
  }
  m_knots[numSpans] = (float)tb;
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleKnotVector::appendKnot(float t)
{
  NMP_ASSERT(m_numKnots < m_maxNumKnots);
  m_knots[m_numKnots] = t;
  m_numKnots++;
}

//----------------------------------------------------------------------------------------------------------------------
SimpleKnotVector& SimpleKnotVector::operator=(const SimpleKnotVector& knots)
{
  NMP_ASSERT(m_maxNumKnots >= knots.m_numKnots);
  if (this != &knots)
  {
    m_numKnots = knots.m_numKnots;

    // Set the knot vector data
    for (uint32_t i = 0; i < m_numKnots; ++i)
    {
      m_knots[i] = knots.m_knots[i];
    }
  }
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleKnotVector::swap(SimpleKnotVector& knots)
{
  if (this != &knots)
  {
    uint32_t maxNumKnots = m_maxNumKnots;
    uint32_t numKnots = m_numKnots;
    float* knotData = m_knots;

    m_maxNumKnots = knots.m_maxNumKnots;
    m_numKnots = knots.m_numKnots;
    m_knots = knots.m_knots;

    knots.m_maxNumKnots = maxNumKnots;
    knots.m_numKnots = numKnots;
    knots.m_knots = knotData;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleKnotVector::getKnotsForSamples(float start, float end, uint32_t& startKnotIndex, uint32_t& endKnotIndex) const
{
  uint32_t low, high, intv;

  NMP_ASSERT(start < end);
  if (m_numKnots < 2) return false;

  // Check if start is outside the knot range
  uint32_t ext = m_numKnots - 1;
  if (start < m_knots[0]) return false;
  NMP_ASSERT(start < m_knots[ext]);

  // Check if end is outside the knot range
  if (end > m_knots[ext]) return false;
  NMP_ASSERT(end > m_knots[0]);

  //
  // Find the knot index left of the start value
  //
  high = ext - 1;
  if (start >= m_knots[high])
  {
    startKnotIndex = high; // Special case for the rightmost interval
  }
  else
  {
    // Determine the correct interval by binary search
    low = 0;
    intv = (high + low) >> 1;
    while (true)
    {
      if (start < m_knots[intv])
      {
        high = intv;
      }
      else if (start >= m_knots[intv+1])
      {
        low = intv;
      }
      else
      {
        break; // Found the interval
      }
      intv = (high + low) >> 1;
    }
    startKnotIndex = intv;
  }

  //
  // Find the knot index right of the end value
  //
  high = ext - 1;
  if (end > m_knots[high])
  {
    endKnotIndex = ext; // Special case for the rightmost interval
  }
  else
  {
    // Determine the correct interval by binary search
    low = startKnotIndex;
    intv = (high + low) >> 1;
    while (true)
    {
      if (end <= m_knots[intv])
      {
        high = intv;
      }
      else if (end > m_knots[intv+1])
      {
        low = intv;
      }
      else
      {
        break; // Found the interval
      }
      intv = (high + low) >> 1;
    }
    endKnotIndex = intv + 1;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
int32_t SimpleKnotVector::interval(float t, uint32_t& intv) const
{
  NMP_ASSERT(m_numKnots >= 2);

  int32_t rgn = 0;
  uint32_t ext = m_numKnots - 1;
  uint32_t low = 1;
  uint32_t high = ext - 1;

  // Check if outside the knot range
  if (t < m_knots[low])
  {
    if (t < m_knots[0]) rgn = -1;
    intv = 0;
  }
  else if (t >= m_knots[high])
  {
    intv = high; // Special case for the rightmost interval
    if (t > m_knots[ext]) rgn = 1;
  }
  else
  {
    // Determine the correct interval by binary search
    intv = (high + low) >> 1;
    while (true)
    {
      if (t < m_knots[intv])
      {
        high = intv;
      }
      else if (t >= m_knots[intv+1])
      {
        low = intv;
      }
      else
      {
        break; // Found the interval
      }
      intv = (high + low) >> 1;
    }
  }

  return rgn;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleKnotVector::closestKnot(float t) const
{
  NMP_ASSERT(m_numKnots >= 2);

  float ta, tb;
  uint32_t indx, ext = m_numKnots - 1;

  // First knot
  tb = 0.5f * (m_knots[0] + m_knots[1]);
  if (t < tb) return 0;

  // Last knot
  ta = 0.5f * (m_knots[ext-1] + m_knots[ext]);
  if (t >= ta) return ext;

  // Determine the correct knot index by binary search
  uint32_t low = 1;
  uint32_t high = ext;

  // Unroll first knot range
  indx = (high + low) >> 1;
  ta = 0.5f * (m_knots[indx-1] + m_knots[indx]);
  tb = 0.5f * (m_knots[indx] + m_knots[indx+1]);

  while (t < ta || t >= tb)
  {
    // Update the bracket
    if (t < m_knots[indx])
    {
      high = indx;
    }
    else
    {
      low = indx;
    }

    // Bisect the range
    indx = (high + low) >> 1;
    ta = 0.5f * (m_knots[indx-1] + m_knots[indx]);
    tb = 0.5f * (m_knots[indx] + m_knots[indx+1]);
  }

  return indx;
}

//----------------------------------------------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const SimpleKnotVector& knots)
{
  float t;

  uint32_t numKnots = knots.getNumKnots();

  os << "Num knots = " << numKnots << std::endl;
  for (uint32_t i = 0; i < numKnots; ++i)
  {
    t = knots.getKnot(i);
    os << t << std::endl;
  }
  os << std::endl;

  return os;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif
